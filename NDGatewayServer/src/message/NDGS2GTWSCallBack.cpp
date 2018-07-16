#include "message/NDGS2GTWSCallBack.h"


#include "protocol/NDGatewayToNDGame/NDGWS2GS_PlayerLogin.h"


#include "main/data/NDGTWSPlayer.h"
#include "main/local/NDGatewayServer.h"



NDGS2GTWSCallBack::NDGS2GTWSCallBack(void)
{
	NDRegisterCallBackMACRO( sNDGatewayServer.dataProcess(), CMDP_NDGS2GTWS_PlayerLoginDataSuccess_Nty, this )
}

NDGS2GTWSCallBack::~NDGS2GTWSCallBack(void)
{
}

NDBool NDGS2GTWSCallBack::process( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	NDBool	bRet = NDFalse;

	switch (protocolHeader.m_nProtocolID)
	{
	case CMDP_NDGS2GTWS_PlayerLoginDataSuccess_Nty:
		{
			bRet = gs2gtwsPlayerLoginSuccessNtyDispose( rIStream, protocolHeader );
		}
		break;
	}

	return bRet;
}



NDBool NDGS2GTWSCallBack::gs2gtwsPlayerLoginSuccessNtyDispose( NDIStream& rIStream, NDProtocolHeader& header )
{
	NDGS2GTWS_PlayerLoginDataSuccess_Nty playerLoginDataSuccessNty;
	if ( NDFalse == playerLoginDataSuccessNty.deserialize(rIStream) ) 
	{
		NDLOG_ERROR( " [NDGS2GTWSCallBack::gs2gtwsPlayerLoginSuccessNtyDispose] NDGS2GTWS_PlayerLoginDataSuccess_Nty deserialize failed!" )
		return NDFalse;
	}

	NDGTWSPlayerManager* pNDGTWSPlayerManager = sNDGatewayServer.playerManager();
	if ( NULL == pNDGTWSPlayerManager )
	{
		return NDFalse;
	}
	NDGTWSPlayer* pPlayer = pNDGTWSPlayerManager->getPlayerByPlayerGUID( playerLoginDataSuccessNty.m_nPlayerGUID );
	if ( NULL == pPlayer )
	{
		pPlayer = pNDGTWSPlayerManager->getIdlePlayer();
	}
	else
	{
		NDLOG_WARNING( " [NDGS2GTWSCallBack::gs2gtwsPlayerLoginSuccessNtyDispose] find player has exist. PlayerGUID:[%llu].", playerLoginDataSuccessNty.m_nPlayerGUID );
	}

	pPlayer->setPlayerGUID( playerLoginDataSuccessNty.m_nPlayerGUID );

	const NDServerInfo* pGSInfo = NDServerManager::getSingleton().getConnServerInfo( GAME_SERVER, playerLoginDataSuccessNty.m_nGameServerID );
	if ( NULL == pGSInfo )
	{
		NDLOG_WARNING( " [NDGS2GTWSCallBack::gs2gtwsPlayerLoginSuccessNtyDispose] can't find pGSInfo. GameServerID:[%u].", playerLoginDataSuccessNty.m_nGameServerID );
	}
	pPlayer->setGSServerInfo( pGSInfo );

	pNDGTWSPlayerManager->addPlayer( pPlayer );

	return NDTrue;
}


