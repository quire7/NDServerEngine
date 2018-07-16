#include "net/session/NDSessionManager.h"

#include "NDSessionManagerImpl.h"



_NDSHAREBASE_BEGIN

NDSessionManager*	NDSessionManager::m_pSessionManager = NULL;

NDSessionManager::NDSessionManager(void) : m_pSessionManagerImpl( new NDSessionManagerImpl )
{
}

NDSessionManager::~NDSessionManager(void)
{
	SAFE_DELETE( m_pSessionManagerImpl );
}

NDSessionManager*	NDSessionManager::getInstance()
{
	if ( NULL == m_pSessionManager )
	{
		m_pSessionManager = new NDSessionManager();
	}
	return m_pSessionManager;
}

void NDSessionManager::releaseInstance()
{
	SAFE_DELETE( m_pSessionManager );
}

NDBool NDSessionManager::sendToServer( NDProtocol& rProtocol, NDUint32 nSessionID, NDUint16 nProDataHeadBitWise )
{
	return m_pSessionManagerImpl->sendToServer( rProtocol, nSessionID, nProDataHeadBitWise );
}

NDBool NDSessionManager::sendToClient( NDProtocol& rProtocol, NDUint32 nSessionID, NDUint16 nProDataHeadBitWise )
{
	return m_pSessionManagerImpl->sendToClient( rProtocol, nSessionID, nProDataHeadBitWise );
}

NDSession* NDSessionManager::createServerSession()
{
	return m_pSessionManagerImpl->createServerSession();
}

NDSession* NDSessionManager::createClientSession()
{
	return m_pSessionManagerImpl->createClientSession();
}

NDBool	NDSessionManager::addWorkServerSession( NDSession* pServerSession )
{
	return m_pSessionManagerImpl->addWorkServerSession( pServerSession );
}

NDBool	NDSessionManager::addWorkClientSession( NDSession* pClientSession )
{
	return m_pSessionManagerImpl->addWorkClientSession( pClientSession );
}

NDSession*  NDSessionManager::removePrepServerSession( NDUint32 nSessionID )
{
	return m_pSessionManagerImpl->removePrepServerSession( nSessionID );
}

NDSession* NDSessionManager::findServerSession( NDUint32 nSessionID )
{
	return m_pSessionManagerImpl->findServerSession( nSessionID );
}

NDSession* NDSessionManager::findClientSession( NDUint32 nSessionID )
{
	return m_pSessionManagerImpl->findClientSession( nSessionID );
}

void NDSessionManager::releaseServerSession( NDUint32 nSessionID )
{
	m_pSessionManagerImpl->releaseServerSession( nSessionID );
}

void NDSessionManager::releaseClientSession( NDUint32 nSessionID )
{
	m_pSessionManagerImpl->releaseClientSession( nSessionID );
}

void NDSessionManager::updateReleaseSessionMap()
{
	m_pSessionManagerImpl->updateReleaseSessionMap();
}

void NDSessionManager::updateServerSessionMap()
{
	m_pSessionManagerImpl->updateServerSessionMap();
}

NDBool NDSessionManager::eventLoop()
{
	return m_pSessionManagerImpl->eventLoop();
}

NDBool NDSessionManager::setInvalidWorkSession( SOCKET nSocketID )
{
	if ( INVALID_SOCKET == nSocketID )	return NDFalse;

	m_pSessionManagerImpl->setInvalidWorkSession( nSocketID );

	return NDTrue;
}

void NDSessionManager::closeInvalidWorkSession()
{
	return m_pSessionManagerImpl->closeInvalidWorkSession();
}

NDBool NDSessionManager::registerWriteEvent( SOCKET fd )
{
	if ( INVALID_SOCKET == fd )	return NDFalse;

	return m_pSessionManagerImpl->registerWriteEvent( fd );
}

NDBool NDSessionManager::unregisterWriteEvent( SOCKET fd )
{
	if ( INVALID_SOCKET == fd )	return NDFalse;

	return m_pSessionManagerImpl->unregisterWriteEvent( fd ); 
}

NDBool NDSessionManager::setCommonDisconnectNtyProtocol( NDProtocol* pDisconnectNtyProtocol )
{
	return m_pSessionManagerImpl->setCommonDisconnectNtyProtocol( pDisconnectNtyProtocol );
}

NDBool NDSessionManager::popCommonDisconnectNtyProtocol( NDUint32 nSessionID, NDUint8 nDisconnectionType )
{
	return m_pSessionManagerImpl->popCommonDisconnectNtyProtocol( nSessionID, nDisconnectionType );
}

void NDSessionManager::setSessionParsePacketFun( PParsePacketFun parsePacketFun )
{
	return m_pSessionManagerImpl->setSessionParsePacketFun( parsePacketFun );
}	

NDBool NDSessionManager::setServerSessionProtocolType(NDUint32 nSessionID, NDUint8 nSessionProtocolType)
{
	return m_pSessionManagerImpl->setServerSessionProtocolType( nSessionID, nSessionProtocolType );
}

NDBool NDSessionManager::setClientSessionProtocolType(NDUint32 nSessionID, NDUint8 nSessionProtocolType)
{
	return m_pSessionManagerImpl->setClientSessionProtocolType( nSessionID, nSessionProtocolType );
}

void NDSessionManager::setMaxSessionType(NDUint8 nMaxSessionType)
{
	return m_pSessionManagerImpl->setMaxSessionType( nMaxSessionType );
}

void NDSessionManager::setSpecialProtocol(NDUint16 nSpecialProtocolStart, NDUint16 nSpecialProtocolEnd)
{
	return m_pSessionManagerImpl->setSpecialProtocol( nSpecialProtocolStart, nSpecialProtocolEnd );
}

NDBool NDSessionManager::setDisposeSessionProtocol(NDUint8 nSessionType, NDUint16 nProtocolStart, NDUint16 nProtocolEnd)
{
	return m_pSessionManagerImpl->setDisposeSessionProtocol( nSessionType, nProtocolStart, nProtocolEnd );
}


_NDSHAREBASE_END

