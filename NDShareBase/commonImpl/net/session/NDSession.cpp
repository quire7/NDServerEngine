#include "net/session/NDSession.h"

#include "NDShareBaseEnums.h"
#include "thread/NDMutex.h"
#include "memory/NDByteBuffer.h"
#include "memory/NDByteBufferPool.h"


#include "net/session/NDSessionManager.h"




_NDSHAREBASE_BEGIN

PParsePacketFun NDSession::s_pParsePacket = NULL;

NDSession::NDSession( NDUint32 nSessionID ) : m_pSocket(NULL), m_nSessionID( nSessionID ), m_nSocketSendBuf( 0 ), m_nProtocolType(ND_UINT8_MAX), m_nEventFlag( EIOEventType_Invalid ), 
											m_bAlive( NDFalse ), m_bSendPipe( NDFalse ), m_nRecvPacketNum( 0 ), m_nSendPacketNum( 0 ),
											m_nRecvPacketBytes( 0 ), m_nSendPacketBytes( 0 ), m_pSendBuffer( NULL), m_pRecvBuffer( NULL )
{
	m_pSendDataMutex = new NDMutexLock;
	m_pRecvDataMutex = new NDMutexLock;
}

NDSession::~NDSession()
{
	if ( NULL != m_pSendBuffer )
	{
		NDByteBufferPool::getInstance()->destroyByteBuffer( m_pSendBuffer );
		m_pSendBuffer = NULL;
	}
	if ( NULL != m_pRecvBuffer )
	{
		NDByteBufferPool::getInstance()->destroyByteBuffer( m_pRecvBuffer );
		m_pRecvBuffer = NULL;
	}

	SAFE_DELETE( m_pSendDataMutex );
	SAFE_DELETE( m_pRecvDataMutex );
}


NDBool NDSession::initSession()
{
	if ( NULL == m_pSocket )	return NDFalse;

	m_nSocketSendBuf	= m_pSocket->getSendBufferSize();
	if ( 0 == m_nSocketSendBuf )
	{
		return NDFalse;
	}

	m_nProtocolType		= ND_UINT8_MAX;
	m_nEventFlag		= 0;
	m_bAlive			= NDTrue;
	m_bSendPipe			= NDTrue;

	m_nRecvPacketNum	= 0;
	m_nSendPacketNum	= 0;
	m_nRecvPacketBytes	= 0;
	m_nSendPacketBytes	= 0;

	if ( NULL == m_pSendBuffer )
	{
		m_pSendBuffer = NDByteBufferPool::getInstance()->createByteBuffer();
	}

	if ( NULL == m_pRecvBuffer )
	{
		m_pRecvBuffer = NDByteBufferPool::getInstance()->createByteBuffer();
	}

	return setSocketOptions();
}

NDBool NDSession::releaseSession()
{
	if ( NULL == m_pSocket )	return NDFalse;

	m_pSocket->close();

	m_nSocketSendBuf	= 0;

	m_nProtocolType		= ND_UINT8_MAX;
	m_nEventFlag		= 0;
	m_bAlive			= NDFalse;
	m_bSendPipe			= NDFalse;

	m_nRecvPacketNum	= 0;
	m_nSendPacketNum	= 0;
	m_nRecvPacketBytes	= 0;
	m_nSendPacketBytes	= 0;

	if ( NULL != m_pSendBuffer )
	{
		NDByteBufferPool::getInstance()->destroyByteBuffer( m_pSendBuffer );
		m_pSendBuffer = NULL;
	}
	if ( NULL != m_pRecvBuffer )
	{
		NDByteBufferPool::getInstance()->destroyByteBuffer( m_pRecvBuffer );
		m_pRecvBuffer = NULL;
	}

	return NDTrue;
}

NDBool NDSession::setSocketOptions()
{
	if ( NULL == m_pSocket )	return NDFalse;

	//set reuse addr;
	if ( NDFalse == m_pSocket->setReuseAddr() )
	{
		return NDFalse;
	}

	//set no block;
	if ( NDFalse == m_pSocket->setNonblocking() )
	{
		return NDFalse;
	}

	//set no delay;
	if ( NDFalse == m_pSocket->setDisableBuffering() )
	{
		return NDFalse;
	}

	return NDTrue;
}


NDSocket* NDSession::getSocket()
{
	return m_pSocket;
}

SOCKET NDSession::getSOCKET() const
{
	if ( NULL == m_pSocket )	return INVALID_SOCKET;

	return m_pSocket->getSOCKET();
}

const char* NDSession::getPeerIP() const
{
	if ( NULL == m_pSocket )	return NULL;

	return m_pSocket->getPeerIP();
}

NDUint16 NDSession::getPeerPort() const
{
	if ( NULL == m_pSocket )	return 0;

	return m_pSocket->getPeerPort();
}

NDBool NDSession::isValidAddr() const
{
	if ( NULL == m_pSocket )	return NDFalse;

	return m_pSocket->isValidAddr();
}


NDBool NDSession::isConnected() const
{
	if ( NULL == m_pSocket )	return NDFalse;

	return m_bAlive;
}

NDBool NDSession::sendData( const NDByteBuffer& refMsgBuf )
{
	if ( NULL == m_pSocket ||  NDFalse == m_bAlive || NDFalse == m_bSendPipe )	return NDFalse;
	if ( NULL == m_pSendBuffer )	return NDFalse;

	if ( NULL == m_pSendDataMutex )	return NDFalse;
	NDGuardLock locker( *m_pSendDataMutex );

	NDUint32 nSendDataSize = refMsgBuf.GetDataSize();
	if ( 0 == nSendDataSize )
	{
		return NDFalse;
	}

	if ( nSendDataSize != m_pSendBuffer->WriteBuffer( refMsgBuf ) )
	{
		return NDFalse;
	}

	m_nSendPacketBytes += nSendDataSize;
	++m_nSendPacketNum;
	//设置SEND标志;
	registerWriteEvent();

	return NDTrue;
}

NDBool NDSession::sendData( const char* pszBuf, NDUint32 nSize )
{
	if ( NULL == m_pSocket ||  NDFalse == m_bAlive || NDFalse == m_bSendPipe )	return NDFalse;
	if ( NULL == pszBuf || 0 == nSize || NULL == m_pSendBuffer )				return NDFalse;


	if ( NULL == m_pSendDataMutex )	return NDFalse;
	NDGuardLock locker( *m_pSendDataMutex );

	if ( nSize != m_pSendBuffer->WriteBuffer( pszBuf, nSize ) )
	{	
		return NDFalse;
	}

	m_nSendPacketBytes += nSize;
	++m_nSendPacketNum;

	//设置SEND标志;
	registerWriteEvent();

	return NDTrue;
}

NDBool NDSession::handleRead()
{
	if ( NULL == m_pSocket ||  NDFalse == m_bAlive || NULL == m_pRecvBuffer )	return NDFalse;

	if ( NULL == m_pRecvDataMutex )	return NDFalse;
	NDGuardLock locker( *m_pRecvDataMutex );

	NDUint32 nCanRecvBytes = m_pSocket->getRecvBufBytes();
	if ( nCanRecvBytes == 0 )	
	{	//优雅关闭SOCKET的条件;(这个地方是否合适有待讨论);
		handleClose();
		return NDFalse;
	}
	if ( nCanRecvBytes > m_pRecvBuffer->GetSpaceSize() )
	{	//重新分配空间;
		m_pRecvBuffer->ReSet( m_pRecvBuffer->GetCapacitySize() + nCanRecvBytes );
	}
	
	NDUint32 nRefRealRecvLen		= 0;
	NDUint32 nRecvBufferDataSize	= m_pRecvBuffer->GetDataSize();
	if ( NDFalse == m_pSocket->recv( ( m_pRecvBuffer->ReadBuffer() + nRecvBufferDataSize ) , nCanRecvBytes, nRefRealRecvLen ) )
	{
		if ( 0 == nRefRealRecvLen )
		{	//优雅关闭SOCKET的条件;
			handleClose();
		}
		else
		{	//先关闭SEND通道;
			shutdownSendPipe();
		}
		return NDFalse;
	}

	if ( nRefRealRecvLen > 0 )
	{
		m_nRecvPacketBytes += nRefRealRecvLen;
		m_pRecvBuffer->SetWriteBufSize( nRefRealRecvLen );

		parseRecvData();
	}
	if ( nRefRealRecvLen == nCanRecvBytes )
	{
		m_nEventFlag &= (~EIOEventType_Read);
	}

	return NDTrue;
}

NDBool NDSession::handleWrite()
{
	if ( NULL == m_pSocket || NDFalse == m_bAlive || NDFalse == m_bSendPipe || NULL == m_pSendBuffer )	return NDFalse;

	if ( NULL == m_pSendDataMutex )	return NDFalse;
	NDGuardLock locker( *m_pSendDataMutex );

	NDUint32 nSendBufferSize = m_pSendBuffer->GetDataSize();
	if ( 0 == nSendBufferSize )
	{
		unregisterWriteEvent();

		return NDTrue;
	}

	if ( nSendBufferSize > m_nSocketSendBuf )
	{	//要发送的数据大于m_pSocket的发送缓存区大小;
		nSendBufferSize = m_nSocketSendBuf;
	}
	
	if ( NDFalse == m_pSocket->send( m_pSendBuffer->ReadBuffer(), nSendBufferSize, nSendBufferSize ) )
	{
		//先关闭SEND通道;
		shutdownSendPipe();
		return NDFalse;
	}

	if ( nSendBufferSize > 0 )
	{
		m_pSendBuffer->SetReadBufSize( nSendBufferSize );

		if ( 0 == m_pSendBuffer->GetDataSize() )
		{
			unregisterWriteEvent();
		}
	}

	////方案二(循环发送所有数据);
	//do 
	//{
	//	if ( nSendBufferSize > m_nSocketSendBuf )
	//	{	//要发送的数据大于m_pSocket的发送缓存区大小;
	//		nSendBufferSize = m_nSocketSendBuf;
	//	}

	//	if ( NDFalse == m_pSocket->send( m_pSendBuffer->ReadBuffer(), nSendBufferSize, nSendBufferSize ) )
	//	{
	//		//先关闭SEND通道;
	//		shutdownSendPipe();
	//		return NDFalse;
	//	}

	//	if ( nSendBufferSize > 0 )
	//	{
	//		m_pSendBuffer->SetReadBufSize( nSendBufferSize );
	//		nSendBufferSize = m_pSendBuffer->GetDataSize();
	//	}
	//} while ( nSendBufferSize > 0 );

	////方案三(循环发送所有数据,优化了方案二);
	//NDUint32 nTotalRealSendBufferSize = 0;
	//do 
	//{
	//	if ( nSendBufferSize > m_nSocketSendBuf )
	//	{	//要发送的数据大于m_pSocket的发送缓存区大小;
	//		nSendBufferSize = m_nSocketSendBuf;
	//	}

	//	NDUint32 nRealSendBufferSize = 0;
	//	if ( NDFalse == m_pSocket->send( m_pSendBuffer->ReadBuffer(), nSendBufferSize, nRealSendBufferSize ) )
	//	{
	//		//先关闭SEND通道;
	//		shutdownSendPipe();
	//		return NDFalse;
	//	}

	//	if ( nRealSendBufferSize > 0 )
	//	{
	//		nTotalRealSendBufferSize += nRealSendBufferSize;

	//		m_pSendBuffer->SetReadBufSize( nSendBufferSize );
	//	}

	//	if ( nSendBufferSize == nRealSendBufferSize )
	//	{
	//		nSendBufferSize = m_pSendBuffer->GetDataSize();
	//	}
	//} while ( ( nSendBufferSize > 0 ) && ( nTotalRealSendBufferSize < m_nSocketSendBuf ) );

	return NDTrue;
}

NDBool NDSession::handleClose()
{
	if ( NULL == m_pSocket || NDFalse == m_bAlive ) return NDFalse;

	//设置为无效的SESSION,目的是为移除事件监听和删除自己;
	NDSessionManager::getInstance()->setInvalidWorkSession( m_pSocket->getSOCKET() );

	m_pSocket->close();
	m_bAlive = NDFalse;

	return NDTrue;
}

NDBool NDSession::parseRecvData()
{
	if ( NDFalse == m_bAlive || NULL == s_pParsePacket || NULL == m_pRecvDataMutex || NULL == m_pRecvBuffer )	
	{
		return NDFalse;
	}

	NDGuardLock locker(*m_pRecvDataMutex);

	NDParseSessionDataEx parseSessionDataEx;
	parseSessionDataEx.m_nProtocolType	= m_nProtocolType;
	parseSessionDataEx.m_nSessionID		= m_nSessionID;
	
	if ( NDFalse == (*s_pParsePacket)( *m_pRecvBuffer, parseSessionDataEx ) )
	{
		parseSessionDataEx.m_nErrorCode;
		handleClose();
		return NDFalse;
	}

	m_nRecvPacketNum += parseSessionDataEx.m_nParsePacket;

	//NDProDataHead	dataHeader;

	//NDUint32 nHeadTypeSize = sizeof( dataHeader );
	//memset( &dataHeader, 0, nHeadTypeSize );


	//NDGuardLock locker(*m_pRecvDataMutex);
	//NDUint32 nRecvDataSize = m_pRecvBuffer->GetDataSize();

	//if ( nRecvDataSize <= nHeadTypeSize )
	//{	//recv data less than header size;
	//	return NDTrue;
	//}

	//NDBool bRet = NDTrue;

	////recv data large than header size 
	//NDUint32 nUint32Size = sizeof(NDUint32); //sizeof(m_nSessionID)

	//do
	//{
	//	m_pRecvBuffer->SetRollBackFlag();
	//	//read header;
	//	m_pRecvBuffer->ReadIntactBuffer( (char*)&dataHeader, nHeadTypeSize );
	//	if ( dataHeader.m_nBodySize > MAX_MSGPACKET_SIZE )
	//	{	//beyond max packet size;
	//		handleClose();
	//		bRet = NDFalse;
	//		break;
	//	}

	//	if ( ( nRecvDataSize - nHeadTypeSize ) < dataHeader.m_nBodySize )
	//	{	// 数据不足组成一个包;
	//		m_pRecvBuffer->RollBack();
	//		m_pRecvBuffer->CancelRollBack();
	//		break;
	//	}
	//	// body size is enough so that can make up a packet;
	//	if ( (dataHeader.m_nBitWise & ND_PDHMSG_PROTO_NUM) != ND_PDHMSG_PROTO_NUM )
	//	{	//it's not my protocol, disconnect session;
	//		handleClose();
	//		bRet = NDFalse;
	//		break;
	//	}

	//	++m_nRecvPacketNum;

	//	//// check protocol ID;
	//	//if ( m_bServer && !IsSessionProtocolID( dataHeader ) )
	//	//{	//it's not my protocol, disconnect session;
	//	//	handleClose();
	//	//	bRet = NDFalse;
	//	//	break;
	//	//}

	//	////it's my protocol;
	//	NDByteBuffer* pParseBuffer = NDByteBufferPool::getInstance()->createByteBuffer();
	//	if ( NULL == pParseBuffer )
	//	{
	//		m_pRecvBuffer->RollBack();
	//		m_pRecvBuffer->CancelRollBack();
	//		bRet = NDFalse;
	//		break;
	//	}

	//	pParseBuffer->WriteBuffer( (const char*)&m_nSessionID, nUint32Size );
	//	pParseBuffer->WriteBuffer( *m_pRecvBuffer, dataHeader.m_nBodySize );
	//	//set have readbuffer size;
	//	m_pRecvBuffer->SetReadBufSize( dataHeader.m_nBodySize );
	//	m_pRecvBuffer->CancelRollBack();

	//	if ( NDFalse == parsePacketAndPutQueue( dataHeader, pParseBuffer ) )
	//	{
	//		NDByteBufferPool::getInstance()->releaseByteBuffer( pParseBuffer );
	//		//是否要断开连接有待考虑;
	//		bRet = NDFalse;
	//		break;
	//	}

	//	nRecvDataSize = m_pRecvBuffer->GetDataSize();
	//	memset( &dataHeader, 0, nHeadTypeSize );

	//} while ( nRecvDataSize > nHeadTypeSize );

	//return bRet;
	return NDTrue;
}

NDBool NDSession::shutdownSendPipe()
{
	if ( m_bSendPipe == NDFalse )	return NDTrue;
	if ( NULL == m_pSocket )		return NDFalse;
	
	m_bSendPipe	= NDFalse;
	m_pSocket->shutdownSendPipe();

	return NDTrue;
}

//NDBool NDSession::parsePacketAndPutQueue( const NDProDataHead& refDataHeader, NDByteBuffer* pParseBuffer )
//{
//	if ( NULL == pParseBuffer )
//	{
//		return NDFalse;
//	}
//
//	if ( (refDataHeader.m_nBitWise & ND_PDHMSG_COMPRESSION) == ND_PDHMSG_COMPRESSION )
//	{	//加密包需要另外生成一个buf传进入函数;
//		NDByteBuffer* pDecryptBuf = NDByteBufferPool::getInstance()->createByteBuffer();
//		if ( NULL == pDecryptBuf )
//		{	//丢包了;
//			return NDFalse;
//		}
//		if ( NDFalse == NDProtocolPacket::parsePacket( *pDecryptBuf, *pParseBuffer, refDataHeader ) )
//		{
//			NDByteBufferPool::getInstance()->releaseByteBuffer( pDecryptBuf );
//			return NDFalse;
//		}
//
//		//put msg into  msg dispose deque;
//		NDServerTask::getInstance()->putQueue( pDecryptBuf );
//
//		NDByteBufferPool::getInstance()->releaseByteBuffer( pParseBuffer );
//	}
//	else
//	{	//先处理特殊的PING包;
//		NDUint32 nUint32Size= sizeof(NDUint32); //sizeof(m_nSessionID)
//		if ( refDataHeader.m_nBodySize == nUint32Size )
//		{	//dispose ping protocol (ping协议不加解密.不压缩.不计算crc);
//			NDUint32 nProtocolID	= 0;
//			char* pszBuf			= pParseBuffer->ReadBuffer();
//
//			memcpy( &nProtocolID, pszBuf + nUint32Size, refDataHeader.m_nBodySize );
//			//if ( CMD_PING_PROTOCOL == nProtocolID )
//			//{
//			//	HandlePing( *pParseBuffer );
//			//	NDByteBufferPool::getInstance()->releaseByteBuffer( pParseBuffer );
//			//	return NDTrue;
//			//}
//		}
//
//		if ( NDFalse == NDProtocolPacket::parsePacket( *pParseBuffer, *pParseBuffer, refDataHeader ) )
//		{
//			return NDFalse;
//		}
//
//		//put msg into  msg dispose deque;
//		NDServerTask::getInstance()->putQueue( pParseBuffer );
//	}
//
//	return NDTrue;	
//}

//NDBool NDSession::isSessionProtocolID( const NDProDataHead& refDataHeader )
//{
//	refDataHeader;
//	return NDTrue;
//}

NDBool NDSession::registerWriteEvent()
{
	if ( NULL == m_pSocket || NDFalse == m_bAlive || NDFalse == m_bSendPipe || NULL == m_pSendBuffer )	return NDFalse;

	NDUint32 nSendBufferSize = m_pSendBuffer->GetDataSize();
	if ( 0 == nSendBufferSize )
	{
		return NDFalse;
	}

	if ( (m_nEventFlag & EIOEventType_Write) != EIOEventType_Write )
	{
		m_nEventFlag |= EIOEventType_Write;

		NDSessionManager::getInstance()->registerWriteEvent( m_pSocket->getSOCKETRef() );
	}

	return NDTrue;
}

NDBool NDSession::unregisterWriteEvent()
{
	if ( NULL == m_pSocket || NDFalse == m_bAlive || NDFalse == m_bSendPipe || NULL == m_pSendBuffer )	return NDFalse;

	NDUint32 nSendBufferSize = m_pSendBuffer->GetDataSize();
	if ( 0 != nSendBufferSize )
	{
		return NDFalse;
	}

	m_nEventFlag &= (~EIOEventType_Write);

	NDSessionManager::getInstance()->unregisterWriteEvent( m_pSocket->getSOCKETRef() );

	return NDTrue;
}

void NDSession::setParsePacketFun( PParsePacketFun pParsePacketFun )
{
	if ( NULL == pParsePacketFun || NULL != s_pParsePacket )
	{
		return;
	}
		
	s_pParsePacket = pParsePacketFun;
}


_NDSHAREBASE_END

