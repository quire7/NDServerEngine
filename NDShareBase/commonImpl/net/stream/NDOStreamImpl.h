#ifndef __NETIO_ND_OSTREAM_IMPL_H__
#define __NETIO_ND_OSTREAM_IMPL_H__

#include "net/stream/NDOStream.h"

_NDSHAREBASE_BEGIN

class NDByteBuffer;

class NDOStreamImpl : public NDOStream
{
private:
	NDByteBuffer&	m_rByteBuf;
public:
	NDOStreamImpl(NDByteBuffer& rByteBuf);
	~NDOStreamImpl(void);

	NDBool		Write( const void* pChar, NDUint32 nSize );
	NDUint32	GetTotalSize() const;

private:
	NDOStreamImpl( const NDOStreamImpl& other );
	NDOStreamImpl& operator = ( const NDOStreamImpl& other );
};

_NDSHAREBASE_END
#endif
