#ifndef __GAMESERVER_ND_DS2GS_CALLBACK_H__
#define __GAMESERVER_ND_DS2GS_CALLBACK_H__

#include "main/local/NDPreCompiler.h"

class NDDS2GSCallBack : public NDProtocolCallBack
{
public:
	NDDS2GSCallBack(void);
	~NDDS2GSCallBack(void);

	NDBool process( NDIStream& rIStream, NDProtocolHeader& protocolHeader );

private:
	NDBool ds2gsSendPlayerMainNtyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );
	NDBool ds2gsPlayerLoginResDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );
};

#endif
