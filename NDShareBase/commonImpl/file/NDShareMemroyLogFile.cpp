#include "file/NDShareMemroyLogFile.h"

#include "NDCLibFun.h"
#include "NDShareBaseGlobal.h"

#include "thread/NDMutex.h"

#include "sharememory/NDShareMemoryUnitManager.h"

_NDSHAREBASE_BEGIN

NDBool NDShareLogCacheSMU::tryLock( NDUint16 nLockType )
{
	return NDShareBaseGlobal::sm_try_lock( &m_NDSMUHead.m_nOwnType, nLockType );
}

NDBool NDShareLogCacheSMU::tryUnlock( NDUint16 nUnlockType )
{
	return NDShareBaseGlobal::sm_try_unlock( &m_NDSMUHead.m_nOwnType, nUnlockType );
}

NDBool NDShareLogCacheSMU::setLogName( const char* szLogName )
{
	if ( NULL == szLogName || '\0' == szLogName[0] )
	{
		return NDFalse;
	}
	NDUint32 nLen		= (NDUint32)strlen( szLogName );
	NDUint32 nNameLen	= sizeof( m_szLogName );
	if ( nLen >= nNameLen )
	{
		return NDFalse;
	}
	
	memcpy( m_szLogName, szLogName, nLen );
	m_szLogName[nLen] = '\0';

	return NDTrue;
}

NDBool NDShareLogCacheSMU::writeLog( const char* szBuf, NDUint32 nBufSize )
{
	static NDUint32 nLogBufMaxSize = sizeof(m_szLogBuf);
	if ( (NULL == szBuf) || ('\0' == szBuf[0]) || (0 == nBufSize) || (m_nLogDataSize + nBufSize) > nLogBufMaxSize )
	{
		return NDFalse;
	}

	memcpy( m_szLogBuf + m_nLogDataSize, szBuf, nBufSize );
	m_nLogDataSize += nBufSize;

	m_NDSMUHead.m_nSaveTime = NDShareBaseGlobal::getCurSecondTimeOfUTC();

	return NDTrue;
}

//////////////////////////////////////////////////////////////////////////////////////

NDShareMemoryLogFile::NDShareMemoryLogFile():
m_nTotalSize( 0 ),
m_nCacheSize( 0 ),
m_pNDShareLogCacheSMU( NULL ),
m_pLogMutex( new NDMutexLock )
{	
	
}

NDShareMemoryLogFile::~NDShareMemoryLogFile()
{
	release();

	if ( NULL != m_pLogMutex )
	{
		delete m_pLogMutex;
		m_pLogMutex = NULL;
	}
}

NDBool NDShareMemoryLogFile::init( NDShareLogCacheSMU* pNDShareLogCacheSMU )
{
	if ( NULL == pNDShareLogCacheSMU )
	{
		return NDFalse;
	}

	const char* szLogName = pNDShareLogCacheSMU->getLogName();
	if ( '\0' == szLogName[0] )
	{
		return NDFalse;
	}
	
	NDUint32 nCacheSize = pNDShareLogCacheSMU->getLogBufSize();
	NDUint32 nHashValue = pNDShareLogCacheSMU->getLogNameHash();
	if ( ( 0 == nCacheSize ) || ( 0 == nHashValue ) )
	{
		return NDFalse;
	}

	if ( nHashValue != m_nCacheSize )
	{	//比较前后两次的HASH值,确认是否是同一文件;
		m_nTotalSize = 0;
	}

	m_nCacheSize			= nCacheSize;
	m_pNDShareLogCacheSMU	= pNDShareLogCacheSMU;

	m_pNDShareLogCacheSMU->setUseStatus( eNDSMU_USE_STATE_HOLDDATA );
	m_pNDShareLogCacheSMU->setSaveTime( NDShareBaseGlobal::getCurSecondTimeOfUTC() );

	return NDTrue;
}

void NDShareMemoryLogFile::release()
{
	if ( NULL == m_pNDShareLogCacheSMU )	return;

	m_pNDShareLogCacheSMU->setUseStatus( eNDSMU_USE_STATE_READFREE );

	m_nCacheSize			= m_pNDShareLogCacheSMU->getLogNameHash();	//用于临时存储;
	m_pNDShareLogCacheSMU	= NULL;
}

NDBool NDShareMemoryLogFile::write( const char* pFile, NDInt32 nLine, NDInt32 nLevel, const char* pFormat, ... )
{
	va_list ap;
	va_start( ap, pFormat );
	NDBool bRet = write( pFile, nLine, nLevel, pFormat, ap );
	va_end(ap);

	return bRet;
}

NDBool NDShareMemoryLogFile::write( const char* pFile, NDInt32 nLine, NDInt32 nLevel, const char* pFormat, va_list ap )
{
	if ( NULL == m_pNDShareLogCacheSMU )  return NDFalse;

	NDUint32 nCurCacheDataSize = cacheDataSize();
	if ( ( (NDUint32)ND_INVALID_ID == nCurCacheDataSize ) || ( nCurCacheDataSize >= m_nCacheSize ) )
	{
		return NDFalse;
	}

	const char* szLogTypeName = NDShareBaseGlobal::getLogTypeName( nLevel );
	if ( NULL == szLogTypeName )
	{
		return NDFalse;
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
																							nLine );
	nlen += ND_VSNPRINTF( (szBuf + nlen), (MAX_STRING_LEN - nlen - 1), pFormat, ap );
	nlen += ND_SNPRINTF( (szBuf + nlen), (MAX_STRING_LEN - nlen - 1), "%s", NEWLINE_SYMBOL );


	if ( (nlen + nCurCacheDataSize) > m_nCacheSize )
	{	//写入的数据大于缓存大小;(这是不允许的);
		return NDFalse;
	}
	//add locker;
	NDGuardLock logLocker( *m_pLogMutex );
	m_pNDShareLogCacheSMU->writeLog( szBuf, nlen );
	m_nTotalSize += nlen;
	return NDTrue;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////

NDShareMemoryLogManager* g_pSMLogManager = NULL;

NDShareMemoryLogManager::NDShareMemoryLogManager():
m_pszLogPath( NULL ),
m_pszLogBaseName( NULL ),
m_nLogMaxSize( 0 ),
m_pStandbyShareLogCacheSMU( NULL ),
m_pNDShareMemoryLogFile( new NDShareMemoryLogFile ),
m_pNDShareLogCacheSMUPool( new NDShareMemoryUnitPool<NDShareLogCacheSMU>() )
{
}

NDShareMemoryLogManager::~NDShareMemoryLogManager()
{
	if ( NULL != m_pszLogPath )
	{
		delete[] m_pszLogPath;
		m_pszLogPath = NULL;
	}
	if ( NULL != m_pszLogBaseName )
	{
		delete[] m_pszLogBaseName;
		m_pszLogBaseName = NULL;
	}
	if ( NULL != m_pStandbyShareLogCacheSMU )
	{
		m_pStandbyShareLogCacheSMU->setUseStatus(eNDSMU_USE_STATE_READFREE);
		m_pStandbyShareLogCacheSMU = NULL;
	}
	if ( NULL != m_pNDShareMemoryLogFile )
	{
		delete m_pNDShareMemoryLogFile;
		m_pNDShareMemoryLogFile = NULL;
	}
	if ( NULL != m_pNDShareLogCacheSMUPool )
	{
		delete m_pNDShareLogCacheSMUPool;
		m_pNDShareLogCacheSMUPool = NULL;
	}
}

NDBool NDShareMemoryLogManager::init(  NDSM_KEY nKey, NDUint32 nUnitMax, NDUint8 nPoolType, const char* szLogPath, const char* szLogBaseName, NDUint32 nLogMaxSize )
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

	if ( NULL == m_pNDShareLogCacheSMUPool )
	{
		return NDFalse;
	}
	if ( NDFalse == m_pNDShareLogCacheSMUPool->init( nKey, nUnitMax, nPoolType ) )
	{
		return NDFalse;
	}

	getStandbyShareLogCacheSMU();

	return initFile( m_pNDShareLogCacheSMUPool->createObj() );
}

NDBool NDShareMemoryLogManager::initFile( NDShareLogCacheSMU* pNDShareLogCacheSMU )
{
	if ( NULL == pNDShareLogCacheSMU || NULL == m_pNDShareMemoryLogFile )
	{
		return NDFalse;
	}

	setLogNameInfo( pNDShareLogCacheSMU );

	return m_pNDShareMemoryLogFile->init( pNDShareLogCacheSMU );
}

NDBool NDShareMemoryLogManager::write( const char* pFile, NDInt32 nLine, NDInt32 nLevel, const char* pFormat, ... )
{
	va_list ap;
	va_start( ap, pFormat );
	NDBool bRet = write( pFile, nLine, nLevel, pFormat, ap );
	va_end(ap);
	return bRet;
}

NDBool NDShareMemoryLogManager::write( const char* pFile, NDInt32 nLine, NDInt32 nLevel, const char* pFormat, va_list ap )
{
	if ( NULL == m_pNDShareMemoryLogFile )	return NDFalse;

	if ( ( m_pNDShareMemoryLogFile->size() >= m_nLogMaxSize ) || ( m_pNDShareMemoryLogFile->cacheSpaceSize() < BUF_LEN_512 ) )
	{
		m_pNDShareMemoryLogFile->release();

		if ( NULL == m_pStandbyShareLogCacheSMU )
		{
			if ( NDFalse == initFile( m_pNDShareLogCacheSMUPool->createObj() ) )
			{
				return NDFalse;
			}
		}
		else
		{
			if ( NDFalse == initFile( m_pStandbyShareLogCacheSMU ) )
			{
				return NDFalse;
			}
			m_pStandbyShareLogCacheSMU = NULL;
		}
	}
	if ( NULL == m_pStandbyShareLogCacheSMU )
	{
		getStandbyShareLogCacheSMU();
	}
	else
	{	//设置备用的共享内存也在使用;
		m_pStandbyShareLogCacheSMU->setSaveTime( NDShareBaseGlobal::getCurSecondTimeOfUTC() );
	}

	return m_pNDShareMemoryLogFile->write( pFile, nLine, nLevel, pFormat, ap );
}

NDBool NDShareMemoryLogManager::setLogNameInfo( NDShareLogCacheSMU* pNDShareLogCacheSMU )
{
	if ( NULL == pNDShareLogCacheSMU )
	{
		return NDFalse;
	}

	char szTimeBuf[ND_TIME_LENGTH] = { 0 };
	NDShareBaseGlobal::getLocalSecondTimeStr(  szTimeBuf, ND_TIME_LENGTH );
	szTimeBuf[10] = '-';
	szTimeBuf[13] = '-';
	szTimeBuf[16] = '-';	//fopen路径规则中不能有:这个符号,否则创建不了文件;

	//最终文件名;
	char szFileName[MAX_PATH_LEN] = { 0 };

	NDUint32 nLogPathLen = (NDUint32)strlen( m_pszLogPath );
	if ( '/' != m_pszLogPath[ nLogPathLen - 1 ] )
	{
		ND_SNPRINTF( szFileName, MAX_PATH_LEN - 1, "%s/%s-%s.log", m_pszLogPath, m_pszLogBaseName, szTimeBuf );
	}
	else
	{
		ND_SNPRINTF( szFileName, MAX_PATH_LEN - 1, "%s%s-%s.log", m_pszLogPath, m_pszLogBaseName, szTimeBuf );
	}

	pNDShareLogCacheSMU->setLogName( szFileName );
	pNDShareLogCacheSMU->setLogNameHash( NDShareBaseGlobal::bkdr_hash( szFileName ) );

	return NDTrue;
}


NDBool NDShareMemoryLogManager::getStandbyShareLogCacheSMU()
{
	if ( NULL == m_pNDShareLogCacheSMUPool )
	{
		return NDFalse;
	}
	if ( NULL != m_pStandbyShareLogCacheSMU )
	{
		return NDTrue;
	}

	if ( NULL == ( m_pStandbyShareLogCacheSMU = m_pNDShareLogCacheSMUPool->createObj() ) )
	{
		return NDFalse;
	}

	//设置备用共享内存为被占用状态;
	m_pStandbyShareLogCacheSMU->setUseStatus( eNDSMU_USE_STATE_HOLDDATA );
	m_pStandbyShareLogCacheSMU->setSaveTime( NDShareBaseGlobal::getCurSecondTimeOfUTC() );

	setLogNameInfo( m_pStandbyShareLogCacheSMU );

	return NDTrue;
}

_NDSHAREBASE_END


