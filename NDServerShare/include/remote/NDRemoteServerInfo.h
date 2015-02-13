#ifndef __SERVER_SHARE_ND_REMOTE_SERVER_INFO_H__
#define __SERVER_SHARE_ND_REMOTE_SERVER_INFO_H__

#include "local/NDServerInfo.h"

class NDProtocol;
class NDRemoteServerInfo
{
public:
	NDRemoteServerInfo(void);
	virtual ~NDRemoteServerInfo(void);

	void				setServerType( SERVERTYPE eServerType );
	SERVERTYPE			getServerType() const;

	void				setSessionID( NDUint32 nSessionID );
	NDUint32			getSessionID() const;

	NDBool				setServerName( const char* szName );
	const char*			getServerName() const;	

	void				setLoadFactor( const NDLoadFactor& loadFactor );
	const NDLoadFactor& getLoadFactor() const;

	NDUint32			getPercentLoadFactor() const;

	const NDSocketAddress&	getNetAddress() const;
	void				setNetAddress(const NDSocketAddress& netAddress);

	NDBool				send( NDProtocol& protocol, NDBool encrypt=NDFalse, NDBool compression=NDFalse, NDBool crc=NDFalse );

private:

	SERVERTYPE		m_eServType;					// Server type
	NDUint32		m_nSessionID;					// Session ID
	char			m_szRemoteName[MAX_SERVER_NAME];// Remote server name
	NDLoadFactor	m_stLoadFactor;					// Load balance factor
	NDSocketAddress	m_stRemoteAddress;				// Remote address

};



#endif
