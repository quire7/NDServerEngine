#ifndef _SHARE_BASE_ND_BYTE_BUFFER_IMPL_H_
#define _SHARE_BASE_ND_BYTE_BUFFER_IMPL_H_

#include "NDTypes.h"

_NDSHAREBASE_BEGIN

typedef void GMemPool;
class NDByteBufferImpl
{
public:
	NDByteBufferImpl( NDUint32 nBufSize );
	NDByteBufferImpl( const NDByteBufferImpl& other );
	~NDByteBufferImpl(void);

	void		Release();

	char*		ReadBuffer();
	NDUint32	SetReadBufSize( NDUint32 nSize );
	NDUint32	SetWriteBufSize( NDUint32 nSize );

	void		SetRollBackFlag();
	void		RollBack();
	void		CancelRollBack();

	NDUint32	ReadBuffer( char *pszBuf, NDUint32 nSize );
	NDUint32	WriteBuffer( const char *pszBuf, NDUint32 nSize );
	NDUint32	WriteBuffer( const char *pszBuf );
	NDUint32	WriteBuffer( const NDByteBufferImpl& refMsgBufImpl );
	NDUint32	WriteBuffer( const NDByteBufferImpl& refMsgBufImpl, NDUint32 nSize );

	NDBool		ReadIntactBuffer( char *pszBuf, NDUint32 nSize );
	NDBool		WriteIntactBuffer( const char *pszBuf, NDUint32 nSize );

	NDBool		ReadInt8( NDInt8 &refInt8 );
	NDBool		ReadInt16( NDInt16 &refInt16 );
	NDBool		ReadInt32( NDInt32 &refInt32 );
	NDBool		ReadInt64( NDInt64 &refInt64 );

	NDBool		ReadUint8( NDUint8 &refUint8 );
	NDBool		ReadUint16( NDUint16 &refUint16 );
	NDBool		ReadUint32( NDUint32 &refUint32 );
	NDBool		ReadUint64( NDUint64 &refUint64 );

	NDBool		ReadFloat32( NDFloat32 &refFloat32 );
	NDBool		ReadFloat64( NDFloat64 &refFloat64 );

	NDBool		WriteInt8( NDInt8 nInt8 );
	NDBool		WriteInt16( NDInt16 nInt16 );
	NDBool		WriteInt32( NDInt32 nInt32 );
	NDBool		WriteInt64( NDInt64 nInt64 );

	NDBool		WriteUint8( NDUint8 nUint8 );
	NDBool		WriteUint16( NDUint16 nUint16 );
	NDBool		WriteUint32( NDUint32 nUint32 );
	NDBool		WriteUint64( NDUint64 nUint64 );

	NDBool		WriteFloat32( NDFloat32 fFloat32 );
	NDBool		WriteFloat64( NDFloat64 fFloat64 );

	//resume assign buffer size
	NDBool		ReSet( NDUint32 nSize );
	

	NDUint32	GetCapacitySize()	const { return m_nCapacitySize; }
	NDUint32	GetSpaceSize()		const { return ( m_nCapacitySize - m_nDataSize ); }
	NDUint32	GetDataSize()		const { return ( m_nDataSize ); }

	NDByteBufferImpl& operator = ( const NDByteBufferImpl& other );

private:
	//clear NDMsgBufferImpl structure
	void		Clear();
private:
	GMemPool *m_pPool;

	char	*m_pszBuffer;

	NDUint32 m_nHeadPos;		//数据开始位置;
	NDUint32 m_nTailPos;		//数据结束位置;
	NDUint32 m_nDataSize;		//实有数据的大小;
	NDUint32 m_nCapacitySize;	//可容纳数据的大小;

	NDBool	 m_bRollBack;		//回滚标记;
	NDUint32 m_nLastHeadPos;	//回滚前记下数据开始位置;
	NDUint32 m_nLastTailPos;	//回滚前记下数据结束位置;
};


_NDSHAREBASE_END
#endif
