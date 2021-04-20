/*!
 * @class FTPClient 使用FTP或SFTP上传文件到服务器
 * @version 0.1
 * @date 2021-04
 */

#include "FTPClient.h"
#include "GLog.h"

FTPClient::FTPClient(const std::string& hostName, const std::string& port, const std::string& account, const std::string& passwd) {
	hostName_  = hostName;
	hostPort_  = port;
	account_   = account;
	passwd_    = passwd;
	connected_ = false;
}

FTPClient::~FTPClient() {
	if (connected_) {
		DisConnect();
	}
}

void FTPClient::SetRemoteDIR(const std::string& dirPath) {
	dirRemote_ = dirPath;
	if (connected_) // 设置远程主机上接收文件的目录
		update_directory_remote();
}

bool FTPClient::Connect() {
	if (!connected_) {
		if (hostName_.empty())
			_gLog->Write(LOG_FAULT, "name of FTP Server is empty");
		else if (hostPort_.empty())
			_gLog->Write(LOG_FAULT, "port of FTP Server is empty");
		else if (account_.empty())
			_gLog->Write(LOG_FAULT, "account is empty");
		else {
			_gLog->Write("try to connect FTP server [%s@%s:%s]",
					account_.c_str(),
					hostName_.c_str(), hostPort_.c_str());
			//...尝试打开套接口
		}

		if (connected_ && dirRemote_.size()) update_directory_remote();
	}

	return connected_;
}

void FTPClient::DisConnect() {
	if (connected_) {
		// ...若正在上传则中止?

		// ...关闭套接口

		connected_ = false;
		_gLog->Write("disconnect from server");
	}
}

bool FTPClient::UploadFile(const std::string& filePath) {
	if (!(connected_ || Connect()))
		return false;

	_gLog->Write("try to upload file [%s]", filePath.c_str());
	//...上传文件

	return true;
}

void FTPClient::update_directory_remote() {

}
