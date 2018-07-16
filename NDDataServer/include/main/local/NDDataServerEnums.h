#ifndef __DATASERVER_ND_DATASERVER_ENUMS_H__
#define __DATASERVER_ND_DATASERVER_ENUMS_H__

//DataServer DBThreadµÄÆðÊ¼ID;
enum EDataServerDBThreadID
{
	EDataServerDBThreadID_SELECT_START		= 0x01,
	EDataServerDBThreadID_SELECT_END		= 0x10,
	EDataServerDBThreadID_UPDATE_START		= 0x11,
	EDataServerDBThreadID_UPDATE_END		= 0x20,		
};


//account table field index;
enum NDPlayerAccountTableField
{
	NDPlayerAccountTableField_PlayerGUID		=	0x00,
	NDPlayerAccountTableField_Account			=	0x01,
	NDPlayerAccountTableField_PassWord			=	0x02,
	NDPlayerAccountTableField_LoginCount		=	0x03,
	NDPlayerAccountTableField_OnlineTimes		=	0x04,
	NDPlayerAccountTableField_RegTime			=	0x05,
	NDPlayerAccountTableField_LastLoginTime		=	0x06,
};



#endif
