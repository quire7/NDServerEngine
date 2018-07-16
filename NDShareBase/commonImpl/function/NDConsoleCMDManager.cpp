#include "function/NDConsoleCMDManager.h"

#include "NDCLibFun.h"

#include "NDShareBaseGlobal.h"
#include "file/NDLog.h"



_NDSHAREBASE_BEGIN

NDConsoleCMDManager::NDConsoleCMDManager()
{
	m_consoleCMDMap.clear();
}

NDConsoleCMDManager::~NDConsoleCMDManager()
{
	for ( ConsoleCMDMapIter iterBegin = m_consoleCMDMap.begin(),
							iterEnd	  = m_consoleCMDMap.end();
							iterBegin!= iterEnd;
							++iterBegin )
	{
		SAFE_DELETE( iterBegin->second )
	}
	m_consoleCMDMap.clear();
}

void NDConsoleCMDManager::registerCommad( const string& strKey, NDConsoleCommand* pConsoleCMD )
{
	ConsoleCMDMapIter iterFind = m_consoleCMDMap.find( strKey );
	if ( iterFind == m_consoleCMDMap.end() )
	{
		m_consoleCMDMap.insert( std::make_pair( strKey, pConsoleCMD ) );
	}
}

NDBool NDConsoleCMDManager::process( const string& strMsg )
{
	vector<string> strVec;
	NDShareBaseGlobal::strsplit( strMsg, string(" "), strVec );

	if (strVec.empty())
	{
		NDLOG_ERROR( " message: %s not dispose. ", strMsg.c_str() );

		return NDFalse;
	}

	ConsoleCMDMapIter iterFind = m_consoleCMDMap.find( strVec[0] );
	if ( iterFind == m_consoleCMDMap.end() )
	{
		NDLOG_ERROR( " message: %s not dispose. ", strMsg.c_str() );

		return NDFalse;
	}

	NDConsoleCommand* pNDConsoleCommand = iterFind->second;
	if ( NULL == pNDConsoleCommand )
	{
		NDLOG_ERROR( " message: %s not dispose. ", strMsg.c_str() );

		return NDFalse;
	}

	pNDConsoleCommand->process( strVec );

	return NDTrue;
}


_NDSHAREBASE_END
