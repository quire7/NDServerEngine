#ifndef __DATASERVER_ND_SELECT_DBTHREAD_H__
#define __DATASERVER_ND_SELECT_DBTHREAD_H__

#include "main/local/NDPreCompiler.h"

#include "thread/NDDBThread.h"
using NDShareBase::NDDBThread;

struct NDDSWaitingAccount;
class NDSelectDBThread : public NDDBThread
{
public:
	NDSelectDBThread();
	~NDSelectDBThread();
	
	NDBool				disposeOperateData();								//ͬ�����ݵ����ݿ��е�����;
	NDBool				isOperateDataEmpty();								//����Ƿ�����Ҫͬ�������ݿ��е�����;
	NDBool				isQuitDBThread();									//�Ƿ��˳�DBThread;

private:
	NDBool				loadOnePlayerInfo( const NDDSWaitingAccount& refWaitingAccount );
};

#endif

