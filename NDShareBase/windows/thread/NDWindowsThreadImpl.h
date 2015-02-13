#ifndef __NDSHAREBASE_WINDOWS_THREAD_IMPL_H__
#define __NDSHAREBASE_WINDOWS_THREAD_IMPL_H__


#include "NDWindowsCommon.h"
#include "NDShareBaseMacros.h"
#include "thread/NDThreadImpl.h"



_NDSHAREBASE_BEGIN
class NDWindowsThreadImpl : public NDThreadImpl
{
private:
	ThreadID			m_nThreadID;
	
	EThreadStatus		m_eThreadStatus;

	HANDLE				m_hThread;

	char				m_szName[MAX_PATH_LEN];
	
public:
	NDWindowsThreadImpl();
	~NDWindowsThreadImpl();

	void	start();
	void	exit( void* pRetVal );

	virtual	void	run();
	virtual void	stop();

	ThreadID		getThreadID() const					{ return m_nThreadID; };

	EThreadStatus	getStatus() const					{ return m_eThreadStatus; };
	void			setStatus( EThreadStatus eStatus )	{ m_eThreadStatus = eStatus; };

	const char*		getName() const						{ return m_szName; };
	void			setName( const char* szName );
	
	static	unsigned int WINAPI myThreadProcess( void* pDerivedThread );
};

_NDSHAREBASE_END
#endif
