#ifndef __SERVER_SHARE_ND_SERVER_MANAGER_H__
#define __SERVER_SHARE_ND_SERVER_MANAGER_H__

#include <vector>
using std::vector;
#include <map>
using std::map;

#include "NDTypes.h"
#include "net/socket/NDSocketAddress.h"
#include "function/NDSingleton.h"

#include "NDProtocolBase.h"


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

class NDServerManager : public NDSingleton<NDServerManager>
{
private:

	typedef vector<NDServerInfo*>			ConnServerTable;
	typedef ConnServerTable::iterator		ConnServerTableIter;

	typedef map<NDUint32, NDClientNetIO*>	ConnServerInstance;
	typedef ConnServerInstance::iterator	ConnServerInstanceIter;

private:
	NDLocalServer*				m_pListenServer;		// ����������ʵ��;

	NDServerNetIO*				m_pListenNetIO;			// ����������������netio;

	NDMutexLock*				m_pCSTMutex;			// m_connServerTable mutex;

	NDMutexLock*				m_pCSIMutex;			// m_connServerInstance mutex;

	ConnServerTable				m_connServerTable;		// ���ӵ�Ŀ��Server�б�;

	ConnServerInstance			m_connServerInstance;	// �������ӵ�netioʵ��(NDClientNetIO Instance);

	NDUint32					m_nMainProcMilliSeconds;// main��ѭ����ÿ���ٺ���ִ��һ���¼�;

public:
	NDServerManager();
	~NDServerManager(void);

	//��������������;
	NDBool					startServer( NDLocalServer* pLocalServer, NDBool bConnOtherServer=NDTrue );
	NDBool					mainLoop();

	NDBool					getClientAddress( NDUint32 nSessionID, NDSocketAddress& refClientAddress );
	NDBool					disConnectClient( NDUint32 nSessionID );

	NDBool					setSessionProtocolType( NDUint32 nSessionID, NDSessionProtocolType sessionProtocolType );

	//send protocol to client(real client);
	NDBool					sendToClient( NDProtocol& protocol, NDUint32 nSessionID, NDBool bCompression=NDFalse, NDBool bEncrypt=NDFalse, NDBool bCrc=NDFalse );

	//����ͻ��˵�PINGЭ��;
	NDBool					pingProtocolCommonDispose( NDUint32 nSessionID );

	////////////////////////////////////////////////////////////////////////////////////////////////
	/* connect server dispose start */

	// get connect server Info;
	const NDServerInfo*		getConnServerInfo( NDUint32 nSessionID );

	// register remote server(need to connect server);
	void					registerConnServer( NDServerInfo* pServerInfo );

	// connect config file server;
	NDBool					connectServer( NDConnectProcess* pConnecter );
	// connect singleton server;
	NDBool					connectServer( NDServerInfo* pServerInfo, NDConnectProcess* pConnecter );

	//������������������������ӵ�״̬;
	NDBool					checkAllConnectServerState();
	//�������ӶϿ�����������������;
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

	//�����Ѿ����ӵ���������������������ΪDISCONNECTED״̬;
	NDBool					setConnServerDisconnectState( NDUint32 nSessionID );

	NDClientNetIO*			removeConnServerClientNetIO( NDUint32 nSessionID );

	//���÷�����Ҫ�����Э������;
	void					setDisposeProtocol();

};


#endif


