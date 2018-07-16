#ifndef __GATEWAYSERVER_ND_PALYER_MANAGER_H__
#define __GATEWAYSERVER_ND_PALYER_MANAGER_H__

#include "main/local/NDPreCompiler.h"

#include <deque>
using std::deque;

class NDGTWSPlayer;
class NDGTWSPlayerManager
{
private:
	typedef	deque<NDGTWSPlayer*>			PlayerDeque;
	typedef PlayerDeque::iterator			PlayerDequeIter;
	typedef map<NDUint64, NDGTWSPlayer*>	PlayerMap;
	typedef PlayerMap::iterator				PlayerMapIter;

	PlayerDeque								m_PlayerDeque;		//用于生成空闲PLAYER的队列;
	PlayerMap								m_PlayerMap;		//player map;

public:
	NDGTWSPlayerManager();
	~NDGTWSPlayerManager();

	void					clear();
	void					release();

	NDGTWSPlayer*			getIdlePlayer();

	NDBool					addPlayer( NDGTWSPlayer* pPlayer );
	NDBool					removePlayerByPlayerGUID( NDUint64 nPlayerGUID );
	NDBool					removePlayerBySessionID( NDUint32 nSessionID );

	NDGTWSPlayer*			getPlayerByPlayerGUID( NDUint64 nPlayerGUID );
	NDGTWSPlayer*			getPlayerBySessionID( NDUint32 nSessionID );
};



#endif
