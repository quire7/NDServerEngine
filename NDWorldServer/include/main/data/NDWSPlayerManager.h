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

	//��ӽ�ɫ;
	NDWSPlayer*					addPlayer( const NDPlayerMain& refPlayerMain );
	//ɾ����ɫ;
	void						removePlayer( NDUint64 nPlayerGUID );

	//ͨ����ɫID��ý�ɫ;
	NDWSPlayer*					getPlayerByGUID( NDUint64 nPlayerGUID );
};

#endif
