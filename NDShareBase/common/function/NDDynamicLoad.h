/********************************************************************
created:	2014/10/01
filename: 	d:\code\NDServer2\NDShareBase\common\function\NDDynamicLoad.h
file base:	NDRefCounted
purpose:	dynamic load dynamic library(dll or so);
author:		fanxiangdong;
mail:		fanxiangdong7@126.com;
qq:			435337751;
*********************************************************************/
#ifndef __SHARE_BASE_ND_DYNAMIC_LOAD_H__
#define __SHARE_BASE_ND_DYNAMIC_LOAD_H__

#include "NDTypes.h"

_NDSHAREBASE_BEGIN

class NDDynamicLoad
{
private:
	void*	m_handle;
#ifdef WIN32
	char	m_szMsgBuf[256];
#endif

public:
	NDDynamicLoad();
	virtual ~NDDynamicLoad();

protected:
	void*	getProcAddress( const char* szSymbol );
	char*	getError();

	virtual bool initLibrary( const char* szLibraryFile );
	virtual bool load() = 0;
};

_NDSHAREBASE_END

#endif