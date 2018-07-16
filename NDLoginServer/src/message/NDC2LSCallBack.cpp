#include "message/NDC2LSCallBack.h"

#include "NDClientToNDLogin/NDC2LS_Protocol.h"

#include "main/local/NDLoginServer.h"


NDC2LSCallBack::NDC2LSCallBack(void)
{
	NDRegisterCallBackMACRO( sNDLoginServer.dataProcess(), CMDP_NDC2LS_Login_Req, this )
	//NDRegisterCallBackMACRO( sNDLoginServer.dataProcess(), CMDP_NDWS2LS_PlayerOffline_Nty, this )
}

NDC2LSCallBack::~NDC2LSCallBack(void)
{
}

NDBool NDC2LSCallBack::process( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	NDBool bRet = NDFalse;

	switch (protocolHeader.m_nProtocolID)
	{
	case CMDP_NDC2LS_Login_Req:
		{
			bRet = c2lsLoginReqDispose( rIStream, protocolHeader );
		}
		break;
	//case CMD_NDCS2L_PlayerOffline_Nty:
	//	{
	//		bRet = c2lPlayerOfflineNtyDispose( rIStream, protocolHeader );
	//	}
		break;
	}

	return bRet;
}

NDBool NDC2LSCallBack::c2lsLoginReqDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	NDC2LS_Login_Req loginReq;
	if ( NDFalse == loginReq.deserialize( rIStream ) )
	{
		NDLOG_ERROR( " [NDC2LSCallBack::c2lsLoginReqDispose] NDC2LS_Login_Req deserialize failed! " )
		return NDFalse;
	}

	NDServerManager::getSingleton().setServerSessionProtocolType( protocolHeader.m_nSessionID, NDSessionProtocolType_C2LS );

	NDC2LS_Login_Res loginRes;
	loginRes.nPacketIndex = loginReq.nPacketIndex;
	NDShareBaseGlobal::safeStrCopy( loginRes.m_szBuf, loginReq.m_szBuf, sizeof(loginRes.m_szBuf), sizeof(loginReq.m_szBuf) );

	return NDServerManager::getSingleton().sendToClient( loginRes, protocolHeader.m_nSessionID );
}

//NDBool NDC2LSCallBack::c2lPlayerOfflineNtyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
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


