#include "net/process/NDDataProcess.h"
#include ".\ndmessagemanager.h"

NDMessageManager* NDMessageManager::m_pNDMessageManager = NULL;

NDMessageManager::NDMessageManager(void)
{
	m_pNDDataProcess = new NDDataProcess;
}

NDMessageManager::~NDMessageManager(void)
{
	SAFE_DELETE( m_pNDDataProcess );
}

void NDMessageManager::RegisterMessage()
{
	m_pNDDataProcess->RegisterCallBack(1, &m_EchoCallBack);
}

NDMessageManager* NDMessageManager::getInstance()
{
	if (NULL == m_pNDMessageManager)
	{
		m_pNDMessageManager = new NDMessageManager;
	}

	return m_pNDMessageManager;
}

void NDMessageManager::releaseInstance()
{
	if (NULL != m_pNDMessageManager)
	{
		delete m_pNDMessageManager;
		m_pNDMessageManager = NULL;
	}
}