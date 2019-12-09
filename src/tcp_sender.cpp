#include "tcp_sender.h"

#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <signal.h>

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>

using namespace std;

tcp_sender::tcp_sender(const std::string& ip_address, int port)
{
	ip_address_ = ip_address;
	port_ = port;
	signal(SIGPIPE, SIG_IGN);
	sock_ = -1;
}

void tcp_sender::start()
{

	addr_.sin_family = AF_INET;
	addr_.sin_port = htons(port_);
	addr_.sin_addr.s_addr = inet_addr(ip_address_.c_str());

	reconnect();
	thread_ = make_shared<thread>(&tcp_sender::exe, this);
}

void tcp_sender::reconnect()
{
	if(sock_ >= 0){
		close(sock_);
	}
	sock_ = socket(AF_INET, SOCK_STREAM, 0);
	int yes = 1;
	setsockopt(sock_, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes));
	fcntl(sock_, F_SETFL, O_NONBLOCK);

	connect(sock_, (struct sockaddr*)&addr_, sizeof(addr_));
	fd_set fdset;
	FD_ZERO(&fdset);
	struct timeval tv;
	cout << "connecting..." << endl;
	while(!flag_stop_){
		usleep(5*1000);
		FD_SET(sock_, &fdset);
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		if(select(sock_+1, NULL, &fdset, NULL, &tv) <= 0){
			cout << "reconnecting...(by select)" << endl;
			continue;
		}
		int v;
		socklen_t len_v = 4;
		getsockopt(sock_, SOL_SOCKET, SO_ERROR, &v, &len_v);
		if(v != 0){
			cout << "reconnecting...(by SO_ERROR)" << endl;
			continue;
		}
		if(!FD_ISSET(sock_, &fdset)){
			continue;
		}
		cout << "connected." << endl;
		return;
	}
}

void tcp_sender::exe()
{
	fd_set fdset;
	struct timeval tv;
	shared_ptr<vector<char>> data = nullptr;
	while(!flag_stop_){
		usleep(5*1000);
		if(data == nullptr){
			mutex_.lock();
			if(!queue_.empty()){
				data = queue_.front();
				queue_.pop_front();
			}
			mutex_.unlock();
		}
		if(data == nullptr) continue;

		FD_ZERO(&fdset);
		while(!flag_stop_){
			FD_SET(sock_, &fdset);
			tv.tv_sec = 1;
			tv.tv_usec = 0;
			if(select(sock_+1, NULL, &fdset, NULL, &tv) <= 0){
				cout << "reconnecting..." << endl;
				reconnect();
				continue;
			}
			if(!FD_ISSET(sock_, &fdset)){
				continue;
			}
			// tcp connection check
			vector<char> buf_read;
			buf_read.resize(80);
			if(read(sock_, &buf_read[0], 1) == 0){
				cout << "reconnecting..." << endl;
				reconnect();
				continue;
			}
			int send_flag = 0;
			vector<char>* d = data.get();
			int len = send(sock_, &(*d)[0], data->size(), send_flag);
			if(len >= 0){
				cout << "sended size:" << len << endl;
				int rest = data->size() - len;
				if(rest == 0){
					data = nullptr;
					break;
				}else{
					data = make_shared<vector<char>>(&(*d)[len], &(*d)[d->size()]);
				}
			}
		}
	}
	close(sock_);
}

void tcp_sender::send_data(std::shared_ptr<std::vector<char>> d)
{
	mutex_.lock();
	queue_.push_back(d);
	mutex_.unlock();
}

void tcp_sender::stop()
{
	flag_stop_ = true;
}

int tcp_sender::get_num_queued()
{
	int num = 0;
	mutex_.lock();
	num = queue_.size();
	mutex_.unlock();
	return num;
}
