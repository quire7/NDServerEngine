/********************************************************************
	created:	2010/04/27
	file base:	XmlConfigBase
	file ext:	h
	author:		quire
	
	purpose:	parse XML file config;
*********************************************************************/

#ifndef __SERVER_SHARE_ND_XML_CONFIG_SERVER_COMMON_H__
#define __SERVER_SHARE_ND_XML_CONFIG_SERVER_COMMON_H__

#include "NDShareBaseMacros.h"
#include "file/NDXmlConfigBase.h"
#include "NDProtocolCommonEnums.h"

using NDShareBase::NDXmlConfigBase;

#ifndef SYSCFG
#define SYSCFG					"SystemCfg"
#define SYSCFG_GAMENAMEID		"GameNameID"
#define SYSCFG_COUNTRYID		"CountryID"
#define SYSCFG_AGENTID			"AgentID"
#define SYSCFG_GROUPID			"GroupID"
#endif

#ifndef SERINFO
#define SERINFO					
#define SERINFO_COUNT			"Count"
#define SERINFO_LISTENIP		"ListenIP"
#define SERINFO_LISTENPORT		"ListenPort"
#define SERINFO_DUMPPATH		"DumpPath"
#define SERINFO_LOGPATH			"LogPath"
#endif

#ifndef CONNINFO
#define CONNINFO				"Conn"
#define CONNINFO_COUNT			"Count"
#define CONNINFO_SERVERNAME		"ServerName"
#define CONNINFO_SERVERID		"ServerID"
#endif


#ifndef DBINFO
#define DBINFO					"DataBase"
#define DBINFO_TYPENAME			"DB"
#define DBINFO_HOST				"Host"
#define DBINFO_USER				"User"
#define DBINFO_PASSWORD			"Pwd"
#define DBINFO_PORT				"Port"
#define DBINFO_DBNAME			"DBName"
#define DBINFO_VERSION			"Version"
#endif

struct NDServerCfgSytemInfo;
class NDLocalServerInfo;
class NDXmlConfigServerCommon : public NDXmlConfigBase
{
protected:

	struct  AppInfoBaseAttribute
	{
		SERVERTYPE	eServerType;
		char*		szServerTypeName;
		char*		szServerName;
	};

	enum
	{
		eAppType_MAX = 7,
	};

	static AppInfoBaseAttribute	s_AppInfoBaseAttribute[eAppType_MAX];

	struct ConnInfoBaseAttribute
	{
		NDUint32	nServerID;
		char		szServerName[MAX_SERVER_NAME];
	};


	NDServerCfgSytemInfo*	m_pServerCfgSytemInfo;	
	NDLocalServerInfo*		m_pLocalServerInfo;

public:
	NDXmlConfigServerCommon(void){};
	virtual ~NDXmlConfigServerCommon(){};

	void setServerCfgSytemInfo( NDServerCfgSytemInfo* pServerCfgSytemInfo ) { m_pServerCfgSytemInfo = pServerCfgSytemInfo; }
	void setLocalServerInfo( NDLocalServerInfo* pLocalServerInfo )			{ m_pLocalServerInfo = pLocalServerInfo; }

protected:
	virtual NDBool readXmlConfigContent( const TiXmlElement *pRootElement );

private:
	NDBool	readSystemCfgCommonContent( const TiXmlElement *pParentElement );
	NDBool	readSerInfoXmlCfgCommonContent( const TiXmlElement *pParentElement, vector<ConnInfoBaseAttribute>& refConnInfoBaseAttributeVec );
	NDBool	readConnInfoCfgCommonContent( const TiXmlElement *pParentElement, vector<ConnInfoBaseAttribute>& refConnInfoBaseAttributeVec  );
	NDBool	readAppInfoXmlCfgCommonContent( const TiXmlElement *pParentElement, vector<ConnInfoBaseAttribute>& refConnInfoBaseAttributeVec );
	NDBool	readSerChildInfoXmlCfgCommonContent( const TiXmlElement *pParentElement );
	NDBool	readAppChildInfoXmlCfgCommonContent( const TiXmlElement *pParentElement, ConnInfoBaseAttribute& refConnInfoBaseAttribute );

	static AppInfoBaseAttribute* getAppInfoBaseAttribute( const char* szServerName );
	static AppInfoBaseAttribute* getAppInfoBaseAttribute( SERVERTYPE eServerType );
};


namespace NDShareBase
{
	struct MysqlConnParam;
}

using NDShareBase::MysqlConnParam;

class NDDBServerConfig : public NDXmlConfigServerCommon 
{
private:
	MysqlConnParam* m_pConnParam;

public:
	NDDBServerConfig();
	virtual ~NDDBServerConfig(void);

	void	setMysqlConnParam( MysqlConnParam* pMysqlConnParam ){ m_pConnParam = pMysqlConnParam; }

protected:
	virtual NDBool readXmlConfigContent(const TiXmlElement *pRootElement);

private:
	NDBool readDBInfoXmlCfgCommonContent(const TiXmlElement *pRootElement);

};



#endif

