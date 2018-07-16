#include "sharememory/NDShareMemoryAccessObject.h"

#include "NDShareBaseGlobal.h"
#include "sharememory/NDShareMemory.h"

_NDSHAREBASE_BEGIN

NDShareMemoryAccessObject::NDShareMemoryAccessObject() : m_bCreate( NDFalse ),
m_pNDShareMemory( new NDShareMemory ), 
m_nSize( 0 ),
m_pHeader( NULL ),
m_pDataPtr( NULL )
{
}

NDShareMemoryAccessObject::~NDShareMemoryAccessObject()
{
	if ( m_bCreate )
	{
		destroy();
	}
	else
	{
		detach();
	}

	if ( NULL != m_pNDShareMemory )
	{
		delete m_pNDShareMemory;
		m_pNDShareMemory = NULL;
	}
}

NDBool NDShareMemoryAccessObject::create( NDSM_KEY nKey, NDUint32 nSize )
{
	if ( NULL == m_pNDShareMemory )
	{
		return NDFalse;
	}

	NDUint32 nNDSMHeadSize = sizeof(NDSMHead);
	if ( nSize <= nNDSMHeadSize )
	{	//生成SHAREMEMORY无意义;
		return NDFalse;
	}

	if ( NDFalse == m_pNDShareMemory->createShareMemory( nKey, nSize ) )
	{
		//Log::SaveLog( m_LogPath, "Create ShareMem Error NDSM_KEY = %u", nKey );
		return NDFalse;
	}

	m_pHeader = m_pNDShareMemory->mapShareMemory();
	
	if ( NULL == m_pHeader )
	{
		//Log::SaveLog( m_LogPath, "Map ShareMem Error NDSM_KEY = %u", nKey );
		return NDFalse;
	}

	m_pDataPtr	= m_pHeader + nNDSMHeadSize;
	m_nSize		= nSize;

	((NDSMHead*)m_pHeader)->m_nKey		=	nKey;
	((NDSMHead*)m_pHeader)->m_nSize		=	nSize;
	((NDSMHead*)m_pHeader)->m_nOwnType	=	0;		// 0==eNDSMU_OWN_TYPE_FREE;
	//((NDSMHead*)m_pHeader)->m_nHeadVer	=	0;
	//Log::SaveLog( m_LogPath,"Create ShareMem Ok NDSM_KEY = %u", nKey );

	m_bCreate = NDTrue;

	return NDTrue;
}

void NDShareMemoryAccessObject::destroy()
{
	if ( NULL == m_pNDShareMemory ) return;

	if( NULL != m_pHeader )
	{
		m_pNDShareMemory->unMapShareMem( m_pHeader );
		m_pHeader = NULL;
	}

	m_pNDShareMemory->destoryShareMemory();

	m_nSize		= 0;
	m_pDataPtr	= NULL;
}

NDBool NDShareMemoryAccessObject::attach( NDSM_KEY nKey, NDUint32 nSize )
{
	if ( NULL == m_pNDShareMemory )
	{
		return NDFalse;
	}

	NDUint32 nNDSMHeadSize = sizeof(NDSMHead);
	if ( nSize <= nNDSMHeadSize )
	{	//生成SHAREMEMORY无意义;
		return NDFalse;
	}

	if ( NDFalse == m_pNDShareMemory->openShareMemory( nKey, nSize ) )
	{
		//Log::SaveLog( m_LogPath, "Attach ShareMem Error NDSM_KEY = %u", nKey );
		return NDFalse;
	}

	m_pHeader = m_pNDShareMemory->mapShareMemory();

	if ( NULL == m_pHeader )
	{
		//Log::SaveLog( m_LogPath, " Attach Map ShareMem Error NDSM_KEY = %u", nKey );
		return NDFalse;
	}

	if ( ( ((NDSMHead*)m_pHeader)->m_nKey != nKey ) || ( ((NDSMHead*)m_pHeader)->m_nSize !=	nSize ) )
	{
		//打印日志;
		return NDFalse;
	}

	m_pDataPtr	= m_pHeader + nNDSMHeadSize;
	m_nSize		= nSize;

	//Log::SaveLog( m_LogPath,"Attach ShareMem Ok NDSM_KEY = %u", nKey );
	return NDTrue;
}

void NDShareMemoryAccessObject::detach()
{
	if ( NULL == m_pNDShareMemory ) return;

	if( NULL != m_pHeader )
	{
		m_pNDShareMemory->unMapShareMem( m_pHeader );
		m_pHeader = NULL;
	}

	m_pNDShareMemory->closeShareMemory();

	m_nSize		= 0;
	m_pDataPtr	= NULL;
}

char* NDShareMemoryAccessObject::getTypePtr( NDUint32 nSize, NDUint32 nIndex )
{
	if ( NULL == m_pNDShareMemory || NULL == m_pHeader || NULL == m_pDataPtr )
	{
		return NULL;
	}
	NDUint32 nPosition = nSize * nIndex;
	if ( nSize == 0 || nPosition >= m_nSize )
	{
		return NULL;
	}

	return ( m_pDataPtr + nPosition );
}


NDSM_KEY NDShareMemoryAccessObject::getKey() const
{
	if ( NULL == m_pNDShareMemory || NULL == m_pHeader )
	{
		return 0;
	}

	return (((NDSMHead*)m_pHeader)->m_nKey);
}

void NDShareMemoryAccessObject::setHeadVersion( NDUint32 nVersion )
{
	if ( NULL == m_pNDShareMemory || NULL == m_pHeader ) return;

	((NDSMHead*)m_pHeader)->m_nHeadVer = nVersion;
}

NDUint32 NDShareMemoryAccessObject::getHeadVersion() const
{
	if ( NULL == m_pNDShareMemory || NULL == m_pHeader )
	{
		return 0;
	}

	return (((NDSMHead*)m_pHeader)->m_nHeadVer);
}


void NDShareMemoryAccessObject::setUnitSize( NDUint32 nCurSize )
{
	if ( NULL == m_pNDShareMemory || NULL == m_pHeader )
	{
		return;
	}

	((NDSMHead*)m_pHeader)->m_nUnitPosition = nCurSize;
}

NDUint32 NDShareMemoryAccessObject::getUnitSize() const
{
	if ( NULL == m_pNDShareMemory || NULL == m_pHeader )
	{
		return 0;
	}

	return ((NDSMHead*)m_pHeader)->m_nUnitPosition;
}

void NDShareMemoryAccessObject::setUnitCapacity( NDUint32 nCapacity )
{
	if ( NULL == m_pNDShareMemory || NULL == m_pHeader )
	{
		return ;
	}

	((NDSMHead*)m_pHeader)->m_nUnitCapacity = nCapacity;
}

NDUint32 NDShareMemoryAccessObject::getUnitCapacity() const
{
	if ( NULL == m_pNDShareMemory || NULL == m_pHeader )
	{
		return 0;
	}

	return ((NDSMHead*)m_pHeader)->m_nUnitCapacity;
}


NDBool NDShareMemoryAccessObject::tryLockHead( NDUint16 nLockType )
{
	if ( NULL == m_pNDShareMemory || NULL == m_pHeader )
	{
		return NDFalse;
	}

	return NDShareBaseGlobal::sm_try_lock( &((NDSMHead*)m_pHeader)->m_nOwnType, nLockType  );
}

NDBool NDShareMemoryAccessObject::tryUnLockHead( NDUint16 nUnlockType )
{
	if ( NULL == m_pNDShareMemory || NULL == m_pHeader )
	{
		return NDFalse;
	}

	return NDShareBaseGlobal::sm_try_unlock( &((NDSMHead*)m_pHeader)->m_nOwnType, nUnlockType  );
}

NDBool NDShareMemoryAccessObject::dumpToFile( const char* szFilePath )
{
	if ( NULL == szFilePath || '\0' == szFilePath[0] || 0 == m_nSize )
	{
		return NDFalse;
	}

	FILE* pFile	= fopen( szFilePath, "wb" );
	if( NULL == pFile )
	{
		return NDFalse;
	}
	fwrite( m_pHeader, 1, m_nSize, pFile );
	fclose( pFile );

	return NDTrue;
}

NDBool NDShareMemoryAccessObject::mergeFromFile( const char* szFilePath )
{
	if ( NULL == szFilePath || '\0' == szFilePath[0] )
	{
		return NDFalse;
	}

	FILE* pFile = fopen( szFilePath, "rb" );
	if( NULL == pFile )
	{
		return NDFalse;
	}
	fseek( pFile, 0L, SEEK_END );
	NDUint32 nFileLength = ftell( pFile );
	fseek( pFile, 0L, SEEK_SET );
	fread( m_pHeader, nFileLength, 1, pFile );
	fclose( pFile );

	return NDTrue;
}


_NDSHAREBASE_END

