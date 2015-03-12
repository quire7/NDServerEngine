#ifndef __DATASERVER_ND_S2S_REGISTER_CALLBACK_H__
#define __DATASERVER_ND_S2S_REGISTER_CALLBACK_H__

#include "main/local/NDPreCompiler.h"

class NDS2SRegisterCallBack : public NDProtocolCallBack
{
public:
	NDS2SRegisterCallBack(void);
	~NDS2SRegisterCallBack(void);

	NDBool process( NDIStream& rIStream, NDProtocolHeader& protocolHeader );

private:
	NDBool gs2dsRegisterReqDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );
	//NDBool l2aRegisterResDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );
};

#endif
