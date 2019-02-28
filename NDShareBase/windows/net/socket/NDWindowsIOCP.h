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

	//�Ż�READ_WRITE_EVENT,����Ҫ���͵�ʱ���ע��WRITE_EVENT;
	NDBool				registerWriteEvent( SOCKET fd, void* pData );
	NDBool				unregisterWriteEvent( SOCKET fd, void* pData );

	NDBool				run( NDUint32& refNum );
};



_NDSHAREBASE_END

#endif

