#include "net/stream/NDProtocolPacket.h"

#include "NDShareBaseGlobal.h"

#include "memory/NDByteBuffer.h"
#include "memory/NDByteBufferPool.h"
#include "net/stream/NDProtocol.h"
#include "net/session/NDSession.h"

#include "net/process/NDServerTask.h"

#include "NDOStreamImpl.h"


_NDSHAREBASE_BEGIN

NDCrypt NDProtocolPacket::m_sEncrypt;


void NDProtocolPacket::setEncrptKey( const NDUint8* szKey, NDUint32 nLen )
{
	m_sEncrypt.setKey( szKey, nLen );
}

NDBool NDProtocolPacket::composePacket( NDByteBuffer& outMsgBuf, NDProtocol& protocol, NDUint8 nProDataHeadBitWise )
{
	NDBool bCompression = ( ( nProDataHeadBitWise & ND_PDHMSG_COMPRESSION ) == ND_PDHMSG_COMPRESSION ); 
	NDBool bEncrypt		= ( ( nProDataHeadBitWise & ND_PDHMSG_ENCRYPT ) == ND_PDHMSG_ENCRYPT );
	NDBool bCrc			= ( ( nProDataHeadBitWise & ND_PDHMSG_CRC32 ) == ND_PDHMSG_CRC32 );

	NDProDataHead dataHeader;
	NDUint32 nDataHeadSize = (NDUint32)sizeof(dataHeader);
	memset( &dataHeader, 0, nDataHeadSize );

	outMsgBuf.WriteBuffer( (const char*)&dataHeader, nDataHeadSize );

	NDUint32 nCRC32			= 0;
	NDUint32 nOriginalSize	= 0;
	NDUint32 nExtraHeadSize = 0;
	NDUint32 nUint32Size	= sizeof(NDUint32);
	if ( NDTrue == bCrc )
	{
		outMsgBuf.WriteBuffer( (const char*)&nCRC32, nUint32Size );
		nExtraHeadSize += nUint32Size;
	}

	if ( NDTrue == bCompression )
	{
		outMsgBuf.WriteBuffer( (const char*)&nOriginalSize, nUint32Size );
		nExtraHeadSize += nUint32Size;
	}

	//COMPRESS
	if ( NDTrue == bCompression )
	{
		NDByteBuffer* pProtocolBuf = NDByteBufferPool::getInstance()->createByteBuffer();
		if ( NULL == pProtocolBuf )
		{
			return NDFalse;
		}
		NDOStreamImpl out( *pProtocolBuf );
		if ( NDFalse == protocol.Serialize(out) )
		{
			NDByteBufferPool::getInstance()->destroyByteBuffer( pProtocolBuf );
			return NDFalse;
		}

		nOriginalSize = pProtocolBuf->GetDataSize();
		NDUint32 nPlenty = nOriginalSize + 512;
		if (nPlenty > outMsgBuf.GetSpaceSize())
		{
			outMsgBuf.ReSet(nPlenty);
		}

		NDUint32 nCompression;
		NDBool bRet = NDShareBaseGlobal::compress( (char*)(outMsgBuf.ReadBuffer() + nDataHeadSize + nExtraHeadSize), &nCompression, (const char*)pProtocolBuf->ReadBuffer(), nOriginalSize );
		if ( NDFalse == bRet )
		{
			NDByteBufferPool::getInstance()->destroyByteBuffer( pProtocolBuf );
			return NDFalse;
		}
		outMsgBuf.SetWriteBufSize(nCompression);
		dataHeader.m_nBodySize = (nExtraHeadSize + nCompression);			
		dataHeader.m_nBitWise |= ND_PDHMSG_COMPRESSION;

		NDByteBufferPool::getInstance()->destroyByteBuffer( pProtocolBuf );
	}
	else
	{
		NDOStreamImpl out(outMsgBuf);
		if ( NDFalse == protocol.Serialize(out) )
		{
			return NDFalse;
		}

		dataHeader.m_nBodySize = outMsgBuf.GetDataSize() - nDataHeadSize;
	}

	// data encrypt;
	if ( NDTrue == bEncrypt )
	{
		m_sEncrypt.init();
		m_sEncrypt.encryptSend( (NDUint8*)(outMsgBuf.ReadBuffer() + nDataHeadSize + nExtraHeadSize), dataHeader.m_nBodySize - nExtraHeadSize );
		dataHeader.m_nBitWise |= ND_PDHMSG_ENCRYPT;
	}

	//CRC CODE;
	if ( NDTrue == bCrc )
	{
		dataHeader.m_nBitWise |= ND_PDHMSG_CRC32;
		nCRC32 = NDShareBaseGlobal::crc32( nCRC32, (const char*)(outMsgBuf.ReadBuffer() + nDataHeadSize + nExtraHeadSize), dataHeader.m_nBodySize - nExtraHeadSize );
	}

	//set protocol flag;
	dataHeader.m_nBitWise |= ND_PDHMSG_PROTO_NUM;

	//SET PROTOCOLID;
	dataHeader.m_nProtocolID = protocol.m_unProtocolID;

	memcpy( outMsgBuf.ReadBuffer(), &dataHeader, nDataHeadSize );
	if ( NDTrue == bCrc )
	{
		memcpy( outMsgBuf.ReadBuffer() + nDataHeadSize, &nCRC32, nUint32Size );
	}
	if ( NDTrue == bCompression )
	{
		memcpy( outMsgBuf.ReadBuffer() + nDataHeadSize + nUint32Size, &nOriginalSize, nUint32Size );
	}

	return NDTrue;
}

NDBool NDProtocolPacket::parsePacket( NDByteBuffer& decryptBuf, NDByteBuffer& encryptBuf, const NDProDataHead& dataHeader )
{
	NDBool bCompression		= ( ( dataHeader.m_nBitWise & ND_PDHMSG_COMPRESSION ) == ND_PDHMSG_COMPRESSION ); 
	NDBool bEncrypt			= ( ( dataHeader.m_nBitWise & ND_PDHMSG_ENCRYPT ) == ND_PDHMSG_ENCRYPT );
	NDBool bCrc				= ( ( dataHeader.m_nBitWise & ND_PDHMSG_CRC32 ) == ND_PDHMSG_CRC32 );

	NDUint32 nSessionIDSize = sizeof(NDUint32);

	NDUint32 nUint32Size	= sizeof(NDUint32);
	NDUint32 nRecvCRC		= 0;
	NDUint32 nOriginalSize	= 0;
	NDUint32 nExtraHeadSize = 0;
	if ( bCrc )
	{
		NDUint32 nSessionID = 0;
		encryptBuf.ReadBuffer( (char*)&nSessionID, nSessionIDSize );

		char* pCurEncryptBuf = encryptBuf.ReadBuffer();
		memcpy( &nRecvCRC, pCurEncryptBuf, nUint32Size);
		memcpy( pCurEncryptBuf, &nSessionID, nSessionIDSize);
		nExtraHeadSize += nUint32Size;
	}

	if ( bCompression )
	{
		NDUint32 nSessionID = 0;
		encryptBuf.ReadBuffer( (char*)&nSessionID, nSessionIDSize );

		char* pCurEncryptBuf = encryptBuf.ReadBuffer();
		memcpy( &nOriginalSize, pCurEncryptBuf, nUint32Size);
		memcpy( pCurEncryptBuf, &nSessionID, nSessionIDSize);
		nExtraHeadSize += nUint32Size;
	}

	if ( dataHeader.m_nBodySize < nExtraHeadSize )
	{
		return NDFalse;
	}

	char* pEncryptBuf = encryptBuf.ReadBuffer();
	//crc code;
	if ( bCrc )
	{
		NDUint32 nCRC = 0;
		nCRC = NDShareBaseGlobal::crc32( nCRC, (const char*)(pEncryptBuf + nSessionIDSize) , dataHeader.m_nBodySize - nExtraHeadSize );
		if ( nCRC != nRecvCRC )
		{
			return NDFalse;
		}
	}

	//decrypt code;
	if ( bEncrypt )
	{
		m_sEncrypt.init();
		m_sEncrypt.decryptRecv( (NDUint8*)(pEncryptBuf + nSessionIDSize) , dataHeader.m_nBodySize - nExtraHeadSize );
	}

	//uncompress code;
	if ( bCompression )
	{
		NDUint32 nDecryptBufSize = decryptBuf.GetCapacitySize();
		NDUint32 nNeedBufSize	 = (nOriginalSize + nSessionIDSize);
		if ( nDecryptBufSize < nNeedBufSize )
		{
			decryptBuf.ReSet( nNeedBufSize );
		}
		decryptBuf.WriteBuffer( pEncryptBuf, nSessionIDSize );

		NDUint32 nUnCompression;
		NDBool bRet = NDShareBaseGlobal::uncompress((char*)(decryptBuf.ReadBuffer()+nSessionIDSize), &nUnCompression, (const char*)(pEncryptBuf + nSessionIDSize), dataHeader.m_nBodySize - nExtraHeadSize );
		if ( NDTrue == bRet )
		{
			decryptBuf.SetWriteBufSize(nUnCompression);
		}
		else
		{
			return NDFalse;
		}
	}

	return NDTrue;
}

NDBool NDProtocolPacket::parsePacket( NDByteBuffer& recvMsgOriginalBuf, NDParseSessionDataEx& refNDParseSessionDataEx )
{
	NDProDataHead	dataHeader;

	NDUint32 nHeadTypeSize = sizeof( dataHeader );
	memset( &dataHeader, 0, nHeadTypeSize );

	NDUint32 nRecvDataSize = recvMsgOriginalBuf.GetDataSize();

	if ( nRecvDataSize <= nHeadTypeSize )
	{	//recv data less than header size;
		return NDTrue;
	}

	NDBool bRet = NDTrue;

	//recv data large than header size 
	NDUint32 nUint32Size = sizeof(NDUint32); //sizeof(m_nSessionID)

	do
	{
		recvMsgOriginalBuf.SetRollBackFlag();
		//read header;
		recvMsgOriginalBuf.ReadIntactBuffer( (char*)&dataHeader, nHeadTypeSize );
		if ( dataHeader.m_nBodySize > MAX_MSGPACKET_SIZE )
		{	//beyond max packet size;
			//handleClose();
			bRet = NDFalse;
			break;
		}

		if ( ( nRecvDataSize - nHeadTypeSize ) < dataHeader.m_nBodySize )
		{	// 数据不足组成一个包;
			recvMsgOriginalBuf.RollBack();
			recvMsgOriginalBuf.CancelRollBack();
			break;
		}
		// body size is enough so that can make up a packet;
		if ( (dataHeader.m_nBitWise & ND_PDHMSG_PROTO_NUM) != ND_PDHMSG_PROTO_NUM )
		{	//it's not my protocol, disconnect session;
			//handleClose();
			bRet = NDFalse;
			break;
		}

		//// check protocol ID;
		//if ( m_bServer && !IsSessionProtocolID( dataHeader ) )
		//{	//it's not my protocol, disconnect session;
		//	handleClose();
		//	bRet = NDFalse;
		//	break;
		//}

		////it's my protocol;
		NDByteBuffer* pParseBuffer = NDByteBufferPool::getInstance()->createByteBuffer();
		if ( NULL == pParseBuffer )
		{
			recvMsgOriginalBuf.RollBack();
			recvMsgOriginalBuf.CancelRollBack();
			bRet = NDFalse;
			break;
		}

		pParseBuffer->WriteBuffer( (const char*)&refNDParseSessionDataEx.m_nSessionID, nUint32Size );
		pParseBuffer->WriteBuffer( recvMsgOriginalBuf, dataHeader.m_nBodySize );
		//set have readbuffer size;
		recvMsgOriginalBuf.SetReadBufSize( dataHeader.m_nBodySize );
		recvMsgOriginalBuf.CancelRollBack();

		if ( NDFalse == parsePacketAndPutQueue( pParseBuffer, dataHeader ) )
		{
			NDByteBufferPool::getInstance()->destroyByteBuffer( pParseBuffer );
			//是否要断开连接有待考虑;
			bRet = NDFalse;
			break;
		}

		++refNDParseSessionDataEx.m_nParsePacket;

		nRecvDataSize = recvMsgOriginalBuf.GetDataSize();
		memset( &dataHeader, 0, nHeadTypeSize );

	} while ( nRecvDataSize > nHeadTypeSize );

	return bRet;
}

NDBool NDProtocolPacket::parsePacketAndPutQueue( NDByteBuffer* pParseBuffer, const NDProDataHead& refDataHeader )
{
	if ( NULL == pParseBuffer )	return NDFalse;

	if ( (refDataHeader.m_nBitWise & ND_PDHMSG_COMPRESSION) == ND_PDHMSG_COMPRESSION )
	{	//加密包需要另外生成一个buf传进入函数;
		NDByteBuffer* pDecryptBuf = NDByteBufferPool::getInstance()->createByteBuffer();
		if ( NULL == pDecryptBuf )
		{	//丢包了;
			return NDFalse;
		}
		if ( NDFalse == NDProtocolPacket::parsePacket( *pDecryptBuf, *pParseBuffer, refDataHeader ) )
		{
			NDByteBufferPool::getInstance()->destroyByteBuffer( pDecryptBuf );
			return NDFalse;
		}

		//put msg into  msg dispose deque;
		NDServerTask::getInstance()->putQueue( pDecryptBuf );

		NDByteBufferPool::getInstance()->destroyByteBuffer( pParseBuffer );
	}
	else
	{	//先处理特殊的PING包;
		NDUint32 nUint32Size= sizeof(NDUint32); //sizeof(m_nSessionID)
		if ( refDataHeader.m_nBodySize == nUint32Size )
		{	//dispose ping protocol (ping协议不加解密.不压缩.不计算crc);
			NDUint32 nProtocolID	= 0;
			char* pszBuf			= pParseBuffer->ReadBuffer();

			memcpy( &nProtocolID, pszBuf + nUint32Size, refDataHeader.m_nBodySize );
			//if ( CMD_PING_PROTOCOL == nProtocolID )
			//{
			//	HandlePing( *pParseBuffer );
			//	NDByteBufferPool::getInstance()->releaseByteBuffer( pParseBuffer );
			//	return NDTrue;
			//}
		}

		if ( NDFalse == NDProtocolPacket::parsePacket( *pParseBuffer, *pParseBuffer, refDataHeader ) )
		{
			return NDFalse;
		}

		//put msg into  msg dispose deque;
		NDServerTask::getInstance()->putQueue( pParseBuffer );
	}

	return NDTrue;
}

_NDSHAREBASE_END

