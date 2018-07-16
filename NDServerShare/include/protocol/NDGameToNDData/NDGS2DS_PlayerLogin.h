#ifndef __SERVER_SHARE_GS2DS_PLAYER_LOGIN_H__
#define __SERVER_SHARE_GS2DS_PLAYER_LOGIN_H__

#include "NDServerShare.h"
#include "protocol/NDCmdProtocolS2S.h"

class NDGS2DS_PlayerLogin_Req : public NDProtocol
{
public:
	NDUint32		m_nGameServerID;
	NDUint64		m_nPlayerGUID;

public:
	NDGS2DS_PlayerLogin_Req() : NDProtocol( CMDP_NDGS2DS_PlayerLogin_Req ) { clear(); }
	~NDGS2DS_PlayerLogin_Req() {}

	NDBool serialize( NDOStream& stream )
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )

		NDOSTREAM_WRITE( stream, &m_nGameServerID, sizeof(m_nGameServerID) )
		NDOSTREAM_WRITE( stream, &m_nPlayerGUID, sizeof(m_nPlayerGUID) )

		return NDTrue;
	}

	NDBool deserialize( NDIStream& stream )
	{
		NDISTREAM_READ( stream, &m_nGameServerID, sizeof(m_nGameServerID) )
		NDISTREAM_READ( stream, &m_nPlayerGUID, sizeof(m_nPlayerGUID) )

		return NDTrue;
	}

	NDUint16	getSize() const
	{ 
		return sizeof(m_unProtocolID) + sizeof(m_nGameServerID) + sizeof(m_nPlayerGUID);
	}

	void clear()
	{
		m_nGameServerID		= 0;
		m_nPlayerGUID		= 0;
	}
};

class NDGS2DS_PlayerLogin_Res : public NDProtocol
{
public:
	NDUint32		m_nErrorCode;
	NDUint64		m_nPlayerGUID;

public:
	NDGS2DS_PlayerLogin_Res() : NDProtocol( CMDP_NDGS2DS_PlayerLogin_Res ) { clear(); }
	~NDGS2DS_PlayerLogin_Res() {}

	NDBool serialize( NDOStream& stream )
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )

		NDOSTREAM_WRITE( stream, &m_nErrorCode, sizeof(m_nErrorCode) )
		NDOSTREAM_WRITE( stream, &m_nPlayerGUID, sizeof(m_nPlayerGUID) )
		return NDTrue;
	}

	NDBool deserialize( NDIStream& stream )
	{
		NDISTREAM_READ( stream, &m_nErrorCode, sizeof(m_nErrorCode) )
		NDISTREAM_READ( stream, &m_nPlayerGUID, sizeof(m_nPlayerGUID) )
		return NDTrue;
	}

	NDUint16	getSize() const
	{
		return sizeof(m_unProtocolID) + sizeof(m_nErrorCode) + sizeof(m_nPlayerGUID);
	}

	void   clear() 
	{
		m_nErrorCode = 0;
	}
};

#endif
