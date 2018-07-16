#ifndef __DATASERVER_ND_DSDATA_STRUCTS_H__
#define __DATASERVER_ND_DSDATA_STRUCTS_H__

#include "NDTypes.h"

//等待获取数据的帐号;
struct NDDSWaitingAccount
{
	NDUint16		nGameServerID;
	NDUint64		nPlayerGUID;
};



#endif
