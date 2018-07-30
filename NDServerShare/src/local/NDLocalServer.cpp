#include "local/NDLocalServer.h"

#include <sstream>

#include "NDShareBaseMacros.h"
#include "NDShareBaseGlobal.h"
#include "NDCLibFun.h"

#include "file/NDLog.h"

#include "net/process/NDDataProcess.h"
#include "function/NDConsoleCMDManager.h"

#include "event/functor/NDSubFunctorSlot.h"
#include "event/timerEvent/NDTimerEventManager.h"
#include "memory/object/NDMemoryPoolExManager.h"

#include "local/NDServerInfo.h"
#include "local/NDXmlConfigServerCommon.h"
#include "local/NDServerManager.h"

using NDShareBase::g_bConsole;
using NDShareBase::g_pFileLogManager;
using NDShareBase::g_pSMLogManager;

using NDShareBase::NDTimerEventManager;
using NDShareBase::NDMemoryPoolExManager;


#ifdef _DEBUG
const char* NDLocalServer::s_szTimerPrintTimerList = "timerPrintTimerList";
#endif


NDLocalServer::NDLocalServer()
:m_eState(eServState_Null),
 m_pLocalServerInfo(NULL),
 m_pDataProcess(NULL),
 m_pConfig(NULL),
 m_pConnProcess(NULL),
 m_pCallBack(NULL),
 m_pConsoleCMDMagager(NULL),
 m_pCLogManager(NULL),
 m_pSMLogManager(NULL),
 m_nStartServerSecondTimeOfUTC( (NDUint32)ND_INVALID_ID )
{
	memset( &m_serverCfgSystemInfo, 0, sizeof(m_serverCfgSystemInfo) );
	memset( m_szDumpFilePath, 0, sizeof(m_szDumpFilePath) );

	m_postringstream = new ostringstream;
}

NDLocalServer::~NDLocalServer()
{
	release();
}	


void NDLocalServer::release()
{
	SAFE_DELETE(m_pLocalServerInfo)
	SAFE_DELETE(m_pDataProcess)
	SAFE_DELETE(m_pConfig)
	SAFE_DELETE(m_pConnProcess)
	SAFE_DELETE(m_pCallBack)
	SAFE_DELETE(m_pConsoleCMDMagager)
	SAFE_DELETE(m_pCLogManager)
	SAFE_DELETE(m_pSMLogManager)

	SAFE_DELETE(m_postringstream)

#ifdef _DEBUG
	if ( m_timerPrintTimerListConn.isvaild() )
	{
		m_timerPrintTimerListConn->disconnect();
	}
#endif

	NDTimerEventManager::releaseInstance();
	NDMemoryPoolExManager::releaseInstance();
}

NDBool NDLocalServer::initialize( SERVERTYPE eType, NDUint32 nServerID, const char* szXmlConfig )
{
	if ( eType < LOGIN_SERVER || eType >= MANAGE_SERVER || nServerID <= 0 || NULL == szXmlConfig || '\0' == szXmlConfig[0] )
	{
		return NDFalse;
	}

	if ( (NULL == m_pLocalServerInfo) || (NULL == m_pDataProcess) || (NULL == m_pConfig) || (NULL ==m_pCallBack) )
	{
		return NDFalse;
	}

	m_pLocalServerInfo->setServerType( eType );
	m_pLocalServerInfo->setServerID( nServerID );

	m_pConfig->setServerCfgSytemInfo( &m_serverCfgSystemInfo );
	m_pConfig->setLocalServerInfo( m_pLocalServerInfo );

	if( NDFalse == m_pConfig->readXmlConfig( szXmlConfig ) )
	{
		return NDFalse;
	}

	//先初始化时间和GUID分配器(因为nGroupID是在配置文件中读取的,所以所有初始化函数都应该在这个函数之后);
	NDShareBaseGlobal::init( (NDUint8)m_serverCfgSystemInfo.nGroupID, (NDUint8)nServerID );	
	NDTimerEventManager::getInstance();
	NDMemoryPoolExManager::getInstance()->init();

	if ( NDFalse == InitLogMgr() )
	{
		return NDFalse;
	}

	SET_NDLOG_MGR( m_pCLogManager, m_pSMLogManager, NDTrue )

	if ( NDFalse == InitDumpFilePath() )
	{
		return NDFalse;
	}

	m_pLocalServerInfo->setLocalRemote( NDServerInfo::eLR_Local );
	
	m_nStartServerSecondTimeOfUTC = NDShareBaseGlobal::getCurSecondTimeOfUTC();

#ifdef _DEBUG
	NDTimerEventArgs printTimerListSubFunctorSlotArgs( NDLocalServer::s_szTimerPrintTimerList, ND_PRINT_OBJMEMEX_INFO_MILLISECONDS,  NDShareBaseGlobal::getCurMSTimeOfUTC() + ND_PRINT_OBJMEMEX_INFO_MILLISECONDS );
	m_timerPrintTimerListConn = NDTimerEventManager::getInstance()->addTimer( NDSubFunctorSlot( &NDTimerEventManager::printTimerList, NDTimerEventManager::getInstance() ), printTimerListSubFunctorSlotArgs );
	if ( !m_timerPrintTimerListConn.isvaild() )
	{
		return NDFalse;
	}
#endif

	return NDTrue;
}

NDBool NDLocalServer::InitLogMgr()
{
	if ( NULL == m_pLocalServerInfo )
	{
		return NDFalse;
	}
	
	const char* pszLogPath = m_pLocalServerInfo->getLogPath();
	if ( NDFalse == NDShareBaseGlobal::isDirectoryExist( pszLogPath ) )
	{
		if ( NDFalse == NDShareBaseGlobal::createDirectory( pszLogPath ) )
		{
			return NDFalse;
		}
	}

	m_pSMLogManager = new NDShareMemoryLogManager;
	if ( NULL == m_pSMLogManager )
	{
		return NDFalse;
	}

	//这个地方byOwnType设计要主要的问题是:不同进程不能用了同一个byOwnType,保证byOwnType唯一性;
	NDUint32 nServerType= m_pLocalServerInfo->getServerType();
	NDUint32 nServerID	= m_pLocalServerInfo->getServerID();
	NDUint16 byOwnType	= (NDUint16)( ( nServerType & MANAGE_SERVER ) + ( nServerID & 0xFFF ) );

	if ( NDFalse == m_pSMLogManager->init( eNDSMKEY_LOG, ND_LOG_SMU_MAX, byOwnType, pszLogPath, m_pLocalServerInfo->getServerName() ) )
	{
		return NDFalse;
	}

	m_pCLogManager = new NDCLogManager();
	if ( NULL == m_pCLogManager )
	{
		return NDFalse;
	}

	return m_pCLogManager->init( pszLogPath, m_pLocalServerInfo->getServerName() );
}

NDDataProcess* NDLocalServer::dataProcess()
{
	return m_pDataProcess;
}

NDXmlConfigServerCommon* NDLocalServer::config()
{
	return m_pConfig;
}

NDConnectProcess* NDLocalServer::connectProcess()
{
	return m_pConnProcess;
}

NDConsoleCMDManager* NDLocalServer::consoleCMDMgr()
{
	return m_pConsoleCMDMagager;
}

NDLocalServer::eServState_Types NDLocalServer::getState() const
{
	return m_eState;
}

void NDLocalServer::setState( const NDLocalServer::eServState_Types& _eState )
{
	m_eState = _eState;
}

NDCLogManager* NDLocalServer::logMgr()
{
	return m_pCLogManager;
}

const char* NDLocalServer::getDumpFilePath()
{
	return m_szDumpFilePath;
}

NDTime NDLocalServer::getStartServerSecondTimeOfUTC() const
{
	return m_nStartServerSecondTimeOfUTC;
}

NDBool NDLocalServer::InitDumpFilePath()
{
	if ( NULL == m_pLocalServerInfo )
	{
		return NDFalse;
	}

	const char* pszDumpPath = m_pLocalServerInfo->getDumpPath();
	if ( NDFalse == NDShareBaseGlobal::isDirectoryExist( pszDumpPath ) )
	{
		if ( NDFalse == NDShareBaseGlobal::createDirectory( pszDumpPath ) )
		{
			return NDFalse;
		}
	}

	char szTimeBuf[ND_TIME_LENGTH] = {0};
	NDShareBaseGlobal::getLocalSecondTimeStr( szTimeBuf, ND_TIME_LENGTH );
	szTimeBuf[10] = '-';
	szTimeBuf[13] = '-';
	szTimeBuf[16] = '-';	//fopen路径规则中不能有:这个符号,否则创建不了文件;

	//最终文件名;
	NDUint32 nLen =	ND_SNPRINTF( m_szDumpFilePath, MAX_PATH_LEN - 1, "%s/%s-%s.dmp", pszDumpPath, m_pLocalServerInfo->getServerName(), szTimeBuf );

	m_szDumpFilePath[nLen] = '\0';

	return NDTrue; 
}

void NDLocalServer::run()
{
	NDTimerEventManager::getInstance()->detectTimerList();
}

NDShareBase::NDTimerBoundSlotConn NDLocalServer::addTimer( const NDSubFunctorSlot& refSubFunctorSlot, const NDTimerEventArgs& refTimerEventArgs )
{
	return NDTimerEventManager::getInstance()->addTimer( refSubFunctorSlot, refTimerEventArgs );
}

const NDServerCfgSytemInfo& NDLocalServer::getServerCfgSytemInfo() const
{
	return m_serverCfgSystemInfo;
}

NDLocalServerInfo* NDLocalServer::getLocalServerInfo()
{
	return m_pLocalServerInfo;
}

ostringstream& NDLocalServer::getostringstream()
{
	return *m_postringstream;
}

NDShareMemoryLogManager* NDLocalServer::smLogMgr()
{
	return m_pSMLogManager;
}

NDUint32 NDLocalServer::getLocalServerID()
{
	if ( NULL == m_pLocalServerInfo )
	{
		return (NDUint32)ND_INVALID_ID;
	}

	return m_pLocalServerInfo->getServerID();
}



