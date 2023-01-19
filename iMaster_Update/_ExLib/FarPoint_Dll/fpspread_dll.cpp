#include "pch.h"
#include "fpspread_dll.h"

CFpspread_dll::CFpspread_dll(void)
{
	m_strComboText = _T("");
	m_bFocused = TRUE;
	m_nClickCol = -1;
	m_nClickRow = -1;
	m_pDummySpread = NULL;
}

CFpspread_dll::~CFpspread_dll(void)
{
}
void CFpspread_dll::SetDummySpread(CFpspread_dll* pDummySpread)
{
	m_pDummySpread = pDummySpread;
}

void CFpspread_dll::SetClickRowCol(int nCol,int nRow)
{
	m_nClickCol = nCol;
	m_nClickRow = nRow;
}
void CFpspread_dll::GetClickRowCol(int* pCol,int* pRow)
{
	if(pCol!=NULL)
	{
		*pCol = m_nClickCol;
	}
	if(pRow!=NULL)
	{
		*pRow = m_nClickRow;
	}
}

void CFpspread_dll::SetFocus(BOOL bFocus)
{
	m_bFocused = bFocus;

	if(m_bFocused==FALSE)
	{
		m_nClickCol = -1;
		m_nClickRow = -1;
	}
}
BOOL CFpspread_dll::GetFocus()
{
	return m_bFocused;
}
void CFpspread_dll::SetCellTypeCombo(int nCol,int nRow,CString strData,int nCount,int nDefault,BOOL bLock)
{
	SS_CELLTYPE CellType;
	memset(&CellType, 0, sizeof(CellType));

	m_strComboText = strData;
	this->SetTypeComboBox(&CellType,SSS_ALIGN_VCENTER|SSS_ALIGN_CENTER,strData);
	this->SetCellType(nCol,nRow,&CellType);

	this->SetLock(nCol,nRow,bLock);

	/* COMBO BOX 아이템 리스트 문자열로 추출하기 샘플 코드(2016.10.05)
	// Cell Type 데이터 얻기
	SS_CELLTYPE cellTypeCheck;
	this->GetCellType(nCol,nRow,&cellTypeCheck);

	// 문자열 메모리 할당
	HGLOBAL pSrc;
	pSrc = GlobalLock(cellTypeCheck.Spec.ComboBox.hItems);  
	// 아이템 개수가져오기
	int nItems = cellTypeCheck.Spec.ComboBox.dItemCnt;
	// 포인터 변경
	BYTE* pData = (BYTE*)pSrc;
	// 문자열 인덱스 변수
	int nStrIndex=0;
	// 아이템 개수만큼 반복하기
	for(int nCnt=0;nCnt<nItems;nCnt++)
	{
		// 문자열 생성
		char cTempData[512] = {0,};
		int nDataCount = sprintf_s(cTempData,"%s", &pData[nStrIndex]);
		// 추출된 데이터
		TRACE2("Idx=%d, %s",nCnt+1,cTempData);
		nDataCount+=1;// NULL 문자개수 증가
		nStrIndex+=nDataCount;// Index 증가
	}
	// 문자열 메모리 해제
	GlobalFree(pSrc);*/
}
void CFpspread_dll::SetCellTypeBmp(int nCol, int nRow, CString strPath)
{
	SS_CELLTYPE CellType;
	memset(&CellType, 0, sizeof(CellType));
	this->SetTypePicture(&CellType, VPS_MAINTAINSCALE | VPS_STRETCH | VPS_BMP | SSS_ALIGN_VCENTER | SSS_ALIGN_CENTER, strPath);
	this->SetCellType(nCol, nRow, &CellType);
}
void CFpspread_dll::SetCellTypeText(int nCol, int nRow, CString strText)
{
	SS_CELLTYPE CellType;
	memset(&CellType, 0, sizeof(CellType));
	this->SetTypeEdit(&CellType, SS_TEXT_CENTER | SS_TEXT_VCENTER,16, SS_CHRSET_ALPHANUM, SS_CASE_UCASE);
	this->SetCellType(nCol, nRow, &CellType);
}

void CFpspread_dll::SetCellTypeEdit(int nCol, int nRow, int nMaxLen, int nChrSet, int nChrCase)
{
	SS_CELLTYPE CellType;
	memset(&CellType, 0, sizeof(CellType));
	this->SetTypeEdit(&CellType, SSS_ALIGN_CENTER | SSS_ALIGN_VCENTER, nMaxLen, nChrSet, nChrCase);
	this->SetCellType(nCol, nRow, &CellType);
}

void CFpspread_dll::Block_SetColorAll(COLORREF cForeColor,COLORREF cBackColor)
{
	this->SetColorRange(1,1,this->GetMaxCols(),this->GetMaxRows(),cBackColor,cForeColor);
}
void CFpspread_dll::SetHeaderHide()
{
	this->ShowRow(SS_HEADER,FALSE);
	this->ShowCol(SS_HEADER,FALSE);
}

void CFpspread_dll::SetCellTypeCheckBox(int nCol, int nRow, CString strCaption)
{
	SS_CELLTYPE cellType;
	memset(&cellType, 0, sizeof(cellType));

	this->SetTypeCheckBox(
		&cellType,
		BS_CENTER, strCaption,
		NULL, BT_NONE,
		NULL, BT_NONE,
		NULL, BT_NONE,
		NULL, BT_NONE,
		NULL, BT_NONE,
		NULL, BT_NONE
	);
	this->SetCellType(nCol, nRow, &cellType);
}

void CFpspread_dll::SetCellTypeNumber(int nCol,int nRow,int nDigits,double dMin,double dMax,BOOL bLock)
{
	SS_CELLTYPE cellType;
	memset(&cellType, 0, sizeof(cellType));

	if(nDigits==0)
	{
		this->SetTypeIntegerExt(&cellType, SSS_ALIGN_VCENTER|SSS_ALIGN_CENTER, (long)dMin, (long)dMax, FALSE, 0); 
	}
	else
	{
		this->SetTypeNumber(&cellType, SSS_ALIGN_VCENTER|SSS_ALIGN_CENTER, nDigits, dMin, dMax, FALSE); 
	}

	this->SetCellType(nCol,nRow,&cellType);
	this->SetLock(nCol,nRow,bLock);
}
void CFpspread_dll::SetCellTypeButton(int nCol,int nRow,CString strCaption,BOOL bLock)
{
	SS_CELLTYPE cellType;
	memset(&cellType, 0, sizeof(cellType));

	this->SetTypeButton(&cellType,SBS_TEXTTEXT,strCaption,NULL,SUPERBTN_PICT_NONE,NULL,SUPERBTN_PICT_NONE,SUPERBTN_TYPE_NORMAL,1,NULL);
	this->SetCellType(nCol,nRow,&cellType);
	this->SetLock(nCol,nRow,bLock);
}
BOOL CFpspread_dll::CompareSpreadCell(int nCol,int nRow,CFpspread_dll* pSpread)
{
	CString strSV = this->GetText(nCol,nRow);
	CString strPV = pSpread->GetText(nCol,nRow);

	if(strSV!=strPV)
	{
		pSpread->UpdateCellForeColor(nCol,nRow,COLOR_RED);
		return TRUE;
	}
	pSpread->UpdateCellForeColor(nCol,nRow,COLOR_BLACK);
	return FALSE;
}
BOOL CFpspread_dll::CompareSpread(CFpspread_dll* pSpread)
{
	int nMaxCols=this->GetMaxCols();
	int nMaxRows=this->GetMaxRows();

	for(int nRow=1;nRow<=nMaxRows;nRow++)
	{
		for(int nCol=1;nCol<=nMaxCols;nCol++)
		{
			TCHAR cTemp1[512];
			TCHAR cTemp2[512];

			ZeroMemory(cTemp1,sizeof(cTemp1));
			ZeroMemory(cTemp2,sizeof(cTemp2));

			pSpread->GetData(nCol,nRow,cTemp1);
			this->GetData(nCol,nRow,cTemp2);
			
			if(memcmp(cTemp1,cTemp2,sizeof(cTemp1))!=0)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}
void CFpspread_dll::CopySpread2(CFpspread_dll* pSpread)
{
	int nSheetCount = pSpread->GetSheetCount();

	this->Reset();

	this->SetSheetCount(nSheetCount);
	for (int nSheet = 0; nSheet < nSheetCount; nSheet++)
	{
		this->SetSheet(nSheet + 1);
		pSpread->SetSheet(nSheet + 1);
		CString strSheetName;
		TCHAR cData[512] = { 0, };
		pSpread->GetSheetName(nSheet + 1, cData, 511);
		strSheetName.Format(_T("%s"), cData);
		this->SetSheetName(nSheet + 1, strSheetName);

		this->SetMaxCols(pSpread->GetMaxCols());
		this->SetMaxRows(pSpread->GetMaxRows());

		int nMaxCols = this->GetMaxCols();
		int nMaxRows = this->GetMaxRows();

		for (int nRow = 1; nRow <= nMaxRows; nRow++)
		{
			for (int nCol = 1; nCol <= nMaxCols; nCol++)
			{
				TCHAR cData[512] = { 0, };
				pSpread->GetData(nCol, nRow, cData);
				this->SetData(nCol, nRow, cData);
			}
		}
	}
}

void CFpspread_dll::CopySpread(CFpspread_dll* pSpread)
{
	this->SetMaxCols(pSpread->GetMaxCols());
	this->SetMaxRows(pSpread->GetMaxRows());

	int nMaxCols=this->GetMaxCols();
	int nMaxRows=this->GetMaxRows();

	for(int nRow=1;nRow<=nMaxRows;nRow++)
	{
		for(int nCol=1;nCol<=nMaxCols;nCol++)
		{
			TCHAR cData[512]={0,};
			pSpread->GetData(nCol,nRow,cData);
			this->SetData(nCol,nRow,cData);
		}
	}
}
void CFpspread_dll::Block_SetBackColor(int nSt_Col,int nEnd_Col,int nSt_Row,int nEnd_Row,COLORREF cBackColor)
{
	SetColorRange(nSt_Col, nSt_Row, nEnd_Col, nEnd_Row, cBackColor, COLOR_BLACK); 
}
void CFpspread_dll::Block_SetColor(int nSt_Col,int nEnd_Col,int nSt_Row,int nEnd_Row,COLORREF cForeColor,COLORREF cBackColor)
{
	SetColorRange(nSt_Col, nSt_Row, nEnd_Col, nEnd_Row, cBackColor, cForeColor); 
}
void CFpspread_dll::Block_SetAlignTextCenter()
{
	SS_CELLTYPE cellType;
	memset(&cellType, 0, sizeof(cellType));

	//this->SetTypeStaticText(&cellType,SS_TEXT_CENTER|SS_TEXT_VCENTER);
	this->SetTypeEdit(&cellType,ES_MULTILINE|SSS_ALIGN_CENTER|SSS_ALIGN_VCENTER,512,SS_CHRSET_CHR,SS_CASE_NOCASE);
//	this->SetTypeStaticText(&cellType,SSS_ALIGN_CENTER|SSS_ALIGN_VCENTER);
	//SS_TT_MULTILINE_MULTI
	this->SetCellType(SS_ALLCOLS, SS_ALLROWS, &cellType); 
}
void CFpspread_dll::Block_SetTypeMultiLine(int nSt_Col, int nEnd_Col)
{
	SS_CELLTYPE cellType;
	memset(&cellType, 0, sizeof(cellType));

	//this->SetTypeStaticText(&cellType,SS_TEXT_CENTER|SS_TEXT_VCENTER);
	this->SetTypeEdit(&cellType, ES_MULTILINE | SSS_ALIGN_CENTER | SSS_ALIGN_VCENTER, 512, SS_CHRSET_CHR, SS_CASE_NOCASE);

	this->SetCellTypeRange(nSt_Col, nEnd_Col, nSt_Col, nEnd_Col, &cellType);
}
void CFpspread_dll::Block_SetAlignText(int nSt_Col,int nEnd_Col,int nSt_Row,int nEnd_Row,int nAlign_H,int nAlign_V)
{
	SS_CELLTYPE cellType;
	memset(&cellType, 0, sizeof(cellType));


	long nStyle = 0;

	nStyle = ES_MULTILINE;

	if(nAlign_H==0)
	{
		nStyle = nStyle | SSS_ALIGN_LEFT;
	}
	else if(nAlign_H==1)
	{
		nStyle = nStyle | SSS_ALIGN_RIGHT;
	}
	else if(nAlign_H==2)
	{
		nStyle = nStyle | SSS_ALIGN_CENTER;
	}
	if(nAlign_V==0)
	{
		nStyle = nStyle | SSS_ALIGN_TOP;
	}
	else if(nAlign_V==1)
	{
		nStyle = nStyle | SSS_ALIGN_BOTTOM;
	}
	else if(nAlign_V==2)
	{
		nStyle = nStyle | SSS_ALIGN_VCENTER;
	}

	this->SetTypeEdit(&cellType,nStyle,512,SS_CHRSET_CHR,SS_CASE_NOCASE);

	this->SetCellTypeRange(nSt_Col,nSt_Row,nEnd_Col,nEnd_Row, &cellType); 
}


void CFpspread_dll::Block_SetHeaderText_Row(int nCol,int nStRow,CString strText)
{
	int nMaxRows = this->GetMaxRows();
	int nRow = nStRow;
	while(1)
	{
		if(nRow>nMaxRows)
			break;
		// Parsing
		int nIndex = strText.Find(_T(","));
		if(nIndex!=-1) // Find & Next
		{
			CString strTemp;
			strTemp = strText.Left(nIndex);
			strText.Delete(0,nIndex+1);
			SetText(nCol, nRow, (strTemp));
		}
		else
		{
			if(!strText.IsEmpty())
			{
				SetText(nCol, nRow, (strText));
			}
			break;
		}
		nRow++;
	}
}
void CFpspread_dll::Block_SetHeaderText_Col(int nStCol,int nRow,CString strText)
{
	int nMaxCols = this->GetMaxCols();
	int nCol = nStCol;
	while(1)
	{
		if(nCol>nMaxCols)
			break;
		// Parsing
		int nIndex = strText.Find(_T(","));
		if(nIndex!=-1) // Find & Next
		{
			CString strTemp;
			strTemp = strText.Left(nIndex);
			strText.Delete(0,nIndex+1);
			SetText(nCol, nRow, (strTemp));
		}
		else
		{
			if(!strText.IsEmpty())
			{
				SetText(nCol, nRow, (strText));
			}
			break;
		}
		nCol++;
	}

}

void CFpspread_dll::Set_Init(int nMaxCol, int nMaxRow, long lColorBack, long lColorBorder, CString strFont, float fFontSize)
{
/*	this->SetCursorStyle(this->CursorStyleArrow);
	this->SetActiveCellHighlightStyle(this->ActiveCellHighlightStyleOff);
	this->SetEditMode(FALSE);
	this->SetAllowDragDrop(FALSE);	
	this->SetUserResize(this->UserResizeNone);
	this->SetTypeVAlign(this->TypeVAlignCenter);
	this->SetTypeHAlign(this->TypeHAlignCenter);
	this->SetTypeTextWordWrap(FALSE);
	this->SetMaxCols(nMaxCol);
	this->SetMaxRows(nMaxRow);
	this->SetGrayAreaBackColor(lColorBack);
	this->SetScrollBars(this->ScrollBarsNone);	
	this->SetColHeaderRows(0);
	this->SetRowHeaderCols(0);
	this->SetDisplayRowHeaders(FALSE);
	this->SetDisplayColHeaders(FALSE);
	this->SetColHeadersAutoText(this->DispBlank);
	this->SetRowHeadersAutoText(this->DispBlank);
	this->SetCellBorder(1, 1, nMaxCol, nMaxRow, 15, lColorBorder, this->CellBorderStyleSolid);

	// 글자크기를 줄이자..
	this->SetCol(1);
	this->SetRow(1);
	this->SetCol2(nMaxCol);
	this->SetRow2(nMaxRow);
	this->SetBlockMode(TRUE);
	this->SetCellType(this->CellTypeStaticText);
	this->SetTypeTextWordWrap(TRUE);
	this->SetTypeHAlign(this->TypeHAlignCenter);
	this->SetTypeVAlign(this->TypeVAlignCenter);
	this->SetFontBold(FALSE);
	this->SetFontName(strFont);
	this->SetFontSize(fFontSize);
	this->SetBackColor(lColorBack);
	this->SetBlockMode(FALSE);

	this->SetCol(1);
	this->SetRow(1);
	this->SetCol2(1);
	this->SetRow2(nMaxRow);
	this->SetBlockMode(TRUE);	
	this->SetFontBold(TRUE);
	this->SetBlockMode(FALSE);

	this->SetCol(1);
	this->SetRow(1);
	this->SetCol2(nMaxCol);
	this->SetRow2(1);
	this->SetBlockMode(TRUE);	
	this->SetFontBold(TRUE);
	this->SetBlockMode(FALSE);

	this->SetCol(1);
	this->SetRow(1);
	this->SetCol2(nMaxCol);
	this->SetRow2(nMaxRow);
	this->SetBlockMode(TRUE);
	this->SetLock(TRUE);
	this->SetBlockMode(FALSE);*/
}
void CFpspread_dll::SetFixedMode()
{
	//return;
	this->SetBool(SSB_ALLOWEDITOVERFLOW,FALSE);
	this->SetBool(SSB_ALLOWROWMOVE,FALSE);
	this->SetBool(SSB_ALLOWDRAGDROP,FALSE);
	this->SetBool(SSB_EDITMODEREPLACE,FALSE);
	this->SetBool(SSB_ALLOWMULTIBLOCKS,FALSE);
	this->SetBool(SSB_AUTOSIZE,FALSE);
	this->EnableWindow(TRUE);
}

void CFpspread_dll::SetMonitoringMode()
{
	this->SetBool(SSB_ALLOWDRAGDROP,FALSE);
	this->SetBool(SSB_ALLOWMULTIBLOCKS,FALSE);
	this->SetBool(SSB_AUTOSIZE,FALSE);
	this->SetBool(SSB_ALLOWEDITOVERFLOW,FALSE);
	this->SetBool(SSB_RETAINSELBLOCK,FALSE);
	this->EnableWindow(FALSE);
}
void CFpspread_dll::SetAllDataRowHeight(double dHeight)
{
	this->SetRowHeight(SS_ALLROWS,dHeight);
}
void CFpspread_dll::SetAllDataColWidth(double dWidth)
{
	this->SetColWidth(SS_ALLCOLS,dWidth);
}
void CFpspread_dll::SetText_AddCellSpan(int nCol,int nRow,int nColNum,int nRowNum,CString strText)
{
	SetText(nCol, nRow, (strText));
	AddCellSpan(nCol, nRow, nColNum, nRowNum);
}

void CFpspread_dll::UpdateCellTextForeColor(int nCol,int nRow,CString strText,COLORREF cColor)
{
	if(nCol>this->GetMaxCols())
	{
		return;
	}
	if(nRow>this->GetMaxRows())
	{
		return;
	}
	CString strPre;
	strPre = this->GetText(nCol,nRow);

	COLORREF cBack=0,cFore=0;
	this->GetColor(nCol,nRow,&cBack,&cFore);

	if((strText!=strPre)||(cFore!=cColor))
	{
		this->SetText(nCol,nRow,strText);
		this->SetColor(nCol,nRow,cBack,cColor);
	}
}
void CFpspread_dll::UpdateCellTextBackColor(int nCol,int nRow,CString strText,COLORREF cColor)
{
	if(nCol>this->GetMaxCols())
	{
		return;
	}
	if(nRow>this->GetMaxRows())
	{
		return;
	}
	CString strPre;
	strPre = this->GetText(nCol,nRow);

	COLORREF cBack=0,cFore=0;
	this->GetColor(nCol,nRow,&cBack,&cFore);

	if((strText!=strPre)||(cBack!=cColor))
	{
		this->SetText(nCol,nRow,strText);
		this->SetColor(nCol,nRow,cColor,cFore);
	}
}
void CFpspread_dll::UpdateCellText(int nCol,int nRow,CString strText)
{
	if(nCol>this->GetMaxCols())
	{
		return;
	}
	if(nRow>this->GetMaxRows())
	{
		return;
	}
	CString strPre;
	strPre = this->GetText(nCol,nRow);

	if((strText!=strPre))
	{
		this->SetText(nCol,nRow,strText);
	}

	/*
	if( this->CellTypeTest == this->CellTypeCheckBox )
		this->put_EventEnabled(this->EventButtonClicked,FALSE);

	if(strText!=strPre)
	{
		this->SetText(strText);
	}

	if( this->CellTypeTest == this->CellTypeCheckBox )
		this->put_EventEnabled(this->EventButtonClicked,TRUE);*/
}
void CFpspread_dll::UpdateCellForeColor(int nCol,int nRow,COLORREF cColor)
{
	if(nCol>this->GetMaxCols())
	{
		return;
	}
	if(nRow>this->GetMaxRows())
	{
		return;
	}

	COLORREF cBack=0,cFore=0;

	this->GetColor(nCol, nRow, &cBack, &cFore);

	if(cFore==cColor)
	{
		return;
	}

	this->SetColor(nCol, nRow, cBack, cColor);
}
void CFpspread_dll::UpdateCellBackColor(int nCol,int nRow,COLORREF cColor)
{
	if(nCol>this->GetMaxCols())
	{
		return;
	}
	if(nRow>this->GetMaxRows())
	{
		return;
	}

	COLORREF cBack=0,cFore=0;

	this->GetColor(nCol, nRow, &cBack, &cFore);

	if(cBack==cColor)
	{
		return;
	}

	this->SetColor(nCol, nRow, cColor, cFore);
}

void CFpspread_dll::SetChangeShow(BOOL bShow)
{
	this->ShowWindow(bShow?SW_SHOW:SW_HIDE);
}

void CFpspread_dll::SetText(int nCol, int nRow, CString strValue)
{
#ifdef _UNICODE
	this->SetData(nCol, nRow, strValue);
	
#else
	this->SetData(nCol, nRow, strValue);
#endif // _UNICODE

	
}
void CFpspread_dll::SetText(int nCol, int nRow, int nValue)
{
	CString strValue;
	strValue.Format(_T("%d"), nValue);
	this->SetData(nCol, nRow, strValue);
}

CString CFpspread_dll::GetText(int nCol, int nRow)
{
	TCHAR cTemp[512];
	ZeroMemory(cTemp,sizeof(cTemp));

	this->GetData(nCol,nRow,cTemp);
	CString strTemp;
	strTemp.Format(_T("%s"),cTemp);

	return strTemp;
}


long CFpspread_dll::Get_CellType(int nCol, int nRow)
{
	SS_CELLTYPE cellType;
	memset(&cellType, 0, sizeof(cellType));

	this->GetCellType(nCol,nRow,&cellType);
	return cellType.Type;
}


void CFpspread_dll::Set_AllBlockLock(BOOL bLock)
{
	SetLockRange(1,1,this->GetMaxCols(),this->GetMaxRows(),bLock);
}
void CFpspread_dll::Set_BlockLock(int nCol, int nRow, int nCol2, int nRow2, BOOL bLock)
{
	SetLockRange(nCol,nRow,nCol2,nRow2,bLock);
}
void CFpspread_dll::Set_Font(int nCol, int nRow, int nCol2, int nRow2, CString strFont, double dFontSize, BOOL bBold)
{
	CFont hFontText;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));       
	lf.lfHeight = (long)dFontSize;
	if(bBold==TRUE)
		lf.lfWeight = FW_BOLD;

#ifdef _UNICODE
	wcscpy_s(lf.lfFaceName, strFont);
#else
	strcpy_s(lf.lfFaceName, strFont);
#endif
		
	VERIFY(hFontText.CreateFontIndirect(&lf));  

	this->SetFontRange(nCol, nRow, nCol2, nRow2, (HFONT)hFontText.GetSafeHandle(), TRUE); 

	hFontText.DeleteObject();
}

BOOL CFpspread_dll::IsCellSpanCols(int nCol,int nRow, int& nStartCol)
{
	SS_COORD varCol=0,varRow=0,varNumCols=0,varNumRows=0;
	//SS_COORD lCol, SS_COORD lRow, LPSS_COORD lplColAnchor, LPSS_COORD lplRowAnchor, 	LPSS_COORD lplNumCols, LPSS_COORD lplNumRows
	GetCellSpan(nCol,nRow,&varCol,&varRow,&varNumCols,&varNumRows);

	if(varNumCols>1)
	{
		nStartCol = varCol;
		return TRUE;
	}
	return FALSE;
}
BOOL CFpspread_dll::IsCellSpanRows(int nCol,int nRow, int& nStartRow)
{
	SS_COORD varCol=0,varRow=0,varNumCols=0,varNumRows=0;
	//SS_COORD lCol, SS_COORD lRow, LPSS_COORD lplColAnchor, LPSS_COORD lplRowAnchor, 	LPSS_COORD lplNumCols, LPSS_COORD lplNumRows
	GetCellSpan(nCol,nRow,&varCol,&varRow,&varNumCols,&varNumRows);

	if(varNumRows>1)
	{
		nStartRow = varRow;
		return TRUE;
	}
	return FALSE;
}

BOOL CFpspread_dll::CopyCellType(int nCol, int nRow,CFpspread_dll* pSource)
{
	SS_CELLTYPE cellType;
	memset(&cellType, 0, sizeof(cellType));

	pSource->GetCellType(nCol,nRow,&cellType);

	return this->SetCellType(nCol,nRow,&cellType);
}
BOOL CFpspread_dll::IsValidValue(int nCol, int nRow,CString strInput,CString& strRange)
{
	SS_CELLTYPE cellType;
	memset(&cellType, 0, sizeof(cellType));

	this->GetCellType(nCol,nRow,&cellType);

	cellType.Spec.Number.Min;
//	cellType.Float.Min;

	long nInput = _ttoi(strInput);
	double dInput = _ttoi(strInput);
	double dMinValue = cellType.Spec.Number.Min;
	double dMaxValue = cellType.Spec.Number.Max;
	long nMinValue = (long)dMinValue;
	long nMaxValue = (long)dMaxValue;
	
	
	if(cellType.Type==SS_TYPE_NUMBER)
	{
		if(cellType.Spec.Number.Right==0)
		{
			if(!(nMinValue<=nInput&&nInput<=nMaxValue))
			{
				strRange.Format(_T("입력 값이 잘못되었습니다.(%d ~ %d)"),nMinValue,nMaxValue);
				return FALSE;
			}
		}
		else
		{
			if(!(dMinValue<=dInput&&dInput<=dMaxValue))
			{
				strRange.Format(_T("입력 값이 잘못되었습니다.(%f ~ %f)"),dMinValue,dMaxValue);
				return FALSE;
			}
		}
	}
	else if(cellType.Type==SS_TYPE_SCIENTIFIC)
	{
		dMinValue = cellType.Spec.Scientific.Min;
		dMaxValue = cellType.Spec.Scientific.Max;

		if(!(dMinValue<=dInput&&dInput<=dMaxValue))
		{
			strRange.Format(_T("입력 값이 잘못되었습니다.(%02.02E ~ %02.02E)"),dMinValue,dMaxValue);
			return FALSE;
		}

	}
	else if(cellType.Type==SS_TYPE_COMBOBOX)
	{
		CString strTemp = m_strComboText;

		vector<long> vecListInt;
		vector<CString> vecListStr;
		int nStart=0;
		BOOL bDigits=TRUE;
		CString strRangeString;
		strRangeString = strTemp;
		strRangeString.Replace(_T("\t"),_T(","));
		while(1)
		{
			CString strBlock = strTemp.Tokenize(_T("\t"),nStart);
			if(nStart==-1)
				break;
			vecListStr.push_back(strBlock);
			BOOL bTemp;
			if(IsDigits(strBlock,bTemp)==FALSE)
			{
				bDigits = FALSE;
			}
			else
			{
				long nTemp = _ttoi(strBlock);
				vecListInt.push_back(nTemp);
			}
		}

		BOOL bResult = FALSE;
		if(bDigits==TRUE)
		{
			int nSize = (int)vecListInt.size();
			for(int nCnt=0;nCnt<nSize;nCnt++)
			{
				long nTemp = vecListInt.at(nCnt);
				if(nTemp==nInput)
				{
					bResult = TRUE;
					break;
				}
			}

		}
		else
		{
			int nSize = (int)vecListStr.size();
			for(int nCnt=0;nCnt<nSize;nCnt++)
			{
				CString strTemp = vecListStr.at(nCnt);
				if(strTemp==strInput)
				{
					bResult = TRUE;
					break;
				}
			}
		}
		strRange.Format(_T("입력 값이 잘못되었습니다.(%s)"),strRangeString);

		vecListStr.clear();
		vecListInt.clear();

		return bResult;

	}
	else
	{
		return FALSE;
	}
	return TRUE;
}
void CFpspread_dll::UpdateCellData(int nCol, int nRow,CString strPreText,CString strText)
{/*
	long nInput = atoi(strText);
	double dInput = atof(strText);

	long nPreInput = atoi(strPreText);
	double dPreInput = atof(strPreText);


	if(this->CellTypeTest==this->CellTypeNumber)
	{
		if(this->GetTypeNumberDecPlaces()==0)
		{
			if(nInput!=nPreInput)
			{
				this->SetFloat(nCol,nRow,(double)nInput);
				this->SetForeColor(COLOR_RED);
			}
		}
		else
		{
			if(dInput!=dPreInput)
			{
				this->SetFloat(nCol,nRow,dInput);
				this->SetForeColor(COLOR_RED);
			}
		}
	}
	else if(this->CellTypeTest==this->CellTypeScientific)
	{
		if(dInput!=dPreInput)
		{
			this->SetText(strText);
			this->SetForeColor(COLOR_RED);
		}
	}
	else if(this->CellTypeTest==this->CellTypeComboBox)
	{
		if(strPreText!=strText)
		{
			this->SetText(strText);
			this->SetForeColor(COLOR_RED);
		}
	}*/
}
void CFpspread_dll::UpdateChangeCell(int nCol, int nRow, CString strPV,CString strSV)
{/*
	if(strSV.IsEmpty())
	{
		CString strMsg;
		strMsg.Format("입력(붙여넣기) 값이 잘못되었습니다.이전 설정(PV)으로 되돌아 갑니다.");
		MSG_BOX(strMsg,MSG_ICON_CONFIRMATION);
		this->SetText(strPV);
		this->UpdateCellForeColor(nCol,nRow,COLOR_MAGENTA_LOW);

		return;
	}

	this->SetCol(nCol);
	this->SetRow(nRow);

	long nInput = atoi(strSV);
	double dInput = atof(strSV);
	long nPreInput = atoi(strPV);
	double dPreInput = atof(strPV);

	if(this->CellTypeTest==this->CellTypeNumber)
	{
		double dMinValue=0.0,dMaxValue=0.0;
		dMinValue = this->GetTypeNumberMin();
		dMaxValue = this->GetTypeNumberMax();
		long nMinValue = (long)this->GetTypeNumberMin();
		long nMaxValue = (long)this->GetTypeNumberMax();


		if(this->GetTypeNumberDecPlaces()==0)
		{
			if(dInput==dPreInput)
			{
				return;
			}
			if(nMinValue<=nInput&&nInput<=nMaxValue)
			{
				this->UpdateCellForeColor(nCol,nRow,COLOR_RED);
			}
			else
			{
				CString strMsg;
				strMsg.Format("입력(붙여넣기) 값이 잘못되었습니다.(%d ~ %d)\r\n이전 설정(PV)으로 되돌아 갑니다.",nMinValue,nMaxValue);
				MSG_BOX(strMsg,MSG_ICON_CONFIRMATION);
				this->SetText(strPV);
				this->UpdateCellForeColor(nCol,nRow,COLOR_BLUE);
			}
		}
		else
		{
			if(dInput==dPreInput)
			{
				return;
			}
			if(dMinValue<=dInput&&dInput<=dMaxValue)
			{
				this->UpdateCellForeColor(nCol,nRow,COLOR_RED);
			}
			else
			{
				CString strMsg;
				strMsg.Format("입력(붙여넣기) 값이 잘못되었습니다.(%f ~ %f)\r\n이전 설정(PV)으로 되돌아 갑니다.",dMinValue,dMaxValue);
				MSG_BOX(strMsg,MSG_ICON_CONFIRMATION);
				this->SetText(strPV);
				this->UpdateCellForeColor(nCol,nRow,COLOR_BLUE);
			}
		}
	}
	else if(this->CellTypeTest==this->CellTypeScientific)
	{
		double dMinValue=0.0,dMaxValue=0.0;
		dMinValue = this->GetTypeScientificMin();
		dMaxValue = this->GetTypeScientificMax();
		if(dMinValue<=dInput&&dInput<=dMaxValue)
		{
			if(dInput==dPreInput)
			{
				return;
			}
			this->UpdateCellForeColor(nCol,nRow,COLOR_RED);
		}
		else
		{
			CString strMsg;
			strMsg.Format("입력(붙여넣기) 값이 잘못되었습니다.(%f ~ %f)\r\n이전 설정(PV)으로 되돌아 갑니다.",dMinValue,dMaxValue);
			MSG_BOX(strMsg,MSG_ICON_CONFIRMATION);
			this->SetText(strPV);
			this->UpdateCellForeColor(nCol,nRow,COLOR_BLUE);
		}
	}
	else if(this->CellTypeTest==this->CellTypeComboBox)
	{
		if(strPV!=strSV)
		{
			this->UpdateCellForeColor(nCol,nRow,COLOR_RED);
		}
	}
	*/
}

void CFpspread_dll::ChangeCfgSpread(CFpspread_dll* pSpread_SV,CFpspread_dll* pSpread_PV,long Col,long Row, BOOL bSkipLock)
{
	if (pSpread_SV == NULL)
	{
		return;
	}
	if (pSpread_PV == NULL)
	{
		return;
	}
	if(pSpread_SV->GetLock(Col,Row)==TRUE&&bSkipLock==FALSE)
	{
		return; 
	}
	int nMaxCols = pSpread_SV->GetMaxCols();
	SS_CELLTYPE cellType;
	memset(&cellType, 0, sizeof(cellType));
	pSpread_SV->GetCellType(Col,Row,&cellType);
	if(cellType.Type==SS_TYPE_BUTTON)
	{
		return;
	}
	
	int nStartCol=0;
	if(pSpread_SV->IsCellSpanCols(Col,Row,nStartCol)==TRUE)
	{
		if (1<= nStartCol && nStartCol<=nMaxCols )
		{
			Col = nStartCol;
		}
	}



	CString strPV;
	strPV = pSpread_PV->GetText(Col, Row);

	CString strSV;
	strSV = pSpread_SV->GetText(Col, Row);

	if(strSV.IsEmpty())
	{
		//MSG_BOX("범위를 초과하는 값을 붙여넣기 하였습니다.\r\n이전 설정으로 돌아갑니다.",MSG_ICON_CONFIRMATION);
		//MSG_BOX("Input range in excess.\r\nTo return to the previous settings",MSG_ICON_CONFIRMATION);
		pSpread_SV->SetData(Col,Row,strPV);
		pSpread_SV->UpdateCellForeColor(Col,Row,COLOR_BLACK);
		return;
	}
	if(strPV!=strSV)
	{
		pSpread_SV->UpdateCellForeColor(Col,Row,COLOR_RED);
	}
	else
	{
		pSpread_SV->UpdateCellForeColor(Col,Row,COLOR_BLACK);
	}
}

BOOL CFpspread_dll::InputCellNumber(int nCol, int nRow)
{
	// 이전 데이터 가져오기
	CString strPre = this->GetText(nCol, nRow);
	// Item 항목가져오기
	CString strTitle = this->GetText(nCol - 1, nRow);
	// Keypad 입력
	CString strInput;
	if (0)//GetKeypadBox(strPre, strInput, strTitle))
	{
		double dMin = this->GetTypeNumberMin(nCol, nRow);
		double dMax = this->GetTypeNumberMax(nCol, nRow);
		double dInputValue = _ttof(strInput);
		if (!(dMin <= dInputValue&&dInputValue <= dMax))
		{
			//MSG_BOX("Input range in excess.\r\nTo return to the previous settings", MSG_ICON_CONFIRMATION);
			return FALSE;
		}
		// 데이터 입력
		this->SetText(nCol, nRow, (strInput));
		if (m_pDummySpread != NULL)
		{
			CFpspread_dll::ChangeCfgSpread(this, m_pDummySpread, nCol, nRow);
		}

		return TRUE;
	}

	return FALSE;
}

void CFpspread_dll::SetScrollBarTrackBoth()
{
	SetBool(SSB_HSCROLLBARTRACK,TRUE);
	SetBool(SSB_VSCROLLBARTRACK,TRUE);
}
void CFpspread_dll::SetScrollBars(long propVal)
{
	if(propVal==0)
	{
		this->SetBool(SSB_VERTSCROLLBAR,FALSE);
		this->SetBool(SSB_HORZSCROLLBAR,FALSE);
	}
	else if(propVal==1)
	{
		this->SetBool(SSB_VERTSCROLLBAR,FALSE);
		this->SetBool(SSB_HORZSCROLLBAR,TRUE);
	}
	else if(propVal==2)
	{
		this->SetBool(SSB_VERTSCROLLBAR,TRUE);
		this->SetBool(SSB_HORZSCROLLBAR,FALSE);
	}
	else if(propVal==3)
	{
		this->SetBool(SSB_VERTSCROLLBAR,TRUE);
		this->SetBool(SSB_HORZSCROLLBAR,TRUE);
	}
}
void CFpspread_dll::SetCellBorder(long lCol, long lRow, long lCol2, long lRow2, long nIndex, unsigned long crColor, long nStyle)
{
	this->SetBorderRange(lCol,lRow,lCol2,lRow2,(WORD)nIndex, (WORD)nStyle,crColor);
}

void CFpspread_dll::SetGrayAreaBackColor(COLORREF cBackColor)
{
	COLORREF cBackground=0,cForeground=0;
	this->GetGrayAreaColor(&cBackground,&cForeground);
	this->SetGrayAreaColor(cBackColor, cForeground);

}

void CFpspread_dll::put_RowHeight(long lRow, double newValue)
{
	this->SetRowHeight(lRow, newValue);
}
void CFpspread_dll::put_ColWidth(long lCol, double newValue)
{
	this->SetColWidth(lCol, newValue);
}
void CFpspread_dll::SetTypeComboBoxCurSel(long lCol, long lRow, int nSelIndex)
{
	this->ComboBoxSendMessage(lCol,lRow,SS_CBM_SETCURSEL,nSelIndex,0);
}
void CFpspread_dll::SetTypeComboBoxCurSel(long lCol, long lRow, CString strText)
{
	int nItems = (int)this->ComboBoxSendMessage(lCol, lRow, SS_CBM_GETCOUNT, NULL, NULL);
	int nFindIndex = -1;

	// 아이템 개수만큼 반복하기
	for(int nCnt=0;nCnt<nItems;nCnt++)
	{
		// 문자열 생성
		char cTempData[512] = {0,};
		ZeroMemory(cTempData, sizeof(cTempData));

		WPARAM wParam = nCnt;

		int nLen = (int)this->ComboBoxSendMessage(lCol, lRow, SS_CBM_GETLBTEXT, wParam, (LPARAM)cTempData);
		if (nLen != 0)
		{
			CString strIndex(cTempData, nLen);
			if (strIndex == strText)
			{
				nFindIndex = nCnt;
				break;
			}
		}
		
	}
	if (nFindIndex != -1)
	{
		SetTypeComboBoxCurSel(lCol, lRow, nFindIndex);
	}
}
int CFpspread_dll::GetTypeComboBoxCurSel(long lCol, long lRow)
{
	TCHAR cTemp[512];
	ZeroMemory(cTemp,sizeof(cTemp));

	this->GetValue(lCol,lRow,cTemp);
	CString strTemp;
	strTemp.Format(_T("%s"),cTemp);

	return _ttoi(strTemp);
}
CString CFpspread_dll::GetTypeComboBoxCurSelText(long lCol, long lRow)
{
	CString strText;
	char cTemp[512];
	ZeroMemory(cTemp, sizeof(cTemp));
	int nCurSel = (int)this->ComboBoxSendMessage(lCol, lRow, SS_CBM_GETCURSEL, NULL, NULL);
	WPARAM wParam = nCurSel;
	int nLen = (int)this->ComboBoxSendMessage(lCol, lRow, SS_CBM_GETLBTEXT, wParam, (LPARAM)cTemp);
	if (nLen != 0)
	{
		strText = CString(cTemp, nLen);
	}
	return strText;
}

double CFpspread_dll::GetTypeNumberMin(long lCol, long lRow)
{
	SS_CELLTYPE cellType;
	memset(&cellType, 0, sizeof(cellType));

	this->GetCellType(lCol,lRow,&cellType);
	if(cellType.Type==SS_TYPE_NUMBER)
	{
		return cellType.Spec.Number.Min;
	}
	else if(cellType.Type==SS_TYPE_INTEGER)
	{
		return cellType.Spec.Integer.Min;
	}
	else if(cellType.Type==SS_TYPE_FLOAT)
	{
		return cellType.Spec.Float.Min;
	}
	else
	{
		//MSG_BOX("신규 추가할것",MSG_ICON_CONFIRMATION);
	}
	return 0;
}
double CFpspread_dll::GetTypeNumberMax(long lCol, long lRow)
{
	SS_CELLTYPE cellType;
	memset(&cellType, 0, sizeof(cellType));

	this->GetCellType(lCol,lRow,&cellType);
	if(cellType.Type==SS_TYPE_NUMBER)
	{
		return cellType.Spec.Number.Max;
	}
	else if(cellType.Type==SS_TYPE_INTEGER)
	{
		return cellType.Spec.Integer.Max;
	}
	else if(cellType.Type==SS_TYPE_FLOAT)
	{
		return cellType.Spec.Float.Max;
	}
	else
	{
		//MSG_BOX("신규 추가할것",MSG_ICON_CONFIRMATION);
	}
	return 0;
}

void CFpspread_dll::Set_BlockDisable(int nCol, int nRow, int nCol2, int nRow2, BOOL bDisable)
{
	Set_BlockLock(nCol,nRow,nCol2,nRow2,bDisable);
	if(bDisable)		
	{
		//Block_SetColor(nCol,nCol2,nRow,nRow2,COLOR_SILVER_LIGHT,COLOR_SILVER_HLIGHT);
		Block_SetColor(nCol,nCol2,nRow,nRow2,COLOR_SLATEGRAY,COLOR_SILVER_HLIGHT);
	}
	else				
	{
		Block_SetColor(nCol,nCol2,nRow,nRow2,COLOR_BLACK,COLOR_WHITE);
	}
}
void CFpspread_dll::SetForeColorRange(long lCol, long lRow, long lCol2, long lRow2, COLORREF cForeColor)
{
	for(long nCol=lCol;nCol<=lCol2;nCol++)
	{
		for(long nRow=lRow;nRow<=lRow2;nRow++)
		{
			if(this->GetLock(nCol,nRow)==TRUE)
				continue;
			COLORREF cBack=0,cFore=0;
			this->GetColor(nCol,nRow,&cBack,&cFore);
			this->SetColor(nCol,nRow,cBack,cForeColor);
		}
	}
}
void CFpspread_dll::SetCellBorderSolidLine()
{
	this->SetCellBorder(1, 1, this->GetMaxCols(), this->GetMaxRows(), 1, COLOR_SLATEGRAY, 1);
	this->SetCellBorder(1, 1, this->GetMaxCols(), this->GetMaxRows(), 2, COLOR_SLATEGRAY, 1);
	this->SetCellBorder(1, 1, this->GetMaxCols(), this->GetMaxRows(), 4, COLOR_SLATEGRAY, 1);
	this->SetCellBorder(1, 1, this->GetMaxCols(), this->GetMaxRows(), 8, COLOR_SLATEGRAY, 1);

}

void CFpspread_dll::SetCheckValue(long lCol, long lRow, int nValue)
{
	CString strTemp;
	strTemp.Format(_T("%d"), nValue);
	this->SetValue(lCol, lRow, strTemp);
}
int  CFpspread_dll::GetCheckValue(long lCol, long lRow)
{
	const int nLen = this->GetValueLen(lCol, lRow);
	CString strTemp = CString("", nLen + 1);
	this->GetValue(lCol, lRow, strTemp.GetBuffer());
	int nResult = _ttoi(strTemp);
	strTemp.Empty();

	return nResult;
}


// CString Check Digits
BOOL CFpspread_dll::IsDigits(CString strData, BOOL& bExp)
{
	BOOL bFindDot = FALSE;
	BOOL bFindExp = FALSE;
	bExp = FALSE;
	int nLen = strData.GetLength();
	if (nLen == 0)
	{
		return FALSE;
	}
	for (int i = 0; i < nLen; i++)
	{

		TCHAR cTCHAR = strData.GetAt(i);
		if (isascii(cTCHAR) == FALSE)
		{
			return FALSE;
		}

		if (isdigit(cTCHAR) == FALSE)
		{
			if (cTCHAR == '.'&&bFindDot == FALSE)
			{
				bFindDot = TRUE;
			}
			else if ((cTCHAR == 'E' || cTCHAR == 'e') && bFindExp == FALSE)
			{
				TCHAR cTCHAR = strData.GetAt(i + 1);
				if (cTCHAR == '+' || cTCHAR == '-')
				{
					bFindExp = TRUE;
					i++;
				}
				else
				{
					return FALSE;
				}

			}
			else if (cTCHAR == '-'&&i == 0)
			{
				continue;
			}
			else if (cTCHAR == '+'&&i == 0)
			{
				continue;
			}
			else
			{
				return FALSE;
			}
		}
	}
	bExp = bFindExp;
	return TRUE;
}