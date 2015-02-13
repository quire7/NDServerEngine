#ifndef __LOGINSERVER_ND_S2S_REGISTER_CALLBACK_H__
#define __LOGINSERVER_ND_S2S_REGISTER_CALLBACK_H__

#include "main/local/NDPreCompiler.h"

class NDS2SRegisterCallBack : public NDProtocolCallBack
{
public:
	NDS2SRegisterCallBack(void);
	~NDS2SRegisterCallBack(void);

	NDBool Process( NDIStream& rIStream, NDProtocolHeader& protocolHeader );

private:
	NDBool c2lRegisterReqDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );
	//NDBool l2aRegisterResDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );
};

#endif
