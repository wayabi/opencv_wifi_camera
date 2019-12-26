#include <algorithm>
#include <iomanip>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <chrono>

#include "Util.h"

using namespace std;

string Util::urlEncode(const char *source)
{
  unsigned long   count_source = 0;
  unsigned long   count_buf = 0;
  unsigned char   cbyte;

  unsigned long length_source = strlen(source);
  unsigned long length_buf = length_source*3+1;
  char *buf = new char[length_buf];

  while(true) {
    cbyte = *(source + count_source);
    if( ((cbyte >= 0x81) && (cbyte <= 0x9F)) ||
      ((cbyte >= 0xE0) && (cbyte <= 0xEF)) ) {
      sprintf(buf+count_buf, "%%%02X", cbyte);
      ++count_source;
      count_buf += 3;
      sprintf(buf+count_buf, "%%%02X", *(source+count_source));
      ++count_source;
      count_buf += 3;
    } else if(cbyte == 0x20) {
      *(buf+count_buf) = '+';
      ++count_source;
      ++count_buf;
    } else if( ((cbyte >= 0x40) && (cbyte <= 0x5A)) ||
           ((cbyte >= 0x61) && (cbyte <= 0x7A)) ||
           ((cbyte >= 0x30) && (cbyte <= 0x39)) ||
           (cbyte == 0x2A) ||
           (cbyte == 0x2D) ||
           (cbyte == 0x2E) ||
           (cbyte == 0x5F) ) {
      *(buf+count_buf) = *(source+count_source);
      ++count_source;
      ++count_buf;
    } else {
      sprintf(buf+count_buf, "%%%02X", cbyte);
      ++count_source;
      count_buf += 3;
    }
    if(count_source >= length_source){
		*(buf+count_buf) = 0;
		break;
	}
  }
  string s;
  s.append(buf);
  delete [](buf);
  return s;
}

string Util::getHostFromURL(const string &url)
{
	const char *c = url.c_str();
	int length = url.length();
	const char *prefix = "://";
	int index_head = 0;
	for(int i=0;i<length-3;++i){
		bool flag = true;
		for(int j=0;j<3;++j){
			if(*(c+i+j) != *(prefix+j)) flag = false;
		}
		if(flag){
			index_head = i+3;
			break;
		}
	}
	int index_foot = 0;
	int i;
	for(i=index_head;i<length;++i){
		if(*(c+i) == '/' || *(c+i) == '?') break;
	}
	if(i == length) i = 0;
	index_foot = i;
	if(index_head > index_foot) return url;
	return string((c+index_head), index_foot-index_head);

}

string Util::getMACAddress()
{
 int fd;
 struct ifreq ifr;

 fd = socket(AF_INET, SOCK_DGRAM, 0);

 ifr.ifr_addr.sa_family = AF_INET;
 strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

 ioctl(fd, SIOCGIFHWADDR, &ifr);

 close(fd);

 char buf[80];
 sprintf(buf, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
         (unsigned char)(ifr.ifr_hwaddr.sa_data[0]),
         (unsigned char)(ifr.ifr_hwaddr.sa_data[1]),
         (unsigned char)(ifr.ifr_hwaddr.sa_data[2]),
         (unsigned char)(ifr.ifr_hwaddr.sa_data[3]),
         (unsigned char)(ifr.ifr_hwaddr.sa_data[4]),
         (unsigned char)(ifr.ifr_hwaddr.sa_data[5]));

 return string(buf);
}

vector<string> Util::getListFile(string path_dir)
{
	DIR* dir;
	struct dirent* dp;
	vector<string> ret;

	if ((dir = opendir(path_dir.c_str())) == NULL){
		return ret;
	}

	for(int i=0;(dp = readdir(dir)) != NULL;++i){
		if(dp->d_type == DT_REG){
			string s(dp->d_name);
			ret.push_back(s);
		}
	}

	closedir(dir);

	sort(ret.begin(), ret.end(), Util::sort_string);
	return ret;
}

bool Util::sort_string(const string& a, const string& b)
{
	return (a.compare(b) < 0);
}

string Util::getExtension(string name_file)
{
	int length = name_file.length();
	if(length == 0) return "";
	const char *c = name_file.c_str();
	for(int i=length-1;i>=0;--i){
		if(*(c+i) == '.'){
			if(i != length-1){
				return string(c, i+1, length-i-1);
			}
		}
	}
	return "";
}

bool Util::existFile(const char *filename)
{
	FILE  *fp;
	if ((fp = fopen(filename, "r")) == NULL) return false;
	fclose(fp);
	return true;
}

vector<string> Util::split(string source, char delimiter)
{
	vector<string> v_ret;
	int size = source.length();
	int index = 0;
	const char *c = source.c_str();
	for(int i=0;i<size;++i){
		if(i==size-1){
			v_ret.push_back(string(c, index, i-index+1));
		}else if(*(c+i) == delimiter){
			v_ret.push_back(string(c, index, i-index));
			index = i+1;
		}
	}
	return v_ret;
}

size_t Util::getSizeFile(const char *filename)
{
	if(!Util::existFile(filename)) return -1;
	FILE *f;
	if((f = fopen(filename, "rb")) == NULL){
		return -1;
	}
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	fclose(f);
	return size;
}

time_t Util::getTimeFromSeperatedFormat(const char *seperated)
{
	//2013_12_31_23_59_59_999 23文字
	if(strlen(seperated) < 23) return -1;
	int year, month, date, hour, minute, second, m_sec;
	year = atoi(seperated);
	month = atoi(seperated+5);
	date = atoi(seperated+8);
	hour = atoi(seperated+11);
	minute = atoi(seperated+14);
	second = atoi(seperated+17);
	m_sec = atoi(seperated+20);

	time_t timer;
	struct tm tm1;

	tm1.tm_year = year - 1900;
	tm1.tm_mon  = month - 1;
	tm1.tm_mday = date;
	tm1.tm_hour = hour;
	tm1.tm_min  = minute;
	tm1.tm_sec  = second;
	timer = mktime(&tm1);
	return timer;
}

static int x00_to_byte(const char *x00)
{
	if(x00 == NULL) return -1;
	int t1;
	int t2;
	if(*(x00+0) >= '0' && *(x00+0) <= '9'){
		t1 = *(x00+0)-'0';
	}else if(*(x00+0) >= 'a' && *(x00+0) <= 'f'){
		t1 = *(x00+0)-'a'+10;
	}else if(*(x00+0) >= 'A' && *(x00+0) <= 'F'){
		t1 = *(x00+0)-'A'+10;
	}else{
		return -1;
	}
	if(*(x00+1) >= '0' && *(x00+1) <= '9'){
		t2 = *(x00+1)-'0';
	}else if(*(x00+1) >= 'a' && *(x00+1) <= 'f'){
		t2 = *(x00+1)-'a'+10;
	}else if(*(x00+1) >= 'A' && *(x00+1) <= 'F'){
		t2 = *(x00+1)-'A'+10;
	}else{
		return -1;
	}
	return t1*16+t2;
}

static int ucs2_to_utf8 (int ucs2, char * utf8)
{
		if (ucs2 < 0) return -1;
    if (ucs2 < 0x80) {
        utf8[0] = ucs2;
        utf8[1] = '\0';
        return 1;
    }
    if (ucs2 >= 0x80  && ucs2 < 0x800) {
        utf8[0] = (ucs2 >> 6)   | 0xC0;
        utf8[1] = (ucs2 & 0x3F) | 0x80;
        utf8[2] = '\0';
        return 2;
    }
    if (ucs2 >= 0x800 && ucs2 < 0xFFFF) {
        utf8[0] = ((ucs2 >> 12)       ) | 0xE0;
        utf8[1] = ((ucs2 >> 6 ) & 0x3F) | 0x80;
        utf8[2] = ((ucs2      ) & 0x3F) | 0x80;
        utf8[3] = '\0';
        return 3;
    }
    return -1;
}

string Util::hex_texted_ucs2_to_utf8(const char *hex_texted_ucs2)
{
	if(hex_texted_ucs2 == NULL || strlen(hex_texted_ucs2) == 0) return NULL;
	int length = strlen(hex_texted_ucs2);
	int max_length_utf8 = length/4*3;
	char *utf8 = (char *)calloc(max_length_utf8, sizeof(char));
	int index = 0;
	int i;
	char buf_utf8[4];
	char buf_hex[3];
	buf_hex[2] = '\0';
	//4文字, 2byte（UTF-8での1文字)ずつ順に変換していく
	for(i=0;i<length/4;++i){
		int u = 0;
		int t;
		buf_hex[0] = *(hex_texted_ucs2+i*4+0);
		buf_hex[1] = *(hex_texted_ucs2+i*4+1);
		t = x00_to_byte(buf_hex);
		if(t < 0) t = 0;
		t = t << (1*8);
		u |= t;
		buf_hex[0] = *(hex_texted_ucs2+i*4+2);
		buf_hex[1] = *(hex_texted_ucs2+i*4+3);
		t = x00_to_byte(buf_hex);
		if(t < 0) t = 0;
		t = t << (0*8);
		u |= t;
		int length_t = ucs2_to_utf8(u, buf_utf8);
		if(length_t < 0) break;
		int j;
		for(j=0;j<length_t;++j){
			*(utf8+index) = *(buf_utf8+j);
			++index;
		}
	}
	*(utf8+index) = '\0';
	string ret = utf8;
	free(utf8);
	return ret;
}

string Util::trim(const string &s, const char *list_char_trim)
{
	int length = s.length();
	int num_list = strlen(list_char_trim);
	int index_start = length;
	int index_end = -1;
	int i, j;
	//index_start
	for(i=0;i<length;++i){
		bool flag = false;
		for(j=0;j<num_list;++j){
			if(*(s.c_str()+i) == *(list_char_trim+j)){
				flag = true;
				break;
			}
		}
		if(!flag){
			index_start = i;
			break;
		}
	}
	//index_end
	for(i=length-1;i>=index_start;--i){
		bool flag = false;
		for(j=0;j<num_list;++j){
			if(*(s.c_str()+i) == *(list_char_trim+j)){
				flag = true;
				break;
			}
		}
		if(!flag){
			index_end = i;
			break;
		}
	}
	if(index_start > index_end) return "";
	return string(s, index_start, index_end-index_start+1);
}


string Util::getIPAddress(const string &name_interface)
{
	int fd;
	struct ifreq ifr;
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, name_interface.c_str(), IFNAMSIZ-1);
	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);
	string ret = inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
	return ret;
}

time_t Util::getTime(int year, int month, int date, int hour, int minute, int second)
{
	time_t timer;
	struct tm tm1;

	tm1.tm_year = year - 1900;
	tm1.tm_mon  = month - 1;
	tm1.tm_mday = date;
	tm1.tm_hour = hour;
	tm1.tm_min  = minute;
	tm1.tm_sec  = second;
	timer = mktime(&tm1);
	return timer;
}

string Util::getTimeString(time_t t)
{
	struct tm st;
 	localtime_r(&t, &st);
	char buf[80];
	sprintf(buf, "%d/%02d/%02d-%02d:%02d:%02d.000", st.tm_year+1900, st.tm_mon+1, st.tm_mday, st.tm_hour, st.tm_min, st.tm_sec);
	string ret = buf;
	return ret;
}

static double lowess_w(double x)
{
	if(x < 0 || x >= 1) return 0;
	double a = 1-x*x*x;
	return a*a*a;
}

//p = 1
static double lowess_s(int r, int t, int index_start_nn, int index_end_nn)
{
	double sum = 0;
	for(int i=index_start_nn;i<=index_end_nn;++i){
		double a = (i-t);
		a = pow(a, r);
		double b = lowess_w(abs(i-t)/(double)((index_end_nn-index_start_nn+1)/2));
//printf("(%f, %f),", a, b);
		sum += a*b;
	}
//printf("\nsum = %f, %d(%d,%d)\n", sum, t, index_start_nn, index_end_nn);
	return sum;
}

void Util::lowess(double len_window, const std::vector<double>& src, std::vector<double>& out)
{
	if(&src == &out) abort();
	int n = src.size();
	out.resize(n);
	int size_nn = n*len_window;
	int num_nn_before = size_nn/2;

	int index_start_nn = 0;
	int index_end_nn = 0;
	for(int i=0;i<n;++i){
		index_start_nn = i - num_nn_before;
		if(index_start_nn < 0) index_start_nn = 0;
		index_end_nn = index_start_nn+size_nn-1;
		if(index_end_nn >= n){
			index_end_nn = n-1;
			index_start_nn = index_end_nn-size_nn+1;
		}

		double s0 = lowess_s(0, i, index_start_nn, index_end_nn);
		double s1 = lowess_s(1, i, index_start_nn, index_end_nn);
		double s2 = lowess_s(2, i, index_start_nn, index_end_nn);
		double sum = 0;
		for(int j=index_start_nn;j<=index_end_nn;++j){
			int max_range = size_nn/2;
			if(max_range == 0) max_range = 1;
			sum += (s2-(j-i)*s1) / (s0*s2-s1*s1) * lowess_w(abs(j-i)/(double)max_range) * src[j];
		}
		out[i] = sum;
	}
}

string Util::getCRLFString(const char* c)
{
	const char CR = 0x0d;
	const char LF = 0x0a;
	int length = strlen(c);
	int index_crlf = -1;

	for(int i=length-1;i>=0;--i){
		if(*(c+i) == CR){
			if(i<length-1 && *(c+i+1) == LF){
				if(index_crlf == -1){
					index_crlf = i;
				}else{
					return string(c, i+2, index_crlf-(i+2));
				}
			}
		}
	}
	return "";
}

bool Util::hasTheHead(const char* source, const char* head)
{
	int length_source = strlen(source);
	int length_head = strlen(head);
	if(length_head == 0) return true;
	if(length_head > length_source) return false;

	for(int i=0;i<length_head;++i){
			if(*(source+i) != *(head+i)) return false;
	}
	return true;
}

bool Util::hasTheHeadFoot(const char* source, const char* head, const char* foot)
{
	if(!Util::hasTheHead(source, head)) return false;
	int length_source = strlen(source);
	int length_foot = strlen(foot);
	if(length_foot == 0) return true;
	if(length_foot > length_source) return false;

	for(int i=0;i<length_foot;++i){
		if(*(source+length_source-length_foot+i) != *(foot+i)) return false;
	}
	return true;
}

int Util::getIndexText(const char* source, const char* text)
{
	int length_source = strlen(source);
	int length_text = strlen(text);
	for(int i=0;i<=length_source-length_text;++i){
		bool flag = true;
		for(int j=0;j<length_text;++j){
			if(*(source+i+j) != *(text+j)){
				flag = false;
				break;
			}
		}
		if(flag) return i;
	}
	return -1;
}

string Util::getHexString(void* buf, int size, bool flag_capital)
{
	char hex1[] = {
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	char hex2[] = {
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
	char* hex = (flag_capital?hex1:hex2);

	string ret = "";
	char s_add[2];
	s_add[1] = '\0';
	for(int i=0;i<size;++i){
		int byte_ = *(((unsigned char*)buf)+i);
		int b1 = byte_ >> 4;
		int b2 = (byte_ & 0x0F);
		s_add[0] = hex[b1];
		ret.append(s_add);
		s_add[0] = hex[b2];
		ret.append(s_add);
	}
	return ret;
}

int Util::atoiHeading(const char* c)
{
	int len = strlen(c);
	for(int i=0;i<len;++i){
		if(*(c+i) >= '1' && *(c+i) <= '9'){
			if(i > 0){
				if(*(c+i-1) == '-') --i;
			}
			return atoi(c+i);
		}
	}
	return 0;
}

ostream& Util::hexdump(ostream& ost, const void* bin, size_t length)
{
	if (length == 0) return ost;
	if (bin == NULL) return ost;

	ios_base::fmtflags orgflags = ost.flags();
	char f = ost.fill();
	ost << setfill('0') << hex;

	ssize_t remain = length;

	for (long head = 0; remain > 0; head += 16, remain -= 16) {
		ost << setw(8) << head;

		const ssize_t len = min(ssize_t(16), remain);

		uint8_t* p = ((uint8_t*)bin) + head;

		int i;
		for (i = 0; i < len; i++, p++) {
			ost << ' ' << setw(2) << int(*p);
		}
		for (; i < 16; i++) {
			ost << "   ";
		}

		ost << ' ';
		p = ((uint8_t*)bin) + head;
		for (i = 0; i < len; i++, p++) {
			ost << (isgraph(*p) ? char(*p) : '.');
		}
		ost << endl;
	}

	ost.fill(f);
	ost.flags(orgflags);
	return ost;
}

vector<unsigned char> Util::hexToBinary(const string& hex)
{
	vector<unsigned char> ret;
	for(int i=0;i<hex.length()-1;i+=2){
		int b = x00_to_byte(hex.c_str()+i);
		if(b == -1){
			ret.clear();
			return ret;
		}
		ret.push_back((unsigned char)b);
	}
	return ret;
}

string Util::getStringMillisec(const struct timeval* tv)
{
	struct timeval tv1;
	if(tv == NULL){
		gettimeofday(&tv1, NULL);
	}else{
		tv1 = *tv;
	}
	struct tm* time_st = localtime(&tv1.tv_sec);
	const int size_buf = 80;
	char buf[size_buf+1];
	sprintf(buf, "%d-%02d-%02dT%02d:%02d:%02d.%03dZ",
		time_st->tm_year+1900,
		time_st->tm_mon+1,
		time_st->tm_mday,
		time_st->tm_hour,
		time_st->tm_min,
		time_st->tm_sec,
		(int)(tv1.tv_usec/1000));
	return string(buf);
}

std::vector<std::vector<string> > Util::load_csv(const char* path_file)
{
	vector<vector<string> > ret;
	FILE* f;
	if((f = fopen(path_file, "r")) == NULL){
		printf("file open error:%s\n", path_file);
		return ret;
	}
	vector<char> buf;
	const int size_buf = 1024*128;
	buf.resize(size_buf);
	while(fgets(&buf[0], size_buf, f)){
		vector<string> ss = Util::split(Util::trim(string(&buf[0])), ',');
		ret.push_back(ss);
	}
	fclose(f);
	return ret;
}

void Util::moving_average(int len_window, const std::vector<double>& src, std::vector<double>& out)
{
	if(&src == &out) abort();
	int n = src.size();
	out.resize(n);
	double sum = 0.0;
	int start_old = 0;
	int end_old = len_window-1;
	int start = start_old;
	int end = end_old;
	for(int i=0;i<len_window;++i){
		sum += src[i];
	}
	for(int i=0;i<n;++i){
		start = i-len_window/2;
		if(start < 0) start = 0;
		end = start+len_window-1;
		if(end >= n-1){
			end = n-1;
			start = end-len_window+1;
		}
		if(start != start_old){
			sum -= src[start_old];
			sum += src[end];
			start_old = start;
			end_old = end;
		}
		out[i] = sum / len_window;
	}
}

long Util::get_millisec()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()
		).count();
}

double Util::root_mean_square(double* d, int size)
{
	if(size <= 0) return 0;
	double sum = 0;
	for(int i=0;i<size;++i){
		sum += *(d+i) * *(d+i);
	}
	return sqrt(sum) / size;
}
