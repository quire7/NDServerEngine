#ifndef __ROOMSERVER_ND_ROOM_CONNECT_PROCESS_H__
#define __ROOMSERVER_ND_ROOM_CONNECT_PROCESS_H__

#include "NDPreCompiler.h"

class NDRoomConnectProcess : public NDConnectProcess
{
public:
	NDRoomConnectProcess(void);
	~NDRoomConnectProcess(void);

	void connected(const NDServerInfo* pServerInfo);
	void error(const NDServerInfo* pServerInfo);
};

#endif
