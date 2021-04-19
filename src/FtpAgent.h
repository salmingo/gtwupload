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

class FtpAgent {
public:
	FtpAgent();
	virtual ~FtpAgent();
};

#endif /* FTPAGENT_H_ */
