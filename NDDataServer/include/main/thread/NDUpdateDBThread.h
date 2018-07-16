#ifndef __DATASERVER_ND_UPDATE_DBTHREAD_H__
#define __DATASERVER_ND_UPDATE_DBTHREAD_H__

#include "main/local/NDPreCompiler.h"

#include "database/NDTableBase.h"
#include "thread/NDMutex.h"
#include "thread/NDDBThread.h"

using NDShareBase::NDMutexLock;
using NDShareBase::NDGuardLock;
using NDShareBase::NDTableBase;
using NDShareBase::NDTableBaseList;
using NDShareBase::NDDBThread;
using NDShareBase::NDDBThreadPtrVec;
using NDShareBase::NDDBThreadPtrVecIter;

class NDUpdateDBThread : public NDDBThread
{
private:
	NDMutexLock			m_DataLock;				//用于TableBaseList的Lock;
	NDTableBaseList		m_TableProducelist;		//存储插入更新删除数据的LIST;

public:
	NDUpdateDBThread();
	~NDUpdateDBThread();

	
	NDBool				disposeOperateData();								//同步数据到数据库中的数据;
	NDBool				isOperateDataEmpty();								//检查是否还有需要同步到数据库中的数据;
	NDBool				isQuitDBThread();									//是否退出DBThread;

	void				insertOperateData( NDTableBase* pTableBaseProduce );	//向list中插入要同步的数据;
};

#endif

