// CFeatureAdd_Surface_SubDlg.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "MainFrm.h"
#include "CFeatureEdit_AddDlg.h"
#include "CFeatureAdd_Surface_SubDlg.h"
#include "afxdialogex.h"

#include "CMakeSymbolDlg.h"
#include "CMakeAttributeDlg.h"
#include "CSelectSymbolDlg.h"


// CFeatureAdd_Surface_SubDlg 대화 상자

IMPLEMENT_DYNAMIC(CFeatureAdd_Surface_SubDlg, CDialogEx)

CFeatureAdd_Surface_SubDlg::CFeatureAdd_Surface_SubDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDIT_ADD_SUB_SURFACE, pParent)
{

}

CFeatureAdd_Surface_SubDlg::~CFeatureAdd_Surface_SubDlg()
{
}
void CFeatureAdd_Surface_SubDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFeatureAdd_Surface_SubDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_SELECT_ATTRIBUTE, &CFeatureAdd_Surface_SubDlg::OnBnClickedButtonSelectAttribute)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_POLARITY_POS, IDC_RADIO_POLARITY_NEG, &CFeatureAdd_Surface_SubDlg::OnPolyTypeButtonClicked)
	ON_BN_CLICKED(IDC_BUTTON_ADD_POSITON, &CFeatureAdd_Surface_SubDlg::OnBnClickedButtonAddPositon)



END_MESSAGE_MAP()


// CFeatureAdd_Surface_SubDlg 메시지 처리기


void CFeatureAdd_Surface_SubDlg::SetLink(CFeatureEdit_AddDlg * pParent)
{
	m_pParent = pParent;
}



BOOL CFeatureAdd_Surface_SubDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	_InitValue();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CFeatureAdd_Surface_SubDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow == TRUE)
	{
		ClearTempPoint();
	}
	else
	{
		m_nPointIndex = 0;
		m_vecPoint.clear();
		m_vecSurfacePoint.clear();
	}
}

UINT32 CFeatureAdd_Surface_SubDlg::SetSelectPoint(IN const int&nIndex, IN const float &fX, IN const float &fY)
{
	SurfacePoint _vecPoint;

	if (_vecPoint.bOsOc == true)
	{
		_vecPoint.AddPoint = CPointD(fX, fY);
		m_vecPoint.push_back(_vecPoint);
		m_nPointIndex++;
	}

	SetPointMode(TRUE, m_nPointIndex, _T("Add Point"));



	return RESULT_GOOD;


}

UINT32 CFeatureAdd_Surface_SubDlg::SetSelectPoint_Center(IN const int&nIndex, IN const float &fX, IN const float &fY)
{
	if (static_cast<int>(m_vecPoint.size()) <= nIndex) return RESULT_BAD;

	m_vecPoint[nIndex].bOsOc = false;
	m_vecPoint[nIndex].bCCW = true;
	m_vecPoint[nIndex].CenterPoint = CPointD(fX, fY);
	m_nPointIndex = nIndex + 1;

	SetPointMode(TRUE, m_nPointIndex, _T("Center Point"));

	return RESULT_GOOD;
}


UINT32 CFeatureAdd_Surface_SubDlg::MakeFeature(vector<CFeature*> &vecFeature)
{
	vecFeature.resize(1);

	CObOe* pObOe = new CObOe;
	pObOe->m_dXbs = m_vecPoint.at(0).AddPoint.x;
	pObOe->m_dYbs = m_vecPoint.at(0).AddPoint.y;
	pObOe->m_bHighlight = true;

	pObOe->m_bPolyType = m_stAddParam.bPolytype;


	CFeature* pFeature = new CFeatureS;
	CFeatureS *pFeatureS = (CFeatureS *)pFeature;



	COsOc* pOsOc = new COsOc;

	for (int i = 1; i < m_nPointIndex; i++)
	{
		if (m_vecPoint.at(i).bOsOc == true)
		{
			COsOc* pOsOc = new COs;
			pOsOc->m_bOsOc = true;
			pOsOc->m_dX = m_vecPoint.at(i).AddPoint.x;
			pOsOc->m_dY = m_vecPoint.at(i).AddPoint.y;
			pObOe->m_arrOsOc.Add(pOsOc);
		}

		else
		{
			COsOc* pOsOc = new COc;
			pOsOc->m_bOsOc = false;
			pOsOc->m_dX = m_vecPoint.at(i).AddPoint.x;
			pOsOc->m_dY = m_vecPoint.at(i).AddPoint.y;
			((COc*)pOsOc)->m_bCw = m_vecPoint.at(i).bCCW;
			((COc*)pOsOc)->m_dXc = m_vecPoint.at(i).CenterPoint.x;
			((COc*)pOsOc)->m_dYc = m_vecPoint.at(i).CenterPoint.y;
			pObOe->m_arrOsOc.Add(pOsOc);
		}


	}



	pOsOc->m_bOsOc = true;
	pOsOc->m_dX = m_vecPoint[0].AddPoint.x;
	pOsOc->m_dY = m_vecPoint[0].AddPoint.y;
	pObOe->m_arrOsOc.Add(pOsOc);

	pFeatureS->m_arrObOe.Add(pObOe);

	pFeatureS->m_bPolarity = true;
	vecFeature[0] = pFeatureS;

	return RESULT_GOOD;

}




void CFeatureAdd_Surface_SubDlg::_InitValue()
{
	m_stAddParam.Reset();
	_UpdateDataToUI();
}

void CFeatureAdd_Surface_SubDlg::_UpdateDataToUI()
{
	if (m_stAddParam.bPolytype == TRUE)
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

	GetDlgItem(IDC_EDIT_ATTRIBUTE_STRING)->SetWindowTextW(strTemp);
}

void CFeatureAdd_Surface_SubDlg::_UpdateUIToData()
{
}

BOOL CFeatureAdd_Surface_SubDlg::IsAllDataInput()
{
	BOOL bRet = TRUE;

	return bRet;
}


BOOL CFeatureAdd_Surface_SubDlg::PreTranslateMessage(MSG* pMsg)
{

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == 'C')
	{
		int nCurIndex = m_nPointIndex - 1;
		if (m_vecPoint.size() > nCurIndex && nCurIndex > 0)
		{
			//Set_CW(!m_vecPoint[nCurIndex].bCCW); // 가상 Arc CW 설정
			SetCW(!m_vecPoint[nCurIndex].bCCW); // Surface Arc CW 설정 

		}
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);

}



void CFeatureAdd_Surface_SubDlg::OnBnClickedButtonSelectAttribute()
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

void CFeatureAdd_Surface_SubDlg::OnPolyTypeButtonClicked(UINT32 nID)
{
	if (nID <= 0) return;

	int nIndex = nID - IDC_RADIO_POLARITY_POS;

	if (nIndex == 0)
	{
		m_stAddParam.bPolytype = TRUE;
	}

	else
	{
		m_stAddParam.bPolytype = FALSE;
	}

}



UINT32 CFeatureAdd_Surface_SubDlg::_InitSymbol()
{
	return RESULT_GOOD;
}



void CFeatureAdd_Surface_SubDlg::OnBnClickedButtonAddPositon()
{
	m_nPointIndex = 0;
	ClearTempPoint();
	m_vecPoint.clear();
	SetPointMode(TRUE, m_nPointIndex, _T("Add Point"));
}


UINT32 CFeatureAdd_Surface_SubDlg::SetCW(IN BOOL bCCW)

{
	int prePoint = m_nPointIndex - 1;

	if (m_vecPoint[prePoint].bOsOc == false && m_vecPoint[prePoint].bCCW == false)
	{
		m_vecPoint[prePoint].bCCW = true;
	}
	else
	{
		m_vecPoint[prePoint].bCCW = false;
	}

	return RESULT_GOOD;
}



