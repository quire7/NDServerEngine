/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\session\NDSessionManager.h
	file base:	NDSessionManager
	purpose:	session manager;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_SESSION_MANAGER_H__
#define __SHARE_BASE_ND_SESSION_MANAGER_H__

#include "NDTypes.h"


_NDSHAREBASE_BEGIN

class NDByteBuffer;
class NDSession;
class NDSessionManagerImpl;
class NDProtocol;

class NDSessionManager
{
private:
	NDSessionManagerImpl*		m_pSessionManagerImpl;

	static NDSessionManager*	m_pSessionManager;

public:
	static NDSessionManager*	getInstance();
	static void					releaseInstance();

	~NDSessionManager(void);
	
	NDSession*	createServerSession();
	NDSession*	createClientSession();

	NDBool		addWorkServerSession( NDSession* pServerSession );
	NDBool		addWorkClientSession( NDSession* pClientSession );

	NDSession*  removePrepServerSession( NDUint32 nSessionID );

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
	//关闭所有无效的状态的session;
	void		closeInvalidWorkSession();
	//设置session为无效的状态;
	NDBool		setInvalidWorkSession( SOCKET nSocketID );


	NDBool		eventLoop();

	//优化注册事件,在需要发送的时候才注册WRITE_EVENT;
	NDBool		registerWriteEvent( SOCKET fd );
	NDBool		unregisterWriteEvent( SOCKET fd );


	//设置通用客户端断开连接时的内部向上层抛出的DisconnectNtyProtocol;
	NDBool		setCommonDisconnectNtyProtocol( NDProtocol* pDisconnectNtyProtocol );
	NDBool		popCommonDisconnectNtyProtocol( NDUint32 nSessionID );

	//设置Session接收的数据包的解析函数;
	void		setSessionParsePacketFun( PParsePacketFun parsePacketFun );

	//是否要处理的协议;
	//NDBool		isSessionTypeProtocol( NDInt8 sessionProtocolType, NDUint32 nProtocolID );
	//void			setSessionTypeProtocol( NDSessionProtocolType eType, NDUint32 nBeginProtocol, NDUint32 nEndProtocol );

protected:
	NDSessionManager(void);

private:
	NDSessionManager(const NDSessionManager& other);
	NDSessionManager& operator = (const NDSessionManager& other);


};

_NDSHAREBASE_END

#endif
