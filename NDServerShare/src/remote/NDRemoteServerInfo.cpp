#include "remote/NDRemoteServerInfo.h"

#include "local/NDServerManager.h"


NDRemoteServerInfo::NDRemoteServerInfo(void) : m_eServType(SERVER_MAX), m_nSessionID(0)
{
	memset( m_szRemoteName, 0, sizeof(m_szRemoteName) );
}

NDRemoteServerInfo::~NDRemoteServerInfo(void)
{
}

void NDRemoteServerInfo::setServerType( SERVERTYPE eServerType )
{
	m_eServType = eServerType;
}

SERVERTYPE NDRemoteServerInfo::getServerType() const
{
	return m_eServType;
}

void NDRemoteServerInfo::setSessionID( NDUint32 nSessionID )
{
	m_nSessionID = nSessionID;
}

NDUint32 NDRemoteServerInfo::getSessionID() const
{
	return m_nSessionID;
}

NDBool NDRemoteServerInfo::setServerName( const char* szName )
{
	if ( NULL == szName || '\0' == szName[0] )
	{
		return NDFalse;
	}

	NDUint32 nNameSize	= sizeof( m_szRemoteName );
	NDUint32 nLen		= (NDUint32)strlen( szName );
	if ( nLen >= nNameSize )
	{
		nLen = nNameSize - 1;
	}
	memcpy( m_szRemoteName, szName, nLen );
	m_szRemoteName[nLen] = '\0';

	return NDTrue;
}

const char* NDRemoteServerInfo::getServerName() const
{
	return m_szRemoteName;
}

void NDRemoteServerInfo::setLoadFactor( const NDLoadFactor& loadFactor )
{
	m_stLoadFactor = loadFactor;
}

const NDLoadFactor& NDRemoteServerInfo::getLoadFactor() const
{
	return m_stLoadFactor;
}

NDUint32 NDRemoteServerInfo::getPercentLoadFactor() const
{
	NDUint32 nMaxConn = m_stLoadFactor.getMaxConnected();

	if (nMaxConn == 0)	return 0;

	return (m_stLoadFactor.getCurConnected() * 100) / nMaxConn;
}

const NDSocketAddress& NDRemoteServerInfo::getNetAddress() const
{
	return m_stRemoteAddress;
}

void NDRemoteServerInfo::setNetAddress( const NDSocketAddress& netAddress )
{
	m_stRemoteAddress = netAddress;
}

NDBool NDRemoteServerInfo::send( NDProtocol& protocol, NDBool encrypt/*=NDFalse*/, NDBool compression/*=NDFalse*/, NDBool crc/*=NDFalse */ )
{
	return NDServerManager::getSingleton().sendToClient( protocol, m_nSessionID, encrypt, compression, crc );
}

