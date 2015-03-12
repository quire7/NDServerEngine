#include "NDOStreamImpl.h"

#include "memory/NDByteBuffer.h"


_NDSHAREBASE_BEGIN

NDOStreamImpl::NDOStreamImpl( NDByteBuffer& rByteBuf ) : m_rByteBuf( rByteBuf )
{
}

NDOStreamImpl::~NDOStreamImpl(void)
{
}

NDBool NDOStreamImpl::write( const void* pChar, NDUint32 nSize )
{
	if ( (NULL == pChar) || (nSize <= 0) )
	{
		return 0;
	}

	return ( nSize == m_rByteBuf.writeBuffer( (const char*)pChar, nSize ) );
}

NDUint32 NDOStreamImpl::getTotalSize() const
{
	return m_rByteBuf.getDataSize();
}

_NDSHAREBASE_END
