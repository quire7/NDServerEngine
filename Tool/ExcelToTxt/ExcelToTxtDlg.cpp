// ExcelToTxtDlg.cpp : implementation file
//

#include "stdafx.h"

#include <direct.h>
#include <process.h>

#include "ExcelToTxt.h"
#include "ExcelToTxtDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CExcelToTxtDlg dialog




CExcelToTxtDlg::CExcelToTxtDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExcelToTxtDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pProgressCtrl = NULL;
	m_pListBox		= NULL;
	m_bIsRun		= false;
}


CExcelToTxtDlg::~CExcelToTxtDlg()
{
	m_CExcelOp.Release();
}

void CExcelToTxtDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CExcelToTxtDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_EXCEL_PATH, &CExcelToTxtDlg::OnBnClickedButtonExcelPath)
	ON_BN_CLICKED(IDC_BUTTON_TXT_PATH, &CExcelToTxtDlg::OnBnClickedButtonTxtPath)
	ON_BN_CLICKED(IDC_RUN, &CExcelToTxtDlg::OnBnClickedRun)
END_MESSAGE_MAP()


// CExcelToTxtDlg message handlers

BOOL CExcelToTxtDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here;

	if ( !m_CExcelOp.Init() )
	{
		return FALSE;
	}

	char szPath[MAX_PATH];
	::GetCurrentDirectory( MAX_PATH, szPath );

	SetDlgItemText( IDC_EDIT_EXCEL_PATH, szPath );
	SetDlgItemText( IDC_EDIT_TXT_PATH, szPath );

	m_strExcelPath	= szPath;
	m_strTxtPath	= szPath;

	m_pProgressCtrl = (CProgressCtrl*)GetDlgItem( IDC_PROGRESS1 );
	if ( NULL == m_pProgressCtrl )
	{
		return FALSE;
	}
	m_pListBox = (CListBox*)GetDlgItem( IDC_LIST1 );
	if ( NULL == m_pListBox )
	{
		return FALSE;
	}


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CExcelToTxtDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CExcelToTxtDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CExcelToTxtDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


bool CExcelToTxtDlg::ExcelToTxtRun( CString& strExcelTxtName )
{
	int nLen = strExcelTxtName.GetLength();
	
	char* szBuf	= new char[nLen + 1];
	char* pszBuf= szBuf;
	memcpy( pszBuf, strExcelTxtName.GetBuffer(), nLen );
	pszBuf[nLen] = '\0';

	//文件时间;
	strtok( pszBuf, "\t" );
	unsigned __int64 nFileTime = atol( pszBuf );
	//文件大小;
	pszBuf = strtok( NULL, "\t" );
	unsigned __int64 nFileSize = atol( pszBuf );
	//Excel文件名;
	pszBuf = strtok( NULL, "\t" );
	CString strExcelFilePath( m_strExcelPath );
	strExcelFilePath += "/";
	strExcelFilePath += pszBuf;
	//TXT文件名(ANSI);
	pszBuf = strtok( NULL, "\t" );
	CString strANSITxtFilePath( m_strANSITxtPath );
	strANSITxtFilePath += "/";
	strANSITxtFilePath += pszBuf;
	//TXT文件名(UTF8);
	CString strUTF8TxtFilePath( m_strUTF8TxtPath );
	strUTF8TxtFilePath += "/";
	strUTF8TxtFilePath += pszBuf;

	delete [] szBuf;
	szBuf = NULL;

	if ( OpenExcelToTxt( strExcelFilePath.GetBuffer(), strANSITxtFilePath.GetBuffer(), strUTF8TxtFilePath.GetBuffer() ) )
	{

	}
	
	return true;
}


bool CExcelToTxtDlg::OpenExcelToTxt( const char* szExcelFileName, const char* szANSITxtFileName, const char* szUTF8TxtFileName )
{
	if ( NULL == szExcelFileName || NULL == szANSITxtFileName || '\0' == szExcelFileName[0] || '\0' == szANSITxtFileName )
	{
		return false;
	}

	try
	{	
		if ( !m_CExcelOp.OpenExcel( (char*)szExcelFileName ) )
		{
			m_CExcelOp.ReleaseDispatch();
			return false;
		}

		if ( !m_CExcelOp.OpenSheet( 1 ) )
		{
			m_CExcelOp.ReleaseDispatch();
			return false;
		}

		int nRow = m_CExcelOp.GetRowCount();		//行;
		int nCol = m_CExcelOp.GetColumnCount();		//列;


		int nColIndex = 1;
		CString str;

		for ( int x = 1; x <= nRow; ++x )
		{
			if ( x > 2 )
			{
				nCol = nColIndex;						//以第二行的列数为基准(其实这个可以不要);
			}
			for ( int y = 1; y <= nCol; ++y )
			{
				CString addStr = m_CExcelOp.GetCellValue( x, y);
				if ( y == 1 && addStr == "" )
				{
					break;
				}

				if ( y < nCol )
				{
					if ( x <= 2 && ( "" == m_CExcelOp.GetCellValue( x, y + 1) ) )
					{
						str += addStr + "\r\n";
						break;
					}

					str += addStr + "\t";
					if ( x == 2 )
					{
						nColIndex++;			//以第二行的列数为基准(其实这个可以不要);
					}
				}
				else
				{
					str += addStr + "\r\n";
				}
			}
		}

		FILE* pFile = fopen( szANSITxtFileName, "wb+" );
		if ( NULL == pFile )
		{
			m_CExcelOp.ReleaseDispatch();
			return false;
		}
		int nLen	= str.GetLength();
		char* szBuf = str.GetBuffer();
		fwrite( szBuf, nLen, 1, pFile );
		fclose( pFile );

		if ( NULL != szUTF8TxtFileName && '\0' != szUTF8TxtFileName[0] )
		{
			theApp.WriteContentToUTF8File( szUTF8TxtFileName, str );
		}
		

		m_CExcelOp.ReleaseDispatch();

	}
	catch (CMemoryException* e)
	{
		char szBuf[256] = {0};
		_snprintf( szBuf, sizeof(szBuf) -1, " CMemoryException, run excel %s error!", szExcelFileName );

		AfxMessageBox(_T(szBuf));

		theApp.CloseProcess( "EXCEL.EXE" );

		return false;
	}
	catch (CFileException* e)
	{
		char szBuf[256] = {0};
		_snprintf( szBuf, sizeof(szBuf) -1, " CFileException, run excel %s error!", szExcelFileName );
		AfxMessageBox(_T(szBuf));

		theApp.CloseProcess( "EXCEL.EXE" );
		return false;
	}
	catch (CException* e)
	{
		TCHAR tszErrString[256] = {0};
		e->GetErrorMessage( tszErrString, sizeof( tszErrString ) );
		e->Delete();
		
		CString strError;
		strError.Format( _T( "Occur the exception: %s" ), tszErrString );

		char szBuf[256] = {0};
		_snprintf( szBuf, sizeof(szBuf) -1, " CException, run excel %s error!", szExcelFileName );
		AfxMessageBox(_T(szBuf));

		theApp.CloseProcess( "EXCEL.EXE" );
		return false;
	}

	return true;
}



bool CExcelToTxtDlg::GetAllExcelFileName( CString& strDir, vector<CString>& refExcelFileNameVec )
{
	//遍历文件下EXCEL文件,将所有EXCEL文件名添加到refExcelFileNameVec中;
	CString strDirTemp(strDir);
	strDirTemp += "\\*.xlsx";

	WIN32_FIND_DATA	findFileData;
	HANDLE hFind = ::FindFirstFile( strDirTemp, &findFileData );
	if ( INVALID_HANDLE_VALUE == hFind )
	{
		return false;
	}

	do 
	{
		if ( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			if ( findFileData.cFileName[0] != _T('.') )
			{

			}
		}
		else
		{
			CString strFileName( findFileData.cFileName );
			//strBuf的构造(时间\t文件大小\tExcelName\tTxtName);
			CString strBuf = "0\t0\t" + strFileName + "\t" + strFileName.Mid( 0, strFileName.Find(".") ) +".txt";
			refExcelFileNameVec.push_back( strBuf );
		}

		if ( !FindNextFile( hFind, &findFileData ) )
		{
			break;
		}

	} while (1);

	return true;
}

void CExcelToTxtDlg::OnBnClickedButtonExcelPath()
{
	// TODO: Add your control notification handler code here;
	char szSelectPath[MAX_PATH] = {0};	//用来存放文件夹路径;
	BROWSEINFO bi;
	bi.hwndOwner		= m_hWnd;
	bi.pidlRoot			= NULL;
	bi.pszDisplayName	= szSelectPath;
	bi.lpszTitle		= "选择输出文件路径";
	bi.ulFlags			= BIF_RETURNONLYFSDIRS;
	bi.lpfn				= NULL;
	bi.lParam			= NULL;
	bi.iImage			= NULL;

	LPITEMIDLIST	pidl = NULL;
	if ( NULL != ( pidl = SHBrowseForFolder(&bi) ) )
	{
		if ( SUCCEEDED( SHGetPathFromIDList( pidl, szSelectPath ) ) )
		{
			m_strExcelPath = szSelectPath;
			SetDlgItemText( IDC_EDIT_EXCEL_PATH, szSelectPath );	//显示路径名;
		}
	}
}

void CExcelToTxtDlg::OnBnClickedButtonTxtPath()
{
	// TODO: Add your control notification handler code here;
	char szSelectPath[MAX_PATH] = {0};	//用来存放文件夹路径;
	BROWSEINFO bi;
	bi.hwndOwner		= m_hWnd;
	bi.pidlRoot			= NULL;
	bi.pszDisplayName	= szSelectPath;
	bi.lpszTitle		= "选择输出文件路径";
	bi.ulFlags			= BIF_RETURNONLYFSDIRS;
	bi.lpfn				= NULL;
	bi.lParam			= NULL;
	bi.iImage			= NULL;

	LPITEMIDLIST	pidl = NULL;
	if ( NULL != ( pidl = SHBrowseForFolder(&bi) ) )
	{
		if ( SUCCEEDED( SHGetPathFromIDList( pidl, szSelectPath ) ) )
		{
			m_strTxtPath = szSelectPath;
			SetDlgItemText( IDC_EDIT_TXT_PATH, szSelectPath );	//显示路径名;
		}
	}
}

unsigned int WINAPI ExcelToTxtAllRun( void* pParam )
{
	if ( NULL == pParam )
	{
		return 0;
	}

	CExcelToTxtDlg* pThis = (CExcelToTxtDlg*)pParam;

	if ( pThis->m_allExcelFileNameVec.empty() )
	{
		return 1;
	}

	int nSize		= pThis->m_allExcelFileNameVec.size();
	float fPercent	= 100.0f / (float)nSize;	//进度条的百分比;
	int	nPercent	= fPercent;		
	int nCurPercent	= 0;

	for ( int i = 0; i < nSize; ++i )
	{
		CString &refStrExcelTxtName = pThis->m_allExcelFileNameVec[i];

		if ( pThis->ExcelToTxtRun( refStrExcelTxtName ) )
		{
			nCurPercent += nPercent;
			pThis->m_pProgressCtrl->SetPos( nCurPercent );
		}
	}


	return 0;
}

void CExcelToTxtDlg::OnBnClickedRun()
{
	// TODO: Add your control notification handler code here;
	
	if ( m_bIsRun )
	{
		::MessageBox( NULL, "请耐心等待完成!", "错误!", MB_OK );
		return;
	}

	m_bIsRun = true;

	//表格文件所在路径;
	CString strExcelPath( m_strExcelPath );
	strExcelPath += "/";
	GetAllExcelFileName( strExcelPath, m_allExcelFileNameVec );

	//TXT文件所在路径;
	m_strANSITxtPath = m_strTxtPath;
	m_strUTF8TxtPath = m_strTxtPath;
	m_strANSITxtPath += "/Config_for_server";
	m_strUTF8TxtPath += "/Config_for_client";
	//创建文件目录;
	_mkdir( m_strANSITxtPath.GetBuffer() );
	_mkdir( m_strUTF8TxtPath.GetBuffer() );

	
	m_pProgressCtrl->SetRange( 0, 100 ); //设置进度条范围;
	m_pListBox->ResetContent();			 //clear content;
	


	if ( m_allExcelFileNameVec.empty() )
	{
		return;
	}

	int nSize		= m_allExcelFileNameVec.size();
	float fPercent	= 100.0f / (float)nSize;	//进度条的百分比;
	int	nPercent	= fPercent;		
	int nCurPercent	= 0;

	for ( int i = 0; i < nSize; ++i )
	{
		CString &refStrExcelTxtName = m_allExcelFileNameVec[i];

		if ( ExcelToTxtRun( refStrExcelTxtName ) )
		{
			nCurPercent += nPercent;
			m_pProgressCtrl->SetPos( nCurPercent );
		}
	}

	m_bIsRun = false;

	//do run txt thread;
	//HANDLE hHANDLE = (HANDLE)_beginthreadex( NULL, 0, ExcelToTxtAllRun, this, 0, NULL );
	//CloseHandle( hHANDLE );
}
