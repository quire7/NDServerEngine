#include "event/timerEvent/NDTimerEventManager.h"

#include "NDShareBaseMacros.h"
#include "NDShareBaseGlobal.h"



_NDSHAREBASE_BEGIN


///////////////////////////////////////////////////////////

NDTimerManager* NDTimerManager::s_pNDTimerManager = NULL;


NDTimerManager* NDTimerManager::getInstance()
{
	if ( NULL == s_pNDTimerManager )
	{
		s_pNDTimerManager = new NDTimerManager;
	}

	return s_pNDTimerManager;
}

void NDTimerManager::releaseInstance()
{
	SAFE_DELETE( s_pNDTimerManager );
}

NDTimerManager::NDTimerManager()
{
	m_nStartMSTime = NDShareBaseGlobal::getCurMSTimeOfUTC();
	
	m_wheels[0]	= new SWheel( WHEEL_SIZE1 );
	for ( int i = 1; i < WHEEL_NUM; ++i )
	{
		m_wheels[i]	= new SWheel( WHEEL_SIZE2 );
	}
}

NDTimerManager::~NDTimerManager()
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

void NDTimerManager::addTimerNode( NDTime nMillSeconds, STimerNode* node )
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

NDShareBase::NDTimerBoundSlotConn NDTimerManager::addTimer( const NDSubFunctorSlot& refSubFunctorSlot, const NDTimerEventArgs& refTimerEventArgs )
{
	STimerNode *node = new STimerNode( refSubFunctorSlot, refTimerEventArgs );

	NDTime nMillSeconds = refTimerEventArgs.m_nProcessMSTimes - m_nStartMSTime;
	addTimerNode( nMillSeconds, node );

	return node->timerBoundConn;
}

void NDTimerManager::detectTimerList()
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

void NDTimerManager::addToReadyNode( STimerNode* node )
{
	SNodeLink *nodelink		= &(node->link);
	nodelink->prev			= m_readnodes.prev;
	m_readnodes.prev->next	= nodelink;
	nodelink->next			= &m_readnodes;
	m_readnodes.prev		= nodelink;
}

void NDTimerManager::doTimeOutCallBack()
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

NDUint32 NDTimerManager::cascade( NDUint32 nWheelIndex )
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

void NDTimerManager::removeTimer( STimerNode* node )
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

void NDTimerManager::printTimerList()
{
	//BS_XLOG(XLOG_DEBUG, "CTimerManager::%s ===== \n", __FUNCTION__);
	for (int i = 0; i < WHEEL_NUM; ++i) {
		SWheel *wheel = m_wheels[i];
		//BS_XLOG(XLOG_DEBUG, "    wheel[%d].size[%u], spokeindex[%u]:\n", i, wheel->size, wheel->spokeindex);
		for (NDUint32 j = 0; j < wheel->size; ++j) {
			SNodeLink *spoke = wheel->spokes + j;
			SNodeLink *link = spoke->next;
			if (link != spoke) {
				//BS_XLOG(XLOG_DEBUG, "       spoke index[%d], addr[%p], next[%p], prev[%p]\n", j, spoke, spoke->next, spoke->prev);
			}
			while (link != spoke) {
				STimerNode *node = (STimerNode *)link;
				//BS_XLOG(XLOG_DEBUG, "          node[%p], next[%p], prev[%p] interval[%u], dead_time[%lld]\n",
				//	link, link->next, link->prev, node->timer->GetInterval(), node->dead_time);
				link = node->link.next;
			}
		}
	}
}



_NDSHAREBASE_END

