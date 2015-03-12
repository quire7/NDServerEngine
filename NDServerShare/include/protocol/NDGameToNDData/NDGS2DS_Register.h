#ifndef __SERVER_SHARE_GS2DS_REGISTER_H__
#define __SERVER_SHARE_GS2DS_REGISTER_H__

#include "NDServerShare.h"
#include "protocol/NDCmdProtocolS2S.h"

class NDGS2DS_Register_Req : public NDProtocol
{
public:
	NDUint16		m_nMapID;
	NDSocketAddress	m_netAddress;

public:
	NDGS2DS_Register_Req() : NDProtocol( CMDP_NDGS2DS_Register_Req ) { clear(); }
	~NDGS2DS_Register_Req() {}

	NDBool serialize( NDOStream& stream )
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )

		NDOSTREAM_WRITE( stream, &m_nMapID, sizeof(m_nMapID) )
		NDOSTREAM_WRITE( stream, &m_netAddress, sizeof(m_netAddress) )

		return NDTrue;
	}

	NDBool deserialize( NDIStream& stream )
	{
		NDISTREAM_READ( stream, &m_nMapID, sizeof(m_nMapID) )
		NDISTREAM_READ( stream, &m_netAddress, sizeof(m_netAddress) )

		return NDTrue;
	}

	void clear()
	{
		m_netAddress.clear();
	}
};

class NDGS2DS_Register_Res : public NDProtocol
{
public:
	NDUint32	m_nErrorCode;

public:
	NDGS2DS_Register_Res() : NDProtocol( CMDP_NDGS2DS_Register_Res ) { clear(); }
	~NDGS2DS_Register_Res() {}

	NDBool serialize( NDOStream& stream )
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )

		NDOSTREAM_WRITE( stream, &m_nErrorCode, sizeof(m_nErrorCode) )
		return NDTrue;
	}

	NDBool deserialize( NDIStream& stream )
	{
		NDISTREAM_READ( stream, &m_nErrorCode, sizeof(m_nErrorCode) )
		return NDTrue;
	}
};

#endif
