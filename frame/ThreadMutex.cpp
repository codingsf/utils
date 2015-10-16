#include "ThreadMutex.h"
#include "../log/log.h"

///	构造与析构
ThreadMutex::ThreadMutex()
{
	_bIsValid = false;
	pthread_mutexattr_init( &_csAttr );
	pthread_mutexattr_settype( &_csAttr, PTHREAD_MUTEX_RECURSIVE );
	if( 0 == pthread_mutex_init( &_cs, &_csAttr ) ){
		_bIsValid = true;
	}
	else
		pthread_mutexattr_destroy( &_csAttr );
}

ThreadMutex::~ThreadMutex(){
	if( _bIsValid ){
		pthread_mutex_destroy( &_cs );
		pthread_mutexattr_destroy( &_csAttr );
		_bIsValid = false;
	}
}

///	加锁
bool ThreadMutex::lock(){
	if( _bIsValid ){
		int ret = pthread_mutex_lock( &_cs );
		return( 0 == ret );
	}
	return false;
}

///	解锁
void ThreadMutex::unlock(){
	if( _bIsValid ){
		pthread_mutex_unlock( &_cs );
	}
}

///	尝试加锁
bool ThreadMutex::try_lock()
{
	if( _bIsValid ){
		int ret = pthread_mutex_trylock( &_cs );
		return( 0 == ret );
	}
	return false;
}

