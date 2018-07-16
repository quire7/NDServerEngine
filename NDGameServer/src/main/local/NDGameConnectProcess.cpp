#include "main/local/NDGameConnectProcess.h"

#include "protocol/NDGameToNDWorld/NDGS2WS_Register.h"
#include "protocol/NDGameToNDData/NDGS2DS_Register.h"


#include "main/local/NDGameServer.h"


NDGameConnectProcess::NDGameConnectProcess(void)
{
}

NDGameConnectProcess::~NDGameConnectProcess(void)
{
}

void NDGameConnectProcess::connected( const NDServerInfo* pServerInfo )
{
	if (NULL == pServerInfo)	return;

	NDLocalServerInfo* pLocalServerInfo = sNDGameServer.getLocalServerInfo();
	if ( NULL == pLocalServerInfo )
	{
		NDLOG_ERROR( " NDGameConnectProcess::connected NDLocalServerInfo* pLocalServerInfo is NULL. " )
		return;
	}

	switch( pServerInfo->getServerType() )
	{
	case WORLD_SERVER:
		{
			NDGS2WS_Register_Req registerReq;
			registerReq.m_nMapID		= (NDUint16)pLocalServerInfo->getServerID();
			registerReq.m_netAddress    = pLocalServerInfo->getNetAddress();

			NDServerManager::getSingleton().sendToServer( registerReq, pServerInfo->getSessionID() );

			NDServerManager::getSingleton().setClientSessionProtocolType( pServerInfo->getSessionID(), NDSessionProtocolType_WS2GS );
		}
		break;
	case DB_SERVER:
		{
			NDGS2DS_Register_Req registerReq;
			registerReq.m_nMapID		= (NDUint16)pLocalServerInfo->getServerID();
			registerReq.m_netAddress	= pLocalServerInfo->getNetAddress();

			NDServerManager::getSingleton().sendToServer( registerReq, pServerInfo->getSessionID() );

			NDServerManager::getSingleton().setClientSessionProtocolType( pServerInfo->getSessionID(), NDSessionProtocolType_DS2GS );
		}	
		break;
	default:
		break;
	}
}

void NDGameConnectProcess::error( const NDServerInfo* pServerInfo )
{
	if (NULL == pServerInfo)	return;

	switch( pServerInfo->getServerType() )
	{
	case WORLD_SERVER:
		{
		}
		break;
	case DB_SERVER:
		{
		}
		break;
	default:
		break;
	}
}

