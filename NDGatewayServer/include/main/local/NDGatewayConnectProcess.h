#ifndef __GATEWAYSERVER_ND_GATEWAY_CONNECT_PROCESS_H__
#define __GATEWAYSERVER_ND_GATEWAY_CONNECT_PROCESS_H__

#include "NDPreCompiler.h"

class NDGatewayConnectProcess : public NDConnectProcess
{
public:
	NDGatewayConnectProcess(void);
	~NDGatewayConnectProcess(void);

	void connected(const NDServerInfo* pServerInfo);
	void error(const NDServerInfo* pServerInfo);
};

#endif
