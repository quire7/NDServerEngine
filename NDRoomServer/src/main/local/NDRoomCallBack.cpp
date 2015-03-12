#include "main/local/NDRoomCallBack.h"


#include "main/local/NDRoomServer.h"


NDRoomCallBack::NDRoomCallBack(void)
{
	NDRegisterCallBackMACRO( sNDRoomServer.dataProcess(), CMDP_PING, this )
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

NDBool NDRoomCallBack::disconnectNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{	
	NDRemoteServerInfo* pRemoteServerInfo = sNDRoomServer.gatewayServerManager()->getRemoteServerInfoBySessionID( protocolHeader.m_nSessionID );
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

		sNDRoomServer.gatewayServerManager()->removeRemoteServer( protocolHeader.m_nSessionID );

		return NDTrue;
	}

	char szErrorBuf[BUF_LEN_128] = {0};
	ND_SNPRINTF( szErrorBuf, sizeof(szErrorBuf) - 1, " NDRoomCallBack::disconnectNotifyDispose connect error link. SessionID=%u. ", protocolHeader.m_nSessionID );
	NDLOG_ERROR( szErrorBuf )
	return NDTrue;
}

//NDBool NDWorldCallBack::timerNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
//{
//	//每500毫秒受到此消息
//	return NDTrue;
//}
