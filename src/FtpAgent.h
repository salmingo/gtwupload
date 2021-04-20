/*!
 * @class FtpAgent FTP上传助理
 * @version 0.1
 * @date 2021-04
 * @note
 * - 检查本地目录是否有数据需要上传
 * - 压缩本地数据
 * - 尝试上传
 * - 如果上传失败, 进入重复上传尝试流程
 */

#ifndef FTPAGENT_H_
#define FTPAGENT_H_

#include <boost/thread/thread.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <deque>
#include <string>
#include "Parameter.h"
//#include "FTPClient.h"
#include "FTPManager.h"

class FtpAgent {
public:
	FtpAgent();
	virtual ~FtpAgent();

protected:
	struct LocalDIR {
		std::string dirPath;	/// 子目录名
		std::string fileName;	/// 压缩后文件名
		std::string filePath;	/// 压缩后文件全路径名
	};

	using ThreadPtr = boost::shared_ptr<boost::thread>;	///< boost线程指针
	using dequedir  = std::deque<LocalDIR>;	/// 待上传子目录名
	using FTPCliPtr = boost::shared_ptr<CFTPManager>;

protected:
	Parameter* param_;	/// 配置参数
	dequedir queSubDir_;/// 子目录集合
//	FTPCliPtr ftpCli_;	/// FTP客户端
	FTPCliPtr ftpCli_;	/// FTP客户端
	ThreadPtr thrd_cycle_;		///< 周期线程, 定时检查并压缩目录、尝试上传

public:
	/*!
	 * @brief 启动守护服务
	 * @return
	 * 服务启动结果
	 */
	bool Start(Parameter* param);
	/*!
	 * @brief 停止守护服务
	 */
	void Stop();

protected:
	/* 消息响应函数 */
	/*!
	 * @brief 周期线程: 每日定时上传目录数据
	 */
	void thread_cycle();
	/*!
	 * @brief 查找前一日数据目录是否存在, 若存在则尝试压缩并上传
	 */
	void new_day(boost::posix_time::ptime& today);
	/*!
	 * @brief 上传一个目录
	 */
	bool upload_dir(LocalDIR& toUpload);
};

#endif /* FTPAGENT_H_ */
