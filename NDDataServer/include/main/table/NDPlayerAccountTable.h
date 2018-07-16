#ifndef __DATASERVER_ND_PLAEYR_ACCOUNT_TABLE_H__
#define __DATASERVER_ND_PLAEYR_ACCOUNT_TABLE_H__

#include "main/local/NDPreCompiler.h"
#include "database/NDTableBase.h"

namespace NDShareBase
{
	class NDMysqlQueryResult;
}
using NDShareBase::NDMysqlQueryResult;
using NDShareBase::NDTableBase;

class NDPlayerAccountTable : public NDTableBase
{
public:
	NDPlayerAccount		m_playerAccount;			//玩家帐号表;
public:
	NDPlayerAccountTable(){}
	~NDPlayerAccountTable(){}

	//获得数据操作类型;
	NDUint8		getOperType() const		{ return m_playerAccount.m_nOperType; };

	//数据库表枚举类型;
	NDUint8		getTableType() const	{ return ETableType_Account; };

	//获得数据所属玩家GUID,如果数据不属于任何一个玩家,则返回0;
	NDUint64	getPlayerGUID() const	{ return m_playerAccount.m_nPlayerGUID; };

	// 获得构造的MYSQL的query语句;
	string		getQueryString( EDataSourceOperType nOperType );
	// 得到执行结果;
	NDBool		getQueryResult( NDMysqlQueryResult& refQueryResult );
};


#endif
