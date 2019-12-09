#ifndef __U_TCP_SENDER__
#define __U_TCP_SENDER__

#include <string>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

class tcp_sender {
public:
	tcp_sender(const std::string& ip_address, int port);
	void send_data(std::shared_ptr<std::vector<char>> data);
	int get_num_queued();
	void start();
	void stop();

private:
	void reconnect();
	void exe();

private:
	std::string ip_address_;
	int port_;
	int sock_;
	struct sockaddr_in addr_;
	std::shared_ptr<std::thread> thread_;
	std::mutex mutex_;
	bool flag_stop_ = false;
	std::list<std::shared_ptr<std::vector<char>>> queue_;
};

#endif
