#include "net/session/NDClientSession.h"

#include "net/socket/NDSockOps.h"
#include "net/socket/NDClientSocket.h"

#include "net/session/NDSessionManager.h"



_NDSHAREBASE_BEGIN

NDClientSession::NDClientSession( NDUint32 nSessionID ) : NDSession( nSessionID )
{
	m_pSocket = new NDClientSocket;
}

NDClientSession::~NDClientSession()
{
	SAFE_DELETE( m_pSocket )
}

void NDClientSession::destroy()
{
	NDSessionManager::getInstance()->releaseClientSession( m_nSessionID );
}

NDBool NDClientSession::connect( const char* pszHostIP, NDUint16 nPort )
{
	if (NULL == pszHostIP)
	{
		return NDFalse;
	}

	if ( NDFalse == ((NDClientSocket*)m_pSocket)->connect( pszHostIP, nPort ) )
	{
		return NDFalse;
	}

	if ( NDFalse == initSession() )
	{
		return NDFalse;
	}

	return  NDTrue;
}

_NDSHAREBASE_END

