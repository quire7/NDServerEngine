
#ifdef WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <Windows.h>
#endif

#include <signal.h>

#include "NDLogServer.h"

#include "NDShareBaseGlobal.h"
#include "sharememory/NDShareMemoryUnitManager.h"
#include "function/NDRefCounted.h"
using NDShareBase::NDRefCounted;

initialiseSingleton(NDLogServer);
NDRefCounted<NDLogServer>		refNDLogServer( new NDLogServer );

#ifdef	WIN32
BOOL WINAPI handlerRoutine( DWORD dwCtrlType );
#endif

void	exceptionDispose( void *pParam );
int		mainWapper( int argc, char** argv );

int main(int argc, char** argv)
{
	ND_EXCEPTION_PROTECTED_BEGIN()

	mainWapper( argc, argv );

	ND_EXCEPTION_PROTECTED_CATCH( "", exceptionDispose )

	return 0;
}


int mainWapper( int argc, char** argv )
{
#ifdef	WIN32
	//for output memory leak report;
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	if( !SetConsoleCtrlHandler( (PHANDLER_ROUTINE)handlerRoutine, TRUE ) ) 
	{
		//NDLOG_ERROR( " SetConsoleCtrlHandler failed! " )
		return 1;
	}
#endif

	if ( NDFalse == refNDLogServer->init() )
	{
		return 1;
	}
	
	if ( NDFalse == refNDLogServer->loop() )
	{
		return 1;
	}

	if ( NDFalse == refNDLogServer->release() )
	{
		return 1;
	}

	return 0;
}


void exceptionDispose( void *pParam )
{
	//NDLOG_ERROR( " exception has generated! " )
	refNDLogServer->release();
}

#ifdef	WIN32
//回调函数;
BOOL WINAPI handlerRoutine( DWORD dwCtrlType )
{
	NDLogServer* pLogServer = NDLogServer::getSingletonPtr();
	if ( NULL == pLogServer )
	{
		return FALSE;
	}

	switch( dwCtrlType )
	{
	case CTRL_C_EVENT:
		//NDLOG_WARNGING( " CTRL_C_EVENT " )
		break;
	case CTRL_BREAK_EVENT:
		//NDLOG_WARNGING( " CTRL_BREAK_EVENT " )
		break;
	case CTRL_CLOSE_EVENT:
		//NDLOG_WARNGING( " CTRL_CLOSE_EVENT " )
		break;
	case CTRL_LOGOFF_EVENT:
		//NDLOG_WARNGING( " CTRL_LOGOFF_EVENT " )
		break;
	case CTRL_SHUTDOWN_EVENT:
		//NDLOG_WARNGING( " CTRL_SHUTDOWN_EVENT " )
		break;
	default:
		//NDLOG_WARNGING( " default " )
		break;
	}

	pLogServer->release();
	
	SAFE_DELETE( pLogServer )

	return TRUE;
}
#endif

////////////////////////////////////////////////////////////////////////
NDLogServer::NDLogServer() :m_bQuit( NDTrue ), m_pNDShareLogCacheSMUPool ( new NDShareMemoryUnitPool<NDShareLogCacheSMU>() )
{
	m_NDLogFileInfoMap.clear();
}

NDLogServer::~NDLogServer()
{
}

NDBool NDLogServer::newStaticManager()
{
	return NDTrue;
}

NDBool NDLogServer::delStaticManager()
{
	return NDTrue;
}

NDBool NDLogServer::initStaticManager()
{
	if ( NULL == m_pNDShareLogCacheSMUPool )
	{
		return NDFalse;
	}

	NDBool bRet = m_pNDShareLogCacheSMUPool->init( eNDSMKEY_LOG, ND_LOG_SMU_MAX, eNDSMU_OWN_TYPE_SELF );
	if ( NDFalse == bRet )
	{
		return NDFalse;
	}

	return NDTrue;
}

NDBool NDLogServer::init()
{
	if ( NDFalse == newStaticManager() )
	{
		return NDFalse;
	}

	if ( NDFalse == initStaticManager() )
	{
		return NDFalse;
	}

	m_bQuit = NDFalse;

	regSignalFunction();

	return NDTrue;
}

NDBool NDLogServer::release()
{
	delStaticManager();

	for ( NDLogFileInfoMapIter iter = m_NDLogFileInfoMap.begin(), iterEnd = m_NDLogFileInfoMap.end();
								iter != iterEnd;
								++iter )
	{
		NDLogFileInfo* pNDLogFileInfo = iter->second;
		if ( NULL == pNDLogFileInfo )
		{
			continue;
		}

		pNDLogFileInfo->m_file.close();

		delete pNDLogFileInfo;
	}
	if ( NULL != m_pNDShareLogCacheSMUPool )
	{
		delete m_pNDShareLogCacheSMUPool;
		m_pNDShareLogCacheSMUPool = NULL;
	}

	return NDTrue;
}

NDBool NDLogServer::loop()
{
	NDBool bInvalid			= NDFalse;
	NDTime nCurSeconds		= 0;
	NDTime nLastOpUnitTime	= 0;

	NDUint32 i				= 0;
	NDUint32 nPoolSize		= 0;

	NDShareLogCacheSMU* pNDShareLogCacheSMU = NULL;

	vector<NDShareLogCacheSMU*>	NDShareLogCacheSMUVec;

	do 
	{
		nCurSeconds = NDShareBaseGlobal::getCurSecondTimeOfUTC();
		nPoolSize	= m_pNDShareLogCacheSMUPool->getPoolSize();
		for ( i = 0; i < nPoolSize; ++i )
		{
			pNDShareLogCacheSMU = m_pNDShareLogCacheSMUPool->getPoolObj( i );
			if ( NULL == pNDShareLogCacheSMU )
			{
				continue;
			}

			bInvalid		= NDFalse;
			nLastOpUnitTime	= pNDShareLogCacheSMU->getSaveTime();
			if ( ( nCurSeconds - nLastOpUnitTime ) > 180 )
			{
				bInvalid = NDTrue;
			}

			if ( ( eNDSMU_USE_STATE_READFREE ==  pNDShareLogCacheSMU->getUseStatus() ) || bInvalid )
			{
				if ( NDTrue == saveLog( pNDShareLogCacheSMU ) )
				{
					pNDShareLogCacheSMU->setLogDataSize( 0 );
					pNDShareLogCacheSMU->setLogNameHash( 0 );
					//pNDShareLogCacheSMU->setUseStatus( eNDSMU_USE_STATE_FREEED );
				}
				
				//还是LOGSERVER自己直接回收吧;
				pNDShareLogCacheSMU->setUseStatus( eNDSMU_USE_STATE_FREE );
				pNDShareLogCacheSMU->setOwnType( eNDSMU_OWN_TYPE_SELF );
				NDShareLogCacheSMUVec.push_back( pNDShareLogCacheSMU );
			}
		}

		if ( !NDShareLogCacheSMUVec.empty() )
		{
			nPoolSize = (NDUint32)NDShareLogCacheSMUVec.size();
			for ( i = 0; i < nPoolSize; ++i )
			{
				m_pNDShareLogCacheSMUPool->destroyObj( NDShareLogCacheSMUVec[i] );
			}
			NDShareLogCacheSMUVec.clear();
		}

		checkLogFileInfo();

		NDShareBaseGlobal::sleep2( 3000 );

	} while ( !m_bQuit );

	return NDTrue;
}

NDBool NDLogServer::saveLog( NDShareLogCacheSMU *pNDShareLogCacheSMU )
{
	if ( NULL == pNDShareLogCacheSMU )
	{
		return NDFalse;
	}

	NDUint32 nLogDataSize = pNDShareLogCacheSMU->getLogDataSize();
	NDUint32 nLogNameHash = pNDShareLogCacheSMU->getLogNameHash();
	if ( 0 == nLogDataSize || 0 == nLogNameHash )
	{
		return NDTrue;
	}

	NDLogFileInfo *pNDLogFileInfo	= NULL;
	NDLogFileInfoMapIter iterFind	= m_NDLogFileInfoMap.find( nLogNameHash );
	if ( iterFind == m_NDLogFileInfoMap.end() )
	{
		pNDLogFileInfo	= new NDLogFileInfo;
		pNDLogFileInfo->m_nLogNameHash	= nLogNameHash;

		if ( NDFalse == pNDLogFileInfo->m_file.open( pNDShareLogCacheSMU->getLogName(), "ab+" ) )
		{	
			delete pNDLogFileInfo;
			return NDFalse;
		}

		m_NDLogFileInfoMap.insert( std::make_pair( nLogNameHash, pNDLogFileInfo ) );
	}
	else
	{
		pNDLogFileInfo = iterFind->second;
		if ( NULL == pNDLogFileInfo )
		{
			return NDFalse;
		}
	}

	//向文件中写入数据;
	pNDLogFileInfo->m_file.write( pNDShareLogCacheSMU->getLogBuf(), nLogDataSize );
	pNDLogFileInfo->m_file.flush();
	
	pNDLogFileInfo->m_nLastWriteTime = NDShareBaseGlobal::getCurSecondTimeOfUTC();

	return NDTrue;
}

void NDLogServer::checkLogFileInfo()
{
	if ( m_NDLogFileInfoMap.empty() )	return;

	NDTime nCurSeconds = NDShareBaseGlobal::getCurSecondTimeOfUTC();

	NDLogFileInfo *pNDLogFileInfo	= NULL;
	for ( NDLogFileInfoMapIter iter = m_NDLogFileInfoMap.begin(); iter != m_NDLogFileInfoMap.end(); )
	{
		pNDLogFileInfo = iter->second;
		if ( NULL == pNDLogFileInfo )
		{
			m_NDLogFileInfoMap.erase( iter++ );
			continue;
		}
 
		if ( ( nCurSeconds - pNDLogFileInfo->m_nLastWriteTime ) > 360 )
		{
			pNDLogFileInfo->m_file.close();

			delete pNDLogFileInfo;

			m_NDLogFileInfoMap.erase( iter++ );
			continue;
		}

		++iter;
	}
}


void NDLogServer::termHandlerDispose()
{
	m_bQuit = NDTrue;
}

void termHandler(int)
{
	refNDLogServer->termHandlerDispose();
}

NDBool NDLogServer::regSignalFunction()
{
	signal( SIGTERM, termHandler );

	return NDTrue;
}


