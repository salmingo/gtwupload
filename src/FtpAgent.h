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

class FtpAgent {
public:
	FtpAgent();
	virtual ~FtpAgent();

protected:
	using ThreadPtr = boost::shared_ptr<boost::thread>;	///< boost线程指针

protected:
	ThreadPtr thrd_cycle_;		///< 周期线程, 定时检查并压缩目录、尝试上传
	//...缓存待上传子目录名称. 当网络异常时一直缓存, 直至恢复正常后自动上传. 子目录名最多保存30天?

public:
	/*!
	 * @brief 启动守护服务
	 * @return
	 * 服务启动结果
	 */
	bool Start();
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
};

#endif /* FTPAGENT_H_ */
