// CFeatureAdd_TP_SubDlg.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "MainFrm.h"
#include "CFeatureAdd_TP_SubDlg.h"
#include "CFeatureEdit_AddDlg.h"
#include "afxdialogex.h"


// CFeatureAdd_TP_SubDlg 대화 상자

IMPLEMENT_DYNAMIC(CFeatureAdd_TP_SubDlg, CDialogEx)

CFeatureAdd_TP_SubDlg::CFeatureAdd_TP_SubDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDIT_ADD_SUB_TP, pParent)
{

}

CFeatureAdd_TP_SubDlg::~CFeatureAdd_TP_SubDlg()
{
}

void CFeatureAdd_TP_SubDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFeatureAdd_TP_SubDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ADD_CROSS, &CFeatureAdd_TP_SubDlg::OnBnClickedButtonAddCross)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_CROSS_CENTER, &CFeatureAdd_TP_SubDlg::OnBnClickedButtonSelectCrossCenter)
END_MESSAGE_MAP()


// CFeatureAdd_TP_SubDlg 메시지 처리기


void CFeatureAdd_TP_SubDlg::SetLink (CFeatureEdit_AddDlg * pParent)
{
	m_pParent = pParent;
}

BOOL CFeatureAdd_TP_SubDlg::IsAllDataInput()
{
	BOOL bRet = TRUE;
	return bRet;
}

UINT32 CFeatureAdd_TP_SubDlg::SetSetlectPoint(IN const float &fX, IN const float &fY)
{

	SurfaceCross _vecCross;

	if (fX != 0.f && fY != 0.f)
	{
		
		m_CenterPoint.x = fX;
		m_CenterPoint.y = fY;
		

		return RESULT_GOOD;
	}

	return RESULT_BAD;
}


UINT32 CFeatureAdd_TP_SubDlg::SetCross() 
{
	SurfaceCross _vecCross;


	_vecCross.AddPoint.x = m_CenterPoint.x - 0.1;
	_vecCross.AddPoint.y = m_CenterPoint.y - 0.3;
	_vecCross.bOsOc = true;
	m_vecPoint.push_back(_vecCross);

	_vecCross.AddPoint.x = m_CenterPoint.x - 0.1;
	_vecCross.AddPoint.y = m_CenterPoint.y - 0.1;
	_vecCross.bOsOc = true;
	m_vecPoint.push_back(_vecCross);

	_vecCross.AddPoint.x = m_CenterPoint.x - 0.3;
	_vecCross.AddPoint.y = m_CenterPoint.y - 0.1;
	_vecCross.bOsOc = true;
	m_vecPoint.push_back(_vecCross);

	_vecCross.AddPoint.x = m_CenterPoint.x - 0.3;
	_vecCross.AddPoint.y = m_CenterPoint.y + 0.1;
	_vecCross.bOsOc = true;
	m_vecPoint.push_back(_vecCross);

	_vecCross.AddPoint.x = m_CenterPoint.x - 0.1;
	_vecCross.AddPoint.y = m_CenterPoint.y + 0.1;
	_vecCross.bOsOc = true;
	m_vecPoint.push_back(_vecCross);

	_vecCross.AddPoint.x = m_CenterPoint.x - 0.1;
	_vecCross.AddPoint.y = m_CenterPoint.y + 0.3;
	_vecCross.bOsOc = true;
	m_vecPoint.push_back(_vecCross);

	_vecCross.AddPoint.x = m_CenterPoint.x + 0.1;
	_vecCross.AddPoint.y = m_CenterPoint.y + 0.3;
	_vecCross.bOsOc = true;
	m_vecPoint.push_back(_vecCross);

	_vecCross.AddPoint.x = m_CenterPoint.x + 0.1;
	_vecCross.AddPoint.y = m_CenterPoint.y + 0.1;
	_vecCross.bOsOc = true;
	m_vecPoint.push_back(_vecCross);

	_vecCross.AddPoint.x = m_CenterPoint.x + 0.3;
	_vecCross.AddPoint.y = m_CenterPoint.y + 0.1;
	_vecCross.bOsOc = true;
	m_vecPoint.push_back(_vecCross);

	_vecCross.AddPoint.x = m_CenterPoint.x + 0.3;
	_vecCross.AddPoint.y = m_CenterPoint.y - 0.1;
	_vecCross.bOsOc = true;
	m_vecPoint.push_back(_vecCross);

	_vecCross.AddPoint.x = m_CenterPoint.x + 0.1;
	_vecCross.AddPoint.y = m_CenterPoint.y - 0.1;
	_vecCross.bOsOc = true;
	m_vecPoint.push_back(_vecCross);

	_vecCross.AddPoint.x = m_CenterPoint.x + 0.1;
	_vecCross.AddPoint.y = m_CenterPoint.y - 0.3;
	_vecCross.bOsOc = true;
	m_vecPoint.push_back(_vecCross);

	m_nPointIndex = static_cast<int>(m_vecPoint.size());


	return RESULT_GOOD;
}

UINT32 CFeatureAdd_TP_SubDlg::MakeFeature(vector<CFeature *> &vecFeature)
{
	
	SetCross();

	vecFeature.resize(1);

	CObOe* pObOe = new CObOe;
	pObOe->m_dXbs = m_vecPoint.at(0).AddPoint.x;
	pObOe->m_dYbs = m_vecPoint.at(0).AddPoint.y;
	pObOe->m_bHighlight = true;
	CFeature* pFeature = new CFeatureS;
	CFeatureS *pFeatureS = (CFeatureS *)pFeature;
	pObOe->m_bPolyType = true;

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
	}

	pFeatureS->m_arrObOe.Add(pObOe);

	pFeatureS->m_bPolarity = true;
	vecFeature[0] = pFeatureS;

	return RESULT_GOOD;
}

void CFeatureAdd_TP_SubDlg::OnBnClickedButtonAddCross()
{


	CString strLayerName = m_pParent->GetLayerName();

	MakeFeature(m_vecFeatureData);

	SetAdd_Apply(strLayerName,m_vecFeatureData);
	
	m_pParent->ShowWindow(FALSE);

}


void CFeatureAdd_TP_SubDlg::OnBnClickedButtonSelectCrossCenter()
{
	m_vecPoint.clear();

	SetPointMode(TRUE, 0, _T("Cross Center"));
}
