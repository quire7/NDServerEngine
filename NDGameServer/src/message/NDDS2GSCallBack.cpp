#include "message/NDDS2GSCallBack.h"

#include "protocol/NDGameToNDData/NDGS2DS_PlayerLogin.h"
#include "protocol/NDGameToNDData/NDGS2DS_PlayerData.h"

#include "main/data/NDGSPlayer.h"
#include "main/local/NDGameServer.h"


NDDS2GSCallBack::NDDS2GSCallBack(void)
{
	NDRegisterCallBackMACRO( sNDGameServer.dataProcess(), CMDP_NDDS2GS_SendPlayerMain_Nty, this )
	NDRegisterCallBackMACRO( sNDGameServer.dataProcess(), CMDP_NDGS2DS_PlayerLogin_Res, this )
		
}

NDDS2GSCallBack::~NDDS2GSCallBack(void)
{

}

NDBool NDDS2GSCallBack::process(NDIStream& rIStream, NDProtocolHeader& protocolHeader)
{
	NDBool	bRet = NDFalse;

	switch (protocolHeader.m_nProtocolID)
	{
	case CMDP_NDDS2GS_SendPlayerMain_Nty:
		{
			bRet = ds2gsSendPlayerMainNtyDispose( rIStream, protocolHeader );
		}
		break;
	case CMDP_NDGS2DS_PlayerLogin_Res:
		{
			bRet = ds2gsPlayerLoginResDispose( rIStream, protocolHeader );
		}
		break;
	}

	return bRet;
}

NDBool NDDS2GSCallBack::ds2gsSendPlayerMainNtyDispose(NDIStream& rIStream, NDProtocolHeader& protocolHeader)
{
	NDDS2GS_SendPlayerMain_Nty sendPlayerMainNty;
	if ( NDFalse == sendPlayerMainNty.deserialize( rIStream ) )
	{
		NDLOG_ERROR( " [NDDS2GSCallBack::ds2gsSendPlayerMainNtyDispose] NDDS2GS_SendPlayerMain_Nty deserialize failed!" )
		return NDFalse;
	}

	NDGSPlayerManager* pNDGSPlayerManager = sNDGameServer.playerManager();
	if ( NULL == pNDGSPlayerManager )
	{
		NDLOG_ERROR( " [NDDS2GSCallBack::ds2gsSendPlayerMainNtyDispose] pNDGSPlayerManager is NULL." )
		return NDFalse;
	}
	
	const NDPlayerMain& refPlayerMain = sendPlayerMainNty.m_playerMain;
	NDGSPlayer* pPlayer = pNDGSPlayerManager->getPlayer( refPlayerMain.m_nPlayerGUID );
	if ( NULL == pPlayer )
	{
		NDLOG_ERROR( " [NDDS2GSCallBack::ds2gsSendPlayerMainNtyDispose] pPlayer is NULL. PlayerGUID:[%llu].", refPlayerMain.m_nPlayerGUID );
		return NDFalse;
	}

	if ( pPlayer->getRecvAllDSPlayerData() )
	{
		NDLOG_ERROR( " [NDDS2GSCallBack::ds2gsSendPlayerMainNtyDispose] PlayerDataOK on playermain. PlayerGUID:[%llu].", refPlayerMain.m_nPlayerGUID );
		return NDFalse;
	}

	pPlayer->setPlayerMain( refPlayerMain );


	NDLOG_INFO( " [NDDS2GSCallBack::ds2gsSendPlayerMainNtyDispose] recv playermain success. PlayerGUID:[%llu].", refPlayerMain.m_nPlayerGUID );

	return NDTrue;
}

NDBool NDDS2GSCallBack::ds2gsPlayerLoginResDispose(NDIStream& rIStream, NDProtocolHeader& protocolHeader)
{
	NDGS2DS_PlayerLogin_Res response;
	if ( NDFalse == response.deserialize( rIStream ) )
	{
		NDLOG_ERROR( " [NDDS2GSCallBack::ds2gsPlayerLoginResDispose] NDGS2DS_PlayerLogin_Res deserialize failed!" )
		return NDFalse;
	}

	NDGSPlayerManager* pNDGSPlayerManager = sNDGameServer.playerManager();
	if ( NULL == pNDGSPlayerManager )
	{
		NDLOG_ERROR( " [NDDS2GSCallBack::ds2gsPlayerLoginResDispose] pNDGSPlayerManager is NULL." )
		return NDFalse;
	}

	NDGSPlayer* pPlayer = pNDGSPlayerManager->getPlayer( response.m_nPlayerGUID );
	if ( NULL == pPlayer )
	{
		NDLOG_ERROR( " [NDDS2GSCallBack::ds2gsPlayerLoginResDispose] pPlayer is NULL. PlayerGUID:[%llu].", response.m_nPlayerGUID );
		return NDFalse;
	}

	if ( eND_SRS_OK != response.m_nErrorCode )
	{
		//加载玩家数据失败,则清除GS上玩家数据,防止由于坏账号导致内存长时间占用;
		NDLOG_ERROR( " [NDDS2GSCallBack::ds2gsPlayerLoginResDispose] player login Data response error. PlayerGUID:[%llu].", response.m_nPlayerGUID );
		return NDFalse;
	}

	//设置数据是完整的;
	pPlayer->setRecvAllDSPlayerData( NDTrue );


	//通知GTWS上的玩家可以登录;
	pPlayer->sendPlayerLoginDataSuccessToGTWS();

	
	NDLOG_INFO( " [NDDS2GSCallBack::ds2gsPlayerLoginResDispose] player login Data response success. PlayerGUID:[%llu].", response.m_nPlayerGUID );

	return NDTrue;
}


