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
 *	const int command = value;
 *	valueȡֵΪЭ�����������Ŀ������ (SERVERTYPE) + n.;
 *	nȡֵ��Χ:0~0x000FFFFF.;
 *	 ����淶:	��16���Ʒ�ʽ������0~0xF����;
 *				0x10~0xFFF��������Э����;
 *				0x1000���������ڷ�������ͻ��˽���;
 *				ÿ��Э��(req,res,[nty])����n��ֵ����ʹ����ֵͬ��ʼ;(�������Э����nty��Ϣ����˳��+1);
*/
////////////////////////////////////////////////////////////////////////////////////////////////////

enum eToLoginServerCMDProtocolID
{
	CMDP_NDWS2LS_Start								=	LOGIN_SERVER		+ 0x010,
	CMDP_NDWS2LS_Register_Req,						//WS��LS����ע������;
	CMDP_NDWS2LS_LoadFactor_Nty,					//WS��LS����GTWS��LoadFactor;

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
	CMDP_NDLS2WS_Register_Res,						//WS��LS����ע�����󷵻�;

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
