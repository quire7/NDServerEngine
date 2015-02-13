#ifndef __SHARE_BASE_ND_WINDOWS_CONDITION_IMPL_H__
#define __SHARE_BASE_ND_WINDOWS_CONDITION_IMPL_H__

#include <deque>
using std::deque;

#include "NDWindowsCommon.h"
#include "thread/NDConditionImpl.h"

_NDSHAREBASE_BEGIN

class NDWindowsConditionImpl : public NDConditionImpl
{
private:
	NDUint32			m_nLockCount;
	CRITICAL_SECTION	m_critsecExternalProtection;	//�ⲿ���ݵ��ٽ���;
	CRITICAL_SECTION	m_critsecWaitSetProtection;		//�ڲ�m_deqWaitSet���ٽ���;
	deque<HANDLE>		m_deqWaitSet;

public:
	explicit NDWindowsConditionImpl();
	~NDWindowsConditionImpl();

	void		beginSynchronized();
	void		endSynchronized();

	NDBool		wait();
	NDBool		wait( NDUint32 nMilliSecondsTimeOut );

	void		signal();
	void		broadcast();

private:
	HANDLE		push();
	HANDLE		pop();
};


_NDSHAREBASE_END
#endif

