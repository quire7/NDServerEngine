#include "event/event/NDBoundSlot.h"

#include "event/functor/NDSubFunctorSlot.h"

#include "event/event/NDEvent.h"

_NDSHAREBASE_BEGIN

NDShareBase::NDBoundSlot::NDBoundSlot( NDGroup group, const NDSubFunctorSlot& refNDSubFunctorSlot, NDEvent& refEvent )
: m_nGroup( group ), m_pSubFunctorSlot( new NDSubFunctorSlot(refNDSubFunctorSlot) ), m_pEvent( &refEvent )
{
}

NDBoundSlot::NDBoundSlot( const NDBoundSlot& other )
: m_nGroup( other.m_nGroup ), m_pSubFunctorSlot( other.m_pSubFunctorSlot ), m_pEvent( other.m_pEvent )
{

}

NDShareBase::NDBoundSlot::~NDBoundSlot()
{
	disconnect();
	
	if ( NULL == m_pSubFunctorSlot )
	{
		delete m_pSubFunctorSlot;
		m_pSubFunctorSlot = NULL;
	}
}


NDBool NDBoundSlot::connected() const
{
	return ( NULL != m_pSubFunctorSlot ) && m_pSubFunctorSlot->connected();
}

void NDBoundSlot::disconnect()
{
	//清理订阅者;
	if ( connected() )
	{
		m_pSubFunctorSlot->cleanup();
	}

	//从事件中取消订阅;
	if ( NULL != m_pEvent )
	{
		m_pEvent->unsubscribe(*this);
		m_pEvent = 0;
	}
}

bool NDBoundSlot::operator==( const NDBoundSlot& other ) const
{
	return m_pSubFunctorSlot == other.m_pSubFunctorSlot;
}

bool NDBoundSlot::operator!=( const NDBoundSlot& other ) const
{
	return !(*this == other);
}

NDBoundSlot& NDBoundSlot::operator=( const NDBoundSlot& other )
{
	m_nGroup			= other.m_nGroup;
	m_pSubFunctorSlot	= other.m_pSubFunctorSlot;
	m_pEvent			= other.m_pEvent;

	return *this;
}



_NDSHAREBASE_END

