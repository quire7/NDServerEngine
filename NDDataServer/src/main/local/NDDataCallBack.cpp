#include "main/local/NDDataCallBack.h"



#include "main/local/NDDataServer.h"


NDDataCallBack::NDDataCallBack(void)
{
	NDRegisterCallBackMACRO( sNDDataServer.dataProcess(), CMDP_PING, this )
	NDRegisterCallBackMACRO( sNDDataServer.dataProcess(), CMDP_DISCONNECT_NOTIFY, this )
	//NDRegisterCallBackMACRO( sNDLoginServer.dataProcess(), CMD_TIMER_NOTIFY, this )
}

NDDataCallBack::~NDDataCallBack(void)
{
}

NDBool NDDataCallBack::process( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
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


NDBool NDDataCallBack::pingProtocolDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	return NDServerManager::getSingleton().pingProtocolCommonDispose( protocolHeader.m_nSessionID );
}

NDBool NDDataCallBack::disconnectNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	NDRemoteServerInfo* pRemoteServerInfo = sNDDataServer.gameServerManager()->getRemoteServerInfoBySessionID( protocolHeader.m_nSessionID );
	if ( NULL != pRemoteServerInfo )
	{	//dispose NDGameServer offline;
		//NDRemoteGameServerCommonInfo* pGameServerInfo	= dynamic_cast<NDRemoteGameServerCommonInfo*>(pRemoteServerInfo);
		NDRemoteServerInfo* pGameServerInfo = pRemoteServerInfo;
		const NDSocketAddress& rNetAddress	= pGameServerInfo->getNetAddress();

		char szErrorBuf[BUF_LEN_128] = {0};
		ND_SNPRINTF( szErrorBuf, sizeof(szErrorBuf) - 1, " %s [%s:%u] [GameServerID:%u] offline. ", pGameServerInfo->getServerName(),
																									rNetAddress.getIP(),
																									rNetAddress.getPort(),
																									pGameServerInfo->getServerID() );

		NDLOG_ERROR( szErrorBuf )

		sNDDataServer.gameServerManager()->removeRemoteServer( protocolHeader.m_nSessionID );
	}
	else
	{
		//走到这里就TM的错了,根据设计只有NDGameServer连接NDDataServer.(除非设计改变);
		char szErrorBuf[BUF_LEN_64] = {0};
		ND_SNPRINTF( szErrorBuf, sizeof(szErrorBuf) - 1, " NDDataCallBack::disconnectNotifyDispose error type sessionID=%u. ", protocolHeader.m_nSessionID );
		NDLOG_ERROR( szErrorBuf )

		return NDFalse;
	}
	
	return NDTrue;
}

//NDBool NDDataCallBack::timerNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
//{
//	//每500毫秒受到此消息
//	return NDTrue;
//}
