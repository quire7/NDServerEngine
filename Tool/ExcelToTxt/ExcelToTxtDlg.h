// ExcelToTxtDlg.h : header file
//

#pragma once

#include <vector>
using std::vector;

#include "excel/ExcelOp.h"

// CExcelToTxtDlg dialog
class CExcelToTxtDlg : public CDialog
{
// Construction
public:
	CExcelToTxtDlg(CWnd* pParent = NULL);	// standard constructor
	~CExcelToTxtDlg();

// Dialog Data
	enum { IDD = IDD_EXCELTOTXT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CString			m_strTxtPath;
	CString			m_strExcelPath;

	CString			m_strANSITxtPath;
	CString			m_strUTF8TxtPath;
	vector<CString>	m_allExcelFileNameVec;

	
	CProgressCtrl*	m_pProgressCtrl;
	CListBox*		m_pListBox;

	bool			m_bIsRun;

	CExcelOp		m_CExcelOp;

public:
	bool	OpenExcelToTxt( const char* szExcelFileName, const char* szANSITxtFileName, const char* szUTF8TxtFileName );
	bool	ExcelToTxtRun( CString& strExcelTxtName );
	
	bool	GetAllExcelFileName( CString& strDir, vector<CString>& refExcelFileNameVec );
public:

	afx_msg void OnBnClickedButtonExcelPath();	//选择EXCEL文件的路径;
	afx_msg void OnBnClickedButtonTxtPath();	//选择TXT文件的路径;
	afx_msg void OnBnClickedRun();				//生成TXT;


};
