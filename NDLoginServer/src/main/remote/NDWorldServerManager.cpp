#include "main/remote/NDWorldServerManager.h"

#include "main/remote/NDWorldServerInfo.h"



NDWorldServerManager::NDWorldServerManager()
{

}

NDWorldServerManager::~NDWorldServerManager()
{

}


NDWorldServerInfo* NDWorldServerManager::getWorldByWorldID( NDUint32 nWorldID )
{
	NDWorldServerInfo* pWorld = NULL;

	RemoteServerMapIter iterFind = m_remoteServerMap.find( nWorldID );
	if ( iterFind != m_remoteServerMap.end())
	{
		pWorld = dynamic_cast<NDWorldServerInfo*>(iterFind->second);
	}

	return pWorld;
}

NDWorldServerInfo* NDWorldServerManager::getBestWorld()
{
	NDWorldServerInfo* pWorld = NULL;

	NDUint32 nFactor = 100;

	RemoteServerMapIter it = m_remoteServerMap.begin();
	for (; it != m_remoteServerMap.end(); ++it)
	{
		NDWorldServerInfo* pWorldTemp = dynamic_cast<NDWorldServerInfo*>(it->second);
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

NDWorldServerInfo* NDWorldServerManager::getWorldByWorldName( const char* pWorldName )
{
	NDWorldServerInfo* pWorld = NULL;

	RemoteServerMapIter iterBegin = m_remoteServerMap.begin();
	RemoteServerMapIter iterEnd	  = m_remoteServerMap.end();
	for ( ; iterBegin != iterEnd; ++iterBegin )
	{
		NDWorldServerInfo* pWorldTemp = dynamic_cast<NDWorldServerInfo*>(iterBegin->second);
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

NDBool NDWorldServerManager::getAllWorldName( vector<string>& refWorldNameVec )
{
	RemoteServerMapIter iterBegin = m_remoteServerMap.begin();
	RemoteServerMapIter iterEnd	  = m_remoteServerMap.end();
	for ( ; iterBegin != iterEnd; ++iterBegin )
	{
		NDWorldServerInfo* pWorldTemp = dynamic_cast<NDWorldServerInfo*>(iterBegin->second);
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


