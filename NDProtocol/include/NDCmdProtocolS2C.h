#ifndef __PROTOCOL_ND_CMD_PROTOCOL_S2C_H__
#define __PROTOCOL_ND_CMD_PROTOCOL_S2C_H__

//�������ͻ��˷��͵�Э��ID;
enum eS2CCMDProtocolID
{
	CMDP_NDLS2C_Start		=	GAME_CLIENT				+ 0x10,

	CMDP_NDC2LS_Login_Res	=	GAME_CLIENT				+ 0x11,

	CMDP_NDLS2C_End			=	GAME_CLIENT				+ 0x1F,

	CMDP_NDGTWS2C_Start		=	GAME_CLIENT				+ 0x20,

	CMDP_NDGTWS2C_End		=	GAME_CLIENT				+ 0xFFF,
};


#endif
