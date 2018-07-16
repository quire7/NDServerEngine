#ifndef __LOGINSERVER_ND_LOGIN_SERVER_H__
#define __LOGINSERVER_ND_LOGIN_SERVER_H__


#include "NDLoginCallBack.h"

#include "main/remote/NDRemoteWorldServerManager.h"

class NDLoginServer : public NDLocalServer, public NDSingleton<NDLoginServer>
{
private:
	NDRemoteWorldServerManager*		m_pWorldServerManager;
	
public:
	NDLoginServer(void){}
	~NDLoginServer(void){ release(); }

	NDBool							initialize( SERVERTYPE eType, NDUint32 nServerID, const char* szXmlConfig )
	{
		m_pLocalServerInfo		= new NDLocalServerInfo;
		m_pDataProcess			= new NDDataProcess;
		m_pConfig				= new NDXmlConfigServerCommon;
		m_pCallBack				= new NDLoginCallBack;
		m_pConsoleCMDMagager	= new NDConsoleCMDManager;

		m_pWorldServerManager	= new NDRemoteWorldServerManager;

		return NDLocalServer::initialize( eType, nServerID, szXmlConfig );
	}
	void							release()
	{
		SAFE_DELETE( m_pWorldServerManager );
	}

	NDRemoteWorldServerManager*		worldManager()		{	return m_pWorldServerManager; }

};

#define sNDLoginServer	NDLoginServer::getSingleton()

#endif

