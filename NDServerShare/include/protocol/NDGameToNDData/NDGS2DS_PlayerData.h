#ifndef __SERVER_SHARE_GS2DS_PLAYER_DATA_H__
#define __SERVER_SHARE_GS2DS_PLAYER_DATA_H__

#include "NDServerShare.h"
#include "protocol/NDCmdProtocolS2S.h"

class NDGS2DS_UpdatePlayerMain_Nty : public NDProtocol
{
public:
	NDPlayerMain	m_playerMain;

public:
	NDGS2DS_UpdatePlayerMain_Nty() : NDProtocol( CMDP_NDGS2DS_UpdatePlayerMain_Nty ) { clear(); }
	~NDGS2DS_UpdatePlayerMain_Nty() {}

	NDBool serialize( NDOStream& stream )
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )

		NDOSTREAM_WRITE( stream, &m_playerMain, sizeof(m_playerMain) )

		return NDTrue;
	}

	NDBool deserialize( NDIStream& stream )
	{
		NDISTREAM_READ( stream, &m_playerMain, sizeof(m_playerMain) )

		return NDTrue;
	}

	NDUint16	getSize() const
	{ 
		return sizeof(m_unProtocolID) + sizeof(m_playerMain);
	}

	void clear()
	{
		m_playerMain.clear();
	}
};



class NDDS2GS_SendPlayerMain_Nty : public NDProtocol
{
public:
	NDPlayerMain	m_playerMain;

public:
	NDDS2GS_SendPlayerMain_Nty() : NDProtocol( CMDP_NDDS2GS_SendPlayerMain_Nty ) { clear(); }
	~NDDS2GS_SendPlayerMain_Nty() {}

	NDBool serialize( NDOStream& stream )
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )

		NDOSTREAM_WRITE( stream, &m_playerMain, sizeof(m_playerMain) )
		return NDTrue;
	}

	NDBool deserialize( NDIStream& stream )
	{
		NDISTREAM_READ( stream, &m_playerMain, sizeof(m_playerMain) )
		return NDTrue;
	}

	NDUint16	getSize() const
	{
		return sizeof(m_unProtocolID) + sizeof(m_playerMain);
	}

	void   clear() 
	{
		m_playerMain.clear();
	}
};

#endif
