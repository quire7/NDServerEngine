#ifndef __WORLDSERVER_ND_WS_DATA_MANAGER_H__
#define __WORLDSERVER_ND_WS_DATA_MANAGER_H__

#include "main/local/NDPreCompiler.h"

#include "NDWSDataStructs.h"

class NDWSDataManager
{
private:
	vector<NDWSDBAccountInfo>			m_wsDBAccountInfoVec;

public:
	NDWSDataManager();
	~NDWSDataManager();

	void						clear();			

	NDBool						init();
	void						release();

	//DB账号参数信息相关;
	vector<NDWSDBAccountInfo>*	getWSDBAccountInfoVecPtr()			{ return &m_wsDBAccountInfoVec; }
	//NDMysqlConnParam*			getMysqlConnParamPtr()				{ return &m_wsDBAccountInfo.mysqlConnParam; }

};

#endif
