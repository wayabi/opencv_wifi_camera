#ifndef __RP_STDIN_THREAD__
#define __RP_STDIN_THREAD__

#include "BaseThread.h"
#include "StringSequence.h"
#include <string>
#include <pthread.h>
#include "Util.h"

using namespace std;

class StdInThread : public BaseThread {
public:
	StdInThread(void);
	void stop(void){this->flag_exit = true;}

protected:
	void setup(void);
	void execute(void*);

public:
	StringSequence ss;

private:
	static const char* TAG;

private:
	bool flag_exit;

};

#endif
