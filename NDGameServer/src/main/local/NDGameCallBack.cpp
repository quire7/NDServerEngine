#include "main/local/NDGameCallBack.h"


#include "main/local/NDGameServer.h"


NDGameCallBack::NDGameCallBack(void)
{
	NDRegisterCallBackMACRO( sNDGameServer.dataProcess(), CMDP_PING, this )
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
	case CMDP_PING:
		{
			bRet = pingProtocolDispose( rIStream, protocolHeader );
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
	NDRemoteServerInfo* pRemoteServerInfo = sNDGameServer.roomServerManager()->getRemoteServerInfoBySessionID( protocolHeader.m_nSessionID );
	if ( NULL != pRemoteServerInfo )
	{	//dispose NDRoomServer offline;
		//NDRemoteRoomServerCommonInfo* pRoomServerInfo	= dynamic_cast<NDRemoteRoomServerCommonInfo*>(pRemoteServerInfo);
		NDRemoteServerInfo* pRoomServerInfo = pRemoteServerInfo;
		const NDSocketAddress& rNetAddress	= pRoomServerInfo->getNetAddress();

		char szBuf[BUF_LEN_128] = {0};
		ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " %s [%s:%u] [RoomServerID:%u] offline. ",	pRoomServerInfo->getServerName(),
																							rNetAddress.getIP(),
																							rNetAddress.getPort(),
																							pRoomServerInfo->getServerID() );
		NDLOG_ERROR( szBuf )

		sNDGameServer.roomServerManager()->removeRemoteServer( protocolHeader.m_nSessionID );

		return NDTrue;
	}

	pRemoteServerInfo = sNDGameServer.gatewayServerManager()->getRemoteServerInfoBySessionID( protocolHeader.m_nSessionID );
	if ( NULL != pRemoteServerInfo )
	{	//dispose NDGatewayServer offline;
		//NDRemoteGatewayServerCommonInfo* pGatewayServerInfo	= dynamic_cast<NDRemoteGatewayServerCommonInfo*>(pRemoteServerInfo);
		NDRemoteServerInfo* pGatewayServerInfo = pRemoteServerInfo;
		const NDSocketAddress& rNetAddress	= pGatewayServerInfo->getNetAddress();

		char szBuf[BUF_LEN_128] = {0};
		ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " %s [%s:%u] [GatewayServerID:%u] offline. ",	pGatewayServerInfo->getServerName(),
																								rNetAddress.getIP(),
																								rNetAddress.getPort(),
																								pGatewayServerInfo->getServerID() );
		NDLOG_ERROR( szBuf )

		sNDGameServer.gatewayServerManager()->removeRemoteServer( protocolHeader.m_nSessionID );

		return NDTrue;
	}

	char szErrorBuf[BUF_LEN_128] = {0};
	ND_SNPRINTF( szErrorBuf, sizeof(szErrorBuf) - 1, " NDGameCallBack::disconnectNotifyDispose connect error link. SessionID=%u. ", protocolHeader.m_nSessionID );
	NDLOG_ERROR( szErrorBuf )

	return NDFalse;
}

//NDBool NDWorldCallBack::timerNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
//{
//	//每500毫秒受到此消息
//	return NDTrue;
//}
