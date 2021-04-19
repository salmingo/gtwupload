/*!
 * @class FTPClient 使用FTP或SFTP上传文件到服务器
 * @version 0.1
 * @date 2021-04
 */

#include <boost/bind/bind.hpp>
#include "FTPClient.h"
#include "GLog.h"

using namespace boost::posix_time;

FTPClient::FTPClient(const std::string& hostName, const int port) {
	hostName_  = hostName;
	hostPort_  = port;
	connected_ = false;
	uploading_ = false;
}

FTPClient::~FTPClient() {
	if (connected_) {
		disconnect();
	}
	if (thrd_idle_.unique()) {
		thrd_idle_->interrupt();
		thrd_idle_->join();
	}
}

void FTPClient::SetRemoteDIR(const std::string& dirPath) {
	dirRemote_ = dirPath;
}

bool FTPClient::TryConnect(const std::string& account, const std::string& passwd) {
	account_ = account;
	if (passwd.size()) passwd_ = passwd;
	if (connected_)
		disconnect();
	if (hostName_.empty()) {
		_gLog->Write(LOG_FAULT, "name of FTP Server is empty");
		return false;
	}
	if (account_.empty()) {
		_gLog->Write(LOG_FAULT, "account is empty");
		return false;
	}
	return connect();
}

bool FTPClient::UploadFile(const std::string& filePath) {
	if (!(connected_ || connect()))
		return false;
	if (dirRemote_.empty()) {
		_gLog->Write(LOG_FAULT, "remote directory is empty");
		return false;
	}
	else {
		_gLog->Write("try to upload file [%s]", filePath.c_str());
		tmLast_ = second_clock::universal_time();
		uploading_ = true;
		//...上传文件
	}

	return true;
}

bool FTPClient::connect() {
 	_gLog->Write("try to connect FTP server [%s@%s:%d]",
			account_.c_str(),
			hostName_.c_str(), hostPort_);
	//...尝试打开套接口
	if (connected_) {
		if (thrd_idle_.unique()) {
			thrd_idle_->interrupt();
			thrd_idle_->join();
		}
		thrd_idle_.reset(new boost::thread(boost::bind(&FTPClient::thread_idle, this)));
	}
	else
		_gLog->Write(LOG_FAULT, "failed to connect server");
	return connected_;
}

void FTPClient::disconnect() {
	// ...仅关闭套接口

	connected_ = false;
	_gLog->Write("disconnect from server");
}

void FTPClient::thread_idle() {
	boost::chrono::minutes t(1);
	ptime now;

	tmLast_ = second_clock::universal_time();
	while (connected_) {
		boost::this_thread::sleep_for(t);

		now = second_clock::universal_time();
		if (uploading_)
			tmLast_ = now;
		else if ((now - tmLast_).total_seconds() > 300)
			disconnect();
	}
}
