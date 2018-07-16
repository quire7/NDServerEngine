#ifndef __GAMESERVER_ND_GS_PALYER_MANAGER_H__
#define __GAMESERVER_ND_GS_PALYER_MANAGER_H__

#include "main/local/NDPreCompiler.h"


class NDGSPlayer;
class NDGSPlayerManager
{
private:
	typedef map<NDUint64, NDGSPlayer*>		NDGSPlayerMap;
	typedef NDGSPlayerMap::iterator			NDGSPlayerMapIter;
	
	NDServerInfo*							m_pWSServerInfo;			//WS连接信息;
	NDGSPlayerMap							m_NDGSPlayerMap;

public:
	NDGSPlayerManager();
	~NDGSPlayerManager();

	void					clear();
	void					release();

	NDBool					init();


	NDBool					addPlayer( NDGSPlayer* pPlayer );
	NDBool					removePlayer( NDGSPlayer* pPlayer );
	
	NDGSPlayer*				getPlayer( NDUint64 nPlayerGUID );

	//设置和获得WS连接信息;
	void					setWSServerInfo( NDServerInfo* pWSServerInfo )		{ m_pWSServerInfo = pWSServerInfo; }
	NDUint32				getWSServerID() const;
	NDUint32				getWSServerSessionID() const;
	NDBool					sendProtocolToWS( NDProtocol& refProtocol );		//发送协议到WS的统一接口;
};



#endif
