#include "message/NDWS2LSCallBack.h"

//#include "../../NDServerShare/include/protocol/NDCenterToNDLogin/NDC2L_Register.h"
//#include "../../NDServerShare/include/protocol/NDCenterToNDLogin/NDC2L_MiddleProtocol.h"
//#include "../../NDServerShare/include/protocol/NDLoginToNDAccountDB/NDL2A_MiddleProtocol.h"

//#include "../world/NDWorld.h"

#include "main/local/NDLoginServer.h"


NDWS2LSCallBack::NDWS2LSCallBack(void)
{
	//NDRegisterCallBackMACRO( sNDLoginServer.dataProcess(), CMDP_NDWS2LS_LoadFactor_Nty, this )
	//NDRegisterCallBackMACRO( sNDLoginServer.dataProcess(), CMDP_NDWS2LS_PlayerOffline_Nty, this )
}

NDWS2LSCallBack::~NDWS2LSCallBack(void)
{
}

NDBool NDWS2LSCallBack::Process( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	NDBool bRet = NDFalse;

	//switch (protocolHeader.m_nProtocolID)
	//{
	//case CMDP_NDWS2LS_LoadFactor_Nty:
	//	{
	//		bRet = c2lLoadFactorNtyDispose( rIStream, protocolHeader );
	//	}
	//	break;
	//case CMD_NDCS2L_PlayerOffline_Nty:
	//	{
	//		bRet = c2lPlayerOfflineNtyDispose( rIStream, protocolHeader );
	//	}
	//	break;
	//}

	return bRet;
}

//NDBool NDWS2LSCallBack::c2lLoadFactorNtyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
//{
//	NDCS2L_LoadFactor_Nty loadFactorNty;
//	if ( NDFalse == loadFactorNty.Deserialize( rIStream ) )
//	{
//		NDTOTAL_LOG_ERROR( " [NDC2LCallBack::c2lLoadFactorNtyDispose] NDC2L_LoadFactor_Nty deserialize failed! " )
//		return NDFalse;
//	}
//
//	NDWorld* pWorld =  dynamic_cast<NDWorld*>( sNDLoginServer.worldManager()->getRemoteServerInfo( protocolHeader.m_nSessionID ) );
//	if ( NULL ==  pWorld )
//	{
//		NDTOTAL_LOG_ERROR( " [NDC2LCallBack::c2lLoadFactorNtyDispose] NDWorld find failed! " )
//		return NDFalse;
//	}
//
//	pWorld->setLoadFactor( loadFactorNty.m_stLoadFactor );
//	pWorld->setBestGate( loadFactorNty.m_stBestGate );
//	pWorld->setBestGateSessionID( loadFactorNty.m_nBestGateSessionID );
//
//
//	return NDTrue;
//}
//
//NDBool NDWS2LSCallBack::c2lPlayerOfflineNtyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
//{
//	NDCS2L_PlayerOffline_Nty playerOfflineNty;
//	if ( NDFalse == playerOfflineNty.Deserialize( rIStream ) )
//	{
//		NDTOTAL_LOG_ERROR( " [NDC2LCallBack::c2lPlayerOfflineNtyDispose] NDC2L_PlayerOffline_Nty Deserialize is failed! " )
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


