// CFeatureAdd_Pad_SubDlg.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "MainFrm.h"
#include "CFeatureEdit_AddDlg.h"
#include "CFeatureAdd_Pad_SubDlg.h"
#include "afxdialogex.h"

#include "CMakeSymbolDlg.h"
#include "CMakeAttributeDlg.h"
#include "CSelectSymbolDlg.h"

// CFeatureAdd_Pad_SubDlg 대화 상자

IMPLEMENT_DYNAMIC(CFeatureAdd_Pad_SubDlg, CDialogEx)

CFeatureAdd_Pad_SubDlg::CFeatureAdd_Pad_SubDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDIT_ADD_SUB_PAD, pParent)
{

}

CFeatureAdd_Pad_SubDlg::~CFeatureAdd_Pad_SubDlg()
{
}

void CFeatureAdd_Pad_SubDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFeatureAdd_Pad_SubDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_SELECT_POS, &CFeatureAdd_Pad_SubDlg::OnBnClickedButtonSelectPos)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_ATTRIBUTE, &CFeatureAdd_Pad_SubDlg::OnBnClickedButtonSelectAttribute)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_POLARITY_POS, IDC_RADIO_POLARITY_NEG, &CFeatureAdd_Pad_SubDlg::OnPolarityButtonClicked)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_ANGLE_0, IDC_RADIO_ANGLE_270, &CFeatureAdd_Pad_SubDlg::OnAngleButtonClicked)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_MIRROR_NO, IDC_RADIO_MIRROR_YES, &CFeatureAdd_Pad_SubDlg::OnMirrorButtonClicked)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_SYMBOL, &CFeatureAdd_Pad_SubDlg::OnBnClickedButtonSelectSymbol)
	ON_BN_CLICKED(IDC_BUTTON_MAKE_SYMBOL, &CFeatureAdd_Pad_SubDlg::OnBnClickedButtonMakeSymbol)
	ON_EN_CHANGE(IDC_EDIT_CENTER_POS_X, &CFeatureAdd_Pad_SubDlg::OnEnChangeEditCenterPosX)
	ON_EN_CHANGE(IDC_EDIT_CENTER_POS_Y, &CFeatureAdd_Pad_SubDlg::OnEnChangeEditCenterPosY)
END_MESSAGE_MAP()


// CFeatureAdd_Pad_SubDlg 메시지 처리기

void	CFeatureAdd_Pad_SubDlg::SetLink(CFeatureEdit_AddDlg *pParent)
{
	m_pParent = pParent;
}


BOOL CFeatureAdd_Pad_SubDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	_InitValue();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CFeatureAdd_Pad_SubDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow == TRUE)
	{
		_InitValue();
		ClearTempPoint();
	}
}

void CFeatureAdd_Pad_SubDlg::OnBnClickedButtonSelectPos()
{
	SetPointMode(TRUE, 0, _T("Pad Center"));
}

UINT32 CFeatureAdd_Pad_SubDlg::SetSetlectPoint(IN const float &fX, IN const float &fY)
{
	if (fX != 0.f && fY != 0.f)
	{

		m_stAddParam.dCenterX = fX;
		m_stAddParam.dCenterY = fY;

		_UpdateDataToUI();

		return RESULT_GOOD;
	}

	return RESULT_BAD;
}

UINT32 CFeatureAdd_Pad_SubDlg::MakeFeature(vector<CFeature*> &vecFeatureData)
{
	_UpdateUIToData();

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;
	
	pFeatureP->m_dX = m_stAddParam.dCenterX;
	pFeatureP->m_dY = m_stAddParam.dCenterY;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = m_stAddParam.bPolariry;

	//Orient
	if (m_stAddParam.bMirror == FALSE)
	{
		pFeatureP->m_eOrient = static_cast<Orient>(m_stAddParam.nAngleMode);
	}
	else
	{
		pFeatureP->m_eOrient = static_cast<Orient>(m_stAddParam.nAngleMode + 4);
	}

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	//
	if (m_stAddParam.strAttributeName != _T(""))
	{
		CAttribute* pAttribute = new CAttribute();

		pAttribute->m_strName = m_stAddParam.strAttributeName;

		pFeatureP->m_arrAttribute.Add(pAttribute);
	}

	int nStringCount = static_cast<int>(m_stAddParam.vecAttributeString.size());
	for (int i = 0; i < nStringCount; i++)
	{
		CAttributeTextString* pAttributeString = new CAttributeTextString();

		pAttributeString->m_strText = m_stAddParam.vecAttributeString[i];

		pFeatureP->m_arrAttributeTextString.Add(pAttributeString);
	}
	

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = m_stAddParam.eSymbolName;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;
	pSymbol->m_vcParams = m_stAddParam.vecPara;
	pSymbol->m_strUserSymbol = m_stAddParam.strSymbol;

	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	return RESULT_GOOD;
}

void CFeatureAdd_Pad_SubDlg::OnBnClickedButtonSelectAttribute()
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

		//StringUtil::Split(strAttString, _T(","), m_stAddParam.vecAttributeString);

		_UpdateDataToUI();
	}
}

void	CFeatureAdd_Pad_SubDlg::_InitValue()
{
	m_stAddParam.Reset();
	_InitSymbol();
	_UpdateDataToUI();
}

void	CFeatureAdd_Pad_SubDlg::_UpdateDataToUI()
{
	CString strTemp;

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

	//Pos
	strTemp.Format(_T("%.6lf"), m_stAddParam.dCenterX);
	GetDlgItem(IDC_EDIT_CENTER_POS_X)->SetWindowText(strTemp);

	strTemp.Format(_T("%.6lf"), m_stAddParam.dCenterY);
	GetDlgItem(IDC_EDIT_CENTER_POS_Y)->SetWindowText(strTemp);
	
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
	
	switch (m_stAddParam.nAngleMode)
	{
	default:
	case 0 : 
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_0))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_90))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_180))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_270))->SetCheck(FALSE);
		break;
	case 1:
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_0))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_90))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_180))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_270))->SetCheck(FALSE);
		break;
	case 2:
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_0))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_90))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_180))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_270))->SetCheck(FALSE);
		break;
	case 3:
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_0))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_90))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_180))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_270))->SetCheck(TRUE);
		break;
	}
	
	if (m_stAddParam.bMirror == TRUE)
	{
		((CButton*)GetDlgItem(IDC_RADIO_MIRROR_NO))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_MIRROR_YES))->SetCheck(TRUE);
		
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO_MIRROR_NO))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_MIRROR_YES))->SetCheck(FALSE);
	}
}

void	CFeatureAdd_Pad_SubDlg::_UpdateUIToData()
{
}

BOOL	CFeatureAdd_Pad_SubDlg::IsAllDataInput()
{
	BOOL bRet = TRUE;

	CString strTemp;
	GetDlgItem(IDC_EDIT_SYMBOL)->GetWindowText(strTemp);
	if (strTemp == _T(""))
	{
		bRet = FALSE;
	}

	return bRet;
}

BOOL CFeatureAdd_Pad_SubDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		if (pMsg->hwnd == GetDlgItem(IDC_EDIT_CENTER_POS_X)->m_hWnd ||
			pMsg->hwnd == GetDlgItem(IDC_EDIT_CENTER_POS_Y)->m_hWnd)
		{
			_ManualMove();
			return TRUE;
		}
	}
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == 'S')
	{//Start Point
		SetPointMode(TRUE, 0, _T("Pad Center"));
		return TRUE;
	}

	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CFeatureAdd_Pad_SubDlg::OnPolarityButtonClicked(UINT32 nID)
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

void CFeatureAdd_Pad_SubDlg::OnAngleButtonClicked(UINT32 nID)
{
	if (nID <= 0) return;

	int nIndex = nID - IDC_RADIO_ANGLE_0;

	m_stAddParam.nAngleMode = nIndex;

}

void CFeatureAdd_Pad_SubDlg::OnMirrorButtonClicked(UINT32 nID)
{
	if (nID <= 0) return;

	int nIndex = nID - IDC_RADIO_MIRROR_NO;

	if (nIndex == 0)
	{
		m_stAddParam.bMirror = FALSE;
	}
	else
	{
		m_stAddParam.bMirror = TRUE;
	}
}

void CFeatureAdd_Pad_SubDlg::OnBnClickedButtonSelectSymbol()
{
	if (m_pParent == nullptr) return;
	CString strLayerName = m_pParent->GetLayerName();

	CSelectSymbolDlg dlg(TRUE, strLayerName);

	if (dlg.DoModal() == IDOK)
	{
		m_stAddParam.strSymbol = dlg.GetSymbolName();
		m_stAddParam.eSymbolName = static_cast<SymbolName>(dlg.GetType());
		m_stAddParam.vecPara = dlg.GetSymbolPara();

		_UpdateDataToUI();
	}
}


void CFeatureAdd_Pad_SubDlg::OnBnClickedButtonMakeSymbol()
{
	CMakeSymbolDlg dlg(eMakeSymbolMode::enumMode_Pad);

	if (dlg.DoModal() == IDOK)
	{
		m_stAddParam.strSymbol = dlg.GetSymbolName();
		m_stAddParam.eSymbolName = static_cast<SymbolName>(dlg.GetType());
		m_stAddParam.vecPara = dlg.GetSymbolPara();

		_UpdateDataToUI();
	}
}

void CFeatureAdd_Pad_SubDlg::OnEnChangeEditCenterPosX()
{
	CString strTemp = _T("");

	GetDlgItem(IDC_EDIT_CENTER_POS_X)->GetWindowText(strTemp);
	if (strTemp.GetLength() > 1)
		m_stAddParam.dCenterX = _ttof(strTemp);
	else
		m_stAddParam.dCenterX = 0.0;
}

void CFeatureAdd_Pad_SubDlg::OnEnChangeEditCenterPosY()
{
	CString strTemp = _T("");

	GetDlgItem(IDC_EDIT_CENTER_POS_Y)->GetWindowText(strTemp);
	if (strTemp.GetLength() > 1)
		m_stAddParam.dCenterY = _ttof(strTemp);
	else
		m_stAddParam.dCenterY = 0.0;
}

UINT32 CFeatureAdd_Pad_SubDlg::_InitSymbol()
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
	double dMaxWidth = 0.0;
	vector<CString> vcSymbolNames;
	int nSymbolCount = static_cast<int>(pLayer->m_FeatureFile.m_arrSymbol.GetCount());
	for (int i = 0; i < nSymbolCount; i++)
	{
		CSymbol *pSymbol = pLayer->m_FeatureFile.m_arrSymbol.GetAt(i);
		if (pSymbol == nullptr) continue;

		SymbolName eSybolType = CDrawFunction::GetSymbolType(pSymbol->m_strUserSymbol);
				
		vcSymbolNames.emplace_back(pSymbol->m_strUserSymbol);

		CString strTemp = pSymbol->m_strUserSymbol;
		strTemp.Delete(0, 1);
		strTemp.Trim();

		if (strTemp.GetLength() < 1)
			continue;

		double dWidth = _ttof(strTemp);
		if (dWidth > dMaxWidth)
		{
			dMaxWidth = dWidth;
			iSelSymbol = i;
		}
	}

	if (iSelSymbol >= 0)
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

UINT32 CFeatureAdd_Pad_SubDlg::_ManualMove()
{
	SetPointMode(TRUE, 0, _T("Pad Center"));
	PointDXY ptXY = CPointD(m_stAddParam.dCenterX, m_stAddParam.dCenterY);

	Draw_ManualMove(0, ptXY);
	return RESULT_GOOD;
}