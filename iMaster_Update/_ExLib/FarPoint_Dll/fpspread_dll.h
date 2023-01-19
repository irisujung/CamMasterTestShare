#pragma once

#include <vector>
using namespace std;




#include "Ssdllmfc.h"

// -------------------------------------- SPREAD CONFIG 간편화 -----------------------------------//
// Spread Config용 Cell Type 정의
enum DEF_SPREAD_CELL_TYPE {
	SPREAD_CELL_TYPE_NUMBER,
	SPREAD_CELL_TYPE_COMBO,
	SPREAD_CELL_TYPE_TEXT,
	SPREAD_CELL_TYPE_FILE,
	SPREAD_CELL_TYPE_FOLDER,
};
// Spread Config용 ITEM 정의
enum DEF_SPREAD_CFG {
	SPREAD_CFG_ITEM,
	SPREAD_CFG_SUBITEM,
	SPREAD_CFG_RANGE_MIN,
	SPREAD_CFG_RANGE_MAX,
	SPREAD_CFG_UNIT,
	SPREAD_CFG_DESCRIPTION,
	SPREAD_CFG_COUNT
};
// Spread Config용 Cell Type 구조체
typedef struct tagstSpreadData_
{
	int nCellType;
	int nDigits;
	double dMin;
	double dMax;
	CString strComboText;
	int nComboCount;
}stSpreadData_;
// Spread Config용 구조체
typedef struct tagstSpreadCfg_
{
	// Data 구조체 리스트
	vector<stSpreadData_> vecData;
	CString strItemText[SPREAD_CFG_COUNT];

	tagstSpreadCfg_()
	{
		vecData.clear();
	};
	~tagstSpreadCfg_()
	{
		vecData.clear();
	};
}stSpreadCfg_;

// Spread Data Vector
typedef vector<stSpreadData_> VECTOR_SPREAD_DATA;
// Spread Config Vector
typedef vector<stSpreadCfg_> VECTOR_SPREAD_CFG;

// -------------------------------------- SPREAD CONFIG 간편화 -----------------------------------//

class CFpspread_dll :
	public TSpread
{
public:
	CFpspread_dll(void);
	~CFpspread_dll(void);
protected:
	CString m_strComboText;
	BOOL m_bFocused; // Dialog에서 WM_COMMAND처리를 해야함.
	int m_nClickCol;
	int m_nClickRow;
	CFpspread_dll* m_pDummySpread;
public:
	void SetDummySpread(CFpspread_dll* pDummySpread);
	void SetClickRowCol(int nCol,int nRow);
	void GetClickRowCol(int* pCol,int* pRow);
	void SetFocus(BOOL bFocus);
	BOOL GetFocus();
	void SetHeaderHide();
	void SetCellTypeCheckBox(int nCol, int nRow, CString strCaption);
	void SetCellTypeNumber(int nCol, int nRow, int nDigits, double dMin, double dMax, BOOL bLock = FALSE);
	void SetCellTypeButton(int nCol,int nRow,CString strCaption,BOOL bLock=FALSE);
	void SetCellTypeCombo(int nCol, int nRow, CString strData, int nCount, int nDefault, BOOL bLock = FALSE);
	void SetCellTypeBmp(int nCol, int nRow, CString strPath);
	void SetCellTypeText(int nCol, int nRow, CString strText);
	void SetCellTypeEdit(int nCol, int nRow, int nMaxLen = 16, int nChrSet = SS_CHRSET_ALPHANUM, int nChrCase = SS_CASE_UCASE);

	void CopySpread(CFpspread_dll* pSpread);
	void CopySpread2(CFpspread_dll* pSpread);
	BOOL CompareSpread(CFpspread_dll* pSpread);
	BOOL CompareSpreadCell(int nCol,int nRow,CFpspread_dll* pSpread);
	void Block_SetColorAll(COLORREF cForeColor,COLORREF cBackColor);
	void Set_Init(int nMaxCol, int nMaxRow, long lColorBack, long lColorBorder, CString strFont, float fFontSize);
	void SetMonitoringMode();
	void SetFixedMode();
	void SetAllDataRowHeight(double dHeight);
	void SetAllDataColWidth(double dWidth);
	void Block_SetColor(int nSt_Col,int nEnd_Col,int nSt_Row,int nEnd_Row,COLORREF cForeColor,COLORREF cBackColor);
	void Block_SetBackColor(int nSt_Col,int nEnd_Col,int nSt_Row,int nEnd_Row,COLORREF cBackColor);
	
	void Block_SetAlignTextCenter();
	void Block_SetAlignText(int nSt_Col, int nEnd_Col, int nSt_Row, int nEnd_Row, int nAlign_H, int nAlign_V);
	void Block_SetTypeMultiLine(int nSt_Col,int nEnd_Col);

	void Block_SetHeaderText_Row(int nCol,int nStRow,CString strText);
	void Block_SetHeaderText_Col(int nStCol,int nRow,CString strText);
	
	void SetText_AddCellSpan(int nCol,int nRow,int nColNum,int nRowNum,CString strText);

	void SetScrollBars(long propVal);

	void UpdateCellTextForeColor(int nCol,int nRow,CString strText,COLORREF cColor);
	void UpdateCellTextBackColor(int nCol,int nRow,CString strText,COLORREF cColor);
	void UpdateCellText(int nCol,int nRow,CString strText);
	void UpdateCellForeColor(int nCol,int nRow,COLORREF cColor);
	void UpdateCellBackColor(int nCol,int nRow,COLORREF cColor);

	void SetChangeShow(BOOL bShow);

	void SetText(int nCol, int nRow, CString strValue);
	void SetText(int nCol, int nRow, int nValue);
	CString GetText(int nCol, int nRow);
	long Get_CellType(int nCol, int nRow);
	void Set_BlockLock(int nCol, int nRow, int nCol2, int nRow2, BOOL bLock);
	void Set_AllBlockLock(BOOL bLock=TRUE);
	void Set_Font(int nCol, int nRow, int nCol2, int nRow2, CString strFont, double dFontSize, BOOL bBold);
	BOOL IsCellSpanCols(int nCol,int nRow, int& nStartCol);
	BOOL IsCellSpanRows(int nCol,int nRow, int& nStartRow);
	static void ChangeCfgSpread(CFpspread_dll* pSpread_SV,CFpspread_dll* pSpread_PV,long Col,long Row, BOOL bSkipLock=FALSE);
	BOOL InputCellNumber(int nCol, int nRow);

	void SetCheckValue(long lCol, long lRow, int nValue);
	int  GetCheckValue(long lCol, long lRow);

	
	BOOL CopyCellType(int nCol, int nRow,CFpspread_dll* pSource);
	BOOL IsValidValue(int nCol, int nRow,CString strInput,CString& strRange);
	void SetScrollBarTrackBoth();
	void UpdateCellData(int nCol, int nRow, CString strPreText, CString strText);
	void UpdateChangeCell(int nCol, int nRow, CString strPV,CString strSV);

	void SetCellBorder(long lCol, long lRow, long lCol2, long lRow2, long nIndex, unsigned long crColor, long nStyle);
	void SetGrayAreaBackColor(COLORREF cBackColor);

	void put_RowHeight(long lRow, double newValue);
	void put_ColWidth(long lCol, double newValue);

	void SetTypeComboBoxCurSel(long lCol, long lRow, int nSelIndex);
	void SetTypeComboBoxCurSel(long lCol, long lRow, CString strText);

	int GetTypeComboBoxCurSel(long lCol, long lRow);
	CString GetTypeComboBoxCurSelText(long lCol, long lRow);


	double GetTypeNumberMin(long lCol, long lRow);
	double GetTypeNumberMax(long lCol, long lRow);
	void Set_BlockDisable(int nCol, int nRow, int nCol2, int nRow2, BOOL bDisable);

	void SetForeColorRange(long lCol, long lRow, long lCol2, long lRow2, COLORREF cForeColor);

	void SetCellBorderSolidLine();
	
	BOOL IsDigits(CString strData, BOOL& bExp);
};
