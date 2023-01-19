#include "pch.h"
#include "iMaster.h"
#include "LayerInfoFormView.h"
#include "MainFrm.h"
#include <vector>

using namespace std;

#define IDC_LISTCTRL_LAYERS   990
#define IDC_STATICTEXT        991
#define IDC_STATICCHECK       992
#define IDC_STATICCHECK       992
#define IDC_STATICCOMBO       993

#define NUMBER_OF_DEFAULT_COLORS 18
typedef struct {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char alpha;
}layer_color;

 layer_color defaultColors[NUMBER_OF_DEFAULT_COLORS] = {
	{ 115,115,222,177 },
	{ 255,127,115,177 },
	{ 193,0,224,177 },
	{ 117,242,103,177 },
	{ 0,195,195,177 },
	{ 213,253,51,177 },
	{ 209,27,104,177 },
	{ 255,197,51,177 },
	{ 186,186,186,177 },
	{ 211,211,255,177 },
	{ 253,210,206,177 },
	{ 236,194,242,177 },
	{ 208,249,204,177 },
	{ 183,255,255,177 },
	{ 241,255,183,177 },
	{ 255,202,225,177 },
	{ 253,238,197,177 },
	{ 226,226,226,177 }
};

#define DEFCOLOR_R(x)   defaultColors[x % NUMBER_OF_DEFAULT_COLORS].red*257
#define DEFCOLOR_G(x)   defaultColors[x % NUMBER_OF_DEFAULT_COLORS].green*257
#define DEFCOLOR_B(x)   defaultColors[x % NUMBER_OF_DEFAULT_COLORS].blue*257

 //Side 제외

 CLayerInfoFormView::CLayerInfoFormView()
{	
	 m_pSysSpec = nullptr;
	 m_strPatternLayer = L"";
	 m_strDrillLayer = L"";
	 m_strViaLayer = L"";
	 m_strMeasureLayer = L"";
	 m_strThicknessLayer = L"";

	 m_nStartLayerIdx = -1;
	 m_nEndLayerIdx = -1;
}


 CLayerInfoFormView::~CLayerInfoFormView()
{
	 ClearHistogram();
	 ClearPTWidthHisto();
}

BEGIN_MESSAGE_MAP(CLayerInfoFormView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_NOTIFY(NM_CLICK, IDC_LISTCTRL_LAYERS, OnItemClick)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_LISTCTRL_LAYERS, OnItemChanging)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTCTRL_LAYERS, OnItemChangedList)
	ON_REGISTERED_MESSAGE(WM_LISTCTRL_COMBOBOX_CHANGE, OnComboBoxChange)
	ON_REGISTERED_MESSAGE(WM_LISTCTRL_LAYERCOLOR_CHANGE, OnLayerColorChanged)
	ON_REGISTERED_MESSAGE(WM_LISTCTRL_ITEM_RCLICK, OnLayerItemRClick)
	ON_COMMAND(ID_HISTOGRAM_FEATURE, &CLayerInfoFormView::OnPopMenu_FeatureHisto)
	ON_COMMAND(ID_HISTOGRAM_ATTRIBUTE, &CLayerInfoFormView::OnPopMenu_AttributeHisto)
	ON_COMMAND(ID_HISTOGRAM_SYMBOL, &CLayerInfoFormView::OnPopMenu_SymbolHisto)
	ON_CBN_SELCHANGE(IDC_STATICCOMBO, OnCbnSelChangeInspLayerComboBox)
//	ON_WM_RBUTTONDOWN()
//	ON_WM_RBUTTONUP()
//	ON_WM_MOUSEACTIVATE()
ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


int CLayerInfoFormView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create List:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_EDITLABELS | LVTVIF_AUTOSIZE | WS_VSCROLL;

	if (!m_stInspLayer.Create(_T("Please Select Inspection Layer : "), WS_CHILD | WS_VISIBLE | SS_LEFT, rectDummy, this))
	{
		TRACE0("Failed to create List control \n");
		return -1;      // fail to create
	}
	m_stInspLayer.ShowWindow(FALSE);

	if (!m_cbInspLayer.Create(CBS_DROPDOWN | WS_CHILD | WS_VSCROLL | WS_TABSTOP | WS_VISIBLE, rectDummy, this, IDC_STATICCOMBO))
	{
		TRACE0("Failed to create List control \n");
		return -1;      // fail to create
	}
	m_cbInspLayer.ShowWindow(FALSE);

	if (!m_LayerList.Create(dwViewStyle, rectDummy, this, IDC_LISTCTRL_LAYERS))
	{
		TRACE0("Failed to create List control \n");
		return -1;      // fail to create
	}	

	if (!m_StepList.Create(dwViewStyle, rectDummy, this, IDC_LISTCTRL_LAYERS))
	{
		TRACE0("Failed to create List control \n");
		return -1;      // fail to create
	}
	m_StepHeaderCtrl.SubclassWindow(m_StepList.GetHeaderCtrl()->m_hWnd);
	m_LayerHeaderCtrl.SubclassWindow(m_LayerList.GetHeaderCtrl()->m_hWnd);

	m_LayerList.SetExtendedStyle(m_LayerList.GetStyle() | LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_BORDERSELECT | LVS_OWNERDRAWFIXED | LVS_EX_TWOCLICKACTIVATE | LVS_EX_DOUBLEBUFFER);
	m_LayerList.InsertColumn(LIST_VIEW_COLUMN, L"", LVCFMT_CENTER, 25);
	m_LayerList.InsertColumn(LIST_LAYER_COLUMN, L"Layer Name", LVCFMT_CENTER, 260);
//	m_LayerList.InsertColumn(LIST_SIDE_COLUMN, L"F/B", LVCFMT_CENTER, 75);
	m_LayerList.InsertColumn(LIST_DEFINE_COLUMN, L"Define", LVCFMT_CENTER, 250);
	m_LayerList.InsertColumn(LIST_COLOR_COLUMN, L"Color", LVCFMT_CENTER, 75);
// 	m_LayerList.InsertColumn(LIST_POLARITY_COLUMN, L"Polarity", LVCFMT_CENTER, 75);
// 	m_LayerList.InsertColumn(LIST_SCALEX_COLUMN, L"ScaleX", LVCFMT_CENTER, 75);
// 	m_LayerList.InsertColumn(LIST_SCALEY_COLUMN, L"ScaleY", LVCFMT_CENTER, 75);
	m_LayerList.SetReadOnlyColumns(LIST_VIEW_COLUMN);
	m_LayerList.SetReadOnlyColumns(LIST_LAYER_COLUMN);
	m_LayerList.SetReadOnlyColumns(LIST_COLOR_COLUMN);
//	m_LayerList.SetComboColumns(LIST_SIDE_COLUMN);
	m_LayerList.SetComboColumns(LIST_DEFINE_COLUMN);
	//m_LayerList.SetComboColumns(LIST_POLARITY_COLUMN);
	m_LayerList.SetColorColumn(LIST_COLOR_COLUMN);
	m_LayerList.SetDefaultTextColor(Foreground_Color);

	m_StepList.SetExtendedStyle(m_StepList.GetStyle() | LVS_EX_FULLROWSELECT | LVS_EX_BORDERSELECT | LVS_OWNERDRAWFIXED | LVS_EX_TWOCLICKACTIVATE | LVS_EX_DOUBLEBUFFER);
	m_StepList.SetReadOnlyColumns(LIST_STEP_NAME_COL);
	m_StepList.InsertColumn(LIST_STEP_NAME_COL, L"Step Name", LVCFMT_CENTER, 212);
	m_StepList.InsertColumn(LIST_STEP_DEFINE_COL, L"Define", LVCFMT_CENTER, 187);
	m_StepList.SetComboColumns(LIST_STEP_DEFINE_COL);
	m_StepList.SetDefaultTextColor(Foreground_Color);

	InitializeList();

	CImageList imagelist;
	imagelist.Create(22, 22, ILC_COLOR4, 10, 10);
	m_StepList.SetImageList(&imagelist, LVSIL_SMALL);
	m_LayerList.SetImageList(&imagelist, LVSIL_SMALL);

	return 0;
}

void CLayerInfoFormView::ClearList()
{
	if (m_nStepListCount > 0)
		m_StepList.DeleteAllItems();
	m_nStepListCount = 0;

	if (m_nLayerListCount > 0)
		m_LayerList.DeleteAllItems();
	m_nLayerListCount = 0;

	if (m_cbInspLayer.m_hWnd)
		m_cbInspLayer.ResetContent();
}

void CLayerInfoFormView::InitializeList()
{
	ClearList();

	//Step Name
	m_strStepArr.RemoveAll();
	m_strStepArr.Add(UNIT_STRING);
	m_strStepArr.Add(STRIP_STRING);
	m_strStepArr.Add(PANEL_STRING);
	m_strStepArr.Add(NULL_STRING);

	//Layer Name
	m_strLayerArr.RemoveAll();
	m_strLayerArr.Add(PATTERN_LAYER_TXT);
	m_strLayerArr.Add(SR_LAYER_TXT);
	m_strLayerArr.Add(DOUBLE_SR_LAYER_TXT);
	m_strLayerArr.Add(VIA_LAYER_TXT);
	m_strLayerArr.Add(VIA2_LAYER_TXT);
	m_strLayerArr.Add(DRILL_LAYER_TXT);
	m_strLayerArr.Add(DRILL2_LAYER_TXT);
	m_strLayerArr.Add(MASK_LAYER_TXT);
	m_strLayerArr.Add(UNIT_OUT_LAYER_TXT);
	m_strLayerArr.Add(INSIDE_PATTERN_LAYER_TXT);
	m_strLayerArr.Add(MEASURE_LAYER_TXT);
	m_strLayerArr.Add(SR_DAM_LAYER_TXT);
	m_strLayerArr.Add(AU_LAYER_TXT);
	m_strLayerArr.Add(ETCH_LAYER_TXT);
	m_strLayerArr.Add(USER_LAYER1_TXT);
	m_strLayerArr.Add(USER_LAYER2_TXT);
	m_strLayerArr.Add(USER_LAYER3_TXT);
	m_strLayerArr.Add(PROFILE_LAYER_TXT);
	m_strLayerArr.Add(THICKNESS_LAYER_TXT);
	m_strLayerArr.Add(NULL_STRING);

	//Side
//	m_strSideArr.RemoveAll();
//	m_strSideArr.Add(FRONT_SIDE_TEXT);
//	m_strSideArr.Add(BACK_SIDE_TEXT);
//	m_strSideArr.Add(ALL_SIDE_TEXT);
//	m_strSideArr.Add(NULL_STRING);

	//Polarity
// 	m_strPolarityArr.RemoveAll();
// 	m_strPolarityArr.Add(POSITIVE_TEXT);
// 	m_strPolarityArr.Add(NEGATIVE_TEXT);

	m_StepList.SetComboBoxList(LIST_STEP_DEFINE_COL, m_strStepArr);
//	m_LayerList.SetComboBoxList(LIST_SIDE_COLUMN, m_strSideArr);
	m_LayerList.SetComboBoxList(LIST_DEFINE_COLUMN, m_strLayerArr);
	//m_LayerList.SetComboBoxList(LIST_POLARITY_COLUMN, m_strPolarityArr);

	m_pSysSpec = nullptr;
	m_bIsStepSet = FALSE;
	m_bIsDone = TRUE;
}

void CLayerInfoFormView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

// BOOL CLayerInfoFormView::OnEraseBkgnd(CDC* pDC)
// {
// 	return FALSE;
// }

void CLayerInfoFormView::OnPaint()
{
	CPaintDC dc(this);
}

void CLayerInfoFormView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL) return;

	CRect rectClient;
	GetClientRect(&rectClient);
	
	int nStepListHeight = (m_nStepListCount + 1) * STEP_LIST_HEIGHT;
	if (nStepListHeight == STEP_LIST_HEIGHT)
		nStepListHeight = rectClient.Height() / 6;
	else if (nStepListHeight >= rectClient.Height() / 2)
		nStepListHeight = rectClient.Height() / 4;

	int layerListTop = nStepListHeight - 1;
	m_StepList.SetWindowPos(NULL, rectClient.left, 0, rectClient.Width(), nStepListHeight, SWP_NOACTIVATE | SWP_NOZORDER);
		
	if (m_pSysSpec)
	{
		if (m_pSysSpec->sysBasic.McType == eMachineType::eNSIS)
			m_LayerList.SetWindowPos(NULL, rectClient.left, layerListTop, rectClient.Width(), rectClient.Height() - layerListTop, SWP_NOACTIVATE | SWP_NOZORDER);
		else
		{
			m_stInspLayer.SetWindowPos(NULL, rectClient.left, layerListTop, STEP_LIST_HEIGHT * 10, STEP_LIST_HEIGHT, SWP_NOACTIVATE | SWP_NOZORDER);
			m_cbInspLayer.SetWindowPos(NULL, rectClient.left + STEP_LIST_HEIGHT * 10, layerListTop, rectClient.Width() - (rectClient.left + STEP_LIST_HEIGHT * 10), STEP_LIST_HEIGHT, SWP_NOACTIVATE | SWP_NOZORDER);
			m_LayerList.SetWindowPos(NULL, rectClient.left, layerListTop + STEP_LIST_HEIGHT - 1, rectClient.Width(), rectClient.Height() - layerListTop - STEP_LIST_HEIGHT, SWP_NOACTIVATE | SWP_NOZORDER);

			m_stInspLayer.ShowWindow(TRUE);
			m_cbInspLayer.ShowWindow(TRUE);
		}
	}
	else
		m_LayerList.SetWindowPos(NULL, rectClient.left, layerListTop, rectClient.Width(), rectClient.Height() - layerListTop, SWP_NOACTIVATE | SWP_NOZORDER);
}

// UINT32 CLayerInfoFormView::DisplayLayerInfo(SystemSpec_t* pSysSpec, CString strJobPath, 
// 	std::vector<stStepInfo*>* vcStepInfo, std::vector<stLayerInfo*>* vcLayerInfo, UserLayerSet_t* pUserLayerSet,
// 	eProdSide prodSide, std::vector<stLayerPair*> *m_vcLayerList, INT32 iFrontlayerIdx, INT32 iBacklayerIdx)
 UINT32 CLayerInfoFormView::DisplayLayerInfo(SystemSpec_t* pSysSpec,
	 CString strJobPath, 
	 vector<stStepInfo*>* vcStepInfo, 
	 vector<stLayerInfo*>* vcLayerInfo,
	 UserLayerSet_t* pUserLayerSet,
	 UserSetInfo_t* pUserSetInfo, 
	 eProdSide prodSide, 	
	 vector<vector<stLayerInfo*>> pPatternLayerInfo,
	 vector<vector<stLayerInfo*>> pSRLayerInfo,
	 IN const CString& strFrontLayer,
	 IN const CString& strBackLayer)

{
	m_nStartLayerIdx = -1;
	m_nEndLayerIdx = -1;

	if (GetSafeHwnd() == NULL) 
		return RESULT_BAD;

	m_bIsStepSet = FALSE;

	ClearList();

	if (pSysSpec == nullptr || vcStepInfo == nullptr || 
		vcLayerInfo == nullptr || pUserSetInfo == nullptr)
		return RESULT_BAD;

	m_pSysSpec = pSysSpec;
	m_pstUserLayerSet = pUserLayerSet;
	m_pstUserSetInfo = pUserSetInfo;
	m_pPatternLayerInfo = pPatternLayerInfo;
	m_pSRLayerInfo = pSRLayerInfo;
	

	UINT32 iStepNum = static_cast<UINT32>(vcStepInfo->size());
	UINT32 iLayerNum = static_cast<UINT32>(vcLayerInfo->size());

	if (iStepNum <= 0 || iLayerNum <= 0)
		return RESULT_GOOD;

	m_bIsDone = FALSE;

	INT32 iFrontlayerIdx, iBacklayerIdx;
	iFrontlayerIdx = _ttoi(strFrontLayer);
	iBacklayerIdx = _ttoi(strBackLayer);

	if (m_pSysSpec->sysBasic.McType == eMachineType::eNSIS && 
		GetUserSite() != eUserSite::eSEM )
	{
		if (DisplayStep_for_NSIS(strJobPath, vcStepInfo) != RESULT_GOOD)
			return RESULT_BAD;

		//if (DisplayLayer_for_NSIS(strJobPath, vcStepInfo, vcLayerInfo, prodSide, m_vcLayerList, iFrontlayerIdx, iBacklayerIdx) != RESULT_GOOD)		
		//if (DisplayLayer_for_NSIS_SEM_V2(strJobPath, vcStepInfo, vcLayerInfo, prodSide, m_pPatternLayerInfo, strFrontLayer, strBackLayer) != RESULT_GOOD)
			return RESULT_BAD;
	}
	else if (m_pSysSpec->sysBasic.McType == eMachineType::eNSIS &&
		GetUserSite() == eUserSite::eSEM)
	{
		if (DisplayStep_for_NSIS(strJobPath, vcStepInfo) != RESULT_GOOD)
			return RESULT_BAD;
		
		//if (DisplayLayer_for_NSIS_SEM(strJobPath, vcStepInfo, vcLayerInfo, prodSide, m_vcLayerList, iFrontlayerIdx, iBacklayerIdx) != RESULT_GOOD)
		//if (DisplayLayer_for_NSIS_SEM(strJobPath, vcStepInfo, vcLayerInfo, prodSide, strFrontLayer, strBackLayer) != RESULT_GOOD)
		if (DisplayLayer_for_NSIS_SEM_V2(strJobPath, vcStepInfo, vcLayerInfo, prodSide,pUserSetInfo, strFrontLayer, strBackLayer) != RESULT_GOOD)
			return RESULT_BAD;		
	}
	else if (m_pSysSpec->sysBasic.McType == eMachineType::eAOI)
	{
		if (DisplayStep_for_AOI(strJobPath, vcStepInfo) != RESULT_GOOD)
			return RESULT_BAD;

		if (DisplayLayer_for_AOI(strJobPath, vcStepInfo, vcLayerInfo) != RESULT_GOOD)
			return RESULT_BAD;
	}
	else if (m_pSysSpec->sysBasic.McType == eMachineType::eAVI)
	{

		if (DisplayStep_for_AVI(strJobPath, vcStepInfo) != RESULT_GOOD)
			return RESULT_BAD;

		if (DisplayLayer_for_AVI(strJobPath, vcStepInfo, vcLayerInfo, iFrontlayerIdx, iBacklayerIdx) != RESULT_GOOD)
			return RESULT_BAD;
	}

	m_nStartLayerIdx = iFrontlayerIdx;
	m_nEndLayerIdx = iBacklayerIdx;

	m_bIsDone = TRUE;
	InitUserLayerInfo();
	InitHistogram();

	if (GetMachineType() == eMachineType::eNSIS)
		SetUserLayerInfo();

	AdjustLayout();

	return RESULT_GOOD;
}



void CLayerInfoFormView::OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = FALSE;

	if ((pNMListView->uNewState & LVIS_SELECTED))
	{
		*pResult = TRUE;
	}
}

void CLayerInfoFormView::OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;

	if (pNMListView->uOldState == 0 && pNMListView->uNewState == 0)
		return;    // No change

	// Old check box state
	BOOL bOldState = (BOOL)(((pNMListView->uOldState & LVIS_STATEIMAGEMASK) >> 12) - 1);
	if (bOldState < 0)  bOldState = 0;// On startup there's no previous state, so assign as false (unchecked)		 

	// New check box state
	BOOL bNewState = (BOOL)(((pNMListView->uNewState & LVIS_STATEIMAGEMASK) >> 12) - 1);
	if (bNewState < 0) bNewState = 0;// On non-checkbox notifications assume false		

	int nIndex = pNMListView->iItem;


	int nSize = static_cast<int>(m_pstUserLayerSet->vcLayerSet.size());
	if (nIndex < 0) return;
	if (nIndex >= nSize) return;

	if (GetMachineType() != eMachineType::eNSIS && m_bIsDone)
	{		
		m_pstUserLayerSet->vcLayerSet[nIndex]->bCheck = bNewState;
		if (GetFlag_LoadLayers())
			DrawCamData(FALSE);
	}
}

LRESULT CLayerInfoFormView::OnLayerColorChanged(WPARAM wparam, LPARAM lparam)
{	
	SetUserLayerInfo();
	return 0L;
}

//hj.kim
LRESULT CLayerInfoFormView::OnLayerItemRClick(WPARAM wparam, LPARAM lparam)
{
	int nIndex = static_cast<int>(wparam);

	int nItemCount = m_LayerList.GetItemCount();
	

	if (nIndex < 0) return 0L;
	if (nIndex >= nItemCount) return 0L;

	//
	CString strTemp;
	strTemp.Format(_T("Select Index = %d\n"), nIndex);
	TRACE(strTemp);

	//
	CPoint CurrentPosition;
	::GetCursorPos(&CurrentPosition);
	CMenu MenuTemp;
	CMenu *pContextMenu = NULL;
	MenuTemp.LoadMenu(IDR_POPUP_HISTOGRAM);
	pContextMenu = MenuTemp.GetSubMenu(0);
	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN, CurrentPosition.x, CurrentPosition.y, this);

	m_nSelectR_Item = nIndex;
	m_strSelectR_LayerName = m_LayerList.GetItemText(nIndex, LIST_LAYER_COLUMN);

	return 0L;
}

LRESULT CLayerInfoFormView::OnComboBoxChange(WPARAM wParam, LPARAM lParam)
{
	if (m_pSysSpec == nullptr)
		return 0L;

	CUSTOM_NMITEM *pNMI = (CUSTOM_NMITEM*)wParam;

	if (pNMI->hwndFrom == m_StepList)
	{
		BOOL bPanelSet = FALSE, bStripSet = FALSE, bUnitSet = FALSE;
		CString strName = L"", strDefine = L"";
		//= m_StepList.GetComboText(pNMI->iItem, LIST_STEP_NAME_COL);

		if (pNMI->iSubItem == LIST_STEP_DEFINE_COL)
			//m_StepList.SetItemTextColor(pNMI->iItem, LIST_STEP_DEFINE_COL, MAGENTA_COLOR);
			m_StepList.SetItemTextColor(pNMI->iItem, LIST_STEP_DEFINE_COL, PURPLE_COLOR);
		
		m_pstUserLayerSet->ClearStep();
		for (UINT32 iLoop = 0; iLoop < m_nStepListCount; iLoop++)
		{
			strDefine = m_StepList.GetItemText(iLoop, LIST_STEP_DEFINE_COL);

			if (strDefine.CompareNoCase(PANEL_STRING) == 0)
				m_pstUserLayerSet->nPanelStepIdx = iLoop;
			if (strDefine.CompareNoCase(STRIP_STRING) == 0)
				m_pstUserLayerSet->vcStripStepIdx.emplace_back(iLoop);
			if (strDefine.CompareNoCase(UNIT_STRING) == 0)
				m_pstUserLayerSet->vcUnitStepIdx.emplace_back(iLoop);
		}

		//Condition : Panel Step must Exist and either Strip or Unit Step Exists
		if (m_pstUserLayerSet->nPanelStepIdx >= 0 && 
			(m_pstUserLayerSet->vcStripStepIdx.size() >= 0 || m_pstUserLayerSet->vcUnitStepIdx.size() >= 0))
			m_bIsStepSet = TRUE;
		else
			m_bIsStepSet = FALSE;
	
		m_StepList.Invalidate();
	}
	else if (pNMI->hwndFrom == m_LayerList)
	{
		//ResetCamLoaded(FALSE);
		if (pNMI->iSubItem == LIST_DEFINE_COLUMN)	// Layer
		{
			// Define 정보에 따라 Layer Color 변경
			if (pNMI->cmbIndex < m_pSysSpec->sysLayer.vcLayer.size())
			{
				COLORREF color = m_pSysSpec->sysLayer.vcLayer[pNMI->cmbIndex]->layerColor;
				m_LayerList.SetItemColor(pNMI->iItem, color);
			}
			
			if (GetMachineType() == eMachineType::eNSIS && GetUserSite() == eUserSite::eSEM) 
			{
				// 2022.10.05 KJH ADD
				// SR Layer 로 변경 하기 위해 주석처리
				// Origin Code 
				if (pNMI->cmbIndex < m_pSysSpec->sysLayer.vcLayer.size() &&
					m_pSysSpec->sysLayer.vcLayer[pNMI->cmbIndex]->strDefine.CompareNoCase(
					_T("PATTERN")) == 0)
				{
					CString strPatternLayer = m_LayerList.GetItemText(pNMI->iItem, LIST_LAYER_COLUMN);
					
					SetStartLayer(strPatternLayer);					
					int nSize = static_cast<int>(m_pPatternLayerInfo.size());
					//for (auto it : m_pPatternLayerInfo)
					for( int i = 0; i < nSize; i++)
					{
						int nSize2 = static_cast<int>(m_pPatternLayerInfo[i].size());
						for (int j = 0; j < nSize2; j++)
						{
							CString strTemp = m_pPatternLayerInfo[i][j]->strName;
							CString strTempPair = m_pPatternLayerInfo[i][j]->strPairLayerName;
							if (strTemp.CompareNoCase(strPatternLayer) == 0)
							{
								SetEndLayer(strTempPair);
							}
						}
					}
					

					int nCurSelItem = pNMI->iItem;

					int nCount = m_LayerList.GetItemCount();
					//Pattern이 선택되었다면, 기존 Pattern으로 선택된 정보를 클리어 한다.
					for (int i = 0; i < nCount; i++)
					{  
						if (nCurSelItem == i ) continue;

						if (m_LayerList.GetItemText(i, LIST_DEFINE_COLUMN) == PATTERN_LAYER_TXT)
						{
							m_LayerList.SetComboBoxCurSel(i, LIST_DEFINE_COLUMN, static_cast<int>(m_strLayerArr.GetCount() - 1));
						}
						else if (m_LayerList.GetItemText(i, LIST_DEFINE_COLUMN) == MEASURE_LAYER_TXT)
						{
							m_LayerList.SetComboBoxCurSel(i, LIST_DEFINE_COLUMN, static_cast<int>(m_strLayerArr.GetCount() - 1));
						}
						else if (m_LayerList.GetItemText(i, LIST_DEFINE_COLUMN) == THICKNESS_LAYER_TXT)
						{
							m_LayerList.SetComboBoxCurSel(i, LIST_DEFINE_COLUMN, static_cast<int>(m_strLayerArr.GetCount() - 1));
						}
					}
					//

					//Pattern이 선택되었다면 자동으로 MP, TP 선택
					nCount = m_LayerList.GetItemCount();
					for (int i = 0; i < nCount; i++)
					{
						CString strName;
						//Select MP
						strName.Format(_T("%s%s"), strPatternLayer, _T("_MP"));
						if (m_LayerList.GetItemText(i, LIST_LAYER_COLUMN) == strName)
						{
							m_LayerList.SetComboBoxCurSel(i, LIST_DEFINE_COLUMN, 10);
						}
						//Select TP
						strName.Format(_T("%s%s"), strPatternLayer, _T("_TP"));
						if (m_LayerList.GetItemText(i, LIST_LAYER_COLUMN) == strName)
						{
							m_LayerList.SetComboBoxCurSel(i, LIST_DEFINE_COLUMN, 18);
						}

					}

				}
				else if (pNMI->cmbIndex == m_strLayerArr.GetCount() - 1)
				{
					BOOL bPattern = FALSE;
					//현재 선택되어 있는 Pattern Layer가 없다면
					int nCount = m_LayerList.GetItemCount();
					for (int i = 0; i < nCount; i++)
					{
						if (m_LayerList.GetItemText(i, LIST_DEFINE_COLUMN) == PATTERN_LAYER_TXT)
						{
							bPattern = TRUE;
							break;
						}
					}

					if (bPattern == FALSE)
					{
						//Pattern Layer가 없다면, MP, TP Layer 해제
						for (int i = 0; i < nCount; i++)
						{
							if (m_LayerList.GetItemText(i, LIST_DEFINE_COLUMN) == MEASURE_LAYER_TXT)
							{
								m_LayerList.SetComboBoxCurSel(i, LIST_DEFINE_COLUMN, static_cast<int>(m_strLayerArr.GetCount() - 1));
							}
							else if (m_LayerList.GetItemText(i, LIST_DEFINE_COLUMN) == THICKNESS_LAYER_TXT)
							{
								m_LayerList.SetComboBoxCurSel(i, LIST_DEFINE_COLUMN, static_cast<int>(m_strLayerArr.GetCount() - 1));
							}
						}
					}
				}
				/*		
				if (pNMI->cmbIndex < m_pSysSpec->sysLayer.vcLayer.size() &&
					m_pSysSpec->sysLayer.vcLayer[pNMI->cmbIndex]->strDefine.CompareNoCase(SR_LAYER_TXT) == 0)
				{
					CString strSRLayer = m_LayerList.GetItemText(pNMI->iItem, LIST_LAYER_COLUMN);

					SetStartLayer(strSRLayer);
					int nSize = static_cast<int>(m_pSRLayerInfo.size());
					//for (auto it : m_pPatternLayerInfo)
					for (int i = 0; i < nSize; i++)
					{
						int nSize2 = static_cast<int>(m_pSRLayerInfo[i].size());
						for (int j = 0; j < nSize2; j++)
						{
							CString strTemp = m_pSRLayerInfo[i][j]->strName;
							CString strTempPair = m_pSRLayerInfo[i][j]->strPairLayerName;
							if (strTemp.CompareNoCase(strSRLayer) == 0)
							{
								SetEndLayer(strTempPair);
							}
						}
					}


					int nCurSelItem = pNMI->iItem;

					int nCount = m_LayerList.GetItemCount();
					//Pattern이 선택되었다면, 기존 Pattern으로 선택된 정보를 클리어 한다.
					for (int i = 0; i < nCount; i++)
					{
						if (nCurSelItem == i) continue;

						if (m_LayerList.GetItemText(i, LIST_DEFINE_COLUMN) == SR_LAYER_TXT)
						{
							m_LayerList.SetComboBoxCurSel(i, LIST_DEFINE_COLUMN, static_cast<int>(m_strLayerArr.GetCount() - 1));
						}
						else if (m_LayerList.GetItemText(i, LIST_DEFINE_COLUMN) == MEASURE_LAYER_TXT)
						{
							m_LayerList.SetComboBoxCurSel(i, LIST_DEFINE_COLUMN, static_cast<int>(m_strLayerArr.GetCount() - 1));
						}
						else if (m_LayerList.GetItemText(i, LIST_DEFINE_COLUMN) == THICKNESS_LAYER_TXT)
						{
							m_LayerList.SetComboBoxCurSel(i, LIST_DEFINE_COLUMN, static_cast<int>(m_strLayerArr.GetCount() - 1));
						}
					}
					//

					//Pattern이 선택되었다면 자동으로 MP, TP 선택
					nCount = m_LayerList.GetItemCount();
					for (int i = 0; i < nCount; i++)
					{
						CString strName;
						//Select MP
						strName.Format(_T("%s%s"), strSRLayer, _T("_MP"));
						if (m_LayerList.GetItemText(i, LIST_LAYER_COLUMN) == strName)
						{
							m_LayerList.SetComboBoxCurSel(i, LIST_DEFINE_COLUMN, 10);
						}
						//Select TP
						strName.Format(_T("%s%s"), strSRLayer, _T("_TP"));
						if (m_LayerList.GetItemText(i, LIST_LAYER_COLUMN) == strName)
						{
							m_LayerList.SetComboBoxCurSel(i, LIST_DEFINE_COLUMN, 18);
						}

					}

				}
				else if (pNMI->cmbIndex == m_strLayerArr.GetCount() - 1)
				{
					BOOL bSR = FALSE;
					//현재 선택되어 있는 Pattern Layer가 없다면
					int nCount = m_LayerList.GetItemCount();
					for (int i = 0; i < nCount; i++)
					{
						if (m_LayerList.GetItemText(i, LIST_DEFINE_COLUMN) == SR_LAYER_TXT)
						{
							bSR = TRUE;
							break;
						}
					}

					if (bSR == FALSE)
					{
						//Pattern Layer가 없다면, MP, TP Layer 해제
						for (int i = 0; i < nCount; i++)
						{
							if (m_LayerList.GetItemText(i, LIST_DEFINE_COLUMN) == MEASURE_LAYER_TXT)
							{
								m_LayerList.SetComboBoxCurSel(i, LIST_DEFINE_COLUMN, static_cast<int>(m_strLayerArr.GetCount() - 1));
							}
							else if (m_LayerList.GetItemText(i, LIST_DEFINE_COLUMN) == THICKNESS_LAYER_TXT)
							{
								m_LayerList.SetComboBoxCurSel(i, LIST_DEFINE_COLUMN, static_cast<int>(m_strLayerArr.GetCount() - 1));
							}
						}
					}
				}*/
			}//NSIS - SEM End

			
			
//			if (color != m_LayerList.GetItemColor(pNMI->iItem))
			SetUserLayerInfo();

			m_LayerList.Invalidate();
		}		
	}


	return 0L;
}

UINT32 CLayerInfoFormView::SetUserLayerInfo()
{
	if (!m_bIsDone) return RESULT_BAD;
	if (m_pstUserLayerSet == nullptr) return RESULT_BAD;
	if (m_nLayerListCount <= 0) return RESULT_GOOD;
	
	if (!GetFlag_IsAllLayerLoaded())
		IsDoneLoadLayers(FALSE);

	m_strPatternLayer = L"";
	m_strSRLayer = L"";
	m_strDrillLayer = L"";
	m_strViaLayer = L"";
	m_strMeasureLayer = L"";
	m_strThicknessLayer = L"";
	m_LayerList.SetRedraw(FALSE);
	m_LayerList.EnableWindow(FALSE);
	//m_pstUserLayerSet->ClearLayer();

	m_pstUserLayerSet->iSelectedNum = 0;

	UINT32 iIndex = 0;
	CString strDefine = L"";	

	for (UINT32 iLoop = 0; iLoop < m_nLayerListCount; iLoop++)
	{
		LayerSet_t *pLayer = m_pstUserLayerSet->vcLayerSet[iLoop];
		if (pLayer == nullptr) continue;

		
		//pLayer->bPolarity = (m_LayerList.GetComboText(iLoop, LIST_POLARITY_COLUMN) == _T("+") ? true : false);
		pLayer->color = m_LayerList.GetItemColor(iLoop);
// 		pLayer->dScaleXY.x = _ttof(m_LayerList.GetItemText(iLoop, LIST_SCALEX_COLUMN));
// 		pLayer->dScaleXY.y = _ttof(m_LayerList.GetItemText(iLoop, LIST_SCALEY_COLUMN));
		pLayer->strLayer = m_LayerList.GetItemText(iLoop, LIST_LAYER_COLUMN);
		pLayer->strDefine = m_LayerList.GetItemText(iLoop, LIST_DEFINE_COLUMN);
		pLayer->index = iIndex;

		m_LayerList.SetCheck(iLoop, FALSE);

		pLayer->bCheck = m_LayerList.GetCheck(iLoop) ? true : false;

		if (pLayer->strDefine.GetLength() > 0)
		{
			pLayer->bCheck = true;
			m_LayerList.SetCheck(iLoop, TRUE);
			m_pstUserLayerSet->iSelectedNum++;
		}
		else
		{
			pLayer->bCheck = false;
		}
		

		if (pLayer->strDefine.CompareNoCase(PATTERN_LAYER_TXT) == 0)
			m_strPatternLayer = pLayer->strLayer;
		else if (pLayer->strDefine.CompareNoCase(SR_LAYER_TXT) == 0)
			m_strSRLayer = pLayer->strLayer;
		else if (pLayer->strDefine.CompareNoCase(DRILL_LAYER_TXT) == 0)
			m_strDrillLayer = pLayer->strLayer;
		else if (pLayer->strDefine.CompareNoCase(VIA_LAYER_TXT) == 0)
			m_strViaLayer = pLayer->strLayer;
		else if (pLayer->strDefine.CompareNoCase(MEASURE_LAYER_TXT) == 0)
			m_strMeasureLayer = pLayer->strLayer;
		else if (pLayer->strDefine.CollateNoCase(THICKNESS_LAYER_TXT) == 0)
			m_strThicknessLayer = pLayer->strLayer;
		iIndex++;
	}

	m_LayerList.EnableWindow(TRUE);
	m_LayerList.SetRedraw(TRUE);
	m_LayerList.Invalidate();

	if (!m_pstUserLayerSet->bIsPreview && (
		m_pstUserLayerSet->vcLayerSet.size() <= 0 ||
		m_pstUserLayerSet->iSelectedNum <= 0 ))
	{
		CString strMsg;
		strMsg.Format(L"작업할 레이어를 설정하여 주십시오.");
		AfxMessageBox(strMsg, MB_OK | MB_ICONEXCLAMATION);		
		SetLayerInfoView();		
		return RESULT_BAD;
	}	

	if (GetFlag_LoadLayers())
		DrawCamData();
	else
		DrawCamData(TRUE, TRUE);

	return RESULT_GOOD;
}

UINT32 CLayerInfoFormView::InitUserLayerInfo()
{
	m_LayerList.SetRedraw(FALSE);
	m_LayerList.EnableWindow(FALSE);
	m_pstUserLayerSet->ClearLayer();

	CString strDefine = L"";
	for (UINT32 iLoop = 0; iLoop < m_nLayerListCount; iLoop++)
	{
		LayerSet_t* pLayer = new LayerSet_t;

		pLayer->bCheck = false;// m_LayerList.GetCheck(iLoop) ? true : false;
		//pLayer->bPolarity = (m_LayerList.GetComboText(iLoop, LIST_POLARITY_COLUMN) == _T("+") ? true : false);
		pLayer->color = m_LayerList.GetItemColor(iLoop);
// 		pLayer->dScaleXY.x = _ttof(m_LayerList.GetItemText(iLoop, LIST_SCALEX_COLUMN));
// 		pLayer->dScaleXY.y = _ttof(m_LayerList.GetItemText(iLoop, LIST_SCALEY_COLUMN));
		pLayer->strLayer = m_LayerList.GetItemText(iLoop, LIST_LAYER_COLUMN);
		pLayer->strDefine = _T("");
		pLayer->index = iLoop;
		
		m_pstUserLayerSet->vcLayerSet.emplace_back(pLayer);
	}

	m_pstUserLayerSet->iPreviewNum = m_nLayerListCount;

	m_LayerList.EnableWindow(TRUE);
	m_LayerList.SetRedraw(TRUE);
	m_LayerList.Invalidate();

	return RESULT_GOOD;
}

void CLayerInfoFormView::InitHistogram()
{
	ClearHistogram();
	
	m_vecvecHistogram.resize(DEF_HISTOGRAM::TypeMax);

	int nLayerCount = static_cast<int>(m_pstUserLayerSet->vcLayerSet.size());
	if (nLayerCount == -1) return;

	for (int i = 0; i < DEF_HISTOGRAM::TypeMax; i++)
	{
		m_vecvecHistogram[i].resize(nLayerCount);
// 		for (int j = 0; j < nLayerCount; j++)
// 		{
// 			m_vecvecHistogram[i][j] = new CHistogram();
// 
// 			m_vecvecHistogram[i][j]->Create(IDD_HISTOGRAM, this);
// 			m_vecvecHistogram[i][j]->SetHistoType(i);
// 			m_vecvecHistogram[i][j]->SetLayerName(m_pstUserLayerSet->vcLayerSet[j]->strLayer);
// 
// 		}
	}
	
}

BOOL CLayerInfoFormView::MakeHistogram(IN const int &nType, IN const int &nLayer)
{
	int nTypeCount = static_cast<int>(m_vecvecHistogram.size());
	if (nType < 0) return FALSE;
	if (nType >= nTypeCount) return FALSE;

	int nLayerCount = static_cast<int>(m_vecvecHistogram[nType].size());
	if (nLayer < 0) return FALSE;
	if (nLayer >= nLayerCount) return FALSE;

	if (m_vecvecHistogram[nType][nLayer] == nullptr)
	{
		m_vecvecHistogram[nType][nLayer] = new CHistogram();
		
		m_vecvecHistogram[nType][nLayer]->Create(IDD_HISTOGRAM, this);
		m_vecvecHistogram[nType][nLayer]->SetHistoType(nType);
	}

	return TRUE;


}

void CLayerInfoFormView::ClearHistogram()
{
	int nTypeSize = static_cast<int>(m_vecvecHistogram.size());
	
	for (int i = 0; i < nTypeSize; i++)
	{
		int nSize = static_cast<int>(m_vecvecHistogram[i].size());

		if (nSize == 0) return;

		for (int j = 0; j < nSize; j++)
		{
			if (m_vecvecHistogram[i][j] == nullptr) continue;

			m_vecvecHistogram[i][j]->DestroyWindow();

			delete m_vecvecHistogram[i][j];
			m_vecvecHistogram[i][j] = nullptr;
		}
		m_vecvecHistogram[i].clear();
	}
	
	m_vecvecHistogram.clear();
}

int CLayerInfoFormView::_GetLayerIndex(IN CTypedPtrArray <CObArray, CLayer*> &arrLayer, IN const CString &strLayer)
{
	int nLayerIndex = -1;

	CLayer *pLayerTmp = nullptr;
	//선택된 레이어와 같은 이름의 layer를 찾는다.
	int nLayerCount = static_cast<int>(arrLayer.GetCount());
	for (int j = 0; j < nLayerCount; j++)
	{
		CLayer *pLayerTmp = arrLayer.GetAt(j);
		if (pLayerTmp == nullptr) return -1;

		if (pLayerTmp->m_strLayerName == strLayer)
		{
			nLayerIndex = j;
		}
	}

	return nLayerIndex;
}

void CLayerInfoFormView::SetHistogram(IN const int &nType, IN const int &nLayer, IN const CString &strLayerName)
{
	if (nType < 0) return;
	if (nType >= DEF_HISTOGRAM::TypeMax) return;
	if (nLayer < 0) return;

	CJobFile* pJobFile;
	IArchive_GetJobFile((void**)&pJobFile);

	INT32 nCurStepIndex = GetCurStepIndex();

	CStep* pStep = pJobFile->m_arrStep.GetAt(nCurStepIndex);
	if (pStep == nullptr) return;

	INT_PTR nSize = pStep->m_arrLayer.GetSize();
	if (nSize <= 0)
	{
		AfxMessageBox(_T("먼저 검사 레이어 추출을 진행하여 주세요."));
		return;
	}

	int nLayerIndex = 0;
	nLayerIndex = _GetLayerIndex(pStep->m_arrLayer, strLayerName);
	
	if (nLayerIndex < 0) return;
	if (nLayerIndex >= nSize) return;

	for (int j = 0; j < nSize; j++)
	{
		CLayer* pLayer = pStep->m_arrLayer.GetAt(j);
		if (pLayer == nullptr) continue;

		if (m_vecvecHistogram[nType][nLayer] == nullptr)
		{
			MakeHistogram(nType, nLayer);
		}

		if (pLayer->m_strLayerName == m_pstUserLayerSet->vcLayerSet[nLayer]->strLayer)
		{
			if (m_vecvecHistogram[nType][nLayer] == nullptr) continue;

			m_vecvecHistogram[nType][nLayer]->SetStepName(pStep->m_strStepName);
			m_vecvecHistogram[nType][nLayer]->SetLayerName(pLayer->m_strLayerName);
			m_vecvecHistogram[nType][nLayer]->SetLayerPtr(pLayer);

			m_vecvecHistogram[nType][nLayer]->ShowWindow(SW_SHOW);

			break;
		}

	}
}

void CLayerInfoFormView::OnPopMenu_FeatureHisto()
{//popup menu feature histogram 클릭시 실행
	int nHistoSize = static_cast<int>(m_vecvecHistogram[DEF_HISTOGRAM::Type_Feature].size());
	if (m_nSelectR_Item < 0) return;
	if (m_nSelectR_Item >= nHistoSize) return;

	

	SetHistogram(DEF_HISTOGRAM::Type_Feature, m_nSelectR_Item, m_strSelectR_LayerName);
}

void CLayerInfoFormView::OnPopMenu_AttributeHisto()
{//popup menu Attribute histogram 클릭시 실행
	int nHistoSize = static_cast<int>(m_vecvecHistogram[DEF_HISTOGRAM::Type_Attribute].size());
	if (m_nSelectR_Item < 0) return;
	if (m_nSelectR_Item >= nHistoSize) return;

	SetHistogram(DEF_HISTOGRAM::Type_Attribute, m_nSelectR_Item, m_strSelectR_LayerName);
}

void CLayerInfoFormView::OnPopMenu_SymbolHisto()
{//popup menu Symbol histogram 클릭시 실행
	int nHistoSize = static_cast<int>(m_vecvecHistogram[DEF_HISTOGRAM::Type_Symbol].size());
	if (m_nSelectR_Item < 0) return;
	if (m_nSelectR_Item >= nHistoSize) return;

	SetHistogram(DEF_HISTOGRAM::Type_Symbol, m_nSelectR_Item, m_strSelectR_LayerName);
}

void CLayerInfoFormView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	//우클릭 막기
	//도킹패널에서 사용되는 
	if (&m_LayerList == pWnd)
	{
		return;
	}

	return CDockablePane::OnContextMenu(pWnd, point);
}

afx_msg void CLayerInfoFormView::OnItemClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (GetMachineType() != eMachineType::eNSIS)
		return;

	//NSIS - Sem
	if (GetUserSite() == eUserSite::eSEM)
		return;

	//NSIS - Daduck Only
	
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;
	
	if (pNMListView == nullptr)
		return;

	INT32 iRow = pNMListView->iItem;
	INT32 iCol = pNMListView->iSubItem;
	if (iCol > 1)
		return;

	CString strLayer = L"";
	strLayer = m_LayerList.GetItemText(iRow, 1);
	strLayer.Replace(_T("_MP"), _T(""));
	strLayer.Replace(_T("_mp"), _T(""));

	ChangeLayerInfo(strLayer);
	

//	void* lParam = (void*)&strLayer;
//	CWnd *pParent = GetParentOwner();

//	if(pParent)
//		pParent->SendMessage(WM_CHANGE_LAYER_INFO, iRow, (LPARAM)lParam);
}

UINT32 CLayerInfoFormView::DisplayStep_for_NSIS(CString strJobPath, std::vector<stStepInfo*>* vcStepInfo)
{
	if (m_pSysSpec == nullptr || m_pstUserLayerSet == nullptr)
		return RESULT_BAD;

	CFileFind file;
	CString strFileName = L"", strTemp = L"";

	UINT32 iStepNum = static_cast<UINT32>(vcStepInfo->size());

	//1. Step	
	m_pstUserLayerSet->ClearStep();
	for (UINT32 iLoop = 0; iLoop < iStepNum; iLoop++)
	{
		stStepInfo* pInfo = vcStepInfo->at(iLoop);
		if (pInfo == nullptr)
			return RESULT_BAD;

		strFileName.Format(L"%s\\steps\\%s\\profile", strJobPath, pInfo->strName);
		if (!file.FindFile(strFileName))
			continue;

		m_StepList.InsertItem(iLoop, NULL_STRING);
		m_StepList.SetItemText(iLoop, LIST_STEP_NAME_COL, pInfo->strName);

		//m_StepList.SetItemTextColor(iLoop, LIST_STEP_DEFINE_COL, MAGENTA_COLOR);
		m_StepList.SetItemTextColor(iLoop, LIST_STEP_DEFINE_COL, PURPLE_COLOR);

		for (auto it : m_pSysSpec->sysStep.vcPanelNames)
		{
			if (it.CompareNoCase(pInfo->strName) == 0)
			{
				m_StepList.SetItemText(iLoop, LIST_STEP_DEFINE_COL, PANEL_STRING);
				m_pstUserLayerSet->nPanelStepIdx = iLoop;
			}
		}

		for (auto it : m_pSysSpec->sysStep.vcStripNames)
		{
			if (it.CompareNoCase(pInfo->strName) == 0)
			{
				m_StepList.SetItemText(iLoop, LIST_STEP_DEFINE_COL, STRIP_STRING);
				m_pstUserLayerSet->vcStripStepIdx.emplace_back(iLoop);
			}
		}

		for (auto it : m_pSysSpec->sysStep.vcUnitNames)
		{
			if (it.CompareNoCase(pInfo->strName) == 0)
			{
				m_StepList.SetItemText(iLoop, LIST_STEP_DEFINE_COL, UNIT_STRING);
				m_pstUserLayerSet->vcUnitStepIdx.emplace_back(iLoop);
			}
		}

		m_nStepListCount++;
	}

	if (m_pstUserLayerSet->nPanelStepIdx >= 0 &&
		(m_pstUserLayerSet->vcStripStepIdx.size() >= 0 || m_pstUserLayerSet->vcUnitStepIdx.size() >= 0))
		m_bIsStepSet = TRUE;

	return RESULT_GOOD;
}

UINT32 CLayerInfoFormView::DisplayStep_for_AOI(CString strJobPath, std::vector<stStepInfo*>* vcStepInfo)
{
	if (m_pSysSpec == nullptr || m_pstUserLayerSet == nullptr)
		return RESULT_BAD;

	CFileFind file;
	CString strFileName = L"", strTemp = L"";
	std::vector<CString> vcUnitSteps;

	UINT32 iStepNum = static_cast<UINT32>(vcStepInfo->size());

	//1. Step	
	m_pstUserLayerSet->ClearStep();
	for (UINT32 iLoop = 0; iLoop < iStepNum; iLoop++)
	{
		stStepInfo* pInfo = vcStepInfo->at(iLoop);
		if (pInfo == nullptr)
			return RESULT_BAD;

		strFileName.Format(L"%s\\steps\\%s\\profile", strJobPath, pInfo->strName);
		if (!file.FindFile(strFileName))
			continue;

		m_StepList.InsertItem(iLoop, NULL_STRING);
		m_StepList.SetItemText(iLoop, LIST_STEP_NAME_COL, pInfo->strName);

		//m_StepList.SetItemTextColor(iLoop, LIST_STEP_DEFINE_COL, MAGENTA_COLOR);
		m_StepList.SetItemTextColor(iLoop, LIST_STEP_DEFINE_COL, PURPLE_COLOR);

		for (auto it : m_pSysSpec->sysStep.vcPanelNames)
		{
			if (it.CompareNoCase(pInfo->strName) == 0)
			{
				m_StepList.SetItemText(iLoop, LIST_STEP_DEFINE_COL, it);
				m_pstUserLayerSet->nPanelStepIdx = iLoop;
			}
		}

		for (auto it : m_pSysSpec->sysStep.vcStripNames)
		{
			if (pInfo->strName.Find(it) == 0 && pInfo->strName.Find(L"VM") == -1 &&
				pInfo->strName.Find(L"COUPON") == -1)
			{
				m_StepList.SetItemText(iLoop, LIST_STEP_DEFINE_COL, it);
				m_pstUserLayerSet->vcStripStepIdx.emplace_back(iLoop);
			}
		}

		for (auto it : m_pSysSpec->sysStep.vcUnitNames)
		{
			if (pInfo->strName.Find(it) == 0 && pInfo->strName.Find(L"VM") == -1 &&
				pInfo->strName.Find(L"COUPON") == -1)
			{
				m_StepList.SetItemText(iLoop, LIST_STEP_DEFINE_COL, it);
				m_pstUserLayerSet->vcUnitStepIdx.emplace_back(iLoop);

				AfxExtractSubString(strTemp, pInfo->strName, 1, '-');
				if (strTemp.GetLength() > 0)
					vcUnitSteps.emplace_back(pInfo->strName);
			}
			else if (pInfo->strName.CompareNoCase(L"ALIGN") == 0)
			{
				m_StepList.SetItemText(iLoop, LIST_STEP_DEFINE_COL, it);
				m_pstUserLayerSet->vcUnitStepIdx.emplace_back(iLoop);
			}
		}

		m_nStepListCount++;
	}

	for (auto itStep : m_pSysSpec->sysStep.vcUnitNames)
	{
		for (auto it : vcUnitSteps)
		{
			CString strUnitStep = it;
			strUnitStep.Replace(L"UNIT", L"ALIGN");

			for (UINT32 iLoop = 0; iLoop < m_nStepListCount; iLoop++)
			{
				CString strName = m_StepList.GetItemText(iLoop, LIST_STEP_NAME_COL);

				if (strName.CompareNoCase(strUnitStep) == 0)
				{
					m_StepList.SetItemText(iLoop, LIST_STEP_DEFINE_COL, itStep);
					m_pstUserLayerSet->vcUnitStepIdx.emplace_back(iLoop);
				}
			}
		}
	}
	
	vcUnitSteps.clear();

	if (m_pstUserLayerSet->nPanelStepIdx >= 0 &&
		(m_pstUserLayerSet->vcStripStepIdx.size() >= 0 || m_pstUserLayerSet->vcUnitStepIdx.size() >= 0))
		m_bIsStepSet = TRUE;

	return RESULT_GOOD;
}


UINT32 CLayerInfoFormView::DisplayLayer_for_NSIS(CString strJobPath, std::vector<stStepInfo*>* vcStepInfo,
	std::vector<stLayerInfo*>* vcLayerInfo, eProdSide prodSide, std::vector<stLayerPair*> *m_vcLayerList,
	INT32 iFrontlayerIdx, INT32 iBacklayerIdx)
{
	if (m_pSysSpec == nullptr || m_pstUserLayerSet == nullptr)
		return RESULT_BAD;

	UINT32 iStepNum = static_cast<UINT32>(vcStepInfo->size());
	UINT32 iLayerNum = static_cast<UINT32>(vcLayerInfo->size());

	CFileFind file;
	CString strFileName = L"";

	//2. Layer
	UINT32 iColorIdx = 0, iLayerCount = 0;
	for (UINT32 iLoop = 0; iLoop < iLayerNum; iLoop++)
	{
		stLayerInfo* pInfo = vcLayerInfo->at(iLoop);
		if (pInfo == nullptr)
		{
			m_bIsDone = TRUE;
			return RESULT_BAD;
		}

		iLayerCount = 0;
		for (UINT32 iStep = 0; iStep < iStepNum; iStep++)
		{
			stStepInfo* pStepInfo = vcStepInfo->at(iStep);
			if (pInfo == nullptr)
			{
				m_bIsDone = TRUE;
				return RESULT_BAD;
			}

			strFileName.Format(L"%s\\steps\\%s\\layers\\%s\\features", strJobPath, pStepInfo->strName, pInfo->strName);
			if (file.FindFile(strFileName))
				iLayerCount++;
		}

		if (iLayerCount != iStepNum)
			continue;

		CString strMPLayer = L"";
		bool bIsLayerSet = false;

		for (auto Layer : *m_vcLayerList)
		{
			if (Layer->strTop.CompareNoCase(pInfo->strName) == 0)
				bIsLayerSet = true;
			else if (Layer->strBot.CompareNoCase(pInfo->strName) == 0)
				bIsLayerSet = true;

			//MP
			strMPLayer.Format(_T("%s_MP"), Layer->strTop);
			if (strMPLayer.CompareNoCase(pInfo->strName) == 0)
				bIsLayerSet = true;

			strMPLayer.Format(_T("%s_MP"), Layer->strBot);
			if (strMPLayer.CompareNoCase(pInfo->strName) == 0)
				bIsLayerSet = true;

			//TP
			strMPLayer.Format(_T("%s_TP"), Layer->strTop);
			if (strMPLayer.CompareNoCase(pInfo->strName) == 0)
				bIsLayerSet = true;

			strMPLayer.Format(_T("%s_TP"), Layer->strBot);
			if (strMPLayer.CompareNoCase(pInfo->strName) == 0)
				bIsLayerSet = true;
		}

		if (!bIsLayerSet)
			continue;

		m_LayerList.InsertItem(m_nLayerListCount, NULL_STRING);
		m_LayerList.SetCheck(m_nLayerListCount, FALSE);
		m_LayerList.SetItemText(m_nLayerListCount, LIST_LAYER_COLUMN, pInfo->strName);
		m_LayerList.SetItemTextColor(m_nLayerListCount, LIST_DEFINE_COLUMN, BLUE_COLOR);
		m_LayerList.SetItemColor(m_nLayerListCount, RGB(DEFCOLOR_R(iColorIdx), DEFCOLOR_G(iColorIdx), DEFCOLOR_B(iColorIdx)));

		UINT32 iCurSel = 0;
		CString strLayerIdx = L"", strLayerName = L"";

		for (auto it : m_pSysSpec->sysLayer.vcLayer)
		{
			if (prodSide == eProdSide::eTop)
			{
				if (!pInfo->bIsTop)
					continue;

				for (auto itNames : it->vcFrontLayerNames)
				{
					strLayerName = itNames;
					strLayerIdx.Format(L"%d", iFrontlayerIdx);
					strLayerName.Replace(L"[s]", strLayerIdx);

					if (strLayerName.CompareNoCase(pInfo->strName) == 0)
					{
						m_LayerList.SetComboBoxCurSel(m_nLayerListCount, LIST_DEFINE_COLUMN, iCurSel);
						COLORREF color = m_pSysSpec->sysLayer.vcLayer[iCurSel]->layerColor;
						m_LayerList.SetItemColor(m_nLayerListCount, color);
					}
				}
			}
			else
			{
				if (pInfo->bIsTop)
					continue;

				for (auto itNames : it->vcBackLayerNames)
				{
					strLayerName = itNames;
					strLayerIdx.Format(L"%d", iBacklayerIdx);
					strLayerName.Replace(L"[e]", strLayerIdx);

					if (strLayerName.CompareNoCase(pInfo->strName) == 0)
					{
						m_LayerList.SetComboBoxCurSel(m_nLayerListCount, LIST_DEFINE_COLUMN, iCurSel);
						COLORREF color = m_pSysSpec->sysLayer.vcLayer[iCurSel]->layerColor;
						m_LayerList.SetItemColor(m_nLayerListCount, color);
					}
				}
			}
			iCurSel++;
		}

		iColorIdx++;
		m_nLayerListCount++;
	}

	return RESULT_GOOD;
}

/*UINT32 CLayerInfoFormView::DisplayLayer_for_NSIS_SEM(CString strJobPath, std::vector<stStepInfo*>* vcStepInfo,
	std::vector<stLayerInfo*>* vcLayerInfo, eProdSide prodSide, std::vector<stLayerPair*> *m_vcLayerList,
	INT32 iFrontlayerIdx, INT32 iBacklayerIdx)*/
UINT32 CLayerInfoFormView::DisplayLayer_for_NSIS_SEM(CString strJobPath, std::vector<stStepInfo*>* vcStepInfo,
	std::vector<stLayerInfo*>* vcLayerInfo, eProdSide prodSide,	CString strFrontlayer, CString strBacklayer)
{
	if (m_pSysSpec == nullptr || m_pstUserLayerSet == nullptr)
		return RESULT_BAD;

	UINT32 iStepNum = static_cast<UINT32>(vcStepInfo->size());
	UINT32 iLayerNum = static_cast<UINT32>(vcLayerInfo->size());

	CFileFind file;
	CString strFileName = L"";

	//2. Layer
	UINT32 iColorIdx = 0, iLayerCount = 0;
	for (UINT32 iLoop = 0; iLoop < iLayerNum; iLoop++)
	{
		stLayerInfo* pInfo = vcLayerInfo->at(iLoop);
		if (pInfo == nullptr)
		{
			m_bIsDone = TRUE;
			return RESULT_BAD;
		}

		iLayerCount = 0;
		for (UINT32 iStep = 0; iStep < iStepNum; iStep++)
		{
			stStepInfo* pStepInfo = vcStepInfo->at(iStep);
			if (pInfo == nullptr)
			{
				m_bIsDone = TRUE;
				return RESULT_BAD;
			}

			strFileName.Format(L"%s\\steps\\%s\\layers\\%s\\features", strJobPath, pStepInfo->strName, pInfo->strName);
			if (file.FindFile(strFileName))
				iLayerCount++;
		}

		if (iLayerCount != iStepNum)
			continue;

// 		CString strMPLayer = L"";
// 		bool bIsLayerSet = false;
// 
// 		for (auto Layer : *m_vcLayerList)
// 		{
// 			if (Layer->strTop.CompareNoCase(pInfo->strName) == 0)
// 				bIsLayerSet = true;
// 			else if (Layer->strBot.CompareNoCase(pInfo->strName) == 0)
// 				bIsLayerSet = true;
//  
//  		//	//MP
//  			strMPLayer.Format(_T("%s_MP"), Layer->strTop);
//  			if (strMPLayer.CompareNoCase(pInfo->strName) == 0)
//  				bIsLayerSet = true;
//  
//  			strMPLayer.Format(_T("%s_MP"), Layer->strBot);
//  			if (strMPLayer.CompareNoCase(pInfo->strName) == 0)
//  				bIsLayerSet = true;
//  
//  		//	//TP
//  			strMPLayer.Format(_T("%s_TP"), Layer->strTop);
//  			if (strMPLayer.CompareNoCase(pInfo->strName) == 0)
//  				bIsLayerSet = true;
//  
//  			strMPLayer.Format(_T("%s_TP"), Layer->strBot);
//  			if (strMPLayer.CompareNoCase(pInfo->strName) == 0)
//  				bIsLayerSet = true;
//  		}
//  
// 		if (!bIsLayerSet)
// 			continue;

		m_LayerList.InsertItem(m_nLayerListCount, NULL_STRING);
		m_LayerList.SetCheck(m_nLayerListCount, FALSE);
		m_LayerList.SetItemText(m_nLayerListCount, LIST_LAYER_COLUMN, pInfo->strName);
		m_LayerList.SetItemTextColor(m_nLayerListCount, LIST_DEFINE_COLUMN, BLUE_COLOR);
		m_LayerList.SetItemColor(m_nLayerListCount, RGB(DEFCOLOR_R(iColorIdx), DEFCOLOR_G(iColorIdx), DEFCOLOR_B(iColorIdx)));

		UINT32 iCurSel = 0;
		CString strLayerIdx = L"", strLayerName = L"";
		
		CString tempStr = L"";

		INT32 iFrontIdx = -1;
		vector<CString> vecSliceFilter;
		BOOL bCheck = FALSE;
		CString strMP, strTP;

		for (auto it : m_pSysSpec->sysLayer.vcLayer)
		{
			if (prodSide == eProdSide::eTop)
			{
 				//if (!pInfo->bIsTop)
 				//	continue;

				for (auto itNames : it->vcFrontLayerNames)
				{
					strLayerName = itNames;

					strLayerName.MakeUpper();

					CUtils::SliceString(strLayerName, _T("[S]"), vecSliceFilter);

					CUtils::StringOutputIndex(pInfo->strName, vecSliceFilter, iFrontIdx, bCheck);
					
					if (iFrontIdx != -1 && bCheck == TRUE)
					{
						m_LayerList.SetComboBoxCurSel(m_nLayerListCount, LIST_DEFINE_COLUMN, iCurSel);
						COLORREF color = m_pSysSpec->sysLayer.vcLayer[iCurSel]->layerColor;
						m_LayerList.SetItemColor(m_nLayerListCount, color);					
					}

					//strLayerIdx.Format(L"%d", iFrontlayerIdx);
					//strLayerName.Replace(L"[s]", strLayerIdx);

					//int iLength = strLayerName.GetLength();
					//tempStr = pInfo->strName.Left(iLength);

					//if (strLayerName.CompareNoCase(pInfo->strName) == 0)
 					//if (strLayerName.CompareNoCase(tempStr) == 0)
					//{
						//m_LayerList.SetComboBoxCurSel(m_nLayerListCount, LIST_DEFINE_COLUMN, iCurSel);
						//COLORREF color = m_pSysSpec->sysLayer.vcLayer[iCurSel]->layerColor;
						//m_LayerList.SetItemColor(m_nLayerListCount, color);
					//}
					/*CString strTemp = strFrontlayer;
					strTemp.Replace(_T("L"), _T(""));
					strTemp.Replace(_T("l"), _T(""));
					strTemp.Replace(_T("S"), _T(""));
					strTemp.Replace(_T("s"), _T(""));

					strLayerIdx.Format(L"%d", _ttoi(strTemp));
					strLayerName.Replace(L"[s]", strLayerIdx);*/

					//CString strMP, strTP;
					//strMP.Format(_T("%s_MP"), strFrontlayer);
					//strTP.Format(_T("%s_TP"), strFrontlayer);					

					///*if (strLayerName.CompareNoCase(pInfo->strName) == 0 &&
					//	(strFrontlayer.CompareNoCase(pInfo->strName) == 0 ||
					//		strMP.CompareNoCase(pInfo->strName) == 0))*/
					//if (strFrontlayer.CompareNoCase(pInfo->strName) == 0 ||
					//		strMP.CompareNoCase(pInfo->strName) == 0 ||
					//		strTP.CompareNoCase(pInfo->strName) == 0)
					//{
					//	m_LayerList.SetComboBoxCurSel(m_nLayerListCount, LIST_DEFINE_COLUMN, iCurSel);
					//	COLORREF color = m_pSysSpec->sysLayer.vcLayer[iCurSel]->layerColor;
					//	m_LayerList.SetItemColor(m_nLayerListCount, color);
					//}
				}
			}
			else
			{
 				//if (pInfo->bIsTop)
 				//	continue;

				for (auto itNames : it->vcBackLayerNames)
				{
					strLayerName = itNames;
					/*strLayerIdx.Format(L"%d", iBacklayerIdx);
					strLayerName.Replace(L"[e]", strLayerIdx);

					if (strLayerName.CompareNoCase(pInfo->strName) == 0)
					{
						m_LayerList.SetComboBoxCurSel(m_nLayerListCount, LIST_DEFINE_COLUMN, iCurSel);
						COLORREF color = m_pSysSpec->sysLayer.vcLayer[iCurSel]->layerColor;
						m_LayerList.SetItemColor(m_nLayerListCount, color);
					}*/
					CString strTemp = strBacklayer;
					strTemp.Replace(_T("L"), _T(""));
					strTemp.Replace(_T("l"), _T(""));
					strTemp.Replace(_T("S"), _T(""));
					strTemp.Replace(_T("s"), _T(""));
					strTemp.Replace(_T("B"), _T(""));
					strTemp.Replace(_T("b"), _T(""));

					strLayerIdx.Format(L"%d", _ttoi(strTemp));
					strLayerName.Replace(L"[e]", strLayerIdx);

					CString strMP;
					strMP.Format(_T("%s_MP"), strBacklayer);

					if (strLayerName.CompareNoCase(pInfo->strName) == 0 &&
						(strBacklayer.CompareNoCase(pInfo->strName) == 0 ||
							strMP.CompareNoCase(pInfo->strName) == 0))
					{
						m_LayerList.SetComboBoxCurSel(m_nLayerListCount, LIST_DEFINE_COLUMN, iCurSel);
						COLORREF color = m_pSysSpec->sysLayer.vcLayer[iCurSel]->layerColor;
						m_LayerList.SetItemColor(m_nLayerListCount, color);
					}

				}
			}
			iCurSel++;
		}

		iColorIdx++;
		m_nLayerListCount++;
	}

	return RESULT_GOOD;
}

// UINT32 CLayerInfoFormView::DisplayLayer_for_NSIS_SEM_V2(CString strJobPath, std::vector<stStepInfo*>* vcStepInfo,
// 	std::vector<stLayerInfo*>* vcLayerInfo, eProdSide prodSide, std::vector<stLayerPair*> *m_vcLayerList,
// 	INT32 iFrontlayerIdx, INT32 iBacklayerIdx)
UINT32 CLayerInfoFormView::DisplayLayer_for_NSIS_SEM_V2(CString strJobPath, 
	std::vector<stStepInfo*>* vcStepInfo,
	std::vector<stLayerInfo*>* vcLayerInfo,
	eProdSide prodSide,
	UserSetInfo_t* pUserSetInfo,
	CString strFrontLayer,
	CString strBackLayer)
{
	if (m_pSysSpec == nullptr || m_pstUserLayerSet == nullptr)
		return RESULT_BAD;

	UINT32 iStepNum = static_cast<UINT32>(vcStepInfo->size());
	UINT32 iLayerNum = static_cast<UINT32>(vcLayerInfo->size());

	CFileFind file;
	CString strFileName = L"";

	//2. Layer
	UINT32 iColorIdx = 0, iLayerCount = 0;
	CString strTemp = _T("");
	CString strMPTemp = _T("");
	CString strTPTemp = _T("");
	CString strFilterName = _T("");
	vector<CString> vecSliceFilter;
	INT32 iFrontIdx = -1;
	BOOL bCheck = FALSE;
	BOOL bEscapeLoop = FALSE;

	BOOL bPatternFind = FALSE, bSRFind = FALSE, bMPFind = FALSE, bTPFind = FALSE;

	for (UINT32 iLoop = 0; iLoop < iLayerNum; iLoop++)
	{
		stLayerInfo* pInfo = vcLayerInfo->at(iLoop);
		if (pInfo == nullptr)
		{
			m_bIsDone = TRUE;
			return RESULT_BAD;
		}

		iLayerCount = 0;
		for (UINT32 iStep = 0; iStep < iStepNum; iStep++)
		{
			stStepInfo* pStepInfo = vcStepInfo->at(iStep);
			if (pInfo == nullptr)
			{
				m_bIsDone = TRUE;
				return RESULT_BAD;
			}

			strFileName.Format(L"%s\\steps\\%s\\layers\\%s\\features", strJobPath, pStepInfo->strName, pInfo->strName);
			if (file.FindFile(strFileName))
				iLayerCount++;
		}

		if (iLayerCount != iStepNum)
			continue;

		m_LayerList.InsertItem(m_nLayerListCount, NULL_STRING);
		m_LayerList.SetCheck(m_nLayerListCount, FALSE);
		m_LayerList.SetItemText(m_nLayerListCount, LIST_LAYER_COLUMN, pInfo->strName);
		m_LayerList.SetItemTextColor(m_nLayerListCount, LIST_DEFINE_COLUMN, BLUE_COLOR);
		m_LayerList.SetItemColor(m_nLayerListCount, RGB(DEFCOLOR_R(iColorIdx), DEFCOLOR_G(iColorIdx), DEFCOLOR_B(iColorIdx)));

		UINT32 iCurSel = 0;
		CString strLayer = pInfo->strName;
		strMPTemp.Format(_T("%s_MP"), strFrontLayer);
		strTPTemp.Format(_T("%s_TP"), strFrontLayer);
		//INT32 iKind = -1;

		if (strLayer.Find(_T("_BACKUP")) >= 0 || strLayer.Find(_T("_backup")) >= 0)
			continue;

		for (auto it : m_pSysSpec->sysLayer.vcLayer)
		{
			if (pInfo->bIsTop == TRUE)
			{
				for (int i = 0; i < it->vcFrontLayerNames.size(); i++)
				{
					if (bPatternFind == TRUE && it->strDefine == PATTERN_LAYER_TXT) continue;
					if (bSRFind == TRUE && it->strDefine == SR_LAYER_TXT) continue;
					if (bMPFind == TRUE && it->strDefine == MEASURE_LAYER_TXT) continue;
					if (bTPFind == TRUE && it->strDefine == THICKNESS_LAYER_TXT) continue;
					
					strFilterName = it->vcFrontLayerNames[i];
					strFilterName.MakeUpper();

					CUtils::SliceString(strFilterName, _T("[S]"), vecSliceFilter);

					CUtils::StringOutputIndex(strLayer, vecSliceFilter, iFrontIdx, bCheck);

					if (iFrontIdx != -1 && bCheck == TRUE)
					{
						if (it->strDefine == PATTERN_LAYER_TXT && bPatternFind == FALSE)
							bPatternFind = TRUE;
						else if (it->strDefine == SR_LAYER_TXT && bSRFind == FALSE)
							bSRFind = TRUE;
						else if (it->strDefine == MEASURE_LAYER_TXT && bMPFind == FALSE)
						{
							bMPFind = TRUE;
							if (strLayer.CompareNoCase(strMPTemp) != 0)
							{
								bMPFind = FALSE; 
								bEscapeLoop = FALSE;
								break;
							}
						}
						else if (it->strDefine == THICKNESS_LAYER_TXT && bTPFind == FALSE)
						{
							bTPFind = TRUE;
							if (strLayer.CompareNoCase(strTPTemp) != 0)
							{
								bTPFind = FALSE;
								bEscapeLoop = FALSE;
								break;
							}
						}
						
						bEscapeLoop = TRUE;
						break;
					}
				}
				if (bEscapeLoop == TRUE)				
				{					
					m_LayerList.SetComboBoxCurSel(m_nLayerListCount, LIST_DEFINE_COLUMN, iCurSel);
					COLORREF color = m_pSysSpec->sysLayer.vcLayer[iCurSel]->layerColor;
					m_LayerList.SetItemColor(m_nLayerListCount, color);

					bEscapeLoop = FALSE;
					break;
				}
			}			
			iCurSel++;

		}
		
		
		// 주석처리 
		/*
		if (strLayer.Find(_T("_BACKUP")) >= 0 || strLayer.Find(_T("_backup")) >= 0)
			continue;

		if (strLayer.Find(_T("_MP")) >= 0 || strLayer.Find(_T("_mp")) >= 0)
		{// MP Layer
			iKind = 1;
		}
		else if (strLayer.Find(_T("_TP")) >= 0 || strLayer.Find(_T("_tp")) >= 0)
		{// TP Layer
			iKind = 2;
		}
		else
		{// Pattern Layer
			iKind = 0;
		}
		
		if (iKind == 0 && bPatternFind == FALSE)
		{// Pattern Layer
			
			CString strTemp = _T("");
			if (pUserSetInfo->strWorkLayer.GetLength() <= 0)
			{
				strTemp = strFrontLayer;
			}
			else
			{
				strTemp = pUserSetInfo->strWorkLayer;
			}
			
			if (strTemp.CompareNoCase(strLayer) == 0)
			{
				// Pattern
				iCurSel = 0;
				m_LayerList.SetComboBoxCurSel(m_nLayerListCount, LIST_DEFINE_COLUMN, iCurSel);
				COLORREF color = m_pSysSpec->sysLayer.vcLayer[iCurSel]->layerColor;
				m_LayerList.SetItemColor(m_nLayerListCount, color);
				bPatternFind = TRUE;

				strMPTemp.Format(_T("%s_MP"), strTemp);
				strTPTemp.Format(_T("%s_TP"), strTemp);				
			}					
		}
		if (bPatternFind == TRUE && bMPFind == FALSE)
		{
			if (strMPTemp.CompareNoCase(strLayer) == 0)
			{
				// MP
				iCurSel = 10;
				m_LayerList.SetComboBoxCurSel(m_nLayerListCount, LIST_DEFINE_COLUMN, iCurSel);
				COLORREF color = m_pSysSpec->sysLayer.vcLayer[iCurSel]->layerColor;
				m_LayerList.SetItemColor(m_nLayerListCount, color);
				bMPFind = TRUE;	
			}
		}

		if (bPatternFind == TRUE && bMPFind == TRUE && bTPFind == FALSE)
		{
			if (strTPTemp.CompareNoCase(strLayer) == 0)
			{
				// TP
				iCurSel = 18;
				m_LayerList.SetComboBoxCurSel(m_nLayerListCount, LIST_DEFINE_COLUMN, iCurSel);
				COLORREF color = m_pSysSpec->sysLayer.vcLayer[iCurSel]->layerColor;
				m_LayerList.SetItemColor(m_nLayerListCount, color);
				bTPFind = TRUE;
			}
		}
		*/

		iColorIdx++;
		m_nLayerListCount++;
	}

	return RESULT_GOOD;
}


UINT32 CLayerInfoFormView::DisplayLayer_for_AOI(CString strJobPath, std::vector<stStepInfo*>* vcStepInfo, std::vector<stLayerInfo*>* vcLayerInfo)
{
	if (m_pSysSpec == nullptr || m_pstUserLayerSet == nullptr)
		return RESULT_BAD;

	UINT32 iStepNum = static_cast<UINT32>(vcStepInfo->size());
	UINT32 iLayerNum = static_cast<UINT32>(vcLayerInfo->size());

	CFileFind file;
	CString strFileName = L"";

	//2. Layer
	UINT32 iColorIdx = 0, iLayerCount = 0;
	for (UINT32 iLoop = 0; iLoop < iLayerNum; iLoop++)
	{
		stLayerInfo* pInfo = vcLayerInfo->at(iLoop);
		if (pInfo == nullptr)
		{
			m_bIsDone = TRUE;
			return RESULT_BAD;
		}

		iLayerCount = 0;
		for (UINT32 iStep = 0; iStep < iStepNum; iStep++)
		{
			stStepInfo* pStepInfo = vcStepInfo->at(iStep);
			if (pInfo == nullptr)
			{
				m_bIsDone = TRUE;
				return RESULT_BAD;
			}

			strFileName.Format(L"%s\\steps\\%s\\layers\\%s\\features", strJobPath, pStepInfo->strName, pInfo->strName);
			if (file.FindFile(strFileName))
				iLayerCount++;
		}

		if (iLayerCount != iStepNum)
			continue;

		//Pattern Layer : BOARD & SIGNAL
		if (pInfo->strContext.CompareNoCase(L"BOARD") == 0 &&
			pInfo->strType.CompareNoCase(L"SIGNAL") == 0)
		{
			m_cbInspLayer.AddString(pInfo->strName);
		}
		else
		{
			CString strExt = _T("");
			if (pInfo->strType.CompareNoCase(L"SIGNAL") == 0)
			{
				AfxExtractSubString(strExt, pInfo->strName, 2, '-');
				if (strExt.GetLength() < 1)
				{
					AfxExtractSubString(strExt, pInfo->strName, 1, '-');
					if (strExt.CompareNoCase(L"TT") == 0 || strExt.CompareNoCase(L"TB") == 0)
					{
						AfxExtractSubString(strExt, pInfo->strName, 0, '-');
						if (strExt.Find(L"P") != -1 || strExt.Find(L"p") != -1)							
							m_cbInspLayer.AddString(pInfo->strName);								
					}
				}
			}
		}

		m_LayerList.InsertItem(m_nLayerListCount, NULL_STRING);
		m_LayerList.SetCheck(m_nLayerListCount, FALSE);
		m_LayerList.SetItemText(m_nLayerListCount, LIST_LAYER_COLUMN, pInfo->strName);
//		m_LayerList.SetItemTextColor(m_nLayerListCount, LIST_SIDE_COLUMN, BLUE_COLOR);
		m_LayerList.SetItemTextColor(m_nLayerListCount, LIST_DEFINE_COLUMN, BLUE_COLOR);
		m_LayerList.SetItemColor(m_nLayerListCount, RGB(DEFCOLOR_R(iColorIdx), DEFCOLOR_G(iColorIdx), DEFCOLOR_B(iColorIdx)));

// 		m_LayerList.SetItemText(m_nLayerListCount, LIST_SCALEX_COLUMN, _T("1.0"));
// 		m_LayerList.SetItemText(m_nLayerListCount, LIST_SCALEY_COLUMN, _T("1.0"));
// 		m_LayerList.SetEditBoxMinMax(0.5, 1.5);

// 		if (pInfo->bIsPositive)
// 			m_LayerList.SetItemText(m_nLayerListCount, LIST_POLARITY_COLUMN, _T("+"));
// 		else
// 			m_LayerList.SetItemText(m_nLayerListCount, LIST_POLARITY_COLUMN, _T("-"));		

		iColorIdx++;
		m_nLayerListCount++;
	}

	return RESULT_GOOD;
}

void CLayerInfoFormView::OnCbnSelChangeInspLayerComboBox()
{
	int iSel = m_cbInspLayer.GetCurSel();
	if (CB_ERR == iSel) return;

	UINT32 iRst = RESULT_GOOD;
	CString strText = L"";
	m_cbInspLayer.GetLBText(iSel, strText);
	if (strText.GetLength() < 1) return;

	if (!GetFlag_IsAllLayerLoaded())
	{
		ClearLayers();

		UserSetInfo_t* pstUserSetInfo = GetUserSetInfo();
		if (pstUserSetInfo)
		{
			pstUserSetInfo->bShowStepRepeat = false;
			pstUserSetInfo->bShowFOV = false;
			pstUserSetInfo->bShowCell = false;
			pstUserSetInfo->bShowSurface = false;
			pstUserSetInfo->bShowUnitAlign = false;
			pstUserSetInfo->bShowInspMask = false;
		}

		IsDoneLoadLayers(FALSE);
	}

	ClearDisplay();	

	eUserSite userSite = GetUserSite();
	eMachineType machineType = GetMachineType();
	if (eUserSite::eLGIT == userSite)
	{
		iRst = SetLayerInfo_for_LGIT(strText);
	}
	else if (eUserSite::eDAEDUCK == userSite)
	{
		if (machineType == eMachineType::eAVI)
		{
			iRst = SetLayerInfo_for_DAEDUCK_AVI(strText);
		}
		else
		{
			iRst = SetLayerInfo_for_DAEDUCK(strText); 
		}
		
	}

	if (iRst == RESULT_GOOD)	
		SetUserLayerInfo();	

	m_LayerList.Invalidate();	
}

UINT32 CLayerInfoFormView::SetLayerInfo_for_LGIT(CString strInspLayer)
{
	bool bIsOdd = false;
	int iMinInnerCnt = 6;
	int iPatternCnt = m_cbInspLayer.GetCount();

	CString strLayer = L"", strExt = L"";
	std::vector<int> vcLayerIdx;
	for (int iLoop = 0; iLoop < iPatternCnt; iLoop++)
	{
		m_cbInspLayer.GetLBText(iLoop, strExt);
		AfxExtractSubString(strLayer, strExt, 0, '-');
		strLayer.Replace(L"L", L"");
		strLayer.Replace(L"P", L"");
		strLayer.Trim();

		bool bExist = false;
		int idx = _ttoi(strLayer);
		for (auto it : vcLayerIdx)
		{
			if (it == idx)
			{
				bExist = true;
				break;
			}
		}

		if (!bExist)
			vcLayerIdx.emplace_back(_ttoi(strLayer));
	}

	if (iPatternCnt != vcLayerIdx.size())
		iPatternCnt = static_cast<int>(vcLayerIdx.size());

	vcLayerIdx.clear();
	if (iPatternCnt % 2)
	{
		bIsOdd = true;
		iMinInnerCnt = 5;
	}

	CString strOut = L"OUT-UNIT";

	//내층일 경우, Skip
	CString strSR = L"", strDoubleSR = L"";
	if (strInspLayer.Find(L"P") == -1)
	{
		if (strInspLayer.Find(L"L1") != -1)
		{
			strSR.Format(L"L%d-1SR", iPatternCnt + 1);
			strDoubleSR.Format(L"L%d-2SR", iPatternCnt + 1);
		}
		else
		{
			CString strLast = L"";
			strLast.Format(L"L%d", iPatternCnt);
			if (strInspLayer.Find(strLast) != -1)
			{
				strSR.Format(L"L%d-1SR", iPatternCnt + 2);
				strDoubleSR.Format(L"L%d-2SR", iPatternCnt + 2);
			}
		}
	}

	UINT32 iIdxDrl = m_nLayerListCount + 1;
	UINT32 iIdxDrl2 = m_nLayerListCount + 1;
	CString strDrl = L"D-2", strDrl2 = L"D";
	CString strMask = L"";
	strMask.Format(L"%s-AOIMASK", strInspLayer);

	//Via
	AfxExtractSubString(strLayer, strInspLayer, 0, '-');

	CString strInsidePT = L"", strInsidePT2 = L"";
	strInsidePT.Format(L"IN-%s", strLayer);
	strInsidePT2.Format(L"ATTR-%s", strLayer);
	strLayer.Replace(L"L", L"");
	strLayer.Trim();

	AfxExtractSubString(strExt, strInspLayer, 1, '-');
	strExt = strExt.Right(1);
	strExt.Trim();

	int iLayerIdx = -1;
	UINT32 iIdxVia = m_nLayerListCount + 1;
	UINT32 iIdxVia2 = m_nLayerListCount + 1;
	UINT32 iIdxVia3 = m_nLayerListCount + 1;
	UINT32 iIdxVia4 = m_nLayerListCount + 1;
	CString strVia = L"", strVia2 = L"", strVia3 = L"", strVia4 = L"";
	
	if (strLayer.Find(L"P") != -1)
	{
		strLayer.Replace(L"P", L"");
		strLayer.Trim();
	}

	iLayerIdx = _ttoi(strLayer);
	if (iLayerIdx > 0)
	{
		if (iPatternCnt >= iMinInnerCnt)
		{
			if (iLayerIdx == iPatternCnt / 2)
			{
				strVia.Format(L"C-%d%d", iLayerIdx, iLayerIdx + 1);
				strVia2.Format(L"C-%d%d", iLayerIdx - 1, iLayerIdx);
				strVia3.Format(L"C-%d%d", iLayerIdx + 1, iLayerIdx);
			}
			else if (iLayerIdx == iPatternCnt / 2 + 1)
			{
				strVia.Format(L"C-%d%d", iLayerIdx - 1, iLayerIdx);
				strVia2.Format(L"C-%d%d", iLayerIdx + 1, iLayerIdx);
				strVia3.Format(L"C-%d%d", iLayerIdx, iLayerIdx - 1);
				if (strInspLayer.Find(strExt) != -1)
					strVia4.Format(L"C-%d%d", iLayerIdx, iLayerIdx + 1);
			}
			else
			{
				if (iLayerIdx > 1)
				{
					strVia.Format(L"C-%d%d", iLayerIdx, iLayerIdx - 1);
					strVia3.Format(L"C-%d%d", iLayerIdx - 1, iLayerIdx);

					strVia2.Format(L"C-%d%d", iLayerIdx + 1, iLayerIdx);
					strVia4.Format(L"C-%d%d", iLayerIdx, iLayerIdx + 1);
				}
				else
				{
					strVia.Format(L"C-%d%d", iLayerIdx, iLayerIdx + 1);
					strVia2.Format(L"C-%d%d", iLayerIdx + 1, iLayerIdx);
				}
			}
		}
		else
		{
			if (iLayerIdx > 1)
			{
				strVia.Format(L"C-%d%d", iLayerIdx, iLayerIdx - 1);
				strVia3.Format(L"C-%d%d", iLayerIdx - 1, iLayerIdx);

				strVia2.Format(L"C-%d%d", iLayerIdx + 1, iLayerIdx);
				strVia4.Format(L"C-%d%d", iLayerIdx, iLayerIdx + 1);
			}
			else
			{
				strVia.Format(L"C-%d%d", iLayerIdx, iLayerIdx + 1);
				strVia2.Format(L"C-%d%d", iLayerIdx + 1, iLayerIdx);
			}
		}
	}
	//	}

	m_LayerList.SetRedraw(FALSE);
	m_LayerList.EnableWindow(FALSE);

	bool bFindInsidePT = false;
	bool bFindSR = false;
	for (UINT32 iLoop = 0; iLoop < m_nLayerListCount; iLoop++)
	{
		m_LayerList.SetItemText(iLoop, LIST_DEFINE_COLUMN, NULL_LAYER_TXT);
		strLayer = m_LayerList.GetItemText(iLoop, LIST_LAYER_COLUMN);

		if (strLayer.CompareNoCase(strInspLayer) == 0)
		{
			m_LayerList.SetItemText(iLoop, LIST_DEFINE_COLUMN, PATTERN_LAYER_TXT);
			m_LayerList.SetItemColor(iLoop, m_pSysSpec->sysLayer.vcLayer[(int)eRefLayer::EN_PATTERN_LAYER]->layerColor);
		}
		else if (strLayer.CompareNoCase(strInsidePT) == 0)
		{
			m_LayerList.SetItemText(iLoop, LIST_DEFINE_COLUMN, INSIDE_PATTERN_LAYER_TXT);
			m_LayerList.SetItemColor(iLoop, m_pSysSpec->sysLayer.vcLayer[(int)eRefLayer::EN_INSIDE_PATTERN_LAYER]->layerColor);
			bFindInsidePT = true;
		}
		else if (strLayer.CompareNoCase(strSR) == 0)
		{
			m_LayerList.SetItemText(iLoop, LIST_DEFINE_COLUMN, SR_LAYER_TXT);
			m_LayerList.SetItemColor(iLoop, m_pSysSpec->sysLayer.vcLayer[(int)eRefLayer::EN_SR_LAYER]->layerColor);
			bFindSR = true;
		}
		else if (strLayer.CompareNoCase(strDoubleSR) == 0)
		{
			m_LayerList.SetItemText(iLoop, LIST_DEFINE_COLUMN, DOUBLE_SR_LAYER_TXT);
			m_LayerList.SetItemColor(iLoop, m_pSysSpec->sysLayer.vcLayer[(int)eRefLayer::EN_DOUBLE_SR_LAYER]->layerColor);
		}
		else if (strLayer.CompareNoCase(strMask) == 0)
		{
			m_LayerList.SetItemText(iLoop, LIST_DEFINE_COLUMN, MASK_LAYER_TXT);
			m_LayerList.SetItemColor(iLoop, m_pSysSpec->sysLayer.vcLayer[(int)eRefLayer::EN_MASK_LAYER]->layerColor);
		}
		else if (strLayer.Find(strVia) == 0)
		{
			if (strLayer.Find(L"HOLE") != -1 || strLayer.Find(L"MASK") != -1 ||
				strLayer.Find(L"VM") != -1 || strLayer.Find(L"F") != -1 ||
				strLayer.Find(L"W") != -1 || strLayer.Find(L"+") != -1)
				continue;

			CString strTemp = L"";
			AfxExtractSubString(strTemp, strLayer, 2, '-');
			if (strTemp.GetLength() > 0)
			{
				if (strExt.GetLength() > 0 && strTemp.Find(strExt) == -1)
					continue;
			}

			if (iPatternCnt >= iMinInnerCnt)
			{
				if (bIsOdd)
				{
					if (iLayerIdx == iPatternCnt / 2 + 1)
					{
						if (strLayer.Find(strExt) != -1)
							iIdxVia = iLoop;
						else
							iIdxVia = iLoop;
					}
					else
						iIdxVia = iLoop;
				}
				else
				{
					if (iLayerIdx == iPatternCnt / 2 || iLayerIdx == iPatternCnt / 2 + 1)
					{
						if (strLayer.Find(strExt) != -1)
							iIdxVia = iLoop;
					}
					else
						iIdxVia = iLoop;
				}
			}
			else
				iIdxVia = iLoop;
		}
		else if (strLayer.Find(strVia2) == 0)
		{
			if (strLayer.Find(L"HOLE") != -1 || strLayer.Find(L"MASK") != -1 ||
				strLayer.Find(L"VM") != -1 || strLayer.Find(L"F") != -1 ||
				strLayer.Find(L"W") != -1 || strLayer.Find(L"+") != -1)
				continue;

			CString strTemp = L"";
			AfxExtractSubString(strTemp, strLayer, 2, '-');
			if (strTemp.GetLength() > 0)
			{
				if (strExt.GetLength() > 0 && strTemp.Find(strExt) == -1)
					continue;
			}

			iIdxVia2 = iLoop;
		}
		else if (strVia3.GetLength() > 0 && strLayer.Find(strVia3) == 0)
		{
			if (strLayer.Find(L"HOLE") != -1 || strLayer.Find(L"MASK") != -1 ||
				strLayer.Find(L"VM") != -1 || strLayer.Find(L"F") != -1 ||
				strLayer.Find(L"W") != -1 || strLayer.Find(L"+") != -1)
				continue;

			CString strTemp = L"";
			AfxExtractSubString(strTemp, strLayer, 2, '-');
			if (strTemp.GetLength() > 0)
			{
				if (strExt.GetLength() > 0 && strTemp.Find(strExt) == -1)
					continue;
			}

			iIdxVia3 = iLoop;
		}
		else if (strVia4.GetLength() > 0 && strLayer.Find(strVia4) == 0)
		{
			if (strLayer.Find(L"HOLE") != -1 || strLayer.Find(L"MASK") != -1 ||
				strLayer.Find(L"VM") != -1 || strLayer.Find(L"F") != -1 ||
				strLayer.Find(L"W") != -1 || strLayer.Find(L"+") != -1)
				continue;

			CString strTemp = L"";
			AfxExtractSubString(strTemp, strLayer, 2, '-');
			if (strTemp.GetLength() > 0)
			{
				if (strExt.GetLength() > 0 && strTemp.Find(strExt) == -1)
					continue;
			}

			iIdxVia4 = iLoop;
		}
		else if (strLayer.CompareNoCase(strDrl) == 0)
			iIdxDrl = iLoop;
		else if (strLayer.CompareNoCase(strDrl2) == 0)
			iIdxDrl2 = iLoop;
		else if (strLayer.CompareNoCase(strOut) == 0)
		{
			m_LayerList.SetItemText(iLoop, LIST_DEFINE_COLUMN, UNIT_OUT_LAYER_TXT);
			m_LayerList.SetItemColor(iLoop, m_pSysSpec->sysLayer.vcLayer[(int)eRefLayer::EN_UNIT_OUT_LAYER]->layerColor);
		}
	}

	if (iIdxVia < m_nLayerListCount || iIdxVia3 < m_nLayerListCount)
	{
		if (iIdxVia < m_nLayerListCount)
		{
			m_LayerList.SetItemText(iIdxVia, LIST_DEFINE_COLUMN, VIA_LAYER_TXT);
			m_LayerList.SetItemColor(iIdxVia, m_pSysSpec->sysLayer.vcLayer[(int)eRefLayer::EN_VIA_LAYER]->layerColor);
		}
		else if (iIdxVia3 < m_nLayerListCount)
		{
			m_LayerList.SetItemText(iIdxVia3, LIST_DEFINE_COLUMN, VIA_LAYER_TXT);
			m_LayerList.SetItemColor(iIdxVia3, m_pSysSpec->sysLayer.vcLayer[(int)eRefLayer::EN_VIA_LAYER]->layerColor);
			iIdxVia = iIdxVia3;
		}

		if (iIdxVia2 < m_nLayerListCount)
		{
			m_LayerList.SetItemText(iIdxVia2, LIST_DEFINE_COLUMN, VIA2_LAYER_TXT);
			m_LayerList.SetItemColor(iIdxVia2, m_pSysSpec->sysLayer.vcLayer[(int)eRefLayer::EN_VIA2_LAYER]->layerColor);
		}
		else if (iIdxVia4 < m_nLayerListCount)
		{
			m_LayerList.SetItemText(iIdxVia4, LIST_DEFINE_COLUMN, VIA2_LAYER_TXT);
			m_LayerList.SetItemColor(iIdxVia4, m_pSysSpec->sysLayer.vcLayer[(int)eRefLayer::EN_VIA2_LAYER]->layerColor);
			iIdxVia2 = iIdxVia4;
		}
		else
		{
			if (iIdxVia3 < m_nLayerListCount && iIdxVia != iIdxVia3)
			{
				m_LayerList.SetItemText(iIdxVia3, LIST_DEFINE_COLUMN, VIA2_LAYER_TXT);
				m_LayerList.SetItemColor(iIdxVia3, m_pSysSpec->sysLayer.vcLayer[(int)eRefLayer::EN_VIA2_LAYER]->layerColor);
				iIdxVia2 = iIdxVia3;
			}
		}
	}
	else if (iIdxVia2 < m_nLayerListCount || iIdxVia4 < m_nLayerListCount)
	{
		if (iIdxVia2 < m_nLayerListCount)
		{
			m_LayerList.SetItemText(iIdxVia2, LIST_DEFINE_COLUMN, VIA_LAYER_TXT);
			m_LayerList.SetItemColor(iIdxVia2, m_pSysSpec->sysLayer.vcLayer[(int)eRefLayer::EN_VIA_LAYER]->layerColor);
		}
		else if (iIdxVia4 < m_nLayerListCount)
		{
			m_LayerList.SetItemText(iIdxVia4, LIST_DEFINE_COLUMN, VIA_LAYER_TXT);
			m_LayerList.SetItemColor(iIdxVia4, m_pSysSpec->sysLayer.vcLayer[(int)eRefLayer::EN_VIA_LAYER]->layerColor);
			iIdxVia2 = iIdxVia4;
		}
	}

	if (iIdxVia < m_nLayerListCount && iIdxVia2 < m_nLayerListCount)
	{
		strVia = m_LayerList.GetItemText(iIdxVia, LIST_LAYER_COLUMN);
		strVia2 = m_LayerList.GetItemText(iIdxVia2, LIST_LAYER_COLUMN);
		strVia3.Format(L"%d", iLayerIdx);
		int idx1 = strVia.Find(strVia3);
		int idx2 = strVia2.Find(strVia3);

		bool bExcept = false;
		if (strExt.GetLength() > 0 && strVia.Find(strExt) != -1)
			bExcept = true;

		if (idx2 < idx1)
		{
			m_LayerList.SetItemText(iIdxVia, LIST_DEFINE_COLUMN, VIA2_LAYER_TXT);
			m_LayerList.SetItemText(iIdxVia2, LIST_DEFINE_COLUMN, VIA_LAYER_TXT);
		}
		else if (idx1 == idx2)
		{
			AfxExtractSubString(strVia3, strVia, 1, '-');
			strVia = strVia3.Left(1);
			idx1 = _ttoi(strVia);

			AfxExtractSubString(strVia3, strVia2, 1, '-');
			strVia2 = strVia3.Left(1);
			idx2 = _ttoi(strVia2);

			if (bIsOdd)
			{
				if (idx1 > idx2)
				{
					m_LayerList.SetItemText(iIdxVia, LIST_DEFINE_COLUMN, VIA2_LAYER_TXT);
					m_LayerList.SetItemText(iIdxVia2, LIST_DEFINE_COLUMN, VIA_LAYER_TXT);
				}
			}
			else
			{
				if (idx2 > idx1)
				{
					if (!bExcept)
					{
						if (iPatternCnt < 6 || iLayerIdx != iPatternCnt / 2 + 1)
						{
							m_LayerList.SetItemText(iIdxVia, LIST_DEFINE_COLUMN, VIA2_LAYER_TXT);
							m_LayerList.SetItemText(iIdxVia2, LIST_DEFINE_COLUMN, VIA_LAYER_TXT);
						}
					}
				}
			}
		}
	}

	if (iIdxDrl < m_nLayerListCount)
	{
		m_LayerList.SetItemText(iIdxDrl, LIST_DEFINE_COLUMN, DRILL_LAYER_TXT);
		m_LayerList.SetItemColor(iIdxDrl, m_pSysSpec->sysLayer.vcLayer[(int)eRefLayer::EN_DRILL_LAYER]->layerColor);

		if (iIdxDrl2 < m_nLayerListCount)
		{
			m_LayerList.SetItemText(iIdxDrl2, LIST_DEFINE_COLUMN, DRILL2_LAYER_TXT);
			m_LayerList.SetItemColor(iIdxDrl2, m_pSysSpec->sysLayer.vcLayer[(int)eRefLayer::EN_DRILL2_LAYER]->layerColor);
		}
	}
	else if (iIdxDrl2 < m_nLayerListCount)
	{
		m_LayerList.SetItemText(iIdxDrl2, LIST_DEFINE_COLUMN, DRILL_LAYER_TXT);
		m_LayerList.SetItemColor(iIdxDrl2, m_pSysSpec->sysLayer.vcLayer[(int)eRefLayer::EN_DRILL_LAYER]->layerColor);
	}

	if (!bFindSR && strSR.GetLength() > 0)
	{
		for (UINT32 iLoop = 0; iLoop < m_nLayerListCount; iLoop++)
		{
			strLayer = m_LayerList.GetItemText(iLoop, LIST_LAYER_COLUMN);
			if (strLayer.Find(strSR) != -1)
			{
				m_LayerList.SetItemText(iLoop, LIST_DEFINE_COLUMN, SR_LAYER_TXT);
				m_LayerList.SetItemColor(iLoop, m_pSysSpec->sysLayer.vcLayer[(int)eRefLayer::EN_SR_LAYER]->layerColor);
				bFindSR = true;
				break;
			}
		}
	}

	if (!bFindInsidePT && strInsidePT2.GetLength() > 0)
	{
		for (UINT32 iLoop = 0; iLoop < m_nLayerListCount; iLoop++)
		{
			strLayer = m_LayerList.GetItemText(iLoop, LIST_LAYER_COLUMN);

			if (strLayer.CompareNoCase(strInsidePT2) == 0)
			{
				m_LayerList.SetItemText(iLoop, LIST_DEFINE_COLUMN, INSIDE_PATTERN_LAYER_TXT);
				m_LayerList.SetItemColor(iLoop, m_pSysSpec->sysLayer.vcLayer[(int)eRefLayer::EN_INSIDE_PATTERN_LAYER]->layerColor);
				bFindInsidePT = true;
				break;
			}
		}
	}

	m_LayerList.EnableWindow(TRUE);
	m_LayerList.SetRedraw(TRUE);
	m_LayerList.Invalidate();

	//	if (bFindInsidePT)
	//		AfxMessageBox(L"Inside Pattern Exists");

	return RESULT_GOOD;
}

UINT32 CLayerInfoFormView::SetLayerInfo_for_DAEDUCK(CString strInspLayer)
{
	if (m_nStartLayerIdx < 0 || m_nEndLayerIdx < 0) return RESULT_BAD;
	if (m_nStartLayerIdx > m_nEndLayerIdx) return RESULT_BAD;

	int iPatternCnt = m_cbInspLayer.GetCount();
	if (iPatternCnt <= 0) return RESULT_BAD;

	int iSel = m_cbInspLayer.GetCurSel();
	if (CB_ERR == iSel) return RESULT_BAD;

	CString strNumber = L"0123456789";
	CString strLayer = L"", strDefine = L"";
	m_cbInspLayer.GetLBText(iSel, strLayer);

	// 내층 제외...외층만 고려
	CString strOnlyNumber = strLayer;
	CString strExcept = strOnlyNumber.SpanExcluding(strNumber);
	strOnlyNumber.Replace(strExcept, _T(""));
	strOnlyNumber.Trim();

	INT32 iInspLayerIdx = _ttoi(strOnlyNumber);
	if (iInspLayerIdx <= 0) return RESULT_BAD;

	CString strInner = L"";
	if (strLayer.Find(L"B") >= 0)
		strInner = L"B";

	// Layer Name 설정
	std::map < std::pair<CString, eRefLayer>, std::vector<CString> > hLayerMap;
	std::vector<CString> vcLayerName;

	// 1. Pattern
//	strLayer.Format(L"L%02d", iInspLayerIdx);
	vcLayerName.emplace_back(strInspLayer);
	hLayerMap.insert(std::make_pair(std::make_pair(PATTERN_LAYER_TXT, eRefLayer::EN_PATTERN_LAYER), vcLayerName));
	vcLayerName.clear();

	// 2. SR
	strLayer.Format(L"S%02d", iInspLayerIdx);
	vcLayerName.emplace_back(strLayer);
	hLayerMap.insert(std::make_pair(std::make_pair(SR_LAYER_TXT, eRefLayer::EN_SR_LAYER), vcLayerName));
	vcLayerName.clear();

	// 3. Via
	if (iInspLayerIdx == m_nStartLayerIdx)
	{
		strLayer.Format(L"C%02d%02d%s", iInspLayerIdx, iInspLayerIdx + 1, strInner);
		strLayer.Trim();
		vcLayerName.emplace_back(strLayer);

		strLayer.Format(L"C%02d%02d%s", iInspLayerIdx + 1, iInspLayerIdx, strInner);
		strLayer.Trim();
		vcLayerName.emplace_back(strLayer);
	}
	else
	{
		strLayer.Format(L"C%02d%02d%s", iInspLayerIdx, iInspLayerIdx - 1, strInner);
		strLayer.Trim();
		vcLayerName.emplace_back(strLayer);

		strLayer.Format(L"C%02d%02d%s", iInspLayerIdx - 1, iInspLayerIdx, strInner);
		strLayer.Trim();
		vcLayerName.emplace_back(strLayer);
	}
	hLayerMap.insert(std::make_pair(std::make_pair(VIA_LAYER_TXT, eRefLayer::EN_VIA_LAYER), vcLayerName));
	vcLayerName.clear();

	// 4. Via (추가)
	if (iInspLayerIdx == m_nStartLayerIdx)
	{
		strLayer.Format(L"Y%02d%02d%s", iInspLayerIdx, iInspLayerIdx + 1, strInner);
		strLayer.Trim();
		vcLayerName.emplace_back(strLayer);

		strLayer.Format(L"Y%02d%02d%s", iInspLayerIdx + 1, iInspLayerIdx, strInner);
		strLayer.Trim();
		vcLayerName.emplace_back(strLayer);
	}
	else
	{
		strLayer.Format(L"Y%02d%02d%s", iInspLayerIdx, iInspLayerIdx - 1, strInner);
		strLayer.Trim();
		vcLayerName.emplace_back(strLayer);

		strLayer.Format(L"Y%02d%02d%s", iInspLayerIdx - 1, iInspLayerIdx, strInner);
		strLayer.Trim();
		vcLayerName.emplace_back(strLayer);
	}
	hLayerMap.insert(std::make_pair(std::make_pair(VIA2_LAYER_TXT, eRefLayer::EN_VIA2_LAYER), vcLayerName));
	vcLayerName.clear();

	// 5. Drill (전층)
	strLayer.Format(L"D%02d%02d", m_nStartLayerIdx, m_nEndLayerIdx);
	vcLayerName.emplace_back(strLayer);
	strLayer.Format(L"D%02d%02d-S", m_nStartLayerIdx, m_nEndLayerIdx);
	vcLayerName.emplace_back(strLayer);
	hLayerMap.insert(std::make_pair(std::make_pair(DRILL_LAYER_TXT, eRefLayer::EN_DRILL_LAYER), vcLayerName));
	vcLayerName.clear();

	// 6. Drill (그 외)
	if (iInspLayerIdx == m_nStartLayerIdx)
	{
		for (int i = m_nStartLayerIdx + 1; i < m_nEndLayerIdx; i++)
		{
			strLayer.Format(L"D%02d%02d", m_nStartLayerIdx, i);
			vcLayerName.emplace_back(strLayer);
		}
	}
	else
	{
		for (int i = m_nStartLayerIdx + 1; i < m_nEndLayerIdx; i++)
		{
			strLayer.Format(L"D%02d%02d", i, m_nEndLayerIdx);
			vcLayerName.emplace_back(strLayer);
		}
	}
	hLayerMap.insert(std::make_pair(std::make_pair(DRILL2_LAYER_TXT, eRefLayer::EN_DRILL2_LAYER), vcLayerName));
	vcLayerName.clear();

	// 7. Dam
	strLayer.Format(L"S%02d-2", iInspLayerIdx);
	vcLayerName.emplace_back(strLayer);
	hLayerMap.insert(std::make_pair(std::make_pair(SR_DAM_LAYER_TXT, eRefLayer::EN_SR_DAM_LAYER), vcLayerName));
	vcLayerName.clear();

	// 8. Au
	strLayer.Format(L"%s-2SOFT", strInspLayer);
	vcLayerName.emplace_back(strLayer);
	hLayerMap.insert(std::make_pair(std::make_pair(AU_LAYER_TXT, eRefLayer::EN_AU_LAYER), vcLayerName));
	vcLayerName.clear();

	// 9. EtchBack
	strLayer.Format(L"%s-2ETCH", strInspLayer);
	vcLayerName.emplace_back(strLayer);
	strLayer.Format(L"%s-3ETCH", strInspLayer);
	vcLayerName.emplace_back(strLayer);
	hLayerMap.insert(std::make_pair(std::make_pair(ETCH_LAYER_TXT, eRefLayer::EN_ETCH_LAYER), vcLayerName));
	vcLayerName.clear();	
	
	// Layer List 설정
	m_LayerList.SetRedraw(FALSE);
	m_LayerList.EnableWindow(FALSE);

	std::map< CString, std::pair<bool, std::vector<UINT32>> > hCompletedMap;
	for (auto itDefine : hLayerMap)
	{
		std::vector<UINT32> vcIndex;
		hCompletedMap.insert(std::make_pair(itDefine.first.first, std::make_pair(false, vcIndex)));
	}

	for (UINT32 iLoop = 0; iLoop < m_nLayerListCount; iLoop++)
	{
		m_LayerList.SetItemText(iLoop, LIST_DEFINE_COLUMN, NULL_LAYER_TXT);
		strLayer = m_LayerList.GetItemText(iLoop, LIST_LAYER_COLUMN);

		for (auto itDefine : hLayerMap)
		{
			strDefine = itDefine.first.first;

			auto itDone = hCompletedMap.find(strDefine);
			if (itDone == hCompletedMap.end())
				continue;

			eRefLayer layerIndex = itDefine.first.second;

			for (auto itLayer : itDefine.second)
			{
				if (strLayer.CompareNoCase(itLayer) != 0)
					continue;

				m_LayerList.SetItemText(iLoop, LIST_DEFINE_COLUMN, strDefine);
				m_LayerList.SetItemColor(iLoop, m_pSysSpec->sysLayer.vcLayer[(int)layerIndex]->layerColor);

				itDone->second.first = true;
				itDone->second.second.emplace_back(iLoop);
			}
		}
	}

	// Verify
	for (auto it : hCompletedMap)
	{
		if (!it.second.first) continue;
		if (it.second.second.size() <= 1) continue;

		strDefine = it.first;
		if (strDefine.CompareNoCase(VIA_LAYER_TXT) == 0)
		{
			bool bIsSubSet = true;

			auto Main = hLayerMap.find(std::make_pair(VIA_LAYER_TXT, eRefLayer::EN_VIA_LAYER));
			if (Main == hLayerMap.end()) continue;
			
			auto Sub = hCompletedMap.find(VIA2_LAYER_TXT);
			if (Sub != hCompletedMap.end() && !Sub->second.first)
				bIsSubSet = false;

			for (auto itLayer : it.second.second)
			{
				strLayer = m_LayerList.GetItemText(itLayer, LIST_LAYER_COLUMN);
				if (strLayer.CompareNoCase(Main->second.at(0)) != 0)
				{
					if (bIsSubSet)
						m_LayerList.SetItemText(itLayer, LIST_DEFINE_COLUMN, NULL_LAYER_TXT);
					else
					{
						m_LayerList.SetItemText(itLayer, LIST_DEFINE_COLUMN, VIA2_LAYER_TXT);
						m_LayerList.SetItemColor(itLayer, m_pSysSpec->sysLayer.vcLayer[(int)eRefLayer::EN_VIA2_LAYER]->layerColor);

						Sub->second.first = true;
						Sub->second.second.emplace_back(itLayer);
					}
				}
			}
		}

		if (strDefine.CompareNoCase(VIA2_LAYER_TXT) == 0)
		{
			auto Main = hLayerMap.find(std::make_pair(VIA2_LAYER_TXT, eRefLayer::EN_VIA2_LAYER));
			if (Main == hLayerMap.end()) continue;

			INT32 iMainIdx = -1;
			for (auto itLayer : it.second.second)
			{
				strLayer = m_LayerList.GetItemText(itLayer, LIST_LAYER_COLUMN);
				if (strLayer.CompareNoCase(Main->second.at(0)) == 0)
					iMainIdx = itLayer;
				else 				
					m_LayerList.SetItemText(itLayer, LIST_DEFINE_COLUMN, NULL_LAYER_TXT);
				
			}

			for (auto itLayer = it.second.second.begin(); itLayer != it.second.second.end(); )
			{
				if (*itLayer == iMainIdx) itLayer++;
				else
					itLayer = it.second.second.erase(itLayer);
			}
		}
	}

	hCompletedMap.clear();
	hLayerMap.clear();

	CString strError = L"";
	for (auto it : hCompletedMap)
	{
		if (!it.second.first) continue;
		if (it.second.second.size() <= 1) continue;

		strError.Format(L"[%s Layer] 설정된 레이어 개수가 1개 이상입니다.", it.first);
		AfxMessageBox(strError);
	}

	m_LayerList.EnableWindow(TRUE);
	m_LayerList.SetRedraw(TRUE);
	m_LayerList.Invalidate();

	return RESULT_GOOD;
}

UINT32 CLayerInfoFormView::SetLayerInfo_for_DAEDUCK_AVI(CString strInspLayer)
{
	if (m_nStartLayerIdx < 0 || m_nEndLayerIdx < 0) return RESULT_BAD;
	if (m_nStartLayerIdx > m_nEndLayerIdx) return RESULT_BAD;

	int iPatternCnt = m_cbInspLayer.GetCount();
	if (iPatternCnt <= 0) return RESULT_BAD;

	int iSel = m_cbInspLayer.GetCurSel();
	if (CB_ERR == iSel) return RESULT_BAD;

	CString strNumber = L"0123456789";
	CString strLayer = L"", strDefine = L"";
	m_cbInspLayer.GetLBText(iSel, strLayer);

	// 내층 제외...외층만 고려
	CString strOnlyNumber = strLayer;
	CString strExcept = strOnlyNumber.SpanExcluding(strNumber);
	strOnlyNumber.Replace(strExcept, _T(""));
	strOnlyNumber.Trim();

	INT32 iInspLayerIdx = _ttoi(strOnlyNumber);
	if (iInspLayerIdx <= 0) return RESULT_BAD;

	CString strInner = L"";
	if (strLayer.Find(L"B") >= 0)
		strInner = L"B";

	// Layer Name 설정
	std::map < std::pair<CString, eRefLayer>, std::vector<CString> > hLayerMap;
	std::vector<CString> vcLayerName;

	// 1. Pattern
//	strLayer.Format(L"L%02d", iInspLayerIdx);
	vcLayerName.emplace_back(strInspLayer);
	hLayerMap.insert(std::make_pair(std::make_pair(PATTERN_LAYER_TXT, eRefLayer::EN_PATTERN_LAYER), vcLayerName));
	vcLayerName.clear();

	// 2. SR
	strLayer.Format(L"S%02d", iInspLayerIdx);
	vcLayerName.emplace_back(strLayer);
	hLayerMap.insert(std::make_pair(std::make_pair(SR_LAYER_TXT, eRefLayer::EN_SR_LAYER), vcLayerName));
	vcLayerName.clear();
// 
// 	// 3. Via
// 	if (iInspLayerIdx == m_nStartLayerIdx)
// 	{
// 		strLayer.Format(L"C%02d%02d%s", iInspLayerIdx, iInspLayerIdx + 1, strInner);
// 		strLayer.Trim();
// 		vcLayerName.emplace_back(strLayer);
// 
// 		strLayer.Format(L"C%02d%02d%s", iInspLayerIdx + 1, iInspLayerIdx, strInner);
// 		strLayer.Trim();
// 		vcLayerName.emplace_back(strLayer);
// 	}
// 	else
// 	{
// 		strLayer.Format(L"C%02d%02d%s", iInspLayerIdx, iInspLayerIdx - 1, strInner);
// 		strLayer.Trim();
// 		vcLayerName.emplace_back(strLayer);
// 
// 		strLayer.Format(L"C%02d%02d%s", iInspLayerIdx - 1, iInspLayerIdx, strInner);
// 		strLayer.Trim();
// 		vcLayerName.emplace_back(strLayer);
// 	}
// 	hLayerMap.insert(std::make_pair(std::make_pair(VIA_LAYER_TXT, eRefLayer::EN_VIA_LAYER), vcLayerName));
// 	vcLayerName.clear();
// 
// 	// 4. Via (추가)
// 	if (iInspLayerIdx == m_nStartLayerIdx)
// 	{
// 		strLayer.Format(L"Y%02d%02d%s", iInspLayerIdx, iInspLayerIdx + 1, strInner);
// 		strLayer.Trim();
// 		vcLayerName.emplace_back(strLayer);
// 
// 		strLayer.Format(L"Y%02d%02d%s", iInspLayerIdx + 1, iInspLayerIdx, strInner);
// 		strLayer.Trim();
// 		vcLayerName.emplace_back(strLayer);
// 	}
// 	else
// 	{
// 		strLayer.Format(L"Y%02d%02d%s", iInspLayerIdx, iInspLayerIdx - 1, strInner);
// 		strLayer.Trim();
// 		vcLayerName.emplace_back(strLayer);
// 
// 		strLayer.Format(L"Y%02d%02d%s", iInspLayerIdx - 1, iInspLayerIdx, strInner);
// 		strLayer.Trim();
// 		vcLayerName.emplace_back(strLayer);
// 	}
// 	hLayerMap.insert(std::make_pair(std::make_pair(VIA2_LAYER_TXT, eRefLayer::EN_VIA2_LAYER), vcLayerName));
// 	vcLayerName.clear();

	// 5. Drill (전층)
	strLayer.Format(L"D%02d%02d", m_nStartLayerIdx, m_nEndLayerIdx);
	vcLayerName.emplace_back(strLayer);
	strLayer.Format(L"D%02d%02d-S", m_nStartLayerIdx, m_nEndLayerIdx);
	vcLayerName.emplace_back(strLayer);
	hLayerMap.insert(std::make_pair(std::make_pair(DRILL_LAYER_TXT, eRefLayer::EN_DRILL_LAYER), vcLayerName));
	vcLayerName.clear();

	// 6. Drill (그 외)
	if (iInspLayerIdx == m_nStartLayerIdx)
	{
		for (int i = m_nStartLayerIdx + 1; i < m_nEndLayerIdx; i++)
		{
			strLayer.Format(L"D%02d%02d", m_nStartLayerIdx, i);
			vcLayerName.emplace_back(strLayer);
		}
	}
	else
	{
		for (int i = m_nStartLayerIdx + 1; i < m_nEndLayerIdx; i++)
		{
			strLayer.Format(L"D%02d%02d", i, m_nEndLayerIdx);
			vcLayerName.emplace_back(strLayer);
		}
	}
	hLayerMap.insert(std::make_pair(std::make_pair(DRILL2_LAYER_TXT, eRefLayer::EN_DRILL2_LAYER), vcLayerName));
	vcLayerName.clear();
// 
// 	// 7. Dam
// 	strLayer.Format(L"S%02d-2", iInspLayerIdx);
// 	vcLayerName.emplace_back(strLayer);
// 	hLayerMap.insert(std::make_pair(std::make_pair(SR_DAM_LAYER_TXT, eRefLayer::EN_SR_DAM_LAYER), vcLayerName));
// 	vcLayerName.clear();
// 
// 	// 8. Au
// 	strLayer.Format(L"%s-2SOFT", strInspLayer);
// 	vcLayerName.emplace_back(strLayer);
// 	hLayerMap.insert(std::make_pair(std::make_pair(AU_LAYER_TXT, eRefLayer::EN_AU_LAYER), vcLayerName));
// 	vcLayerName.clear();
// 
// 	// 9. EtchBack
// 	strLayer.Format(L"%s-2ETCH", strInspLayer);
// 	vcLayerName.emplace_back(strLayer);
// 	strLayer.Format(L"%s-3ETCH", strInspLayer);
// 	vcLayerName.emplace_back(strLayer);
// 	hLayerMap.insert(std::make_pair(std::make_pair(ETCH_LAYER_TXT, eRefLayer::EN_ETCH_LAYER), vcLayerName));
// 	vcLayerName.clear();

	// Layer List 설정
	m_LayerList.SetRedraw(FALSE);
	m_LayerList.EnableWindow(FALSE);

	std::map< CString, std::pair<bool, std::vector<UINT32>> > hCompletedMap;
	for (auto itDefine : hLayerMap)
	{
		std::vector<UINT32> vcIndex;
		hCompletedMap.insert(std::make_pair(itDefine.first.first, std::make_pair(false, vcIndex)));
	}

	for (UINT32 iLoop = 0; iLoop < m_nLayerListCount; iLoop++)
	{
		m_LayerList.SetItemText(iLoop, LIST_DEFINE_COLUMN, NULL_LAYER_TXT);
		strLayer = m_LayerList.GetItemText(iLoop, LIST_LAYER_COLUMN);

		for (auto itDefine : hLayerMap)
		{
			strDefine = itDefine.first.first;

			auto itDone = hCompletedMap.find(strDefine);
			if (itDone == hCompletedMap.end())
				continue;

			eRefLayer layerIndex = itDefine.first.second;

			for (auto itLayer : itDefine.second)
			{
				if (strLayer.CompareNoCase(itLayer) != 0)
					continue;

				m_LayerList.SetItemText(iLoop, LIST_DEFINE_COLUMN, strDefine);
				m_LayerList.SetItemColor(iLoop, m_pSysSpec->sysLayer.vcLayer[(int)layerIndex]->layerColor);

				itDone->second.first = true;
				itDone->second.second.emplace_back(iLoop);
			}
		}
	}

	// Verify
	for (auto it : hCompletedMap)
	{
		if (!it.second.first) continue;
		if (it.second.second.size() <= 1) continue;

		strDefine = it.first;
		if (strDefine.CompareNoCase(VIA_LAYER_TXT) == 0)
		{
			bool bIsSubSet = true;

			auto Main = hLayerMap.find(std::make_pair(VIA_LAYER_TXT, eRefLayer::EN_VIA_LAYER));
			if (Main == hLayerMap.end()) continue;

			auto Sub = hCompletedMap.find(VIA2_LAYER_TXT);
			if (Sub != hCompletedMap.end() && !Sub->second.first)
				bIsSubSet = false;

			for (auto itLayer : it.second.second)
			{
				strLayer = m_LayerList.GetItemText(itLayer, LIST_LAYER_COLUMN);
				if (strLayer.CompareNoCase(Main->second.at(0)) != 0)
				{
					if (bIsSubSet)
						m_LayerList.SetItemText(itLayer, LIST_DEFINE_COLUMN, NULL_LAYER_TXT);
					else
					{
						m_LayerList.SetItemText(itLayer, LIST_DEFINE_COLUMN, VIA2_LAYER_TXT);
						m_LayerList.SetItemColor(itLayer, m_pSysSpec->sysLayer.vcLayer[(int)eRefLayer::EN_VIA2_LAYER]->layerColor);

						Sub->second.first = true;
						Sub->second.second.emplace_back(itLayer);
					}
				}
			}
		}

		if (strDefine.CompareNoCase(VIA2_LAYER_TXT) == 0)
		{
			auto Main = hLayerMap.find(std::make_pair(VIA2_LAYER_TXT, eRefLayer::EN_VIA2_LAYER));
			if (Main == hLayerMap.end()) continue;

			INT32 iMainIdx = -1;
			for (auto itLayer : it.second.second)
			{
				strLayer = m_LayerList.GetItemText(itLayer, LIST_LAYER_COLUMN);
				if (strLayer.CompareNoCase(Main->second.at(0)) == 0)
					iMainIdx = itLayer;
				else
					m_LayerList.SetItemText(itLayer, LIST_DEFINE_COLUMN, NULL_LAYER_TXT);

			}

			for (auto itLayer = it.second.second.begin(); itLayer != it.second.second.end(); )
			{
				if (*itLayer == iMainIdx) itLayer++;
				else
					itLayer = it.second.second.erase(itLayer);
			}
		}
	}

	hCompletedMap.clear();
	hLayerMap.clear();

	CString strError = L"";
	for (auto it : hCompletedMap)
	{
		if (!it.second.first) continue;
		if (it.second.second.size() <= 1) continue;

		strError.Format(L"[%s Layer] 설정된 레이어 개수가 1개 이상입니다.", it.first);
		AfxMessageBox(strError);
	}

	m_LayerList.EnableWindow(TRUE);
	m_LayerList.SetRedraw(TRUE);
	m_LayerList.Invalidate();

	return RESULT_GOOD;
}

UINT32 CLayerInfoFormView::DisplayStep_for_AVI(CString strJobPath, std::vector<stStepInfo*>* vcStepInfo)
{
	if (m_pSysSpec == nullptr || m_pstUserLayerSet == nullptr)
		return RESULT_BAD;

	CFileFind file;
	CString strFileName = L"", strTemp = L"";
	std::vector<CString> vcUnitSteps;

	UINT32 iStepNum = static_cast<UINT32>(vcStepInfo->size());

	//1. Step	
	m_pstUserLayerSet->ClearStep();
	for (UINT32 iLoop = 0; iLoop < iStepNum; iLoop++)
	{
		stStepInfo* pInfo = vcStepInfo->at(iLoop);
		if (pInfo == nullptr)
			return RESULT_BAD;

		strFileName.Format(L"%s\\steps\\%s\\profile", strJobPath, pInfo->strName);
		if (!file.FindFile(strFileName))
			continue;

		m_StepList.InsertItem(iLoop, NULL_STRING);
		m_StepList.SetItemText(iLoop, LIST_STEP_NAME_COL, pInfo->strName);

		//m_StepList.SetItemTextColor(iLoop, LIST_STEP_DEFINE_COL, MAGENTA_COLOR);
		m_StepList.SetItemTextColor(iLoop, LIST_STEP_DEFINE_COL, PURPLE_COLOR);

		for (auto it : m_pSysSpec->sysStep.vcPanelNames)
		{
			// 2023.01.16 KJH ADD
			// System InI : PANEL, 실제 ODB Layer PANEL.XXXXXX 인경우 CmpareNoCase로 조건문으로 구분이 안되 Find 로 변경 
			//if (it.CompareNoCase(pInfo->strName) == 0)
			if (pInfo->strName.Find(it) == 0)
			{
				m_StepList.SetItemText(iLoop, LIST_STEP_DEFINE_COL, it);
				m_pstUserLayerSet->nPanelStepIdx = iLoop;
			}
		}

		for (auto it : m_pSysSpec->sysStep.vcStripNames)
		{
			if (pInfo->strName.Find(it) == 0 && pInfo->strName.Find(L"VM") == -1 &&
				pInfo->strName.Find(L"COUPON") == -1)
			{
				// 2023.01.17 KJH ADD
				//STRIP, PCB 일 경우 Define STRIP 로 Fix
				//m_StepList.SetItemText(iLoop, LIST_STEP_DEFINE_COL, it);
				m_StepList.SetItemText(iLoop, LIST_STEP_DEFINE_COL, m_pSysSpec->sysStep.vcStripNames[0]);
				m_pstUserLayerSet->vcStripStepIdx.emplace_back(iLoop);
			}
		}

		for (auto it : m_pSysSpec->sysStep.vcUnitNames)
		{
			if (pInfo->strName.Find(it) == 0 && pInfo->strName.Find(L"VM") == -1 &&
				pInfo->strName.Find(L"COUPON") == -1)
			{
				m_StepList.SetItemText(iLoop, LIST_STEP_DEFINE_COL, it);
				m_pstUserLayerSet->vcUnitStepIdx.emplace_back(iLoop);

				AfxExtractSubString(strTemp, pInfo->strName, 1, '-');
				if (strTemp.GetLength() > 0)
					vcUnitSteps.emplace_back(pInfo->strName);
			}
			else if (pInfo->strName.CompareNoCase(L"ALIGN") == 0)
			{
				m_StepList.SetItemText(iLoop, LIST_STEP_DEFINE_COL, it);
				m_pstUserLayerSet->vcUnitStepIdx.emplace_back(iLoop);
			}
		}

		m_nStepListCount++;
	}

	for (auto itStep : m_pSysSpec->sysStep.vcUnitNames)
	{
		for (auto it : vcUnitSteps)
		{
			CString strUnitStep = it;
			strUnitStep.Replace(L"UNIT", L"ALIGN");

			for (UINT32 iLoop = 0; iLoop < m_nStepListCount; iLoop++)
			{
				CString strName = m_StepList.GetItemText(iLoop, LIST_STEP_NAME_COL);

				if (strName.CompareNoCase(strUnitStep) == 0)
				{
					m_StepList.SetItemText(iLoop, LIST_STEP_DEFINE_COL, itStep);
					m_pstUserLayerSet->vcUnitStepIdx.emplace_back(iLoop);
				}
			}
		}
	}

	vcUnitSteps.clear();

	if (m_pstUserLayerSet->vcUnitStepIdx.size() <= 0)
	{
		for (auto itStrip : m_pstUserLayerSet->vcStripStepIdx)
		{
			stStepInfo* pInfo = vcStepInfo->at(itStrip);
			if (pInfo == nullptr)
				return RESULT_BAD;

			for (auto itSub : pInfo->vcStepRepeat)
			{
				if ((itSub->iNx * itSub->iNy) <= 1)
					continue;

				auto itStep = find(vcUnitSteps.begin(), vcUnitSteps.end(), itSub->strName);
				if (itStep == vcUnitSteps.end())
					vcUnitSteps.emplace_back(itSub->strName);
			}
		}

		for (auto it : vcUnitSteps)
		{
			for (UINT32 iLoop = 0; iLoop < m_nStepListCount; iLoop++)
			{
				CString strName = m_StepList.GetItemText(iLoop, LIST_STEP_NAME_COL);

				if (strName.CompareNoCase(it) == 0)
				{
					m_StepList.SetItemText(iLoop, LIST_STEP_DEFINE_COL, it);
					m_pstUserLayerSet->vcUnitStepIdx.emplace_back(iLoop);
				}
			}
		}
	}

	if (m_pstUserLayerSet->nPanelStepIdx >= 0 &&
		(m_pstUserLayerSet->vcStripStepIdx.size() >= 0 || m_pstUserLayerSet->vcUnitStepIdx.size() >= 0))
		m_bIsStepSet = TRUE;

	return RESULT_GOOD;
}

UINT32 CLayerInfoFormView::DisplayLayer_for_AVI(CString strJobPath, std::vector<stStepInfo*>* vcStepInfo, std::vector<stLayerInfo*>* vcLayerInfo,
	INT32 iFrontlayerIdx, INT32 iBacklayerIdx)
{
	if (m_pSysSpec == nullptr || m_pstUserLayerSet == nullptr)
		return RESULT_BAD;

	UINT32 iStepNum = static_cast<UINT32>(vcStepInfo->size());
	UINT32 iLayerNum = static_cast<UINT32>(vcLayerInfo->size());
	
	CFileFind file;
	CString strFileName = L"";
	CString strNumber = L"0123456789";

	//2. Layer
	UINT32 iColorIdx = 0, iLayerCount = 0;
	for (UINT32 iLoop = 0; iLoop < iLayerNum; iLoop++)
	{
		stLayerInfo* pInfo = vcLayerInfo->at(iLoop);
		if (pInfo == nullptr)
		{
			m_bIsDone = TRUE;
			return RESULT_BAD;
		}

		iLayerCount = 0;
		for (UINT32 iStep = 0; iStep < iStepNum; iStep++)
		{
			stStepInfo* pStepInfo = vcStepInfo->at(iStep);
			if (pInfo == nullptr)
			{
				m_bIsDone = TRUE;
				return RESULT_BAD;
			}

			strFileName.Format(L"%s\\steps\\%s\\layers\\%s\\features", strJobPath, pStepInfo->strName, pInfo->strName);
			if (file.FindFile(strFileName))
				iLayerCount++;
		}

		if (iLayerCount != iStepNum)
			continue;

		//Pattern Layer : BOARD & SIGNAL
		if (pInfo->strContext.CompareNoCase(L"BOARD") == 0 &&
			pInfo->strType.CompareNoCase(L"SIGNAL") == 0)
		{
			// 내층 제외...외층만 고려
			CString strOnlyNumber = pInfo->strName;
			CString strExcept = strOnlyNumber.SpanExcluding(strNumber);
			strOnlyNumber.Replace(strExcept, _T(""));
			strOnlyNumber.Trim();

			INT32 iLayerIndex = _ttoi(strOnlyNumber);
			
			if (iLayerIndex == iFrontlayerIdx || iLayerIndex == iBacklayerIdx)
				m_cbInspLayer.AddString(pInfo->strName);
		}

		m_LayerList.InsertItem(m_nLayerListCount, NULL_STRING);
		m_LayerList.SetCheck(m_nLayerListCount, FALSE);
		m_LayerList.SetItemText(m_nLayerListCount, LIST_LAYER_COLUMN, pInfo->strName);
		m_LayerList.SetItemTextColor(m_nLayerListCount, LIST_DEFINE_COLUMN, BLUE_COLOR);
		m_LayerList.SetItemColor(m_nLayerListCount, RGB(DEFCOLOR_R(iColorIdx), DEFCOLOR_G(iColorIdx), DEFCOLOR_B(iColorIdx)));

		iColorIdx++;
		m_nLayerListCount++;
	}

	return RESULT_GOOD;
}

UINT32 CLayerInfoFormView::GetHisto_PatternWidth(IN const int &eLineOption, std::map<INT32, std::vector<CString>> &vcHisto_Pos, std::map<INT32, std::vector<CString>> &vcHisto_Neg)
{
	ClearPTWidthHisto();

	vcHisto_Pos.clear();
	vcHisto_Neg.clear();

	//Pattern Layer Line Feature Only : 선폭
	CString strLayerName = _T("");
	INT32 iLayerIdx = -1;
	for (UINT32 iLoop = 0; iLoop < m_nLayerListCount; iLoop++)
	{
		LayerSet_t *pLayer = m_pstUserLayerSet->vcLayerSet[iLoop];
		if (pLayer == nullptr) continue;

		if (pLayer->strDefine.CompareNoCase(PATTERN_LAYER_TXT) == 0)
		{
			strLayerName = pLayer->strLayer;
			iLayerIdx = static_cast<INT32>(iLoop);
			break;
		}
	}

	if (iLayerIdx < 0 || strLayerName.GetLength() < 1)
		return RESULT_BAD;

	CJobFile* pJobFile;
	IArchive_GetJobFile((void**)&pJobFile);
	if (pJobFile == nullptr) return RESULT_BAD;

	CHistogram		histoPTWidth;

	int nStepCount = static_cast<int>(pJobFile->m_arrStep.GetCount());
	for (int nStep = 0; nStep < nStepCount; nStep++)
	{
		CStep *pStep = pJobFile->m_arrStep.GetAt(nStep);
		if (pStep == nullptr) return RESULT_BAD;

		INT_PTR nSize = pStep->m_arrLayer.GetSize();
		if (nSize <= 0)
		{
			AfxMessageBox(_T("먼저 검사 레이어 추출을 진행하여 주세요."));
			return RESULT_BAD;
		}

		int nLayerIndex = _GetLayerIndex(pStep->m_arrLayer, strLayerName);
		if (nLayerIndex < 0 || nLayerIndex >= nSize) return RESULT_BAD;	

		for (int j = 0; j < nSize; j++)
		{
			CLayer* pLayer = pStep->m_arrLayer.GetAt(j);
			if (pLayer == nullptr) continue;

			if (pLayer->m_strLayerName != m_pstUserLayerSet->vcLayerSet[iLayerIdx]->strLayer)
				continue;

			histoPTWidth.SetUpdateUI(FALSE);
			histoPTWidth.SetHistoType(DEF_HISTOGRAM::Type_Feature);

			histoPTWidth.SetStepName(pStep->m_strStepName);
			histoPTWidth.SetLayerName(pLayer->m_strLayerName);
			switch (eLineOption)
			{
			case eLineWidthOption::eLinePos:
			case eLineWidthOption::eLineNeg:
				histoPTWidth.SetLayerPtr(pLayer, eDrawOption::eLineOnly);
				break;
			case eLineWidthOption::eArcPos:
			case eLineWidthOption::eArcNeg:
				histoPTWidth.SetLayerPtr(pLayer, eDrawOption::eArcOnly);
				break;
			case eLineWidthOption::eLineArcPos:
			case eLineWidthOption::eLineArcNeg:
				histoPTWidth.SetLayerPtr(pLayer, eDrawOption::eLineArcOnly);
				break;
			default:
				break;
			}

			break;
		}

		vector<FeatureHistoData>* pvcFeatureHisto = histoPTWidth.GetFeatureHisto();
		if (pvcFeatureHisto->size() <= 0)
			return RESULT_BAD;

		vector<FeatureHistoData*> vcLineHisto;
		vector<FeatureHistoData*> vcArcHisto;
		for (auto it : *pvcFeatureHisto)
		{
			switch (eLineOption)
			{
			case eLineWidthOption::eLinePos:
			case eLineWidthOption::eLineNeg:
				if (it.strTypeName.CompareNoCase(L"Line") != 0)
					continue;
				break;
			case eLineWidthOption::eArcPos:
			case eLineWidthOption::eArcNeg:
				if (it.strTypeName.CompareNoCase(L"Arc") != 0)
					continue;
				break;
			case eLineWidthOption::eLineArcPos:
			case eLineWidthOption::eLineArcNeg:
				break;
			default:
				continue;
			}

			if (it.strTypeName == _T("Line"))
			{
				FeatureHistoData* pHisto = new FeatureHistoData;
				*pHisto = it;

				vcLineHisto.emplace_back(pHisto);
			}
			else if (it.strTypeName == _T("Arc"))
			{
				FeatureHistoData* pHisto = new FeatureHistoData;
				*pHisto = it;

				vcArcHisto.emplace_back(pHisto);
			}
		}
		m_vcHistoPTWidth_Line.insert(std::make_pair(nStep, vcLineHisto));
		m_vcHistoPTWidth_Arc.insert(std::make_pair(nStep, vcArcHisto));
	}
	
	if (m_vcHistoPTWidth_Line.size() != nStepCount)
	{
		ClearPTWidthHisto();
		return RESULT_BAD;
	}
	if (m_vcHistoPTWidth_Arc.size() != nStepCount)
	{
		ClearPTWidthHisto();
		return RESULT_BAD;
	}
	
	return Estimate_Pattern_Width(eLineOption, vcHisto_Pos, vcHisto_Neg);
}

UINT32 CLayerInfoFormView::Estimate_Pattern_Width(IN const int &eLineOption, std::map<INT32, std::vector<CString>> &vcHisto_Pos, std::map<INT32, std::vector<CString>> &vcHisto_Neg)
{
	INT32 iStepIdx = GetCurStepIndex();
	if (iStepIdx != m_pstUserLayerSet->nPanelStepIdx)
		iStepIdx = m_pstUserLayerSet->nPanelStepIdx;

	CJobFile* pJobFile;
	IArchive_GetJobFile((void**)&pJobFile);
	if (pJobFile == nullptr) return RESULT_BAD;

	CStep *pStep = pJobFile->m_arrStep[iStepIdx];
	if (pStep == nullptr)
		return RESULT_BAD;

	std::vector<INT32> vcStepExist;
	vcStepExist.emplace_back(static_cast<INT32>(iStepIdx));

	for (UINT i = 0; i < pStep->m_nSubStepCnt; i++)
	{
		CSubStep* pSubStep = pStep->m_arrSubStep[i];
		if (pSubStep == nullptr) continue;

		Check_If_SubStep_Exist(pSubStep, vcStepExist);
	}

	if (vcStepExist.size() <= 0)
		return RESULT_BAD;

	CString strPenWidth;
	std::vector<CString> vcPos, vcNeg;
	// 판넬 기준, 비 포함 스텝의 데이터는 미 참조 필요
	// 선폭 기준, 중복 선폭 제거해서 출력 필요 (r,s 구분 의미 없음)	
	
	int nStepCount = static_cast<int>(m_vcHistoPTWidth_Line.size());
	for (int nStep = 0; nStep < nStepCount; nStep++)
	{
		auto it = std::find(vcStepExist.begin(), vcStepExist.end(), nStep);
		if (it == vcStepExist.end())
			continue;

		auto itStep = m_vcHistoPTWidth_Line.find(nStep);
		if (itStep == m_vcHistoPTWidth_Line.end()) continue;
		
		for (auto itLine : itStep->second)
		{
			for (auto itFeature : itLine->vecHistoData)
			{
				if (itFeature.bPositive)
				{
					//r, s 제거
					CString strPenWidth_mm = itFeature.strName.Mid(1);

					auto itWidth = std::find(vcPos.begin(), vcPos.end(), strPenWidth_mm);
					if (itWidth == vcPos.end())
						vcPos.emplace_back(strPenWidth_mm);
				}
				else
				{
					CString strPenWidth_mm = itFeature.strName.Mid(1);

					auto itWidth = std::find(vcNeg.begin(), vcNeg.end(), strPenWidth_mm);
					if (itWidth == vcNeg.end())
						vcNeg.emplace_back(strPenWidth_mm);
				}
			}
		}

		itStep = m_vcHistoPTWidth_Arc.find(nStep);
		if (itStep == m_vcHistoPTWidth_Arc.end()) continue;

		for (auto itLine : itStep->second)
		{
			for (auto itFeature : itLine->vecHistoData)
			{
				if (itFeature.bPositive)
				{
					//r, s 제거
					CString strPenWidth_mm = itFeature.strName.Mid(1);

					auto itWidth = std::find(vcPos.begin(), vcPos.end(), strPenWidth_mm);
					if (itWidth == vcPos.end())
						vcPos.emplace_back(strPenWidth_mm);
				}
				else
				{
					CString strPenWidth_mm = itFeature.strName.Mid(1);

					auto itWidth = std::find(vcNeg.begin(), vcNeg.end(), strPenWidth_mm);
					if (itWidth == vcNeg.end())
						vcNeg.emplace_back(strPenWidth_mm);
				}
			}
		}

		//
		std::sort(vcPos.begin(), vcPos.end());
		vcHisto_Pos.insert(std::make_pair(nStep, vcPos));

		std::sort(vcNeg.begin(), vcNeg.end());
		vcHisto_Neg.insert(std::make_pair(nStep, vcNeg));
	}
// 
// 	for (auto itStep : m_vcHistoPTWidth_Line)
// 	{
// 		auto it = std::find(vcStepExist.begin(), vcStepExist.end(), itStep.first);
// 		if (it == vcStepExist.end())
// 			continue;
// 
// 		for (auto itLine : itStep.second)
// 		{
// 			switch (eLineOption)
// 			{
// 			case eLineWidthOption::eLinePos :
// 			case eLineWidthOption::eLineNeg:
// 				if (itLine->strTypeName.CompareNoCase(L"Line") != 0)
// 					continue;
// 				break;
// 			case eLineWidthOption::eArcPos:
// 			case eLineWidthOption::eArcNeg:
// 				if (itLine->strTypeName.CompareNoCase(L"Arc") != 0)
// 					continue;
// 			break;
// 			case eLineWidthOption::eLineArcPos:
// 			case eLineWidthOption::eLineArcNeg:
// 				break;
// 			default :
// 				continue;
// 			}
// 			
// 			for (auto itFeature : itLine->vecHistoData)
// 			{
// 				if (itFeature.bPositive)
// 				{
// 					//r, s 제거
// 					CString strPenWidth_mm = itFeature.strName.Mid(1);
// 
// 					auto itWidth = std::find(vcPos.begin(), vcPos.end(), strPenWidth_mm);
// 					if (itWidth == vcPos.end())
// 						vcPos.emplace_back(strPenWidth_mm);
// 				}
// 				else
// 				{
// 					CString strPenWidth_mm = itFeature.strName.Mid(1);
// 
// 					auto itWidth = std::find(vcNeg.begin(), vcNeg.end(), strPenWidth_mm);
// 					if (itWidth == vcNeg.end())
// 						vcNeg.emplace_back(strPenWidth_mm);
// 				}
// 			}
// 		}
// 
// 		
// 	}



	

	if (vcHisto_Pos.size() <= 0 && vcHisto_Neg.size() <= 0)
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CLayerInfoFormView::Check_If_SubStep_Exist(CSubStep* pStep, std::vector<INT32>& vcStepExist)
{
	if (pStep == nullptr) return RESULT_BAD;

	CStepRepeat* pSubStepRepeat = &(pStep->m_arrStepRepeat);
	if (pSubStepRepeat == nullptr)
		return RESULT_BAD;

	auto it = std::find(vcStepExist.begin(), vcStepExist.end(), pStep->m_nStepID);
	if (it == vcStepExist.end())
		vcStepExist.emplace_back(static_cast<INT32>(pStep->m_nStepID));

	for (UINT i = 0; i < pStep->m_nNextStepCnt; i++)
	{
		CSubStep* pSubSubStep = pStep->m_arrNextStep[i];
		if (pSubSubStep == nullptr) continue;

		Check_If_SubStep_Exist(pSubSubStep, vcStepExist);
	}

	return RESULT_GOOD;
}

void CLayerInfoFormView::ClearPTWidthHisto()
{
	for (auto itStep : m_vcHistoPTWidth_Line)
	{
		for (auto itHisto : itStep.second)
		{
			if (itHisto)
			{
				itHisto->Clear();
				delete itHisto;
			}
			itHisto = nullptr;
		}
	}
	m_vcHistoPTWidth_Line.clear();

	for (auto itStep : m_vcHistoPTWidth_Arc)
	{
		for (auto itHisto : itStep.second)
		{
			if (itHisto)
			{
				itHisto->Clear();
				delete itHisto;
			}
			itHisto = nullptr;
		}
	}
	m_vcHistoPTWidth_Arc.clear();
}
