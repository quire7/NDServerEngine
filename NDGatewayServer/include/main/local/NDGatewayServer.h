#ifndef __GATEWAYSERVER_ND_GATEWAY_SERVER_H__
#define __GATEWAYSERVER_ND_GATEWAY_SERVER_H__


#include "main/data/NDGTWSPlayerManager.h"

#include "NDGatewayCallBack.h"
#include "NDGatewayConnectProcess.h"


class NDGatewayServer : public NDLocalServer, public NDSingleton<NDGatewayServer>
{
private:
	NDGTWSPlayerManager*			m_pNDGTWSPlayerManager;
public:
	NDGatewayServer(void){}
	~NDGatewayServer(void){ release(); }

	NDBool						initialize( SERVERTYPE eType, NDUint32 nServerID, const char* szXmlConfig )
	{
		m_pLocalServerInfo		= new NDLocalServerInfo;
		m_pDataProcess			= new NDDataProcess;
		m_pConfig				= new NDXmlConfigServerCommon;
		m_pConnProcess			= new NDGatewayConnectProcess;
		m_pCallBack				= new NDGatewayCallBack;
		m_pConsoleCMDMagager	= new NDConsoleCMDManager;

		m_pNDGTWSPlayerManager	= new NDGTWSPlayerManager;

		return NDLocalServer::initialize( eType, nServerID, szXmlConfig );
	}
	void						release()
	{
		SAFE_DELETE( m_pNDGTWSPlayerManager )
	}

	NDGTWSPlayerManager*		playerManager()				{ return m_pNDGTWSPlayerManager; }
};

#define sNDGatewayServer	NDGatewayServer::getSingleton()

#endif

