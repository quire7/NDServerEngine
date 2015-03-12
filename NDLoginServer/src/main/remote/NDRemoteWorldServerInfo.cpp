
#include "main/remote/NDRemoteWorldServerInfo.h"


NDRemoteWorldServerInfo::NDRemoteWorldServerInfo(): m_nWorldID(0)
{
	memset( m_szWorldName, 0, ND_WORLDNAME_LEN );
}

NDRemoteWorldServerInfo::~NDRemoteWorldServerInfo()
{

}

NDUint16 NDRemoteWorldServerInfo::getWorldID() const
{
	return m_nWorldID;
}

void NDRemoteWorldServerInfo::setWorldID( NDUint16 nWorldID )
{
	m_nWorldID = nWorldID;
}


const NDSocketAddress& NDRemoteWorldServerInfo::getBestGatewayAddr() const
{
	return m_stBestGate;
}

void NDRemoteWorldServerInfo::setBestGatewayAddr( const NDSocketAddress& gateAddress )
{
	m_stBestGate = gateAddress;
}

void NDRemoteWorldServerInfo::setBestGatewaySessionID( NDUint32 nSessionID )
{
	m_nBestGateSessionID = nSessionID;
}

NDUint32 NDRemoteWorldServerInfo::getBestGatewaySessionID() const
{
	return m_nBestGateSessionID;
}

void NDRemoteWorldServerInfo::setWorldName( const char* pWorldName )
{
	memcpy( m_szWorldName, pWorldName, ND_WORLDNAME_LEN );
}

const char* NDRemoteWorldServerInfo::getWorldName() const
{
	return m_szWorldName;
}

