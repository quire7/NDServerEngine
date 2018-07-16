#include "net/session/NDServerSession.h"

#include "net/session/NDSessionManager.h"
#include "net/socket/NDServerSocket.h"




_NDSHAREBASE_BEGIN

NDServerSession::NDServerSession( NDUint32 nSessionID ) : NDSession( nSessionID ), m_nLastPingSecondTime( 0 )
{
	m_pSocket = new NDServerSocket;
}

NDServerSession::~NDServerSession()
{
	SAFE_DELETE( m_pSocket )
}

void NDServerSession::destroy()
{
	NDSessionManager::getInstance()->popCommonDisconnectNtyProtocol( m_nSessionID, m_nDisconnectionType );

	NDSessionManager::getInstance()->releaseServerSession( m_nSessionID );	
}

_NDSHAREBASE_END

