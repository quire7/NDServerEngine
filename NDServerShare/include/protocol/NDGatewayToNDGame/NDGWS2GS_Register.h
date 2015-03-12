#ifndef __SERVER_SHARE_GWS2GS_REGISTER_H__
#define __SERVER_SHARE_GWS2GS_REGISTER_H__


#include "NDServerShare.h"
#include "protocol/NDCmdProtocolS2S.h"

class NDGWS2GS_Register_Req : public NDProtocol
{
public:
	NDGWS2GS_Register_Req() : NDProtocol( CMDP_NDGWS2GS_Register_Req ) { clear(); }
	~NDGWS2GS_Register_Req() {}

	NDBool serialize(NDOStream& stream)
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )

		NDOSTREAM_WRITE( stream, &m_nServerID, sizeof(m_nServerID) )
		NDOSTREAM_WRITE( stream, &m_netAddress, sizeof(m_netAddress) )

		return NDTrue;
	}

	NDBool deserialize(NDIStream& stream)
	{
		NDISTREAM_READ( stream, &m_nServerID, sizeof(m_nServerID) )
		NDISTREAM_READ( stream, &m_netAddress, sizeof(m_netAddress) )

		return NDTrue;
	}

	void   clear() 
	{
		m_nServerID	= 0;
		m_netAddress.clear();
	}
public:
	NDUint16		m_nServerID;
	NDSocketAddress	m_netAddress;
};

class NDGWS2GS_Register_Res : public NDProtocol
{
public:
	NDGWS2GS_Register_Res() : NDProtocol( CMDP_NDGWS2GS_Register_Res ) {}
	~NDGWS2GS_Register_Res() {}

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
