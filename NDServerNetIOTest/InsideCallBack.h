#ifndef _INSIDE_CALL_BACK_H_
#define _INSIDE_CALL_BACK_H_

#include <iostream>

#include "NDShareBaseGlobal.h"

#include "net/stream/NDProtocolPacket.h"
#include "net/process/NDDataProcess.h"

#include "net/session/NDServerSession.h"
#include "net/session/NDSessionManager.h"

using namespace NDShareBase;


class PingCallBack : public NDProtocolCallBack
{
public:
	NDBool process( NDIStream&, NDProtocolHeader& rNDProtocolHeader )
	{
		NDServerSession* pServerSession = (NDServerSession*)NDSessionManager::getInstance()->findServerSession( rNDProtocolHeader.m_nSessionID );
		if ( NULL == pServerSession )
		{
			return NDFalse;
		}

		pServerSession->setLastPingSecondTime( NDShareBase::NDShareBaseGlobal::getCurSecondTimeOfUTC() );

		return NDTrue;
	}
};

#endif
