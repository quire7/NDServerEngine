#ifndef __GATEWAYSERVER_ND_GATEWAY_SERVER_H__
#define __GATEWAYSERVER_ND_GATEWAY_SERVER_H__


#include "NDGatewayCallBack.h"
#include "NDGatewayConnectProcess.h"


class NDGatewayServer : public NDLocalServer, public NDSingleton<NDGatewayServer>
{
private:
	
public:
	NDGatewayServer(void)
	{
		m_pLocalServerInfo		= new NDLocalServerInfo;
		m_pDataProcess			= new NDDataProcess;
		m_pConfig				= new NDXmlConfigServerCommon;
		m_pConnProcess			= new NDGatewayConnectProcess;
		m_pCallBack				= new NDGatewayCallBack;
		m_pConsoleCMDMagager	= new NDConsoleCMDManager;

	}
	~NDGatewayServer(void)
	{
	}


};

#define sNDGatewayServer	NDGatewayServer::getSingleton()

#endif

