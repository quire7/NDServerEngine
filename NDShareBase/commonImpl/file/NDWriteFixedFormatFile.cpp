#include "file/NDWriteFixedFormatFile.h"

#include <stdarg.h>

_NDSHAREBASE_BEGIN

NDWriteFixedFormatFile::NDWriteFixedFormatFile(void) :m_pFile(NULL), m_nSize(0)
{

}

NDShareBase::NDWriteFixedFormatFile::~NDWriteFixedFormatFile(void)
{
	if ( NULL != m_pFile )
	{
		close();
	}
}

NDBool NDShareBase::NDWriteFixedFormatFile::isopen() const
{
	return ( NULL != m_pFile );
}

NDBool NDShareBase::NDWriteFixedFormatFile::open(const char* pFileName, const char* pOpenFlag/*="a+" */)
{
	if (NULL != m_pFile || NULL == pFileName || (NULL == pOpenFlag)) return NDFalse;

	return ( NULL != ( m_pFile = fopen(pFileName, pOpenFlag) ) );
}

NDBool NDShareBase::NDWriteFixedFormatFile::writeHeader(const char* szWriteBuf, NDUint32 nSize)
{
	if ( !isopen() )
	{
		return NDFalse;
	}

	::fseek( m_pFile, 0, SEEK_END );
	long lLen = ::ftell( m_pFile );
	::fseek( m_pFile, 0, SEEK_SET );

	if ( 0 != lLen )
	{
		return NDFalse;
	}

	m_nSize = nSize;

	return ( 1 == fwrite( szWriteBuf, nSize, 1, m_pFile ) );
}

NDBool NDShareBase::NDWriteFixedFormatFile::writeContent( const char* szFormat, ... )
{
	if (!isopen())
	{
		return NDFalse;
	}

	int nSize = 0;
	char szBuf[1024] = { 0 };

	va_list ap;
	va_start(ap, szFormat);
	nSize = vsnprintf( szBuf, sizeof(szBuf)-1, szFormat, ap );
	va_end(ap);

	if ( 1 == fwrite(szBuf, nSize, 1, m_pFile) )
	{
		m_nSize += nSize;
		return NDTrue;
	}
	return NDFalse;
}

NDUint32 NDShareBase::NDWriteFixedFormatFile::size()
{
	return m_nSize;
}

void NDShareBase::NDWriteFixedFormatFile::close()
{
	if ( NULL == m_pFile ) return;

	::fflush(m_pFile);
	::fclose(m_pFile);

	m_pFile = NULL;
}

NDBool NDShareBase::NDWriteFixedFormatFile::flush()
{
	if (NULL == m_pFile) return NDFalse;
	return (0 == ::fflush(m_pFile));
}


_NDSHAREBASE_END
