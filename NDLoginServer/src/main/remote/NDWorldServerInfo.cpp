
#include "main/remote/NDWorldServerInfo.h"


NDWorldServerInfo::NDWorldServerInfo(): m_nWorldID(0)
{
	memset( m_szWorldName, 0, ND_WORLDNAME_LEN );
}

NDWorldServerInfo::~NDWorldServerInfo()
{

}

NDUint16 NDWorldServerInfo::getWorldID() const
{
	return m_nWorldID;
}

void NDWorldServerInfo::setWorldID( NDUint16 nWorldID )
{
	m_nWorldID = nWorldID;
}


const NDSocketAddress& NDWorldServerInfo::getBestGate() const
{
	return m_stBestGate;
}

void NDWorldServerInfo::setBestGate( const NDSocketAddress& gateAddress )
{
	m_stBestGate = gateAddress;
}

void NDWorldServerInfo::setBestGateSessionID( NDUint32 nSessionID )
{
	m_nBestGateSessionID = nSessionID;
}

NDUint32 NDWorldServerInfo::getBestGateSessionID() const
{
	return m_nBestGateSessionID;
}

void NDWorldServerInfo::setWorldName( const char* pWorldName )
{
	memcpy( m_szWorldName, pWorldName, ND_WORLDNAME_LEN );
}

const char* NDWorldServerInfo::getWorldName() const
{
	return m_szWorldName;
}

