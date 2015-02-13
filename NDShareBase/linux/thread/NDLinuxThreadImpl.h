#ifndef __SHARE_BASE_ND_LINUX_THREAD_IMPL_H__
#define __SHARE_BASE_ND_LINUX_THREAD_IMPL_H__


#include "NDLinuxCommon.h"
#include "NDShareBaseMacros.h"
#include "thread/NDThreadImpl.h"

_NDSHAREBASE_BEGIN

class NDLiunxThreadImpl : public NDThreadImpl
{
private:
	ThreadID			m_nThreadID;
	
	EThreadStatus		m_eThreadStatus;

	char				m_szName[MAX_PATH_LEN];

public:
	NDLiunxThreadImpl();
	~NDLiunxThreadImpl();

	void	start();
	void	exit( void* pRetVal );

	virtual	void	run();
	virtual void	stop();

	ThreadID		getThreadID() const					{ return m_nThreadID; };

	EThreadStatus	getStatus() const					{ return m_eThreadStatus; };
	void			setStatus( EThreadStatus eStatus )	{ m_eThreadStatus = eStatus; };

	const char*		getName() const						{ return m_szName; };
	void			setName( const char* szName );
	
	static	void*	myThreadProcess( void* pDerivedThread );
};

_NDSHAREBASE_END
#endif
