#include "net/process/NDServerTask.h"

#include "memory/NDByteBuffer.h"
#include "memory/NDByteBufferPool.h"
#include "net/stream/NDIStreamImpl.h"

#include "net/process/NDProcess.h"


_NDSHAREBASE_BEGIN

NDServerTask*	NDServerTask::m_pNDServerTask = NULL;

NDServerTask* NDServerTask::getInstance()
{
	if ( NULL == m_pNDServerTask )
	{
		m_pNDServerTask = new NDServerTask();
	}

	return m_pNDServerTask;
}

void NDServerTask::releaseInstance()
{
	if ( NULL != m_pNDServerTask )
	{
		delete m_pNDServerTask;
		m_pNDServerTask = NULL;
	}
}


NDServerTask::NDServerTask(void) : m_pProcess( NULL )
{
	
}

NDServerTask::~NDServerTask(void)
{
	MsgBufQueue::TMessageDeque*	pDueue = NULL;
	if ( m_msgDeque.get_deque_nowait( pDueue ) )
	{	
		if ( NULL != pDueue )
		{
			MsgBufQueue::TMessageDeque::iterator iterBegin	= pDueue->begin() ;
			MsgBufQueue::TMessageDeque::iterator iterEnd	= pDueue->end();
			for ( ; iterBegin != iterEnd; ++iterBegin )
			{
				NDByteBuffer* pMsgBuffer = *iterBegin;
				if ( NULL != pMsgBuffer )
				{
					NDByteBufferPool::getInstance()->destroyByteBuffer( pMsgBuffer );
				}
			}
		}
	}

	m_pProcess = NULL;
}

NDBool NDServerTask::setProcessor( NDProcess* pProcess )
{
	if ( NULL == m_pProcess )
	{
		m_pProcess = pProcess;
		return NDTrue;
	}
	return NDFalse;
}

void NDServerTask::putQueue( NDByteBuffer* pMsgBuf )
{
	m_msgDeque.push( pMsgBuf );
}

void NDServerTask::getQueue( NDByteBuffer*& rpMsgBuf )
{
	m_msgDeque.pop( rpMsgBuf );
}

void NDServerTask::taskProcess()
{

	MsgBufQueue::TMessageDeque*	pDueue = NULL;

	MsgBufQueue::TMessageDeque::iterator iterBegin;
	MsgBufQueue::TMessageDeque::iterator iterEnd;

	while ( m_msgDeque.get_deque_nowait(pDueue) )
	{
		if ( NULL == pDueue )
		{
			break;
		}

		iterEnd = pDueue->end();
		for ( iterBegin = pDueue->begin(); iterBegin != iterEnd; ++iterBegin )
		{
			NDByteBuffer* pMsgBuffer = *iterBegin;
			if ( NULL != pMsgBuffer )
			{
				disposeMessage( (*pMsgBuffer) );
			}

			NDByteBufferPool::getInstance()->destroyByteBuffer( pMsgBuffer );
		}

		pDueue->clear();
		pDueue = NULL;
	}
}

void NDServerTask::disposeMessage( NDByteBuffer& rMsgBuf )
{
	NDUint32 nSize = rMsgBuf.getDataSize();
	if ( (nSize > 0) && (NULL != m_pProcess) )
	{
		NDIStreamImpl istreamImpl( rMsgBuf );
		if ( NULL != m_pProcess )
		{
			m_pProcess->process( istreamImpl );
		}
	}
}



_NDSHAREBASE_END

