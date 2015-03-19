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
		AfxMessageBox(_T("初始化Com失败!"));   
		return false;  
	}  

	//验证office文件是否可以正确运行; 
	if( !app.CreateDispatch(_T("Excel.Application")) )  
	{  
		AfxMessageBox(_T("无法创建Excel应用！"));  
		return false;  
	}

	/*判断当前Excel的版本*/
	int iStart = 0;

	CString strExcelVersion = app.GetVersion();
	strExcelVersion = strExcelVersion.Tokenize(_T("."), iStart);
	if (_T("11") == strExcelVersion)
	{
		AfxMessageBox(_T("当前Excel的版本是2003。"));
	}
	else if (_T("12") == strExcelVersion)
	{
		AfxMessageBox(_T("当前Excel的版本是2007。"));
	}
	else if (_T("14") == strExcelVersion)
	{
		AfxMessageBox(_T("当前Excel的版本是2010。"));
	}
	else
	{
		AfxMessageBox(_T("当前Excel的版本是其他版本。"));
	}

	app.SetVisible(FALSE);			//使Excel不可见;
	app.SetUserControl(FALSE);		//不允许其它用户控制Excel;

	return true;
}

void CExcelOp::Release()
{
	ReleaseDispatch();

	//退出程序;
	app.Quit();
	//app一定要释放，否则程序结束后还会有一个Excel进程驻留在内存中，而且程序重复运行的时候会出错;
	app.ReleaseDispatch();

	::CoUninitialize();
}


void CExcelOp::ReleaseDispatch()
{	//释放对象相当重要;
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
		strFileName.Format( "%s 文件不存在.", szFileName );

		AfxMessageBox(_T( strFileName.GetBuffer() ));
		return false;
	}

	books = app.GetWorkbooks();

	COleVariant covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);

	//接口指针;
	LPDISPATCH lpDisp = books.Open(szFileName,  
		covOptional, covOptional, covOptional, covOptional,  
		covOptional, covOptional, covOptional, covOptional,  
		covOptional, covOptional, covOptional, covOptional,  
		covOptional, covOptional  
		);                                      //与office 2000的不同，是个参数的，直接在后面加了两个covOptional成功了;  
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
	value=range.GetItem(varRow,varCol);                    //返回的类型是VT_DISPATCH 这是一个指针;
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
		//为了解决FLOAT和整数值无法兼容问题;
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
		value=range.GetItem(_variant_t(row),_variant_t(col));                  //返回的类型是VT_DISPATCH 这是一个指针; 
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
		value=range.GetItem(_variant_t(row),_variant_t(col));                  //返回的类型是VT_DISPATCH 这是一个指针;  
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



