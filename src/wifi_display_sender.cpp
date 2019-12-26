#include "wifi_display_sender.h"
#include <memory>
#include <vector>
#include <iostream>
#include <memory.h>
#include <opencv2/imgcodecs.hpp>

using namespace std;

wifi_display_sender::wifi_display_sender(int id, const std::string& ip_address, int port)
{
	id_ = (unsigned char)id;
	tcp_sender_ = make_shared<tcp_sender>(ip_address, port);
	tcp_sender_->start();
}

void wifi_display_sender::send(const cv::Mat& m)
{
	if(tcp_sender_->get_num_queued() > 0) return;
	tcp_sender_->send_data(serialize(m));
}

std::shared_ptr<std::vector<char>> wifi_display_sender::serialize(const cv::Mat& m)
{
	shared_ptr<vector<char>> ret = make_shared<vector<char>>();
	int h = m.rows;
	int w = m.cols;
	int size_header = 13;//uchar int int

	vector<unsigned char> buf_jpg;
	vector<int> param_jpg;
	param_jpg.push_back(cv::IMWRITE_JPEG_QUALITY);
	param_jpg.push_back(50);
	cv::imencode(".jpg", m, buf_jpg, param_jpg);
	int size = buf_jpg.size();
	ret->resize(size+size_header);
	unsigned char* c = (unsigned char*)(&(*(ret.get()))[0]);
	*(c+0) = id_;
	memcpy(c+1, &w, 4);
	memcpy(c+5, &h, 4);
	memcpy(c+9, &size, 4);
	memcpy(c+13, &buf_jpg[0], size);
	//cout << "size:" << ret->size() << endl;
	return ret;
}
