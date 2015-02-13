#include "message/NDS2SRegisterCallBack.h"

//#include "../../NDServerShare/include/Protocol/NDCenterToNDGameDB/NDC2GDB_Register.h"
#include "protocol/NDWorldToNDLogin/NDWS2LS_Register.h"
//#include "../../NDServerShare/include/protocol/NDGateToNDCenter/NDG2C_Register.h"
//#include "../../NDServerShare/include/protocol/NDMapToNDCenter/NDM2C_Register.h"

//#include "../RemoteServerInfo/NDMapServerInfo.h"

#include "main/local/NDWorldServer.h"



NDS2SRegisterCallBack::NDS2SRegisterCallBack(void)
{
	//NDRegisterCallBackMACRO( sNDWorldServer.dataProcess(), CMD_NDCS2GDB_Register_Res, this )
	NDRegisterCallBackMACRO( sNDWorldServer.dataProcess(), CMDP_NDLS2WS_Register_Res, this )
	//NDRegisterCallBackMACRO( sNDWorldServer.dataProcess(), CMD_NDG2CS_Register_Req, this )
	//NDRegisterCallBackMACRO( sNDWorldServer.dataProcess(), CMD_NDM2CS_Register_Req, this )
}

NDS2SRegisterCallBack::~NDS2SRegisterCallBack(void)
{
}

NDBool NDS2SRegisterCallBack::Process( NDIStream& rIStream, NDProtocolHeader& protocolHeader )
{
	NDBool	bRet = NDFalse;

	switch (protocolHeader.m_nProtocolID)
	{
	//case CMD_NDCS2GDB_Register_Res:
	//	{
	//		bRet = c2gdbRegisterResDispose( rIStream, protocolHeader );
	//	}
	//	break;
	case CMDP_NDLS2WS_Register_Res:
		{
			bRet = ls2wsRegisterResDispose( rIStream, protocolHeader );
		}
		break;
	//case CMD_NDG2CS_Register_Req:
	//	{
	//		bRet = g2cRegisterReqDispose( rIStream, protocolHeader );
	//	}
	//	break;
	//case CMD_NDM2CS_Register_Req:
	//	{
	//		bRet = m2cRegisterReqDispose( rIStream, protocolHeader );
	//	}
	//	break;
	}

	return bRet;
}

//
//NDBool NDS2SRegisterCallBack::c2gdbRegisterResDispose( NDIStream& rIStream, NDProtocolHeader& header )
//{
//	NDCS2GDB_Register_Res registerRes;
//	if ( NDFalse == registerRes.Deserialize(rIStream) ) 
//	{
//		NDLOG_ERROR( " [NDS2SRegisterCallBack::c2gdbRegisterResDispose] NDC2GDB_Register_Res deserialize failed! " )
//		return NDFalse;
//	}
//		
//	const NDServerInfo* pGameDBServerInfo = NDServerManager::getSingleton().getConnServerInfo( header.m_nSessionID );
//	if ( NULL == pGameDBServerInfo )
//	{
//		NDLOG_ERROR( " [NDS2SRegisterCallBack::c2gdbRegisterResDispose] pGameDBServerInfo is NULL! " )
//		return NDFalse;
//	}
//
//	if ( eND_SRS_OK == registerRes.m_nErrorCode )
//	{
//		std::ostringstream oStr;
//		oStr		<< " register "
//					<< pGameDBServerInfo->getServerName()	<< "("
//					<< pGameDBServerInfo->getServerIP()		<< ":"
//					<< pGameDBServerInfo->getServerPort()	<< ")"
//					<< " return response success! ";
//		NDLOG_INFO( oStr.str().c_str() )
//		oStr.clear();
//	}
//	else
//	{
//		std::ostringstream oStr;
//		oStr		<< " register "
//					<< pGameDBServerInfo->getServerName()	<< "("
//					<< pGameDBServerInfo->getServerIP()		<< ":"
//					<< pGameDBServerInfo->getServerPort()	<< ")"
//					<< " return response failed! ";			
//		NDLOG_ERROR( oStr.str().c_str() );
//		oStr.clear();
//	}
//
//	return NDTrue;
//}


NDBool NDS2SRegisterCallBack::ls2wsRegisterResDispose( NDIStream& rIStream, NDProtocolHeader& header )
{
	NDLS2WS_Register_Res registerRes;
	if ( NDFalse == registerRes.Deserialize(rIStream) ) 
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::ls2wsRegisterResDispose] NDLS2WS_Register_Res deserialize failed!" )
		return NDFalse;
	}

	const NDServerInfo* pLoginServerInfo = NDServerManager::getSingleton().getConnServerInfo( header.m_nSessionID );
	if ( NULL == pLoginServerInfo )
	{
		NDLOG_ERROR( " [NDS2SRegisterCallBack::ls2wsRegisterResDispose] pLoginServerInfo is NULL!" )
		return NDFalse;
	}

	char szBuf[BUF_LEN_128] = {0};
	if ( eND_SRS_OK == registerRes.m_uiErrCode )
	{
		ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " register %s [%s:%u] return response success. ",	pLoginServerInfo->getServerName(),
																									pLoginServerInfo->getServerIP(),
																									pLoginServerInfo->getServerPort() );
		NDLOG_INFO( szBuf )
	}
	else
	{
		ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " register %s [%s:%u] return response failed. ",	pLoginServerInfo->getServerName(),
																								pLoginServerInfo->getServerIP(),
																								pLoginServerInfo->getServerPort() );
		NDLOG_ERROR( szBuf )
	}

	return NDTrue;
}

//NDBool NDS2SRegisterCallBack::g2cRegisterReqDispose( NDIStream& rIStream, NDProtocolHeader& header )
//{
//	NDG2C_Register_Req	registerReq;
//	if ( NDFalse == registerReq.Deserialize(rIStream) )
//	{
//		NDLOG_ERROR( " [NDS2SRegisterCallBack::g2cRegisterReqDispose] NDG2C_Register_Req deserialize failed! " )
//		return NDFalse;
//	}
//
//	NDRemoteServerInfo* pGateServerInfo = new NDRemoteServerInfo;
//	if ( NULL == pGateServerInfo )
//	{
//		NDLOG_ERROR( " [NDS2SRegisterCallBack::g2cRegisterReqDispose] pGateServerInfo is NULL! " )
//		return NDFalse;
//	}
//
//	pGateServerInfo->setServerType( GATE_SERVER );
//	pGateServerInfo->setServerName( "NDGateServer" );
//
//	pGateServerInfo->setSessionID( header.m_nSessionID );
//	pGateServerInfo->setLoadFactor( NDLoadFactor(1000) );
//	pGateServerInfo->setNetAddress( registerReq.m_netAddress );
//
//	NDGateServerManager* pGateServerMgr = sNDCenterServer.gateServerManager();
//	if ( NULL == pGateServerMgr )
//	{
//		NDLOG_ERROR( " [NDS2SRegisterCallBack::g2cRegisterReqDispose] pGateServerMgr is NULL! " )
//		return NDFalse;
//	}
//
//	//the first gate server info send to login server;
//	if ( NDTrue == pGateServerMgr->isEmptyRemoteServerInfo() )
//	{
//		NDCS2L_LoadFactor_Nty loadFactorNty;
//		loadFactorNty.m_stBestGate			= pGateServerInfo->getNetAddress();
//		loadFactorNty.m_nBestGateSessionID	= pGateServerInfo->getSessionID();
//		loadFactorNty.m_stLoadFactor		= NDLoadFactor(1000);
//
//		NDServerManager::getSingleton().sendToServer( loadFactorNty, LOGIN_SERVER );
//	} 
//	pGateServerMgr->addRemoteServer( pGateServerInfo );
//
//	NDServerManager::getSingleton().SetSessionProtocolType( header.m_nSessionID, NDSessionProtocolType_GT2CS );
//
//	const NDNetAddress& rNetAddress = pGateServerInfo->getNetAddress();
//
//	std::ostringstream oStr;
//	oStr		<< " " 
//				<< pGateServerInfo->getServerName()		<< "("
//				<< rNetAddress.getIP()					<< ":"
//				<< rNetAddress.getPort()				<< ")"
//				<< " connected! ";
//	NDLOG_INFO( oStr.str().c_str() )
//	oStr.clear();
//
//
//	NDG2C_Register_Res	registerRes;
//	registerRes.m_nErrorCode = eND_SRS_OK;
//	registerRes.m_nGateWayID = 0;
//	registerRes.m_nWorldID   = 0;
//
//	sNDCenterServer.mapServerManager()->getAllMapServerInfo( registerRes.m_gameServerAddress );
//
//	return NDServerManager::getSingleton().sendToClient( registerRes, header.m_nSessionID );
//	 
//}
//
//NDBool NDS2SRegisterCallBack::m2cRegisterReqDispose( NDIStream& rIStream, NDProtocolHeader& header )
//{
//	NDM2CS_Register_Req	m2csRegisterReq;
//	if ( NDFalse == m2csRegisterReq.Deserialize(rIStream) )
//	{
//		NDLOG_ERROR( " [NDS2SRegisterCallBack::m2cRegisterReqDispose] NDM2C_Register_Req deserialize failed! " )
//		return NDFalse;
//	}
//
//	NDMapServerInfo* pMapServerInfo =  new NDMapServerInfo;
//	if ( NULL == pMapServerInfo )
//	{
//		NDLOG_ERROR( " [NDS2SRegisterCallBack::m2cRegisterReqDispose] new NDMapServerInfo failed! " )
//		return NDFalse;
//	}
//
//	pMapServerInfo->setServerType( MAP_SERVER );
//	pMapServerInfo->setServerName( "NDMapServer" );
//	//pMapServerInfo->setLoadFactor(NDLoadFactor(2000));
//	pMapServerInfo->setSessionID( header.m_nSessionID );
//	pMapServerInfo->setNetAddress( m2csRegisterReq.m_netAddress );
//	pMapServerInfo->setMapID(m2csRegisterReq.m_nMapID);
//
//	sNDCenterServer.mapServerManager()->addRemoteServer( pMapServerInfo );
//
//	sNDCenterServer.gateServerManager()->sendMapInfo( pMapServerInfo );
//
//	NDServerManager::getSingleton().SetSessionProtocolType( header.m_nSessionID, NDSessionProtocolType_M2CS );
//
//	const NDNetAddress& rNetAddress = pMapServerInfo->getNetAddress();
//
//	std::ostringstream oStr;
//	oStr		<< " " 
//				<< pMapServerInfo->getServerName()		<< "("
//				<< rNetAddress.getIP()					<< ":"
//				<< rNetAddress.getPort()				<< ")"
//				<< " connected! ";
//	NDLOG_INFO( oStr.str().c_str() )
//	oStr.clear();
//
//	NDM2C_Register_Res	m2cRegisterRes;
//	m2cRegisterRes.m_nErrorCode	= eND_SRS_OK;
//
//	return NDServerManager::getSingleton().sendToClient(  m2cRegisterRes, header.m_nSessionID );
//
//}

