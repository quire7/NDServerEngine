/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\file\NDWriteFixedFormatFile.h
	file base:	NDCFile
	purpose:	write fixed format file, ex:([excel:'\t'] or [csv:',']);
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_WRITE_FIXED_FORMAT_FILE_H__
#define __SHARE_BASE_ND_WRITE_FIXED_FORMAT_FILE_H__

#include <stdio.h>
#include "NDTypes.h"

_NDSHAREBASE_BEGIN

class NDCFileImpl;
class NDWriteFixedFormatFile
{
private:
	FILE*		m_pFile;
	NDUint32	m_nSize;

public:
	NDWriteFixedFormatFile(void);
	~NDWriteFixedFormatFile(void);

	NDBool		isopen() const;
	NDBool		open( const char* pFileName, const char* pOpenFlag="a+" );

	NDBool		writeHeader( const char* szWriteBuf, NDUint32 nSize );
	NDBool		writeContent( const char* szFormat, ... );

	NDUint32	size();
	void		close();
	NDBool		flush();

};

_NDSHAREBASE_END

#endif

