#ifndef __LOGINSERVER_ND_LOGIN_SERVER_H__
#define __LOGINSERVER_ND_LOGIN_SERVER_H__


#include "NDLoginCallBack.h"

#include "main/remote/NDWorldServerManager.h"
//#include "Player/NDPlayerManager.h"

class NDLoginServer : public NDLocalServer, public NDSingleton<NDLoginServer>
{
private:
	NDWorldServerManager*		m_pWorldServerManager;
	//NDPlayerManager*			m_pPlayerMgr;
public:
	NDLoginServer(void)
	{
		m_pLocalServerInfo		= new NDLocalServerInfo;
		m_pDataProcess			= new NDDataProcess;
		m_pConfig				= new NDXmlConfigServerCommon;
		m_pCallBack				= new NDLoginCallBack;
		m_pConsoleCMDMagager	= new NDConsoleCMDManager;

		m_pWorldServerManager	= new NDWorldServerManager;
		//m_pPlayerMgr			= new NDPlayerManager;
	}
	~NDLoginServer(void)
	{
		SAFE_DELETE( m_pWorldServerManager );
		//SAFE_DELETE( m_pPlayerMgr );
	}

	NDWorldServerManager*		worldManager()		{	return m_pWorldServerManager; }

//	NDPlayerManager*	playerManager()		{	return m_pPlayerMgr; }

};

#define sNDLoginServer	NDLoginServer::getSingleton()

#endif

