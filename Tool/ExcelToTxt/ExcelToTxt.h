// ExcelToTxt.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

// CExcelToTxtApp:
// See ExcelToTxt.cpp for the implementation of this class
//

class CExcelToTxtApp : public CWinApp
{
public:
	CExcelToTxtApp();
	~CExcelToTxtApp();

// Overrides
	public:
	virtual BOOL	InitInstance();
	virtual int		ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()

public:
	bool	WriteContentToUTF8File( const char* szFileName, CString& strANSI );
	bool	CloseProcess( const char* szProcessName );
private:

	void	ConvertANSIToUTF8( CString &strANSI );
	void	ConvertUTF8ToANSI( CString &strUTF8 );	
};

extern CExcelToTxtApp theApp;