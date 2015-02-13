/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\stl\NDSTLExtend.h
	file base:	NDSTLExtend
	purpose:	stl extend;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_STL_EXTEND_H__
#define __SHARE_BASE_ND_STL_EXTEND_H__

#include <functional>
using std::unary_function;
using std::binary_function;

_NDSHAREBASE_BEGIN
////this foreach just run in LINUX.
//#ifndef	foreach	
//#define foreach(container,it) \
//	for( typeof((container).begin()) it = (container).begin(); it!=(container).end(); ++it )
//#endif

#ifndef	foreach	
#define foreach( container, it, itType )\
		for( itType it = (container).begin(), iterEnd = (container).end(); it != iterEnd; ++it )
#endif

//IsEqualUnary used, for example:
//struct Corps
//{
//public:
//	int		m_nMemNum;
//	int		m_nCorpsID;
//
//	Corps()	{memset( this, 0, sizeof(Corps) );}
//	int		GetCorpsID() const { return m_nCorpsID; };
//};
//typedef vector<Corps>		CorpsVec;
//typedef CorpsVec::iterator	CorpsVecIter;
//typedef vector<Corps*>				CorpsPtrVec;
//typedef CorpsPtrVec::iterator		CorpsPtrVecIter;
//
//int temp = 7;
//CorpsVec corpsVec;
//CorpsVecIter iterFind = find_if( corpsVec.begin(), corpsVec.end(), IsObjEqualUnary<int, Corps>( temp, &Corps::GetCorpsID ) );

template< typename TValue, typename TClass >
class IsObjEqualUnary : public unary_function< TClass, bool >
{
public:
	typedef TValue	(TClass::*PMemFun)() const ; //声明成public是为了构造函数中PMemFun类型能让外部知道是什么;
private:
	TValue		m_value;
	PMemFun		m_pPMemFun;

public:
	IsObjEqualUnary( TValue nValue, PMemFun pPMemFun ): m_value( nValue ), m_pPMemFun( pPMemFun ) {};

	bool	operator()( const TClass& refTC  ) const
	{
		return ( (refTC.*m_pPMemFun)() == m_value );
	}
};

//IsPtrEqualUnary used, for example:
//int temp = 3;
//CorpsPtrVec corpsPtrVec;
//CorpsPtrVecIter iterFind2 = find_if( corpsPtrVec.begin(), corpsPtrVec.end(), IsPtrEqualUnary<int, Corps>( temp, &Corps::GetCorpsID ) );

template< typename TValue, typename TClass >
class IsPtrEqualUnary : public unary_function< const TClass*, bool >
{
public:
	typedef TValue	(TClass::*PMemFun)() const ;
private:
	TValue		m_value;
	PMemFun		m_pPMemFun;

public:
	IsPtrEqualUnary( TValue nValue, PMemFun pPMemFun ): m_value( nValue ), m_pPMemFun( pPMemFun ) {};

	bool	operator()( const TClass* pTC  ) const
	{
		return ( (pTC->*m_pPMemFun)() == m_value );
	}
};


//PtrGreatCmp used, for example:
//std::sort_heap( corpsPtrVec.begin(), corpsPtrVec.end(), PtrGreatCmp<int, Corps>(&Corps::GetCorpsID) );

template< typename TValue, typename TClass >
class PtrGreatCmp : public binary_function< const TClass*, const TClass*, bool >
{
public:
	typedef TValue	(TClass::*PMemFun)() const ;
private:
	PMemFun		m_pPMemFun;

public:
	PtrGreatCmp( PMemFun pPMemFun ): m_pPMemFun( pPMemFun ) {};

	bool	operator()( const TClass* pLHS, const TClass* pRHS ) const
	{
		return ( (pLHS->*m_pPMemFun)() > (pRHS->*m_pPMemFun)() );
	}
};


//ObjGreatCmp used, for example:
//std::sort_heap( corpsVec.begin(), corpsVec.end(), ObjGreatCmp<int, Corps>(&Corps::GetCorpsID) );

template< typename TValue, typename TClass >
class ObjGreatCmp : public binary_function< TClass, TClass, bool >
{
public:
	typedef TValue	(TClass::*PMemFun)() const ;
private:
	PMemFun		m_pPMemFun;

public:
	ObjGreatCmp( PMemFun pPMemFun ): m_pPMemFun( pPMemFun ) {};

	bool	operator()( const TClass& refLHS, const TClass& refRHS ) const
	{
		return ( (refLHS.*m_pPMemFun)() > (refRHS.*m_pPMemFun)() );
	}
};


template< typename Iter, typename Diff >
void	safe_advance( Iter& refCur, const Iter& refEnd, Diff nDiff )
{
	if ( nDiff >= 0 )
	{
		size_t nRemaining( std::distance( refCur, refEnd ) );
		if ( nDiff > nRemaining )
		{
			nDiff = nRemaining;
		}
	}
	else
	{
		size_t nRemaining( std::distance( refEnd, refCur ) );
		if ( -nDiff > nRemaining )
		{
			nDiff = -nRemaining;
		}
	}

	std::advance( refCur, nDiff );
}









_NDSHAREBASE_END
#endif

