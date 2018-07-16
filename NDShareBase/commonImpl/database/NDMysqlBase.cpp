#include "database/NDMysqlBase.h"

//#include "NDTypes.h"	//因为mysql.h中要用到SOCKET定义,所以要前置,(这个不需要了,database/NDMysqlBase.h前置了);

//#include <mysql.h>

#include "NDShareBaseMacros.h"
#include "file/NDLog.h"
#include "function/NDRefCounted.h"
#include "database/NDMysqlBaseOp.h"





_NDSHAREBASE_BEGIN

NDMysqlBase::NDMysqlBase() : m_pMysqlBaseOp( new NDMysqlBaseOp )
{

}

NDMysqlBase::~NDMysqlBase()
{
	SAFE_DELETE( m_pMysqlBaseOp );
}

NDBool NDMysqlBase::initialize( const NDMysqlConnParam& connParam, NDUint32 nConnCount )
{
	if ( NULL == m_pMysqlBaseOp )
	{
		return NDFalse;
	}

	return m_pMysqlBaseOp->initialize( connParam, nConnCount );
}

void NDMysqlBase::disConnectDB()
{
	if ( NULL == m_pMysqlBaseOp )
	{
		return ;
	}

	m_pMysqlBaseOp->disConnectDB();
}

string NDMysqlBase::getState()
{
	if ( NULL == m_pMysqlBaseOp )
	{
		return string("");
	}

	return m_pMysqlBaseOp->getState();
}

string NDMysqlBase::getServerInfo()
{
	if ( NULL == m_pMysqlBaseOp )
	{
		return string("");
	}

	return m_pMysqlBaseOp->getServerInfo();
}

string NDMysqlBase::getHostInfo()
{
	if ( NULL == m_pMysqlBaseOp )
	{
		return string("");
	}

	return m_pMysqlBaseOp->getHostInfo();
}

string NDMysqlBase::getClientInfo()
{
	if ( NULL == m_pMysqlBaseOp )
	{
		return string("");
	}

	return m_pMysqlBaseOp->getClientInfo();
}

NDUint32 NDMysqlBase::getProtocolInfo()
{
	if ( NULL == m_pMysqlBaseOp )
	{
		return 0;
	}

	return m_pMysqlBaseOp->getProtocolInfo();
}


NDBool NDMysqlBase::selectSql( NDUint32 nProtocolID, void* pProtocolReq, void* &pRefResult, NDUint32 &nRefFieldCount, NDUint32 &nRefRowCount, NDBool bChinese )
{
	if ( NULL == m_pMysqlBaseOp )
	{
		NDLOG_ERROR( " NDMysqlBase::selectSql m_pMysqlBaseOp is NULL! " )
		return NDFalse;
	}

	//build query string;
	string strQuery = getQueryString( nProtocolID, pProtocolReq );

	const char* pszQuery = strQuery.c_str();
	if ( (NULL == pszQuery) || ('\0' == pszQuery[0]) )
	{
		NDLOG_ERROR( " NDMysqlBase::selectSql pszQuery is NULL! " )
		return NDFalse;
	}

	//do real select query operation;
	MYSQL_RES* pMysqlRes = NULL;
	if ( NDFalse == m_pMysqlBaseOp->selectSql( pszQuery, (NDUint32)strQuery.length(), pMysqlRes, nRefFieldCount, nRefRowCount, bChinese ) )
	{
		return NDFalse;
	}

	NDMysqlQueryResult queryResult( pMysqlRes, nRefFieldCount, nRefRowCount );
	return getResult( nProtocolID, queryResult, pRefResult );
}


NDBool NDMysqlBase::updateSql( NDUint32 nProtocolID, void* pProtocolReq, NDUint32 &refAffectedRows, NDBool bChinese )
{
	if ( NULL == m_pMysqlBaseOp )
	{
		NDLOG_ERROR( " NDMysqlBase::updateSql m_pMysqlBaseOp is NULL! " )
		return NDFalse;
	}

	//build query string;
	string strQuery = getQueryString( nProtocolID, pProtocolReq );

	const char* pszQuery = strQuery.c_str();
	if ( (NULL == pszQuery) || ('\0' == pszQuery[0]) )
	{
		NDLOG_ERROR( " NDMysqlBase::updateSql pszQuery is NULL! " )
		return NDFalse;
	}

	return m_pMysqlBaseOp->updateSql( pszQuery, (NDUint32)strQuery.length(), refAffectedRows, bChinese );
}


NDBool NDMysqlBase::insertSql( NDUint32 nProtocolID, void* pProtocolReq, NDUint32 &refAffectedRows, NDUint32 &refLastInsertID, NDBool bChinese )
{
	if ( NULL == m_pMysqlBaseOp )
	{
		NDLOG_ERROR( " NDMysqlBase::insertSql m_pMysqlBaseOp is NULL! " )
		return NDFalse;
	}
	
	//build query string;
	string strQuery = getQueryString( nProtocolID, pProtocolReq );

	const char* pszQuery = strQuery.c_str();
	if ( (NULL == pszQuery) || ('\0' == pszQuery[0]) )
	{
		NDLOG_ERROR( " NDMysqlBase::insertSql pszQuery is NULL! " )
		return NDFalse;
	}

	return m_pMysqlBaseOp->insertSql( pszQuery, (NDUint32)strQuery.length(), refAffectedRows, refLastInsertID, bChinese );
}


NDBool NDMysqlBase::deleteSql( NDUint32 nProtocolID, void* pProtocolReq, NDUint32 &refAffectedRows, NDBool bChinese )
{
	if ( NULL == m_pMysqlBaseOp )
	{
		NDLOG_ERROR( " NDMysqlBase::deleteSql m_pMysqlBaseOp is NULL! " )
		return NDFalse;
	}

	//build query string;
	string strQuery = getQueryString( nProtocolID, pProtocolReq );

	const char* pszQuery = strQuery.c_str();
	if ( (NULL == pszQuery) || ('\0' == pszQuery[0]) )
	{
		NDLOG_ERROR( " NDMysqlBase::deleteSql pszQuery is NULL! " )
		return NDFalse;
	}

	return m_pMysqlBaseOp->deleteSql( pszQuery, (NDUint32)strQuery.length(), refAffectedRows, bChinese );
}


_NDSHAREBASE_END
