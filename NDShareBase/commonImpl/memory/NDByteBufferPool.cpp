#include "memory/NDByteBufferPool.h"

#include "memory/GMemPool.h"




_NDSHAREBASE_BEGIN

NDByteBufferPool* NDByteBufferPool::m_spNDByteBufferPool = NULL;


NDByteBufferPool::NDByteBufferPool()
{
	GInitMemoryDLL();
	m_NDByteBufferObjPool.init( MSGBUF_POOL_SIZE );
}

NDByteBufferPool::~NDByteBufferPool( void )
{
	m_NDByteBufferObjPool.clear();
	GCloseMemoryDLL();
}

NDByteBufferPool* NDByteBufferPool::getInstance()
{
	if ( NULL == m_spNDByteBufferPool )
	{
		m_spNDByteBufferPool = new NDByteBufferPool();
	}

	return m_spNDByteBufferPool;
}

void NDByteBufferPool::releaseInstance()
{
	if ( NULL != m_spNDByteBufferPool )
	{
		delete m_spNDByteBufferPool;
		m_spNDByteBufferPool = NULL;
	}
}

NDByteBuffer* NDByteBufferPool::createByteBuffer()
{
	return m_NDByteBufferObjPool.createObj();
}

NDBool NDByteBufferPool::destroyByteBuffer( NDByteBuffer* pByteBuffer )
{
	if ( NULL == pByteBuffer )	return NDFalse;

	pByteBuffer->Release();
	return m_NDByteBufferObjPool.destroyObj( pByteBuffer );
}




_NDSHAREBASE_END


