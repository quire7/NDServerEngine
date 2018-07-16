#ifndef __PROTOCOL_ND_SPECIAL_PROTOCOL_H__
#define __PROTOCOL_ND_SPECIAL_PROTOCOL_H__

#include "NDProtocolBase.h"

class NDPingReqProtocol : public NDProtocol
{
public:
	NDPingReqProtocol() : NDProtocol( CMDP_PING_Req ) {}
	~NDPingReqProtocol() {}

	NDBool serialize( NDOStream& stream )
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )
		return NDTrue;
	}

	NDBool deserialize( NDIStream& )
	{	
		//stream;	//为了消除 warning C4100:"name" :未引用的形参;
		return NDTrue;
	}

	NDUint16	getSize() const 
	{ 
		return sizeof(m_unProtocolID);
	}
};

class NDPingResProtocol : public NDProtocol
{
public:
	NDPingResProtocol() : NDProtocol( CMDP_PING_Res ) {}
	~NDPingResProtocol() {}

	NDBool serialize( NDOStream& stream )
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )
		return NDTrue;
	}

	NDBool deserialize( NDIStream&  )
	{	
		//stream;	//为了消除 warning C4100:"name" :未引用的形参;
		return NDTrue;
	}

	NDUint16	getSize() const 
	{ 
		return sizeof(m_unProtocolID);
	}
};


class NDDisconnectNtyProtocol : public NDProtocol
{	
public:
	NDUint8			m_nDisconnectionType;		//断开类型;
public:
	NDDisconnectNtyProtocol() : NDProtocol( CMDP_DISCONNECT_NOTIFY ) {}
	~NDDisconnectNtyProtocol() {}

	NDBool serialize( NDOStream& stream )
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )

		//此处不把结构写入流中,这个协议比较特殊,内部协议;
		//具体处理看NDSessionManagerImpl::popCommonDisconnectNtyProtocol;

		return NDTrue;
	}

	NDBool deserialize( NDIStream& stream )
	{
		NDISTREAM_READ( stream, &m_nDisconnectionType, sizeof(m_nDisconnectionType) )
		return NDTrue;
	}

	NDUint16	getSize() const 
	{ 
		return sizeof(m_unProtocolID) + sizeof(m_nDisconnectionType);
	}

	void	clear()
	{
		m_nDisconnectionType = 0;
	}
};


#endif
