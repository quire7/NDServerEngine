#include <iostream>
#include <cstdlib>

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
	printf( " getLocalSecondTimeStrForSingleThread:%s\n", pszTime );

	NDUint32 nDayofweek = NDShareBaseGlobal::getdayofweek( 2012, 11, 4 );
	printf( " 2012-11-4 is 星期:%d.\n", nDayofweek);

	char szBuf[64] = {0};

	NDShareBaseGlobal::getLocalSecondTimeStr( szBuf, 64 );
	memset( szBuf, 0, sizeof(szBuf) );

	string strAFile = strPath + "a.txt";
	NDCFile file;
	if (NDFalse == file.open( strAFile.c_str(), "a"))
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
	
	string str_crew_up_cost_path = strPath + "crew_up_cost.txt";
	NDExcelFile excelFile;
	if (NDFalse == excelFile.open(str_crew_up_cost_path.c_str()))
	{
		return 1;
	}

	printf( " nFileSize = %u, row=%u, column=%u. \n", excelFile.getFileSize(), excelFile.getRowNum(), excelFile.getColumnNum() );

	

	NDShareBaseGlobal::init( 1, 2 );

	NDUint64 nCurMSSeconds	= NDShareBaseGlobal::getCurMSTimeOfLoacl();
	time_t nCurSeconds	= NDShareBaseGlobal::getCurSecondTimeOfUTC();
	
	struct tm* localTM = localtime( &nCurSeconds );

	NDUint32 nCurSeconds2 = (NDUint32)mktime(localTM);

	printf(" nCurMSSeconds:%lld, nCurSeconds:%lld, nCurSeconds2:%u. \n", nCurMSSeconds, nCurSeconds, nCurSeconds2);

	char szCurTimeBuf[32] = {0};
	NDShareBaseGlobal::getLocalSecondTimeStr(szCurTimeBuf, sizeof(szCurTimeBuf));
	printf("%s\n", szCurTimeBuf);

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

		NDSessionManager::getInstance()->setMaxSessionType( NDSessionProtocolType_MAX );
		NDSessionManager::getInstance()->setSpecialProtocol( CMDP_Special_Start, CMDP_Special_End );
		//服务器LOGIN到客户端类型;
		NDSessionManager::getInstance()->setDisposeSessionProtocol( NDSessionProtocolType_GTWS2C, 0, 2 );


		for ( NDUint32 i = 0; i < 500; ++i )
		{
			NDClientNetIO* pClientNetIO = new NDClientNetIO;
			//NDProtocol* pPingProtocol	= new NDPingProtocol;
			pClientNetIO->setPingProtocol( new NDPingReqProtocol );
			if (pClientNetIO->connect( strIP.c_str(), nPort ))
			{
				pClientNetIO->setSessionProtocolType( NDSessionProtocolType_GTWS2C );

				NDEchoProtocol echoProtocol;
				echoProtocol.m_strData = strEchoData;
				
				pClientNetIO->sendToServer( echoProtocol, NDFalse, NDFalse, NDTrue );

				gClientMap[ pClientNetIO->getSessionID() ] = pClientNetIO;

				NDSessionManager::getInstance()->eventLoop();

				NDServerTask::getInstance()->taskProcess();

				NDShareBase::NDTimerEventManager::getInstance()->detectTimerList();
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

			NDShareBase::NDTimerEventManager::getInstance()->detectTimerList();

			NDShareBaseGlobal::sleep(100);
		}
	}
	else
	{
		std::cout << " read config file error! " << std::endl;
	}

	system("pause");
	return 0;
};
