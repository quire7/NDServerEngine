#include "main/data/NDDSDataManager.h"


#include "protocol/NDGameToNDData/NDGS2DS_PlayerLogin.h"

#include "database/NDTableBase.h"
#include "main/local/NDDataServer.h"

NDDSDataManager::NDDSDataManager()
{
	clear();
}

NDDSDataManager::~NDDSDataManager()
{

}


void NDDSDataManager::clear()
{
	m_waitingAccountPlayerGUIDSet.clear();
	m_waitingAccountQueue.clear();
	m_loadSuccessAccountQueue.clear();

	memset( &m_NDDSConfigBaseInfo, 0, sizeof(m_NDDSConfigBaseInfo) );
}

NDBool NDDSDataManager::isInWaitingQueue( NDUint64 nPlayerGUID )
{
	NDGuardLock guardLock( m_waitingAccountQueueLock );
	NDPlayerGUIDSetIter iterFind = m_waitingAccountPlayerGUIDSet.find( nPlayerGUID );
	if ( iterFind == m_waitingAccountPlayerGUIDSet.end() )
	{
		return NDFalse;
	}

	return NDTrue;
}

void NDDSDataManager::addWaitingQueue(const NDDSWaitingAccount& refData)
{
	NDGuardLock guardLock( m_waitingAccountQueueLock );
	m_waitingAccountPlayerGUIDSet.insert( refData.nPlayerGUID );
	m_waitingAccountQueue.push_back( refData );
}


NDBool NDDSDataManager::isWaitingQueueEmpty()
{
	NDGuardLock guardLock( m_waitingAccountQueueLock );
	return m_waitingAccountQueue.empty();
}

NDBool NDDSDataManager::removeWaitingQueue(NDDSWaitingAccount& refData)
{
	NDGuardLock guardLock( m_waitingAccountQueueLock );
	if ( m_waitingAccountQueue.empty() )
	{
		return NDFalse;
	}

	refData = m_waitingAccountQueue.front();
	m_waitingAccountQueue.pop_front();

	NDPlayerGUIDSetIter iterFind = m_waitingAccountPlayerGUIDSet.find( refData.nPlayerGUID );
	if ( iterFind != m_waitingAccountPlayerGUIDSet.end() )
	{
		m_waitingAccountPlayerGUIDSet.erase( iterFind );
	}

	return NDTrue;
}

NDBool NDDSDataManager::isLoadSuccessQueueEmpty()
{
	NDGuardLock guardLock( m_loadSuccessAccountQueueLock );
	return m_loadSuccessAccountQueue.empty();
}

void NDDSDataManager::addLoadSuccessQueue(const NDDSWaitingAccount& refData)
{
	NDGuardLock guardLock( m_loadSuccessAccountQueueLock );
	m_loadSuccessAccountQueue.push_back( refData );
}

NDBool NDDSDataManager::setOperatorData(NDTableBase* pTableBaseProduce)
{
	if ( NULL == pTableBaseProduce )
	{
		return NDFalse;
	}

	NDUint8 nOperType = pTableBaseProduce->getOperType();
	if ( nOperType <= EDataSourceOperType_SELECT || nOperType >= EDataSourceOperType_MAX )
	{	//EDataSourceOperType_SELECT操作也不走UPDATE线程;
		NDLOG_ERROR( " [NDDSDataManager::setOperatorData] pTableBaseProduce nOperType:[%u]. ", nOperType );
		return NDFalse;
	}

	NDDBThreadManager* pDBThreadManager = sNDDataServer.dbThreadManager();
	if ( NULL == pDBThreadManager )
	{
		return NDFalse;
	}

	NDUpdateDBThread* pUpdateDBThread = pDBThreadManager->getUpdateDBThread( pTableBaseProduce );
	if ( NULL == pUpdateDBThread )
	{
		return NDFalse;
	}

	pUpdateDBThread->insertOperateData( pTableBaseProduce );

	return NDTrue;
}

void NDDSDataManager::heartBeat()
{
	if ( NDFalse == isLoadSuccessQueueEmpty() )
	{
		sendLoadSuccessQueuePlayerDataToGS();
	}
	
}

NDBool NDDSDataManager::sendLoadSuccessQueuePlayerDataToGS()
{
	NDDSWaitingAccountList copyLoadSuccessAccountQueue;

	do 
	{
		NDGuardLock guardLock( m_loadSuccessAccountQueueLock );
		copyLoadSuccessAccountQueue.assign( m_loadSuccessAccountQueue.begin(), m_loadSuccessAccountQueue.end() );
		m_loadSuccessAccountQueue.clear();
	} while (0);

	NDDSPlayerManager* pNDDSPlayerManager = sNDDataServer.playerManager();
	if ( NULL == pNDDSPlayerManager )
	{
		NDLOG_ERROR( " [NDDSDataManager::sendLoadSuccessQueuePlayerDataToGS] get pNDDSPlayerManager is NULL." )
		return NDFalse;
	}

	NDDSWaitingAccountListIter iter		= copyLoadSuccessAccountQueue.begin();
	NDDSWaitingAccountListIter iterEnd	= copyLoadSuccessAccountQueue.end();
	for ( ; iter != iterEnd; ++iter )
	{
		NDDSWaitingAccount& refLoadSuccessAccount = *iter;

		NDDSPlayer* pDSPlayer = pNDDSPlayerManager->getPlayerByGUID( refLoadSuccessAccount.nPlayerGUID );
		if ( NULL == pDSPlayer )
		{
			NDLOG_ERROR( " [NDDSDataManager::sendLoadSuccessQueuePlayerDataToGS] get pDSPlayer is NULL. PlayerGUID:[%llu].", refLoadSuccessAccount.nPlayerGUID );
			continue;
		}

		NDGS2DS_PlayerLogin_Res	response;
		response.m_nPlayerGUID	= refLoadSuccessAccount.nPlayerGUID;
		response.m_nErrorCode	= eND_SRE_LOGIC_ERROR;
		if ( NDTrue == pDSPlayer->sendAllPlayerDataToGS() )
		{
			response.m_nErrorCode	= eND_SRS_OK;
		}
		pDSPlayer->sendProtocolToGS( response );
	}

	return NDTrue;
}



