//============================================================
// File: 			 NDCmdProtocolS2S.h	
// Creation Date:    2010-9-6
// Original Author:	 quire
//
// Purpose:			 ����������ص�Э�������ֶ���;
// Modify:  
//============================================================
#ifndef __SERVERSHARE_ND_CMD_PROTOCOL_S2S_H__
#define __SERVERSHARE_ND_CMD_PROTOCOL_S2S_H__

#include "NDProtocolBase.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	�����ֶ������:;
 *	const NDUint32 m_unProtocolID = value;
 *	valueȡֵΪЭ�����������Ŀ������ (SERVERTYPE) + n.;
 *	nȡֵ��Χ:0~0x00000FFF.;
 *	 ����淶:	��16���Ʒ�ʽ������0~0xF����;
 *				0x10~0x7FF��������Э����;
 *				0x800���������ڷ�������ͻ��˽���;
 *				ÿ��Э��(req,res,[nty])����n��ֵ����ʹ����ֵͬ��ʼ;(�������Э����nty��Ϣ����˳��+1);
*/
////////////////////////////////////////////////////////////////////////////////////////////////////

enum eToLoginServerCMDProtocolID
{
	////NDWorldServer��NDLoginServer���͵�Э���;
	CMDP_NDWS2LS_Start								=	LOGIN_SERVER		+ 0x010,
	CMDP_NDWS2LS_Register_Req,						//WS��LS����ע������;
	CMDP_NDWS2LS_LoadFactor_Nty,					//WS��LS����GTWS��LoadFactor;

	CMDP_NDWS2LS_End								=	LOGIN_SERVER		+ 0x020,
};

enum eToGateWayServerCMDProtocolID
{
	//NDGameServer��NDGatewayServer���͵�Э���;
	CMDP_NDGS2GTWS_Start							=	GATEWAY_SERVER		+ 0x010,
	CMDP_NDGWS2GS_Register_Res,
	CMDP_NDGS2GTWS_PlayerLoginDataSuccess_Nty,
	CMDP_NDGS2GTWS_End								=	GATEWAY_SERVER		+ 0x300,

	//NDRoomServer��NDGatewayServer���͵�Э���;
	CMDP_NDRS2GTWS_Start							=	GATEWAY_SERVER		+ 0x301,
	CMDP_NDGWS2RS_Register_Res,
	CMDP_NDRS2GTWS_End								=	GATEWAY_SERVER		+ 0x500,
};

enum eToWorldServerCMDProtocolID
{
	//NDLoginServer��NDWorldServer���͵�Э���;
	CMDP_NDLS2WS_Start								=	WORLD_SERVER		+ 0x010,
	CMDP_NDLS2WS_Register_Res,						//WS��LS����ע�����󷵻�;
	CMDP_NDLS2WS_End								=	WORLD_SERVER		+ 0x020,

	//NDGameServer��NDWorldServer���͵�Э���;
	CMDP_NDGS2WS_Start								=	WORLD_SERVER		+ 0x021,
	CMDP_NDGS2WS_Register_Req,
	CMDP_NDGS2WS_End								=	WORLD_SERVER		+ 0x090,

	//NDRoomServer��NDWorldServer���͵�Э���;
	CMDP_NDRS2WS_Start								=	WORLD_SERVER		+ 0x091,
	CMDP_NDRS2WS_Register_Req,
	CMDP_NDRS2WS_End								=	WORLD_SERVER		+ 0x0C0,
	
};

enum eToGameServerCMDProtocolID
{
	//NDWorldServer��NDGameServer���͵�Э���;
	CMDP_NDWS2GS_Start								=	GAME_SERVER			+ 0x010,
	CMDP_NDGS2WS_Register_Res,						
	CMDP_NDWS2GS_End								=	GAME_SERVER			+ 0x020,

	//NDDataServer��NDGameServer���͵�Э���;
	CMDP_NDDS2GS_Start								=	GAME_SERVER			+ 0x021,
	CMDP_NDGS2DS_Register_Res,
	CMDP_NDGS2DS_PlayerLogin_Res,
	CMDP_NDDS2GS_SendPlayerMain_Nty,
	CMDP_NDDS2GS_End								=	GAME_SERVER			+ 0x090,
	
	//NDRoomServer��NDGameServer���͵�Э���;
	CMDP_NDRS2GS_Start								=	GAME_SERVER			+ 0x091,
	CMDP_NDRS2GS_Register_Req,					
	CMDP_NDRS2GS_End								=	GAME_SERVER			+ 0x0C0,

	//NDGatewayServer��NDGameServer���͵�Э���;
	CMDP_NDGWS2GS_Start								=	GAME_SERVER			+ 0x0C1,
	CMDP_NDGWS2GS_Register_Req,
	CMDP_NDGWS2GS_End								=	GAME_SERVER			+ 0x300,
};


enum eToRoomServerCMDProtocolID
{
	//NDWorldServer��NDRoomServer���͵�Э���;
	CMDP_NDWS2RS_Start								=	ROOM_SERVER			+ 0x010,
	CMDP_NDRS2WS_Register_Res,						
	CMDP_NDWS2RS_End								=	ROOM_SERVER			+ 0x020,

	//NDGameServer��NDRoomServer���͵�Э���;
	CMDP_NDGS2RS_Start								=	ROOM_SERVER			+ 0x021,
	CMDP_NDRS2GS_Register_Res,						
	CMDP_NDGS2RS_End								=	ROOM_SERVER			+ 0x090,
	
	//NDGatewayServer��NDRoomServer���͵�Э���;
	CMDP_NDGWS2RS_Start								=	ROOM_SERVER			+ 0x091,
	CMDP_NDGWS2RS_Register_Req,						
	CMDP_NDGWS2RS_End								=	ROOM_SERVER			+ 0x200,
	
};

enum eToDBServerCMDProtocolID
{
	//NDWorldServer��NDDataServer���͵�Э���(�б�Ҫ��WSȥ����DS��);
	CMDP_NDWS2DB_Start								=	DB_SERVER			+ 0x010,
	CMDP_NDWS2DB_End								=	DB_SERVER			+ 0x020,

	//NDGameServer��NDDataServer���͵�Э���;
	CMDP_NDGS2DS_Start								=	DB_SERVER			+ 0x021,
	CMDP_NDGS2DS_Register_Req,
	CMDP_NDGS2DS_PlayerLogin_Req,
	CMDP_NDGS2DS_UpdatePlayerMain_Nty,
	CMDP_NDGS2DS_End								=	DB_SERVER			+ 0x090,
	
};




#endif
