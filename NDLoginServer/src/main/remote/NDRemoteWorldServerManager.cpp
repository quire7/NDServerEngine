#include "main/remote/NDRemoteWorldServerManager.h"

#include "main/remote/NDRemoteWorldServerInfo.h"



NDRemoteWorldServerManager::NDRemoteWorldServerManager()
{

}

NDRemoteWorldServerManager::~NDRemoteWorldServerManager()
{

}


NDRemoteWorldServerInfo* NDRemoteWorldServerManager::getWorldByWorldID( NDUint32 nWorldID )
{
	NDRemoteWorldServerInfo* pWorld = NULL;

	RemoteServerMapIter iterFind = m_remoteServerMap.find( nWorldID );
	if ( iterFind != m_remoteServerMap.end())
	{
		pWorld = dynamic_cast<NDRemoteWorldServerInfo*>(iterFind->second);
	}

	return pWorld;
}

NDRemoteWorldServerInfo* NDRemoteWorldServerManager::getBestWorld()
{
	NDRemoteWorldServerInfo* pWorld = NULL;

	NDUint32 nFactor = 100;

	RemoteServerMapIter it = m_remoteServerMap.begin();
	for (; it != m_remoteServerMap.end(); ++it)
	{
		NDRemoteWorldServerInfo* pWorldTemp = dynamic_cast<NDRemoteWorldServerInfo*>(it->second);
		if ( NULL != pWorldTemp)
		{
			if ( pWorldTemp->getPercentLoadFactor() <= nFactor )
			{
				pWorld = pWorldTemp;
			}
		}
	}

	return pWorld;
}

NDRemoteWorldServerInfo* NDRemoteWorldServerManager::getWorldByWorldName( const char* pWorldName )
{
	NDRemoteWorldServerInfo* pWorld = NULL;

	RemoteServerMapIter iterBegin = m_remoteServerMap.begin();
	RemoteServerMapIter iterEnd	  = m_remoteServerMap.end();
	for ( ; iterBegin != iterEnd; ++iterBegin )
	{
		NDRemoteWorldServerInfo* pWorldTemp = dynamic_cast<NDRemoteWorldServerInfo*>(iterBegin->second);
		if ( NULL != pWorldTemp)
		{
			const char* pWorldNameTemp = pWorldTemp->getWorldName();
			if ( 0 == strncmp( pWorldName, pWorldNameTemp, ND_WORLDNAME_LEN ) )
			{
				pWorld = pWorldTemp;
				break;
			}
		}
	}

	return pWorld;
}

NDBool NDRemoteWorldServerManager::getAllWorldName( vector<string>& refWorldNameVec )
{
	RemoteServerMapIter iterBegin = m_remoteServerMap.begin();
	RemoteServerMapIter iterEnd	  = m_remoteServerMap.end();
	for ( ; iterBegin != iterEnd; ++iterBegin )
	{
		NDRemoteWorldServerInfo* pWorldTemp = dynamic_cast<NDRemoteWorldServerInfo*>(iterBegin->second);
		if ( NULL != pWorldTemp)
		{
			const char* pWorldNameTemp = pWorldTemp->getWorldName();
			refWorldNameVec.push_back( string(pWorldNameTemp) );
		}
	}

	if ( refWorldNameVec.empty() )
	{
		return NDFalse;
	}
	
	return NDTrue;
}


