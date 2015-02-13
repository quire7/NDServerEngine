/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\stream\NDProtocolPacket.h
	file base:	NDProtocolPacket
	purpose:	protocol packet;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef	__SHARE_BASE_ND_PROTOCOL_PACKET_H__
#define __SHARE_BASE_ND_PROTOCOL_PACKET_H__


#include "NDTypes.h"
#include "authentic/NDCrypt.h"

_NDSHAREBASE_BEGIN

#ifndef ND_PDHMSG
#define ND_PDHMSG
#define ND_PDHMSG_ENCRYPT		0X0001		//data encrypt;
#define ND_PDHMSG_COMPRESSION	0X0002		//data compression;
#define ND_PDHMSG_CRC32			0X0004		//CRC CODE;
#define ND_PDHMSG_PROTO_NUM		0X0008		//protocol number;
#endif

struct NDProDataHead
{
	NDUint8		m_nBitWise;					//composition bit;
	NDUint32	m_nBodySize;				//protocol body size;
	NDUint32	m_nProtocolID;				//protocol ID;
};

class NDByteBuffer;
class NDProtocol;

class NDProtocolPacket
{
public:
	static void		setEncrptKey( const NDUint8* szKey, NDUint32 nLen );
	static NDBool	composePacket( NDByteBuffer& outMsgBuf, NDProtocol& protocol, NDUint8 nProDataHeadBitWise );
	static NDBool	parsePacket( NDByteBuffer& decryptBuf, NDByteBuffer& encryptBuf, const NDProDataHead& dataHeader );
	static NDBool	parsePacket( NDByteBuffer& recvMsgOriginalBuf, NDParseSessionDataEx& refNDParseSessionDataEx );

private:
	//解析包并放入处理队列;
	static NDBool	parsePacketAndPutQueue( NDByteBuffer* pParseBuffer, const NDProDataHead& refDataHeader );
private:
	static NDCrypt m_sEncrypt;
};

_NDSHAREBASE_END
#endif

