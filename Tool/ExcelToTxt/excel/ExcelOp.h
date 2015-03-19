#pragma once

#include <comdef.h>
#include "excel.h"


class CExcelOp
{
public:
	CExcelOp(void);
	~CExcelOp(void);

	bool	Init();													//初始化;
	void	Release();												//释放操作;

	void	ReleaseDispatch();										//释放操作;

	bool	OpenExcel( char* szFileName );							//打开EXCEL文件;
	bool	OpenSheet( int nIndex );								//打开SHEET工作表(从一开始);
	bool	OpenSheet( char* szSheetName );							//打开名称为szSheetName的SHEET工作表;

	CString	GetCellValue( int nRow, int nCol );						//获得cell的字符串;
	CString	GetCellValue( char* szRowName, char* szColName );		//获得cell的字符串;

	int		GetRowCount();											//获得SHEET工作表已经使用的行数;
	int		GetColumnCount();										//获得SHEET工作表已经使用的列数;

private:
	Workbooks	books;			//工作薄集合;
	_Workbook	book;			//工作薄;
	Worksheets	sheets;			//工作表集合;
	_Worksheet	sheet;			//工作表;
	Range		range;			//Excel中针对单元格的操作都应先获取其对应的Range对象;
	
	COleVariant	m_oleOptional;

	static	_Application	app;//Excel应用程序接口;
};


