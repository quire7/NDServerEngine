#include "main/local/NDGameCallBack.h"

#include "special/NDSpecialProtocol.h"


#include "main/local/NDGameServer.h"


NDGameCallBack::NDGameCallBack(void)
{
	NDRegisterCallBackMACRO( sNDGameServer.dataProcess(), CMDP_PING_Req, this )
	NDRegisterCallBackMACRO( sNDGameServer.dataProcess(), CMDP_PING_Res, this )
	NDRegisterCallBackMACRO( sNDGameServer.dataProcess(), CMDP_DISCONNECT_NOTIFY, this )
	//NDRegisterCallBackMACRO( sNDWorldServer.dataProcess(), CMD_TIMER_NOTIFY, this )
}

NDGameCallBack::~NDGameCallBack(void)
{
}


NDBool NDGameCallBack::pingProtocolDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	return NDServerManager::getSingleton().pingProtocolCommonDispose( protocolHeader.m_nSessionID );
}

NDBool NDGameCallBack::process( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
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

NDBool NDGameCallBack::disconnectNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{	
	NDDisconnectNtyProtocol disconnectNty;
	if ( NDFalse == disconnectNty.deserialize( rIStream ) )
	{
		NDLOG_ERROR( " [NDGameCallBack::disconnectNotifyDispose] NDDisconnectNtyProtocol deserialize failed! " )
		return NDFalse;
	}
	//dispose NDRoomServer offline;
	if ( NDTrue == NDServerManager::getSingleton().disconnectNotifyCommonDispose( protocolHeader.m_nSessionID, disconnectNty.m_nDisconnectionType, sNDGameServer.roomServerManager() ) )
	{
		return NDTrue;
	}

	//dispose NDGatewayServer offline;
	if ( NDTrue == NDServerManager::getSingleton().disconnectNotifyCommonDispose( protocolHeader.m_nSessionID, disconnectNty.m_nDisconnectionType, sNDGameServer.gatewayServerManager() ) )
	{
		return NDTrue;
	}

	//�ߵ������TM�Ĵ���,���������NDRoomServer��NDGatewayServer����NDGameServer.(������Ƹı�);
	NDServerManager::getSingleton().disconnectNotifyCommonErrorDispose( protocolHeader.m_nSessionID, disconnectNty.m_nDisconnectionType, "NDGameServer" );

	return NDFalse;
}

NDBool NDGameCallBack::pingResProtocolDispose(NDIStream& rIStream, NDProtocolHeader& protocolHeader)
{
	NDPingResProtocol pingRes;
	if ( NDFalse == pingRes.deserialize(rIStream) ) 
	{
		NDLOG_ERROR( " [NDGameCallBack::pingResProtocolDispose] NDPingResProtocol deserialize failed!" )
		return NDFalse;
	}

	//pServerInfo is NDWorldServer or NDDataServer;
	return NDServerManager::getSingleton().pingResProtocolCommonDispose( protocolHeader.m_nSessionID );	
}

//NDBool NDWorldCallBack::timerNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
//{
//	//ÿ500�����ܵ�����Ϣ
//	return NDTrue;
//}
