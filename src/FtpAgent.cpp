/*!
 * @class FtpAgent FTP上传助理
 * @version 0.1
 * @date 2021-04
 */

#include <boost/bind/bind.hpp>
#include "FtpAgent.h"

using namespace boost::placeholders;

FtpAgent::FtpAgent() {

}

FtpAgent::~FtpAgent() {

}

bool FtpAgent::Start() {

	thrd_cycle_.reset(new boost::thread(boost::bind(&FtpAgent::thread_cycle, this)));

	return true;
}

void FtpAgent::Stop() {
	thrd_cycle_->interrupt();
	thrd_cycle_->join();
}

void FtpAgent::thread_cycle() {

}
