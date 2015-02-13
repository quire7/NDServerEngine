#ifndef __NDSHAREBASE_THREAD_IMPL_H__
#define __NDSHAREBASE_THREAD_IMPL_H__

#include "NDTypes.h"
#include "NDShareBaseEnums.h"

_NDSHAREBASE_BEGIN

class NDThreadImpl
{
public:
	NDThreadImpl(){};
	virtual ~NDThreadImpl(){};

	virtual void			start()=0;
	virtual	void			exit( void* pRetVal )=0;

	virtual	void			run()=0;
	virtual void			stop()=0;

	virtual ThreadID		getThreadID() const=0;

	virtual	EThreadStatus	getStatus() const=0;
	virtual	void			setStatus( EThreadStatus eStatus )=0;

	virtual	const char*		getName() const=0;
	virtual	void			setName( const char* szName )=0;
};

_NDSHAREBASE_END
#endif

