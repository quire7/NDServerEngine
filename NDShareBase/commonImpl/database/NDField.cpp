#include "database/NDField.h"

#include "NDCLibFun.h"


_NDSHAREBASE_BEGIN

NDField::NDField() : m_pValue( NULL )
{
}

NDField::~NDField()
{
}

void NDField::setValue( char* pValue )
{
	m_pValue = pValue;
}

const char* NDField::getString()
{
	return m_pValue;
}

NDInt8 NDField::getInt8()
{
	return m_pValue ? static_cast<NDInt8>( atoi(m_pValue) ) : 0;
}

NDUint8 NDField::getUint8()
{
	return m_pValue ? static_cast<NDUint8>( atoi(m_pValue) ) : 0;
}

NDInt16 NDField::getInt16()
{
	return m_pValue ? static_cast<NDInt16>( atoi(m_pValue) ) : 0;
}

NDUint16 NDField::getUint16()
{
	return m_pValue ? static_cast<NDUint16>( atoi(m_pValue) ) : 0;
}

NDInt32 NDField::getInt32()
{
	return m_pValue ? static_cast<NDInt32>( atol(m_pValue) ) : 0;
}

NDUint32 NDField::getUint32()
{
	return m_pValue ? static_cast<NDUint32>( atol(m_pValue) ) : 0;
}

NDInt64 NDField::getInt64()
{
#if ND_BYTES_PER_LONG == 8
	return m_pValue ? static_cast<NDUint64>( atol(m_pValue) ) : 0;
#elif defined(WIN16) || defined(WIN32)
	return m_pValue ? static_cast<NDUint64>( _atoi64(m_pValue) ) : 0;
#else
	NDUint64 value;
	ND_SSCANF( m_pValue, "%lld", &value );
	return value;
#endif
}

NDUint64 NDField::getUint64()
{
#if ND_BYTES_PER_LONG == 8
	return m_pValue ? static_cast<NDUint64>( atol(m_pValue) ) : 0;
#elif defined(WIN16) || defined(WIN32)
	return m_pValue ? static_cast<NDUint64>( _atoi64(m_pValue) ) : 0;
#else
	NDUint64 value;
	ND_SSCANF( m_pValue, "%llu", &value );
	return value;
#endif
}

NDFloat32 NDField::getFloat()
{
	return m_pValue ? static_cast<NDFloat32>( atof(m_pValue) ) : 0;
}

NDFloat64 NDField::getDouble()
{
	return m_pValue ? static_cast<NDFloat64>( atof(m_pValue) ) : 0;
}

NDBool NDField::getBool()
{
	if ( NULL == m_pValue )
	{
		return NDFalse;
	}
	
	return ( ( atoi(m_pValue) > 0 ) ? NDTrue : NDFalse );
}

_NDSHAREBASE_END
