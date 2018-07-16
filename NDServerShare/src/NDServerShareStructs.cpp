#include "NDServerShareStructs.h"

#include "NDShareBaseGlobal.h"

using NDShareBase::NDShareBaseGlobal;



NDBool NDPlayerAccount::setAccount(const char* szAccount)
{
	NDUint32 nLen = 0;
	if ( NDFalse == NDShareBaseGlobal::isValidStrParam( szAccount, nLen ) )
	{
		return NDFalse;
	}

	NDShareBaseGlobal::safeStrCopy( m_szAccount, szAccount, NDACCOUNT_TABLE_ACCOUNT_LEN, nLen );

	return NDTrue;
}

NDBool NDPlayerAccount::setPassWord(const char* szPassWord)
{
	NDUint32 nLen = 0;
	if ( NDFalse == NDShareBaseGlobal::isValidStrParam( szPassWord, nLen ) )
	{
		return NDFalse;
	}

	NDShareBaseGlobal::safeStrCopy( m_szPassWord, szPassWord, NDACCOUNT_TABLE_PASSWORD_LEN, nLen );

	return NDTrue;
}

NDBool NDPlayerAccount::setRegTime(const char* szRegTime)
{
	NDUint32 nLen = 0;
	if ( NDFalse == NDShareBaseGlobal::isValidStrParam( szRegTime, nLen ) )
	{
		return NDFalse;
	}

	NDShareBaseGlobal::safeStrCopy( m_szRegTime, szRegTime, ND_TIME_STRLEN_MIN, nLen );

	return NDTrue;
}

NDBool NDPlayerAccount::setLastLoginTime(const char* szLastTime)
{
	NDUint32 nLen = 0;
	if ( NDFalse == NDShareBaseGlobal::isValidStrParam( szLastTime, nLen ) )
	{
		return NDFalse;
	}

	NDShareBaseGlobal::safeStrCopy( m_szLastLoginTime, szLastTime, ND_TIME_STRLEN_MIN, nLen );

	return NDTrue;
}

NDBool NDPlayerMain::setPlayerName(const char* szPlayerName)
{
	NDUint32 nLen = 0;
	if ( NDFalse == NDShareBaseGlobal::isValidStrParam( szPlayerName, nLen ) )
	{
		return NDFalse;
	}

	NDShareBaseGlobal::safeStrCopy( m_szPlayerName, szPlayerName, NDPLAYERMAIN_TABLE_PLAYERNAME_LEN, nLen );

	return NDTrue;
}

