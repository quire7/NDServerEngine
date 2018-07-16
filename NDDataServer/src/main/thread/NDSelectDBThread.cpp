#include "main/thread/NDSelectDBThread.h"

#include "main/local/NDDataServer.h"
#include "main/data/NDDSDataStructs.h"

#include "main/table/NDPlayerAccountTable.h"
#include "main/table/NDPlayerMainTable.h"

NDSelectDBThread::NDSelectDBThread()
{
		
}

NDSelectDBThread::~NDSelectDBThread()
{

}

NDBool NDSelectDBThread::disposeOperateData()
{
	NDDSDataManager* pNDDSDataManager = sNDDataServer.dataManager();
	if ( NULL == pNDDSDataManager )
	{
		return NDFalse;
	}

	NDDSWaitingAccount waitingAccountData;
	while (true)
	{
		if ( NULL == pNDDSDataManager )
		{
			break;
		}

		memset( &waitingAccountData, 0, sizeof(waitingAccountData) );
		if ( NDFalse == pNDDSDataManager->removeWaitingQueue( waitingAccountData ) )
		{
			break;
		}
		
		//加载一个玩家的所有数据;
		if ( NDFalse == loadOnePlayerInfo( waitingAccountData ) )
		{
			continue;
		}

		pNDDSDataManager->addLoadSuccessQueue( waitingAccountData );

		
		NDLOG_INFO( " DS Load player data success! PlayerGUID:[%llu].", waitingAccountData.nPlayerGUID );
	}
	return NDTrue;
}

NDBool NDSelectDBThread::isOperateDataEmpty()
{
	NDDSDataManager* pNDDSDataManager = sNDDataServer.dataManager();
	if ( NULL == pNDDSDataManager )
	{
		return NDFalse;
	}
	return pNDDSDataManager->isWaitingQueueEmpty();
}

NDBool NDSelectDBThread::isQuitDBThread()
{
	return NDFalse;
}

NDBool NDSelectDBThread::loadOnePlayerInfo( const NDDSWaitingAccount& refWaitingAccount )
{
	if ( NULL == m_pNDMysqlBaseOp )
	{
		NDLOG_ERROR( " NDSelectDBThread::loadOnePlayerInfo m_pNDMysqlBaseOp is NULL.")
		return NDFalse;
	}
	//loading player_account table;
	NDPlayerAccountTable playerAccountTable;
	playerAccountTable.m_playerAccount.m_nPlayerGUID = refWaitingAccount.nPlayerGUID;
	if ( NDFalse == playerAccountTable.onSelect( m_pNDMysqlBaseOp ) )
	{
		NDLOG_ERROR( " load PlayerAccount table error! PlayerGUID:[%llu].", refWaitingAccount.nPlayerGUID );
		return NDFalse;
	}
	
	//loading player_account table;
	NDPlayerMainTable playerMainTable;
	playerMainTable.m_playerMain.m_nPlayerGUID = refWaitingAccount.nPlayerGUID;
	if ( NDFalse == playerMainTable.onSelect( m_pNDMysqlBaseOp ) )
	{
		NDLOG_ERROR( " load PlayerMain table error! PlayerGUID:[%llu].", refWaitingAccount.nPlayerGUID );
		return NDFalse;
	}

	//加载数据完毕后,设置GS等相应数据;
	NDDSPlayerManager* pNDDSPlayerManager = sNDDataServer.playerManager();
	if ( NULL == pNDDSPlayerManager )
	{
		NDLOG_ERROR( " [NDSelectDBThread::loadOnePlayerInfo] get pNDDSPlayerManager is NULL. PlayerGUID:[%llu].", refWaitingAccount.nPlayerGUID );
		return NDFalse;
	}
	NDDSPlayer* pDSPlayer = pNDDSPlayerManager->getPlayerByGUID( refWaitingAccount.nPlayerGUID );
	if ( NULL == pDSPlayer )
	{
		NDLOG_ERROR( " [NDSelectDBThread::loadOnePlayerInfo] get pDSPlayer is NULL. PlayerGUID:[%llu].", refWaitingAccount.nPlayerGUID );
		return NDFalse;
	}
	pDSPlayer->setGSServerID( refWaitingAccount.nGameServerID );

	return NDTrue;
}

