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
	/* ȡDB�����Ϣ����                                                     */
	/************************************************************************/
	string			getState();												//������״̬;
	string			getServerInfo();										//��������Ϣ;
	string			getHostInfo();											//������Ϣ;
	string			getClientInfo();										//�ͻ�����Ϣ;
	NDUint32		getProtocolInfo();										//Э����Ϣ;
	
	/* �������ڴ�������SQL�����ʹ�õĺϷ�SQL�ַ�����;*/
	/*MySQL����Ҫ��б�ܺ������ַ�����������ת���ѯ�е��ַ�����;*/
	/*�ú��������������ַ�,�Ӷ�ʹ����������־�ļ��о��и��õĿɶ���.;*/
	string	escapeString( const char* pszBuf, NDUint32 nSize );

	/************************************************************************/
	/* ���ݿ������������;                                                  */
	/************************************************************************/
	MYSQL_RES*				selectSql( const char *szSql , NDUint32 nSize, NDBool bChinese=NDFalse );				//ѡ���¼�����ؽ����;
	NDUint32				updateSql( const char *szSql , NDUint32 nSize, NDBool bChinese=NDFalse );				//���¼�¼;
	NDUint32				insertSql( const char *szSql , NDUint32 nSize, NDBool bChinese=NDFalse );				//�����¼;
	NDUint32				deleteSql( const char *szSql , NDUint32 nSize, NDBool bChinese=NDFalse );				//ɾ����¼;

	/************************************************************************/
	/* ���ݿ������������;                                                   */
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
