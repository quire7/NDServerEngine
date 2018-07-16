#ifndef __GATEWAYSERVER_ND_PALYER_H__
#define __GATEWAYSERVER_ND_PALYER_H__

#include "main/local/NDPreCompiler.h"

class NDGTWSPlayer
{
private:
	NDUint8				m_nPlayerState;					//���״̬;
	NDServerInfo*		m_pGSServerInfo;				//���Ǹ�GameServerID��;
	NDServerInfo*		m_pRSServerInfo;				//���Ǹ�RoomServerID��;
	NDUint32			m_nClientSessionID;				//sessionID(��ͻ������ӵ�Session);
	NDUint64			m_nPlayerGUID;					//player GUID;

public:
	NDGTWSPlayer();
	~NDGTWSPlayer();

	void		clear();

	void		setPlayerState( NDUint8 nPlayerState )					{ m_nPlayerState = nPlayerState; }
	NDUint8		getPlayerState() const									{ return m_nPlayerState; }

	void		setClientSessionID( NDUint32 nSessionID )				{ m_nClientSessionID = nSessionID; }
	NDUint32	getClientSessionID() const								{ return m_nClientSessionID; }

	void		setPlayerGUID( NDUint64 nPlayerGUID )					{ m_nPlayerGUID = nPlayerGUID; }
	NDUint64	getPlayerGUID() const									{ return m_nPlayerGUID; }

	void		setGSServerInfo( const NDServerInfo* pGSServerInfo )	{ m_pGSServerInfo = (NDServerInfo*)pGSServerInfo; }
	NDUint32	getGSServerID() const;
	NDUint32	getGSSessionID() const;
	NDBool		sendProtocolToGS( NDProtocol& refProtocol );

	void		setRSServerInfo( const NDServerInfo* pRSServerInfo )	{ m_pRSServerInfo = (NDServerInfo*)pRSServerInfo; }
	NDUint32	getRSServerID() const;
	NDUint32	getRSSessionID() const;
	NDBool		sendProtocolToRS( NDProtocol& refProtocol );

	NDBool		sendProtocolToClient( NDProtocol& refProtocol );
};



#endif
