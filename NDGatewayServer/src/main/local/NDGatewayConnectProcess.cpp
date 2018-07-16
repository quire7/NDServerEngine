#include "main/local/NDGatewayConnectProcess.h"

#include "protocol/NDGatewayToNDRoom/NDGWS2RS_Register.h"
#include "protocol/NDGatewayToNDGame/NDGWS2GS_Register.h"


#include "main/local/NDGatewayServer.h"


NDGatewayConnectProcess::NDGatewayConnectProcess(void)
{
}

NDGatewayConnectProcess::~NDGatewayConnectProcess(void)
{
}

void NDGatewayConnectProcess::connected( const NDServerInfo* pServerInfo )
{
	if (NULL == pServerInfo)	return;

	NDLocalServerInfo* pLocalServerInfo = sNDGatewayServer.getLocalServerInfo();

	switch( pServerInfo->getServerType() )
	{
	case GAME_SERVER:
		{
			NDGWS2GS_Register_Req registerReq;
			registerReq.m_nServerID		= (NDUint16)pLocalServerInfo->getServerID();
			registerReq.m_netAddress    = pLocalServerInfo->getNetAddress();

			NDServerManager::getSingleton().sendToServer( registerReq, pServerInfo->getSessionID() );

			NDServerManager::getSingleton().setClientSessionProtocolType( pServerInfo->getSessionID(), NDSessionProtocolType_GS2GTWS );
		}
		break;
	case ROOM_SERVER:
		{
			NDGWS2RS_Register_Req registerReq;
			registerReq.m_nServerID		= (NDUint16)pLocalServerInfo->getServerID();
			registerReq.m_netAddress    = pLocalServerInfo->getNetAddress();

			NDServerManager::getSingleton().sendToServer( registerReq, pServerInfo->getSessionID() );

			NDServerManager::getSingleton().setClientSessionProtocolType( pServerInfo->getSessionID(), NDSessionProtocolType_RS2GTWS );
		}
		break;
	default:
		break;
	}
}

void NDGatewayConnectProcess::error( const NDServerInfo* pServerInfo )
{
	if (NULL == pServerInfo)	return;

	switch( pServerInfo->getServerType() )
	{
	case GAME_SERVER:
		{
		}
		break;
	case ROOM_SERVER:
		{
		}
		break;
	default:
		break;
	}
}

