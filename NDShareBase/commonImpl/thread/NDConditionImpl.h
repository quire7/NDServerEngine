#ifndef __SHARE_BASE_ND_CONDITION_IMPL_H__
#define __SHARE_BASE_ND_CONDITION_IMPL_H__


#include "NDTypes.h"

_NDSHAREBASE_BEGIN

class NDConditionImpl
{
public:
	NDConditionImpl(){};
	virtual ~NDConditionImpl() {};

	virtual void beginSynchronized()= 0;
	virtual void endSynchronized()	= 0;

	virtual NDBool wait()	= 0;
	virtual NDBool wait( NDUint32 nMilliSecondsTimeOut ) = 0;

	virtual void signal()		= 0;
	virtual void broadcast()	= 0;
};


_NDSHAREBASE_END
#endif

