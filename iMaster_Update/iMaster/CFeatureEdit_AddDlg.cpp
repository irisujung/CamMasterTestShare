// CFeatureEdit_AddDlg.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "MainFrm.h"
#include "CFeatureEdit_AddDlg.h"
#include "afxdialogex.h"

#include "CFeatureAdd_Pad_SubDlg.h"
#include "CFeatureAdd_Line_SubDlg.h"
#include "CFeatureAdd_Arc_SubDlg.h"
#include "CFeatureAdd_Surface_SubDlg.h"
#include "CFeatureAdd_TP_SubDlg.h"

// CFeatureEdit_AddDlg 대화 상자

IMPLEMENT_DYNAMIC(CFeatureEdit_AddDlg, CDialogEx)

CFeatureEdit_AddDlg::CFeatureEdit_AddDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDIT_ADD, pParent)
{

}

CFeatureEdit_AddDlg::~CFeatureEdit_AddDlg()
{
	_DeleteSubDlg();
}

void CFeatureEdit_AddDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_LAYER, m_cbLayer);
}


BEGIN_MESSAGE_MAP(CFeatureEdit_AddDlg, CDialogEx)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_SHAPE_LINE, IDC_RADIO_SHAPE_TP, &CFeatureEdit_AddDlg::OnModeButtonClicked)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CFeatureEdit_AddDlg::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CFeatureEdit_AddDlg::OnBnClickedButtonCancel)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_SETDATA, &CFeatureEdit_AddDlg::OnBnClickedButtonSetdata)

END_MESSAGE_MAP()


// CFeatureEdit_AddDlg 메시지 처리기

BOOL CFeatureEdit_AddDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	_InitSubDlg();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CFeatureEdit_AddDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow == TRUE)
	{
		((CButton*)GetDlgItem(IDC_RADIO_SHAPE_LINE))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_SHAPE_PAD))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_SHAPE_ARC))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_SHAPE_SURFACE))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_SHAPE_TP))->SetCheck(FALSE);
		_SetLayerName();//Combo Box에 선택된 Layer 이름 입력
		_ShowSubDlg(enumSubDlg_Line);
	}
	else
	{
		_ShowSubDlg(enumSubDlg_Hide);

		SetViewerEditMode(EditMode::enumMode_EditMode);//AddMode 에서 일반 Edit Mode로 변경

		AddApplicationLog(END_EDIT_ADD_TXT);
	}
}

void CFeatureEdit_AddDlg::OnModeButtonClicked(UINT32 nID)
{
	if (nID <= 0) return;

	int nIndex = nID - IDC_RADIO_SHAPE_LINE;

	_ShowSubDlg(nIndex);
}

UINT32 CFeatureEdit_AddDlg::SetSetlectPoint(IN const int &nIndex, IN const float &fX, IN const float &fY)
{
	UINT32 nRet = RESULT_BAD;
	switch (m_nMode)
	{
	case enumSubDlg_Line:
		nRet = m_pLineSubDlg->SetSetlectPoint(nIndex, fX, fY);
		break;
	case enumSubDlg_Pad:
		nRet = m_pPadSubDlg->SetSetlectPoint(fX, fY);
		break;
	case enumSubDlg_Arc:
		nRet = m_pArcSubDlg->SetSelectPoint(nIndex, fX, fY);
	case enumSubDlg_Surface:
		nRet = m_pSurfaceSubDlg->SetSelectPoint(nIndex, fX, fY);
		break;
	case enumSubDlg_Tp:
		nRet = m_pTpSubDlg->SetSetlectPoint(fX, fY);
	default:
		nRet = RESULT_BAD;
		break;

	}

	return nRet;
}

UINT32 CFeatureEdit_AddDlg::SetSetlectPoint_Center(IN const int &nIndex, IN const float &fX, IN const float &fY)
{
	UINT32 nRet = RESULT_BAD;
	switch (m_nMode)
	{
	case enumSubDlg_Arc:
		//nRet = m_pArcSubDlg->SetSelectPoint(nIndex, fX, fY);
	default:
		nRet = RESULT_BAD;
		break;
	case enumSubDlg_Surface:
		nRet = m_pSurfaceSubDlg->SetSelectPoint_Center(nIndex, fX, fY);
		break;

	}

	return nRet;
}

void CFeatureEdit_AddDlg::_InitSubDlg()
{
	if (m_pLineSubDlg == nullptr)
	{
		m_pLineSubDlg = new CFeatureAdd_Line_SubDlg();
		m_pLineSubDlg->Create(IDD_EDIT_ADD_SUB_LINE, this);
		m_pLineSubDlg->SetWindowPos(nullptr, 0, enumSubDlg_StartY, 0, 0, SWP_NOSIZE);
		m_pLineSubDlg->SetLink(this);
	}

	if (m_pPadSubDlg == nullptr)
	{
		m_pPadSubDlg = new CFeatureAdd_Pad_SubDlg();
		m_pPadSubDlg->Create(IDD_EDIT_ADD_SUB_PAD, this);
		m_pPadSubDlg->SetWindowPos(nullptr, 0, enumSubDlg_StartY, 0, 0, SWP_NOSIZE);
		m_pPadSubDlg->SetLink(this);
	}
	if (m_pArcSubDlg == nullptr)
	{
		m_pArcSubDlg = new CFeatureAdd_Arc_SubDlg();
		m_pArcSubDlg->Create(IDD_EDIT_ADD_SUB_ARC, this);
		m_pArcSubDlg->SetWindowPos(nullptr, 0, enumSubDlg_StartY, 0, 0, SWP_NOSIZE);
		m_pArcSubDlg->SetLink(this);
	}
	if (m_pSurfaceSubDlg == nullptr)
	{
		m_pSurfaceSubDlg = new CFeatureAdd_Surface_SubDlg();
		m_pSurfaceSubDlg->Create(IDD_EDIT_ADD_SUB_SURFACE, this);
		m_pSurfaceSubDlg->SetWindowPos(nullptr, 0, enumSubDlg_StartY, 0, 0, SWP_NOSIZE);
		m_pSurfaceSubDlg->SetLink(this);
	}
	if (m_pTpSubDlg == nullptr)
	{
		m_pTpSubDlg = new CFeatureAdd_TP_SubDlg();
		m_pTpSubDlg->Create(IDD_EDIT_ADD_SUB_TP, this);
		m_pTpSubDlg->SetWindowPos(nullptr, 0, enumSubDlg_StartY, 0, 0, SWP_NOSIZE);
		m_pTpSubDlg->SetLink(this);

	}


}

void CFeatureEdit_AddDlg::_ShowSubDlg(int nSubMode)
{
	m_nMode = nSubMode;
	switch (nSubMode)
	{
	default:
	case enumSubDlg_Line:
	{
		m_pLineSubDlg->ShowWindow(SW_SHOW);
		m_pPadSubDlg->ShowWindow(SW_HIDE);
		m_pArcSubDlg->ShowWindow(SW_HIDE);
		m_pSurfaceSubDlg->ShowWindow(SW_HIDE);
		m_pTpSubDlg->ShowWindow(SW_HIDE);
		SetWindowText(_T("Feature Add - Line"));
	}
	break;
	case enumSubDlg_Pad:
	{
		m_pLineSubDlg->ShowWindow(SW_HIDE);
		m_pPadSubDlg->ShowWindow(SW_SHOW);
		m_pArcSubDlg->ShowWindow(SW_HIDE);
		m_pSurfaceSubDlg->ShowWindow(SW_HIDE);
		m_pTpSubDlg->ShowWindow(SW_HIDE);
		SetWindowText(_T("Feature Add - Pad"));
	}
	break;
	case enumSubDlg_Arc:
	{
		m_pLineSubDlg->ShowWindow(SW_HIDE);
		m_pPadSubDlg->ShowWindow(SW_HIDE);
		m_pArcSubDlg->ShowWindow(SW_SHOW);
		m_pSurfaceSubDlg->ShowWindow(SW_HIDE);
		m_pTpSubDlg->ShowWindow(SW_HIDE);
		SetWindowText(_T("Feature Add-Arc"));
	}
	break;
	case enumSubDlg_Surface:
	{
		m_pLineSubDlg->ShowWindow(SW_HIDE);
		m_pPadSubDlg->ShowWindow(SW_HIDE);
		m_pArcSubDlg->ShowWindow(SW_HIDE);
		m_pSurfaceSubDlg->ShowWindow(SW_SHOW);
		m_pTpSubDlg->ShowWindow(SW_HIDE);
		SetWindowText(_T("Feature Add-Surface"));
	}
	break;
	case enumSubDlg_Tp:
	{
		m_pLineSubDlg->ShowWindow(SW_HIDE);
		m_pPadSubDlg->ShowWindow(SW_HIDE);
		m_pArcSubDlg->ShowWindow(SW_HIDE);
		m_pSurfaceSubDlg->ShowWindow(SW_HIDE);
		m_pTpSubDlg->ShowWindow(SW_SHOW);
		SetWindowText(_T("Feature Add-Surface"));
	}
	break;
	case enumSubDlg_Hide:
	{
		m_pLineSubDlg->ShowWindow(SW_HIDE);
		m_pPadSubDlg->ShowWindow(SW_HIDE);
		m_pArcSubDlg->ShowWindow(SW_HIDE);
		m_pSurfaceSubDlg->ShowWindow(SW_HIDE);
		m_pTpSubDlg->ShowWindow(SW_HIDE);
	}
	break;
	}
}

void CFeatureEdit_AddDlg::_DeleteSubDlg()
{
	if (m_pPadSubDlg != nullptr)
	{
		delete m_pPadSubDlg;
		m_pPadSubDlg = nullptr;

	}

	if (m_pLineSubDlg != nullptr)
	{
		delete m_pLineSubDlg;
		m_pLineSubDlg = nullptr;
	}

	if (m_pArcSubDlg != nullptr)
	{
		delete m_pArcSubDlg;
		m_pArcSubDlg = nullptr;
	}

	if (m_pSurfaceSubDlg != nullptr)
	{
		delete m_pSurfaceSubDlg;
		m_pSurfaceSubDlg = nullptr;
	}

	if (m_pTpSubDlg != nullptr)
	{
		delete m_pTpSubDlg;
		m_pTpSubDlg;
	}
}

void CFeatureEdit_AddDlg::_SetLayerName()
{
	m_cbLayer.ResetContent();

	UserLayerSet_t* pUserLayerSet = GetUserLayerSet();
	if (pUserLayerSet == nullptr) return;

	SystemSpec_t* pSystemSpec = GetSystemSpec();
	if (pSystemSpec == nullptr) return;
	eMachineType eMcType = pSystemSpec->sysBasic.McType;

	int nCount = static_cast<int>(pUserLayerSet->vcLayerSet.size());
	for (int i = 0; i < nCount; i++)
	{
		//NSIS일 경우 Measure Layer만 편집
		if (eMcType == eMachineType::eNSIS &&
			pUserLayerSet->vcLayerSet[i]->strDefine != MEASURE_LAYER_TXT)
		{
			continue;
		}

		if (pUserLayerSet->vcLayerSet[i]->bCheck == true)
		{
			m_cbLayer.AddString(pUserLayerSet->vcLayerSet[i]->strLayer);
		}

	}

	if (nCount > 0)
	{
		m_cbLayer.SetCurSel(0);
	}

}

CString CFeatureEdit_AddDlg::GetLayerName()
{//Combo Box에서 선택된 Layer 이름 가져오기
	CString strLayer;
	m_cbLayer.GetWindowText(strLayer);

	return strLayer;
}

BOOL CFeatureEdit_AddDlg::_IsAllDataInput()
{
	BOOL bRet = FALSE;
	switch (m_nMode)
	{
	default:
	case enumSubDlg_Line:
	{
		bRet = m_pLineSubDlg->IsAllDataInput();
	}
	break;
	case enumSubDlg_Pad:
	{
		bRet = m_pPadSubDlg->IsAllDataInput();
	}
	break;
	case enumSubDlg_Arc:
	{
		bRet = m_pArcSubDlg->IsAllDataInput();
	}
	break;
	case enumSubDlg_Surface:
	{
		bRet = m_pSurfaceSubDlg->IsAllDataInput();
	}
	break;
	case enumSubDlg_Tp:
	{
		bRet = m_pTpSubDlg->IsAllDataInput();
	}
	break;
	}

	return bRet;
}

void CFeatureEdit_AddDlg::_ClearFeatureData()
{
	int nCount = static_cast<int>(m_vecFeatureData.size());
	for (int i = 0; i < nCount; i++)
	{
		if (m_vecFeatureData[i]->m_pSymbol != nullptr)
		{
			delete m_vecFeatureData[i]->m_pSymbol;
			m_vecFeatureData[i]->m_pSymbol = nullptr;
		}

		int nAttNameCount = static_cast<int>(m_vecFeatureData[i]->m_arrAttribute.GetCount());
		for (int j = 0; j < nAttNameCount; j++)
		{
			if (m_vecFeatureData[i]->m_arrAttribute[j] != nullptr)
			{
				delete m_vecFeatureData[i]->m_arrAttribute[j];
				m_vecFeatureData[i]->m_arrAttribute[j] = nullptr;
			}
		}

		int nAttStringCount = static_cast<int>(m_vecFeatureData[i]->m_arrAttributeTextString.GetCount());
		for (int j = 0; j < nAttStringCount; j++)
		{
			if (m_vecFeatureData[i]->m_arrAttributeTextString[j] != nullptr)
			{
				delete m_vecFeatureData[i]->m_arrAttributeTextString[j];
				m_vecFeatureData[i]->m_arrAttributeTextString[j] = nullptr;
			}
		}

		if (m_vecFeatureData[i] != nullptr)
		{
			delete m_vecFeatureData[i];
			m_vecFeatureData[i] = nullptr;
		}
	}

	m_vecFeatureData.clear();
}

UINT32 CFeatureEdit_AddDlg::_MakeFeature()
{
	UINT32 nRet = RESULT_BAD;
	_ClearFeatureData();

	switch (m_nMode)
	{
	default:
	case enumSubDlg_Line:
	{
		nRet = m_pLineSubDlg->MakeFeature(m_vecFeatureData);
	}
	break;
	case enumSubDlg_Pad:
	{
		nRet = m_pPadSubDlg->MakeFeature(m_vecFeatureData);
	}
	break;
	case enumSubDlg_Arc:
	{
		nRet = m_pArcSubDlg->MakeFeature(m_vecFeatureData);
	}
	break;
	case enumSubDlg_Surface:
	{
		nRet = m_pSurfaceSubDlg->MakeFeature(m_vecFeatureData);
	}
	break;
	case enumSubDlg_Tp:
	{
		nRet = m_pTpSubDlg->MakeFeature(m_vecFeatureData);
	}
	break;
	}

	return nRet;
}

void CFeatureEdit_AddDlg::OnBnClickedButtonSetdata()
{
	if (_IsAllDataInput() == FALSE)
	{
		AfxMessageBox(_T("필수 데이터가 입력되지 않았습니다"));
		return;
	}

	if (_MakeFeature() == RESULT_GOOD)
	{
		SetAdd(GetLayerName(), m_vecFeatureData);

		_ClearFeatureData();
	}
}

void CFeatureEdit_AddDlg::OnBnClickedButtonApply()
{
	if (_IsAllDataInput() == FALSE)
	{
		AfxMessageBox(_T("필수 데이터가 입력되지 않았습니다"));
		return;
	}

	if (_MakeFeature() == RESULT_GOOD)
	{	
		SetAdd_Apply(GetLayerName(), m_vecFeatureData);

		AddApplicationLog(APPLY_EDIT_ADD_TXT);

		CString strLog;
		for (auto it : m_vecFeatureData)
		{
			CString strAttr = _T(""), strAttrString = _T(""), strSymbol = _T("");

			if (it->m_arrAttribute.GetSize() > 0)
			{
				CAttribute* pAttr = it->m_arrAttribute.GetAt(it->m_arrAttribute.GetSize() - 1);
				if (pAttr)
					strAttr = pAttr->m_strName;
			}

			if (it->m_arrAttributeTextString.GetSize() > 0)
			{
				CAttributeTextString* pAttrString = it->m_arrAttributeTextString.GetAt(it->m_arrAttributeTextString.GetSize() - 1);
				if (pAttrString)
					strAttrString = pAttrString->m_strText;
			}

			CSymbol* pSymbol = it->m_pSymbol;
			if (pSymbol)
				strSymbol = pSymbol->m_strUserSymbol;


			switch (it->m_eType)
			{
			case FeatureType::P:
				strLog.Format(_T("[FEATURE-PAD] %s, %s, %s "), strSymbol, strAttr, strAttrString);
				break;
			case FeatureType::L:
				strLog.Format(_T("[FEATURE-LINE] %s, %s, %s"), strSymbol, strAttr, strAttrString);
				break;
			case FeatureType::A:
				strLog.Format(_T("[FEATURE_ARC] %s, %s, %s"), strSymbol, strAttr, strAttrString);
				break;
			case FeatureType::S:
				strLog.Format(_T("[FEATURE_SURFACE] %s,%s"), strAttr, strAttrString);
			}

			AddApplicationLog(strLog);
		}

		_ClearFeatureData();
	}

	ShowWindow(FALSE);
}

void CFeatureEdit_AddDlg::OnBnClickedButtonCancel()
{
	ShowWindow(FALSE);
}

BOOL CFeatureEdit_AddDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) return TRUE;

	if (m_pLineSubDlg->IsWindowVisible() == TRUE)
	{
		m_pLineSubDlg->PreTranslateMessage(pMsg);
	}
	else if (m_pPadSubDlg->IsWindowVisible() == TRUE)
	{
		m_pPadSubDlg->PreTranslateMessage(pMsg);
	}
	else if (m_pArcSubDlg->IsWindowVisible() == TRUE)
	{
		m_pArcSubDlg->PreTranslateMessage(pMsg);
	}
	else if (m_pSurfaceSubDlg->IsWindowVisible() == TRUE)
	{
		m_pSurfaceSubDlg->PreTranslateMessage(pMsg);
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}



