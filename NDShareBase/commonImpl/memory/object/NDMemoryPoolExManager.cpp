
#include "memory/object/NDMemoryPoolExManager.h"

#include "NDShareBaseMacros.h"
#include "NDShareBaseGlobal.h"

#include "NDCLibFun.h"
#include "file/NDLog.h"

#include "event/functor/NDSubFunctorSlot.h"
#include "event/timerEvent/NDTimerEventManager.h"

_NDSHAREBASE_BEGIN
//----------------------------------------------------------------------------

const char* NDMemoryPoolExManager::s_szTimerPrintMemInfo = "timerPrintMemInfo";
NDMemoryPoolExManager* NDMemoryPoolExManager::s_pNDMemoryPoolExManager = NULL;

NDMemoryPoolExManager* NDMemoryPoolExManager::getInstance()
{
	if ( NULL == s_pNDMemoryPoolExManager )
	{
		s_pNDMemoryPoolExManager = new NDMemoryPoolExManager;
	}

	return s_pNDMemoryPoolExManager;
}

void NDMemoryPoolExManager::releaseInstance()
{
	SAFE_DELETE( s_pNDMemoryPoolExManager )
}

NDMemoryPoolExManager::NDMemoryPoolExManager()
{
}

NDMemoryPoolExManager::~NDMemoryPoolExManager()
{
	release();
}


NDBool NDMemoryPoolExManager::init()
{
	NDTimerEventArgs printMemInfoSubFunctorSlotArgs( NDMemoryPoolExManager::s_szTimerPrintMemInfo , ND_PRINT_OBJMEMEX_INFO_MILLISECONDS,  NDShareBaseGlobal::getCurMSTimeOfUTC() + ND_PRINT_OBJMEMEX_INFO_MILLISECONDS );
	m_timerPrintMemInfoConn = NDShareBase::NDTimerEventManager::getInstance()->addTimer( NDSubFunctorSlot(&NDMemoryPoolExManager::printMemInfo, this), printMemInfoSubFunctorSlotArgs );

	return m_timerPrintMemInfoConn.isvaild();
}

//*****************************************************************************
//清除;
void NDMemoryPoolExManager::release()
{
	std::vector<NDMemoryPoolBase*>::iterator itBegin	= m_MemoryPoolVec.begin();
	std::vector<NDMemoryPoolBase*>::iterator itEnd		= m_MemoryPoolVec.end();

	for ( ; itBegin != itEnd; ++itBegin )
	{
		SAFE_DELETE(*itBegin)
	}

	m_MemoryPoolVec.clear();

	if ( m_timerPrintMemInfoConn.isvaild() )
	{
		m_timerPrintMemInfoConn->disconnect();
	}
}
//*****************************************************************************
//定时打印内存当前信息;
NDBool NDMemoryPoolExManager::printMemInfo( const NDEventArgs& )
{
	if ( m_MemoryPoolVec.empty() )
	{
		return NDTrue;
	}

	NDGuardLock Lock(m_Lock);

	std::vector<NDMemoryPoolBase*>::iterator itBegin	= m_MemoryPoolVec.begin();
	std::vector<NDMemoryPoolBase*>::iterator itEnd		= m_MemoryPoolVec.end();
	for ( ; itBegin != itEnd; ++itBegin )
	{	
		NDMemoryPoolBase* pMemoryPoolBase = (*itBegin);
		if ( NULL == pMemoryPoolBase )
		{
			continue;
		}

		NDMemoryAssignUse& refMemWatch = pMemoryPoolBase->m_MemWatch;

		NDLOG_INFO( " [MemoryPoolEX] Memory type:[%s], AssignNum:[%u], UseNum:[%u].", pMemoryPoolBase->type().name(), refMemWatch.nAssignNum, refMemWatch.nUseNum );
	}

	return NDTrue;
}



_NDSHAREBASE_END
//*****************************************************************************

