#include "NDShareBaseGlobal.h"

#include <iconv.h>  
#include <stdlib.h>  
#include <stdio.h>
#include <string.h>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <execinfo.h>
#include <algorithm>

#include "zlib.h"

#include "NDCLibFun.h"
#include "function/NDStatWatcher.h"

_NDSHAREBASE_BEGIN

static bool code_convert(char *from_charset, char *to_charset, char *inbuf, size_t inlen, char *outbuf, size_t outlen)
{
	iconv_t cd;
	char **pin = &inbuf;
	char **pout = &outbuf;

	cd = iconv_open(to_charset, from_charset);
	if (cd == 0)
		return false;
	memset(outbuf, 0, outlen);
	if (iconv(cd, pin, &inlen, pout, &outlen) == -1)
		return false;
	iconv_close(cd);
	*pout = '\0';

	return true;
}

NDBool NDShareBaseGlobal::utf8ToGBK(char *inbuf, NDUint32 inlen, char *outbuf, NDUint32 outlen)
{
	return code_convert( "utf-8", "gb2312", inbuf, (size_t)inlen, outbuf, (size_t)outlen );
}

NDBool NDShareBaseGlobal::gbkToUTF8(char *inbuf, NDUint32 inlen, char *outbuf, NDUint32 outlen)
{
	return code_convert( "gb2312", "utf-8", inbuf, (size_t)inlen, outbuf, (size_t)outlen );
}

_NDSHAREBASE_END
