#include "function/NDStatWatcher.h"

#include <sstream>

#include "NDShareBaseGlobal.h"

_NDSHAREBASE_BEGIN

StatData::StatData( const char** pItemVec, NDUint32 nSize )
{
	m_nItemSize		= nSize;
	m_itemsStringVec.reserve( nSize );
	for ( NDUint32 i = 0; i < nSize; ++i )
	{
		m_itemsStringVec.push_back( pItemVec[i] );
	}

	m_StatInfoMap.clear();
}

StatData::~StatData()
{
	m_itemsStringVec.clear();
	m_StatInfoMap.clear();
}

void StatData::RecordConst( StatInfo* pInfo, NDUint32 nItem, NDUint64 nValue )
{
	StatItemInfo* pFind = GetStatItemInfo( pInfo, nItem );
	if ( NULL == pFind )
	{
		return;
	}

	++pFind->nCount;

	StatItemBaseInfo& refStatItemInfoContent = pFind->bsInfoContent;

	refStatItemInfoContent.nTotal += nValue;
	if ( nValue > refStatItemInfoContent.nMaxV )	refStatItemInfoContent.nMaxV = nValue;
	if ( nValue < refStatItemInfoContent.nMinV )	refStatItemInfoContent.nMinV = nValue;
}

void StatData::BeginDiff( StatInfo* pInfo, NDUint32 nItem, NDUint64 nValue )
{
	StatItemInfo* pFind = GetStatItemInfo( pInfo, nItem );
	if ( NULL == pFind )
	{
		return;
	}
	//记录开始时间;
	StatItemBaseInfo& refStatItemInfoTime		= pFind->bsInfoTime;
	refStatItemInfoTime.nDiff					= NDShareBaseGlobal::getMSTimeSinceStartBoot();
	//记录内容;
	StatItemBaseInfo& refStatItemInfoContent	= pFind->bsInfoContent;
	refStatItemInfoContent.nDiff				= nValue;
}

void StatData::EndDiff( StatInfo* pInfo, NDUint32 nItem, NDUint64 nValue )
{
	StatItemInfo* pFind = GetStatItemInfo( pInfo, nItem );
	if ( NULL == pFind )
	{
		return;
	}

	StatItemBaseInfo& refStatItemInfoContent = pFind->bsInfoContent;
	if ( nValue < refStatItemInfoContent.nDiff)
	{
		return;
	}

	++pFind->nCount;

	//记录内容;
	NDUint64 nDiff = nValue - refStatItemInfoContent.nDiff;
	refStatItemInfoContent.nTotal += nDiff;
	
	if ( nDiff > refStatItemInfoContent.nMaxV )	refStatItemInfoContent.nMaxV = nDiff;
	if ( nDiff < refStatItemInfoContent.nMinV )	refStatItemInfoContent.nMinV = nDiff;

	//记录结束时间;
	StatItemBaseInfo& refStatItemInfoTime = pFind->bsInfoTime;
	nDiff = NDShareBaseGlobal::getMSTimeSinceStartBoot() - refStatItemInfoTime.nDiff;
	refStatItemInfoTime.nTotal += nDiff;

	if ( nDiff > refStatItemInfoTime.nMaxV )	refStatItemInfoTime.nMaxV = nDiff;
	if ( nDiff < refStatItemInfoTime.nMinV )	refStatItemInfoTime.nMinV = nDiff;
}

void StatData::Count( StatInfo* pInfo, NDUint32 nItem )
{
	StatItemInfo* pFind = GetStatItemInfo( pInfo, nItem );
	if ( NULL == pFind )
	{
		return;
	}

	++pFind->nCount;
}


NDUint64 StatData::GetTotal( StatInfo* pInfo, NDUint32 nItem )
{
	StatItemInfo* pFind = GetStatItemInfo( pInfo, nItem );
	if ( NULL == pFind )
	{
		return 0;
	}

	StatItemBaseInfo& refStatItemInfoContent = pFind->bsInfoContent;
	return refStatItemInfoContent.nTotal;
}

NDUint64 StatData::GetMax( StatInfo* pInfo, NDUint32 nItem )
{
	StatItemInfo* pFind = GetStatItemInfo( pInfo, nItem );
	if ( NULL == pFind )
	{
		return 0;
	}

	StatItemBaseInfo& refStatItemInfoContent = pFind->bsInfoContent;
	return refStatItemInfoContent.nMaxV;
}

NDUint64 StatData::GetMin( StatInfo* pInfo, NDUint32 nItem )
{
	StatItemInfo* pFind = GetStatItemInfo( pInfo, nItem );
	if ( NULL == pFind )
	{
		return 0;
	}

	StatItemBaseInfo& refStatItemInfoContent = pFind->bsInfoContent;
	return refStatItemInfoContent.nMinV;
}

float StatData::GetAvg( StatInfo* pInfo, NDUint32 nItem )
{
	StatItemInfo* pFind = GetStatItemInfo( pInfo, nItem );
	if ( NULL == pFind )
	{
		return 0;
	}

	StatItemBaseInfo& refStatItemInfoContent = pFind->bsInfoContent;

	return (float)( (double)refStatItemInfoContent.nTotal / (double) pFind->nCount );
}

void StatData::MakeArrayStat( stringstream& refStream, const char** pArrayAddr, NDUint32 nLen )
{
	if ( m_StatInfoMap.empty() )	return;

	refStream.str("");
	
	//make head;
	MakeStdStatHead( refStream );
	
	//body;
	StatInfoMap::iterator iter		= m_StatInfoMap.begin();
	StatInfoMap::iterator iterEnd	= m_StatInfoMap.end();
	for ( ; iter != iterEnd; ++iter )
	{
		StatInfo& refStatInfo	= iter->second;
		NDUint32& refTypeID		= refStatInfo.nTypeID;

		refStream << refTypeID << "\t";
		if ( refTypeID >= nLen )
		{
			refStream << "" << "\t";
		}
		else
		{
			refStream << pArrayAddr[refTypeID] << "\t";
		}
		MakeStatItemInfo( refStream, refStatInfo.itemInfoVec );
	}
}

void StatData::MakeMapStdStat( stringstream& refStream, map< NDUint32, string >& refStrMap )
{
	if ( m_StatInfoMap.empty() )	return;

	refStream.str("");

	//make head;
	MakeStdStatHead( refStream );

	//body;
	StatInfoMap::iterator iter		= m_StatInfoMap.begin();
	StatInfoMap::iterator iterEnd	= m_StatInfoMap.end();
	for ( ; iter != iterEnd; ++iter )
	{
		StatInfo& refStatInfo	= iter->second;
		NDUint32& refTypeID		= refStatInfo.nTypeID;

		refStream << refTypeID << "\t";

		map< NDUint32, string >::iterator iterNameFind = refStrMap.find( refTypeID );
		if ( iterNameFind == refStrMap.end() )
		{
			refStream << "" << "\t";
		}
		else
		{
			refStream << iterNameFind->second << "\t";
		}

		MakeStatItemInfo( refStream, refStatInfo.itemInfoVec );
	}
}


void StatData::MakeStatItemInfo( stringstream& refStream, StatItemInfoVec& refItemInfoVec )
{
	NDUint32 nSize = (NDUint32)refItemInfoVec.size();
	for ( NDUint32 i = 0; i < nSize; ++i )
	{
		StatItemInfo& refStatItemInfo		= refItemInfoVec[i];

		NDUint64 nCount						= refStatItemInfo.nCount;
		StatItemBaseInfo& refbsInfoContent	= refStatItemInfo.bsInfoContent;	
		StatItemBaseInfo& refbsInfoTime		= refStatItemInfo.bsInfoTime;		

		refStream << nCount						<< "\t";
		refStream << refbsInfoContent.nTotal	<< "\t";
		refStream << refbsInfoContent.nMinV		<< "\t";
		refStream << refbsInfoContent.nMaxV		<< "\t";
		refStream << (double)refbsInfoContent.nTotal / (double)nCount	<< "\t";
		refStream << refbsInfoTime.nTotal		<< "\t";
		refStream << refbsInfoTime.nMinV		<< "\t";
		refStream << refbsInfoTime.nMaxV		<< "\t";
		refStream << (double)refbsInfoTime.nTotal / (double)nCount		<< "\t";
	}
	refStream << "\n";
}

void StatData::SaveArrayTypeLog( const char* pFileName, const char** pArrayAddr, NDUint32 nLen )
{
	if ( NULL == pFileName || NULL == pArrayAddr || nLen <= 0 )
	{
		return;
	}

	stringstream resStream;
	MakeArrayStat( resStream, pArrayAddr, nLen );

	//创建文件;
	FILE *pFile = fopen( pFileName, "w+" );
	if ( NULL == pFile )
	{
		return;
	}

	string resStreamStr = resStream.str();
	fwrite( resStreamStr.c_str(), 1, resStreamStr.size(), pFile );
	fclose( pFile );
}

void StatData::SaveMapTypeLog( const char* pFileName, map< NDUint32, string >& refStrMap )
{
	if ( NULL == pFileName || refStrMap.size() <= 0 )
	{
		return;
	}

	stringstream resStream;
	MakeMapStdStat( resStream, refStrMap );

	//创建文件;
	FILE *pFile = fopen( pFileName, "w+" );
	if ( NULL == pFile )
	{
		return;
	}

	string resStreamStr = resStream.str();
	fwrite( resStreamStr.c_str(), 1, resStreamStr.size(), pFile );
	fclose( pFile );
}

StatData::StatInfo* StatData::GetStatInfo( NDUint32 nTypeID )
{
	StatInfoMap::iterator iterFind = m_StatInfoMap.find( nTypeID );
	if ( iterFind != m_StatInfoMap.end() )
	{
		return &(iterFind->second);
	}

	//create new statinfo;
	StatInfo statInfo;
	statInfo.nTypeID = nTypeID;

	StatItemInfoVec& refStatItemInfoVec = statInfo.itemInfoVec;
	refStatItemInfoVec.reserve( m_nItemSize );

	StatItemInfo statItemInfo;
	for ( NDUint32 i = 0; i < m_nItemSize; ++i )
	{
		refStatItemInfoVec.push_back( statItemInfo );
	}

	iterFind = m_StatInfoMap.lower_bound( nTypeID );
	iterFind = m_StatInfoMap.insert( iterFind, std::make_pair( nTypeID, statInfo ) );

	return &(iterFind->second) ;
}



StatData::StatItemInfo* StatData::GetStatItemInfo( StatInfo* pInfo, NDUint32 nItem )
{
	if ( NULL == pInfo || nItem >= m_nItemSize )
	{
		return NULL;
	}

	NDUint32 nItemInfoVecSize = (NDUint32)pInfo->itemInfoVec.size();
	if ( nItem >= nItemInfoVecSize )
	{
		return NULL;
	}

	return &( pInfo->itemInfoVec[nItem] );
}

StatData::StatItemInfo* StatData::GetStatItemInfo( NDUint32 nTypeID, NDUint32 nItem, NDBool bCount/*=NDFalse */ )
{
	StatInfo* pStatInfo = GetStatInfo( nTypeID );
	if ( NULL == pStatInfo )
	{
		return NULL;
	}

	StatData::StatItemInfo* pStatItemInfo = GetStatItemInfo( pStatInfo, nItem );
	if ( NULL == pStatItemInfo )
	{
		return NULL;
	}

	if ( bCount )
	{
		++(pStatItemInfo->nCount);
	}
	return pStatItemInfo;
}

void StatData::MakeStdStatHead( stringstream& refStream )
{
	refStream << "Index\tType\t";

	NDUint32  nSize = (NDUint32)m_itemsStringVec.size();
	for ( NDUint32 i = 0; i < nSize; ++i )
	{
		const string& refConstStr = m_itemsStringVec[i];
		refStream << "Count"	<< refConstStr << "\t";
		refStream << "Total"	<< refConstStr << "\t";
		refStream << "Min"		<< refConstStr << "\t";
		refStream << "Max"		<< refConstStr << "\t";
		refStream << "Avg"		<< refConstStr << "\t";
		refStream << "Total"	<< refConstStr << "Time"	<< "\t";
		refStream << "Min"		<< refConstStr << "Time"	<< "\t";
		refStream << "Max"		<< refConstStr << "Time"	<< "\t";
		refStream << "Avg"		<< refConstStr << "Time"	<< "\t";
	}
	
	refStream << "\n";
}




//////////////////////////////////////////////////////////////////////////////////

void NDStatWatcher::Record( NDUint32 nItem, NDUint64 nValue )
{
	if ( NULL == m_pStatData || NULL == m_pStatInfo )
	{
		return;
	}

	m_pStatData->RecordConst( m_pStatInfo, nItem, nValue );
}

void NDStatWatcher::BeginDiff( NDUint32 nItem, NDUint64 nValue )
{
	if ( NULL == m_pStatData || NULL == m_pStatInfo )
	{
		return;
	}

	m_pStatData->BeginDiff( m_pStatInfo, nItem, nValue );
}

void NDStatWatcher::EndDiff( NDUint32 nItem, NDUint64 nValue )
{
	if ( NULL == m_pStatData || NULL == m_pStatInfo )
	{
		return;
	}

	m_pStatData->EndDiff( m_pStatInfo, nItem, nValue );	
}

void NDStatWatcher::Begin( StatData* pData, NDUint32 nType )
{
	if ( NULL == pData)
	{
		return;
	}

	m_pStatInfo = pData->GetStatInfo( nType );

	BeginDiff( ST_PROTIME_INDEX, NDShareBaseGlobal::getMSTimeSinceStartBoot() );
}

void NDStatWatcher::End()
{
	EndDiff( ST_PROTIME_INDEX, NDShareBaseGlobal::getMSTimeSinceStartBoot() );
}



_NDSHAREBASE_END










