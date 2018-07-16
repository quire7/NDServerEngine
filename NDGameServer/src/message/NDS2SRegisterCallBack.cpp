#include "message/NDS2SRegisterCallBack.h"


#include "protocol/NDGameToNDData/NDGS2DS_Register.h"
#include "protocol/NDGameToNDWorld/NDGS2WS_Register.h"
#include "protocol/NDRoomToNDGame/NDRS2GS_Register.h"
#include "protocol/NDGatewayToNDGame/NDGWS2GS_Register.h"


#include "main/local/NDGameServer.h"



NDS2SRegisterCallBack::NDS2SRegisterCallBack(void)
{
	NDRegisterCallBackMACRO( sNDGameServer.dataProcess(), CMDP_NDGS2DS_Register_Res, this )
	NDRegisterCallBackMACRO( sNDGameServer.dataProcess(), CMDP_NDGS2WS_Register_Res, this )
	NDRegisterCallBackMACRO( sNDGameServer.dataProcess(), CMDP_NDRS2GS_Register_Req, this )
	NDRegisterCallBackMACRO( sNDGameServer.dataProcess(), CMDP_NDGWS2GS_Register_Req, this )
}

NDS2SRegisterCallBack::~NDS2SRegisterCallBack(void)
{
}

NDBool NDS2SRegisterCallBack::process( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	NDBool	bRet = NDFalse;

	switch (protocolHeader.m_nProtocolID)
	{
	case CMDP_NDGS2DS_Register_Res:
		{
			bRet = gs2dsRegisterResDispose( rIStream, protocolHeader );
		}
		break;
	case CMDP_NDGS2WS_Register_Res:
		{
			bRet = gs2wsRegisterResDispose( rIStream, protocolHeader );
		}
		break;
	case CMDP_NDRS2GS_Register_Req:
		{
			bRet = rs2gsRegisterReqDispose( rIStream, protocolHeader );
		}
		break;
	case CMDP_NDGWS2GS_Register_Req:
		{
			bRet = gws2gsRegisterReqDispose( rIStream, protocolHeader );
		}
		break;
	}

	return bRet;
}


NDBool NDS2SRegisterCallBack::gs2dsRegisterResDispose( NDIStream& rIStream, NDProtocolHeader& header )
{
	NDGS2DS_Register_Res registerRes;
	if ( NDFalse == registerRes.deserialize(rIStream) ) 
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::gs2dsRegisterResDispose] NDGS2DS_Register_Res deserialize failed! " )
		return NDFalse;
	}
		
	//DS回复GS注册消息;
	NDServerManager::getSingleton().registerResCommonDispose( header.m_nSessionID, registerRes.m_nErrorCode );

	return NDTrue;
}


NDBool NDS2SRegisterCallBack::gs2wsRegisterResDispose( NDIStream& rIStream, NDProtocolHeader& header )
{
	NDGS2WS_Register_Res registerRes;
	if ( NDFalse == registerRes.deserialize(rIStream) ) 
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::gs2wsRegisterResDispose] NDGS2WS_Register_Res deserialize failed!" )
		return NDFalse;
	}
	//WS回复GS注册消息;
	NDServerManager::getSingleton().registerResCommonDispose( header.m_nSessionID, registerRes.m_nErrorCode );

	return NDTrue;
}

NDBool NDS2SRegisterCallBack::rs2gsRegisterReqDispose( NDIStream& rIStream, NDProtocolHeader& header )
{
	NDRS2GS_Register_Req	registerReq;
	if ( NDFalse == registerReq.deserialize(rIStream) )
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::rs2gsRegisterReqDispose] NDRS2GS_Register_Req deserialize failed! " )
		return NDFalse;
	}

	NDRemoteServerInfo* pRoomServerInfo = new NDRemoteServerInfo;
	if ( NULL == pRoomServerInfo )
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::rs2gsRegisterReqDispose] pRoomServerInfo is NULL! " )
		return NDFalse;
	}

	pRoomServerInfo->setServerType( ROOM_SERVER );
	pRoomServerInfo->setServerName( "NDRoomServer" );
	pRoomServerInfo->setSessionID( header.m_nSessionID );
	//pRoomServerInfo->setLoadFactor( NDLoadFactor(1000) );
	pRoomServerInfo->setNetAddress( registerReq.m_netAddress );
	pRoomServerInfo->setServerID( registerReq.m_nRoomServerID );

	NDRemoteServerManager* pRoomServerMgr = sNDGameServer.roomServerManager();
	if ( NULL == pRoomServerMgr )
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::rs2gsRegisterReqDispose] pRoomServerMgr is NULL! " )
		return NDFalse;
	}

	pRoomServerMgr->addRemoteServer( pRoomServerInfo );

	NDServerManager::getSingleton().setServerSessionProtocolType( header.m_nSessionID, NDSessionProtocolType_RS2GS );

	const NDSocketAddress& rNetAddress = pRoomServerInfo->getNetAddress();

	NDLOG_INFO( " %s [%s:%u] [RoomServerID:%u] connected. ", pRoomServerInfo->getServerName(),
															rNetAddress.getIP(),
															rNetAddress.getPort(),
															pRoomServerInfo->getServerID() );


	NDRS2GS_Register_Res	registerRes;
	registerRes.m_nErrorCode = eND_SRS_OK;

	return NDServerManager::getSingleton().sendToClient( registerRes, header.m_nSessionID );
}

NDBool NDS2SRegisterCallBack::gws2gsRegisterReqDispose( NDIStream& rIStream, NDProtocolHeader& header )
{
	NDGWS2GS_Register_Req	registerReq;
	if ( NDFalse == registerReq.deserialize(rIStream) )
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::gws2gsRegisterReqDispose] NDGWS2GS_Register_Req deserialize failed! " )
		return NDFalse;
	}

	NDRemoteServerInfo* pGatewayServerInfo =  new NDRemoteServerInfo;
	if ( NULL == pGatewayServerInfo )
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::NDGWS2GS_Register_Req] new NDMapServerInfo failed! " )
		return NDFalse;
	}

	pGatewayServerInfo->setServerType( GATEWAY_SERVER );
	pGatewayServerInfo->setServerName( "NDGatewayServer" );
	//pMapServerInfo->setLoadFactor(NDLoadFactor(2000));
	pGatewayServerInfo->setSessionID( header.m_nSessionID );
	pGatewayServerInfo->setNetAddress( registerReq.m_netAddress );
	pGatewayServerInfo->setServerID( registerReq.m_nServerID );

	sNDGameServer.gatewayServerManager()->addRemoteServer( pGatewayServerInfo );


	NDServerManager::getSingleton().setServerSessionProtocolType( header.m_nSessionID, NDSessionProtocolType_GTWS2GS );

	const NDSocketAddress& rNetAddress = pGatewayServerInfo->getNetAddress();

	
	NDLOG_INFO( " %s [%s:%u] [GatewayServerID:%u] connected. ", pGatewayServerInfo->getServerName(),
																rNetAddress.getIP(),
																rNetAddress.getPort(),
																pGatewayServerInfo->getServerID() );


	NDGWS2GS_Register_Res	m2cRegisterRes;
	m2cRegisterRes.m_nErrorCode	= eND_SRS_OK;

	return NDServerManager::getSingleton().sendToClient(  m2cRegisterRes, header.m_nSessionID );
}

