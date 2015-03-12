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

#include "local/NDServerInfo.h"
#include "local/NDLocalServer.h"



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
		char szErrBuf[BUF_LEN_128] = {0};
		ND_SNPRINTF( szErrBuf, sizeof(szErrBuf) - 1, " %s [%s:%u] start failed! ",  pListenServerInfo->getServerName(), 
																					pListenServerInfo->getServerIP(), 
																					pListenServerInfo->getServerPort() );
		NDLOG_ERROR( szErrBuf )

		return NDFalse;
	}
	

	m_pListenNetIO->setProcessor( process );
	m_pListenNetIO->setCommonDisconnectNtyProtocol( new NDDisconnectNtyProtocol );

	setDisposeProtocol();

	//m_pListenNetIO->SetCheckSessionTimer();

	ostringstream& oStr = m_pListenServer->getostringstream();
	oStr		<< " " << pListenServerInfo->getServerName() << " [" << pListenServerInfo->getServerIP() << ":" << pListenServerInfo->getServerPort() << "] listening ...";

	string strString( oStr.str() );
	const char* szStr = strString.c_str();
	NDLOG_INFO( szStr )
	oStr.clear();
	oStr.str("");

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
				pClientNetIO->setPingProtocol( new NDPingProtocol );
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

				char szBuf[BUF_LEN_128] = {0};
				ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " connect %s [%s:%u] success! ",	pServerInfo->getServerName(),
																						pServerInfo->getServerIP(),
																						pServerInfo->getServerPort() );
				NDLOG_INFO( szBuf )

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
					char szBuf[BUF_LEN_128] = {0};
					ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " connect %s [%s:%u] error! ",	pServerInfo->getServerName(),
																							pServerInfo->getServerIP(),
																							pServerInfo->getServerPort() );
					NDLOG_ERROR( szBuf )
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

			char szBuf[BUF_LEN_128] = {0};
			ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " disconnect %s [%s:%u]. ",	pServerInfo->getServerName(),
																				pServerInfo->getServerIP(),
																				pServerInfo->getServerPort() );
			NDLOG_ERROR( szBuf )


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
		ostringstream& oStr = m_pListenServer->getostringstream();
		oStr << " NDServerManager::mainLoop pConnecter is NULL! ListenServerType = " << pListenServerInfo->getServerName();
		string oStrTemp( oStr.str() );
		NDLOG_ERROR( oStrTemp.c_str() )
		oStr.clear();	//clear()清除当前错误控制状态;
		oStr.str("");	//str("")将缓冲区清零,清除脏数据;

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

NDBool NDServerManager::setSessionProtocolType( NDUint32 nSessionID, NDSessionProtocolType sessionProtocolType )
{
	if ( NULL == m_pListenNetIO )
	{
		return NDFalse;
	}

	return m_pListenNetIO->setSessionProtocolType( nSessionID, (NDUint8)sessionProtocolType );
}

void NDServerManager::setDisposeProtocol()
{
	if ( NULL == m_pListenNetIO )
	{
		return;
	}

	////客户端到服务器LOGIN类型;
	//m_pListenNetIO->SetDisposeProtocol( NDSessionProtocolType_CLIENT2L, CMD_NDClient2L_Start, CMD_NDClient2L_End );
	////客户端到服务器GATE类型;
	//m_pListenNetIO->SetDisposeProtocol( NDSessionProtocolType_CLIENT2GT, CMD_NDClient2Gate_Start, CMD_NDClient2Gate_End );
	////服务器GATE到服务器CENTER类型;
	//m_pListenNetIO->SetDisposeProtocol( NDSessionProtocolType_GT2CS, CMD_NDG2CS_Start, CMD_NDG2CS_End );
	////服务器GATE到服务器MAP类型;
	//m_pListenNetIO->SetDisposeProtocol( NDSessionProtocolType_GT2M, CMD_NDG2M_Start, CMD_NDG2M_End );
	////服务器MAP到服务器CENTER类型;
	//m_pListenNetIO->SetDisposeProtocol( NDSessionProtocolType_M2CS, CMD_NDM2CS_Start, CMD_NDM2CS_End );
	////服务器MAP到服务器GAMEDB类型;
	//m_pListenNetIO->SetDisposeProtocol( NDSessionProtocolType_M2GDB, CMD_NDM2GDB_Start, CMD_NDM2GDB_End );
	////服务器CENTER到服务器LOGIN类型;
	//m_pListenNetIO->SetDisposeProtocol( NDSessionProtocolType_CS2L, CMD_NDCS2L_Start, CMD_NDCS2L_End );
	////服务器CENTER到服务器GAMEDB类型;
	//m_pListenNetIO->SetDisposeProtocol( NDSessionProtocolType_CS2GDB, CMD_NDCS2GDB_Start, CMD_NDCS2GDB_End );
	////服务器LOGIN到服务器ACCOUNTDB类型;
	//m_pListenNetIO->SetDisposeProtocol( NDSessionProtocolType_L2ADB, CMD_NDL2A_Start, CMD_NDL2A_End );

	////服务器GATE到服务器MAP类型;(这条是客户端发送给GATESERVER,转化给MAPSERVER的);
	//m_pListenNetIO->SetDisposeProtocol( NDSessionProtocolType_GT2M, CMD_NDClient2Gate_Start, CMD_NDClient2Gate_End );
}

NDBool NDServerManager::pingProtocolCommonDispose( NDUint32 nSessionID )
{
	NDServerSession* pServerSession = dynamic_cast<NDServerSession*>( NDSessionManager::getInstance()->findServerSession( nSessionID ) );
	if ( NULL == pServerSession )
	{
		return NDFalse;
	}

	pServerSession->setLastPingSecondTime( NDShareBase::NDShareBaseGlobal::getCurSecondTimeOfUTC() );

	return NDTrue;
}



