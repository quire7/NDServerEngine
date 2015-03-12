#include "message/NDS2SRegisterCallBack.h"


#include "protocol/NDGatewayToNDRoom/NDGWS2RS_Register.h"
#include "protocol/NDGatewayToNDGame/NDGWS2GS_Register.h"



#include "main/local/NDGatewayServer.h"



NDS2SRegisterCallBack::NDS2SRegisterCallBack(void)
{
	NDRegisterCallBackMACRO( sNDGatewayServer.dataProcess(), CMDP_NDGWS2GS_Register_Res, this )
	NDRegisterCallBackMACRO( sNDGatewayServer.dataProcess(), CMDP_NDGWS2RS_Register_Res, this )
}

NDS2SRegisterCallBack::~NDS2SRegisterCallBack(void)
{
}

NDBool NDS2SRegisterCallBack::process( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	NDBool	bRet = NDFalse;

	switch (protocolHeader.m_nProtocolID)
	{
	case CMDP_NDGWS2GS_Register_Res:
		{
			bRet = gws2gsRegisterResDispose( rIStream, protocolHeader );
		}
		break;
	case CMDP_NDGWS2RS_Register_Res:
		{
			bRet = gws2rsRegisterResDispose( rIStream, protocolHeader );
		}
		break;
	}

	return bRet;
}



NDBool NDS2SRegisterCallBack::gws2gsRegisterResDispose( NDIStream& rIStream, NDProtocolHeader& header )
{
	NDGWS2GS_Register_Res registerRes;
	if ( NDFalse == registerRes.deserialize(rIStream) ) 
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::gws2gsRegisterResDispose] NDGWS2GS_Register_Res deserialize failed!" )
		return NDFalse;
	}

	const NDServerInfo* pGameServerInfo = NDServerManager::getSingleton().getConnServerInfo( header.m_nSessionID );
	if ( NULL == pGameServerInfo )
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::gws2gsRegisterResDispose] pWorldServerInfo is NULL!" )
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

NDBool NDS2SRegisterCallBack::gws2rsRegisterResDispose( NDIStream& rIStream, NDProtocolHeader& header )
{
	NDGWS2RS_Register_Res registerRes;
	if ( NDFalse == registerRes.deserialize(rIStream) ) 
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::gws2rsRegisterResDispose] NDGWS2RS_Register_Res deserialize failed!" )
		return NDFalse;
	}

	const NDServerInfo* pRoomServerInfo = NDServerManager::getSingleton().getConnServerInfo( header.m_nSessionID );
	if ( NULL == pRoomServerInfo )
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::gws2rsRegisterResDispose] pRoomServerInfo is NULL!" )
		return NDFalse;
	}

	char szBuf[BUF_LEN_128] = {0};
	if ( eND_SRS_OK == registerRes.m_nErrorCode )
	{
		ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " register %s [%s:%u] return response success. ",	pRoomServerInfo->getServerName(),
																									pRoomServerInfo->getServerIP(),
																									pRoomServerInfo->getServerPort() );
		NDLOG_INFO( szBuf )
	}
	else
	{
		ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " register %s [%s:%u] return response failed. nErrorCode=%u. ",	pRoomServerInfo->getServerName(),
																												pRoomServerInfo->getServerIP(),
																												pRoomServerInfo->getServerPort(),
																												registerRes.m_nErrorCode );
		NDLOG_ERROR( szBuf )
	}

	return NDTrue;
}

