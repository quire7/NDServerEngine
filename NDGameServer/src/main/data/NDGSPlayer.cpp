#include "main/data/NDGSPlayer.h"


#include "protocol/NDGameToNDData/NDGS2DS_PlayerLogin.h"

#include "protocol/NDGatewayToNDGame/NDGWS2GS_PlayerLogin.h"

#include "main/local/NDGameServer.h"

NDGSPlayer::NDGSPlayer()
{
	clear();
}

NDGSPlayer::~NDGSPlayer()
{
	release();
	clear();
}

void NDGSPlayer::clear()
{
	memset( m_szSessionKey, 0, sizeof(m_szSessionKey) );
	m_bRecvAllDSPlayerData	= NDFalse;
	m_pDSServerInfo			= NULL;
	m_nGTWSServerID			= ND_INVALID_ID;
	memset( &m_NDPlayerMain, 0, sizeof(m_NDPlayerMain) );
}

void NDGSPlayer::release()
{
	
}

NDBool NDGSPlayer::init()
{
	return NDTrue;
}

void NDGSPlayer::setPlayerMain(const NDPlayerMain& refData)
{	
	m_NDPlayerMain = refData;
}

NDUint32 NDGSPlayer::getDSServerID() const
{
	if ( NULL == m_pDSServerInfo )
	{
		return ND_INVALID_ID;
	}

	return m_pDSServerInfo->getServerID();
}

NDUint32 NDGSPlayer::getDSSessionID() const
{
	if ( NULL == m_pDSServerInfo )
	{
		return ND_INVALID_ID;
	}

	return m_pDSServerInfo->getSessionID();
}

NDBool NDGSPlayer::sendProtocolToDS(NDProtocol& refProtocol)
{
	NDUint32 nDSServerSessionID = getDSSessionID();
	if ( nDSServerSessionID == ND_INVALID_ID )
	{
		return NDFalse;
	}

	return NDServerManager::getSingleton().sendToServer( refProtocol, nDSServerSessionID );
}

NDBool NDGSPlayer::sendPlayerMainToDS()
{
	return NDTrue;
}

NDBool NDGSPlayer::sendPlayerLoginToDS()
{
	NDGS2DS_PlayerLogin_Req playerLoginReq;
	playerLoginReq.m_nPlayerGUID	= m_NDPlayerMain.m_nPlayerGUID;
	playerLoginReq.m_nGameServerID	= sNDGameServer.getLocalServerID();

	return sendProtocolToDS( playerLoginReq );
}

NDBool NDGSPlayer::sendPlayerLoginToWS()
{
	return NDTrue;
}


NDBool NDGSPlayer::sendProtocolToGTWS(NDProtocol& refProtocol)
{
	NDRemoteServerManager* pGatewayServerManager = sNDGameServer.gatewayServerManager();
	if ( NULL == pGatewayServerManager )
	{
		NDLOG_ERROR( " NDGSPlayer::sendProtocolToGTWS pGatewayServerManager is NULL." )
		return NDFalse;
	}

	NDRemoteServerInfo* pGTWSServerInfo = pGatewayServerManager->getRemoteServerInfoByServerID( (NDUint16)m_nGTWSServerID );
	if ( NULL == pGTWSServerInfo )
	{
		NDLOG_ERROR( " NDGSPlayer::sendProtocolToGTWS pGTWSServerInfo is NULL. nGTWSServerID:[%u].", m_nGTWSServerID );
		return NDFalse;
	}

	return NDServerManager::getSingleton().sendToClient( refProtocol, pGTWSServerInfo->getSessionID() );
}

NDBool NDGSPlayer::sendPlayerLoginDataSuccessToGTWS()
{
	NDGS2GTWS_PlayerLoginDataSuccess_Nty	playerLoginDataSuccessNty;
	playerLoginDataSuccessNty.m_nPlayerGUID		= m_NDPlayerMain.m_nPlayerGUID;
	playerLoginDataSuccessNty.m_nGameServerID	= sNDGameServer.getLocalServerID();

	return sendProtocolToGTWS( playerLoginDataSuccessNty );
}





