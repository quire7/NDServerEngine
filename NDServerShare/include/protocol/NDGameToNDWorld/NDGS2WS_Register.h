#ifndef __SERVER_SHARE_GS2WS_REGISTER_H__
#define __SERVER_SHARE_GS2WS_REGISTER_H__


#include "NDServerShare.h"
#include "protocol/NDCmdProtocolS2S.h"

class NDGS2WS_Register_Req : public NDProtocol
{
public:
	NDGS2WS_Register_Req() : NDProtocol( CMDP_NDGS2WS_Register_Req ) { clear(); }
	~NDGS2WS_Register_Req() {}

	NDBool serialize(NDOStream& stream)
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )

		NDOSTREAM_WRITE( stream, &m_nMapID, sizeof(m_nMapID) )
		NDOSTREAM_WRITE( stream, &m_netAddress, sizeof(m_netAddress) )

		return NDTrue;
	}

	NDBool deserialize(NDIStream& stream)
	{
		NDISTREAM_READ( stream, &m_nMapID, sizeof(m_nMapID) )
		NDISTREAM_READ( stream, &m_netAddress, sizeof(m_netAddress) )

		return NDTrue;
	}

	void   clear() 
	{
		m_nMapID	= 0;
		m_netAddress.clear();
	}
public:
	NDUint16		m_nMapID;
	NDSocketAddress	m_netAddress;
};

class NDGS2WS_Register_Res : public NDProtocol
{
public:
	NDGS2WS_Register_Res() : NDProtocol( CMDP_NDGS2WS_Register_Res ) {}
	~NDGS2WS_Register_Res() {}

	NDBool serialize(NDOStream& stream)
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )

		NDOSTREAM_WRITE( stream, &m_nErrorCode, sizeof(m_nErrorCode) )

		return NDTrue;
	}

	NDBool deserialize(NDIStream& stream)
	{
		NDISTREAM_READ( stream, &m_nErrorCode, sizeof(m_nErrorCode) )

		return NDTrue;
	}

public:
	NDUint32 m_nErrorCode;
};

#endif
