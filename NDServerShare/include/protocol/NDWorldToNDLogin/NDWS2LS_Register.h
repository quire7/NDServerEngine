#ifndef __SERVER_SHARE_ND_WS2LS_REGISTER_H__
#define __SERVER_SHARE_ND_WS2LS_REGISTER_H__

#include "NDServerShare.h"
#include "protocol/NDCmdProtocolS2S.h"

class NDWS2LS_Register_Req : public NDProtocol
{
public:
	NDUint16		m_nWorldID;
	char			m_szWorldName[ND_WORLDNAME_LEN];
	NDSocketAddress	m_netAddress;

public:
	NDWS2LS_Register_Req() : NDProtocol( CMDP_NDWS2LS_Register_Req ) { clear(); }
	~NDWS2LS_Register_Req() {}

	NDBool serialize( NDOStream& stream )
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )

		NDOSTREAM_WRITE( stream, &m_nWorldID, sizeof(m_nWorldID) )
		NDOSTREAM_WRITE( stream, m_szWorldName, ND_WORLDNAME_LEN )
		NDOSTREAM_WRITE( stream, &m_netAddress, sizeof(m_netAddress) )

		return NDTrue;
	}

	NDBool deserialize( NDIStream& stream )
	{
		NDISTREAM_READ( stream, &m_nWorldID, sizeof(m_nWorldID) )
		NDISTREAM_READ( stream, m_szWorldName, ND_WORLDNAME_LEN )
		NDISTREAM_READ( stream, &m_netAddress, sizeof(m_netAddress) )
					
		return NDTrue;
	}

	NDUint16	getSize() const 
	{ 
		return sizeof(m_unProtocolID) + sizeof(m_szWorldName) + sizeof(m_netAddress);
	}

	void clear()
	{
		m_nWorldID		= 0;
		memset( m_szWorldName, 0, ND_WORLDNAME_LEN );
		m_netAddress.clear();
	}

};

class NDLS2WS_Register_Res : public NDProtocol
{
public:
	NDUint32	m_uiErrCode;

public:
	NDLS2WS_Register_Res() : NDProtocol( CMDP_NDLS2WS_Register_Res ) { clear(); }
	~NDLS2WS_Register_Res() {}

	NDBool serialize( NDOStream& stream )
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )

		NDOSTREAM_WRITE( stream, &m_uiErrCode, sizeof(m_uiErrCode) )

		return NDTrue;
	}

	NDBool deserialize( NDIStream& stream )
	{
		NDISTREAM_READ( stream, &m_uiErrCode, sizeof(m_uiErrCode) )
		return NDTrue;
	}

	NDUint16	getSize() const 
	{ 
		return sizeof(m_unProtocolID) + sizeof(m_uiErrCode);
	}

	void clear()
	{
		m_uiErrCode = 0;
	}
};


class NDWS2LS_LoadFactor_Nty : public NDProtocol
{
public:
	NDLoadFactor	m_stLoadFactor;
	NDUint32		m_nBestGateSessionID;
	NDSocketAddress	m_stBestGateAddr;

public:
	NDWS2LS_LoadFactor_Nty() : NDProtocol( CMDP_NDWS2LS_LoadFactor_Nty ) { clear(); }
	~NDWS2LS_LoadFactor_Nty() {}

	NDBool serialize( NDOStream& stream )
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )
		NDOSTREAM_WRITE( stream, &m_stLoadFactor, sizeof(m_stLoadFactor) )

		NDOSTREAM_WRITE( stream, &m_nBestGateSessionID, sizeof(m_nBestGateSessionID) )
		NDOSTREAM_WRITE( stream, &m_stBestGateAddr, sizeof(m_stBestGateAddr) )

		
		return NDTrue;
	}

	NDBool deserialize( NDIStream& stream )
	{
		NDISTREAM_READ( stream, &m_stLoadFactor, sizeof(m_stLoadFactor) )

		NDISTREAM_READ( stream, &m_nBestGateSessionID, sizeof(m_nBestGateSessionID) )
		NDISTREAM_READ( stream, &m_stBestGateAddr, sizeof(m_stBestGateAddr) )
		
		return NDTrue;
	}

	NDUint16	getSize() const
	{ 
		return sizeof(m_unProtocolID) + sizeof(m_stLoadFactor) + sizeof(m_nBestGateSessionID) + sizeof(m_stBestGateAddr);
	}

	void clear()
	{
		m_nBestGateSessionID = 0;

		m_stLoadFactor.clear();
		m_stBestGateAddr.clear();
	}
};


#endif

