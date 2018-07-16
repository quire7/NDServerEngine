#include "main/thread/NDDBThreadManager.h"

#include "main/local/NDDataServerEnums.h"

#include "main/thread/NDSelectDBThread.h"


NDDBThreadManager::NDDBThreadManager()
{

}

NDDBThreadManager::~NDDBThreadManager()
{
	release();
}

NDBool NDDBThreadManager::init(NDUint8 nSelectDBThreadNum, NDUint8 nUpdateDBThreadNum, const NDMysqlConnParam& refMysqlConnParam)
{
	char szBuf[BUF_LEN_64] = {0};
	for ( NDUint32 i = EDataServerDBThreadID_SELECT_START; i < EDataServerDBThreadID_SELECT_START + nSelectDBThreadNum; ++i )
	{
		NDSelectDBThread* pSelectDBThread = new NDSelectDBThread;
		if ( NULL == pSelectDBThread )
		{
			return NDFalse;
		}
		if ( NDFalse == pSelectDBThread->init( i, 100, refMysqlConnParam ) )
		{
			SAFE_DELETE( pSelectDBThread )
			return NDFalse;
		}
		
		memset( szBuf, 0, sizeof(szBuf) );
		ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, "NDSelectDBThread_%u", i );
		pSelectDBThread->setName( szBuf );
		pSelectDBThread->start();

		m_selectDBThreadPtrVec.push_back( pSelectDBThread );

		NDShareBaseGlobal::sleep2( 100 );
	}

	for ( NDUint32 i = EDataServerDBThreadID_UPDATE_START; i < EDataServerDBThreadID_UPDATE_START + nUpdateDBThreadNum; ++i )
	{
		NDUpdateDBThread* pUpdateDBThread = new NDUpdateDBThread;
		if ( NULL == pUpdateDBThread )
		{
			return NDFalse;
		}
		if ( NDFalse == pUpdateDBThread->init( i, 100, refMysqlConnParam ) )
		{
			SAFE_DELETE( pUpdateDBThread )
			return NDFalse;
		}

		memset( szBuf, 0, sizeof(szBuf) );
		ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, "NDUpdateDBThread_%u", i );
		pUpdateDBThread->setName( szBuf );
		pUpdateDBThread->start();

		m_updateDBThreadPtrVec.push_back( pUpdateDBThread );

		NDShareBaseGlobal::sleep2( 100 );
	}

	return NDTrue;
}

void NDDBThreadManager::release()
{
	NDDBThreadPtrVecIter iter		= m_selectDBThreadPtrVec.begin();
	NDDBThreadPtrVecIter iterEnd	= m_selectDBThreadPtrVec.end();
	for ( ; iter != iterEnd; ++iter )
	{
		SAFE_DELETE( *iter )
	}

	iter	= m_updateDBThreadPtrVec.begin();
	iterEnd	= m_updateDBThreadPtrVec.end();
	for ( ; iter != iterEnd; ++iter )
	{
		SAFE_DELETE( *iter )
	}
}

NDUpdateDBThread* NDDBThreadManager::getUpdateDBThread(NDTableBase* pTableBaseProduce)
{
	NDUint32 nSize = (NDUint32)m_updateDBThreadPtrVec.size();
	if ( nSize == 0 )
	{
		return NULL;
	}

	return (NDUpdateDBThread*)( m_updateDBThreadPtrVec[ (NDUint32)(pTableBaseProduce->getPlayerGUID() % nSize ) ] );
}

void NDDBThreadManager::stopSelectDBThread()
{
	NDDBThreadPtrVecIter iter		= m_selectDBThreadPtrVec.begin();
	NDDBThreadPtrVecIter iterEnd	= m_selectDBThreadPtrVec.end();
	for ( ; iter != iterEnd; ++iter )
	{
		NDSelectDBThread* pSelectDBThread = (NDSelectDBThread*)*iter;
		if ( NULL == pSelectDBThread )
		{
			continue;
		}

	}
}

void NDDBThreadManager::stopUpdateDBThread()
{
	NDDBThreadPtrVecIter iter		= m_updateDBThreadPtrVec.begin();
	NDDBThreadPtrVecIter iterEnd	= m_updateDBThreadPtrVec.end();
	for ( ; iter != iterEnd; ++iter )
	{
		NDUpdateDBThread* pUpdateDBThread = (NDUpdateDBThread*)*iter;
		if ( NULL == pUpdateDBThread )
		{
			continue;
		}

	}
}

void NDDBThreadManager::stopAllThread()
{
	stopSelectDBThread();
	stopUpdateDBThread();
}

NDBool NDDBThreadManager::isQuitAllThread()
{
	NDDBThreadPtrVecIter iter		= m_selectDBThreadPtrVec.begin();
	NDDBThreadPtrVecIter iterEnd	= m_selectDBThreadPtrVec.end();
	for ( ; iter != iterEnd; ++iter )
	{
		NDSelectDBThread* pSelectDBThread = (NDSelectDBThread*)*iter;
		if ( NULL == pSelectDBThread )
		{
			continue;
		}

		if ( NDFalse == pSelectDBThread->isQuitDBThread() )
		{
			return NDFalse;
		}
	}

	iter	= m_updateDBThreadPtrVec.begin();
	iterEnd	= m_updateDBThreadPtrVec.end();
	for ( ; iter != iterEnd; ++iter )
	{
		NDUpdateDBThread* pUpdateDBThread = (NDUpdateDBThread*)*iter;
		if ( NULL == pUpdateDBThread )
		{
			continue;
		}

		if ( NDFalse == pUpdateDBThread->isQuitDBThread() )
		{
			return NDFalse;
		}
	}

	return NDTrue;
}

