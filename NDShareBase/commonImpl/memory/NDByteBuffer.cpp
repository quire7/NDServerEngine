#include "memory/NDByteBuffer.h"

#include "memory/NDByteBufferImpl.h"


_NDSHAREBASE_BEGIN

NDByteBuffer::NDByteBuffer( NDUint32 nBufSize ) : m_nPoolID((NDUint32)ND_INVALID_ID), m_pNDByteBufferImpl( new NDByteBufferImpl(nBufSize) )
{

}

NDByteBuffer::NDByteBuffer( const NDByteBuffer& other ) : m_nPoolID((NDUint32)ND_INVALID_ID), m_pNDByteBufferImpl( new NDByteBufferImpl(*other.m_pNDByteBufferImpl) )
{
}

NDByteBuffer::~NDByteBuffer(void)
{
	SAFE_DELETE( m_pNDByteBufferImpl )
}

void NDByteBuffer::Release()
{
	if ( NULL == m_pNDByteBufferImpl )	return;

	m_pNDByteBufferImpl->Release();
}


char* NDByteBuffer::ReadBuffer()
{
	if ( NULL == m_pNDByteBufferImpl )	return NULL;

	return m_pNDByteBufferImpl->ReadBuffer();
}


NDUint32  NDByteBuffer::SetReadBufSize( NDUint32 nSize )
{
	if ( NULL == m_pNDByteBufferImpl )	return 0;

	return m_pNDByteBufferImpl->SetReadBufSize( nSize );
}


NDUint32 NDByteBuffer::SetWriteBufSize( NDUint32 nSize )
{
	if ( NULL == m_pNDByteBufferImpl )	return 0;

	return m_pNDByteBufferImpl->SetWriteBufSize( nSize );
}

NDUint32  NDByteBuffer::ReadBuffer( char *pszBuf, NDUint32 nSize )
{
	if ( NULL == m_pNDByteBufferImpl )	return 0;

	return m_pNDByteBufferImpl->ReadBuffer( pszBuf, nSize );
}

NDBool NDByteBuffer::ReadIntactBuffer( char *pszBuf, NDUint32 nSize )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->ReadIntactBuffer( pszBuf, nSize );
}


NDUint32  NDByteBuffer::WriteBuffer( const char *pszBuf, NDUint32 nSize )
{
	if ( NULL == m_pNDByteBufferImpl )	return 0;
	
	return m_pNDByteBufferImpl->WriteBuffer( pszBuf, nSize );
}


NDUint32  NDByteBuffer::WriteBuffer( const char *pszBuf )
{
	if ( NULL == m_pNDByteBufferImpl )	return 0;

	return m_pNDByteBufferImpl->WriteBuffer( pszBuf );
}

NDUint32 NDByteBuffer::WriteBuffer( const NDByteBuffer& refMsgBuf )
{
	if ( NULL == m_pNDByteBufferImpl )			return 0;
	if ( NULL == refMsgBuf.m_pNDByteBufferImpl )	return 0;

	return m_pNDByteBufferImpl->WriteBuffer( *(refMsgBuf.m_pNDByteBufferImpl) );
}

NDUint32 NDByteBuffer::WriteBuffer( const NDByteBuffer& refMsgBuf, NDUint32 nSize )
{
	if ( NULL == m_pNDByteBufferImpl )			return 0;
	if ( NULL == refMsgBuf.m_pNDByteBufferImpl )	return 0;

	//return m_pNDMsgBufferImpl->WriteBuffer( *(refMsgBuf.m_pNDMsgBufferImpl), refMsgBuf.GetDataSize() );
	return m_pNDByteBufferImpl->WriteBuffer( *(refMsgBuf.m_pNDByteBufferImpl), nSize );
}


NDBool NDByteBuffer::WriteIntactBuffer( const char *pszBuf, NDUint32 nSize )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->WriteIntactBuffer( pszBuf, nSize );
}




NDByteBuffer& NDByteBuffer::operator = ( const NDByteBuffer& other )
{
	if ( ( this == &other ) || ( NULL == m_pNDByteBufferImpl ) )
	{
		return *this;
	}
	
	*m_pNDByteBufferImpl = *other.m_pNDByteBufferImpl;

	return *this;
}

NDUint32 NDByteBuffer::GetCapacitySize() const
{
	if ( NULL == m_pNDByteBufferImpl )	return 0;

	return m_pNDByteBufferImpl->GetCapacitySize();
}

NDUint32 NDByteBuffer::GetSpaceSize() const
{
	if ( NULL == m_pNDByteBufferImpl )	return 0;

	return m_pNDByteBufferImpl->GetSpaceSize();
}

NDUint32 NDByteBuffer::GetDataSize() const
{
	if ( NULL == m_pNDByteBufferImpl )	return 0;

	return m_pNDByteBufferImpl->GetDataSize();
}

NDBool NDByteBuffer::ReSet( NDUint32 nSize )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->ReSet( nSize );
}

void NDByteBuffer::setPoolID( NDUint32 nPoolID )
{
	m_nPoolID = nPoolID;
}

NDUint32 NDByteBuffer::getPoolID() const
{
	return m_nPoolID;
}


NDBool NDByteBuffer::ReadInt8( NDInt8 &refInt8 )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->ReadInt8( refInt8 );
}

NDBool NDByteBuffer::ReadInt16( NDInt16 &refInt16 )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->ReadInt16( refInt16 );
}

NDBool NDByteBuffer::ReadInt32( NDInt32 &refInt32 )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->ReadInt32( refInt32 );
}

NDBool NDByteBuffer::ReadInt64( NDInt64 &refInt64 )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->ReadInt64( refInt64 );
}

NDBool NDByteBuffer::ReadUint8( NDUint8 &refUint8 )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->ReadUint8( refUint8 );
}

NDBool NDByteBuffer::ReadUint16( NDUint16 &refUint16 )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->ReadUint16( refUint16 );
}

NDBool NDByteBuffer::ReadUint32( NDUint32 &refUint32 )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->ReadUint32( refUint32 );
}

NDBool NDByteBuffer::ReadUint64( NDUint64 &refUint64 )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->ReadUint64( refUint64 );
}

NDBool NDByteBuffer::ReadFloat32( NDFloat32 &refFloat32 )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->ReadFloat32( refFloat32 );
}

NDBool NDByteBuffer::ReadFloat64( NDFloat64 &refFloat64 )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->ReadFloat64( refFloat64 );
}

NDBool NDByteBuffer::WriteInt8( NDInt8 nInt8 )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->WriteInt8( nInt8 );
}

NDBool NDByteBuffer::WriteInt16( NDInt16 nInt16 )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->WriteInt16( nInt16 );
}

NDBool NDByteBuffer::WriteInt32( NDInt32 nInt32 )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->WriteInt32( nInt32 );
}

NDBool NDByteBuffer::WriteInt64( NDInt64 nInt64 )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->WriteInt64( nInt64 );
}

NDBool NDByteBuffer::WriteUint8( NDUint8 nUint8 )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->WriteUint8( nUint8 );
}

NDBool NDByteBuffer::WriteUint16( NDUint16 nUint16 )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->WriteUint16( nUint16 );
}

NDBool NDByteBuffer::WriteUint32( NDUint32 nUint32 )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->WriteUint32( nUint32 );
}

NDBool NDByteBuffer::WriteUint64( NDUint64 nUint64 )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->WriteUint64( nUint64 );
}

NDBool NDByteBuffer::WriteFloat32( NDFloat32 fFloat32 )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->WriteFloat32( fFloat32 );
}

NDBool NDByteBuffer::WriteFloat64( NDFloat64 fFloat64 )
{
	if ( NULL == m_pNDByteBufferImpl )	return NDFalse;

	return m_pNDByteBufferImpl->WriteFloat64( fFloat64 );
}

void NDByteBuffer::SetRollBackFlag()
{
	if ( NULL == m_pNDByteBufferImpl )	return;

	return m_pNDByteBufferImpl->SetRollBackFlag();
}

void NDByteBuffer::RollBack()
{
	if ( NULL == m_pNDByteBufferImpl )	return;

	return m_pNDByteBufferImpl->RollBack();
}

void NDByteBuffer::CancelRollBack()
{
	if ( NULL == m_pNDByteBufferImpl )	return;

	return m_pNDByteBufferImpl->CancelRollBack();
}



_NDSHAREBASE_END

