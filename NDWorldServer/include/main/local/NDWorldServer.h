#ifndef __WORLDSERVER_ND_LOGIN_SERVER_H__
#define __WORLDSERVER_ND_LOGIN_SERVER_H__


#include "NDWorldCallBack.h"
#include "NDWorldConnectProcess.h"

//#include "Player/NDPlayerManager.h"

class NDWorldServer : public NDLocalServer, public NDSingleton<NDWorldServer>
{
private:
	//NDPlayerManager*			m_pPlayerMgr;
public:
	NDWorldServer(void)
	{
		m_pLocalServerInfo		= new NDLocalServerInfo;
		m_pDataProcess			= new NDDataProcess;
		m_pConfig				= new NDXmlConfigServerCommon;
		m_pConnProcess			= new NDWorldConnectProcess;
		m_pCallBack				= new NDWorldCallBack;
		m_pConsoleCMDMagager	= new NDConsoleCMDManager;

		//m_pPlayerMgr			= new NDPlayerManager;
	}
	~NDWorldServer(void)
	{
		//SAFE_DELETE( m_pPlayerMgr );
	}


//	NDPlayerManager*	playerManager()		{	return m_pPlayerMgr; }

};

#define sNDWorldServer	NDWorldServer::getSingleton()

#endif

