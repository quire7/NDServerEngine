#include "main/table/NDPlayerAccountTable.h"

#include "database/NDField.h"
#include "database/NDMysqlBaseOp.h"

#include "main/local/NDDataServer.h"

using NDShareBase::NDField;
using NDShareBase::NDSelectResultType;


static NDPlayerAccount	s_NDPlayerAccount;
static const NDSelectResultType s_NDPlayerAccountSelectResultType[] =
{
	{ NDPlayerAccountTableField_PlayerGUID,		NDShareBase::FIELD_UI64,		0,									(NDUint64)(&s_NDPlayerAccount.m_nPlayerGUID) - (NDUint64)(&s_NDPlayerAccount.m_nOperType) },
	{ NDPlayerAccountTableField_Account,		NDShareBase::FIELD_STRING,		NDACCOUNT_TABLE_ACCOUNT_LEN,		(NDUint64)(s_NDPlayerAccount.m_szAccount) - (NDUint64)(&s_NDPlayerAccount.m_nOperType) },
	{ NDPlayerAccountTableField_PassWord,		NDShareBase::FIELD_STRING,		NDACCOUNT_TABLE_PASSWORD_LEN,		(NDUint64)(s_NDPlayerAccount.m_szPassWord) - (NDUint64)(&s_NDPlayerAccount.m_nOperType) },
	{ NDPlayerAccountTableField_LoginCount,		NDShareBase::FIELD_UI32,		0,									(NDUint64)(&s_NDPlayerAccount.m_nLoginCount) - (NDUint64)(&s_NDPlayerAccount.m_nOperType) },
	{ NDPlayerAccountTableField_OnlineTimes,	NDShareBase::FIELD_UI32,		0,									(NDUint64)(&s_NDPlayerAccount.m_nOnlineMinuteTimes) - (NDUint64)(&s_NDPlayerAccount.m_nOperType) },
	{ NDPlayerAccountTableField_RegTime,		NDShareBase::FIELD_STRING,		ND_TIME_STRLEN_MIN,					(NDUint64)(s_NDPlayerAccount.m_szRegTime) - (NDUint64)(&s_NDPlayerAccount.m_nOperType) },
	{ NDPlayerAccountTableField_LastLoginTime,	NDShareBase::FIELD_STRING,		ND_TIME_STRLEN_MIN,					(NDUint64)(s_NDPlayerAccount.m_szLastLoginTime) - (NDUint64)(&s_NDPlayerAccount.m_nOperType) },

};

string NDPlayerAccountTable::getQueryString(EDataSourceOperType nOperType)
{
	if ( nOperType <= EDataSourceOperType_NULL || nOperType >= EDataSourceOperType_MAX )
	{
		NDLOG_ERROR( " NDPlayerAccountTable::getQueryString nOperType is error. " )
		return string("");
	}

	char szSql[BUF_LEN_512] = {0};
	switch ( nOperType )
	{
	case EDataSourceOperType_SELECT:
		{
			ND_SNPRINTF( szSql, sizeof(szSql) - 1, "call sp_player_account_select_by_guid('%llu')", m_playerAccount.m_nPlayerGUID );
		}
		break;
	case EDataSourceOperType_INSERT:
	case EDataSourceOperType_UPDATE:
		{
			ND_SNPRINTF( szSql, sizeof(szSql) - 1, "call sp_player_account_insert_update('%llu','%s','%s','%u','%u','%s','%s')", 
						m_playerAccount.m_nPlayerGUID,
						m_playerAccount.m_szAccount,
						m_playerAccount.m_szPassWord,
						m_playerAccount.m_nLoginCount,
						m_playerAccount.m_nOnlineMinuteTimes,
						m_playerAccount.m_szRegTime,
						m_playerAccount.m_szLastLoginTime);
		}
		break;
	case EDataSourceOperType_DELETE:
		{
			ND_SNPRINTF( szSql, sizeof(szSql) - 1, "call sp_player_account_delete('%llu')", m_playerAccount.m_nPlayerGUID );
		}
		break;
	}

	return string(szSql);
}

NDBool NDPlayerAccountTable::getQueryResult(NDMysqlQueryResult& refQueryResult)
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
			
			NDPlayerAccount		playerAccount;

			NDUint8 nMemberNum = sizeof(s_NDPlayerAccountSelectResultType)/sizeof(s_NDPlayerAccountSelectResultType[0]);
			if ( NDFalse == NDMysqlQueryResult::ParseSelectResultToStruct( &playerAccount, s_NDPlayerAccountSelectResultType, nMemberNum, pField ) )
			{
				NDLOG_ERROR( " [NDPlayerAccountTable::getQueryResult] NDMysqlQueryResult::ParseSelectResultToStruct parse is error. PlayerGUID:[%llu].", m_playerAccount.m_nPlayerGUID );
				break;
			}

			NDDSPlayerManager* pNDDSPlayerManager = sNDDataServer.playerManager();
			if ( NULL == pNDDSPlayerManager )
			{
				break;
			}

			pNDDSPlayerManager->addPlayer( playerAccount );
		}

		return NDTrue;
	}

	NDLOG_ERROR( " [NDPlayerAccountTable::getQueryResult] QueryResult RowCount:[%u]. PlayerGUID:[%llu].", nRowCount, m_playerAccount.m_nPlayerGUID );
	return NDFalse;
}
