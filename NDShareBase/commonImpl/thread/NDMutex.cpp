#include "thread/NDMutex.h"


#ifdef WIN32
#include "../windows/thread/NDWindowsMutexLockImpl.h"
#else
#include "../linux/thread/NDLinuxMutexLockImpl.h"
#endif


_NDSHAREBASE_BEGIN

NDMutexLock::NDMutexLock()
{
#ifdef  WIN32
	m_pMutexLock = new NDWindowsMutexLockImpl;
#else
	m_pMutexLock = new NDLinuxMutexLockImpl;
#endif
}

NDMutexLock::~NDMutexLock()
{
	if ( NULL != m_pMutexLock )
	{
		delete m_pMutexLock;
		m_pMutexLock = NULL;
	}
}

void NDMutexLock::lock()
{
	m_pMutexLock->lock();
}

void NDMutexLock::unlock()
{
	m_pMutexLock->unlock();
}


_NDSHAREBASE_END
