#pragma once

#include "../iCommonIF/CommonIF-Define.h"
#include "../iCommonIF/CommonIF-Error.h"
#include "SystemSpec.h"

#define COLOR_BLACK							RGB(0,0,0)
#define COLOR_WHITE							RGB(255,255,255)
#define COLOR_GREEN1						RGB(204, 255, 204)
#define COLOR_GREEN2						RGB(64, 96, 96)
#define COLOR_GRAY1							((COLORREF)-1)

#define STRING_BASIC						_T("Basic")

#define STRING_USER_SITE					_T("User Site")
#define STRING_USER_SITE_LGIT				_T("LGIT")
#define STRING_USER_SITE_DAEDUCK			_T("DAEDUCK")
#define STRING_USER_SITE_SEM				_T("SEM")
#define STRING_USER_SITE_NONE				_T("NONE")
#define STRING_USER_SITE_DESC				_T("Set User Site")

#define STRING_MC_TYPE						_T("Machine Type")
#define STRING_MC_TYPE_NSIS					_T("NSIS")
#define STRING_MC_TYPE_AOI					_T("AOI")
#define STRING_MC_TYPE_AVI					_T("AVI")
#define STRING_MC_TYPE_NONE					_T("NONE")
#define STRING_MC_TYPE_DESC					_T("Set Machien Type")

#define STRING_PLATFORM_TYPE				_T("Platform Type")
#define STRING_PLATFORM_TYPE_BGA			_T("BGA")
#define STRING_PLATFORM_TYPE_FCB			_T("FCB")
#define STRING_PLATFORM_TYPE_HDI			_T("HDI")
#define STRING_PLATFORM_TYPE_DESC			_T("One of: BGA, HDI or FCB")

#define STRING_PRODUCT_TYPE				    _T("Product Type")
#define STRING_PRODUCT_TYPE_STRIP			_T("STRIP")
#define STRING_PRODUCT_TYPE_PANEL			_T("PANEL")
#define STRING_PRODUCT_TYPE_DESC			_T("One of: STRIP or PANEL")

#define STRING_CAM_DATA_TYPE				_T("Cam Data Type")
#define STRING_CAM_DATA_TYPE_ODB			_T("ODB++")
#define STRING_CAM_DATA_TYPE_GERBER			_T("GERBER")
#define STRING_CAM_DATA_TYPE_DXF			_T("DXF")
#define STRING_CAM_DATA_TYPE_DESC			_T("One of: ODB++, GERBER or DXF")

#define STRING_MASTER_DATA_LEVEL			_T("Master Data Level")
#define STRING_MASTER_DATA_BLOCK_LEVEL		_T("BLOCK")
#define STRING_MASTER_DATA_PANEL_LEVEL		_T("PANEL")
#define STRING_MASTER_DATA_LEVEL_DESC		_T("One of: BLOCK or PANEL")

#define STRING_STEP_REPEAT_MODE				_T("Step Repeat Mode")
#define STRING_STEP_REPEAT_MODE_SHOW_ONE	_T("SHOW ONE")
#define STRING_STEP_REPEAT_MODE_SHOW_ALL	_T("SHOW ALL")
#define STRING_STEP_REPEAT_MODE_DESC		_T("One of: Show One or Show All")

#define STRING_REFERANCE_REGION				_T("Reference Region")
#define STRING_REFERANCE_REGION_FEATURE		_T("FEATURE")
#define STRING_REFERANCE_REGION_PROFILE	    _T("PROFILE")
#define STRING_REFERANCE_REGION_DESC		_T("One of: FEATURE or PROFILE")

#define STRING_EXCEPT_DUMMY				    _T("Except Dummy")
#define STRING_EXCEPT_DUMMY_TRUE	        _T("TRUE")
#define STRING_EXCEPT_DUMMY_FALSE	        _T("FALSE")
#define STRING_EXCEPT_DUMMY_DESC		    _T("One of: TRUE or FALSE")

#define STRING_KEEP_LOCAL_COPY			     _T("Keep Local Copy")
#define STRING_KEEP_LOCAL_COPY_TRUE			 _T("TRUE")
#define STRING_KEEP_LOCAL_COPY_FALSE		 _T("FALSE")
#define STRING_KEEP_LOCAL_DESC		         _T("One of: TRUE or FALSE")

#define STRING_SCAN_TYPE			        _T("Scan Type")
#define STRING_SCAN_TYPE_LINE	            _T("LINE SCAN")
#define STRING_SCAN_TYPE_AREA	            _T("AREA SCAN")
#define STRING_SCAN_TYPE_DESC		        _T("One of: LINE SCAN or AREA SCAN")

#define STRING_MAX_LAYER_NUM				_T("Max Layer Number")
#define STRING_MAX_IPU_NUM					_T("Max IPU Number")

#define STRING_IMAGE_TYPE					_T("Image Type")
#define STRING_IMAGE_TYPE_BMP				_T("bmp")
#define STRING_IMAGE_TYPE_TIF				_T("tif")
#define STRING_IMAGE_TYPE_PNG				_T("png")
#define STRING_IMAGE_TYPE_JPG				_T("jpg")
#define STRING_IMAGE_TYPE_DESC				_T("One of Types")

#define STRING_EXCEPT_IMAGE				    _T("Except Image Data(Master Data)")
#define STRING_EXCEPT_IMAGE_TRUE	        _T("Expect Master Image")
#define STRING_EXCEPT_IMAGE_FALSE	        _T("All Save")
#define STRING_EXCEPT_IMAGE_DESC		    _T("One of: All Save or Expect Master Image")

#define STRING_LINEARC_SUBSAVE				_T("Line Arc Save By Line Width")
#define STRING_LINEARC_SUBSAVE_TRUE	        _T("Save By Line Width")
#define STRING_LINEARC_SUBSAVE_FALSE	    _T("Basic Mode")
#define STRING_LINEARC_SUBSAVE_DESC		    _T("One of: Basic Mode or Save By Line Width")

#define STRING_2D_MEASURE		            _T("2D Measure")
#define STRING_3D_MEASURE	                _T("3D Measure")
#define STRING_MAGNIFICATION                _T("Mag")
#define STRING_ZOOM							_T("Zoom")
#define STRING_RESOLUTION                   _T("Resolution")
#define STRING_FOV_WIDTH                    _T("FOV Width")
#define STRING_FOV_HEIGHT                   _T("FOV Height")
#define STRING_USER_EDITABLE                _T("Edit value")
#define STRING_CAMERA_ORIENT				_T("Camera Orient")
#define STRING_CAMERA_ORIENT_TOP			_T("Top")
#define STRING_CAMERA_ORIENT_BOT			_T("Bottom")
#define STRING_ALIGN_RATIO					_T("Align Ratio")
#define STRING_ALIGN						_T("Align")
#define STRING_UNIT_ALIGN					_T("Unit Align")
#define STRING_NETWORK						_T("Network")
#define STRING_NSIS_PORT					_T("NSIS Port")
#define STRING_CAMERA_PORT					_T("Camera Port")
#define STRING_NSIS_IP						_T("NSIS IP")
#define STRING_CAMERA_IP					_T("Camera IP")
#define STRING_STATIC_OPTIC					_T("Static Optic")
#define STRING_ALIGN_MAG					_T("Align Mag")
#define STRING_ALIGN_ZOOM					_T("Align Zoom")
#define STRING_UNIT_ALIGN_MAG				_T("Unit Align Mag")
#define STRING_UNIT_ALIGN_ZOOM				_T("Unit Align Zoom")
#define STRING_ZOOM_MARGIN					_T("Zoom Margin")

#define STRING_LINE_CAMERA                  _T("Line Camera")
#define STRING_AREA_CAMERA                  _T("Area Camera")
#define STRING_CAM_RESOLUTION               _T("Resolution [㎛]")
#define STRING_FRAME_WIDTH                  _T("Frame Width [pxl]")
#define STRING_FRAME_HEIGHT                 _T("Frame Height [pxl]")
#define STRING_MIN_OVRLP_X                  _T("Min Overlap X [pxl]")
#define STRING_MIN_OVRLP_Y                  _T("Min Overlap Y [pxl]")
#define STRING_MIN_MARGIN_X                 _T("Min Margin X [pxl]")
#define STRING_MIN_MARGIN_Y                 _T("Min Margin Y [pxl]")
#define STRING_CELL_COL_NUM                 _T("Cell Col Num")
#define STRING_CELL_ROW_NUM                 _T("Cell Row Num")
#define STRING_CELL_MIN_OVERLAP_X           _T("Cell Min Overlap X [pxl]")
#define STRING_CELL_MIN_OVERLAP_Y           _T("Cell Min Overlap Y [pxl]")
#define STRING_USER_EDITABLE                _T("Edit value")
#define STRING_DUMMY_MARGIN_X               _T("Dummy Margin X [mm]")
#define STRING_DUMMY_MARGIN_Y               _T("Dummy Margin Y [mm]")

#define STRING_PATH                         _T("Path Directory")
#define STRING_CAM_DATA_PATH				_T("Cam Data Path")
#define STRING_MASTER_DATA_PATH             _T("Master Data Path")
#define STRING_LOG_PATH						_T("Log Data Path")

#define STRING_STEP_DEFINE                  _T("Step Define")
#define STRING_PNL_STEP						_T("Panel")
#define STRING_STRIP_STEP					_T("Strip")
#define STRING_UNIT_STEP					_T("Unit")
#define STRING_LAYER_DEFINE					_T("Layer Define")
#define STRING_MEASURE_RATIO_DEFINE			_T("Measure Ratio Define")
#define STRING_SAVE_MEASURE_POINT_DEFINE	_T("Save Measure Point Image Define")


// 2022.10.12 KJH ADD
// NSIS SEM AUTO Align Spec ADD
#define STRING_AUTO_ALIGN					_T("AUTO ALIGN")
#define STRING_AUTO_ALIGN_RECT_WIDTH		_T("RECT WIDTH")
#define STRING_AUTO_ALIGN_RECT_HEIGHT		_T("RECT HEIGHT")
#define STRING_AUTO_ALIGN_POS_X				_T("AUTO ALIGN X")
#define STRING_AUTO_ALIGN_POS_Y				_T("AUTO ALIGN Y")


// CSettingsFormView 폼 보기

struct stGridCamSpec
{
	CMFCPropertyGridProperty *pMode;
	CMFCPropertyGridProperty *pMagnification;
	CMFCPropertyGridProperty *pZoom;
	CMFCPropertyGridProperty *pResolution;
	CMFCPropertyGridProperty *pFovWidth;
	CMFCPropertyGridProperty *pFovHeight;

	stGridCamSpec() : pMode(nullptr), pMagnification(nullptr), pZoom(nullptr),
		pResolution(nullptr), pFovWidth(nullptr), pFovHeight(nullptr) {};
};

class CSettingsFormView : public CFormView
{
	DECLARE_DYNCREATE(CSettingsFormView)

protected:
	CSettingsFormView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CSettingsFormView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SETTINGS_DLG };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:
	BOOL					m_bInit = FALSE;
	CButton					m_btnSaveApply;
	CButton					m_btnReset;
	CMFCPropertyGridCtrl	m_PropGridCtrl;

	CMFCPropertyGridProperty *m_pUserSite, *m_pMcType, *m_pPlatType, *m_pProdType, *m_pCamType, *m_pMasterLevel;
	CMFCPropertyGridProperty *m_pStepRepeatMode, *m_pReferenceMode, *m_pDummyMode, *m_pKeepLocalCopy, *m_pExceptImageData, *m_pLineArcSubSave;
	CMFCPropertyGridProperty *m_pScanType, *m_pMaxLayerNum, *m_pIPUNum, *m_pImageType;

	CMFCPropertyGridProperty *m_pResolution[2], *m_pFrameWidth[2], *m_pFrameHeight[2], *m_pOverlapX[2],
							 *m_pOverlapY[2], *m_pMarginX[2], *m_pMarginY[2], *m_pCellX[2], *m_pCellY[2],
							 *m_pCellOverlapX[2], *m_pCellOverlapY[2], *m_pDummyMarginX[2], *m_pDummyMarginY[2];

	stGridCamSpec m_st3DCamProp[MAX_3D_MODE_NUM];
	stGridCamSpec m_st2DCamProp[MAX_2D_MODE_NUM];

	CMFCPropertyGridProperty *m_pCamOrient[2], *m_pAlignRatio[2], *m_pZoomMarginRatio, *m_pNetwork[4], *m_pStaticOptic[4];
	CMFCPropertyGridProperty *m_pMeasureDefaultRatio, *m_pMeasureRatio[static_cast<UINT32>(MeasureType::MeasureTypeNone)];
	//YJD START 22.06.13 - SAVE MEASURE IMAGE
	CMFCPropertyGridProperty *m_pUseSaveMeasurePoint, *m_pSaveMeasurePointScale;
	//YJD END 22.06.13 - SAVE MEASURE IMAGE

	// 2022.11.02 KJH ADD
	// Unit Layer Image Save Parameter
	CMFCPropertyGridProperty *m_pUseSaveUnitLayer;

	CMFCPropertyGridFileProperty*m_pCamPath, *m_pMasterPath, *m_pLogPath;
	CMFCPropertyGridProperty *m_pPnlStepName, *m_pStripStepName, *m_pUnitStepName;

	CMFCPropertyGridProperty* m_pFrontLayerName[static_cast<UINT32>(eRefLayer::EN_MAX_LAYER_NUM)];
	CMFCPropertyGridProperty* m_pBackLayerName[static_cast<UINT32>(eRefLayer::EN_MAX_LAYER_NUM)];
	CMFCPropertyGridColorProperty* m_pLayerColor[static_cast<UINT32>(eRefLayer::EN_MAX_LAYER_NUM)];

	// 2022.10.12 KJH ADD
	// NSIS SEM Auto Align Spec ADD
	CMFCPropertyGridProperty *m_pAutoAlign, *m_pAutoAlignRect[2], *m_pAutoAlignPosX[4], *m_pAutoAlignPosY[4];

private:	
	UINT32 SetBasicSpec(BasicSpec_t* pSpec);
	UINT32 SetCameraSpec(bool bIsLine, CameraSpec_t* pSpec);
	UINT32 SetCameraSpec_for_NSIS(bool bIs3D, std::vector<NSIS_CameraSpec_t*>& pSpec);
	UINT32 SetPathSpec(PathSpec_t* pSpec);
	UINT32 SetStepSpec(StepSpec_t* pSpec);
	UINT32 SetLayerSpec(LayerSpec_t* pSpec);
	UINT32 SetCameraOrient(NSISSpec_t* pSpec);
	UINT32 SetAlignRatio(NSISSpec_t* pSpec);	
	UINT32 SetNetwork(NSISSpec_t* pSpec);
	UINT32 SetStaticOptic(NSISSpec_t* pSpec);
	UINT32 SetMeasureRatioSpec(NSISSpec_t* pSpec);
	//YJD START 22.06.13 - SAVE MEASURE IMAGE
	UINT32 SetSaveMeasurePointSpec(NSISSpec_t* pSpec);
	//YJD END 22.06.13 - SAVE MEASURE IMAGE

	UINT32 SaveSpecData();
	UINT32 GetBasicSpec(BasicSpec_t* pSpec);
	UINT32 GetCameraSpec(bool bIsLine, CameraSpec_t* pSpec);
	UINT32 GetCameraSpec_for_NSIS(bool bIs3D, std::vector<NSIS_CameraSpec_t*>& pSpec);
	UINT32 GetPathSpec(PathSpec_t* pSpec);
	UINT32 GetStepSpec(StepSpec_t* pSpec);
	UINT32 GetLayerSpec(LayerSpec_t* pSpec);
	UINT32 GetCameraOrient(NSISSpec_t* pSpec);
	UINT32 GetAlignRatio(NSISSpec_t* pSpec);
	UINT32 GetNetwork(NSISSpec_t* pSpec);
	UINT32 GetStaticOptic(NSISSpec_t* pSpec);
	UINT32 GetMeasureRatioSpec(NSISSpec_t* pSpec);
	//YJD START 22.06.13 - SAVE MEASURE IMAGE
	UINT32 GetSaveMeasurePointSpec(NSISSpec_t* pSpec);
	//YJD END 22.06.13 - SAVE MEASURE IMAGE

	UINT32 InitializeSpecWindow();

	UINT32 InitializeBasicSpec();	
	UINT32 InitializeUserSite();
	UINT32 InitializeMachineType();
	UINT32 InitializePlatformType();
	UINT32 InitializeProductType();
	UINT32 InitializeCamType();
	UINT32 InitializeMasterLevel();
	UINT32 InitializeStepRepeatMode();
	UINT32 InitializeReferenceMode();
	UINT32 InitializeDummyMode();
	UINT32 InitializeKeepLocalCopy();
	UINT32 InitializeScanType();
	UINT32 InitializeExceptImageMode();
	UINT32 InitializeLineArc_SubSave();
	UINT32 InitializeMaxLayerNum();
	UINT32 InitializeIPUNum();
	UINT32 InitializeImageType();

	UINT32 InitializeCameraSpec_for_NSIS(bool bIs3D);
	UINT32 InitializeResolution(bool bIs3D, UINT32 idx);
	UINT32 InitializeMagnification(bool bIs3D, UINT32 idx);
	UINT32 InitializeZoom(bool bIs3D, UINT32 idx);
	UINT32 InitializeFovWidth(bool bIs3D, UINT32 idx);
	UINT32 InitializeFovHeight(bool bIs3D, UINT32 idx);
	UINT32 InitializeCameraOrient();
	UINT32 InitializeAlignRatio();
	UINT32 InitializeNetwork();
	UINT32 InitializeStaticOptic();

	UINT32 InitializeCameraSpec(bool bIsLine);
	UINT32 InitializeResolution(bool bIsLine);
	UINT32 InitializeFrameWidth(bool bIsLine);
	UINT32 InitializeFrameHeight(bool bIsLine);
	UINT32 InitializeOverlapX(bool bIsLine);
	UINT32 InitializeOverlapY(bool bIsLine);
	UINT32 InitializeMarginX(bool bIsLine);
	UINT32 InitializeMarginY(bool bIsLine);
	UINT32 InitializeCellColNum(bool bIsLine);
	UINT32 InitializeCellRowNum(bool bIsLine);
	UINT32 InitializeCellOverlapX(bool bIsLine);
	UINT32 InitializeCellOverlapY(bool bIsLine);
	UINT32 InitializeDummyMarginX(bool bIsLine);
	UINT32 InitializeDummyMarginY(bool bIsLine);

	UINT32 InitializePathSpec();
	UINT32 InitializeStepSpec();
	UINT32 InitializeLayerSpec();
	UINT32 InitializeMeasureRatioSpec();
	//YJD START 22.06.13 - SAVE MEASURE IMAGE
	UINT32 InitializeSaveMeasurePointSpec();
	//YJD END 22.06.13 - SAVE MEASURE IMAGE

	// 2022.10.12 KJH ADD
	// NSIS SEM Rect 4 Focus Move Spec Add
	UINT32 InitializeAutoAlignSpec();
	UINT32 SetAutoAlignSpec(AutoAlign_t* pSpec);
	UINT32 GetAutoAlignSpec(AutoAlign_t* pSpec);


	CString GetLayerDefine(UINT32 layerIdx);
	CString GetMeasureDefine(UINT32 index);
	void RemoveBtnFocus(CButton* btn);

	UINT32 _UpdateIU_Pos();

public:
	UINT32 LoadSpecData();

	eUserSite GetUserSite();
	eMachineType GetMachineType();
	ePlatFormType GetPlatformType();
	eProductType GetProductType();
	eCamType GetCamType();
	eMasterLevel GetMasterLevel();
	eShowStepRepeatType GetStepRepeatMode();
	eRefRegionMode GetReferenceRegion();
	bool GetExceptDummy();
	bool GetKeepLocalCopy();
	eScanType GetScanType();
	bool GetExceptImageData();
	bool GetLineArc_SubSave();
	UINT32 GetMaxLayerNum();
	UINT32 GetIPUNum();
	CString GetImageType();

	double GetCamMagnification(bool bIs3D, UINT32 idx);
	double GetCamZoom(bool bIs3D, UINT32 idx);
	double GetCamResolution(bool bIs3D, UINT32 idx);
	UINT32 GetCamFovWidth(bool bIs3D, UINT32 idx);
	UINT32 GetCamFovHeight(bool bIs3D, UINT32 idx);

	void SetCamMagnification(bool bIs3D, UINT32 idx, double dData);
	void SetCamZoom(bool bIs3D, UINT32 idx, double dData);
	void SetCamResolution(bool bIs3D, UINT32 idx, double dData);
	void SetCamFovWidth(bool bIs3D, UINT32 idx, LONG iData);
	void SetCamFovHeight(bool bIs3D, UINT32 idx, LONG iData);

	void SetUserSite(INT32 iData);
	void SetMachineType(INT32 iData);
	void SetPlatformType(INT32 iData);
	void SetProductType(INT32 iData);
	void SetCamType(INT32 iData);
	void SetMasterLevel(INT32 iData);
	void SetStepRepeatMode(INT32 iData);
	void SetReferenceRegion(INT32 iData);
	void SetExceptDummy(INT32 iData);
	void SetKeepLocalCopy(INT32 iData);
	void SetScanType(INT32 iData);
	void SetExceptImageData(INT32 iData);
	void SetLineArcSubSave(INT32 iData);
	void SetMaxLayerNum(LONG iData);
	void SetIPUNum(LONG iData);
	void SetImageType(CString strType);

	double GetCamResolution(bool bIsLine);
	double GetDummyMarginX(bool bIsLine);
	double GetDummyMarginY(bool bIsLine);
	UINT32 GetCamFrameWidth(bool bIsLine);
	UINT32 GetCamFrameHeight(bool bIsLine);
	UINT32 GetCamMinOverlapX(bool bIsLine);
	UINT32 GetCamMinOverlapY(bool bIsLine);
	UINT32 GetCamMinMarginX(bool bIsLine);
	UINT32 GetCamMinMarginY(bool bIsLine);
	UINT32 GetCamCellColNum(bool bIsLine);
	UINT32 GetCamCellRowNum(bool bIsLine);
	UINT32 GetCamCellOverlapX(bool bIsLine);
	UINT32 GetCamCellOverlapY(bool bIsLine);

	void SetCamResolution(bool bIsLine, double dData);
	void SetDummyMarginX(bool bIsLine, double dData);
	void SetDummyMarginY(bool bIsLine, double dData);
	void SetCamFrameWidth(bool bIsLine, LONG iData);
	void SetCamFrameHeight(bool bIsLine, LONG iData);
	void SetCamMinOverlapX(bool bIsLine, LONG iData);
	void SetCamMinOverlapY(bool bIsLine, LONG iData);
	void SetCamMinMarginX(bool bIsLine, LONG iData);
	void SetCamMinMarginY(bool bIsLine, LONG iData);
	void SetCamCellColNum(bool bIsLine, LONG iData);
	void SetCamCellRowNum(bool bIsLine, LONG iData);
	void SetCamCellOverlapX(bool bIsLine, LONG iData);
	void SetCamCellOverlapY(bool bIsLine, LONG iData);

	CString GetCamDataPath();
	CString GetMasterDataPath();
	CString GetLogDataPath();

	void SetCamDataPath(CString strData);
	void SetMasterDataPath(CString strData);
	void SetLogDataPath(CString strData);
	
	CString GetPanelStepName();
	CString GetStripStepName();
	CString GetUnitStepName();

	void SetPanelStepName(CString strData);
	void SetStripStepName(CString strData);
	void SetUnitStepName(CString strData);

	COLORREF GetLayerColor(UINT32 index);
	CString GetFrontLayerName(UINT32 index);
	CString GetBackLayerName(UINT32 index);

	void SetLayerColor(UINT32 index, COLORREF color);
	void SetFrontLayerName(UINT32 index, CString strData);
	void SetBackLayerName(UINT32 index, CString strData);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	static CSettingsFormView* CreateOne(CWnd* pParent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInitialUpdate();
	afx_msg void OnSaveBtnClick();
	afx_msg void OnResetBtnClick();
};


