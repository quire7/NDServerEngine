#include "thread/NDDBThread.h"

#include "NDShareBaseGlobal.h"
#include "file/NDLog.h"
#include "database/NDMysqlBaseOp.h"

_NDSHAREBASE_BEGIN
NDDBThread::NDDBThread() : m_nDBThreadID( 0 ), m_pNDMysqlBaseOp( NULL ), m_nSleepMSTime( 0 )
{
		
}

NDDBThread::~NDDBThread()
{
	release();
}

NDBool NDDBThread::init( NDUint32 nDBThreadID, NDTime nSleepMSTime, const NDMysqlConnParam& refMysqlConnParam )
{
	m_nDBThreadID	= nDBThreadID;
	m_nSleepMSTime	= nSleepMSTime;

	m_pNDMysqlBaseOp = new NDMysqlBaseOp;
	if ( NULL == m_pNDMysqlBaseOp )
	{
		return NDFalse;
	}
	if ( NDFalse == m_pNDMysqlBaseOp->initialize( refMysqlConnParam, 1 ) )
	{
		return NDFalse;
	}


	return NDTrue;
}

void NDDBThread::release()
{
	SAFE_DELETE( m_pNDMysqlBaseOp )
}

void NDDBThread::run()
{
	while ( NDTrue )
	{
		if ( NULL == m_pNDMysqlBaseOp )
		{
			NDLOG_ERROR( " [NDDBThread::run] m_pNDMysqlBaseOp is NULL." )
			break;
		}
		//不再利用网络异常,直接尝试创建新线程的方式,网络如果中断,创建新线程也没任何帮助;
		if( NDFalse == m_pNDMysqlBaseOp->checkConnections() )
		{
			NDLOG_ERROR( " [NDDBThread::run] mysql checkConnections failed. " )
			NDShareBaseGlobal::sleep2( m_nSleepMSTime );
			continue;
		}
		
		//处理数据;
		disposeOperateData();

		//DBThread的退出;
		if ( isQuitDBThread() )
		{
			break;
		}

		// 线程睡眠时间;
		NDShareBaseGlobal::sleep2( m_nSleepMSTime );
	}

	stop();
}

void NDDBThread::stop()
{
}

_NDSHAREBASE_END

