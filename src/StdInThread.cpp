#include "StdInThread.h"

StdInThread::StdInThread(void)
{
	this->flag_exit = false;
}

void StdInThread::execute(void*)
{
	const int size_buf = 4096;
	char buf[size_buf+1];
	while(!this->flag_exit){
		const int fd_stdin = 0;
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    fd_set set;
    FD_ZERO(&set);
    FD_SET(fd_stdin, &set);
    int ret = select(fd_stdin+1, &set, NULL, NULL, &timeout);
    if(ret <= 0) continue;
    if(!FD_ISSET(fd_stdin, &set)) continue;

		fgets(buf, size_buf, stdin);
		if(strlen(buf) > 1){
			this->ss.lock();
			this->ss.push_back(buf);
			this->ss.unlock();
		}
	}
}

void StdInThread::setup(void)
{

}
