#include "remote/NDRemoteServerManager.h"

#include "NDShareBaseMacros.h"

#include "remote/NDRemoteServerInfo.h"


NDRemoteServerManager::NDRemoteServerManager(void)
{
	m_remoteServerMap.clear();
}

NDRemoteServerManager::~NDRemoteServerManager(void)
{
	RemoteServerMapIter rsmIterBegin =  m_remoteServerMap.begin();
	RemoteServerMapIter rsmIterEnd	 =  m_remoteServerMap.end();
	for ( ; rsmIterBegin != rsmIterEnd; ++rsmIterBegin )
	{
		NDRemoteServerInfo* pRemoteServerInfo = rsmIterBegin->second;
		SAFE_DELETE( pRemoteServerInfo );
	}
	m_remoteServerMap.clear();
}

NDBool NDRemoteServerManager::addRemoteServer( NDRemoteServerInfo* pRemoteServerInfo )
{
	RemoteServerMapIter iterFind = m_remoteServerMap.find( pRemoteServerInfo->getSessionID() );
	if ( iterFind == m_remoteServerMap.end() )
	{
		m_remoteServerMap.insert( std::make_pair( pRemoteServerInfo->getSessionID(), pRemoteServerInfo ) );
		return NDTrue;
	}

	return NDFalse;
}

NDBool	NDRemoteServerManager::removeRemoteServer( NDUint32 nSessionID )
{
	RemoteServerMapIter iterFind = m_remoteServerMap.find( nSessionID );
	if ( iterFind != m_remoteServerMap.end() )
	{
		NDRemoteServerInfo* pRemoteServerInfo = iterFind->second;
		SAFE_DELETE( pRemoteServerInfo )
		iterFind->second = NULL;
		
		m_remoteServerMap.erase( iterFind );
		return NDTrue;
	}

	return NDFalse;	
}

NDRemoteServerInfo* NDRemoteServerManager::getRemoteServerInfo( NDUint32 nSessionID )
{
	RemoteServerMapIter iterFind = m_remoteServerMap.find( nSessionID );
	if ( iterFind == m_remoteServerMap.end() )
	{
		return NULL;
	}

	return iterFind->second;
}


