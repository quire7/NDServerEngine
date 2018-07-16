#include "main/local/NDDataCallBack.h"

#include "special/NDSpecialProtocol.h"

#include "main/local/NDDataServer.h"


NDDataCallBack::NDDataCallBack(void)
{
	NDRegisterCallBackMACRO( sNDDataServer.dataProcess(), CMDP_PING_Req, this )
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


NDBool NDDataCallBack::pingProtocolDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	return NDServerManager::getSingleton().pingProtocolCommonDispose( protocolHeader.m_nSessionID );
}

NDBool NDDataCallBack::disconnectNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	NDDisconnectNtyProtocol disconnectNty;
	if ( NDFalse == disconnectNty.deserialize( rIStream ) )
	{
		NDLOG_ERROR( " [NDDataCallBack::disconnectNotifyDispose] NDDisconnectNtyProtocol deserialize failed! " )
		return NDFalse;
	}
	//dispose NDGameServer offline;
	if ( NDTrue == NDServerManager::getSingleton().disconnectNotifyCommonDispose( protocolHeader.m_nSessionID, disconnectNty.m_nDisconnectionType, sNDDataServer.gameServerManager() ) )
	{	
		return NDTrue;
	}

	//�ߵ������TM�Ĵ���,�������ֻ��NDGameServer����NDDataServer.(������Ƹı�);
	NDServerManager::getSingleton().disconnectNotifyCommonErrorDispose( protocolHeader.m_nSessionID, disconnectNty.m_nDisconnectionType, "NDDataServer" );

	return NDFalse;
}

//NDBool NDDataCallBack::timerNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
//{
//	//ÿ500�����ܵ�����Ϣ
//	return NDTrue;
//}
