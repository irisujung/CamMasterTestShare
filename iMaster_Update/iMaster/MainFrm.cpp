
// MainFrm.cpp: CMainFrame 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "iMaster.h"

#include "MainFrm.h"

#include "CFeatureEdit_ResizeDlg.h"
#include "CFeatureEdit_MoveDlg.h"
#include "CFeatureEdit_DeleteDlg.h"
#include "CFeatureEdit_AddDlg.h"
#include "CAlignMaskEditDlg.h"
#include "CFeatureEdit_CopyDlg.h"
#include "CFeatureEdit_RotateDlg.h"
#include "CFeatureEdit_FeatureDlg.h"

#include "CSaveAsDlg.h"

#include "../iUtilities/PerfTimerEx.h"
#include <TlHelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
//	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
//	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_WM_SETTINGCHANGE()

	ON_COMMAND(IDS_RIB_REFRESH, OnRefreshButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_REFRESH, OnUpdateRefreshButton)
	ON_COMMAND(IDS_RIB_SETTING, OnSettingButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_SETTING, OnUpdateSettingButton)

	ON_COMMAND(IDS_RIB_LINESCAN, OnLineModeButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_LINESCAN, OnUpdateLineModeButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_NSIS, OnUpdateLineModeButton)
	ON_COMMAND(IDS_RIB_AREASCAN, OnAreaModeButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_AREASCAN, OnUpdateAreaModeButton)
	ON_COMMAND(IDS_RIB_PANEL, OnPanelModeButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_PANEL, OnUpdatePanelModeButton)

	ON_COMMAND(IDS_RIBBON_FRONT, OnViewFrontButton)
	ON_UPDATE_COMMAND_UI(IDS_RIBBON_FRONT, OnUpdateViewFrontButton)
	ON_COMMAND(IDS_RIBBON_BACK, OnViewBackButton)
	ON_UPDATE_COMMAND_UI(IDS_RIBBON_BACK, OnUpdateViewBackButton)

	ON_COMMAND(IDS_RIB_LOAD_LAYER, OnLoadLayer)
	ON_UPDATE_COMMAND_UI(IDS_RIB_LOAD_LAYER, OnUpdateLoadLayer)
	ON_COMMAND(IDS_RIB_MAKE_ALIGN, OnMakeAlign)
	ON_UPDATE_COMMAND_UI(IDS_RIB_MAKE_ALIGN, OnUpdateMakeAlign)	
	ON_COMMAND(IDS_RIB_MAKE_MASTER, OnMakeMaster)
	ON_UPDATE_COMMAND_UI(IDS_RIB_MAKE_MASTER, OnUpdateMakeMaster)
	ON_COMMAND(IDS_RIB_TRANSFER_MASTER, OnTransferMaster)
	ON_UPDATE_COMMAND_UI(IDS_RIB_TRANSFER_MASTER, OnUpdateTransferMaster)
		
	ON_COMMAND(IDS_RIB_PRESR, OnSetPreSR)
	ON_UPDATE_COMMAND_UI(IDS_RIB_PRESR, OnUpdateSetPreSR)
	ON_COMMAND(IDS_RIB_POSTSR, OnSetPostSR)
	ON_UPDATE_COMMAND_UI(IDS_RIB_POSTSR, OnUpdateSetPostSR)

 	ON_COMMAND(IDS_RIB_SET_ALIGN, OnSetAlign)
 	ON_UPDATE_COMMAND_UI(IDS_RIB_SET_ALIGN, OnUpdateSetAlign)
	ON_COMMAND(IDS_RIB_SET_ALIGNAUTO, OnSetAlignAuto)
	ON_UPDATE_COMMAND_UI(IDS_RIB_SET_ALIGNAUTO, OnUpdateSetAlignAuto)
	ON_COMMAND(IDS_RIB_SET_ALIGNMANUAL, OnSetAlignManual)
	ON_UPDATE_COMMAND_UI(IDS_RIB_SET_ALIGNMANUAL, OnUpdateSetAlignManual)

	ON_COMMAND(IDS_RIB_SET_MEASURE_NSIS, OnSetMeasure_NSIS)
	ON_UPDATE_COMMAND_UI(IDS_RIB_SET_MEASURE_NSIS, OnUpdateSetMeasure_NSIS)

	ON_COMMAND(IDS_RIB_SET_MEASURE_SR, OnSetMeasure_SR)
	ON_UPDATE_COMMAND_UI(IDS_RIB_SET_MEASURE_SR, OnUpdateSetMeasure_SR)	

	ON_COMMAND(IDS_RIB_SET_MONITOR, OnSetMonitorMode)
	ON_UPDATE_COMMAND_UI(IDS_RIB_SET_MONITOR, OnUpdateOnSetMonitorMode)
	
	ON_COMMAND(IDS_RIB_PREVIEW, OnPreviewButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_PREVIEW, OnUpdatePreviewButton)
	ON_COMMAND(IDS_RIB_FOV_VIEW, OnViewFOVButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_FOV_VIEW, OnUpdateViewFOVButton)
	ON_COMMAND(IDS_RIB_CELL_AREA, OnViewCellButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_CELL_AREA, OnUpdateViewCellButton)
	ON_COMMAND(IDS_RIB_STEP_REPEAT, OnViewStepRepeatButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_STEP_REPEAT, OnUpdateViewStepRepeatButton)
	ON_COMMAND(IDS_RIB_PROFILE_VIEW, OnViewProfileButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_PROFILE_VIEW, OnUpdateViewProfileButton)
	ON_COMMAND(IDS_RIB_SURFACE_VIEW, OnViewSurfaceButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_SURFACE_VIEW, OnUpdateViewSurfaceButton)
	ON_COMMAND(IDS_RIB_BOUNDARY_VIEW, OnViewBoundaryButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_BOUNDARY_VIEW, OnUpdateViewBoundaryButton)	

	//Save
	ON_COMMAND(IDS_RIB_SAVE, OnSaveButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_SAVE, OnUpdateSaveButton)
	ON_COMMAND(IDS_RIB_SAVE_AS, OnSaveAsButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_SAVE_AS, OnUpdateSaveAsButton)

	ON_COMMAND(IDS_RIB_VIEW_ALIGN, OnViewAlignButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_VIEW_ALIGN, OnUpdateViewAlignButton)
	ON_COMMAND(IDS_RIB_VIEW_MASK, OnViewMaskButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_VIEW_MASK, OnUpdateViewMaskButton)

	ON_COMMAND(IDS_RIB_MIRROR_H, OnMirrorHButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_MIRROR_H, OnUpdateMirrorHButton)
	ON_COMMAND(IDS_RIB_MIRROR_V, OnMirrorVButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_MIRROR_V, OnUpdateMirrorVButton)
	ON_COMMAND(IDS_RIB_ROTATION90, OnRotation90Button)
	ON_UPDATE_COMMAND_UI(IDS_RIB_ROTATION90, OnUpdateRotation90Button)
	ON_COMMAND(IDS_RIB_ROTATION180, OnRotation180Button)
	ON_UPDATE_COMMAND_UI(IDS_RIB_ROTATION180, OnUpdateRotation180Button)
	ON_COMMAND(IDS_RIB_ROTATION270, OnRotation270Button)
	ON_UPDATE_COMMAND_UI(IDS_RIB_ROTATION270, OnUpdateRotation270Button)
	ON_COMMAND(IDS_RIB_ROTATION360, OnRotation360Button)	
	ON_UPDATE_COMMAND_UI(IDS_RIB_ROTATION360, OnUpdateRotation360Button)

	ON_COMMAND(IDS_RIB_FOV_HSCAN, OnFovHScanButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_FOV_HSCAN, OnUpdateFovHScanButton)
	ON_COMMAND(IDS_RIB_FOV_VSCAN, OnFovVScanButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_FOV_VSCAN, OnUpdateFovVScanButton)
	ON_COMMAND(IDS_RIB_FOV_HJIGJAG, OnFovHJigjagButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_FOV_HJIGJAG, OnUpdateFovHJigjagButton)
	ON_COMMAND(IDS_RIB_FOV_VJIGJAG, OnFovVJigjagButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_FOV_VJIGJAG, OnUpdateFovVJigjagButton)
	ON_COMMAND(IDS_RIB_RULER, OnRulerButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_RULER, OnUpdateRulerButton)	

	
	ON_COMMAND(IDS_RIB_EDIT_EDITMODE, OnEditModeButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_EDIT_EDITMODE, OnUpdateEditModeButton)

	ON_COMMAND(IDS_RIB_EDIT_RESIZE, OnEditResizeButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_EDIT_RESIZE, OnUpdateEditResizeButton)
	ON_COMMAND(IDS_RIB_EDIT_MOVE, OnEditMoveButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_EDIT_MOVE, OnUpdateEditMoveButton)
	ON_COMMAND(IDS_RIB_EDIT_DELETE, OnEditDeleteButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_EDIT_DELETE, OnUpdateEditDeleteButton)
	ON_COMMAND(IDS_RIB_EDIT_ADD, OnEditAddButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_EDIT_ADD, OnUpdateEditAddButton)

	ON_UPDATE_COMMAND_UI(IDS_RIB_EDIT_COPY, OnUpdateEditCopyButton)
	ON_COMMAND(IDS_RIB_EDIT_COPY, OnEditCopyButton)

	ON_UPDATE_COMMAND_UI(IDS_RIB_EDIT_ROTATE, OnUpdateEditRotateButton)
	ON_COMMAND(IDS_RIB_EDIT_ROTATE, OnEditRotateButton)

	ON_UPDATE_COMMAND_UI(IDS_RIB_EDIT_EDITMODE, OnUpdateEditFeauterButton)
	ON_COMMAND(IDS_RIB_EDIT_EDITMODE, OnEditFeatureButton)

	//Feature Add
	ON_COMMAND(IDS_RIB_ADD_SURFACE,onAddSurfaceButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_ADD_SURFACE, OnUpdateSurfaceButton)
	ON_COMMAND(IDS_RIB_ADD_CROSS,OnAddTpButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_ADD_CROSS, OnUpdateTpButton)
	ON_COMMAND(IDS_RIB_ADD_ARC,OnAddArcButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_ADD_ARC, OnUpdateArcButton)
	ON_COMMAND(IDS_RIB_ADD_LINE_X,OnAddLineXButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_ADD_LINE_X, OnUpdateAddLineXButton)
	ON_COMMAND(IDS_RIB_ADD_LINE_Y,OnAddLineYButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_ADD_LINE_Y, OnUpdateLineYButton)
	ON_COMMAND(IDS_RIB_ADD_CIRCLE,OnAddCircleButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_ADD_CIRCLE, OnUpdateCircleButton)
	ON_COMMAND(IDS_RIB_ADD_LINE,OnAddLineButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_ADD_LINE, OnUpdateLineButton)
	ON_COMMAND(IDS_RIB_ADD_RECT,OnAddRectangleButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_ADD_RECT, OnUpdateRectagneButton)
	ON_COMMAND(IDS_RIB_ADD_LINE_X_FEATURE, OnAddLineX_FeatureBaseButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_ADD_LINE_X_FEATURE, OnUpdateLineX_FeatureBaseButton)
	ON_COMMAND(IDS_RIB_ADD_LINE_Y_FEATURE, OnAddLineY_FeatureBaseButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_ADD_LINE_Y_FEATURE, OnUpdateLineY_FeatureBaseButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_LAYER_COMBOBOX,OnUpdateLayerComboBox)

	ON_COMMAND(IDS_RIB_ADD_CIRCLE_AUTO, OnAddCircle_FeatureBaseButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_ADD_CIRCLE_AUTO, OnUpdateCircle_FeatureBaseButton)
	ON_COMMAND(IDS_RIB_ADD_RECT_AUTO, OnAddRect_FeatureBaseButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_ADD_RECT_AUTO, OnUpdateRect_FeatureBaseButton)

	// 2022.11.24 KJH ADD
	// ADD TP 기능 추가
	ON_COMMAND(IDS_RIB_ADD_CROSS_AUTO, OnAddTpAutoButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_ADD_CROSS_AUTO, OnUpdateTpAutoButton)


	//ON_COMMAND(IDS_RIB_ADD_SQUARE,OnAddSquareButton)
// 	ON_COMMAND(IDS_RIB_ADD_RECTXR,OnAddRectxrButton)
// 	ON_COMMAND(IDS_RIB_ADD_RECTXC,OnAddRectxcButton)
// 
// 	ON_COMMAND(IDS_RIB_ADD_OVAL,OnAddOvalButton)
// 	ON_COMMAND(IDS_RIB_ADD_DIAMOND,OnAddDiamondButton)
// 	ON_COMMAND(IDS_RIB_ADD_OCTAGON,OnAddOctagonButton)
// 	ON_COMMAND(IDS_RIB_ADD_ROUND_DONUT,OnAddDonut_rButton)
// 	ON_COMMAND(IDS_RIB_ADD_SQUARE_DONUT,OnAddDonut_sButton)
// 
//     ON_COMMAND(IDS_RIB_ADD_HORIZONTAL_HEXAGON,OnAddHex_lButton)
// 	ON_COMMAND(IDS_RIB_ADD_VETICAL_HEXAGON,OnAddHex_sButton)
// 	ON_COMMAND(IDS_RIB_ADD_BUTTERFLY,OnAddBfrButton)
// 	ON_COMMAND(IDS_RIB_ADD_SQUARE_BUTTERFLY,OnAddBfsButton)
// 	ON_COMMAND(IDS_RIB_ADD_TRIANGLE,OnAddTriButton)
// 
// 	ON_COMMAND(IDS_RIB_ADD_HALF_OVAL,OnAddOval_hButton)
// 	ON_COMMAND(IDS_RIB_ADD_ROUND_THERMAL_ROUNDED,OnAddThrButton)
// 	ON_COMMAND(IDS_RIB_ADD_ROUND_THERMAL_SQUARED,OnAddThsButton)
// 	ON_COMMAND(IDS_RIB_ADD_SQUARE_THERMAL,OnAddS_ThsButton)
// 	ON_COMMAND(IDS_RIB_ADD_SQUARE_THERMAL_OPEN_CORNERS,OnAddS_ThoButton)
// 
// 	ON_COMMAND(IDS_RIB_ADD_SQUARE_ROUND_THERMAL,OnAddSr_ThsButton)
// 	ON_COMMAND(IDS_RIB_ADD_RECTANGULAR_THERMAL,OnAddRc_ThsButton)
// 	ON_COMMAND(IDS_RIB_ADD_RECTANGULAR_THERMAL_OPEN_CORNERS,OnAddRc_ThoButton)
//     ON_COMMAND(IDS_RIB_ADD_ELLIPSE,OnAddElButton)
// 	ON_COMMAND(IDS_RIB_ADD_MOIRE,OnAddMorieButton)
// 
//     ON_COMMAND(IDS_RIB_ADD_HOLE,OnAddHoleButton)

	ON_COMMAND(IDS_RIB_EDIT_MASK, OnEditMaskButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_EDIT_MASK, OnUpdateEditMaskButton)

	ON_COMMAND(IDS_RIB_EDIT_TEST_SAVE, OnTestSaveButton)
	ON_UPDATE_COMMAND_UI(IDS_RIB_EDIT_TEST_SAVE, OnUpdateTestSaveButton)
	
	ON_COMMAND(IDS_RIB_VIEW_JOB_INFO, OnViewJobInfo)
	ON_COMMAND(IDS_RIB_VIEW_CAM_INFO, OnViewCamInfo)
	ON_COMMAND(IDS_RIB_VIEW_LAYER_INFO, OnViewLayerInfo)
	ON_COMMAND(IDS_RIB_VIEW_MEASURE_INFO, OnViewMeasureInfo)
	ON_COMMAND(IDS_RIB_VIEW_MINIMAP_VIEW, OnViewMinimap)
	ON_COMMAND(IDS_RIB_VIEW_LOG, OnViewLogInfo)
	ON_COMMAND(IDS_RIB_VERSION, OnVersionButton)

	ON_MESSAGE(WM_AUTO_RECIPE_PIPE, OnNsisPipe)
//	ON_MESSAGE(WM_CHANGE_LAYER_INFO, OnChangeLayerInfo)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame() noexcept
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(2422);

	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	//theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLACK);
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_AQUA);

	m_pProgressDlg = nullptr;
	m_bIsExtractCam = FALSE;
	m_bIsLoadLayers = FALSE;
	m_bIsProcessMaster = FALSE;

#ifdef _DEBUG

#else
	if (GetMachineType() == eMachineType::eNSIS)
	{
		if (CheckDongle() == FALSE)
		{
			AfxMessageBox(_T("Check CamMaster License Dongle. Exit Program"));
			::PostQuitMessage(WM_QUIT);
			return;
		}
	}
#endif // Release

}

CMainFrame::~CMainFrame()
{
	if (m_pProgressDlg)
		delete m_pProgressDlg;
	m_pProgressDlg = NULL;

	ReleaseFont();

	Destroy_iArchive();
	Destroy_iDraw();
	Destroy_iProcess();

	_DeleteFeatureEditDlg();
	m_UDPforNSIS.Close();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// 2022.09.23 KJH ADD
	// 중복실행 방지 때문에 추가 했으나 CreateMutex로 대체 iMaster.cpp
	/*if (m_bProgrameCheck == FALSE)
	{
		BOOL bRet = FALSE;
		bRet = GetRunProcess();
		if (bRet == TRUE)
			m_bProgrameCheck = TRUE;
		else
			m_bProgrameCheck = FALSE;
	}
	else
	{
		exit(1);
	}*/

	m_wndRibbonBar.Create(this);
		
	CDockingManager::SetDockingMode(DT_SMART);
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	//OnApplicationLook(ID_VIEW_APPLOOK_OFF_2007_BLACK);// theApp.m_nAppLook);
	
	// 2022.06.21
	// 김준호 주석추가
	// 배색 변경을 위해 BLACK -> AQUA 로 변경
	OnApplicationLook(ID_VIEW_APPLOOK_OFF_2007_AQUA);

	//EnableDocking(CBRS_ALIGN_ANY);
	//CMFCPopupMenu::SetForceMenuFocus(FALSE);
	
	Initialize_SetMainFrm(this);	
	Initialize_Spec();

	CString strCamPath = L"", strLogPath = L"";
	SystemSpec_t* pSysSpec = GetSystemSpec();
	if (pSysSpec)
	{
		strCamPath = pSysSpec->sysPath.strCamPath;
		strLogPath = pSysSpec->sysPath.strLogPath;
	}

	CreateLogViewDocPane(strLogPath);

	///////////////////////////////////////////////////////////////////////////	
	CreateFormViewDocPane(ID_JOBINFO_DOCPANE, L"Model Info", LEFT_DOCK_WIDTH_VALUE, 100, CBRS_LEFT, &m_docPaneJobInfo);
	m_docPaneJobInfo.SetJobPath(strCamPath);
	CreateFormViewDocPane(ID_CAMINFO_DOCPANE, L"Step", RIGHT_DOCK_WIDTH_VALUE, 200, CBRS_RIGHT, &m_docPaneCamInfo);

	CreateFormViewDocPane(ID_SETTINGS_DOCPANE, L"Settings", RIGHT_DOCK_WIDTH_VALUE, 200, CBRS_RIGHT, &m_docPaneSettings);
	CreateFormViewDocPane(ID_FEATURE_EDIT_DOCPANE, L"Feature Edit", RIGHT_DOCK_WIDTH_VALUE, 200, CBRS_RIGHT, &m_docPaneFeatureEdit);
	CreateLayerInfoDocPane();	

	Initialize_iArchive();
		
	InitializeRibbon();
	InitializeFont();
	if (!InitializeStatusBar())
		return -1;

	if (pSysSpec && pSysSpec->sysBasic.McType == eMachineType::eNSIS && pSysSpec->sysBasic.UserSite == eUserSite::eDAEDUCK)		
	{
		CreateMeasureInfoDocPane();

		CDockablePane* pTabberBar = NULL;
		m_docPaneCamInfo.AttachToTabWnd(&m_docPaneLogView, DM_SHOW, TRUE, &pTabberBar);
		m_docPaneFeatureEdit.AttachToTabWnd(&m_docPaneLogView, DM_SHOW, TRUE, &pTabberBar);
		m_docPaneLayerInfo.AttachToTabWnd(&m_docPaneLogView, DM_SHOW, TRUE, &pTabberBar);
		m_docPaneMeasureInfo.AttachToTabWnd(&m_docPaneLogView, DM_SHOW, TRUE, &pTabberBar);
		m_docPaneSettings.AttachToTabWnd(&m_docPaneLogView, DM_SHOW, TRUE, &pTabberBar);		
				
	}
		//CreateFormViewDocPane(ID_MINIMAP_DOCPANE, L"Minimap", 200, 100, CBRS_LEFT, &m_docPaneMinimapView);

	else if (pSysSpec && pSysSpec->sysBasic.McType == eMachineType::eNSIS && pSysSpec->sysBasic.UserSite == eUserSite::eSEM)
	{
		//Measure Info
		CreateMeasureInfo_V2_DocPane();					
			
		//Minimap V2
		CreateFormViewDocPane(ID_MINIMAP_DOCPANE_V2, L"Minimap_v2", 200, 100, CBRS_LEFT, &m_docPaneMinimapView_V2);			
		//CreateMinimap_V2_DocPane();

		m_docPaneMinimapView_V2.DockToWindow(&m_docPaneJobInfo, CBRS_ALIGN_BOTTOM);

		CDockablePane* pTabberBar = NULL;
		m_docPaneCamInfo.AttachToTabWnd(&m_docPaneLogView, DM_SHOW, TRUE, &pTabberBar);	
		m_docPaneFeatureEdit.AttachToTabWnd(&m_docPaneLogView, DM_SHOW, TRUE, &pTabberBar);	
		m_docPaneLayerInfo.AttachToTabWnd(&m_docPaneLogView, DM_SHOW, TRUE, &pTabberBar);	
		m_docPaneMeasureInfo_V2.AttachToTabWnd(&m_docPaneLogView, DM_SHOW, TRUE, &pTabberBar);	
		m_docPaneSettings.AttachToTabWnd(&m_docPaneLogView, DM_SHOW, TRUE, &pTabberBar);			
		
	}
	else if (pSysSpec && pSysSpec->sysBasic.McType == eMachineType::eAVI)
	{
		CDockablePane* pTabberBar = NULL;
		m_docPaneCamInfo.AttachToTabWnd(&m_docPaneLogView, DM_SHOW, TRUE, &pTabberBar);
		m_docPaneFeatureEdit.AttachToTabWnd(&m_docPaneLogView, DM_SHOW, TRUE, &pTabberBar);
		m_docPaneLayerInfo.AttachToTabWnd(&m_docPaneLogView, DM_SHOW, TRUE, &pTabberBar);
		m_docPaneSettings.AttachToTabWnd(&m_docPaneLogView, DM_SHOW, TRUE, &pTabberBar);
	}
	
	//AdjustDockPanes();


	//Edit Dialog
	_CreateFeatureEditDlg();

	if (pSysSpec)
	{
		if (pSysSpec->sysBasic.McType == eMachineType::eNSIS)
		{
			m_iPort = pSysSpec->sysNSIS.sysNSISPort;
			m_csIP = pSysSpec->sysNSIS.sysNSISIP;

			if (!m_UDPforNSIS.Create(this->m_hWnd, pSysSpec->sysNSIS.sysCamPort, WM_AUTO_RECIPE_PIPE, pSysSpec->sysNSIS.sysCamIP))
			{
				AfxMessageBox(_T("ERROR!! CANNOT INITIALIZE CAM MASTER UDP!!"));
			}

			ClearLayerList();
			SendPipeNSIS(_T("MASTER_START"));
			m_strStartLayer.Empty();
			m_strEndLayer.Empty();
		}

		m_stUserSetInfo.userSite = pSysSpec->sysBasic.UserSite;
		m_stUserSetInfo.mcType = pSysSpec->sysBasic.McType;

		OnSetMonitorMode();
	}	

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~FWS_ADDTOTITLE;
//	cs.style &= ~FWS_PREFIXTITLE;

	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	
	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 메뉴를 검색합니다. */);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}

	// 모든 사용자 도구 모음에 사용자 지정 단추를 활성화합니다.
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != nullptr)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}

BOOL CMainFrame::CreateFormViewDocPane(INT32 iResId, CString strCaption, UINT nSizeX, UINT nSizeY, DWORD dwPaneStyle, CFormViewDocPane* pWndDocPane)
{
	CDockingManager::SetDockingMode(DT_SMART);	

	if (!pWndDocPane->Create(strCaption, this,
		CRect(0, 0, nSizeX, nSizeY), TRUE, iResId, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_FLOAT_MULTI | dwPaneStyle))		
	{
		TRACE0("Failed to create form view doc pane\n");
		return FALSE; // failed to create
	}

	

	//	pWndDocPane->SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);
	pWndDocPane->EnableDocking(CBRS_ALIGN_ANY);
	DockPane(pWndDocPane);
	pWndDocPane->EnableAutohideAll(FALSE);	
	pWndDocPane->m_bDisableAnimation = TRUE;
	pWndDocPane->m_bHideInAutoHideMode = TRUE;

	return TRUE;
}

void CMainFrame::LockJobTree(BOOL bEnable)
{
	m_docPaneJobInfo.LockJobTree(bEnable);
}

BOOL CMainFrame::CreateLogViewDocPane(CString strPath)
{
	CDockingManager::SetDockingMode(DT_SMART);

	m_docPaneLogView.SetLogPath(strPath);
	/*If bottom doc pane creation failed return*/
	//if (!m_docPaneLogView.Create(_T("Application Log"), this,
	if (!m_docPaneLogView.Create(_T("Log"), this,
		CRect(0, 0, RIGHT_DOCK_WIDTH_VALUE, 100), TRUE, ID_GENERALINFO_DOCPANE, WS_CHILD |
		WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create bottom doc pane\n");
		return FALSE; // failed to create
	}	

	

	m_docPaneLogView.SetIcon(AfxGetApp()->LoadIconW(IDR_MAINFRAME), TRUE);
	m_docPaneLogView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_docPaneLogView);
	m_docPaneLogView.SetAutoHideMode(FALSE, CBRS_ALIGN_ANY);

	m_docPaneLogView.m_bDisableAnimation = TRUE;	
	m_docPaneLogView.m_bHideInAutoHideMode = TRUE;

	return TRUE;
}

BOOL CMainFrame::CreateMeasureInfoDocPane()
{
	CDockingManager::SetDockingMode(DT_SMART);
	
	// Create Layers window	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_FLOAT_MULTI;

	if (!m_docPaneMeasureInfo.Create(L"Measure Info", this, CRect(0, 0, 200, 200), TRUE, ID_MEATURE_INFO_DOCPANE, dwStyle | CBRS_LEFT))
	{
		TRACE0("Failed to Create Measure Info docPane\n");
		return FALSE; // failed to create
	}

	m_docPaneMeasureInfo.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_docPaneMeasureInfo);

	m_docPaneMeasureInfo.SetAutoHideMode(FALSE, CBRS_ALIGN_ANY);

	m_docPaneMeasureInfo.m_bDisableAnimation = TRUE;
	m_docPaneMeasureInfo.m_bHideInAutoHideMode = TRUE;

	return TRUE;
}

BOOL CMainFrame::CreateMeasureInfo_V2_DocPane()
{
	CDockingManager::SetDockingMode(DT_SMART);

	// Create Layers window	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_FLOAT_MULTI | CBRS_RIGHT;

	if (!m_docPaneMeasureInfo_V2.Create(L"Measure Info V2", this, CRect(0, 0, 200, 200), TRUE, IDD_MEASURE_INFO_V2, dwStyle ))
	{
		TRACE0("Failed to Create Measure Info docPane\n");
		return FALSE; // failed to create
	}
	m_docPaneMeasureInfo_V2.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_docPaneMeasureInfo_V2);

	m_docPaneMeasureInfo_V2.SetAutoHideMode(FALSE, CBRS_ALIGN_ANY);
	m_docPaneMeasureInfo_V2.m_bDisableAnimation = TRUE;
	m_docPaneMeasureInfo_V2.m_bHideInAutoHideMode = TRUE;

	return TRUE;
}

/*
BOOL CMainFrame::CreateMinimap_V2_DocPane()
{
	CDockingManager::SetDockingMode(DT_SMART);

	// Create Layers window	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_FLOAT_MULTI;

	if (!m_docPaneMinimapView_V2.Create(L"Minimap V2", this, CRect(0, 0, 200, 200), TRUE, ID_MINIMAP_FORM_V2, dwStyle | CBRS_LEFT))
	{
		TRACE0("Failed to Create Minimap V2 docPane\n");
		return FALSE; // failed to create
	}

	m_docPaneMinimapView_V2.SetIcon(AfxGetApp()->LoadIconW(IDR_MAINFRAME), TRUE);
	
	m_docPaneMinimapView_V2.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_docPaneMinimapView_V2);

	m_docPaneMinimapView_V2.SetAutoHideMode(FALSE, CBRS_ALIGN_ANY);

	m_docPaneMinimapView_V2.m_bDisableAnimation = TRUE;
	m_docPaneMinimapView_V2.m_bHideInAutoHideMode = TRUE;

	

	return TRUE;
}
*/

BOOL CMainFrame::CreateLayerInfoDocPane()
{
	CDockingManager::SetDockingMode(DT_SMART);

	// Create Layers window	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_FLOAT_MULTI;

	if (!m_docPaneLayerInfo.Create(L"Layer", this, CRect(0, 0, RIGHT_DOCK_WIDTH_VALUE, 200), TRUE, ID_LAYER_INFO_DOCPANE, dwStyle | CBRS_LEFT))
	{
		TRACE0("Failed to Create Layer Info docPane\n");
		return FALSE; // failed to create
	}
	
	m_docPaneLayerInfo.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_docPaneLayerInfo);

	m_docPaneLayerInfo.SetAutoHideMode(FALSE, CBRS_ALIGN_ANY);

	m_docPaneLayerInfo.m_bDisableAnimation = TRUE;
	m_docPaneLayerInfo.m_bHideInAutoHideMode = TRUE;

	return TRUE;
}

void CMainFrame::IsDoneExtractCam(BOOL b)
{ 
	m_bIsExtractCam = b;

	if (!b)
		m_stUserSetInfo.strWorkLayer = L"";
}

void CMainFrame::IsDoneLoadLayers(BOOL b)
{
	m_bIsLoadLayers = b;
}

void CMainFrame::IsDoneProcessMaster(BOOL b)
{
	m_bIsProcessMaster = b;

}

void CMainFrame::IsAllLayerLoaded(BOOL b)
{
	m_bIsAllLayerLoaded = b;
}

void CMainFrame::AddApplicationLog(CString strLog, LogMsgType enMsgType)
{
	m_docPaneLogView.AddLog(strLog, enMsgType);
}

void CMainFrame::ShowPopupInitialize(CString strText)
{
	if (m_pProgressDlg)
	{
		delete m_pProgressDlg;
		m_pProgressDlg = nullptr;
	}
	
	m_pProgressDlg = new CProgressDlg;
	m_pProgressDlg->SetText(strText);
	m_pProgressDlg->Create(IDD_PROGRESS_DLG, this);
	m_pProgressDlg->ModifyStyle(0, DS_SYSMODAL);
	m_pProgressDlg->ShowWindow(SW_SHOWDEFAULT);
}

void CMainFrame::HidePopupInitialize()
{
	if (m_pProgressDlg)
		m_pProgressDlg->EndDialog(0);
}

void CMainFrame::AdjustDockPanes()
{
	m_docPaneJobInfo.ShowPane(TRUE, FALSE, FALSE);

	if (!m_docPaneJobInfo.IsDocked())
	{
		m_docPaneJobInfo.ShowPane(FALSE, FALSE, FALSE);
		m_docPaneJobInfo.UndockPane();
		m_docPaneJobInfo.FloatPane(CRect(0, 0, 100, 100), DM_UNKNOWN, false);

		m_docPaneJobInfo.DockToFrameWindow(CBRS_LEFT);
		m_docPaneJobInfo.ShowPane(TRUE, FALSE, FALSE);
	}
	m_docPaneJobInfo.InitialUpdate();

	if (!m_docPaneLogView.IsDocked())
		m_docPaneLogView.ShowPane(TRUE, FALSE, FALSE);

	m_docPaneSettings.ShowPane(TRUE, FALSE, FALSE);
	m_docPaneCamInfo.ShowPane(TRUE, FALSE, FALSE);
	m_docPaneLayerInfo.ShowPane(TRUE, FALSE, FALSE);
	m_docPaneFeatureEdit.ShowPane(TRUE, FALSE, FALSE);

	if (GetMachineType() != eMachineType::eNSIS)
	{
		CDockablePane* pTabbedBar = NULL;
		if (!m_docPaneSettings.IsDocked() || !m_docPaneCamInfo.IsDocked() ||
			!m_docPaneLogView.IsDocked() || !m_docPaneLayerInfo.IsDocked() || !m_docPaneFeatureEdit.IsDocked())
		{
			CDockablePane* pTabbedBar = NULL;
			m_docPaneSettings.ShowPane(FALSE, FALSE, FALSE);
			m_docPaneSettings.UndockPane();
			m_docPaneSettings.FloatPane(CRect(0, 0, 100, 100), DM_UNKNOWN, false);

			m_docPaneLogView.ShowPane(FALSE, FALSE, FALSE);
			m_docPaneLogView.UndockPane();
			m_docPaneLogView.FloatPane(CRect(0, 0, 100, 100), DM_UNKNOWN, false);

			m_docPaneLogView.DockToFrameWindow(CBRS_RIGHT);
			m_docPaneLogView.ShowPane(TRUE, FALSE, FALSE);
		}

		if (!m_docPaneCamInfo.IsDocked())
		{
			CDockablePane* pTabbedBar = NULL;

			m_docPaneFeatureEdit.ShowPane(FALSE, FALSE, FALSE);
			m_docPaneFeatureEdit.UndockPane();
			m_docPaneFeatureEdit.FloatPane(CRect(0, 0, 100, 100), DM_UNKNOWN, false);

			pTabbedBar = NULL;
			m_docPaneSettings.DockToFrameWindow(CBRS_RIGHT);
			m_docPaneSettings.ShowPane(TRUE, FALSE, FALSE);
			m_docPaneCamInfo.ShowPane(FALSE, FALSE, FALSE);
			m_docPaneCamInfo.UndockPane();
			m_docPaneCamInfo.FloatPane(CRect(0, 0, 100, 100), DM_UNKNOWN, false);

			m_docPaneCamInfo.DockToFrameWindow(CBRS_RIGHT);
			m_docPaneCamInfo.ShowPane(TRUE, FALSE, FALSE);
		}
		m_docPaneCamInfo.InitialUpdate();

		if (!m_docPaneSettings.IsDocked())
		{
			CDockablePane* pTabbedBar = NULL;

			m_docPaneSettings.ShowPane(FALSE, FALSE, FALSE);
			m_docPaneSettings.UndockPane();
			m_docPaneSettings.FloatPane(CRect(0, 0, 100, 100), DM_UNKNOWN, false);

			m_docPaneSettings.DockToFrameWindow(CBRS_RIGHT);
			m_docPaneSettings.ShowPane(TRUE, FALSE, FALSE);
			m_docPaneFeatureEdit.AttachToTabWnd(&m_docPaneFeatureEdit, DM_SHOW, TRUE, &pTabbedBar);
			m_docPaneFeatureEdit.ShowPane(TRUE, FALSE, FALSE);

			m_docPaneSettings.InitialUpdate();
			m_docPaneCamInfo.InitialUpdate();
			m_docPaneFeatureEdit.InitialUpdate();
		}
	}
	m_docPaneSettings.InitialUpdate();

	if (!m_docPaneLayerInfo.IsDocked())
	{
		m_docPaneLayerInfo.ShowPane(TRUE, FALSE, FALSE);

		CDockablePane* pTabbedBar = NULL;

		m_docPaneLayerInfo.ShowPane(FALSE, FALSE, FALSE);
		m_docPaneLayerInfo.UndockPane();
		//m_docPaneLayerInfo.FloatPane(CRect(0, 0, 100, 100), DM_UNKNOWN, false);

		m_docPaneLayerInfo.DockToFrameWindow(CBRS_RIGHT);
		m_docPaneLayerInfo.ShowPane(TRUE, FALSE, FALSE);
	}

// 	m_docPaneLogView.ShowPane(TRUE, FALSE, FALSE);
// 	m_docPaneSettings.ShowPane(TRUE, FALSE, FALSE);
// 	m_docPaneCamInfo.ShowPane(TRUE, FALSE, FALSE);
// 	
// 
// 	
// 		if (!.IsDocked() || !.IsDocked() ||
// 			!.IsDocked() || !.IsDocked())
// 		{
// 			
// 			m_docPaneSettings.ShowPane(FALSE, FALSE, FALSE);
// 			m_docPaneSettings.UndockPane();
// 			m_docPaneSettings.FloatPane(CRect(0, 0, 100, 100), DM_UNKNOWN, false);
// 
// 			m_docPaneCamInfo.ShowPane(FALSE, FALSE, FALSE);
// 			m_docPaneCamInfo.UndockPane();
// 			m_docPaneCamInfo.FloatPane(CRect(0, 0, 100, 100), DM_UNKNOWN, false);
// 
// 			
// 
// 			m_docPaneLayerInfo.ShowPane(FALSE, FALSE, FALSE);
// 			m_docPaneLayerInfo.UndockPane();
// 			m_docPaneLayerInfo.FloatPane(CRect(0, 0, 100, 100), DM_UNKNOWN, false);
// 
// 			pTabbedBar = NULL;
// 			m_docPaneSettings.DockToFrameWindow(CBRS_RIGHT);
// 			m_docPaneSettings.ShowPane(TRUE, FALSE, FALSE);
// 
// 			m_docPaneCamInfo.AttachToTabWnd(&m_docPaneSettings, DM_SHOW, TRUE, &pTabbedBar);
// 			m_docPaneCamInfo.ShowPane(TRUE, FALSE, FALSE);
// 
// 			m_docPaneLogView.AttachToTabWnd(&m_docPaneCamInfo, DM_SHOW, TRUE, &pTabbedBar);
// 			m_docPaneLogView.ShowPane(TRUE, FALSE, FALSE);
// 
// 			m_docPaneLayerInfo.AttachToTabWnd(&m_docPaneLogView, DM_SHOW, TRUE, &pTabbedBar);
// 			m_docPaneLayerInfo.ShowPane(TRUE, FALSE, FALSE);
// 
// 			m_docPaneSettings.InitialUpdate();
// 			m_docPaneCamInfo.InitialUpdate();
// 		}


// 	else
// 	{
// 		
// 
// 		if (!m_docPaneSettings.IsDocked() || !m_docPaneCamInfo.IsDocked() ||
// 			!m_docPaneLogView.IsDocked() || !m_docPaneLayerInfo.IsDocked() )
// 		{
// 			CDockablePane* pTabbedBar = NULL;
// 			m_docPaneSettings.ShowPane(FALSE, FALSE, FALSE);
// 			m_docPaneSettings.UndockPane();
// 			m_docPaneSettings.FloatPane(CRect(0, 0, 100, 100), DM_UNKNOWN, false);
// 
// 			m_docPaneCamInfo.ShowPane(FALSE, FALSE, FALSE);
// 			m_docPaneCamInfo.UndockPane();
// 			m_docPaneCamInfo.FloatPane(CRect(0, 0, 100, 100), DM_UNKNOWN, false);
// 
// 			m_docPaneLogView.ShowPane(FALSE, FALSE, FALSE);
// 			m_docPaneLogView.UndockPane();
// 			m_docPaneLogView.FloatPane(CRect(0, 0, 100, 100), DM_UNKNOWN, false);
// 
// 			m_docPaneLayerInfo.ShowPane(FALSE, FALSE, FALSE);
// 			m_docPaneLayerInfo.UndockPane();
// 			m_docPaneLayerInfo.FloatPane(CRect(0, 0, 100, 100), DM_UNKNOWN, false);
// 			
// 			
// 
// 
// 			
// 
// 			pTabbedBar = NULL;
// 			m_docPaneSettings.DockToFrameWindow(CBRS_RIGHT);
// 			m_docPaneSettings.ShowPane(TRUE, FALSE, FALSE);
// 
// 			m_docPaneCamInfo.AttachToTabWnd(&m_docPaneSettings, DM_SHOW, TRUE, &pTabbedBar);
// 			m_docPaneCamInfo.ShowPane(TRUE, FALSE, FALSE);
// 
// 			m_docPaneLogView.AttachToTabWnd(&m_docPaneCamInfo, DM_SHOW, TRUE, &pTabbedBar);
// 			m_docPaneLogView.ShowPane(TRUE, FALSE, FALSE);
// 
// 			m_docPaneLayerInfo.AttachToTabWnd(&m_docPaneLogView, DM_SHOW, TRUE, &pTabbedBar);
// 			m_docPaneLayerInfo.ShowPane(TRUE, FALSE, FALSE);
// 			
// 			m_docPaneSettings.InitialUpdate();
// 			m_docPaneCamInfo.InitialUpdate();
// 		}

	if (GetMachineType() == eMachineType::eNSIS)
	{
		CDockablePane* pTabbedBar = NULL;
		SystemSpec_t* pSysSpec = GetSystemSpec();
		if (pSysSpec->sysBasic.UserSite == eUserSite::eDAEDUCK)
		{
			m_docPaneMeasureInfo.ShowPane(TRUE, FALSE, FALSE);
			
			if (!m_docPaneMeasureInfo.IsDocked())
			{
				m_docPaneMeasureInfo.DockToFrameWindow(CBRS_RIGHT);

				m_docPaneMeasureInfo.ShowPane(FALSE, FALSE, FALSE);
				m_docPaneMeasureInfo.UndockPane();
				m_docPaneMeasureInfo.FloatPane(CRect(0, 0, 100, 100), DM_UNKNOWN, false);

				m_docPaneMeasureInfo.AttachToTabWnd(&m_docPaneLayerInfo, DM_SHOW, TRUE, &pTabbedBar);
				m_docPaneMeasureInfo.ShowPane(TRUE, FALSE, FALSE);

			}
		}
		else if (pSysSpec->sysBasic.UserSite == eUserSite::eSEM)
		{
			/*
			if (!m_docPaneMinimapView_V2.IsDocked())
			{
				m_docPaneMinimapView_V2.DockToFrameWindow(CBRS_LEFT);

				m_docPaneMinimapView_V2.ShowPane(FALSE, FALSE, FALSE);
				m_docPaneMinimapView_V2.UndockPane();
				m_docPaneMinimapView_V2.FloatPane(CRect(0, 0, 100, 100), DM_UNKNOWN, false);

				m_docPaneMinimapView_V2.AttachToTabWnd(&m_docPaneLayerInfo, DM_SHOW, TRUE, &pTabbedBar);
				m_docPaneMinimapView_V2.ShowPane(TRUE, FALSE, FALSE);
			}
			*/

			//m_docPaneMinimapView_V2.ShowPane(TRUE, FALSE, FALSE);
			//m_docPaneMeasureInfo_V2.ShowPane(TRUE, FALSE, FALSE);

			if (!m_docPaneMinimapView_V2.IsDocked())
			{
				m_docPaneMinimapView_V2.ShowPane(FALSE, FALSE, FALSE);
				m_docPaneMinimapView_V2.UndockPane();
				//m_docPaneMinimapView_V2.FloatPane(CRect(0, 0, 100, 100), DM_UNKNOWN, false);

				m_docPaneMinimapView_V2.DockToFrameWindow(CBRS_LEFT);
				//m_docPaneMinimapView_V2.AttachToTabWnd(&m_docPaneLayerInfo, DM_SHOW, TRUE, &pTabbedBar);
				//m_docPaneMinimapView_V2.DockToWindow(&m_docPaneJobInfo, CBRS_BOTTOM);
				m_docPaneJobInfo.DockToWindow(&m_docPaneMinimapView_V2, CBRS_TOP);
				m_docPaneMinimapView_V2.ShowPane(TRUE, FALSE, FALSE);
			}

			if (!m_docPaneMeasureInfo_V2.IsDocked())
			{
				//m_docPaneMeasureInfo_V2.DockToFrameWindow(CBRS_LEFT);

				/*m_docPaneFeatureEdit.ShowPane(FALSE, FALSE, FALSE);
				m_docPaneFeatureEdit.UndockPane();
				m_docPaneFeatureEdit.FloatPane(CRect(0, 0, 100, 100), DM_UNKNOWN, false);

				m_docPaneFeatureEdit.AttachToTabWnd(&m_docPaneLayerInfo, DM_SHOW, TRUE, &pTabbedBar);
				m_docPaneFeatureEdit.ShowPane(TRUE, FALSE, FALSE);*/

				m_docPaneMeasureInfo_V2.ShowPane(FALSE, FALSE, FALSE);
				m_docPaneMeasureInfo_V2.UndockPane();
				m_docPaneMeasureInfo_V2.FloatPane(CRect(0, 0, 100, 100), DM_UNKNOWN, false);
				m_docPaneMeasureInfo_V2.DockToFrameWindow(CBRS_RIGHT);

				m_docPaneMeasureInfo_V2.AttachToTabWnd(&m_docPaneLayerInfo, DM_SHOW, TRUE, &pTabbedBar);
				m_docPaneMeasureInfo_V2.ShowPane(TRUE, FALSE, FALSE);
			}
		}
		m_docPaneFeatureEdit.InitialUpdate();
	}
}

void CMainFrame::SetStepCount(UINT32& iStepCnt, std::vector<UINT32>& vcStepRepeatCnt)
{
	if (m_docPaneCamInfo.SetStepCount(iStepCnt, vcStepRepeatCnt) == RESULT_GOOD)
		AddApplicationLog(SUCCEED_SET_STEP_INFO);
	else
		AddApplicationLog(FAILED_SET_STEP_INFO, LogMsgType::EN_ERROR);
}

UINT32 CMainFrame::GetStepInfo(std::vector<stStepInfo*>** v)
{
	return m_docPaneCamInfo.GetStepInfo(v);
}

UINT32 CMainFrame::DisplayCamInfo()
{
	if (m_docPaneCamInfo.DisplayCamInfo() == RESULT_GOOD)
	{
		AddApplicationLog(SUCCEED_DISPLAY_CAM_INFO);
		return RESULT_GOOD;
	}
	else
	{
		AddApplicationLog(FAILED_DISPLAY_CAM_INFO, LogMsgType::EN_ERROR);
		return RESULT_BAD;
	}
}

UINT32 CMainFrame::EstimateStepOrient()
{
	return m_docPaneCamInfo.EstimateStepOrient();
}


//UINT32 CMainFrame::DisplayLayerInfo(INT32 iFrontlayerIdx, INT32 iBacklayerIdx)
UINT32 CMainFrame::DisplayLayerInfo(IN const CString& strFrontLayer, IN const CString& strBackLayer)
{
	if (!m_bIsExtractCam)
		return RESULT_BAD;

	std::vector<stStepInfo*>* pStepInfo = nullptr;
	if (m_docPaneCamInfo.GetStepInfo(&pStepInfo) != RESULT_GOOD)
	{
		AddApplicationLog(FAILED_DISPLAY_LAYER_INFO, LogMsgType::EN_ERROR);
		return RESULT_BAD;	
	}
	
	std::vector<stLayerInfo*>* pLayerInfo = nullptr;
	
	if (m_docPaneCamInfo.GetLayerInfo(&pLayerInfo) != RESULT_GOOD)
	{
		AddApplicationLog(FAILED_DISPLAY_LAYER_INFO, LogMsgType::EN_ERROR);
		return RESULT_BAD;
	}
	
	if (GetMachineType() == eMachineType::eNSIS)
	{ 
		if (m_docPaneCamInfo.CheckLayerInfo(&pLayerInfo, m_pPatternLayerInfo, m_pSRLayerInfo, m_pMPLayerInfo, m_pTPLayerInfo) != RESULT_GOOD)
		{
			AddApplicationLog(FAILED_DISPLAY_LAYER_INFO, LogMsgType::EN_ERROR);
			return RESULT_BAD;
		}
	}
		
	SystemSpec_t* pSysSpec = GetSystemSpec();
	
	CString strModel = L"";
	GetWindowText(strModel);

	CString strPath = GetCurAppPath() + JOB_DATA_PATH + strModel;

	/*if (m_docPaneLayerInfo.DisplayLayerInfo(pSysSpec, strPath, pStepInfo, pLayerInfo, &m_stUserLayerSet,
		m_stUserSetInfo.prodSide, &m_vcLayerList, iFrontlayerIdx, iBacklayerIdx) == RESULT_GOOD)*/
	if (m_docPaneLayerInfo.DisplayLayerInfo(pSysSpec, strPath, pStepInfo, pLayerInfo, &m_stUserLayerSet, &m_stUserSetInfo,
		m_stUserSetInfo.prodSide, m_pPatternLayerInfo, m_pSRLayerInfo, strFrontLayer, strBackLayer) == RESULT_GOOD)
	{
		AddApplicationLog(SUCCEED_DISPLAY_LAYER_INFO);

		//hj.kim
		EstimateStepOrient();

//		if (GetMachineType() != eMachineType::eNSIS)
//			OnLoadLayer();

		return RESULT_GOOD;
	}
	else
	{
		AddApplicationLog(FAILED_DISPLAY_LAYER_INFO, LogMsgType::EN_ERROR);
		return RESULT_BAD;
	}
	
}

void CMainFrame::SetLayerCount(UINT32& iLayerCnt)
{
	if (m_docPaneCamInfo.SetLayerCount(iLayerCnt) == RESULT_GOOD)
		AddApplicationLog(SUCCEED_SET_LAYER_INFO);
	else
		AddApplicationLog(FAILED_SET_LAYER_INFO, LogMsgType::EN_ERROR);
}

UINT32 CMainFrame::GetLayerInfo(std::vector<stLayerInfo*>** v, 
	vector<vector<stLayerInfo*>> &vecPatternList, 
	vector<vector<stLayerInfo*>> &vecSRList, 
	vector<vector<stLayerInfo*>> &vecMPList,
	vector<vector<stLayerInfo*>> &vecTPList)
{
	if (GetMachineType() == eMachineType::eNSIS)
	{
		if (RESULT_GOOD == m_docPaneCamInfo.GetLayerInfo(v))
		{
			return m_docPaneCamInfo.CheckLayerInfo(v, vecPatternList, vecSRList, vecMPList, vecTPList);
		}
		else
			return RESULT_BAD;
	}
	else
		return m_docPaneCamInfo.GetLayerInfo(v);
}

void CMainFrame::SetComponentCount(UINT32& iStepCnt, std::vector<UINT32>& vcCompCnt)
{
	if (m_docPaneCamInfo.SetComponentCount(iStepCnt, vcCompCnt) == RESULT_GOOD)
		AddApplicationLog(SUCCEED_SET_COMP_INFO);
	else
		AddApplicationLog(FAILED_SET_COMP_INFO, LogMsgType::EN_ERROR);
}

UINT32 CMainFrame::GetComponentInfo(std::vector<stComponentInfo*>** v)
{
	return m_docPaneCamInfo.GetComponentInfo(v);
}

void CMainFrame::InitializeRibbon()
{
	CString strTemp = L"";
	CMFCRibbonCategory	*pCategoryHome = nullptr, *pCategroyView = nullptr, *pCategroyEdit = nullptr;
	CMFCRibbonPanel		*pPanelHome = nullptr, *pPanelProduct = nullptr, *pPanelProcess = nullptr, *pRecipe = nullptr;
	CMFCRibbonPanel		*pManufacture = nullptr, *pPanelView = nullptr, *pPanelDirection = nullptr, *pPanelViewer = nullptr;
	CMFCRibbonPanel		*pPanelFOV = nullptr, *pPanelVersion = nullptr, *pPanelSide = nullptr;
	CMFCRibbonPanel		*pPanelEdit = nullptr , *pPanelAdd = nullptr;
	CMFCRibbonPanel		*pPanelSave = nullptr;

	m_btnRibbonApp.SetImage(IDB_MAIN);
	m_btnRibbonApp.SetText(_T("\nf")); 
	m_wndRibbonBar.SetApplicationButton(&m_btnRibbonApp, CSize(45, 45));
	
	SystemSpec_t *pSysSpec = GetSystemSpec();

	//1. Cam Master Panel	
	//BOOL bNameValid;
	//pCategoryHome = m_wndRibbonBar.AddCategory(strTemp, 0, 0);
	
	// 2022.06.03 
	// 김준호 주석추가
	// 작은 이미지 버튼 구현 테스트 위해 변경
	//pCategoryHome = m_wndRibbonBar.AddCategory(strTemp, IDB_SMALL_FEATURE, IDB_SMALL_FEATURE);	
	pCategoryHome = m_wndRibbonBar.AddCategory(pSysSpec->sysLanguage.vecCamMaster[0], IDB_SMALL_ADD_FEATURE, IDB_SMALL_ADD_FEATURE);

	//2. View Panel
	//bNameValid = strTemp.LoadString(IDS_RIB_VIEW_CATEGORY);
	//ASSERT(bNameValid);	
	pCategroyView = m_wndRibbonBar.AddCategory(pSysSpec->sysLanguage.vecHome[0], 0, 0);

	//2. View Panel
// 	bNameValid = strTemp.LoadString(IDS_RIB_EDIT_CATEGORY);
// 	ASSERT(bNameValid);
// 	pCategroyEdit = m_wndRibbonBar.AddCategory(strTemp, 0, 0);

	//1-1. Setting

// 	bNameValid = strTemp.LoadString(IDS_RIB_HOME);
// 	ASSERT(bNameValid);

	pPanelHome = pCategoryHome->AddPanel(pSysSpec->sysLanguage.vecSetting[0]);

	//CMFCRibbonButton* pBtnRefresh = CreateButton(IDS_RIB_REFRESH, IDI_EMPTY_ICON, _T("캠 데이터 갱신"));
	
	CMFCRibbonButton* pBtnRefresh = CreateButton_V2(IDS_RIB_REFRESH, pSysSpec->sysLanguage.vecRefresh[0],IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecRefresh[1]);
	pPanelHome->Add(pBtnRefresh);


	CMFCRibbonButton* pBtnSetting = CreateButton_V2(IDS_RIB_SETTING, pSysSpec->sysLanguage.vecSetting[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecSetting[1]);
	//CMFCRibbonButton* pBtnSetting = CreateButton_V2(IDS_RIB_SETTING, _T("환경 설정")); 
	pPanelHome->Add(pBtnSetting);

	//1-2. Mode
// 	bNameValid = strTemp.LoadString(IDS_RIB_MODE);
// 	ASSERT(bNameValid);
	pPanelProduct = pCategoryHome->AddPanel(pSysSpec->sysLanguage.vecMode[0]);

	if (GetMachineType() == eMachineType::eNSIS)
	{
		CMFCRibbonButton* pBtnLine = CreateButton(IDS_RIB_NSIS, IDI_EMPTY_ICON, _T("NSIS"));
		//CMFCRibbonButton* pBtnLine = CreateButton_V2(IDS_RIB_NSIS,_T("NSIS"));

		pPanelProduct->Add(pBtnLine);
	}
	else
	{
		if (GetScanType() == eScanType::eLineScan)
		{
			//CMFCRibbonButton* pBtnLine = CreateButton(IDS_RIB_LINESCAN, IDI_EMPTY_ICON, _T("Line Scan Mode"));
			CMFCRibbonButton* pBtnLine = CreateButton_V2(IDS_RIB_LINESCAN, pSysSpec->sysLanguage.vecLineScan[0],IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecLineScan[1]);

			pPanelProduct->Add(pBtnLine);
		}
		else
		{
			//CMFCRibbonButton* pBtnArea = CreateButton(IDS_RIB_AREASCAN, IDI_EMPTY_ICON, _T("Area Scan Mode"));
			CMFCRibbonButton* pBtnArea = CreateButton_V2(IDS_RIB_AREASCAN, pSysSpec->sysLanguage.vecAreaScan[0],IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecAreaScan[1]);

			pPanelProduct->Add(pBtnArea);
		}
	}	

	CMFCRibbonButton* pBtnPanel = CreateButton_V2(IDS_RIB_PANEL, pSysSpec->sysLanguage.vecPanel[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecPanel[0]);
	//CMFCRibbonButton* pBtnPanel = CreateButton_V2(IDS_RIB_PANEL, _T("Panel Mode"));

	pPanelProduct->Add(pBtnPanel);

	if (GetMachineType() != eMachineType::eNSIS)
	{
		//CMFCRibbonButton* pBtnPreview = CreateButton_V2(IDS_RIB_PREVIEW, pSysSpec->sysLanguage.vecPreview[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecPreview[1]);
		CMFCRibbonButton* pBtnPreview = CreateButton_V2(IDS_RIB_PREVIEW, pSysSpec->sysLanguage.vecPreview[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecPreview[0]);
		//CMFCRibbonButton* pBtnPreview = CreateButton_V2(IDS_RIB_PREVIEW, _T("Preview Mode"));

		pPanelProduct->Add(pBtnPreview);
	}

	if (GetMachineType() == eMachineType::eNSIS &&
		GetUserSite() == eUserSite::eSEM)
	{
// 		bNameValid = strTemp.LoadString(IDS_RIB_MANUFACTURE);
// 		ASSERT(bNameValid);
		pManufacture = pCategoryHome->AddPanel(pSysSpec->sysLanguage.vecManufacture[0]);

		//CMFCRibbonButton* pBtnPreSR = CreateButton(IDS_RIB_PRESR, IDI_EMPTY_ICON, _T("Pre-SR"));
		CMFCRibbonButton* pBtnPreSR = CreateButton_V2(IDS_RIB_PRESR, pSysSpec->sysLanguage.vecPreSR[0],IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecPreSR[0]);

		pManufacture->Add(pBtnPreSR);
		CMFCRibbonButton* pBtnPostSR = CreateButton_V2(IDS_RIB_POSTSR, pSysSpec->sysLanguage.vecPostSR[0],IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecPostSR[0]);
		//CMFCRibbonButton* pBtnPostSR = CreateButton_V2(IDS_RIB_POSTSR, _T("Post-SR"));

		pManufacture->Add(pBtnPostSR);
	}

	if (GetMachineType() == eMachineType::eNSIS)
	{
// 		bNameValid = strTemp.LoadString(IDS_RIB_SIDE);
// 		ASSERT(bNameValid);
		pPanelSide = pCategoryHome->AddPanel(pSysSpec->sysLanguage.vecSide[0]);


		CMFCRibbonButton* pBtnViewFront = CreateButton_V2(IDS_RIBBON_FRONT, pSysSpec->sysLanguage.vecFront[0],IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecFront[0]);
		//CMFCRibbonButton* pBtnViewFront = CreateButton_V2(IDS_RIBBON_FRONT, _T("Front Side"));

		pPanelSide->Add(pBtnViewFront);


		CMFCRibbonButton* pBtnViewBack = CreateButton_V2(IDS_RIBBON_BACK, pSysSpec->sysLanguage.vecBack[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecBack[0]);
		//CMFCRibbonButton* pBtnViewBack = CreateButton_V2(IDS_RIBBON_BACK, _T("Back Side"));

		pPanelSide->Add(pBtnViewBack);
	}	


	//1-3. Process
// 	bNameValid = strTemp.LoadString(IDS_RIB_PROCESS);
// 	ASSERT(bNameValid);

	pPanelProcess = pCategoryHome->AddPanel(pSysSpec->sysLanguage.vecProcess[0]);


	CMFCRibbonButton* pBtnLoadLayer = CreateButton_V2(IDS_RIB_LOAD_LAYER, pSysSpec->sysLanguage.vecLoadLayer[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecLoadLayer[1]);
	//CMFCRibbonButton* pBtnLoadLayer = CreateButton_V2(IDS_RIB_LOAD_LAYER, _T("Load Layers from CAM"));

	pPanelProcess->Add(pBtnLoadLayer);

	if (GetMachineType() == eMachineType::eNSIS)
	{

		CMFCRibbonButton* pBtnSaveMaster = CreateButton_V2(IDS_RIB_MAKE_MASTER, pSysSpec->sysLanguage.vecMakeData[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecMakeData[1]);
		//CMFCRibbonButton* pBtnSaveMaster = CreateButton_V2(IDS_RIB_MAKE_MASTER,_T("Make Master Image from CAM"));

		pPanelProcess->Add(pBtnSaveMaster);
	}
	else
	{

		//CMFCRibbonButton* pBtnSaveAlign = CreateButton(IDS_RIB_MAKE_ALIGN, IDI_EMPTY_ICON, _T("Make Align Image from CAM"));
		CMFCRibbonButton* pBtnSaveAlign = CreateButton_V2(IDS_RIB_MAKE_ALIGN, pSysSpec->sysLanguage.vecMakeAlign[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecMakeAlign[1]);

		pPanelProcess->Add(pBtnSaveAlign);

		CMFCRibbonButton* pBtnSaveMaster = CreateButton_V2(IDS_RIB_MAKE_MASTER, pSysSpec->sysLanguage.vecMakeData[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecMakeData[1]);
		//CMFCRibbonButton* pBtnSaveMaster = CreateButton_V2(IDS_RIB_MAKE_MASTER,  _T("Make Cell Image from CAM"));

		pPanelProcess->Add(pBtnSaveMaster);
	}
		

	CMFCRibbonButton* pBtnDataTransfer = CreateButton_V2(IDS_RIB_TRANSFER_MASTER, pSysSpec->sysLanguage.vecTransferData[0],IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecTransferData[1]);
	//CMFCRibbonButton* pBtnDataTransfer = CreateButton_V2(IDS_RIB_TRANSFER_MASTER,  _T("Copy Master Data to Inspection PCs"));

	pPanelProcess->Add(pBtnDataTransfer);

	//NSIS Recipe
	if (GetMachineType() == eMachineType::eNSIS &&
		GetUserSite() == eUserSite::eSEM)
	{
		//bNameValid = strTemp.LoadString(IDS_RIB_RECIPE);
		//ASSERT(bNameValid);
		pRecipe = pCategoryHome->AddPanel(pSysSpec->sysLanguage.vecRecipe[0]);
		
		//CMFCRibbonButton* pBtnSetAlign = CreateButton(IDS_RIB_SET_ALIGN, IDI_EMPTY_ICON, m_vcButtons_Pop[8]);
		CMFCRibbonButton* pBtnSetAlign = CreateButton_V2(IDS_RIB_SET_ALIGN, pSysSpec->sysLanguage.vecRecipeAlign[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecRecipeAlign[1]);
		
		//2022.09.16 KJH 주석처리
		//SNU 고성진 부장님 요청으로 UI 숨김 
		/*CMFCRibbonButton* pBtnSetAlignAuto = CreateButton_V2(IDS_RIB_SET_ALIGNAUTO, pSysSpec->sysLanguage.vecRecipeAlignAuto[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecRecipeAlignAuto[0]);
		CMFCRibbonButton* pBtnSetAlignManual = CreateButton_V2(IDS_RIB_SET_ALIGNMANUAL, pSysSpec->sysLanguage.vecRecipeAlignManual[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecRecipeAlignManual[0]);
		pBtnSetAlign->AddSubItem(pBtnSetAlignAuto);
		pBtnSetAlign->AddSubItem(pBtnSetAlignManual);*/

		pRecipe->Add(pBtnSetAlign);		
		
		//CMFCRibbonButton* pBtnSetMeasure_NSIS = CreateButton(IDS_RIB_SET_MEASURE_NSIS, IDI_EMPTY_ICON, m_vcButtons_Pop[9]);
		CMFCRibbonButton* pBtnSetMeasure_NSIS = CreateButton_V2(IDS_RIB_SET_MEASURE_NSIS, pSysSpec->sysLanguage.vecRecipeNSIS[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecRecipeNSIS[1]);

		pRecipe->Add(pBtnSetMeasure_NSIS);
				
		//CMFCRibbonButton* pBtnSetMeasure_SR = CreateButton(IDS_RIB_SET_MEASURE_SR, IDI_EMPTY_ICON, m_vcButtons_Pop[10]);
		CMFCRibbonButton* pBtnSetMeasure_SR = CreateButton_V2(IDS_RIB_SET_MEASURE_SR, pSysSpec->sysLanguage.vecRecipeSR[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecRecipeSR[1]);

		pRecipe->Add(pBtnSetMeasure_SR);	
		
		//CMFCRibbonButton* pBtnMonitorMode = CreateButton(IDS_RIB_SET_MONITOR, IDI_EMPTY_ICON, m_vcButtons_Pop[11]);
		CMFCRibbonButton* pBtnMonitorMode = CreateButton_V2(IDS_RIB_SET_MONITOR, pSysSpec->sysLanguage.vecRecipeALL[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecRecipeALL[1]);

		pRecipe->Add(pBtnMonitorMode);
	}


	// Edit 

	if (GetMachineType() != eMachineType::eNSIS)
	{
		//bNameValid = strTemp.LoadString(IDS_RIB_EDIT_EDITMODE);
		//ASSERT(bNameValid);
		//pPanelEdit = pCategroyEdit->AddPanel(strTemp);
		pPanelEdit = pCategoryHome->AddPanel(pSysSpec->sysLanguage.vecEdit[0]);


		//CMFCRibbonButton* pBtnEditMode = CreateButton(IDS_RIB_EDIT_EDITMODE, IDI_EMPTY_ICON, m_vcButtons_Pop[12]);
		CMFCRibbonButton* pBtnEditMode = CreateButton_V2(IDS_RIB_EDIT_EDITMODE, pSysSpec->sysLanguage.vecEdit[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecEdit[1]);
		pPanelEdit->Add(pBtnEditMode);


		//CMFCRibbonButton* pBtnEditAlign_Mask = CreateButton(IDS_RIB_EDIT_MASK, IDI_EMPTY_ICON, m_vcButtons_Pop[13]);
		CMFCRibbonButton* pBtnEditAlign_Mask = CreateButton_V2(IDS_RIB_EDIT_MASK, pSysSpec->sysLanguage.vecEditAlign[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecEditAlign[0]);
		//CMFCRibbonButton* pBtnEditAlign_Mask = CreateButton_V2(IDS_RIB_EDIT_MASK, _T("Align/Mask Edit"));

		pPanelEdit->Add(pBtnEditAlign_Mask);
	}

	// Feature Add 

	//bNameValid = strTemp.LoadString(IDS_RIB_ADD_SUBJECT);
	//ASSERT(bNameValid);
	pPanelAdd = pCategoryHome->AddPanel(pSysSpec->sysLanguage.vecADDFeature[0]);

	m_pAddLineX = new CMFCRibbonButton(IDS_RIB_ADD_LINE_X, _T(""), 0);
	m_pAddLineX->SetToolTipText(pSysSpec->sysLanguage.vecADDFeature[1]);
	pPanelAdd->Add(m_pAddLineX);

	CMFCRibbonButton *pAddLineY = new CMFCRibbonButton(IDS_RIB_ADD_LINE_Y, _T(""), 1);
	pAddLineY->SetToolTipText(pSysSpec->sysLanguage.vecADDFeature[2]);
	pPanelAdd->Add(pAddLineY);

	m_pAddLine = new CMFCRibbonButton(IDS_RIB_ADD_LINE, _T(""), 2);
	m_pAddLine->SetToolTipText(pSysSpec->sysLanguage.vecADDFeature[3]);
	pPanelAdd->Add(m_pAddLine);

	CMFCRibbonButton *pAddLineX_Feature = new CMFCRibbonButton(IDS_RIB_ADD_LINE_X_FEATURE, _T(""), 3);
	pAddLineX_Feature->SetToolTipText(pSysSpec->sysLanguage.vecADDFeature[4]);
	pPanelAdd->Add(pAddLineX_Feature);

	CMFCRibbonButton *pAddLineY_Feature = new CMFCRibbonButton(IDS_RIB_ADD_LINE_Y_FEATURE, _T(""), 4);
	pAddLineY_Feature->SetToolTipText(pSysSpec->sysLanguage.vecADDFeature[5]);
	pPanelAdd->Add(pAddLineY_Feature);
	

	CMFCRibbonButton *pAddArc = new CMFCRibbonButton(IDS_RIB_ADD_ARC, _T(""), 5);
	pAddArc->SetToolTipText(pSysSpec->sysLanguage.vecADDFeature[6]);
	pPanelAdd->Add(pAddArc);

	CMFCRibbonButton *pAddCircle = new CMFCRibbonButton(IDS_RIB_ADD_CIRCLE, _T(""), 6);
	pAddCircle->SetToolTipText(pSysSpec->sysLanguage.vecADDFeature[7]);
	pPanelAdd->Add(pAddCircle);


	CMFCRibbonButton *pAddRect = new CMFCRibbonButton(IDS_RIB_ADD_RECT, _T(""), 7);	
	pAddRect->SetToolTipText(pSysSpec->sysLanguage.vecADDFeature[8]);
	pPanelAdd->Add(pAddRect);

	if (GetMachineType() == eMachineType::eNSIS)
	{
		CMFCRibbonButton *pAddTp = new CMFCRibbonButton(IDS_RIB_ADD_CROSS, _T(""), 8);
		pAddTp->SetToolTipText(pSysSpec->sysLanguage.vecADDFeature[9]);
		pPanelAdd->Add(pAddTp);
	}

	CMFCRibbonButton *pAddCircleAuto = new CMFCRibbonButton(IDS_RIB_ADD_CIRCLE_AUTO, _T(""), 9);
	pAddCircleAuto->SetToolTipText(pSysSpec->sysLanguage.vecADDFeature[10]);
	pPanelAdd->Add(pAddCircleAuto);

	CMFCRibbonButton *pAddRectAuto = new CMFCRibbonButton(IDS_RIB_ADD_RECT_AUTO, _T(""), 10);
	pAddRectAuto->SetToolTipText(pSysSpec->sysLanguage.vecADDFeature[11]);
	pPanelAdd->Add(pAddRectAuto);

	if (GetMachineType() == eMachineType::eNSIS)
	{
		CMFCRibbonButton *pAddTpAuto = new CMFCRibbonButton(IDS_RIB_ADD_CROSS_AUTO, _T(""), 11);
		pAddTpAuto->SetToolTipText(pSysSpec->sysLanguage.vecADDFeature[9]);
		pPanelAdd->Add(pAddTpAuto);
	}

	CMFCRibbonButton *pAddSurface = new CMFCRibbonButton(IDS_RIB_ADD_SURFACE, _T(""), 12);
	pAddSurface->SetToolTipText(pSysSpec->sysLanguage.vecADDFeature[12]);
	pPanelAdd->Add(pAddSurface);
	
	m_pLayerComboBox = new CMFCRibbonComboBox(IDS_RIB_LAYER_COMBOBOX, FALSE, 70, 0, -1);
	pPanelAdd->Add(m_pLayerComboBox);

	
	

	// 	CMFCRibbonGallery *pBtnAddRectxr = new CMFCRibbonGallery(IDS_RIB_ADD_RECTXR, _T("ROUNDE RECTANGLE"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddRectxr);
	// 	CMFCRibbonGallery *pBtnAddRectxc = new CMFCRibbonGallery(IDS_RIB_ADD_RECTXC, _T("CHAMFERED RECTANGLE"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddRectxc);
	// 
	// 	CMFCRibbonGallery *pBtnAddOval = new CMFCRibbonGallery(IDS_RIB_ADD_OVAL, _T("OVAL"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddOval);
	// 	CMFCRibbonGallery *pBtnAddDiamon = new CMFCRibbonGallery(IDS_RIB_ADD_DIAMOND, _T("DIAMOND"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddDiamon);
	// 	CMFCRibbonGallery *pBtnAddOct = new CMFCRibbonGallery(IDS_RIB_ADD_OCTAGON, _T("OCTAGON"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddOct);
	// 	CMFCRibbonGallery *pBtnAddDonut_r = new CMFCRibbonGallery(IDS_RIB_ADD_ROUND_DONUT, _T("ROUND DONUT"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddDonut_r);
	// 	CMFCRibbonGallery *pBtnAddDonut_s = new CMFCRibbonGallery(IDS_RIB_ADD_SQUARE_DONUT, _T("SQUARE DONUT"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddDonut_s);
	// 
	// 	CMFCRibbonGallery *pBtnAddHex_l = new CMFCRibbonGallery(IDS_RIB_ADD_HORIZONTAL_HEXAGON, _T("Horizontal Hexagon"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddHex_l);
	// 	CMFCRibbonGallery *pBtnAddHex_s = new CMFCRibbonGallery(IDS_RIB_ADD_VETICAL_HEXAGON, _T("Vertical Hexagon"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddHex_s);
	// 	CMFCRibbonGallery *pBtnAddBfr = new CMFCRibbonGallery(IDS_RIB_ADD_BUTTERFLY, _T("Butterfly"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddBfr);
	// 	CMFCRibbonGallery *pBtnAddBfs = new CMFCRibbonGallery(IDS_RIB_ADD_SQUARE_BUTTERFLY, _T("Square Butterfly"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddBfs);
	// 	CMFCRibbonGallery *pBtnAddTri = new CMFCRibbonGallery(IDS_RIB_ADD_TRIANGLE, _T("Triangle"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddTri);
	// 
	// 	CMFCRibbonGallery *pBtnAddOval_h = new CMFCRibbonGallery(IDS_RIB_ADD_HALF_OVAL, _T("Half Oval"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddOval_h);
	// 	CMFCRibbonGallery *pBtnAddThr = new CMFCRibbonGallery(IDS_RIB_ADD_ROUND_THERMAL_ROUNDED, _T("Round Thermal(Rounded)"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddThr);
	// 	CMFCRibbonGallery *pBtnAddThs = new CMFCRibbonGallery(IDS_RIB_ADD_ROUND_THERMAL_SQUARED, _T("Round Thermal(Squared)"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddThs);
	// 	CMFCRibbonGallery *pBtnAddS_Ths = new CMFCRibbonGallery(IDS_RIB_ADD_SQUARE_THERMAL, _T("Square Thermal"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddS_Ths);
	// 	CMFCRibbonGallery *pBtnAddS_Tho = new CMFCRibbonGallery(IDS_RIB_ADD_SQUARE_THERMAL_OPEN_CORNERS, _T("Square Thermal(Open Corners)"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddS_Tho);
	// 
	// 	CMFCRibbonGallery *pBtnAddSr_Ths = new CMFCRibbonGallery(IDS_RIB_ADD_SQUARE_ROUND_THERMAL, _T("Square Round Thermal"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddSr_Ths);
	// 	CMFCRibbonGallery *pBtnAddRc_Ths = new CMFCRibbonGallery(IDS_RIB_ADD_RECTANGULAR_THERMAL, _T("Rectangluar Thermal"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddRc_Ths);
	// 	CMFCRibbonGallery *pBtnAddRc_Tho = new CMFCRibbonGallery(IDS_RIB_ADD_RECTANGULAR_THERMAL_OPEN_CORNERS, _T("Rectangluar Thermal(Open Corners)"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddRc_Tho);
	// 	CMFCRibbonGallery *pBtnAddEl = new CMFCRibbonGallery(IDS_RIB_ADD_ELLIPSE, _T("Ellipse"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddEl);
	// 	CMFCRibbonGallery *pBtnAddMoire = new CMFCRibbonGallery(IDS_RIB_ADD_MOIRE, _T("Moire"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddMoire);
	// 
	// 	CMFCRibbonGallery *pBtnAddHole = new CMFCRibbonGallery(IDS_RIB_ADD_HOLE, _T("Hole"), 0, 0, imamgePallette);
	// 	pPanelAdd->Add(pBtnAddHole);

	
	//1-4. Cam View
	//bNameValid = strTemp.LoadString(IDS_RIB_DISPLAY);
	//ASSERT(bNameValid);
	//pPanelView = pCategoryHome->AddPanel(m_vcButtons[2]);
	pPanelView = pCategoryHome->AddPanel(pSysSpec->sysLanguage.vecShow[0]);

	if (GetMachineType() != eMachineType::eNSIS)
	{
		//CMFCRibbonButton* pBtnViewFOV = CreateButton(IDS_RIB_FOV_VIEW, IDI_EMPTY_ICON, m_vcButtons_Pop[22]);
		CMFCRibbonButton* pBtnViewFOV = CreateButton_V2(IDS_RIB_FOV_VIEW, pSysSpec->sysLanguage.vecShowCAMFOV[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecShowCAMFOV[0]);
		//CMFCRibbonButton* pBtnViewFOV = CreateButton_V2(IDS_RIB_FOV_VIEW,  _T("Show/Hide FOV"));

		pPanelView->Add(pBtnViewFOV);

		//CMFCRibbonButton* pBtnViewCell = CreateButton(IDS_RIB_CELL_AREA, IDI_EMPTY_ICON, m_vcButtons_Pop[23]);
		CMFCRibbonButton* pBtnViewCell = CreateButton_V2(IDS_RIB_CELL_AREA, pSysSpec->sysLanguage.vecShowCAMCELL[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecShowCAMCELL[0]);
		//CMFCRibbonButton* pBtnViewCell = CreateButton_V2(IDS_RIB_CELL_AREA, _T("Show/Hide CELL"));

		pPanelView->Add(pBtnViewCell);
	}

	//CMFCRibbonButton* pBtnViewStepRepeat = CreateButton(IDS_RIB_STEP_REPEAT, IDI_EMPTY_ICON, m_vcButtons_Pop[24]);
	CMFCRibbonButton* pBtnViewStepRepeat = CreateButton_V2(IDS_RIB_STEP_REPEAT, pSysSpec->sysLanguage.vecShowAll[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecShowAll[0]);

	pPanelView->Add(pBtnViewStepRepeat);

	//CMFCRibbonButton* pBtnViewProfile = CreateButton(IDS_RIB_PROFILE_VIEW, IDI_EMPTY_ICON, m_vcButtons_Pop[25]);
	CMFCRibbonButton* pBtnViewProfile = CreateButton_V2(IDS_RIB_PROFILE_VIEW, pSysSpec->sysLanguage.vecShowProfile[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecShowProfile[1]);

	pPanelView->Add(pBtnViewProfile);

	if (GetMachineType() == eMachineType::eNSIS)
	{
		//CMFCRibbonButton* pBtnViewBoundary = CreateButton(IDS_RIB_BOUNDARY_VIEW, IDI_EMPTY_ICON, m_vcButtons_Pop[26]);
		CMFCRibbonButton* pBtnViewBoundary = CreateButton_V2(IDS_RIB_BOUNDARY_VIEW, pSysSpec->sysLanguage.vecShowDummy[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecShowDummy[1]);

		pPanelView->Add(pBtnViewBoundary);
	}

	//CMFCRibbonButton* pBtnViewSurface = CreateButton(IDS_RIB_SURFACE_VIEW, IDI_EMPTY_ICON, m_vcButtons_Pop[27]);
	CMFCRibbonButton* pBtnViewSurface = CreateButton_V2(IDS_RIB_SURFACE_VIEW, pSysSpec->sysLanguage.vecShowSurface[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecShowSurface[1]);

	pPanelView->Add(pBtnViewSurface);

	if (GetMachineType() != eMachineType::eNSIS)
	{
		//CMFCRibbonButton* pBtnViewAlign = CreateButton(IDS_RIB_VIEW_ALIGN, IDI_EMPTY_ICON, m_vcButtons_Pop[28]);
		CMFCRibbonButton* pBtnViewAlign = CreateButton_V2(IDS_RIB_VIEW_ALIGN, pSysSpec->sysLanguage.vecShowAlign[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecShowAlign[0]);

		pPanelView->Add(pBtnViewAlign);

		//CMFCRibbonButton* pBtnViewMask = CreateButton(IDS_RIB_VIEW_MASK, IDI_EMPTY_ICON, m_vcButtons_Pop[29]);
		CMFCRibbonButton* pBtnViewMask = CreateButton_V2(IDS_RIB_VIEW_MASK, pSysSpec->sysLanguage.vecShowMask[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecShowMask[0]);

		pPanelView->Add(pBtnViewMask);
	}

	//1-5. Work Direction (Rotation & Filp)
	//bNameValid = strTemp.LoadString(IDS_RIB_WORK_DIRECTION);
	//ASSERT(bNameValid);
	pPanelDirection = pCategoryHome->AddPanel(pSysSpec->sysLanguage.vecFOV[0]);
	
	//CMFCRibbonButton* pBtnViewMirrorH = CreateButton(IDS_RIB_MIRROR_H, IDI_EMPTY_ICON, m_vcButtons_Pop[30]);
	CMFCRibbonButton* pBtnViewMirrorH = CreateButton_V2(IDS_RIB_MIRROR_H, pSysSpec->sysLanguage.vecWorkDirHor[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecWorkDirHor[0]);

	pPanelDirection->Add(pBtnViewMirrorH);

	if (GetMachineType() != eMachineType::eNSIS)
	{
		//CMFCRibbonButton* pBtnViewMirrorV = CreateButton(IDS_RIB_MIRROR_V, IDI_EMPTY_ICON, m_vcButtons_Pop[31]);
		CMFCRibbonButton* pBtnViewMirrorV = CreateButton_V2(IDS_RIB_MIRROR_V, pSysSpec->sysLanguage.vecWorkDirVer[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecWorkDirVer[0]);

		pPanelDirection->Add(pBtnViewMirrorV);
	}

	//CMFCRibbonButton* pBtnView360 = CreateButton(IDS_RIB_ROTATION360, IDI_EMPTY_ICON, m_vcButtons_Pop[33]);
	CMFCRibbonButton* pBtnView360 = CreateButton_V2(IDS_RIB_ROTATION360, pSysSpec->sysLanguage.vecView360[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecView360[0]);

	pPanelDirection->Add(pBtnView360);
	//CMFCRibbonButton* pBtnView90 = CreateButton(IDS_RIB_ROTATION90, IDI_EMPTY_ICON, m_vcButtons_Pop[34]);
	CMFCRibbonButton* pBtnView90 = CreateButton_V2(IDS_RIB_ROTATION90, pSysSpec->sysLanguage.vecView90[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecView90[0]);

	pPanelDirection->Add(pBtnView90);
	//CMFCRibbonButton* pBtnView180 = CreateButton(IDS_RIB_ROTATION180, IDI_EMPTY_ICON, m_vcButtons_Pop[35]);
	CMFCRibbonButton* pBtnView180 = CreateButton_V2(IDS_RIB_ROTATION180, pSysSpec->sysLanguage.vecView180[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecView180[0]);

	pPanelDirection->Add(pBtnView180);
	//CMFCRibbonButton* pBtnView270 = CreateButton(IDS_RIB_ROTATION270, IDI_EMPTY_ICON, m_vcButtons_Pop[36]);
	CMFCRibbonButton* pBtnView270 = CreateButton_V2(IDS_RIB_ROTATION270, pSysSpec->sysLanguage.vecView270[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecView270[0]);

	pPanelDirection->Add(pBtnView270);

	if (GetMachineType() != eMachineType::eNSIS)
	{
		//1-6. FOV Setting
		//bNameValid = strTemp.LoadString(IDS_RIB_FOV_EDIT);
		//ASSERT(bNameValid);
		pPanelFOV = pCategoryHome->AddPanel(pSysSpec->sysLanguage.vecFOV[0]);

//		CMFCRibbonButton* pBtnViewHFOV = CreateButton(IDS_RIB_FOV_HSCAN, IDI_HSCAN_ICON, _T("Horizontal Sequential"));
//		pPanelFOV->Add(pBtnViewHFOV);

//		CMFCRibbonButton* pBtnViewVFOV = CreateButton(IDS_RIB_FOV_VSCAN, IDI_VSCAN_ICON, _T("Vertical Sequential"));
//		pPanelFOV->Add(pBtnViewVFOV);

//		CMFCRibbonButton* pBtnViewHFOV2 = CreateButton(IDS_RIB_FOV_HJIGJAG, IDI_HJIGJAG_ICON, _T("Horizontal JigJag"));
//		pPanelFOV->Add(pBtnViewHFOV2);

		//CMFCRibbonButton* pBtnViewVFOV2 = CreateButton(IDS_RIB_FOV_VJIGJAG, IDI_EMPTY_ICON, m_vcButtons_Pop[37]);
		CMFCRibbonButton* pBtnViewVFOV2 = CreateButton_V2(IDS_RIB_FOV_VJIGJAG, pSysSpec->sysLanguage.vecViewFOV2[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecViewFOV2[0]);

		pPanelFOV->Add(pBtnViewVFOV2);
	}

	//1-7. 저장

	//bNameValid = strTemp.LoadString(IDS_RIB_GROUP_SAVE);
	//ASSERT(bNameValid);
	pPanelSave = pCategoryHome->AddPanel(pSysSpec->sysLanguage.vecSaveCategory[0]);

	//CMFCRibbonButton* pBtnSave = CreateButton(IDS_RIB_SAVE, IDI_EMPTY_ICON, m_vcButtons[27]);
	CMFCRibbonButton* pBtnSave = CreateButton_V2(IDS_RIB_SAVE, pSysSpec->sysLanguage.vecSave[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecSave[0]);

	pPanelSave->Add(pBtnSave);

	//CMFCRibbonButton* pBtnSaveAs = CreateButton(IDS_RIB_SAVE_AS, IDI_EMPTY_ICON, m_vcButtons[28]);
	CMFCRibbonButton* pBtnSaveAs = CreateButton_V2(IDS_RIB_SAVE_AS, pSysSpec->sysLanguage.vecSaveAs[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecSaveAs[0]);

	pPanelSave->Add(pBtnSaveAs);


	//1-8. Version
	//bNameValid = strTemp.LoadString(IDS_RIB_INFO);
	//ASSERT(bNameValid);
	pPanelVersion = pCategoryHome->AddPanel(pSysSpec->sysLanguage.vecInformation[0]);

	//CMFCRibbonButton* pBtnRuler = CreateButton(IDS_RIB_RULER, IDI_EMPTY_ICON, m_vcButtons[30]);
	CMFCRibbonButton* pBtnRuler = CreateButton_V2(IDS_RIB_RULER, pSysSpec->sysLanguage.vecRuler[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecRuler[0]);

	pPanelVersion->Add(pBtnRuler);

	//CMFCRibbonButton* pBtnVer = CreateButton(IDS_RIB_VERSION, IDI_EMPTY_ICON, m_vcButtons[31]);
	CMFCRibbonButton* pBtnVer = CreateButton_V2(IDS_RIB_VERSION, pSysSpec->sysLanguage.vecVersion[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecVersion[0]);

	pPanelVersion->Add(pBtnVer);


	//2-1. Form Views
	//bNameValid = strTemp.LoadString(IDS_RIB_VIEW_FORMS);
	//ASSERT(bNameValid);
	pPanelViewer = pCategroyView->AddPanel(pSysSpec->sysLanguage.vecDocCategory[0]);

	//CMFCRibbonButton* pBtnJobView = CreateButton(IDS_RIB_VIEW_JOB_INFO, IDI_EMPTY_ICON, m_vcButtons_Pop[38]);
	CMFCRibbonButton* pBtnJobView = CreateButton_V2(IDS_RIB_VIEW_JOB_INFO, pSysSpec->sysLanguage.vecDocJob[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecDocJob[1]);

	pPanelViewer->Add(pBtnJobView);

	//CMFCRibbonButton* pBtnCamView = CreateButton(IDS_RIB_VIEW_CAM_INFO, IDI_EMPTY_ICON, m_vcButtons_Pop[39]);
	CMFCRibbonButton* pBtnCamView = CreateButton_V2(IDS_RIB_VIEW_CAM_INFO, pSysSpec->sysLanguage.vecDocCam[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecDocCam[1]);

	pPanelViewer->Add(pBtnCamView);

	//CMFCRibbonButton* pBtnLayerView = CreateButton(IDS_RIB_VIEW_LAYER_INFO, IDI_EMPTY_ICON, m_vcButtons_Pop[40]);
	CMFCRibbonButton* pBtnLayerView = CreateButton_V2(IDS_RIB_VIEW_LAYER_INFO, pSysSpec->sysLanguage.vecDocLayer[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecDocLayer[1]);

	pPanelViewer->Add(pBtnLayerView);
	
	if (GetMachineType() == eMachineType::eNSIS)
	{
		//CMFCRibbonButton* pBtnMeasureView = CreateButton(IDS_RIB_VIEW_MEASURE_INFO, IDI_EMPTY_ICON, m_vcButtons_Pop[41]);
		CMFCRibbonButton* pBtnMeasureView = CreateButton_V2(IDS_RIB_VIEW_MEASURE_INFO, pSysSpec->sysLanguage.vecDocMeasure[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecDocMeasure[1]);

		pPanelViewer->Add(pBtnMeasureView);

		//CMFCRibbonButton* pBtnMinimapView = CreateButton(IDS_RIB_VIEW_MINIMAP_VIEW, IDI_EMPTY_ICON, m_vcButtons_Pop[42]);
		CMFCRibbonButton* pBtnMinimapView = CreateButton_V2(IDS_RIB_VIEW_MINIMAP_VIEW, pSysSpec->sysLanguage.vecDocMinimap[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecDocMinimap[1]);

		pPanelViewer->Add(pBtnMinimapView);
	}

	//CMFCRibbonButton* pBtnLogView = CreateButton(IDS_RIB_VIEW_LOG, IDI_EMPTY_ICON, m_vcButtons_Pop[43]);
	CMFCRibbonButton* pBtnLogView = CreateButton_V2(IDS_RIB_VIEW_LOG, pSysSpec->sysLanguage.vecDocLog[0], IDI_EMPTY_ICON, pSysSpec->sysLanguage.vecDocLog[1]);

	pPanelViewer->Add(pBtnLogView);

	//3-1. Edit
	
	

// 
// 	CMFCRibbonButton* pBtnEditResize = CreateButton(IDS_RIB_EDIT_RESIZE, IDI_EDIT_RESIZE, _T("Resize"));
// 	pPanelEdit->Add(pBtnEditResize);
// 
// 	CMFCRibbonButton* pBtnEditMove = CreateButton(IDS_RIB_EDIT_MOVE, IDI_EDIT_MOVE, _T("Move"));
// 	pPanelEdit->Add(pBtnEditMove);
// 
// 	CMFCRibbonButton* pBtnEditDelete  = CreateButton(IDS_RIB_EDIT_DELETE, IDI_EDIT_DELETE, _T("Delete"));
// 	pPanelEdit->Add(pBtnEditDelete);
// 
// 	CMFCRibbonButton* pBtnEditAdd = CreateButton(IDS_RIB_EDIT_ADD, IDI_EDIT_ADD, _T("Add"));
// 	pPanelEdit->Add(pBtnEditAdd);
// 
// 	CMFCRibbonButton* pBtnEditCopy = CreateButton(IDS_RIB_EDIT_COPY, IDI_EDIT_ADD, _T("Copy"));
// 	pPanelEdit->Add(pBtnEditCopy);
// 
// 	CMFCRibbonButton* pBtnEditRotate = CreateButton(IDS_RIB_EDIT_ROTATE, IDI_EDIT_ADD, _T("Rotate/Mirror"));
// 	pPanelEdit->Add(pBtnEditRotate);
// 
// 	CMFCRibbonButton* pBtnEditFeature = CreateButton(IDS_RIB_EDIT_FEATURE, IDI_EDIT_ADD, _T("Edit Feature"));
// 	pPanelEdit->Add(pBtnEditFeature);

// 	CMFCRibbonButton* pBtnEditAlign_Mask = CreateButton(IDS_RIB_EDIT_MASK, IDI_EMPTY_ICON, _T("Align/Mask Edit"));
// 	//CMFCRibbonButton* pBtnEditAlign_Mask = CreateButton_V2(IDS_RIB_EDIT_MASK,  _T("Align/Mask Edit"));
// 
// 	pPanelEdit->Add(pBtnEditAlign_Mask);

	

	

	



//	CMFCRibbonButton* pBtnTestSaveMaster = CreateButton(IDS_RIB_EDIT_TEST_SAVE, IDI_MAKE_MASTER_ICON, _T("Test Unit Save"));
//	pPanelEdit->Add(pBtnTestSaveMaster);

}

CMFCRibbonButton* CMainFrame::CreateButton(INT32 iButtonID, INT32 iResIcon, CString strToolTip)
{
	CString strBtnCaption = L"";

	BOOL bNameValid = strBtnCaption.LoadString(iButtonID);
	ASSERT(bNameValid);

	CMFCRibbonButton* pBtnNew = new CMFCRibbonButton(iButtonID, strBtnCaption, AfxGetApp()->LoadIcon(iResIcon), FALSE, 0, 0, TRUE);
//	pBtnNew->SetTextAlwaysOnRight(TRUE);
	pBtnNew->SetToolTipText(strBtnCaption);	
	pBtnNew->SetDescription(strToolTip);
//	pBtnNew->SetAlwaysLargeImage();

	return pBtnNew;
}

CMFCRibbonButton* CMainFrame::CreateButton_V2(INT32 iButtonID,CString strButtonName,INT32 iResIcon,CString strToolTip)
{
	CString strBtnCaption = strButtonName;

// 	BOOL bNameValid = strBtnCaption.LoadString(iButtonID);
// 	ASSERT(bNameValid);

	CMFCRibbonButton* pBtnNew = new CMFCRibbonButton(iButtonID, strBtnCaption, AfxGetApp()->LoadIcon(iResIcon), FALSE, 0, 0, TRUE);
	//	pBtnNew->SetTextAlwaysOnRight(TRUE);
	pBtnNew->SetToolTipText(strBtnCaption);
	pBtnNew->SetDescription(strToolTip);
	//	pBtnNew->SetAlwaysLargeImage();

	return pBtnNew;
}




void CMainFrame::InitializeFont()
{
	m_StatusBarFont.CreateFont(15, 0, 0, 0, 600, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS,
		PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI Semibold");
}

void CMainFrame::ReleaseFont()
{
	m_StatusBarFont.DeleteObject();
}

BOOL CMainFrame::InitializeStatusBar()
{
	if (!m_wndStatusBar.Create(this))
		return FALSE;

	m_wndStatusBar.SetFont(&m_StatusBarFont);

	int count = 3;
	if (!m_wndStatusBar.AllocElements(count, sizeof(CMFCStatusBarPaneInfo)))
		return FALSE;

	RecalcLayout();

	m_wndStatusBar.SetPaneTextColor(0, RGB(0, 48, 86));
	m_wndStatusBar.SetPaneTextColor(1, RGB(0, 48, 86));
	m_wndStatusBar.SetPaneTextColor(2, RGB(0, 48, 86));

	int height = 19;
	HBITMAP hempty = ::CreateBitmap(1, height, 1, 4, NULL);
	m_wndStatusBar.SetPaneIcon(0, hempty, RGB(0, 0, 0));
	DeleteObject(hempty);

	CRect rect;
	m_wndStatusBar.GetWindowRect(&rect);

	int width = rect.Width() / 2;
	m_wndStatusBar.SetPaneWidth(MODEL_STATUS_POS, 250);
	m_wndStatusBar.SetPaneText(MODEL_STATUS_POS, L"Ready");

	m_wndStatusBar.SetPaneWidth(COORDINATE_STATUS_POS, 300);
	m_wndStatusBar.SetPaneText(COORDINATE_STATUS_POS, L"X = 0.0\",  Y = 0.0\"");

	m_wndStatusBar.SetPaneWidth(FOV_STATUS_POS, 500);
	m_wndStatusBar.SetPaneStyle(FOV_STATUS_POS, SBPS_STRETCH);
	m_wndStatusBar.SetPaneText(FOV_STATUS_POS, L"[SWATH or CELL]");

	m_wndStatusBar.SetBorders(NULL);

	return TRUE;
}

void CMainFrame::UpdateStatusBar(int nIndex, CString strModel)
{
	m_wndStatusBar.SetPaneText(nIndex, strModel);
}

void CMainFrame::OnRefreshButton()
{
	m_docPaneJobInfo.SetJobPath(GetCamPath());
}

void CMainFrame::OnUpdateRefreshButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnSettingButton()
{
	BOOL bShowPane = FALSE;

	if (m_docPaneSettings.IsPaneVisible() == FALSE)
		bShowPane = TRUE;

	m_docPaneSettings.ShowPane(bShowPane, FALSE, TRUE);
	m_docPaneSettings.RedrawWindow();
	m_docPaneSettings.Invalidate();
}

void CMainFrame::OnUpdateSettingButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnLineModeButton()
{
	if (GetScanType() != eScanType::eLineScan)	
		SetScanType(eScanType::eLineScan);	
}

void CMainFrame::OnUpdateLineModeButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	if (GetMachineType() == eMachineType::eNSIS)
	{
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		if (GetScanType() == eScanType::eLineScan) pCmdUI->SetCheck(TRUE);
		else pCmdUI->SetCheck(FALSE);
	}
}

void CMainFrame::OnAreaModeButton()
{
	if (GetScanType() != eScanType::eAreaScan)	
		SetScanType(eScanType::eAreaScan);	
}

void CMainFrame::OnUpdateAreaModeButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	if (GetMachineType() == eMachineType::eNSIS)
	{
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		if (GetScanType() == eScanType::eAreaScan) pCmdUI->SetCheck(TRUE);
		else pCmdUI->SetCheck(FALSE);
	}
}

void CMainFrame::OnPanelModeButton()
{
	eProductType prdType = GetProductType();
	if (prdType != eProductType::ePanel && prdType != eProductType::eStripBatch)	
		SetProductType(eProductType::ePanel);	
}

void CMainFrame::OnUpdatePanelModeButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	eProductType prdType = GetProductType();
	if (prdType == eProductType::ePanel || prdType == eProductType::eStripBatch)
		pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnLoadLayer()
{
	if (GetFlag_IsAllLayerLoaded())
		return;

	if (m_docPaneLayerInfo.SetUserLayerInfo() != RESULT_GOOD)
		return;

	LoadLayerData();

	

	if (GetMachineType() == eMachineType::eNSIS)
	{		
		m_dlgExtractLayer.SetData(GetModelName(), m_docPaneCamInfo.GetWorkStepName(),
			m_docPaneLayerInfo.GetPatternLayer(), m_docPaneLayerInfo.GetMeasureLayer(),
			m_docPaneLayerInfo.GetThicknessLayer());
		m_dlgExtractLayer.DoModal();
	}
	else
	{
		m_dlgExtractLayer.SetData(GetModelName(), m_docPaneCamInfo.GetWorkStepName(),
			m_docPaneLayerInfo.GetPatternLayer(), m_docPaneLayerInfo.GetDrillLayer(),
			m_docPaneLayerInfo.GetViaLayer(), m_stUserLayerSet.bIsPreview ? TRUE : FALSE);
		m_dlgExtractLayer.DoModal();
	}

	SetLayerName();
}

void CMainFrame::EndExtractLayer()
{
	m_dlgExtractLayer.EndExtract();
}

void CMainFrame::OnUpdateLoadLayer(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsExtractCam);
}

void CMainFrame::OnMakeMaster()
{
	if (!m_bIsExtractCam)
		return;

	SystemSpec_t* pSysSpec = GetSystemSpec();
	if (pSysSpec)
	{
		pSysSpec->sysBasic.bExceptImageData = false;
	}

	m_stUserSetInfo.strWorkLayer = L"";
	CString strWorkLayer = m_docPaneLayerInfo.GetPatternLayer();

	CString strResolution = _T("2.5㎛");
	if (GetMachineType() == eMachineType::eNSIS)
	{
		/*if (m_stMeasureData.vcAlign.size() <= 0)
		{
			AfxMessageBox(_T("판넬 얼라인 포인트가 없습니다."), MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		if (m_stMeasureData.vcUnits.size() <= 0)
		{
			AfxMessageBox(_T("유닛 얼라인 및 측정 포인트가 없습니다."), MB_OK | MB_ICONEXCLAMATION);
			return;
		}	*/	

		strWorkLayer.Trim();
		m_stUserSetInfo.strWorkLayer = strWorkLayer;

		if (m_stUserSetInfo.prodSide == eProdSide::eTop)
			strResolution = _T("Front");
		else
			strResolution = _T("Back");
	}
	else
	{
		//Pattern Layer 이름으로 WorkLayer 유지하기 위하여 주석처리함. hj.kim
		
// 		if (GetUserSite() == eUserSite::eLGIT)
// 		{
// 			int nTmp = strWorkLayer.Find(_T("L"));
// 			if (nTmp >= 0)
// 			{
// 				strWorkLayer.Delete(nTmp + 2, strWorkLayer.GetLength() - 1);
// 				strWorkLayer.Trim();
// 				m_stUserSetInfo.strWorkLayer = strWorkLayer;
// 			}
// 		}
// 		else
			m_stUserSetInfo.strWorkLayer = strWorkLayer;

		SystemSpec_t* pSysSpec = GetSystemSpec();
		if (pSysSpec)
		{
			bool bIsLine = true;
			strResolution.Format(_T("%.1f㎛"), pSysSpec->sysCamera[bIsLine].dResolution);
		}
	}


	eDirectionType tempMirrorDir = eDirectionType::DirectionTypeNone;
	double tempAngle = 0.0;
	bool tempMirror = false;
	
	if (m_stUserSetInfo.mcType == eMachineType::eNSIS && m_stUserSetInfo.userSite == eUserSite::eSEM)
	{
		if (m_stUserSetInfo.prodSide == eProdSide::eTop)
		{
			//g_pMainFrame->OnViewFrontButton();
			//g_pMainFrame->OnRotation360Button();
			tempMirrorDir = m_stUserSetInfo.mirrorDir;
			tempAngle = m_stUserSetInfo.dAngle;
			tempMirror = m_stUserSetInfo.bMirror;
					
			m_stUserSetInfo.mirrorDir = eDirectionType::DirectionTypeNone;
			m_stUserSetInfo.dAngle = 0.0;
			m_stUserSetInfo.bMirror = false;

			

		}
		else if (m_stUserSetInfo.prodSide == eProdSide::eBot)
		{
			//g_pMainFrame->OnViewBackButton();
			//g_pMainFrame->OnRotation360Button();
			tempMirrorDir = m_stUserSetInfo.mirrorDir;
			tempAngle = m_stUserSetInfo.dAngle;
			tempMirror = m_stUserSetInfo.bMirror;

			m_stUserSetInfo.mirrorDir = eDirectionType::eHorizontal;
			//m_stUserSetInfo.dAngle = 0.0;
			//m_stUserSetInfo.bMirror = true;

			// 2022.10.26 KJH ADD
			// 마스터 데이터 생성시 Back 방향 Parameter 
			m_stUserSetInfo.bMirror = pSysSpec->sysBasic.bBackMirror;
			m_stUserSetInfo.dAngle = pSysSpec->sysBasic.dBackAngle;
		}

		DrawCamData();
	}

	MakeMasterData();

	Sleep(1000);

	m_dlgMakeMaster.SetData(GetModelName(), m_docPaneCamInfo.GetWorkStepName(),
		strWorkLayer, strResolution);

	m_dlgMakeMaster.DoModal();

	if (m_stUserSetInfo.mcType == eMachineType::eNSIS && m_stUserSetInfo.userSite == eUserSite::eSEM)
	{
		m_stUserSetInfo.mirrorDir = tempMirrorDir;
		m_stUserSetInfo.dAngle = tempAngle;
		m_stUserSetInfo.bMirror = tempMirror;

		DrawCamData();
	}


	//2022.09.20 KJH ADD
	//삼성전기 고성진 부장님 요청 - 마스터 데이터 생성 버튼 클릭시 ODB Data 자동 저장
	if (GetMachineType() == eMachineType::eNSIS && GetUserSite() == eUserSite::eSEM)
	{
		//if (AfxMessageBox(_T("ODB 데이터를 저장하시겠습니까?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			SaveCamData();
		}
	}
}

void CMainFrame::OnUpdateMakeMaster(CCmdUI* pCmdUI)
{
	if (m_stUserLayerSet.nPanelStepIdx == GetCurStepIndex())
	{
		pCmdUI->Enable(m_bIsLoadLayers);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::EndMakeMaster()
{
	m_dlgMakeMaster.EndMakeMaster();
}

void CMainFrame::OnTransferMaster()
{

}

void CMainFrame::OnUpdateTransferMaster(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsProcessMaster);
}

void CMainFrame::OnViewFOVButton()
{
	m_stUserSetInfo.bShowFOV = !m_stUserSetInfo.bShowFOV;
	if (m_stUserSetInfo.bShowFOV && m_stUserSetInfo.bShowCell)
		m_stUserSetInfo.bShowCell = false;

	DrawCamData();
}

void CMainFrame::OnUpdateViewFOVButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);
	if (m_stUserSetInfo.bShowFOV) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnViewCellButton()
{
	m_stUserSetInfo.bShowCell = !m_stUserSetInfo.bShowCell;
	if (m_stUserSetInfo.bShowCell && m_stUserSetInfo.bShowFOV)
		m_stUserSetInfo.bShowFOV = false;

	DrawCamData();
}

void CMainFrame::OnUpdateViewCellButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);
	if (m_stUserSetInfo.bShowCell) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnViewStepRepeatButton() 
{
	m_stUserSetInfo.bShowStepRepeat = !m_stUserSetInfo.bShowStepRepeat;
	DrawCamData();
}

void CMainFrame::OnUpdateViewStepRepeatButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers || m_stUserLayerSet.bIsPreview);
	if (m_stUserSetInfo.bShowStepRepeat) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnViewProfileButton()
{
	m_stUserSetInfo.bShowProfile = !m_stUserSetInfo.bShowProfile;
	DrawCamData();
}

void CMainFrame::OnUpdateViewProfileButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers || m_stUserLayerSet.bIsPreview);
	if (m_stUserSetInfo.bShowProfile) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnViewSurfaceButton()
{
	m_stUserSetInfo.bShowSurface = !m_stUserSetInfo.bShowSurface;
	DrawCamData();
}

void CMainFrame::OnUpdateViewSurfaceButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers || m_stUserLayerSet.bIsPreview);
	if (m_stUserSetInfo.bShowSurface) pCmdUI->SetCheck(FALSE);
	else pCmdUI->SetCheck(TRUE);
}

void CMainFrame::OnViewBoundaryButton()
{
	m_stUserSetInfo.bShowPanelDummy = !m_stUserSetInfo.bShowPanelDummy;
	DrawCamData();
}

void CMainFrame::OnUpdateViewBoundaryButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers || m_stUserLayerSet.bIsPreview);
	if (m_stUserSetInfo.bShowPanelDummy) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnViewAlignButton()
{
	m_stUserSetInfo.bShowUnitAlign = !m_stUserSetInfo.bShowUnitAlign;

	if (m_stUserSetInfo.bShowUnitAlign)
	{
		m_stUserSetInfo.bShowStepRepeat = true;
		m_stUserSetInfo.bShowCell = true;
	}
	else
	{
		m_stUserSetInfo.bShowStepRepeat = false;
		m_stUserSetInfo.bShowCell = false;
	}

	DrawCamData();
}

void CMainFrame::OnUpdateViewAlignButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);
	if (m_stUserSetInfo.bShowUnitAlign) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnViewMaskButton()
{
	m_stUserSetInfo.bShowInspMask = !m_stUserSetInfo.bShowInspMask;

	if (m_stUserSetInfo.bShowInspMask)
	{
		m_stUserSetInfo.bShowStepRepeat = true;
		m_stUserSetInfo.bShowCell = true;
	}
	else
	{
		m_stUserSetInfo.bShowStepRepeat = false;
		m_stUserSetInfo.bShowCell = false;
	}

	DrawCamData();
}

void CMainFrame::OnUpdateViewMaskButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);
	if (m_stUserSetInfo.bShowInspMask) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnMirrorHButton()
{
	m_bMirrorH = !m_bMirrorH;	
	m_stUserSetInfo.bMirror = m_bMirrorH | m_bMirrorV;

	if (m_bMirrorH)
	{
		m_stUserSetInfo.mirrorDir = eDirectionType::eHorizontal;
		if (m_bMirrorV)
			m_stUserSetInfo.mirrorDir = eDirectionType::eBoth;
	}
	else
	{
		m_stUserSetInfo.mirrorDir = eDirectionType::DirectionTypeNone;
		if (m_bMirrorV)
			m_stUserSetInfo.mirrorDir = eDirectionType::eVertical;
	}

	if (GetMachineType() == eMachineType::eNSIS)
	{
		Update_NSIS_MeasureData();
	}
	DrawCamData();
}

void CMainFrame::OnUpdateMirrorHButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);
	if (m_stUserSetInfo.bMirror == false)
	{
		m_bMirrorH = false;
		m_bMirrorV = false;
	}
	else
	{
		if (m_stUserSetInfo.mirrorDir == eDirectionType::eBoth)
		{
			m_bMirrorH = true;
			m_bMirrorV = true;
		}
		else if (m_stUserSetInfo.mirrorDir == eDirectionType::eHorizontal)
		{
			m_bMirrorH = true;
			m_bMirrorV = false;
		}
		else if (m_stUserSetInfo.mirrorDir == eDirectionType::eVertical)
		{
			m_bMirrorH = false;
			m_bMirrorV = true;
		}
	}

	if (m_bMirrorH) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnMirrorVButton()
{
	m_bMirrorV = !m_bMirrorV;
	m_stUserSetInfo.bMirror = m_bMirrorH | m_bMirrorV;

	if (m_bMirrorV)
	{
		m_stUserSetInfo.mirrorDir = eDirectionType::eVertical;
		if (m_bMirrorH)
			m_stUserSetInfo.mirrorDir = eDirectionType::eBoth;
	}
	else
	{
		m_stUserSetInfo.mirrorDir = eDirectionType::DirectionTypeNone;
		if (m_bMirrorH)
			m_stUserSetInfo.mirrorDir = eDirectionType::eHorizontal;
	}

	if (GetMachineType() == eMachineType::eNSIS)
	{
		Update_NSIS_MeasureData();
	}
	DrawCamData();
}

void CMainFrame::OnUpdateMirrorVButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);
	if (m_stUserSetInfo.bMirror == false)
	{
		m_bMirrorH = false;
		m_bMirrorV = false;
	}
	else
	{
		if (m_stUserSetInfo.mirrorDir == eDirectionType::eBoth)
		{
			m_bMirrorH = true;
			m_bMirrorV = true;
		}
		else if (m_stUserSetInfo.mirrorDir == eDirectionType::eHorizontal)
		{
			m_bMirrorH = true;
			m_bMirrorV = false;
		}
		else if (m_stUserSetInfo.mirrorDir == eDirectionType::eVertical)
		{
			m_bMirrorH = false;
			m_bMirrorV = true;
		}
	}

	if (m_bMirrorV)	pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnRotation90Button()
{
	m_stUserSetInfo.dAngle = 90.0;
	if (GetMachineType() == eMachineType::eNSIS)
	{
		Update_NSIS_MeasureData();
	}
	DrawCamData();

	m_pAlignMaskEditDlg->ChangeButtonUI();
}

void CMainFrame::OnUpdateRotation90Button(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);
	if (m_stUserSetInfo.dAngle == 90.0) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnRotation180Button()
{
	m_stUserSetInfo.dAngle = 180.0;
	if (GetMachineType() == eMachineType::eNSIS)
	{
		Update_NSIS_MeasureData();
	}
	DrawCamData();

	m_pAlignMaskEditDlg->ChangeButtonUI();
}

void CMainFrame::OnUpdateRotation180Button(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);
	if (m_stUserSetInfo.dAngle == 180.0) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnRotation270Button()
{
	m_stUserSetInfo.dAngle = 270.0;
	if (GetMachineType() == eMachineType::eNSIS)
	{
		Update_NSIS_MeasureData();
	}
	DrawCamData();

	m_pAlignMaskEditDlg->ChangeButtonUI();
}

void CMainFrame::OnUpdateRotation270Button(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);
	if (m_stUserSetInfo.dAngle == 270.0) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnRotation360Button()
{
	m_stUserSetInfo.dAngle = 0.0;
	if (GetMachineType() == eMachineType::eNSIS)
	{
		Update_NSIS_MeasureData();
	}
	DrawCamData();

	m_pAlignMaskEditDlg->ChangeButtonUI();
}

void CMainFrame::OnUpdateRotation360Button(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);
	if (m_stUserSetInfo.dAngle == 0.0) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnFovHScanButton()
{
	m_stUserSetInfo.fovType = eFovType::eHorizontalSequential;
	DrawCamData();
}

void CMainFrame::OnUpdateFovHScanButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
	if (m_stUserSetInfo.fovType == eFovType::eHorizontalSequential) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnFovVScanButton()
{
	m_stUserSetInfo.fovType = eFovType::eVerticalSequential;
	DrawCamData();
}

void CMainFrame::OnUpdateFovVScanButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
	if (m_stUserSetInfo.fovType == eFovType::eVerticalSequential) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnFovHJigjagButton()
{
	m_stUserSetInfo.fovType = eFovType::eHorizontalJigjag;
	DrawCamData();
}

void CMainFrame::OnUpdateFovHJigjagButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(FALSE);
	if (m_stUserSetInfo.fovType == eFovType::eHorizontalJigjag) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnFovVJigjagButton()
{
	m_stUserSetInfo.fovType = eFovType::eVerticalJigjag;
	DrawCamData();
}

void CMainFrame::OnUpdateFovVJigjagButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);
	if (m_stUserSetInfo.fovType == eFovType::eVerticalJigjag) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnVersionButton()
{

}

void CMainFrame::OnRulerButton()
{
	if (m_stUserSetInfo.coordUnit == eCoordinateUnit::eMMToInch)
		m_stUserSetInfo.coordUnit = eCoordinateUnit::eInchToMM;
	else
		m_stUserSetInfo.coordUnit = eCoordinateUnit::eMMToInch;	
}

void CMainFrame::OnUpdateRulerButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);
	if (m_stUserSetInfo.coordUnit == eCoordinateUnit::eInchToMM)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnSaveButton()
{
	SaveCamData();	

	//2022.09.20 KJH ADD
	//삼성전기 고성진 부장님 요청 - Save 버튼 클릭시 XML 데이터 생성 팝업 표시
	if (GetMachineType() == eMachineType::eNSIS && GetUserSite() == eUserSite::eSEM)
	{
		if (AfxMessageBox(_T("마스터 데이터를 생성하시겠습니까?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			OnMakeMaster();
		}		
	}

}

void CMainFrame::OnUpdateSaveButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);
}

void CMainFrame::OnSaveAsButton()
{
	CSaveAsDlg dlgSaveAs(GetJobPtr()->GetModelName());
	if (dlgSaveAs.DoModal() == IDOK)
	{
		CString strNewJobName = dlgSaveAs.GetJobName();
		SaveAsCamData(strNewJobName);
	}
	
}

void CMainFrame::OnUpdateSaveAsButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);
}

void CMainFrame::onAddSurfaceButton()
{
	UINT32 nEditMode = EditMode::enumMode_EditMode;
	SetViewerEditMode(nEditMode);

	ResetSelect();

	SetPointMode(TRUE,0,_T("Surface Start"));
	m_strType = _T("Surface");

}

void CMainFrame::OnUpdateSurfaceButton(CCmdUI* pCmdUI)
{
	
	UserSetInfo_t *pUserSetInfo = GetUserSetInfo();

	if (GetMachineType() == eMachineType::eNSIS)
	{
		if (pUserSetInfo->rcpMode == eRecipeMode::eRecipe_Monitor_Panel)
			pCmdUI->Enable(FALSE);
		else
			pCmdUI->Enable(m_bEditMode);
	}
	else
	{
		pCmdUI->Enable(m_bEditMode);
	}

	//pCmdUI->Enable(m_bEditMode);
}

void CMainFrame::OnAddTpButton()
{
	UINT32 nEditMode = EditMode::enumMode_EditTP;
	//UINT32 nEditMode = EditMode::enumMode_EditMode;		
	SetViewerEditMode(nEditMode);

	ResetSelect();
	
	//SetPointMode(TRUE, 0, _T("Cross Center"));
	//m_strType = _T("TP");
	m_bCheckSR = !m_bCheckSR;

	if (m_bCheckSRAuto == TRUE)
	{
		m_bCheckSRAuto = FALSE;
	}
}

void CMainFrame::OnUpdateTpButton(CCmdUI* pCmdUI)
{
	UserSetInfo_t *pUserSetInfo = GetUserSetInfo();

	if (GetMachineType() == eMachineType::eNSIS)
	{
		if (m_bIsLoadLayers == FALSE || pUserSetInfo->rcpMode == eRecipeMode::eRecipe_Monitor_Panel)
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			pCmdUI->Enable(!m_bEditMode);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}	
	pCmdUI->SetCheck(m_bCheckSR);
}


void CMainFrame::OnAddTpAutoButton()
{
	UINT32 nEditMode = EditMode::enumMode_EditTP_Center;
	//UINT32 nEditMode = EditMode::enumMode_EditMode;		
	SetViewerEditMode(nEditMode);

	ResetSelect();

	//SetPointMode(TRUE, 0, _T("Cross Center"));
	//m_strType = _T("TP");
	m_bCheckSRAuto = !m_bCheckSRAuto;

	if (m_bCheckSR == TRUE)
	{
		m_bCheckSR = FALSE;
	}
}

void CMainFrame::OnUpdateTpAutoButton(CCmdUI* pCmdUI)
{
	UserSetInfo_t *pUserSetInfo = GetUserSetInfo();

	if (GetMachineType() == eMachineType::eNSIS)
	{
		if (m_bIsLoadLayers == FALSE || pUserSetInfo->rcpMode == eRecipeMode::eRecipe_Monitor_Panel)
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			pCmdUI->Enable(!m_bEditMode);
		}
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}

	pCmdUI->SetCheck(m_bCheckSRAuto);
}

void CMainFrame::OnAddArcButton()
{
	UINT32 nEditMode = EditMode::enumMode_EditMode;
	SetViewerEditMode(nEditMode);

	ResetSelect();


	SetPointMode(TRUE, 0, _T("Arc Start"));
	m_strType = _T("Arc");
}

void CMainFrame::OnUpdateArcButton(CCmdUI* pCmdUI)
{
	UserSetInfo_t *pUserSetInfo = GetUserSetInfo();

	if (GetMachineType() == eMachineType::eNSIS)
	{
		if (pUserSetInfo->rcpMode == eRecipeMode::eRecipe_Monitor_Panel)
			pCmdUI->Enable(FALSE);
		else
			pCmdUI->Enable(m_bEditMode);
	}
	else
	{
		pCmdUI->Enable(m_bEditMode);
	}
}

void CMainFrame::OnAddLineButton()
{
	UINT32 nEditMode = EditMode::enumMode_EditMode;
	SetViewerEditMode(nEditMode);

	ResetSelect();

	SetPointMode(TRUE, 0, _T("Line Start"));
	m_strType = _T("Line");
	m_nLineMode = AddLineMode::enumLineMode_XY;
}

void CMainFrame::OnUpdateLineButton(CCmdUI* pCmdUI)
{
	UserSetInfo_t *pUserSetInfo = GetUserSetInfo();

	if (GetMachineType() == eMachineType::eNSIS)
	{
		if (pUserSetInfo->rcpMode == eRecipeMode::eRecipe_Monitor_Panel)
			pCmdUI->Enable(FALSE);
		else
			pCmdUI->Enable(m_bEditMode);
	}
	else
	{
		pCmdUI->Enable(m_bEditMode);
	}
}


void CMainFrame::OnAddCircleButton()
{
	UINT32 nEditMode = EditMode::enumMode_EditMode;
	SetViewerEditMode(nEditMode);
	//AddApplicationLog(START_EDIT_MASK_TXT);

	ResetSelect();

	SetPointMode(TRUE, 0, _T("Pad Center"));
	m_strType = _T("Pad");
	m_strSymbol = _T("Circle");

}
void CMainFrame::OnUpdateCircleButton(CCmdUI* pCmdUI)
{
	
	UserSetInfo_t *pUserSetInfo = GetUserSetInfo();

	if (GetMachineType() == eMachineType::eNSIS)
	{
		if (pUserSetInfo->rcpMode == eRecipeMode::eRecipe_Monitor_Panel)
			pCmdUI->Enable(FALSE);
		else
			pCmdUI->Enable(m_bEditMode);
	}
	else
	{
		pCmdUI->Enable(m_bEditMode);
	}
}

void CMainFrame::OnAddRectangleButton()
{
	UINT32 nEditMode = EditMode::enumMode_EditMode;
	SetViewerEditMode(nEditMode);
	ResetSelect();

	SetPointMode(TRUE, 0, _T("Pad Center"));
	m_strType = _T("Pad");
	m_strSymbol = _T("Rectangle");

}

void CMainFrame::OnUpdateRectagneButton(CCmdUI* pCmdUI)
{
	UserSetInfo_t *pUserSetInfo = GetUserSetInfo();

	if (GetMachineType() == eMachineType::eNSIS)
	{
		if (pUserSetInfo->rcpMode == eRecipeMode::eRecipe_Monitor_Panel)
			pCmdUI->Enable(FALSE);
		else
			pCmdUI->Enable(m_bEditMode);
	}
	else
	{
		pCmdUI->Enable(m_bEditMode);
	}
}

void CMainFrame::OnAddLineXButton()
{
	UINT32 nEditMode = EditMode::enumMode_EditMode;
	SetViewerEditMode(nEditMode);
	ResetSelect();

	SetPointMode(TRUE, 0, _T("Line Start"));
	m_strType = _T("Line_X");
	m_nLineMode = AddLineMode::enumLineMode_X;
}

void CMainFrame::OnUpdateAddLineXButton(CCmdUI* pCmdUI)
{	
	UserSetInfo_t *pUserSetInfo = GetUserSetInfo();

	if (GetMachineType() == eMachineType::eNSIS)
	{
		if (pUserSetInfo->rcpMode == eRecipeMode::eRecipe_Monitor_Panel)
			pCmdUI->Enable(FALSE);
		else
			pCmdUI->Enable(m_bEditMode);
	}
	else
	{
		pCmdUI->Enable(m_bEditMode);
	}
}

void CMainFrame::OnAddLineYButton()
{
	UINT32 nEditMode = EditMode::enumMode_EditMode;
	SetViewerEditMode(nEditMode);
	ResetSelect();

	SetPointMode(TRUE, 0, _T("Line Start"));
	m_strType = _T("Line_Y");
	m_nLineMode = AddLineMode::enumLineMode_Y;
}

void CMainFrame::OnUpdateLineYButton(CCmdUI* pCmdUI)
{
	UserSetInfo_t *pUserSetInfo = GetUserSetInfo();

	if (GetMachineType() == eMachineType::eNSIS)
	{
		if (pUserSetInfo->rcpMode == eRecipeMode::eRecipe_Monitor_Panel)
			pCmdUI->Enable(FALSE);
		else
			pCmdUI->Enable(m_bEditMode);
	}
	else
	{
		pCmdUI->Enable(m_bEditMode);
	}
}

void CMainFrame::OnAddLineX_FeatureBaseButton()
{
	UINT32 nEditMode = EditMode::enumMode_EditAdd;
	SetViewerEditMode(nEditMode);
	ResetSelect();

	SetPointMode(TRUE, 0, _T("Line End"));
	m_strType = _T("Line_X_FEATURE");
	m_nLineMode = AddLineMode::enumLineMode_X;
}

void CMainFrame::OnUpdateLineX_FeatureBaseButton(CCmdUI* pCmdUI)
{
	UserSetInfo_t *pUserSetInfo = GetUserSetInfo();
	
	if (GetMachineType() == eMachineType::eNSIS)
	{
		if (pUserSetInfo->rcpMode == eRecipeMode::eRecipe_Monitor_Panel ||
			pUserSetInfo->rcpMode == eRecipeMode::eRecipe_MeasureSR_Panel ||
			pUserSetInfo->rcpMode == eRecipeMode::eRecipe_Align_Panel)
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			pCmdUI->Enable(m_bEditMode);
		}
	}
	else
	{
		pCmdUI->Enable(m_bEditMode);
	}
}

void CMainFrame::OnAddLineY_FeatureBaseButton()
{
	UINT32 nEditMode = EditMode::enumMode_EditAdd;
	SetViewerEditMode(nEditMode);
	ResetSelect();

	SetPointMode(TRUE, 0, _T("Line End"));
	m_strType = _T("Line_Y_FEATURE");
	m_nLineMode = AddLineMode::enumLineMode_Y;
}

void CMainFrame::OnUpdateLineY_FeatureBaseButton(CCmdUI* pCmdUI)
{
	UserSetInfo_t *pUserSetInfo = GetUserSetInfo();

	if (GetMachineType() == eMachineType::eNSIS)
	{
		if (pUserSetInfo->rcpMode == eRecipeMode::eRecipe_Monitor_Panel ||
			pUserSetInfo->rcpMode == eRecipeMode::eRecipe_MeasureSR_Panel ||
			pUserSetInfo->rcpMode == eRecipeMode::eRecipe_Align_Panel)
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			pCmdUI->Enable(m_bEditMode);
		}
	}
	else
	{
		pCmdUI->Enable(m_bEditMode);
	}
}

void CMainFrame::OnAddCircle_FeatureBaseButton()
{
	UINT32 nEditMode = EditMode::enumMode_EditAdd;
	SetViewerEditMode(nEditMode);
	ResetSelect();

	SetPointMode(TRUE, 0, _T("Pad Center"));
	m_strType = _T("Pad_FEATURE");
	m_strSymbol = _T("Circle");
}

void CMainFrame::OnUpdateCircle_FeatureBaseButton(CCmdUI* pCmdUI)
{
	UserSetInfo_t *pUserSetInfo = GetUserSetInfo();

	if (GetMachineType() == eMachineType::eNSIS)
	{
		if (pUserSetInfo->rcpMode == eRecipeMode::eRecipe_Monitor_Panel ||
			pUserSetInfo->rcpMode == eRecipeMode::eRecipe_MeasureSR_Panel ||
			pUserSetInfo->rcpMode == eRecipeMode::eRecipe_Align_Panel)
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			pCmdUI->Enable(m_bEditMode);
		}
	}
	else
	{
		pCmdUI->Enable(m_bEditMode);
	}
}

void CMainFrame::OnAddRect_FeatureBaseButton()
{
	UINT32 nEditMode = EditMode::enumMode_EditAdd;
	SetViewerEditMode(nEditMode);
	ResetSelect();

	SetPointMode(TRUE, 0, _T("Pad Center"));
	m_strType = _T("Pad_FEATURE");
	m_strSymbol = _T("Rectangle");
}

void CMainFrame::OnUpdateRect_FeatureBaseButton(CCmdUI* pCmdUI)
{
	UserSetInfo_t *pUserSetInfo = GetUserSetInfo();

	if (GetMachineType() == eMachineType::eNSIS)
	{
		if (pUserSetInfo->rcpMode == eRecipeMode::eRecipe_Monitor_Panel ||
			pUserSetInfo->rcpMode == eRecipeMode::eRecipe_MeasureSR_Panel ||
			pUserSetInfo->rcpMode == eRecipeMode::eRecipe_Align_Panel)
		{
			pCmdUI->Enable(FALSE);
		}
		else
		{
			pCmdUI->Enable(m_bEditMode);
		}
	}
	else
	{
		pCmdUI->Enable(m_bEditMode);
	}
}

// void CMainFrame::OnAddSquareButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 	m_strType = _T("Pad");
// 	m_strSymbol = _T("Square");
// 
// }
// void CMainFrame::OnAddRectxrButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 	m_nIndex = 3;
// }
// 
// void CMainFrame::OnAddRectxcButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 
// 	m_nIndex = 4;
// }
// 
// void CMainFrame::OnAddOvalButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 
// 	m_nIndex = 5;
// }
// 
// void CMainFrame::OnAddDiamondButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 
// 	m_nIndex = 6;
// }
// 
// void CMainFrame::OnAddOctagonButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 
// 	m_nIndex = 7;
// }
// 
// void CMainFrame::OnAddDonut_rButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 
// 	m_nIndex = 8;
// }
// 
// void CMainFrame::OnAddDonut_sButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 
// 	m_nIndex = 9;
// }
// 
// void CMainFrame::OnAddHex_lButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 
// 	m_nIndex = 10;
// }
// 
// void CMainFrame::OnAddHex_sButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 
// 	m_nIndex = 11;
// }
// 
// void CMainFrame::OnAddBfrButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 
// 	m_nIndex = 12;
// }
// 
// void CMainFrame::OnAddBfsButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 
// 	m_nIndex = 13;
// }
// void CMainFrame::OnAddTriButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 
// 	m_nIndex = 14;
// }
// 
// void CMainFrame::OnAddOval_hButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 
// 	m_nIndex = 15;
// }
// 
// void CMainFrame::OnAddThrButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 
// 	m_nIndex = 16;
// }
// 
// void CMainFrame::OnAddThsButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 
// 	m_nIndex = 17;
// }
// 
// void CMainFrame::OnAddS_ThsButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 
// 	m_nIndex = 18;
// }
// 
// void CMainFrame::OnAddS_ThoButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 
// 	m_nIndex = 19;
// }
// 
// void CMainFrame::OnAddSr_ThsButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 
// 	m_nIndex = 20;
// }
// 
// void CMainFrame::OnAddRc_ThsButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 
// 	m_nIndex = 21;
// }
// 
// void CMainFrame::OnAddRc_ThoButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 
// 	m_nIndex = 22;
// }
// 
// void CMainFrame::OnAddElButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 
// 	m_nIndex = 23;
// }
// 
// void CMainFrame::OnAddMorieButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 
// 	m_nIndex = 24;
// }
// 
// void CMainFrame::OnAddHoleButton()
// {
// 	SetPointMode(TRUE, 0, _T("Pad Center"));
// 
// 	m_nIndex = 25;
//}


//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnEditModeButton()
{
	UINT32 nEditMode = GetViewerEditMode();

	BOOL bEditMode = FALSE;
	if (nEditMode >= EditMode::enumMode_EditMode && nEditMode < EditMode::enumMode_Max)
	{
		//bEditMode = TRUE;
		m_bEditMode = TRUE;
	}

	//bEditMode = !bEditMode;
	m_bEditMode = !m_bEditMode;

	//if (bEditMode == TRUE)
	if(m_bEditMode == TRUE)
	{
		


		if (GetMachineType() == eMachineType::eNSIS)
		{
			m_stUserSetInfo.bShowSurface = true;
			m_stUserSetInfo.bShowStepRepeat = false;
			m_stUserSetInfo.bShowPanelDummy = true;
			
		}
		else
		{
			m_stUserSetInfo.bShowSurface = false;
			m_stUserSetInfo.bShowStepRepeat = false;
		}
	}
	
	
	//SetViewerEditMode(bEditMode);
	SetViewerEditMode(m_bEditMode);


	DrawCamData(FALSE);

	if (bEditMode)
		AddApplicationLog(START_EDIT_MODE_TXT);
	else
		AddApplicationLog(END_EDIT_MODE_TXT);

	
}

UINT32 CMainFrame::GetLineInfo_PadBase(IN const SELECT_INFO_UNIT &stUnitInfo, IN const int &nLineMode,
	OUT float &fXs, OUT float &fYs, OUT float &fXe, OUT float &fYe)
{
	float fXc = 0.f, fYc = 0.f;
	float fWidth = 0.f;
	float fHeight = 0.f;

	fXc = static_cast<float>(stUnitInfo.dCx);
	fYc = static_cast<float>(stUnitInfo.dCy);
	fWidth = static_cast<float>(stUnitInfo.dWidth);
	fHeight = static_cast<float>(stUnitInfo.dHeight);

	if (nLineMode == AddLineMode::enumLineMode_X)
	{
		if (m_stUserSetInfo.dAngle == 90. || m_stUserSetInfo.dAngle == 270.)
		{
			fXs = fXc;
			fYs = fYc - (fHeight / 2);
			fXe = fXc;
			fYe = fYc + (fHeight / 2);
		}
		else
		{
			fXs = fXc - (fWidth / 2);
			fYs = fYc;
			fXe = fXc + (fWidth / 2);
			fYe = fYc;
		}
	}
	else//nLineMode == AddLineMode::enumLineMode_Y
	{
		if (m_stUserSetInfo.dAngle == 90. || m_stUserSetInfo.dAngle == 270.)
		{
			fXs = fXc - (fWidth / 2);
			fYs = fYc;
			fXe = fXc + (fWidth / 2);
			fYe = fYc;
		}
		else
		{
			fXs = fXc;
			fYs = fYc - (fHeight / 2);
			fXe = fXc;
			fYe = fYc + (fHeight / 2);
		}
	}

	return RESULT_GOOD;
}

UINT32 CMainFrame::GetLineInfo_LineBase(IN SELECT_INFO_UNIT &stUnitInfo, IN const int &nLineMode,
	OUT float &fXs, OUT float &fYs, OUT float &fXe, OUT float &fYe)
{
	double dXc = 0., dYc = 0.;
	float fWidth = 0.f;
	float fHeight = 0.f;

	dXc = stUnitInfo.dCx;
	dYc = stUnitInfo.dCy;

	CFeature *pFeature = stUnitInfo.pFeature;
	if (pFeature == nullptr) return RESULT_BAD;
	if (pFeature->m_eType != FeatureType::L) return RESULT_BAD;

	CFeatureL *pFeatureL = (CFeatureL*)pFeature;

	double dFeature_Xs = pFeatureL->m_dXs;
	double dFeature_Ys = pFeatureL->m_dYs;
	double dFeature_Xe = pFeatureL->m_dXe;
	double dFeature_Ye = pFeatureL->m_dYe;

	if (pFeatureL->m_dXs == pFeatureL->m_dXe &&
		pFeatureL->m_dYs == pFeatureL->m_dYe)
	{//선을 만들수 없는 경우 
		return RESULT_BAD;
	}

	stUnitInfo.arrRotMirr.FinalPoint(&dFeature_Xs, &dFeature_Ys, dFeature_Xs, dFeature_Ys);
	stUnitInfo.arrRotMirr.FinalPoint(&dFeature_Xe, &dFeature_Ye, dFeature_Xe, dFeature_Ye);

	_Line2 stLine_Feature;
	CDrawMakePoint::GetLine(dFeature_Xs, dFeature_Ys, dFeature_Xe, dFeature_Ye, stLine_Feature);

	D2D1_POINT_2F fptClickPoint = D2D1::Point2F(DoubleToFloat(dXc), DoubleToFloat(dYc));
	D2D1_POINT_2F fptCrossPoint;
	CDrawMakePoint::GetCrossPoint_LNtoPT(stLine_Feature, fptClickPoint, fptCrossPoint);

	//대상 feature의 Angle을 얻어 온다.
	double dAngle = CDrawMakePoint::GetAngle(dFeature_Xs, dFeature_Ys,
		dFeature_Xe, dFeature_Ye);

	if (dAngle > 180.) dAngle -= 360.;
	if (dAngle < -180.) dAngle += 360.;

	//대상 Feature의 선 굵기
	double dWidth = pFeatureL->m_pSymbol->m_vcParams[0];
	
	//
	D2D1_POINT_2F fptCenter = D2D1::Point2F(DoubleToFloat(fptCrossPoint.x), DoubleToFloat(fptCrossPoint.y));
	D2D1_POINT_2F fptOrgPoint = D2D1::Point2F(DoubleToFloat(fptCenter.x+(dWidth/2.)), fptCenter.y);
	
	Orient eOrient = stUnitInfo.arrRotMirr.FinalOrient();
	double dAngle_rot;
	bool bMirror;
	CDrawFunction::GetAngle(eOrient, dAngle_rot, bMirror);

	//Set Rotate
	dAngle += 90;
	if (dAngle > 180.) dAngle -= 360.;
	if (dAngle < -180.) dAngle += 360.;
// 	if (dAngle >= 360.) dAngle -= 360.;
// 	if (dAngle < 0.) dAngle += 360.;
	dAngle *= -1;
	D2D1_POINT_2F fptStart = CDrawMakePoint::Rotate(fptOrgPoint, fptCenter, dAngle);
	
	dAngle += 180.;
	if (dAngle > 180.) dAngle -= 360.;
	if (dAngle < -180.) dAngle += 360.;
// 	if (dAngle >= 360.) dAngle -= 360.;
// 	if (dAngle < 0.) dAngle += 360.;
	
	D2D1_POINT_2F fptEnd = CDrawMakePoint::Rotate(fptOrgPoint, fptCenter, dAngle);

	fXs = fptStart.x;
	fYs = fptStart.y;
	fXe = fptEnd.x;
	fYe = fptEnd.y;

	return RESULT_GOOD;
}

UINT32 CMainFrame::GetLineInfo_ArcBase(IN SELECT_INFO_UNIT &stUnitInfo, IN const int &nLineMode,
	OUT float &fXs, OUT float &fYs, OUT float &fXe, OUT float &fYe)
{
	double dXc = 0., dYc = 0.;
	float fWidth = 0.f;
	float fHeight = 0.f;

	dXc = stUnitInfo.dCx;
	dYc = stUnitInfo.dCy;

	CFeature *pFeature = stUnitInfo.pFeature;
	if (pFeature == nullptr) return RESULT_BAD;
	if (pFeature->m_eType != FeatureType::A) return RESULT_BAD;

	CFeatureA *pFeatureA = (CFeatureA*)pFeature;

	double dFeature_Xs = pFeatureA->m_dXs;
	double dFeature_Ys = pFeatureA->m_dYs;
	double dFeature_Xe = pFeatureA->m_dXe;
	double dFeature_Ye = pFeatureA->m_dYe;
	double dFeature_Xc = pFeatureA->m_dXc;
	double dFeature_Yc = pFeatureA->m_dYc;

	stUnitInfo.arrRotMirr.FinalPoint(&dFeature_Xs, &dFeature_Ys, dFeature_Xs, dFeature_Ys);
	stUnitInfo.arrRotMirr.FinalPoint(&dFeature_Xe, &dFeature_Ye, dFeature_Xe, dFeature_Ye);
	stUnitInfo.arrRotMirr.FinalPoint(&dFeature_Xc, &dFeature_Yc, dFeature_Xc, dFeature_Yc);


	//대상 feature의 Angle을 얻어 온다.
	double dAngle = CDrawMakePoint::GetAngle(dXc, dYc,
		dFeature_Xc, dFeature_Yc);

	//대상 Feature의 선 굵기
	double dWidth = pFeatureA->m_pSymbol->m_vcParams[0];

	//
	D2D1_POINT_2F fptCenter = D2D1::Point2F(DoubleToFloat(dXc), DoubleToFloat(dYc));
	D2D1_POINT_2F fptOrgPoint = D2D1::Point2F(DoubleToFloat(fptCenter.x + (dWidth / 2.)), fptCenter.y);

	//Set Rotate
	D2D1_POINT_2F fptStart = CDrawMakePoint::Rotate(fptOrgPoint, fptCenter, -1 * dAngle);

	dAngle += 180.;
	if (dAngle >= 360.) dAngle -= 360.;

	D2D1_POINT_2F fptEnd = CDrawMakePoint::Rotate(fptOrgPoint, fptCenter, -1 * dAngle);

	fXs = fptStart.x;
	fYs = fptStart.y;
	fXe = fptEnd.x;
	fYe = fptEnd.y;

	return RESULT_GOOD;
}


void CMainFrame::SetLayerName()
{

	m_pLayerComboBox->RemoveAllItems();

	UserLayerSet_t* pUserLayerSet = GetUserLayerSet();
	if (pUserLayerSet == nullptr) return;
	SystemSpec_t* pSystemSpec = GetSystemSpec();
	if (pSystemSpec == nullptr) return;
	eMachineType eMcType = pSystemSpec->sysBasic.McType;
	int nCount = static_cast<int>(pUserLayerSet->vcLayerSet.size());
	for (int i = 0; i < nCount; i++)
	{
		//NSIS일 경우 Measure Layer만 편집
		if (eMcType == eMachineType::eNSIS)
		{
			if (pUserLayerSet->vcLayerSet[i]->strDefine == MEASURE_LAYER_TXT || pUserLayerSet->vcLayerSet[i]->strDefine == THICKNESS_LAYER_TXT)
			{
				/*if (pUserLayerSet->vcLayerSet[i]->bCheck == true)*/
				{
					
					m_pLayerComboBox->AddItem(pUserLayerSet->vcLayerSet[i]->strLayer);

					//m_pLayerComboBox->SelectItem(0);
				}
			}

			
		}
		else if (eMcType == eMachineType::eAVI)
		{
			if (pUserLayerSet->vcLayerSet[i]->strDefine == PATTERN_LAYER_TXT)
			{
				if (pUserLayerSet->vcLayerSet[i]->bCheck == true)
				{
					m_pLayerComboBox->AddItem(pUserLayerSet->vcLayerSet[i]->strLayer);
					m_pLayerComboBox->SelectItem(0);
				}
			}
		}
		else
		{
			if (pUserLayerSet->vcLayerSet[i]->bCheck == true)
			{
				m_pLayerComboBox->AddItem(pUserLayerSet->vcLayerSet[i]->strLayer);

				//m_pLayerComboBox->SelectItem(0);
			}
		}
			
	}
	
}

CString CMainFrame::GetLayerName()
{
	int LayerIndex = m_pLayerComboBox->GetCurSel();
	m_strLayer = m_pLayerComboBox->GetItem(LayerIndex);
	return m_strLayer;
}

BOOL CMainFrame::GetCheckSR()
{
	return m_bCheckSR;
}

BOOL CMainFrame::GetCheckSRAuto()
{
	return m_bCheckSRAuto;
}

void CMainFrame::OnUpdateLayerComboBox(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bEditMode);
}

void CMainFrame::OnUpdateEditModeButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);

	UINT32 nEditMode = GetViewerEditMode();

	//BOOL bEditMode = FALSE;
	
	if (nEditMode >= EditMode::enumMode_EditMode /*&& nEditMode <= EditMode::enumMode_EditAdd*/)
	{
		m_bEditMode = TRUE;
	}

	pCmdUI->SetCheck(m_bEditMode);

	if (!m_bEditMode)
	{
		INT32 iCurStepIdx = GetCurStepIndex();
		if (m_stUserSetInfo.iStepIdx != iCurStepIdx)
		{
			m_stUserSetInfo.iStepIdx = iCurStepIdx;
			DrawCamData();
		}
	}

	

}

void CMainFrame::OnEditResizeButton()
{
	if (m_pFeatureEditResize != nullptr)
	{
		UINT32 nEditMode = EditMode::enumMode_EditResize;
		AddApplicationLog(START_EDIT_RESIZE_TXT);
		SetViewerEditMode(nEditMode);

		m_pFeatureEditResize->ShowWindow(SW_SHOW);
	}
}

void CMainFrame::OnUpdateEditResizeButton(CCmdUI* pCmdUI)
{
	UINT32 nEditMode = GetViewerEditMode();

	if (nEditMode == EditMode::enumMode_EditMode)
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(FALSE);
	}
	else if (nEditMode == EditMode::enumMode_EditResize)
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(FALSE);
	}
}

void CMainFrame::OnEditMoveButton()
{
	if (m_pFeatureEditMove != nullptr)
	{
		UINT32 nEditMode = EditMode::enumMode_EditMove;
		AddApplicationLog(START_EDIT_MOVE_TXT);
		SetViewerEditMode(nEditMode);

		m_pFeatureEditMove->ShowWindow(SW_SHOW);
	}
}

void CMainFrame::OnUpdateEditMoveButton(CCmdUI* pCmdUI)
{
	UINT32 nEditMode = GetViewerEditMode();

	if (nEditMode == EditMode::enumMode_EditMode)
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(FALSE);
	}
	else if (nEditMode == EditMode::enumMode_EditMove)
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(FALSE);
	}
}

void CMainFrame::OnEditDeleteButton()
{
	if (m_pFeatureEditDelete != nullptr)
	{
		UINT32 nEditMode = EditMode::enumMode_EditDelete;
		AddApplicationLog(START_EDIT_DELETE_TXT);
		SetViewerEditMode(nEditMode);

		m_pFeatureEditDelete->ShowWindow(SW_SHOW);
	}

}

void CMainFrame::OnUpdateEditDeleteButton(CCmdUI* pCmdUI)
{
	UINT32 nEditMode = GetViewerEditMode();

	if (nEditMode == EditMode::enumMode_EditMode)
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(FALSE);
	}
	else if (nEditMode == EditMode::enumMode_EditDelete)
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(FALSE);
	}
}

void CMainFrame::OnEditRotateButton()
{
	if (m_pFeatureEditRotate != nullptr)
	{

		UINT32 nEditMode = EditMode::enumMode_EditRotate;
		AddApplicationLog(START_EDIT_ROTATE_TXT);
		SetViewerEditMode(nEditMode);

		m_pFeatureEditRotate->ShowWindow(SW_SHOW);
	}
}

void CMainFrame::OnUpdateEditRotateButton(CCmdUI*pCmdUI)
{
	UINT32 nEditMode = GetViewerEditMode();

	if (nEditMode == EditMode::enumMode_EditMode)
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(FALSE);
	}
	else if (nEditMode == EditMode::enumMode_EditRotate)
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(FALSE);
	}
}

void CMainFrame::OnEditAddButton()
{
	if (m_pFeatureEditAdd != nullptr)
	{	
		ResetSelect();

		UINT32 nEditMode = EditMode::enumMode_EditAdd;
		AddApplicationLog(START_EDIT_ADD_TXT);
		SetViewerEditMode(nEditMode);

		m_pFeatureEditAdd->ShowWindow(SW_SHOW);
	}
}

void CMainFrame::OnUpdateEditAddButton(CCmdUI* pCmdUI)
{
	UINT32 nEditMode = GetViewerEditMode();
	
	if (nEditMode == EditMode::enumMode_EditMode)
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(FALSE);
	}
	else if (nEditMode == EditMode::enumMode_EditAdd)
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(FALSE);
	}	
}


void CMainFrame::OnEditCopyButton()
{
	if (m_pFeatureEditCopy != nullptr)
	{
		UINT32 nEditMode = EditMode::enumMode_EditCopy;
		AddApplicationLog(START_EDIT_COPY_TXT);
		SetViewerEditMode(nEditMode);

		m_pFeatureEditCopy->ShowWindow(SW_SHOW);
	}
}





void CMainFrame::OnUpdateEditCopyButton(CCmdUI* pCmdUI)
{
	UINT32 nEditMode = GetViewerEditMode();

	if (nEditMode == EditMode::enumMode_EditMode)
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(FALSE);
	}
	else if (nEditMode == EditMode::enumMode_EditCopy)
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(FALSE);
	}
}

void CMainFrame::OnEditFeatureButton()
{
	BOOL bShowPane = FALSE;

	if (m_docPaneFeatureEdit.IsPaneVisible() == FALSE)
		bShowPane = TRUE;

	m_docPaneFeatureEdit.ShowPane(bShowPane, FALSE, TRUE);
	m_docPaneFeatureEdit.SetAutoHideMode(FALSE, CBRS_ALIGN_RIGHT);
	m_docPaneFeatureEdit.RedrawWindow();
	m_docPaneFeatureEdit.Invalidate();

}

void CMainFrame::OnUpdateEditFeauterButton(CCmdUI* pCmdUI)
{
	UINT32 nEditMode = GetViewerEditMode();

	
	if (nEditMode == EditMode::enumMode_EditMode)
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(FALSE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(FALSE);
	}
}


void CMainFrame::OnEditMaskButton()
{
	if (m_pAlignMaskEditDlg != nullptr)
	{
		ResetSelect();

		UINT32 nEditMode = EditMode::enumMode_EditMask;		
		AddApplicationLog(START_EDIT_MASK_TXT);
		SetViewerEditMode(nEditMode);		

		m_pAlignMaskEditDlg->ShowWindow(SW_SHOW);

		DrawCamData(FALSE, FALSE);

	}
	
}

void CMainFrame::OnUpdateEditMaskButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);

	UINT32 nEditMode = GetViewerEditMode();

	if (nEditMode == EditMode::enumMode_EditMask)
	{
		pCmdUI->SetCheck(TRUE);
	}
	else
	{
		pCmdUI->SetCheck(FALSE);
	}

// 	if (m_stUserLayerSet.nPanelStepIdx == m_stUserSetInfo.iStepIdx)
// 	{
// 		pCmdUI->Enable(TRUE);
// 	}
// 	else
// 	{
// 		pCmdUI->Enable(FALSE);
// 	}
}

void CMainFrame::OnTestSaveButton()
{
// 	static TCHAR BASED_CODE szFilter[] = _T("Image File(*.bmp) |*.bmp||");
// 
// 	CFileDialog dlg(FALSE, _T("*.bmp"), _T(""), OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, szFilter);
// 
// 	if (IDOK == dlg.DoModal())
// 	{
// 		CString strFilePathName = dlg.GetPathName();
// 
// 		strFilePathName = CUtils::ChangeExt(strFilePathName, _T(".bmp"));
// 		
// 		bool bIsLine = true;
// 		SystemSpec_t* pSysSpec = GetSystemSpec();
// 		double dResolution = 0.0025;//2.5um
// 		if (pSysSpec != nullptr)
// 		{
// 			dResolution = pSysSpec->sysCamera[bIsLine].dResolution / 1000.;
// 		}
// 		
// 		SaveLayer(dResolution, strFilePathName);
// 	}

	CFolderPickerDialog Picker(m_strSaveFolder, OFN_FILEMUSTEXIST, NULL, 0);

	if (Picker.DoModal() == IDCANCEL) return;

	//선택된 경로를 얻음
	m_strSaveFolder = Picker.GetPathName() + _T("\\");

	bool bIsLine = true;
	SystemSpec_t* pSysSpec = GetSystemSpec();
	double dResolution = 0.0025;//0.0025;//2.5um
// 	if (pSysSpec != nullptr)
// 	{
// 		dResolution = pSysSpec->sysCamera[bIsLine].dResolution / 1000.;
// 	}

	SaveLayer_Test(dResolution, m_strSaveFolder);

}

void CMainFrame::OnUpdateTestSaveButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);
}

////////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewJobInfo()
{
	ShowHideFormView(ID_JOBINFO_DOCPANE);
}

void CMainFrame::OnViewCamInfo()
{
	ShowHideFormView(ID_CAMINFO_DOCPANE);
}

void CMainFrame::OnViewLayerInfo()
{
	BOOL bShowPane = FALSE;

	if (m_docPaneLayerInfo.IsPaneVisible() == FALSE)
		bShowPane = TRUE;

	m_docPaneLayerInfo.ShowPane(bShowPane, FALSE, TRUE);
	//m_docPaneLayerInfo.SetAutoHideMode(FALSE, CBRS_ALIGN_RIGHT);
	m_docPaneLayerInfo.RedrawWindow();
	m_docPaneLayerInfo.Invalidate();
}

void CMainFrame::OnViewLogInfo()
{
	BOOL bShowPane = FALSE;

	if (m_docPaneLogView.IsPaneVisible() == FALSE)
		bShowPane = TRUE;

	m_docPaneLogView.ShowPane(bShowPane, FALSE, TRUE);
	//m_docPaneLogView.SetAutoHideMode(FALSE, CBRS_ALIGN_RIGHT);
	m_docPaneLogView.RedrawWindow();
	m_docPaneLogView.Invalidate();
}

void CMainFrame::ShowHideFormView(UINT id)
{
	BOOL bShowPane = FALSE;
	CFormViewDocPane* pViewPane = NULL;

	switch (id)
	{
	case ID_JOBINFO_DOCPANE:
		pViewPane = &m_docPaneJobInfo;
		break;
	case ID_CAMINFO_DOCPANE:
		pViewPane = &m_docPaneCamInfo;
		break;
	case ID_SETTINGS_DOCPANE:
		pViewPane = &m_docPaneSettings;
		break;
	case ID_FEATURE_EDIT_DOCPANE:
		pViewPane = &m_docPaneFeatureEdit;
		break;
	}

	if (pViewPane == NULL) return;
	if (pViewPane->IsPaneVisible() == FALSE)
		bShowPane = TRUE;

	pViewPane->ShowPane(bShowPane, FALSE, TRUE);
	//pViewPane->SetAutoHideMode(FALSE, CBRS_ALIGN_RIGHT);
	pViewPane->RedrawWindow();
	pViewPane->Invalidate();
}

void CMainFrame::_CreateFeatureEditDlg()
{
	if (m_pFeatureEditResize == nullptr)
	{
		m_pFeatureEditResize = new CFeatureEdit_ResizeDlg();
		m_pFeatureEditResize->Create(IDD_EDIT_RESIZE, this);
	}

	if (m_pFeatureEditMove == nullptr)
	{
		m_pFeatureEditMove = new CFeatureEdit_MoveDlg();
		m_pFeatureEditMove->Create(IDD_EDIT_MOVE, this);
	}

	if (m_pFeatureEditDelete == nullptr)
	{
		m_pFeatureEditDelete = new CFeatureEdit_DeleteDlg();
		m_pFeatureEditDelete->Create(IDD_EDIT_DELETE, this);
	}

	if (m_pFeatureEditAdd == nullptr)
	{
		m_pFeatureEditAdd = new CFeatureEdit_AddDlg();
		m_pFeatureEditAdd->Create(IDD_EDIT_ADD, this);
	}

	if (m_pFeatureEditCopy == nullptr)
	{
		m_pFeatureEditCopy = new CFeatureEdit_CopyDlg();
		m_pFeatureEditCopy->Create(IDD_EDIT_COPY, this);
	}

	if (m_pAlignMaskEditDlg == nullptr)
	{
		m_pAlignMaskEditDlg = new CAlignMaskEditDlg();
		m_pAlignMaskEditDlg->Create(IDD_DIALOG_ALIGN_MASK3, this);
	}
	if (m_pFeatureEditRotate == nullptr)
	{
		m_pFeatureEditRotate = new CFeatureEdit_RotateDlg();
		m_pFeatureEditRotate->Create(IDD_EDIT_ROTATE, this);
	}

	if (m_pFeatureEditFeature == nullptr)
	{
		m_pFeatureEditFeature = new CFeatureEdit_FeatureDlg();
		m_pFeatureEditFeature->Create(IDD_EDIT_FEATURE, this);
	}

}

void CMainFrame::_DeleteFeatureEditDlg()
{
	if (m_pFeatureEditResize != nullptr)
	{
		delete m_pFeatureEditResize;
		m_pFeatureEditResize = nullptr;
	}

	if (m_pFeatureEditMove != nullptr)
	{
		delete m_pFeatureEditMove;
		m_pFeatureEditMove = nullptr;
	}

	if (m_pFeatureEditDelete != nullptr)
	{
		delete m_pFeatureEditDelete;
		m_pFeatureEditDelete = nullptr;
	}

	if (m_pFeatureEditAdd != nullptr)
	{
		delete m_pFeatureEditAdd;
		m_pFeatureEditAdd = nullptr;
	}


	if (m_pFeatureEditCopy != nullptr)
	{
		delete m_pFeatureEditCopy;
		m_pFeatureEditCopy = nullptr;
	}

	if (m_pFeatureEditRotate != nullptr)
	{
		delete m_pFeatureEditRotate;
		m_pFeatureEditRotate = nullptr;
	}

	if (m_pAlignMaskEditDlg != nullptr)
	{
		delete m_pAlignMaskEditDlg;
		m_pAlignMaskEditDlg = nullptr;
	}

	if (m_pFeatureEditFeature != nullptr)
	{
		delete m_pFeatureEditFeature;
		m_pFeatureEditFeature = nullptr;
	}
}


INT32 CMainFrame::GetCurStepIndex()
{
	return m_docPaneCamInfo.GetCurStepIndex();
}

UINT32 CMainFrame::Refresh_JobInfoList()
{
	return m_docPaneJobInfo.Refresh_JobList();
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CFrameWndEx::PreTranslateMessage(pMsg);
}

void CMainFrame::OnViewFrontButton()
{
	
	if (m_stUserSetInfo.prodSide != eProdSide::eTop)	
		m_stUserSetInfo.bSideChanged = true;
	else
		m_stUserSetInfo.bSideChanged = false;

	m_bMirrorH = m_bMirrorV = false;
	m_stUserSetInfo.prodSide = eProdSide::eTop;
	m_stUserSetInfo.bMirror = false;
	m_stUserSetInfo.mirrorDir = eDirectionType::DirectionTypeNone;
	
	/*CString strTemp = GetWorkLayer();
	if (strTemp.CompareNoCase(m_strStartLayer) == 0)
	{
		m_stUserSetInfo.strWorkLayer = m_strEndLayer;
		m_stUserSetInfo.strPairLayer = m_strStartLayer;
	}
	else
	{
		m_stUserSetInfo.strWorkLayer = m_strStartLayer;
		m_stUserSetInfo.strPairLayer = m_strEndLayer;
	}*/

	/*if (m_stUserSetInfo.strWorkLayer.CompareNoCase(m_strStartLayer) == 0)
		m_stUserSetInfo.prodSide = eProdSide::eTop;
	else
		m_stUserSetInfo.prodSide = eProdSide::eBot;*/


	if (m_bIsExtractCam)
		SetLayerInfo();

	m_stMeasureData.Clear();
	SystemSpec_t* pSysSpec = GetSystemSpec();
	if (pSysSpec->sysBasic.UserSite == eUserSite::eDAEDUCK)
	{
		m_docPaneMeasureInfo.ClearMeasureList();
	}
	else if (pSysSpec->sysBasic.UserSite == eUserSite::eSEM)
	{
		m_docPaneMeasureInfo_V2.ClearMeasureList();
	}
}

void CMainFrame::OnUpdateViewFrontButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsExtractCam);

	if (m_stUserSetInfo.prodSide == eProdSide::eTop)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnViewBackButton()
{
	if (m_stUserSetInfo.prodSide != eProdSide::eBot)	
		m_stUserSetInfo.bSideChanged = true;
	else
		m_stUserSetInfo.bSideChanged = false;

	m_bMirrorH = m_bMirrorV = false;
	m_stUserSetInfo.prodSide = eProdSide::eBot;	
	m_stUserSetInfo.bMirror = true;
	if (GetMachineType() == eMachineType::eNSIS)
	{
		m_stUserSetInfo.mirrorDir = eDirectionType::eHorizontal;
		m_bMirrorH = true;
	}
	else
	{
		m_stUserSetInfo.mirrorDir = eDirectionType::eVertical;
		m_bMirrorV = true;
	}	

	/*CString strTemp = GetWorkLayer();
	if (strTemp.CompareNoCase(m_strStartLayer) == 0)
	{
		m_stUserSetInfo.strWorkLayer = m_strEndLayer;
		m_stUserSetInfo.strPairLayer = m_strStartLayer;
	}
	else
	{
		m_stUserSetInfo.strWorkLayer = m_strStartLayer;
		m_stUserSetInfo.strPairLayer = m_strEndLayer;
	}*/

	/*if (m_stUserSetInfo.strWorkLayer.CompareNoCase(m_strStartLayer) == 0)
		m_stUserSetInfo.prodSide = eProdSide::eBot;
	else
		m_stUserSetInfo.prodSide = eProdSide::eTop;*/

	if (m_bIsExtractCam)
		SetLayerInfo();

	m_stMeasureData.Clear();
	m_docPaneMeasureInfo_V2.ClearMeasureList();	
	
}

void CMainFrame::OnUpdateViewBackButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsExtractCam);

	if (m_stUserSetInfo.prodSide == eProdSide::eBot)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);

}

void CMainFrame::OnViewMeasureInfo()
{
	BOOL bShowPane = FALSE;
	

	SystemSpec_t* pSysSpec = GetSystemSpec();
	if (pSysSpec == nullptr) return ;
	if (pSysSpec->sysBasic.UserSite == eUserSite::eSEM)
	{
		if (m_docPaneMeasureInfo_V2.IsPaneVisible() == FALSE)
			bShowPane = TRUE;
	}
	else
	{
		if (m_docPaneMeasureInfo.IsPaneVisible() == FALSE)
			bShowPane = TRUE;
		
	}

	ShowHideMeasureInfo(bShowPane);
}

UINT32 CMainFrame::UpdataMeasureInfoView()
{	
	SystemSpec_t* pSysSpec = GetSystemSpec();
	if (pSysSpec == nullptr) return RESULT_BAD;
	if (pSysSpec->sysBasic.UserSite == eUserSite::eDAEDUCK)
	{
		if (RESULT_GOOD == m_docPaneMeasureInfo.SetMeasureData())
			return m_docPaneMeasureInfo.UpdateFeatureSelect_UI();
	}
	else if (pSysSpec->sysBasic.UserSite == eUserSite::eSEM)
	{
		if (RESULT_GOOD == m_docPaneMeasureInfo_V2.SetMeasureData())
			return m_docPaneMeasureInfo_V2.UpdateFeatureSelect_UI();
	}

	return RESULT_BAD;	
}

UINT32 CMainFrame::SetMeasure_ResetSelect()
{
	SystemSpec_t* pSysSpec = GetSystemSpec();
	if (pSysSpec == nullptr) return RESULT_BAD;
	if (pSysSpec->sysBasic.UserSite == eUserSite::eDAEDUCK)
	{
		return m_docPaneMeasureInfo.ResetFeatureSelect_UI();
	}
	else if (pSysSpec->sysBasic.UserSite == eUserSite::eSEM)
	{
		return m_docPaneMeasureInfo_V2.ResetFeatureSelect_UI();
	}

	return RESULT_BAD;
}

UINT32 CMainFrame::SetMeasure_Select_UI()
{
	SystemSpec_t* pSysSpec = GetSystemSpec();
	if (pSysSpec == nullptr) return RESULT_BAD;
	if (pSysSpec->sysBasic.UserSite == eUserSite::eDAEDUCK)
	{
		return m_docPaneMeasureInfo.UpdateFeatureSelect_UI();
	}
	else if (pSysSpec->sysBasic.UserSite == eUserSite::eSEM)
	{
		return m_docPaneMeasureInfo_V2.UpdateFeatureSelect_UI();
	}
	return RESULT_BAD;
}


void CMainFrame::ClearMeasureData()
{
	SystemSpec_t* pSysSpec = GetSystemSpec();
	if (pSysSpec == nullptr) return ;
	if (pSysSpec->sysBasic.UserSite == eUserSite::eDAEDUCK)
	{
		m_docPaneMeasureInfo.ClearMeasureList();
	}
	else if (pSysSpec->sysBasic.UserSite == eUserSite::eSEM)
	{
		m_docPaneMeasureInfo_V2.ClearMeasureList();
	}
}

int CMainFrame::GetCode_MakeMaster()
{
	if (m_pMakeMasterResult.GetSize() > 0)
	{
		return ((CErrorInfo *)m_pMakeMasterResult.GetAt(0))->m_nErrorCode;
	}

	return (int)ODB_ERROR_CODE::NONE;
}

void CMainFrame::InitFlag_MakeMasterResult()
{
	for (int i = 0; i < m_pMakeMasterResult.GetSize(); i++)
	{
		CErrorInfo *item = (CErrorInfo *)m_pMakeMasterResult.GetAt(i);
		delete item;
	}
	m_pMakeMasterResult.RemoveAll();
}

void CMainFrame::SetFlag_MakeMasterResult(UINT32 result, int errorcode)
{
	if (m_bAutoRun == false)
	{
		return;
	}

	for (int i = 0; i < m_pMakeMasterResult.GetSize(); i++)
	{
		CErrorInfo *item = (CErrorInfo *)m_pMakeMasterResult.GetAt(i);

		if (item->m_nErrorCode == errorcode)
		{
			return;
		}
	}

	CErrorInfo *item = new CErrorInfo();
	item->m_nResult = result;
	item->m_nErrorCode = errorcode;

	m_pMakeMasterResult.Add(item);
}

BOOL CMainFrame::GetFlag_MakeMasterResult()
{
	if (m_pMakeMasterResult.GetCount() > 0)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void CMainFrame::SetFlag_AutoRun(BOOL autorun)
{
	m_bAutoRun = autorun;
}

BOOL CMainFrame::GetFlag_AutoRun()
{
	return m_bAutoRun;
}

afx_msg LRESULT CMainFrame::OnNsisPipe(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam))	// HIWORD(lParam)
	{
		return 0;
	}
	else
	{
		char string[250] = "";
		int iAddrSize = 0;
		CString csUdpLog = L"";

		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_READ:
			char szBuf[BUFSIZE];
			memset(szBuf, 0, BUFSIZE);
			m_UDPforNSIS.OnRead(szBuf);

			int mUdpCod = UDP_LIVE + 1;

			csUdpLog = (LPSTR)szBuf;
			if (csUdpLog.GetLength() != 0)
				AutoRunODBParsing(csUdpLog);
			break;
		}
	}
	return 0;
}

BOOL CMainFrame::SendPipeNSIS(CString lpszFormat)
{
	// FORMAT AND WRITE THE DATA WE WERE GIVEN..
	int Lenght = WideCharToMultiByte(CP_ACP, 0, lpszFormat.GetBuffer(0), -1, NULL, 0, NULL, NULL);

	char* cCommand = new char[Lenght];
	if (cCommand == nullptr)
		return FALSE;

	WideCharToMultiByte(CP_ACP, 0, lpszFormat.GetBuffer(0), -1, cCommand, Lenght, NULL, NULL);
	Lenght = WideCharToMultiByte(CP_ACP, 0, m_csIP.GetBuffer(0), -1, NULL, 0, NULL, NULL);

	char* cNSISIP = new char[Lenght];
	if (cNSISIP == nullptr)
	{
		delete[]cCommand;
		return FALSE;
	}

	WideCharToMultiByte(CP_ACP, 0, m_csIP.GetBuffer(0), -1, cNSISIP, Lenght, NULL, NULL);
	m_UDPforNSIS.SendToWithPort(cCommand, lpszFormat.GetLength(), cNSISIP, 0, m_iPort);

	delete[]cCommand;
	delete[]cNSISIP;

	return TRUE;
}


BOOL CMainFrame::AutoRunODBParsing(CString csODBFileName)
{
	InitFlag_MakeMasterResult();
	SetFlag_AutoRun(TRUE);

	CString FileName = L"";
	CString csLog = L"";

	UINT32 bResult = m_docPaneJobInfo.SetJobPath(GetCamPath());
	if (bResult != RESULT_GOOD)
	{
		SetFlag_MakeMasterResult(bResult, (int)ODB_ERROR_CODE::EXIST_ODB_FAIL);
		csLog.Format(_T("ODB_READ_FAIL:%d"), GetCode_MakeMaster());
		SendPipeNSIS(csLog);

		m_docPaneJobInfo.LockJobTree(TRUE);
		SetFlag_AutoRun(FALSE);
		return FALSE;
	}
	
	Sleep(1000);

	csLog.Format(_T("Auto ODB Parsing Start!! (ODB Name : %s )"), csODBFileName.GetBuffer(0));
	AddApplicationLog(csLog);
	if (!m_docPaneJobInfo.SetODBFile(csODBFileName))
	{
		SetFlag_MakeMasterResult(bResult, (int)ODB_ERROR_CODE::EXIST_ODB_FAIL);
		csLog.Format(_T("ODB_READ_FAIL:%d"), GetCode_MakeMaster());
		SendPipeNSIS(csLog);

		m_docPaneJobInfo.LockJobTree(TRUE);
		SetFlag_AutoRun(FALSE);
		return FALSE;
	}

	csLog.Format(_T("ODB Read Start"));
	AddApplicationLog(csLog);

	FileName.Format(_T("%s.tgz"), csODBFileName.GetBuffer(0));

	csLog.Format(_T("ODB Front Parsing Start"));
	AddApplicationLog(csLog);

	for (auto Layer : m_vcLayerList)
	{
		m_strStartLayer = Layer->strTop;
		m_strEndLayer = Layer->strBot;

		bResult = SetDrawLayer(eProdSide::eTop);
		if (bResult != RESULT_GOOD)
		{
			SetFlag_MakeMasterResult(bResult, (int)ODB_ERROR_CODE::CHANGE_MODE_FAIL);
			csLog.Format(_T("ODB_READ_FAIL:%d"), GetCode_MakeMaster());
			SendPipeNSIS(csLog);

			m_docPaneJobInfo.LockJobTree(TRUE);
			SetFlag_AutoRun(FALSE);
			return FALSE;
		}

		OnLoadLayer();
		csLog.Format(_T("ODB Front Parsing Complete"));
		AddApplicationLog(csLog);

		csLog.Format(_T("ODB Front Data Make Start"));
		AddApplicationLog(csLog);
		OnMakeMaster();
		csLog.Format(_T("ODB Front Data Make Complete"));
		AddApplicationLog(csLog);

		csLog.Format(_T("ODB Bottom Parsing Start"));
		AddApplicationLog(csLog);

		bResult = SetDrawLayer(eProdSide::eBot);
		if (bResult != RESULT_GOOD)
		{
			SetFlag_MakeMasterResult(bResult, (int)ODB_ERROR_CODE::CHANGE_MODE_FAIL);
			csLog.Format(_T("ODB_READ_FAIL:%d"), GetCode_MakeMaster());
			SendPipeNSIS(csLog);

			m_docPaneJobInfo.LockJobTree(TRUE);
			SetFlag_AutoRun(FALSE);
			return FALSE;		
		}

		OnLoadLayer();
		csLog.Format(_T("ODB Bottom Parsing Complete"));
		AddApplicationLog(csLog);

		csLog.Format(_T("ODB Bottom Data Make Start"));
		AddApplicationLog(csLog);
		OnMakeMaster();
		csLog.Format(_T("ODB Bottom Data Make Complete"));
		AddApplicationLog(csLog);
	}

	if (GetFlag_MakeMasterResult())
		SendPipeNSIS(_T("ODB_READ_OK"));
	else
	{
		csLog.Format(_T("ODB_READ_FAIL:%d"), GetCode_MakeMaster());
		SendPipeNSIS(csLog);
	}

	m_docPaneJobInfo.LockJobTree(TRUE);
	SetFlag_AutoRun(FALSE);
	return TRUE;
}

BOOL CMainFrame::CheckLayerInfo()
{
	ClearLayerList();

	std::vector<stLayerInfo*>* pLayerInfo = nullptr;	
	if (m_docPaneCamInfo.GetLayerInfo(&pLayerInfo) != RESULT_GOOD)
	{
		AddApplicationLog(_T("FAIL GET LAYER INFO"), LogMsgType::EN_ERROR);
		return RESULT_BAD;
	}

	if (GetMachineType() == eMachineType::eNSIS)
	{

		m_pPatternLayerInfo.clear();
		m_pSRLayerInfo.clear();
		m_pMPLayerInfo.clear();
		m_pTPLayerInfo.clear();		

		if (m_docPaneCamInfo.CheckLayerInfo(&pLayerInfo, m_pPatternLayerInfo, m_pSRLayerInfo, m_pMPLayerInfo, m_pTPLayerInfo) != RESULT_GOOD)
		{
			AddApplicationLog(_T("FAIL GET LAYER INFO"), LogMsgType::EN_ERROR);
			return RESULT_BAD;
		}
	}

	//CString strLayer = L"";
	//for (auto LayerInfo : *pLayerInfo)
	//{
	//	if (LayerInfo->strName.Find(_T("MP")) >= 0 || LayerInfo->strName.Find(_T("mp")) >= 0)
	//		continue;
	//			
	//	stLayerPair* pLayerPair = new stLayerPair;
	//	if (LayerInfo->bIsTop)
	//	{
	//		pLayerPair->strTop = LayerInfo->strName;
	//		pLayerPair->strBot = LayerInfo->strPairLayerName;
	//	}
	//	else
	//	{
	//		pLayerPair->strBot = LayerInfo->strName;
	//		pLayerPair->strTop = LayerInfo->strPairLayerName;
	//	}

	//	//m_vcLayerList.emplace_back(pLayerPair);
	//}

	SystemSpec_t* pSysSpec = GetSystemSpec();

	/*if (pSysSpec->sysBasic.McType == eMachineType::eNSIS && pSysSpec->sysBasic.UserSite == eUserSite::eSEM)
	{
		int nLayerCount = static_cast<int>(m_pSRLayerInfo.size());
		bool bCheck = false;
		for (int i = 0; i < nLayerCount; i++)
		{
			int nLayerCount2 = static_cast<int>(m_pSRLayerInfo[i].size());
			for (int j = 0; j < nLayerCount2; j++)
			{
				stLayerInfo* tempLayerInfo = m_pSRLayerInfo[i][j];
				if (tempLayerInfo->bIsTop == true && tempLayerInfo->strPairLayerName != _T(""))
				{
					m_strStartLayer = tempLayerInfo->strName;
					if (pSysSpec->sysBasic.UserSite == eUserSite::eSEM)
						m_strEndLayer = _T("");
					else
						m_strEndLayer = tempLayerInfo->strPairLayerName;
					bCheck = true;
					break;
				}
			}

			if (bCheck == true)
				break;
		}
	}*/
	//else if(pSysSpec->sysBasic.McType == eMachineType::eNSIS && pSysSpec->sysBasic.UserSite == eUserSite::eDAEDUCK)
	if(pSysSpec->sysBasic.McType == eMachineType::eNSIS)
	{
		int nLayerCount = static_cast<int>(m_pPatternLayerInfo.size());
		bool bCheck = false;
		for (int i = 0; i < nLayerCount; i++)
		{
			int nLayerCount2 = static_cast<int>(m_pPatternLayerInfo[i].size());
			for (int j = 0; j < nLayerCount2; j++)
			{
				stLayerInfo* tempLayerInfo = m_pPatternLayerInfo[i][j];
				if (tempLayerInfo->bIsTop == true && tempLayerInfo->strPairLayerName != _T(""))
				{
					m_strStartLayer = tempLayerInfo->strName;
					if (pSysSpec->sysBasic.UserSite == eUserSite::eSEM)
						m_strEndLayer = _T("");
					else
						m_strEndLayer = tempLayerInfo->strPairLayerName;
					bCheck = true;
					break;
				}
			}

			if (bCheck == true)
				break;
		}
	}

	


	/*CString strLayer = L"";

	for (auto LayerInfo : *pLayerInfo)
	{
		if (LayerInfo->strName.Find(_T("MP")) >= 0 || LayerInfo->strName.Find(_T("mp")) >= 0)
			continue;

		BOOL bFind = FALSE;
		for (INT32 i = 0; i < 2; i++)
		{
			CStringArray *pList = m_docPaneCamInfo.GetLayerList(i);
			if (pList == nullptr)
				continue;

			for (INT32 j = 0; j < pList->GetSize(); j++)
			{
				strLayer = pList->GetAt(j);

				if (strLayer.CompareNoCase(LayerInfo->strName) == 0)
				{
					bFind = TRUE;
					break;
				}
			}
		}

		if (!bFind)
			continue;

		BOOL bAlready = FALSE;
		for (auto Layer : m_vcLayerList)
		{
			if (LayerInfo->bIsTop)
			{
				if (Layer->strTop.CompareNoCase(LayerInfo->strName) == 0)
				{
					bAlready = TRUE;
					break;
				}
				else if (Layer->strBot.CompareNoCase(LayerInfo->strPairLayerName) == 0)
				{
					bAlready = TRUE;
					break;
				}
			}
			else
			{
				if (Layer->strTop.CompareNoCase(LayerInfo->strPairLayerName) == 0)
				{
					bAlready = TRUE;
					break;
				}

				else if (Layer->strBot.CompareNoCase(LayerInfo->strName) == 0)
				{
					bAlready = TRUE;
					break;
				}
			}
		}

		if (!bAlready)
		{
			stLayerPair* pLayerPair = new stLayerPair;
			if (LayerInfo->bIsTop)
			{
				pLayerPair->strTop = LayerInfo->strName;
				pLayerPair->strBot = LayerInfo->strPairLayerName;
			}
			else
			{
				pLayerPair->strBot = LayerInfo->strName;
				pLayerPair->strTop = LayerInfo->strPairLayerName;
			}

			m_vcLayerList.emplace_back(pLayerPair);
		}
	}

	if (m_vcLayerList.size() > 0)
	{
		m_strStartLayer = m_vcLayerList[0]->strTop;
		m_strEndLayer = m_vcLayerList[0]->strBot;
	}
	else
	{
		m_strStartLayer.Empty();
		m_strEndLayer.Empty();
	}*/

	return RESULT_GOOD;
}

void CMainFrame::ClearLayerList()
{
	for (auto Layer : m_vcLayerList)
	{
		delete Layer;
		Layer = nullptr;
	}

	m_vcLayerList.clear();
}

UINT32 CMainFrame::SetDrawLayer(eProdSide side)
{
	ChangeSide(side);

	return m_docPaneLayerInfo.SetUserLayerInfo();
}

UINT32 CMainFrame::ChangeLayerInfo(CString strLayer)
{
	if (m_stUserSetInfo.prodSide == eProdSide::eTop)
	{
		if (m_strStartLayer.CompareNoCase(strLayer) == 0)
			return RESULT_GOOD;
	}
	else
	{
		if (m_strEndLayer.CompareNoCase(strLayer) == 0)
			return RESULT_GOOD;
	}

	int nPatternSize = static_cast<int>(m_pPatternLayerInfo.size());
	for (int i = 0; i < nPatternSize; i++)
	{		
		int nPatternSize2 = static_cast<int>(m_pPatternLayerInfo[i].size());
		for (int j = 0; j < nPatternSize2; j++)
		{
			stLayerInfo* tempLayer = m_pPatternLayerInfo[i][j];

			if (tempLayer->strName.CompareNoCase(strLayer))
			{
				m_strStartLayer = tempLayer->strName;
				m_strEndLayer = tempLayer->strPairLayerName;

				ChangeSide(eProdSide::eTop);
			}

			else if (tempLayer->strPairLayerName.CompareNoCase(strLayer) == 0)
			{
				m_strStartLayer = tempLayer->strPairLayerName;
				m_strEndLayer = tempLayer->strName;

				ChangeSide(eProdSide::eBot);
			}
		}
	}

	/*for (auto Layer : m_vcLayerList)
	{
		if (Layer->strTop.CompareNoCase(strLayer) == 0)
		{
			m_strStartLayer = Layer->strTop;
			m_strEndLayer = Layer->strBot;

			ChangeSide(eProdSide::eTop);
		}
		else if (Layer->strBot.CompareNoCase(strLayer) == 0)
		{
			m_strStartLayer = Layer->strTop;
			m_strEndLayer = Layer->strBot;

			ChangeSide(eProdSide::eBot);
		}
	}*/

	return RESULT_GOOD;
}

void CMainFrame::ChangeSide(eProdSide side)
{
	if (side == eProdSide::eTop)
		OnViewFrontButton();
	else
		OnViewBackButton();
}

//Viewer에서 Point Select 후 OnLbuttonUp에서 호출 되는 함수
UINT32 CMainFrame::SetSetlectPoint(IN const int &nIndex, IN const float &fX, IN const float &fY)
{
	UINT32 nRet = RESULT_BAD;

	if (m_pFeatureEditAdd == nullptr) return RESULT_BAD;

	if (m_pFeatureEditAdd->IsWindowVisible() == TRUE)
	{
		nRet = m_pFeatureEditAdd->SetSetlectPoint(nIndex, fX, fY);
	}

	if (m_pAlignMaskEditDlg->IsWindowVisible() == TRUE)
	{
		nRet = m_pAlignMaskEditDlg->SetSetlectPoint(nIndex, fX, fY);
	}

	return nRet;
}

UINT32 CMainFrame::SetSetlectPoint_Center(IN const int &nIndex, IN const float &fX, IN const float &fY)
{
	UINT32 nRet = RESULT_BAD;

	if (m_pFeatureEditAdd == nullptr) return RESULT_BAD;

	if (m_pFeatureEditAdd->IsWindowVisible() == TRUE)
	{
		nRet = m_pFeatureEditAdd->SetSetlectPoint_Center(nIndex, fX, fY);
	}

	return nRet;
}

UINT32 CMainFrame::SetSetlectFeatureInfo(IN SELECT_INFO *pSelectInfo)
{
	UINT32 nRet = RESULT_GOOD;

	if (m_pAlignMaskEditDlg == nullptr) return RESULT_BAD;

	//if (m_pAlignMaskEditDlg->IsWindowVisible() == TRUE)
	{
		nRet = m_pAlignMaskEditDlg->SetSetlectFeatureInfo(pSelectInfo);
	}

	return nRet;
}


void CMainFrame::OnPreviewButton()
{
	m_stUserLayerSet.bIsPreview = !m_stUserLayerSet.bIsPreview;

	if (m_stUserLayerSet.bIsPreview)
	{
		if (RESULT_GOOD == SetLayerInfo())
			OnLoadLayer();

		IsAllLayerLoaded(TRUE);
	}
}

void CMainFrame::OnUpdatePreviewButton(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	if (m_stUserLayerSet.bIsPreview) pCmdUI->SetCheck(TRUE);
	else pCmdUI->SetCheck(FALSE);
}

void CMainFrame::LoadLayer()
{
	OnLoadLayer();
}

void CMainFrame::ShowHideMeasureInfo(BOOL bShow)
{

	SystemSpec_t* pSysSpec = GetSystemSpec();
	if (pSysSpec == nullptr) return ;
	if (pSysSpec->sysBasic.UserSite == eUserSite::eDAEDUCK)
	{
		m_docPaneMeasureInfo.ShowPane(bShow, FALSE, TRUE);
		//m_docPaneMeasureInfo.SetAutoHideMode(FALSE, CBRS_ALIGN_RIGHT);
		m_docPaneMeasureInfo.RedrawWindow();
		m_docPaneMeasureInfo.Invalidate();
	}
	else if (pSysSpec->sysBasic.UserSite == eUserSite::eSEM)
	{
		m_docPaneMeasureInfo_V2.ShowPane(bShow, FALSE, TRUE);
		//m_docPaneMeasureInfo_V2.SetAutoHideMode(FALSE, CBRS_ALIGN_RIGHT);
		//m_docPaneMeasureInfo_V2.RedrawWindow();
		m_docPaneMeasureInfo_V2.Invalidate();
	}
}

void CMainFrame::OnMakeAlign()
{
	if (!m_bIsExtractCam)
		return;

	SystemSpec_t* pSysSpec = GetSystemSpec();
	if (pSysSpec)
	{
		pSysSpec->sysBasic.bExceptImageData = true;
	}

	m_stUserSetInfo.strWorkLayer = L"";
	CString strWorkLayer = m_docPaneLayerInfo.GetPatternLayer();

	CString strResolution = _T("2.5㎛");
	if (GetMachineType() == eMachineType::eNSIS)
	{
		if (m_stMeasureData.vcAlign.size() <= 0)
		{
			AfxMessageBox(_T("판넬 얼라인 포인트가 없습니다."), MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		if (m_stMeasureData.vcUnits.size() <= 0)
		{
			AfxMessageBox(_T("유닛 얼라인 및 측정 포인트가 없습니다."), MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		strWorkLayer.Trim();
		m_stUserSetInfo.strWorkLayer = strWorkLayer;

		if (m_stUserSetInfo.prodSide == eProdSide::eTop)
			strResolution = _T("Front");
		else
			strResolution = _T("Back");
	}
	else
	{
		if (GetUserSite() == eUserSite::eLGIT)
		{
			int nTmp = strWorkLayer.Find(_T("L"));
			if (nTmp >= 0)
			{
				strWorkLayer.Delete(nTmp + 2, strWorkLayer.GetLength() - 1);
				strWorkLayer.Trim();
				m_stUserSetInfo.strWorkLayer = strWorkLayer;
			}
		}
		else
			m_stUserSetInfo.strWorkLayer = strWorkLayer;

		SystemSpec_t* pSysSpec = GetSystemSpec();
		if (pSysSpec)
		{
			bool bIsLine = true;
			strResolution.Format(_T("%.1f㎛"), pSysSpec->sysCamera[bIsLine].dResolution);
		}
	}

	MakeMasterData();

	Sleep(1000);

	m_dlgMakeMaster.SetData(GetModelName(), m_docPaneCamInfo.GetWorkStepName(),
		strWorkLayer, strResolution);

	m_dlgMakeMaster.DoModal();
}

void CMainFrame::OnUpdateMakeAlign(CCmdUI* pCmdUI)
{
	if (m_stUserLayerSet.nPanelStepIdx == GetCurStepIndex())
	{
		pCmdUI->Enable(m_bIsLoadLayers);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnViewMinimap()
{
	BOOL bShowPane = FALSE;
// 	if (m_docPaneMinimapView.IsPaneVisible() == FALSE)
// 		bShowPane = TRUE;
	if (m_docPaneMinimapView_V2.IsPaneVisible() == FALSE)
		bShowPane = TRUE;
	
	ShowHideMinimapView(bShowPane);
}

void CMainFrame::ShowHideMinimapView(BOOL bShow)
{
	SystemSpec_t* pSysSpec = GetSystemSpec();
	if (pSysSpec == nullptr) return ;
	if (pSysSpec->sysBasic.UserSite == eUserSite::eSEM)
	{
		m_docPaneMinimapView_V2.ShowPane(bShow, FALSE, TRUE);
		//m_docPaneMinimapView_V2.SetAutoHideMode(FALSE, CBRS_ALIGN_LEFT);
		m_docPaneMinimapView_V2.RedrawWindow();
		m_docPaneMinimapView_V2.Invalidate();
	}
}

void CMainFrame::ShowHideFeatureEdit()
{
	m_docPaneFeatureEdit.ShowPane(TRUE, FALSE, TRUE);
	SetViewerEditMode(EditMode::enumMode_EditMode);
}

void CMainFrame::ShowHideLayerInfo()
{
	m_docPaneLayerInfo.ShowPane(TRUE, FALSE, TRUE);
}

CString CMainFrame::GetWorkLayer()
{
	CString strWorkLayer = m_docPaneLayerInfo.GetPatternLayer();
	

	strWorkLayer.Trim();

	return strWorkLayer;
}

UINT32 CMainFrame::LoadSpecData()
{
	return m_docPaneSettings.LoadSpecData();
}

UINT32 CMainFrame::LoadFeatureData()
{
	return m_docPaneFeatureEdit.LoadFeatureData();
}

UINT32 CMainFrame::LoadMoveData()
{
	return m_docPaneFeatureEdit.LoadMoveData();
}

UINT32 CMainFrame::LoadMove_KeyData()
{
	return m_docPaneFeatureEdit.LoadMove_KeyData();
}


// UINT32 CMainFrame::LoadMoveData()
// {
// 	return m_docPaneFeatureEdit.LoadFeatureData();
// }

UINT32 CMainFrame::GetHisto_PatternWidth(IN const int &eLineOption, std::map<INT32, std::vector<CString>> &vcHisto_Pos, std::map<INT32, std::vector<CString>> &vcHisto_Neg)
{
	return m_docPaneLayerInfo.GetHisto_PatternWidth(eLineOption, vcHisto_Pos, vcHisto_Neg);
}

UINT32 CMainFrame::ChangeStep(CString strStep)
{
	SystemSpec_t* pSysSpec = GetSystemSpec();
	if (pSysSpec == nullptr || pSysSpec->sysBasic.McType != eMachineType::eNSIS)
		return RESULT_BAD;

	CJobFile* pJobFile = GetJobPtr();
	if (pJobFile == nullptr)
		return RESULT_BAD;

	if (m_stUserSetInfo.rcpMode == eRecipeMode::RecipeModeNone)
		return RESULT_BAD;

	if (strStep.GetLength() > 0)
	{
		if (strStep.CompareNoCase(PANEL_STRING) == 0)
			m_docPaneCamInfo.ChangeStep(m_stUserLayerSet.nPanelStepIdx);
		else if (strStep.CompareNoCase(STRIP_STRING) == 0)
		{
			for (auto it : m_stUserLayerSet.vcStripStepIdx)
			{
				m_docPaneCamInfo.ChangeStep(it);
				break;
			}
		}
		else if (strStep.CompareNoCase(UNIT_STRING) == 0)
		{
			for (auto it : m_stUserLayerSet.vcUnitStepIdx)
			{
				m_docPaneCamInfo.ChangeStep(it);
				break;
			}
		}
	}
	else
	{
		POINT stCurPos;
		GetCursorPos(&stCurPos);

		HMENU hMenu = ::CreatePopupMenu();
		if (hMenu == nullptr)
			return RESULT_BAD;

		int nStepCount = static_cast<int>(pJobFile->m_arrStep.GetCount());
		if (nStepCount <= 0)
			return RESULT_BAD;

		for (int nStep = 0; nStep < nStepCount; nStep++)
		{
			CStep *pStep = pJobFile->m_arrStep.GetAt(nStep);
			if (pStep == nullptr) continue;

			::AppendMenu(hMenu, MF_STRING, MENU_STEP_UNIT + nStep, pStep->m_strStepName);

			if (GetCurStepIndex() == nStep)
				::CheckMenuItem(hMenu, MENU_STEP_UNIT + nStep, MF_CHECKED);
		}

		INT uiMenuCmd = ::TrackPopupMenuEx(hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, stCurPos.x, stCurPos.y, AfxGetMainWnd()->GetSafeHwnd(), nullptr);
		m_docPaneCamInfo.ChangeStep(uiMenuCmd - MENU_STEP_UNIT);

		::DestroyMenu(hMenu);
	}

	return RESULT_GOOD;
}

void CMainFrame::OnSetAlign()
{
	if (!m_bIsExtractCam)
		return;

	if (GetMachineType() != eMachineType::eNSIS)
		return;

	UserLayerSet_t* pUserLayerSet = GetUserLayerSet();
	if (pUserLayerSet == nullptr) return;
	SystemSpec_t* pSystemSpec = GetSystemSpec();
	if (pSystemSpec == nullptr) return;
	eMachineType eMcType = pSystemSpec->sysBasic.McType;
	int nCount = static_cast<int>(pUserLayerSet->vcLayerSet.size());

	m_pLayerComboBox->RemoveAllItems();

	for (int i = 0; i < nCount; i++)
	{
		if (pUserLayerSet->vcLayerSet[i]->strDefine == MEASURE_LAYER_TXT)
		{
			pUserLayerSet->vcLayerSet[i]->bCheck = true;
			m_pLayerComboBox->AddItem(pUserLayerSet->vcLayerSet[i]->strLayer);
			m_pLayerComboBox->SelectItem(0);

		}

		//필요 없음.
// 		if (pUserLayerSet->vcLayerSet[i]->strDefine == THICKNESS_LAYER_TXT)
// 		{
// 			pUserLayerSet->vcLayerSet[i]->bCheck = false;
// 		}
	}

	//m_stUserSetInfo.rcpMode = eRecipeMode::eRecipe_MeasureNSIS_Panel;
	m_stUserSetInfo.rcpMode = eRecipeMode::eRecipe_Align_Panel;
	
	m_stUserSetInfo.bShowStepRepeat = false;
	m_stUserSetInfo.bShowSurface = true;
	m_stUserSetInfo.bShowPanelDummy = true;
	m_stUserSetInfo.bShowProfile = true;	

	OnEditMaskButton();

	UpdataMeasureInfoView();	

}

void CMainFrame::OnUpdateSetAlign(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);
	
	BOOL bCheck = FALSE;
	if (m_stUserSetInfo.rcpMode == eRecipeMode::eRecipe_Align_Panel)
	{
		bCheck = TRUE;
	}
	
	pCmdUI->SetCheck(bCheck);

	//m_bEditMode = FALSE;
}

void CMainFrame::OnSetMeasure_NSIS()
{
	if (!m_bIsExtractCam)
		return;

	if (GetMachineType() != eMachineType::eNSIS)
		return;

	m_pLayerComboBox->RemoveAllItems();

	UserLayerSet_t* pUserLayerSet = GetUserLayerSet();
	if (pUserLayerSet == nullptr) return;
	SystemSpec_t* pSystemSpec = GetSystemSpec();
	if (pSystemSpec == nullptr) return;
	eMachineType eMcType = pSystemSpec->sysBasic.McType;
	int nCount = static_cast<int>(pUserLayerSet->vcLayerSet.size());
	for (int i = 0; i < nCount; i++)
	{
		if (pUserLayerSet->vcLayerSet[i]->strDefine == MEASURE_LAYER_TXT)
			{
				pUserLayerSet->vcLayerSet[i]->bCheck = true;
				m_pLayerComboBox->AddItem(pUserLayerSet->vcLayerSet[i]->strLayer);
				m_pLayerComboBox->SelectItem(0);
				
			}

		// 2022.08.05 KJH ADD
		// 08.02 삼성전기 라인에서 주석처리 했으나 NSIS / SR Feature 분리위해 주석 해제
 		if (pUserLayerSet->vcLayerSet[i]->strDefine == THICKNESS_LAYER_TXT)
 		{
 			pUserLayerSet->vcLayerSet[i]->bCheck = false;
 		}
	}
	

	//BOOL bEditMode = TRUE;
	m_bEditMode = TRUE;
	
	if(m_bEditMode == TRUE)
	{
		if (GetMachineType() == eMachineType::eNSIS)
		{
			m_stUserSetInfo.bShowSurface = true;
			m_stUserSetInfo.bShowStepRepeat = false;
			m_stUserSetInfo.bShowPanelDummy = true;
		}
		else
		{
			m_stUserSetInfo.bShowSurface = false;
			m_stUserSetInfo.bShowStepRepeat = false;
		}
	}

	SetViewerEditMode(m_bEditMode);

	// Minimap Mode 확인위한 0 번은 NSIS 1번은 TP 2번은 전체
	Minimap_SetMode(0);

	DrawCamData(FALSE);

	if (m_bEditMode)
		AddApplicationLog(START_EDIT_MP_LAYER_TXT);
	else
		AddApplicationLog(END_EDIT_MODE_TXT); 

	

	if (m_stUserSetInfo.rcpMode == eRecipeMode::eRecipe_MeasureNSIS_Panel)
	{//Check false
		m_stUserSetInfo.rcpMode = eRecipeMode::eRecipe_Monitor_Panel;
		m_stUserSetInfo.bShowPanelDummy = false;
		m_pLayerComboBox->RemoveAllItems();
		SetViewerEditMode(FALSE);
		m_bEditMode = FALSE;
	}
	
	else
	{
		m_stUserSetInfo.rcpMode = eRecipeMode::eRecipe_MeasureNSIS_Panel;
		m_stUserSetInfo.bShowPanelDummy = true;
	}

	UpdataMeasureInfoView();

}

void CMainFrame::OnUpdateSetMeasure_NSIS(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);

	BOOL bCheck = FALSE;
	if (m_stUserSetInfo.rcpMode == eRecipeMode::eRecipe_MeasureNSIS_Panel)
	{
		bCheck = TRUE;
		m_bEditMode = TRUE;
	}

	pCmdUI->SetCheck(bCheck);
}

void CMainFrame::OnSetMeasure_SR()
{
	if (!m_bIsExtractCam)
		return;

	if (GetMachineType() != eMachineType::eNSIS)
		return;

	m_pLayerComboBox->RemoveAllItems();

	//m_pAddLine->OnShow(FALSE);

	UserLayerSet_t* pUserLayerSet = GetUserLayerSet();
	if (pUserLayerSet == nullptr) return;
	SystemSpec_t* pSystemSpec = GetSystemSpec();
	if (pSystemSpec == nullptr) return;
	eMachineType eMcType = pSystemSpec->sysBasic.McType;
	int nCount = static_cast<int>(pUserLayerSet->vcLayerSet.size());
	for (int i = 0; i < nCount; i++)
	{

		if (pUserLayerSet->vcLayerSet[i]->strDefine == THICKNESS_LAYER_TXT)
		{
			pUserLayerSet->vcLayerSet[i]->bCheck = true;
			m_pLayerComboBox->AddItem(pUserLayerSet->vcLayerSet[i]->strLayer);
			m_pLayerComboBox->SelectItem(0);			
		}			

		// 2022.08.05 KJH ADD
		// 08.02 삼성전기 라인에서 주석처리 했으나 NSIS / SR Feature 분리위해 주석 해제
 		if (pUserLayerSet->vcLayerSet[i]->strDefine == MEASURE_LAYER_TXT)
 		{
 			pUserLayerSet->vcLayerSet[i]->bCheck = false;
 		}
	}

	//BOOL bEditMode = TRUE;
	m_bEditMode = FALSE;

	if (m_bEditMode == FALSE)
	{
		if (GetMachineType() == eMachineType::eNSIS)
		{
			m_stUserSetInfo.bShowSurface = true;
			m_stUserSetInfo.bShowStepRepeat = false;
			m_stUserSetInfo.bShowPanelDummy = true;			
		}
		else
		{
			m_stUserSetInfo.bShowSurface = false;
			m_stUserSetInfo.bShowStepRepeat = false;
		}
	}

	//SetViewerEditMode(EditMode::enumMode_EditTP);

	// Minimap Mode 확인위한 0 번은 NSIS 1번은 TP 2번은 전체
	Minimap_SetMode(1);


	DrawCamData(FALSE);

	if (m_bEditMode)
		AddApplicationLog(START_EDIT_TP_LAYER_TXT);
	else
		AddApplicationLog(END_EDIT_MODE_TXT);


	if (m_stUserSetInfo.rcpMode == eRecipeMode::eRecipe_MeasureSR_Panel)
	{//Check false
		m_stUserSetInfo.rcpMode = eRecipeMode::eRecipe_Monitor_Panel;
		m_stUserSetInfo.bShowPanelDummy = false;
		m_pLayerComboBox->RemoveAllItems();
		SetViewerEditMode(FALSE);
		m_bEditMode = FALSE;
	}

	/*else if (m_stUserSetInfo.rcpMode == eRecipeMode::eRecipe_Align_Panel)
	{
		m_stUserSetInfo.rcpMode = eRecipeMode::eRecipe_MeasureSR_Panel;
		m_bEditMode = TRUE;
	}*/


	else
	{
		m_stUserSetInfo.rcpMode = eRecipeMode::eRecipe_MeasureSR_Panel;		
		m_stUserSetInfo.bShowPanelDummy = true;		
	}

	UpdataMeasureInfoView();

	//m_bCheckSR = TRUE;
}

void CMainFrame::OnUpdateSetMeasure_SR(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);
	
	BOOL bCheck = FALSE;
	if (m_stUserSetInfo.rcpMode == eRecipeMode::eRecipe_MeasureSR_Panel)
	{
		bCheck = TRUE;
	}

	pCmdUI->SetCheck(bCheck);
}

void CMainFrame::OnSetMonitorMode()
{
	SetViewerEditMode(FALSE);

	m_pLayerComboBox->RemoveAllItems();


	SetPointMode(FALSE);

	// Minimap Mode 확인위한 0 번은 NSIS 1번은 TP 2번은 전체
	Minimap_SetMode(2);

	DrawCamData(FALSE);

	m_stUserSetInfo.rcpMode = eRecipeMode::eRecipe_Monitor_Panel;

	//필요 없음
// 	UserLayerSet_t* pUserLayerSet = GetUserLayerSet();
// 
// 	
// 	int nCount = static_cast<int>(pUserLayerSet->vcLayerSet.size());
// 	for (int i = 0; i < nCount; i++)
// 	{
// 
// 		if (pUserLayerSet->vcLayerSet[i]->strDefine == MEASURE_LAYER_TXT)
// 		{
// 			pUserLayerSet->vcLayerSet[i]->bCheck = true;
// 		}
// 
// 
// 		if (pUserLayerSet->vcLayerSet[i]->strDefine == THICKNESS_LAYER_TXT)
// 		{
// 			pUserLayerSet->vcLayerSet[i]->bCheck = true;
// 		}
// 
// 
// 	}

	UpdataMeasureInfoView();
}

void CMainFrame::OnUpdateOnSetMonitorMode(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);

	BOOL bCheck = FALSE;
	if (m_stUserSetInfo.rcpMode == eRecipeMode::eRecipe_Monitor_Panel)
	{
		bCheck = TRUE;
	}

	pCmdUI->SetCheck(bCheck);
}

void CMainFrame::OnSetPreSR()
{
	if (!m_bIsExtractCam)
		return;

	if (GetMachineType() != eMachineType::eNSIS)
		return;

	m_stUserSetInfo.prodProcess = eProdProcess::ePreSR;
}

void CMainFrame::OnUpdateSetPreSR(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsExtractCam);
	pCmdUI->SetCheck(m_stUserSetInfo.prodProcess == eProdProcess::ePreSR);
}

void CMainFrame::OnSetPostSR()
{
	if (!m_bIsExtractCam)
		return;

	if (GetMachineType() != eMachineType::eNSIS)
		return;

	m_stUserSetInfo.prodProcess = eProdProcess::ePostSR;
}

void CMainFrame::OnUpdateSetPostSR(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsExtractCam);
	pCmdUI->SetCheck(m_stUserSetInfo.prodProcess == eProdProcess::ePostSR);
}



void CMainFrame::OnSetAlignAuto()
{
	if (!m_bIsExtractCam)
		return;

	if (GetMachineType() != eMachineType::eNSIS)
		return;

	UserLayerSet_t* pUserLayerSet = GetUserLayerSet();
	if (pUserLayerSet == nullptr) return;
	SystemSpec_t* pSystemSpec = GetSystemSpec();
	if (pSystemSpec == nullptr) return;
	eMachineType eMcType = pSystemSpec->sysBasic.McType;
	int nCount = static_cast<int>(pUserLayerSet->vcLayerSet.size());

	m_pLayerComboBox->RemoveAllItems();

	for (int i = 0; i < nCount; i++)
	{
		if (pUserLayerSet->vcLayerSet[i]->strDefine == MEASURE_LAYER_TXT)
		{
			pUserLayerSet->vcLayerSet[i]->bCheck = true;
			m_pLayerComboBox->AddItem(pUserLayerSet->vcLayerSet[i]->strLayer);
			m_pLayerComboBox->SelectItem(0);
		}

		//필요 없음.
// 		if (pUserLayerSet->vcLayerSet[i]->strDefine == THICKNESS_LAYER_TXT)
// 		{
// 			pUserLayerSet->vcLayerSet[i]->bCheck = false;
// 		}
	}

	m_stUserSetInfo.rcpMode = eRecipeMode::eRecipe_Align_Panel;

	m_stUserSetInfo.bShowStepRepeat = false;
	m_stUserSetInfo.bShowSurface = true;
	m_stUserSetInfo.bShowPanelDummy = true;
	m_stUserSetInfo.bShowProfile = true;

	ResetSelect();

	UINT32 nEditMode = EditMode::enumMode_EditMask;
	AddApplicationLog(START_EDIT_MASK_TXT);
	SetMaskSelectMode(MaskType::enumType_Align);
	SetViewerEditMode(nEditMode);		

	m_pAlignMaskEditDlg->OnBnClickedButtonAutoFindAlign();

	//m_bEditMode = TRUE;
}

void CMainFrame::OnUpdateSetAlignAuto(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);

	BOOL bCheck = FALSE;
	if (m_stUserSetInfo.rcpMode == eRecipeMode::eRecipe_Align_Panel)
	{
		bCheck = TRUE;
	}

	pCmdUI->SetCheck(bCheck);
}

void CMainFrame::OnSetAlignManual()
{
	if (!m_bIsExtractCam)
		return;

	if (GetMachineType() != eMachineType::eNSIS)
		return;

	UserLayerSet_t* pUserLayerSet = GetUserLayerSet();
	if (pUserLayerSet == nullptr) return;
	SystemSpec_t* pSystemSpec = GetSystemSpec();
	if (pSystemSpec == nullptr) return;
	eMachineType eMcType = pSystemSpec->sysBasic.McType;
	int nCount = static_cast<int>(pUserLayerSet->vcLayerSet.size());

	m_pLayerComboBox->RemoveAllItems();

	for (int i = 0; i < nCount; i++)
	{
		if (pUserLayerSet->vcLayerSet[i]->strDefine == MEASURE_LAYER_TXT)
		{
			pUserLayerSet->vcLayerSet[i]->bCheck = true;
			m_pLayerComboBox->AddItem(pUserLayerSet->vcLayerSet[i]->strLayer);
			m_pLayerComboBox->SelectItem(0);
		}

		//필요 없음.
// 		if (pUserLayerSet->vcLayerSet[i]->strDefine == THICKNESS_LAYER_TXT)
// 		{
// 			pUserLayerSet->vcLayerSet[i]->bCheck = false;
// 		}
	}

	m_stUserSetInfo.rcpMode = eRecipeMode::eRecipe_Align_Panel;

	m_stUserSetInfo.bShowStepRepeat = false;
	m_stUserSetInfo.bShowSurface = true;
	m_stUserSetInfo.bShowPanelDummy = true;
	m_stUserSetInfo.bShowProfile = true;

	OnEditMaskButton();

	UpdataMeasureInfoView();

}

void CMainFrame::OnUpdateSetAlignManual(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bIsLoadLayers);

	BOOL bCheck = FALSE;
	if (m_stUserSetInfo.rcpMode == eRecipeMode::eRecipe_Align_Panel)
	{
		bCheck = TRUE;
	}

	pCmdUI->SetCheck(bCheck);
}


DOUBLE CMainFrame::GetUserSetAngle()
{
	return m_stUserSetInfo.dAngle;
}

BOOL CMainFrame::GetUserSetMirror()
{
	return m_stUserSetInfo.bMirror;
}

eDirectionType CMainFrame::GetUserSetMirrorDir()
{
	return m_stUserSetInfo.mirrorDir;
}

BOOL CMainFrame::ProcessKill(CString strProcessName)
{
	HANDLE hProcessSnap = NULL;
	BOOL bRet = FALSE;
	PROCESSENTRY32 pe32 = { 0 };
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == (HANDLE)-1) return FALSE;
	pe32.dwSize = sizeof(PROCESSENTRY32); //프로세스가 메모리상에 있으면 첫번째 프로세스를 얻는다 
	if (Process32First(hProcessSnap, &pe32))
	{
		BOOL bCurrent = FALSE;
		MODULEENTRY32 me32 = { 0 };
		do {
			bCurrent = GetProcessModule(pe32.th32ProcessID, strProcessName);
			if (bCurrent)
			{
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
				if (hProcess)
				{
					if (TerminateProcess(hProcess, 0))
					{
						unsigned long nCode; //프로세스 종료 상태 
						GetExitCodeProcess(hProcess, &nCode);
					}
					CloseHandle(hProcess);
				}
			}
		} while (Process32Next(hProcessSnap, &pe32)); //다음 프로세스의 정보를 구하여 있으면 루프를 돈다. 

	}
	CloseHandle(hProcessSnap);
	return TRUE;
}

BOOL CMainFrame::GetProcessModule(DWORD dwPID, CString sProcessName)
{
	HANDLE hModuleSnap = NULL;
	MODULEENTRY32 me32 = { 0 };
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if (hModuleSnap == (HANDLE)-1) return (FALSE);
	me32.dwSize = sizeof(MODULEENTRY32);
	//해당 프로세스의 모듈리스트를 루프로 돌려서 프로세스이름과 동일하면 true를 리턴한다. 
	if (Module32First(hModuleSnap, &me32))
	{
		do {
			if (me32.szModule == sProcessName)
			{
				CloseHandle(hModuleSnap);
				return TRUE;
			}
		} while (Module32Next(hModuleSnap, &me32));
	}
	CloseHandle(hModuleSnap);
	return FALSE;
}

BOOL CMainFrame::GetRunProcess() // 해당 실행 되고 있는 프로세스 있는지 확인 - 있으면 TRUE, 없으면 FALSE
{
	//CString strTemp = _T("iMaster.exe");
	const DWORD process_id = GetCurrentProcessId();
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL); //현재 모든 프로세스들의 상태를 스냅

	if (INVALID_HANDLE_VALUE != hProcessSnap)
	{
		BOOL first = Process32First(hProcessSnap, &pe32);

		do 
		{
			if (process_id == pe32.th32ProcessID)
			//if(strTemp.CompareNoCase(pe32.szExeFile) == 0)
				return TRUE;

		} while (Process32Next(hProcessSnap, &pe32));

		CloseHandle(hProcessSnap);
		
		return FALSE;
	}

	return FALSE;
}
