#include "file/NDIniFile.h"


#include <stdlib.h>
#include <string.h>

_NDSHAREBASE_BEGIN

NDIniFile::NDIniFile() : m_szFileName( NULL ), m_szModifyData( NULL ), m_nFileSize( 0 )
{
	m_SectionValueMap.clear();
}

NDIniFile::~NDIniFile()
{
	close();
}

NDBool NDIniFile::open( const char* szFileName )
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

	modifyData();

	NDUint32 nLen	= (NDUint32)strlen( szFileName );
	m_szFileName	= (char*)malloc( nLen + 1 );
	memcpy( m_szFileName, szFileName, nLen );
	m_szFileName[nLen] = '\0';

	return NDTrue;
}


NDBool NDIniFile::reOpen()
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

	
	m_SectionValueMap.clear();
	modifyData();

	return NDTrue;
}

void NDIniFile::close()
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

	m_SectionValueMap.clear();
}

NDBool NDIniFile::modifyData()
{
	for ( NDUint32 i = 0; i < m_nFileSize; ++i )
	{	//找到段;
		if( m_szModifyData[i] == '[' && ( i == 0 || m_szModifyData[i-1] == '\n' ) )
		{
			NDUint32 nNewIndex	= 0;
			NDBool bSectionEnd	= NDFalse;
			NDBool bKeyStart	= NDFalse;
			NDBool bValueStart	= NDFalse;
			char* szKey			= NULL;
			char* szValue		= NULL;

			PropertyValueMap propertyValueMap;

			for ( NDUint32 j = i + 1; j < m_nFileSize; ++j )
			{
				nNewIndex = j;
				if ( m_szModifyData[j] == '[' &&  m_szModifyData[j-1] == '\n' )
				{	//一个新的段;
					break;
				}
				if ( !bSectionEnd )
				{
					if ( m_szModifyData[j] == ']' )
					{
						m_szModifyData[j] = '\0';
						bSectionEnd = NDTrue;
					}
					continue;
				}
				if ( NDFalse == bKeyStart &&  NDFalse == bValueStart )
				{
					if ( j == m_nFileSize - 1 )
					{
						break;
					}
					if ( m_szModifyData[j] == '\n' && m_szModifyData[j+1] != ';' )
					{
						bKeyStart = NDTrue;
						continue;
					}
				}
				else
				{
					if ( bKeyStart )
					{
						if ( NULL == szKey )
						{
							if ( m_szModifyData[j] != ' ' && m_szModifyData[j] != '\t' )
							{
								szKey = &m_szModifyData[j];
							}
						}
						else
						{
							if ( m_szModifyData[j] == ' ' || m_szModifyData[j] == '\t' )
							{
								m_szModifyData[j] = '\0';
							}
							else if ( m_szModifyData[j] == '=' )
							{
								m_szModifyData[j] = '\0';
								bKeyStart	= NDFalse;
								bValueStart = NDTrue;
							}
						}
						continue;
					}
					if ( bValueStart )
					{
						if ( NULL == szValue )
						{
							if ( m_szModifyData[j] != ' ' && m_szModifyData[j] != '\t' )
							{
								szValue = &m_szModifyData[j];
							}
						}
						else 
						{
							if ( m_szModifyData[j] == ' ' || m_szModifyData[j] == '\t' || m_szModifyData[j] == '\r' )
							{
								m_szModifyData[j] = '\0';
							}
							else if ( m_szModifyData[j] == '\n' )
							{
								--j;							//让'\n'重新被发现一次;
								//m_szModifyData[j] = '\0';		//最后重新来一遍吧;
								propertyValueMap.insert( std::make_pair( szKey, szValue ) );
								szKey		= NULL;
								szValue		= NULL;
								bValueStart = NDFalse;
							}
						}
					}
				}
			}
			m_SectionValueMap.insert( std::make_pair( &m_szModifyData[i + 1], propertyValueMap ) );
			i = nNewIndex - 1;
		}
	}

	for ( NDUint32 j = 0; j < m_nFileSize; ++j )
	{
		if ( m_szModifyData[j] == '\n' )
		{
			m_szModifyData[j] = '\0';
		}
	}

	return NDTrue;
}

const char* NDIniFile::getValue( const char* szSection, const char* szKey ) const
{
	if ( NULL == szSection || '\0' == szSection[0] || NULL == szKey || '\0' == szKey[0] )
	{
		return NULL;
	}

	SectionValueMap::const_iterator sectionIterFind = m_SectionValueMap.find( (char*)szSection );
	if ( sectionIterFind == m_SectionValueMap.end() )
	{
		return NULL;
	}

	const PropertyValueMap &refPropertyValueMap			= sectionIterFind->second;
	PropertyValueMap::const_iterator propertyIterFind	= refPropertyValueMap.find ( (char*)szKey );
	if ( propertyIterFind == refPropertyValueMap.end() )
	{
		return NULL;
	}

	return propertyIterFind->second;
}

NDBool NDIniFile::getInt32( const char* szSection, const char* szKey, NDInt32 &refValue ) const
{
	const char* szResult = getValue( szSection, szKey );
	if ( NULL == szResult )
	{
		return NDFalse;
	}

	refValue = atoi( szResult );

	return NDTrue;
}

NDBool NDIniFile::getUint32( const char* szSection, const char* szKey, NDUint32 &refValue ) const
{
	const char* szResult = getValue( szSection, szKey );
	if ( NULL == szResult )
	{
		return NDFalse;
	}

	refValue = atol( szResult );

	return NDTrue;
}

NDBool NDIniFile::getFloat32( const char* szSection, const char* szKey, NDFloat32 &refValue ) const
{
	const char* szResult = getValue( szSection, szKey );
	if ( NULL == szResult )
	{
		return NDFalse;
	}

	refValue = (NDFloat32)atof( szResult );

	return NDTrue;
}

NDBool NDIniFile::getFloat64( const char* szSection, const char* szKey, NDFloat64 &refValue ) const
{
	const char* szResult = getValue( szSection, szKey );
	if ( NULL == szResult )
	{
		return NDFalse;
	}

	refValue = atof( szResult );

	return NDTrue;
}

_NDSHAREBASE_END
