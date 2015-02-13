#ifndef __SHARE_BASE_ND_LINUX_EPOLL_H__
#define __SHARE_BASE_ND_LINUX_EPOLL_H__

#include "NDLinuxCommon.h"
#include <sys/epoll.h>

_NDSHAREBASE_BEGIN

#ifndef EPOLL_EVENT_MAX
#define EPOLL_EVENT_MAX		(20480)					//epoll管理的事件句柄最大为2w;
#endif

class NDLinuxEpoll
{
private:
	int				m_nEpoolFD;						//epoll句柄;
	epoll_event		m_epollEvents[EPOLL_EVENT_MAX];	//epoll事件数组;(不包括监听SOCKET);

public:
	NDLinuxEpoll();
	~NDLinuxEpoll();

	
	NDBool				registerReadWriteEvent( SOCKET fd, void* pData );
	NDBool				unregisterReadWriteEvent( SOCKET fd, void* pData );

	//优化READ_WRITE_EVENT,在需要发送的时候才注册WRITE_EVENT;
	NDBool				registerWriteEvent( SOCKET fd, void* pData );
	NDBool				unregisterWriteEvent( SOCKET fd, void* pData );

	NDBool				run( NDUint32& refNum );
};



_NDSHAREBASE_END

#endif

