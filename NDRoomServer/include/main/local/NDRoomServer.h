#ifndef __ROOMSERVER_ND_ROOM_SERVER_H__
#define __ROOMSERVER_ND_ROOM_SERVER_H__


#include "NDRoomCallBack.h"
#include "NDRoomConnectProcess.h"


class NDRoomServer : public NDLocalServer, public NDSingleton<NDRoomServer>
{
private:
	NDRemoteServerManager*			m_pGatewayServerManager;
public:
	NDRoomServer(void)
	{
		m_pLocalServerInfo		= new NDLocalServerInfo;
		m_pDataProcess			= new NDDataProcess;
		m_pConfig				= new NDXmlConfigServerCommon;
		m_pConnProcess			= new NDRoomConnectProcess;
		m_pCallBack				= new NDRoomCallBack;
		m_pConsoleCMDMagager	= new NDConsoleCMDManager;

		m_pGatewayServerManager	= new NDRemoteServerManager;
	}
	~NDRoomServer(void)
	{
		SAFE_DELETE( m_pGatewayServerManager )
	}

	
	NDRemoteServerManager*		gatewayServerManager()		{	return m_pGatewayServerManager; }

};

#define sNDRoomServer	NDRoomServer::getSingleton()

#endif

