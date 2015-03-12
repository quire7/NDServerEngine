#ifndef __NETIO_ND_ISTREAM_IMPLEMENT_H__ 
#define __NETIO_ND_ISTREAM_IMPLEMENT_H__ 


#include "net/stream/NDIStream.h"

_NDSHAREBASE_BEGIN

class NDByteBuffer;

class NDIStreamImpl : public NDIStream
{
private:
	NDByteBuffer&	m_rByteBuf;
public:
	NDIStreamImpl(NDByteBuffer& rByteBuf);
	~NDIStreamImpl(void);

	NDBool		read( void* pChar, NDUint32 nSize );
	NDUint32	getTotalSize() const;

private:
	NDIStreamImpl(const NDIStreamImpl& other);
	NDIStreamImpl& operator = (const NDIStreamImpl& other);
};

_NDSHAREBASE_END
#endif

