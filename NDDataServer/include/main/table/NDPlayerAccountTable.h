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
	NDPlayerAccount		m_playerAccount;			//����ʺű�;
public:
	NDPlayerAccountTable(){}
	~NDPlayerAccountTable(){}

	//������ݲ�������;
	NDUint8		getOperType() const		{ return m_playerAccount.m_nOperType; };

	//���ݿ��ö������;
	NDUint8		getTableType() const	{ return ETableType_Account; };

	//��������������GUID,������ݲ������κ�һ�����,�򷵻�0;
	NDUint64	getPlayerGUID() const	{ return m_playerAccount.m_nPlayerGUID; };

	// ��ù����MYSQL��query���;
	string		getQueryString( EDataSourceOperType nOperType );
	// �õ�ִ�н��;
	NDBool		getQueryResult( NDMysqlQueryResult& refQueryResult );
};


#endif
