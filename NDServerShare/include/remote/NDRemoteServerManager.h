#ifndef __SERVER_SHARE_ND_REMOTE_SERVER_MANAGER_H__
#define __SERVER_SHARE_ND_REMOTE_SERVER_MANAGER_H__

#include <map>
using std::map;

#include "NDTypes.h"

class NDRemoteServerInfo;
class NDRemoteServerManager
{
protected:
	typedef	map<NDUint32, NDRemoteServerInfo*>		RemoteServerMap;
	typedef RemoteServerMap::iterator				RemoteServerMapIter;
public:
	NDRemoteServerManager(void);
	virtual ~NDRemoteServerManager(void);

	NDBool	addRemoteServer( NDRemoteServerInfo* pRemoteServerInfo );
	NDBool	removeRemoteServer( NDUint32 nSessionID );

	NDRemoteServerInfo* getRemoteServerInfo( NDUint32 nSessionID );

protected:
	RemoteServerMap		m_remoteServerMap;
};



#endif


