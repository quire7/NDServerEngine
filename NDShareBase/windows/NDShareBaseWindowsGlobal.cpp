#include "NDShareBaseGlobal.h"

#include <sys/timeb.h>
#include <sys/stat.h>
#include <io.h>
#include <direct.h>

#include "zlib.h"

#include "NDCLibFun.h"
#include "function/NDStatWatcher.h"

_NDSHAREBASE_BEGIN

NDBool NDShareBaseGlobal::utf8ToGBK(char *inbuf, NDUint32 inlen, char *outbuf, NDUint32 outlen)
{
	int nLen = ::MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, inbuf, -1, NULL, 0 );
	WCHAR* wszANSI = new WCHAR[nLen + 1];
	memset(wszANSI, 0, (nLen + 1)*sizeof(WCHAR));
	nLen = ::MultiByteToWideChar( CP_UTF8, 0, inbuf, -1, wszANSI, nLen );

	nLen = ::WideCharToMultiByte( CP_ACP, 0, wszANSI, -1, NULL, 0, NULL, NULL );
	if ( (int)outlen < nLen )
	{
		delete[]wszANSI;
		return NDFalse;
	}
	
	memset(outbuf, 0, outlen);
	::WideCharToMultiByte( CP_ACP, 0, wszANSI, -1, outbuf, nLen, NULL, NULL );

	delete[]wszANSI;

	return NDTrue;
}

NDBool NDShareBaseGlobal::gbkToUTF8(char *inbuf, NDUint32 inlen, char *outbuf, NDUint32 outlen)
{
	int nLen = ::MultiByteToWideChar( CP_ACP, MB_ERR_INVALID_CHARS, inbuf, -1, NULL, 0 );
	WCHAR* wszUTF8 = new WCHAR[nLen + 1];
	memset(wszUTF8, 0, (nLen + 1)*sizeof(WCHAR));
	nLen = ::MultiByteToWideChar( CP_ACP, 0, inbuf, -1, wszUTF8, nLen );

	nLen = ::WideCharToMultiByte( CP_UTF8, 0, wszUTF8, -1, NULL, 0, NULL, NULL );
	if ( (int)outlen < nLen )
	{
		delete[]wszUTF8;
		return NDFalse;
	}

	memset( outbuf, 0, outlen );
	::WideCharToMultiByte( CP_UTF8, 0, wszUTF8, -1, outbuf, nLen, NULL, NULL );

	delete[]wszUTF8;

	return NDTrue;
}

_NDSHAREBASE_END
