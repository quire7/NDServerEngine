#include "NDLinuxEpoll.h"

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
	epv.events		= (EPOLLIN|EPOLLOUT);	//ÉèÖÃ¼àÌý¿É¶Á¿ÉÐ´;
	epv.data.fd		= fd;
	epv.data.ptr	= pData;

	return ( 0 == epoll_ctl( m_nEpoolFD, EPOLL_CTL_ADD, fd, &epv ) );
}

NDBool NDLinuxEpoll::unregisterReadWriteEvent( SOCKET fd, void* pData )
{
	if ( INVALID_SOCKET == m_nEpoolFD )	return NDFalse;

	struct epoll_event epv = { 0, {0} };
	epv.events		= (EPOLLIN|EPOLLOUT);	//Çå¿Õ¼àÌý¿É¶Á¿ÉÐ´;
	epv.data.fd		= fd;
	epv.data.ptr	= pData;

	return ( 0 == epoll_ctl( m_nEpoolFD, EPOLL_CTL_DEL, fd, &epv ) );
}

NDBool NDLinuxEpoll::registerWriteEvent( SOCKET fd, void* pData )
{
	if ( INVALID_SOCKET == m_nEpoolFD )	return NDFalse;

	struct epoll_event epv = { 0, {0} };
	epv.events		= (EPOLLIN|EPOLLOUT);			//ÉèÖÃ¼àÌý¿ÉÐ´;
	epv.data.fd		= fd;
	epv.data.ptr	= pData;

	return ( 0 == epoll_ctl( m_nEpoolFD, EPOLL_CTL_MOD, fd, &epv ) );
}

NDBool NDLinuxEpoll::unregisterWriteEvent( SOCKET fd, void* pData )
{
	if ( INVALID_SOCKET == m_nEpoolFD )	return NDFalse;

	struct epoll_event epv = { 0, {0} };
	epv.events		= (EPOLLIN);					//Çå¿Õ¼àÌý¿ÉÐ´;
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

	refNum = nFDS;

	return NDTrue;
}





_NDSHAREBASE_END

