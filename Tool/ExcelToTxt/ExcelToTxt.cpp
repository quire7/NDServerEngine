// ExcelToTxt.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ExcelToTxt.h"
#include "ExcelToTxtDlg.h"

#include <TlHelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CExcelToTxtApp

BEGIN_MESSAGE_MAP(CExcelToTxtApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CExcelToTxtApp construction

CExcelToTxtApp::CExcelToTxtApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


CExcelToTxtApp::~CExcelToTxtApp()
{
	CloseProcess("EXCEL.EXE");
}


// The one and only CExcelToTxtApp object

CExcelToTxtApp theApp;


// CExcelToTxtApp initialization

BOOL CExcelToTxtApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

/*	if (::CoInitialize( NULL ) == E_INVALIDARG)   
	{   
		AfxMessageBox(_T("初始化Com失败!"));   
		return false;  
	}*/ 

	AfxEnableControlContainer();


	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CExcelToTxtDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		//CString strDir( "D:/code/Tool/ExcelToTxt/debug/" );
		//vector<CString>	allExcelFileNameVec;
		//GetAllExcelFileName( strDir, allExcelFileNameVec );

		//OpenExcelToTxt( "D:/code/Tool/ExcelToTxt/debug/crew_up_cost.xlsx", "crew_up_cost.txt" );
		
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel

		CloseProcess("EXCEL.EXE");
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CExcelToTxtApp::ConvertANSIToUTF8( CString &strANSI )
{
	int nLen = ::MultiByteToWideChar( CP_ACP, MB_ERR_INVALID_CHARS, strANSI.GetBuffer(), -1, NULL, 0 );
	WCHAR* wszUTF8 = new WCHAR[nLen + 1];
	memset( wszUTF8, 0, (nLen + 1)*sizeof(WCHAR) );
	nLen = ::MultiByteToWideChar( CP_ACP, 0, strANSI.GetBuffer(), -1, wszUTF8, nLen );

	nLen = ::WideCharToMultiByte( CP_UTF8, 0, wszUTF8, -1, NULL, 0, NULL, NULL );
	char* szUTF8 = new char[nLen + 1];
	memset( szUTF8, 0, nLen + 1 );
	::WideCharToMultiByte( CP_UTF8, 0, wszUTF8, -1, szUTF8, nLen, NULL, NULL );

	strANSI = szUTF8;

	delete	[]wszUTF8;
	delete	[]szUTF8;
}

void CExcelToTxtApp::ConvertUTF8ToANSI( CString &strUTF8 )
{
	int nLen = ::MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, strUTF8.GetBuffer(), -1, NULL, 0 );
	WCHAR* wszANSI = new WCHAR[nLen + 1];
	memset( wszANSI, 0, (nLen + 1)*sizeof(WCHAR) );
	nLen = ::MultiByteToWideChar( CP_UTF8, 0, strUTF8.GetBuffer(), -1, wszANSI, nLen );

	nLen = ::WideCharToMultiByte( CP_ACP, 0, wszANSI, -1, NULL, 0, NULL, NULL );
	char* szANSI = new char[nLen + 1];
	memset( szANSI, 0, nLen + 1 );
	::WideCharToMultiByte( CP_ACP, 0, wszANSI, -1, szANSI, nLen, NULL, NULL );

	strUTF8 = szANSI;

	delete	[]wszANSI;
	delete	[]szANSI;
}

bool CExcelToTxtApp::WriteContentToUTF8File( const char* szFileName, CString& strANSI )
{
	if ( NULL == szFileName || '\0' == szFileName[0] )
	{
		return false;
	}

	CFile cFile;
	BOOL bRet = cFile.Open( szFileName, CFile::modeCreate | CFile::modeWrite );
	if ( !bRet )
	{
		return false;
	}

	ConvertANSIToUTF8( strANSI );
	
	//文件开头;
	cFile.SeekToBegin();
	//写BOM,同样低位写在前面;
	cFile.Write( "\xef\xbb\xbf", 3 );
	//写入内容;
	cFile.Write( strANSI.GetBuffer(), strANSI.GetLength() );
	cFile.Flush();
	cFile.Close();

	return true;
}

bool CExcelToTxtApp::CloseProcess( const char* szProcessName )
{
	//创建系统快照;
	HANDLE hSnapshot = ::CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, NULL );
	if ( INVALID_HANDLE_VALUE == hSnapshot )
	{
		return false;
	}

	PROCESSENTRY32	lppe;
	lppe.dwSize		= sizeof(lppe);

	BOOL bFind = Process32First( hSnapshot, &lppe );
	if ( bFind )
	{
		if ( 0 == strcmp( szProcessName, lppe.szExeFile ) )
		{
			HANDLE hProcess = ::OpenProcess( PROCESS_ALL_ACCESS, FALSE, lppe.th32ProcessID );
			if ( NULL != hProcess )
			{
				TerminateProcess( hProcess, 0 );
				CloseHandle( hProcess );
			}
		}
		bFind = Process32Next( hSnapshot, &lppe );
	}

	CloseHandle( hSnapshot );

	return true;
}

int CExcelToTxtApp::ExitInstance()
{
	//::CoUninitialize();

	return CWinApp::ExitInstance();
}

