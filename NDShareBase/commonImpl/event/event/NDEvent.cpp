#include "event/event/NDEvent.h"

#include <algorithm>

#include "event/functor/NDEventArgs.h"
#include "event/functor/NDSubFunctorSlot.h"

_NDSHAREBASE_BEGIN

class SubComp
{
public:
	SubComp(const NDBoundSlot& s) : d_s(s) {}

	bool operator()(std::pair<NDGroup, NDEvent::NDConnection> e) const
	{
		return *(e.second) == d_s;
	}

private:
	void operator=(const SubComp&) {}
	const NDBoundSlot& d_s;
};

NDEvent::NDEvent( const char* szName ): m_name( szName )
{

}

NDEvent::~NDEvent()
{
	SlotContainer::iterator iter(m_slots.begin());
	const SlotContainer::const_iterator end_iter(m_slots.end());

	for (; iter != end_iter; ++iter)
	{
		iter->second->m_pEvent = 0;
		iter->second->m_pSubFunctorSlot->cleanup();
	}

	m_slots.clear();
}

NDEvent::NDConnection NDEvent::subscribe( const NDSubFunctorSlot& slot )
{
	return subscribe( static_cast<NDGroup>(-1), slot );
}

NDEvent::NDConnection NDEvent::subscribe( NDGroup group, const NDSubFunctorSlot& slot )
{
	NDEvent::NDConnection c( new NDBoundSlot( group, slot, *this ) );
	m_slots.insert( std::pair<NDGroup, NDConnection>( group, c) );
	return c;
}

void NDEvent::operator()( NDEventArgs& args )
{
	SlotContainer::iterator iter(m_slots.begin());
	const SlotContainer::const_iterator end_iter(m_slots.end());

	for (; iter != end_iter; ++iter)
	{
		if ( (*iter->second->m_pSubFunctorSlot)(args) )
		{
			++args.m_nhandled;
		}
	}
}

void NDEvent::unsubscribe( const NDBoundSlot& slot )
{
	SlotContainer::iterator curr =
		std::find_if(m_slots.begin(),
		m_slots.end(),
		SubComp(slot));

	if (curr != m_slots.end())
	{
		m_slots.erase(curr);
	}
}



_NDSHAREBASE_END

