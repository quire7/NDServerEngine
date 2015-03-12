#include "main/local/NDWorldCallBack.h"


#include "main/local/NDWorldServer.h"


NDWorldCallBack::NDWorldCallBack(void)
{
	NDRegisterCallBackMACRO( sNDWorldServer.dataProcess(), CMDP_PING, this )
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

NDBool NDWorldCallBack::disconnectNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	NDRemoteServerInfo* pRemoteServerInfo = sNDWorldServer.gameServerManager()->getRemoteServerInfoBySessionID( protocolHeader.m_nSessionID );
	if ( NULL != pRemoteServerInfo )
	{	//dispose NDGameServer offline;
		//NDRemoteGameServerCommonInfo* pGameServerInfo	= dynamic_cast<NDRemoteGameServerCommonInfo*>(pRemoteServerInfo);
		NDRemoteServerInfo* pGameServerInfo = pRemoteServerInfo;
		const NDSocketAddress& rNetAddress	= pGameServerInfo->getNetAddress();

		char szBuf[BUF_LEN_128] = {0};
		ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " %s [%s:%u] [GameServerID:%u] offline. ",	pGameServerInfo->getServerName(),
																							rNetAddress.getIP(),
																							rNetAddress.getPort(),
																							pGameServerInfo->getServerID());
		NDLOG_ERROR( szBuf )

		sNDWorldServer.gameServerManager()->removeRemoteServer( protocolHeader.m_nSessionID );

		return NDTrue;
	}
	
	pRemoteServerInfo = sNDWorldServer.roomServerManager()->getRemoteServerInfoBySessionID( protocolHeader.m_nSessionID );
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

		sNDWorldServer.roomServerManager()->removeRemoteServer( protocolHeader.m_nSessionID );

		return NDTrue;
	}

	char szErrorBuf[BUF_LEN_128] = {0};
	ND_SNPRINTF( szErrorBuf, sizeof(szErrorBuf) - 1, " NDWorldCallBack::disconnectNotifyDispose connect error link. SessionID=%u. ", protocolHeader.m_nSessionID );
	NDLOG_ERROR( szErrorBuf )

	return NDFalse;
}

//NDBool NDWorldCallBack::timerNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
//{
//	//每500毫秒受到此消息
//	return NDTrue;
//}
