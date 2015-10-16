#ifndef	__THREAD_QUEUE_H__
#define	__THREAD_QUEUE_H__

#include <queue>
#include "ThreadMutex.h"

#define	DEFAULT_QUEUE_SIZE		100

///	线程安全队列
template<class Object>	class ThreadQueue
{
///	属性
private:
	std::queue< Object >	_queue;		///	队列
	ThreadMutex				_queueLock;	///	线程互斥锁
	unsigned int			_iMaxCount;	///	队列最大容量

///	构造与析构
public:
	ThreadQueue( unsigned int iMaxCount = DEFAULT_QUEUE_SIZE );
	virtual ~ThreadQueue();

///	方法
public:
	///	是否为空
	bool	IsEmpty();
	///	向队列尾添加对象
	bool	Push( Object& obj );
	///	从队列中提取对象
	bool	Pop( Object& obj );
	///	队列中对象个数
	int		Count();
	///	获得队列的首对象
	bool	Front( Object& obj );
	///	获得队列的尾对象
	bool	Back( Object& obj );
	///	清除队列
	void	Clear();
};

///	构造与析构
template<class Object>
ThreadQueue<Object>::ThreadQueue(unsigned int iMaxCount)
	: _iMaxCount( iMaxCount )
{
	if( _iMaxCount <= 0 )
		_iMaxCount = DEFAULT_QUEUE_SIZE;
}

template<class Object>
ThreadQueue<Object>::~ThreadQueue()
{
	Clear();
}

///	是否为空
template<class Object>
bool ThreadQueue<Object>::IsEmpty()
{
	bool bRet = false;
	if( _queueLock.lock() )
	{
		if( _queue.size() == 0 )
			bRet = true;
		_queueLock.unlock();
	}

	return bRet;
}

///	向队列尾添加对象
template<class Object>
bool	ThreadQueue<Object>::Push( Object& obj )
{
	bool bRet = false;
	if( _queueLock.lock() )
	{
		if( _queue.size() < _iMaxCount )
		{
			_queue.push( obj );
			bRet = true;
		}
		_queueLock.unlock();
	}

	return bRet;
}

///	从队列中提取对象
template<class Object>
bool	ThreadQueue<Object>::Pop( Object& obj )
{
	bool bRet = false;
	if( _queueLock.lock() )
	{
		if( _queue.size() > 0 )
		{
			obj = _queue.front();
			_queue.pop();
			bRet = true;
		}
		_queueLock.unlock();
	}

	return bRet;
}

///	队列中对象个数
template<class Object>
int		ThreadQueue<Object>::Count()
{
	int nItemCount = -1;
	if( _queueLock.lock() )
	{
		nItemCount = _queue.size();
		_queueLock.unlock();
	}

	return nItemCount;
}

///	获得队列的首对象
template<class Object>
bool	ThreadQueue<Object>::Front( Object& obj )
{
	bool bRet = false;
	if( _queueLock.lock() )
	{
		if( _queue.size() > 0 )
		{
			obj = _queue.front();
			bRet = true;
		}
		_queueLock.unlock();
	}

	return bRet;
}

///	获得队列的尾对象
template<class Object>
bool	ThreadQueue<Object>::Back( Object& obj )
{
	bool bRet = false;
	if( _queueLock.lock() )
	{
		if( _queue.size() > 0 )
		{
			obj = _queue.back();
			bRet = true;
		}
		_queueLock.unlock();
	}

	return bRet;
}

///	清除队列
template<class Object>
void	ThreadQueue<Object>::Clear()
{
	if( _queueLock.lock() )
	{
		while( !_queue.empty() )
			_queue.pop();

		_queueLock.unlock();
	}
}

#endif	/*__THREAD_QUEUE_H__*/

