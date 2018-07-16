#ifndef __SERVER_SHARE_RS2GS_REGISTER_H__
#define __SERVER_SHARE_RS2GS_REGISTER_H__


#include "NDServerShare.h"
#include "protocol/NDCmdProtocolS2S.h"

class NDRS2GS_Register_Req : public NDProtocol
{
public:
	NDUint16		m_nRoomServerID;
	NDSocketAddress	m_netAddress;
public:
	NDRS2GS_Register_Req() : NDProtocol( CMDP_NDRS2GS_Register_Req ) { clear(); }
	~NDRS2GS_Register_Req() {}

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

	NDUint16	getSize() const
	{ 
		return sizeof(m_unProtocolID) + sizeof(m_nRoomServerID) + sizeof(m_netAddress);
	}

	void   clear() 
	{
		m_nRoomServerID	= 0;
		m_netAddress.clear();
	}
};

class NDRS2GS_Register_Res : public NDProtocol
{
public:
	NDUint32 m_nErrorCode;
public:
	NDRS2GS_Register_Res() : NDProtocol( CMDP_NDRS2GS_Register_Res ) { clear(); }
	~NDRS2GS_Register_Res() {}

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

	NDUint16	getSize() const
	{ 
		return sizeof(m_unProtocolID) + sizeof(m_nErrorCode);
	}

	void   clear() 
	{
		m_nErrorCode = 0;
	}
};

#endif
