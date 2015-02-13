#ifndef __PROTOCOL_ND_SPECIAL_PROTOCOL_H__
#define __PROTOCOL_ND_SPECIAL_PROTOCOL_H__

#include "NDProtocolBase.h"

class NDPingProtocol : public NDProtocol
{	
public:
	NDPingProtocol() : NDProtocol( CMDP_PING ) {}
	~NDPingProtocol() {}

	NDBool Serialize( NDOStream& rOStream )
	{
		rOStream.Write( (void*)&m_unProtocolID, sizeof(m_unProtocolID) );
		return NDTrue;
	}

	NDBool Deserialize( NDIStream&  )
	{	
		//rIStream;	//为了消除 warning C4100:"name" :未引用的形参;
		return NDTrue;
	}
};


class NDDisconnectNtyProtocol : public NDProtocol
{	
public:
	NDDisconnectNtyProtocol() : NDProtocol( CMDP_DISCONNECT_NOTIFY ) {}
	~NDDisconnectNtyProtocol() {}

	NDBool Serialize( NDOStream& rOStream )
	{
		rOStream.Write( (void*)&m_unProtocolID, sizeof(m_unProtocolID) );
		return NDTrue;
	}

	NDBool Deserialize( NDIStream&  )
	{
		//rIStream;	//为了消除 warning C4100:"name" :未引用的形参;
		return NDTrue;
	}
};


#endif
