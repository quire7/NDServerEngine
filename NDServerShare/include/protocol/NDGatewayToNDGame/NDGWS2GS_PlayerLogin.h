#ifndef __SERVER_SHARE_GWS2GS_PLAYERLOGIN_H__
#define __SERVER_SHARE_GWS2GS_PLAYERLOGIN_H__


#include "NDServerShare.h"
#include "protocol/NDCmdProtocolS2S.h"

//class NDGWS2GS_Register_Req : public NDProtocol
//{
//public:
//	NDUint16		m_nServerID;
//	NDSocketAddress	m_netAddress;
//public:
//	NDGWS2GS_Register_Req() : NDProtocol( CMDP_NDGWS2GS_Register_Req ) { clear(); }
//	~NDGWS2GS_Register_Req() {}
//
//	NDBool serialize(NDOStream& stream)
//	{
//		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )
//
//		NDOSTREAM_WRITE( stream, &m_nServerID, sizeof(m_nServerID) )
//		NDOSTREAM_WRITE( stream, &m_netAddress, sizeof(m_netAddress) )
//
//		return NDTrue;
//	}
//
//	NDBool deserialize(NDIStream& stream)
//	{
//		NDISTREAM_READ( stream, &m_nServerID, sizeof(m_nServerID) )
//		NDISTREAM_READ( stream, &m_netAddress, sizeof(m_netAddress) )
//
//		return NDTrue;
//	}
//
//	NDUint16	getSize() const
//	{ 
//		return sizeof(m_unProtocolID) + sizeof(m_nServerID) + sizeof(m_netAddress);
//	}
//
//	void   clear() 
//	{
//		m_nServerID	= 0;
//		m_netAddress.clear();
//	}
//};

class NDGS2GTWS_PlayerLoginDataSuccess_Nty : public NDProtocol
{
public:
	NDUint32	m_nGameServerID;
	NDUint64	m_nPlayerGUID;
public:
	NDGS2GTWS_PlayerLoginDataSuccess_Nty() : NDProtocol( CMDP_NDGS2GTWS_PlayerLoginDataSuccess_Nty ) { clear(); }
	~NDGS2GTWS_PlayerLoginDataSuccess_Nty() {}

	NDBool serialize(NDOStream& stream)
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )

		NDOSTREAM_WRITE( stream, &m_nGameServerID, sizeof(m_nGameServerID) )
		NDOSTREAM_WRITE( stream, &m_nPlayerGUID, sizeof(m_nPlayerGUID) )
		return NDTrue;
	}

	NDBool deserialize(NDIStream& stream)
	{
		NDISTREAM_READ( stream, &m_nGameServerID, sizeof(m_nGameServerID) )
		NDISTREAM_READ( stream, &m_nPlayerGUID, sizeof(m_nPlayerGUID) )

		return NDTrue;
	}

	NDUint16	getSize() const
	{ 
		return sizeof(m_unProtocolID) + sizeof(m_nGameServerID) + sizeof(m_nPlayerGUID);
	}

	void   clear() 
	{
		m_nGameServerID = 0;
		m_nPlayerGUID	= 0;
	}
};

#endif
