#include "NDByteBufferImpl.h"

#include <string.h>
#include "memory/GMemPool.h"


_NDSHAREBASE_BEGIN

NDByteBufferImpl::NDByteBufferImpl( NDUint32 nBufSize )
{
	m_pPool = GNewMemory();
	m_pszBuffer = (char*)GMalloc(m_pPool, (int)nBufSize);
	memset( m_pszBuffer, 0, (int)nBufSize );

	m_nHeadPos		= 0;
	m_nTailPos		= 0;
	m_nDataSize		= 0;
	m_nCapacitySize	= nBufSize;

	m_bRollBack		= NDFalse;
	m_nLastHeadPos	= 0;
	m_nLastTailPos	= 0;
}

NDByteBufferImpl::NDByteBufferImpl( const NDByteBufferImpl& other )
{
	m_pPool = GNewMemory();

	m_pszBuffer = (char*)GMalloc(m_pPool, (int)other.m_nCapacitySize);
	memset( m_pszBuffer, 0, (int)other.m_nCapacitySize );

	if ( other.m_nDataSize > 0 )
	{
		if ( other.m_nHeadPos < other.m_nTailPos )
		{
			memcpy( m_pszBuffer, other.m_pszBuffer + other.m_nHeadPos, other.GetDataSize() );
		}
		else
		{
			NDUint32 nPreRead = other.m_nCapacitySize - other.m_nHeadPos;
			memcpy( m_pszBuffer, other.m_pszBuffer + other.m_nHeadPos, nPreRead );
			if ( other.m_nDataSize > nPreRead )
			{
				memcpy( m_pszBuffer + nPreRead, other.m_pszBuffer, other.m_nTailPos );
			}
		}
	}

	m_nHeadPos		= 0;
	m_nTailPos		= other.m_nDataSize;
	m_nDataSize		= other.m_nDataSize;
	m_nCapacitySize	= other.m_nCapacitySize;

	m_bRollBack		= NDFalse;
	m_nLastHeadPos	= 0;
	m_nLastTailPos	= 0;
}

NDByteBufferImpl::~NDByteBufferImpl(void)
{
	Clear();
}


void NDByteBufferImpl::Clear()
{
	GDestroyMemory(m_pPool);

	m_nHeadPos		= 0;
	m_nTailPos		= 0;
	m_nDataSize		= 0;
	m_nCapacitySize	= 0;

	m_bRollBack		= NDFalse;
	m_nLastHeadPos	= 0;
	m_nLastTailPos	= 0;
}

void NDByteBufferImpl::Release()
{
	memset( m_pszBuffer, 0, m_nCapacitySize * sizeof(char) );
	
	m_nHeadPos		= 0;
	m_nTailPos		= 0;
	m_nDataSize		= 0;

	m_bRollBack		= NDFalse;
	m_nLastHeadPos	= 0;
	m_nLastTailPos	= 0;
}


char* NDByteBufferImpl::ReadBuffer()
{
	if ( 0 == m_pszBuffer )
	{
		return 0;
	}

	if ( m_nHeadPos <= m_nTailPos )
	{	//数据末尾位置大于数据头位置;
		return ( m_pszBuffer + m_nHeadPos );
	}

	NDUint32 nPreRead = m_nCapacitySize - m_nHeadPos;
	if ( m_nHeadPos >= ( nPreRead + m_nTailPos ) )
	{
		memcpy( m_pszBuffer + nPreRead, m_pszBuffer, m_nTailPos );
		memcpy( m_pszBuffer, m_pszBuffer + m_nHeadPos, nPreRead );

		m_nHeadPos = 0;
		m_nTailPos = m_nDataSize;
	}
	else
	{
		char* pNewBuffer = (char*)GMalloc(m_pPool, (int)m_nCapacitySize);
		if ( 0 == pNewBuffer )
		{
			return 0;
		}
		memset( pNewBuffer, 0, (int)m_nCapacitySize );

		NDUint32 nDataSize = m_nDataSize;
		if ( m_nDataSize > 0 )
		{
			if ( !ReadIntactBuffer( pNewBuffer, nDataSize) )
			{
				return 0;
			}
		}

		m_pszBuffer		= pNewBuffer;
		m_nHeadPos		= 0;
		m_nTailPos		= nDataSize;
		m_nDataSize     = nDataSize;
	}

	return m_pszBuffer;
}


NDUint32  NDByteBufferImpl::SetReadBufSize( NDUint32 nSize )
{
	if ( m_nHeadPos > m_nTailPos )
	{	//潜规则必须是m_nTailPos大于m_nHeadPos;
		return 0;
	}

	if ( ( m_nHeadPos + nSize ) >= m_nTailPos )
	{
		nSize = m_nTailPos - m_nHeadPos;

		m_nHeadPos = 0;
		m_nTailPos = 0;
		m_nDataSize= 0;
	}
	else
	{
		m_nHeadPos	+= nSize;
		m_nDataSize -= nSize;
	}

	return nSize;
}


NDUint32 NDByteBufferImpl::SetWriteBufSize( NDUint32 nSize )
{
	if ( m_nHeadPos > m_nTailPos )
	{	//潜规则必须是m_nTailPos大于m_nHeadPos;
		return 0;
	}

	if ( ( m_nTailPos + nSize ) >= m_nCapacitySize )
	{
		nSize = m_nCapacitySize - m_nTailPos;
	}

	m_nTailPos	+= nSize;
	m_nDataSize	+= nSize;

	return nSize;
}


NDUint32  NDByteBufferImpl::ReadBuffer( char *pszBuf, NDUint32 nSize )
{
	if ( ( 0 == pszBuf ) || ( nSize <= 0 ) )
	{
		return 0;
	}


	if ( m_nDataSize >= nSize )
	{	//数据足够的情况;
		if ( !ReadIntactBuffer( pszBuf, nSize ) )
		{
			return 0;
		}
		return nSize;
	}

	//数据不足,只能有多少读多少了;
	nSize = m_nDataSize;
	if ( nSize > 0 )
	{
		if ( m_nTailPos > m_nHeadPos )
		{	//数据尾部大于数据头(一段连续的内存数据);
			memcpy( pszBuf, m_pszBuffer + m_nHeadPos, nSize );
		}
		else
		{	//数据尾部小于数据头(两段内存数据);
			NDUint32 nPreRead	= m_nCapacitySize - m_nHeadPos;
			NDUint32 nNextRead	= nSize - nPreRead;
			memcpy( pszBuf, m_pszBuffer + m_nHeadPos, nPreRead );
			memcpy( pszBuf + nPreRead, m_pszBuffer, nNextRead );
		}

		m_nHeadPos = 0;
		m_nTailPos = 0;
		m_nDataSize= 0;
	}

	return nSize;
}

NDBool	NDByteBufferImpl::ReadIntactBuffer( char *pszBuf, NDUint32 nSize )
{
	if ( ( 0 == pszBuf ) || ( nSize <= 0 ) )
	{
		return NDFalse;
	}

	if ( m_nDataSize < nSize )
	{	//可读的数据大小不够;
		return NDFalse;
	}


	if ( m_nTailPos > m_nHeadPos )
	{	//数据尾部大于数据头(一段连续的内存数据);
		memcpy( pszBuf, m_pszBuffer + m_nHeadPos, nSize );
		m_nHeadPos += nSize;
	}
	else
	{	//数据尾部小于数据头(两段内存数据);
		NDUint32 nPreRead = m_nCapacitySize - m_nHeadPos;
		if ( nPreRead >= nSize )
		{	//数据尾部数据已经足够了;
			memcpy( pszBuf, m_pszBuffer + m_nHeadPos, nSize );
			m_nHeadPos += nSize;
			if ( nPreRead ==  nSize )
			{
				m_nHeadPos = 0;
			}
		}
		else
		{
			NDUint32 nNextRead = nSize - nPreRead;
			memcpy( pszBuf, m_pszBuffer + m_nHeadPos, nPreRead );
			memcpy( pszBuf + nPreRead, m_pszBuffer, nNextRead );
			m_nHeadPos = nNextRead;
		}
	}

	m_nDataSize -= nSize;
	if ( m_nDataSize == 0)
	{
		m_nHeadPos = 0;
		m_nTailPos = 0;
	}

	return NDTrue;
}


NDUint32  NDByteBufferImpl::WriteBuffer( const char *pszBuf, NDUint32 nSize )
{
	if ( ( 0 == pszBuf ) || ( nSize <= 0 ) )
	{
		return 0;
	}

	NDUint32 nSpaceSize = m_nCapacitySize - m_nDataSize;
	if ( nSize > nSpaceSize )
	{	//写入数据的大小大于剩余空间;
		if (nSize > m_nCapacitySize)
		{
			if ( !ReSet( (m_nCapacitySize + nSize) ) )
			{
				return 0;
			}
		}
		else
		{
			if ( !ReSet( (m_nCapacitySize * 2) ) )
			{
				return 0;
			}
		}
	}
	
	if ( m_nTailPos > m_nHeadPos )
	{	//数据尾部大于数据头(一段连续的内存数据);
		NDUint32 nPreWrite = m_nCapacitySize - m_nTailPos;
		if ( nPreWrite >= nSize )
		{	//尾部空间已经足够了;
			memcpy( m_pszBuffer + m_nTailPos, pszBuf, nSize );
			m_nTailPos += nSize;
		}
		else
		{
			NDUint32 nNextWrite = nSize - nPreWrite;
			memcpy( m_pszBuffer + m_nTailPos, pszBuf, nPreWrite );
			memcpy( m_pszBuffer, pszBuf + nPreWrite, nNextWrite );
			m_nTailPos = nNextWrite;
		}
	}
	else
	{
		memcpy( m_pszBuffer + m_nTailPos, pszBuf, nSize );
		m_nTailPos += nSize;
	}

	m_nDataSize += nSize;

	return nSize;

}


NDUint32  NDByteBufferImpl::WriteBuffer( const char *pszBuf )
{
	return WriteBuffer( pszBuf, (NDUint32)strlen(pszBuf) );
}

NDUint32 NDByteBufferImpl::WriteBuffer( const NDByteBufferImpl& refMsgBufImpl )
{
	//NDUint32 nSize = refMsgBufImpl.GetDataSize();
	//if ( nSize > 0 )
	//{
	//	NDUint32 nSpaceSize = m_nCapacitySize - m_nDataSize;
	//	if ( nSize > nSpaceSize )
	//	{
	//		if (nSize > m_nCapacitySize)
	//		{
	//			if ( !ReSet( (m_nCapacitySize + nSize) ) )
	//			{
	//				return 0;
	//			}
	//		}
	//		else
	//		{
	//			if ( !ReSet( (m_nCapacitySize * 2) ) )
	//			{
	//				return 0;
	//			}
	//		}
	//	}

	//	if ( m_nTailPos > m_nHeadPos )
	//	{	//数据尾部大于数据头(一段连续的内存数据);
	//		NDUint32 nPreWrite = m_nCapacitySize - m_nTailPos;
	//		if ( nPreWrite < nSize )
	//		{
	//			memmove( m_pszBuffer, m_pszBuffer + m_nHeadPos, m_nDataSize );
	//			m_nHeadPos = 0;
	//			m_nTailPos = m_nDataSize;
	//		}
	//	}

	//	if ( refMsgBufImpl.m_nTailPos > refMsgBufImpl.m_nHeadPos )
	//	{
	//		memcpy( m_pszBuffer + m_nTailPos, refMsgBufImpl.m_pszBuffer + refMsgBufImpl.m_nHeadPos, nSize );
	//	}
	//	else
	//	{
	//		NDUint32 nRefPreWrite = refMsgBufImpl.m_nCapacitySize - refMsgBufImpl.m_nHeadPos;
	//		memcpy( m_pszBuffer + m_nTailPos, refMsgBufImpl.m_pszBuffer + refMsgBufImpl.m_nHeadPos, nRefPreWrite );
	//		memcpy( m_pszBuffer + m_nTailPos + nRefPreWrite, refMsgBufImpl.m_pszBuffer, refMsgBufImpl.m_nTailPos );
	//	}

	//	m_nTailPos	+= nSize;
	//	m_nDataSize += nSize;

	//	return nSize;
	//}

	//return 0;
	return WriteBuffer( refMsgBufImpl, refMsgBufImpl.GetDataSize() );
}

NDUint32 NDByteBufferImpl::WriteBuffer( const NDByteBufferImpl& refMsgBufImpl, NDUint32 nSize )
{
	NDUint32 nRefRealSize = refMsgBufImpl.GetDataSize();
	if ( nSize > nRefRealSize )
	{
		nSize = nRefRealSize;
	}

	if ( nSize <= 0 )
	{
		return 0;
	}

	NDUint32 nSpaceSize = m_nCapacitySize - m_nDataSize;
	if ( nSize > nSpaceSize )
	{
		if (nSize > m_nCapacitySize)
		{
			if ( !ReSet( (m_nCapacitySize + nSize) ) )
			{
				return 0;
			}
		}
		else
		{
			if ( !ReSet( (m_nCapacitySize * 2) ) )
			{
				return 0;
			}
		}
	}

	if ( m_nTailPos > m_nHeadPos )
	{	//数据尾部大于数据头(一段连续的内存数据);
		NDUint32 nPreWrite = m_nCapacitySize - m_nTailPos;
		if ( nPreWrite < nSize )
		{
			memmove( m_pszBuffer, m_pszBuffer + m_nHeadPos, m_nDataSize );
			m_nHeadPos = 0;
			m_nTailPos = m_nDataSize;
		}
	}

	if ( refMsgBufImpl.m_nTailPos > refMsgBufImpl.m_nHeadPos )
	{
		memcpy( m_pszBuffer + m_nTailPos, refMsgBufImpl.m_pszBuffer + refMsgBufImpl.m_nHeadPos, nSize );
	}
	else
	{
		NDUint32 nRefPreWrite = refMsgBufImpl.m_nCapacitySize - refMsgBufImpl.m_nHeadPos;
		if ( nRefPreWrite >= nSize )
		{
			memcpy( m_pszBuffer + m_nTailPos, refMsgBufImpl.m_pszBuffer + refMsgBufImpl.m_nHeadPos, nSize );
		}
		else
		{
			NDUint32 nRefNextWrite = nSize - nRefPreWrite;
			memcpy( m_pszBuffer + m_nTailPos, refMsgBufImpl.m_pszBuffer + refMsgBufImpl.m_nHeadPos, nRefPreWrite );
			memcpy( m_pszBuffer + m_nTailPos + nRefPreWrite, refMsgBufImpl.m_pszBuffer, nRefNextWrite );
		}
	}

	m_nTailPos	+= nSize;
	m_nDataSize += nSize;

	return nSize;
}


NDBool NDByteBufferImpl::WriteIntactBuffer( const char *pszBuf, NDUint32 nSize )
{
	return ( nSize == WriteBuffer( pszBuf, nSize ) );
}

NDBool NDByteBufferImpl::ReSet( NDUint32 nSize )
{
	if ( nSize <= m_nCapacitySize )
	{
		return NDFalse;
	}

	nSize = (((nSize) + ((4) - 1)) & ~((4) - 1));

	char* pNewBuffer = (char*)GMalloc(m_pPool, (int)nSize);
	if ( 0 == pNewBuffer )
	{
		return NDFalse;
	}
	memset( pNewBuffer, 0, (int)nSize );

	NDUint32 nDataSize = m_nDataSize;
	if ( m_nDataSize > 0 )
	{
		if ( !ReadIntactBuffer( pNewBuffer, nDataSize) )
		{
			return NDFalse;
		}
	}

	m_pszBuffer		= pNewBuffer;
	m_nHeadPos		= 0;
	m_nTailPos		= nDataSize;
	m_nDataSize     = nDataSize;
	m_nCapacitySize = nSize;

	m_bRollBack		= NDFalse;
	m_nLastHeadPos	= 0;
	m_nLastTailPos	= 0;

	return NDTrue;
}


NDByteBufferImpl& NDByteBufferImpl::operator = ( const NDByteBufferImpl& other )
{
	if (this == &other)
	{
		return *this;
	}


	char* pNewBuffer = (char*)GMalloc(m_pPool, (int)other.m_nCapacitySize);
	if ( 0 == pNewBuffer )
	{
		return *this;
	}
	memset( pNewBuffer, 0, (int)other.m_nCapacitySize );

	if ( other.m_nDataSize > 0 )
	{
		if ( other.m_nHeadPos < other.m_nTailPos )
		{
			memcpy( pNewBuffer, other.m_pszBuffer + other.m_nHeadPos, other.GetDataSize() );
		}
		else
		{
			NDUint32 nPreRead = other.m_nCapacitySize - other.m_nHeadPos;
			memcpy( pNewBuffer, other.m_pszBuffer + other.m_nHeadPos, nPreRead );
			if ( other.m_nDataSize > nPreRead )
			{
				memcpy( pNewBuffer + nPreRead, other.m_pszBuffer, other.m_nTailPos );
			}
		}
	}

	m_pszBuffer		= pNewBuffer;
	m_nHeadPos		= 0;
	m_nTailPos		= other.m_nDataSize;
	m_nDataSize		= other.m_nDataSize;
	m_nCapacitySize	= other.m_nCapacitySize;

	m_bRollBack		= NDFalse;
	m_nLastHeadPos	= 0;
	m_nLastTailPos	= 0;

	return *this;
}




NDBool NDByteBufferImpl::ReadInt8( NDInt8 &refInt8 )
{
	return ReadIntactBuffer( (char*)&refInt8, sizeof( refInt8 ) );
}

NDBool NDByteBufferImpl::ReadInt16( NDInt16 &refInt16 )
{
	NDBool bRet	= ReadIntactBuffer( (char*)&refInt16, sizeof( refInt16 ) );

#ifdef USING_BIG_ENDIAN
	if ( bRet == NDTrue )
	{
		NDShareBaseGlobal::swap16( &refInt16 );
	}
#else
#endif

	return bRet;
}

NDBool NDByteBufferImpl::ReadInt32( NDInt32 &refInt32 )
{
	NDBool bRet	= ReadIntactBuffer( (char*)&refInt32, sizeof( refInt32 ) );

#ifdef USING_BIG_ENDIAN
	if ( bRet == NDTrue )
	{
		NDShareBaseGlobal::swap32( &refInt32 );
	}
#else
#endif

	return bRet;
}

NDBool NDByteBufferImpl::ReadInt64( NDInt64 &refInt64 )
{
	NDBool bRet	= ReadIntactBuffer( (char*)&refInt64, sizeof( refInt64 ) );

#ifdef USING_BIG_ENDIAN
	if ( bRet == NDTrue )
	{
		NDShareBaseGlobal::swap64( &refInt64 );
	}
#else
#endif

	return bRet;
}

NDBool NDByteBufferImpl::ReadUint8( NDUint8 &refUint8 )
{
	return ReadIntactBuffer( (char*)&refUint8, sizeof(refUint8) );
}

NDBool NDByteBufferImpl::ReadUint16( NDUint16 &refUint16 )
{
	NDBool bRet = ReadIntactBuffer( (char*)&refUint16, sizeof( refUint16 ) );

#ifdef USING_BIG_ENDIAN
	if ( bRet == NDTrue )
	{
		NDShareBaseGlobal::swap16( &refUint16 );
	}
#else
#endif

	return bRet;
}

NDBool NDByteBufferImpl::ReadUint32( NDUint32 &refUint32 )
{
	NDBool bRet	= ReadIntactBuffer( (char*)&refUint32, sizeof( refUint32 ) );

#ifdef USING_BIG_ENDIAN
	if ( bRet == NDTrue )
	{
		NDShareBaseGlobal::swap32( &refUint32 );
	}
#else
#endif

	return bRet;
}

NDBool NDByteBufferImpl::ReadUint64( NDUint64 &refUint64 )
{
	NDBool bRet	= ReadIntactBuffer( (char*)&refUint64, sizeof( refUint64 ) );

#ifdef USING_BIG_ENDIAN
	if ( bRet == NDTrue )
	{
		NDShareBaseGlobal::swap64( &refUint64 );
	}
#else
#endif

	return bRet;
}

NDBool NDByteBufferImpl::ReadFloat32( NDFloat32 &refFloat32 )
{
	NDBool bRet	= ReadIntactBuffer( (char*)&refFloat32, sizeof( refFloat32 ) );

#ifdef USING_BIG_ENDIAN
	if ( bRet == NDTrue )
	{
		NDShareBaseGlobal::swapfloat( &refFloat32 );
	}
#else
#endif

	return bRet;
}

NDBool NDByteBufferImpl::ReadFloat64( NDFloat64 &refFloat64 )
{
	NDBool bRet	= ReadIntactBuffer( (char*)&refFloat64, sizeof( refFloat64 ) );

#ifdef USING_BIG_ENDIAN
	if ( bRet == NDTrue )
	{
		NDShareBaseGlobal::swapdouble( &refFloat64 );
	}
#else
#endif

	return bRet;
}

NDBool NDByteBufferImpl::WriteInt8( NDInt8 nInt8 )
{
	NDUint32 nSize = sizeof( nInt8 );
	return ( nSize == WriteBuffer( (const char*)&nInt8, nSize ) );
}

NDBool NDByteBufferImpl::WriteInt16( NDInt16 nInt16 )
{
	NDUint32 nSize = sizeof( nInt16 );

#ifdef USING_BIG_ENDIAN
	NDShareBaseGlobal::swap16( &nInt16 );
#else
#endif

	return ( nSize == WriteBuffer( (const char*)&nInt16, nSize ) );
}

NDBool NDByteBufferImpl::WriteInt32( NDInt32 nInt32 )
{
	NDUint32 nSize = sizeof( nInt32 );

#ifdef USING_BIG_ENDIAN
	NDShareBaseGlobal::swap32( &nInt32 );
#else
#endif

	return ( nSize == WriteBuffer( (const char*)&nInt32, nSize ) );
}

NDBool NDByteBufferImpl::WriteInt64( NDInt64 nInt64 )
{
	NDUint32 nSize = sizeof( nInt64 );

#ifdef USING_BIG_ENDIAN
	NDShareBaseGlobal::swap64( &nInt64 );
#else
#endif

	return ( nSize == WriteBuffer( (const char*)&nInt64, nSize ) );
}

NDBool NDByteBufferImpl::WriteUint8( NDUint8 nUint8 )
{
	NDUint32 nSize = sizeof( nUint8 );
	return ( nSize == WriteBuffer( (const char*)&nUint8, nSize ) );
}

NDBool NDByteBufferImpl::WriteUint16( NDUint16 nUint16 )
{
	NDUint32 nSize = sizeof( nUint16 );

#ifdef USING_BIG_ENDIAN
	NDShareBaseGlobal::swap16( &nUint16 );
#else
#endif

	return  ( nSize == WriteBuffer( (const char*)&nUint16, nSize ) );
}

NDBool NDByteBufferImpl::WriteUint32( NDUint32 nUint32 )
{
	NDUint32 nSize = sizeof( nUint32 );

#ifdef USING_BIG_ENDIAN
	NDShareBaseGlobal::swap32( &nUint32 );
#else
#endif

	return ( nSize == WriteBuffer( (const char*)&nUint32, nSize ) );
}

NDBool NDByteBufferImpl::WriteUint64( NDUint64 nUint64 )
{
	NDUint32 nSize = sizeof( nUint64 );

#ifdef USING_BIG_ENDIAN
	NDShareBaseGlobal::swap64( &nUint64 );
#else
#endif

	return ( nSize == WriteBuffer( (const char*)&nUint64, nSize ) );
}

NDBool NDByteBufferImpl::WriteFloat32( NDFloat32 fFloat32 )
{
	NDUint32 nSize = sizeof( fFloat32 );

#ifdef USING_BIG_ENDIAN
	NDShareBaseGlobal::swapfloat( &fFloat32 );
#else
#endif

	return ( nSize == WriteBuffer( (const char*)&fFloat32, nSize ) );
}

NDBool NDByteBufferImpl::WriteFloat64( NDFloat64 fFloat64 )
{
	NDUint32 nSize = sizeof( fFloat64 );

#ifdef USING_BIG_ENDIAN
	NDShareBaseGlobal::swapdouble( &fFloat64 );
#else
#endif

	return ( nSize == WriteBuffer( (const char*)&fFloat64, nSize ) );
}

void NDByteBufferImpl::SetRollBackFlag()
{
	if ( NDFalse == m_bRollBack )
	{
		m_bRollBack		= NDTrue;
		m_nLastHeadPos	= m_nHeadPos;
		m_nLastTailPos	= m_nTailPos;
	}
}

void NDByteBufferImpl::CancelRollBack()
{
	m_bRollBack = NDFalse;
}

void NDByteBufferImpl::RollBack()
{
	if ( NDTrue == m_bRollBack )
	{
		m_nHeadPos = m_nLastHeadPos;
		m_nTailPos = m_nLastTailPos;

		if (m_nLastTailPos > m_nLastHeadPos) {
			m_nDataSize= (m_nLastTailPos - m_nLastHeadPos);
		} else {
			if (m_nLastTailPos == m_nLastHeadPos) {
				m_nHeadPos = 0;
				m_nTailPos = 0;
				m_nDataSize= 0;
			} else {
				m_nDataSize = (m_nCapacitySize - m_nHeadPos) + m_nTailPos;
			}
		}
	}	
}



_NDSHAREBASE_END



