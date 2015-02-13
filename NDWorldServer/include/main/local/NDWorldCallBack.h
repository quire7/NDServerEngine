#ifndef __WORLDSERVER_ND_LOGIN_CALL_BACK_H__
#define __WORLDSERVER_ND_LOGIN_CALL_BACK_H__

#include "NDPreCompiler.h"

#include "message/NDS2SRegisterCallBack.h"
//#include "message/NDC2LCallBack.h"
//#include "message/NDL2ACallBack.h"
//#include "message/NDClient2LCallBack.h"

class NDWorldCallBack : public NDProtocolCallBack
{
public:
	NDWorldCallBack(void);
	~NDWorldCallBack(void);

	NDBool Process( NDIStream& rIStream, NDProtocolHeader& protocolHeader );

private:
	//client ping protocol dispose;
	NDBool pingProtocolDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );
	//client disconnect dispose;
	NDBool disconnectNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );

	//NDBool timerNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );

private:
	NDS2SRegisterCallBack		m_NDS2SRegisterCallBack;
	//NDC2LCallBack				m_NDC2LCallBack;
	//NDL2ACallBack				m_NDL2ACallBack;
	//NDClient2LCallBack		m_NDClient2LCallBack;
};

#endif
