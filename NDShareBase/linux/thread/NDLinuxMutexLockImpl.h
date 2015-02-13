#ifndef __SHARE_BASE_ND_LINUX_MUTEX_LOCK_IMPL_H__
#define __SHARE_BASE_ND_LINUX_MUTEX_LOCK_IMPL_H__


#include "NDLinuxCommon.h"
#include "thread/NDMutex.h"


_NDSHAREBASE_BEGIN

class NDLinuxMutexLockImpl : public NDSysLock
{
private:
	pthread_mutex_t			m_Mutex;

public:
	NDLinuxMutexLockImpl()		
	{ 
		pthread_mutexattr_t attr;
		pthread_mutexattr_init( &attr );
		pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE_NP );	//PTHREAD_MUTEX_RECURSIVE_NP互斥量类型允许同一线程在互斥量解锁之前对该互斥量进行多次加锁;
		pthread_mutex_init( &m_Mutex, &attr );
		pthread_mutexattr_destroy( &attr );
	};
	~NDLinuxMutexLockImpl()		{ pthread_mutex_destroy( &m_Mutex ); };

	void lock()					{ pthread_mutex_lock( &m_Mutex ); };
	void unlock()				{ pthread_mutex_unlock( &m_Mutex ); };
};

_NDSHAREBASE_END
#endif
