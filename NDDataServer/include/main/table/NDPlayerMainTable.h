#ifndef __DATASERVER_ND_PLAEYR_MAIN_TABLE_H__
#define __DATASERVER_ND_PLAEYR_MAIN_TABLE_H__

#include "main/local/NDPreCompiler.h"
#include "database/NDTableBase.h"

namespace NDShareBase
{
	class NDMysqlQueryResult;
}
using NDShareBase::NDMysqlQueryResult;
using NDShareBase::NDTableBase;

class NDPlayerMainTable : public NDTableBase
{
public:
	NDPlayerMain		m_playerMain;			//玩家主表数据;
public:
	NDPlayerMainTable(){}
	~NDPlayerMainTable(){}

	//获得数据操作类型;
	NDUint8		getOperType() const		{ return m_playerMain.m_nOperType; };

	//数据库表枚举类型;
	NDUint8		getTableType() const	{ return ETableType_PlayerMain; };

	//获得数据所属玩家GUID,如果数据不属于任何一个玩家,则返回0;
	NDUint64	getPlayerGUID() const	{ return m_playerMain.m_nPlayerGUID; };

	// 获得构造的MYSQL的query语句;
	string		getQueryString( EDataSourceOperType nOperType );
	// 得到执行结果;
	NDBool		getQueryResult( NDMysqlQueryResult& refQueryResult );
};


#endif
