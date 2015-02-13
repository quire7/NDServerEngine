#include "file/NDExcelFile.h"

#include <stdlib.h>
#include <string.h>

#include "NDCLibFun.h"
#include "NDShareBaseGlobal.h"

_NDSHAREBASE_BEGIN

NDExcelFile::NDExcelFile() : m_szFileName( NULL ), m_szModifyData( NULL ), m_nFileSize( 0 ), m_nRowNum( 0 ), m_nColumnNum( 0 )
{
	m_columnType.clear();
	m_vDataBuf.clear();
}

NDExcelFile::~NDExcelFile()
{
	close();
}

NDBool NDExcelFile::open( const char* szFileName )
{
	if ( NULL == szFileName || '\0' == szFileName[0] )
	{
		return NDFalse;
	}

	close();

	FILE* pFile = ::fopen( szFileName, "rb" );
	if ( NULL == pFile )
	{
		return NDFalse;
	}

	//获取文件大小;
	::fseek( pFile, 0, SEEK_END );
	m_nFileSize = ::ftell( pFile );
	::fseek( pFile, 0, SEEK_SET );

	m_szModifyData = (char*)malloc( m_nFileSize + 1 );
	memset( m_szModifyData, 0, m_nFileSize + 1 );

	::fread( m_szModifyData, m_nFileSize, 1, pFile );		//读数据;
	fclose( pFile );

	if ( NDFalse == modifyData() )
	{
		free( m_szModifyData );
		m_szModifyData = NULL;
		return NDFalse;
	}

	NDUint32 nLen	= (NDUint32)strlen( szFileName );
	m_szFileName	= (char*)malloc( nLen + 1 );
	memcpy( m_szFileName, szFileName, nLen );
	m_szFileName[nLen] = '\0';

	return NDTrue;
}


NDBool NDExcelFile::reOpen()
{
	if ( NULL == m_szFileName || NULL == m_szModifyData || m_nFileSize == 0 )
	{
		return NDFalse;
	}

	FILE* pFile = ::fopen( m_szFileName, "rb" );
	if ( NULL == pFile )
	{
		return NDFalse;
	}

	//获取文件大小;
	::fseek( pFile, 0, SEEK_END );
	NDUint32 nNewFileSize = ::ftell( pFile );
	::fseek( pFile, 0, SEEK_SET );

	if ( nNewFileSize > m_nFileSize )
	{
		free( m_szModifyData );
		m_nFileSize		= nNewFileSize;
		m_szModifyData	= (char*)malloc( m_nFileSize + 1 );
	}
	//重置清零;
	memset( m_szModifyData, 0, m_nFileSize + 1 );

	::fread( m_szModifyData, m_nFileSize, 1, pFile );		//读数据;
	fclose( pFile );

	
	m_nRowNum	= 0;
	m_nColumnNum= 0;

	m_columnType.clear();
	m_vDataBuf.clear();

	if ( NDFalse == modifyData() )
	{
		close();
		return NDFalse;
	}

	return NDTrue;
}

void NDExcelFile::close()
{
	if ( NULL != m_szFileName )
	{
		free( m_szFileName );
		m_szFileName = NULL;
	}
	if ( NULL != m_szModifyData )
	{
		free( m_szModifyData );
		m_szModifyData = NULL;
	}

	m_nFileSize = 0;

	m_nRowNum	= 0;
	m_nColumnNum= 0;

	m_columnType.clear();
	m_vDataBuf.clear();
}

NDBool NDExcelFile::modifyData()
{
	static const int LINEMAXLEN = 10240;	//1024*10;
	char szLineBuf[LINEMAXLEN]	= {0};
	const char* szModifyDataEnd = m_szModifyData + m_nFileSize + 1;

	//读取第一行表头;
	char* szBuf = getLineFromMemory( szLineBuf, LINEMAXLEN, m_szModifyData, szModifyDataEnd );
	
	//分析列数和类型;
	vector<string> fieldTypeStrVec;
	NDShareBaseGlobal::strsplit( string(szLineBuf), "\t", fieldTypeStrVec );
	if ( fieldTypeStrVec.empty() )
	{
		return NDFalse;
	}
	//生成TYPE;
	m_nColumnNum = (NDUint32)fieldTypeStrVec.size();
	m_columnType.reserve( m_nColumnNum );

	for ( NDUint32 i = 0; i < m_nColumnNum; ++i )
	{
		string &refStr = fieldTypeStrVec[i];
		if ( 0 == ND_STRNICMP( refStr.c_str(), "INT", sizeof("INT") )  )
		{
			m_columnType.push_back( T_INT );
		}
		else if ( 0 == ND_STRNICMP( refStr.c_str(), "FLOAT", sizeof("FLOAT") ) )
		{
			m_columnType.push_back( T_FLOAT );
		}
		else
		{	//STRING列名后面必须标明大小;
			if ( refStr.length() > 6 )
			{
				if ( 0 == ND_STRNICMP( refStr.c_str(), "STRING", sizeof("STRING") ) )
				{
					m_columnType.push_back( T_STRING );
				}
				else
				{
					return NDFalse;
				}
			}
			else
			{
				return NDFalse;
			}
		}
	}
	if ( m_nColumnNum != m_columnType.size() )
	{
		return NDFalse;
	}

	//读取第二行(英文列名);
	szBuf = getLineFromMemory( szLineBuf, LINEMAXLEN, szBuf, szModifyDataEnd );
	//读取第三行(注释列名);
	szBuf = getLineFromMemory( szLineBuf, LINEMAXLEN, szBuf, szModifyDataEnd );

	NDUint32 nParseDataStart = (NDUint32)(szBuf - m_szModifyData);
	if ( nParseDataStart == 0 || nParseDataStart >= m_nFileSize )
	{
		return NDFalse;
	}

	m_nRowNum			= 0;		//清空重新设置;
	NDUint32 nColumnNow	= 0;		//开始解析第几列;
	NDBool bIgnoreLine	= NDFalse;	//是否忽略这一行;
	NDBool bValueStart	= NDTrue;	//是否可以设置新值开始位置;
	char* szValueStart	= NULL;		//列值的开始位置;
	for ( NDUint32 i = nParseDataStart; i < m_nFileSize; ++i )
	{
		if ( bIgnoreLine )
		{	//忽略整行;
			if ( '\n' == m_szModifyData[i] )
			{
				bIgnoreLine = NDFalse;
			}
			continue;
		}

		if ( '\n' == m_szModifyData[i] )
		{	//换行重置;
			++m_nRowNum;

			nColumnNow	= 0;
			bIgnoreLine	= NDFalse;
			bValueStart	= NDTrue;
			szValueStart= NULL;
			continue;
		}

		if ( bValueStart )
		{
			if ( '\t' == m_szModifyData[i] )
			{
				if ( 0 == nColumnNow )
				{	//第一列不能为空,为空就忽略整行;
					bIgnoreLine = NDTrue;
					continue;
				}
				else
				{
					m_vDataBuf.push_back( FIELD(0) );
					++nColumnNow;
					continue;
				}
			}
			szValueStart	= &m_szModifyData[i];
			bValueStart		= NDFalse;
		}
		else
		{
			if ( '\t' == m_szModifyData[i] || '\r' == m_szModifyData[i] )
			{
				m_szModifyData[i] = '\0';	//设置结束符;

				FIELD_TYPE& refFIELDTYPE = m_columnType[nColumnNow];
				if ( refFIELDTYPE == T_INT )
				{
					m_vDataBuf.push_back( FIELD( atoi( szValueStart ) ) );
				}
				else if ( refFIELDTYPE == T_FLOAT )
				{
					m_vDataBuf.push_back( FIELD( (NDFloat32)atof( szValueStart ) ) );
				}
				else
				{
					m_vDataBuf.push_back( FIELD( szValueStart ) );
				}

				++nColumnNow;
				bValueStart = NDTrue;
			}
		}
	}
	return NDTrue;
}

char* NDExcelFile::getLineFromMemory( char* szOutBuf, NDUint32 nOutBufSize, const char* pMemory, const char* pMemoryEnd )
{
	if ( NULL == szOutBuf || NULL == pMemory || NULL == pMemoryEnd || pMemory >= pMemoryEnd || pMemory[0] == '\0' )
	{
		return NULL;
	}

	//寻找换行符;
	char* pMemBuf = (char*)pMemory;
	while ( (pMemBuf < pMemoryEnd) && ((NDUint32)(pMemBuf - pMemory + 1) < nOutBufSize) 
		&& (*pMemBuf != 0) && (*pMemBuf != '\r') && (*pMemBuf != '\n')  )
	{
		*(szOutBuf++) = *(pMemBuf++);
	}

	*szOutBuf = '\0';

	//剔除这行的换行符;
	while ( (pMemBuf < pMemoryEnd) && (*pMemBuf != 0) && ( (*pMemBuf == '\r') || (*pMemBuf == '\n') )  ) pMemBuf++;

	return pMemBuf;
}


const NDExcelFile::FIELD* NDExcelFile::getFIELD( NDUint32 nRowNum, NDUint32 nColumn ) const
{
	if ( nRowNum >= m_nRowNum || nColumn >= m_nColumnNum )
	{
		return NULL;
	}

	NDUint32 nPosition = nRowNum * m_nColumnNum + nColumn;
	if ( nPosition >= m_vDataBuf.size() )
	{
		return NULL;
	}
	return &m_vDataBuf[nPosition];
}

const char* NDExcelFile::getValue( NDUint32 nRowNum, NDUint32 nColumn ) const 
{
	const FIELD* pFIELD = getFIELD( nRowNum, nColumn );
	if ( NULL == pFIELD )
	{
		return NULL;
	}
	return pFIELD->pString;
}

NDBool NDExcelFile::getInt32( NDUint32 nRowNum, NDUint32 nColumn, NDInt32 &refValue ) const
{
	const FIELD* pFIELD = getFIELD( nRowNum, nColumn );
	if ( NULL == pFIELD )
	{
		return NDFalse;
	}

	refValue = pFIELD->iValue;
	return NDTrue;
}

NDBool NDExcelFile::getFloat32( NDUint32 nRowNum, NDUint32 nColumn, NDFloat32 &refValue ) const
{
	const FIELD* pFIELD = getFIELD( nRowNum, nColumn );
	if ( NULL == pFIELD )
	{
		return NDFalse;
	}

	refValue = pFIELD->fValue;
	return NDTrue;
}

_NDSHAREBASE_END
