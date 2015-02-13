#include "function/NDDistributeID.h"

_NDSHAREBASE_BEGIN

NDDistributeID::NDDistributeID( NDUint32 nTotalSize /*= 10000 */ ) : m_nTotalSize( nTotalSize ), m_nCurPos( 0 ), m_nCount( 0 )
{
	m_idleDeque.clear();
}

NDDistributeID::~NDDistributeID()
{
	m_idleDeque.clear();
}

NDUint32 NDDistributeID::createID()
{
	if ( m_nCount < m_nTotalSize )
	{
		++m_nCount;
		if ( m_nCurPos < m_nTotalSize )
		{
			return (m_nCurPos++);
		}

		NDUint32 nRetValue = m_idleDeque.front();
		m_idleDeque.pop_front();
		return nRetValue;
	}

	//分配完时，换回最大值;
	return m_nTotalSize;
}

void NDDistributeID::releaseID( NDUint32 nIndex )
{
	if ( m_nCount > 0 )
	{
		--m_nCount;
		m_idleDeque.push_back( nIndex );
	}
}

void NDDistributeID::resetSize( NDUint32 nTotalSize )
{
	m_nTotalSize = nTotalSize;
}

NDUint32 NDDistributeID::getTotalSize() const
{
	return m_nTotalSize;
}

NDUint32 NDDistributeID::getAliveSize() const
{
	return m_nCount;
}

NDUint32 NDDistributeID::getIdleSize() const
{
	return (m_nTotalSize - m_nCount);
}


_NDSHAREBASE_END

