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

#include "NDServerShareStructs.h"

using NDShareBase::NDXmlConfigBase;

#ifndef SYSCFG
#define SYSCFG							"SystemCfg"
#define SYSCFG_GAMENAMEID				"GameNameID"
#define SYSCFG_COUNTRYID				"CountryID"
#define SYSCFG_AGENTID					"AgentID"
#define SYSCFG_GROUPID					"GroupID"
#endif

#ifndef SERINFO
#define SERINFO							
#define SERINFO_COUNT					"Count"
#define SERINFO_LISTENIP				"ListenIP"
#define SERINFO_LISTENPORT				"ListenPort"
#define SERINFO_DUMPPATH				"DumpPath"
#define SERINFO_LOGPATH					"LogPath"
#endif

#ifndef SERINFO_EX
#define SERINFO_EX					
#define SERINFO_EX_SELECT_THREAD		"SelectThread"
#define SERINFO_EX_UPDATE_THREAD		"UpdateThread"
#endif

#ifndef CONNINFO
#define CONNINFO						"Conn"
#define CONNINFO_COUNT					"Count"
#define CONNINFO_SERVERNAME				"ServerName"
#define CONNINFO_SERVERID				"ServerID"
#endif


#ifndef DBINFO
#define DBINFO							"DataBase"
#define DBINFO_COUNT					"Count"
#define DBINFO_TYPENAME					"DB"
#define DBINFO_HOST						"Host"
#define DBINFO_USER						"User"
#define DBINFO_PASSWORD					"Pwd"
#define DBINFO_PORT						"Port"
#define DBINFO_DBNAME					"DBName"
#define DBINFO_VERSION					"Version"
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
	virtual NDBool	readXmlConfigContent( const TiXmlElement *pRootElement );

	//具体SERVER字段下的延伸字段解析;(例如:DS1字段下:selectDBThreadNum属性);
	virtual NDBool	readSerChildExtendInfoXmlCfgCommonContent( const TiXmlElement* ) { return NDTrue; }

	//读取DataBase属性下的通用部分(即DS1下的相应字段);
	NDBool readDBInfoXmlCfgCommonContent( const TiXmlElement *pDataBaseElement, NDUint32 nDBIndex, NDMysqlConnParam& refMysqlConnParam );
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
	struct NDMysqlConnParam;
}

using NDShareBase::NDMysqlConnParam;

struct NDDSConfigBaseInfo;
class NDDBServerConfig : public NDXmlConfigServerCommon 
{
private:
	NDDSConfigBaseInfo*		m_pDSConfigBaseInfo;
	NDMysqlConnParam*		m_pConnParam;

public:
	NDDBServerConfig();
	virtual ~NDDBServerConfig(void);

	void	setMysqlConnParam( NDMysqlConnParam* pNDMysqlConnParam )		{ m_pConnParam = pNDMysqlConnParam; }
	void	setDSConfigBaseInfo( NDDSConfigBaseInfo* pDSConfigBaseInfo )	{ m_pDSConfigBaseInfo = pDSConfigBaseInfo; }

protected:
	virtual NDBool	readXmlConfigContent(const TiXmlElement *pRootElement);
	
	virtual NDBool	readSerChildExtendInfoXmlCfgCommonContent( const TiXmlElement *pParentElement );

private:
	NDBool	readDBInfoXmlCfgContent(const TiXmlElement *pRootElement);
};


class NDWSServerConfig : public NDXmlConfigServerCommon
{
private:
	vector<NDWSDBAccountInfo>*			m_pNDWSDBAccountInfoVec;
public:
	NDWSServerConfig();
	~NDWSServerConfig();

	void	setWSDBAccountInfoVec( vector<NDWSDBAccountInfo>* pNDWSDBAccountInfoVec )		{ m_pNDWSDBAccountInfoVec = pNDWSDBAccountInfoVec; }

protected:
	NDBool	readXmlConfigContent( const TiXmlElement *pRootElement );

private:
	NDBool	readAllDBInfoXmlCfgCommonContent( const TiXmlElement *pRootElement );
};


#endif

