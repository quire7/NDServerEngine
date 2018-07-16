#ifndef __DATASERVER_ND_WSPALYER_MANAGER_H__
#define __DATASERVER_ND_WSPALYER_MANAGER_H__

#include "main/local/NDPreCompiler.h"

#include "thread/NDMutex.h"

#include "main/data/NDWSPlayer.h"

using NDShareBase::NDMutexLock;
using NDShareBase::NDGuardLock;

class NDWSPlayerManager
{
private:
	typedef	map<NDUint64, NDWSPlayer*>			NDWSPlayerMap;
	typedef	NDWSPlayerMap::iterator				NDWSPlayerMapIter;

	NDMutexLock									m_mutexLock;
	NDWSPlayerMap								m_NDWSPlayerMap;

public:
	NDWSPlayerManager();
	~NDWSPlayerManager();

	void						clear();

	NDBool						init();
	void						release();

	//添加角色;
	NDWSPlayer*					addPlayer( const NDPlayerMain& refPlayerMain );
	//删除角色;
	void						removePlayer( NDUint64 nPlayerGUID );

	//通过角色ID获得角色;
	NDWSPlayer*					getPlayerByGUID( NDUint64 nPlayerGUID );
};

#endif
