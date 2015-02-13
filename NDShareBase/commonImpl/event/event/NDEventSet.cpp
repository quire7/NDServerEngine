#include "event/event/NDEventSet.h"

#include "event/functor/NDSubFunctorSlot.h"

_NDSHAREBASE_BEGIN

NDEventSet::NDEventSet() : m_bMuted( NDFalse )
{

}

NDEventSet::~NDEventSet()
{
	removeAllEvents();
}

void NDEventSet::addEvent( const char* name )
{
	if ( isEventPresent( name ) )
	{
		return;
	}

	m_Events.insert( std::make_pair( (char*)name, new NDEvent( name ) ) );
}

void NDEventSet::removeEvent( const char* name )
{
	NDEventMapIter iterFind = m_Events.find( (char*)name );
	if ( iterFind != m_Events.end() )
	{
		delete iterFind->second;
		m_Events.erase( iterFind );
	}
}

void NDEventSet::removeAllEvents()
{
	NDEventMapIter iterPos = m_Events.begin();
	NDEventMapIter iterEnd = m_Events.end();
	for (; iterPos != iterEnd; ++iterPos )
	{
		delete iterPos->second;
	}

	m_Events.clear();
}

bool NDEventSet::isEventPresent( const char* name )
{
	return ( m_Events.find( (char*)name ) != m_Events.end() );
}

NDEvent::NDConnection NDEventSet::subscribeEvent( const char* name, NDSubFunctorSlot subscriber )
{
	return getEventObject( name, NDTrue )->subscribe( subscriber );
}

NDEvent::NDConnection NDEventSet::subscribeEvent( const char* name, NDGroup group, NDSubFunctorSlot subscriber )
{
	return getEventObject( name, NDTrue )->subscribe( group, subscriber );
}

void NDEventSet::fireEvent( const char* name, NDEventArgs& args, const char* eventNamespace /*= "" */ )
{
	//处理全局事件;
	eventNamespace;
	//处理本地事件;
	fireEvent_impl( name, args );
}

NDBool NDEventSet::isMuted() const
{
	return m_bMuted;
}

void NDEventSet::setMutedState( NDBool setting )
{
	m_bMuted = setting;
}

NDEvent* NDEventSet::getEventObject( const char* name, NDBool autoAdd /*= NDFalse */ )
{
	NDEventMapIter iterPos = m_Events.find( (char*)name );
	if ( iterPos == m_Events.end() )
	{
		if ( NDFalse == autoAdd )
		{
			return 0;
		}

		addEvent( name );

		iterPos = m_Events.find( (char*)name );
	}

	return iterPos->second;
}

void NDEventSet::fireEvent_impl( const char* name, NDEventArgs& args )
{
	//查找事件;
	NDEvent* pEvent = getEventObject( name );

	//发送事件,当事件存在,并且不静音的时候;
	if ( ( 0 != pEvent ) && ( NDFalse == m_bMuted ) )
	{
		(*pEvent)(args);
	}
}



_NDSHAREBASE_END

