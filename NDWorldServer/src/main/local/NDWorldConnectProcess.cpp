#include "main/local/NDWorldConnectProcess.h"

#include "protocol/NDWorldToNDLogin/NDWS2LS_Register.h"
//#include "../NDServerShare/include/Protocol/NDCenterToNDGameDB/NDC2GDB_Register.h"

#include "main/local/NDWorldServer.h"


NDWorldConnectProcess::NDWorldConnectProcess(void)
{
}

NDWorldConnectProcess::~NDWorldConnectProcess(void)
{
}

void NDWorldConnectProcess::connected( const NDServerInfo* pServerInfo )
{
	if (NULL == pServerInfo)	return;

	//NDCenterServerConfig* pCenterCfg = (NDCenterServerConfig*)sNDCenterServer.config();
	//if ( NULL == pCenterCfg )
	//{
	//	NDTOTAL_LOG_ERROR( " [NDCenterConnectProcess::connected] pCenterCfg is NULL! " )
	//	return;
	//}

	switch( pServerInfo->getServerType() )
	{
	case LOGIN_SERVER:
		{
			NDLocalServerInfo* pLocalServerInfo = sNDWorldServer.getLocalServerInfo();

			NDWS2LS_Register_Req registerReq;
			registerReq.m_nWorldID		= (NDUint16)pLocalServerInfo->getServerID();
			registerReq.m_netAddress    = pLocalServerInfo->getNetAddress();

			const char* szServerName	= pLocalServerInfo->getServerName();
			NDUint32 nSize				= (NDUint32)strlen( szServerName );
			NDShareBaseGlobal::safeStrCopy( registerReq.m_szWorldName, szServerName, ND_WORLDNAME_LEN, nSize );
			//memcpy( registerReq.m_szWorldName, pLocalServerInfo->getServerName(), ND_WORLDNAME_LEN );

			NDServerManager::getSingleton().sendToServer( registerReq, pServerInfo->getSessionID() );
		}
		break;
	//case GAMEDB_SERVER:
	//	{
	//		NDServerInfo* pLocalServerInfo = NDServerManager::getSingleton().getServerInfo();

	//		NDCS2GDB_Register_Req registerReq;
	//		registerReq.m_nWorldID		= pCenterCfg->getWorldID();
	//		registerReq.m_netAddress	= pLocalServerInfo->getNetAddress();

	//		NDServerManager::getSingleton().sendToServer( registerReq, pServerInfo->getSessionID() );
	//	}	
	//	break;
	default:
		break;
	}
}

void NDWorldConnectProcess::error( const NDServerInfo* pServerInfo )
{
	if (NULL == pServerInfo)	return;

	switch( pServerInfo->getServerType() )
	{
	case LOGIN_SERVER:
		{
		}
		break;
	//case GAMEDB_SERVER:
	//	{
	//	}
	//	break;
	default:
		break;
	}
}

