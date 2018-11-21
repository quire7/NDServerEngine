#include "NDSessionManagerImpl.h"

#include "NDShareBaseEnums.h"
#include "NDShareBaseGlobal.h"
#include "thread/NDMutex.h"

#include "memory/NDByteBuffer.h"
#include "memory/NDByteBufferPool.h"

#include "net/stream/NDProtocol.h"
#include "net/stream/NDProtocolPacket.h"
#include "net/stream/NDOStreamImpl.h"

#include "net/process/NDServerTask.h"

#include "net/session/NDClientSession.h"
#include "net/session/NDServerSession.h"

#ifdef ND_USE_EPOLL
#include "net/socket/NDLinuxEpoll.h"
#else
#include "net/socket/NDSelect.h"
#endif


_NDSHAREBASE_BEGIN

NDSessionManagerImpl::NDSessionManagerImpl(void) :	m_generateID( 0 ), m_nCreateNum( 0 ), 
													m_nSpareNum( 0 ), m_nAliveNum( 0 ),
													m_pGenerateMutex( new NDMutexLock ),
													m_pServerMutex	( new NDMutexLock ), 
													m_pClientMutex	( new NDMutexLock ),
													m_pPrepSMapMutex( new NDMutexLock ),
													m_pCSSDequeMutex( new NDMutexLock ), 
													m_pRSSMapMutex	( new NDMutexLock )
{
	m_serverSessionMap.clear();
	m_prepServerSessionMap.clear();
	m_createServerSessionDeque.clear();
	m_releaseServerSessionMap.clear();

	m_clientSessionMap.clear();
	m_createClientSessionDeque.clear();

	m_pDisconnectNtyProtocol = NULL;

#ifdef ND_USE_EPOLL
	m_pEpoll = new NDLinuxEpoll();
#else
	m_pSelect= new NDSelect();
#endif

	NDSession::setParsePacketFun( NDProtocolPacket::parsePacket );

	//setSessionTypeProtocol();
}

NDSessionManagerImpl::~NDSessionManagerImpl(void)
{
	NDSession* pSession = NULL;
	do 
	{
		NDGuardLock locker( *m_pServerMutex );
		SessionMapIter iterBegin	= m_serverSessionMap.begin();
		SessionMapIter iterEnd		= m_serverSessionMap.end();
		for ( ; iterBegin != iterEnd; ++iterBegin )
		{
			pSession = iterBegin->second;
			if ( NULL != pSession )
			{
				SAFE_DELETE( pSession )
				iterBegin->second = NULL;
			}
		}
	} while (0);
	m_serverSessionMap.clear();

	do 
	{
		NDGuardLock locker( *m_pPrepSMapMutex );
		SessionMapIter iterBegin	= m_prepServerSessionMap.begin();
		SessionMapIter iterEnd		= m_prepServerSessionMap.end();
		for ( ; iterBegin != iterEnd; ++iterBegin )
		{
			pSession = iterBegin->second;
			if ( NULL != pSession )
			{
				SAFE_DELETE( pSession )
				iterBegin->second = NULL;
			}
		}
	} while (0);
	m_prepServerSessionMap.clear();

	do 
	{
		NDGuardLock locker( *m_pCSSDequeMutex );
		SessionDequeIter iterBegin	= m_createServerSessionDeque.begin();
		SessionDequeIter iterEnd	= m_createServerSessionDeque.end();
		for ( ; iterBegin != iterEnd; ++iterBegin )
		{
			pSession = *iterBegin;
			if ( NULL != pSession )
			{
				SAFE_DELETE( pSession )
				*iterBegin = NULL;
			}
		}
	} while (0);
	m_createServerSessionDeque.clear();

	do 
	{
		NDGuardLock locker( *m_pRSSMapMutex );
		ReleaseSessionMapIter iterBegin	= m_releaseServerSessionMap.begin();
		ReleaseSessionMapIter iterEnd	= m_releaseServerSessionMap.end();
		for ( ; iterBegin != iterEnd; ++iterBegin )
		{
			pSession = iterBegin->first;
			if ( NULL != pSession )
			{
				SAFE_DELETE( pSession )
			}
		}
	} while (0);
	m_releaseServerSessionMap.clear();

	do 
	{
		NDGuardLock locker( *m_pClientMutex );
		SessionMapIter iterBegin	= m_clientSessionMap.begin();
		SessionMapIter iterEnd		= m_clientSessionMap.end();
		for ( ; iterBegin != iterEnd; ++iterBegin )
		{
			pSession = iterBegin->second;
			if ( NULL != pSession )
			{
				SAFE_DELETE( pSession )
				iterBegin->second = NULL;
			}
		}

		SessionDequeIter dequeiterBegin	= m_createClientSessionDeque.begin();
		SessionDequeIter dequeiterEnd	= m_createClientSessionDeque.end();
		for ( ; dequeiterBegin != dequeiterEnd; ++dequeiterBegin )
		{
			pSession = *dequeiterBegin;
			if ( NULL != pSession )
			{
				SAFE_DELETE( pSession )
				*dequeiterBegin = NULL;
			}
		}
	} while (0);
	m_clientSessionMap.clear();
	m_createClientSessionDeque.clear();

	SAFE_DELETE( m_pGenerateMutex );
	SAFE_DELETE( m_pServerMutex );
	SAFE_DELETE( m_pClientMutex );
	SAFE_DELETE( m_pPrepSMapMutex );
	SAFE_DELETE( m_pCSSDequeMutex );
	SAFE_DELETE( m_pRSSMapMutex );

	SAFE_DELETE( m_pDisconnectNtyProtocol )

#ifdef ND_USE_EPOLL
	SAFE_DELETE(m_pEpoll)
#else
	SAFE_DELETE(m_pSelect)
#endif
}


NDBool NDSessionManagerImpl::sendData( NDProtocol& rProtocol, NDUint32 nSessionID, NDUint16 nProDataHeadBitWise, NDBool bClient )
{
	NDByteBuffer* pMsgBuf = NDByteBufferPool::getInstance()->createByteBuffer();
	if ( NULL == pMsgBuf )
	{
		return NDFalse;
	}
	if ( NDFalse == NDProtocolPacket::composePacket( *pMsgBuf, rProtocol, nProDataHeadBitWise ) )
	{
		NDByteBufferPool::getInstance()->destroyByteBuffer( pMsgBuf );
		return NDFalse;
	}


	NDSysLock* pMutex		= NULL;
	SessionMap* pSessionMap = NULL;
	if (bClient)
	{
		pMutex		= m_pServerMutex;
		pSessionMap = &m_serverSessionMap;
	}
	else
	{
		pMutex		= m_pClientMutex;
		pSessionMap = &m_clientSessionMap;
	}
	NDGuardLock locker( *pMutex );
	SessionMapIter iterFind = pSessionMap->find( nSessionID );
	if ( iterFind != pSessionMap->end() )
	{
		NDSession* pSession = iterFind->second;
		if ( ( NULL != pSession ) && ( NULL != pMsgBuf ) )
		{
			pSession->sendData( *pMsgBuf );

			NDByteBufferPool::getInstance()->destroyByteBuffer( pMsgBuf );
			return NDTrue;
		}
	}

	NDByteBufferPool::getInstance()->destroyByteBuffer( pMsgBuf );

	return NDFalse;
}

NDBool NDSessionManagerImpl::sendToServer( NDProtocol& rProtocol, NDUint32 nSessionID, NDUint16 nProDataHeadBitWise )
{	
	return sendData( rProtocol, nSessionID, nProDataHeadBitWise, NDFalse );
}

NDBool NDSessionManagerImpl::sendToClient( NDProtocol& rProtocol, NDUint32 nSessionID, NDUint16 nProDataHeadBitWise )
{	
	return sendData( rProtocol, nSessionID, nProDataHeadBitWise, NDTrue );
}


NDSession* NDSessionManagerImpl::createServerSession()
{
	NDGuardLock cssDequelocker( *m_pCSSDequeMutex );
	createServerSessionDeque();

	NDSession *pSession = m_createServerSessionDeque.front();
	m_createServerSessionDeque.pop_front();

	--m_nSpareNum;

	do
	{
		NDGuardLock prepSMaplocker( *m_pPrepSMapMutex );
		m_prepServerSessionMap.insert( std::make_pair(pSession->getSessionID(), pSession) );
	}while ( 0 );
	
	return pSession;
}


NDSession* NDSessionManagerImpl::createClientSession()
{
	NDGuardLock locker( *m_pClientMutex );
	createClientSessionDeque();

	NDSession *pSession = m_createClientSessionDeque.front();
	m_createClientSessionDeque.pop_front();

	return pSession;
}

NDSession*  NDSessionManagerImpl::removePrepServerSession( NDUint32 nSessionID )
{
	NDSession* pSession = NULL;
	NDGuardLock locker( *m_pPrepSMapMutex );
	SessionMapIter iterFind = m_prepServerSessionMap.find( nSessionID );
	if ( iterFind != m_prepServerSessionMap.end() )
	{
		pSession = iterFind->second;
		m_prepServerSessionMap.erase( iterFind );
	}
	return pSession;	
}

NDBool NDSessionManagerImpl::addWorkServerSession( NDSession* pServerSession )
{
	if ( NULL == pServerSession ) return NDFalse;

	NDGuardLock locker( *m_pServerMutex );
	//pServerSession->SetInitPingANSISecondTime();

	SOCKET fd = pServerSession->getSOCKET();
#ifdef ND_USE_EPOLL
	if ( NDFalse == m_pEpoll->registerReadWriteEvent( fd, (void*)pServerSession ) )
	{
		return NDFalse;
	}
#else
	if ( NDFalse == m_pSelect->registerReadWriteEvent( fd ) )
	{
		return NDFalse;
	}
#endif

	m_serverSessionMap.insert( std::make_pair( pServerSession->getSessionID(), pServerSession ) );
	m_allWorkSessionMap.insert( std::make_pair( fd, pServerSession ) );

	removePrepServerSession( pServerSession->getSessionID() );

	++m_nAliveNum;
	
	return NDTrue;
}


NDBool NDSessionManagerImpl::addWorkClientSession( NDSession* pClientSession )
{
	if ( NULL == pClientSession ) return NDFalse;

	NDGuardLock locker( *m_pClientMutex );
	//pClientSession->SetInitPingANSISecondTime();

	SOCKET fd = pClientSession->getSOCKET();
#ifdef ND_USE_EPOLL
	if ( NDFalse == m_pEpoll->registerReadWriteEvent( fd, (void*)pClientSession ) )
	{
		return NDFalse;
	}
#else
	if ( NDFalse == m_pSelect->registerReadWriteEvent( fd ) )
	{
		return NDFalse;	
	}
#endif

	m_clientSessionMap.insert( std::make_pair( pClientSession->getSessionID(), pClientSession ) );
	m_allWorkSessionMap.insert( std::make_pair( fd, pClientSession ) );

	return NDTrue;
}


NDSession* NDSessionManagerImpl::findServerSession( NDUint32 nSessionID )
{
	NDGuardLock locker( *m_pServerMutex );
	SessionMapIter iterFind = m_serverSessionMap.find( nSessionID );
	if ( iterFind != m_serverSessionMap.end() )
	{
		return iterFind->second;
	}
	return NULL;
}

NDSession* NDSessionManagerImpl::findClientSession( NDUint32 nSessionID )
{
	NDGuardLock locker( *m_pClientMutex );
	SessionMapIter iterFind = m_clientSessionMap.find( nSessionID );
	if ( iterFind != m_clientSessionMap.end() )
	{
		return iterFind->second;
	}
	return NULL;
}

void NDSessionManagerImpl::releaseServerSession( NDUint32 nSessionID )
{
	NDGuardLock locker( *m_pServerMutex );
	SessionMapIter iterFind = m_serverSessionMap.find( nSessionID );
	if ( iterFind != m_serverSessionMap.end() )
	{
		do 
		{
			NDGuardLock rssMaplocker( *m_pRSSMapMutex );
			m_releaseServerSessionMap.insert( std::make_pair( iterFind->second, ( NDShareBaseGlobal::getCurSecondTimeOfUTC() + SOCKET_GC_TIMEOUT)) );
		} while (0);

		m_serverSessionMap.erase( iterFind );

		--m_nAliveNum;
	}
	else
	{	//session init failed, into m_prepServerSessionMap;
		NDSession* pServerSession = removePrepServerSession( nSessionID );
		if ( NULL != pServerSession )
		{
			pServerSession->releaseSession();

			do 
			{
				NDGuardLock cssDequelocker( *m_pCSSDequeMutex );
				m_createServerSessionDeque.push_back( pServerSession );
				++m_nSpareNum;
			} while (0);
		}
	}
}

void NDSessionManagerImpl::releaseClientSession( NDUint32 nSessionID )
{
	NDGuardLock locker( *m_pClientMutex );
	SessionMapIter iterFind = m_clientSessionMap.find( nSessionID );
	if ( iterFind != m_clientSessionMap.end() )
	{
		NDSession* pSession = iterFind->second;
		if ( NULL != pSession )
		{
			pSession->releaseSession();
			m_createClientSessionDeque.push_back( pSession );
		}
		m_clientSessionMap.erase( iterFind );
	}
}

NDUint32 NDSessionManagerImpl::generateID()
{
	NDGuardLock locker( *m_pGenerateMutex );
	return ++m_generateID;
}


void NDSessionManagerImpl::createClientSessionDeque()
{
	if ( m_createClientSessionDeque.empty() )
	{
		NDUint32 nIndex		= 0;
		NDUint32 nID		= 0;
		NDSession *pSession	= NULL;

		do 
		{
			nID = generateID();
			pSession = new NDClientSession( nID );
			m_createClientSessionDeque.push_back( pSession );
		} while ( ( ++nIndex ) < CREATE_SESSION_NUM );
	}
}

void NDSessionManagerImpl::createServerSessionDeque()
{
	if ( 0 == m_nSpareNum )
	{
		NDUint32 nIndex		= 0;
		NDUint32 nID		= 0;
		NDSession *pSession	= NULL;

		do 
		{
			nID = generateID();
			pSession = new NDServerSession( nID );
			m_createServerSessionDeque.push_back( pSession );
		} while ( ( ++nIndex ) < CREATE_SESSION_NUM );

		m_nSpareNum  += CREATE_SESSION_NUM;
		m_nCreateNum += CREATE_SESSION_NUM;
	}
}

void NDSessionManagerImpl::updateReleaseSessionMap()
{
	if ( m_releaseServerSessionMap.empty() )
	{
		return;
	}

	NDTime tCurrentTime = NDShareBaseGlobal::getCurSecondTimeOfUTC();
	NDSession* pSession	= NULL;
	ReleaseSessionMapIter iterBegin;
	ReleaseSessionMapIter iterEnd;

	NDGuardLock rssMaplocker( *m_pRSSMapMutex );
	for ( iterBegin = m_releaseServerSessionMap.begin(),
			iterEnd = m_releaseServerSessionMap.end();
			iterBegin != iterEnd; )
	{
		if ( iterBegin->second <= tCurrentTime )
		{
			pSession = iterBegin->first;
			pSession->releaseSession();

			do 
			{
				NDGuardLock cssDequelocker( *m_pCSSDequeMutex );
				m_createServerSessionDeque.push_back( pSession );
				++m_nSpareNum;
			} while (0);

			m_releaseServerSessionMap.erase( iterBegin++ );
		}
		else
		{
			++iterBegin;
		}
	}
}


void NDSessionManagerImpl::updateServerSessionMap()
{
	//UINTDeque  destorySessionDeque;
	SessionMap serverSessionMapCopy;
	//copy current session map
	do 
	{
		NDGuardLock locker( *m_pServerMutex );
		serverSessionMapCopy.insert( m_serverSessionMap.begin(), m_serverSessionMap.end() );
	} while (0);

	NDServerSession* pSession	  = NULL;
	NDTime nCurrentANSISecondTime = NDShareBaseGlobal::getCurSecondTimeOfUTC();

	SessionMapIter iterBegin = serverSessionMapCopy.begin();
	SessionMapIter iterEnd	 = serverSessionMapCopy.end();
	for ( ;	iterBegin != iterEnd; ++iterBegin )
	{
		pSession = (NDServerSession*)iterBegin->second;
		if ( NULL != pSession )
		{
			if ( SOCKET_PING_TIMEOUT <= ( NDShareBaseGlobal::getDiffMilliSecondTime( nCurrentANSISecondTime, pSession->getLastPingSecondTime() ) ) )
			{
				pSession->setDisconnectionType( ESessionDisconnectionType_PING_BEYOND_TIME_ACTIVE );
				pSession->handleClose();	//这个地方不需要删除m_serverSessionMap中的session,你懂的,仔细看;
				//destorySessionDeque.push_back( iterBegin->first );
			}
		}
	}

	//if ( false == destorySessionDeque.empty() )
	//{
	//	do 
	//	{
	//		UINTDequeIter UINTDequeIterBegin  = destorySessionDeque.begin();
	//		UINTDequeIter UINTDequeIterEnd	  = destorySessionDeque.end();

	//		NDGuardLock locker( *m_pServerMutex );
	//		for ( ;	UINTDequeIterBegin != UINTDequeIterEnd; ++UINTDequeIterBegin )
	//		{
	//			SessionMapIter destoryIterFind = m_serverSessionMap.find( *UINTDequeIterBegin );
	//			if ( destoryIterFind != m_serverSessionMap.end() )
	//			{
	//				pSession = (NDServerSession*)destoryIterFind->second;
	//				if ( NULL != pSession )
	//				{
	//					pSession->handleClose();	
	//				}
	//			}
	//		}
	//	} while (0);
	//}
}


NDBool NDSessionManagerImpl::eventLoop()
{
	NDUint32 nRet = 0;

#ifdef ND_USE_EPOLL
	return m_pEpoll->run( nRet );
#else
	if ( NDFalse == m_pSelect->run( nRet ) )
	{
		return NDFalse;
	}

	if ( nRet == 0 )
	{	//超时;
		return NDTrue;
	}

	////优化后代码;
	if ( !m_allWorkSessionMap.empty() )
	{
		NDUint32 nDispose = 0;

		SessionSOCKETMapIter	iter	= m_allWorkSessionMap.begin();
		SessionSOCKETMapIter	iterEnd	= m_allWorkSessionMap.end();

		do 
		{
			SOCKET s			= iter->first;
			NDSession* pSession	= iter->second;
			if ( s == INVALID_SOCKET || NULL == pSession )
			{
				++iter;
				continue;
			}

			if ( m_pSelect->isReadEvent( s ) )
			{
				pSession->handleRead();

				++nDispose;
			}
			if ( m_pSelect->isWriteEvent( s ) )
			{
				pSession->handleWrite();

				++nDispose;
			}

			++iter;

		} while ( ( ( nDispose < nRet ) && ( iter != iterEnd ) ) );
	}
#endif

	return NDTrue;
}

void NDSessionManagerImpl::setInvalidWorkSession( SOCKET nSocketID )
{
	m_invalidSessionDeque.push_back( nSocketID );
}

void NDSessionManagerImpl::closeInvalidWorkSession()
{
	if ( m_invalidSessionDeque.empty() )
	{
		return;
	}

	SOCKETDequeIter socketIter		= m_invalidSessionDeque.begin();
	SOCKETDequeIter socketIterEnd	= m_invalidSessionDeque.end();
	for ( ; socketIter != socketIterEnd; ++socketIter )
	{
		SOCKET s = *socketIter;
		if ( s == INVALID_SOCKET )
		{
			continue;
		}

		NDSession* pSession = NULL;
		SessionSOCKETMapIter invalidSessionIterFind = m_allWorkSessionMap.find( s );
		if ( invalidSessionIterFind != m_allWorkSessionMap.end() )
		{
			pSession = invalidSessionIterFind->second;
			if ( NULL != pSession )
			{
				pSession->destroy();
			}

			m_allWorkSessionMap.erase( invalidSessionIterFind );
		}
#ifdef ND_USE_EPOLL
		m_pEpoll->unregisterReadWriteEvent( s, pSession );
#else
		m_pSelect->unregisterReadWriteEvent( s );
#endif
	}
	m_invalidSessionDeque.clear();

#ifdef ND_USE_EPOLL
#else
	if ( !m_allWorkSessionMap.empty() )
	{	//重新设置最大SOCKET句柄;
		m_pSelect->setMaxFD( m_allWorkSessionMap.rbegin()->first );
	}
#endif
}

NDBool NDSessionManagerImpl::registerWriteEvent( SOCKET fd, NDSession* pSession )
{
#ifdef ND_USE_EPOLL
	return m_pEpoll->registerWriteEvent( fd, (void*)pSession );
#else
	return m_pSelect->registerWriteEvent( fd );
#endif
}

NDBool NDSessionManagerImpl::unregisterWriteEvent( SOCKET fd, NDSession* pSession )
{
#ifdef ND_USE_EPOLL
	return m_pEpoll->unregisterWriteEvent( fd, (void*)pSession );
#else
	return m_pSelect->unregisterWriteEvent( fd );
#endif
}

NDBool NDSessionManagerImpl::setCommonDisconnectNtyProtocol( NDProtocol* pDisconnectNtyProtocol )
{
	if ( NULL == pDisconnectNtyProtocol )	return NDFalse;

	m_pDisconnectNtyProtocol = pDisconnectNtyProtocol;

	return NDTrue;
}

NDBool NDSessionManagerImpl::popCommonDisconnectNtyProtocol( NDUint32 nSessionID, NDUint8 nDisconnectionType )
{
	if ( NULL == m_pDisconnectNtyProtocol )	return NDFalse;
	
	NDByteBuffer* pProtocolBuf = NDByteBufferPool::getInstance()->createByteBuffer();
	if ( NULL == pProtocolBuf )
	{
		return NDFalse;
	}

	pProtocolBuf->writeBuffer( (const char*)&nSessionID, sizeof(nSessionID) );

	NDOStreamImpl out( *pProtocolBuf );
	if ( NDFalse == m_pDisconnectNtyProtocol->serialize(out) )
	{
		NDByteBufferPool::getInstance()->destroyByteBuffer( pProtocolBuf );
		return NDFalse;
	}

	//NDDisconnectNtyProtocol特殊处理(由于底层不知道具体协议中的结构,所以在这个地方做特色处理,;
	//NDDisconnectNtyProtocol中serialize函数中不需要写入结构,直接在底层写入,deserialize函数中;
	//要做读处理跟其他协议一样);
	pProtocolBuf->writeBuffer( (const char*)&nDisconnectionType, sizeof(nDisconnectionType) );

	//put msg into  msg dispose deque;
	NDServerTask::getInstance()->putQueue( pProtocolBuf );

	return NDTrue;
}

void NDSessionManagerImpl::setSessionParsePacketFun( PParsePacketFun parsePacketFun )
{
	NDSession::setParsePacketFun( parsePacketFun );
}

NDBool NDSessionManagerImpl::setServerSessionProtocolType(NDUint32 nSessionID, NDUint8 nSessionProtocolType)
{
	//相对于客户端的连接,但是实质是服务器的Session;
	NDSession* pSession = findServerSession( nSessionID );
	if ( NULL == pSession )
	{
		return NDFalse;
	}

	pSession->setProtocolType( nSessionProtocolType );

	return NDTrue;
}

NDBool NDSessionManagerImpl::setClientSessionProtocolType(NDUint32 nSessionID, NDUint8 nSessionProtocolType)
{
	NDSession* pSession = findClientSession( nSessionID );
	if ( NULL == pSession )
	{
		return NDFalse;
	}

	pSession->setProtocolType( nSessionProtocolType );

	return NDTrue;
}

void NDSessionManagerImpl::setMaxSessionType(NDUint8 nMaxSessionType)
{
	NDProtocolPacket::setMaxSessionType( nMaxSessionType );
}

void NDSessionManagerImpl::setSpecialProtocol(NDUint16 nSpecialProtocolStart, NDUint16 nSpecialProtocolEnd)
{
	NDProtocolPacket::setSpecialProtocol( nSpecialProtocolStart, nSpecialProtocolEnd );
}

NDBool NDSessionManagerImpl::setDisposeSessionProtocol(NDUint8 nSessionType, NDUint16 nProtocolStart, NDUint16 nProtocolEnd)
{
	return NDProtocolPacket::setDisposeSessionProtocol( nSessionType, nProtocolStart, nProtocolEnd );
}






_NDSHAREBASE_END

