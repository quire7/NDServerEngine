/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\database\NDMysqlBase.h
	file base:	NDMysqlBase
	purpose:	mysql encapsulation;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SERVERSHARE_ND_MYSQL_BASE_H__
#define __SERVERSHARE_ND_MYSQL_BASE_H__


#include "NDTypes.h"


_NDSHAREBASE_BEGIN

struct MysqlConnParam;
class NDMysqlQueryResult;
class NDMysqlBaseOp;
class NDMysqlBase
{
public:
	NDMysqlBase();
	virtual ~NDMysqlBase();

	/* mysql initialize operation */
	NDBool	initialize( const MysqlConnParam& connParam, NDUint32 nConnCount );
	void	disConnectDB();

	/************************************************************************/
	/* ȡDB�����Ϣ����                                                     */
	/************************************************************************/
	string			getState();												//������״̬
	string			getServerInfo();										//��������Ϣ
	string			getHostInfo();											//������Ϣ
	string			getClientInfo();										//�ͻ�����Ϣ
	NDUint32		getProtocolInfo();										//Э����Ϣ
	

	/************************************************************************/
	/* ���ݿ������������                                                   */
	/************************************************************************/
	void*					selectSql( NDUint32 nProtocolID, void* pProtocolReq, NDBool bChinese=NDFalse );				//ѡ���¼�����ؽ����
	// ����(���ظ���Ӱ�������) 
	NDUint32				updateSql( NDUint32 nProtocolID, void* pProtocolReq, NDBool bChinese=NDFalse );				//���¼�¼
	// ����(�������²����е��Զ�ID)
	NDUint32				insertSql( NDUint32 nProtocolID, void* pProtocolReq, NDBool bChinese=NDFalse );				//�����¼
	// ɾ��(����ɾ��Ӱ�������)
	NDUint32				deleteSql( NDUint32 nProtocolID, void* pProtocolReq, NDBool bChinese=NDFalse );				//ɾ����¼

protected:
	
	// ����MYSQL���
	virtual string			getQueryString( NDUint32 nProtocolID, void* pProtocolReq ) = 0;
	// �õ�ִ�н��
	virtual	void*			getResult( NDUint32	nProtocolID, NDMysqlQueryResult& refQueryResult ) = 0;

private:

	NDMysqlBaseOp*	m_pMysqlBaseOp;

};


_NDSHAREBASE_END

#endif

