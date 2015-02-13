#ifndef __PROTOCOL_ND_PROTOCOL_COMMON_ENUMS_H__
#define __PROTOCOL_ND_PROTOCOL_COMMON_ENUMS_H__

enum CLIENTTYPE
{
	GAME_CLIENT			= 0x00000000,
};

//����������;
enum SERVERTYPE
{
	LOGIN_SERVER		= 0x00100000,
	GATEWAY_SERVER		= 0x00200000,
	WORLD_SERVER		= 0x00300000,
	GAME_SERVER			= 0x00400000,
	ROOM_SERVER			= 0x00500000,
	MSG_SERVER			= 0x00600000,
	DB_SERVER			= 0x00700000,
	LOG_SERVER			= 0x00800000,

	MANAGE_SERVER		= 0x00F00000,
	SERVER_MAX			= 0x7FFFFFFF,	//(0x7FFFFFFF��int�������2147483647);
};

////�ͻ��˺ͷ����Э��ID�ٽ�ֵ;
//enum csCriticalCMDProtocolID
//{
//	CMDP_NDC2LS_Start		=	LOGIN_SERVER			+ 0x1000,
//	CMDP_NDC2LS_End			=	LOGIN_SERVER			+ 0x100F,
//	CMDP_NDC2GTWS_Start		=	GATEWAY_SERVER			+ 0x1000,
//	CMDP_NDC2GTWS_End		=	GATEWAY_SERVER			+ 0x2000,
//	CMDP_NDLS2C_Start		=	GAME_CLIENT				+ 0x10,
//	CMDP_NDLS2C_End			=	GAME_CLIENT				+ 0x1F,
//	CMDP_NDGTWS2C_Start		=	GAME_CLIENT				+ 0x20,
//	CMDP_NDGTWS2C_End		=	GAME_CLIENT				+ 0xFFF,
//};

enum eSpecialCMDProtocolID
{
	CMDP_PING				= (MANAGE_SERVER + 1),
	CMDP_DISCONNECT_NOTIFY,
};



enum NDSessionProtocolType
{
	NDSessionProtocolType_C2LS,				//�ͻ��˵�������LS����;
	NDSessionProtocolType_C2GTWS,			//�ͻ��˵�������GTWS����;
	NDSessionProtocolType_WS2LS,			//������WS��������LS����;
	NDSessionProtocolType_RS2WS,			//������RS��������WS����;
	NDSessionProtocolType_GTWS2WS,			//������GTWS��������WS����;
	NDSessionProtocolType_GTWS2GS,			//������GTWS��������GS����;
	NDSessionProtocolType_GTWS2RS,			//������GTWS��������RS����;
	NDSessionProtocolType_GTWS2MS,			//������GTWS��������MS����;
	NDSessionProtocolType_GS2DS,			//������GS��������DS����;
	NDSessionProtocolType_GS2WS,			//������GS��������WS����;
	NDSessionProtocolType_GS2RS,			//������GS��������RS����;
	
	
	NDSessionProtocolType_DS2GS,			//������DS��������GS����;
	NDSessionProtocolType_WS2GTWS,			//������WS��������GTWS����;
	NDSessionProtocolType_WS2GS,			//������WS��������GS����;
	NDSessionProtocolType_WS2RS,			//������WS��������RS����;
	NDSessionProtocolType_RS2GS,			//������RS��������GS����;
	NDSessionProtocolType_RS2GTWS,			//������RS��������GTWS����;
	NDSessionProtocolType_GS2GTWS,			//������GS��������GTWS����;
	NDSessionProtocolType_MS2GTWS,			//������MS��������GTWS����;
	NDSessionProtocolType_GTWS2C,			//������GTWS���ͻ�������;
	NDSessionProtocolType_LS2WS,			//������LS��������WS����;
	NDSessionProtocolType_LS2C,				//������LS���ͻ�������;

	NDSessionProtocolType_MAX,
};

enum ND_SERVER_RET_SUCCESSENUM
{
	eND_SRS_OK							= 0X00,							//����ɹ�;
};

enum ND_SERVER_RET_ERRORENUM
{
	eND_SRE_LOGIC_ERROR					= 0X01,							//�����������߼�����;
};

#endif
