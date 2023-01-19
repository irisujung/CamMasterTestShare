// CFeatureAdd_Line_SubDlg.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "MainFrm.h"
#include "CFeatureEdit_AddDlg.h"
#include "CFeatureAdd_Line_SubDlg.h"
#include "afxdialogex.h"

#include "CMakeSymbolDlg.h"
#include "CMakeAttributeDlg.h"
#include "CSelectSymbolDlg.h"

// CFeatureAdd_Line_SubDlg 대화 상자

IMPLEMENT_DYNAMIC(CFeatureAdd_Line_SubDlg, CDialogEx)

CFeatureAdd_Line_SubDlg::CFeatureAdd_Line_SubDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDIT_ADD_SUB_LINE, pParent)
{

}

CFeatureAdd_Line_SubDlg::~CFeatureAdd_Line_SubDlg()
{
}

void CFeatureAdd_Line_SubDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFeatureAdd_Line_SubDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_START_POS, &CFeatureAdd_Line_SubDlg::OnBnClickedButtonSelectStartPos)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_END_POS, &CFeatureAdd_Line_SubDlg::OnBnClickedButtonSelectEndPos)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_SELECT_ATTRIBUTE, &CFeatureAdd_Line_SubDlg::OnBnClickedButtonSelectAttribute)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_POLARITY_POS, IDC_RADIO_POLARITY_NEG, &CFeatureAdd_Line_SubDlg::OnPolarityButtonClicked)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_SYMBOL, &CFeatureAdd_Line_SubDlg::OnBnClickedButtonSelectSymbol)
	ON_BN_CLICKED(IDC_BUTTON_MAKE_SYMBOL, &CFeatureAdd_Line_SubDlg::OnBnClickedButtonMakeSymbol)
	ON_EN_CHANGE(IDC_EDIT_START_POS_X, &CFeatureAdd_Line_SubDlg::OnEnChangeEditStartPosX)
	ON_EN_CHANGE(IDC_EDIT_START_POS_Y, &CFeatureAdd_Line_SubDlg::OnEnChangeEditStartPosY)
	ON_EN_CHANGE(IDC_EDIT_END_POS_X, &CFeatureAdd_Line_SubDlg::OnEnChangeEditEndPosX)
	ON_EN_CHANGE(IDC_EDIT_END_POS_Y, &CFeatureAdd_Line_SubDlg::OnEnChangeEditEndPosY)
END_MESSAGE_MAP()


// CFeatureAdd_Line_SubDlg 메시지 처리기

void	CFeatureAdd_Line_SubDlg::SetLink(CFeatureEdit_AddDlg *pParent)
{
	m_pParent = pParent;
}

BOOL CFeatureAdd_Line_SubDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	_InitValue();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CFeatureAdd_Line_SubDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow == TRUE)
	{
		_InitValue();
		ClearTempPoint();
	}
}

UINT32 CFeatureAdd_Line_SubDlg::SetSetlectPoint(IN const int &nIndex, IN const float &fX, IN const float &fY)
{
	switch (nIndex)
	{
	default:
	case enumSelectPoint_Start:
		m_stAddParam.dStartX = fX;
		m_stAddParam.dStartY = fY;
		break;
	case enumSelectPoint_End:
		m_stAddParam.dEndX = fX;
		m_stAddParam.dEndY = fY;
		break;
	}

	_UpdateDataToUI();

	return RESULT_GOOD;
}

UINT32 CFeatureAdd_Line_SubDlg::MakeFeature(vector<CFeature*> &vecFeatureData)
{
	_UpdateUIToData();

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureL();
	CFeatureL *pFeatureL = (CFeatureL *)pFeature;

	pFeatureL->m_dXs = m_stAddParam.dStartX;
	pFeatureL->m_dYs = m_stAddParam.dStartY;
	pFeatureL->m_dXe = m_stAddParam.dEndX;
	pFeatureL->m_dYe = m_stAddParam.dEndY;

	pFeatureL->m_bShow = true;

	pFeatureL->m_nDecode = 0;
	pFeatureL->m_bPolarity = m_stAddParam.bPolariry;

	pFeatureL->m_eType = FeatureType::L;
	pFeatureL->m_bHighlight = true;

	//
	if (m_stAddParam.strAttributeName != _T(""))
	{
		CAttribute* pAttribute = new CAttribute();

		pAttribute->m_strName = m_stAddParam.strAttributeName;

		pFeatureL->m_arrAttribute.Add(pAttribute);
	}

	int nStringCount = static_cast<int>(m_stAddParam.vecAttributeString.size());
	for (int i = 0; i < nStringCount; i++)
	{
		CAttributeTextString* pAttributeString = new CAttributeTextString();

		pAttributeString->m_strText = m_stAddParam.vecAttributeString[i];

		pFeatureL->m_arrAttributeTextString.Add(pAttributeString);
	}

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = m_stAddParam.eSymbolName;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;
	pSymbol->m_vcParams = m_stAddParam.vecPara;
	pSymbol->m_strUserSymbol = m_stAddParam.strSymbol;

	pFeatureL->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureL;

	return RESULT_GOOD;
}

void	CFeatureAdd_Line_SubDlg::_InitValue()
{
	m_stAddParam.Reset();	
	_InitSymbol();
	_UpdateDataToUI();
}

void	CFeatureAdd_Line_SubDlg::_UpdateDataToUI()
{
	if (m_stAddParam.bPolariry == TRUE)
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

	//Pos
	strTemp.Format(_T("%.3lf"), m_stAddParam.dStartX);
	GetDlgItem(IDC_EDIT_START_POS_X)->SetWindowText(strTemp);

	strTemp.Format(_T("%.3lf"), m_stAddParam.dStartY);
	GetDlgItem(IDC_EDIT_START_POS_Y)->SetWindowText(strTemp);

	strTemp.Format(_T("%.3lf"), m_stAddParam.dEndX);
	GetDlgItem(IDC_EDIT_END_POS_X)->SetWindowText(strTemp);

	strTemp.Format(_T("%.3lf"), m_stAddParam.dEndY);
	GetDlgItem(IDC_EDIT_END_POS_Y)->SetWindowText(strTemp);

	//Shape Name
	GetDlgItem(IDC_EDIT_SYMBOL)->SetWindowText(m_stAddParam.strSymbol);

	//Attribute Name
	GetDlgItem(IDC_EDIT_ATTRIBUTE_NAME)->SetWindowText(m_stAddParam.strAttributeName);

	//Attribute String
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

void	CFeatureAdd_Line_SubDlg::_UpdateUIToData()
{
}

BOOL	CFeatureAdd_Line_SubDlg::IsAllDataInput()
{
	BOOL bRet = TRUE;

	CString strTemp;
	GetDlgItem(IDC_EDIT_SYMBOL)->GetWindowText(strTemp);
	if (strTemp == _T(""))
	{
		bRet = FALSE;
	}

	double dSx, dSy, dEx, dEy;

	GetDlgItem(IDC_EDIT_START_POS_X)->GetWindowText(strTemp);
	dSx = _ttof(strTemp);

	GetDlgItem(IDC_EDIT_START_POS_Y)->GetWindowText(strTemp);
	dSy = _ttof(strTemp);

	GetDlgItem(IDC_EDIT_END_POS_X)->GetWindowText(strTemp);
	dEx = _ttof(strTemp);

	GetDlgItem(IDC_EDIT_END_POS_Y)->GetWindowText(strTemp);
	dEy = _ttof(strTemp);

	if (dSx == 0. && dSy == 0. && dEx == 0. && dEy == 0.)
	{
		bRet = FALSE;
	}

	return bRet;
}

void CFeatureAdd_Line_SubDlg::OnBnClickedButtonSelectStartPos()
{
	SetPointMode(TRUE, enumSelectPoint_Start, _T("Line Start"));
}


void CFeatureAdd_Line_SubDlg::OnBnClickedButtonSelectEndPos()
{
	SetPointMode(TRUE, enumSelectPoint_End, _T("Line End"));
}

BOOL CFeatureAdd_Line_SubDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		if (pMsg->hwnd == GetDlgItem(IDC_EDIT_START_POS_X)->m_hWnd ||
			pMsg->hwnd == GetDlgItem(IDC_EDIT_START_POS_Y)->m_hWnd)
		{
			SetPointMode(TRUE, enumSelectPoint_Start, _T("Line Start"));
			_ManualMove(enumSelectPoint_Start);
			return TRUE;
		}
		else if (pMsg->hwnd == GetDlgItem(IDC_EDIT_END_POS_X)->m_hWnd ||
			pMsg->hwnd == GetDlgItem(IDC_EDIT_END_POS_Y)->m_hWnd)
		{
			SetPointMode(TRUE, enumSelectPoint_End, _T("Line End"));
			_ManualMove(enumSelectPoint_End);
			return TRUE;
		}
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == 'S')
	{//Start Point
		SetPointMode(TRUE, enumSelectPoint_Start, _T("Line Start"));
		return TRUE;
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == 'E')
	{//Start Point
		SetPointMode(TRUE, enumSelectPoint_End, _T("Line End"));
		return TRUE;
	}

	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CFeatureAdd_Line_SubDlg::OnBnClickedButtonSelectAttribute()
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

void CFeatureAdd_Line_SubDlg::OnPolarityButtonClicked(UINT32 nID)
{
	if (nID <= 0) return;

	int nIndex = nID - IDC_RADIO_POLARITY_POS;

	if (nIndex == 0)
	{
		m_stAddParam.bPolariry = TRUE;
	}
	else
	{
		m_stAddParam.bPolariry = FALSE;
	}

}


void CFeatureAdd_Line_SubDlg::OnBnClickedButtonSelectSymbol()
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


void CFeatureAdd_Line_SubDlg::OnBnClickedButtonMakeSymbol()
{
	CMakeSymbolDlg dlg(eMakeSymbolMode::enumMode_Line);

	if (dlg.DoModal() == IDOK)
	{
		m_stAddParam.strSymbol = dlg.GetSymbolName();
		m_stAddParam.eSymbolName = static_cast<SymbolName>(dlg.GetType());
		m_stAddParam.vecPara = dlg.GetSymbolPara();

		_UpdateDataToUI();
	}
}

void CFeatureAdd_Line_SubDlg::OnEnChangeEditStartPosX()
{
	CString strTemp = _T("");

	GetDlgItem(IDC_EDIT_START_POS_X)->GetWindowText(strTemp);
	if (strTemp.GetLength() > 1)
		m_stAddParam.dStartX = _ttof(strTemp);
	else
		m_stAddParam.dStartX = 0.0;
}

void CFeatureAdd_Line_SubDlg::OnEnChangeEditStartPosY()
{
	CString strTemp = _T("");

	GetDlgItem(IDC_EDIT_START_POS_Y)->GetWindowText(strTemp);
	if (strTemp.GetLength() > 1)
		m_stAddParam.dStartY = _ttof(strTemp);
	else
		m_stAddParam.dStartY = 0.0;
}

void CFeatureAdd_Line_SubDlg::OnEnChangeEditEndPosX()
{
	CString strTemp = _T("");

	GetDlgItem(IDC_EDIT_END_POS_X)->GetWindowText(strTemp);
	if (strTemp.GetLength() > 1)
		m_stAddParam.dEndX = _ttof(strTemp);
	else
		m_stAddParam.dEndX = 0.0;
}

void CFeatureAdd_Line_SubDlg::OnEnChangeEditEndPosY()
{
	CString strTemp = _T("");

	GetDlgItem(IDC_EDIT_END_POS_Y)->GetWindowText(strTemp);
	if (strTemp.GetLength() > 1)
		m_stAddParam.dEndY = _ttof(strTemp);
	else
		m_stAddParam.dEndY = 0.0;
}

UINT32 CFeatureAdd_Line_SubDlg::_InitSymbol()
{
	CJobFile* pJobData = GetJobPtr();
	if (pJobData == nullptr) return RESULT_BAD;

	int nSelectStep = GetCurStepIndex();
	if (nSelectStep < 0 || nSelectStep >= pJobData->m_arrStep.GetSize())
		return RESULT_BAD;

	CStep *pStep = pJobData->m_arrStep.GetAt(nSelectStep);
	if (pStep == nullptr) return RESULT_BAD;

	if (m_pParent == nullptr) return RESULT_BAD;
	CString strLayerName = m_pParent->GetLayerName();

	int nLayerIndex;//사용안함.
	CLayer* pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, strLayerName, nLayerIndex);
	if (pLayer == nullptr) return RESULT_BAD;

	int iSelSymbol = -1;
	double dMinWidth = DBL_MAX;
	vector<CString> vcSymbolNames;
	int nSymbolCount = static_cast<int>(pLayer->m_FeatureFile.m_arrSymbol.GetCount());
	for (int i = 0; i < nSymbolCount; i++)
	{
		CSymbol *pSymbol = pLayer->m_FeatureFile.m_arrSymbol.GetAt(i);
		if (pSymbol == nullptr) continue;

		SymbolName eSybolType = CDrawFunction::GetSymbolType(pSymbol->m_strUserSymbol);

		if (eSybolType == SymbolName::r || eSybolType == SymbolName::s)
		{
			vcSymbolNames.emplace_back(pSymbol->m_strUserSymbol);

			CString strTemp = pSymbol->m_strUserSymbol;
			strTemp.Delete(0, 1);
			strTemp.Trim();

			if (strTemp.GetLength() < 1)
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
			if (pSymbol == nullptr) continue;

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

UINT32 CFeatureAdd_Line_SubDlg::_ManualMove(int nIndex)
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

	Draw_ManualMove(0, ptXY);
	return RESULT_GOOD;
}