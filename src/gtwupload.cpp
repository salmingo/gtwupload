/**
 @name 通过ftp或sftp，每天定时上传GTW文件
 @version 0.1
 @date 2021-04

 @note
 - 使用配置文件管理本地和远程参数
 - 文件上传前压缩
 - 当命令行无参数时, 以守护服务方式运行
 - 当命令行有参数时, 以实时方式运行一次
 - 每天本地时间8:00--16:00, 当上传失败时, 每隔30分钟重新尝试一次

 @note
 秘钥对操作指南:
 1. 在本机生成秘钥对
 $ ssh-keygent -t rsa
 依据提示输入秘钥存储位置($HOME/.ssh/id_rsa);
 不要输入密码(此处密码是读取秘钥对要输入的密码)
 2. 将公钥上传到服务器并加入信任区
 $ scp $HOME/.ssh/id_rsa.pub $REMOTE/Public
 $ ssh $REMOTE
 $ cd .ssh
 $ cat ~/Public/id_rsa.pub >> authorized_keys

 */

#include <iostream>
#include <stdio.h>
#include <getopt.h>
#include "daemon.h"
#include "GLog.h"
#include "Parameter.h"
#include "FtpAgent.h"
#include "FTPClient.h"

#define DAEMON_NAME		"gtwupload"

using namespace std;

GLog* _gLog;		//< 工作日志

///////////////////////////////////////////////////////////////////////
/*!
 * @brief 显示使用说明
 */
void Usage() {
	printf("Usage:\n");
	printf(" adips [options] [<image file 1> <image file 2> ...]\n");
	printf("\nOptions\n");
	printf(" -h / --help    : print this help message\n");
	printf(" -d / --default : generate default configuration file here\n");
	printf(" -c / --config  : specify configuration file\n");
	printf(" -s / --daemon  : run program as daemon\n");
}

int main(int argc, char** argv) {
	bool isDaemon(false);
	_gLog = NULL;
	/* 解析命令行参数 */
	struct option longopts[] = {
		{ "help",    no_argument,       NULL, 'h' },
		{ "default", no_argument,       NULL, 'd' },
		{ "config",  required_argument, NULL, 'c' },
		{ "daemon",  no_argument,       NULL, 's' },
		{ NULL,      0,                 NULL,  0  }
	};
	char optstr[] = "hdc:s";
	int ch, optndx;
	Parameter* param = NULL;

	while ((ch = getopt_long(argc, argv, optstr, longopts, &optndx)) != -1) {
		switch(ch) {
		case 'h':
			Usage();
			return -1;
		case 'd':
			cout << "generating default configuration file here" << endl;
			param = new Parameter;
			param->Init("gtwupload.xml");
			return -2;
		case 'c':
			param = new Parameter;
			if (!param->Load(argv[optind - 1])) {
				cout << "failed to load configuration file " <<  argv[optind - 1] << endl;
				return -3;
			}
			break;
		case 's':
			param = new Parameter;
			if (!param->Load("/usr/local/etc/gtwupload.xml")) {
				cout << "failed to load configuration file [/usr/local/etc/gtwupload.xml]" << endl;
				return -3;
			}
			_gLog = new GLog("/var/log/gtwupload", "gtwupload_");
			isDaemon = true;
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if (!isDaemon && !argc) {
		cout << "require file(s) or directory to upload" << endl;
		return -3;
	}

	if (isDaemon) {// 启动服务
		boost::asio::io_service ios;
		boost::asio::signal_set signals(ios, SIGINT, SIGTERM);  // interrupt signal
		signals.async_wait(boost::bind(&boost::asio::io_service::stop, &ios));

		if (!MakeItDaemon(ios)) return 1;
		if (!isProcSingleton("/var/run/gtwupload.pid")) {
			_gLog->Write("%s is already running or failed to access PID file", DAEMON_NAME);
			return 2;
		}

		_gLog->Write("Try to launch %s %s as daemon", DAEMON_NAME, "v0.1");
		// 主程序入口
		FtpAgent agent;
		if (agent.Start(param)) {
			_gLog->Write("Daemon goes running");
			ios.run();
			agent.Stop();
		}
		else {
			_gLog->Write(LOG_FAULT, "Fail to launch %s", DAEMON_NAME);
		}
		_gLog->Write("Daemon stopped");
	}
	else {// 立即尝试上传
		if (!param) {
			param = new Parameter;
			if (!(param->Load("gtwupload.xml") || param->Load("/usr/local/etc/gtwupload.xml"))) {
				cout << "failed to load configuration file " << endl;
				delete param;
				param = NULL;
			}
		}
		if (param) {
			_gLog = new GLog(stdout);

			FTPCliPtr ftpCli;
			ftpCli.reset(new FTPClient(param->hostName, param->hostPort, param->account, param->passwd));
			if (ftpCli->Connect()) {
				ftpCli->SetRemoteDIR(param->dirRemote);
				//...目录: 压缩后上传; 文件: 直接上传
				for (int i = 0; i < argc; ++i)
					ftpCli->UploadFile(string(argv[i]));

				ftpCli->DisConnect();
			}

			ftpCli.reset();
		}
	}

	if (_gLog) delete _gLog;
	if (param) delete param;

	return 0;
}
