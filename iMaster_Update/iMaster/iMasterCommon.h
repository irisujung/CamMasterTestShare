#pragma once

#include "../iArchiveIF/iArchiveIF.h"
#include "../iDxDraw/iDxDraw.h"
#include "../iCommonIF/CommonIF-Struct.h"

using namespace IARCHIVEIF;

class CFeatureEdit_AddDlg;
class CAlignMaskEditDlg;
class FeatureEditFormView;

//Initialize 



UINT32			Initialize_SetMainFrm(void* pMainFrm);
UINT32			Initialize_SetMainView(void *pMainView);
UINT32			Initialize_Spec();
UINT32			Initialize_iArchive();

UINT32			Initialize_DxDraw(ViewInfo_t *pViewInfo);
UINT32			Initialize_Minimap(IN const ViewInfo_t &stViewInfo);
UINT32			Initialize_iProcess();

CJobFile*		GetJobPtr();

UINT32			GetFeatureRotate(FeatureSpec_t* pFeatureSpec);

//Destory
UINT32			Destroy_iArchive();
UINT32			Destroy_iDraw();
UINT32			Destroy_iProcess();

//Extract CAM
UINT32			ExtractCamData(CString fileName);
UINT32			ExtractCam(LPVOID Param);
UINT32			EndExtractCam();
UINT32			LoadCamData();
UINT32			SetCamInfo();
UINT32			SetLayerInfo();
UINT32			LoadLayer(LPVOID Param);
UINT32			SetStepList();
UINT32			SetLayerList();
UINT32			SetComponentList();

//Load Layer
UINT32			LoadLayerData();
void			IsDoneLoadLayers(BOOL b);
BOOL			GetFlag_LoadLayers();
BOOL			GetFlag_IsAllLayerLoaded();



//Save Layer

UINT32			SaveCamData();
UINT32			SaveAsCamData(IN CString &strPathName);

CString			GetWorkLayer();

//Display
void			InitCursorPt();
void			ClearDisplay();
UINT32			DrawCamData(IN const BOOL &bScaleFitView = TRUE, IN const BOOL &bResetViewer = FALSE);
UINT32			DrawZoomInit();
//UINT32			DrawZoomInRect(RECT* rcZoomRect);
//UINT32			MoveZoomInRect(POINT* ptMove);
//UINT32			DrawZoomInOut(eZoomType zoomType);

UINT32			DrawLButtonDown(UINT nFlags, CPoint point);
UINT32			DrawLButtonUp(UINT nFlags, CPoint point);
UINT32			DrawMouseMove(UINT nFlags, CPoint point);
UINT32			DrawMouseWheel(UINT nFlags, short zDelta, CPoint pt);
UINT32			DrawRButtonDblClk(UINT nFlags, CPoint point);
UINT32			DrawRButtonDown(UINT nFlags, CPoint point);
UINT32			DrawRButtonUp(UINT nFlags, CPoint point);

UINT32			DrawLBtuttonDblClk(UINT nFlags, CPoint point);

UINT32			DrawZoomIn();
UINT32			DrawZoomOut();
UINT32			DrawZoomRect(IN const RECTD &drtRect);

UINT32			SetViewInfo(IN ViewInfo_t* pViewInfo);
UINT32			SetMinimapSizeChange(IN ViewInfo_t* pViewInfo);

void			SetViewerEditMode(IN const UINT32 &bEditMode);
UINT32			GetViewerEditMode();

//Feature Edit
UINT32			SetResize_Apply(IN const float &fResizePercent);
UINT32			SetResize_Apply_SelectOnly(IN const double &dResizePercent);
UINT32			SetDelete_Apply();
UINT32			SetMove_Apply();
UINT32			SetAdd_Apply(IN const CString &strLayer, vector<CFeature*> &vecFeature);

UINT32			Feature_Move_KeyBoard(PointDXY &point, enum eKeyDir);
UINT32			GetMoveValue(PointDXY&point);


//Preview Temp Data Apply
UINT32			SetTempData_Apply();

void			SetEdit_Feature();

void			SetEdit_Apply();

UINT32			Set_Preview(IN const PointDXY &point, double &fResizePercent, IN const int&nIndex, IN const BOOL&bMirror, IN const CPointD &point2, 
	IN const CPointD &dptPitch, IN const CPoint &ptCount, IN  BOOL &bRepeat, IN  BOOL &bCopy, IN const double&dRadius, IN const double&dWidth, IN const double&dHeight, IN const CString &strShape, IN const double dLineWidth);

//MoveAxisMode
UINT32			SetMoveAxis(IN const UINT32 &nEditAxis);

//Edit/Resize
UINT32			SetResize(IN const int &nResizePercent);

//Add
UINT32			ClearTempPoint();
UINT32			SetPointMode(IN const BOOL &bMode = TRUE, int nIndex = 0, IN const CString &strName = _T(""));
UINT32			SetAdd(IN const CString &strLayer, vector<CFeature*> &vecFeature);

UINT32			SaveLayer_Test(IN const double &dResolution, IN const CString &strPathName);

//Feature Select
UINT32			ResetSelect();
UINT32			SetSelect(vector<FEATURE_INFO> &vecFeatureInfo);
UINT32			SetUnSelect(vector<FEATURE_INFO> &vecFeatureInfo);

UINT32			ResetEdit();

UINT32			UpdateSelectInfo();

//Align / Mask
UINT32			SetMaskSelectMode(IN const MaskType &eSelectMode);
MaskType		GetMaskSelectMode();
UINT32			SetAdd_Apply_Mask(IN const MaskType eMaskType, IN const CString &strLayer, vector<CFeature*> &vecFeature);
UINT32			SetDelete_Apply_Mask(IN const MaskType eMaskType);
UINT32			SetResize_Manual_Mask(IN const MaskType eMaskType, IN const float& fPercentX, IN const float& fPercentY);
UINT32			SetEditApply_Mask(IN const MaskType eMaskType);


UINT32			SetRotate(IN const int&nIndex, IN const BOOL&bMirror);
UINT32			SetRotate_Apply(IN const int&nIndex, IN const BOOL&bMirror);


UINT32			SetCopy(IN const CPointD &point);
UINT32			SetCopy_Repeat(IN const CPointD &point, IN const CPoint &ptCount);
UINT32			SetCopy_Apply(IN const CPointD &point);
UINT32			SetCopy_Repeat_Apply(IN const CPointD &dptPitch, IN const CPoint &ptCount);

UINT32			SetEdit_Shape(IN const double&dRadius, IN const double &dWidth, IN const double &dHeight , IN const CString &strShape);

RECTD			GetViewRect();//현재 화면에 보이는 View의 실제 좌표

UINT32			SetAttribute_Apply(IN const CString&strAttrName, IN const CString&strAttrString);

UINT32			Set_CW();

UINT32			Reset();

//Feature Show/Hide
UINT32			ResetHide();
UINT32			SetShow(vector<FEATURE_INFO> &vecFeatureInfo);
UINT32			SetHide(vector<FEATURE_INFO> &vecFeatureInfo);

//
UserLayerSet_t*	GetUserLayerSet();

//Make Master
UINT32			MakeMasterData();
UINT32			MakeMaster(LPVOID Param);

//Spec
SystemSpec_t*	GetSystemSpec();
UINT32			SaveSystemSpec();

FeatureSpec_t*   GetFeatureSpec();

eUserSite		GetUserSite();
eMachineType    GetMachineType();
CString			GetCamPath();
eScanType		GetScanType();
void			SetScanType(eScanType scanType);
eProductType	GetProductType();
void			SetProductType(eProductType prdType);
CString			GetModelName();
UINT32			GetMaxLayerNum();
CString			GetImageType();



//Log
CString			GetCurAppPath();
void			AddApplicationLog(CString strLog, LogMsgType enMsgType = LogMsgType::EN_INFORMATION);

//Status Bar
//UINT32			GetCurViewScl(OUT ViewScale_t *pViewScl);
UINT32			UpdateStatusBar(IN CPoint point);
UINT32			UpdateStatusBar_FeatureSelect();

INT32			GetCurStepIndex();

MeasurePanel_t* GetMeasureData();
void			Update_NSIS_MeasureData();
UINT32			ChangeLayerInfo(CString strLayer);
UserSetInfo_t*  GetUserSetInfo();

UINT32			ManualMove(IN const PointDXY &point);
UINT32			Draw_ManualMove(UINT nFlags, PointDXY &point);

UINT32			Log_FeatureSelect();

UINT32			SetAutoAlignCross();
UINT32			ClearEditTemp();
UINT32			ClearLayers();

PanelInfo_t*	GetProductInfo();
UINT32			SetDrawOnly_MeasureUnit(BOOL bMeaureUnit);

//Get DlgPtr
CFeatureEdit_AddDlg *GetFeature_AddDlg();
CAlignMaskEditDlg *GetAlignMaskEditDlg();

BOOL			IsZoomRectMode();
BOOL			IsMeasureMode();

BOOL			IsSelect();
BOOL			IsMove();
BOOL			IsResize();
BOOL			IsEditBox();
//Minimap V2
UINT32			MinimapMouseMove(UINT nFlags, CPoint point);
UINT32			MinimapMouseLButtonDown(UINT nFlags, CPoint point);
UINT32			MinimapMouseLButtonUp(UINT nFlags, CPoint point);
UINT32			MinimapMouseWheel(UINT nFlags, short zDelta, CPoint pt);
UINT32			MinimapResetZoom();
UINT32			Minimap_SetInspect();
UINT32			Minimap_SetCopy(BOOL &bCopy);
UINT32			Minimap_SetPaste(BOOL &bCopy);
UINT32			Minimap_SetDelete();
UINT32			Minimap_GetSelectRect(OUT RECTD &drtSelect);
UINT32			Minimap_SetMode(UINT32 nMode);


UINT32			SetFeatureData();


// 2022.08.08 KJH ADD
void SetLayerInfoView();
UINT32 SetAutoFindAlign();
AutoAlign_t* GetAutoAlignSpec();
// KJH END

// 2022.09.19 KJH ADD
UINT32 SetStartLayer(CString strTemp);
UINT32 SetEndLayer(CString strTemp);
UINT32 SetViewerLocation(CString strTemp);
UINT32 AutoAlignRect(CString strTemp, RECTD &drtRect);

// KJH END
