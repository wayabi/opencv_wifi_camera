/***********************************************************************/
/*! @file  BaseThread.cpp
 *  @brief 基底スレッド
 *  
 *  @author 
 *  @date 
 */
/***********************************************************************/
#include "BaseThread.h"

BaseThread::BaseThread()
{
	this->thread = 0;
	this->arg = NULL;
	this->ret = NULL;
}

BaseThread::~BaseThread()
{

}

/***********************************************************************/
/*! @brief スレッドの開始する
 * 
 *  @param arg 開始時に渡すオブジェクトのポインタ
 *  @retval int 0:スレッド作成成功 0以外:失敗
 */
/***********************************************************************/
int BaseThread::start(void *arg)
{
	setArg(arg);
	int ret = pthread_create(&(this->thread), NULL, BaseThread::entryPoint, this);
	return ret;
}

void BaseThread::run(void *arg)
{
	this->setup();
	this->execute(arg);
}

void * BaseThread::entryPoint(void *p_this)
{
	BaseThread *pt = (BaseThread *)p_this;
	pt->run(pt->getArg());
	return NULL;
}

void BaseThread::setup()
{

}

void BaseThread::execute(void *arg)
{

}

/***********************************************************************/
/*! @brief スレッドの終了待ちをする
 * 
 *  @retval void * 終了時に返されるオブジェクトのポインタ
 */
/***********************************************************************/
void * BaseThread::join()
{
	 pthread_join(this->thread, NULL);
	 return this->ret;
}
