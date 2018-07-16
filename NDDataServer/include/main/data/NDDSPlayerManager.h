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

	//��ӽ�ɫ;
	NDDSPlayer*					addPlayer( const NDPlayerAccount& refPlayerAccount );
	//ɾ����ɫ;
	void						removePlayer( NDUint64 nPlayerGUID );

	//ͨ����ɫID��ý�ɫ;
	NDDSPlayer*					getPlayerByGUID( NDUint64 nPlayerGUID );

	//������������;
	void						increaseOnlineNum();
	//������������;
	void						decreaseOnlineNum();
	//�����������;
	NDUint32					getOnlineNum() const	{ return m_nOnlineNum; };
};

#endif

