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
	
	NDBool				disposeOperateData();								//同步数据到数据库中的数据;
	NDBool				isOperateDataEmpty();								//检查是否还有需要同步到数据库中的数据;
	NDBool				isQuitDBThread();									//是否退出DBThread;

private:
	NDBool				loadOnePlayerInfo( const NDDSWaitingAccount& refWaitingAccount );
};

#endif

