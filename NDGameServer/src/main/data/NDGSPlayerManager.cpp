#include "main/data/NDGSPlayerManager.h"



#include "main/data/NDGSPlayer.h"

NDGSPlayerManager::NDGSPlayerManager()
{
	clear();
}

NDGSPlayerManager::~NDGSPlayerManager()
{
	release();
	clear();
}

void NDGSPlayerManager::clear()
{
	m_pWSServerInfo	= NULL;
	m_NDGSPlayerMap.clear();
}


void NDGSPlayerManager::release()
{
	NDGSPlayerMapIter iter		= m_NDGSPlayerMap.begin();
	NDGSPlayerMapIter iterEnd	= m_NDGSPlayerMap.end();
	for ( ; iter != iterEnd; ++iter )
	{
		NDGSPlayer* pPlayer = iter->second;
		if ( NULL == pPlayer )
		{
			continue;
		}

		NDMemoryPoolExManager::getInstance()->freeMemory( pPlayer );
	}
}

NDBool NDGSPlayerManager::init()
{
	return NDTrue;
}

NDBool NDGSPlayerManager::addPlayer(NDGSPlayer* pPlayer)
{
	if ( NULL == pPlayer )
	{
		return NDFalse;
	}

	m_NDGSPlayerMap.insert( std::make_pair( pPlayer->getPlayerGUID(), pPlayer ) );

	return NDTrue;
}

NDBool NDGSPlayerManager::removePlayer(NDGSPlayer* pPlayer)
{
	if ( NULL == pPlayer )
	{
		return NDFalse;
	}

	m_NDGSPlayerMap.erase( pPlayer->getPlayerGUID() );

	NDMemoryPoolExManager::getInstance()->freeMemory( pPlayer );

	return NDTrue;
}

NDGSPlayer* NDGSPlayerManager::getPlayer(NDUint64 nPlayerGUID)
{
	NDGSPlayerMapIter iterFind = m_NDGSPlayerMap.find( nPlayerGUID );
	if ( iterFind == m_NDGSPlayerMap.end() )
	{
		return NULL;
	}

	return iterFind->second;
}

NDUint32 NDGSPlayerManager::getWSServerID() const
{
	if ( NULL == m_pWSServerInfo )
	{
		return ND_INVALID_ID;
	}
	return m_pWSServerInfo->getServerID();
}

NDUint32 NDGSPlayerManager::getWSServerSessionID() const
{
	if ( NULL == m_pWSServerInfo )
	{
		return ND_INVALID_ID;
	}
	return m_pWSServerInfo->getSessionID();
}

NDBool NDGSPlayerManager::sendProtocolToWS(NDProtocol& refProtocol)
{
	NDUint32 nWSServerSessionID = getWSServerSessionID();
	if ( ND_INVALID_ID == nWSServerSessionID )
	{
		return NDFalse;
	}

	return NDServerManager::getSingleton().sendToServer( refProtocol, nWSServerSessionID );
}
