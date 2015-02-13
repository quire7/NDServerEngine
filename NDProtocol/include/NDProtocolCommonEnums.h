#ifndef __PROTOCOL_ND_PROTOCOL_COMMON_ENUMS_H__
#define __PROTOCOL_ND_PROTOCOL_COMMON_ENUMS_H__

enum CLIENTTYPE
{
	GAME_CLIENT			= 0x00000000,
};

//服务器类型;
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
	SERVER_MAX			= 0x7FFFFFFF,	//(0x7FFFFFFF是int最大正数2147483647);
};

////客户端和服务端协议ID临界值;
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
	NDSessionProtocolType_C2LS,				//客户端到服务器LS类型;
	NDSessionProtocolType_C2GTWS,			//客户端到服务器GTWS类型;
	NDSessionProtocolType_WS2LS,			//服务器WS到服务器LS类型;
	NDSessionProtocolType_RS2WS,			//服务器RS到服务器WS类型;
	NDSessionProtocolType_GTWS2WS,			//服务器GTWS到服务器WS类型;
	NDSessionProtocolType_GTWS2GS,			//服务器GTWS到服务器GS类型;
	NDSessionProtocolType_GTWS2RS,			//服务器GTWS到服务器RS类型;
	NDSessionProtocolType_GTWS2MS,			//服务器GTWS到服务器MS类型;
	NDSessionProtocolType_GS2DS,			//服务器GS到服务器DS类型;
	NDSessionProtocolType_GS2WS,			//服务器GS到服务器WS类型;
	NDSessionProtocolType_GS2RS,			//服务器GS到服务器RS类型;
	
	
	NDSessionProtocolType_DS2GS,			//服务器DS到服务器GS类型;
	NDSessionProtocolType_WS2GTWS,			//服务器WS到服务器GTWS类型;
	NDSessionProtocolType_WS2GS,			//服务器WS到服务器GS类型;
	NDSessionProtocolType_WS2RS,			//服务器WS到服务器RS类型;
	NDSessionProtocolType_RS2GS,			//服务器RS到服务器GS类型;
	NDSessionProtocolType_RS2GTWS,			//服务器RS到服务器GTWS类型;
	NDSessionProtocolType_GS2GTWS,			//服务器GS到服务器GTWS类型;
	NDSessionProtocolType_MS2GTWS,			//服务器MS到服务器GTWS类型;
	NDSessionProtocolType_GTWS2C,			//服务器GTWS到客户端类型;
	NDSessionProtocolType_LS2WS,			//服务器LS到服务器WS类型;
	NDSessionProtocolType_LS2C,				//服务器LS到客户端类型;

	NDSessionProtocolType_MAX,
};

enum ND_SERVER_RET_SUCCESSENUM
{
	eND_SRS_OK							= 0X00,							//处理成功;
};

enum ND_SERVER_RET_ERRORENUM
{
	eND_SRE_LOGIC_ERROR					= 0X01,							//服务器处理逻辑错误;
};

#endif
