
#ifdef WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif


#include "main/local/NDGatewayServer.h"

initialiseSingleton(NDGatewayServer);

NDRefCounted<NDServerManager>	refNDServerManager( new NDServerManager );
NDRefCounted<NDGatewayServer>	refNDGatewayServer( new NDGatewayServer );

#ifdef	WIN32
BOOL WINAPI handlerRoutine( DWORD dwCtrlType );
#endif

void	exceptionDispose( void *pParam );
int		mainWapper( int argc, char** argv );

int main(int argc, char** argv)
{
	ND_EXCEPTION_PROTECTED_BEGIN()

	mainWapper( argc, argv );

	ND_EXCEPTION_PROTECTED_CATCH( sNDGatewayServer.getDumpFilePath(), exceptionDispose )

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
	if ( NDFalse == sNDGatewayServer.initialize( GATEWAY_SERVER, nServerID, "ServerConfig.xml") )
	{
		NDLOG_ERROR( " NDGatewayServer initialize failed! " )
		return 1;
	}

	//start server service;
	if ( NDFalse == NDServerManager::getSingleton().startServer( NDGatewayServer::getSingletonPtr() ) )
	{
		NDLOG_ERROR( " NDGatewayServer startServer failed! " )
		return 1;
	}

	//set server started state;
	sNDGatewayServer.setState( NDLocalServer::eServState_Started );


	NDServerManager::getSingleton().mainLoop();

	return 0;
}


void exceptionDispose( void *pParam )
{
	NDLOG_ERROR( " exception has generated! " )
	sNDGatewayServer.setState( NDLocalServer::eServState_Stopping );
	sNDGatewayServer.logMgr()->flush();
}

#ifdef	WIN32
//»Øµ÷º¯Êý;
BOOL WINAPI handlerRoutine( DWORD dwCtrlType )
{
	NDGatewayServer* pGatewayServer	= NDGatewayServer::getSingletonPtr();
	NDServerManager* pServerMgr		= NDServerManager::getSingletonPtr();
	if ( NULL == pGatewayServer || NULL == pServerMgr )
	{
		return FALSE;
	}

	pGatewayServer->setState( NDLocalServer::eServState_Stopping );

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

	pGatewayServer->logMgr()->close();

	SAFE_DELETE( pServerMgr )
	SAFE_DELETE( pGatewayServer )

	return TRUE;
}
#endif

