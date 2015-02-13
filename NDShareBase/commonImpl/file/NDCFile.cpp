#include "file/NDCFile.h"

#include "NDCFileImpl.h"


_NDSHAREBASE_BEGIN

NDCFile::NDCFile(void) : m_pNDCFileImpl( new NDCFileImpl )
{
}

NDCFile::~NDCFile(void)
{
	if ( NULL != m_pNDCFileImpl ) 
	{
		delete m_pNDCFileImpl;
		m_pNDCFileImpl = NULL;
	}
}

NDBool NDCFile::isopen() const
{
	if ( NULL == m_pNDCFileImpl )	return NDFalse;
	
	return m_pNDCFileImpl->isopen();
}

NDBool NDCFile::open( const char* pFileName, const char* pOpenFlag )
{
	if ( NULL == m_pNDCFileImpl )	return NDFalse;

	return m_pNDCFileImpl->open( pFileName, pOpenFlag );
}

NDUint32 NDCFile::read( void* pReadBuf, NDUint32 nSize )
{
	if ( NULL == m_pNDCFileImpl )	return 0;

	return m_pNDCFileImpl->read( pReadBuf, nSize );
}

NDUint32 NDCFile::write( const void* pWriteBuf, NDUint32 nSize )
{
	if ( NULL == m_pNDCFileImpl )	return 0;

	return m_pNDCFileImpl->write( pWriteBuf, nSize );
}

NDUint32 NDCFile::size()
{
	if ( NULL == m_pNDCFileImpl )	return 0;

	return m_pNDCFileImpl->size();
}

void NDCFile::close()
{
	if ( NULL == m_pNDCFileImpl )	return ;
	
	m_pNDCFileImpl->close();
}

NDInt32	NDCFile::flush()
{
	if ( NULL == m_pNDCFileImpl )	return 1;

	return m_pNDCFileImpl->flush();
}

NDBool NDCFile::readline( void* pReadBuf, NDUint32 nSize )
{
	if ( NULL == m_pNDCFileImpl )	return NDFalse;

	return m_pNDCFileImpl->readline( pReadBuf, nSize );
}


_NDSHAREBASE_END

