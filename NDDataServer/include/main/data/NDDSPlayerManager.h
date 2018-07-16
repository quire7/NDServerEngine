#ifndef __DATASERVER_ND_DSPALYER_MANAGER_H__
#define __DATASERVER_ND_DSPALYER_MANAGER_H__

#include "main/local/NDPreCompiler.h"

#include "thread/NDMutex.h"

#include "main/data/NDDSPlayer.h"

using NDShareBase::NDMutexLock;
using NDShareBase::NDGuardLock;

class NDDSPlayerManager
{
private:
	typedef	map<NDUint64, NDDSPlayer*>			NDDSPlayerMap;
	typedef	NDDSPlayerMap::iterator				NDDSPlayerMapIter;

	NDUint32									m_nOnlineNum;
	NDMutexLock									m_mutexLock;
	NDDSPlayerMap								m_NDDSPlayerMap;
public:
	NDDSPlayerManager();
	~NDDSPlayerManager();

	NDBool						init();
	void						release();

	//添加角色;
	NDDSPlayer*					addPlayer( const NDPlayerAccount& refPlayerAccount );
	//删除角色;
	void						removePlayer( NDUint64 nPlayerGUID );

	//通过角色ID获得角色;
	NDDSPlayer*					getPlayerByGUID( NDUint64 nPlayerGUID );

	//增加在线人数;
	void						increaseOnlineNum();
	//减少在线人数;
	void						decreaseOnlineNum();
	//获得在线人数;
	NDUint32					getOnlineNum() const	{ return m_nOnlineNum; };
};

#endif

