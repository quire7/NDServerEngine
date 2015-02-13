#include "function/NDWordFilter.h"

#include "NDShareBaseGlobal.h"



_NDSHAREBASE_BEGIN

NDWordFilter::NDWordFilter()
{
	m_FilterWordRootMap.clear();
	m_FilterWordMap.clear();
}

NDWordFilter::~NDWordFilter()
{
	m_FilterWordRootMap.clear();
	m_FilterWordMap.clear();
}

NDBool NDWordFilter::AddFilterWordRoot( const char* pStr )
{
	if ( NULL == pStr || '\0' == pStr[0] )
	{
		return NDFalse;
	}

	NDUint32 nStrRealSize	= (NDUint32)strlen( pStr );
	NDUint32 nStrSize		= nStrRealSize;
	if ( nStrSize > ND_FilterWordRootStr_Max )
	{	//只取4个字符(4个字符组成一个整数);
		nStrSize = ND_FilterWordRootStr_Max;
	}

	NDUint32 nCystrSize = 0;
	FilterWordRoot	filterWordRoot;
	for ( NDUint32 i = 0; i < nStrSize; ++i )
	{
		FilterWordRootStrUnion &refBufUnion = filterWordRoot.bufUnion;
		//拷贝词根;
		nCystrSize = i + 1;
		memcpy( refBufUnion.szBuf, pStr, nCystrSize );
		
		//是否已经加入;
		FilterWordRootMapIter iterFind = m_FilterWordRootMap.find( refBufUnion.nNumber );
		if ( iterFind == m_FilterWordRootMap.end() )
		{
			filterWordRoot.nFindFlag |= EFilterWordRoot_Continue;
			if ( nCystrSize == nStrRealSize )
			{
				filterWordRoot.nFindFlag |= EFilterWordRoot_End;
			}

			m_FilterWordRootMap.insert( std::make_pair( refBufUnion.nNumber, filterWordRoot ) );

			memset( &filterWordRoot, 0, sizeof(filterWordRoot) );
		}
		else
		{
			FilterWordRoot &refRoot = iterFind->second;
			if ( (nCystrSize == nStrRealSize) && (EFilterWordRoot_End != (refRoot.nFindFlag & EFilterWordRoot_End)) )
			{
				refRoot.nFindFlag |= EFilterWordRoot_End;
			}
		}
	}

	return NDTrue;
}

NDBool NDWordFilter::DelFilterWordRoot( const char* pStr )
{
	if ( NULL == pStr || '\0' == pStr[0] )
	{
		return NDFalse;
	}

	NDUint32 nStrRealSize	= (NDUint32)strlen( pStr );
	NDUint32 nStrSize		= nStrRealSize;
	if ( nStrSize > ND_FilterWordRootStr_Max )
	{	//只取4个字符(4个字符组成一个整数);
		nStrSize = ND_FilterWordRootStr_Max;
	}

	NDUint32 nCystrSize = 0;
	FilterWordRoot	filterWordRoot;
	for ( NDUint32 i = 0; i < nStrSize; ++i )
	{
		FilterWordRootStrUnion &refBufUnion = filterWordRoot.bufUnion;
		//拷贝词根;
		nCystrSize = i + 1;
		memcpy( refBufUnion.szBuf, pStr, nCystrSize );

		//是否已经加入;
		FilterWordRootMapIter iterFind = m_FilterWordRootMap.find( refBufUnion.nNumber );
		if ( iterFind != m_FilterWordRootMap.end() )
		{
			FilterWordRoot &refRoot = iterFind->second;
			if ( (nCystrSize == nStrRealSize) && (EFilterWordRoot_End == (refRoot.nFindFlag & EFilterWordRoot_End)) )
			{
				NDInt8 nTemp = (~EFilterWordRoot_End);
				refRoot.nFindFlag &= nTemp;
			}
		}
	}

	return NDTrue;
}

NDInt8 NDWordFilter::IsHaveWholeFilterWordRoot( const char* pStr, FilterWordRootStrUnion& refUnion )
{
	if ( NULL == pStr || '\0' == pStr[0] )
	{
		return EFilterWordRoot_Invalid;
	}

	NDUint32 nStrSize = (NDUint32)strlen( pStr );
	if ( nStrSize > ND_FilterWordRootStr_Max )
	{
		return EFilterWordRoot_Invalid;
	}

	FilterWordRootStrUnion filterWordRootStrUnion;
	memset( &filterWordRootStrUnion, 0, sizeof(filterWordRootStrUnion) );
	memcpy( filterWordRootStrUnion.szBuf, pStr, nStrSize );

	FilterWordRootMapIter iterFind = m_FilterWordRootMap.find( filterWordRootStrUnion.nNumber );
	if ( iterFind != m_FilterWordRootMap.end() )
	{
		refUnion = iterFind->second.bufUnion;
		return iterFind->second.nFindFlag;
	}

	return EFilterWordRoot_Invalid;
}

NDBool NDWordFilter::IsHaveFilterWord( const char* pStr, NDBool bWhole/*=NDTrue */ )
{
	if ( NULL == pStr || '\0' == pStr[0] )
	{
		return NDTrue;
	}

	NDBool bFindWholeWord	= NDFalse;
	NDInt8	nFindFlag		= 0;
	char	szBufTemp[256]	= {0};

	FilterWordRootStrUnion filterStrUnion;

	NDUint32 nStrSize = (NDUint32)strlen( pStr );
	for ( NDUint32 i = 0; i < nStrSize; ++i )
	{
		bFindWholeWord = NDFalse;

		memset( szBufTemp, 0, sizeof(szBufTemp) );

		const char* pCurStr = &pStr[i];
		NDUint32 nCurStrSize= (NDUint32)strlen( pCurStr );
		for ( NDUint32 j = 0; j < nCurStrSize; ++j )
		{
			NDUint32 nszBufTempSize = j + 1;
			memcpy( szBufTemp, pCurStr, nszBufTempSize );

			//在词根中寻找;
			if ( j < ND_FilterWordRootStr_Max )
			{
				memset( &filterStrUnion, 0, sizeof(filterStrUnion) );

				nFindFlag = IsHaveWholeFilterWordRoot( szBufTemp, filterStrUnion );
				if ( nFindFlag == EFilterWordRoot_Invalid )
				{
					break;
				}
				if ( EFilterWordRoot_End == (nFindFlag & EFilterWordRoot_End) )
				{
					//已经找到;
					if ( !bWhole )
					{
						return NDTrue;
					}
					if ( 0 == strncmp( szBufTemp, filterStrUnion.szBuf, nszBufTempSize ) )
					{
						return NDTrue;
					}
				}
				if ( EFilterWordRoot_Continue == (nFindFlag & EFilterWordRoot_Continue) )
				{
					if ( nszBufTempSize == ND_FilterWordRootStr_Max )
					{	//继续在词中寻找;
						bFindWholeWord = NDTrue;
					}
				}

				continue;
			}

			if ( !bFindWholeWord )
			{	//在词根中都没找到,没有被设置到词中寻找,就不需要再循环了;
				break;
			}

			//在词中寻找;
			NDUint32 nCRC = NDShareBaseGlobal::bkdr_hash(szBufTemp);
			FilterWordMapIter iterFind = m_FilterWordMap.find( nCRC );
			if ( iterFind != m_FilterWordMap.end() )
			{
				if ( !bWhole )
				{
					return NDTrue;
				}

				string& refStr = iterFind->second;
				const char* pSecondStr = refStr.c_str();
				if ( 0 == strncmp( szBufTemp, pSecondStr, nszBufTempSize ) )
				{
					return NDTrue;
				}
			}
		}
	}

	return NDFalse;
}

NDBool NDWordFilter::IsHaveWholeFilterWord( const char* pStr )
{
	return IsHaveFilterWord( pStr, NDTrue );
}

NDBool NDWordFilter::IsHaveFuzzyFilterWord( const char* pStr )
{
	return IsHaveFilterWord( pStr, NDFalse );
}

NDBool NDWordFilter::AddFilterWord( const char* pStr )
{
	if ( NULL == pStr || '\0' == pStr[0] )
	{
		return NDFalse;
	}

	AddFilterWordRoot( pStr );

	NDUint32 nCRC = NDShareBaseGlobal::bkdr_hash(pStr);
	FilterWordMapIter iterFind = m_FilterWordMap.find( nCRC );
	if ( iterFind != m_FilterWordMap.end() )
	{
		return NDFalse;
	}

	m_FilterWordMap.insert( std::make_pair( nCRC, pStr ) );

	return NDTrue;
}

NDBool NDWordFilter::DelFilterWord( const char* pStr )
{
	if ( NULL == pStr || '\0' == pStr[0] )
	{
		return NDFalse;
	}

	DelFilterWordRoot( pStr );

	NDUint32 nCRC = NDShareBaseGlobal::bkdr_hash(pStr);
	FilterWordMapIter iterFind = m_FilterWordMap.find( nCRC );
	if ( iterFind != m_FilterWordMap.end() )
	{
		m_FilterWordMap.erase( iterFind );
	}

	return NDTrue;
}



_NDSHAREBASE_END

