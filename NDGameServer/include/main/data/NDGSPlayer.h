#ifndef __GAMESERVER_ND_GS_PALYER_H__
#define __GAMESERVER_ND_GS_PALYER_H__

#include "main/local/NDPreCompiler.h"

class NDServerInfo;
class NDGSPlayer
{
private:
	char						m_szSessionKey[ND_SHA1_LEN];					//client登录验证码(WS生成);

	NDBool						m_bRecvAllDSPlayerData;							//接受DS发送过来的数据是否完整;
	NDServerInfo*				m_pDSServerInfo;								//玩家分配在那个DS上;
	NDUint32					m_nGTWSServerID;								//玩家分配在那个GatewayServer上;

	NDPlayerMain				m_NDPlayerMain;									//玩家主表数据;

public:
	NDGSPlayer();
	~NDGSPlayer();

	void						clear();
	void						release();

	NDBool						init();

	//玩家主表信息-start;
	void						setPlayerMain( const NDPlayerMain& refData );
	
	NDBool						setPlayerName( const char* szPlayerName )					{ return m_NDPlayerMain.setPlayerName(szPlayerName); }
	const char*					getPlayerName() const										{ return m_NDPlayerMain.m_szPlayerName; }

	void						setPlayerGUID( NDUint64 nPlayerGUID )						{ m_NDPlayerMain.m_nPlayerGUID = nPlayerGUID; }
	NDUint64					getPlayerGUID() const										{ return m_NDPlayerMain.m_nPlayerGUID; }
	
	//玩家主表信息-end;

	//设置和获得玩家在那个GTWS上;
	void						setGTWSServerID( NDUint32 nServerID )						{ m_nGTWSServerID = nServerID; }
	NDUint32					getGTWSServerID() const										{ return m_nGTWSServerID; }

	//设置和获得从DS接收的玩家数据是否完整;
	void						setRecvAllDSPlayerData( NDBool bOK )						{ m_bRecvAllDSPlayerData = bOK; }
	NDBool						getRecvAllDSPlayerData() const								{ return m_bRecvAllDSPlayerData; }
	//设置和获取DS信息;
	void						setDSServerInfo( const NDServerInfo* pDSServerInfo )		{ m_pDSServerInfo = (NDServerInfo*)pDSServerInfo; }
	NDUint32					getDSServerID() const;
	NDUint32					getDSSessionID() const;

	//更新数据到DS;
	NDBool						sendProtocolToDS( NDProtocol& refProtocol );				//向DS发送消息接口;
	NDBool						sendPlayerLoginToDS();										//通知DS登录,请求数据;
	NDBool						sendPlayerMainToDS();										//更新玩家主表数据到DS;


	//WS;
	NDBool						sendPlayerLoginToWS();										//发送登录信息到WS;


	//发送数据到GTWS(也就是要发送到客户端);
	NDBool						sendProtocolToGTWS( NDProtocol& refProtocol );				//发送消息到GTWS上的接口;
	NDBool						sendPlayerLoginDataSuccessToGTWS();							//通知GTWS,GS上数据完毕,客户端可以登录;
};


#endif
