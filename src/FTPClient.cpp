/*!
 * @class FTPClient 使用FTP或SFTP上传文件到服务器
 * @version 0.1
 * @date 2021-04
 */

#include <boost/bind/bind.hpp>
#include "FTPClient.h"

using namespace boost::posix_time;

FTPClient::FTPClient(const std::string& hostName, const int port) {
	hostName_  = hostName;
	hostPort_  = port;
	connected_ = false;
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

bool FTPClient::TryConnect(const std::string& account, const std::string& passwd) {
	account_ = account;
	if (passwd.size()) passwd_ = passwd;
	return connect();
}

void FTPClient::SetRemoteDIR(const std::string& dirPath) {
	dirRemote_ = dirPath;
}

bool FTPClient::UploadFile(const std::string& filePath) {
	if (!connected_ && !connect())
		return false;

	tmLast_ = second_clock::universal_time();

	return true;
}

bool FTPClient::connect() {

	if (connected_) {
		thrd_idle_.reset(new boost::thread(boost::bind(&FTPClient::thread_idle, this)));
	}
	return connected_;
}

void FTPClient::disconnect() {
	//...断开连接不要销毁线程
}

void FTPClient::thread_idle() {
	boost::chrono::minutes t(1);
	ptime now;

	tmLast_ = second_clock::universal_time();
	while (1) {
		boost::this_thread::sleep_for(t);

		now = second_clock::universal_time();
		if ((now - tmLast_).total_seconds() > 300)
			disconnect();
	}
}
