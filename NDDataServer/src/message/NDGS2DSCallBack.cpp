#include "message/NDGS2DSCallBack.h"

#include "protocol/NDGameToNDData/NDGS2DS_PlayerLogin.h"

#include "main/local/NDDataServer.h"


NDGS2DSCallBack::NDGS2DSCallBack(void)
{
	NDRegisterCallBackMACRO( sNDDataServer.dataProcess(), CMDP_NDGS2DS_PlayerLogin_Req, this )
}

NDGS2DSCallBack::~NDGS2DSCallBack(void)
{
}

NDBool NDGS2DSCallBack::process( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	NDBool bRet = NDFalse;

	switch (protocolHeader.m_nProtocolID)
	{
	case CMDP_NDGS2DS_PlayerLogin_Req:
		{
			bRet = gs2dsPlayerLoginReqDispose( rIStream, protocolHeader );
		}
		break;
	//case CMD_NDCS2L_PlayerOffline_Nty:
	//	{
	//		bRet = c2lPlayerOfflineNtyDispose( rIStream, protocolHeader );
	//	}
	//	break;
	}

	return bRet;
}

NDBool NDGS2DSCallBack::gs2dsPlayerLoginReqDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	NDGS2DS_PlayerLogin_Req request;
	if ( NDFalse == request.deserialize( rIStream ) )
	{
		NDLOG_ERROR( " [NDGS2DSCallBack::gs2dsPlayerLoginReqDispose] NDGS2DS_PlayerLogin_Req deserialize failed!" )
		return NDFalse;
	}

	NDDSDataManager* pNDDSDataManager = sNDDataServer.dataManager();
	if ( NULL == pNDDSDataManager )
	{
		NDLOG_ERROR( " [NDGS2DSCallBack::gs2dsPlayerLoginReqDispose] pNDDSDataManager is NULL." )
		return NDFalse;
	}
	//等待队列已经有这个玩家;
	if ( NDTrue == pNDDSDataManager->isInWaitingQueue( request.m_nPlayerGUID ) )
	{
		return NDTrue;
	}

	NDDSPlayerManager* pNDDSPlayerManager = sNDDataServer.playerManager();
	if ( NULL == pNDDSPlayerManager )
	{
		NDLOG_ERROR( " [NDGS2DSCallBack::gs2dsPlayerLoginReqDispose] pNDDSPlayerManager is NULL." )
		return NDFalse;
	}
	NDDSPlayer* pPlayer = pNDDSPlayerManager->getPlayerByGUID( request.m_nPlayerGUID );
	if ( NULL == pPlayer )
	{	//玩家数据不在线，则插入查询等待队列，以便让select线程去查找;
		NDDSWaitingAccount waitingAccount;
		waitingAccount.nPlayerGUID		= request.m_nPlayerGUID;
		waitingAccount.nGameServerID	= request.m_nGameServerID;
		
		pNDDSDataManager->addWaitingQueue( waitingAccount );

		return NDTrue;
	}

	pPlayer->setGSServerID( request.m_nGameServerID );

	NDGS2DS_PlayerLogin_Res	response;
	response.m_nPlayerGUID	= request.m_nPlayerGUID;
	response.m_nErrorCode	= eND_SRE_LOGIC_ERROR;
	//发送玩家数据到GS;	
	if ( NDTrue == pPlayer->sendAllPlayerDataToGS() )
	{
		response.m_nErrorCode = eND_SRS_OK;
	}
	
	return NDServerManager::getSingleton().sendToClient( response, protocolHeader.m_nSessionID );
}

//NDBool NDWS2LSCallBack::c2lPlayerOfflineNtyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
//{
//	NDCS2L_PlayerOffline_Nty playerOfflineNty;
//	if ( NDFalse == playerOfflineNty.deserialize( rIStream ) )
//	{
//		NDTOTAL_LOG_ERROR( " [NDC2LCallBack::c2lPlayerOfflineNtyDispose] NDC2L_PlayerOffline_Nty deserialize is failed! " )
//		return NDFalse;
//	}
//
//
//	//send player off line notify to NDAccountDBServer
//	NDL2A_PlayerOffline_Nty l2aPlayerOfflineNty;
//	l2aPlayerOfflineNty.m_nAccountID = playerOfflineNty.m_nAccountID;
//
//	return NDServerManager::getSingleton().sendToServer( l2aPlayerOfflineNty, ACCOUNTDB_SERVER );
//
//}


