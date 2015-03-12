#include "remote/NDRemoteServerManager.h"

#include <algorithm>

#include "NDShareBaseMacros.h"

#include "stl/NDSTLExtend.h"

#include "remote/NDRemoteServerInfo.h"


using NDShareBase::IsObjEqualUnary;


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

NDRemoteServerInfo* NDRemoteServerManager::getRemoteServerInfoBySessionID( NDUint32 nSessionID )
{
	RemoteServerMapIter iterFind = m_remoteServerMap.find( nSessionID );
	if ( iterFind == m_remoteServerMap.end() )
	{
		return NULL;
	}

	return iterFind->second;
}

NDRemoteServerInfo* NDRemoteServerManager::getRemoteServerInfoByServerID( NDUint16 nServerID )
{
	//for ( RemoteServerMapIter	iter = m_remoteServerMap.begin(),
	//							iterEnd  = m_remoteServerMap.end();
	//							iter != iterEnd;
	//							++iter )
	//{
	//	NDRemoteServerInfo* pRemoteServerInfo = iter->second;
	//	if ( nServerID == pRemoteServerInfo->getServerID() )
	//	{
	//		return pRemoteServerInfo;
	//	}
	//}
	//return NULL;

	RemoteServerMapIter iterFind = std::find_if(m_remoteServerMap.begin(), m_remoteServerMap.end(),
								   IsObjEqualUnary<NDUint16, std::pair<NDUint32, NDRemoteServerInfo*> >(nServerID, &NDRemoteServerInfo::getServerID));

	if ( iterFind == m_remoteServerMap.end() )
	{
		return NULL;
	}

	return iterFind->second;
}


