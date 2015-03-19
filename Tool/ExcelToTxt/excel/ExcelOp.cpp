#include "StdAfx.h"
#include "ExcelOp.h"

_Application CExcelOp::app;

CExcelOp::CExcelOp(void)
{
}

CExcelOp::~CExcelOp(void)
{
}

bool CExcelOp::Init()
{
	if (::CoInitialize( NULL ) == E_INVALIDARG)   
	{   
		AfxMessageBox(_T("��ʼ��Comʧ��!"));   
		return false;  
	}  

	//��֤office�ļ��Ƿ������ȷ����; 
	if( !app.CreateDispatch(_T("Excel.Application")) )  
	{  
		AfxMessageBox(_T("�޷�����ExcelӦ�ã�"));  
		return false;  
	}

	/*�жϵ�ǰExcel�İ汾*/
	int iStart = 0;

	CString strExcelVersion = app.GetVersion();
	strExcelVersion = strExcelVersion.Tokenize(_T("."), iStart);
	if (_T("11") == strExcelVersion)
	{
		AfxMessageBox(_T("��ǰExcel�İ汾��2003��"));
	}
	else if (_T("12") == strExcelVersion)
	{
		AfxMessageBox(_T("��ǰExcel�İ汾��2007��"));
	}
	else if (_T("14") == strExcelVersion)
	{
		AfxMessageBox(_T("��ǰExcel�İ汾��2010��"));
	}
	else
	{
		AfxMessageBox(_T("��ǰExcel�İ汾�������汾��"));
	}

	app.SetVisible(FALSE);			//ʹExcel���ɼ�;
	app.SetUserControl(FALSE);		//�����������û�����Excel;

	return true;
}

void CExcelOp::Release()
{
	ReleaseDispatch();

	//�˳�����;
	app.Quit();
	//appһ��Ҫ�ͷţ������������󻹻���һ��Excel����פ�����ڴ��У����ҳ����ظ����е�ʱ������;
	app.ReleaseDispatch();

	::CoUninitialize();
}


void CExcelOp::ReleaseDispatch()
{	//�ͷŶ����൱��Ҫ;
	range.ReleaseDispatch();
	sheet.ReleaseDispatch();
	sheets.ReleaseDispatch();
	book.ReleaseDispatch();	
	books.ReleaseDispatch();
}


bool CExcelOp::OpenExcel( char* szFileName )
{
	CFileFind fileFind;
	if ( !fileFind.FindFile( szFileName ) )
	{
		CString strFileName;
		strFileName.Format( "%s �ļ�������.", szFileName );

		AfxMessageBox(_T( strFileName.GetBuffer() ));
		return false;
	}

	books = app.GetWorkbooks();

	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	//�ӿ�ָ��;
	LPDISPATCH lpDisp = books.Open(szFileName,  
		covOptional, covOptional, covOptional, covOptional,  
		covOptional, covOptional, covOptional, covOptional,  
		covOptional, covOptional, covOptional, covOptional,  
		covOptional, covOptional  
		);                                      //��office 2000�Ĳ�ͬ���Ǹ������ģ�ֱ���ں����������covOptional�ɹ���;  
	book.AttachDispatch( lpDisp, TRUE );  
	
	sheets.AttachDispatch( book.GetSheets(), TRUE );

	return true;
}

bool CExcelOp::OpenSheet( int nIndex )
{
	int nShellCount = sheets.GetCount();
	if ( nIndex <= 0 || nIndex > nShellCount )
	{
		AfxMessageBox(_T( " shell index error! "));
		return false;
	}

	range.ReleaseDispatch();
	sheet.ReleaseDispatch();

	LPDISPATCH lpDisp = sheets.GetItem( _variant_t((long)nIndex) );
	if ( NULL == lpDisp )
	{

		return false;
	}

	sheet.AttachDispatch( lpDisp, TRUE );
	range.AttachDispatch( sheet.GetCells(), TRUE );

	return true;
}

bool CExcelOp::OpenSheet( char* szSheetName )
{
	if ( NULL == szSheetName || '\0' == szSheetName[0] )
	{
		return false;
	}

	range.ReleaseDispatch();
	sheet.ReleaseDispatch();

	LPDISPATCH lpDisp = sheets.GetItem( _variant_t(szSheetName) );
	if ( NULL == lpDisp )
	{
		return false;
	}

	sheet.AttachDispatch( lpDisp, TRUE );
	range.AttachDispatch( sheet.GetCells(), TRUE );

	return true;
}

CString CExcelOp::GetCellValue( int nRow, int nCol )
{
	_variant_t varRow( nRow );  
	_variant_t varCol( nCol );
	COleVariant value; 

	range.AttachDispatch( sheet.GetCells(),TRUE );  
	value=range.GetItem(varRow,varCol);                    //���ص�������VT_DISPATCH ����һ��ָ��;
	range.AttachDispatch(value.pdispVal,TRUE);  
	VARIANT value2=range.GetValue2();  

	CString strValue;

	if ( value2.vt == VT_BSTR )
	{
		strValue = value2.bstrVal;
	}
	else if ( value2.vt == VT_INT )
	{
		strValue.Format( "%d", value2.pintVal );
	}
	else if ( value2.vt == VT_R8 )
	{
		//Ϊ�˽��FLOAT������ֵ�޷���������;
		CString valueTemp;
		valueTemp.Format( "%.3f", value2.dblVal );
		int nLen = valueTemp.GetLength();
		char szFirstChar	= valueTemp[nLen - 3];
		char szSecondChar	= valueTemp[nLen - 2];
		char szThirdChar	= valueTemp[nLen - 1];
		if ( szThirdChar == '0' && szSecondChar == '0' && szThirdChar == '0' )
		{
			strValue.Format( "%d", (int)value2.dblVal );
		}
		else
		{
			strValue.Format( "%.3f", value2.dblVal );
		}
	}
	else if( value2.vt == VT_EMPTY )
	{
		strValue = "";
	}

	range.ReleaseDispatch();

	return strValue;
}

CString CExcelOp::GetCellValue( char* szRowName, char* szColName )
{
	COleVariant value;  
	CString strValue;  
	long row=0,col=0;  
	long re_row=0,re_col=0;  
	range.AttachDispatch( sheet.GetCells(),TRUE );  

	for (row=1,col=1; col<range.GetColumn(); col++)  
	{  
		value=range.GetItem(_variant_t(row),_variant_t(col));                  //���ص�������VT_DISPATCH ����һ��ָ��; 
		range.AttachDispatch(value.pdispVal,TRUE);  
		VARIANT value2=range.GetValue2();  
		CString strValue( value2.bstrVal );
		if (strValue==szColName)  
			break;  
	}  
	re_col=col;
	range.ReleaseDispatch();

	for (row=1,row=1; row<range.GetRow(); row++)
	{  
		value=range.GetItem(_variant_t(row),_variant_t(col));                  //���ص�������VT_DISPATCH ����һ��ָ��;  
		range.AttachDispatch(value.pdispVal,TRUE);  
		VARIANT value2=range.GetValue2();  
		CString strValue(value2.bstrVal);  
		if (strValue==szRowName)        
			break;  
	}  
	re_row=row;
	range.ReleaseDispatch();

	return GetCellValue( re_row, re_col );
}

int CExcelOp::GetRowCount()
{
	Range usedRange;
	usedRange.AttachDispatch( sheet.GetUsedRange(), TRUE );
	range.AttachDispatch( usedRange.GetRows(), TRUE );
	int nRowCount = range.GetCount();
	
	usedRange.ReleaseDispatch();
	range.ReleaseDispatch();

	return nRowCount;
}

int CExcelOp::GetColumnCount()
{
	Range usedRange;
	usedRange.AttachDispatch( sheet.GetUsedRange(), TRUE );
	range.AttachDispatch( usedRange.GetColumns(), TRUE );
	int nColumnCount = range.GetCount();

	usedRange.ReleaseDispatch();
	range.ReleaseDispatch();

	return nColumnCount;
}



