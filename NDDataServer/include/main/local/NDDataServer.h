#ifndef __DATASERVER_ND_DATA_SERVER_H__
#define __DATASERVER_ND_DATA_SERVER_H__


#include "NDDataCallBack.h"


class NDDataServer : public NDLocalServer, public NDSingleton<NDDataServer>
{
private:
	NDRemoteServerManager*			m_pGameServerManager;

public:
	NDDataServer(void)
	{
		m_pLocalServerInfo		= new NDLocalServerInfo;
		m_pDataProcess			= new NDDataProcess;
		m_pConfig				= new NDXmlConfigServerCommon;
		m_pCallBack				= new NDDataCallBack;
		m_pConsoleCMDMagager	= new NDConsoleCMDManager;

		m_pGameServerManager	= new NDRemoteServerManager;
		
	}
	~NDDataServer(void)
	{
		SAFE_DELETE( m_pGameServerManager );
		
	}

	NDRemoteServerManager*		gameServerManager()		{	return m_pGameServerManager; }


};

#define sNDDataServer	NDDataServer::getSingleton()

#endif

