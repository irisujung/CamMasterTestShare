// CSelectSymbolDlg.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "MainFrm.h"
#include "CSelectSymbolDlg.h"
#include "afxdialogex.h"


// CSelectSymbolDlg 대화 상자

IMPLEMENT_DYNAMIC(CSelectSymbolDlg, CDialogEx)

CSelectSymbolDlg::CSelectSymbolDlg(IN const BOOL &bPadMode, IN const CString &strLayerName, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDIT_SELECT_SYMBOL, pParent),
	m_bPadMode(bPadMode),
	m_strSelectLayerName(strLayerName)
{

}

CSelectSymbolDlg::~CSelectSymbolDlg()
{
}

void CSelectSymbolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TABLE_SYMBOL, m_Spread);
}


BEGIN_MESSAGE_MAP(CSelectSymbolDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CSelectSymbolDlg::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CSelectSymbolDlg::OnBnClickedButtonCancel)
END_MESSAGE_MAP()


// CSelectSymbolDlg 메시지 처리기

CString			CSelectSymbolDlg::GetSymbolName()
{
	return m_strSymbol;
}

vector<double>	CSelectSymbolDlg::GetSymbolPara()
{
	return m_vecPara_mm;
}

int				CSelectSymbolDlg::GetType()
{
	return m_nSelectShape;
}


UINT32			CSelectSymbolDlg::_MakeResult()
{
	m_strSymbol = _GetSelectSymbolName();
	if (m_strSymbol == _T("")) return RESULT_BAD;

	m_vecPara_mm = _GetSymbolPara(m_strSymbol);
	if (m_strSymbol != _T("null") && static_cast<int>(m_vecPara_mm.size()) == 0) return RESULT_BAD;

	m_nSelectShape = static_cast<int>(CDrawFunction::GetSymbolType(m_strSymbol));

	return RESULT_GOOD;
}

CString CSelectSymbolDlg::_GetSelectSymbolName()
{
	CString strSelectSymbol = _T("");
	CString strTemp;
	
	SS_COORD  cellCoordCol;
	SS_COORD  cellCoordRow;
	m_Spread.GetActiveCell(&cellCoordCol, &cellCoordRow);

	if (cellCoordRow <= 1) return strSelectSymbol;
	if (cellCoordRow > m_Spread.GetMaxRows()) return strSelectSymbol;

	strSelectSymbol = m_Spread.GetText(enumCol_Data, cellCoordRow);

	return strSelectSymbol;
}

vector<double> CSelectSymbolDlg::_GetSymbolPara(IN const CString strSymbolName)
{
	vector<double> vecPara;
	if (strSymbolName == _T("")) return vector<double>();

	CJobFile* pJobData = GetJobPtr();
	if (pJobData == nullptr) return vector<double>();

	CStep *pStep = pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr) return vector<double>();

	if (m_strSelectLayerName == _T("")) return vector<double>();

	int nLayerIndex;//사용안함.
	CLayer* pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, m_strSelectLayerName, nLayerIndex);
	if (pLayer == nullptr) return vector<double>();

	CString strTemp;
	int nSymbolCount = static_cast<int>(pLayer->m_FeatureFile.m_arrSymbol.GetCount());
	for (int i = 0; i < nSymbolCount; i++)
	{
		CSymbol *pSymbol = pLayer->m_FeatureFile.m_arrSymbol.GetAt(i);
		if (pSymbol == nullptr) continue;

		if (pSymbol->m_strUserSymbol == strSymbolName)
		{
			vecPara = pSymbol->m_vcParams;
			break;
		}
	}

	return vecPara;
}

BOOL CSelectSymbolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	_InitSpread();

	m_nSelectStep = GetCurStepIndex();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CSelectSymbolDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow == TRUE)
	{
		_InitSpread();
		_UpdateSpread(m_bPadMode);

		m_nSelectStep = GetCurStepIndex();
	}
}

void CSelectSymbolDlg::_InitSpread()
{
	int nMaxCols = enumColMax - 1;


	m_Spread.Reset();

	// Max Col, Row 설정
	m_Spread.SetMaxCols(nMaxCols);
	m_Spread.SetMaxRows(1);

	// Font 설정
	//m_Spread.Set_Font(1, 1, nMaxCols, nMaxRows, _T("Malgun Gothic"), 10 * FONT_RATIO, TRUE);

	// Header Hide
	m_Spread.SetHeaderHide();

	// Header Display None

	// Text Display
	int nIndex = 1;

	m_Spread.SetText(enumCol_No, nIndex, _T("No."));
	m_Spread.SetText(enumCol_Data, nIndex, _T("Data"));
	nIndex = 2;

	CRect rcClient;
	m_Spread.GetClientRect(rcClient);



	//1 : 8 pixel
	//10 : 80 pixel
	//150 : 1200 pixel
	//Width 계산
	int nWidth = rcClient.Width() - 5;//Scroll bar Size
	double dWidthTemp;
	double dWidthSum = 0.0;
	for (int j = enumCol_No; j < enumColMax; j++)
	{
		switch (j)
		{
		case enumCol_No:
			dWidthTemp = nWidth * 0.1 / 8.;
			dWidthSum += dWidthTemp;
			break;
		case enumCol_Data:
			dWidthTemp = nWidth / 8. - dWidthSum;
			break;
		default:
			break;
		}

		m_Spread.SetColWidth(j, dWidthTemp);
	}

	// Scrollbar Hide
	m_Spread.SetScrollBars(0/*ScrollBarsNone*/);

	/* 전체 텍스트 얼라인 Center로 - 주의 Static Text로 설정하기 때문에 Edit가 전부 Disable됨
	   호출 순서에 주의할것.*/
	m_Spread.Block_SetAlignTextCenter();


	//-------------------------------------------------------------------

	// 배경색 설정
	//Header
	m_Spread.Block_SetBackColor(1, nMaxCols, 1, 1, RGB(255, 250, 230));
	//
	m_Spread.Set_Font(1, 1, nMaxCols, 1, _T("Malgun Gothic"), 12 * 1, TRUE);

	// 고정모드 설정
	m_Spread.SetFixedMode();
	// 배경색 설정

	// 테두리 설정		
	m_Spread.SetCellBorder(1, 1, nMaxCols, 1, 1, RGB(112, 128, 144), 1);
	// 	m_Spread.SetCellBorder(1, 1, nMaxCols, nMaxRows, 1, COLOR_SLATEGRAY, 1);
	// 	m_Spread.SetCellBorder(1, 1, nMaxCols, nMaxRows, 2, COLOR_SLATEGRAY, 1);
	// 	m_Spread.SetCellBorder(1, 1, nMaxCols, nMaxRows, 4, COLOR_SLATEGRAY, 1);
	// 	m_Spread.SetCellBorder(1, 1, nMaxCols, nMaxRows, 8, COLOR_SLATEGRAY, 1);

		//Header Lock
		//m_Spread.Set_BlockLock(enumList_1_No, 1, nMaxCols, 1, TRUE);
	m_Spread.Set_AllBlockLock();

	m_Spread.SetGrayAreaBackColor(RGB(255, 255, 255));

	m_Spread.SetAllDataRowHeight(30);

	m_Spread.SetOperationMode(3);//operationModeSingle//Single Select Mode
}

void CSelectSymbolDlg::_UpdateSpread(BOOL bPadMode)
{
	CJobFile* pJobData = GetJobPtr();
	if (pJobData == nullptr) return;

	CStep *pStep = pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr) return;

	if (m_strSelectLayerName == _T("")) return;

	int nLayerIndex;//사용안함.
	CLayer* pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, m_strSelectLayerName, nLayerIndex);
	if (pLayer == nullptr) return;

	CString strTemp;
	int nSymbolCount = static_cast<int>(pLayer->m_FeatureFile.m_arrSymbol.GetCount());
	int nRowCount = 2;
	for (int i = 0; i < nSymbolCount; i++)
	{
		CSymbol *pSymbol = pLayer->m_FeatureFile.m_arrSymbol.GetAt(i);
		if (pSymbol == nullptr) continue;

		SymbolName eSybolType = CDrawFunction::GetSymbolType(pSymbol->m_strUserSymbol);

		if (m_bPadMode == FALSE && 
			( eSybolType == SymbolName::r || eSybolType == SymbolName::s))
		{//Pad가 아닐경우 r, s만 선택적으로 표시
			
			m_Spread.SetMaxRows(nRowCount);//none+Header

			strTemp.Format(_T("%d"), nRowCount - 1);
			m_Spread.SetText(enumCol_No, nRowCount, strTemp);
			m_Spread.SetText(enumCol_Data, nRowCount, pSymbol->m_strUserSymbol);
			
			nRowCount++;
		}
		else
		{
			m_Spread.SetMaxRows(nRowCount);//none+Header

			strTemp.Format(_T("%d"), nRowCount - 1);
			m_Spread.SetText(enumCol_No, nRowCount, strTemp);
			m_Spread.SetText(enumCol_Data, nRowCount, pSymbol->m_strUserSymbol);

			nRowCount++;
		}		
	}
}

void CSelectSymbolDlg::OnBnClickedButtonApply()
{
	UINT32 bRet;

	bRet = _MakeResult();
	if (bRet == RESULT_GOOD)
	{
		CDialogEx::OnOK();
	}
	else
	{
		AfxMessageBox(_T("심볼이 제대로 선택되지 않았습니다."));
	}
}


void CSelectSymbolDlg::OnBnClickedButtonCancel()
{
	CDialogEx::OnCancel();
}
