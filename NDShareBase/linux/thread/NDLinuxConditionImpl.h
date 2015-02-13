#ifndef __SHARE_BASE_ND_LINUX_CONDITION_IMPL_H__
#define __SHARE_BASE_ND_LINUX_CONDITION_IMPL_H__


#include "NDLinuxCommon.h"
#include "thread/NDConditionImpl.h"

_NDSHAREBASE_BEGIN

class NDLinuxConditionImpl : public NDConditionImpl
{
private:
	pthread_mutex_t		m_Mutex;
	pthread_cond_t		m_Cond;
	
public:
	explicit NDLinuxConditionImpl();
	~NDLinuxConditionImpl();

	void		beginSynchronized();
	void		endSynchronized();

	NDBool		wait();
	NDBool		wait( NDUint32 nMilliSecondsTimeOut );

	void		signal();
	void		broadcast();

};


_NDSHAREBASE_END
#endif

