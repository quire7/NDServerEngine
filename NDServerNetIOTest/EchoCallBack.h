#ifndef _ECHO_CALL_BACK_H_
#define _ECHO_CALL_BACK_H_

#include <iostream>

#include "net/stream/NDProtocolPacket.h"
#include "net/process/NDDataProcess.h"
#include "NDEchoProtocol.h"

using namespace NDShareBase;


class EchoCallBack : public NDProtocolCallBack
{
public:
	NDBool Process( NDIStream& rIStream , NDProtocolHeader& rNDProtocolHeader )
	{
		NDEchoProtocol echoProtocol;
		if ( NDFalse == echoProtocol.Deserialize(rIStream) )
		{
			return NDFalse;
		}

		std::cout << "Session ID is :" << rNDProtocolHeader.m_nSessionID << std::endl;
		std::cout << "protocol data is :" << echoProtocol.m_strData << std::endl;

		//pServerNetIO.SetSessionProtocolType( rNDProtocolHeader.m_nSessionID, NDSessionProtocolType_CLIENT2GT  );

		return pServerNetIO.sendToClient(echoProtocol, rNDProtocolHeader.m_nSessionID, NDFalse, NDFalse, NDTrue );
	}
};

#endif