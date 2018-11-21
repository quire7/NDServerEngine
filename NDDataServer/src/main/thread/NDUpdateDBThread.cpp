#include "main/thread/NDUpdateDBThread.h"

#include "database/NDMysqlBaseOp.h"

NDUpdateDBThread::NDUpdateDBThread()
{
		
}

NDUpdateDBThread::~NDUpdateDBThread()
{

}

void NDUpdateDBThread::insertOperateData( NDTableBase* pTableBaseProduce )
{
	NDGuardLock autolock(m_DataLock);
	m_TableProducelist.push_back( pTableBaseProduce );
}

NDBool NDUpdateDBThread::disposeOperateData()
{
	if ( NULL == m_pNDMysqlBaseOp )
		return NDFalse;

	NDUint32 nOperatorDataCount = 0;		//每次循环最多操作数据的数量;
	while (true)
	{
		//每更新一条数据就查询一次SQL状态吧,先不管性能了;
		if( NDFalse == m_pNDMysqlBaseOp->checkConnections() )
		{
			continue;
		}

		NDTableBase *pData = NULL;
		{
			NDGuardLock autolock( m_DataLock );
			if ( m_TableProducelist.empty() )
			{
				break;
			}
			pData = m_TableProducelist.front();
			m_TableProducelist.pop_front();
		}

		//超过某个数值就返回吧,防止大量数据出现while时间过长;	
		nOperatorDataCount++;
		if ( nOperatorDataCount > 1000 )
		{
			break;
		}

		if ( NULL == pData )
		{
			NDLOG_ERROR( " [NDDBUpdateThread::disposeOperateData] TableBase *pData is NULL! " )
			continue;
		}
		// 2013.8.30 修改以后不在判断EDataSourceOperType_UPDATE和EDataSourceOperType_INSERT;
		NDUint8 nOperType = pData->getOperType();
		if ( EDataSourceOperType_DELETE == nOperType )
		{
			NDUint32 nAffectedRows = 0;
			if ( NDFalse == pData->onDelete( m_pNDMysqlBaseOp, nAffectedRows ) )
			{
				NDLOG_ERROR( " [NDDBUpdateThread::disposeOperateData] Data delete failed! " )
			}
		}
		else if ( EDataSourceOperType_INSERT == nOperType || EDataSourceOperType_UPDATE == nOperType )
		{	//不是DELETE肯定是update或insert;
			NDUint32 nAffectedRows = 0;
			if( NDFalse == pData->onUpdate( m_pNDMysqlBaseOp, nAffectedRows ) )
			{
				NDLOG_ERROR( " [NDDBUpdateThread::disposeOperateData] DATA update Fail! " )
			}
		}
		else
		{
			//error;
			NDLOG_ERROR( " [NDDBUpdateThread::disposeOperateData] pData operateType error. nOperType:[%u].", nOperType );
		}

		{
			NDMemoryPoolExManager::getInstance()->freeMemory( pData );
			pData = NULL;
		}
	}
	return NDTrue;
}

NDBool NDUpdateDBThread::isOperateDataEmpty()
{
	NDGuardLock autolock( m_DataLock );
	return m_TableProducelist.empty();
}

NDBool NDUpdateDBThread::isQuitDBThread()
{
	return NDFalse;
}



