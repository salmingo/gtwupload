/*!
 * @class FTPClient 使用FTP或SFTP上传文件到服务器
 * @version 0.1
 * @date 2021-04
 * - 调用顺序:
 *   (1) Connect():      连接服务器
 *   (2) SetRemoteDIR(): 设置远程主机目录
 *   (3) UploadFile():   上传文件
 *   (4) DisConnect():   断开连接
 *  (2)和(3)可以循环/多次调用
 * - 默认采用PASSV模式
 */

#ifndef FTPCLIENT_H_
#define FTPCLIENT_H_

#include <string>
#include <boost/smart_ptr/shared_ptr.hpp>
#include "AsioTCP.h"

class FTPClient {
public:
	FTPClient(const std::string& hostName, const std::string& port, const std::string& account, const std::string& passwd);
	virtual ~FTPClient();

public:
	typedef boost::shared_ptr<FTPClient> Pointer;

protected:
	std::string hostName_;	/// FTP服务器名称或IP
	std::string hostPort_;	/// FTP服务器端口
	std::string account_;	/// 在FTP服务器上的账号
	std::string passwd_;	/// 账号密码
	std::string dirRemote_;	/// 数据在远程主机上的目录名
	bool connected_;		/// 连接标志

	TcpCPtr tcpCmd_;	/// TCP连接: 指令
	TcpCPtr tcpData_;	/// TCP连接: 数据

public:
	/* 接口 */
	static Pointer Create(const std::string& hostName, const std::string& port, const std::string& account, const std::string& passwd) {
		return Pointer(new FTPClient(hostName, port, account, passwd));
	}

	/*!
	 * @brief 设置远程主机上的数据目录
	 * @param dirPath  目录名
	 */
	void SetRemoteDIR(const std::string& dirPath);
	/*!
	 * @brief 尝试连接服务器
	 * @return
	 * 与服务器的连接结果
	 */
	bool Connect();
	/*!
	 * @brief 断开与服务器的连接
	 */
	void DisConnect();
	/*!
	 * @brief 上传文件
	 * @param filePath  在本机上的文件路径
	 * @return
	 * 文件上传结果
	 */
	bool UploadFile(const std::string& filePath);

protected:
	/* 功能 */
	/*!
	 * @brief 更新远程主机工作目录
	 */
	void update_directory_remote();
};
typedef FTPClient::Pointer FTPCliPtr;

#endif /* FTPCLIENT_H_ */
