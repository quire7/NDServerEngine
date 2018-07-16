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

	NDBool				disposeOperateData();								//ͬ�����ݵ����ݿ��е�����;
	NDBool				isOperateDataEmpty();								//����Ƿ�����Ҫͬ�������ݿ��е�����;
	NDBool				isQuitDBThread();									//�Ƿ��˳�DBThread;

private:
	NDBool				onPlayerMainSelectAll();							//���ݿ����,��������ݲ�����map;
};



#endif
