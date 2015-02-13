#include "NDIStreamImpl.h"

#include "memory/NDByteBuffer.h"


_NDSHAREBASE_BEGIN

NDIStreamImpl::NDIStreamImpl( NDByteBuffer& rByteBuf ) : m_rByteBuf( rByteBuf )
{
}

NDIStreamImpl::~NDIStreamImpl(void)
{
}

NDBool NDIStreamImpl::Read( void* pChar, NDUint32 nSize )
{
	if ( (NULL == pChar) || (nSize <= 0) )
	{
		return NDFalse;
	}

	return m_rByteBuf.ReadIntactBuffer( (char*)pChar, nSize );
}

NDUint32 NDIStreamImpl::GetTotalSize() const
{
	return m_rByteBuf.GetDataSize();
}

_NDSHAREBASE_END
