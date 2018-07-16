
#include "main/data/NDGTWSPlayerManager.h"


#include "main/data/NDGTWSPlayer.h"

NDGTWSPlayerManager::NDGTWSPlayerManager()
{

}

NDGTWSPlayerManager::~NDGTWSPlayerManager()
{

}

void NDGTWSPlayerManager::clear()
{
	m_PlayerDeque.clear();
	m_PlayerMap.clear();
}

void NDGTWSPlayerManager::release()
{
	PlayerDequeIter dequeIter	= m_PlayerDeque.begin();
	PlayerDequeIter dequeEnd	= m_PlayerDeque.end();
	for ( ; dequeIter != dequeEnd; ++dequeIter )
	{
		NDGTWSPlayer* pPlayer = *dequeIter;
		if ( NULL == pPlayer )
		{
			continue;
		}
		SAFE_DELETE( pPlayer )
	}

	PlayerMapIter iter		= m_PlayerMap.begin();
	PlayerMapIter iterEnd	= m_PlayerMap.end();
	for ( ; iter != iterEnd; ++iter )
	{
		NDGTWSPlayer* pPlayer = iter->second;
		if ( NULL == pPlayer )
		{
			continue;
		}

		SAFE_DELETE( pPlayer )
	}
}

NDGTWSPlayer* NDGTWSPlayerManager::getIdlePlayer()
{
	NDGTWSPlayer* pPlayer = NULL;

	if ( m_PlayerDeque.empty() )
	{
		pPlayer = new NDGTWSPlayer;
	}
	else
	{
		pPlayer = m_PlayerDeque.front();
		m_PlayerDeque.pop_front();
	}

	return pPlayer;
}

NDBool NDGTWSPlayerManager::addPlayer(NDGTWSPlayer* pPlayer)
{
	NDBool bRet				= NDFalse;
	NDUint64 nPlayerGUID	= pPlayer->getPlayerGUID();
	PlayerMapIter iterFind	= m_PlayerMap.find( nPlayerGUID );
	if ( iterFind == m_PlayerMap.end() )
	{
		m_PlayerMap.insert( std::make_pair( nPlayerGUID, pPlayer ) );
		bRet = NDTrue;
	}

	return bRet;
}

NDBool NDGTWSPlayerManager::removePlayerByPlayerGUID(NDUint64 nPlayerGUID)
{
	PlayerMapIter iterFind	= m_PlayerMap.find( nPlayerGUID );
	if ( iterFind == m_PlayerMap.end() )
	{
		return NDFalse;
	}

	NDGTWSPlayer* pPlayer = iterFind->second;
	if ( NULL != pPlayer )
	{
		pPlayer->clear();
		m_PlayerDeque.push_back( pPlayer );
	}

	m_PlayerMap.erase( iterFind );

	return NDTrue;
}

NDBool NDGTWSPlayerManager::removePlayerBySessionID(NDUint32 nSessionID)
{
	PlayerMapIter iterFind = std::find_if( m_PlayerMap.begin(), m_PlayerMap.end(), 
		IsObjEqualUnary< NDUint32, std::pair<NDUint64, NDGTWSPlayer*> >(nSessionID, &NDGTWSPlayer::getClientSessionID) );
	if ( iterFind == m_PlayerMap.end() )
	{
		return NDFalse;
	}

	NDGTWSPlayer* pPlayer = iterFind->second;
	if ( NULL != pPlayer )
	{
		pPlayer->clear();
		m_PlayerDeque.push_back( pPlayer );
	}

	m_PlayerMap.erase( iterFind );

	return NDTrue;
}

NDGTWSPlayer* NDGTWSPlayerManager::getPlayerByPlayerGUID(NDUint64 nPlayerGUID)
{
	PlayerMapIter iterFind	= m_PlayerMap.find( nPlayerGUID );
	if ( iterFind == m_PlayerMap.end() )
	{
		return NULL;
	}
	return iterFind->second;
}

NDGTWSPlayer* NDGTWSPlayerManager::getPlayerBySessionID(NDUint32 nSessionID)
{
	PlayerMapIter iterFind = std::find_if( m_PlayerMap.begin(), m_PlayerMap.end(), 
		IsObjEqualUnary< NDUint32, std::pair<NDUint64, NDGTWSPlayer*> >(nSessionID, &NDGTWSPlayer::getClientSessionID) );
	if ( iterFind == m_PlayerMap.end() )
	{
		return NULL;
	}

	return iterFind->second;
}
