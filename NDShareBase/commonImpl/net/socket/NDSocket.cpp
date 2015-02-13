#include "net/socket/NDSocket.h"

#include "net/socket/NDSockOps.h"




_NDSHAREBASE_BEGIN


NDSocket::NDSocket(): m_nSocketID( INVALID_SOCKET )
{
}

NDSocket::NDSocket( const char* szPeerIP, NDUint16 nPeerPort ) : m_nSocketID( INVALID_SOCKET ), m_peerAddr( szPeerIP, nPeerPort )
{
}

NDSocket::~NDSocket()
{
}

void NDSocket::close()
{
	if ( INVALID_SOCKET == m_nSocketID )	return ;

	NDSockOps::closeSocket( &m_nSocketID );

	m_nSocketID = INVALID_SOCKET;
	m_peerAddr.clear();
}


void NDSocket::shutdownSendPipe()
{
	if ( INVALID_SOCKET == m_nSocketID )	return ;

	NDSockOps::shutdownSendPipe( &m_nSocketID );
}

NDBool NDSocket::isValidSocket()
{
	if ( INVALID_SOCKET == m_nSocketID )	return NDFalse;

	return NDSockOps::isValidSocket( &m_nSocketID );
}

NDBool NDSocket::send( const void* buf, NDUint32 len, NDUint32& refRealSendLen, NDUint32 flags /*= 0*/ )
{
	if ( INVALID_SOCKET == m_nSocketID )	return NDFalse;

	return NDSockOps::send( &m_nSocketID, buf, len, refRealSendLen, flags );
}

NDBool NDSocket::recv( void* buf, NDUint32 len, NDUint32& refRealRecvLen, NDUint32 flags /*= 0 */ )
{
	if ( INVALID_SOCKET == m_nSocketID )	return NDFalse;

	return NDSockOps::recv( &m_nSocketID, buf, len, refRealRecvLen, flags );
}

NDUint32 NDSocket::getRecvBufBytes()
{
	if ( INVALID_SOCKET == m_nSocketID )	return 0;

	return NDSockOps::getSocketRecvBufferBytes( &m_nSocketID );
}

NDBool NDSocket::isNonblocking()
{
	if ( INVALID_SOCKET == m_nSocketID )	return NDFalse;

	return NDSockOps::isNonblocking( &m_nSocketID );
}

NDBool NDSocket::setNonblocking()
{
	if ( INVALID_SOCKET == m_nSocketID )	return NDFalse;

	return NDSockOps::setNonblocking( &m_nSocketID );
}

NDUint32 NDSocket::getRecvBufferSize()
{
	if ( INVALID_SOCKET == m_nSocketID )	return 0;
	
	return NDSockOps::getRecvBufferSize( &m_nSocketID );
}

NDBool NDSocket::setRecvBufferSize( NDUint32 nSize )
{
	if ( INVALID_SOCKET == m_nSocketID )	return NDFalse;

	return NDSockOps::setRecvBufferSize( &m_nSocketID, nSize );
}

NDUint32 NDSocket::getSendBufferSize()
{
	if ( INVALID_SOCKET == m_nSocketID )	return 0;

	return NDSockOps::getSendBufferSize( &m_nSocketID );
}

NDBool NDSocket::setSendBufferSize( NDUint32 nSize )
{
	if ( INVALID_SOCKET == m_nSocketID )	return NDFalse;

	return NDSockOps::setSendBufferSize( &m_nSocketID, nSize );
}


NDBool NDSocket::isReuseAddr()
{
	if ( INVALID_SOCKET == m_nSocketID )	return NDFalse;

	return NDSockOps::isReuseAddr( &m_nSocketID );
}

NDBool NDSocket::setReuseAddr()
{
	if ( INVALID_SOCKET == m_nSocketID )	return NDFalse;

	return NDSockOps::setReuseAddr( &m_nSocketID );
}

NDBool NDSocket::setDisableBuffering()
{
	if ( INVALID_SOCKET == m_nSocketID )	return NDFalse;
	
	return NDSockOps::setDisableBuffering( &m_nSocketID );
}

NDBool NDSocket::setEnableBuffering()
{
	if ( INVALID_SOCKET == m_nSocketID )	return NDFalse;

	return NDSockOps::setEnableBuffering( &m_nSocketID );
}

_NDSHAREBASE_END

