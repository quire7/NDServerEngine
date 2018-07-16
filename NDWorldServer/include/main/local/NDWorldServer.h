#ifndef __WORLDSERVER_ND_WORLD_SERVER_H__
#define __WORLDSERVER_ND_WORLD_SERVER_H__

#include "main/data/NDWSPlayerManager.h"
#include "main/data/NDWSDataManager.h"
#include "main/thread/NDWSDBThreadManager.h "

#include "NDWorldCallBack.h"
#include "NDWorldConnectProcess.h"

class NDWorldServer : public NDLocalServer, public NDSingleton<NDWorldServer>
{
private:
	NDRemoteServerManager*			m_pGameServerManager;
	NDRemoteServerManager*			m_pRoomServerManager;
	NDWSPlayerManager*				m_pNDWSPlayerManager;
	NDWSDataManager*				m_pNDWSDataManager;
	NDWSDBThreadManager*			m_pNDWSDBThreadManager;
public:
	NDWorldServer(void){}
	~NDWorldServer(void){ release(); }

	NDBool						initialize( SERVERTYPE eType, NDUint32 nServerID, const char* szXmlConfig )
	{
		m_pLocalServerInfo		= new NDLocalServerInfo;
		m_pDataProcess			= new NDDataProcess;
		m_pConfig				= new NDWSServerConfig;
		m_pConnProcess			= new NDWorldConnectProcess;
		m_pCallBack				= new NDWorldCallBack;
		m_pConsoleCMDMagager	= new NDConsoleCMDManager;

		m_pGameServerManager	= new NDRemoteServerManager;
		m_pRoomServerManager	= new NDRemoteServerManager;
		m_pNDWSPlayerManager	= new NDWSPlayerManager;
		m_pNDWSDataManager		= new NDWSDataManager;
		m_pNDWSDBThreadManager	= new NDWSDBThreadManager;

		NDWSServerConfig* pWSServerConfig = (NDWSServerConfig*)m_pConfig;
		if ( NULL == pWSServerConfig )
		{
			return NDFalse;
		}
		pWSServerConfig->setWSDBAccountInfoVec( m_pNDWSDataManager->getWSDBAccountInfoVecPtr() );

		if ( NDFalse == NDLocalServer::initialize( eType, nServerID, szXmlConfig ) )
		{
			return NDFalse;
		}

		return NDTrue;
	}
	void						release()
	{
		SAFE_DELETE( m_pGameServerManager )
		SAFE_DELETE( m_pRoomServerManager )
		SAFE_DELETE( m_pNDWSPlayerManager )
		SAFE_DELETE( m_pNDWSDataManager )
		SAFE_DELETE( m_pNDWSDBThreadManager )
	}

	NDRemoteServerManager*		gameServerManager()		{	return m_pGameServerManager; }
	NDRemoteServerManager*		roomServerManager()		{	return m_pRoomServerManager; }
	NDWSPlayerManager*			playerManager()			{	return m_pNDWSPlayerManager; }
	NDWSDataManager*			dataManager()			{	return m_pNDWSDataManager; }
	NDWSDBThreadManager*		dbThreadManager()		{	return m_pNDWSDBThreadManager; }
};

#define sNDWorldServer	NDWorldServer::getSingleton()

#endif

