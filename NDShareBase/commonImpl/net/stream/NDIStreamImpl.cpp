#include "NDIStreamImpl.h"

#include "memory/NDByteBuffer.h"


_NDSHAREBASE_BEGIN

NDIStreamImpl::NDIStreamImpl( NDByteBuffer& rByteBuf ) : m_rByteBuf( rByteBuf )
{
}

NDIStreamImpl::~NDIStreamImpl(void)
{
}

NDBool NDIStreamImpl::read( void* pChar, NDUint32 nSize )
{
	if ( (NULL == pChar) || (nSize <= 0) )
	{
		return NDFalse;
	}

	return m_rByteBuf.readIntactBuffer( (char*)pChar, nSize );
}

NDUint32 NDIStreamImpl::getTotalSize() const
{
	return m_rByteBuf.getDataSize();
}

_NDSHAREBASE_END
