#include "thread/NDCondition.h"


#ifdef WIN32
#include "../windows/thread/NDWindowsConditionImpl.h"
#else
#include "../linux/thread/NDLinuxConditionImpl.h"
#endif


_NDSHAREBASE_BEGIN

NDCondition::NDCondition()
{
#ifdef  WIN32
	m_pNDConditionImpl = new NDWindowsConditionImpl();
#else
	m_pNDConditionImpl = new NDLinuxConditionImpl();
#endif
}

NDCondition::~NDCondition()
{
	if ( NULL != m_pNDConditionImpl )
	{
		delete m_pNDConditionImpl;
		m_pNDConditionImpl = NULL;
	}
}

void NDCondition::beginSynchronized()
{
	if ( NULL == m_pNDConditionImpl ) return ;
	
	m_pNDConditionImpl->beginSynchronized();
}

void NDCondition::endSynchronized()
{
	if ( NULL == m_pNDConditionImpl ) return ;

	m_pNDConditionImpl->endSynchronized();
}

NDBool NDCondition::wait()
{
	if ( NULL == m_pNDConditionImpl ) return NDFalse;

	return m_pNDConditionImpl->wait();
}

NDBool NDCondition::wait( NDUint32 nMilliSecondsTimeOut )
{
	if ( NULL == m_pNDConditionImpl ) return NDFalse;

	return m_pNDConditionImpl->wait( nMilliSecondsTimeOut );
}

void NDCondition::signal()
{
	if ( NULL == m_pNDConditionImpl ) return ;

	m_pNDConditionImpl->signal();
}

void NDCondition::broadcast()
{
	if ( NULL == m_pNDConditionImpl ) return ;

	m_pNDConditionImpl->broadcast();
}

_NDSHAREBASE_END

