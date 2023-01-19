// CMakeAttributeDlg.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "MainFrm.h"
#include "CMakeAttributeDlg.h"
#include "afxdialogex.h"


// CMakeAttributeDlg 대화 상자

IMPLEMENT_DYNAMIC(CMakeAttributeDlg, CDialogEx)

CMakeAttributeDlg::CMakeAttributeDlg(IN const CString &strLayerName, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDIT_SELECT_ATTRIBUTE, pParent),
	m_strSelectLayerName(strLayerName)
{

}

CMakeAttributeDlg::~CMakeAttributeDlg()
{
}

void CMakeAttributeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TABLE_ATTRIBUTE_NAME, m_pSpread[enumSpread_Name]);
	DDX_Control(pDX, IDC_TABLE_ATTRIBUTE_STRING, m_pSpread[enumSpread_String]);
}


BEGIN_MESSAGE_MAP(CMakeAttributeDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CMakeAttributeDlg::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CMakeAttributeDlg::OnBnClickedButtonCancel)
END_MESSAGE_MAP()


// CMakeAttributeDlg 메시지 처리기

CString CMakeAttributeDlg::GetAttributeName()
{
	return m_strAttributeName;
}

CString CMakeAttributeDlg::GetAttributeString()
{
	return m_strAttributeString;
}

BOOL CMakeAttributeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	_InitUI();
	_InitSpread();

	m_nSelectStep = GetCurStepIndex();

	_UpdateSpread_Name();
	_UpdateSpread_String();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CMakeAttributeDlg::_InitUI()
{
	((CButton*)GetDlgItem(IDC_RADIO_TABLE_ATTRIBUTE_NAME))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO_NEW_ATTRIBUTE_NAME))->SetCheck(FALSE);

	((CButton*)GetDlgItem(IDC_RADIO_TABLE_ATTRIBUTE_STRING))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO_NEW_ATTRIBUTE_STRING))->SetCheck(FALSE);

	GetDlgItem(IDC_EDIT_ATTRIBUTE_NAME)->SetWindowText(_T(""));
	GetDlgItem(IDC_EDIT_ATTRIBUTE_STRING)->SetWindowText(_T(""));

}

void CMakeAttributeDlg::_InitSpread()
{
	int nMaxCols = enumColMax - 1;

	for (int i = 0; i < enumSpread_Max; i++)
	{
		m_pSpread[i].Reset();

		// Max Col, Row 설정
		m_pSpread[i].SetMaxCols(nMaxCols);
		m_pSpread[i].SetMaxRows(1);

		// Font 설정
		//m_pSpread[i].Set_Font(1, 1, nMaxCols, nMaxRows, _T("Malgun Gothic"), 10 * FONT_RATIO, TRUE);

		// Header Hide
		m_pSpread[i].SetHeaderHide();

		// Header Display None

		// Text Display
		int nIndex = 1;

		m_pSpread[i].SetText(enumCol_No, nIndex, _T("No."));
		m_pSpread[i].SetText(enumCol_Data, nIndex, _T("Data"));
		nIndex = 2;

		CRect rcClient;
		m_pSpread[i].GetClientRect(rcClient);



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

			m_pSpread[i].SetColWidth(j, dWidthTemp);
		}

		// Scrollbar Hide
		m_pSpread[i].SetScrollBars(0/*ScrollBarsNone*/);

		/* 전체 텍스트 얼라인 Center로 - 주의 Static Text로 설정하기 때문에 Edit가 전부 Disable됨
		   호출 순서에 주의할것.*/
		m_pSpread[i].Block_SetAlignTextCenter();


		//-------------------------------------------------------------------

		// 배경색 설정
		//Header
		m_pSpread[i].Block_SetBackColor(1, nMaxCols, 1, 1, RGB(255, 250, 230));
		//
		m_pSpread[i].Set_Font(1, 1, nMaxCols, 1, _T("Malgun Gothic"), 12 * 1, TRUE);

		// 고정모드 설정
		m_pSpread[i].SetFixedMode();
		// 배경색 설정

		// 테두리 설정		
		m_pSpread[i].SetCellBorder(1, 1, nMaxCols, 1, 1, RGB(112, 128, 144), 1);
		// 	m_pSpread[i].SetCellBorder(1, 1, nMaxCols, nMaxRows, 1, COLOR_SLATEGRAY, 1);
		// 	m_pSpread[i].SetCellBorder(1, 1, nMaxCols, nMaxRows, 2, COLOR_SLATEGRAY, 1);
		// 	m_pSpread[i].SetCellBorder(1, 1, nMaxCols, nMaxRows, 4, COLOR_SLATEGRAY, 1);
		// 	m_pSpread[i].SetCellBorder(1, 1, nMaxCols, nMaxRows, 8, COLOR_SLATEGRAY, 1);

			//Header Lock
			//m_pSpread[i].Set_BlockLock(enumList_1_No, 1, nMaxCols, 1, TRUE);
		m_pSpread[i].Set_AllBlockLock();

		m_pSpread[i].SetGrayAreaBackColor(RGB(255, 255, 255));

		m_pSpread[i].SetAllDataRowHeight(30);

		m_pSpread[i].SetOperationMode(3);//operationModeSingle//Single Select Mode
	}
}

void CMakeAttributeDlg::_UpdateSpread_Name()
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
	int nAttNameCount = static_cast<int>(pLayer->m_FeatureFile.m_arrAttribute.GetCount());
	m_pSpread[enumSpread_Name].SetMaxRows(1 + nAttNameCount);//none+Header
	for (int i = 2; i < nAttNameCount+2; i++)
	{
		CAttribute *pAttName = pLayer->m_FeatureFile.m_arrAttribute.GetAt(i-2);
		if (pAttName == nullptr ) continue;

		strTemp.Format(_T("%d"), i - 1);
		m_pSpread[enumSpread_Name].SetText(enumCol_No, i, strTemp);

		
		m_pSpread[enumSpread_Name].SetText(enumCol_Data, i, pAttName->m_strName);
	}

	m_pSpread[enumSpread_Name].Set_AllBlockLock();
}

void CMakeAttributeDlg::_UpdateSpread_String()
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
	int nAttStringCount = static_cast<int>(pLayer->m_FeatureFile.m_arrAttributeTextString.GetCount());
	m_pSpread[enumSpread_String].SetMaxRows(1 + nAttStringCount);//none+Header
	for (int i = 2; i < nAttStringCount +2; i++)
	{
		CAttributeTextString *pAttString = pLayer->m_FeatureFile.m_arrAttributeTextString.GetAt(i-2);
		if (pAttString == nullptr) continue;

		strTemp.Format(_T("%d"), i - 1);
		m_pSpread[enumSpread_String].SetText(enumCol_No, i, strTemp);


		m_pSpread[enumSpread_String].SetText(enumCol_Data, i, pAttString->m_strText);
	}

	m_pSpread[enumSpread_String].Set_AllBlockLock();
}


BOOL CMakeAttributeDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

UINT32 CMakeAttributeDlg::_SetAttributeName()
{
	m_strAttributeName = _T("");

	CString strTemp;
	if (((CButton*)GetDlgItem(IDC_RADIO_TABLE_ATTRIBUTE_NAME))->GetCheck() == TRUE)
	{
		SS_COORD  cellCoordCol;
		SS_COORD  cellCoordRow;
		m_pSpread[enumSpread_Name].GetActiveCell(&cellCoordCol, &cellCoordRow);

		if (cellCoordRow <= 1) return RESULT_BAD;
		if (cellCoordRow > m_pSpread[enumSpread_Name].GetMaxRows()) return RESULT_BAD;

		strTemp = m_pSpread[enumSpread_Name].GetText(enumCol_Data, cellCoordRow);

		m_strAttributeName = strTemp;

	}
	else
	{
		GetDlgItem(IDC_EDIT_ATTRIBUTE_NAME)->GetWindowText(strTemp);
		if (strTemp == _T("")) return RESULT_BAD;

		m_strAttributeName = strTemp;
	}

	return RESULT_GOOD;
	
}

UINT32 CMakeAttributeDlg::_SetAttributeString()
{
	CString strTemp;
	if (((CButton*)GetDlgItem(IDC_RADIO_TABLE_ATTRIBUTE_STRING))->GetCheck() == TRUE)
	{
		SS_COORD  cellCoordCol;
		SS_COORD  cellCoordRow;
		m_pSpread[enumSpread_String].GetActiveCell(&cellCoordCol, &cellCoordRow);

		if (cellCoordRow <= 1) return RESULT_BAD;
		if (cellCoordRow > m_pSpread[enumSpread_String].GetMaxRows()) return RESULT_BAD;

		strTemp = m_pSpread[enumSpread_String].GetText(enumCol_Data, cellCoordRow);
		if (strTemp == _T("")) return RESULT_BAD;

		m_strAttributeString = strTemp;

	}
	else
	{
		GetDlgItem(IDC_EDIT_ATTRIBUTE_STRING)->GetWindowText(strTemp);
		if (strTemp == _T("")) return RESULT_BAD;

		m_strAttributeString = strTemp;
	}

	return RESULT_GOOD;
}

void CMakeAttributeDlg::OnBnClickedButtonApply()
{
	UINT32 bRet;
	bRet = _SetAttributeName();
	bRet |= _SetAttributeString();

	if (bRet == RESULT_GOOD)
	{
		CDialogEx::OnOK();
	}
	else
	{
		AfxMessageBox(_T("Attribute가 제대로 입력되지 않았습니다."));
	}
	
}


void CMakeAttributeDlg::OnBnClickedButtonCancel()
{
	m_strAttributeName = _T("");
	m_strAttributeString = _T("");

	CDialogEx::OnCancel();
}
