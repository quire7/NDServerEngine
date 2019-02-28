#include "NDWindowsIOCP.h"

#include "net/session/NDSession.h"

_NDSHAREBASE_BEGIN

NDWindowsIOCP::NDWindowsIOCP()
{
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
}

NDWindowsIOCP::~NDWindowsIOCP()
{
	//关闭完成端口;
	if (m_hCompletionPort != NULL)
	{
		CloseHandle(m_hCompletionPort);
		m_hCompletionPort = NULL;
	}
}

NDBool NDWindowsIOCP::registerReadWriteEvent(SOCKET fd, void* pData)
{
	return NDTrue;
}

NDBool NDWindowsIOCP::unregisterReadWriteEvent(SOCKET fd, void* pData)
{
	return NDTrue;
}

NDBool NDWindowsIOCP::registerWriteEvent(SOCKET fd, void* pData)
{
	return NDTrue;
}

NDBool NDWindowsIOCP::unregisterWriteEvent(SOCKET fd, void* pData)
{
	return NDTrue;
}

NDBool NDWindowsIOCP::run(NDUint32& refNum)
{
	return NDTrue;
}





_NDSHAREBASE_END

