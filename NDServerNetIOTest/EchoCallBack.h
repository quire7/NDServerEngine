#ifndef _ECHO_CALL_BACK_H_
#define _ECHO_CALL_BACK_H_

#include <iostream>

#include "net/stream/NDProtocolPacket.h"
#include "net/process/NDDataProcess.h"
#include "net/session/NDSessionManager.h"
#include "NDProtocolCommonEnums.h"

#include "NDEchoProtocol.h"

using namespace NDShareBase;


class EchoCallBack : public NDProtocolCallBack
{
public:
	NDBool process( NDIStream& rIStream , NDProtocolHeader& rNDProtocolHeader )
	{
		NDEchoProtocol echoProtocol;
		if ( NDFalse == echoProtocol.deserialize(rIStream) )
		{
			return NDFalse;
		}

		std::cout << "Session ID is :" << rNDProtocolHeader.m_nSessionID << std::endl;
		std::cout << "protocol data is :" << echoProtocol.m_strData << std::endl;

		NDSessionManager::getInstance()->setServerSessionProtocolType( rNDProtocolHeader.m_nSessionID, NDSessionProtocolType_C2LS );
		

		return pServerNetIO.sendToClient(echoProtocol, rNDProtocolHeader.m_nSessionID, NDFalse, NDFalse, NDTrue );
	}
};

#endif
