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
	/* 取DB相关信息函数                                                     */
	/************************************************************************/
	string			getState();												//服务器状态
	string			getServerInfo();										//服务器信息
	string			getHostInfo();											//主机信息
	string			getClientInfo();										//客户机信息
	NDUint32		getProtocolInfo();										//协议信息
	

	/************************************************************************/
	/* 数据库基本操作函数                                                   */
	/************************************************************************/
	void*					selectSql( NDUint32 nProtocolID, void* pProtocolReq, NDBool bChinese=NDFalse );				//选择记录，返回结果集
	// 更新(返回更新影响的行数) 
	NDUint32				updateSql( NDUint32 nProtocolID, void* pProtocolReq, NDBool bChinese=NDFalse );				//更新记录
	// 插入(返回最新插入行的自动ID)
	NDUint32				insertSql( NDUint32 nProtocolID, void* pProtocolReq, NDBool bChinese=NDFalse );				//插入记录
	// 删除(返回删除影响的行数)
	NDUint32				deleteSql( NDUint32 nProtocolID, void* pProtocolReq, NDBool bChinese=NDFalse );				//删除记录

protected:
	
	// 构造MYSQL语句
	virtual string			getQueryString( NDUint32 nProtocolID, void* pProtocolReq ) = 0;
	// 得到执行结果
	virtual	void*			getResult( NDUint32	nProtocolID, NDMysqlQueryResult& refQueryResult ) = 0;

private:

	NDMysqlBaseOp*	m_pMysqlBaseOp;

};


_NDSHAREBASE_END

#endif

