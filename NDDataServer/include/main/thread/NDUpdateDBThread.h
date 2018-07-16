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
	NDMutexLock			m_DataLock;				//����TableBaseList��Lock;
	NDTableBaseList		m_TableProducelist;		//�洢�������ɾ�����ݵ�LIST;

public:
	NDUpdateDBThread();
	~NDUpdateDBThread();

	
	NDBool				disposeOperateData();								//ͬ�����ݵ����ݿ��е�����;
	NDBool				isOperateDataEmpty();								//����Ƿ�����Ҫͬ�������ݿ��е�����;
	NDBool				isQuitDBThread();									//�Ƿ��˳�DBThread;

	void				insertOperateData( NDTableBase* pTableBaseProduce );	//��list�в���Ҫͬ��������;
};

#endif

