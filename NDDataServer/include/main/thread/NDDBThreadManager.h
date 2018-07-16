#ifndef __DATASERVER_ND_DBTHREAD_MANAGER_H__
#define __DATASERVER_ND_DBTHREAD_MANAGER_H__

#include "NDUpdateDBThread.h"

class NDDBThreadManager
{
private:
	NDDBThreadPtrVec	m_selectDBThreadPtrVec;
	NDDBThreadPtrVec	m_updateDBThreadPtrVec;

public:
	NDDBThreadManager();
	~NDDBThreadManager();

	NDBool				init( NDUint8 nSelectDBThreadNum, NDUint8 nUpdateDBThreadNum, const NDMysqlConnParam& refMysqlConnParam );
	void				release();

	void				stopAllThread();
	NDBool				isQuitAllThread();

	//����GUID�ҵ������߳�;
	NDUpdateDBThread*	getUpdateDBThread( NDTableBase* pTableBaseProduce );

	NDUint32			getTotalDBThreadSize() const	{ return (NDUint32)( m_selectDBThreadPtrVec.size() + m_updateDBThreadPtrVec.size() ); }
	NDUint32			getSelectDBThreadSize() const	{ return (NDUint32)m_selectDBThreadPtrVec.size(); }
	NDUint32			getUpdateDBThreadSize() const	{ return (NDUint32)m_updateDBThreadPtrVec.size(); }

private:
	void				stopSelectDBThread();
	void				stopUpdateDBThread();
};


#endif

