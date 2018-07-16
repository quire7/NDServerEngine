#ifndef __GATEWAYSERVER_ND_GS2GTWS_CALLBACK_H__
#define __GATEWAYSERVER_ND_GS2GTWS_CALLBACK_H__



#include "main/local/NDPreCompiler.h"

class NDGS2GTWSCallBack : public NDProtocolCallBack
{
public:
	NDGS2GTWSCallBack(void);
	~NDGS2GTWSCallBack(void);

	NDBool process( NDIStream& rIStream, NDProtocolHeader& protocolHeader );

private:

	NDBool gs2gtwsPlayerLoginSuccessNtyDispose( NDIStream& rIStream, NDProtocolHeader& header );
};

#endif
