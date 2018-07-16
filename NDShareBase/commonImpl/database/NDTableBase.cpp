#include "database/NDTableBase.h"

#include "NDCLibFun.h"
#include "file/NDLog.h"

#include "database/NDMysqlBaseOp.h"

_NDSHAREBASE_BEGIN


NDBool NDTableBase::onSelect( NDMysqlBaseOp* pMysqlBaseOp )
{
	if ( NULL == pMysqlBaseOp )
	{
		NDLOG_ERROR( " NDTableBase::onSelect pMysqlBaseOp is NULL! " )
		return NDFalse;
	}

	//build query string;
	string strQuery = getQueryString( EDataSourceOperType_SELECT );

	const char* pszQuery = strQuery.c_str();
	if ( (NULL == pszQuery) || ('\0' == pszQuery[0]) )
	{
		NDLOG_ERROR( " NDTableBase::onSelect pszQuery is NULL! " )
		return NDFalse;
	}

	//do real select query operation;
	MYSQL_RES* pMysqlRes	= NULL;
	NDUint32 nRefFieldCount = 0;
	NDUint32 nRefRowCount	= 0;
	if ( NDFalse == pMysqlBaseOp->selectSql( pszQuery, (NDUint32)strQuery.length(), pMysqlRes, nRefFieldCount, nRefRowCount, NDFalse ) )
	{
		return NDFalse;
	}

	NDMysqlQueryResult queryResult( pMysqlRes, nRefFieldCount, nRefRowCount );
	return getQueryResult( queryResult );
}

NDBool NDTableBase::onUpdate( NDMysqlBaseOp* pMysqlBaseOp, NDUint32 &refAffectedRows )
{
	if ( NULL == pMysqlBaseOp )
	{
		NDLOG_ERROR( " NDTableBase::onUpdate pMysqlBaseOp is NULL! " )
		return NDFalse;
	}

	//build query string;
	string strQuery = getQueryString( EDataSourceOperType_UPDATE );

	const char* pszQuery = strQuery.c_str();
	if ( (NULL == pszQuery) || ('\0' == pszQuery[0]) )
	{
		NDLOG_ERROR( " NDTableBase::onUpdate pszQuery is NULL! TableType:[%u].", getTableType() );
		return NDFalse;
	}

	return pMysqlBaseOp->updateSql( pszQuery, (NDUint32)strQuery.length(), refAffectedRows, NDFalse );
}

NDBool NDTableBase::onDelete( NDMysqlBaseOp* pMysqlBaseOp, NDUint32 &refAffectedRows )
{
	if ( NULL == pMysqlBaseOp )
	{
		NDLOG_ERROR( " NDTableBase::onDelete pMysqlBaseOp is NULL! " )
		return NDFalse;
	}

	//build query string;
	string strQuery = getQueryString( EDataSourceOperType_DELETE );

	const char* pszQuery = strQuery.c_str();
	if ( (NULL == pszQuery) || ('\0' == pszQuery[0]) )
	{
		NDLOG_ERROR( " NDTableBase::onDelete pszQuery is NULL! TableType:[%u].", getTableType() );
		return NDFalse;
	}

	return pMysqlBaseOp->deleteSql( pszQuery, (NDUint32)strQuery.length(), refAffectedRows, NDFalse );
}


_NDSHAREBASE_END

