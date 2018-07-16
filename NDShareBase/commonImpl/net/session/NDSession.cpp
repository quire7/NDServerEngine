#include "net/session/NDSession.h"

#include "NDShareBaseEnums.h"
#include "thread/NDMutex.h"
#include "memory/NDByteBuffer.h"
#include "memory/NDByteBufferPool.h"


#include "net/session/NDSessionManager.h"




_NDSHAREBASE_BEGIN

PParsePacketFun NDSession::s_pParsePacket = NULL;

NDSession::NDSession( NDUint32 nSessionID ) : m_pSocket( NULL ), m_nSessionID( nSessionID ), m_nSocketSendBuf( 0 ), 
											m_nDisconnectionType( ESessionDisconnectionType_NULL ), m_nProtocolType( ND_UINT8_MAX ), m_nEventFlag( EIOEventType_Invalid ), 
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

	m_nDisconnectionType= ESessionDisconnectionType_NULL;
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

	m_nDisconnectionType= ESessionDisconnectionType_NULL;
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

	NDUint32 nSendDataSize = refMsgBuf.getDataSize();
	if ( 0 == nSendDataSize )
	{
		return NDFalse;
	}

	if ( nSendDataSize != m_pSendBuffer->writeBuffer( refMsgBuf ) )
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

	if ( nSize != m_pSendBuffer->writeBuffer( pszBuf, nSize ) )
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
		if ( m_nDisconnectionType == ESessionDisconnectionType_NULL )
		{
			m_nDisconnectionType = ESessionDisconnectionType_READ_PASSIVE;
		}
		
		handleClose();
		return NDFalse;
	}
	if ( nCanRecvBytes > m_pRecvBuffer->getSpaceSize() )
	{	//重新分配空间;
		m_pRecvBuffer->reSet( m_pRecvBuffer->getCapacitySize() + nCanRecvBytes );
	}
	
	NDUint32 nRefRealRecvLen		= 0;
	NDUint32 nRecvBufferDataSize	= m_pRecvBuffer->getDataSize();
	if ( NDFalse == m_pSocket->recv( ( m_pRecvBuffer->readBuffer() + nRecvBufferDataSize ) , nCanRecvBytes, nRefRealRecvLen ) )
	{
		if ( m_nDisconnectionType == ESessionDisconnectionType_NULL )
		{
			m_nDisconnectionType = ESessionDisconnectionType_READ_PASSIVE;
		}

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
		m_pRecvBuffer->setWriteBufSize( nRefRealRecvLen );

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

	NDUint32 nSendBufferSize = m_pSendBuffer->getDataSize();
	if ( 0 == nSendBufferSize )
	{
		unregisterWriteEvent();

		return NDTrue;
	}

	if ( nSendBufferSize > m_nSocketSendBuf )
	{	//要发送的数据大于m_pSocket的发送缓存区大小;
		nSendBufferSize = m_nSocketSendBuf;
	}
	
	if ( NDFalse == m_pSocket->send( m_pSendBuffer->readBuffer(), nSendBufferSize, nSendBufferSize ) )
	{
		m_nDisconnectionType = ESessionDisconnectionType_SEND_PASSIVE;

		//先关闭SEND通道;
		shutdownSendPipe();
		return NDFalse;
	}

	if ( nSendBufferSize > 0 )
	{
		m_pSendBuffer->setReadBufSize( nSendBufferSize );

		if ( 0 == m_pSendBuffer->getDataSize() )
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
	parseSessionDataEx.m_nParsePacket	= (m_nRecvPacketNum > 0 ? 1 : 0);	//特殊处理是否是地方一个包,也可以在结构中增加一个字段;
	parseSessionDataEx.m_nErrorCode		= 0;
	
	if ( NDFalse == (*s_pParsePacket)( *m_pRecvBuffer, parseSessionDataEx ) )
	{
		parseSessionDataEx.m_nErrorCode;

		m_nDisconnectionType = ESessionDisconnectionType_PACKET_ERROR_ACTIVE;

		handleClose();
		return NDFalse;
	}

	m_nRecvPacketNum += parseSessionDataEx.m_nParsePacket;

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



NDBool NDSession::registerWriteEvent()
{
	if ( NULL == m_pSocket || NDFalse == m_bAlive || NDFalse == m_bSendPipe || NULL == m_pSendBuffer )	return NDFalse;

	NDUint32 nSendBufferSize = m_pSendBuffer->getDataSize();
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

	NDUint32 nSendBufferSize = m_pSendBuffer->getDataSize();
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

