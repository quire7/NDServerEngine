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
	NDDSWaitingAccountList							m_waitingAccountQueue;				//DS��¼�ȴ�����ʵ��;

	NDMutexLock										m_loadSuccessAccountQueueLock;
	NDDSWaitingAccountList							m_loadSuccessAccountQueue;			//DS���سɹ�������ݵ���Ҷ���;

	NDDSConfigBaseInfo								m_NDDSConfigBaseInfo;				//DS���û�����Ϣ;
	NDMysqlConnParam								m_NDMysqlConnParam;					//���ݿ����Ӳ���;

public:
	NDDSDataManager();
	~NDDSDataManager();

	void						clear();

	void						heartBeat();

	//DS�ȴ��������ݵĲ���;
	NDBool						isWaitingQueueEmpty();
	NDBool						isInWaitingQueue( NDUint64 nPlayerGUID );
	void						addWaitingQueue( const NDDSWaitingAccount& refData );
	NDBool						removeWaitingQueue( NDDSWaitingAccount& refData );	

	//DS���سɹ�������ݵ���Ҷ��еĲ���;
	NDBool						isLoadSuccessQueueEmpty();
	void						addLoadSuccessQueue( const NDDSWaitingAccount& refData );

	//����Ҫͬ�������ݿ�����ݷ��뵽�����߳���;
	NDBool						setOperatorData( NDTableBase* pTableBaseProduce );


	//��ȡDS��������;
	NDMysqlConnParam*			getMysqlConnParam()		{ return &m_NDMysqlConnParam; }
	NDDSConfigBaseInfo*			getDSConfigBaseInfo()	{ return &m_NDDSConfigBaseInfo; }

private:
	NDBool						sendLoadSuccessQueuePlayerDataToGS();
};

#endif
