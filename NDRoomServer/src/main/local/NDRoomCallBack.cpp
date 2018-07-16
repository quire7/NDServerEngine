#include "main/local/NDRoomCallBack.h"

#include "special/NDSpecialProtocol.h"

#include "main/local/NDRoomServer.h"


NDRoomCallBack::NDRoomCallBack(void)
{
	NDRegisterCallBackMACRO( sNDRoomServer.dataProcess(), CMDP_PING_Req, this )
	NDRegisterCallBackMACRO( sNDRoomServer.dataProcess(), CMDP_PING_Res, this )
	NDRegisterCallBackMACRO( sNDRoomServer.dataProcess(), CMDP_DISCONNECT_NOTIFY, this )
	//NDRegisterCallBackMACRO( sNDWorldServer.dataProcess(), CMD_TIMER_NOTIFY, this )
}

NDRoomCallBack::~NDRoomCallBack(void)
{
}


NDBool NDRoomCallBack::pingProtocolDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	return NDServerManager::getSingleton().pingProtocolCommonDispose( protocolHeader.m_nSessionID );
}

NDBool NDRoomCallBack::process( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
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

NDBool NDRoomCallBack::disconnectNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{	
	NDDisconnectNtyProtocol disconnectNty;
	if ( NDFalse == disconnectNty.deserialize( rIStream ) )
	{
		NDLOG_ERROR( " [NDRoomCallBack::disconnectNotifyDispose] NDDisconnectNtyProtocol deserialize failed! " )
		return NDFalse;
	}

	//dispose NDGatewayServer offline;
	if ( NDTrue == NDServerManager::getSingleton().disconnectNotifyCommonDispose( protocolHeader.m_nSessionID, disconnectNty.m_nDisconnectionType, sNDRoomServer.gatewayServerManager() ) )
	{
		return NDTrue;
	}

	//走到这里就TM的错了,根据设计只有NDGatewayServer连接NDRoomServer.(除非设计改变);
	NDServerManager::getSingleton().disconnectNotifyCommonErrorDispose( protocolHeader.m_nSessionID, disconnectNty.m_nDisconnectionType, "NDRoomServer" );

	return NDFalse;
}

NDBool NDRoomCallBack::pingResProtocolDispose(NDIStream& rIStream, NDProtocolHeader& protocolHeader)
{
	NDPingResProtocol pingRes;
	if ( NDFalse == pingRes.deserialize(rIStream) ) 
	{
		NDLOG_ERROR( " [NDRoomCallBack::pingResProtocolDispose] NDPingResProtocol deserialize failed!" )
		return NDFalse;
	}

	//pServerInfo is NDWorldServer or NDGameServer;
	return NDServerManager::getSingleton().pingResProtocolCommonDispose( protocolHeader.m_nSessionID );
}

//NDBool NDWorldCallBack::timerNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
//{
//	//每500毫秒受到此消息
//	return NDTrue;
//}
