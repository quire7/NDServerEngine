#include "message/NDS2SRegisterCallBack.h"


#include "protocol/NDRoomToNDWorld/NDRS2WS_Register.h"
#include "protocol/NDRoomToNDGame/NDRS2GS_Register.h"
#include "protocol/NDGatewayToNDRoom/NDGWS2RS_Register.h"


#include "main/local/NDRoomServer.h"



NDS2SRegisterCallBack::NDS2SRegisterCallBack(void)
{
	NDRegisterCallBackMACRO( sNDRoomServer.dataProcess(), CMDP_NDRS2WS_Register_Res, this )
	NDRegisterCallBackMACRO( sNDRoomServer.dataProcess(), CMDP_NDRS2GS_Register_Res, this )
	NDRegisterCallBackMACRO( sNDRoomServer.dataProcess(), CMDP_NDGWS2RS_Register_Req, this )
}

NDS2SRegisterCallBack::~NDS2SRegisterCallBack(void)
{
}

NDBool NDS2SRegisterCallBack::process( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	NDBool	bRet = NDFalse;

	switch (protocolHeader.m_nProtocolID)
	{
	case CMDP_NDRS2WS_Register_Res:
		{
			bRet = rs2wsRegisterResDispose( rIStream, protocolHeader );
		}
		break;
	case CMDP_NDRS2GS_Register_Res:
		{
			bRet = rs2gsRegisterResDispose( rIStream, protocolHeader );
		}
		break;
	case CMDP_NDGWS2RS_Register_Req:
		{
			bRet = gws2rsRegisterReqDispose( rIStream, protocolHeader );
		}
		break;
	}

	return bRet;
}



NDBool NDS2SRegisterCallBack::rs2wsRegisterResDispose( NDIStream& rIStream, NDProtocolHeader& header )
{
	NDRS2WS_Register_Res registerRes;
	if ( NDFalse == registerRes.deserialize(rIStream) ) 
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::rs2wsRegisterResDispose] NDRS2WS_Register_Res deserialize failed!" )
		return NDFalse;
	}

	//WS回复RS注册消息;
	NDServerManager::getSingleton().registerResCommonDispose( header.m_nSessionID, registerRes.m_nErrorCode );

	return NDTrue;
}


NDBool NDS2SRegisterCallBack::rs2gsRegisterResDispose( NDIStream& rIStream, NDProtocolHeader& header )
{
	NDRS2GS_Register_Res registerRes;
	if ( NDFalse == registerRes.deserialize(rIStream) ) 
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::rs2gsRegisterResDispose] NDRS2GS_Register_Res deserialize failed!" )
		return NDFalse;
	}

	//GS回复RS注册消息;
	NDServerManager::getSingleton().registerResCommonDispose( header.m_nSessionID, registerRes.m_nErrorCode );

	return NDTrue;
}

NDBool NDS2SRegisterCallBack::gws2rsRegisterReqDispose( NDIStream& rIStream, NDProtocolHeader& header )
{
	NDGWS2RS_Register_Req	registerReq;
	if ( NDFalse == registerReq.deserialize(rIStream) )
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::gws2rsRegisterReqDispose] NDGWS2RS_Register_Req deserialize failed! " )
		return NDFalse;
	}

	NDRemoteServerInfo* pGatewayServerInfo = new NDRemoteServerInfo;
	if ( NULL == pGatewayServerInfo )
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::gws2rsRegisterReqDispose] pGatewayServerInfo is NULL! " )
		return NDFalse;
	}

	pGatewayServerInfo->setServerType( GATEWAY_SERVER );
	pGatewayServerInfo->setServerName( "NDGatewayServer" );
	pGatewayServerInfo->setSessionID( header.m_nSessionID );
	pGatewayServerInfo->setLoadFactor( NDLoadFactor(1000) );
	pGatewayServerInfo->setNetAddress( registerReq.m_netAddress );
	pGatewayServerInfo->setServerID( registerReq.m_nServerID );

	NDRemoteServerManager* pGatewayServerMgr = sNDRoomServer.gatewayServerManager();
	if ( NULL == pGatewayServerMgr )
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::gws2rsRegisterReqDispose] pGatewayServerMgr is NULL! " )
		return NDFalse;
	}
 
	pGatewayServerMgr->addRemoteServer( pGatewayServerInfo );

	NDServerManager::getSingleton().setServerSessionProtocolType( header.m_nSessionID, NDSessionProtocolType_GTWS2RS );

	const NDSocketAddress& rNetAddress = pGatewayServerInfo->getNetAddress();

	NDLOG_INFO( " %s [%s:%u] [GatewayServerID:%u] connected. ", pGatewayServerInfo->getServerName(),
																rNetAddress.getIP(),
																rNetAddress.getPort(),
																pGatewayServerInfo->getServerID() );

	NDGWS2RS_Register_Res	registerRes;
	registerRes.m_nErrorCode = eND_SRS_OK;

	return NDServerManager::getSingleton().sendToClient( registerRes, header.m_nSessionID );
	 
}


