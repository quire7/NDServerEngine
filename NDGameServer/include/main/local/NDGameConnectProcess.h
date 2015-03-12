#ifndef __GAMESERVER_ND_GAME_CONNECT_PROCESS_H__
#define __GAMESERVER_ND_GAME_CONNECT_PROCESS_H__

#include "NDPreCompiler.h"

class NDGameConnectProcess : public NDConnectProcess
{
public:
	NDGameConnectProcess(void);
	~NDGameConnectProcess(void);

	void connected(const NDServerInfo* pServerInfo);
	void error(const NDServerInfo* pServerInfo);
};

#endif
