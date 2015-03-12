//============================================================
// File: 			 NDRemoteWorldServerManager.h	
// Creation Date:    2010-4-29   16:41
// Original Author:	 fanxiangdong(quire);
//
// Purpose:	         ���������;
// Modify:  
//============================================================
#ifndef __LOGINSERVER_ND_WORLD_SERVER_MANAGER_H__
#define __LOGINSERVER_ND_WORLD_SERVER_MANAGER_H__

#include "main/local/NDPreCompiler.h"

class NDRemoteWorldServerInfo;

class NDRemoteWorldServerManager : public NDRemoteServerManager
{	
public:
	NDRemoteWorldServerManager();
	~NDRemoteWorldServerManager();

	NDRemoteWorldServerInfo*		getWorldByWorldID( NDUint32 nWorldID );

	NDRemoteWorldServerInfo*		getWorldByWorldName( const char* pWorldName );

	//��ȡ��͸��ص�����;
	NDRemoteWorldServerInfo*		getBestWorld();

	NDBool							getAllWorldName( vector<string>& refWorldNameVec );

};

#endif

