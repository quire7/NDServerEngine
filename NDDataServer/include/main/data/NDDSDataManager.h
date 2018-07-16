#ifndef __DATASERVER_ND_DSDATA_MANAGER_H__
#define __DATASERVER_ND_DSDATA_MANAGER_H__


#include "main/local/NDPreCompiler.h"

#include <list>
#include <set>


#include "thread/NDMutex.h"
#include "database/NDMysqlConst.h"


#include "NDDSDataStructs.h"



namespace NDShareBase
{
	class NDTableBase;
}

using std::list;
using std::set;
using NDShareBase::NDTableBase;
using NDShareBase::NDMysqlConnParam;
using NDShareBase::NDMutexLock;
using NDShareBase::NDGuardLock;

class NDDSDataManager
{
private:
	typedef	list<NDDSWaitingAccount>				NDDSWaitingAccountList;
	typedef NDDSWaitingAccountList::iterator		NDDSWaitingAccountListIter;
	typedef set<NDUint64>							NDPlayerGUIDSet;
	typedef NDPlayerGUIDSet::iterator				NDPlayerGUIDSetIter;

	NDMutexLock										m_waitingAccountQueueLock;
	NDPlayerGUIDSet									m_waitingAccountPlayerGUIDSet;
	NDDSWaitingAccountList							m_waitingAccountQueue;				//DS登录等待数据实体;

	NDMutexLock										m_loadSuccessAccountQueueLock;
	NDDSWaitingAccountList							m_loadSuccessAccountQueue;			//DS加载成功玩家数据的玩家队列;

	NDDSConfigBaseInfo								m_NDDSConfigBaseInfo;				//DS配置基本信息;
	NDMysqlConnParam								m_NDMysqlConnParam;					//数据库连接参数;

public:
	NDDSDataManager();
	~NDDSDataManager();

	void						clear();

	void						heartBeat();

	//DS等待加载数据的操作;
	NDBool						isWaitingQueueEmpty();
	NDBool						isInWaitingQueue( NDUint64 nPlayerGUID );
	void						addWaitingQueue( const NDDSWaitingAccount& refData );
	NDBool						removeWaitingQueue( NDDSWaitingAccount& refData );	

	//DS加载成功玩家数据的玩家队列的操作;
	NDBool						isLoadSuccessQueueEmpty();
	void						addLoadSuccessQueue( const NDDSWaitingAccount& refData );

	//将需要同步到数据库的数据放入到更新线程中;
	NDBool						setOperatorData( NDTableBase* pTableBaseProduce );


	//获取DS基本数据;
	NDMysqlConnParam*			getMysqlConnParam()		{ return &m_NDMysqlConnParam; }
	NDDSConfigBaseInfo*			getDSConfigBaseInfo()	{ return &m_NDDSConfigBaseInfo; }

private:
	NDBool						sendLoadSuccessQueuePlayerDataToGS();
};

#endif
