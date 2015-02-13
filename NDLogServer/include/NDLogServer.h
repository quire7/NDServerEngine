#ifndef __LOGSERVER_ND_LOG_SERVER_H__
#define __LOGSERVER_ND_LOG_SERVER_H__

#include <map>
using std::map;

#include "NDTypes.h"
#include "function/NDSingleton.h"
#include "file/NDCFile.h"
#include "file/NDShareMemroyLogFile.h"


//ǰ������;
namespace NDShareBase
{
	struct NDShareBaseGlobal;

	class NDCLogManager;

	template<typename T>
	class NDShareMemoryUnitPool;
}

//��using����ʹ����ȫ�޶���;
using NDShareBase::NDShareBaseGlobal;
using NDShareBase::NDSingleton;
using NDShareBase::NDShareLogCacheSMU;
using NDShareBase::NDShareMemoryUnitPool;
using NDShareBase::NDCFile;


class NDLogServer : public NDSingleton<NDLogServer>
{
private:
	NDShareMemoryUnitPool<NDShareLogCacheSMU>	*m_pNDShareLogCacheSMUPool;

	struct NDLogFileInfo
	{
		NDUint32		m_nLogNameHash;				//log name's hash value;
		NDCFile			m_file;						//log file obj;
		NDTime			m_nLastWriteTime;			//���д���ݽ����ļ���ʱ��(����);
	};
	typedef map< NDUint32, NDLogFileInfo* >		NDLogFileInfoMap;
	typedef NDLogFileInfoMap::iterator			NDLogFileInfoMapIter;

	NDLogFileInfoMap							m_NDLogFileInfoMap;

public:
	NDLogServer();
	~NDLogServer();

	NDBool		init();
	NDBool		release();

	NDBool		loop();

private:
	NDBool		newStaticManager();
	NDBool		delStaticManager();
	NDBool		initStaticManager();

	NDBool		saveLog( NDShareLogCacheSMU *pNDShareLogCacheSMU );

	void		checkLogFileInfo();
};


#endif
