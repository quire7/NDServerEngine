#ifndef __SHARE_BASE_ND_LINUX_EPOLL_H__
#define __SHARE_BASE_ND_LINUX_EPOLL_H__

#include "NDLinuxCommon.h"
#include <sys/epoll.h>

_NDSHAREBASE_BEGIN

#ifndef EPOLL_EVENT_MAX
#define EPOLL_EVENT_MAX		(20480)					//epoll������¼�������Ϊ2w;
#endif

class NDLinuxEpoll
{
private:
	int				m_nEpoolFD;						//epoll���;
	epoll_event		m_epollEvents[EPOLL_EVENT_MAX];	//epoll�¼�����;(����������SOCKET);

public:
	NDLinuxEpoll();
	~NDLinuxEpoll();

	
	NDBool				registerReadWriteEvent( SOCKET fd, void* pData );
	NDBool				unregisterReadWriteEvent( SOCKET fd, void* pData );

	//�Ż�READ_WRITE_EVENT,����Ҫ���͵�ʱ���ע��WRITE_EVENT;
	NDBool				registerWriteEvent( SOCKET fd, void* pData );
	NDBool				unregisterWriteEvent( SOCKET fd, void* pData );

	NDBool				run( NDUint32& refNum );
};



_NDSHAREBASE_END

#endif

