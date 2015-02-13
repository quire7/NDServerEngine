

#include "net/socket/NDSelect.h"

#ifndef WIN32
#include <errno.h>				//for errno;
#include <algorithm>
#endif

_NDSHAREBASE_BEGIN

NDSelect::NDSelect()
{
	m_nMaxFD						= INVALID_SOCKET;
	m_timeout[SELECT_BAK].tv_sec	= 0;
	m_timeout[SELECT_BAK].tv_usec	= 0;

	FD_ZERO( &m_readFDs[SELECT_BAK] );
	FD_ZERO( &m_writeFDs[SELECT_BAK] );
	FD_ZERO( &m_readFDs[SELECT_USE] );
	FD_ZERO( &m_writeFDs[SELECT_USE] );
}

NDSelect::~NDSelect()
{

}

NDBool NDSelect::setMaxFD( SOCKET fd )
{
	if ( INVALID_SOCKET == fd )	return NDFalse;

	if ( INVALID_SOCKET == m_nMaxFD )
	{
		m_nMaxFD = fd;
		return NDTrue;
	}

#ifdef WIN32
	m_nMaxFD = max( fd, m_nMaxFD );
#else
	m_nMaxFD = std::max( fd, m_nMaxFD );
#endif
	

	return NDTrue;
}

NDBool NDSelect::registerReadWriteEvent( SOCKET fd )
{
	if ( INVALID_SOCKET == fd )		return	NDFalse;

	if ( FD_ISSET( fd, &m_readFDs[SELECT_BAK] ) )
	{
		return NDFalse;
	}

	//设置监听可读,可写;
	FD_SET( fd, &m_readFDs[SELECT_BAK] );
	FD_SET( fd, &m_writeFDs[SELECT_BAK] );

	if ( INVALID_SOCKET == m_nMaxFD )
	{
		m_nMaxFD = fd;
	}
	else
	{
#ifdef WIN32
		m_nMaxFD = max( fd, m_nMaxFD );
#else
		m_nMaxFD = std::max( fd, m_nMaxFD );
#endif
	}
	
	return NDTrue;	
}

NDBool NDSelect::unregisterReadWriteEvent( SOCKET fd )
{
	if ( INVALID_SOCKET == fd )	return	NDFalse;

	//清空监听可读,可写;
	FD_CLR( fd, &m_readFDs[SELECT_BAK] );
	FD_CLR( fd, &m_writeFDs[SELECT_BAK] );
	FD_CLR( fd, &m_readFDs[SELECT_USE] );
	FD_CLR( fd, &m_writeFDs[SELECT_USE] );

	m_nMaxFD = INVALID_SOCKET;

	return NDTrue;
}

//错误代码:;
//WSANOTINITIALISED：在使用此API之前应首先成功地调用WSAStartup();
//WSAENETDOWN：WINDOWS套接口实现检测到网络子系统失效;
//WSAEINVAL：超时时间值非法;
//WSAEINTR：通过一个WSACancelBlockingCall()来取消一个（阻塞的）调用;
//WSAEINPROGRESS：一个阻塞的WINDOWS套接口调用正在运行中;
//WSAENOTSOCK：描述字集合中包含有非套接口的元素;

NDBool NDSelect::run( NDUint32& refNum )
{
	if ( INVALID_SOCKET == m_nMaxFD )
	{
		refNum = 0;
		return NDFalse;
	}

	m_timeout[SELECT_USE].tv_sec	= m_timeout[SELECT_BAK].tv_sec;
	m_timeout[SELECT_USE].tv_usec	= m_timeout[SELECT_BAK].tv_usec;

	m_readFDs[SELECT_USE]			= m_readFDs[SELECT_BAK];
	m_writeFDs[SELECT_USE]			= m_writeFDs[SELECT_BAK];

	int nRet = ::select( (int)m_nMaxFD + 1, &m_readFDs[SELECT_USE], &m_writeFDs[SELECT_USE], NULL, &m_timeout[SELECT_USE] );
	if ( nRet <= SOCKET_ERROR )
	{
#ifdef WIN32
		refNum = WSAGetLastError();
#else
		refNum = errno;
#endif
		return NDFalse;
	}
	
	refNum = nRet;

	return NDTrue;
}

NDBool NDSelect::isReadEvent( SOCKET fd )
{
	return ( 0 != FD_ISSET( fd, &m_readFDs[SELECT_USE] ) );
}

NDBool NDSelect::isWriteEvent( SOCKET fd )
{
	return ( 0 != FD_ISSET( fd, &m_writeFDs[SELECT_USE] ) );
}

NDBool NDSelect::registerWriteEvent( SOCKET fd )
{
	if ( INVALID_SOCKET == fd || INVALID_SOCKET == m_nMaxFD )	return	NDFalse;

	//设置监听可写;
	FD_SET( fd, &m_writeFDs[SELECT_BAK] );

	return NDTrue;
}

NDBool NDSelect::unregisterWriteEvent( SOCKET fd )
{
	if ( INVALID_SOCKET == fd || INVALID_SOCKET == m_nMaxFD )	return	NDFalse;

	//清空监听可写;
	FD_CLR( fd, &m_writeFDs[SELECT_BAK] );

	return NDTrue;
}

_NDSHAREBASE_END

