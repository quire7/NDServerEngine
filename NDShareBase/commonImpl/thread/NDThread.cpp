#include "thread/NDThread.h"


_NDSHAREBASE_BEGIN

NDThread::NDThread()
{
	m_nThreadID		= 0;
	m_eThreadStatus	= EThreadStatus_invalid;

	memset( m_szName, 0, sizeof(m_szName) );

#ifdef WIN32
	m_hThread		= NULL;
#endif
}

NDThread::~NDThread()
{
	m_nThreadID		= 0;
	m_eThreadStatus	= EThreadStatus_exit;
}

void NDThread::start()
{
#ifdef WIN32
	m_hThread	= (HANDLE)::_beginthreadex( NULL, 0, myThreadProcess, this, 0, &m_nThreadID );
#elif defined(LINUX)
	pthread_create( &m_nThreadID, NULL, myThreadProcess, this );
#endif
}

void NDThread::exit( void* pRetVal )
{
#ifdef WIN32
	pRetVal;
	if ( m_hThread )
	{
		::CloseHandle( m_hThread );
		m_hThread = NULL;
	}
#elif defined(LINUX)
	pthread_exit( pRetVal );
#endif
}

void NDThread::run()
{
}

void NDThread::stop()
{
}

void NDThread::setName( const char* szName )
{
	if ( NULL == szName || '\0' == szName[0])
	{
		return;
	}

	NDUint32 nSize = (NDUint32)strlen( szName );
	if ( nSize >= ND_THREAD_NAME_MAX )
	{
		nSize = ND_THREAD_NAME_MAX - 1;
	}

	memcpy( m_szName, szName, nSize );

	m_szName[nSize] = '\0';
}

#ifdef WIN32
unsigned int WINAPI NDThread::myThreadProcess(void* pDerivedThread)
{
	NDThread* pthread = (NDThread*)pDerivedThread;
	if ( NULL == pthread )
	{
		return 0;
	}

	//set thread's status to "running";
	pthread->setStatus( EThreadStatus_running );

	//here - polymorphism used.( derived::run() called .);
	pthread->run();

	//set thread's status to "exit";
	pthread->setStatus( EThreadStatus_exit );

	pthread->exit( NULL );

	::_endthreadex(0);

	return 0;
}
#elif defined(LINUX)
void*	NDThread::myThreadProcess( void* pDerivedThread )
{
	NDThread* pthread = (NDThread*)pDerivedThread;
	if ( NULL == pthread )
	{
		return 0;
	}

	//set thread's status to "running";
	pthread->setStatus( EThreadStatus_running );

	//here - polymorphism used.( derived::run() called .);
	pthread->run();

	//set thread's status to "exit";
	pthread->setStatus( EThreadStatus_exit );

	//int nRet = 0;
	//pthread->exit( &nRet );

	return 0;
}
#endif

_NDSHAREBASE_END
