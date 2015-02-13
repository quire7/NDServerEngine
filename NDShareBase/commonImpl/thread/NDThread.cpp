#include "thread/NDThread.h"

#ifdef WIN32
#include "../windows/thread/NDWindowsThreadImpl.h"
#elif defined(LINUX)
#include "../linux/thread/NDLinuxThreadImpl.h"
#endif




_NDSHAREBASE_BEGIN

NDThread::NDThread()
{
#ifdef WIN32
	m_pNDThreadImpl = new NDWindowsThreadImpl;
#elif defined(LINUX)
	m_pNDThreadImpl = new NDLiunxThreadImpl;
#endif
}

NDThread::~NDThread()
{
	if ( NULL == m_pNDThreadImpl )
	{
		delete m_pNDThreadImpl;
		m_pNDThreadImpl = NULL;
	}
}

void NDThread::start()
{
	if( NULL == m_pNDThreadImpl )	return;
	m_pNDThreadImpl->start();
}

void NDThread::exit( void* pRetVal )
{
	if( NULL == m_pNDThreadImpl )	return;
	m_pNDThreadImpl->exit( pRetVal );
}

void NDThread::run()
{
	if( NULL == m_pNDThreadImpl )	return;
	m_pNDThreadImpl->run();
}

void NDThread::stop()
{
	if( NULL == m_pNDThreadImpl )	return;
	m_pNDThreadImpl->stop();
}

ThreadID NDThread::getThreadID() const
{
	if( NULL == m_pNDThreadImpl )	return 0;
	return m_pNDThreadImpl->getThreadID();
}

EThreadStatus NDThread::getStatus() const
{	
	if( NULL == m_pNDThreadImpl )	return EThreadStatus_invalid;
	return m_pNDThreadImpl->getStatus();
}

void NDThread::setStatus( EThreadStatus eStatus )
{
	if( NULL == m_pNDThreadImpl )	return;

	m_pNDThreadImpl->setStatus( eStatus );
}

const char* NDThread::getName() const
{
	if( NULL == m_pNDThreadImpl )	return NULL;

	return m_pNDThreadImpl->getName();
}

void NDThread::setName( const char* szName )
{
	if( NULL == m_pNDThreadImpl )	return;

	m_pNDThreadImpl->setName( szName );
}


_NDSHAREBASE_END
