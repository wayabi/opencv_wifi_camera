#ifndef __RS_UTIL__
#define __RS_UTIL__

#include <string>
#include <vector>
#include <iostream>

class Util {
public:
	static std::string urlEncode(const char *source);
	static std::string getHostFromURL(const std::string &url);
	static std::string getMACAddress(void);
	static std::vector<std::string> getListFile(std::string path_dir);
	static std::string getExtension(std::string name_file);
	static std::vector<std::string> split(std::string source, char delimiter);
	static bool existFile(const char *filename);
	static size_t getSizeFile(const char *filename);
	static time_t getTimeFromSeperatedFormat(const char *seperated);
	static std::string getTimeString(time_t t);

	static std::string hex_texted_ucs2_to_utf8(const char *hex_texted_ucs2);
	static std::string trim(const std::string &s, const char *list_char_trim = " \r\t\v\n");
	static std::string getIPAddress(const std::string &name_interface);

	static time_t getTime(int year, int month, int date, int hour, int minute, int second);
	static void lowess(double len_window, const std::vector<double>& src, std::vector<double>& out);
	static std::string getCRLFString(const char* c);
	static bool hasTheHead(const char* source, const char* head);
	static bool hasTheHeadFoot(const char* source, const char* head, const char* foot);
	static int getIndexText(const char* source, const char* text);
	static std::string getHexString(void* buf, int size, bool flag_capital = true);
	static int atoiHeading(const char* c);
	static std::ostream& hexdump(std::ostream& ost, const void* bin, size_t length);
	static std::vector<unsigned char> hexToBinary(const std::string& hex);
	static std::string getStringMillisec(const struct timeval* tv);
	static std::vector<std::vector<std::string> > load_csv(const char* path_file);
	static void moving_average(int len_window, const std::vector<double>& src, std::vector<double>& out);
	static long get_millisec();
	static double root_mean_square(double* d, int size);

private:
	static bool sort_string(const std::string& a, const std::string& b);
};

#endif
