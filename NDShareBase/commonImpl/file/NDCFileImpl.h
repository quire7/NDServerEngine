#ifndef __SHARE_BASE_ND_C_FILE_IMPL_H__
#define __SHARE_BASE_ND_C_FILE_IMPL_H__

#include <stdio.h>

#include "NDTypes.h"

_NDSHAREBASE_BEGIN

class NDCFileImpl
{
private:
	FILE*	m_pFile;
	char*	m_pFileName;
	long	m_nReadStart;

public:
	NDCFileImpl(void);
	~NDCFileImpl(void);

	NDBool		isopen() const;
	NDBool		open( const char* pFileName, const char* pOpenFlag );

	NDBool		readline( void* pReadBuf, NDUint32 nSize );

	NDUint32	read( void* pReadBuf, NDUint32 nSize );
	NDUint32	write( const void* pWriteBuf, NDUint32 nSize );
	NDUint32	size();
	void		close();
	NDBool		flush();	//����ɹ�ˢ��,flush����0;

};


_NDSHAREBASE_END
#endif

