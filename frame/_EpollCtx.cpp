#include "_EpollCtx.h"
#include "../log/log.h"
//#include <sys/poll.h>

///	_SocketCtx  构造与析构
_SocketCtx::_SocketCtx( int fd )
	: _socket(fd)
{
}

_SocketCtx::~_SocketCtx()
{
	_SocketCtx::Close();
}

///     开始监听
BOOL    _SocketCtx::Listen( int port, int backlog/* = 128*/ )
{
        _SocketCtx::Close();

        _socket = socket( AF_INET, SOCK_STREAM, IPPROTO_IP );
        if( _socket < 0 ){
                ERROR("创建SOCKET失败");
                return FALSE;
        }

        int on = 1;
        if( setsockopt( _socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof( on ) ) ){
                ERROR("REUSEADDR 失败");
                return FALSE;
        }

        struct sockaddr_in local;
        local.sin_addr.s_addr = htonl(INADDR_ANY);
        local.sin_family = AF_INET;
        local.sin_port = htons( port );

	///	绑定端口并开启侦听
        if( bind( _socket, (struct sockaddr *)&local, sizeof(local) )!=0 || listen( _socket, backlog ) != 0 ){
                close( _socket );
                _socket = -1;
                ERROR( "绑定端口或侦听失败!" );
                return FALSE;
        }

	///	设置为非阻塞
	int	value = fcntl( _socket, F_GETFL, 0 );
	value |= O_NONBLOCK;
	fcntl( _socket, F_SETFL, value );

        return TRUE;
}

///     连接
BOOL    _SocketCtx::Connect2( struct sockaddr *address, size_t addrSize, int timeout/* = 5*/ )
{
	_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( _socket < 0 ){
		ERROR( "创建SOCKET失败" );
		return FALSE;
	}
/*
	unsigned long ul = 1;
	int ret = ioctl( _socket, FIONBIO, &ul );
	if( ret == -1 ){
		ERROR( "设置非阻塞失败");
		goto CONNECT_ERROR;
	}
*/
	int ret = connect( _socket, address, addrSize );
	if( ret == 0 )
	{
		unsigned long ul = 1;
	        int ret = ioctl( _socket, FIONBIO, &ul );
        	if( ret == -1 ){
                	ERROR( "设置非阻塞失败");
             	   	goto CONNECT_ERROR;
        	}

		return TRUE;
	}

	ERROR( "连接失敗" );
/*
	if( errno != EINPROGRESS )
	{
		ERROR( "连接失败" );
		goto CONNECT_ERROR;
	}

	struct timeval waitTime;
	fd_set r;
	FD_ZERO( &r );
	FD_SET( _socket, &r );
	waitTime.tv_sec = 5;
	waitTime.tv_usec = 0;

	ret = select( _socket+1, NULL, &r, NULL, &waitTime );

	if( ret < 0 ){
		ERROR( "Select 错误" );
		goto CONNECT_ERROR;
	}

	if( ret == 0 ){
		ERROR( "连接超时");
		goto CONNECT_ERROR;
	}
	
	if( FD_ISSET( _socket, &r ) )
		return TRUE;
*/
CONNECT_ERROR:
	close( _socket );
	_socket = -1;
	return FALSE;
}

///     连接
BOOL    _SocketCtx::Connect( const char* host, int port, int timeout/* = 5*/ )
{
	_SocketCtx::Close();

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr( host );
	if (server.sin_addr.s_addr == INADDR_NONE )
	{
		struct hostent *inhost = gethostbyname( host );
		if ( inhost )
		{
			int i = 0;
			for(; inhost->h_addr_list[i]; i++)
			{
				memcpy( &server.sin_addr, inhost->h_addr_list[i], inhost->h_length );
				if( Connect2( (struct sockaddr *)&server, sizeof( server ), timeout ) )
					break;
			}
			if( NULL == inhost->h_addr_list[i] )
			{
				ERROR( "连接失败" );
				return FALSE;
			}
		}
		else
		{
			ERROR("域名解析失败");
			return FALSE;
		}
	}
	else
	{
		return Connect2( (struct sockaddr *)&server, sizeof( server ), timeout );
	}

	return TRUE;
}

///     关闭
void    _SocketCtx::Close()
{
	if( _socket >= 0 )
	{
		close( _socket );
		_socket = -1;
	}
}

///     处理EPOLLIN事件
int	_SocketCtx::ProcessEpollIn( void )
{
	return -1;
}

///     处理EPOLLOUT事件
int	_SocketCtx::ProcessEpollOut( void )
{
	return -1;
}

///	是否有数据待发送
bool	_SocketCtx::HasData2Send( void )
{
	return false;
}

///	_EpollCtx  构造与析构
_EpollCtx::_EpollCtx()
	: _epfd( -1 )
	, _epSize( 100 )
	, _epEvents( NULL )
{
}

_EpollCtx::~_EpollCtx()
{
	_EpollCtx::CloseEpoll();
}

///     初始化epoll文件
BOOL    _EpollCtx::InitEpoll( int epsize )
{
	_EpollCtx::CloseEpoll();

	if( epsize > 0 )
		_epSize = epsize;

	_epEvents = new struct epoll_event[ _epSize ];

	_epfd = epoll_create( _epSize );
        if( _epfd < 0 ){
                FATAL("创建epoll文件描述失败");
                return FALSE;
        }

	return TRUE;
}

///     添加监听事件
BOOL    _EpollCtx::AddSocket( _SocketCtx*  ctx, unsigned int events )
{
	if( _epfd < 0 )
		return FALSE;

	if( NULL == ctx || ctx->_socket < 0 )
		return FALSE;

	if ( !_lock.lock() )
	{
		DEBUG("lock failed");
		return FALSE;
	}

	_epEvent.data.ptr = ctx;
	_epEvent.events = events;
	if( epoll_ctl( _epfd, EPOLL_CTL_ADD, ctx->_socket, &_epEvent ) < 0 ){
		ERROR("添加监听事件失败");
		_lock.unlock();
		return FALSE;
        }

	_lock.unlock();

	return TRUE;
}

///     修改监听事件
BOOL    _EpollCtx::ModifySocket( _SocketCtx* ctx, unsigned int events )
{
	if( _epfd < 0 )
		return FALSE;

	if( NULL == ctx || ctx->_socket < 0 )
		return FALSE;

	if( !_lock.lock() )
	{
		DEBUG("lock failed");
		return FALSE;
	}

	_epEvent.data.ptr = ctx;
	_epEvent.events = events;
	if( epoll_ctl( _epfd, EPOLL_CTL_MOD, ctx->_socket, &_epEvent ) < 0 ){
		ERROR("修改监听事件失败");
		_lock.unlock();
		return FALSE;
	}

	_lock.unlock();
	return TRUE;
}

///	删除监听事件
BOOL	_EpollCtx::RemoveSocket( _SocketCtx* ctx )
{
	if( _epfd < 0 )
		return FALSE;

	if( NULL == ctx || ctx->_socket < 0 )
		return FALSE;

	if( !_lock.lock() )
	{
		DEBUG("lock failed");
		return FALSE;
	}

	if( epoll_ctl( _epfd, EPOLL_CTL_DEL, ctx->_socket, &_epEvent ) < 0 ){
		ERROR("删除监听事件失败");
		_lock.unlock();
		return FALSE;
	}

	_lock.unlock();
	return TRUE;
}

///     关闭epoll文件
void    _EpollCtx::CloseEpoll( void )
{
	if( _epfd >= 0 )
	{
		close( _epfd );
		_epfd = -1;
	}

	if( NULL != _epEvents )
	{
		delete[] _epEvents;
		_epEvents = NULL;
	}
}

///     执行监控
void     _EpollCtx::DoDetect( int timeout/* = 100*/ )
{
	if( _epfd < 0 )
		return;

	if( !_lock.lock() )
		return;

	int nEventNum = epoll_wait( _epfd, _epEvents, _epSize, timeout );
	_lock.unlock();

	if( nEventNum <= 0 )
		usleep( 1000 );

	for( int i = 0; i < nEventNum; i++ )
	{
		_SocketCtx*	pCtx = ( _SocketCtx* )_epEvents[i].data.ptr;
		if( _epEvents[i].events & ( EPOLLIN|EPOLLERR ) )
		{
			if( pCtx->ProcessEpollIn() == 0 && RemoveSocket( pCtx ) )
			{
				OnSocketRemoved( pCtx );
			}
			else if( pCtx->HasData2Send() )
			{
				ModifySocket( pCtx, ( EPOLLOUT|EPOLLERR ) );
			}
		}
		if( _epEvents[i].events & ( EPOLLOUT|EPOLLERR ) )
		{
			if( pCtx->ProcessEpollOut() == 0 && RemoveSocket( pCtx ) )
			{
				OnSocketRemoved( pCtx );
			}
			else if( !pCtx->HasData2Send() )
				ModifySocket( pCtx, ( EPOLLIN|EPOLLERR ) );
		}
	}
}

