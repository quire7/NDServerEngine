#include "net/stream/NDProtocolPacket.h"

#include "NDCLibFun.h"
#include "NDShareBaseGlobal.h"

#include "memory/NDByteBuffer.h"
#include "memory/NDByteBufferPool.h"
#include "net/stream/NDProtocol.h"
#include "net/session/NDSession.h"

#include "net/process/NDServerTask.h"

#include "NDOStreamImpl.h"

#include "file/NDLog.h"


_NDSHAREBASE_BEGIN

NDCrypt NDProtocolPacket::s_encrypt;
NDUint8 NDProtocolPacket::s_nMaxSessionType;
NDUint16 NDProtocolPacket::s_specialProtocolArray[2]={0};
vector<NDUint16> NDProtocolPacket::s_protocolNumVec;


void NDProtocolPacket::setEncrptKey( const NDUint8* szKey, NDUint32 nLen )
{
	s_encrypt.setKey( szKey, nLen );
}

NDBool NDProtocolPacket::composePacket( NDByteBuffer& outMsgBuf, NDProtocol& protocol, NDUint16 nProDataHeadBitWise )
{
	NDUint16 nProtocolOrigianlSize = protocol.getSize();
	if ( nProtocolOrigianlSize >= MAX_MSGPACKET_SIZE )
	{
		NDLOG_ERROR( " Protocol:[%u] beyond MAX_MSGPACKET_SIZE:[%u], curSize:[%u]. ", protocol.m_unProtocolID, MAX_MSGPACKET_SIZE, nProtocolOrigianlSize);
		return NDFalse;
	}

	NDBool bCompression = ( ( nProDataHeadBitWise & ND_PDHMSG_COMPRESSION ) == ND_PDHMSG_COMPRESSION ); 
	NDBool bEncrypt		= ( ( nProDataHeadBitWise & ND_PDHMSG_ENCRYPT ) == ND_PDHMSG_ENCRYPT );
	NDBool bCrc			= ( ( nProDataHeadBitWise & ND_PDHMSG_CRC32 ) == ND_PDHMSG_CRC32 );

	NDProDataHead dataHeader;
	NDUint16 nDataHeadSize = (NDUint16)sizeof(dataHeader);
	memset( &dataHeader, 0, nDataHeadSize );

	outMsgBuf.writeBuffer( (const char*)&dataHeader, nDataHeadSize );

	NDUint32 nCRC32			= 0;
	NDUint16 nOriginalSize	= 0;
	NDUint16 nExtraHeadSize = 0;
	
	NDUint16 nCRC32MemSize		= (NDUint16)sizeof(nCRC32);
	NDUint16 nOriginalMemSize	= (NDUint16)sizeof(nOriginalSize);
	if ( NDTrue == bCrc )
	{
		outMsgBuf.writeBuffer( (const char*)&nCRC32, nCRC32MemSize );
		nExtraHeadSize += nCRC32MemSize;
	}

	if ( NDTrue == bCompression )
	{
		outMsgBuf.writeBuffer( (const char*)&nOriginalSize, nOriginalMemSize );
		nExtraHeadSize += nOriginalMemSize;
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
		if ( NDFalse == protocol.serialize(out) )
		{
			NDByteBufferPool::getInstance()->destroyByteBuffer( pProtocolBuf );
			return NDFalse;
		}

		nOriginalSize = pProtocolBuf->getDataSize();
		NDUint32 nPlenty = nOriginalSize + 512;
		if (nPlenty > outMsgBuf.getSpaceSize())
		{
			outMsgBuf.reSet(nPlenty);
		}

		NDUint32 nCompression;
		NDBool bRet = NDShareBaseGlobal::compress( (char*)(outMsgBuf.readBuffer() + nDataHeadSize + nExtraHeadSize), &nCompression, (const char*)pProtocolBuf->readBuffer(), nOriginalSize );
		if ( NDFalse == bRet )
		{
			NDByteBufferPool::getInstance()->destroyByteBuffer( pProtocolBuf );
			return NDFalse;
		}
		outMsgBuf.setWriteBufSize(nCompression);
		dataHeader.m_nBodySize = (nExtraHeadSize + nCompression);			
		dataHeader.m_nBitWise |= ND_PDHMSG_COMPRESSION;

		NDByteBufferPool::getInstance()->destroyByteBuffer( pProtocolBuf );
	}
	else
	{
		NDOStreamImpl out(outMsgBuf);
		if ( NDFalse == protocol.serialize(out) )
		{
			return NDFalse;
		}

		dataHeader.m_nBodySize = outMsgBuf.getDataSize() - nDataHeadSize;
	}

	if ( dataHeader.m_nBodySize < nExtraHeadSize )
	{	//nCRC32MemSize+nOriginalMemSize > bodysize;
		return NDFalse;
	}

	// data encrypt;
	if ( NDTrue == bEncrypt )
	{
		s_encrypt.init();
		s_encrypt.encryptSend( (NDUint8*)(outMsgBuf.readBuffer() + nDataHeadSize + nExtraHeadSize), dataHeader.m_nBodySize - nExtraHeadSize );
		dataHeader.m_nBitWise |= ND_PDHMSG_ENCRYPT;
	}

	//CRC CODE;
	if ( NDTrue == bCrc )
	{
		dataHeader.m_nBitWise |= ND_PDHMSG_CRC32;
		nCRC32 = NDShareBaseGlobal::crc32( nCRC32, (const char*)(outMsgBuf.readBuffer() + nDataHeadSize + nExtraHeadSize), dataHeader.m_nBodySize - nExtraHeadSize );
	}

	//set protocol flag;
	dataHeader.m_nBitWise |= ND_PDHMSG_PROTO_NUM;

	//SET PROTOCOLID;
	dataHeader.m_nProtocolID = protocol.m_unProtocolID;

	memcpy( outMsgBuf.readBuffer(), &dataHeader, nDataHeadSize );
	if ( NDTrue == bCrc )
	{
		memcpy( outMsgBuf.readBuffer() + nDataHeadSize, &nCRC32, nCRC32MemSize );
	}
	if ( NDTrue == bCompression )
	{
		NDUint32 nExtendCrcMemSize = 0;
		if ( NDTrue == bCrc )
		{
			nExtendCrcMemSize = nCRC32MemSize;
		}
		memcpy( outMsgBuf.readBuffer() + nDataHeadSize + nExtendCrcMemSize, &nOriginalSize, nOriginalMemSize );
	}

	return NDTrue;
}

NDBool NDProtocolPacket::parsePacket( NDByteBuffer& decryptBuf, NDByteBuffer& encryptBuf, const NDProDataHead& dataHeader )
{
	NDBool bCompression		= ( ( dataHeader.m_nBitWise & ND_PDHMSG_COMPRESSION ) == ND_PDHMSG_COMPRESSION ); 
	NDBool bEncrypt			= ( ( dataHeader.m_nBitWise & ND_PDHMSG_ENCRYPT ) == ND_PDHMSG_ENCRYPT );
	NDBool bCrc				= ( ( dataHeader.m_nBitWise & ND_PDHMSG_CRC32 ) == ND_PDHMSG_CRC32 );

	NDUint32 nSessionID		= 0;
	NDUint32 nSessionIDSize = sizeof(NDUint32);

	NDUint32 nRecvCRC		= 0;
	NDUint16 nOriginalSize	= 0;
	NDUint16 nExtraHeadSize = 0;

	NDUint16 nCRC32MemSize		= (NDUint16)sizeof(nRecvCRC);
	NDUint16 nOriginalMemSize	= (NDUint16)sizeof(nOriginalSize);
	if ( bCrc )
	{
		encryptBuf.readBuffer( (char*)&nSessionID, nSessionIDSize );

		char* pCurEncryptBuf = encryptBuf.readBuffer();
		memcpy( &nRecvCRC, pCurEncryptBuf, nCRC32MemSize);
		memcpy( pCurEncryptBuf, &nSessionID, nSessionIDSize);
		nExtraHeadSize += nCRC32MemSize;
	}

	if ( bCompression )
	{	//nSessionID+nOriginalSize+body-->>nSessionID+body;
		char* pCurEncryptBuf = encryptBuf.readBuffer();
		memcpy( &nSessionID, pCurEncryptBuf, nSessionIDSize );
		memcpy( &nOriginalSize, pCurEncryptBuf + nSessionIDSize, nOriginalMemSize);
		memcpy( pCurEncryptBuf + nOriginalMemSize, &nSessionID, nSessionIDSize);
		encryptBuf.setReadBufSize( nOriginalMemSize );	//注意这个地方;
		nExtraHeadSize += nOriginalMemSize;
	}

	if ( dataHeader.m_nBodySize < nExtraHeadSize )
	{
		return NDFalse;
	}

	char* pEncryptBuf = encryptBuf.readBuffer();
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
		s_encrypt.init();
		s_encrypt.decryptRecv( (NDUint8*)(pEncryptBuf + nSessionIDSize) , dataHeader.m_nBodySize - nExtraHeadSize );
	}

	//uncompress code;
	if ( bCompression )
	{
		NDUint32 nDecryptBufSize = decryptBuf.getCapacitySize();
		NDUint32 nNeedBufSize	 = (nOriginalSize + nSessionIDSize);
		if ( nDecryptBufSize < nNeedBufSize )
		{
			decryptBuf.reSet( nNeedBufSize );
		}
		decryptBuf.writeBuffer( pEncryptBuf, nSessionIDSize );

		NDUint32 nUnCompression;
		NDBool bRet = NDShareBaseGlobal::uncompress((char*)(decryptBuf.readBuffer()+nSessionIDSize), &nUnCompression, (const char*)(pEncryptBuf + nSessionIDSize), dataHeader.m_nBodySize - nExtraHeadSize );
		if ( NDTrue == bRet )
		{
			decryptBuf.setWriteBufSize(nUnCompression);
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

	NDUint32 nRecvDataSize = recvMsgOriginalBuf.getDataSize();

	if ( nRecvDataSize <= nHeadTypeSize )
	{	//recv data less than header size;
		return NDTrue;
	}

	//用refNDParseSessionDataEx.m_nParsePacket来判断是否是第一个包;
	NDBool bFirstProtocolPacket = (refNDParseSessionDataEx.m_nParsePacket == 0);
	refNDParseSessionDataEx.m_nParsePacket = 0;

	NDBool bRet = NDTrue;

	//recv data large than header size;
	NDUint32 nSessionIDMemSize = sizeof(refNDParseSessionDataEx.m_nSessionID);

	do
	{
		recvMsgOriginalBuf.setRollBackFlag();
		//read header;
		recvMsgOriginalBuf.readIntactBuffer( (char*)&dataHeader, nHeadTypeSize );
		if ( dataHeader.m_nBodySize >= MAX_MSGPACKET_SIZE )
		{	//beyond max packet size;
			NDLOG_ERROR( " Receive Protocol:[%u] beyond MAX_MSGPACKET_SIZE:[%u], curSize:[%u]. ", dataHeader.m_nProtocolID, MAX_MSGPACKET_SIZE, dataHeader.m_nBodySize);

			bRet = NDFalse;
			break;
		}

		if ( ( nRecvDataSize - nHeadTypeSize ) < dataHeader.m_nBodySize )
		{	// 数据不足组成一个包;
			recvMsgOriginalBuf.rollBack();
			recvMsgOriginalBuf.cancelRollBack();
			break;
		}
		// body size is enough so that can make up a packet;
		if ( (dataHeader.m_nBitWise & ND_PDHMSG_PROTO_NUM) != ND_PDHMSG_PROTO_NUM )
		{	//it's not my protocol, disconnect session;
			NDLOG_ERROR( " Receive Protocol:[%u] bitwise have not ND_PDHMSG_PROTO_NUM:[%u]. ", dataHeader.m_nProtocolID, ND_PDHMSG_PROTO_NUM );
			bRet = NDFalse;
			break;
		}

		// check protocol ID;
		if ( !bFirstProtocolPacket && !isSessionProtocolID( refNDParseSessionDataEx.m_nProtocolType, dataHeader.m_nProtocolID ) )
		{	//it's not my protocol, disconnect session;
			NDLOG_ERROR( " Receive Protocol:[%u] isn't my SessionProtocolType:[%u]. ", dataHeader.m_nProtocolID, refNDParseSessionDataEx.m_nProtocolType );

			bRet = NDFalse;
			break;
		}

		////it's my protocol;
		NDByteBuffer* pParseBuffer = NDByteBufferPool::getInstance()->createByteBuffer();
		if ( NULL == pParseBuffer )
		{
			recvMsgOriginalBuf.rollBack();
			recvMsgOriginalBuf.cancelRollBack();
			bRet = NDFalse;
			break;
		}

		pParseBuffer->writeBuffer( (const char*)&refNDParseSessionDataEx.m_nSessionID, nSessionIDMemSize );
		pParseBuffer->writeBuffer( recvMsgOriginalBuf, dataHeader.m_nBodySize );
		//set have readbuffer size;
		recvMsgOriginalBuf.setReadBufSize( dataHeader.m_nBodySize );
		recvMsgOriginalBuf.cancelRollBack();

		if ( NDFalse == parsePacketAndPutQueue( pParseBuffer, dataHeader ) )
		{
			NDByteBufferPool::getInstance()->destroyByteBuffer( pParseBuffer );
			//是否要断开连接有待考虑;
			bRet = NDFalse;
			break;
		}

		++refNDParseSessionDataEx.m_nParsePacket;

		nRecvDataSize = recvMsgOriginalBuf.getDataSize();
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
	{	
		if ( NDFalse == NDProtocolPacket::parsePacket( *pParseBuffer, *pParseBuffer, refDataHeader ) )
		{
			return NDFalse;
		}

		//put msg into  msg dispose deque;
		NDServerTask::getInstance()->putQueue( pParseBuffer );
	}

	return NDTrue;
}

NDBool NDProtocolPacket::isSessionProtocolID( NDUint8 nSessionProtocolType, NDUint16 nProtocolID )
{
	if ( nSessionProtocolType >= s_nMaxSessionType )
	{
		return NDFalse;
	}

	//先处理特殊情况;
	NDUint16 nSpecialProtocolStart	= s_specialProtocolArray[0];
	NDUint16 nSpecialProtocolEnd	= s_specialProtocolArray[1];
	if ( nProtocolID > nSpecialProtocolStart || nProtocolID < nSpecialProtocolEnd )
	{	
		return NDTrue;
	}

	NDUint32 nSize = (NDUint32)s_protocolNumVec.size();
	if ( nSize < (NDUint32)( (nSessionProtocolType + 1) * 2 ) )
	{
		return NDFalse;
	}

	NDUint16 nSessionProtocolStart	= s_protocolNumVec[2 * nSessionProtocolType];
	NDUint16 nSessionProtocolEnd	= s_protocolNumVec[2 * nSessionProtocolType + 1];
	if ( nProtocolID > nSessionProtocolStart || nProtocolID < nSessionProtocolEnd )
	{
		return NDTrue;
	}

	return NDFalse;
}

void NDProtocolPacket::setMaxSessionType(NDUint8 nMaxSessionType)
{
	s_nMaxSessionType = nMaxSessionType;
	s_protocolNumVec.resize( nMaxSessionType * 2 );
}

void NDProtocolPacket::setSpecialProtocol(NDUint16 nSpecialProtocolStart, NDUint16 nSpecialProtocolEnd)
{
	s_specialProtocolArray[0] = nSpecialProtocolStart;
	s_specialProtocolArray[1] = nSpecialProtocolEnd;
}

NDBool NDProtocolPacket::setDisposeSessionProtocol(NDUint8 nSessionType, NDUint16 nProtocolStart, NDUint16 nProtocolEnd)
{
	if ( nSessionType >= s_nMaxSessionType)
	{
		return NDFalse;
	}

	s_protocolNumVec[2 * nSessionType]		= nProtocolStart;
	s_protocolNumVec[2 * nSessionType + 1]	= nProtocolEnd;

	return NDTrue;
}


_NDSHAREBASE_END

