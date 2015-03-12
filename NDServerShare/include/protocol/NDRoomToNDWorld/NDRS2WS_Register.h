#ifndef __SERVER_SHARE_RS2WS_REGISTER_H__
#define __SERVER_SHARE_RS2WS_REGISTER_H__


#include "NDServerShare.h"
#include "protocol/NDCmdProtocolS2S.h"

class NDRS2WS_Register_Req : public NDProtocol
{
public:
	NDRS2WS_Register_Req() : NDProtocol( CMDP_NDRS2WS_Register_Req ) { clear(); }
	~NDRS2WS_Register_Req() {}

	NDBool serialize(NDOStream& stream)
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )

		NDOSTREAM_WRITE( stream, &m_nRoomServerID, sizeof(m_nRoomServerID) )
		NDOSTREAM_WRITE( stream, &m_netAddress, sizeof(m_netAddress) )

		return NDTrue;
	}

	NDBool deserialize(NDIStream& stream)
	{
		NDISTREAM_READ( stream, &m_nRoomServerID, sizeof(m_nRoomServerID) )
		NDISTREAM_READ( stream, &m_netAddress, sizeof(m_netAddress) )

		return NDTrue;
	}

	void   clear() 
	{
		m_nRoomServerID	= 0;
		m_netAddress.clear();
	}
public:
	NDUint16		m_nRoomServerID;
	NDSocketAddress	m_netAddress;
};

class NDRS2WS_Register_Res : public NDProtocol
{
public:
	NDRS2WS_Register_Res() : NDProtocol( CMDP_NDRS2WS_Register_Res ) {}
	~NDRS2WS_Register_Res() {}

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
