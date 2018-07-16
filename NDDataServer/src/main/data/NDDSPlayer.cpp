#include "main/data/NDDSPlayer.h"


#include "protocol/NDGameToNDData/NDGS2DS_PlayerData.h"


#include "main/local/NDDataServer.h"


NDDSPlayer::NDDSPlayer()
{
	clear();
}

NDDSPlayer::~NDDSPlayer()
{
	clear();
}

void NDDSPlayer::clear()
{
	m_playerState		= ND_INVALID_ID;
	m_nGSServerID		= ND_INVALID_ID;
	m_playerAccount.clear();
	m_playerMain.clear();

	memset( m_tableDirtyDataFlag, 0, sizeof(m_tableDirtyDataFlag) );
	memset( m_tableOperateTime, 0, sizeof(m_tableOperateTime) );

}

NDBool NDDSPlayer::init()
{
	return NDTrue;
}

void NDDSPlayer::release()
{

}

NDBool NDDSPlayer::insertUpdatePlayerAccount(const NDPlayerAccount& refPlayerAccount)
{
	if ( refPlayerAccount.m_nOperType < EDataSourceOperType_SELECT ||  refPlayerAccount.m_nOperType > EDataSourceOperType_UPDATE )
	{
		return NDFalse;
	}

	m_playerAccount = refPlayerAccount;

	return NDTrue;
}

NDBool NDDSPlayer::insertUpdatePlayerMain(const NDPlayerMain& refPlayerMain)
{
	if ( refPlayerMain.m_nOperType < EDataSourceOperType_SELECT ||  refPlayerMain.m_nOperType > EDataSourceOperType_UPDATE )
	{
		return NDFalse;
	}

	m_playerMain = refPlayerMain;

	return NDTrue;
}


NDBool NDDSPlayer::sendAllPlayerDataToGS()
{
	if ( ND_INVALID_ID == m_nGSServerID )
	{
		return NDFalse;
	}

	if ( NDFalse == sendPlayerMainToGS() )
	{
		return NDFalse;
	}

	return NDTrue;
}

NDBool NDDSPlayer::sendPlayerMainToGS()
{
	NDDS2GS_SendPlayerMain_Nty sendPlayerMainNty;
	sendPlayerMainNty.m_playerMain = m_playerMain;

	return sendProtocolToGS( sendPlayerMainNty );
}

NDBool NDDSPlayer::sendProtocolToGS(NDProtocol& refProtocol)
{
	NDRemoteServerManager* pGameServerManager = sNDDataServer.gameServerManager();
	if ( NULL == pGameServerManager )
	{
		NDLOG_ERROR( " [NDDSPlayer::sendProtocolToGS] pGameServerManager is NULL." )
		return NDFalse;
	}

	NDRemoteServerInfo* pGSServerInfo = pGameServerManager->getRemoteServerInfoByServerID( (NDUint16)m_nGSServerID );
	if ( NULL == pGSServerInfo )
	{
		NDLOG_ERROR( " [NDDSPlayer::sendProtocolToGS] pGSServerInfo is NULL. nGSServerID:[%u].", m_nGSServerID );
		return NDFalse;
	}

	return NDServerManager::getSingleton().sendToClient( refProtocol, pGSServerInfo->getSessionID() );
}

