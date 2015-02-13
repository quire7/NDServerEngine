
#include "NDLinuxConditionImpl.h"

_NDSHAREBASE_BEGIN

NDLinuxConditionImpl::NDLinuxConditionImpl() 
{
	//初始化m_Mutex变量;
	pthread_mutexattr_t attr;
	pthread_mutexattr_init( &attr );
	pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE_NP );	//PTHREAD_MUTEX_RECURSIVE_NP互斥量类型允许同一线程在互斥量解锁之前对该互斥量进行多次加锁;
	pthread_mutex_init( &m_Mutex, &attr );
	pthread_mutexattr_destroy( &attr );

	//初始化m_Cond变量;
	pthread_condattr_t cond_attr;
	pthread_condattr_init( &cond_attr );
	pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_PRIVATE);
	pthread_cond_init(&m_Cond, &cond_attr);
	pthread_condattr_destroy(&cond_attr);
}

NDLinuxConditionImpl::~NDLinuxConditionImpl()
{
	pthread_cond_destroy( &m_Cond );

	pthread_mutex_destroy( &m_Mutex );
}

void NDLinuxConditionImpl::beginSynchronized()
{
	pthread_mutex_lock( &m_Mutex );
}

void NDLinuxConditionImpl::endSynchronized()
{
	pthread_mutex_unlock( &m_Mutex );
}

NDBool NDLinuxConditionImpl::wait()
{
	return ( 0 == pthread_cond_wait( &m_Cond, &m_Mutex ) );
}

//int pthread_cond_timedwait(pthread_cond_t* cond, pthread_mutex_t * mutex, const struct timespec* abstime);
//参数abstime在这里用来表示和超时时间相关的一个参数,但是需要注意的是它所表示的是一个绝对时间;
//而不是一个时间间隔数值,只有当系统的当前时间达到或者超过abstime所表示的时间时,才会触发超时事件;
//
NDBool NDLinuxConditionImpl::wait( NDUint32 nMilliSecondsTimeOut )
{
	//get the current time;
	struct timeval now;
	gettimeofday( &now, NULL );

	struct timespec abstime;
	abstime.tv_nsec = now.tv_usec * 1000 + ( nMilliSecondsTimeOut % 1000 ) * 1000000;
	abstime.tv_sec	= now.tv_sec + nMilliSecondsTimeOut / 1000;

	return ( 0 == pthread_cond_timedwait( &m_Cond, &m_Mutex, &abstime ) );
}

void NDLinuxConditionImpl::signal()
{
	 pthread_cond_signal( &m_Cond );
}

void NDLinuxConditionImpl::broadcast()
{
	pthread_cond_broadcast( &m_Cond );
}

_NDSHAREBASE_END

