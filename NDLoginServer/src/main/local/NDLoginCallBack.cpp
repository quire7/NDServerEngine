#include "main/local/NDLoginCallBack.h"

#include <sstream>

#include "main/remote/NDWorldServerInfo.h"

#include "main/local/NDLoginServer.h"


NDLoginCallBack::NDLoginCallBack(void)
{
	NDRegisterCallBackMACRO( sNDLoginServer.dataProcess(), CMDP_PING, this )
	NDRegisterCallBackMACRO( sNDLoginServer.dataProcess(), CMDP_DISCONNECT_NOTIFY, this )
	//NDRegisterCallBackMACRO( sNDLoginServer.dataProcess(), CMD_TIMER_NOTIFY, this )
}

NDLoginCallBack::~NDLoginCallBack(void)
{
}

NDBool NDLoginCallBack::Process( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
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


NDBool NDLoginCallBack::pingProtocolDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	return NDServerManager::getSingleton().pingProtocolCommonDispose( protocolHeader.m_nSessionID );
}

NDBool NDLoginCallBack::disconnectNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	NDRemoteServerInfo* pWorldServerInfo = sNDLoginServer.worldManager()->getRemoteServerInfo( protocolHeader.m_nSessionID );
	if ( NULL != pWorldServerInfo )
	{	//dispose NDWorldServer offline;
		NDWorldServerInfo* pWorld			= dynamic_cast<NDWorldServerInfo*>(pWorldServerInfo);
		const NDSocketAddress& rNetAddress	= pWorld->getNetAddress();

		ostringstream& oStr = sNDLoginServer.getostringstream();
		oStr<< " "
			<< pWorld->getServerName()				<< "("
			<< rNetAddress.getIP()					<< ":"
			<< rNetAddress.getPort()				<< ")"
			<< "(WorldName:"
			<< pWorld->getWorldName()				<< ")"
			<< "(WorldID:"
			<< pWorld->getWorldID()					<< ")"
			<< " offline! ";
		string oStrTemp( oStr.str() );
		NDLOG_ERROR( oStrTemp.c_str() )
		oStr.clear();
		oStr.str("");

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
