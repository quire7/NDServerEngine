#include "message/NDS2SRegisterCallBack.h"


#include "protocol/NDWorldToNDLogin/NDWS2LS_Register.h"
#include "protocol/NDGameToNDWorld/NDGS2WS_Register.h"
#include "protocol/NDRoomToNDWorld/NDRS2WS_Register.h"


#include "main/local/NDWorldServer.h"



NDS2SRegisterCallBack::NDS2SRegisterCallBack(void)
{
	//NDRegisterCallBackMACRO( sNDWorldServer.dataProcess(), CMD_NDCS2GDB_Register_Res, this )
	NDRegisterCallBackMACRO( sNDWorldServer.dataProcess(), CMDP_NDLS2WS_Register_Res, this )
	NDRegisterCallBackMACRO( sNDWorldServer.dataProcess(), CMDP_NDRS2WS_Register_Req, this )
	NDRegisterCallBackMACRO( sNDWorldServer.dataProcess(), CMDP_NDGS2WS_Register_Req, this )
}

NDS2SRegisterCallBack::~NDS2SRegisterCallBack(void)
{
}

NDBool NDS2SRegisterCallBack::process( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	NDBool	bRet = NDFalse;

	switch (protocolHeader.m_nProtocolID)
	{
	//case CMD_NDCS2GDB_Register_Res:
	//	{
	//		bRet = c2gdbRegisterResDispose( rIStream, protocolHeader );
	//	}
	//	break;
	case CMDP_NDLS2WS_Register_Res:
		{
			bRet = ls2wsRegisterResDispose( rIStream, protocolHeader );
		}
		break;
	case CMDP_NDRS2WS_Register_Req:
		{
			bRet = rs2wsRegisterReqDispose( rIStream, protocolHeader );
		}
		break;
	case CMDP_NDGS2WS_Register_Req:
		{
			bRet = gs2wsRegisterReqDispose( rIStream, protocolHeader );
		}
		break;
	}

	return bRet;
}

//
//NDBool NDS2SRegisterCallBack::c2gdbRegisterResDispose( NDIStream& rIStream, NDProtocolHeader& header )
//{
//	NDCS2GDB_Register_Res registerRes;
//	if ( NDFalse == registerRes.deserialize(rIStream) ) 
//	{
//		NDLOG_ERROR( " [NDS2SRegisterCallBack::c2gdbRegisterResDispose] NDC2GDB_Register_Res deserialize failed! " )
//		return NDFalse;
//	}
//		
//	const NDServerInfo* pGameDBServerInfo = NDServerManager::getSingleton().getConnServerInfo( header.m_nSessionID );
//	if ( NULL == pGameDBServerInfo )
//	{
//		NDLOG_ERROR( " [NDS2SRegisterCallBack::c2gdbRegisterResDispose] pGameDBServerInfo is NULL! " )
//		return NDFalse;
//	}
//
//	if ( eND_SRS_OK == registerRes.m_nErrorCode )
//	{
//		std::ostringstream oStr;
//		oStr		<< " register "
//					<< pGameDBServerInfo->getServerName()	<< "("
//					<< pGameDBServerInfo->getServerIP()		<< ":"
//					<< pGameDBServerInfo->getServerPort()	<< ")"
//					<< " return response success! ";
//		NDLOG_INFO( oStr.str().c_str() )
//		oStr.clear();
//	}
//	else
//	{
//		std::ostringstream oStr;
//		oStr		<< " register "
//					<< pGameDBServerInfo->getServerName()	<< "("
//					<< pGameDBServerInfo->getServerIP()		<< ":"
//					<< pGameDBServerInfo->getServerPort()	<< ")"
//					<< " return response failed! ";			
//		NDLOG_ERROR( oStr.str().c_str() );
//		oStr.clear();
//	}
//
//	return NDTrue;
//}


NDBool NDS2SRegisterCallBack::ls2wsRegisterResDispose( NDIStream& rIStream, NDProtocolHeader& header )
{
	NDLS2WS_Register_Res registerRes;
	if ( NDFalse == registerRes.deserialize(rIStream) ) 
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::ls2wsRegisterResDispose] NDLS2WS_Register_Res deserialize failed!" )
		return NDFalse;
	}

	//LS»Ø¸´WS×¢²áÏûÏ¢;
	NDServerManager::getSingleton().registerResCommonDispose( header.m_nSessionID, registerRes.m_uiErrCode );

	return NDTrue;
}

NDBool NDS2SRegisterCallBack::rs2wsRegisterReqDispose( NDIStream& rIStream, NDProtocolHeader& header )
{
	NDRS2WS_Register_Req	registerReq;
	if ( NDFalse == registerReq.deserialize(rIStream) )
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::rs2wsRegisterReqDispose] NDRS2WS_Register_Req deserialize failed! " )
		return NDFalse;
	}

	NDRemoteServerInfo* pRoomServerInfo = new NDRemoteServerInfo;
	if ( NULL == pRoomServerInfo )
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::rs2wsRegisterReqDispose] pRoomServerInfo is NULL! " )
		return NDFalse;
	}

	pRoomServerInfo->setServerType( ROOM_SERVER );
	pRoomServerInfo->setServerName( "NDRoomServer" );
	pRoomServerInfo->setSessionID( header.m_nSessionID );
	//pGateServerInfo->setLoadFactor( NDLoadFactor(1000) );
	pRoomServerInfo->setNetAddress( registerReq.m_netAddress );
	pRoomServerInfo->setServerID( registerReq.m_nRoomServerID );

	NDRemoteServerManager* pRoomServerMgr = sNDWorldServer.roomServerManager();
	if ( NULL == pRoomServerMgr )
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::rs2wsRegisterReqDispose] pRoomServerMgr is NULL! " )
		return NDFalse;
	}

	pRoomServerMgr->addRemoteServer( pRoomServerInfo );

	NDServerManager::getSingleton().setServerSessionProtocolType( header.m_nSessionID, NDSessionProtocolType_RS2WS );

	const NDSocketAddress& rNetAddress = pRoomServerInfo->getNetAddress();

	NDLOG_INFO( " %s [%s:%u] [RoomServerID:%u] connected. ", pRoomServerInfo->getServerName(),
															rNetAddress.getIP(),
															rNetAddress.getPort(),
															pRoomServerInfo->getServerID() );
	

	NDRS2WS_Register_Res	registerRes;
	registerRes.m_nErrorCode = eND_SRS_OK;

	return NDServerManager::getSingleton().sendToClient( registerRes, header.m_nSessionID );
}

NDBool NDS2SRegisterCallBack::gs2wsRegisterReqDispose( NDIStream& rIStream, NDProtocolHeader& header )
{
	NDGS2WS_Register_Req	gs2wsRegisterReq;
	if ( NDFalse == gs2wsRegisterReq.deserialize(rIStream) )
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::gs2wsRegisterReqDispose] NDGS2WS_Register_Req deserialize failed! " )
		return NDFalse;
	}

	NDRemoteServerInfo* pMapServerInfo =  new NDRemoteServerInfo;
	if ( NULL == pMapServerInfo )
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::gs2wsRegisterReqDispose] new NDGameServerInfo failed! " )
		return NDFalse;
	}

	pMapServerInfo->setServerType( GAME_SERVER );
	pMapServerInfo->setServerName( "NDGameServer" );
	pMapServerInfo->setSessionID( header.m_nSessionID );
	pMapServerInfo->setNetAddress( gs2wsRegisterReq.m_netAddress );
	pMapServerInfo->setServerID( gs2wsRegisterReq.m_nMapID );

	sNDWorldServer.gameServerManager()->addRemoteServer( pMapServerInfo );

	NDServerManager::getSingleton().setServerSessionProtocolType( header.m_nSessionID, NDSessionProtocolType_GS2WS );

	const NDSocketAddress& rNetAddress = pMapServerInfo->getNetAddress();

	NDLOG_INFO( " %s [%s:%u] [GameServerID:%u] connected. ", pMapServerInfo->getServerName(),
															rNetAddress.getIP(),
															rNetAddress.getPort(),
															pMapServerInfo->getServerID() );

	NDGS2WS_Register_Res	gs2wsRegisterRes;
	gs2wsRegisterRes.m_nErrorCode	= eND_SRS_OK;

	return NDServerManager::getSingleton().sendToClient(  gs2wsRegisterRes, header.m_nSessionID );
}

