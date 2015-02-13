/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\thread\NDThread.h
	file base:	NDThread
	purpose:	thread class;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_THREAD_H__
#define __SHARE_BASE_ND_THREAD_H__

#include "NDTypes.h"
#include "NDShareBaseEnums.h"

_NDSHAREBASE_BEGIN

class NDThreadImpl;
class NDThread
{
private:
	NDThreadImpl*	m_pNDThreadImpl;

public:
	NDThread();
	virtual ~NDThread();
	
	void	start();
	void	exit( void* pRetVal );

	virtual	void	run();
	virtual void	stop();

	ThreadID		getThreadID() const;

	EThreadStatus	getStatus() const;
	void			setStatus( EThreadStatus eStatus );

	const char*		getName() const;
	void			setName( const char* szName );
};

_NDSHAREBASE_END
#endif

