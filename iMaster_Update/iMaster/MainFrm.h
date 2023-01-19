
// MainFrm.h: CMainFrame 클래스의 인터페이스
//

#pragma once
#include "../iCommonIF/CommonIF-Error.h"
#include "Lib/MFCStatusBarEx.h"
#include "FormViewDocPane.h"
#include "LogViewDocPane.h"
#include "ProgressDlg.h"
#include "LayerInfoFormView.h"
#include "MeasureInfoFormView.h"
#include "MeasureInfoFormView_V2.h"
#include "iMasterCommon.h"
#include "ExtractLayerDlg.h"
#include "MakeMasterDlg.h"
#include "udpsock.h"
#include "FeatureEditFormView.h"
#include "afxRibbonPaletteGallery.h"




#define ID_JOBINFO_DOCPANE				50000
#define ID_CAMINFO_DOCPANE				50010
#define ID_SETTINGS_DOCPANE				50020
#define ID_FEATURE_EDIT_DOCPANE			50050
#define ID_MINIMAP_DOCPANE_V2			50060

#define MODEL_STATUS_POS				0
#define COORDINATE_STATUS_POS			1
#define FOV_STATUS_POS					2

#define WM_AUTO_RECIPE_PIPE				WM_USER+1000
#define WM_CHANGE_LAYER_INFO			WM_USER+1001

//2022.10.12 KJH ADD
#define RIGHT_DOCK_WIDTH_VALUE			900
#define LEFT_DOCK_WIDTH_VALUE			400

class CFeatureEdit_ResizeDlg;
class CFeatureEdit_MoveDlg;
class CFeatureEdit_DeleteDlg;
class CFeatureEdit_AddDlg;
class CFeatureEdit_CopyDlg;
class CFeatureEdit_RotateDlg;
class CFeatureEdit_FeatureDlg;
class CAlignMaskEditDlg;
class CUtils;

class CErrorInfo : public CObject
{
public:
	UINT32 m_nResult;
	int m_nErrorCode;

	CErrorInfo()
	{
		m_nResult = RESULT_GOOD;
		m_nErrorCode = (int)ODB_ERROR_CODE::NONE;
	}
};

class CMainFrame : public CFrameWndEx
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
private:
	CProgressDlg*		m_pProgressDlg;

	CFormViewDocPane	m_docPaneJobInfo;
	CLogViewDocPane		m_docPaneLogView;
	CFormViewDocPane	m_docPaneCamInfo;
	CFormViewDocPane    m_docPaneSettings;
	CLayerInfoFormView  m_docPaneLayerInfo;
	CMeasureInfoFormView m_docPaneMeasureInfo;
	CMeasureInfoFormView_V2 m_docPaneMeasureInfo_V2;
	//CMiniMapFormView_V2    m_docPaneMinimapView_V2;
	CFormViewDocPane    m_docPaneMinimapView_V2;
	CFormViewDocPane	m_docPaneFeatureEdit;

	CMFCRibbonComboBox *m_pLayerComboBox;

	UserSetInfo_t		m_stUserSetInfo;
	UserLayerSet_t		m_stUserLayerSet;

	MeasurePanel_t		m_stMeasureData;	//for NSIS
	CUDPSock			m_UDPforNSIS;

	BOOL				m_bIsExtractCam;
	BOOL				m_bIsLoadLayers;
	BOOL				m_bEditMode = FALSE;
	BOOL				m_bCheckSR = FALSE;
	BOOL				m_bCheckSRAuto = FALSE;

	BOOL				m_bIsProcessMaster;
	CObArray			m_pMakeMasterResult;
	BOOL				m_bAutoRun;
	

	BOOL				m_bIsAllLayerLoaded;
	std::vector<stLayerPair*> m_vcLayerList;

	FeatureAddPadParam		m_stAddParam;


	//Feature Edit Dialog
	CFeatureEdit_ResizeDlg	*m_pFeatureEditResize = nullptr;
	CFeatureEdit_MoveDlg	*m_pFeatureEditMove = nullptr;
	CFeatureEdit_DeleteDlg	*m_pFeatureEditDelete = nullptr;
	CFeatureEdit_AddDlg		*m_pFeatureEditAdd = nullptr;
	CAlignMaskEditDlg		*m_pAlignMaskEditDlg = nullptr;
	CFeatureEdit_CopyDlg	*m_pFeatureEditCopy = nullptr;
	CFeatureEdit_RotateDlg  *m_pFeatureEditRotate = nullptr;
	CFeatureEdit_FeatureDlg *m_pFeatureEditFeature = nullptr;
// 작업입니다.

	CMFCRibbonButton *m_pAddLine;

public:
	BOOL				m_bProgrameCheck = FALSE;
	CString				m_strEndLayer;
	CString				m_strStartLayer;

	vector<vector<stLayerInfo*>> m_pPatternLayerInfo;
	vector<vector<stLayerInfo*>> m_pSRLayerInfo;
	vector<vector<stLayerInfo*>> m_pMPLayerInfo;
	vector<vector<stLayerInfo*>> m_pTPLayerInfo;

	//Feature Add 시 사용자 입력 정보
	CString m_strType = _T("");
	CString m_strLayer = _T("");
	CString m_strSymbol = _T("");
	int		m_nLineMode = 0;//0 : X Mode, 1 : Y Mode, 2 : XY Mode

// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW/* | FWS_ADDTOTITLE*/, CWnd* pParentWnd = nullptr, CCreateContext* pContext = nullptr);

	void IsDoneExtractCam(BOOL b);
	void IsDoneLoadLayers(BOOL b);
	void IsDoneProcessMaster(BOOL b);
	void IsAllLayerLoaded(BOOL b);
	BOOL GetFlag_ExtractCam() { return m_bIsExtractCam; }
	BOOL GetFlag_LoadLayers() {	return m_bIsLoadLayers;	}
	BOOL GetFlag_ProcessMaster() { return m_bIsProcessMaster; }
	BOOL GetFlag_IsAllLayerLoaded() { return m_bIsAllLayerLoaded; }

	void AddApplicationLog(CString strLog, LogMsgType enMsgType = LogMsgType::EN_INFORMATION);
	void ShowPopupInitialize(CString strText = L"");
	void HidePopupInitialize();
	void AdjustDockPanes();

	void SetStepCount(UINT32& iStepCnt, std::vector<UINT32>& vcStepRepeatCnt);
	UINT32 GetStepInfo(std::vector<stStepInfo*>** v);
	UINT32 DisplayCamInfo();
	//UINT32 DisplayLayerInfo(INT32 iFrontlayerIdx = -1, INT32 iBacklayerIdx = -1);
	UINT32 DisplayLayerInfo(IN const CString& strFrontLayer = _T(""), IN const CString& strBackLayer = _T(""));
	UINT32 EstimateStepOrient();

	void SetLayerCount(UINT32& iLayerCnt);
	UINT32 GetLayerInfo(std::vector<stLayerInfo*>** v,
		vector<vector<stLayerInfo*>> &vecPatternList,
		vector<vector<stLayerInfo*>> &vecSRList,
		vector<vector<stLayerInfo*>> &vecMPList,
		vector<vector<stLayerInfo*>> &vecTPList);

	void SetComponentCount(UINT32& iStepCnt, std::vector<UINT32>& vcCompCnt);
	UINT32 GetComponentInfo(std::vector<stComponentInfo*>** v);

	UserLayerSet_t*	GetUserLayerSet() { return &m_stUserLayerSet; };
	UserSetInfo_t* GetUserSetInfo() { return &m_stUserSetInfo; }
	MeasurePanel_t* GetMeasureData() { return &m_stMeasureData; }

	void EndExtractLayer();
	void EndMakeMaster();
	INT32 GetCurStepIndex();

	//JobList 갱신
	UINT32 Refresh_JobInfoList();

	void UpdateStatusBar(int nIndex, CString strModel);

	
	// 2022.09.23 KJH ADD
	// 중복실행방지위해 추가
	BOOL GetRunProcess(); // 해당 실행 되고 있는 프로세스 있는지 확인 - 있으면 TRUE, 없으면 FALSE
	BOOL GetProcessModule(DWORD dwPID, CString sProcessName);
	BOOL ProcessKill(CString strProcessName);
	

	int GetCode_MakeMaster();
	void InitFlag_MakeMasterResult();
	void SetFlag_MakeMasterResult(UINT32 result, int errorcode);
	BOOL GetFlag_MakeMasterResult();
	void SetFlag_AutoRun(BOOL autorun);
	BOOL GetFlag_AutoRun();

	BOOL SendPipeNSIS(CString lpszFormat);
	BOOL AutoRunODBParsing(CString csODBFileName);
	BOOL CheckLayerInfo();
	void ClearLayerList();
	UINT32 SetDrawLayer(eProdSide side);
	void ChangeSide(eProdSide side);
	UINT32 ChangeLayerInfo(CString strLayer);


	
	//Select Point
	UINT32 SetSetlectPoint(IN const int &nIndex, IN const float &fX, IN const float &fY);
	UINT32 SetSetlectPoint_Center(IN const int &nIndex, IN const float &fX, IN const float &fY);

	//Select Feature Info
	UINT32 SetSetlectFeatureInfo(IN SELECT_INFO *pSelectInfo);

	//Get Feature Dlg Ptr
	CFeatureEdit_AddDlg *GetFeature_AddDlg() { return m_pFeatureEditAdd; };
	CAlignMaskEditDlg *GetAlignMaskEditDlg() { return m_pAlignMaskEditDlg; };

	void LoadLayer();
	void ShowHideMeasureInfo(BOOL bShow);
	void ShowHideMinimapView(BOOL bShow);
	void ShowHideFeatureEdit();
	// 2022.08.08 KJH ADD
	// NSIS 경우 Layer 자동선택 안됬을 때 LayerInfo 표시
	void ShowHideLayerInfo();

	// 2022.09.20 KJH ADD
	// NSIS Align 모서리 위치 이동 
	DOUBLE GetUserSetAngle();
	BOOL   GetUserSetMirror();
	eDirectionType GetUserSetMirrorDir();

	CString GetWorkLayer();
	UINT32 LoadSpecData();

	UINT32 GetHisto_PatternWidth(IN const int &eLineOption, std::map<INT32, std::vector<CString>> &vcHisto_Pos, std::map<INT32, std::vector<CString>> &vcHisto_Neg);
	UINT32 ChangeStep(CString strStep = _T(""));

	//MeasureInfoView
	UINT32 UpdataMeasureInfoView();
	UINT32 SetMeasure_ResetSelect();
	UINT32 SetMeasure_Select_UI();
	void ClearMeasureData();

	CString GetLayerName();
	BOOL	GetCheckSR();
	BOOL	GetCheckSRAuto();

	UINT32 LoadFeatureData();
	UINT32 LoadMoveData();
	UINT32 LoadMove_KeyData();

	//Feauure Add : Feature Base
	UINT32 GetLineInfo_PadBase(IN const SELECT_INFO_UNIT &stUnitInfo, IN const int &nLineMode,
		OUT float &fXs, OUT float &fYs, OUT float &fXe, OUT float &fYe);
	UINT32 GetLineInfo_LineBase(IN SELECT_INFO_UNIT &stUnitInfo, IN const int &nLineMode,
		OUT float &fXs, OUT float &fYs, OUT float &fXe, OUT float &fYe);
	UINT32 GetLineInfo_ArcBase(IN SELECT_INFO_UNIT &stUnitInfo, IN const int &nLineMode,
		OUT float &fXs, OUT float &fYs, OUT float &fXe, OUT float &fYe);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	CFont						m_StatusBarFont;
	CMFCRibbonBar				m_wndRibbonBar;
	CMFCRibbonApplicationButton m_btnRibbonApp;
	CMFCStatusBarEx				m_wndStatusBar;

	CExtractLayerDlg			m_dlgExtractLayer;
	CMakeMaterDlg				m_dlgMakeMaster;
	
	INT32						m_iPort;
	CString						m_strSaveFolder = _T("");
	CString						m_csIP;

	bool						m_bMirrorH = false;
	bool						m_bMirrorV = false;

private:
	BOOL CreateFormViewDocPane(INT32 iResId, CString strCaption, UINT nSizeX, UINT nSizeY, DWORD dwPaneStyle, CFormViewDocPane* pWndDocPane);
	BOOL CreateLogViewDocPane(CString strPath);
	BOOL CreateLayerInfoDocPane();
	BOOL CreateMeasureInfoDocPane();
	BOOL CreateMeasureInfo_V2_DocPane();
	//BOOL CreateMinimap_V2_DocPane();
	BOOL InitializeStatusBar();


	void InitializeRibbon();	
	//vector<CString> InitializeLanguage(eSelection::Language);
	
	CMFCRibbonButton* CreateButton(INT32 iButtonID, INT32 iResIcon, CString strToolTip);
	CMFCRibbonButton* CreateButton_V2(INT32 iButtonID,CString strButtonName, INT32 iResIcon,CString strToolTip);
	CMFCRibbonButton *m_pAddLineX;
	CMFCRibbonPanel *m_pPanelAdd;

	void InitializeFont();
	void ReleaseFont();

	void OnRefreshButton();
	void OnUpdateRefreshButton(CCmdUI* pCmdUI);
	void OnSettingButton();
	void OnUpdateSettingButton(CCmdUI* pCmdUI);
	void OnLineModeButton();
	void OnUpdateLineModeButton(CCmdUI* pCmdUI);
	void OnAreaModeButton();
	void OnUpdateAreaModeButton(CCmdUI* pCmdUI);
	void OnPanelModeButton();
	void OnUpdatePanelModeButton(CCmdUI* pCmdUI);
	void OnViewFrontButton();
	void OnUpdateViewFrontButton(CCmdUI* pCmdUI);
	void OnViewBackButton();
	void OnUpdateViewBackButton(CCmdUI* pCmdUI);
	void OnLoadLayer();
	void OnUpdateLoadLayer(CCmdUI* pCmdUI);
	void OnMakeAlign();
	void OnUpdateMakeAlign(CCmdUI* pCmdUI);
	void OnMakeMaster();
	void OnUpdateMakeMaster(CCmdUI* pCmdUI);
	void OnTransferMaster();
	void OnUpdateTransferMaster(CCmdUI* pCmdUI);

	void OnSetPreSR();
	void OnUpdateSetPreSR(CCmdUI* pCmdUI);
	void OnSetPostSR();
	void OnUpdateSetPostSR(CCmdUI* pCmdUI);

	void OnSetAlign();
	void OnUpdateSetAlign(CCmdUI* pCmdUI);
	
	void OnSetAlignAuto();
	void OnUpdateSetAlignAuto(CCmdUI* pCmdUI);
	void OnSetAlignManual();
	void OnUpdateSetAlignManual(CCmdUI* pCmdUI);

	void OnSetAlign_Panel();
	void OnSetMeasure_NSIS();
	void OnUpdateSetMeasure_NSIS(CCmdUI* pCmdUI);
	
	void OnSetMeasure_SR();
	void OnUpdateSetMeasure_SR(CCmdUI* pCmdUI);
	
	void OnSetMonitorMode();
	void OnUpdateOnSetMonitorMode(CCmdUI* pCmdUI);

	void OnPreviewButton();
	void OnUpdatePreviewButton(CCmdUI* pCmdUI);
	void OnViewFOVButton();
	void OnUpdateViewFOVButton(CCmdUI* pCmdUI);
	void OnViewCellButton();
	void OnUpdateViewCellButton(CCmdUI* pCmdUI);
	void OnViewStepRepeatButton();
	void OnUpdateViewStepRepeatButton(CCmdUI* pCmdUI);
	void OnViewProfileButton();
	void OnUpdateViewProfileButton(CCmdUI* pCmdUI);
	void OnViewSurfaceButton();
	void OnUpdateViewSurfaceButton(CCmdUI* pCmdUI);
	void OnViewBoundaryButton();
	void OnUpdateViewBoundaryButton(CCmdUI* pCmdUI);

	void OnViewAlignButton();
	void OnUpdateViewAlignButton(CCmdUI* pCmdUI);
	void OnViewMaskButton();
	void OnUpdateViewMaskButton(CCmdUI* pCmdUI);

	void OnMirrorHButton();
	void OnUpdateMirrorHButton(CCmdUI* pCmdUI);
	void OnMirrorVButton();
	void OnUpdateMirrorVButton(CCmdUI* pCmdUI);
	void OnRotation90Button();
	void OnUpdateRotation90Button(CCmdUI* pCmdUI);
	void OnRotation180Button();
	void OnUpdateRotation180Button(CCmdUI* pCmdUI);
	void OnRotation270Button();
	void OnUpdateRotation270Button(CCmdUI* pCmdUI);
	void OnRotation360Button();
	void OnUpdateRotation360Button(CCmdUI* pCmdUI);
	
	void OnFovHScanButton();
	void OnUpdateFovHScanButton(CCmdUI* pCmdUI);
	void OnFovVScanButton();
	void OnUpdateFovVScanButton(CCmdUI* pCmdUI);
	void OnFovHJigjagButton();
	void OnUpdateFovHJigjagButton(CCmdUI* pCmdUI);
	void OnFovVJigjagButton();
	void OnUpdateFovVJigjagButton(CCmdUI* pCmdUI);
	void OnRulerButton();
	void OnUpdateRulerButton(CCmdUI* pCmdUI);

	//Save
	void OnSaveButton();
	void OnUpdateSaveButton(CCmdUI* pCmdUI);
	void OnSaveAsButton();
	void OnUpdateSaveAsButton(CCmdUI* pCmdUI);

	//Edit Rib Button
	void OnEditModeButton();
	void OnUpdateEditModeButton(CCmdUI* pCmdUI);

	void OnEditResizeButton();
	void OnUpdateEditResizeButton(CCmdUI* pCmdUI);
	void OnEditMoveButton();
	void OnUpdateEditMoveButton(CCmdUI* pCmdUI);
	void OnEditDeleteButton();
	void OnUpdateEditDeleteButton(CCmdUI* pCmdUI);

	void OnEditCopyButton();
	void OnUpdateEditCopyButton(CCmdUI* pCmdUI);

	void OnEditAddButton();
	void OnUpdateEditAddButton(CCmdUI* pCmdUI);

	void OnEditRotateButton();
	void OnUpdateEditRotateButton(CCmdUI* pCmdUI);

	void OnEditMaskButton();
	void OnUpdateEditMaskButton(CCmdUI* pCmdUI);

	void OnEditFeatureButton();
	void OnUpdateEditFeauterButton(CCmdUI* pCmdUI);


	void OnTestSaveButton();
	void OnUpdateTestSaveButton(CCmdUI* pCmdUI);
	
	void OnViewJobInfo();
	void OnViewCamInfo();
	void OnViewLayerInfo();
	void OnViewMeasureInfo();
	void OnViewMinimap();
	void OnViewLogInfo();
	void OnVersionButton();
	void ShowHideFormView(UINT id);

	

	//Feature Edit Dialog
	void _CreateFeatureEditDlg();
	void _DeleteFeatureEditDlg();

	//Feaure Add
	void onAddSurfaceButton();
	void OnUpdateSurfaceButton(CCmdUI* pCmdUI);
	void OnAddTpButton();
	void OnUpdateTpButton(CCmdUI* pCmdUI);
	void OnAddArcButton();
	void OnUpdateArcButton(CCmdUI* pCmdUI);
	void OnAddLineButton();
	void OnUpdateLineButton(CCmdUI* pCmdUI);

	void OnAddCircleButton();
	void OnUpdateCircleButton(CCmdUI* pCmdUI);
	void OnAddRectangleButton();
	void OnUpdateRectagneButton(CCmdUI* pCmdUI);
	void OnAddLineXButton();
	void OnUpdateAddLineXButton(CCmdUI* pCmdUI);
	void OnAddLineYButton();
	void OnUpdateLineYButton(CCmdUI* pCmdUI);

	void OnAddTpAutoButton();
	void OnUpdateTpAutoButton(CCmdUI* pCmdUI);
	
	//Feature Base
		//Line X
	void OnAddLineX_FeatureBaseButton();
	void OnUpdateLineX_FeatureBaseButton(CCmdUI* pCmdUI);
		//Line Y
	void OnAddLineY_FeatureBaseButton();
	void OnUpdateLineY_FeatureBaseButton(CCmdUI* pCmdUI);
		//Circle
	void OnAddCircle_FeatureBaseButton();
	void OnUpdateCircle_FeatureBaseButton(CCmdUI* pCmdUI);
		//Rect
	void OnAddRect_FeatureBaseButton();
	void OnUpdateRect_FeatureBaseButton(CCmdUI* pCmdUI);
	//

	void OnUpdateLayerComboBox(CCmdUI* pCmdUI);
	//void OnAddSquareButton();
// 	void OnAddRectxrButton();
// 	void OnAddRectxcButton();
// 
// 	void OnAddOvalButton();
// 	void OnAddDiamondButton();
// 	void OnAddOctagonButton();
// 	void OnAddDonut_rButton();
// 	void OnAddDonut_sButton();
// 
// 	void OnAddHex_lButton();
// 	void OnAddHex_sButton();
// 	void OnAddBfrButton();
// 	void OnAddBfsButton();
// 	void OnAddTriButton();
// 
// 	void OnAddOval_hButton();
// 	void OnAddThrButton();
// 	void OnAddThsButton();
// 	void OnAddS_ThsButton();
// 	void OnAddS_ThoButton();
// 
// 	void OnAddSr_ThsButton();
// 	void OnAddRc_ThsButton();
// 	void OnAddRc_ThoButton();
// 	void OnAddElButton();
// 	void OnAddMorieButton();
// 
// 	void OnAddHoleButton();


	



	void SetLayerName();


public:
	void	LockJobTree(BOOL bEnable);

	//시간 측정
	double m_dLoadTime = 0.0;
	double m_dExtractTime = 0.0;
	double m_dMasterDataTime = 0.0;

// 생성된 메시지 맵 함수
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg LRESULT OnNsisPipe(WPARAM wParam, LPARAM lParam);
//	afx_msg LRESULT OnChangeLayerInfo(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


