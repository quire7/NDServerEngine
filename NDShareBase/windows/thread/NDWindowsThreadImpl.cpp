#include "NDWindowsThreadImpl.h"

#include <process.h>


_NDSHAREBASE_BEGIN

NDWindowsThreadImpl::NDWindowsThreadImpl()
{
	m_nThreadID		= 0;
	m_eThreadStatus	= EThreadStatus_invalid;
	m_hThread		= NULL;

	memset( m_szName, 0, sizeof(m_szName) );
}

NDWindowsThreadImpl::~NDWindowsThreadImpl()
{
	m_nThreadID		= 0;
	m_eThreadStatus	= EThreadStatus_exit;
}

void NDWindowsThreadImpl::start()
{
	m_hThread	= (HANDLE)::_beginthreadex( NULL, 0, myThreadProcess, this, 0, &m_nThreadID );
}

void NDWindowsThreadImpl::exit( void* pRetVal )
{
	pRetVal;
	//::_endthreadex(0);		//这个函数有没有调用的必要性;
	::CloseHandle( m_hThread );
}

void NDWindowsThreadImpl::run()
{

}

void NDWindowsThreadImpl::stop()
{

}

void NDWindowsThreadImpl::setName( const char* szName )
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

unsigned int WINAPI NDWindowsThreadImpl::myThreadProcess( void* pDerivedThread )
{
	NDWindowsThreadImpl* pthread = (NDWindowsThreadImpl*)pDerivedThread;
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

	return 0;
}




_NDSHAREBASE_END

