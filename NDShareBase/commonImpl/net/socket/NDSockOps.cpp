#include "net/socket/NDSockOps.h"

#ifdef WIN32
	#include <WinSock2.h>
#else

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>			//for accept();
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>			//for inet_xxx();
#include <netinet/tcp.h>
#include <netinet/in.h>			//for sockaddr_in;
#include <errno.h>				//for errno;

extern int errno;

#endif


#include "net/socket/NDSocketAddress.h"


_NDSHAREBASE_BEGIN

SOCKET NDSockOps::createTCPFileDescriptor()
{
	return ::socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
}

NDBool NDSockOps::setNonblocking( SOCKET* pFd )
{
	if ( (NULL == pFd) || (INVALID_SOCKET == *pFd) ) return NDFalse;

#ifdef WIN32
	unsigned long arg = 1;
	return ioctlsocket( *pFd, FIONBIO, &arg );
#else
	int nFlags = ::fcntl( *pFd, F_GETFL, 0 );
	nFlags |= O_NONBLOCK;
	return ( -1 != ::fcntl( *pFd, F_SETFL, nFlags ) );
#endif
}


NDBool NDSockOps::isNonblocking( SOCKET* pFd )
{
	if ( ( NULL == pFd ) || ( INVALID_SOCKET == *pFd ) )	return NDFalse;

#ifdef WIN32
	return NDFalse;
#else
	int nFlags = fcntl( *pFd, F_GETFL, 0 ) ;
	return ( nFlags | O_NONBLOCK );
#endif
}

NDBool NDSockOps::setblocking( SOCKET* pFd )
{
	if ( (NULL == pFd) || (INVALID_SOCKET == *pFd) ) return NDFalse;

	unsigned long arg = 0;

	return ioctlsocket( *pFd, FIONBIO, &arg );
}

NDBool NDSockOps::setDisableBuffering( SOCKET* pFd )
{
	if ( (NULL == pFd) || (INVALID_SOCKET == *pFd) ) return NDFalse;

	NDUint32 arg = 1;

	return setsockoption( *pFd, IPPROTO_TCP, TCP_NODELAY, (const void*)&arg, sizeof(arg) );
}

NDBool NDSockOps::setEnableBuffering( SOCKET* pFd )
{
	if ( (NULL == pFd) || (INVALID_SOCKET == *pFd) ) return NDFalse;

	NDUint32 arg = 0;

	return setsockoption( *pFd, IPPROTO_TCP, TCP_NODELAY, (const void*)&arg, sizeof(arg) );
}

NDBool NDSockOps::setRecvBufferSize( SOCKET* pFd , NDUint32 size)
{
	if ( (NULL == pFd) || (INVALID_SOCKET == *pFd) ) return NDFalse;

	return setsockoption( *pFd, SOL_SOCKET, SO_RCVBUF, (const void*)&size, sizeof(size) );
}


NDUint32 NDSockOps::getRecvBufferSize( SOCKET* pFd )
{
	if ( ( NULL == pFd ) || ( INVALID_SOCKET == *pFd ) )	return 0;

	int nRecvBufSize;
	int nLen = sizeof(nRecvBufSize);
	if ( getsockoption( *pFd, SOL_SOCKET, SO_RCVBUF, &nRecvBufSize, &nLen ) )
	{
		return nRecvBufSize;
	}
	return 0;
}

NDBool NDSockOps::setSendBufferSize( SOCKET* pFd , NDUint32 size)
{
	if ( (NULL == pFd) || (INVALID_SOCKET == *pFd) ) return NDFalse;

	return setsockoption( *pFd, SOL_SOCKET, SO_SNDBUF, (const void*)&size, sizeof(size) );
}


NDUint32 NDSockOps::getSendBufferSize( SOCKET* pFd )
{
	if ( ( NULL == pFd ) || ( INVALID_SOCKET == *pFd ) )	return 0;

	int nSendBufSize;
	int nLen = sizeof(nSendBufSize);
	if ( getsockoption( *pFd, SOL_SOCKET, SO_SNDBUF, &nSendBufSize, &nLen ) )
	{
		return nSendBufSize;
	}
	return 0;
}


NDBool NDSockOps::setReuseAddr( SOCKET* pFd )
{
	if ( (NULL == pFd) || (INVALID_SOCKET == *pFd) ) return NDFalse;
	 
	NDUint32 option = 1;
	return setsockoption( *pFd, SOL_SOCKET, SO_REUSEADDR, (const void*)&option, sizeof(option) ) ;
}


NDBool NDSockOps::isReuseAddr( SOCKET* pFd )
{
	if ( ( NULL == pFd ) || ( INVALID_SOCKET == *pFd ) )	return NDFalse;

	int nReuse;
	int nLen = sizeof(nReuse);
	return getsockoption( *pFd, SOL_SOCKET, SO_REUSEADDR, &nReuse, &nLen );
}

NDBool NDSockOps::setLinger( SOCKET* pFd )
{
	if( (NULL == pFd) || (INVALID_SOCKET == *pFd) )	return NDFalse;

	struct linger ling = {1, 0};
	return setsockoption( *pFd, SOL_SOCKET, SO_LINGER, (const void*)&ling, sizeof(ling) );
}

void NDSockOps::shutdownSendPipe( SOCKET* pFd )
{
	if( (NULL == pFd) || (INVALID_SOCKET == *pFd) )	return;

#ifdef WIN32
	::shutdown( *pFd, SD_SEND );
#else
	::shutdown( *pFd, SHUT_WR );
#endif
}

void NDSockOps::closeSocket( SOCKET* pFd )
{
	if( (NULL == pFd) || (INVALID_SOCKET == *pFd) )	return;

#ifdef WIN32
	::shutdown( *pFd, SD_BOTH );
	::closesocket( *pFd );
#else
	close( *pFd );
#endif

	*pFd = INVALID_SOCKET;
}

NDBool NDSockOps::setsockoption( SOCKET s, int level, int optname, const void* optval, int optlen )
{
#ifdef WIN32
	return ( 0 == ::setsockopt( s, level, optname, (const char*)optval, optlen ) );
#else
	return ( 0 == ::setsockopt( s, level, optname, (const void*)optval, optlen ) );
#endif
}

NDBool NDSockOps::getsockoption( SOCKET s, int level, int optname, void* optval,int* optlen )
{
#ifdef WIN32
	return ( 0 == ::getsockopt( s, level, optname, (char*)optval, optlen ) );
#else
	return ( 0 == ::getsockopt( s, level, optname, (void*)optval, (socklen_t*)optlen ) );
#endif	
}

NDBool NDSockOps::ioctlsocket( SOCKET s, long cmd, unsigned long *argp )
{
#ifdef WIN32
	return ( ::ioctlsocket( s, cmd, argp ) == 0 );
#else
	return ( ioctl( s, cmd, argp ) == 0 );
#endif
}

NDBool NDSockOps::connect( SOCKET* pFd, const char* szPeerIP, NDUint16 nPeerPort )
{
	if ( ( NULL == pFd ) || ( INVALID_SOCKET == *pFd ) || ( NULL == szPeerIP ) || ( '\0' == szPeerIP[0] ) || ( 0 == nPeerPort ) ) return NDFalse;

	struct sockaddr_in	sockAddr;
	memset( &sockAddr, 0, sizeof( sockAddr ) );

	sockAddr.sin_family			= AF_INET;					//地址族;
	sockAddr.sin_addr.s_addr	= inet_addr( szPeerIP );	//指定服务器的IP地址;
	sockAddr.sin_port			= htons( nPeerPort );		//指定链接的端口号;

	return ( 0 == ::connect( *pFd, (struct sockaddr*)(&sockAddr), sizeof(sockAddr) ) );
}

//ioctl(keyFd, FIONREAD, &b);
//得到缓冲区里有多少字节要被读取，然后将字节数放入b里面;
//接下来就可以用read了;
//read(keyFd, &b, sizeof(b));
NDUint32 NDSockOps::getSocketRecvBufferBytes( SOCKET* pFd )
{
	if ( ( NULL == pFd ) || ( INVALID_SOCKET == *pFd ) )	return 0;
	
	unsigned long argp = 0;
	if ( ioctlsocket( *pFd, FIONREAD, &argp ) )
	{
		return argp;
	}

	return 0;
}


//send把s的发送缓冲中的数据传到连接的另一端的,而是协议传的,send仅仅是把buf中的数据copy到s的发送缓冲区的剩余空间里;
//send返回值为0,是正确的,说明缓存区满了;
NDBool NDSockOps::send( SOCKET* pFd, const void* buf, NDUint32 len, NDUint32& refRealSendLen, NDUint32 flags )
{
	if ( ( NULL == pFd ) || ( INVALID_SOCKET == *pFd ) || ( NULL == buf ) )	return NDFalse;
#ifdef WIN32
	int nSend = ::send( *pFd, (const char*)buf, len, flags );
	if ( SOCKET_ERROR == nSend  )
	{
		int nError = WSAGetLastError();
		if ( WSAEWOULDBLOCK != nError )
		{
			refRealSendLen = nError;	//函数返回值为错误时,refRealSendLen存储错误码;
			return NDFalse;
		}

		refRealSendLen = 0;
		return NDTrue;
	}
	
	refRealSendLen = nSend;
	return NDTrue;
#else
	int nSend = ::send( *pFd, buf, len, flags );
	if ( SOCKET_ERROR == nSend  )
	{
		int nError = errno;
		if ( EWOULDBLOCK != nError )
		{
			refRealSendLen = nError;	//函数返回值为错误时,refRealSendLen存储错误码;
			return NDFalse;
		}

		refRealSendLen = 0;
		return NDTrue;
	}

	refRealSendLen = nSend;
	return NDTrue;
#endif
}

NDBool NDSockOps::recv( SOCKET* pFd, void* buf, NDUint32 len, NDUint32& refRealRecvLen, NDUint32 flags )
{
	if ( ( NULL == pFd ) || ( INVALID_SOCKET == *pFd ) || ( NULL == buf ) )	return NDFalse;
#ifdef WIN32
	int nRecv = ::recv( *pFd, (char*)buf, len, flags );
	if ( 0 == nRecv )
	{	//优雅关闭SOCKET的条件;
		refRealRecvLen = 0;
		return NDFalse;
	}
	if ( SOCKET_ERROR == nRecv )
	{
		int nError = WSAGetLastError();
		if ( WSAEWOULDBLOCK != nError )
		{
			refRealRecvLen = nError;	//函数返回值为错误时,refRealRecvLen存储错误码;
			return NDFalse;
		}

		refRealRecvLen = 0;
		return NDTrue;
	}
	
	refRealRecvLen = nRecv;
	return NDTrue;
#else
	int nRecv = ::recv( *pFd, buf, len, flags );
	if ( 0 == nRecv )
	{	//优雅关闭SOCKET的条件;
		refRealRecvLen = 0;
		return NDFalse;
	}
	if ( SOCKET_ERROR == nRecv )
	{
		int nError = errno;
		if ( EWOULDBLOCK != nError )
		{
			refRealRecvLen = nError;	//函数返回值为错误时,refRealRecvLen存储错误码;
			return NDFalse;
		}

		refRealRecvLen = 0;
		return NDTrue;
	}

	refRealRecvLen = nRecv;
	return NDTrue;
#endif
}

NDBool NDSockOps::isValidSocket( SOCKET* pFd )
{
	if ( ( NULL == pFd ) || ( INVALID_SOCKET == *pFd ) )	return NDFalse;

	int nError;
	int nLen = sizeof(nError);
	return getsockoption( *pFd, SOL_SOCKET, SO_ERROR, &nError, &nLen );
}

NDBool NDSockOps::bind( SOCKET* pServerSocketFd, NDUint16 nPort )
{
	if ( ( NULL == pServerSocketFd ) || ( INVALID_SOCKET == *pServerSocketFd ) )	return NDFalse;

	struct sockaddr_in	server_sockAddr;
	memset( &server_sockAddr, 0, sizeof( server_sockAddr ) );

	server_sockAddr.sin_family			= AF_INET;					//地址族;
	server_sockAddr.sin_addr.s_addr		= htonl(INADDR_ANY);		//指定服务器的IP地址(多张网卡时指定所有本机的IP);
	server_sockAddr.sin_port			= htons( nPort );			//指定链接的端口号;

	return ( 0 == ::bind( *pServerSocketFd, (const struct sockaddr*)(&server_sockAddr), sizeof(server_sockAddr) ) );
}

NDBool NDSockOps::listen( SOCKET* pServerSocketFd, NDUint32 backlog )
{
	if ( ( NULL == pServerSocketFd ) || ( INVALID_SOCKET == *pServerSocketFd ) )	return NDFalse;

	return ( 0 == ::listen( *pServerSocketFd, backlog ) );
}

NDBool NDSockOps::accept( SOCKET* pServerSocketFd, SOCKET* pClientSocketFd, NDSocketAddress& refClientAddr, NDUint32& refErrorCode )
{
	if ( ( NULL == pServerSocketFd ) || ( NULL == pClientSocketFd ) || ( INVALID_SOCKET == *pServerSocketFd ) )	return NDFalse;

	struct sockaddr_in	client_sockAddr;
#ifdef WIN32
	int nLen = sizeof(client_sockAddr);
#else
	socklen_t nLen = sizeof(client_sockAddr);
#endif
	*pClientSocketFd = ::accept( *pServerSocketFd, (struct sockaddr*)(&client_sockAddr), &nLen );
	if ( INVALID_SOCKET == *pClientSocketFd )
	{
#ifdef WIN32
		refErrorCode = WSAGetLastError();
#else
		refErrorCode = errno;
#endif
		return NDFalse;
	}

	refClientAddr.setIP( inet_ntoa(client_sockAddr.sin_addr) );
	refClientAddr.setPort( ntohs( client_sockAddr.sin_port ) );

	return NDTrue;
}

NDBool NDSockOps::isReadEvent( SOCKET* pFd )
{
	if ( NULL == pFd || INVALID_SOCKET == *pFd )	return NDFalse;

	fd_set	readFDS;
	FD_ZERO(&readFDS);
	FD_SET( *pFd, &readFDS );

	timeval	timeout;
	timeout.tv_sec	= 0;
	timeout.tv_usec	= 0;

	int nRet = ::select( (int)*pFd + 1, &readFDS, NULL, NULL, &timeout );
	return ( nRet > 0 && FD_ISSET( *pFd, &readFDS ) );
}

NDBool NDSockOps::isWriteEvent( SOCKET* pFd )
{
	if ( NULL == pFd || INVALID_SOCKET == *pFd )	return NDFalse;

	fd_set	writeFDS;
	FD_ZERO(&writeFDS);
	FD_SET( *pFd, &writeFDS );

	timeval	timeout;
	timeout.tv_sec	= 0;
	timeout.tv_usec	= 0;

	int nRet = ::select( (int)*pFd + 1, NULL, &writeFDS, NULL, &timeout );
	return ( nRet > 0 && FD_ISSET( *pFd, &writeFDS ) );
}





_NDSHAREBASE_END

