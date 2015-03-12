#include <iostream>
#include <cstdlib>

#include <Windows.h>

#include <time.h>

using namespace std;

#include "NDShareBaseGlobal.h"
#include "NDCLibFun.h"
#include "file/NDCFile.h"
#include "file/NDExcelFile.h"
#include "file/NDConfig.h"
#include "file/NDCLogFile.h"
#include "net/session/NDSessionManager.h"
#include "net/process/NDServerTask.h"

#include <algorithm>
#include "stl/NDSTLExtend.h"
using NDShareBase::IsObjEqualUnary;

#include "event/timerEvent/NDTimerEventManager.h"

#include "special/NDSpecialProtocol.h"

#include "NDMessageManager.h"


map<NDUint32, NDClientNetIO*> gClientMap;


struct Corps
{
public:
	int		m_nMemNum;
	int		m_nCorpsID;

	Corps()	{memset( this, 0, sizeof(Corps) );}
	int		GetCorpsID() const { return m_nCorpsID; };
};
typedef vector<Corps>				CorpsVec;
typedef CorpsVec::iterator			CorpsVecIter;
typedef vector<Corps*>				CorpsPtrVec;
typedef CorpsPtrVec::iterator		CorpsPtrVecIter;


int main(int argc, char ** argv)
{
	string strPath = NDShareBaseGlobal::getMoudlePath();
	strPath += "/";
	//NDCLogManager logManager( strPath.c_str() );

	char* pszTime = NDShareBaseGlobal::getLocalSecondTimeStrForSingleThread();

	NDUint32 nDayofweek = NDShareBaseGlobal::getdayofweek( 2012, 11, 4 );

	char szBuf[64] = {0};

	NDShareBaseGlobal::getLocalSecondTimeStr( szBuf, 64 );
	memset( szBuf, 0, sizeof(szBuf) );

	NDCFile file;
	if ( NDFalse == file.open( "a.txt", "a" ) )
	{
		return 1;
	}

	for ( int i = 0; i < 3; ++i )
	{
		ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " ni hao %d \n", i );
		file.write( szBuf, strlen(szBuf) );

		memset( szBuf, 0, sizeof(szBuf) );
	}

	file.close();


	int temp = 3;
	CorpsPtrVec corpsPtrVec;
	for ( int j = 0; j < 10; ++j )
	{
		Corps* pCorps = new Corps;
		pCorps->m_nCorpsID	= j;
		pCorps->m_nMemNum	= j * j;

		corpsPtrVec.push_back( pCorps );
	}
	CorpsPtrVecIter iterFind2 = find_if( corpsPtrVec.begin(), corpsPtrVec.end(), IsObjEqualUnary<int, Corps*>( temp, &Corps::GetCorpsID ) );
	if ( iterFind2 != corpsPtrVec.end() )
	{
		Corps* pCorps = *iterFind2;
		printf( "pCorps->m_nMemNum = %d.\n", pCorps->m_nMemNum );
	}
	
	NDExcelFile excelFile;
	if ( NDFalse == excelFile.open("crew_up_cost.txt") )
	{
		return 1;
	}

	printf( " nFileSize = %u, row=%u, column=%u. \n", excelFile.getFileSize(), excelFile.getRowNum(), excelFile.getColumnNum() );

	

	NDShareBaseGlobal::init( 1, 2 );

	NDUint64 nCurMSSeconds	= NDShareBaseGlobal::getCurMSTimeOfLoacl();
	time_t nCurSeconds	= NDShareBaseGlobal::getCurSecondTimeOfUTC();
	
	struct tm* localTM = localtime( &nCurSeconds );

	NDUint32 nCurSeconds2	= mktime( localTM );

	SYSTEMTIME sys; 
	GetLocalTime( &sys ); 
	printf( "%4d/%02d/%02d %02d:%02d:%02d.%03d ÐÇÆÚ%1d\n",sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond,sys.wMilliseconds,sys.wDayOfWeek);

	string strMoudle = NDShareBaseGlobal::getMoudlePath();
	strMoudle += "/NDClientNetIOTest.conf";

	NDConfigFile configFile;
	if ( NDTrue == configFile.setSource( strMoudle.c_str() ) )
	{
		string strIP	= configFile.getStringDefault( "serverInfo", "address", "127.0.0.1" );
		NDUint16 nPort	= (NDUint16)configFile.getIntDefault( "serverInfo", "port", 5503 );

		string  strEchoData = configFile.getStringDefault( "echo", "data", "hello, sweetheart, wolaiye! IOCP server is OK!" );

		NDClientNetIO::setProcessor( NDMessageManager::getInstance()->getDataProcess() );
		NDMessageManager::getInstance()->RegisterMessage();


		for ( NDUint32 i = 0; i < 500; ++i )
		{
			NDClientNetIO* pClientNetIO = new NDClientNetIO;
			//NDProtocol* pPingProtocol	= new NDPingProtocol;
			pClientNetIO->setPingProtocol( new NDPingProtocol );
			if (pClientNetIO->connect( strIP.c_str(), nPort ))
			{
				NDEchoProtocol echoProtocol;
				echoProtocol.m_strData = strEchoData;
				
				pClientNetIO->sendToServer( echoProtocol, NDFalse, NDFalse, NDTrue );

				gClientMap[ pClientNetIO->getSessionID() ] = pClientNetIO;

				NDSessionManager::getInstance()->eventLoop();

				NDServerTask::getInstance()->taskProcess();

				NDShareBase::NDTimerManager::getInstance()->detectTimerList();
			}
			else
			{
				delete pClientNetIO;
				pClientNetIO = NULL;
			}
		}


		while ( 1 )
		{
			NDSessionManager::getInstance()->eventLoop();

			NDServerTask::getInstance()->taskProcess();

			NDShareBase::NDTimerManager::getInstance()->detectTimerList();

			Sleep( 100 );
		}
	}
	else
	{
		std::cout << " read config file error! " << std::endl;
	}

	system("pause");
	return 0;
};
