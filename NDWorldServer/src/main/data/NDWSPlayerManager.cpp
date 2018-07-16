#include "main/data/NDWSPlayerManager.h"


NDWSPlayerManager::NDWSPlayerManager()
{
	clear();
}

NDWSPlayerManager::~NDWSPlayerManager()
{
	release();
	clear();
}



void NDWSPlayerManager::clear()
{
	m_NDWSPlayerMap.clear();
}

void NDWSPlayerManager::release()
{
	NDGuardLock guardLock( m_mutexLock );
	NDWSPlayerMapIter iter		= m_NDWSPlayerMap.begin();
	NDWSPlayerMapIter iterEnd	= m_NDWSPlayerMap.end();
	for ( ; iter != iterEnd; ++iter )
	{
		if ( NULL != iter->second )
		{
			NDMemoryPoolExManager::getInstance()->freeMemory( iter->second );
		}
	}
	m_NDWSPlayerMap.clear();
}

NDBool NDWSPlayerManager::init()
{
	return NDTrue;
}


NDWSPlayer* NDWSPlayerManager::addPlayer(const NDPlayerMain& refPlayerMain)
{
	NDWSPlayer* pPlayer = getPlayerByGUID( refPlayerMain.m_nPlayerGUID );
	if ( NULL != pPlayer )
	{
		NDLOG_ERROR( " [NDWSPlayerManager::addPlayer] User Already Exits. PlayerGUID:[%llu].", refPlayerMain.m_nPlayerGUID );
		return NULL;
	}

	
	NDMemoryPoolExManager::getInstance()->mallocMemory( pPlayer );
	if ( NULL == pPlayer )
	{
		return NULL;
	}

	pPlayer->init( refPlayerMain );
	
	NDGuardLock guardLock( m_mutexLock );
	m_NDWSPlayerMap.insert( std::make_pair( refPlayerMain.m_nPlayerGUID, pPlayer ) );

	return pPlayer;
}

void NDWSPlayerManager::removePlayer(NDUint64 nPlayerGUID)
{
	NDGuardLock guardLock( m_mutexLock );
	NDWSPlayerMapIter iterFind = m_NDWSPlayerMap.find( nPlayerGUID );
	if ( iterFind != m_NDWSPlayerMap.end() )
	{
		NDMemoryPoolExManager::getInstance()->freeMemory( iterFind->second );
		m_NDWSPlayerMap.erase( iterFind );
	}
}

NDWSPlayer* NDWSPlayerManager::getPlayerByGUID(NDUint64 nPlayerGUID)
{
	NDGuardLock guardLock( m_mutexLock );	//这个可以不要,应为WS加载完后是单线程的;
	NDWSPlayerMapIter iterFind = m_NDWSPlayerMap.find( nPlayerGUID );
	if ( iterFind == m_NDWSPlayerMap.end() )
	{
		return NULL;
	}

	return iterFind->second;
}
