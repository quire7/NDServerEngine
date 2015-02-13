#include "math/NDRectangle.h"

_NDSHAREBASE_BEGIN

NDRectangle::NDRectangle( NDUint32 nWidth, NDUint32 nHeight ) : m_nWidth(nWidth), m_nHeight(nHeight)
{
}

NDRectangle::NDRectangle() : m_nWidth(0), m_nHeight(0)
{
}

NDRectangle::NDRectangle( const NDRectangle& other ) : m_nWidth(other.m_nWidth), m_nHeight(other.m_nHeight)
{
}

NDRectangle& NDRectangle::operator=( const NDRectangle& other )
{
	if ( this == &other )
	{
		return *this;
	}

	m_nWidth	= other.m_nWidth;
	m_nHeight	= other.m_nHeight;

	return *this;
}

bool NDRectangle::operator>( const NDRectangle& other ) const
{
	return ( (m_nWidth > other.m_nWidth) && (m_nHeight > other.m_nHeight) );
}

bool NDRectangle::operator<( const NDRectangle& other ) const
{
	return ( (m_nWidth < other.m_nWidth) && (m_nHeight < other.m_nHeight) );
}

bool NDRectangle::operator==( const NDRectangle& other ) const
{
	return ( (m_nWidth == other.m_nWidth) && (m_nHeight == other.m_nHeight) );
}

void NDRectangle::setWidth( NDUint32 nWidth )
{
	m_nWidth = nWidth;
}

void NDRectangle::setHeight( NDUint32 nHeight )
{
	m_nHeight = nHeight;
}

void NDRectangle::setWidthAndHeight( NDUint32 nWidth, NDUint32 nHeight )
{
	m_nWidth	= nWidth;
	m_nHeight	= nHeight;
}

NDUint32 NDRectangle::getWidth() const
{
	return m_nWidth;
}

NDUint32 NDRectangle::getHeight() const
{
	return m_nHeight;
}

const NDUint32& NDRectangle::getConstRefWidth() const
{
	return m_nWidth;
}

const NDUint32& NDRectangle::getConstRefHeight() const
{
	return m_nHeight;
}


_NDSHAREBASE_END
