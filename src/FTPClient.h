/*!
 * @class FTPClient 使用FTP或SFTP上传文件到服务器
 * @version 0.1
 * @date 2021-04
 * - 连接建立后维持5分钟. 若5分钟内无数据传输则断开连接; 若有数据传输则重置时标
 * - 调用顺序:
 *   (1) 构造函数
 *   (2) SetRemoteDIR(): 设置远程主机目录
 *   (3) TryConnect():   设置账户和密码
 *   (4) UploadFile():   上传文件. 可以循环调用
 */

#ifndef FTPCLIENT_H_
#define FTPCLIENT_H_

#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

class FTPClient {
public:
	FTPClient(const std::string& hostName, const int port = 20);
	virtual ~FTPClient();

protected:
	std::string hostName_;	/// FTP服务器名称或IP
	int hostPort_;			/// FTP服务器端口
	std::string account_;	/// 在FTP服务器上的账号
	std::string passwd_;	/// 账号密码
	std::string dirRemote_;	/// 数据在远程主机上的目录名
	bool connected_;		/// 连接标志
	bool uploading_;		/// 上传标志. 避免网速过慢, 没有在检测周期内完成传输

	boost::posix_time::ptime tmLast_;	/// 上一次数据传输时标
	boost::shared_ptr<boost::thread> thrd_idle_;	/// 线程指针

public:
	/* 接口 */
	/*!
	 * @brief 设置远程主机上的数据目录
	 * @param dirPath  目录名
	 */
	void SetRemoteDIR(const std::string& dirPath);
	/*!
	 * @brief 尝试连接一次服务器
	 * @param account  账号名称
	 * @param passwd   密码
	 * @return
	 * 连接结果
	 * @note
	 * - 连接成功后立即断开
	 * - 若登陆服务器需要密码, 则记录该密码, 避免后续重复输入密码
	 */
	bool TryConnect(const std::string& account, const std::string& passwd);
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
	 * @brief 尝试连接服务器
	 * @return
	 * 与服务器的连接结果
	 */
	bool connect();
	/*!
	 * @brief 断开与服务器的连接
	 */
	void disconnect();
	/*!
	 * @brief 线程: 检查是否有数据传输发生
	 */
	void thread_idle();
};
typedef boost::shared_ptr<FTPClient> FTPCliPtr;

#endif /* FTPCLIENT_H_ */
