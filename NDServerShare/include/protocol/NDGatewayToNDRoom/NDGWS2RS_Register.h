#ifndef __SERVER_SHARE_GWS2RS_REGISTER_H__
#define __SERVER_SHARE_GWS2RS_REGISTER_H__


#include "NDServerShare.h"
#include "protocol/NDCmdProtocolS2S.h"

class NDGWS2RS_Register_Req : public NDProtocol
{
public:
	NDUint16		m_nServerID;
	NDSocketAddress	m_netAddress;
public:
	NDGWS2RS_Register_Req() : NDProtocol( CMDP_NDGWS2RS_Register_Req ) { clear(); }
	~NDGWS2RS_Register_Req() {}

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

	NDUint16	getSize() const
	{ 
		return sizeof(m_unProtocolID) + sizeof(m_nServerID) + sizeof(m_netAddress);
	}

	void   clear() 
	{
		m_nServerID	= 0;
		m_netAddress.clear();
	}
};

class NDGWS2RS_Register_Res : public NDProtocol
{
public:
	NDUint32 m_nErrorCode;
public:
	NDGWS2RS_Register_Res() : NDProtocol( CMDP_NDGWS2RS_Register_Res ) { clear(); }
	~NDGWS2RS_Register_Res() {}

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
