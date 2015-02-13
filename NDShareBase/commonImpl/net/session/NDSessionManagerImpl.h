#ifndef __SHARE_BASE_ND_SESSION_MANAGER_IMPL_H__
#define __SHARE_BASE_ND_SESSION_MANAGER_IMPL_H__

#include <deque>
#include <map>



#include "NDTypes.h"
#include "NDShareBaseMacros.h"
#include "function/NDObjectPool.h"
#include "memory/NDByteBuffer.h"
#include "net/socket/NDSelect.h"
//#include "NDProtocolHead.h"

#ifndef CREATE_SESSION_NUM
#define CREATE_SESSION_NUM	5
#endif

/* Socket Garbage Collector time out */
#ifndef SOCKET_GC_TIMEOUT
#define SOCKET_GC_TIMEOUT	15
#endif

/* Socket PING time out */
#ifndef SOCKET_PING_TIMEOUT
#define SOCKET_PING_TIMEOUT	(120000)	//2 minute
#endif

_NDSHAREBASE_BEGIN

class NDSession;
class NDSysLock;
class NDProtocol;

class NDSessionManagerImpl
{
private:
	typedef	std::deque<NDUint32>			UINTDeque;
	typedef	UINTDeque::iterator				UINTDequeIter;
	typedef	std::deque<NDSession*>			SessionDeque;
	typedef	SessionDeque::iterator			SessionDequeIter;
	typedef std::map<NDUint32, NDSession*>	SessionMap;
	typedef	SessionMap::iterator			SessionMapIter;
	typedef std::map<NDSession*, NDTime>	ReleaseSessionMap;
	typedef	ReleaseSessionMap::iterator		ReleaseSessionMapIter;

	typedef	std::deque<SOCKET>				SOCKETDeque;
	typedef	SOCKETDeque::iterator			SOCKETDequeIter;
	typedef std::map<SOCKET, NDSession*>	SessionSOCKETMap;
	typedef	SessionSOCKETMap::iterator		SessionSOCKETMapIter;

	NDUint32			m_generateID;
	NDUint32			m_nCreateNum;			//	only for server session : total number;
	NDUint32			m_nSpareNum;			//	only for server session : can use number;
	NDUint32			m_nAliveNum;			//	only for server session : inline number;

	NDSysLock*			m_pGenerateMutex;		//	mutex for m_generateID;
	NDSysLock*			m_pServerMutex;			//	mutex for m_serverSessionMap;
	NDSysLock*			m_pClientMutex;			//	mutex for m_clientSessionMap;
	NDSysLock*			m_pPrepSMapMutex;		//	mutex for m_prepServerSessionMap;
	NDSysLock*			m_pCSSDequeMutex;		//	mutex for m_createServerSessionDeque;
	NDSysLock*			m_pRSSMapMutex;			//	mutex for m_releaseServerSessionMap;


	SessionMap			m_serverSessionMap;				// server session map;
	SessionMap			m_prepServerSessionMap;			// preparation work server session map;

	SessionDeque		m_createServerSessionDeque;		// create server session deque;
	ReleaseSessionMap	m_releaseServerSessionMap;		// release server session map;

	SessionMap			m_clientSessionMap;				// client session map;
	SessionDeque		m_createClientSessionDeque;		// create server session deque;

	NDProtocol*			m_pDisconnectNtyProtocol;		// server session disconnect pop DisconnectNtyProtocol(�ͻ��˶Ͽ�����ʱ����������ӦSESSION���ϲ��׳���Э��);
	//NDUint32			m_protocolNumArray[NDSessionProtocolType_MAX][ND_PROTOCOL_SEND_LAYER_MAXNUM*2];	//�洢ĳ��Э������Ҫ�����Э�����ʼID;

private:
	NDSelect			m_select;						// select�¼��ľ��;

	SessionSOCKETMap	m_allWorkSessionMap;			// client and server session map; (key : SOCKET);
	SOCKETDeque			m_invalidSessionDeque;			// invalid SOCKET deque;

public:
	NDSessionManagerImpl(void);
	~NDSessionManagerImpl(void);

	NDSession*	createServerSession();
	NDSession*	createClientSession();

	NDBool		addWorkServerSession( NDSession* pServerSession );
	NDBool		addWorkClientSession( NDSession* pClientSession );

	NDSession*  removePrepServerSession( NDUint32 nSessionID );

	/* find session from work session map */
	NDSession*	findServerSession( NDUint32 nSessionID );
	NDSession*	findClientSession( NDUint32 nSessionID );

	NDBool		sendToServer( NDProtocol& rProtocol, NDUint32 nSessionID, NDUint8 nProDataHeadBitWise );
	NDBool		sendToClient( NDProtocol& rProtocol, NDUint32 nSessionID, NDUint8 nProDataHeadBitWise );

	void		releaseServerSession( NDUint32 nSessionID );
	void		releaseClientSession( NDUint32 nSessionID );

	//update release session to create session map;
	void		updateReleaseSessionMap();
	//update work server session to check invalid session;
	void		updateServerSessionMap();
	//�ر�������Ч��״̬��session;
	void		closeInvalidWorkSession();
	//����sessionΪ��Ч��״̬;
	void		setInvalidWorkSession( SOCKET nSocketID );

	NDBool		eventLoop();

	//�Ż�ע���¼�,����Ҫ���͵�ʱ���ע��WRITE_EVENT;
	NDBool		registerWriteEvent( SOCKET fd );
	NDBool		unregisterWriteEvent( SOCKET fd );		


	//����ͨ�ÿͻ��˶Ͽ�����ʱ���ڲ����ϲ��׳���DisconnectNtyProtocol;
	NDBool		setCommonDisconnectNtyProtocol( NDProtocol* pDisconnectNtyProtocol );
	NDBool		popCommonDisconnectNtyProtocol( NDUint32 nSessionID );
	
	//����Session���յ����ݰ��Ľ�������;
	void		setSessionParsePacketFun( PParsePacketFun parsePacketFun );
	//�Ƿ�Ҫ�����Э��;
	//NDBool		isSessionTypeProtocol( NDInt8 sessionProtocolType, NDUint32 nProtocolID );

	//void			setSessionTypeProtocol(  NDSessionProtocolType eType, NDUint32 nBeginProtocol, NDUint32 nEndProtocol );

private:

	NDUint32	generateID();

	void		createServerSessionDeque();
	void		createClientSessionDeque();

	NDBool		sendData( NDProtocol& rProtocol, NDUint32 nSessionID, NDUint8 nProDataHeadBitWise, NDBool bClient );
};

_NDSHAREBASE_END

#endif

