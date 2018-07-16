#include "database/NDSelectTableBase.h"

#include "file/NDLog.h"

#include "database/NDMysqlBaseOp.h"

_NDSHAREBASE_BEGIN


NDBool NDSelectTableBase::onSelect( NDMysqlBaseOp* pMysqlBaseOp )
{
	if ( NULL == pMysqlBaseOp )
	{
		NDLOG_ERROR( " NDSelectTableBase::onSelect pMysqlBaseOp is NULL! " )
		return NDFalse;
	}

	//build query string;
	string strQuery = getQueryString();

	const char* pszQuery = strQuery.c_str();
	if ( (NULL == pszQuery) || ('\0' == pszQuery[0]) )
	{
		NDLOG_ERROR( " NDSelectTableBase::onSelect pszQuery is NULL! " )
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


_NDSHAREBASE_END

