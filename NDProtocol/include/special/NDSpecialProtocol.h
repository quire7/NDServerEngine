#ifndef __PROTOCOL_ND_SPECIAL_PROTOCOL_H__
#define __PROTOCOL_ND_SPECIAL_PROTOCOL_H__

#include "NDProtocolBase.h"

class NDPingReqProtocol : public NDProtocol
{
public:
	NDPingReqProtocol() : NDProtocol( CMDP_PING_Req ) {}
	~NDPingReqProtocol() {}

	NDBool serialize( NDOStream& stream )
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )
		return NDTrue;
	}

	NDBool deserialize( NDIStream& )
	{	
		//stream;	//Ϊ������ warning C4100:"name" :δ���õ��β�;
		return NDTrue;
	}

	NDUint16	getSize() const 
	{ 
		return sizeof(m_unProtocolID);
	}
};

class NDPingResProtocol : public NDProtocol
{
public:
	NDPingResProtocol() : NDProtocol( CMDP_PING_Res ) {}
	~NDPingResProtocol() {}

	NDBool serialize( NDOStream& stream )
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )
		return NDTrue;
	}

	NDBool deserialize( NDIStream&  )
	{	
		//stream;	//Ϊ������ warning C4100:"name" :δ���õ��β�;
		return NDTrue;
	}

	NDUint16	getSize() const 
	{ 
		return sizeof(m_unProtocolID);
	}
};


class NDDisconnectNtyProtocol : public NDProtocol
{	
public:
	NDUint8			m_nDisconnectionType;		//�Ͽ�����;
public:
	NDDisconnectNtyProtocol() : NDProtocol( CMDP_DISCONNECT_NOTIFY ) {}
	~NDDisconnectNtyProtocol() {}

	NDBool serialize( NDOStream& stream )
	{
		NDOSTREAM_WRITE( stream, &m_unProtocolID, sizeof(m_unProtocolID) )

		//�˴����ѽṹд������,���Э��Ƚ�����,�ڲ�Э��;
		//���崦��NDSessionManagerImpl::popCommonDisconnectNtyProtocol;

		return NDTrue;
	}

	NDBool deserialize( NDIStream& stream )
	{
		NDISTREAM_READ( stream, &m_nDisconnectionType, sizeof(m_nDisconnectionType) )
		return NDTrue;
	}

	NDUint16	getSize() const 
	{ 
		return sizeof(m_unProtocolID) + sizeof(m_nDisconnectionType);
	}

	void	clear()
	{
		m_nDisconnectionType = 0;
	}
};


#endif
