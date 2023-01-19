// CAlignMaskEditDlg.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "MainFrm.h"
#include "CAlignMaskEditDlg.h"
#include "afxdialogex.h"
#include "SystemSpec.h"


// CAlignMaskEditDlg 대화 상자

IMPLEMENT_DYNAMIC(CAlignMaskEditDlg, CDialogEx)

CAlignMaskEditDlg::CAlignMaskEditDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ALIGN_MASK3, pParent)
{

}

CAlignMaskEditDlg::~CAlignMaskEditDlg()
{
}

UINT32 CAlignMaskEditDlg::SetSetlectPoint(IN const int & nIndex, IN const float & fX, IN const float & fY)
{
// 	switch (nIndex)
// 	{
// 	default:
// 	case enumSelectPoint_Start:
// 		m_fAddStartX = fX;
// 		m_fAddStartY = fY;
// 		break;
// 	case enumSelectPoint_End:
// 		m_fAddEndX = fX;
// 		m_fAddEndY = fY;
// 		break;
// 	}

	_UpdataPosSelect_Mode(FALSE);

	//_UpdateDataToUi();
	 
	return RESULT_GOOD;
}

UINT32 CAlignMaskEditDlg::SetSetlectFeatureInfo(IN SELECT_INFO *pSelectInfo)
{
	if (pSelectInfo == nullptr) return RESULT_BAD;

	int nCount = static_cast<int>(pSelectInfo->vecSelectInfoUnit.size());
	if (nCount <= 0)
	{
		((CButton *)GetDlgItem(IDC_CHECK_ADD_ALIGN_MASK_FEATURE_BASE))->SetCheck(FALSE);

 		AfxMessageBox(_T("Feature가 인식되지 않았습니다."));

		UINT32 nEditMode = EditMode::enumMode_EditMask;
		SetViewerEditMode(nEditMode);

		return RESULT_BAD;
	}

	SELECT_INFO_UNIT &stSelectInfo = pSelectInfo->vecSelectInfoUnit[0];

	UINT32 bRet = _MakeSquareFeature_SelectInfo(stSelectInfo, m_vecFeatureData);
	((CButton *)GetDlgItem(IDC_CHECK_ADD_ALIGN_MASK_FEATURE_BASE))->SetCheck(FALSE);
	if (bRet == RESULT_GOOD)
	{
		OnBnClickedCheckAddAlignMaskFeatureBase();
	}
	else
	{
		AfxMessageBox(_T("Feature가 인식되지 않았습니다."));

		UINT32 nEditMode = EditMode::enumMode_EditMask;
		SetViewerEditMode(nEditMode);
	}

	return RESULT_GOOD;
}

void CAlignMaskEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO_LAYER, m_cbLayer);	
	//DDX_Control(pDX, IDC_ALIGN_PROPERTYGRID, m_pPropGridCtrlAlign);
}


BEGIN_MESSAGE_MAP(CAlignMaskEditDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_TYPE_ALIGN, IDC_RADIO_TYPE_MASK, &CAlignMaskEditDlg::OnMaskTypeClicked)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CAlignMaskEditDlg::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CAlignMaskEditDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_SEARCH, &CAlignMaskEditDlg::OnBnClickedButtonAutoSearch)
	ON_BN_CLICKED(IDC_BUTTON_TEST_CLEAR, &CAlignMaskEditDlg::OnBnClickedButtonTestClear)
	ON_BN_CLICKED(IDC_BUTTON_ADD_ALIGN_MASK, &CAlignMaskEditDlg::OnBnClickedButtonAddAlignMask)
	ON_BN_CLICKED(IDC_RADIO_MOVE_XY, &CAlignMaskEditDlg::OnBnClickedRadioMoveXy)
	ON_BN_CLICKED(IDC_RADIO_MOVE_X, &CAlignMaskEditDlg::OnBnClickedRadioMoveX)
	ON_BN_CLICKED(IDC_RADIO_MOVE_Y, &CAlignMaskEditDlg::OnBnClickedRadioMoveY)
	ON_BN_CLICKED(IDC_BUTTON_RESIZE, &CAlignMaskEditDlg::OnBnClickedButtonResize)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CAlignMaskEditDlg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_CHECK_ADD_ALIGN_MASK_FEATURE_BASE, &CAlignMaskEditDlg::OnBnClickedCheckAddAlignMaskFeatureBase)
	ON_BN_CLICKED(IDC_BUTTON_ODB_LOCATION_1, &CAlignMaskEditDlg::OnBnClickedButtonLocationLeftTop)
	ON_BN_CLICKED(IDC_BUTTON_ODB_LOCATION_2, &CAlignMaskEditDlg::OnBnClickedButtonLocationLeftBottom)
	ON_BN_CLICKED(IDC_BUTTON_ODB_LOCATION_3, &CAlignMaskEditDlg::OnBnClickedButtonLocationRightTop)
	ON_BN_CLICKED(IDC_BUTTON_ODB_LOCATION_4, &CAlignMaskEditDlg::OnBnClickedButtonLocationRightBottom)
	//ON_BN_CLICKED(IDC_BUTTON_ADD_AUTO_ALIGN, &CAlignMaskEditDlg::OnBnClickedButtonAutoFindAlign)
	//ON_BN_CLICKED(IDC_BUTTON_SAVE_ALIGN_SPEC, &CAlignMaskEditDlg::OnBnClickedButtonSaveAlignSpec)	
END_MESSAGE_MAP()


// CAlignMaskEditDlg 메시지 처리기


BOOL CAlignMaskEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	_InitUI();
	_InitData();	
	ChangeButtonUI();
	//_InitAlignData();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CAlignMaskEditDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow == TRUE)
	{
		_InitUI();
		_InitData();		
		ChangeButtonUI();
	}
}

void			CAlignMaskEditDlg::_InitUI()
{	
	// 2022.08.08 KJH ADD
	// NISI 모드일 경우 Mask 버튼 막기 위해 구문 추가
	UserSetInfo_t *pUserSetInfo = GetUserSetInfo();
	if (pUserSetInfo == nullptr) return;
	if (pUserSetInfo->mcType == eMachineType::eNSIS)
	{
		((CButton*)GetDlgItem(IDC_RADIO_TYPE_MASK))->ShowWindow(FALSE);
		//((CButton*)GetDlgItem(IDC_BUTTON_ADD_AUTO_ALIGN))->ShowWindow(TRUE);

	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO_TYPE_MASK))->ShowWindow(TRUE);
		//((CButton*)GetDlgItem(IDC_BUTTON_ADD_AUTO_ALIGN))->ShowWindow(FALSE);
	}

	// NISI 삼성전기 모드일 경우 Auto Find Align 버튼 보기.
	/*if (pUserSetInfo->mcType == eMachineType::eNSIS && pUserSetInfo->userSite == eUserSite::eSEM)
	{
		((CButton*)GetDlgItem(IDC_BUTTON_ADD_AUTO_ALIGN))->ShowWindow(TRUE);		
	}
	else
	{
		((CButton*)GetDlgItem(IDC_BUTTON_ADD_AUTO_ALIGN))->ShowWindow(FALSE);
	}*/
	// KJH END

	m_eMaskType = MaskType::enumType_Align;

	((CButton*)GetDlgItem(IDC_RADIO_TYPE_ALIGN))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO_TYPE_MASK))->SetCheck(FALSE);

	//Move 축 기본값
	/*((CButton*)GetDlgItem(IDC_RADIO_MOVE_XY))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO_MOVE_X))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO_MOVE_Y))->SetCheck(FALSE);
	SetMoveAxis(Edit_MoveAxis::enumMoveAxis_XY);*/

	//뷰어 모드 셋
	SetMaskSelectMode(m_eMaskType);
	SetViewerEditMode(static_cast<int>(EditMode::enumMode_EditMask));

	_SetLayerName();
}

void			CAlignMaskEditDlg::_UpdataPosSelect_Mode(IN const BOOL &bOn)
{
	BOOL bEnable = !bOn;

	GetDlgItem(IDC_RADIO_TYPE_ALIGN)->EnableWindow(bEnable);
	GetDlgItem(IDC_RADIO_TYPE_MASK)->EnableWindow(bEnable);

	GetDlgItem(IDC_BUTTON_SETDATA)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(bEnable);
}

void			CAlignMaskEditDlg::_InitData()
{
	m_fResizeWidth = 100.;
	m_fResizeHeight = 100.;

	//_UpdateDataToUi();
}

UINT32			CAlignMaskEditDlg::_InitAlignData()
{
	//m_pPropGridCtrlAlign.SetWindowPos(NULL, 345, 20, 280, 110, NULL);
	m_pPropGridCtrlAlign.SetWindowPos(NULL, 300, 220, 300, 130, NULL);
	//m_pPropGridCtrlAlign.SetVSDotNetLook();	
	m_pPropGridCtrlAlign.EnableHeaderCtrl(FALSE);	
	m_pPropGridCtrlAlign.EnableDescriptionArea(FALSE);		
	m_pPropGridCtrlAlign.SetDescriptionRows(0);
	m_pPropGridCtrlAlign.MarkModifiedProperties();
	m_pPropGridCtrlAlign.SetCustomColors(COLOR_GRAY1, COLOR_GRAY1, COLOR_GRAY1, COLOR_GREEN2, COLOR_GRAY1, COLOR_GRAY1, COLOR_GREEN1);

	int nDesc = m_pPropGridCtrlAlign.GetDescriptionRows();

	
	//CMFCPropertyGridProperty* pAutoAlignGroup = new CMFCPropertyGridProperty(STRING_AUTOALIGN);		
	
	m_pAlignFlag = new CMFCPropertyGridProperty(STRING_AUTO_MANUAL_ALIGN, L"");
	if (m_pAlignFlag)
	{
		_GetAutoAlignFlagData();
		m_pAlignFlag->AddOption(_T("TRUE"));
		m_pAlignFlag->AddOption(_T("FALSE"));		
		m_pAlignFlag->AllowEdit(TRUE);
		m_pAlignFlag->Enable(TRUE);		
	}

	//pAutoAlignGroup->AddSubItem(m_pAlignFlag);		
	m_pPropGridCtrlAlign.AddProperty(m_pAlignFlag);

	vector<double> vecAutoAlignPos = _GetAutoAlignPosData();
	int nVecCount = static_cast<int>(vecAutoAlignPos.size());
	//m_pAlignFlag->Expand(TRUE);

	VARIANT var;
	var.dblVal = 0.0;

	CString str = L"";	
	for (int i = 0; i < nVecCount; i+= 2)
	{
		str.Format(_T("%d"), (i / 2) + 1 );
		CString strTempX = L"AUTO ALIGN X" + str;		
		CString strTempY = L"AUTO ALIGN Y" + str;

		m_pAlignPos[i] = new CMFCPropertyGridProperty(strTempX, (_variant_t)var.dblVal, L"");
		m_pAlignPos[i+1] = new CMFCPropertyGridProperty(strTempY, (_variant_t)var.dblVal, L"");

		if (m_pAlignPos[i])
		{		
			m_pAlignPos[i]->AllowEdit(TRUE);
			m_pAlignPos[i]->Enable(TRUE);
			m_pAlignPos[i]->SetValue((_variant_t)vecAutoAlignPos[i]);
		}
		if (m_pAlignPos[i + 1])
		{
			m_pAlignPos[i+1]->AllowEdit(TRUE);
			m_pAlignPos[i+1]->Enable(TRUE);
			m_pAlignPos[i + 1]->SetValue((_variant_t)vecAutoAlignPos[i + 1]);
		}

		//pAutoAlignGroup->AddSubItem(m_pAlignPos[i]);
		//pAutoAlignGroup->AddSubItem(m_pAlignPos[i+1]);		
		m_pPropGridCtrlAlign.AddProperty(m_pAlignPos[i]);
		m_pPropGridCtrlAlign.AddProperty(m_pAlignPos[i+1]);
	}		
	
	//m_pPropGridCtrlAlign.AddProperty(pAutoAlignGroup);	
	
	//m_pPropGridCtrlAlign.AdjustLayout();

	return RESULT_GOOD;
}

CMFCPropertyGridProperty* CAlignMaskEditDlg::_GetAutoAlignFlagData()
{
	AutoAlign_t* pAutoAlignSpec = GetAutoAlignSpec();

	BOOL bCheck = pAutoAlignSpec->bAutoAlign;

	if (bCheck == TRUE)
	{
		m_pAlignFlag->SetValue((_variant_t)STRING_AUTO_MANUAL_TRUE);
	}
	else
	{
		m_pAlignFlag->SetValue((_variant_t)STRING_AUTO_MANUAL_FALSE);
	}
	
	return m_pAlignFlag;
}

vector<double> CAlignMaskEditDlg::_GetAutoAlignPosData()
{
	AutoAlign_t* pAutoAlignSpec = GetAutoAlignSpec();

	int nCount = static_cast<int>(pAutoAlignSpec->vecAutoAlign.size());		

	vector<double> vecTemp;	

	for (int i = 0; i < nCount; i++)
	{
		vecTemp.push_back(static_cast<double>(pAutoAlignSpec->vecAutoAlign[i]->dAutoAlignX));
		vecTemp.push_back(static_cast<double>(pAutoAlignSpec->vecAutoAlign[i]->dAutoAlignY));
	}
	return vecTemp;
}


BOOL CAlignMaskEditDlg::GetAutoAlignFlagDlg()
{
	CString str = m_pAlignFlag->GetValue().bstrVal;
	if (str == STRING_AUTO_MANUAL_TRUE) {
		return true;
	}
	else if (str == STRING_AUTO_MANUAL_FALSE) {
		return false;
	}
	return false;
}

void CAlignMaskEditDlg::SetAutoAlignFlagDlg(INT32 iData)
{
	AutoAlign_t* pAutoAlignSpec = GetAutoAlignSpec();

	switch (iData)
	{
	case 0:
		m_pAlignFlag->SetValue((_variant_t)STRING_AUTO_MANUAL_FALSE);
		pAutoAlignSpec->bAutoAlign = FALSE;
		break;
	case 1:
		m_pAlignFlag->SetValue((_variant_t)STRING_AUTO_MANUAL_TRUE);
		pAutoAlignSpec->bAutoAlign = TRUE;
		break;
	default:
		break;
	}
}

double CAlignMaskEditDlg::GetAutoAlignDataDlg(UINT32 index)
{
	return(m_pAlignPos[index]->GetValue().dblVal);
}

void CAlignMaskEditDlg::SetAutoAlignDataXDlg(UINT32 index, double strData)
{
	m_pAlignPos[index]->SetValue((_variant_t)strData);
	
	AutoAlign_t* pAutoAlignSpec = GetAutoAlignSpec();
	index = index / 2;
	pAutoAlignSpec->vecAutoAlign[index]->dAutoAlignX = strData;
}
void CAlignMaskEditDlg::SetAutoAlignDataYDlg(UINT32 index, double strData)
{
	m_pAlignPos[index]->SetValue((_variant_t)strData);

	AutoAlign_t* pAutoAlignSpec = GetAutoAlignSpec();	
	index = index / 2;
	pAutoAlignSpec->vecAutoAlign[index]->dAutoAlignY = strData;
}

void			CAlignMaskEditDlg::_UpdateDataToUi()
{
	CString strTemp;

	strTemp.Format(_T("%.3f"), m_fResizeWidth);
	GetDlgItem(IDC_EDIT_FACTOR_WIDTH)->SetWindowText(strTemp);

	strTemp.Format(_T("%.3f"), m_fResizeHeight);
	GetDlgItem(IDC_EDIT_FACTOR_HEIGHT)->SetWindowText(strTemp);
}

void			CAlignMaskEditDlg::_UpdateUIToData()
{
	CString strTemp;

	GetDlgItem(IDC_EDIT_FACTOR_WIDTH)->GetWindowText(strTemp);
	m_fResizeWidth = static_cast<float>(_ttof(strTemp));

	GetDlgItem(IDC_EDIT_FACTOR_HEIGHT)->GetWindowText(strTemp);
	m_fResizeHeight = static_cast<float>(_ttof(strTemp));
}

void			CAlignMaskEditDlg::_SetLayerName()
{//Combo Box에 선택된 Layer 이름 입력
	m_cbLayer.ResetContent();

	UserLayerSet_t* pUserLayerSet = GetUserLayerSet();
	if (pUserLayerSet == nullptr) return;

	SystemSpec_t* pSystemSpec = GetSystemSpec();
	if (pSystemSpec == nullptr) return;
	eMachineType eMcType = pSystemSpec->sysBasic.McType;
	eUserSite eUserSite = pSystemSpec->sysBasic.UserSite;

	int nCount = static_cast<int>(pUserLayerSet->vcLayerSet.size());
	for (int i = 0; i < nCount; i++)
	{
		//NSIS일 경우 Measure Layer만 편집
		if (eMcType == eMachineType::eNSIS)
		{
			if (pUserLayerSet->vcLayerSet[i]->strDefine != MEASURE_LAYER_TXT)
			{
				continue;
			}
		}
 		else 
		{
			if (pUserLayerSet->vcLayerSet[i]->strDefine != PATTERN_LAYER_TXT)
			{
				continue;
			}
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

CString CAlignMaskEditDlg::_GetLayerName()
{//Combo Box에서 선택된 Layer 이름 가져오기
	CString strLayer;
	m_cbLayer.GetWindowText(strLayer);

	return strLayer;
}

BOOL CAlignMaskEditDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) return TRUE;

	if (pMsg->wParam == 'D' || pMsg->wParam == VK_DELETE)
	{
		if (AfxMessageBox(_T("삭제 하시겠습니까?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			SetDelete_Apply_Mask(m_eMaskType);

			if (GetMachineType() == eMachineType::eNSIS)
			{
				Update_NSIS_MeasureData();
			}
		}
		
// 	if (pMsg->wParam == VK_UP || pMsg->wParam == VK_DOWN || pMsg->wParam == VK_LEFT || pMsg->wParam == VK_RIGHT)
// 	{
// 		return TRUE;
// 	}
// 
		return RESULT_GOOD;
	}


	return CDialogEx::PreTranslateMessage(pMsg);
}

void CAlignMaskEditDlg::OnMaskTypeClicked(UINT32 nID)
{
int nIndex = nID - IDC_RADIO_TYPE_ALIGN;

m_eMaskType = static_cast<MaskType>(nIndex + 1);

	SetMaskSelectMode(m_eMaskType);
}

void CAlignMaskEditDlg::OnBnClickedButtonApply()
{
	SetEditApply_Mask(m_eMaskType);
}

void			CAlignMaskEditDlg::_ClearFeatureData()
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

UINT32			CAlignMaskEditDlg::_MakeSquareFeature_ViewCenter(vector<CFeature*> &vecFeatureData)
{
	_ClearFeatureData();

	vecFeatureData.resize(1);

	//화면의 Center 좌표 생성
	RECTD drtView_mm = GetViewRect();
	if (drtView_mm.IsRectNull() == TRUE) return RESULT_BAD;
	drtView_mm.NormalizeRectD();
	//

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	double dCenterX, dCenterY;
	double dWidth, dHeight;

	dCenterX = drtView_mm.CenterX();
	dCenterY = drtView_mm.CenterY();
	dWidth = drtView_mm.width() * 0.3;
	dHeight = drtView_mm.height() * 0.3;

	pFeatureP->m_dX = dCenterX;
	pFeatureP->m_dY = dCenterY;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = true;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::s;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;
	
	// 2022.06.29
	// 김준호 주석추가
	// View Center 일 경우 pSymbol->m_vcParams 가 1개만 있어서 odb 저장할때 문제가 생겨 추가
	// Origin Code
	//pSymbol->m_vcParams.resize(DefSymbol::pParaCount[static_cast<int>(SymbolName::s)]);
	//pSymbol->m_vcParams[0] = dWidth;
	// New Code
	pSymbol->m_vcParams.resize(2);
	pSymbol->m_vcParams[0] = dWidth;
	pSymbol->m_vcParams[1] = dHeight;

	CString strSymbol = CDrawFunction::GetSymbolName(pSymbol->m_eSymbolName, pSymbol->m_vcParams);
	pSymbol->m_strUserSymbol = strSymbol;

	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	return RESULT_GOOD;
}

UINT32			CAlignMaskEditDlg::_MakeRectFeature_ViewCenter(vector<CFeature*> &vecFeatureData)
{
	_ClearFeatureData();

	vecFeatureData.resize(1);
	
	//화면의 Center 좌표 생성
	RECTD drtView_mm = GetViewRect();
	if (drtView_mm.IsRectNull() == TRUE) return RESULT_BAD;
	drtView_mm.NormalizeRectD();
	//

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	double dCenterX, dCenterY;
	double dWidth, dHeight;

	dCenterX = drtView_mm.CenterX();
	dCenterY = drtView_mm.CenterY();
	dWidth = drtView_mm.width() * 0.3;
	dHeight = drtView_mm.height() * 0.3;

	pFeatureP->m_dX = dCenterX;
	pFeatureP->m_dY = dCenterY;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = true;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::rect;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;
	pSymbol->m_vcParams.resize(2);
	pSymbol->m_vcParams[0] = dWidth;
	pSymbol->m_vcParams[1] = dHeight;

	CString strSymbol = CDrawFunction::GetSymbolName(pSymbol->m_eSymbolName, pSymbol->m_vcParams);
	pSymbol->m_strUserSymbol = strSymbol;

	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;
	
	return RESULT_GOOD;
}

UINT32			CAlignMaskEditDlg::_MakeSquareFeature_SelectInfo(IN SELECT_INFO_UNIT &stSelectInfo, vector<CFeature*> &vecFeatureData)
{
	_ClearFeatureData();
	if (stSelectInfo.dWidth == 0. ||
		stSelectInfo.dHeight == 0.)
	{
		return RESULT_BAD;
	}

	vecFeatureData.resize(1);
			
	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	double dCenterX, dCenterY;
	double dWidth, dHeight;

	dCenterX = stSelectInfo.dCx;
	dCenterY = stSelectInfo.dCy;
	dWidth = stSelectInfo.dWidth;
 	dHeight = stSelectInfo.dHeight;

	double dMaxSize = MAX(dWidth, dHeight);

	pFeatureP->m_dX = dCenterX;
	pFeatureP->m_dY = dCenterY;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = true;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::s;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;
	pSymbol->m_vcParams.resize(2);
	pSymbol->m_vcParams[0] = dMaxSize;
	pSymbol->m_vcParams[1] = dMaxSize;

	CString strSymbol = CDrawFunction::GetSymbolName(pSymbol->m_eSymbolName, pSymbol->m_vcParams);
	pSymbol->m_strUserSymbol = strSymbol;

	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	return RESULT_GOOD;
}

void CAlignMaskEditDlg::OnBnClickedButtonClose()
{
	UINT32 nEditMode = EditMode::enumMode_None;
	//UINT32 nEditMode = EditMode::enumMode_EditMode;

	SetViewerEditMode(nEditMode);

	ShowWindow(SW_HIDE);

	
}


void CAlignMaskEditDlg::OnBnClickedButtonAutoSearch()
{
	SetAutoAlignCross();
	DrawCamData(FALSE);
}


void CAlignMaskEditDlg::OnBnClickedButtonTestClear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CAlignMaskEditDlg::OnBnClickedButtonAddAlignMask()
{//Click Add Button
	if (GetMachineType() == eMachineType::eNSIS)
	{
		_MakeSquareFeature_ViewCenter(m_vecFeatureData);
	}
	else
	{
		_MakeRectFeature_ViewCenter(m_vecFeatureData);
	}
	
	CString strLayerName = _GetLayerName();
	SetAdd_Apply_Mask(m_eMaskType, strLayerName, m_vecFeatureData);

}

//////////////////////////////////////////////////////////////////////////
//Resize
void CAlignMaskEditDlg::OnBnClickedButtonResize()
{
	BOOL bRet = TRUE;
	CString strTemp;
	double dWidth, dHeight;

	GetDlgItem(IDC_EDIT_FACTOR_WIDTH)->GetWindowText(strTemp);
	dWidth = _ttof(strTemp);

	if (dWidth <= 0.)
	{
	bRet = FALSE;
	}

	GetDlgItem(IDC_EDIT_FACTOR_HEIGHT)->GetWindowText(strTemp);
	dHeight = _ttof(strTemp);

	if (dHeight <= 0.)
	{
		bRet = FALSE;
	}

	if (bRet == FALSE)
	{
		AfxMessageBox(_T("입력 값이 잘못되었습니다."));
		return;//예외처리
	}

	//
	SetResize_Manual_Mask(m_eMaskType, DoubleToFloat(dWidth), DoubleToFloat(dHeight));
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//Move Axis
void CAlignMaskEditDlg::OnBnClickedRadioMoveXy()
{
	SetMoveAxis(Edit_MoveAxis::enumMoveAxis_XY);
}


void CAlignMaskEditDlg::OnBnClickedRadioMoveX()
{
	SetMoveAxis(Edit_MoveAxis::enumMoveAxis_X);
}


void CAlignMaskEditDlg::OnBnClickedRadioMoveY()
{
	SetMoveAxis(Edit_MoveAxis::enumMoveAxis_Y);
}
//////////////////////////////////////////////////////////////////////////




void CAlignMaskEditDlg::OnBnClickedButtonDelete()
{
	SetDelete_Apply_Mask(m_eMaskType);

	_InitData();

	if (GetMachineType() == eMachineType::eNSIS)
	{
		Update_NSIS_MeasureData();
	}
}

void CAlignMaskEditDlg::OnBnClickedCheckAddAlignMaskFeatureBase()
{
	BOOL bCheck = ((CButton *)GetDlgItem(IDC_CHECK_ADD_ALIGN_MASK_FEATURE_BASE))->GetCheck();

	if (bCheck == TRUE)
	{
		_ClearFeatureData();

		UINT32 nEditMode = EditMode::enumMode_EditMask_Add_FeatureBase;

		SetPointMode(TRUE, 0, _T(""));
		SetViewerEditMode(nEditMode);
	}
	else
	{
		UINT32 nEditMode = EditMode::enumMode_EditMask;

		SetViewerEditMode(nEditMode);

		CString strLayerName = _GetLayerName();
		SetAdd_Apply_Mask(m_eMaskType, strLayerName, m_vecFeatureData);

		if (GetMachineType() == eMachineType::eNSIS)
		{
			Update_NSIS_MeasureData();
		}

		_ClearFeatureData();
	}


}


void CAlignMaskEditDlg::OnBnClickedButtonAutoFindAlign()
{
	_SetLayerName();

	UINT32 nRet = SetAutoFindAlign();

	if( nRet == RESULT_GOOD)
	{
		AfxMessageBox(_T("자동 ALIGN 완료"));
	}
}

void CAlignMaskEditDlg::OnBnClickedButtonSaveAlignSpec()
{
	BOOL bAutoFlag = FALSE;
	bAutoFlag = GetAutoAlignFlagDlg();
	SetAutoAlignFlagDlg(bAutoFlag);

	double tmp = 0.f;
	for (int i = 0; i < 8; i++)
	{
		tmp = GetAutoAlignDataDlg(i);
		if (i % 2 == 0)
		{
			SetAutoAlignDataXDlg(i, tmp);
		}
		else
		{
			SetAutoAlignDataYDlg(i, tmp);
		}
	}	
	SaveSystemSpec();	

	AfxMessageBox(_T("ALIGN SPEC 저장 완료"));
}


void CAlignMaskEditDlg::OnBnClickedButtonLocationLeftTop()
{
	CString strTemp = _T("");
	UserSetInfo_t *pUserSetInfo = GetUserSetInfo();
	if (pUserSetInfo->dAngle == 0.) 
		strTemp = _T("LT");
	else if (pUserSetInfo->dAngle == 90.)
		strTemp = _T("RT");
	else if (pUserSetInfo->dAngle == 180.)
		strTemp = _T("RB");
	else if (pUserSetInfo->dAngle == 270.)
		strTemp = _T("LB");
	//CString strTemp = _T("LT");
	SetViewerLocation(strTemp);
}
void CAlignMaskEditDlg::OnBnClickedButtonLocationLeftBottom()
{
	CString strTemp = _T("");
	UserSetInfo_t *pUserSetInfo = GetUserSetInfo();
	if (pUserSetInfo->dAngle == 0.)
		strTemp = _T("LB");
	else if (pUserSetInfo->dAngle == 90.)
		strTemp = _T("LT");
	else if (pUserSetInfo->dAngle == 180.)
		strTemp = _T("RT");
	else if (pUserSetInfo->dAngle == 270.)
		strTemp = _T("RB");
	//CString strTemp = _T("LB");
	SetViewerLocation(strTemp);
}
void CAlignMaskEditDlg::OnBnClickedButtonLocationRightTop()
{
	CString strTemp = _T("");
	UserSetInfo_t *pUserSetInfo = GetUserSetInfo();
	if (pUserSetInfo->dAngle == 0.)
		strTemp = _T("RT");
	else if (pUserSetInfo->dAngle == 90.)
		strTemp = _T("RB");
	else if (pUserSetInfo->dAngle == 180.)
		strTemp = _T("LB");
	else if (pUserSetInfo->dAngle == 270.)
		strTemp = _T("LT");
	//CString strTemp = _T("RT");
	SetViewerLocation(strTemp);
}
void CAlignMaskEditDlg::OnBnClickedButtonLocationRightBottom()
{
	CString strTemp = _T("");
	UserSetInfo_t *pUserSetInfo = GetUserSetInfo();
	if (pUserSetInfo->dAngle == 0.)
		strTemp = _T("RB");
	else if (pUserSetInfo->dAngle == 90.)
		strTemp = _T("LB");
	else if (pUserSetInfo->dAngle == 180.)
		strTemp = _T("LT");
	else if (pUserSetInfo->dAngle == 270.)
		strTemp = _T("RT");
	//CString strTemp = _T("RB");
	SetViewerLocation(strTemp);
}

UINT32 CAlignMaskEditDlg::ChangeButtonUI()
{
	CRect rectTemp1, rectTemp2, rectTemp3, rectTemp4;
	((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_1))->GetWindowRect(&rectTemp1);
	((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_2))->GetWindowRect(&rectTemp2);
	((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_3))->GetWindowRect(&rectTemp3);
	((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_4))->GetWindowRect(&rectTemp4);
	ScreenToClient(&rectTemp1);
	ScreenToClient(&rectTemp2);
	ScreenToClient(&rectTemp3);
	ScreenToClient(&rectTemp4);

	UserSetInfo_t *pUserSetInfo = GetUserSetInfo();
	if (pUserSetInfo->dAngle == 0. || pUserSetInfo->dAngle == 180.)
	{	
		if (m_iKind == 0) return RESULT_GOOD;
		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_1))->MoveWindow(rectTemp4);
		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_2))->MoveWindow(rectTemp3);
		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_3))->MoveWindow(rectTemp2);
		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_4))->MoveWindow(rectTemp1);

		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_1))->SetWindowTextW(_T("LEFT_TOP"));
		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_2))->SetWindowTextW(_T("LEFT_BOTTOM"));
		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_3))->SetWindowTextW(_T("RIGHT_TOP"));
		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_4))->SetWindowTextW(_T("RIGHT_BOTTOM"));		
		
		m_iKind = 0;
	}		
	else if (pUserSetInfo->dAngle == 90. || pUserSetInfo->dAngle == 270.)
	{		
		if (m_iKind == 1) return RESULT_GOOD;
		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_1))->MoveWindow(rectTemp4);
		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_2))->MoveWindow(rectTemp3);
		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_3))->MoveWindow(rectTemp2);
		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_4))->MoveWindow(rectTemp1);

		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_1))->SetWindowTextW(_T("RIGHT_BOTTOM"));
		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_2))->SetWindowTextW(_T("RIGHT_TOP"));
		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_3))->SetWindowTextW(_T("LEFT_BOTTOM"));
		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_4))->SetWindowTextW(_T("LEFT_TOP"));

		m_iKind = 1;
	}		
	/*else if (pUserSetInfo->dAngle == 180.)
	{
		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_1))->SetWindowTextW(_T("LEFT_TOP"));
		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_2))->SetWindowTextW(_T("LEFT_BOTTOM"));
		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_3))->SetWindowTextW(_T("RIGHT_TOP"));
		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_4))->SetWindowTextW(_T("RIGHT_BOTTOM"));
	}		
	else if (pUserSetInfo->dAngle == 270.)
	{
		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_1))->SetWindowTextW(_T("RIGHT_BOTTOM"));
		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_2))->SetWindowTextW(_T("RIGHT_TOP"));
		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_3))->SetWindowTextW(_T("LEFT_BOTTOM"));
		((CButton*)GetDlgItem(IDC_BUTTON_ODB_LOCATION_4))->SetWindowTextW(_T("LEFT_TOP"));
	}	*/	
	return RESULT_GOOD;
}