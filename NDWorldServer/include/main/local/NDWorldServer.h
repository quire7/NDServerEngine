#ifndef __WORLDSERVER_ND_WORLD_SERVER_H__
#define __WORLDSERVER_ND_WORLD_SERVER_H__


#include "NDWorldCallBack.h"
#include "NDWorldConnectProcess.h"

class NDWorldServer : public NDLocalServer, public NDSingleton<NDWorldServer>
{
private:
	NDRemoteServerManager*		m_pGameServerManager;
	NDRemoteServerManager*		m_pRoomServerManager;
public:
	NDWorldServer(void)
	{
		m_pLocalServerInfo		= new NDLocalServerInfo;
		m_pDataProcess			= new NDDataProcess;
		m_pConfig				= new NDXmlConfigServerCommon;
		m_pConnProcess			= new NDWorldConnectProcess;
		m_pCallBack				= new NDWorldCallBack;
		m_pConsoleCMDMagager	= new NDConsoleCMDManager;

		m_pGameServerManager	= new NDRemoteServerManager;
		m_pRoomServerManager	= new NDRemoteServerManager;
	}
	~NDWorldServer(void)
	{
		SAFE_DELETE( m_pGameServerManager )
		SAFE_DELETE( m_pRoomServerManager )
	}


	NDRemoteServerManager*		gameServerManager()		{	return m_pGameServerManager; }
	NDRemoteServerManager*		roomServerManager()		{	return m_pRoomServerManager; }
};

#define sNDWorldServer	NDWorldServer::getSingleton()

#endif

