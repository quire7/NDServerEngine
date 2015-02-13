#include "net/socket/NDSocketAcceptor.h"

#include "net/socket/NDSockOps.h"
#include "net/socket/NDSocket.h"




_NDSHAREBASE_BEGIN

NDSocketAcceptor::NDSocketAcceptor() : m_nListenSocketID( INVALID_SOCKET )
{
}

NDSocketAcceptor::~NDSocketAcceptor()
{	
}

NDBool NDSocketAcceptor::open( NDUint16 nPort )
{
	m_nListenSocketID = NDSockOps::createTCPFileDescriptor();
	if ( INVALID_SOCKET ==  m_nListenSocketID )
	{
		return NDFalse;
	}

	//Reuse address before bind();
	if ( NDFalse == NDSockOps::setReuseAddr( &m_nListenSocketID ) )
	{
		return NDFalse;
	}

	//set socket non blocking;
	if ( NDFalse == NDSockOps::setNonblocking( &m_nListenSocketID ) )
	{
		return NDFalse;
	}

	//bind address to socket;
	if ( NDFalse == NDSockOps::bind( &m_nListenSocketID, nPort ) )
	{
		return NDFalse;
	}
	
	//set listen queue size;
	if ( NDFalse == NDSockOps::listen( &m_nListenSocketID, 5 ) )
	{
		return NDFalse;
	}

	return NDTrue;
}

NDBool NDSocketAcceptor::accept( NDSocket* pSocket, NDUint32& refErrorCode )
{
	if ( NULL == pSocket || INVALID_SOCKET == m_nListenSocketID )  return NDFalse;

	return NDSockOps::accept( &m_nListenSocketID, &(pSocket->getSOCKETRef()), pSocket->getPeerAddr(), refErrorCode );
}

NDBool NDSocketAcceptor::isAccept()
{
	if ( INVALID_SOCKET == m_nListenSocketID )  return NDFalse;

	return NDSockOps::isReadEvent( &m_nListenSocketID );
}



_NDSHAREBASE_END

