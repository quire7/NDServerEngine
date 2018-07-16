#ifndef __PROTOCOL_ND_C2LS_PROTOCOL_H__
#define __PROTOCOL_ND_C2LS_PROTOCOL_H__

#include "NDProtocolBase.h"


class NDC2LS_Login_Req : public NDProtocol
{
public:
	NDUint16	nPacketIndex;
	char		m_szBuf[32];
public:
	NDC2LS_Login_Req() : NDProtocol( CMDP_NDC2LS_Login_Req ) {}
	~NDC2LS_Login_Req() {}

	NDBool serialize( NDOStream& stream )
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )
		NDOSTREAM_WRITE( stream, &nPacketIndex, sizeof(nPacketIndex) )
		NDOSTREAM_WRITE( stream, m_szBuf, sizeof(m_szBuf) )
		return NDTrue;
	}

	NDBool deserialize( NDIStream& stream )
	{	
		NDISTREAM_READ( stream, &nPacketIndex, sizeof(nPacketIndex) )
		NDISTREAM_READ( stream, m_szBuf, sizeof(m_szBuf) )
		m_szBuf[sizeof(m_szBuf) - 1] = '\0';
		return NDTrue;
	}

	NDUint16	getSize() const 
	{ 
		return sizeof(m_unProtocolID) + sizeof(nPacketIndex) + sizeof(m_szBuf);
	}

	void clear()
	{
		nPacketIndex = 0;
		memset( m_szBuf, 0, sizeof(m_szBuf) );
	}
};

class NDC2LS_Login_Res : public NDProtocol
{
public:
	NDUint16	nPacketIndex;
	char		m_szBuf[32];
public:
	NDC2LS_Login_Res() : NDProtocol( CMDP_NDC2LS_Login_Res ) {}
	~NDC2LS_Login_Res() {}

	NDBool serialize( NDOStream& stream )
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )
		NDOSTREAM_WRITE( stream, &nPacketIndex, sizeof(nPacketIndex) )
		NDOSTREAM_WRITE( stream, m_szBuf, sizeof(m_szBuf) )
		return NDTrue;
	}

	NDBool deserialize( NDIStream& stream )
	{	
		NDISTREAM_READ( stream, &nPacketIndex, sizeof(nPacketIndex) )
		NDISTREAM_READ( stream, m_szBuf, sizeof(m_szBuf) )
		m_szBuf[sizeof(m_szBuf) - 1] = '\0';
		return NDTrue;
	}

	NDUint16	getSize() const 
	{ 
		return sizeof(m_unProtocolID) + sizeof(nPacketIndex) + sizeof(m_szBuf);
	}

	void clear()
	{
		nPacketIndex = 0;
		memset( m_szBuf, 0, sizeof(m_szBuf) );
	}
};


#endif
