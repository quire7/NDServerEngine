
#include "NDWindowsConditionImpl.h"

_NDSHAREBASE_BEGIN

NDWindowsConditionImpl::NDWindowsConditionImpl() : m_nLockCount( 0 ) 
{
	::InitializeCriticalSection( &m_critsecExternalProtection );
	::InitializeCriticalSection( &m_critsecWaitSetProtection );
}

NDWindowsConditionImpl::~NDWindowsConditionImpl()
{
	::DeleteCriticalSection( &m_critsecExternalProtection );
	::DeleteCriticalSection( &m_critsecWaitSetProtection );
}

void NDWindowsConditionImpl::beginSynchronized()
{
	::EnterCriticalSection( &m_critsecExternalProtection );
	++m_nLockCount;
}

void NDWindowsConditionImpl::endSynchronized()
{
	--m_nLockCount;
	::LeaveCriticalSection( &m_critsecExternalProtection );
}

NDBool NDWindowsConditionImpl::wait()
{
	return wait( INFINITE );
}

NDBool NDWindowsConditionImpl::wait( NDUint32 nMilliSecondsTimeOut )
{
	BOOL bAlertable = FALSE;

	// Enter a new event handle into the wait set.
	HANDLE hWaitEvent = push();
	if( NULL == hWaitEvent )
		return NDFalse;

	// Store the current lock count for re-acquisition.
	NDUint32 nThisThreadsLockCount = m_nLockCount;
	m_nLockCount = 0;

	// Release the synchronization lock the appropriate number of times.
	// Win32 allows no error checking here.
	for( NDUint32 i=0; i < nThisThreadsLockCount; ++i )
	{
		::LeaveCriticalSection( &m_critsecExternalProtection );
	}

	// NOTE: Conceptually, releasing the lock and entering the wait
	// state is done in one atomic step. Technically, that is not
	// true here, because we first leave the critical section and
	// then, in a separate line of code, call WaitForSingleObjectEx.
	// The reason why this code is correct is that our thread is placed
	// in the wait set *before* the lock is released. Therefore, if
	// we get preempted right here and another thread notifies us, then
	// that notification will *not* be missed: the wait operation below
	// will find the event signalled.

	// Wait for the event to become signalled.
	DWORD dwWaitResult = ::WaitForSingleObjectEx(
		hWaitEvent,
		(DWORD)nMilliSecondsTimeOut,
		bAlertable
		);

	// If the wait failed, store the last error because it will get
	// overwritten when acquiring the lock.
	DWORD dwLastError = 0;
	if( WAIT_FAILED == dwWaitResult )
		dwLastError = ::GetLastError();

	// Acquire the synchronization lock the appropriate number of times.
	// Win32 allows no error checking here.
	for( NDUint32 j=0; j < nThisThreadsLockCount; ++j )
	{
		::EnterCriticalSection( &m_critsecExternalProtection );
	}

	// Restore lock count.
	m_nLockCount = nThisThreadsLockCount;

	NDBool bRet = NDTrue;
	// Close event handle
	if( ! CloseHandle(hWaitEvent) )
	{
		bRet = NDFalse;
	}

	if( WAIT_FAILED == dwWaitResult )
	{
		bRet = NDFalse;
		::SetLastError(dwLastError);
	}

	return bRet;
}

void NDWindowsConditionImpl::signal()
{
	// Pop the first handle, if any, off the wait set.
	HANDLE hWaitEvent = pop();

	// If there is not thread currently waiting, that's just fine.
	if(NULL == hWaitEvent)
		return;

	// Signal the event.
	::SetEvent(hWaitEvent);
}

void NDWindowsConditionImpl::broadcast()
{
	// Signal all events on the deque, then clear it. Win32 allows no
	// error checking on entering and leaving the critical section.
	//
	::EnterCriticalSection(&m_critsecWaitSetProtection);
	deque<HANDLE>::const_iterator it_run = m_deqWaitSet.begin();
	deque<HANDLE>::const_iterator it_end = m_deqWaitSet.end();
	for( ; it_run < it_end; ++it_run )
	{
		if( ! SetEvent(*it_run) )
		{
			::LeaveCriticalSection(&m_critsecWaitSetProtection);
			return;
		}
	}
	m_deqWaitSet.clear();
	::LeaveCriticalSection(&m_critsecWaitSetProtection);
}

HANDLE NDWindowsConditionImpl::push()
{
	// Create the new event.
	HANDLE hWaitEvent = ::CreateEvent(
		NULL, // no security
		FALSE, // auto-reset event
		FALSE, // initially unsignalled
		NULL // string name
		);
	//
	if( NULL == hWaitEvent ) {
		return NULL;
	}

	// Push the handle on the deque.
	::EnterCriticalSection( &m_critsecWaitSetProtection );
	m_deqWaitSet.push_back( hWaitEvent );
	::LeaveCriticalSection( &m_critsecWaitSetProtection );

	return hWaitEvent;
}

HANDLE NDWindowsConditionImpl::pop()
{
	// Pop the first handle off the deque.
	//
	::EnterCriticalSection( &m_critsecWaitSetProtection );
	HANDLE hWaitEvent = NULL; 
	if( 0 != m_deqWaitSet.size() )
	{
		hWaitEvent = m_deqWaitSet.front();
		m_deqWaitSet.pop_front();
	}
	::LeaveCriticalSection( &m_critsecWaitSetProtection );

	return hWaitEvent;
}

_NDSHAREBASE_END
