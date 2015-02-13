#ifndef __LOGINSERVER_ND_WS2LS_CALLBACK_H__
#define __LOGINSERVER_ND_WS2LS_CALLBACK_H__

#include "main/local/NDPreCompiler.h"

class NDWS2LSCallBack : public NDProtocolCallBack
{
public:
	NDWS2LSCallBack(void);
	~NDWS2LSCallBack(void);

	NDBool Process( NDIStream& rIStream, NDProtocolHeader& protocolHeader );

private:
	//NDBool c2lLoadFactorNtyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );
	//NDBool c2lPlayerOfflineNtyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );
};

#endif
