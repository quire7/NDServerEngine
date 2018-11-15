#include "NDLinuxEpoll.h"

#include "net/session/NDSession.h"

_NDSHAREBASE_BEGIN

NDLinuxEpoll::NDLinuxEpoll()
{
	m_nEpoolFD = epoll_create( EPOLL_EVENT_MAX );
}

NDLinuxEpoll::~NDLinuxEpoll()
{
	close( m_nEpoolFD );
}

NDBool NDLinuxEpoll::registerReadWriteEvent( SOCKET fd, void* pData )
{
	if ( INVALID_SOCKET == m_nEpoolFD )	return NDFalse;

	struct epoll_event epv = { 0, {0} };
	epv.events		= (EPOLLIN|EPOLLOUT);	//设置监听可读可写;
	epv.data.fd		= fd;
	epv.data.ptr	= pData;

	return ( 0 == epoll_ctl( m_nEpoolFD, EPOLL_CTL_ADD, fd, &epv ) );
}

NDBool NDLinuxEpoll::unregisterReadWriteEvent( SOCKET fd, void* pData )
{
	if ( INVALID_SOCKET == m_nEpoolFD )	return NDFalse;

	struct epoll_event epv = { 0, {0} };
	epv.events		= (EPOLLIN|EPOLLOUT);	//清空监听可读可写;
	epv.data.fd		= fd;
	epv.data.ptr	= pData;

	return ( 0 == epoll_ctl( m_nEpoolFD, EPOLL_CTL_DEL, fd, &epv ) );
}

NDBool NDLinuxEpoll::registerWriteEvent( SOCKET fd, void* pData )
{
	if ( INVALID_SOCKET == m_nEpoolFD )	return NDFalse;

	struct epoll_event epv = { 0, {0} };
	epv.events		= (EPOLLIN|EPOLLOUT);			//设置监听可写;
	epv.data.fd		= fd;
	epv.data.ptr	= pData;

	return ( 0 == epoll_ctl( m_nEpoolFD, EPOLL_CTL_MOD, fd, &epv ) );
}

NDBool NDLinuxEpoll::unregisterWriteEvent( SOCKET fd, void* pData )
{
	if ( INVALID_SOCKET == m_nEpoolFD )	return NDFalse;

	struct epoll_event epv = { 0, {0} };
	epv.events		= (EPOLLIN);					//清空监听可写;
	epv.data.fd		= fd;
	epv.data.ptr	= pData;

	return ( 0 == epoll_ctl( m_nEpoolFD, EPOLL_CTL_MOD, fd, &epv ) );
}

NDBool NDLinuxEpoll::run( NDUint32& refNum )
{
	if ( INVALID_SOCKET == m_nEpoolFD )	return NDFalse;

	int nFDS = epoll_wait( m_nEpoolFD, m_epollEvents, EPOLL_EVENT_MAX, 0 );
	if ( nFDS < 0 )
	{
		refNum = 0;
		return NDFalse;
	}

	for ( int i = 0; i < nFDS; ++i )
	{
		if ( m_epollEvents[i].events&EPOLLIN ) //接收到数据,读socket;
		{
			NDSession* pSession = (NDSession*)m_epollEvents[i].data.ptr;
			if (NULL != pSession)
				pSession->handleRead();
		}
		if ( m_epollEvents[i].events&EPOLLOUT ) //有数据待发送,写socket;
		{
			NDSession* pSession = (NDSession*)m_epollEvents[i].data.ptr;
			if (NULL != pSession)
				pSession->handleWrite();
		}
	}

	refNum = nFDS;

	return NDTrue;
}





_NDSHAREBASE_END

