#include "net/socket/NDClientSocket.h"

#include "net/socket/NDSockOps.h"




_NDSHAREBASE_BEGIN

NDClientSocket::NDClientSocket() : NDSocket()
{
}

NDClientSocket::NDClientSocket( const char* szPeerIP, NDUint16 nPeerPort ) : NDSocket( szPeerIP, nPeerPort ) 
{
	createSocket();
}

NDClientSocket::~NDClientSocket()
{
	close();
}

NDBool NDClientSocket::connect()
{
	if ( INVALID_SOCKET == m_nSocketID )	return NDFalse;

	if ( !isValidAddr() )
	{
		return NDFalse;
	}

	return NDSockOps::connect( &m_nSocketID, getPeerIP(), getPeerPort() );
}

NDBool NDClientSocket::connect( const char* szPeerIP, NDUint16 nPeerPort )
{
	if ( INVALID_SOCKET != m_nSocketID )	return NDFalse;

	if ( ( NULL == szPeerIP ) || ( '\0' == szPeerIP[0] ) || ( 0 == nPeerPort ) )
	{
		return NDFalse;
	}

	setPeerIP( szPeerIP );
	setPeerPort( nPeerPort );

	//create new socket;
	if ( NDFalse == createSocket() )
	{
		return NDFalse;
	}

	return NDSockOps::connect( &m_nSocketID, szPeerIP, nPeerPort );
}

NDBool NDClientSocket::reconnect( const char* szPeerIP, NDUint16 nPeerPort )
{
	if ( ( NULL == szPeerIP ) || ( '\0' == szPeerIP[0] ) || ( 0 == nPeerPort ) )
	{
		return NDFalse;
	}

	//delete old socket implement object;
	close();

	return connect( szPeerIP, nPeerPort );
}

NDBool NDClientSocket::createSocket()
{
	m_nSocketID = NDSockOps::createTCPFileDescriptor();
	
	return ( INVALID_SOCKET != m_nSocketID );
}






_NDSHAREBASE_END

