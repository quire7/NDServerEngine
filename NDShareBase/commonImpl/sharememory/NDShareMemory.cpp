#include "sharememory/NDShareMemory.h"

#if LINUX
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <errno.h>
#elif WIN32
#ifndef	WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

_NDSHAREBASE_BEGIN

NDShareMemory::NDShareMemory() : m_bCreate( NDFalse ), m_NDSMHandle( INVALID_SM_HANDLE )
{
}

NDShareMemory::~NDShareMemory()
{
	if ( m_bCreate )
	{
		destoryShareMemory();
	}
	else
	{
		closeShareMemory();
	}
}

NDBool NDShareMemory::createShareMemory( NDSM_KEY nKey, NDUint32 nSize )
{
	m_bCreate = NDTrue;
#if LINUX
	//key = ftok(keybuf,'w'); 
	m_NDSMHandle =  shmget( nKey, nSize, IPC_CREAT|IPC_EXCL|0666 ); 
	printf( "handle = %d ,key = %d ,error: %d \r\n", m_NDSMHandle, nKey, errno );
	return (INVALID_SM_HANDLE != m_NDSMHandle );

#elif WIN32
	CHAR keybuf[32] = {0};
	_snprintf( keybuf, 31, "%u", nKey );
	
	return ( INVALID_SM_HANDLE != ( m_NDSMHandle = CreateFileMapping( (HANDLE)INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, nSize, keybuf ) ) );
#endif
}


void NDShareMemory::destoryShareMemory()
{
	if ( INVALID_SM_HANDLE == m_NDSMHandle )
	{
		return;
	}

#if LINUX
	shmctl( m_NDSMHandle, IPC_RMID, 0 );
#elif WIN32
	CloseHandle(m_NDSMHandle);
#endif

	m_NDSMHandle = INVALID_SM_HANDLE;
}

NDBool NDShareMemory::openShareMemory( NDSM_KEY nKey, NDUint32 nSize )
{
#if LINUX
	//key = ftok(keybuf,'w'); 
	m_NDSMHandle =   shmget( nKey, nSize,0 );
	printf( "handle = %d ,key = %d ,error: %d \r\n", m_NDSMHandle, nKey, errno );
	return (INVALID_SM_HANDLE != m_NDSMHandle );

#elif WIN32
	CHAR keybuf[32] = {0};
	_snprintf( keybuf, 31, "%u", nKey );

	nSize;//ø’“˝”√;

	return ( NULL != ( m_NDSMHandle = OpenFileMapping( FILE_MAP_ALL_ACCESS, TRUE, keybuf ) ) );
#endif
}

void NDShareMemory::closeShareMemory()
{
	if ( INVALID_SM_HANDLE == m_NDSMHandle )
	{
		return;
	}

#if LINUX
	//shmctl( m_NDSMHandle, IPC_RMID, 0 ); 
#elif WIN32
	CloseHandle(m_NDSMHandle);
#endif

	m_NDSMHandle = INVALID_SM_HANDLE;
}

char* NDShareMemory::mapShareMemory()
{
	if ( INVALID_SM_HANDLE == m_NDSMHandle )
	{
		return NULL;
	}

#if LINUX
	return (char*)shmat( m_NDSMHandle, 0, 0 );
#elif WIN32
	return (char*)MapViewOfFile( m_NDSMHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0 );
#endif
}

void NDShareMemory::unMapShareMem( char* pMemoryPtr )
{
	if ( NULL == pMemoryPtr ) return;

#if LINUX
	shmdt( pMemoryPtr );
#elif WIN32
	UnmapViewOfFile( pMemoryPtr );
#endif
}

_NDSHAREBASE_END

