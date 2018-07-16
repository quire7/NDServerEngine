#ifndef __LOGINSERVER_ND_C2LS_CALLBACK_H__
#define __LOGINSERVER_ND_C2LS_CALLBACK_H__

#include "main/local/NDPreCompiler.h"

class NDC2LSCallBack : public NDProtocolCallBack
{
public:
	NDC2LSCallBack(void);
	~NDC2LSCallBack(void);

	NDBool process( NDIStream& rIStream, NDProtocolHeader& protocolHeader );

private:
	NDBool c2lsLoginReqDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );
	//NDBool c2lPlayerOfflineNtyDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader );
};

#endif
