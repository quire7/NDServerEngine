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

void NDDataProcess::registerCallBack( NDUint32 nProtocolID, NDProtocolCallBack *pNDCallBack )
{
#ifdef _DEBUG
	CallBackMapIter iterFind = m_CallBackMap.find( nProtocolID );
	if ( iterFind != m_CallBackMap.end() )
	{
		char szBuf[BUF_LEN_64] = {0};
		ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " MessageID: [%u] has registered!", nProtocolID );
		NDLOG_ERROR( szBuf )

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
	rIStream.read( &protocolHeader, sizeof(NDProtocolHeader) );

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

				char szBuf[BUF_LEN_128] = {0};
				ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " MessageName:[%s],MessageID:[%u] has registered, but no register dispose event!", strName.c_str(), protocolHeader.m_nProtocolID );
				NDLOG_ERROR( szBuf )
			}
			else
			{
				char szBuf[BUF_LEN_128] = {0};
				ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " MessageID:[%u] has registered, but no register dispose event!", protocolHeader.m_nProtocolID );
				NDLOG_ERROR( szBuf )
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

				char szBuf[BUF_LEN_128] = {0};
				ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " MessageName:[%s],MessageID:[%u] has registered, but no register dispose event!", strName.c_str(), protocolHeader.m_nProtocolID );
				NDLOG_ERROR( szBuf )
			}
			else
			{
				char szBuf[BUF_LEN_128] = {0};
				ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " MessageID:[%u] has registered, but no register dispose event!", protocolHeader.m_nProtocolID );
				NDLOG_ERROR( szBuf )
			}
		}
	}
#endif

	return bRet;
}

void NDDataProcess::registerProtocolIDName( NDUint32 nProtocolID, string strProtocolIDName )
{
#ifdef _DEBUG
	ProtocolIDNameMapIter iterFind = m_ProtocolIDNameMap.find( nProtocolID );
	if ( iterFind != m_ProtocolIDNameMap.end() )
	{
		string& strName = iterFind->second;

		char szBuf[BUF_LEN_128] = {0};
		ND_SNPRINTF( szBuf, sizeof(szBuf) - 1, " MessageName:[%s],MessageID:[%u] has registered!", strName.c_str(), nProtocolID );
		NDLOG_ERROR( szBuf )

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
