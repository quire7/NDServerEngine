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

	//GS回复GTWS注册消息;
	NDServerManager::getSingleton().registerResCommonDispose( header.m_nSessionID, registerRes.m_nErrorCode );

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

	//RS回复GTWS注册消息;
	NDServerManager::getSingleton().registerResCommonDispose( header.m_nSessionID, registerRes.m_nErrorCode );

	return NDTrue;
}

