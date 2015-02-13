#include "local/NDServerInfo.h"

#include "NDShareBaseGlobal.h"

using NDShareBase::NDShareBaseGlobal;

//////////////////////////////////////////////////////////////////////////

NDServerInfo::NDServerInfo(void) : m_netAddress(), m_byServType(SERVER_MAX), m_eConnState(eConnState_Null),
								  m_eLocalRemote(eLR_Null), m_nSessionID((NDUint32)ND_INVALID_ID), m_nServerID((NDUint32)ND_INVALID_ID)
{
	memset( m_szServName, 0, sizeof( m_szServName ) );
}

NDServerInfo::~NDServerInfo(void)
{
}

NDBool NDServerInfo::setServerName( const char* szName )
{
	NDUint32 nStrLen = 0;
	if ( NDFalse == NDShareBaseGlobal::isValidStrParam( szName, nStrLen ) )
	{
		return NDFalse;
	}

	return ( nStrLen == NDShareBaseGlobal::safeStrCopy( m_szServName, szName, MAX_PATH_LEN, nStrLen ) );
}

const char* NDServerInfo::getServerName() const
{
	return m_szServName;
}

void NDServerInfo::setNetAddress( const NDSocketAddress& netAddress )
{
	m_netAddress = netAddress;
}

const NDSocketAddress& NDServerInfo::getNetAddress() const
{
	return m_netAddress;
}

NDBool NDServerInfo::setServerIP( const char* szIP )
{
	return m_netAddress.setIP( szIP );
}

const char* NDServerInfo::getServerIP() const
{
	return m_netAddress.getIP();
}

void NDServerInfo::setServerPort( NDUint16 usPort )
{
	m_netAddress.setPort( usPort );
}

NDUint16 NDServerInfo::getServerPort() const
{
	return m_netAddress.getPort();
}

void NDServerInfo::setServerType( SERVERTYPE byServerType )
{
	m_byServType = byServerType;
}

SERVERTYPE NDServerInfo::getServerType() const
{
	return m_byServType;
}

void NDServerInfo::setSessionID( NDUint32 nSessionID )
{
	m_nSessionID = nSessionID;
}

NDUint32 NDServerInfo::getSessionID() const
{
	return m_nSessionID;
}

NDServerInfo::eConnState_Types NDServerInfo::getConnState() const
{
	return m_eConnState;	
}

void NDServerInfo::setConnState( eConnState_Types eState )
{
	m_eConnState = eState;
}

NDServerInfo::eLocalRemote NDServerInfo::getLocalRemote() const
{
	return m_eLocalRemote;
}

void NDServerInfo::setLocalRemote( eLocalRemote eLR )
{
	m_eLocalRemote = eLR;
}

void NDServerInfo::setServerID( NDUint32 nServerID )
{
	m_nServerID = nServerID;
}

NDUint32 NDServerInfo::getServerID() const
{
	return m_nServerID;
}








////////////////////////////////////////////////////////////////////////////////

NDLocalServerInfo::NDLocalServerInfo() : NDServerInfo()
{
	memset( m_szDumpPath, 0 , sizeof( m_szDumpPath ) );
	memset( m_szLogPath, 0 , sizeof( m_szLogPath ) );
}

NDLocalServerInfo::~NDLocalServerInfo()
{

}

NDBool NDLocalServerInfo::setDumpPath( const char* szDumpPath )
{
	NDUint32 nStrLen = 0;
	if ( NDFalse == NDShareBaseGlobal::isValidStrParam( szDumpPath, nStrLen ) )
	{
		return NDFalse;
	}

	return ( nStrLen == NDShareBaseGlobal::safeStrCopy( m_szDumpPath, szDumpPath, MAX_PATH_LEN, nStrLen ) );
}

NDBool NDLocalServerInfo::setLogPath( const char* szLogPath )
{
	NDUint32 nStrLen = 0;
	if ( NDFalse == NDShareBaseGlobal::isValidStrParam( szLogPath, nStrLen ) )
	{
		return NDFalse;
	}

	return ( nStrLen == NDShareBaseGlobal::safeStrCopy( m_szLogPath, szLogPath, MAX_PATH_LEN, nStrLen ) );
}

const char* NDLocalServerInfo::getDumpPath() const
{
	return m_szDumpPath;
}

const char* NDLocalServerInfo::getLogPath() const
{
	return m_szLogPath;
}
