#include <unistd.h>
#include <opencv2/core.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>// for cv::
#include <iostream>
#include <string>

#include "wifi_display_sender.h"

using namespace std;

int main(int argc, const char* argv[])
{
	if(argc < 6){
		printf("%s <id>, <w_camera>, <h_camera>, <ip_address>, <port>\n", *(argv+0));
		return 1;
	}
	int id = atoi(*(argv+1));
	int w = atoi(*(argv+2));
	int h = atoi(*(argv+3));
	const char* ip_address = *(argv+4);
	int port = atoi(*(argv+5));

	cv::VideoCapture cap(0);
	cap.set(cv::CAP_PROP_FPS, 60);
	cap.set(cv::CAP_PROP_FRAME_WIDTH, w);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, h);
	if(!cap.isOpened()){
		return -1;
	}

	wifi_display_sender sender(id, string(ip_address), port);

	cv::Mat frame;
	int idx = 0;
	while(cap.read(frame)){
		sender.send(frame);
		usleep(1000*10);
	}

	return 0;
}
