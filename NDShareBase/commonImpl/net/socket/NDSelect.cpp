

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

	//���ü����ɶ�,��д;
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

	//��ռ����ɶ�,��д;
	FD_CLR( fd, &m_readFDs[SELECT_BAK] );
	FD_CLR( fd, &m_writeFDs[SELECT_BAK] );
	FD_CLR( fd, &m_readFDs[SELECT_USE] );
	FD_CLR( fd, &m_writeFDs[SELECT_USE] );

	m_nMaxFD = INVALID_SOCKET;

	return NDTrue;
}

//�������:;
//WSANOTINITIALISED����ʹ�ô�API֮ǰӦ���ȳɹ��ص���WSAStartup();
//WSAENETDOWN��WINDOWS�׽ӿ�ʵ�ּ�⵽������ϵͳʧЧ;
//WSAEINVAL����ʱʱ��ֵ�Ƿ�;
//WSAEINTR��ͨ��һ��WSACancelBlockingCall()��ȡ��һ���������ģ�����;
//WSAEINPROGRESS��һ��������WINDOWS�׽ӿڵ�������������;
//WSAENOTSOCK�������ּ����а����з��׽ӿڵ�Ԫ��;

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

	//���ü�����д;
	FD_SET( fd, &m_writeFDs[SELECT_BAK] );

	return NDTrue;
}

NDBool NDSelect::unregisterWriteEvent( SOCKET fd )
{
	if ( INVALID_SOCKET == fd || INVALID_SOCKET == m_nMaxFD )	return	NDFalse;

	//��ռ�����д;
	FD_CLR( fd, &m_writeFDs[SELECT_BAK] );

	return NDTrue;
}

_NDSHAREBASE_END

