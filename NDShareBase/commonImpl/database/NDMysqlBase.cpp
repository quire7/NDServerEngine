#include "database/NDMysqlBase.h"

//#include "NDTypes.h"	//因为mysql.h中要用到SOCKET定义,所以要前置,(这个不需要了,database/NDMysqlBase.h前置了);

#include <mysql.h>

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

NDBool NDMysqlBase::initialize( const MysqlConnParam& connParam, NDUint32 nConnCount )
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


void* NDMysqlBase::selectSql( NDUint32 nProtocolID, void* pProtocolReq, NDBool bChinese )
{
	if ( NULL == m_pMysqlBaseOp )
	{
		NDLOG_ERROR( " NDMysqlBase::selectSql m_pMysqlBaseOp is NULL! " )
		return NULL;
	}

	void* pResult = NULL;

	//build query string;
	string strQuery = getQueryString( nProtocolID, pProtocolReq );

	const char* pszQuery = strQuery.c_str();
	if ( (NULL == pszQuery) || ('\0' == pszQuery[0]) )
	{
		NDLOG_ERROR( " NDMysqlBase::selectSql pszQuery is NULL! " )
	}
	else
	{
		//do real select query operation;
		MYSQL_RES* pMysqlRes = m_pMysqlBaseOp->selectSql( pszQuery, (NDUint32)strQuery.length(), bChinese );
		if ( NULL != pMysqlRes )
		{
			NDMysqlQueryResult queryResult( pMysqlRes, (NDUint32)mysql_num_fields(pMysqlRes), (NDUint32)mysql_num_rows(pMysqlRes) );
			pResult = getResult( nProtocolID, queryResult );
		}
	}

	return pResult;
}


NDUint32 NDMysqlBase::updateSql( NDUint32 nProtocolID, void* pProtocolReq, NDBool bChinese )
{
	if ( NULL == m_pMysqlBaseOp )
	{
		NDLOG_ERROR( " NDMysqlBase::updateSql m_pMysqlBaseOp is NULL! " )
		return 0;
	}

	NDUint32 nRet =	0;

	//build query string;
	string strQuery = getQueryString( nProtocolID, pProtocolReq );

	const char* pszQuery = strQuery.c_str();
	if ( (NULL == pszQuery) || ('\0' == pszQuery[0]) )
	{
		NDLOG_ERROR( " NDMysqlBase::updateSql pszQuery is NULL! " )
	}
	else
	{
		nRet = m_pMysqlBaseOp->updateSql( pszQuery, (NDUint32)strQuery.length(), bChinese );
	}

	return nRet;
}


NDUint32 NDMysqlBase::insertSql( NDUint32 nProtocolID, void* pProtocolReq, NDBool bChinese )
{
	if ( NULL == m_pMysqlBaseOp )
	{
		NDLOG_ERROR( " NDMysqlBase::insertSql m_pMysqlBaseOp is NULL! " )
		return 0;
	}

	NDUint32 nRet =	0;
	
	//build query string;
	string strQuery = getQueryString( nProtocolID, pProtocolReq );

	const char* pszQuery = strQuery.c_str();
	if ( (NULL == pszQuery) || ('\0' == pszQuery[0]) )
	{
		NDLOG_ERROR( " NDMysqlBase::insertSql pszQuery is NULL! " )
	}
	else
	{
		nRet = m_pMysqlBaseOp->insertSql( pszQuery, (NDUint32)strQuery.length(), bChinese );
	}
	
	return nRet;
}


NDUint32 NDMysqlBase::deleteSql( NDUint32 nProtocolID, void* pProtocolReq, NDBool bChinese )
{
	if ( NULL == m_pMysqlBaseOp )
	{
		NDLOG_ERROR( " NDMysqlBase::deleteSql m_pMysqlBaseOp is NULL! " )
		return 0;
	}

	NDUint32 nRet =	0;

	//build query string;
	string strQuery = getQueryString( nProtocolID, pProtocolReq );

	const char* pszQuery = strQuery.c_str();
	if ( (NULL == pszQuery) || ('\0' == pszQuery[0]) )
	{
		NDLOG_ERROR( " NDMysqlBase::deleteSql pszQuery is NULL! " )
	}
	else
	{
		nRet = m_pMysqlBaseOp->deleteSql( pszQuery, (NDUint32)strQuery.length(), bChinese );
	}
	
	return nRet;
}


_NDSHAREBASE_END
