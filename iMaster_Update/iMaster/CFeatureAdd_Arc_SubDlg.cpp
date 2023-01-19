// CFeatureAdd_Arc_SubDlg.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "MainFrm.h"
#include "CFeatureEdit_AddDlg.h"
#include "CFeatureAdd_Arc_SubDlg.h"
#include "afxdialogex.h"

#include "CMakeSymbolDlg.h"
#include "CMakeAttributeDlg.h"
#include "CSelectSymbolDlg.h"


// CFeatureAdd_Arc_SubDlg 대화 상자

IMPLEMENT_DYNAMIC(CFeatureAdd_Arc_SubDlg, CDialogEx)

CFeatureAdd_Arc_SubDlg::CFeatureAdd_Arc_SubDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDIT_ADD_SUB_ARC, pParent)
{

}

CFeatureAdd_Arc_SubDlg::~CFeatureAdd_Arc_SubDlg()
{
}

void CFeatureAdd_Arc_SubDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFeatureAdd_Arc_SubDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_START_POS, &CFeatureAdd_Arc_SubDlg::OnBnClickedButtonSelectStartPos)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_END_POS, &CFeatureAdd_Arc_SubDlg::OnBnClickedButtonSelectEndPos)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_CENTER_POS, &CFeatureAdd_Arc_SubDlg::OnBnClickedButtonSelectCenterPos)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_ATTRIBUTE, &CFeatureAdd_Arc_SubDlg::OnBnClickedButtonSelectAttribute)
	ON_BN_CLICKED(IDC_BUTTON_MAKE_SYMBOL, &CFeatureAdd_Arc_SubDlg::OnBnClickedButtonMakeSymbol)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_POLARITY_POS, IDC_RADIO_POLARITY_NEG, &CFeatureAdd_Arc_SubDlg::OnPolarityButtonClicked)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_CW, IDC_RADIO_CCW, &CFeatureAdd_Arc_SubDlg::OnDirectionButtonClicked)
	ON_EN_CHANGE(IDC_EDIT_START_POS_X, &CFeatureAdd_Arc_SubDlg::OnEnChangeEditStartPosX)
	ON_EN_CHANGE(IDC_EDIT_START_POS_Y, &CFeatureAdd_Arc_SubDlg::OnEnChangeEditStartPosY)
	ON_EN_CHANGE(IDC_EDIT_END_POS_X, &CFeatureAdd_Arc_SubDlg::OnEnChangeEditEndPosX)
	ON_EN_CHANGE(IDC_EDIT_END_POS_Y, &CFeatureAdd_Arc_SubDlg::OnEnChangeEditEndPosY)
	ON_EN_CHANGE(IDC_EDIT_CENTER_POS_X, &CFeatureAdd_Arc_SubDlg::OnEnChangeEditCenterPosX)
	ON_EN_CHANGE(IDC_EDIT_CENTER_POS_Y, &CFeatureAdd_Arc_SubDlg::OnEnChangeEditCenterPosY)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_SYMBOL, &CFeatureAdd_Arc_SubDlg::OnBnClickedButtonSelectSymbol)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CFeatureAdd_Arc_SubDlg 메시지 처리기


BOOL CFeatureAdd_Arc_SubDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	_InitValue();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CFeatureAdd_Arc_SubDlg::SetLink(CFeatureEdit_AddDlg * pParent)
{
	m_pParent = pParent;
}


UINT CFeatureAdd_Arc_SubDlg::SetSelectPoint(IN const int&nIndex, IN const float &fX, IN const float &fY)
{
	switch (nIndex)

	{
	default:
	case enumSelectPoint_Start:
		m_stAddParam.dStartX = fX;
		m_stAddParam.dStartY = fY;
		break;
	case enumSelectPoint_Center:
		m_stAddParam.dCenterX = fX;
		m_stAddParam.dCenterY = fY;
		break;
	case enumSelectPoint_End:
		m_stAddParam.dEndX = fX;
		m_stAddParam.dEndY = fY;
		break;
	}

	_UpdateDataToUI();

	return RESULT_GOOD;
}

UINT32 CFeatureAdd_Arc_SubDlg::MakeFeature(vector<CFeature *> &vecFeatureData)
{
	_UpdateUItoData();

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureA();
	CFeatureA *pFeatureA = (CFeatureA *)pFeature;

	pFeatureA->m_dXs = m_stAddParam.dStartX;
	pFeatureA->m_dYs = m_stAddParam.dStartY;
	pFeatureA->m_dXc = m_stAddParam.dCenterX;
	pFeatureA->m_dYc = m_stAddParam.dCenterY;
	pFeatureA->m_dXe = m_stAddParam.dEndX;
	pFeatureA->m_dYe = m_stAddParam.dEndY;


	pFeatureA->m_bShow = true;

	pFeatureA->m_nDecode = 0;
	pFeatureA->m_bPolarity = m_stAddParam.bPolarity;
	pFeatureA->m_bCw = !m_stAddParam.bCw;
	pFeatureA->m_eType = FeatureType::A;
	pFeatureA->m_bHighlight = true;

	if (m_stAddParam.strAttributeName != _T(""))
	{
		CAttribute* pAttribute = new CAttribute();

		pAttribute->m_strName = m_stAddParam.strAttributeName;

		pFeatureA->m_arrAttribute.Add(pAttribute);
	}

	int nStringCount = static_cast<int> (m_stAddParam.vecAttributeString.size());
	for (int i = 0; i < nStringCount; i++)
	{
		CAttributeTextString* pAttributeString = new CAttributeTextString();

		pAttributeString->m_strText = m_stAddParam.vecAttributeString[i];

		pFeatureA->m_arrAttributeTextString.Add(pAttributeString);
	}

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = m_stAddParam.eSymbolName;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;
	pSymbol->m_vcParams = m_stAddParam.vecPara;
	pSymbol->m_strUserSymbol = m_stAddParam.strSymbol;

	pFeatureA->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureA;

	return RESULT_GOOD;
}

void CFeatureAdd_Arc_SubDlg::_InitValue()
{
	m_stAddParam.Reset();
	_IntiSymbol();
	_UpdateDataToUI();
}

void CFeatureAdd_Arc_SubDlg::_UpdateDataToUI()
{

	if (m_stAddParam.bCw == TRUE)
	{
		{
		((CButton*)GetDlgItem(IDC_RADIO_CW))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_CCW))->SetCheck(FALSE);
		}
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO_CW))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_CCW))->SetCheck(TRUE);
	}
	

	if (m_stAddParam.bPolarity == TRUE)
	{
		((CButton*)GetDlgItem(IDC_RADIO_POLARITY_POS))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_POLARITY_NEG))->SetCheck(FALSE);
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO_POLARITY_POS))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_POLARITY_NEG))->SetCheck(TRUE);
	}

	CString strTemp;

	strTemp.Format(_T("%.3lf"), m_stAddParam.dStartX);
	GetDlgItem(IDC_EDIT_START_POS_X)->SetWindowText(strTemp);

	strTemp.Format(_T("%.3lf"), m_stAddParam.dStartY);
	GetDlgItem(IDC_EDIT_START_POS_Y)->SetWindowText(strTemp);

	strTemp.Format(_T("%.3lf"), m_stAddParam.dEndX);
	GetDlgItem(IDC_EDIT_END_POS_X)->SetWindowText(strTemp);

	strTemp.Format(_T("%.3lf"), m_stAddParam.dEndY);
	GetDlgItem(IDC_EDIT_END_POS_Y)->SetWindowText(strTemp);

	strTemp.Format(_T("%.3lf"), m_stAddParam.dCenterX);
	GetDlgItem(IDC_EDIT_CENTER_POS_X)->SetWindowText(strTemp);

	strTemp.Format(_T("%.3lf"), m_stAddParam.dCenterY);
	GetDlgItem(IDC_EDIT_CENTER_POS_Y)->SetWindowText(strTemp);

	GetDlgItem(IDC_EDIT_SYMBOL)->SetWindowText(m_stAddParam.strSymbol);

	GetDlgItem(IDC_EDIT_ATTRIBUTE_NAME)->SetWindowText(m_stAddParam.strAttributeName);

	strTemp = _T("");
	for (int i = 0; i < static_cast<int>(m_stAddParam.vecAttributeString.size()); i++)
	{
		strTemp += m_stAddParam.vecAttributeString[i];

		if (i != m_stAddParam.vecAttributeString.size() - 1)
		{
			strTemp += _T(",");
		}
	}

	GetDlgItem(IDC_EDIT_ATTRIBUTE_STRING)->SetWindowText(strTemp);

}

void CFeatureAdd_Arc_SubDlg::_UpdateUItoData()
{
}

BOOL CFeatureAdd_Arc_SubDlg::IsAllDataInput() 
{
	BOOL bRet = TRUE;

	CString strTemp;
	GetDlgItem(IDC_EDIT_SYMBOL)->GetWindowText(strTemp);
	if (strTemp == __T(""))
	{
		bRet = FALSE;
	}

	double dSx, dSy, dCx, dCy, dEx, dEy;
	GetDlgItem(IDC_EDIT_START_POS_X)->GetWindowText(strTemp);
	dSx = _ttof(strTemp);

	GetDlgItem(IDC_EDIT_START_POS_Y)->GetWindowText(strTemp);
	dSy = _ttof(strTemp);

	GetDlgItem(IDC_EDIT_START_POS_X)->GetWindowText(strTemp);
	dCx = _ttof(strTemp);

	GetDlgItem(IDC_EDIT_START_POS_X)->GetWindowText(strTemp);
	dCy = _ttof(strTemp);

	GetDlgItem(IDC_EDIT_END_POS_X)->GetWindowText(strTemp);
	dEx = _ttof(strTemp);

	GetDlgItem(IDC_EDIT_END_POS_Y)->GetWindowText(strTemp);
	dEy = _ttof(strTemp);

	if (dSx == 0. && dSy == 0. && dCx == 0. && dCy == 0. && dEx == 0. &&dEy == 0.)
	{
		bRet = FALSE;
	}

	return bRet;
}
void CFeatureAdd_Arc_SubDlg::OnBnClickedButtonSelectStartPos()
{
	SetPointMode(TRUE, enumSelectPoint_Start, _T("Arc Start"));
}


void CFeatureAdd_Arc_SubDlg::OnBnClickedButtonSelectEndPos()
{
	SetPointMode(TRUE, enumSelectPoint_End, _T("Arc End"));
}


void CFeatureAdd_Arc_SubDlg::OnBnClickedButtonSelectCenterPos()
{
	SetPointMode(TRUE, enumSelectPoint_Center, _T("Arc Center"));
}





void CFeatureAdd_Arc_SubDlg::OnBnClickedButtonSelectAttribute()
{
	if (m_pParent == nullptr) return;
	CString strLayerName = m_pParent->GetLayerName();

	CMakeAttributeDlg dlg(strLayerName);

	if (dlg.DoModal() == IDOK)
	{
		m_stAddParam.strAttributeName = dlg.GetAttributeName();

		CString strAttString = dlg.GetAttributeString();
		m_stAddParam.vecAttributeString.resize(1);
		m_stAddParam.vecAttributeString[0] = strAttString;

		_UpdateDataToUI();
	}
}

void CFeatureAdd_Arc_SubDlg::OnDirectionButtonClicked(UINT32 nID)
{
	if (nID <= 0) return;

	int nIndex = nID - IDC_RADIO_CW;

	if (nIndex == 0)
	{
		m_stAddParam.bCw = FALSE;
	}
	else
	{
		m_stAddParam.bCw = TRUE;
	}
}

void CFeatureAdd_Arc_SubDlg::OnPolarityButtonClicked(UINT32 nID)
{
	if (nID <= 0) return;

	int nIndex = nID - IDC_RADIO_POLARITY_POS;

	if (nIndex == 0)
	{
		m_stAddParam.bPolarity = TRUE;
	}
	else
	{
		m_stAddParam.bPolarity = FALSE;
	}
}


void CFeatureAdd_Arc_SubDlg::OnBnClickedButtonMakeSymbol()
{
	

	CMakeSymbolDlg dlg(eMakeSymbolMode::emumMode_Arc);

	if (dlg.DoModal() == IDOK)
	{
		m_stAddParam.strSymbol = dlg.GetSymbolName();
		m_stAddParam.eSymbolName = static_cast<SymbolName>(dlg.GetType());
		m_stAddParam.vecPara = dlg.GetSymbolPara();

		_UpdateDataToUI();
	}
}

void CFeatureAdd_Arc_SubDlg::OnEnChangeEditStartPosX()
{
	CString strTemp = _T("");

	GetDlgItem(IDC_EDIT_START_POS_X)->GetWindowText(strTemp);
	if (strTemp.GetLength() > 1)
		m_stAddParam.dStartX = _ttof(strTemp);
	else
		m_stAddParam.dStartX = 0.0;
}

void CFeatureAdd_Arc_SubDlg::OnEnChangeEditStartPosY()
{
	CString strTemp = _T("");

	GetDlgItem(IDC_EDIT_START_POS_Y)->GetWindowText(strTemp);
	if (strTemp.GetLength() > 1)
		m_stAddParam.dStartY = _ttof(strTemp);
	else
		m_stAddParam.dStartY = 0.0;
}
void CFeatureAdd_Arc_SubDlg::OnEnChangeEditEndPosX()
{
	CString strTemp = _T("");

	GetDlgItem(IDC_EDIT_END_POS_X)->GetWindowText(strTemp);
	if (strTemp.GetLength() > 1)
		m_stAddParam.dEndX = _ttof(strTemp);
	else
		m_stAddParam.dEndX = 0.0;
}

void CFeatureAdd_Arc_SubDlg::OnEnChangeEditEndPosY()
{
	CString strTemp = _T("");

	GetDlgItem(IDC_EDIT_END_POS_Y)->GetWindowText(strTemp);
	if (strTemp.GetLength() > 1)
		m_stAddParam.dEndY = _ttof(strTemp);
	else
		m_stAddParam.dEndY = 0.0;
}

void CFeatureAdd_Arc_SubDlg::OnEnChangeEditCenterPosX()
{
	CString strTemp = _T("");

	GetDlgItem(IDC_EDIT_CENTER_POS_X)->GetWindowText(strTemp);
	if (strTemp.GetLength() > 1)
		m_stAddParam.dCenterX = _ttof(strTemp);
	else
		m_stAddParam.dCenterX = 0.0;
}

void CFeatureAdd_Arc_SubDlg::OnEnChangeEditCenterPosY()
{
	CString strTemp = _T("");

	GetDlgItem(IDC_EDIT_CENTER_POS_Y)->GetWindowText(strTemp);
	if (strTemp.GetLength() > 1)
		m_stAddParam.dCenterY = _ttof(strTemp);
	else
		m_stAddParam.dCenterY = 0.0;
}

UINT32 CFeatureAdd_Arc_SubDlg::_IntiSymbol()
{
	CJobFile* pJobData = GetJobPtr();
	if (pJobData == nullptr) return RESULT_BAD;

	int nSelectStep = GetCurStepIndex();
	if (nSelectStep < 0 || nSelectStep >= pJobData->m_arrStep.GetSize())
		return RESULT_BAD;

	CStep *pStep = pJobData->m_arrStep.GetAt(nSelectStep);
	if (pStep == nullptr) return RESULT_BAD;

	if (m_pParent == nullptr) RESULT_BAD;
	CString strLayerName = m_pParent->GetLayerName();

	int nLayerIndex;
	CLayer* pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, strLayerName, nLayerIndex);
	if (pLayer == nullptr) return RESULT_BAD;

	int iSelSymbol = -1;
	double dMinWidth = DBL_MAX;
	vector<CString> vcSymbolNames;
	int nSymbolCount = static_cast<int>(pLayer->m_FeatureFile.m_arrSymbol.GetCount());
	for (int i = 0; i < nSymbolCount; i++)
	{
		CSymbol *pSymbol = pLayer->m_FeatureFile.m_arrSymbol.GetAt(i);
		if(pSymbol == nullptr) continue;

		SymbolName eSymbolType = CDrawFunction::GetSymbolType(pSymbol->m_strUserSymbol);

		if (eSymbolType == SymbolName::r)
		{
			vcSymbolNames.emplace_back(pSymbol->m_strUserSymbol);

			CString strTemp = pSymbol->m_strUserSymbol;
			strTemp.Delete(0, 1);
			strTemp.Trim();

			if(strTemp.GetLength() <1)
				continue;

			double dWidth = _ttof(strTemp);
			if (dWidth < dMinWidth)
			{
				dMinWidth = dWidth;
				iSelSymbol = i;
			}
		}
	}

	if (iSelSymbol >= 0 && iSelSymbol < vcSymbolNames.size())
	{
		CString strSymbolName = vcSymbolNames[iSelSymbol];
		if (strSymbolName.GetLength() < 1)
			return RESULT_BAD;

		vector<double> vecPara;

		CString strTemp;
		int nSymbolCount = static_cast<int>(pLayer->m_FeatureFile.m_arrSymbol.GetCount());
		for (int i = 0; i < nSymbolCount; i++)
		{
			CSymbol *pSymbol = pLayer->m_FeatureFile.m_arrSymbol.GetAt(i);
			if(pSymbol == nullptr) continue;

			if (pSymbol->m_strUserSymbol == strSymbolName)
			{
				vecPara = pSymbol->m_vcParams;
				break;
			}
				
		}

		if (vecPara.size() <= 0) return RESULT_BAD;

		m_stAddParam.strSymbol = strSymbolName;
		m_stAddParam.eSymbolName = static_cast<SymbolName>(static_cast<int>(CDrawFunction::GetSymbolType(strSymbolName)));
		m_stAddParam.vecPara = vecPara;

		vcSymbolNames.clear();

	}

	return RESULT_GOOD;
}

UINT32 CFeatureAdd_Arc_SubDlg::_ManualMove(int nIndex)
{
	if (nIndex == enumSelectPointMax)
		return RESULT_BAD;

	PointDXY ptXY = CPointD(0, 0);

	if (nIndex == enumSelectPoint_Start)
	{
		ptXY.x = m_stAddParam.dStartX;
		ptXY.y = m_stAddParam.dStartY;
	}
	else if (nIndex == enumSelectPoint_End)
	{
		ptXY.x = m_stAddParam.dEndX;
		ptXY.y = m_stAddParam.dEndY;
	}
	else if (nIndex == enumSelectPoint_Center)
	{
		ptXY.x = m_stAddParam.dCenterX;
		ptXY.y = m_stAddParam.dCenterY;
	}

	Draw_ManualMove(0, ptXY);
	return RESULT_GOOD;
}

void CFeatureAdd_Arc_SubDlg::OnBnClickedButtonSelectSymbol()
{
	if (m_pParent == nullptr) return;
	CString strLayerName = m_pParent->GetLayerName();

	CSelectSymbolDlg dlg(FALSE, strLayerName);

	if (dlg.DoModal() == IDOK)
	{
		m_stAddParam.strSymbol = dlg.GetSymbolName();
		m_stAddParam.eSymbolName = static_cast<SymbolName>(dlg.GetType());
		m_stAddParam.vecPara = dlg.GetSymbolPara();

		_UpdateDataToUI();
	}
}





void CFeatureAdd_Arc_SubDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow == TRUE)
	{
		_InitValue();
		ClearTempPoint();
	}
}


BOOL CFeatureAdd_Arc_SubDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)

	{
		if (pMsg->hwnd == GetDlgItem(IDC_EDIT_START_POS_X)->m_hWnd ||
			pMsg->hwnd == GetDlgItem(IDC_EDIT_START_POS_Y)->m_hWnd)
		{
			SetPointMode(TRUE, enumSelectPoint_Start, _T("Arc Start"));
			_ManualMove(enumSelectPoint_Start);
			return TRUE;
		}

		else if (pMsg->hwnd == GetDlgItem(IDC_EDIT_CENTER_POS_X)->m_hWnd ||
			pMsg->hwnd == GetDlgItem(IDC_EDIT_CENTER_POS_Y)->m_hWnd)
		{
			SetPointMode(TRUE, enumSelectPoint_Center, _T("Arc Center"));
			_ManualMove(enumSelectPoint_Center);
			return TRUE;
		}

		else if (pMsg->hwnd == GetDlgItem(IDC_EDIT_END_POS_X)->m_hWnd ||
			pMsg->hwnd == GetDlgItem(IDC_EDIT_END_POS_Y)->m_hWnd)
		{
			SetPointMode(TRUE, enumSelectPoint_End, _T("Arc End"));
			_ManualMove(enumSelectPoint_End);
			return TRUE;
		}
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == 'S')
	{
		SetPointMode(TRUE, enumSelectPoint_Start, _T("Arc Start"));
		return TRUE;
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == 'E')
	{
		SetPointMode(TRUE, enumSelectPoint_End, _T("Arc End"));
		return TRUE;
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == 'C')
	{
		SetPointMode(TRUE, enumSelectPoint_Center, _T("Arc Center"));
		return TRUE;
	}

	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}
