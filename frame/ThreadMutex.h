#ifndef _INCLUDE_THREAD_MUTEX_H
#define _INCLUDE_THREAD_MUTEX_H

#include <pthread.h>

///	线程互斥锁
class ThreadMutex
{
///	构造与析构
public:
	ThreadMutex();
	virtual	~ThreadMutex();

///	方法
public:
	///	加锁
	bool lock();
	///	解锁
	void unlock();
	///	尝试加锁
	bool try_lock();

///	属性
private:
	pthread_mutexattr_t		_csAttr;	///	线程互斥量属性
	pthread_mutex_t 		_cs;		///	线程互斥量
	bool 					_bIsValid;	///	是否有效
};

#endif		/* _INCLUDE_THREAD_MUTEX_H */

