#ifndef __GAMESERVER_ND_GAME_CALL_BACK_H__
#define __GAMESERVER_ND_GAME_CALL_BACK_H__

#include "NDPreCompiler.h"

#include "message/NDS2SRegisterCallBack.h"
#include "message/NDDS2GSCallBack.h"

class NDGameCallBack : public NDProtocolCallBack
{
public:
	NDGameCallBack(void);
	~NDGameCallBack(void);

	NDBool process( NDIStream& rIStream, NDProtocolHeader& protocolHeader );

private:
	//client ping req protocol dispose;
	NDBool pingProtocolDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );
	//client disconnect dispose;
	NDBool disconnectNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );

	//server ping res protocol dispose;
	NDBool pingResProtocolDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );

	//NDBool timerNotifyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );

private:
	NDS2SRegisterCallBack		m_NDS2SRegisterCallBack;
	NDDS2GSCallBack				m_NDDS2GSCallBack;
};

#endif
