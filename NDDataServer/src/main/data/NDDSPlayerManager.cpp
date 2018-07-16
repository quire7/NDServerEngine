#include "main/data/NDDSPlayerManager.h"

NDDSPlayerManager::NDDSPlayerManager() : m_nOnlineNum( 0 )
{
	
}

NDDSPlayerManager::~NDDSPlayerManager()
{
	release();
}


NDBool NDDSPlayerManager::init()
{
	return NDTrue;
}

void NDDSPlayerManager::release()
{
	NDGuardLock guardLock( m_mutexLock );
	NDDSPlayerMapIter iter		= m_NDDSPlayerMap.begin();
	NDDSPlayerMapIter iterEnd	= m_NDDSPlayerMap.end();
	for ( ; iter != iterEnd; ++iter )
	{
		if ( NULL != iter->second )
		{
			NDMemoryPoolExManager::getInstance()->freeMemory( iter->second );
		}
	}
	m_NDDSPlayerMap.clear();
}


NDDSPlayer* NDDSPlayerManager::addPlayer(const NDPlayerAccount& refPlayerAccount)
{
	NDDSPlayer* pPlayer = getPlayerByGUID( refPlayerAccount.m_nPlayerGUID );
	if ( NULL != pPlayer )
	{
		NDLOG_ERROR( " [NDDSPlayerManager::addPlayer] User Already Exits. PlayerGUID:[%llu].", refPlayerAccount.m_nPlayerGUID );
		return NULL;
	}

	NDMemoryPoolExManager::getInstance()->mallocMemory( pPlayer );
	if ( NULL == pPlayer )
	{
		return NULL;
	}

	pPlayer->init();
	pPlayer->insertUpdatePlayerAccount( refPlayerAccount );

	NDGuardLock guardLock( m_mutexLock );
	m_NDDSPlayerMap.insert( std::make_pair( refPlayerAccount.m_nPlayerGUID, pPlayer ) );

	return pPlayer;
}

void NDDSPlayerManager::removePlayer(NDUint64 nPlayerGUID)
{
	NDGuardLock guardLock( m_mutexLock );
	NDDSPlayerMapIter iterFind = m_NDDSPlayerMap.find( nPlayerGUID );
	if ( iterFind != m_NDDSPlayerMap.end() )
	{
		NDMemoryPoolExManager::getInstance()->freeMemory( iterFind->second );
		m_NDDSPlayerMap.erase( iterFind );
	}
}

NDDSPlayer* NDDSPlayerManager::getPlayerByGUID(NDUint64 nPlayerGUID)
{
	NDGuardLock guardLock( m_mutexLock );
	NDDSPlayerMapIter iterFind = m_NDDSPlayerMap.find( nPlayerGUID );
	if ( iterFind == m_NDDSPlayerMap.end() )
	{
		return NULL;
	}

	return iterFind->second;
}

void NDDSPlayerManager::increaseOnlineNum()
{
	++m_nOnlineNum;
}

void NDDSPlayerManager::decreaseOnlineNum()
{
	if ( m_nOnlineNum > 0 )
	{
		--m_nOnlineNum;
	}
}
