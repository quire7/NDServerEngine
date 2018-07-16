#ifndef __DATASERVER_ND_DATA_SERVER_H__
#define __DATASERVER_ND_DATA_SERVER_H__


#include "main/data/NDDSPlayerManager.h"
#include "main/data/NDDSDataManager.h"
#include "main/thread/NDDBThreadManager.h"

#include "NDDataCallBack.h"


class NDDataServer : public NDLocalServer, public NDSingleton<NDDataServer>
{
private:
	NDRemoteServerManager*			m_pGameServerManager;
	NDDSPlayerManager*				m_pNDDSPlayerManager;
	NDDSDataManager*				m_pNDDSDataManager;
	NDDBThreadManager*				m_pNDDBThreadManager;
public:
	NDDataServer(void){}
	~NDDataServer(void){ release(); }

	NDBool							initialize( SERVERTYPE eType, NDUint32 nServerID, const char* szXmlConfig )
	{
		m_pLocalServerInfo		= new NDLocalServerInfo;
		m_pDataProcess			= new NDDataProcess;
		m_pConfig				= new NDDBServerConfig;
		m_pCallBack				= new NDDataCallBack;
		m_pConsoleCMDMagager	= new NDConsoleCMDManager;



		m_pGameServerManager	= new NDRemoteServerManager;
		m_pNDDSPlayerManager	= new NDDSPlayerManager;
		m_pNDDSDataManager		= new NDDSDataManager;
		m_pNDDBThreadManager	= new NDDBThreadManager;

		NDDBServerConfig* pNDDBServerConfig = dynamic_cast<NDDBServerConfig*>(m_pConfig);
		if ( NULL == pNDDBServerConfig )
		{
			return NDFalse;
		}

		NDDSConfigBaseInfo* pNDDSConfigBaseInfo = m_pNDDSDataManager->getDSConfigBaseInfo();
		NDMysqlConnParam* pNDMysqlConnParam		= m_pNDDSDataManager->getMysqlConnParam();
		pNDDBServerConfig->setDSConfigBaseInfo( pNDDSConfigBaseInfo );
		pNDDBServerConfig->setMysqlConnParam( pNDMysqlConnParam );

		if ( NDFalse == NDLocalServer::initialize( eType, nServerID, szXmlConfig ) )
		{
			return NDFalse;
		}

		if ( NDFalse == m_pNDDBThreadManager->init( pNDDSConfigBaseInfo->m_nSelectDBThread, pNDDSConfigBaseInfo->m_nUpdateDBThread, *pNDMysqlConnParam ) )
		{
			return NDFalse;
		}

		return NDTrue;
	}
	void							release()
	{
		SAFE_DELETE( m_pGameServerManager )
		SAFE_DELETE( m_pNDDSPlayerManager )
		SAFE_DELETE( m_pNDDSDataManager )
		SAFE_DELETE( m_pNDDBThreadManager )
	}

	void							run()
	{
		m_pNDDSDataManager->heartBeat();
		NDLocalServer::run();
	}

	NDRemoteServerManager*			gameServerManager()		{	return m_pGameServerManager; }
	NDDSPlayerManager*				playerManager()			{	return m_pNDDSPlayerManager; }
	NDDSDataManager*				dataManager()			{	return m_pNDDSDataManager; }
	NDDBThreadManager*				dbThreadManager()		{	return m_pNDDBThreadManager; }
};

#define sNDDataServer	NDDataServer::getSingleton()

#endif

