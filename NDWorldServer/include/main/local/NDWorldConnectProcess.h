#ifndef __WORLDSERVER_ND_WORLD_CONNECT_PROCESS_H__
#define __WORLDSERVER_ND_WORLD_CONNECT_PROCESS_H__

#include "NDPreCompiler.h"

class NDWorldConnectProcess : public NDConnectProcess
{
public:
	NDWorldConnectProcess(void);
	~NDWorldConnectProcess(void);

	void connected(const NDServerInfo* pServerInfo);
	void error(const NDServerInfo* pServerInfo);
};

#endif
