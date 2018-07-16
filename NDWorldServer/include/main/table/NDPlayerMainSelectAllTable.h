#ifndef __WORLDSERVER_ND_PALYER_MAIN_SELECT_ALLTABLE_H__
#define __WORLDSERVER_ND_PALYER_MAIN_SELECT_ALLTABLE_H__

#include "main/local/NDPreCompiler.h"

#include "database/NDSelectTableBase.h"
using NDShareBase::NDSelectTableBase;
using NDShareBase::NDMysqlQueryResult;
class NDPlayerMainSelectAllTable : public NDSelectTableBase
{
private:
public:
	NDPlayerMainSelectAllTable(){}
	~NDPlayerMainSelectAllTable(){}
	
	string		getQueryString();
	NDBool		getQueryResult( NDMysqlQueryResult& refQueryResult );
};


#endif
