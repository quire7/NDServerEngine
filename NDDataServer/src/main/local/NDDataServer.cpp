
#ifdef WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif


#include "main/local/NDDataServer.h"

initialiseSingleton(NDDataServer);

NDRefCounted<NDServerManager>	refNDServerManager( new NDServerManager );
NDRefCounted<NDDataServer>		refNDDataServer( new NDDataServer );

#ifdef	WIN32
BOOL WINAPI handlerRoutine( DWORD dwCtrlType );
#endif

void	exceptionDispose( void *pParam );
int		mainWapper( int argc, char** argv );

int main(int argc, char** argv)
{
	ND_EXCEPTION_PROTECTED_BEGIN()

	mainWapper( argc, argv );

	ND_EXCEPTION_PROTECTED_CATCH( sNDDataServer.getDumpFilePath(), exceptionDispose )

	return 0;
}


int mainWapper( int argc, char** argv )
{
#ifdef	WIN32
	//for output memory leak report;
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	if( !SetConsoleCtrlHandler( (PHANDLER_ROUTINE)handlerRoutine, TRUE ) ) 
	{
		NDLOG_ERROR( " SetConsoleCtrlHandler failed! " )
		return 1;
	}
#endif

	if ( argc <= 1 )
	{
		NDLOG_ERROR( " argc number less than or equal 1. " )
		return 1;
	}

	NDInt32 nServerID = atoi( argv[1] );
	if ( nServerID <= 0 || nServerID > 200 )
	{
		NDLOG_ERROR( " nServerID less than zero or greater than 200. " )
		return 1;
	}

	//read config file and initialize;
	if ( NDFalse == sNDDataServer.initialize( DB_SERVER, nServerID, "ServerConfig.xml") )
	{
		NDLOG_ERROR( " NDDataServer initialize failed! " )
		return 1;
	}

	//start server service;
	if ( NDFalse == NDServerManager::getSingleton().startServer( NDDataServer::getSingletonPtr(), NDFalse ) )
	{
		NDLOG_ERROR( " NDDataServer startServer failed! " )
		return 1;
	}

	//set server started state;
	sNDDataServer.setState( NDLocalServer::eServState_Started );


	NDServerManager::getSingleton().mainLoop();

	return 0;
}


void exceptionDispose( void *pParam )
{
	NDLOG_ERROR( " exception has generated! " )
	sNDDataServer.setState( NDLocalServer::eServState_Stopping );
	sNDDataServer.logMgr()->flush();
}

#ifdef	WIN32
//»Øµ÷º¯Êý;
BOOL WINAPI handlerRoutine( DWORD dwCtrlType )
{
	NDDataServer* pDataServer		= NDDataServer::getSingletonPtr();
	NDServerManager* pServerMgr		= NDServerManager::getSingletonPtr();
	if ( NULL == pDataServer || NULL == pServerMgr )
	{
		return FALSE;
	}

	pDataServer->setState( NDLocalServer::eServState_Stopping );

	switch( dwCtrlType )
	{
	case CTRL_C_EVENT:
		NDLOG_WARNING( " CTRL_C_EVENT " )
		break;
	case CTRL_BREAK_EVENT:
		NDLOG_WARNING( " CTRL_BREAK_EVENT " )
		break;
	case CTRL_CLOSE_EVENT:
		NDLOG_WARNING( " CTRL_CLOSE_EVENT " )
		break;
	case CTRL_LOGOFF_EVENT:
		NDLOG_WARNING( " CTRL_LOGOFF_EVENT " )
		break;
	case CTRL_SHUTDOWN_EVENT:
		NDLOG_WARNING( " CTRL_SHUTDOWN_EVENT " )
		break;
	default:
		NDLOG_WARNING( " default " )
		break;
	}

	pDataServer->logMgr()->close();

	SAFE_DELETE( pServerMgr )
	SAFE_DELETE( pDataServer )

	return TRUE;
}
#endif

