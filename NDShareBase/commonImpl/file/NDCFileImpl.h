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
	NDInt32		flush();	//如果成功刷新,flush返回0;

};


_NDSHAREBASE_END
#endif

