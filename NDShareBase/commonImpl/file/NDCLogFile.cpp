#include "file/NDCLogFile.h"

#include "NDCLibFun.h"
#include "NDShareBaseGlobal.h"
#include "thread/NDMutex.h"
#include "file/NDCFile.h"


_NDSHAREBASE_BEGIN

NDCLogFile::NDCLogFile( const char* pFileName, const char* pOpenFlag, NDUint32 nCacheSize ) : m_nTotalSize(0), m_nCacheSize(nCacheSize), m_nCacheLogPos(0), 
																							  m_szLogCache(new char[nCacheSize]), m_pNDCFile( new NDCFile ), m_pLogMutex( new NDMutexLock )
{
	if (NULL != m_pNDCFile)
	{
		m_pNDCFile->open( pFileName, pOpenFlag );
	}
}

NDCLogFile::~NDCLogFile(void)
{
	close();
}

NDBool NDCLogFile::isopen() const
{
	if ( NULL == m_pNDCFile ) return NDFalse;
	return m_pNDCFile->isopen();
}

void NDCLogFile::close()
{
	if ( NULL == m_pNDCFile ) return;

	flush();

	m_pNDCFile->flush();
	m_pNDCFile->close();

	if ( NULL != m_szLogCache )
	{
		delete []m_szLogCache;
		m_szLogCache = NULL;
	}

	delete m_pNDCFile;
	m_pNDCFile = NULL;

	if ( NULL != m_pLogMutex )
	{
		delete m_pLogMutex;
		m_pLogMutex = NULL;
	}
}

NDUint32 NDCLogFile::size()
{
	if ( NULL == m_pNDCFile ) return 0;
	return m_nTotalSize;	//return m_pNDCFile->size();
}

NDBool NDCLogFile::write( const char* pFile, NDInt32 nLine, NDInt32 nLevel, const char* pFormat, ... )
{
	if ( NDFalse == isopen() )  return NDFalse;

	va_list ap;
	va_start( ap, pFormat );
	string strContent( getWriteContent( pFile, nLine, nLevel, pFormat, ap ) );
	va_end(ap);

	const char* pBuf = strContent.c_str();
	NDUint32 nlen	 = (NDUint32)strContent.length();
	if ( (nlen + m_nCacheLogPos) >= m_nCacheSize )
	{	//写入的数据大于缓存大小;
		m_nTotalSize += nlen;
		//将缓存日志内存数据写入文件;
		flush();
		//add locker;
		NDGuardLock logLocker( *m_pLogMutex );
		return ( nlen == m_pNDCFile->write( pBuf, nlen ) );		
	}
	//add locker;
	NDGuardLock logLocker( *m_pLogMutex );
	memcpy( m_szLogCache + m_nCacheLogPos, pBuf, nlen );
	m_nCacheLogPos	+= nlen;
	m_nTotalSize	+= nlen;
	return NDTrue;
}

string  NDCLogFile::getWriteContent( const char* pFile, NDInt32 nLine, NDInt32 nLevel, const char* pFormat, va_list ap )
{
	const char* szLogTypeName = NDShareBaseGlobal::getLogTypeName( nLevel );
	if ( NULL == szLogTypeName )
	{
		return string("");
	}

	char szBuf[MAX_STRING_LEN]		= {0};
	char szTimeBuf[ND_TIME_LENGTH]	= {0};
	NDShareBaseGlobal::getLocalSecondTimeStr( szTimeBuf, ND_TIME_LENGTH );
	char szFileName[BUF_LEN_64]		= {0};
	NDShareBaseGlobal::extractFileName( pFile, szFileName, BUF_LEN_64 );

	NDUint32 nlen  = ND_SNPRINTF( szBuf, MAX_STRING_LEN - 1, "[%s] [%s] [%llu] [%s:%d]: ",	szLogTypeName,
																							szTimeBuf,
																							NDShareBaseGlobal::getCurMSTimeOfUTC(),
																							szFileName,
																							nLine);
	nlen += ND_VSNPRINTF( (szBuf + nlen), (MAX_STRING_LEN - nlen - 1), pFormat, ap );
	nlen += ND_SNPRINTF( (szBuf + nlen), (MAX_STRING_LEN - nlen - 1), "%s", NEWLINE_SYMBOL );

	return string(szBuf);
}

NDBool NDCLogFile::write( const char* pFile, NDInt32 nLine, NDInt32 nLevel, const char* pFormat, va_list ap )
{
	if ( NDFalse == isopen() )  return NDFalse;

	string strContent( getWriteContent( pFile, nLine, nLevel, pFormat, ap ) );

	const char* pBuf = strContent.c_str();
	NDUint32 nlen	 = (NDUint32)strContent.length();
	if ( (nlen + m_nCacheLogPos) >= m_nCacheSize )
	{	//写入的数据大于缓存大小;
		m_nTotalSize += nlen;
		//将缓存日志内存数据写入文件;
		flush();
		//add locker;
		NDGuardLock logLocker( *m_pLogMutex );
		return ( nlen == m_pNDCFile->write( pBuf, nlen ) );		
	}
	//add locker;
	NDGuardLock logLocker( *m_pLogMutex );
	memcpy( m_szLogCache + m_nCacheLogPos, pBuf, nlen );
	m_nCacheLogPos	+= nlen;
	m_nTotalSize	+= nlen;
	return NDTrue;
}

NDBool NDCLogFile::flush()
{
	if ( NDFalse == isopen() )  return NDFalse;

	bool bRet = NDFalse;
	NDGuardLock logLocker( *m_pLogMutex );
	if ( m_nCacheLogPos > 0 && ( m_nCacheLogPos == m_pNDCFile->write( m_szLogCache, m_nCacheLogPos ) ) )
	{
		m_nCacheLogPos  = 0;
		bRet			= NDTrue;

		//这个到底要不要强制flush,有待探讨;
		m_pNDCFile->flush();
	}
	return bRet;
}


////////////////////////////////////////////////////////////////////////////////////////////////
NDCLogManager* g_pFileLogManager = NULL;

NDCLogManager::NDCLogManager()
{

}



NDCLogManager::~NDCLogManager()
{
	if ( NULL != m_pszLogPath )
	{
		delete []m_pszLogPath;
		m_pszLogPath = NULL;
	}
	if ( NULL != m_pszLogBaseName )
	{
		delete []m_pszLogBaseName;
		m_pszLogBaseName = NULL;
	}
	if ( NULL != m_pLogFile )
	{
		delete m_pLogFile;
		m_pLogFile = NULL;
	}
}

NDBool NDCLogManager::init( const char* szLogPath, const char* szLogBaseName, NDUint32 nLogMaxSize )
{
	if ( ( NULL == szLogPath ) || ( '\0' == szLogPath ) || ( NULL == szLogBaseName ) || ( '\0' == szLogBaseName ) )
	{
		return NDFalse;
	}

	NDUint32 nLen	= (NDUint32)strlen(szLogPath);
	m_pszLogPath	= new char[nLen + 1];
	memcpy( m_pszLogPath, szLogPath, nLen );
	m_pszLogPath[nLen] = '\0';

	nLen			= (NDUint32)strlen(szLogBaseName);
	m_pszLogBaseName= new char[nLen + 1];
	memcpy( m_pszLogBaseName, szLogBaseName, nLen );
	m_pszLogBaseName[nLen] = '\0';

	m_nLogMaxSize = nLogMaxSize;

	return initFile();
}

NDBool NDCLogManager::initFile()
{
	char szTimeBuf[ND_TIME_LENGTH] = {0};
	NDShareBaseGlobal::getLocalSecondTimeStr( szTimeBuf, ND_TIME_LENGTH );
	szTimeBuf[10] = '-';
	szTimeBuf[13] = '-';
	szTimeBuf[16] = '-';	//fopen路径规则中不能有:这个符号,否则创建不了文件;

	//最终文件名;
	char szFileName[MAX_PATH_LEN] = {0};

	NDUint32 nLogPathLen = (NDUint32)strlen( m_pszLogPath );
	if ( '/' != m_pszLogPath[ nLogPathLen - 1 ])
	{
		ND_SNPRINTF( szFileName, MAX_PATH_LEN - 1, "%s/%s-%s-local.log", m_pszLogPath, m_pszLogBaseName, szTimeBuf );
	}
	else
	{
		ND_SNPRINTF( szFileName, MAX_PATH_LEN - 1, "%s%s-%s-local.log", m_pszLogPath, m_pszLogBaseName, szTimeBuf );
	}
	

	m_pLogFile = new NDCLogFile( szFileName, "ab" );
	if ( NULL == m_pLogFile )
	{
		return NDFalse;
	}

	return m_pLogFile->isopen();
}

NDBool NDCLogManager::write( const char* pFile, NDInt32 nLine, NDInt32 nLevel, const char* pFormat, ... )
{
	if ( NULL == m_pLogFile )				return NDFalse;
	if ( NDFalse == m_pLogFile->isopen() )	return NDFalse;

	if ( m_pLogFile->size() >= m_nLogMaxSize  )
	{
		delete m_pLogFile;
		m_pLogFile = NULL;

		if ( NDFalse == initFile() )
		{
			return NDFalse;
		}
	}

	va_list ap;
	va_start( ap, pFormat );
	NDBool bRet = m_pLogFile->write( pFile, nLine, nLevel, pFormat, ap );
	va_end(ap);
	return bRet;
}

void NDCLogManager::close()
{
	if ( NULL != m_pLogFile )
	{
		m_pLogFile->close();
	}
}

NDBool NDCLogManager::flush()
{
	if ( NULL == m_pLogFile )
	{
		return NDFalse;
	}

	return m_pLogFile->flush();
}







_NDSHAREBASE_END

