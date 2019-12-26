#include <unistd.h>
#include <opencv2/core.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>// for cv::
#include <iostream>
#include <string>

#include "StdInThread.h"
#include "wifi_display_sender.h"
#include "Util.h"

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
	cap.set(cv::CAP_PROP_FOCUS, 255);
	cap.set(cv::CAP_PROP_AUTOFOCUS, 0);
	if(!cap.isOpened()){
		return -1;
	}

	wifi_display_sender sender(id, string(ip_address), port);

	cv::Mat frame;
	int idx = 0;

  StdInThread stdin_thread;
  stdin_thread.start(NULL);

	while(cap.read(frame)){
		sender.send(frame);
		usleep(1000*10);

    stdin_thread.ss.lock();
    vector<char> c = stdin_thread.ss.head();
    stdin_thread.ss.erase();
    stdin_thread.ss.unlock();
    string s = Util::trim(string(&c[0], c.size()), "\n");
    vector<string> command = Util::split(s, ',');
    if(command.size() > 0){
      if(command[0] == "q"){
        stdin_thread.stop();
        break;
			}else if(command[0] == "f" && command.size() >= 2){
				if(command[1] == "a"){
					cap.set(cv::CAP_PROP_AUTOFOCUS, 1);
					cout << "set focus" << endl;
				}else{
					int f = atoi(command[1].c_str());
					if(f >= 0 && f <= 255){
						cout << "set focus" << endl;
						cap.set(cv::CAP_PROP_AUTOFOCUS, 0);
						cap.set(cv::CAP_PROP_FOCUS, f);
					}
				}
			}else if(command[0] == "e" && command.size() >= 2){
				if(command[1] == "a"){
					cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 1);
					cout << "set exposure" << endl;
				}else{
					double e = atof(command[1].c_str());
					cout << "set exposure" << endl;
					cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 0.7);
					cap.set(cv::CAP_PROP_EXPOSURE, e);
				}
			}
		}
	}
	stdin_thread.stop();
	return 0;
}
