#ifndef __SERVERSHARE_ND_SERVER_SHARE_ENUMS_H__
#define __SERVERSHARE_ND_SERVER_SHARE_ENUMS_H__

//数据库表类型;
enum ETableType
{
	ETableType_Account,
	ETableType_PlayerMain,
	ETableType_MAX,
};

//account table field index;
enum NDPlayerMainTableField
{
	NDPlayerMainTableField_PlayerGUID			=	0x00,
	NDPlayerMainTableField_PlayerName			=	0x01,
	NDPlayerMainTableField_PlayerLevel			=	0x02,
	NDPlayerMainTableField_PlayerExp			=	0x03,
	NDPlayerMainTableField_Gold					=	0x04,
	NDPlayerMainTableField_Silver				=	0x05,
	NDPlayerMainTableField_Copper				=	0x06,
};


#endif
