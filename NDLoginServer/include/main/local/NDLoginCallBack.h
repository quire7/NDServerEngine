#ifndef __LOGINSERVER_ND_LOGIN_CALL_BACK_H__
#define __LOGINSERVER_ND_LOGIN_CALL_BACK_H__

#include "NDPreCompiler.h"

#include "message/NDS2SRegisterCallBack.h"
#include "message/NDWS2LSCallBack.h"
//#include "Message/NDL2ACallBack.h"
//#include "Message/NDClient2LCallBack.h"

class NDLoginCallBack : public NDProtocolCallBack
{
public:
	NDLoginCallBack(void);
	~NDLoginCallBack(void);

	NDBool Process( NDIStream& rIStream, NDProtocolHeader& protocolHeader );

private:
	//client ping protocol dispose;
	NDBool pingProtocolDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );
	//client disconnect dispose;
	NDBool disconnectNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );
	//NDBool timerNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );

private:
	NDS2SRegisterCallBack		m_NDS2SRegisterCallBack;
	NDWS2LSCallBack				m_NDWS2LSCallBack;
	//NDL2ACallBack				m_NDL2ACallBack;
	//NDClient2LCallBack			m_NDClient2LCallBack;
};

#endif
