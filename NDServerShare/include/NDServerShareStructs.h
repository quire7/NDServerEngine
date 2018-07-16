#ifndef __SERVERSHARE_ND_SERVER_SHARE_STRUCTS_H__
#define __SERVERSHARE_ND_SERVER_SHARE_STRUCTS_H__


#include "NDTypes.h"
#include "NDShareBaseMacros.h"
#include "NDServerShareMacros.h"
#include "database/NDMysqlConst.h"

using NDShareBase::NDMysqlConnParam;

struct	NDPlayerAccount
{
	NDUint8				m_nOperType;												//���ݿ��������;
	NDUint8				m_nAccountFlag;												//Accout flag;

	NDUint32			m_nLoginCount;												//�ܵ�½����;
	NDUint32			m_nOnlineMinuteTimes;										//������ʱ��(����);

	NDUint64			m_nPlayerGUID;												//�˺�GUID;

	char				m_szPassWord[NDACCOUNT_TABLE_PASSWORD_LEN];					//PASSWORD;
	char				m_szRegTime[ND_TIME_LENGTH];								//ע��ʱ��;
	char				m_szLastLoginTime[ND_TIME_LENGTH];							//�ϴε�¼ʱ��;
	char				m_szAccount[NDACCOUNT_TABLE_ACCOUNT_LEN];					//�˺���;


	NDPlayerAccount(){ clear(); }
	void		clear() { memset( this, 0, sizeof(NDPlayerAccount) ); }

	NDBool		setAccount( const char* szAccount );
	NDBool		setPassWord( const char* szPassWord );
	NDBool		setRegTime( const char* szRegTime );
	NDBool		setLastLoginTime( const char* szLastTime );
};

//�����������;
struct	NDPlayerMain
{
	NDUint8				m_nOperType;												//���ݿ��������;
	
	NDUint32			m_nPlayerLevel;												//��ҵȼ�;
	NDUint32			m_nPlayerExp;												//��Ҿ���;
	NDUint32			m_nGold;													//���;
	NDUint32			m_nSilver;													//����;
	NDUint32			m_nCopper;													//ͭ��(����);
	NDUint64			m_nPlayerGUID;												//�ʺ�GUID;
	char				m_szPlayerName[NDPLAYERMAIN_TABLE_PLAYERNAME_LEN];			//�����;

	NDPlayerMain() { clear(); }
	void		clear()	{ memset( this, 0, sizeof(NDPlayerMain) ); }

	NDBool		setPlayerName( const char* szPlayerName );
};



struct NDDSConfigBaseInfo
{
	NDUint8				m_nSelectDBThread;					//select DBThread�߳���;
	NDUint8				m_nUpdateDBThread;					//update DBThread�߳���;
};


//DB�˺Ų�����Ϣ;
struct NDWSDBAccountInfo
{
	NDUint32			nDBConfigID;						//DB�������ļ��е�ID,ȫ������;
	NDUint32			nDBPlayerCounts;					//DB�ĵ�ǰ����;
	NDMysqlConnParam	mysqlConnParam;						//mysql���Ӳ���; 
};

#endif
