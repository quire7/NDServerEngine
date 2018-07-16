#ifndef __DATASERVER_ND_GS2DS_CALLBACK_H__
#define __DATASERVER_ND_GS2DS_CALLBACK_H__

#include "main/local/NDPreCompiler.h"

class NDGS2DSCallBack : public NDProtocolCallBack
{
public:
	NDGS2DSCallBack(void);
	~NDGS2DSCallBack(void);

	NDBool process( NDIStream& rIStream, NDProtocolHeader& protocolHeader );

private:
	NDBool gs2dsPlayerLoginReqDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );
	//NDBool c2lPlayerOfflineNtyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );
};

#endif
