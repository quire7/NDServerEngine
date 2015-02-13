/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\memory\NDByteBuffer.h
	file base:	NDByteBuffer
	purpose:	byte buffer;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef _SHARE_BASE_ND_BYTE_BUFFER_H_
#define _SHARE_BASE_ND_BYTE_BUFFER_H_

#include "NDTypes.h"
#include "NDShareBaseMacros.h"

_NDSHAREBASE_BEGIN

class NDByteBufferImpl;
class NDByteBuffer
{
private:
	NDUint32			m_nPoolID;
	NDByteBufferImpl*	m_pNDByteBufferImpl;

public:
	explicit NDByteBuffer( NDUint32 nBufSize = MAX_MSGBUF_SIZE );
	NDByteBuffer( const NDByteBuffer& other );
	~NDByteBuffer(void);

	//clear NDMsgBuffer structure for reuse;
	void		Release();

	char*		ReadBuffer();
	NDUint32	SetReadBufSize( NDUint32 nSize );
	NDUint32	SetWriteBufSize( NDUint32 nSize );

	void		SetRollBackFlag();
	void		RollBack();
	void		CancelRollBack();

	//这个函数要慎用,当要读取的数据不足nSize时,有多少读多少;
	NDUint32	ReadBuffer( char *pszBuf, NDUint32 nSize );
	NDUint32	WriteBuffer( const char *pszBuf, NDUint32 nSize );
	NDUint32	WriteBuffer( const char *pszBuf );
	NDUint32	WriteBuffer( const NDByteBuffer& refMsgBuf );
	NDUint32	WriteBuffer( const NDByteBuffer& refMsgBuf, NDUint32 nSize );

	//ReadIntactBuffer是完整的读取nSize个字节的数据,如果数据不足,就不读;
	NDBool		ReadIntactBuffer( char *pszBuf, NDUint32 nSize );
	//WriteIntactBuffer是完整的写入nSize个字节的数据;
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
	
	NDUint32	GetCapacitySize()	const;
	NDUint32	GetSpaceSize()		const;
	NDUint32	GetDataSize()		const;

	NDByteBuffer& operator = ( const NDByteBuffer& other );

	//在NDObjectPool中的位置，用以NDObjectPool的优化
	void		setPoolID( NDUint32 nPoolID );
	NDUint32	getPoolID()const;

};

_NDSHAREBASE_END
#endif
