#ifndef __GAMESERVER_ND_S2S_REGISTER_CALLBACK_H__
#define __GAMESERVER_ND_S2S_REGISTER_CALLBACK_H__



#include "main/local/NDPreCompiler.h"

class NDS2SRegisterCallBack : public NDProtocolCallBack
{
public:
	NDS2SRegisterCallBack(void);
	~NDS2SRegisterCallBack(void);

	NDBool process( NDIStream& rIStream, NDProtocolHeader& protocolHeader );

private:
	NDBool gs2dsRegisterResDispose( NDIStream& rIStream, NDProtocolHeader& header );
	NDBool gs2wsRegisterResDispose( NDIStream& rIStream, NDProtocolHeader& header );
	NDBool rs2gsRegisterReqDispose( NDIStream& rIStream, NDProtocolHeader& header );
	NDBool gws2gsRegisterReqDispose( NDIStream& rIStream, NDProtocolHeader& header );
};

#endif
