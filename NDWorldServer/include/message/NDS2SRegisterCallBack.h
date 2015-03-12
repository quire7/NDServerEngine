#ifndef __WORLDSERVER_ND_S2S_REGISTER_CALLBACK_H__
#define __WORLDSERVER_ND_S2S_REGISTER_CALLBACK_H__



#include "main/local/NDPreCompiler.h"

class NDS2SRegisterCallBack : public NDProtocolCallBack
{
public:
	NDS2SRegisterCallBack(void);
	~NDS2SRegisterCallBack(void);

	NDBool process( NDIStream& rIStream, NDProtocolHeader& protocolHeader );

private:
	//NDBool c2gdbRegisterResDispose( NDIStream& rIStream, NDProtocolHeader& header );

	NDBool ls2wsRegisterResDispose( NDIStream& rIStream, NDProtocolHeader& header );
	NDBool rs2wsRegisterReqDispose( NDIStream& rIStream, NDProtocolHeader& header );
	NDBool gs2wsRegisterReqDispose( NDIStream& rIStream, NDProtocolHeader& header );
};

#endif
