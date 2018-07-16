#ifndef __GAMESERVER_ND_GAME_SERVER_H__
#define __GAMESERVER_ND_GAME_SERVER_H__


#include "main/data/NDGSPlayerManager.h"

#include "NDGameCallBack.h"
#include "NDGameConnectProcess.h"

class NDGameServer : public NDLocalServer, public NDSingleton<NDGameServer>
{
private:
	NDRemoteServerManager*			m_pRoomServerManager;
	NDRemoteServerManager*			m_pGatewayServerManager;

	NDGSPlayerManager*				m_pNDGSPlayerManager;
public:
	NDGameServer(void){}
	~NDGameServer(void){ release(); }

	NDBool							initialize( SERVERTYPE eType, NDUint32 nServerID, const char* szXmlConfig )
	{
		m_pLocalServerInfo		= new NDLocalServerInfo;
		m_pDataProcess			= new NDDataProcess;
		m_pConfig				= new NDXmlConfigServerCommon;
		m_pConnProcess			= new NDGameConnectProcess;
		m_pCallBack				= new NDGameCallBack;
		m_pConsoleCMDMagager	= new NDConsoleCMDManager;

		m_pRoomServerManager	= new NDRemoteServerManager;
		m_pGatewayServerManager	= new NDRemoteServerManager;
		m_pNDGSPlayerManager	= new NDGSPlayerManager;

		return NDLocalServer::initialize( eType, nServerID, szXmlConfig );
	}
	void							release()
	{
		SAFE_DELETE( m_pRoomServerManager )
		SAFE_DELETE( m_pGatewayServerManager )
		SAFE_DELETE( m_pNDGSPlayerManager )
	}

	NDRemoteServerManager*			roomServerManager()		{	return m_pRoomServerManager; }
	NDRemoteServerManager*			gatewayServerManager()	{	return m_pGatewayServerManager; }
	NDGSPlayerManager*				playerManager()			{	return m_pNDGSPlayerManager; }
};

#define sNDGameServer	NDGameServer::getSingleton()

#endif

