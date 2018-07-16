#include "main/table/NDPlayerMainSelectAllTable.h"

#include "database/NDField.h"
#include "database/NDMysqlBaseOp.h"

#include "main/local/NDWorldServer.h"

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

string NDPlayerMainSelectAllTable::getQueryString()
{
	return string("call sp_player_main_select_all()");
}

NDBool NDPlayerMainSelectAllTable::getQueryResult(NDMysqlQueryResult& refQueryResult)
{
	NDWSPlayerManager* pNDWSPlayerManager = sNDWorldServer.playerManager();
	if ( NULL == pNDWSPlayerManager )
	{	
		NDLOG_ERROR( " [NDPlayerMainSelectAllTable::getQueryResult] pNDWSPlayerManager is NULL." )
		return NDFalse;
	}

	NDWSDataManager* pWSDataManager = sNDWorldServer.dataManager();
	if ( NULL == pWSDataManager )
	{
		NDLOG_ERROR( " [NDPlayerMainSelectAllTable::getQueryResult] pWSDataManager is NULL." )
		return NDFalse;
	}

	NDUint32 nCount		= 0;
	NDUint32 nRowCount	= refQueryResult.getRowCount();
	
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
			NDLOG_ERROR( " [NDPlayerMainSelectAllTable::getQueryResult] NDMysqlQueryResult::ParseSelectResultToStruct parse is error." )
			break;
		}


		NDWSPlayer* pWSPlayer = pNDWSPlayerManager->addPlayer( playerMain );
		if ( NULL == pWSPlayer )
		{
			NDLOG_ERROR( " [NDPlayerMainSelectAllTable::getQueryResult] get pWSPlayer is NULL. PlayerGUID:[%llu].", playerMain.m_nPlayerGUID );
			continue;
		}

		++nCount;
	}

	if ( nRowCount != nCount )
	{
		NDLOG_ERROR( " [NDPlayerMainSelectAllTable::getQueryResult] QueryResult RowCount:[%u]. realAddPlayerCount:[%u].", nRowCount, nCount);
	}

	//…Ë÷√DB◊¢≤·»À ˝;
	

	return NDFalse;
}


