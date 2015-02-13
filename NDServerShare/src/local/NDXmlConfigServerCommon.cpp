#include "local/NDXmlConfigServerCommon.h"

#include "NDCLibFun.h"
#include "NDShareBaseGlobal.h "
#include "database/NDMysqlConst.h"

#include "tinyxml/tinyxml.h"

#include "local/NDServerInfo.h"
#include "local/NDLocalServer.h"
#include "local/NDServerManager.h"

using NDShareBase::NDShareBaseGlobal;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

NDXmlConfigServerCommon::AppInfoBaseAttribute NDXmlConfigServerCommon::s_AppInfoBaseAttribute[eAppType_MAX] = {
	{LOGIN_SERVER,		"LS",		"NDLoginServer"},		
	{GATEWAY_SERVER,	"GWS",		"NDGatewayServer"},		
	{WORLD_SERVER,		"WS",		"NDWorldServer"},		
	{GAME_SERVER,		"GS",		"NDGameServer"},			
	{ROOM_SERVER,		"RS",		"NDRoomServer"},			
	{MSG_SERVER,		"MSGS",		"NDMessageServer"},		
	{DB_SERVER,			"DS",		"NDDataServer"},		
};


NDBool NDXmlConfigServerCommon::readXmlConfigContent( const TiXmlElement *pRootElement )
{
	if ( NULL == pRootElement || NULL == m_pServerCfgSytemInfo || NULL == m_pLocalServerInfo ) return NDFalse;
	
	//read system cfg element;
	if ( NDFalse == readSystemCfgCommonContent( pRootElement ) )
	{
		return NDFalse;
	}

	vector<ConnInfoBaseAttribute>	ConnInfoBaseAttributeVec;
	/* read SerInfo self element */
	if ( NDFalse == readSerInfoXmlCfgCommonContent( pRootElement, ConnInfoBaseAttributeVec ) )
	{
		return NDFalse;
	}

	/* read connect SerInfo element */
	if ( !ConnInfoBaseAttributeVec.empty() )
	{
		if ( NDFalse == readAppInfoXmlCfgCommonContent( pRootElement, ConnInfoBaseAttributeVec ) )
		{
			return NDFalse;
		}
	}

	return NDTrue;
}


NDBool NDXmlConfigServerCommon::readSystemCfgCommonContent( const TiXmlElement *pParentElement )
{
	//根据SERVER名称寻找子节点;
	const TiXmlElement* pChildElement = pParentElement->FirstChildElement( SYSCFG );
	if ( NULL == pChildElement )
	{
		return NDFalse;
	}

	NDInt32 nIntValue = 0;
	if ( !getElement2( pChildElement, SYSCFG_GAMENAMEID, nIntValue ) )
	{
		return NDFalse;
	}
	m_pServerCfgSytemInfo->nGameNameID = nIntValue;

	nIntValue = 0;
	if ( !getElement2( pChildElement, SYSCFG_COUNTRYID, nIntValue ) )
	{
		return NDFalse;
	}
	m_pServerCfgSytemInfo->nCountryID = nIntValue;

	nIntValue = 0;
	if ( !getElement2( pChildElement, SYSCFG_AGENTID, nIntValue ) )
	{
		return NDFalse;
	}
	m_pServerCfgSytemInfo->nAgentID = nIntValue;

	nIntValue = 0;
	if ( !getElement2( pChildElement, SYSCFG_GROUPID, nIntValue ) )
	{
		return NDFalse;
	}
	m_pServerCfgSytemInfo->nGroupID = nIntValue;

	return NDTrue;
}

NDBool NDXmlConfigServerCommon::readSerInfoXmlCfgCommonContent(const TiXmlElement *pParentElement, vector<ConnInfoBaseAttribute>& refConnInfoBaseAttributeVec )
{
	SERVERTYPE eSERVERTYPE						= m_pLocalServerInfo->getServerType();
	AppInfoBaseAttribute* pAppInfoBaseAttribute = getAppInfoBaseAttribute( eSERVERTYPE );
	if ( NULL == pAppInfoBaseAttribute )
	{
		return NDFalse;
	}

	//m_pLocalServerInfo->setServerName( pAppInfoBaseAttribute->szServerName );
	m_pLocalServerInfo->setServerName( pAppInfoBaseAttribute->szServerTypeName ); //还是用类型名好些;

	//根据SERVER名称寻找子节点;
	const TiXmlElement* pChildElement = pParentElement->FirstChildElement( pAppInfoBaseAttribute->szServerName );
	if ( NULL == pChildElement )
	{
		return NDFalse;
	}

	NDInt32 nIntValue = 0;
	if ( !getElement2( pChildElement, SERINFO_COUNT, nIntValue ) )
	{	//相应server总数目;
		return NDFalse;
	}
	if ( nIntValue <= 0 )
	{
		return NDFalse;
	}

	NDUint32 nServerID = m_pLocalServerInfo->getServerID();
	if ( nServerID > (NDUint32)nIntValue )
	{
		return NDFalse;
	}

	//重组要寻找的ServerTypeName;
	char szServerTypeNameBuf[32] = {0};
	ND_SNPRINTF( szServerTypeNameBuf, sizeof(szServerTypeNameBuf) - 1, "%s%u", pAppInfoBaseAttribute->szServerTypeName, nServerID );

	//在pChildElement节点中寻找ServerTypeName子节点;
	const TiXmlElement* pSonOfChildElement = pChildElement->FirstChildElement( szServerTypeNameBuf );
	if ( NULL == pSonOfChildElement )
	{
		return NDFalse;
	}
	if ( !readSerChildInfoXmlCfgCommonContent( pSonOfChildElement ) )
	{
		return NDFalse;
	}

	return readConnInfoCfgCommonContent( pChildElement, refConnInfoBaseAttributeVec );
}


NDBool NDXmlConfigServerCommon::readSerChildInfoXmlCfgCommonContent( const TiXmlElement *pParentElement )
{
	const char* pContent = getElement2( pParentElement, SERINFO_LISTENIP );
	if ( NULL == pContent )
	{
		return NDFalse;
	}
	m_pLocalServerInfo->setServerIP( pContent );

	NDInt32 nIntValue = 0;
	if ( !getElement2( pParentElement, SERINFO_LISTENPORT, nIntValue ) )
	{
		return NDFalse;
	}
	m_pLocalServerInfo->setServerPort( (NDUint16)nIntValue );

	pContent = getElement2( pParentElement, SERINFO_DUMPPATH );
	if ( NULL == pContent )
	{
		pContent = NDShareBaseGlobal::getMoudlePath();
	}

	char szPathBuf[MAX_PATH_LEN] = {0};
	ND_SNPRINTF( szPathBuf, MAX_PATH_LEN - 1, "%s/dump/%s/%u/", pContent, m_pLocalServerInfo->getServerName(), m_pLocalServerInfo->getServerID() );

	m_pLocalServerInfo->setDumpPath( szPathBuf );

	pContent = getElement2( pParentElement, SERINFO_LOGPATH );
	if ( NULL == pContent )
	{
		pContent = NDShareBaseGlobal::getMoudlePath();
	}

	memset( szPathBuf, 0, sizeof(szPathBuf) );
	ND_SNPRINTF( szPathBuf, MAX_PATH_LEN - 1, "%s/log/%s/%u/", pContent, m_pLocalServerInfo->getServerName(), m_pLocalServerInfo->getServerID() );

	m_pLocalServerInfo->setLogPath( szPathBuf );

	return NDTrue;	
}


NDBool NDXmlConfigServerCommon::readConnInfoCfgCommonContent( const TiXmlElement *pParentElement, vector<ConnInfoBaseAttribute>& refConnInfoBaseAttributeVec )
{	//read Conn info element;
	//在Server节点中寻找Conn子节点;
	const TiXmlElement* pChildElement = pParentElement->FirstChildElement( CONNINFO );
	if ( NULL == pChildElement )
	{	//不是所有SERVER都需要去连接其他SERVER的;
		return NDTrue;
	}

	NDInt32 nIntValue = 0;
	if ( !getElement2( pChildElement, CONNINFO_COUNT, nIntValue ) )
	{	//相应conn server总数目;
		return NDFalse;
	}
	if ( nIntValue <= 0 )
	{
		return NDFalse;
	}

	NDInt32 nCount = nIntValue;
	for ( NDInt32 i = 1; i <= nCount; ++i )
	{
		//重组要寻找的Conn子节点;
		char szConnNameBuf[32] = {0};
		ND_SNPRINTF( szConnNameBuf, sizeof(szConnNameBuf) - 1, "%s%d", CONNINFO, i );

		//在pChildElement节点中寻找Conn子节点;
		const TiXmlElement* pSonOfChildElement = pChildElement->FirstChildElement( szConnNameBuf );
		if ( NULL == pSonOfChildElement )
		{
			return NDFalse;
		}

		const char* pContent = getElement2( pSonOfChildElement, CONNINFO_SERVERNAME );
		if ( NULL == pContent )
		{
			return NDFalse;
		}
		
		ConnInfoBaseAttribute connInfoBaseAttribute;
		memset( &connInfoBaseAttribute, 0, sizeof(connInfoBaseAttribute) );
		NDShareBaseGlobal::safeStrCopy( connInfoBaseAttribute.szServerName, pContent, sizeof(connInfoBaseAttribute.szServerName), (NDUint32)strlen(pContent) );

		nIntValue = 0;
		if ( getElement2( pSonOfChildElement, CONNINFO_SERVERID, nIntValue) )
		{
			connInfoBaseAttribute.nServerID = nIntValue;
		}

		refConnInfoBaseAttributeVec.push_back( connInfoBaseAttribute );
	}

	return NDTrue;
}


NDBool NDXmlConfigServerCommon::readAppInfoXmlCfgCommonContent( const TiXmlElement *pParentElement, vector<ConnInfoBaseAttribute>& refConnInfoBaseAttributeVec )
{
	NDUint32 nSize = (NDUint32)refConnInfoBaseAttributeVec.size();
	for ( NDUint32 i = 0; i < nSize; ++i )
	{
		ConnInfoBaseAttribute& refConnInfoBaseAttribute = refConnInfoBaseAttributeVec[i];

		const TiXmlElement* pChildElement = pParentElement->FirstChildElement( refConnInfoBaseAttribute.szServerName );
		if (NULL == pChildElement)
		{
			return NDFalse;
		}

		AppInfoBaseAttribute* pAppInfoBaseAttribute = getAppInfoBaseAttribute( refConnInfoBaseAttribute.szServerName );
		if ( NULL == pAppInfoBaseAttribute )
		{
			return NDFalse;
		}

		if ( 0 == refConnInfoBaseAttribute.nServerID )
		{	//连接当前element所有的server;
			NDInt32 nCountValue = 0;
			if ( !getElement2( pChildElement, SERINFO_COUNT, nCountValue ) )
			{	//相应conn server总数目;
				return NDFalse;
			}
			if ( nCountValue <= 0 )
			{
				return NDFalse;
			}

			for ( NDUint32 j = 1; j < (NDUint32)nCountValue; ++j )
			{
				refConnInfoBaseAttribute.nServerID = j;
				if ( !readAppChildInfoXmlCfgCommonContent( pChildElement, refConnInfoBaseAttribute ) )
				{
					return NDFalse;
				}
			}
		}
		else
		{	//只连接server中的serverid的那个element;
			if ( !readAppChildInfoXmlCfgCommonContent( pChildElement, refConnInfoBaseAttribute ) )
			{
				return NDFalse;
			}
		}
	}


	return NDTrue;
}

NDBool NDXmlConfigServerCommon::readAppChildInfoXmlCfgCommonContent( const TiXmlElement *pParentElement, ConnInfoBaseAttribute& refConnInfoBaseAttribute )
{
	AppInfoBaseAttribute* pAppInfoBaseAttribute = getAppInfoBaseAttribute( refConnInfoBaseAttribute.szServerName );
	if ( NULL == pAppInfoBaseAttribute )
	{
		return NDFalse;
	}

	//重组要寻找的ServerTypeName;
	char szServerTypeNameBuf[32] = {0};
	ND_SNPRINTF( szServerTypeNameBuf, sizeof(szServerTypeNameBuf) - 1, "%s%u", pAppInfoBaseAttribute->szServerTypeName, refConnInfoBaseAttribute.nServerID );

	//在pChildElement节点中寻找ServerTypeName子节点;
	const TiXmlElement* pChildElement = pParentElement->FirstChildElement( szServerTypeNameBuf );
	if ( NULL == pChildElement )
	{
		return NDFalse;
	}

	const char* pContent = getElement2( pChildElement, SERINFO_LISTENIP );
	if ( NULL == pContent )
	{
		return NDFalse;
	}

	NDInt32 nIntValue = 0;
	if ( !getElement2( pChildElement, SERINFO_LISTENPORT, nIntValue ) )
	{
		return NDFalse;
	}

	NDServerInfo* pServInfo = new NDServerInfo;
	pServInfo->setServerType( pAppInfoBaseAttribute->eServerType );
	pServInfo->setServerName( refConnInfoBaseAttribute.szServerName );
	pServInfo->setServerID( refConnInfoBaseAttribute.nServerID );
	pServInfo->setServerIP( pContent );
	pServInfo->setServerPort( (NDUint16)nIntValue );

	pServInfo->setConnState( NDServerInfo::eConnState_Initial );
	pServInfo->setLocalRemote( NDServerInfo::eLR_Remote );

	NDServerManager::getSingleton().registerConnServer( pServInfo );

	return NDTrue;
}

NDXmlConfigServerCommon::AppInfoBaseAttribute* NDXmlConfigServerCommon::getAppInfoBaseAttribute( const char* szServerName )
{
	int nSize = sizeof(s_AppInfoBaseAttribute)/sizeof(AppInfoBaseAttribute);

	for ( int i = 0; i < nSize; ++i )
	{
		if ( 0 == strncmp( s_AppInfoBaseAttribute[i].szServerName, szServerName, strlen(s_AppInfoBaseAttribute[i].szServerName) ) )
		{
			return &(s_AppInfoBaseAttribute[i]);
		}
	}

	return NULL;
}

NDXmlConfigServerCommon::AppInfoBaseAttribute* NDXmlConfigServerCommon::getAppInfoBaseAttribute( SERVERTYPE eServerType )
{
	int nSize = sizeof(s_AppInfoBaseAttribute)/sizeof(AppInfoBaseAttribute);

	for ( int i = 0; i < nSize; ++i )
	{
		if ( s_AppInfoBaseAttribute[i].eServerType == eServerType )
		{
			return &(s_AppInfoBaseAttribute[i]);
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////
NDDBServerConfig::NDDBServerConfig():
m_pConnParam(NULL)
{
}

NDDBServerConfig::~NDDBServerConfig( void )
{
}

NDBool NDDBServerConfig::readXmlConfigContent(const TiXmlElement *pRootElement)
{
	if ( !NDXmlConfigServerCommon::readXmlConfigContent( pRootElement ) )
	{
		return NDFalse;
	}

	return readDBInfoXmlCfgCommonContent(pRootElement);
}


NDBool NDDBServerConfig::readDBInfoXmlCfgCommonContent(const TiXmlElement *pRootElement)
{
	if ( NULL == m_pConnParam )
	{
		return NDFalse;
	}

	const TiXmlElement* pChildElement = pRootElement->FirstChildElement(DBINFO);
	if ( NULL == pChildElement )
	{
		return NDFalse;
	}

	//重组要寻找的DB;
	char szDBBuf[32] = {0};
	ND_SNPRINTF( szDBBuf, sizeof(szDBBuf) - 1, "%s%u", DBINFO_TYPENAME, m_pLocalServerInfo->getServerID() );

	//在pChildElement节点中寻找DB子节点;
	const TiXmlElement* pSonOfChildElement = pChildElement->FirstChildElement( szDBBuf );
	if ( NULL == pSonOfChildElement )
	{
		return NDFalse;
	}

	const char* pContent = getElement2( pSonOfChildElement, DBINFO_HOST );
	if ( NULL == pContent )
	{
		return NDFalse;
	}
	m_pConnParam->setHostName( string(pContent) );

	pContent = getElement2( pSonOfChildElement, DBINFO_USER );
	if ( NULL == pContent )
	{
		return NDFalse;
	}
	m_pConnParam->setUserName( string(pContent) );
	

	pContent = getElement2(pSonOfChildElement, DBINFO_PASSWORD);
	if ( NULL == pContent )
	{
		return NDFalse;
	}
	m_pConnParam->setPassWord( string(pContent) );
	

	NDInt32 nIntValue = 0;
	if ( !getElement2( pSonOfChildElement, DBINFO_PORT, nIntValue ) )
	{
		return NDFalse;
	}
	m_pConnParam->setPort( nIntValue );
	

	pContent = getElement2( pSonOfChildElement, DBINFO_DBNAME );
	if ( NULL == pContent )
	{
		return NDFalse;
	}
	m_pConnParam->setDBName( string(pContent) );
	
	return NDTrue;
}


