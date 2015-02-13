//============================================================
// File: 			 NDWorldServerManager.h	
// Creation Date:    2010-4-29   16:41
// Original Author:	 fanxiangdong(quire);
//
// Purpose:	         世界管理器;
// Modify:  
//============================================================
#ifndef __LOGINSERVER_ND_WORLD_SERVER_MANAGER_H__
#define __LOGINSERVER_ND_WORLD_SERVER_MANAGER_H__

#include "main/local/NDPreCompiler.h"

class NDWorldServerInfo;

class NDWorldServerManager : public NDRemoteServerManager
{	
public:
	NDWorldServerManager();
	~NDWorldServerManager();

	NDWorldServerInfo*		getWorldByWorldID( NDUint32 nWorldID );

	NDWorldServerInfo*		getWorldByWorldName( const char* pWorldName );

	//获取最低负载的世界;
	NDWorldServerInfo*		getBestWorld();

	NDBool					getAllWorldName( vector<string>& refWorldNameVec );

};

#endif

