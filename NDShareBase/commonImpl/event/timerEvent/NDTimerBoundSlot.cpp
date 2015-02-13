#include "event/timerEvent/NDTimerBoundSlot.h"

#include "NDShareBaseMacros.h"
#include "NDShareBaseGlobal.h"

#include "event/functor/NDSubFunctorSlot.h"

_NDSHAREBASE_BEGIN

NDTimerBoundSlot::NDTimerBoundSlot( const NDSubFunctorSlot& refSubFunctorSlot, const NDTimerEventArgs& refTimerEventArgs )
: m_pSubscriber( new NDSubFunctorSlot(refSubFunctorSlot) ), m_NDTimerEventArgs( refTimerEventArgs )
{
}

NDTimerBoundSlot::NDTimerBoundSlot( const NDTimerBoundSlot& other )
: m_pSubscriber( other.m_pSubscriber ), m_NDTimerEventArgs( other.m_NDTimerEventArgs )
{

}

NDTimerBoundSlot::~NDTimerBoundSlot()
{
	disconnect();
	SAFE_DELETE( m_pSubscriber );
}

NDBool NDTimerBoundSlot::connected() const
{
	return ( NULL != m_pSubscriber ) && m_pSubscriber->connected();
}

void NDTimerBoundSlot::disconnect()
{
	// cleanup the bound subscriber functor;
	if ( connected() )
		m_pSubscriber->cleanup();
}

bool NDTimerBoundSlot::operator==( const NDTimerBoundSlot& other ) const
{
	return m_pSubscriber == other.m_pSubscriber;
}

bool NDTimerBoundSlot::operator!=( const NDTimerBoundSlot& other ) const
{
	return m_pSubscriber != other.m_pSubscriber;
}

NDBool NDTimerBoundSlot::isLoopTimerEvent() const
{
	return m_NDTimerEventArgs.m_nStepMSTimes != 0;
}


void NDTimerBoundSlot::modifyFireEventTimes()
{
	m_NDTimerEventArgs.m_nProcessMSTimes += m_NDTimerEventArgs.m_nStepMSTimes;
}


NDTime NDTimerBoundSlot::getFireEventTimes() const
{
	return m_NDTimerEventArgs.m_nProcessMSTimes;
}

NDBool NDTimerBoundSlot::fireTimerEvent()
{
	if ( NDFalse == connected() )
	{
		return NDFalse;
	}

	++m_NDTimerEventArgs.m_nhandled;

	return (*m_pSubscriber)(m_NDTimerEventArgs);
}




_NDSHAREBASE_END



