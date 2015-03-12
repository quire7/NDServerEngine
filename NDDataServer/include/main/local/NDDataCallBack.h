#ifndef __DATASERVER_ND_DATA_CALL_BACK_H__
#define __DATASERVER_ND_DATA_CALL_BACK_H__

#include "NDPreCompiler.h"

#include "message/NDS2SRegisterCallBack.h"
#include "message/NDGS2DSCallBack.h"


class NDDataCallBack : public NDProtocolCallBack
{
public:
	NDDataCallBack(void);
	~NDDataCallBack(void);

	NDBool process( NDIStream& rIStream, NDProtocolHeader& protocolHeader );

private:
	//client ping protocol dispose;
	NDBool pingProtocolDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );
	//client disconnect dispose;
	NDBool disconnectNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );
	//NDBool timerNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );

private:
	NDS2SRegisterCallBack		m_NDS2SRegisterCallBack;
	NDGS2DSCallBack				m_NDGS2DSCallBack;
	//NDL2ACallBack				m_NDL2ACallBack;
	//NDClient2LCallBack			m_NDClient2LCallBack;
};

#endif
