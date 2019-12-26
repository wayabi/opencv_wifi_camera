/***********************************************************************/
/*! @file  BaseThread.h
 *  @brief 基底スレッド
 *  
 *  @author 
 *  @date 
 */
/***********************************************************************/
#ifndef __RP_BASE_THREAD__
#define __RP_BASE_THREAD__

#include <pthread.h>

class BaseThread {
public:
	BaseThread();
	virtual ~BaseThread();

	/***********************************************************************/
	/*! @brief スレッドの開始する
	 *
	 *  @param arg 開始時に渡すオブジェクトのポインタ
	 *  @retval int 0:スレッド作成成功 0以外:失敗
	 */
	/***********************************************************************/
	int start(void *arg);

	/***********************************************************************/
	/*! @brief スレッドの終了待ちをする
	 *
	 *  @retval void * 終了時に返されるオブジェクトのポインタ
	 *  BaseThreadはこの値が有効であることを保証しない
	 * 派生クラスで使用したければどうぞ
	 */
	/***********************************************************************/
	void * join();

	/***********************************************************************/
	/*! @brief 現在のretの値を取得する
	 * this->retはjoin()の帰り値として想定されているが、join()前にスレッドの状態を取得するのにthis->retを使う
	 *
	 *  @retval void * 終了時に返されるオブジェクトのポインタ
	 *  BaseThreadはこの値が有効であることを保証しない
	 * 派生クラスで使用したければどうぞ
	 */
	/***********************************************************************/
	void * getReturnValue(){return this->ret;}

protected:
	void run(void *arg);
	static void * entryPoint(void *);
	virtual void setup();
	virtual void execute(void *);
	void * getArg() const {return this->arg;}
	void setArg(void *a){this->arg = a;}

	//join()の戻値
	//派生クラスのメンバを指すことが多いだろう。join()後、内容をコピーする前に開放しないこと。
	void *ret;

private:
	pthread_t thread;
	//開始に渡されるオブジェクトのポインタ
	void *arg;

};

#endif
