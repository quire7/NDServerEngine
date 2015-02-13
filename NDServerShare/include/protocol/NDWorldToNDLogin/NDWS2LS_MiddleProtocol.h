#ifndef __SERVER_SHARE_ND_C2L_MIDDLE_PROTOCOL_H__
#define __SERVER_SHARE_ND_C2L_MIDDLE_PROTOCOL_H__

#include "NDServerShare.h"

class NDL2CS_ToGateCheck_Nty : public NDProtocol
{
public:
	NDL2CS_ToGateCheck_Nty() : NDProtocol( CMD_NDL2CS_ToGateCheck_Nty ) { Clear(); }
	~NDL2CS_ToGateCheck_Nty() {}

	NDBool Serialize( NDOStream& stream )
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )
		
		NDOSTREAM_WRITE( stream, m_szSessionKey, ND_SHA1_LEN )

		NDOSTREAM_WRITE( stream, &m_nAccountID, sizeof(m_nAccountID) )
		NDOSTREAM_WRITE( stream, &m_nGateSessionID, sizeof(m_nGateSessionID) )

		NDOSTREAM_WRITE( stream, &m_netAddress, sizeof(m_netAddress) )

		return NDTrue;
	}

	NDBool Deserialize( NDIStream& stream )
	{
		NDISTREAM_READ( stream, m_szSessionKey, ND_SHA1_LEN )

		NDISTREAM_READ( stream, &m_nAccountID, sizeof(m_nAccountID) )
		NDISTREAM_READ( stream, &m_nGateSessionID, sizeof(m_nGateSessionID) )

		NDISTREAM_READ( stream, &m_netAddress, sizeof(m_netAddress) )

		return NDTrue;
	}

	void Clear()
	{
		memset( m_szSessionKey, 0, ND_SHA1_LEN );

		m_nAccountID		= 0;
		m_nGateSessionID	= 0;

		m_netAddress.clear();
	}

public:
	char			m_szSessionKey[ND_SHA1_LEN];
	NDUint32		m_nAccountID;

	NDUint32		m_nGateSessionID;				//GATESERVER CONNECT CENTERSERVER'S SESSION
	NDNetAddress	m_netAddress;					//GATESERVER address
};

class NDL2CS_ToGateForceUserOffline_Nty : public NDProtocol
{
public:
	NDL2CS_ToGateForceUserOffline_Nty() : NDProtocol( CMD_NDL2CS_ToGateForceUserOffline_Nty ) { Clear(); }
	~NDL2CS_ToGateForceUserOffline_Nty() {}

	NDBool Serialize( NDOStream& stream )
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )

		NDOSTREAM_WRITE( stream, &m_nAccountID, sizeof(m_nAccountID) )

		return NDTrue;
	}

	NDBool Deserialize( NDIStream& stream )
	{
		NDISTREAM_READ( stream, &m_nAccountID, sizeof(m_nAccountID) )

		return NDTrue;
	}

	void Clear()
	{
		m_nAccountID		= 0;
	}

public:
	NDUint32		m_nAccountID;
};


class NDCS2L_PlayerOffline_Nty : public NDProtocol
{
public:
	NDCS2L_PlayerOffline_Nty() : NDProtocol( CMD_NDCS2L_PlayerOffline_Nty ) { Clear(); }
	~NDCS2L_PlayerOffline_Nty() {}

	NDBool Serialize( NDOStream& stream )
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )

		NDOSTREAM_WRITE( stream, &m_nAccountID, sizeof(m_nAccountID) )

		return NDTrue;
	}

	NDBool Deserialize( NDIStream& stream )
	{
		NDISTREAM_READ( stream, &m_nAccountID, sizeof(m_nAccountID) )

		return NDTrue;
	}

	void Clear()
	{
		m_nAccountID		= 0;
	}

public:
	NDUint32		m_nAccountID;
};

#endif

