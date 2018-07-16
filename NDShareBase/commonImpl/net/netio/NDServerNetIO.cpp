#include "net/netio/NDServerNetIO.h"

#include "NDShareBaseGlobal.h"
#include "net/socket/NDSocketAddress.h"
#include "net/socket/NDSocketAcceptor.h"

#include "net/session/NDServerSession.h"
#include "net/session/NDSessionManager.h"

#include "net/stream/NDProtocolPacket.h"

#include "net/process/NDServerTask.h"


_NDSHAREBASE_BEGIN

NDSocketEnv NDServerNetIO::m_pNDSocketEnv;

NDServerNetIO::NDServerNetIO( void ) : m_pNDSocketAcceptor( new NDSocketAcceptor )
{
}

NDServerNetIO::~NDServerNetIO( void )
{
	SAFE_DELETE( m_pNDSocketAcceptor )
}

NDBool NDServerNetIO::startServer( const char* pHostIP, NDUint16 nPort )
{
	if ( NULL == m_pNDSocketAcceptor || NULL == pHostIP )	return NDFalse;
	return m_pNDSocketAcceptor->open( nPort );
}


NDBool NDServerNetIO::accept( NDUint8 nMaxConnectionPer )
{
	if ( NULL == m_pNDSocketAcceptor )	return NDFalse;

	if ( m_pNDSocketAcceptor->isAccept() )
	{
		
		NDTime nCurSecondTime = NDShareBaseGlobal::getCurSecondTimeOfUTC();

		do 
		{
			NDUint32 nErrorCode = 0;

			NDServerSession* pServerSession = (NDServerSession*)NDSessionManager::getInstance()->createServerSession();
			if ( NDFalse == m_pNDSocketAcceptor->accept( pServerSession->getSocket(), nErrorCode ) )
			{
				NDSessionManager::getInstance()->releaseServerSession( pServerSession->getSessionID() );
				break;
			}
			
			if ( NDFalse == pServerSession->initSession() )
			{
				NDSessionManager::getInstance()->releaseServerSession( pServerSession->getSessionID() );
				continue;
			}

			pServerSession->setLastPingSecondTime( nCurSecondTime );

			if ( NDFalse == NDSessionManager::getInstance()->addWorkServerSession( pServerSession ) )
			{
				NDSessionManager::getInstance()->releaseServerSession( pServerSession->getSessionID() );
				continue;
			}

			--nMaxConnectionPer;
		} while ( nMaxConnectionPer > 0 );
	}

	return NDTrue;
}

NDBool NDServerNetIO::sendToClient( NDProtocol& rProtocol, NDUint32 nSessionID, NDBool bCompression, NDBool bEncrypt, NDBool bCrc )
{
	if ( NULL == m_pNDSocketAcceptor )	return NDFalse;

	NDUint16 nProDataHeadBitWise = 0;
	if ( bCompression )	nProDataHeadBitWise |= ND_PDHMSG_COMPRESSION;
	if ( bEncrypt )		nProDataHeadBitWise |= ND_PDHMSG_ENCRYPT;
	if ( bCrc )			nProDataHeadBitWise |= ND_PDHMSG_CRC32;

	return NDSessionManager::getInstance()->sendToClient( rProtocol, nSessionID, nProDataHeadBitWise );
}

NDBool NDServerNetIO::disConnect( NDUint32 nSessionID )
{
	if ( NULL == m_pNDSocketAcceptor )	return NDFalse;

	//相对于客户端的连接,但是实质是服务器的Session;
	NDSession* pSession = NDSessionManager::getInstance()->findServerSession( nSessionID );
	if ( NULL == pSession )
	{
		return NDFalse;
	}
	
	pSession->handleClose();

	return NDTrue;
}

NDBool NDServerNetIO::getClientAddress( NDUint32 nSessionID, NDSocketAddress& refClientAddress )
{
	if ( NULL == m_pNDSocketAcceptor )	return NDFalse;

	//相对于客户端的连接,但是实质是服务器的Session;
	NDSession* pSession = NDSessionManager::getInstance()->findServerSession( nSessionID );
	if ( NULL == pSession )
	{
		return NDFalse;
	}	

	refClientAddress.setIP( pSession->getPeerIP() );
	refClientAddress.setPort( pSession->getPeerPort() );

	return NDTrue;
}




NDBool NDServerNetIO::setSessionProtocolType( NDUint32 nSessionID, NDUint8 nSessionProtocolType )
{
	return NDSessionManager::getInstance()->setServerSessionProtocolType( nSessionID, nSessionProtocolType );
}


NDBool NDServerNetIO::setProcessor( NDProcess* pProcess )
{
	return NDServerTask::getInstance()->setProcessor( pProcess );
}

NDBool NDServerNetIO::checkSessionTimerFunction()
{
	NDSessionManager::getInstance()->updateServerSessionMap();
	NDSessionManager::getInstance()->closeInvalidWorkSession();
	NDSessionManager::getInstance()->updateReleaseSessionMap();	
	return NDTrue;
}

NDBool NDServerNetIO::setCommonDisconnectNtyProtocol( NDProtocol* pDisconnectNtyProtocol )
{
	return NDSessionManager::getInstance()->setCommonDisconnectNtyProtocol( pDisconnectNtyProtocol );
}

_NDSHAREBASE_END

