#pragma once

#include <comdef.h>
#include "excel.h"


class CExcelOp
{
public:
	CExcelOp(void);
	~CExcelOp(void);

	bool	Init();													//��ʼ��;
	void	Release();												//�ͷŲ���;

	void	ReleaseDispatch();										//�ͷŲ���;

	bool	OpenExcel( char* szFileName );							//��EXCEL�ļ�;
	bool	OpenSheet( int nIndex );								//��SHEET������(��һ��ʼ);
	bool	OpenSheet( char* szSheetName );							//������ΪszSheetName��SHEET������;

	CString	GetCellValue( int nRow, int nCol );						//���cell���ַ���;
	CString	GetCellValue( char* szRowName, char* szColName );		//���cell���ַ���;

	int		GetRowCount();											//���SHEET�������Ѿ�ʹ�õ�����;
	int		GetColumnCount();										//���SHEET�������Ѿ�ʹ�õ�����;

private:
	Workbooks	books;			//����������;
	_Workbook	book;			//������;
	Worksheets	sheets;			//��������;
	_Worksheet	sheet;			//������;
	Range		range;			//Excel����Ե�Ԫ��Ĳ�����Ӧ�Ȼ�ȡ���Ӧ��Range����;
	
	COleVariant	m_oleOptional;

	static	_Application	app;//ExcelӦ�ó���ӿ�;
};


