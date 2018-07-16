#ifndef __SERVER_SHARE_ND_LOCAL_SERVER_H__
#define __SERVER_SHARE_ND_LOCAL_SERVER_H__

#include <iosfwd>
using std::ostringstream;

#include "NDTypes.h"
#include "NDShareBaseMacros.h"
#include "event/timerEvent/NDTimerBoundSlot.h"

#include "NDProtocolCommonEnums.h"

//ǰ������;
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

//��using����ʹ����ȫ�޶���;
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

//���õķ�����������Ϣ;
struct NDServerCfgSytemInfo
{
	NDUint32	nGameNameID;	//��Ϸ����ID(TK=1);
	NDUint32	nCountryID;		//����ID(CN=1,USA=2);
	NDUint32	nAgentID;		//��Ӫ��ID(fackbook=1);
	NDUint32	nGroupID;		//��������ID;
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
	eServState_Types		m_eState;							//��������ǰ״̬;

	NDLocalServerInfo*		m_pLocalServerInfo;					//��������Ϣ;
	NDDataProcess*			m_pDataProcess;						//���ݴ���ʵ��;
	NDXmlConfigServerCommon*m_pConfig;							//��ȡ�����ļ�ʵ��;
	NDConnectProcess*		m_pConnProcess;						//���������������Ĵ���ʵ��;
	NDProtocolCallBack*		m_pCallBack;						//Э�鴦���ʵ��;
	NDConsoleCMDManager*	m_pConsoleCMDMagager;				//GM����Ϳ���̨�����ʵ��;

private:
	NDCLogManager*			m_pCLogManager;						//��־������;
	NDShareMemoryLogManager*m_pSMLogManager;					//�����ڴ���־������;
	NDTime					m_nStartServerSecondTimeOfUTC;		//����������ʱ��UTC����;
	NDServerCfgSytemInfo	m_serverCfgSystemInfo;				//����������ϵͳ����;
	char					m_szDumpFilePath[MAX_PATH_LEN];		//dump�ļ���·��;
	ostringstream*			m_postringstream;

#ifdef _DEBUG
	NDTimerBoundSlotConn	m_timerPrintTimerListConn;			//printTimerList��ʱ���Ĺ�����;
	static	const char*		s_szTimerPrintTimerList;			//printTimerList��ʱ��������;
#endif
	

public:
	virtual ~NDLocalServer();

	virtual	void				run();

	//��ʼ��;
	virtual NDBool				initialize( SERVERTYPE eType, NDUint32 nServerID, const char* szXmlConfig );
	//�ͷ�;
	virtual void				release();

	//add timer event;
	NDTimerBoundSlotConn		addTimer( const NDSubFunctorSlot& refSubFunctorSlot, const NDTimerEventArgs& refTimerEventArgs );

	eServState_Types			getState() const;

	void						setState( const eServState_Types& _eState );

	//��õ�ǰ��������SERVERID;
	NDUint32					getLocalServerID();

	//��������Ϣ;
	NDLocalServerInfo*			getLocalServerInfo();

	//��Ϣ������;
	NDDataProcess*				dataProcess();

	//����;
	NDXmlConfigServerCommon*	config();

	//���Ӵ���;
	NDConnectProcess*			connectProcess();

	//GM�Ϳ���̨����Ĵ�����;
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

	//ȫ�ֵ������;(ֻ���ڵ��߳�,���ʹ��ND_SNPRINTF��C����);
	/*---------------------------------------------------------------------------- 
	*** �ظ�ʹ��ͬһ��ostringstream����ʱ,����:;
	***	1:���� clear()�����ǰ�������״̬,��ԭ��Ϊ void clear (iostate state=goodbit); 
	***	2:����str("")�����������㣬���������;
	*** 3:ע������:;
	***   std::ostringstream::str()���ص�����ʱ���󣬲��ܶ���ֱ�Ӳ���;
	***   ���������������:;
	***   const char* pBuffer=oss.str().c_str(); ע��pBufferָ����ڴ��ѱ�����!!;;
	----------------------------------------------------------------------------*/
	ostringstream&				getostringstream();			
protected:
	NDLocalServer();

private:
	NDBool						InitLogMgr();
	NDBool						InitDumpFilePath();
};

#endif

