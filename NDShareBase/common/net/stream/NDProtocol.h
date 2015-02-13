/********************************************************************
	created:	2014/10/01
	filename: 	d:\code\NDServer2\NDShareBase\common\net\stream\NDProtocol.h
	file base:	NDProtocol
	purpose:	protocol base class;
	author:		fanxiangdong;
	mail:		fanxiangdong7@126.com;
	qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_PROTOCOL_H__
#define __SHARE_BASE_ND_PROTOCOL_H__

#include "NDTypes.h"

_NDSHAREBASE_BEGIN

class NDOStream;
class NDIStream;

class NDProtocol
{
public:
	const NDUint32 m_unProtocolID;
public:
	NDProtocol( NDUint32 unProtocolID ) : m_unProtocolID( unProtocolID ) {}

	virtual ~NDProtocol(){}
	virtual NDBool Serialize( NDOStream& rOStream ) = 0;
	virtual NDBool Deserialize( NDIStream& rIStream ) = 0;

	virtual void   Clear() {};								// whether must have or not

private:
	NDProtocol(const NDProtocol& other);
	NDProtocol& operator=(const NDProtocol& other);
};


_NDSHAREBASE_END
#endif
