#include "NDOStreamImpl.h"

#include "memory/NDByteBuffer.h"


_NDSHAREBASE_BEGIN

NDOStreamImpl::NDOStreamImpl( NDByteBuffer& rByteBuf ) : m_rByteBuf( rByteBuf )
{
}

NDOStreamImpl::~NDOStreamImpl(void)
{
}

NDBool NDOStreamImpl::Write( const void* pChar, NDUint32 nSize )
{
	if ( (NULL == pChar) || (nSize <= 0) )
	{
		return 0;
	}

	return ( nSize == m_rByteBuf.WriteBuffer( (const char*)pChar, nSize ) );
}

NDUint32 NDOStreamImpl::GetTotalSize() const
{
	return m_rByteBuf.GetDataSize();
}

_NDSHAREBASE_END
