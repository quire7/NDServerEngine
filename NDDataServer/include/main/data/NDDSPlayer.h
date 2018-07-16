#ifndef __DATASERVER_ND_DS_PALYER_H__
#define __DATASERVER_ND_DS_PALYER_H__


#include "main/local/NDPreCompiler.h"

class NDDSPlayer
{
private:

	NDUint8				m_playerState;									//DS上的玩家状态;

	NDUint32			m_nGSServerID;									//玩家在那个GS上;

	NDPlayerAccount		m_playerAccount;								//玩家账号表;
	NDPlayerMain		m_playerMain;									//玩家主表数据;

	NDBool				m_tableDirtyDataFlag[ETableType_MAX];			//判断表是否有数据要更新;
	NDTime				m_tableOperateTime[ETableType_MAX];				//每张表数据上次更新的时间;

public:
	NDDSPlayer();
	~NDDSPlayer();

	void				clear();

	NDBool				init();
	void				release();

	//设置和获得玩家在那个GS上;
	void				setGSServerID( NDUint32 nGSServerID )			{ m_nGSServerID = nGSServerID; }
	NDUint32			getGSServerID() const							{ return m_nGSServerID; }
	NDBool				sendProtocolToGS( NDProtocol& refProtocol );


	NDUint64			getPlayerGUID() const	{ return m_playerAccount.m_nPlayerGUID; };

	//玩家账号表数据;
	NDBool				insertUpdatePlayerAccount( const NDPlayerAccount& refPlayerAccount );
	//玩家主表数据;
	NDBool				insertUpdatePlayerMain( const NDPlayerMain& refPlayerMain );



	






	//发送玩家所有数据到GS;
	NDBool				sendAllPlayerDataToGS();

private:
	//发送玩家主表数据;
	NDBool				sendPlayerMainToGS();
};


#endif
