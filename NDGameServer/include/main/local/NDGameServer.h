#ifndef __GAMESERVER_ND_GAME_SERVER_H__
#define __GAMESERVER_ND_GAME_SERVER_H__



#include "NDGameCallBack.h"
#include "NDGameConnectProcess.h"

class NDGameServer : public NDLocalServer, public NDSingleton<NDGameServer>
{
private:
	NDRemoteServerManager*			m_pRoomServerManager;
	NDRemoteServerManager*			m_pGatewayServerManager;
public:
	NDGameServer(void)
	{
		m_pLocalServerInfo		= new NDLocalServerInfo;
		m_pDataProcess			= new NDDataProcess;
		m_pConfig				= new NDXmlConfigServerCommon;
		m_pConnProcess			= new NDGameConnectProcess;
		m_pCallBack				= new NDGameCallBack;
		m_pConsoleCMDMagager	= new NDConsoleCMDManager;

		m_pRoomServerManager	= new NDRemoteServerManager;
		m_pGatewayServerManager	= new NDRemoteServerManager;
	}
	~NDGameServer(void)
	{
		SAFE_DELETE( m_pRoomServerManager )
		SAFE_DELETE( m_pGatewayServerManager )
	}

	NDRemoteServerManager*		roomServerManager()		{	return m_pRoomServerManager; }
	NDRemoteServerManager*		gatewayServerManager()	{	return m_pGatewayServerManager; }
};

#define sNDGameServer	NDGameServer::getSingleton()

#endif

