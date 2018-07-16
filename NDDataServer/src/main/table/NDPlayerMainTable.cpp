#include "main/table/NDPlayerMainTable.h"

#include "database/NDField.h"
#include "database/NDMysqlBaseOp.h"

#include "main/local/NDDataServer.h"

using NDShareBase::NDField;
using NDShareBase::NDSelectResultType;


static NDPlayerMain	s_NDPlayerMain;
static const NDSelectResultType s_NDPlayerMainSelectResultType[] =
{
	{ NDPlayerMainTableField_PlayerGUID,		NDShareBase::FIELD_UI64,		0,									(NDUint64)(&s_NDPlayerMain.m_nPlayerGUID) - (NDUint64)(&s_NDPlayerMain.m_nOperType) },
	{ NDPlayerMainTableField_PlayerName,		NDShareBase::FIELD_STRING,		NDPLAYERMAIN_TABLE_PLAYERNAME_LEN,	(NDUint64)(s_NDPlayerMain.m_szPlayerName) - (NDUint64)(&s_NDPlayerMain.m_nOperType) },
	{ NDPlayerMainTableField_PlayerLevel,		NDShareBase::FIELD_UI32,		0,									(NDUint64)(&s_NDPlayerMain.m_nPlayerLevel) - (NDUint64)(&s_NDPlayerMain.m_nOperType) },
	{ NDPlayerMainTableField_PlayerExp,			NDShareBase::FIELD_UI32,		0,									(NDUint64)(&s_NDPlayerMain.m_nPlayerExp) - (NDUint64)(&s_NDPlayerMain.m_nOperType) },
	{ NDPlayerMainTableField_Gold,				NDShareBase::FIELD_UI32,		0,									(NDUint64)(&s_NDPlayerMain.m_nGold) - (NDUint64)(&s_NDPlayerMain.m_nOperType) },
	{ NDPlayerMainTableField_Silver,			NDShareBase::FIELD_UI32,		0,									(NDUint64)(&s_NDPlayerMain.m_nSilver) - (NDUint64)(&s_NDPlayerMain.m_nOperType) },
	{ NDPlayerMainTableField_Copper,			NDShareBase::FIELD_UI32,		0,									(NDUint64)(&s_NDPlayerMain.m_nCopper) - (NDUint64)(&s_NDPlayerMain.m_nOperType) },
};

string NDPlayerMainTable::getQueryString(EDataSourceOperType nOperType)
{
	if ( nOperType <= EDataSourceOperType_NULL || nOperType >= EDataSourceOperType_MAX )
	{
		NDLOG_ERROR( " NDPlayerMainTable::getQueryString nOperType is error. " )
		return string("");
	}

	char szSql[BUF_LEN_512] = {0};
	switch ( nOperType )
	{
	case EDataSourceOperType_SELECT:
		{
			ND_SNPRINTF( szSql, sizeof(szSql) - 1, "call sp_player_main_select_by_guid('%llu')", m_playerMain.m_nPlayerGUID );
		}
		break;
	case EDataSourceOperType_INSERT:
	case EDataSourceOperType_UPDATE:
		{
			ND_SNPRINTF( szSql, sizeof(szSql) - 1, "call sp_player_main_insert_update('%llu','%s','%u','%u','%u','%u','%u')", 
						m_playerMain.m_nPlayerGUID,
						m_playerMain.m_szPlayerName,
						m_playerMain.m_nPlayerLevel,
						m_playerMain.m_nPlayerExp,
						m_playerMain.m_nGold,
						m_playerMain.m_nSilver,
						m_playerMain.m_nCopper);
		}
		break;
	case EDataSourceOperType_DELETE:
		{
			ND_SNPRINTF( szSql, sizeof(szSql) - 1, "call sp_player_main_delete('%llu')", m_playerMain.m_nPlayerGUID );
		}
		break;
	}

	return string(szSql);
}

NDBool NDPlayerMainTable::getQueryResult(NDMysqlQueryResult& refQueryResult)
{
	NDUint32 nRowCount = refQueryResult.getRowCount();
	if ( nRowCount == 1 )
	{
		while( refQueryResult.nextRow() )
		{
			NDField* pField	= refQueryResult.fetchRecord();
			if ( NULL == pField )
			{
				break;
			}
			
			NDPlayerMain		playerMain;
			playerMain.m_nOperType = EDataSourceOperType_SELECT;

			NDUint8 nMemberNum = sizeof(s_NDPlayerMainSelectResultType)/sizeof(s_NDPlayerMainSelectResultType[0]);
			if ( NDFalse == NDMysqlQueryResult::ParseSelectResultToStruct( &playerMain, s_NDPlayerMainSelectResultType, nMemberNum, pField ) )
			{
				NDLOG_ERROR( " [NDPlayerMainTable::getQueryResult] NDMysqlQueryResult::ParseSelectResultToStruct parse is error. PlayerGUID:[%llu].", m_playerMain.m_nPlayerGUID );
				break;
			}

			NDDSPlayerManager* pNDDSPlayerManager = sNDDataServer.playerManager();
			if ( NULL == pNDDSPlayerManager )
			{
				break;
			}

			//pNDDSPlayerManager->addPlayer( playerMain );
			NDDSPlayer* pDSPlayer = pNDDSPlayerManager->getPlayerByGUID( m_playerMain.m_nPlayerGUID );
			if ( NULL == pDSPlayer )
			{
				NDLOG_ERROR( " [NDPlayerMainTable::getQueryResult] get pDSPlayer is NULL. PlayerGUID:[%llu].", m_playerMain.m_nPlayerGUID );
				break;
			}

			pDSPlayer->insertUpdatePlayerMain( playerMain );
		}

		return NDTrue;
	}

	NDLOG_ERROR( " [NDPlayerMainTable::getQueryResult] QueryResult RowCount:[%u]. PlayerGUID:[%llu].", nRowCount, m_playerMain.m_nPlayerGUID );
	return NDFalse;
}
