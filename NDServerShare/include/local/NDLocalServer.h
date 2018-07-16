#ifndef __SERVER_SHARE_ND_LOCAL_SERVER_H__
#define __SERVER_SHARE_ND_LOCAL_SERVER_H__

#include <iosfwd>
using std::ostringstream;

#include "NDTypes.h"
#include "NDShareBaseMacros.h"
#include "event/timerEvent/NDTimerBoundSlot.h"

#include "NDProtocolCommonEnums.h"

//前置声明;
namespace NDShareBase
{
	struct NDShareBaseGlobal;

	class NDDataProcess;
	class NDProtocolCallBack;
	class NDConsoleCMDManager;
	class NDCLogManager;
	class NDShareMemoryLogManager;

	class NDTimerEventArgs;
	class NDSubFunctorSlot;
}

//用using避免使用完全限定名;
using NDShareBase::NDShareBaseGlobal;
using NDShareBase::NDDataProcess;
using NDShareBase::NDProtocolCallBack;
using NDShareBase::NDConsoleCMDManager;
using NDShareBase::NDCLogManager;
using NDShareBase::NDShareMemoryLogManager;
using NDShareBase::NDTimerEventArgs;
using NDShareBase::NDSubFunctorSlot;
using NDShareBase::NDTimerBoundSlotConn;



class NDXmlConfigServerCommon;
class NDServerInfo;
class NDLocalServerInfo;
class NDConnectProcess
{
public:
	virtual ~NDConnectProcess(){};
	virtual void connected(const NDServerInfo* pServerInfo) = 0;
	virtual void error(const NDServerInfo* pServerInfo) = 0;
};

//配置的服务器基本信息;
struct NDServerCfgSytemInfo
{
	NDUint32	nGameNameID;	//游戏名称ID(TK=1);
	NDUint32	nCountryID;		//国家ID(CN=1,USA=2);
	NDUint32	nAgentID;		//运营商ID(fackbook=1);
	NDUint32	nGroupID;		//服务器组ID;
};

class NDLocalServer
{
public:
	enum eServState_Types
	{
		eServState_Null,
		eServState_Starting,
		eServState_Started,
		eServState_Stopping,
		eServState_Stopped,
	};
protected:
	eServState_Types		m_eState;							//服务器当前状态;

	NDLocalServerInfo*		m_pLocalServerInfo;					//服务器信息;
	NDDataProcess*			m_pDataProcess;						//数据处理实例;
	NDXmlConfigServerCommon*m_pConfig;							//读取配置文件实例;
	NDConnectProcess*		m_pConnProcess;						//链接其他服务器的处理实例;
	NDProtocolCallBack*		m_pCallBack;						//协议处理的实例;
	NDConsoleCMDManager*	m_pConsoleCMDMagager;				//GM命令和控制台命令的实例;

private:
	NDCLogManager*			m_pCLogManager;						//日志管理器;
	NDShareMemoryLogManager*m_pSMLogManager;					//共享内存日志管理器;
	NDTime					m_nStartServerSecondTimeOfUTC;		//启动服务器时的UTC秒数;
	NDServerCfgSytemInfo	m_serverCfgSystemInfo;				//开启服务器系统配置;
	char					m_szDumpFilePath[MAX_PATH_LEN];		//dump文件的路径;
	ostringstream*			m_postringstream;

#ifdef _DEBUG
	NDTimerBoundSlotConn	m_timerPrintTimerListConn;			//printTimerList定时器的关联器;
	static	const char*		s_szTimerPrintTimerList;			//printTimerList定时器的名称;
#endif
	

public:
	virtual ~NDLocalServer();

	virtual	void				run();

	//初始化;
	virtual NDBool				initialize( SERVERTYPE eType, NDUint32 nServerID, const char* szXmlConfig );
	//释放;
	virtual void				release();

	//add timer event;
	NDTimerBoundSlotConn		addTimer( const NDSubFunctorSlot& refSubFunctorSlot, const NDTimerEventArgs& refTimerEventArgs );

	eServState_Types			getState() const;

	void						setState( const eServState_Types& _eState );

	//获得当前服务器的SERVERID;
	NDUint32					getLocalServerID();

	//服务器信息;
	NDLocalServerInfo*			getLocalServerInfo();

	//消息处理器;
	NDDataProcess*				dataProcess();

	//配置;
	NDXmlConfigServerCommon*	config();

	//连接处理;
	NDConnectProcess*			connectProcess();

	//GM和控制台命令的处理器;
	NDConsoleCMDManager*		consoleCMDMgr();
	//log file;
	NDCLogManager*				logMgr();

	//sharememory log file;
	NDShareMemoryLogManager*	smLogMgr();

	//dump file full path;
	const char*					getDumpFilePath();

	const NDServerCfgSytemInfo&	getServerCfgSytemInfo() const;

	//get start server UTC second time;
	NDTime						getStartServerSecondTimeOfUTC() const;

	//全局的输出流;(只用在单线程,最好使用ND_SNPRINTF等C函数);
	/*---------------------------------------------------------------------------- 
	*** 重复使用同一个ostringstream对象时,建议:;
	***	1:调用 clear()清除当前错误控制状态,其原型为 void clear (iostate state=goodbit); 
	***	2:调用str("")将缓冲区清零，清除脏数据;
	*** 3:注意事项:;
	***   std::ostringstream::str()返回的是临时对象，不能对其直接操作;
	***   例如会有如下误用:;
	***   const char* pBuffer=oss.str().c_str(); 注意pBuffer指向的内存已被析构!!;;
	----------------------------------------------------------------------------*/
	ostringstream&				getostringstream();			
protected:
	NDLocalServer();

private:
	NDBool						InitLogMgr();
	NDBool						InitDumpFilePath();
};

#endif

