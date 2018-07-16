#include "main/local/NDLoginCallBack.h"

#include <sstream>

#include "main/remote/NDRemoteWorldServerInfo.h"

#include "main/local/NDLoginServer.h"


NDLoginCallBack::NDLoginCallBack(void)
{
	NDRegisterCallBackMACRO( sNDLoginServer.dataProcess(), CMDP_PING_Req, this )
	NDRegisterCallBackMACRO( sNDLoginServer.dataProcess(), CMDP_DISCONNECT_NOTIFY, this )
	//NDRegisterCallBackMACRO( sNDLoginServer.dataProcess(), CMD_TIMER_NOTIFY, this )
}

NDLoginCallBack::~NDLoginCallBack(void)
{
}

NDBool NDLoginCallBack::process( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	NDBool bRet = NDFalse;

	switch (protocolHeader.m_nProtocolID)
	{
	case CMDP_PING_Req:
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


NDBool NDLoginCallBack::pingProtocolDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	return NDServerManager::getSingleton().pingProtocolCommonDispose( protocolHeader.m_nSessionID );
}

NDBool NDLoginCallBack::disconnectNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	NDRemoteServerInfo* pRemoteServerInfo = sNDLoginServer.worldManager()->getRemoteServerInfoBySessionID( protocolHeader.m_nSessionID );
	if ( NULL != pRemoteServerInfo )
	{	//dispose NDWorldServer offline;
		NDRemoteWorldServerInfo* pWorldServerInfo	= dynamic_cast<NDRemoteWorldServerInfo*>(pRemoteServerInfo);
		const NDSocketAddress& rNetAddress	= pWorldServerInfo->getNetAddress();

		NDLOG_ERROR(" %s [%s:%d] [WorldName:%s] [WorldID:%d] offline!", 
					pWorldServerInfo->getServerName(), 
					rNetAddress.getIP(), 
					rNetAddress.getPort(), 
					pWorldServerInfo->getWorldName(),
					pWorldServerInfo->getWorldID())


		sNDLoginServer.worldManager()->removeRemoteServer( protocolHeader.m_nSessionID );

		return NDTrue;
	}

	// client player dispose;
	
	return NDTrue;
}

//NDBool NDLoginCallBack::timerNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
//{
//	//每500毫秒受到此消息
//	return NDTrue;
//}
