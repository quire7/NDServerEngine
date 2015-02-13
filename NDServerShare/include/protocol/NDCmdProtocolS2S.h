//============================================================
// File: 			 NDCmdProtocolS2S.h	
// Creation Date:    2010-9-6
// Original Author:	 quire
//
// Purpose:			 服务器间相关的协义命令字定义;
// Modify:  
//============================================================
#ifndef __SERVERSHARE_ND_CMD_PROTOCOL_S2S_H__
#define __SERVERSHARE_ND_CMD_PROTOCOL_S2S_H__

#include "NDProtocolBase.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	命令字定义规则:;
 *	const int command = value;
 *	value取值为协义流向的最终目标类型 (SERVERTYPE) + n.;
 *	n取值范围:0~0x000FFFFF.;
 *	 定义规范:	以16进制方式递增，0~0xF保留;
 *				0x10~0xFFF服务器间协议字;
 *				0x1000及以上用于服务器与客户端交互;
 *				每组协议(req,res,[nty])建议n的值尽量使用相同值开始;(如果该组协议有nty消息，则顺序+1);
*/
////////////////////////////////////////////////////////////////////////////////////////////////////

enum eToLoginServerCMDProtocolID
{
	CMDP_NDWS2LS_Start								=	LOGIN_SERVER		+ 0x010,
	CMDP_NDWS2LS_Register_Req,						//WS向LS发送注册请求;
	CMDP_NDWS2LS_LoadFactor_Nty,					//WS向LS发送GTWS的LoadFactor;

	CMDP_NDWS2LS_End								=	LOGIN_SERVER		+ 0x020,
};

enum eToGateWayServerCMDProtocolID
{
	CMDP_NDWS2GTWS_Start							=	GATEWAY_SERVER		+ 0x010,
	CMDP_NDWS2GTWS_End								=	GATEWAY_SERVER		+ 0x020,
};

enum eToWorldServerCMDProtocolID
{
	CMDP_NDLS2WS_Start								=	WORLD_SERVER		+ 0x010,
	CMDP_NDLS2WS_Register_Res,						//WS向LS发送注册请求返回;

	CMDP_NDLS2WS_End								=	WORLD_SERVER		+ 0x020,
};

enum eToGameServerCMDProtocolID
{
	CMDP_NDWS2GS_Start								=	GAME_SERVER			+ 0x010,
	CMDP_NDWS2GS_End								=	GAME_SERVER			+ 0x020,
};

enum eToDBServerCMDProtocolID
{
	CMDP_NDWS2DB_Start								=	DB_SERVER			+ 0x010,
	CMDP_NDWS2DB_End								=	DB_SERVER			+ 0x020,
};




#endif
