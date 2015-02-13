/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\database\NDMysqlBaseOp.h
	file base:	NDMysqlBaseOp
	purpose:	mysql base operation;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SERVERSHARE_ND_MYSQL_BASE_OPERATION_H__
#define __SERVERSHARE_ND_MYSQL_BASE_OPERATION_H__


#include "NDTypes.h"

#ifndef MYSQL_RES
typedef struct st_mysql_res MYSQL_RES;
#endif

#ifndef MYSQL
typedef struct st_mysql	MYSQL;
#endif

_NDSHAREBASE_BEGIN

struct NDSelectResultType;
class NDField;
class NDMysqlQueryResult
{
public:
	NDMysqlQueryResult( MYSQL_RES* pResult, NDUint32 nFieldCount, NDUint32 nRowCount );
	~NDMysqlQueryResult();

	NDBool		nextRow();

	NDField*	fetchRecord()		  { return m_pCurrentRow; }
	NDUint32	getFieldCount() const { return m_nFieldCount; }
	NDUint32	getRowCount()	const { return m_nRowCount; }

	static NDBool	ParseSelectResultToStruct( void* pStructAddr, const NDSelectResultType* pSelectResultType, NDUint8 nMemberNum, NDField* pField );

private:
	MYSQL_RES*		m_pQueryResult;
	NDField*		m_pCurrentRow;
	NDUint32		m_nFieldCount;
	NDUint32		m_nRowCount;
};

struct MysqlConnParam;
class NDSysLock;
class NDMysqlBaseOp
{
	struct MysqlConn
	{
		NDUint32	m_nIndex;
		NDBool		m_bBusy;
		MYSQL*		m_pMysql;
	};
public:
	NDMysqlBaseOp();
	~NDMysqlBaseOp();

	/* mysql initialize operation */
	NDBool	initialize( const MysqlConnParam& connParam, NDUint32 nConnCount );
	NDBool	checkConnections();
	void	disConnectDB();

	/************************************************************************/
	/* 取DB相关信息函数                                                     */
	/************************************************************************/
	string			getState();												//服务器状态;
	string			getServerInfo();										//服务器信息;
	string			getHostInfo();											//主机信息;
	string			getClientInfo();										//客户机信息;
	NDUint32		getProtocolInfo();										//协议信息;
	
	/* 函数用于创建可在SQL语句中使用的合法SQL字符串。;*/
	/*MySQL仅需要反斜杠和引号字符，用于引用转义查询中的字符串。;*/
	/*该函数能引用其他字符,从而使得它们在日志文件中具有更好的可读性.;*/
	string	escapeString( const char* pszBuf, NDUint32 nSize );

	/************************************************************************/
	/* 数据库基本操作函数;                                                  */
	/************************************************************************/
	MYSQL_RES*				selectSql( const char *szSql , NDUint32 nSize, NDBool bChinese=NDFalse );				//选择记录，返回结果集;
	NDUint32				updateSql( const char *szSql , NDUint32 nSize, NDBool bChinese=NDFalse );				//更新记录;
	NDUint32				insertSql( const char *szSql , NDUint32 nSize, NDBool bChinese=NDFalse );				//插入记录;
	NDUint32				deleteSql( const char *szSql , NDUint32 nSize, NDBool bChinese=NDFalse );				//删除记录;

	/************************************************************************/
	/* 数据库事务操作函数;                                                   */
	/************************************************************************/
	void			transBegin();
	void			transCommit();
	void			transRollback();

private:

	NDBool		connectDB( MysqlConn* pConn );
	void		disConnectDB( MysqlConn* pConn );

	MysqlConn*	getIdleHandle();
	void		setIdleHandle( MysqlConn* pConn );

	string		escapeString( MysqlConn* pConn, const char* pszBuf, NDUint32 nSize );

	// set chinese font ;
	NDBool		setChineseFont( MysqlConn* pConn );

private:
	
	NDUint32			m_nConnCount;
	NDSysLock*			m_pHanldeMutex;
	MysqlConn*			m_pConnHandle;
	MysqlConnParam*		m_pConnParam;

};

_NDSHAREBASE_END


#endif
