
#include "main/data/NDGTWSPlayer.h"

NDGTWSPlayer::NDGTWSPlayer()
{
	clear();
}


NDGTWSPlayer::~NDGTWSPlayer()
{
	clear();
}

void NDGTWSPlayer::clear()
{
	m_nPlayerState		= 0;
	m_pGSServerInfo		= NULL;
	m_pRSServerInfo		= NULL;
	m_nClientSessionID	= ND_INVALID_ID;
	m_nPlayerGUID		= 0;
}

NDUint32 NDGTWSPlayer::getGSServerID() const
{
	if ( NULL == m_pGSServerInfo )
	{
		return ND_INVALID_ID;
	}

	return m_pGSServerInfo->getServerID();
}

NDUint32 NDGTWSPlayer::getGSSessionID() const
{
	if ( NULL == m_pGSServerInfo )
	{
		return ND_INVALID_ID;
	}

	return m_pGSServerInfo->getSessionID();
}

NDUint32 NDGTWSPlayer::getRSServerID() const
{
	if ( NULL == m_pRSServerInfo )
	{
		return ND_INVALID_ID;
	}

	return m_pRSServerInfo->getServerID();
}

NDUint32 NDGTWSPlayer::getRSSessionID() const
{
	if ( NULL == m_pRSServerInfo )
	{
		return ND_INVALID_ID;
	}

	return m_pRSServerInfo->getSessionID();
}

NDBool NDGTWSPlayer::sendProtocolToGS(NDProtocol& refProtocol)
{
	NDUint32 nGSSessionID = getGSSessionID();
	if ( ND_INVALID_ID == nGSSessionID )
	{
		return NDFalse;
	}

	return NDServerManager::getSingleton().sendToServer( refProtocol, nGSSessionID );
}

NDBool NDGTWSPlayer::sendProtocolToRS(NDProtocol& refProtocol)
{
	NDUint32 nRSSessionID = getRSSessionID();
	if ( ND_INVALID_ID == nRSSessionID )
	{
		return NDFalse;
	}

	return NDServerManager::getSingleton().sendToServer( refProtocol, nRSSessionID );
}

NDBool NDGTWSPlayer::sendProtocolToClient(NDProtocol& refProtocol)
{
	if ( ND_INVALID_ID == m_nClientSessionID )
	{
		return NDFalse;
	}

	return NDServerManager::getSingleton().sendToClient( refProtocol, m_nClientSessionID );
}

