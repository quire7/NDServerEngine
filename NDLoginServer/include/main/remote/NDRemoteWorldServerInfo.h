//============================================================
// File: 			 NDRemoteWorldServerInfo.h	
// Creation Date:    2010-4-29   16:25
// Original Author:	 fanxiangdong(quire);
//
// Purpose:	         ������,�յ�һ��NDWorldServer��ע��ʱ,����һ��NDRemoteWorldServerInfo��ʵ��;
// Modify:  
//============================================================
#ifndef __LOGINSERVER_ND_REMOTE_WORLD_SERVER_INFO_H__
#define __LOGINSERVER_ND_REMOTE_WORLD_SERVER_INFO_H__

#include "main/local/NDPreCompiler.h"

class NDRemoteWorldServerInfo : public NDRemoteServerInfo
{
private:
	NDUint16					m_nWorldID;
	char						m_szWorldName[ND_WORLDNAME_LEN];
	NDUint32					m_nBestGateSessionID;
	NDSocketAddress				m_stBestGate;

public:
	NDRemoteWorldServerInfo();
	~NDRemoteWorldServerInfo();

	void						setWorldName( const char* pWorldName );
	const char*					getWorldName() const;

	NDUint16					getWorldID() const;
	void						setWorldID( NDUint16 nWorldID );

	const NDSocketAddress&		getBestGatewayAddr() const;
	void						setBestGatewayAddr( const NDSocketAddress& gateAddress );

	//for center server to find NDRemoteServerInfo(NDGatewayServer Info).
	void						setBestGatewaySessionID( NDUint32 nSessionID );
	NDUint32					getBestGatewaySessionID() const;

};


#endif

