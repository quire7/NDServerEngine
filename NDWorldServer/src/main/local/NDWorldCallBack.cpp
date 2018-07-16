#include "main/local/NDWorldCallBack.h"

#include "special/NDSpecialProtocol.h"

#include "main/local/NDWorldServer.h"


NDWorldCallBack::NDWorldCallBack(void)
{
	NDRegisterCallBackMACRO( sNDWorldServer.dataProcess(), CMDP_PING_Req, this )
	NDRegisterCallBackMACRO( sNDWorldServer.dataProcess(), CMDP_PING_Res, this )
	NDRegisterCallBackMACRO( sNDWorldServer.dataProcess(), CMDP_DISCONNECT_NOTIFY, this )
	//NDRegisterCallBackMACRO( sNDWorldServer.dataProcess(), CMD_TIMER_NOTIFY, this )
}

NDWorldCallBack::~NDWorldCallBack(void)
{
}


NDBool NDWorldCallBack::pingProtocolDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	return NDServerManager::getSingleton().pingProtocolCommonDispose( protocolHeader.m_nSessionID );
}

NDBool NDWorldCallBack::process( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
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

NDBool NDWorldCallBack::disconnectNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	NDDisconnectNtyProtocol disconnectNty;
	if ( NDFalse == disconnectNty.deserialize( rIStream ) )
	{
		NDLOG_ERROR( " [NDGameCallBack::disconnectNotifyDispose] NDDisconnectNtyProtocol deserialize failed! " )
			return NDFalse;
	}
	//dispose NDRoomServer offline;
	if ( NDTrue == NDServerManager::getSingleton().disconnectNotifyCommonDispose( protocolHeader.m_nSessionID, disconnectNty.m_nDisconnectionType, sNDWorldServer.roomServerManager() ) )
	{
		return NDTrue;
	}

	//dispose NDGameServer offline;
	if ( NDTrue == NDServerManager::getSingleton().disconnectNotifyCommonDispose( protocolHeader.m_nSessionID, disconnectNty.m_nDisconnectionType, sNDWorldServer.gameServerManager() ) )
	{
		return NDTrue;
	}

	//走到这里就TM的错了,根据设计有NDRoomServer和NDGameServer连接NDWorldServer.(除非设计改变);
	NDServerManager::getSingleton().disconnectNotifyCommonErrorDispose( protocolHeader.m_nSessionID, disconnectNty.m_nDisconnectionType, "NDWorldServer" );

	return NDFalse;
}

NDBool NDWorldCallBack::pingResProtocolDispose(NDIStream& rIStream, NDProtocolHeader& protocolHeader)
{
	NDPingResProtocol pingRes;
	if ( NDFalse == pingRes.deserialize(rIStream) ) 
	{
		NDLOG_ERROR( " [NDWorldCallBack::pingResProtocolDispose] NDPingResProtocol deserialize failed!" )
		return NDFalse;
	}

	return NDServerManager::getSingleton().pingResProtocolCommonDispose( protocolHeader.m_nSessionID );
}

//NDBool NDWorldCallBack::timerNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
//{
//	//每500毫秒受到此消息
//	return NDTrue;
//}
