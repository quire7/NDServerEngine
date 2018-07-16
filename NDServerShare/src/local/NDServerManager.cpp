#include "local/NDServerManager.h"

#include <sstream>

#include "NDCLibFun.h"

#include "NDShareBaseMacros.h"
#include "NDShareBaseGlobal.h"

#include "thread/NDMutex.h"

#include "file/NDLog.h"

#include "net/session/NDServerSession.h"
#include "net/session/NDSessionManager.h"
#include "net/process/NDServerTask.h"
#include "net/process/NDDataProcess.h"
#include "net/netio/NDClientNetIO.h"
#include "net/netio/NDServerNetIO.h"

#include "special/NDSpecialProtocol.h"
#include "NDCmdProtocolC2S.h"
#include "NDCmdProtocolS2C.h"
#include "protocol/NDCmdProtocolS2S.h"

#include "local/NDServerInfo.h"
#include "local/NDLocalServer.h"

#include "remote/NDRemoteServerInfo.h"
#include "remote/NDRemoteServerManager.h"


using NDShareBase::NDGuardLock;
using NDShareBase::NDConsoleColor;
using NDShareBase::NDServerSession;
using NDShareBase::NDSessionManager;
using NDShareBase::NDServerTask;
using NDShareBase::g_bConsole;
using NDShareBase::g_pFileLogManager;
using NDShareBase::g_pSMLogManager;


initialiseSingleton(NDServerManager);

NDServerManager::NDServerManager(void) :
m_pListenServer( NULL ),
m_pListenNetIO( NULL ),
m_pCSTMutex( new NDMutexLock ),
m_pCSIMutex( new NDMutexLock )
{
	m_connServerTable.clear();
	m_connServerInstance.clear();

	m_nMainProcMilliSeconds = ND_MAIN_PROC_MILLISECONDS;

}

NDServerManager::~NDServerManager(void)
{
	for (ConnServerTableIter iterBegin = m_connServerTable.begin(),
							 iterEnd   = m_connServerTable.end();
							 iterBegin != iterEnd;
							 ++iterBegin)
	{
		NDServerInfo* pServerInfo = *iterBegin;
		SAFE_DELETE( pServerInfo );
	}
	m_connServerTable.clear();

	for (ConnServerInstanceIter instanceIterBegin = m_connServerInstance.begin(),
								instanceIterEnd	  = m_connServerInstance.end();
								instanceIterBegin!= instanceIterEnd;
								++instanceIterBegin)
	{
		NDClientNetIO* pClientNetIO = instanceIterBegin->second;
		SAFE_DELETE( pClientNetIO );
	}
	m_connServerInstance.clear();


	SAFE_DELETE( m_pCSIMutex )
	SAFE_DELETE( m_pCSTMutex )
	SAFE_DELETE( m_pListenNetIO )
	//SAFE_DELETE( m_pListenServer )	//这个是外部传进来的,还是自己去删除吧;
}

NDBool NDServerManager::startServer( NDLocalServer* pLocalServer, NDBool bConnOtherServer )
{
	if ( NULL == pLocalServer )		return NDFalse;
	if ( NULL != m_pListenNetIO )	return NDFalse;

	NDDataProcess* process = pLocalServer->dataProcess();
	if ( NULL == process )			return NDFalse;

	m_pListenServer = pLocalServer;
	m_pListenNetIO	= new NDServerNetIO;

	NDLocalServerInfo* pListenServerInfo = pLocalServer->getLocalServerInfo();

	NDBool bRet = m_pListenNetIO->startServer( pListenServerInfo->getServerIP(), pListenServerInfo->getServerPort() );//这是监听服务器;
	if ( NDFalse == bRet )
	{
		NDLOG_ERROR( " %s [%s:%u] start failed! ", pListenServerInfo->getServerName(),
													pListenServerInfo->getServerIP(), 
													pListenServerInfo->getServerPort() );

		return NDFalse;
	}
	

	m_pListenNetIO->setProcessor( process );
	m_pListenNetIO->setCommonDisconnectNtyProtocol( new NDDisconnectNtyProtocol );

	setDisposeSessionProtocol();

	//m_pListenNetIO->SetCheckSessionTimer();

	NDLOG_INFO( " %s [%s:%d] listening ...", pListenServerInfo->getServerName(), pListenServerInfo->getServerIP(), pListenServerInfo->getServerPort() )

	if ( NDTrue == bConnOtherServer )
	{
		NDConnectProcess* pConnecter = m_pListenServer->connectProcess();
		if ( NULL == pConnecter )
		{
			return NDFalse;
		}
		if ( NDFalse == connectServer( pConnecter ) )
		{
			NDLOG_ERROR( " [NDServerManager::startServer] connectServer failed! " )
			return NDFalse;
		}
	}

	m_pListenServer->setState( NDLocalServer::eServState_Starting );

	return bRet;
}

NDBool NDServerManager::sendToClient( NDProtocol& protocol, NDUint32 nSessionID, NDBool bCompression, NDBool bEncrypt, NDBool bCrc )
{
	if (NULL == m_pListenNetIO)		return NDFalse;

	return m_pListenNetIO->sendToClient( protocol, nSessionID, bCompression, bEncrypt, bCrc );
}

NDBool NDServerManager::sendToServer( NDProtocol& protocol, SERVERTYPE _servType, NDBool bCompression, NDBool bEncrypt, NDBool bCrc )
{	
	//NDGuardLock	cstLocker( *m_pCSTMutex );
	//ConnServerTableIter iterBegin = m_connServerTable.begin();
	//ConnServerTableIter iterEnd   = m_connServerTable.end();

	ConnServerTable connServerTableTemp( m_connServerTable.begin(), m_connServerTable.end() );
	ConnServerTableIter iterBegin = connServerTableTemp.begin();
	ConnServerTableIter iterEnd   = connServerTableTemp.end();

	for ( ; iterBegin != iterEnd; ++iterBegin )
	{
		NDServerInfo* pServerInfo = *iterBegin;
		if ( (NULL != pServerInfo) && pServerInfo->getServerType() == _servType )
		{
			return sendToServer( protocol, pServerInfo->getSessionID(), bCompression, bEncrypt, bCrc );
		}
	}
	return NDFalse;
}

NDBool NDServerManager::sendToServer( NDProtocol& protocol, NDUint32 nSessionID, NDBool bCompression, NDBool bEncrypt, NDBool bCrc )
{
	//NDGuardLock	csiLocker( *m_pCSIMutex );   //lock or no lock : don't have to lock.
	ConnServerInstanceIter instanceIterFind = m_connServerInstance.find( nSessionID );
	if ( instanceIterFind != m_connServerInstance.end() )
	{
		NDClientNetIO* pClientNetIO = instanceIterFind->second;
		if ( NULL != pClientNetIO )
		{
			if ( NDFalse == pClientNetIO->isConnected() )
			{
				pClientNetIO->disConnect();
				//先设置为DISCONNECTED状态;
				setConnServerDisconnectState( nSessionID );
				return NDFalse;
			}
			return pClientNetIO->sendToServer( protocol, bCompression, bEncrypt, bCrc );
		}
		else
		{	//其实走到这里就说明程序出错了;
			//先设置为DISCONNECTED状态;
			setConnServerDisconnectState( nSessionID );
			m_connServerInstance.erase( instanceIterFind );
		}
	}

	return NDFalse;
}


void NDServerManager::registerConnServer( NDServerInfo* pServerInfo )
{
	//NDGuardLock	cstLocker( *m_pCSTMutex );
	m_connServerTable.push_back( pServerInfo );
}


NDBool NDServerManager::connectServerDisponse( NDServerInfo* pServerInfo, NDConnectProcess* pConnecter )
{
	if ( NULL == pServerInfo )		return NDFalse;
	if ( NULL == pConnecter )		return NDFalse;
	if ( NULL == m_pListenServer )	return NDFalse;

	if ( NDLocalServer::eServState_Stopping == m_pListenServer->getState() )
	{
		return NDFalse;
	}


	NDBool	 bRet		= NDFalse;

	switch (pServerInfo->getConnState())
	{
	case NDServerInfo::eConnState_Null:
		break;
	case NDServerInfo::eConnState_Initial:
	case NDServerInfo::eConnState_DisConnected:
		{
			NDUint32 nSessionID			= 0;
			NDClientNetIO* pClientNetIO = NULL;
			if ( ( ND_INVALID_ID != pServerInfo->getSessionID() ) && ( NDServerInfo::eConnState_DisConnected == pServerInfo->getConnState() ) )
			{
				pClientNetIO = removeConnServerClientNetIO( pServerInfo->getSessionID() );
			}
			
			if ( NULL == pClientNetIO )
			{
				pClientNetIO = new NDClientNetIO;
				pClientNetIO->setPingProtocol( new NDPingReqProtocol );
			}
			
			if ( pClientNetIO->connect( pServerInfo->getServerIP(), pServerInfo->getServerPort() ) )
			{
				nSessionID = pClientNetIO->getSessionID();
				pServerInfo->setConnState( NDServerInfo::eConnState_Connected );
				pServerInfo->setSessionID( nSessionID );

				do 
				{
					NDGuardLock	csiLocker( *m_pCSIMutex );
					m_connServerInstance.insert( std::make_pair( nSessionID, pClientNetIO ) );
				} while (0);

				
				NDLOG_INFO( " connect %s [%s:%u] success! ", pServerInfo->getServerName(),
															pServerInfo->getServerIP(),
															pServerInfo->getServerPort() );

				if ( NULL != pConnecter )
				{
					pConnecter->connected( pServerInfo );
				}

				bRet = NDTrue;
			}
			else
			{
				//connect failed, delete NDClientNetIO.;
				SAFE_DELETE( pClientNetIO );
				
				if ( ( NDLocalServer::eServState_Stopping > m_pListenServer->getState() ) && ( NULL != pServerInfo ) )
				{
					NDLOG_ERROR( " connect %s [%s:%u] error! ", pServerInfo->getServerName(),
																pServerInfo->getServerIP(),
																pServerInfo->getServerPort() );
				}

				if ( NULL != pConnecter )
				{
					pConnecter->error( pServerInfo );
				}
			}	
		}
		break;
	default:
		break;
	}

	return bRet;
}


NDBool NDServerManager::connectServer( NDConnectProcess* pConnecter )
{
	if ( NULL == pConnecter )
	{
		NDLOG_ERROR( " NDServerManager::connectServer pConnecter is NULL! " )
		return NDFalse;
	}

	NDBool bRet = NDTrue;

	//lock
	//NDGuardLock	cstLocker( *m_pCSTMutex );
	//ConnServerTableIter iterBegin = m_connServerTable.begin();
	//ConnServerTableIter iterEnd   = m_connServerTable.end();

	//no lock
	ConnServerTable connServerTableTemp( m_connServerTable.begin(), m_connServerTable.end() );
	ConnServerTableIter iterBegin = connServerTableTemp.begin();
	ConnServerTableIter iterEnd   = connServerTableTemp.end();

	for (;iterBegin != iterEnd; ++iterBegin)
	{
		NDServerInfo* pServerInfo = *iterBegin;
		if ( NDFalse == connectServerDisponse( pServerInfo, pConnecter ) )
		{
			bRet = NDFalse;
		}
	}

	return bRet;
}


NDBool NDServerManager::connectServer( NDServerInfo* pServerInfo, NDConnectProcess* pConnecter )
{
	if ( NULL == pServerInfo )	return NDFalse;
	if ( NULL == pConnecter )	return NDFalse;

	registerConnServer( pServerInfo );

	return connectServerDisponse( pServerInfo, pConnecter );
}


void NDServerManager::disConnectServer()
{
	NDGuardLock	cstLocker( *m_pCSTMutex );
	ConnServerTableIter iterBegin = m_connServerTable.begin();
	ConnServerTableIter iterEnd   = m_connServerTable.end();
	for ( ; iterBegin != iterEnd; ++iterBegin )
	{
		NDServerInfo* pServerInfo = *iterBegin;
		switch (pServerInfo->getConnState())
		{
		case NDServerInfo::eConnState_Connected:
			{
				ConnServerInstanceIter instanceIterFind = m_connServerInstance.find( pServerInfo->getSessionID() );
				if ( instanceIterFind != m_connServerInstance.end() )
				{
					NDClientNetIO *pClientNetIO = instanceIterFind->second;
					if (NULL != pClientNetIO)
					{
						pClientNetIO->disConnect();

						SAFE_DELETE( pClientNetIO );
					}

					NDGuardLock	csiLocker( *m_pCSIMutex );
					m_connServerInstance.erase(instanceIterFind);
				}
				

				pServerInfo->setConnState( NDServerInfo::eConnState_DisConnected );
				pServerInfo->setSessionID( (NDUint32)ND_INVALID_ID );
			}
			break;
		}
	}
}

const NDServerInfo* NDServerManager::getConnServerInfo( NDUint32 nSessionID )
{
	NDGuardLock	cstLocker( *m_pCSTMutex );
	ConnServerTableIter iterBegin = m_connServerTable.begin();
	ConnServerTableIter iterEnd   = m_connServerTable.end();
	for ( ; iterBegin != iterEnd; ++iterBegin )
	{
		NDServerInfo* pServerInfo = *iterBegin;
		if ( nSessionID == pServerInfo->getSessionID() )
		{
			return pServerInfo;
		}
	}

	return NULL;
}

const NDServerInfo* NDServerManager::getConnServerInfo(SERVERTYPE servType, NDUint32 nServerID)
{
	NDGuardLock	cstLocker( *m_pCSTMutex );
	ConnServerTableIter iterBegin = m_connServerTable.begin();
	ConnServerTableIter iterEnd   = m_connServerTable.end();
	for ( ; iterBegin != iterEnd; ++iterBegin )
	{
		NDServerInfo* pServerInfo = *iterBegin;
		if ( ( servType == pServerInfo->getServerType() ) && ( nServerID == pServerInfo->getServerID() ) )
		{
			return pServerInfo;
		}
	}

	return NULL;
}


NDBool NDServerManager::removeConnServer( NDUint32 nSessionID )
{
	NDBool bRet = NDFalse;

	NDGuardLock	cstLocker( *m_pCSTMutex );
	ConnServerTableIter iterBegin = m_connServerTable.begin();
	ConnServerTableIter iterEnd   = m_connServerTable.end();
	for ( ; iterBegin != iterEnd; ++iterBegin )
	{
		NDServerInfo* pServerInfo = *iterBegin;
		if ( nSessionID == pServerInfo->getSessionID() )
		{
			SAFE_DELETE(*iterBegin)
			m_connServerTable.erase( iterBegin );
			bRet = NDTrue;
			break;
		}
	}

	return bRet;	
}

NDBool NDServerManager::disConnectClient( NDUint32 nSessionID )
{
	if ( NULL == m_pListenNetIO )
	{
		return NDFalse;
	}

	return m_pListenNetIO->disConnect( nSessionID );
}

NDBool NDServerManager::getClientAddress( NDUint32 nSessionID, NDSocketAddress& refClientAddress )
{
	if ( NULL == m_pListenNetIO )
	{
		return NDFalse;
	}

	return m_pListenNetIO->getClientAddress( nSessionID, refClientAddress );
}

NDBool NDServerManager::setConnServerDisconnectState( NDUint32 nSessionID )
{
	NDBool bRet = NDFalse;

	NDGuardLock	cstLocker( *m_pCSTMutex );
	ConnServerTableIter iterBegin = m_connServerTable.begin();
	ConnServerTableIter iterEnd   = m_connServerTable.end();
	for ( ; iterBegin != iterEnd; ++iterBegin )
	{
		NDServerInfo* pServerInfo = *iterBegin;
		if ( ( NULL != pServerInfo ) && ( nSessionID == pServerInfo->getSessionID() ) )
		{
			pServerInfo->setConnState( NDServerInfo::eConnState_DisConnected );

			NDLOG_ERROR( " disconnect %s [%s:%u]. ", pServerInfo->getServerName(),
													pServerInfo->getServerIP(),
													pServerInfo->getServerPort() );


			bRet = NDTrue;
			break;
		}
	}

	return bRet;
}

NDBool NDServerManager::checkAllConnectServerState()
{
	NDBool bRet = NDTrue;
	{
		//NDGuardLock	csiLocker( *m_pCSIMutex );   //lock or no lock : don't have to lock.
		ConnServerInstanceIter instanceIterBegin = m_connServerInstance.begin();
		ConnServerInstanceIter instanceIterEnd	 = m_connServerInstance.end();
		for ( ; instanceIterBegin != instanceIterEnd; )
		{
			NDClientNetIO* pClientNetIO = instanceIterBegin->second;
			if ( NULL != pClientNetIO )
			{
				if ( NDFalse == pClientNetIO->isConnected() )
				{
					pClientNetIO->disConnect();
					//先设置为DISCONNECTED状态;
					setConnServerDisconnectState( instanceIterBegin->first );
				}
				++instanceIterBegin;
			}
			else
			{	//其实走到这里就说明程序出错了;
				//先设置为DISCONNECTED状态;
				setConnServerDisconnectState( instanceIterBegin->first );
				m_connServerInstance.erase( instanceIterBegin++ );
			}
		}
	}

	{
		//NDGuardLock	cstLocker( *m_pCSTMutex );
		ConnServerTableIter iterBegin = m_connServerTable.begin();
		ConnServerTableIter iterEnd   = m_connServerTable.end();

		for ( ; iterBegin != iterEnd; ++iterBegin )
		{
			NDServerInfo* pServerInfo = *iterBegin;
			if ( NULL != pServerInfo  )
			{
				NDServerInfo::eConnState_Types eConnState = pServerInfo->getConnState();
				if ( NDServerInfo::eConnState_DisConnected == eConnState || NDServerInfo::eConnState_Initial == eConnState )
				{
					bRet = NDFalse;
					break;
				}
			}
		}
	}

	return bRet;
}

NDClientNetIO* NDServerManager::removeConnServerClientNetIO( NDUint32 nSessionID )
{
	NDClientNetIO* pClientNetIO = NULL;

	//NDGuardLock	csiLocker( *m_pCSIMutex );   //lock or no lock : don't have to lock.
	ConnServerInstanceIter instanceIterFind = m_connServerInstance.find( nSessionID );
	if ( instanceIterFind != m_connServerInstance.end() )
	{
		pClientNetIO = instanceIterFind->second;
		m_connServerInstance.erase( instanceIterFind );
	}
	return pClientNetIO;
}

void NDServerManager::reconnectServer( NDConnectProcess* pConnecter )
{
	if ( NULL == pConnecter )
	{
		NDLOG_ERROR( " NDServerManager::reconnectServer pConnecter is NULL! " )
	}

	//lock
	//NDGuardLock	cstLocker( *m_pCSTMutex );
	ConnServerTableIter iterBegin = m_connServerTable.begin();
	ConnServerTableIter iterEnd   = m_connServerTable.end();

	for (;iterBegin != iterEnd; ++iterBegin)
	{
		NDServerInfo* pServerInfo = *iterBegin;
		if ( NULL != pServerInfo )
		{
			NDServerInfo::eConnState_Types eConnState = pServerInfo->getConnState();
			if ( NDServerInfo::eConnState_Initial == eConnState || NDServerInfo::eConnState_DisConnected == eConnState )
			{
				connectServerDisponse( pServerInfo, pConnecter );
			}
		}
	}	
}

NDBool NDServerManager::mainLoop()
{
	if ( NULL == m_pListenServer )
	{
		return NDFalse;
	}

	NDLocalServerInfo* pListenServerInfo = m_pListenServer->getLocalServerInfo();
	if ( NULL == pListenServerInfo )
	{
		return NDFalse;
	}

	NDConnectProcess* pConnecter = m_pListenServer->connectProcess();
	if ( !m_connServerTable.empty() && NULL == pConnecter )
	{
		NDLOG_ERROR(" NDServerManager::mainLoop pConnecter is NULL! ListenServerType =[%s].", pListenServerInfo->getServerName())
		return NDFalse;
	}

	do
	{
		if ( NULL == m_pListenServer || NULL == m_pListenNetIO )
		{
			return NDFalse;
		}
		if ( NDLocalServer::eServState_Stopping <= m_pListenServer->getState() )
		{
			return true;
		}

		//先检查此服务器与其他服务器的链接(即此服务器主动链接别的服务器的链接);
		if ( NULL != pConnecter )
		{
			if ( NDFalse == checkAllConnectServerState() )
			{
				reconnectServer( pConnecter );
			}
		}
		//记录日志信息到文件中;
		NDCLogManager* pLogMgr = m_pListenServer->logMgr();
		if ( NULL != pLogMgr )
		{
			pLogMgr->flush();
		}



		m_pListenNetIO->accept( 100 );
		m_pListenNetIO->checkSessionTimerFunction();

		NDSessionManager::getInstance()->eventLoop();

		NDServerTask::getInstance()->taskProcess();

		m_pListenServer->run();

		NDShareBaseGlobal::sleep2( 100 );

	}while( true );

	return true;
}

NDBool NDServerManager::setServerSessionProtocolType(NDUint32 nSessionID, NDUint8 nSessionProtocolType)
{
	return NDSessionManager::getInstance()->setServerSessionProtocolType( nSessionID, nSessionProtocolType );
}

NDBool NDServerManager::setClientSessionProtocolType(NDUint32 nSessionID, NDUint8 nSessionProtocolType)
{
	return NDSessionManager::getInstance()->setClientSessionProtocolType( nSessionID, nSessionProtocolType );
}

void NDServerManager::setDisposeSessionProtocol()
{
	NDSessionManager::getInstance()->setMaxSessionType( NDSessionProtocolType_MAX );
	NDSessionManager::getInstance()->setSpecialProtocol( CMDP_Special_Start, CMDP_Special_End );

	//客户端到服务器LOGIN类型;
	NDSessionManager::getInstance()->setDisposeSessionProtocol( NDSessionProtocolType_C2LS, CMDP_NDC2LS_Start, CMDP_NDC2LS_End );
	//客户端到服务器GATEWAY类型;
	NDSessionManager::getInstance()->setDisposeSessionProtocol( NDSessionProtocolType_C2GTWS, CMDP_NDC2GTWS_Start, CMDP_NDC2GTWS_End );
	//服务器WORLD到服务器LOGIN类型;
	NDSessionManager::getInstance()->setDisposeSessionProtocol( NDSessionProtocolType_WS2LS, CMDP_NDWS2LS_Start, CMDP_NDWS2LS_End );
	//服务器ROOM到服务器WORLD类型;
	NDSessionManager::getInstance()->setDisposeSessionProtocol( NDSessionProtocolType_RS2WS, CMDP_NDRS2WS_Start, CMDP_NDRS2WS_End );
	//服务器ROOM到服务器GAME类型;
	NDSessionManager::getInstance()->setDisposeSessionProtocol( NDSessionProtocolType_RS2GS, CMDP_NDRS2GS_Start, CMDP_NDRS2GS_End );
	//服务器GATEWAY到服务器GAME类型;
	NDSessionManager::getInstance()->setDisposeSessionProtocol( NDSessionProtocolType_GTWS2GS, CMDP_NDGWS2GS_Start, CMDP_NDGWS2GS_End );
	//服务器GATEWAY到服务器ROOM类型;
	NDSessionManager::getInstance()->setDisposeSessionProtocol( NDSessionProtocolType_GTWS2RS, CMDP_NDGWS2RS_Start, CMDP_NDGWS2RS_End );
	////服务器GATEWAY到服务器MESSAGE类型;
	//NDSessionManager::getInstance()->setDisposeSessionProtocol( NDSessionProtocolType_GTWS2MSGS, CMDP_NDGWS2MSGS_Start, CMDP_NDGWS2MSGS_End );
	//服务器GAME到服务器DATA类型;
	NDSessionManager::getInstance()->setDisposeSessionProtocol( NDSessionProtocolType_GS2DS, CMDP_NDGS2DS_Start, CMDP_NDGS2DS_End );
	//服务器GMAE到服务器WORLD类型;
	NDSessionManager::getInstance()->setDisposeSessionProtocol( NDSessionProtocolType_GS2WS, CMDP_NDGS2WS_Start, CMDP_NDGS2WS_End );



	//服务器DATA到服务器GAME类型;
	NDSessionManager::getInstance()->setDisposeSessionProtocol( NDSessionProtocolType_DS2GS, CMDP_NDDS2GS_Start, CMDP_NDDS2GS_End );
	//服务器WORLD到服务器GAME类型;
	NDSessionManager::getInstance()->setDisposeSessionProtocol( NDSessionProtocolType_WS2GS, CMDP_NDWS2GS_Start, CMDP_NDWS2GS_End );
	//服务器WORLD到服务器ROOM类型;
	NDSessionManager::getInstance()->setDisposeSessionProtocol( NDSessionProtocolType_WS2RS, CMDP_NDWS2RS_Start, CMDP_NDWS2RS_End );
	//服务器GAME到服务器ROOM类型;
	NDSessionManager::getInstance()->setDisposeSessionProtocol( NDSessionProtocolType_GS2RS, CMDP_NDGS2RS_Start, CMDP_NDGS2RS_End );
	//服务器ROOM到服务器GATEWAY类型;
	NDSessionManager::getInstance()->setDisposeSessionProtocol( NDSessionProtocolType_RS2GTWS, CMDP_NDRS2GTWS_Start, CMDP_NDRS2GTWS_End );
	//服务器GAME到服务器GATEWAY类型;
	NDSessionManager::getInstance()->setDisposeSessionProtocol( NDSessionProtocolType_GS2GTWS, CMDP_NDGS2GTWS_Start, CMDP_NDGS2GTWS_End );
	////服务器MESSAGE到服务器GATEWAY类型;
	//NDSessionManager::getInstance()->setDisposeSessionProtocol( NDSessionProtocolType_MSGS2GTWS, CMDP_NDMSGS2GTWS_Start, CMDP_NDMSGS2GTWS_End );
	//服务器LOGIN到服务器WORLD类型;
	NDSessionManager::getInstance()->setDisposeSessionProtocol( NDSessionProtocolType_LS2WS, CMDP_NDLS2WS_Start, CMDP_NDLS2WS_End );
	//服务器LOGIN到客户端类型;
	NDSessionManager::getInstance()->setDisposeSessionProtocol( NDSessionProtocolType_LS2C, CMDP_NDLS2C_Start, CMDP_NDLS2C_End );
	//服务器LOGIN到客户端类型;
	NDSessionManager::getInstance()->setDisposeSessionProtocol( NDSessionProtocolType_GTWS2C, CMDP_NDGTWS2C_Start, CMDP_NDGTWS2C_End );

}

NDBool NDServerManager::pingProtocolCommonDispose( NDUint32 nSessionID )
{
	NDServerSession* pServerSession = dynamic_cast<NDServerSession*>( NDSessionManager::getInstance()->findServerSession( nSessionID ) );
	if ( NULL == pServerSession )
	{
		return NDFalse;
	}

	pServerSession->setLastPingSecondTime( NDShareBase::NDShareBaseGlobal::getCurSecondTimeOfUTC() );

	NDPingResProtocol pingResProtocol;
	sendToClient( pingResProtocol, nSessionID );

	return NDTrue;
}



NDBool NDServerManager::disconnectNotifyCommonDispose( NDUint32 nSessionID, NDUint8 nDisconnectionType, NDRemoteServerManager* pRemoteServerManager )
{
	if ( NULL == pRemoteServerManager )
	{
		return NDFalse;
	}

	NDRemoteServerInfo* pRemoteServerInfo = pRemoteServerManager->getRemoteServerInfoBySessionID( nSessionID );
	if ( NULL == pRemoteServerInfo )
	{
		return NDFalse;
	}


	if ( nDisconnectionType > ESessionDisconnectionType_SEND_PASSIVE )
	{
		NDLOG_ERROR( " ESessionDisconnectionType is error, code=%u. ", nDisconnectionType );
		return NDFalse;
	}

	//dispose NDRemoteServer offline;
	//NDRemoteGameServerCommonInfo* pGameServerInfo	= dynamic_cast<NDRemoteGameServerCommonInfo*>(pRemoteServerInfo);
	const NDSocketAddress& rNetAddress	= pRemoteServerInfo->getNetAddress();

	NDLOG_ERROR( " %s [%s:%u] [ServerID:%u] offline. DisconnectionType:[%s]. ", pRemoteServerInfo->getServerName(),
																				rNetAddress.getIP(),
																				rNetAddress.getPort(),
																				pRemoteServerInfo->getServerID(),
																				getDisconnectionTypeStr( nDisconnectionType ) );


	pRemoteServerManager->removeRemoteServer( nSessionID );

	return NDTrue;
}

void NDServerManager::disconnectNotifyCommonErrorDispose(NDUint32 nSessionID, NDUint8 nDisconnectionType, const char* szServerName )
{
	NDLOG_ERROR( " [%s::disconnectNotifyDispose] connect error link. sessionID:%u. DisconnectionType:[%s]. ", szServerName,
																											nSessionID, 
																											getDisconnectionTypeStr( nDisconnectionType ) );
}

#define NDSAMEVALUE_STR(v)	(#v)

const char* NDServerManager::getDisconnectionTypeStr( NDUint8 nDisconnectionType )
{
	if ( nDisconnectionType == ESessionDisconnectionType_NULL )
	{
		return NDSAMEVALUE_STR( ESessionDisconnectionType_NULL ) ;
	}
	else if ( nDisconnectionType == ESessionDisconnectionType_PACKET_ERROR_ACTIVE )
	{
		return NDSAMEVALUE_STR( ESessionDisconnectionType_PACKET_ERROR_ACTIVE );
	}
	else if ( nDisconnectionType == ESessionDisconnectionType_PING_BEYOND_TIME_ACTIVE )
	{
		return NDSAMEVALUE_STR( ESessionDisconnectionType_PING_BEYOND_TIME_ACTIVE );
	}
	else if ( nDisconnectionType == ESessionDisconnectionType_READ_PASSIVE )
	{
		return NDSAMEVALUE_STR( ESessionDisconnectionType_READ_PASSIVE );
	}
	else if ( nDisconnectionType == ESessionDisconnectionType_SEND_PASSIVE )
	{
		return NDSAMEVALUE_STR( ESessionDisconnectionType_SEND_PASSIVE );
	}

	return NULL;
}

NDBool NDServerManager::pingResProtocolCommonDispose(NDUint32 nSessionID)
{
	const NDServerInfo* pServerInfo = getConnServerInfo( nSessionID );
	if ( NULL == pServerInfo )
	{
		NDLOG_ERROR( " [NDServerManager::pingResProtocolCommonDispose] pServerInfo is NULL!" )
		return NDFalse;
	}

	NDLOG_INFO( " ping %s [%s:%u] return response success. ", pServerInfo->getServerName(),
															pServerInfo->getServerIP(),
															pServerInfo->getServerPort() );

	return NDTrue;
}

NDBool NDServerManager::registerResCommonDispose(NDUint32 nSessionID, NDUint32 nResCode)
{
	const NDServerInfo* pServerInfo = getConnServerInfo( nSessionID );
	if ( NULL == pServerInfo )
	{
		NDLOG_ERROR( " [NDServerManager::registerResCommonDispose] pServerInfo is NULL!" )
		return NDFalse;
	}

	if ( eND_SRS_OK == nResCode )
	{
		NDLOG_INFO( " register %s [%s:%u] return response success.", pServerInfo->getServerName(),
																	pServerInfo->getServerIP(),
																	pServerInfo->getServerPort() );
	}
	else
	{
		NDLOG_ERROR( " register %s [%s:%u] return response failed. nErrorCode=%u.", pServerInfo->getServerName(),
																					pServerInfo->getServerIP(),
																					pServerInfo->getServerPort(),
																					nResCode );
	}

	return NDTrue;
}





