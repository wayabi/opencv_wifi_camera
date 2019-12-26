#include "StringSequence.h"

using namespace std;

StringSequence::StringSequence(void)
{
	this->max_string = -1;
	this->num_string = 0;
	this->ssc_head = NULL;
	this->ssc_end = NULL;
	this->ssc_current = NULL;
	pthread_mutex_init(&(this->mutex), NULL);
}

StringSequence::~StringSequence(void)
{
	SSC* ssc = this->ssc_head;
	SSC* s;
	while(ssc){
		s = ssc;
		ssc = s->next;
		delete(s);
		
	}
}

std::vector<char> StringSequence::head(void)
{
	if(this->ssc_head == NULL) return vector<char>();
	this->ssc_current = this->ssc_head;
	return this->ssc_head->c;
}

std::vector<char> StringSequence::end(void)
{
	if(this->ssc_end == NULL) return vector<char>(); 
	this->ssc_current = this->ssc_end;
	return this->ssc_end->c;
}

std::vector<char> StringSequence::next(void)
{
	if(this->ssc_current == NULL) return vector<char>();
	this->ssc_current = this->ssc_current->next;
	if(this->ssc_current == NULL) return vector<char>();
	return this->ssc_current->c;
}

std::vector<char> StringSequence::back(void)
{
	if(this->ssc_current == NULL) return vector<char>();
	this->ssc_current = this->ssc_current->next;
	if(this->ssc_current == NULL) return vector<char>();
	return this->ssc_current->c;
}

std::vector<char> StringSequence::erase(void)
{
	if(this->ssc_current == NULL) return vector<char>();
	SSC* b = this->ssc_current->back;
	SSC* n = this->ssc_current->next;
	if(this->ssc_current == this->ssc_head) this->ssc_head = n;
	if(this->ssc_current == this->ssc_end) this->ssc_end = b;
	delete(this->ssc_current);

	this->ssc_current = n;
	if(b){
		b->next = n;
	}
	if(n){
		n->back = b;
	}
	--(this->num_string);
	if(this->ssc_current == NULL) return vector<char>();
	return this->ssc_current->c;
}

std::vector<char> StringSequence::current(void)
{
	if(this->ssc_current == NULL) return vector<char>();
	return this->ssc_current->c;
}

void StringSequence::push_back(const std::vector<char>& c_)
{
	SSC* ssc = new SSC();
	ssc->c = c_;
	if(this->ssc_end == NULL){
		this->ssc_head = ssc;
		this->ssc_end = ssc;
		ssc->next = NULL;
		ssc->back = NULL;
	}else{
		this->ssc_end->next = ssc;
		ssc->back = this->ssc_end;
		ssc->next = NULL;
		this->ssc_end = ssc;
	}
	++(this->num_string);
	if(this->max_string > 0 && this->num_string > this->max_string){
		SSC* s = this->ssc_head;
		if(s == this->ssc_end) this->ssc_end = NULL;
		delete(s);
		this->ssc_head = this->ssc_head->next;
		if(this->ssc_head){
			this->ssc_head->back = NULL;
		}
		--(this->num_string);
	}
}

void StringSequence::push_back(const char* c)
{
	int len = strlen(c);
	vector<char> vc;
	vc.insert(vc.begin(), c, c+len);//add last '\0'
	this->push_back(vc);
}

void StringSequence::push_back(const char* c, int size)
{
	vector<char> vc;
	vc.insert(vc.begin(), c, c+size);
	this->push_back(vc);
}

void StringSequence::lock(void)
{
	pthread_mutex_lock(&(this->mutex));
}

void StringSequence::unlock(void)
{
	pthread_mutex_unlock(&(this->mutex));
}
