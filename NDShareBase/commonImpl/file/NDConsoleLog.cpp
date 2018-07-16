#include "file/NDConsoleLog.h"

#include "NDCLibFun.h"
#include "NDShareBaseMacros.h"

_NDSHAREBASE_BEGIN

NDBool	g_bConsole = NDTrue;

NDConsoleLog::NDConsoleLog(void)
{
}

NDConsoleLog::~NDConsoleLog(void)
{
}


void NDConsoleLog::info( const char* info, ... )
{
	if ( NDFalse == g_bConsole ) return;

	char szConsoleBuf[BUF_LEN_1024] = { 0 };
	va_list ap;
	va_start( ap, info );
	ND_VSNPRINTF( szConsoleBuf, sizeof(szConsoleBuf)-1, info, ap );
	va_end( ap );
	NDCONSOLE_LOG_INFO << szConsoleBuf << NDCONSOLE_LOG_END;
}

void NDConsoleLog::warning( const char* info, ... )
{
	if ( NDFalse == g_bConsole ) return;
		
	char szConsoleBuf[BUF_LEN_1024] = { 0 };
	va_list ap;
	va_start( ap, info );
	ND_VSNPRINTF( szConsoleBuf, sizeof(szConsoleBuf)-1, info, ap );
	va_end( ap );
	NDCONSOLE_LOG_WARNING << szConsoleBuf << NDCONSOLE_LOG_END;
}


void NDConsoleLog::error( const char* info, ... )
{
	if (NDFalse == g_bConsole) return;

	char szConsoleBuf[BUF_LEN_1024] = { 0 };
	va_list ap;
	va_start(ap, info);
	ND_VSNPRINTF(szConsoleBuf, sizeof(szConsoleBuf)-1, info, ap);
	va_end(ap);
	NDCONSOLE_LOG_ERROR << szConsoleBuf << NDCONSOLE_LOG_END;
}


_NDSHAREBASE_END
