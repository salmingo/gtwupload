/*!
 * @class FtpAgent FTP上传助理
 * @version 0.1
 * @date 2021-04
 */

#include <boost/bind/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <unistd.h>
#include <sys/wait.h>
#include "FtpAgent.h"
#include "GLog.h"

using namespace std;
using namespace boost::placeholders;
using namespace boost::filesystem;
using namespace boost::posix_time;

FtpAgent::FtpAgent() {
	param_ = NULL;
}

FtpAgent::~FtpAgent() {

}

bool FtpAgent::Start(Parameter* param) {
	param_ = param;
//	ftpCli_ = FTPClient::Create(param_->hostName, param_->hostPort,
//			param_->account, param_->passwd);
//	ftpCli_->SetRemoteDIR(param_->dirRemote);
	ftpCli_.reset(new CFTPManager);
	thrd_cycle_.reset(new boost::thread(boost::bind(&FtpAgent::thread_cycle, this)));

	return true;
}

void FtpAgent::Stop() {
	if (thrd_cycle_.unique()) {
		thrd_cycle_->interrupt();
		thrd_cycle_->join();
	}
//	ftpCli_->DisConnect();
}

void FtpAgent::thread_cycle() {
	ptime now;
	int toWait(0), secsOfDay, hours, dayOld, dayNew;
	bool success;

	boost::this_thread::sleep_for(boost::chrono::seconds(10)); // 启动时休眠10秒等待完成其他初始化
	dayOld = -1;
	while(1) {
		now = second_clock::universal_time();
		dayNew = now.date().day();
		hours = now.time_of_day().hours();
		secsOfDay = now.time_of_day().total_seconds();

		if (dayOld != dayNew) {
			new_day(now);
			dayNew = dayOld;
		}

		success = true;

		while (success && queSubDir_.size()) {// 尝试上传
			LocalDIR& toUpload = queSubDir_.front();
			if ((success = upload_dir(toUpload)))
				queSubDir_.pop_front();
		}

		if (queSubDir_.size() && hours < 8) {// 上传失败
			toWait = (hours + 1) * 3600 - secsOfDay;
		}
		else {
			toWait = 86410 - secsOfDay;
		}
		boost::this_thread::sleep_for(boost::chrono::seconds(toWait));
	}
}

void FtpAgent::new_day(boost::posix_time::ptime& today) {
	ptime yesterday = today - hours(24);
	path subDir = param_->dirLocal;
	std::string strdate = to_iso_string(yesterday.date());
	subDir /= strdate;

	if (exists(subDir)) {
		LocalDIR toUpload;
		path pathTgz = param_->dirLocal;
		boost::format fmt("%1%.tgz");

		fmt % strdate;
		pathTgz /= fmt.str();

		toUpload.dirPath = strdate;
		toUpload.fileName= fmt.str();
		toUpload.filePath= pathTgz.string();

		queSubDir_.push_back(toUpload);
	}
}

bool FtpAgent::upload_dir(LocalDIR& toUpload) {
	// 检查压缩文件是否存在, 若不存在则进行压缩
	path pathTgz = toUpload.filePath;

	if (!exists(pathTgz)) {// 启动压缩进程
		pid_t pid1(0); // 进程句柄
		pid_t pid2;    // 监测进程句柄
		int status;

		_gLog->Write("try to compress [%s = %s]", toUpload.dirPath.c_str(), toUpload.fileName.c_str());
		/* 以多进程模式启动图像处理 */
		if ((pid1 = fork()) < 0) {// 无法启动多进程
			_gLog->Write(LOG_FAULT, "FtpAgent::upload_dir()", "failed to fork multi-process");
			return false;
		}
		else if (pid1 == 0) {
			chdir(param_->dirLocal.c_str()); // 切换工作目录
			execl("/usr/bin/tar", "tar", "czf",
					toUpload.fileName.c_str(), toUpload.dirPath.c_str(), NULL);
			exit(1);
		}
		while (pid1 != (pid2 = waitpid(pid1, &status, WNOHANG | WUNTRACED)) && pid2 != -1);
		if (pid2 == -1) {
			_gLog->Write(LOG_FAULT, "compression failed");
		}
	}

	if (exists(pathTgz)) {// 尝试上传
		string hostName = param_->hostName + string(":") + param_->hostPort;
		if (ftpCli_->login2Server(hostName) < 0
				|| ftpCli_->inputUserName(param_->account) <= 0
				|| ftpCli_->inputPassWord(param_->passwd) <= 0)
			_gLog->Write(LOG_FAULT, "failed to login FTP server: [%s@%s]",
					param_->account.c_str(), hostName.c_str());
		else {
			ftpCli_->setTransferMode(CFTPManager::binary);
			ftpCli_->Pasv();
			if (param_->dirRemote.size()) ftpCli_->CD(param_->dirRemote);
			_gLog->Write("try to upload file: %s", toUpload.fileName.c_str());
			if (ftpCli_->Put(toUpload.fileName, toUpload.filePath))
				_gLog->Write(LOG_FAULT, "upload failed");
			else
				_gLog->Write("upload succeed");
			ftpCli_->quitServer();
		}
	}

	return false;
}
