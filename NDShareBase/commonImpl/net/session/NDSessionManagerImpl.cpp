#include "NDSessionManagerImpl.h"


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
}


NDBool NDSessionManagerImpl::sendData( NDProtocol& rProtocol, NDUint32 nSessionID, NDUint8 nProDataHeadBitWise, NDBool bClient )
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

NDBool NDSessionManagerImpl::sendToServer( NDProtocol& rProtocol, NDUint32 nSessionID, NDUint8 nProDataHeadBitWise )
{	
	return sendData( rProtocol, nSessionID, nProDataHeadBitWise, NDFalse );
}

NDBool NDSessionManagerImpl::sendToClient( NDProtocol& rProtocol, NDUint32 nSessionID, NDUint8 nProDataHeadBitWise )
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
	if ( NDFalse == m_select.registerReadWriteEvent( fd ) )
	{
		return NDFalse;
	}

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
	if ( NDFalse == m_select.registerReadWriteEvent( fd ) )
	{
		return NDFalse;	
	}

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
	UINTDeque  destorySessionDeque;
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
				destorySessionDeque.push_back( iterBegin->first );
			}
		}
	}

	if ( false == destorySessionDeque.empty() )
	{
		do 
		{
			UINTDequeIter UINTDequeIterBegin  = destorySessionDeque.begin();
			UINTDequeIter UINTDequeIterEnd	  = destorySessionDeque.end();

			NDGuardLock locker( *m_pServerMutex );
			for ( ;	UINTDequeIterBegin != UINTDequeIterEnd; ++UINTDequeIterBegin )
			{
				SessionMapIter destoryIterFind = m_serverSessionMap.find( *UINTDequeIterBegin );
				if ( destoryIterFind != m_serverSessionMap.end() )
				{
					pSession = (NDServerSession*)destoryIterFind->second;
					if ( NULL != pSession )
					{
						pSession->handleClose();	
					}
				}
			}
		} while (0);
	}
}


NDBool NDSessionManagerImpl::eventLoop()
{
	NDUint32 nRet = 0;
	if ( NDFalse == m_select.run( nRet ) )
	{
		return NDFalse;
	}

	if ( nRet == 0 )
	{	//超时;
		return NDTrue;
	}

	////原始代码;
	//NDUint32 nDispose = 0;
	//
	//SessionSOCKETMapIter	iter	= m_allWorkSessionMap.begin();
	//SessionSOCKETMapIter	iterEnd	= m_allWorkSessionMap.end();
	//for ( ; iter != iterEnd; ++iter )
	//{
	//	SOCKET s			= iter->first;
	//	NDSession* pSession	= iter->second;
	//	if ( s == INVALID_SOCKET || NULL == pSession )
	//	{
	//		continue;
	//	}

	//	if ( m_select.isReadEvent( s ) )
	//	{
	//		pSession->handleRead();

	//		++nDispose;
	//	}
	//	if ( m_select.isWriteEvent( s ) )
	//	{
	//		pSession->handleWrite();

	//		++nDispose;
	//	}

	//	if ( nDispose >= nRet )
	//	{
	//		break;
	//	}
	//}

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

			if ( m_select.isReadEvent( s ) )
			{
				pSession->handleRead();

				++nDispose;
			}
			if ( m_select.isWriteEvent( s ) )
			{
				pSession->handleWrite();

				++nDispose;
			}

			++iter;

		} while ( ( ( nDispose < nRet ) && ( iter != iterEnd ) ) );
	}

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

		SessionSOCKETMapIter invalidSessionIterFind = m_allWorkSessionMap.find( s );
		if ( invalidSessionIterFind != m_allWorkSessionMap.end() )
		{
			NDSession* pSession = invalidSessionIterFind->second;
			if ( NULL != pSession )
			{
				pSession->destroy();
			}

			m_allWorkSessionMap.erase( invalidSessionIterFind );
		}

		m_select.unregisterReadWriteEvent( s );
	}
	m_invalidSessionDeque.clear();

	if ( !m_allWorkSessionMap.empty() )
	{	//重新设置最大SOCKET句柄;
		m_select.setMaxFD( m_allWorkSessionMap.rbegin()->first );
	}
	
}

NDBool NDSessionManagerImpl::registerWriteEvent( SOCKET fd )
{
	return m_select.registerWriteEvent( fd );
}

NDBool NDSessionManagerImpl::unregisterWriteEvent( SOCKET fd )
{
	return m_select.unregisterWriteEvent( fd );
}

NDBool NDSessionManagerImpl::setCommonDisconnectNtyProtocol( NDProtocol* pDisconnectNtyProtocol )
{
	if ( NULL == pDisconnectNtyProtocol )	return NDFalse;

	m_pDisconnectNtyProtocol = pDisconnectNtyProtocol;

	return NDTrue;
}

NDBool NDSessionManagerImpl::popCommonDisconnectNtyProtocol( NDUint32 nSessionID )
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

	//put msg into  msg dispose deque;
	NDServerTask::getInstance()->putQueue( pProtocolBuf );

	return NDTrue;
}

void NDSessionManagerImpl::setSessionParsePacketFun( PParsePacketFun parsePacketFun )
{
	NDSession::setParsePacketFun( parsePacketFun );
}


//NDBool NDSessionManagerImpl::isSessionTypeProtocol( NDInt8 sessionProtocolType, NDUint32 nProtocolID )
//{
//	if ( sessionProtocolType < NDSessionProtocolType_CLIENT2L || sessionProtocolType >= NDSessionProtocolType_MAX )
//	{
//		return NDFalse;
//	}
//
//	//先处理特殊情况;
//	if ( nProtocolID == CMD_PING_PROTOCOL || nProtocolID == CMD_TIMER_NOTIFY || nProtocolID == CMD_DISCONNECT_NOTIFY )
//	{	
//		return NDTrue;
//	}
//
//	NDUint32* pProtocolNumArray = m_protocolNumArray[sessionProtocolType];
//
//	for ( NDUint8 i = 0; i < ND_PROTOCOL_SEND_LAYER_MAXNUM; ++i )
//	{
//		if ( ( nProtocolID > pProtocolNumArray[i] ) && ( nProtocolID < pProtocolNumArray[i+1] ) )
//		{
//			return NDTrue;
//		}
//	}
//
//	return NDTrue;
//}
//
//void NDSessionManagerImpl::setSessionTypeProtocol( NDSessionProtocolType eType, NDUint32 nBeginProtocol, NDUint32 nEndProtocol )
//{
//	if ( eType >= NDSessionProtocolType_MAX || eType < NDSessionProtocolType_CLIENT2L )
//	{
//		return;
//	}
//
//	for ( NDUint32 i = 0; i < ND_PROTOCOL_SEND_LAYER_MAXNUM; ++i )
//	{
//		if ( ( m_protocolNumArray[eType][i] <= nBeginProtocol ) && ( m_protocolNumArray[eType][i+1] >= nEndProtocol ) )
//		{
//			return;
//		}
//
//		if ( ( m_protocolNumArray[eType][i] == 0 ) && ( m_protocolNumArray[eType][i+1] == 0 ) )
//		{	
//			m_protocolNumArray[eType][i]	= nBeginProtocol;
//			m_protocolNumArray[eType][i+1]	= nEndProtocol;
//			return;
//		}
//		
//		continue;
//	}
//	
//	//m_protocolNumArray[NDSessionProtocolType_CLIENT2L][0] = CMD_NDClient2L_Start;
//	//m_protocolNumArray[NDSessionProtocolType_CLIENT2L][1] = CMD_NDClient2L_End;
//
//	//m_protocolNumArray[NDSessionProtocolType_CLIENT2GT][0] = CMD_NDClient2Gate_Start;
//	//m_protocolNumArray[NDSessionProtocolType_CLIENT2GT][1] = CMD_NDClient2Gate_End;
//
//	//m_protocolNumArray[NDSessionProtocolType_GT2CS][0] = CMD_NDG2CS_Start;
//	//m_protocolNumArray[NDSessionProtocolType_GT2CS][1] = CMD_NDG2CS_End;
//
//	//m_protocolNumArray[NDSessionProtocolType_GT2M][0] = CMD_NDG2M_Start;
//	//m_protocolNumArray[NDSessionProtocolType_GT2M][1] = CMD_NDG2M_End;
//
//	////m_protocolNumArray[NDSessionProtocolType_GT2ML][0] = 0;
//	////m_protocolNumArray[NDSessionProtocolType_GT2ML][1] = 0;
//
//	//m_protocolNumArray[NDSessionProtocolType_M2CS][0] = CMD_NDM2CS_Start;
//	//m_protocolNumArray[NDSessionProtocolType_M2CS][1] = CMD_NDM2CS_End;
//
//	//m_protocolNumArray[NDSessionProtocolType_M2GDB][0] = CMD_NDM2GDB_Start;
//	//m_protocolNumArray[NDSessionProtocolType_M2GDB][1] = CMD_NDM2GDB_End;
//
//	//m_protocolNumArray[NDSessionProtocolType_CS2L][0] = CMD_NDCS2L_Start;
//	//m_protocolNumArray[NDSessionProtocolType_CS2L][1] = CMD_NDCS2L_End;
//
//	//m_protocolNumArray[NDSessionProtocolType_CS2GDB][0] = CMD_NDCS2GDB_Start;
//	//m_protocolNumArray[NDSessionProtocolType_CS2GDB][1] = CMD_NDCS2GDB_End;
//
//	//m_protocolNumArray[NDSessionProtocolType_L2ADB][0] = CMD_NDL2A_Start;
//	//m_protocolNumArray[NDSessionProtocolType_L2ADB][1] = CMD_NDL2A_End;
//}



_NDSHAREBASE_END

