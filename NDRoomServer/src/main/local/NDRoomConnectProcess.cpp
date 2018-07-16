#include "main/local/NDRoomConnectProcess.h"

#include "protocol/NDRoomToNDWorld/NDRS2WS_Register.h"
#include "protocol/NDRoomToNDGame/NDRS2GS_Register.h"


#include "main/local/NDRoomServer.h"


NDRoomConnectProcess::NDRoomConnectProcess(void)
{
}

NDRoomConnectProcess::~NDRoomConnectProcess(void)
{
}

void NDRoomConnectProcess::connected( const NDServerInfo* pServerInfo )
{
	if (NULL == pServerInfo)	return;

	NDLocalServerInfo* pLocalServerInfo = sNDRoomServer.getLocalServerInfo();

	switch( pServerInfo->getServerType() )
	{
	case WORLD_SERVER:
		{
			NDRS2WS_Register_Req registerReq;
			registerReq.m_nRoomServerID	= (NDUint16)pLocalServerInfo->getServerID();
			registerReq.m_netAddress    = pLocalServerInfo->getNetAddress();

			NDServerManager::getSingleton().sendToServer( registerReq, pServerInfo->getSessionID() );

			NDServerManager::getSingleton().setClientSessionProtocolType( pServerInfo->getSessionID(), NDSessionProtocolType_WS2RS );
		}
		break;
	case GAME_SERVER:
		{
			NDRS2GS_Register_Req registerReq;
			registerReq.m_nRoomServerID	= (NDUint16)pLocalServerInfo->getServerID();
			registerReq.m_netAddress    = pLocalServerInfo->getNetAddress();

			NDServerManager::getSingleton().sendToServer( registerReq, pServerInfo->getSessionID() );

			NDServerManager::getSingleton().setClientSessionProtocolType( pServerInfo->getSessionID(), NDSessionProtocolType_GS2RS );
		}
		break;
	default:
		break;
	}
}

void NDRoomConnectProcess::error( const NDServerInfo* pServerInfo )
{
	if (NULL == pServerInfo)	return;

	switch( pServerInfo->getServerType() )
	{
	case WORLD_SERVER:
		{
		}
		break;
	case GAME_SERVER:
		{
		}
		break;
	default:
		break;
	}
}

