#include "message/NDS2SRegisterCallBack.h"


#include "protocol/NDGameToNDData/NDGS2DS_Register.h"


#include "main/local/NDDataServer.h"


NDS2SRegisterCallBack::NDS2SRegisterCallBack(void)
{
	NDRegisterCallBackMACRO( sNDDataServer.dataProcess(), CMDP_NDGS2DS_Register_Req, this )
}

NDS2SRegisterCallBack::~NDS2SRegisterCallBack(void)
{
}

NDBool NDS2SRegisterCallBack::process( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	NDBool bRet = NDFalse;

	switch (protocolHeader.m_nProtocolID)
	{
	case CMDP_NDGS2DS_Register_Req:
		{
			bRet = gs2dsRegisterReqDispose( rIStream, protocolHeader );
		}
		break;
	}

	return bRet;
}

NDBool NDS2SRegisterCallBack::gs2dsRegisterReqDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	NDGS2DS_Register_Req registerReq;
	if ( NDFalse == registerReq.deserialize(rIStream) )
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::gs2dsRegisterReqDispose] NDGS2DS_Register_Req deserialize failed! " )
		return NDFalse;
	}

	NDRemoteServerInfo* pGameServerInfo =  new NDRemoteServerInfo;
	if ( NULL ==  pGameServerInfo )
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::gs2dsRegisterReqDispose] new NDGameServerInfo failed! " )
		return NDFalse;
	}

	pGameServerInfo->setServerType( GAME_SERVER );
	
	pGameServerInfo->setServerID( registerReq.m_nMapID );
	pGameServerInfo->setServerName( "NDGameServer" );
	pGameServerInfo->setSessionID( protocolHeader.m_nSessionID );
	pGameServerInfo->setNetAddress( registerReq.m_netAddress );

	sNDDataServer.gameServerManager()->addRemoteServer( pGameServerInfo );

	//NDServerManager::getSingleton().SetSessionProtocolType( protocolHeader.m_nSessionID, NDSessionProtocolType_CS2L );

	const NDSocketAddress& rNetAddress = registerReq.m_netAddress;

	char szBuf[BUF_LEN_256] = {0};
	ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " %s [%s:%u] [GameServerID:%u] connected. ",	pGameServerInfo->getServerName(),
																						rNetAddress.getIP(),
																						rNetAddress.getPort(),
																						pGameServerInfo->getServerID() );
	NDLOG_INFO( szBuf )


	NDGS2DS_Register_Res response;
	response.m_nErrorCode = eND_SRS_OK;
	return NDServerManager::getSingleton().sendToClient( response, protocolHeader.m_nSessionID );
}


//NDBool NDS2SRegisterCallBack::l2aRegisterResDispose( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
//{
//	NDL2A_Register_Res response;
//	if ( NDFalse == response.deserialize( rIStream ) )
//	{
//		NDLOG_ERROR( " [NDS2SRegisterCallBack::l2aRegisterResDispose] NDL2A_Register_Res deserialize failed! " )
//		return NDFalse;
//	}
//
//
//	const NDServerInfo* pAccountDBServerInfo = NDServerManager::getSingleton().getConnServerInfo( protocolHeader.m_nSessionID );
//	if ( NULL == pAccountDBServerInfo )
//	{
//		NDLOG_ERROR( " [NDS2SRegisterCallBack::l2aRegisterResDispose] pAccountDBServerInfo is NULL! " )
//		return NDFalse;
//	}
//
//	std::ostringstream oStr;
//	oStr		<< " register "
//				<< pAccountDBServerInfo->getServerName()	<< "("
//				<< pAccountDBServerInfo->getServerIP()		<< ":"
//				<< pAccountDBServerInfo->getServerPort()	<< ")"
//				<< " return response success! ";
//	NDTOTAL_LOG_INFO( oStr.str().c_str() )
//	oStr.clear();
//
//
//	return NDTrue;
//}


