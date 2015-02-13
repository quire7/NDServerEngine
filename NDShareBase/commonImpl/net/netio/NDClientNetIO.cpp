#include "net/netio/NDClientNetIO.h"

#include "NDShareBaseGlobal.h"

#include "net/session/NDClientSession.h"
#include "net/session/NDSessionManager.h"

#include "net/stream/NDProtocol.h"
#include "net/stream/NDProtocolPacket.h"


#include "net/process/NDServerTask.h"

#include "event/functor/NDSubFunctorSlot.h"
#include "event/timerEvent/NDTimerEventManager.h"


_NDSHAREBASE_BEGIN


const char* NDClientNetIO::s_szTimerPingProtocol = "timerPingProtocol";

NDClientNetIO::NDClientNetIO( void ) : m_pClientSession( NULL ), m_pPingProtocol( NULL )
{
}

NDClientNetIO::~NDClientNetIO( void )
{
	disConnect();

	if ( NULL != m_pPingProtocol )
	{
		delete m_pPingProtocol;
		m_pPingProtocol = NULL;
	}
}



void NDClientNetIO::setProcessor( NDProcess* pProcess )
{
	NDServerTask::getInstance()->setProcessor( pProcess );
}


NDBool NDClientNetIO::connect( const char* pszHostIP, NDUint16 nPort )
{
	//check param;
	if ( (NULL == pszHostIP || '\0' == pszHostIP[0]) )
	{
		return NDFalse;
	}

	if ( NULL == m_pPingProtocol )
	{
		return NDFalse;
	}

	if ( NULL != m_pClientSession )
	{
		disConnect();
	}

	//create client session and get it;
	m_pClientSession = NDSessionManager::getInstance()->createClientSession();
	if ( NULL == m_pClientSession )
	{
		return NDFalse;
	}


	//real Connect operator;
	if ( NDFalse == ((NDClientSession*)m_pClientSession)->connect( pszHostIP, nPort ) )
	{
		NDSessionManager::getInstance()->releaseClientSession( m_pClientSession->getSessionID() );
		m_pClientSession = NULL;
		return NDFalse;
	}

	NDSessionManager::getInstance()->addWorkClientSession( m_pClientSession );


	//NDSubFunctorSlot pingSubFunctorSlot(&NDClientNetIO::pingFunction, this);
	NDTimerEventArgs pingSubFunctorSlotArgs( NDClientNetIO::s_szTimerPingProtocol, ND_PING_MILLISECONDS,  NDShareBaseGlobal::getCurMSTimeOfUTC() + ND_PING_MILLISECONDS );
	m_timerPingConn = NDShareBase::NDTimerManager::getInstance()->addTimer( NDSubFunctorSlot(&NDClientNetIO::pingFunction, this), pingSubFunctorSlotArgs );

	return NDTrue;
}

NDBool NDClientNetIO::isConnected() const
{
	if ( NULL ==  m_pClientSession )	return NDFalse;

	return m_pClientSession->isConnected();
}

NDBool NDClientNetIO::sendToServer( NDProtocol& rProtocol, NDBool bCompression, NDBool bEncrypt, NDBool bCrc )
{
	if ( NULL ==  m_pClientSession ) 
	{
		return NDFalse;
	}

	if ( NDFalse == m_pClientSession->isConnected() )
	{
		disConnect();
		return NDFalse;
	}

	NDUint8 nProDataHeadBitWise = 0;
	if ( bCompression )	nProDataHeadBitWise |= ND_PDHMSG_COMPRESSION;
	if ( bEncrypt )		nProDataHeadBitWise |= ND_PDHMSG_ENCRYPT;
	if ( bCrc )			nProDataHeadBitWise |= ND_PDHMSG_CRC32;
	 
	return NDSessionManager::getInstance()->sendToServer( rProtocol, m_pClientSession->getSessionID(), nProDataHeadBitWise );
}

void NDClientNetIO::disConnect()
{
	if ( NULL != m_pClientSession )
	{
		if ( m_timerPingConn.isvaild() )
		{
			m_timerPingConn->disconnect();
		}

		m_pClientSession->handleClose();
		m_pClientSession = NULL;
	}
}


NDBool NDClientNetIO::pingFunction( const NDEventArgs& )
{
	if ( NULL ==  m_pClientSession || NULL == m_pPingProtocol ) 
	{
		return NDFalse;
	}

	return sendToServer( *m_pPingProtocol, NDFalse, NDFalse, NDFalse );
}

NDUint32 NDClientNetIO::getSessionID() const
{
	if ( NULL ==  m_pClientSession )
	{
		return (NDUint32)ND_INVALID_ID;
	}
	return m_pClientSession->getSessionID();
}

NDBool NDClientNetIO::setProtocolType( NDUint8 sessionProtocolType )
{
	if ( NULL == m_pClientSession )		return NDFalse;

	m_pClientSession->setProtocolType( sessionProtocolType );

	return NDTrue;
}

NDBool NDClientNetIO::setPingProtocol( NDProtocol* pPingProtocol )
{
	if ( NULL != m_pPingProtocol )
	{
		return NDFalse;
	}

	m_pPingProtocol		= pPingProtocol;
	
	return NDTrue;
}

_NDSHAREBASE_END


