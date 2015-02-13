#include "authentic/NDCrypt.h"

#include <algorithm>


_NDSHAREBASE_BEGIN

NDCrypt::NDCrypt()
{
	init();

	keyVec.clear();
}

NDCrypt::~NDCrypt()
{

}

void NDCrypt::init()
{
	nSendI = 0;
	nSendJ = 0;
	nRecvI = 0;
	nRecvJ = 0;
}

void NDCrypt::setKey( const NDUint8* szKey, NDUint32 nLen )
{
	keyVec.resize( nLen );
	std::copy( szKey, szKey + nLen, keyVec.begin() );
}

void NDCrypt::encryptSend( NDUint8* szData, NDUint32 nLen )
{
	NDUint32 nKeySize = (NDUint32)keyVec.size();
	if ( nKeySize == 0 )
	{
		nKeySize = setDefaultKey();
	}

	for ( NDUint32 i = 0; i < nLen; ++i )
	{
		nSendI %= nKeySize;
		szData[i] = nSendJ = (szData[i] ^ keyVec[nSendI] ) + nSendJ;
		++nSendI;
	}
}

void NDCrypt::decryptRecv( NDUint8* szData, NDUint32 nLen )
{
	NDUint32 nKeySize = (NDUint32)keyVec.size();
	if ( nKeySize == 0 )
	{
		nKeySize = setDefaultKey();
	}

	char cValue = 0;
	for ( NDUint32 i = 0; i < nLen; ++i )
	{
		nRecvI %= keyVec.size();
		cValue = (szData[i] - nRecvJ) ^ keyVec[nRecvI];
		++nRecvI;
		nRecvJ = szData[i];
		szData[i] = cValue;
	}
}

NDUint32 NDCrypt::setDefaultKey()
{
	//char basis_64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	//char szBase64[] = "A0aBCD1bcd2 E3eFGH4fgh5 IJKLMNOP ijklmnop Q6qRST7rst8 U9uVWX+vwx/ YyzZ";
	char szBase64[]	= "A0aBCD1bcd2E3eFGH4fgh5IJKLMNOPijklmnopQ6qRST7rst8U9uVWX+vwx/YyzZ";
	NDUint32 nRet = (NDUint32)strlen(szBase64);

	setKey( (const NDUint8*)szBase64, nRet );
	return nRet;
}

_NDSHAREBASE_END
