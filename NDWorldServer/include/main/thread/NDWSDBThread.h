#ifndef __WORLDSERVER_ND_WS_DB_THREAD_H__
#define __WORLDSERVER_ND_WS_DB_THREAD_H__

#include "main/local/NDPreCompiler.h"

#include "thread/NDDBThread.h"
using NDShareBase::NDDBThread;

class NDWSDBThread : public NDDBThread
{
public:
	NDWSDBThread();
	~NDWSDBThread();

	NDBool				disposeOperateData();								//同步数据到数据库中的数据;
	NDBool				isOperateDataEmpty();								//检查是否还有需要同步到数据库中的数据;
	NDBool				isQuitDBThread();									//是否退出DBThread;

private:
	NDBool				onPlayerMainSelectAll();							//数据库操作,将玩家数据插入多个map;
};



#endif
