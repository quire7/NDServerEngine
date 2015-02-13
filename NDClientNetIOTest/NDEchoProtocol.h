#ifndef _ND_ECHO_PROTOCOL_H_
#define _ND_ECHO_PROTOCOL_H_

#include <string>
#include <map>
using std::map;

#include "net/stream/NDProtocol.h"
#include "net/stream/NDOStream.h"
#include "net/stream/NDIStream.h"
#include "net/netio/NDClientNetIO.h"

using namespace NDShareBase;


extern map<NDUint32, NDClientNetIO*> gClientMap;

class NDEchoProtocol : public NDProtocol
{
public:
	NDEchoProtocol() : NDProtocol(1) { Clear(); }
	NDBool Serialize( NDOStream &rOSteam )
	{
		//const char *pszData = m_strData.c_str();

		//rOSteam.Write((void*)&m_unProtocolID, sizeof(m_unProtocolID));
		//NDUint32 nSize = 0;
		//if (NULL != pszData)
		//{
		//	nSize = (NDUint32)strlen(pszData);
		//	rOSteam.Write((void*)&nSize, sizeof(nSize));
		//	rOSteam.Write((void*)pszData, nSize);
		//}
		//else
		//{
		//	rOSteam.Write((void*)&nSize, sizeof(nSize));
		//}

		//rOSteam.Write((void*)&m_unProtocolID, sizeof(m_unProtocolID));
		//NDUint32 nSize = (NDUint32)m_strData.length();
		//if ( 0 < nSize )
		//{
		//	const char *pszData = m_strData.c_str();
		//	rOSteam.Write((void*)&nSize, sizeof(nSize));
		//	rOSteam.Write((void*)pszData, nSize);
		//}
		//else
		//{
		//	rOSteam.Write((void*)&nSize, sizeof(nSize));
		//}

		NDOSTREAM_WRITE( rOSteam, &m_unProtocolID, sizeof(m_unProtocolID) )
		NDUint32 nSize = (NDUint32)m_strData.length();
		if ( 0 < nSize )
		{
			const char *pszData = m_strData.c_str();
			NDOSTREAM_WRITE( rOSteam, &nSize, sizeof(nSize) )
			NDOSTREAM_WRITE( rOSteam, pszData, nSize )
		}
		else
		{
			NDOSTREAM_WRITE( rOSteam, &nSize, sizeof(nSize) )
		}

		return true;
	}
	NDBool Deserialize( NDIStream &rIStream )
	{
		//NDUint32 nSize = 0;
		//rIStream.Read((void*)&nSize, sizeof(nSize));
		//if (nSize > 0)
		//{
		//	char *pszData = new char[nSize + 1];

		//	rIStream.Read((void*)pszData, nSize);

		//	pszData[nSize] = '\0';

		//	m_strData = pszData;

		//	delete []pszData;
		//}

		//NDUint32 nSize = 0;
		//rIStream.Read((void*)&nSize, sizeof(nSize));
		//if (nSize > 0)
		//{
		//	if ( nSize > m_strData.capacity() )
		//	{
		//		m_strData.reserve( nSize + 8 );
		//	}
		//	
		//	char* pszData = (char*)m_strData.c_str();
		//	rIStream.Read((void*)pszData, nSize);
		//	pszData[nSize] = '\0';

		//	m_strData = pszData;
		//}

		NDUint32 nSize = 0;
		NDISTREAM_READ( rIStream, &nSize, sizeof(nSize) )
			if (nSize > 0)
			{
				if ( nSize > m_strData.capacity() )
				{
					m_strData.reserve( nSize + 8 );
				}

				char* pszData = (char*)m_strData.c_str();
				NDISTREAM_READ( rIStream, pszData, nSize )
				pszData[nSize] = '\0';

				m_strData = pszData;
			}

			return true;
	}

	void   Clear() 
	{
		m_strData.clear();
	}

public:
	std::string m_strData;
};

#endif