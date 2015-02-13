#ifndef __SHARE_BASE_ND_TOTAL_LOG_H__
#define __SHARE_BASE_ND_TOTAL_LOG_H__


#include "NDCLogFile.h"
#include "NDConsoleLog.h"
#include "NDShareMemroyLogFile.h"


#ifndef SET_NDLOG_MGR
#define SET_NDLOG_MGR( pFileLogMgr, pSMLogMgr, bConsole )\
		SET_NDCONSOLE_LOG_MGR( bConsole )\
		SET_NDFILE_LOG_MGR( pFileLogMgr )\
		SET_NDSM_LOG_MGR( pSMLogMgr )
#endif


#ifndef NDLOG_INFO
#define NDLOG_INFO( info ) \
		if ( g_bConsole ) { NDCONSOLE_LOG_INFO << info << NDCONSOLE_LOG_END; } \
		NDFILE_LOG_INFO( info )	\
		NDSM_LOG_INFO( info )
#endif

#ifndef NDLOG_WARNGING
#define NDLOG_WARNGING( info ) \
		if ( g_bConsole ) { NDCONSOLE_LOG_WARNGING << info << NDCONSOLE_LOG_END; } \
		NDFILE_LOG_WARNGING( info )	\
		NDSM_LOG_WARNGING( info )
#endif

#ifndef NDLOG_ERROR
#define NDLOG_ERROR( info ) \
		if ( g_bConsole ) { NDCONSOLE_LOG_ERROR << info << NDCONSOLE_LOG_END; } \
		NDFILE_LOG_ERROR( info )	\
		NDSM_LOG_ERROR( info )
#endif

#endif

