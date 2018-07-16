#ifndef __WORLDSERVER_ND_WS_PLAYER_H__
#define __WORLDSERVER_ND_WS_PLAYER_H__

#include "main/local/NDPreCompiler.h"


class NDWSPlayer
{
private:
	NDPlayerMain		m_playerMain;									//玩家主表数据;
public:
	NDWSPlayer();
	~NDWSPlayer();

	void				clear();

	NDBool				init( const NDPlayerMain& refPlayerMain );
	void				release();

	NDUint64			getPlayerGUID() const	{ return m_playerMain.m_nPlayerGUID; };

};


#endif
