#ifndef __PROTOCOL_ND_PROTOCOL_COMMON_ENUMS_H__
#define __PROTOCOL_ND_PROTOCOL_COMMON_ENUMS_H__

enum CLIENTTYPE
{
	GAME_CLIENT			= 0x00000000,
};

//����������;
enum SERVERTYPE
{
	LOGIN_SERVER		= 0x00001000,
	GATEWAY_SERVER		= 0x00002000,
	WORLD_SERVER		= 0x00003000,
	GAME_SERVER			= 0x00004000,
	ROOM_SERVER			= 0x00005000,
	MSG_SERVER			= 0x00006000,
	DB_SERVER			= 0x00007000,
	LOG_SERVER			= 0x00008000,

	MANAGE_SERVER		= 0x0000F000,
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
	CMDP_Special_Start = (MANAGE_SERVER + 1),
	CMDP_PING_Req,			
	CMDP_PING_Res,
	CMDP_DISCONNECT_NOTIFY,
	CMDP_Special_End,
};



enum NDSessionProtocolType
{
	NDSessionProtocolType_C2LS,				//�ͻ��˵�������LS����;
	NDSessionProtocolType_C2GTWS,			//�ͻ��˵�������GTWS����;
	NDSessionProtocolType_WS2LS,			//������WS��������LS����;
	NDSessionProtocolType_RS2WS,			//������RS��������WS����;
	NDSessionProtocolType_RS2GS,			//������RS��������GS����;
	NDSessionProtocolType_GTWS2GS,			//������GTWS��������GS����;
	NDSessionProtocolType_GTWS2RS,			//������GTWS��������RS����;
	NDSessionProtocolType_GTWS2MSGS,		//������GTWS��������MSGS����;
	NDSessionProtocolType_GS2DS,			//������GS��������DS����;
	NDSessionProtocolType_GS2WS,			//������GS��������WS����;
	
	
	
	NDSessionProtocolType_DS2GS,			//������DS��������GS����;
	NDSessionProtocolType_WS2GS,			//������WS��������GS����;
	NDSessionProtocolType_WS2RS,			//������WS��������RS����;
	NDSessionProtocolType_GS2RS,			//������GS��������RS����;
	NDSessionProtocolType_RS2GTWS,			//������RS��������GTWS����;
	NDSessionProtocolType_GS2GTWS,			//������GS��������GTWS����;
	NDSessionProtocolType_MSGS2GTWS,		//������MSGS��������GTWS����;
	NDSessionProtocolType_LS2WS,			//������LS��������WS����;
	NDSessionProtocolType_LS2C,				//������LS���ͻ�������;
	NDSessionProtocolType_GTWS2C,			//������GTWS���ͻ�������;

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
