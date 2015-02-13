#include "net/session/NDServerSession.h"

#include "net/session/NDSessionManager.h"
#include "net/socket/NDServerSocket.h"




_NDSHAREBASE_BEGIN

NDServerSession::NDServerSession( NDUint32 nSessionID ) : NDSession( nSessionID ), m_nLastPingSecondTime( 0 )
{
	m_pSocket = new NDServerSocket;
}

NDServerSession::~NDServerSession()
{
	SAFE_DELETE( m_pSocket )
}

void NDServerSession::destroy()
{
	////NDUint32 nDisconnectNty		= CMD_DISCONNECT_NOTIFY;
	//NDUint32 nDisconnectNty		= 0;
	//NDByteBuffer* pDisMsgBuf	= NDByteBufferPool::getInstance()->createByteBuffer();
	//if ( NULL != pDisMsgBuf )
	//{
	//	pDisMsgBuf->WriteBuffer( (const char*)&m_nSessionID, sizeof(m_nSessionID) );
	//	pDisMsgBuf->WriteBuffer( (const char*)&nDisconnectNty, sizeof(nDisconnectNty) );
	//	//put msg into  msg dispose deque
	//	//NDServerTask::getInstance()->PutQueue( pDisMsgBuf );
	//}

	NDSessionManager::getInstance()->popCommonDisconnectNtyProtocol( m_nSessionID );

	NDSessionManager::getInstance()->releaseServerSession( m_nSessionID );	
}

_NDSHAREBASE_END

