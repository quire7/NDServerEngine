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

	const NDServerInfo* pWorldServerInfo = NDServerManager::getSingleton().getConnServerInfo( header.m_nSessionID );
	if ( NULL == pWorldServerInfo )
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::rs2wsRegisterResDispose] pWorldServerInfo is NULL!" )
		return NDFalse;
	}

	char szBuf[BUF_LEN_128] = {0};
	if ( eND_SRS_OK == registerRes.m_nErrorCode )
	{
		ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " register %s [%s:%u] return response success. ",	pWorldServerInfo->getServerName(),
																									pWorldServerInfo->getServerIP(),
																									pWorldServerInfo->getServerPort() );
		NDLOG_INFO( szBuf )
	}
	else
	{
		ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " register %s [%s:%u] return response failed. nErrorCode=%u. ",	pWorldServerInfo->getServerName(),
																												pWorldServerInfo->getServerIP(),
																												pWorldServerInfo->getServerPort(),
																												registerRes.m_nErrorCode );
		NDLOG_ERROR( szBuf )
	}

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

	const NDServerInfo* pGameServerInfo = NDServerManager::getSingleton().getConnServerInfo( header.m_nSessionID );
	if ( NULL == pGameServerInfo )
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::rs2gsRegisterResDispose] pGameServerInfo is NULL!" )
		return NDFalse;
	}

	char szBuf[BUF_LEN_128] = {0};
	if ( eND_SRS_OK == registerRes.m_nErrorCode )
	{
		ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " register %s [%s:%u] return response success. ",	pGameServerInfo->getServerName(),
																									pGameServerInfo->getServerIP(),
																									pGameServerInfo->getServerPort() );
		NDLOG_INFO( szBuf )
	}
	else
	{
		ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " register %s [%s:%u] return response failed. nErrorCode=%u. ",	pGameServerInfo->getServerName(),
																												pGameServerInfo->getServerIP(),
																												pGameServerInfo->getServerPort(),
																												registerRes.m_nErrorCode );
		NDLOG_ERROR( szBuf )
	}

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

	//NDServerManager::getSingleton().SetSessionProtocolType( header.m_nSessionID, NDSessionProtocolType_GT2CS );

	const NDSocketAddress& rNetAddress = pGatewayServerInfo->getNetAddress();

	char szBuf[BUF_LEN_128] = {0};
	ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " %s [%s:%u] [GatewayServerID:%u] connected. ",	pGatewayServerInfo->getServerName(),
																							rNetAddress.getIP(),
																							rNetAddress.getPort(),
																							pGatewayServerInfo->getServerID() );
	NDLOG_INFO( szBuf )

	NDGWS2RS_Register_Res	registerRes;
	registerRes.m_nErrorCode = eND_SRS_OK;

	return NDServerManager::getSingleton().sendToClient( registerRes, header.m_nSessionID );
	 
}


