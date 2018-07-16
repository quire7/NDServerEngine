#ifndef __SERVER_SHARE_ND_SERVER_MANAGER_H__
#define __SERVER_SHARE_ND_SERVER_MANAGER_H__

#include <vector>
using std::vector;
#include <map>
using std::map;

#include "NDTypes.h"
#include "net/socket/NDSocketAddress.h"
#include "function/NDSingleton.h"

#include "NDProtocolCommonEnums.h"

namespace NDShareBase
{
	class NDMutexLock;
	class NDProtocol;
	class NDServerNetIO;
	class NDClientNetIO;

}

using NDShareBase::NDSocketAddress;
using NDShareBase::NDSingleton;
using NDShareBase::NDMutexLock;
using NDShareBase::NDProtocol;
using NDShareBase::NDServerNetIO;
using NDShareBase::NDClientNetIO;


class NDMutexLock;
class NDProtocol;
class NDServerNetIO;
class NDClientNetIO;

class NDServerInfo;
class NDLocalServerInfo;
class NDConnectProcess;
class NDLocalServer;
class NDRemoteServerManager;

class NDServerManager : public NDSingleton<NDServerManager>
{
private:

	typedef vector<NDServerInfo*>			ConnServerTable;
	typedef ConnServerTable::iterator		ConnServerTableIter;

	typedef map<NDUint32, NDClientNetIO*>	ConnServerInstance;
	typedef ConnServerInstance::iterator	ConnServerInstanceIter;

private:
	NDLocalServer*				m_pListenServer;		// 侦听服务器实例;

	NDServerNetIO*				m_pListenNetIO;			// 服务器用于侦听的netio;

	NDMutexLock*				m_pCSTMutex;			// m_connServerTable mutex;

	NDMutexLock*				m_pCSIMutex;			// m_connServerInstance mutex;

	ConnServerTable				m_connServerTable;		// 连接的目标Server列表;

	ConnServerInstance			m_connServerInstance;	// 主动连接的netio实例(NDClientNetIO Instance);

	NDUint32					m_nMainProcMilliSeconds;// main主循环中每多少毫秒执行一次事件;

public:
	NDServerManager();
	~NDServerManager(void);

	//启动侦听服务器;
	NDBool					startServer( NDLocalServer* pLocalServer, NDBool bConnOtherServer=NDTrue );
	NDBool					mainLoop();

	NDBool					getClientAddress( NDUint32 nSessionID, NDSocketAddress& refClientAddress );
	NDBool					disConnectClient( NDUint32 nSessionID );

	//设置Session的协议类型;
	NDBool					setServerSessionProtocolType( NDUint32 nSessionID, NDUint8 nSessionProtocolType );
	NDBool					setClientSessionProtocolType( NDUint32 nSessionID, NDUint8 nSessionProtocolType );

	//send protocol to client(real client);
	NDBool					sendToClient( NDProtocol& protocol, NDUint32 nSessionID, NDBool bCompression=NDFalse, NDBool bEncrypt=NDFalse, NDBool bCrc=NDFalse );

	//处理客户端的PING协议;
	NDBool					pingProtocolCommonDispose( NDUint32 nSessionID );
	//处理客户端的断开连接的协议(服务器内部POP的断开连接的协议);
	NDBool					disconnectNotifyCommonDispose( NDUint32 nSessionID, NDUint8 nDisconnectionType, NDRemoteServerManager* pRemoteServerManager );
	//处理客户端的断开连接的协议的错误处理;
	void					disconnectNotifyCommonErrorDispose( NDUint32 nSessionID, NDUint8 nDisconnectionType, const char* szServerName );
	//处理服务器返回客户端的PING_RES协议;
	NDBool					pingResProtocolCommonDispose( NDUint32 nSessionID );
	//处理服务器内部注册的返回处理;
	NDBool					registerResCommonDispose( NDUint32 nSessionID, NDUint32 nResCode );

	////////////////////////////////////////////////////////////////////////////////////////////////
	/* connect server dispose start */

	// get connect server Info;
	const NDServerInfo*		getConnServerInfo( NDUint32 nSessionID );
	const NDServerInfo*		getConnServerInfo( SERVERTYPE servType, NDUint32 nServerID );

	// register remote server(need to connect server);
	void					registerConnServer( NDServerInfo* pServerInfo );

	// connect config file server;
	NDBool					connectServer( NDConnectProcess* pConnecter );
	// connect singleton server;
	NDBool					connectServer( NDServerInfo* pServerInfo, NDConnectProcess* pConnecter );

	//检测所有链接其他服务器链接的状态;
	NDBool					checkAllConnectServerState();
	//重新链接断开其他服务器的链接;
	void					reconnectServer( NDConnectProcess* pConnecter );

	void					disConnectServer();
	
	// send protocol to connectd server;
	NDBool					sendToServer( NDProtocol& protocol, SERVERTYPE _servType, NDBool bCompression=NDFalse, NDBool bEncrypt=NDFalse, NDBool bCrc=NDFalse );
	NDBool					sendToServer( NDProtocol& protocol, NDUint32 nSessionID, NDBool bCompression=NDFalse, NDBool bEncrypt=NDFalse, NDBool bCrc=NDFalse );

	/* connect server dispose end */
	////////////////////////////////////////////////////////////////////////////////////////////////

private:

	NDBool					connectServerDisponse( NDServerInfo* pServerInfo,  NDConnectProcess* pConnecter );

	NDBool					removeConnServer( NDUint32 nSessionID );

	//设置已经连接的其他服务器的链接设置为DISCONNECTED状态;
	NDBool					setConnServerDisconnectState( NDUint32 nSessionID );

	NDClientNetIO*			removeConnServerClientNetIO( NDUint32 nSessionID );

	//设置服务器要处理的协议类型;
	void					setDisposeSessionProtocol();

	//获得DisconnectionType的字符串;
	const char*				getDisconnectionTypeStr( NDUint8 nDisconnectionType );
};


#endif


