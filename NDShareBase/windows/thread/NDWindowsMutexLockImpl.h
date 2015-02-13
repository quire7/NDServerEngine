#ifndef __NDSHAREBASE_WINDOWS_MUTEX_LOCK_IMPL_H__
#define __NDSHAREBASE_WINDOWS_MUTEX_LOCK_IMPL_H__


#include "NDWindowsCommon.h"
#include "thread/NDMutex.h"

_NDSHAREBASE_BEGIN

class NDWindowsMutexLockImpl : public NDSysLock
{
private:
	CRITICAL_SECTION	m_CriticalSection;

public:
	NDWindowsMutexLockImpl()	{ InitializeCriticalSection( &m_CriticalSection ); };
	~NDWindowsMutexLockImpl()	{ DeleteCriticalSection( &m_CriticalSection ); };

	void lock()					{ EnterCriticalSection( &m_CriticalSection ); };
	void unlock()				{ LeaveCriticalSection( &m_CriticalSection ); };
};

_NDSHAREBASE_END
#endif
