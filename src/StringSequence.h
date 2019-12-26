#ifndef __RP_STRING_SEQUENCE__
#define __RP_STRING_SEQUENCE__

#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include <vector>

class SSC {//StringSequenceContent
public:
	std::vector<char> c;
	SSC* next;
	SSC* back;

};
	
class StringSequence {
public:
	StringSequence(void);
	~StringSequence(void);

	void setMaxString(int max){this->max_string  = max;}

	std::vector<char> head(void);
	std::vector<char> end(void);
	std::vector<char> next(void);
	std::vector<char> back(void);
	std::vector<char> erase(void);
	std::vector<char> current(void);
	void push_back(const std::vector<char>& c);
	void push_back(const char* c);
	void push_back(const char* c, int size);
	
	void lock(void);
	void unlock(void);
	

private:
	//if max_string > 0, it has limit. if max_string <= 0, it doesn't have limit.
	int max_string;
	int num_string;

	SSC* ssc_head;
	SSC* ssc_end;
	SSC* ssc_current;

	pthread_mutex_t mutex;

};

#endif
