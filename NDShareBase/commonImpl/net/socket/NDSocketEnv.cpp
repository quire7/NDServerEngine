#include "net/socket/NDSocketEnv.h"

#ifdef WIN32
#include <WinSock2.h>
#endif



_NDSHAREBASE_BEGIN

NDSocketEnv::NDSocketEnv()
{
#ifdef WIN32
	WSADATA	WsaData;
	WSAStartup( MAKEWORD(2,2), &WsaData );
#endif
}

NDSocketEnv::~NDSocketEnv()
{
#ifdef WIN32
	WSACleanup();
#endif
}


_NDSHAREBASE_END

