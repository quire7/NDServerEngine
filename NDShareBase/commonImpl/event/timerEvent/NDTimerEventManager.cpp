#include "event/timerEvent/NDTimerEventManager.h"

#include "NDShareBaseMacros.h"
#include "NDShareBaseGlobal.h"

#include "NDCLibFun.h"
#include "file/NDLog.h"


_NDSHAREBASE_BEGIN


///////////////////////////////////////////////////////////

NDTimerEventManager* NDTimerEventManager::s_pNDTimerEventManager = NULL;


NDTimerEventManager* NDTimerEventManager::getInstance()
{
	if ( NULL == s_pNDTimerEventManager )
	{
		s_pNDTimerEventManager = new NDTimerEventManager;
	}

	return s_pNDTimerEventManager;
}

void NDTimerEventManager::releaseInstance()
{
	SAFE_DELETE( s_pNDTimerEventManager );
}

NDTimerEventManager::NDTimerEventManager()
{
	m_nStartMSTime = NDShareBaseGlobal::getCurMSTimeOfUTC();
	
	m_wheels[0]	= new SWheel( WHEEL_SIZE1 );
	for ( int i = 1; i < WHEEL_NUM; ++i )
	{
		m_wheels[i]	= new SWheel( WHEEL_SIZE2 );
	}
}

NDTimerEventManager::~NDTimerEventManager()
{
	for (int i = 0; i < WHEEL_NUM; ++i)
	{
		if ( NULL != m_wheels[i] ) 
		{
			delete m_wheels[i]; 
			m_wheels[i] = NULL;
		}
	}
}

void NDTimerEventManager::addTimerNode( NDTime nMillSeconds, STimerNode* node )
{
	SNodeLink *spoke = NULL;
	NDUint32 interval	= (NDUint32)(nMillSeconds / GRANULARITY);
	NDUint32 threshold1 = (1 << WHEEL_BITS1);
	NDUint32 threshold2 = 1 << (WHEEL_BITS1 + WHEEL_BITS2);
	NDUint32 threshold3 = 1 << (WHEEL_BITS1 + 2 * WHEEL_BITS2);
	NDUint32 threshold4 = 1 << (WHEEL_BITS1 + 3 * WHEEL_BITS2);

	if (interval < threshold1) {
		NDUint32 index = (interval + m_wheels[0]->spokeindex) & WHEEL_MASK1;
		spoke = m_wheels[0]->spokes + index;
	} else if (interval < threshold2) {
		NDUint32 index = ((interval - threshold1 + m_wheels[1]->spokeindex * threshold1) >> WHEEL_BITS1) & WHEEL_MASK2;
		spoke = m_wheels[1]->spokes + index;
	} else if (interval < threshold3) {
		NDUint32 index = ((interval - threshold2 + m_wheels[2]->spokeindex * threshold2) >> (WHEEL_BITS1 + WHEEL_BITS2)) & WHEEL_MASK2;
		spoke = m_wheels[2]->spokes + index;
	} else if (interval < threshold4) {
		NDUint32 index = ((interval - threshold3 + m_wheels[3]->spokeindex * threshold3) >> (WHEEL_BITS1 + 2 * WHEEL_BITS2)) & WHEEL_MASK2;
		spoke = m_wheels[3]->spokes + index;
	} else {
		NDUint32 index = ((interval - threshold4 + m_wheels[4]->spokeindex * threshold4) >> (WHEEL_BITS1 + 3 * WHEEL_BITS2)) & WHEEL_MASK2;
		spoke = m_wheels[4]->spokes + index;
	}
	SNodeLink *nodelink = &(node->link);
	nodelink->prev = spoke->prev;
	spoke->prev->next = nodelink;
	nodelink->next = spoke;
	spoke->prev = nodelink;	
}

NDShareBase::NDTimerBoundSlotConn NDTimerEventManager::addTimer( const NDSubFunctorSlot& refSubFunctorSlot, const NDTimerEventArgs& refTimerEventArgs )
{
	STimerNode *node = new STimerNode( refSubFunctorSlot, refTimerEventArgs );

	NDTime nMillSeconds = refTimerEventArgs.m_nProcessMSTimes - m_nStartMSTime;
	addTimerNode( nMillSeconds, node );

	return node->timerBoundConn;
}

void NDTimerEventManager::detectTimerList()
{
	NDTime	nNowMSTime	= NDShareBaseGlobal::getCurMSTimeOfUTC();

	NDUint32 nLoopNum	= ( nNowMSTime > m_nStartMSTime ) ? (NDUint32)( (nNowMSTime - m_nStartMSTime) / GRANULARITY ) : 0;

	SWheel* wheel = m_wheels[0];

	for ( NDUint32 i = 0; i < nLoopNum; ++i )
	{
		SNodeLink *spoke	= wheel->spokes + wheel->spokeindex;
		SNodeLink *link		= spoke->next;

		while (link != spoke) {
			STimerNode *node = (STimerNode *)link;
			link->prev->next = link->next;
			link->next->prev = link->prev;
			link = node->link.next;
			addToReadyNode( node );
		}

		if (++(wheel->spokeindex) >= wheel->size) {
			wheel->spokeindex = 0;
			cascade(1);
		}
		m_nStartMSTime += GRANULARITY;
	}

	doTimeOutCallBack();
}

void NDTimerEventManager::addToReadyNode( STimerNode* node )
{
	SNodeLink *nodelink		= &(node->link);
	nodelink->prev			= m_readnodes.prev;
	m_readnodes.prev->next	= nodelink;
	nodelink->next			= &m_readnodes;
	m_readnodes.prev		= nodelink;
}

void NDTimerEventManager::doTimeOutCallBack()
{
	SNodeLink *link = m_readnodes.next;
	//if (link != &readynodes_) {Dump();}
	while (link != &m_readnodes) {
		STimerNode *node	= (STimerNode *)link;
		link->prev->next	= link->next;
		link->next->prev	= link->prev;
		link = node->link.next;

		if ( NDTrue == node->timerBoundConn->connected() )
		{
			node->timerBoundConn->fireTimerEvent();

			if ( NDTrue == node->timerBoundConn->isLoopTimerEvent() )
			{
				node->timerBoundConn->modifyFireEventTimes();

				//重新插入时间轮;
				NDTime nMillSeconds = node->timerBoundConn->getFireEventTimes() - m_nStartMSTime;
				addTimerNode( nMillSeconds, node );
			}
			else
			{
				delete node;
				node = NULL;
			}
		}
		else
		{
			delete node;
			node = NULL;
		}
	}
	m_readnodes.next = m_readnodes.prev = &m_readnodes;
}

NDUint32 NDTimerEventManager::cascade( NDUint32 nWheelIndex )
{
	if (nWheelIndex < 1 || nWheelIndex >= WHEEL_NUM) {
		return 0;
	}
	
	int casnum = 0;
	NDTime now = NDShareBaseGlobal::getCurMSTimeOfUTC();

	SWheel *wheel		=  m_wheels[nWheelIndex];
	SNodeLink *spoke	= wheel->spokes + (wheel->spokeindex++);
	SNodeLink *link		= spoke->next;
	spoke->next			= spoke->prev = spoke;	//把环指针指向自己;(while里面清空所有节点);

	while (link != spoke) {
		STimerNode *node = (STimerNode *)link;
		link = node->link.next;

		NDTime nProcessMSTimes = node->timerBoundConn->getFireEventTimes();
		if ( nProcessMSTimes <= now ) 
		{
			addToReadyNode( node );
		} 
		else 
		{
			NDTime milseconds = nProcessMSTimes - now;
			addTimerNode(milseconds, node);
			++casnum;
		}
	}

	if (wheel->spokeindex >= wheel->size) {
		wheel->spokeindex = 0;
		casnum += cascade( ++nWheelIndex );
	}
	return casnum;	
}

void NDTimerEventManager::removeTimer( STimerNode* node )
{
	SNodeLink *nodelink = &(node->link);
	if (nodelink->prev) {
		nodelink->prev->next = nodelink->next;
	}
	if (nodelink->next) {
		nodelink->next->prev = nodelink->prev;
	}
	nodelink->prev = nodelink->next = NULL;

	delete node;
	node = NULL;
}

NDBool NDTimerEventManager::printTimerList( const NDEventArgs& )
{
	NDLOG_INFO( " [timerlist] NDTimerEventManager::printTimerList start." )

	for (int i = 0; i < WHEEL_NUM; ++i) {
		SWheel *wheel = m_wheels[i];

		
		NDLOG_INFO( " [timerlist] wheel[%d], wheel_size[%u], spokeindex[%u].", i, wheel->size, wheel->spokeindex );
		
		for (NDUint32 j = 0; j < wheel->size; ++j) {
			SNodeLink *spoke = wheel->spokes + j;
			SNodeLink *link = spoke->next;
			if (link != spoke) {

				NDLOG_INFO( " [timerlist] spoke index[%d], addr[%p], next[%p], prev[%p].", j, spoke, spoke->next, spoke->prev );
				
			}
			while (link != spoke) {
				STimerNode *node = (STimerNode *)link;

				
				NDLOG_INFO( " [timerlist] node[%p], next[%p], prev[%p] fireEventName:[%s], interval:[%lld], dead_time:[%lld].", link, link->next, link->prev,
																																node->timerBoundConn->getFireEventName(),
																																node->timerBoundConn->getFireEventIntervalTimes(),
																																node->timerBoundConn->getFireEventTimes() );

				link = node->link.next;
			}
		}
	}

	NDLOG_INFO( " [timerlist] NDTimerEventManager::printTimerList end." )

	return NDTrue;
}



_NDSHAREBASE_END

