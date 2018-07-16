#ifndef __SERVERSHARE_ND_SERVER_SHARE_STRUCTS_H__
#define __SERVERSHARE_ND_SERVER_SHARE_STRUCTS_H__


#include "NDTypes.h"
#include "NDShareBaseMacros.h"
#include "NDServerShareMacros.h"
#include "database/NDMysqlConst.h"

using NDShareBase::NDMysqlConnParam;

struct	NDPlayerAccount
{
	NDUint8				m_nOperType;												//数据库操作类型;
	NDUint8				m_nAccountFlag;												//Accout flag;

	NDUint32			m_nLoginCount;												//总登陆次数;
	NDUint32			m_nOnlineMinuteTimes;										//总在线时长(分钟);

	NDUint64			m_nPlayerGUID;												//账号GUID;

	char				m_szPassWord[NDACCOUNT_TABLE_PASSWORD_LEN];					//PASSWORD;
	char				m_szRegTime[ND_TIME_LENGTH];								//注册时间;
	char				m_szLastLoginTime[ND_TIME_LENGTH];							//上次登录时间;
	char				m_szAccount[NDACCOUNT_TABLE_ACCOUNT_LEN];					//账号名;


	NDPlayerAccount(){ clear(); }
	void		clear() { memset( this, 0, sizeof(NDPlayerAccount) ); }

	NDBool		setAccount( const char* szAccount );
	NDBool		setPassWord( const char* szPassWord );
	NDBool		setRegTime( const char* szRegTime );
	NDBool		setLastLoginTime( const char* szLastTime );
};

//玩家主表数据;
struct	NDPlayerMain
{
	NDUint8				m_nOperType;												//数据库操作类型;
	
	NDUint32			m_nPlayerLevel;												//玩家等级;
	NDUint32			m_nPlayerExp;												//玩家经验;
	NDUint32			m_nGold;													//金币;
	NDUint32			m_nSilver;													//银币;
	NDUint32			m_nCopper;													//铜币(代币);
	NDUint64			m_nPlayerGUID;												//帐号GUID;
	char				m_szPlayerName[NDPLAYERMAIN_TABLE_PLAYERNAME_LEN];			//玩家名;

	NDPlayerMain() { clear(); }
	void		clear()	{ memset( this, 0, sizeof(NDPlayerMain) ); }

	NDBool		setPlayerName( const char* szPlayerName );
};



struct NDDSConfigBaseInfo
{
	NDUint8				m_nSelectDBThread;					//select DBThread线程数;
	NDUint8				m_nUpdateDBThread;					//update DBThread线程数;
};


//DB账号参数信息;
struct NDWSDBAccountInfo
{
	NDUint32			nDBConfigID;						//DB在配置文件中的ID,全服公用;
	NDUint32			nDBPlayerCounts;					//DB的当前人数;
	NDMysqlConnParam	mysqlConnParam;						//mysql连接参数; 
};

#endif
