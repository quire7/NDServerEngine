#ifndef __SHARE_BASE_ND_WINDOWS_IOCP_H__
#define __SHARE_BASE_ND_WINDOWS_IOCP_H__

#include "NDWindowsCommon.h"


_NDSHAREBASE_BEGIN


class NDWindowsIOCP
{
private:
	HANDLE				m_hCompletionPort;
public:
	NDWindowsIOCP();
	~NDWindowsIOCP();

	
	NDBool				registerReadWriteEvent( SOCKET fd, void* pData );
	NDBool				unregisterReadWriteEvent( SOCKET fd, void* pData );

	//优化READ_WRITE_EVENT,在需要发送的时候才注册WRITE_EVENT;
	NDBool				registerWriteEvent( SOCKET fd, void* pData );
	NDBool				unregisterWriteEvent( SOCKET fd, void* pData );

	NDBool				run( NDUint32& refNum );
};



_NDSHAREBASE_END

#endif

