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
	NDPlayerMain		m_playerMain;			//�����������;
public:
	NDPlayerMainTable(){}
	~NDPlayerMainTable(){}

	//������ݲ�������;
	NDUint8		getOperType() const		{ return m_playerMain.m_nOperType; };

	//���ݿ��ö������;
	NDUint8		getTableType() const	{ return ETableType_PlayerMain; };

	//��������������GUID,������ݲ������κ�һ�����,�򷵻�0;
	NDUint64	getPlayerGUID() const	{ return m_playerMain.m_nPlayerGUID; };

	// ��ù����MYSQL��query���;
	string		getQueryString( EDataSourceOperType nOperType );
	// �õ�ִ�н��;
	NDBool		getQueryResult( NDMysqlQueryResult& refQueryResult );
};


#endif
