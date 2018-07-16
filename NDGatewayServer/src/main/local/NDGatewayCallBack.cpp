#include "main/local/NDGatewayCallBack.h"

#include "special/NDSpecialProtocol.h"

#include "main/local/NDGatewayServer.h"


NDGatewayCallBack::NDGatewayCallBack(void)
{
	NDRegisterCallBackMACRO( sNDGatewayServer.dataProcess(), CMDP_PING_Req, this )
	NDRegisterCallBackMACRO( sNDGatewayServer.dataProcess(), CMDP_PING_Res, this )
	NDRegisterCallBackMACRO( sNDGatewayServer.dataProcess(), CMDP_DISCONNECT_NOTIFY, this )
	//NDRegisterCallBackMACRO( sNDWorldServer.dataProcess(), CMD_TIMER_NOTIFY, this )
}

NDGatewayCallBack::~NDGatewayCallBack(void)
{
}


NDBool NDGatewayCallBack::pingProtocolDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	return NDServerManager::getSingleton().pingProtocolCommonDispose( protocolHeader.m_nSessionID );
}

NDBool NDGatewayCallBack::process( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	NDBool bRet = NDFalse;

	switch (protocolHeader.m_nProtocolID)
	{
	case CMDP_PING_Req:
		{
			bRet = pingProtocolDispose( rIStream, protocolHeader );
		}
		break;
	case CMDP_PING_Res:
		{
			bRet = pingResProtocolDispose( rIStream, protocolHeader );
		}
		break;
	case CMDP_DISCONNECT_NOTIFY:
		{
			bRet = disconnectNotifyDispose( rIStream, protocolHeader );
		}
		break;
	//case CMD_TIMER_NOTIFY:
	//	{
	//		bRet = timerNotifyDispose( rIStream, protocolHeader );
	//	}
	//	break;
	default:
		{

		}
		break;
	}

	return bRet;
}

NDBool NDGatewayCallBack::disconnectNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{	
	return NDTrue;
}

NDBool NDGatewayCallBack::pingResProtocolDispose(NDIStream& rIStream, NDProtocolHeader& protocolHeader)
{
	NDPingResProtocol pingRes;
	if ( NDFalse == pingRes.deserialize(rIStream) ) 
	{
		NDLOG_ERROR( " [NDGatewayCallBack::pingResProtocolDispose] NDPingResProtocol deserialize failed!" )
		return NDFalse;
	}

	//pServerInfo is NDRoomServer or NDGameServer;
	return NDServerManager::getSingleton().pingResProtocolCommonDispose( protocolHeader.m_nSessionID );	
}

//NDBool NDWorldCallBack::timerNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
//{
//	//每500毫秒受到此消息
//	return NDTrue;
//}
