#include "net/process/NDDataProcess.h"

#include "NDCLibFun.h"
#include "file/NDLog.h"

#include "net/stream/NDIStream.h"


_NDSHAREBASE_BEGIN

NDDataProcess::NDDataProcess(void)
{
	m_ProtocolIDNameMap.clear();
	m_CallBackMap.clear();
}

NDDataProcess::~NDDataProcess(void)
{
	m_ProtocolIDNameMap.clear();
	m_CallBackMap.clear();
}

void NDDataProcess::registerCallBack( NDUint16 nProtocolID, NDProtocolCallBack *pNDCallBack )
{
#ifdef _DEBUG
	CallBackMapIter iterFind = m_CallBackMap.find( nProtocolID );
	if ( iterFind != m_CallBackMap.end() )
	{
		NDLOG_ERROR( " MessageID: [%u] has registered!", nProtocolID);

		return;
	}
#endif

	if ( 0 != pNDCallBack )
	{
		m_CallBackMap.insert( std::make_pair( nProtocolID, pNDCallBack ) );
	}
#ifdef _DEBUG
	else
	{
		NDLOG_ERROR( " [NDDataProcess::registerCallBack]  pNDCallBack is NULL! " )
	}
#endif
}

NDBool NDDataProcess::process( NDIStream &rIStream )
{
	NDProtocolHeader protocolHeader;
	rIStream.read( &protocolHeader.m_nSessionID, sizeof(protocolHeader.m_nSessionID) );
	rIStream.read( &protocolHeader.m_nProtocolID, sizeof(protocolHeader.m_nProtocolID) );

	return process( rIStream, protocolHeader );
}

NDBool NDDataProcess::process( NDIStream &rIStream, NDProtocolHeader& protocolHeader, NDBool bShowError )
{
	NDBool bRet = NDFalse;

	CallBackMapIter iter = m_CallBackMap.find( protocolHeader.m_nProtocolID );
	if ( iter != m_CallBackMap.end() )
	{
		NDProtocolCallBack *pNDCallBack = iter->second;
		if ( 0 != pNDCallBack )
		{
			bRet = pNDCallBack->process( rIStream, protocolHeader );
		}
#ifdef _DEBUG	
		else
		{
			ProtocolIDNameMapIter nameIter = m_ProtocolIDNameMap.find( protocolHeader.m_nProtocolID );
			if ( nameIter != m_ProtocolIDNameMap.end() )
			{
				string& strName = nameIter->second;

				NDLOG_ERROR( " MessageName:[%s],MessageID:[%u] has registered, but no register dispose event!", strName.c_str(), protocolHeader.m_nProtocolID );
			}
			else
			{
				NDLOG_ERROR( " MessageID:[%u] has registered, but no register dispose event!", protocolHeader.m_nProtocolID );
			}
			
		}
#endif
	}
#ifdef _DEBUG
	else
	{
		if ( NDTrue == bShowError )
		{
			ProtocolIDNameMapIter nameIter = m_ProtocolIDNameMap.find( protocolHeader.m_nProtocolID );
			if ( nameIter != m_ProtocolIDNameMap.end() )
			{
				string& strName = nameIter->second;

				NDLOG_ERROR( " MessageName:[%s],MessageID:[%u] has registered, but no register dispose event!", strName.c_str(), protocolHeader.m_nProtocolID );
			}
			else
			{
				NDLOG_ERROR( " MessageID:[%u] has registered, but no register dispose event!", protocolHeader.m_nProtocolID );
			}
		}
	}
#endif

	return bRet;
}

void NDDataProcess::registerProtocolIDName( NDUint16 nProtocolID, string strProtocolIDName )
{
#ifdef _DEBUG
	ProtocolIDNameMapIter iterFind = m_ProtocolIDNameMap.find( nProtocolID );
	if ( iterFind != m_ProtocolIDNameMap.end() )
	{
		string& strName = iterFind->second;

		NDLOG_ERROR( " MessageName:[%s],MessageID:[%u] has registered!", strName.c_str(), nProtocolID );

		return;
	}
#endif

	if ( strProtocolIDName.length() > 0 )
	{
		m_ProtocolIDNameMap.insert( std::make_pair( nProtocolID, strProtocolIDName ) );
	}
#ifdef _DEBUG
	else
	{
		NDLOG_ERROR( " Register message name can't be empty." )
	}
#endif
}

_NDSHAREBASE_END
