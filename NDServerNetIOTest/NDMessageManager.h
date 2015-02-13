#ifndef _ND_MESSAGE_MANAGER_H_
#define _ND_MESSAGE_MANAGER_H_


#include "EchoCallBack.h"
#include "InsideCallBack.h"

using NDShareBase::NDDataProcess;
class NDDataProcess;

class NDMessageManager
{
public:
	static NDMessageManager* getInstance();
	static void releaseInstance();

    void RegisterMessage();
	
	NDDataProcess* getDataProcess() { return m_pNDDataProcess; }

protected:
	NDMessageManager(void);
	~NDMessageManager(void);	

private:
	NDDataProcess* m_pNDDataProcess;

	EchoCallBack m_EchoCallBack;

	PingCallBack m_PingCallBack;


	static NDMessageManager* m_pNDMessageManager;
};


#endif