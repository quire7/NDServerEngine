

#include "NDLinuxThreadImpl.h"


_NDSHAREBASE_BEGIN
NDLiunxThreadImpl::NDLiunxThreadImpl()
{
	m_nThreadID		= 0;
	m_eThreadStatus	= EThreadStatus_invalid;

	memset( m_szName, 0, sizeof(m_szName) );
}

NDLiunxThreadImpl::~NDLiunxThreadImpl()
{
	m_nThreadID		= 0;
	m_eThreadStatus	= EThreadStatus_exit;
}

void NDLiunxThreadImpl::start()
{
	pthread_create( &m_nThreadID, NULL, myThreadProcess, this );
}

void NDLiunxThreadImpl::exit( void* pRetVal )
{
	pthread_exit( pRetVal );
}

void NDLiunxThreadImpl::run()
{

}

void NDLiunxThreadImpl::stop()
{

}

void NDLiunxThreadImpl::setName( const char* szName )
{
	if ( NULL == szName || '\0' == szName[0])
	{
		return;
	}

	NDUint32 nSize = (NDUint32)strlen( szName );
	if ( nSize >= MAX_PATH_LEN )
	{
		nSize = MAX_PATH_LEN - 1;
	}

	memcpy( m_szName, szName, nSize );

	m_szName[nSize] = '\0';
}

void* NDLiunxThreadImpl::myThreadProcess( void* pDerivedThread )
{
	NDLiunxThreadImpl* pthread = (NDLiunxThreadImpl*)pDerivedThread;
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




_NDSHAREBASE_END

