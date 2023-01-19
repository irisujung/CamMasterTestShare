#pragma once


 typedef struct _Surfacepoint
{
	 double dAddX;
	 double dAddY;
	 double dCenterX;
	 double dCenterY;
	 bool bOsOc;
	 bool bCCW;
	
	

	_Surfacepoint()
	{
		_Reset();
	}

	void _Reset()
	{
		bCCW = true;
		bOsOc = true;
		dAddX = 0.0;
		dAddY = 0.0;
		dCenterX = 0.0;
		dCenterY = 0.0;
		
	}

 }Surfacepoint;

typedef struct _FeatureAdd_PadParam
{

	double dCenterX;
	double dCenterY;

	double dStartX;
	double dStartY;

	double dEndX;
	double dEndY;

	double dAddX;
	double dAddY;


	_FeatureAdd_PadParam()
	{
		Reset();
	}

	void Reset()
	{
		dCenterX = 0.0;
		dCenterY = 0.0;
		
		dStartX = 0.0;
		dStartY = 0.0;
		dEndX = 0.0;
		dEndY = 0.0;
		
		dAddX = 0.0;
		dAddY = 0.0;
	}

}FeatureAddPadParam;



#include "DxDraw/CDirect_Base.h"
#include "DxDraw/Def_Viewer.h"

class CDirectView;
class CDrawProfile;
class CDrawEdit_Temp;
class CDrawFov;

class CFeatureData;
class CFeatureFilterFunction;


class __declspec(dllexport) CDrawManager : public CDirect_Base
{
public:
	CDrawManager();
	~CDrawManager();

	FeatureSpec_t FeatureSpec;
	
	double m_dResizeRatio = 1.0;
	double m_dResize_Ratio = 100.0;
	float m_dCount = 0.1f;

	UINT32 SetAttribute(IN const CString &AttributeName, IN const CString &AttributeString);
	UINT32 _SetAttribute(IN const CString &strAttrName, IN const CString &strAttrString);
	UINT32 _SetAttribute_Apply(vector<CFeatureData*>&vecFeatureData, CStep *pStep ,IN const CString&strAttName, IN const CString &strAttString);


	UINT32 SetEdit_Shape(IN const double &dRadius, IN const double &dWidth, IN const double &dHeight, IN const CString &strShape);

	UINT32 _SetShape_Pad(vector<CFeatureData*>& vecFeatureData, CStep *pStep);

	UINT32 _SetShape(IN const double &dRadius, IN const double &dWidth, IN const double &dHeight, IN const CString &strShape);

	UINT32 Feature_Move_KeyBoard(PointDXY &point, enum eKeyDir);

	UINT32 SetSelectPoint_Center(IN const int &nIndex, IN const float fX, IN const float fY);
	UINT32 SetSelectPoint_Add(IN const int nIndex, IN const float fX, IN const float fY);

	UINT32 SetSelect_StartPoint(IN const float fXs, IN const float fYs);
	UINT32 SetSelect_EndPoint(IN const float fXe, IN const float fYe);
	UINT32 SetSelectPoint(IN const float fX, IN const float fY);

	UINT32 SetSurface(IN const CString LaerName);
	UINT32 _SetTp(IN const float fX, IN const float fY);
	UINT32 SetTP(IN const CString Layername);
	UINT32 SetTp(IN const CString Layername);
	UINT32 SetArc(IN const CString Layername);
	UINT32 SetLine(IN const CString Layername, IN const int &nLineMode);

	UINT32 SetCircle(IN const CString Layername, IN const double &dDiameter = -999.);
	UINT32 SetSquare(IN const CString Layername, IN const double &dWidth = -999.);
	UINT32 SetRect(IN const CString Layername, IN const double &dWidth = -999., IN const double &dHeight = -999.);
	UINT32 SetRectxr();
	UINT32 SetRectxc();
	
	UINT32 SetOval();
	UINT32 SetDi();
	UINT32 SetOct();
	UINT32 SetDonut_r();
	UINT32 SetDonut_s();

	UINT32 SetHex_l();
	UINT32 SetHex_s();
	UINT32 SetBfr();
	UINT32 SetBfs();
	UINT32 SetTri();

	UINT32 SetOval_h();
	UINT32 SetThr();
	UINT32 SetThs();
	UINT32 SetS_Ths();
	UINT32 SetS_Tho();

	UINT32 SetSr_Ths();
	UINT32 SetRc_Ths();
	UINT32 SetRc_Tho();
	UINT32 SetEl();
	UINT32 SetMorie(IN const CString Layername);

	UINT32 SetHole();

	UINT32 SetFeatureData();

	BOOL Hit_Test();
	
	BOOL IsMove();
	BOOL IsResize();
	BOOL IsEditBox();

	UINT32 Reset();

	UINT32 EditApply();

	BOOL m_bResizeMode = FALSE;
	BOOL m_bWheelMode = FALSE;
	BOOL m_bSelect = FALSE;
	BOOL m_bPreview = FALSE;
	
	D2D1_POINT_2F m_KeyMove = D2D1::Point2F(0.f, 0.f);
	PointDXY m_TotalMoveXY = CPointD(0, 0);
	PointDXY m_KeyMoveXY = CPointD(0, 0);
	PointDXY m_MouseMoveXY = CPointD(0, 0);

	BOOL m_bKeyMode = FALSE;


	UINT32 SetPreview(IN const PointDXY &point, double &fResizePercent, IN const int&nIndex, IN const BOOL&bMirror, IN const CPointD &point2, 
		IN const CPointD &dptPitch, IN const CPoint &ptCount, IN  BOOL &bRepeat, IN BOOL &bCopy, IN const double &dRadius, IN const double  &dWidth, IN const double &dHeight, IN const CString &strShaoe, IN const double &dLineWidth);

	UINT32 SetSystemSpec(SystemSpec_t* pSystemSpec);
	UINT32 SetUserSetInfo(UserSetInfo_t *pUserSetInfo, UserLayerSet_t *pUserLayerSetInfo);
	UINT32 SetDrawOnly_MeasureUnit(IN const BOOL &bMeaureUnit);

	UINT32 Initialize(IN CJobFile* pCAMData, IN ViewInfo_t* pViewInfo, IN PanelInfo_t* pstPanelInfo, IN FovInfo_t* pFovInfo, IN CellInfo_t* pCellInfo);
	UINT32 SetViewInfo(IN ViewInfo_t* pViewInfo);

	UINT32 ResetView();

	ID2D1Factory1 * GetFactory() { return m_pFactory; }
	ID2D1DeviceContext* GetDeviceContext() { return m_pD2DContext; };

	UINT32 SetStepIndex(int nStepIndex);

	UINT32 ZoomIn();
	UINT32 ZoomOut();
	UINT32 ResetZoom();
	UINT32 DrawZoomRect(IN const RECTD &drtRect);

	UINT32 OnLButtonDown(UINT nFlags, CPoint point);
	UINT32 OnLButtonUp(UINT nFlags, CPoint point);
	UINT32 Draw_ManualMove(UINT nFlags, PointDXY &point);
	UINT32 OnMouseMove(UINT nFlags, CPoint point);
	UINT32 OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	UINT32 OnRButtonDblClk(UINT nFlags, CPoint point);

	UINT32 OnRButtonDown(UINT nFlags, CPoint point);
	UINT32 OnRButtonUp(UINT nFlags, CPoint point);

	UINT32 ConvertPos(CPoint &ptSrc, double &dDstX, double &dDstY);
	UINT32 GetPanPos_ForDebug(OUT float &fPanX, OUT float &fPanY);

	UINT32 ClearDisplay();

	//EditMode
	UINT32 SetEditMode(IN const UINT32 &nEditMode);
	UINT32 GetEditMode();

	//Save
	UINT32 SaveLayer_Test(IN const double &dResolution, IN const CString &strPathName);
	UINT32 SaveUnitLayer(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dAngle, IN const bool bMirror, IN const double &dResolution, IN const CString &strPathName);
	UINT32 SaveStripLayer(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dAngle, IN const bool bMirror, IN const double &dResolution, IN const CString &strPathName);
	UINT32 SaveUnitAlignPoint(IN const INT32 iStepIdx, IN const UINT32 iUnitIndex, IN const UINT32 iIndex, IN const double dAngle, IN const bool bMirror, IN const double dCamAngle,
		IN const double &dResolution, IN const RECTD &rcRect, IN const CString &strPathName);
	//YJD START 22.06.13 - SAVE MEASURE IMAGE
	UINT32 SaveMeasurePoint(IN const INT32 iStepIdx, IN const UINT32 iUnitIndex, IN const double dAngle, IN const bool bMirror, IN const double dCamAngle, IN const vector<double> &vecResolution, IN const vector<RECTD> &vecRect, IN const CString &strPathName);
	//YJD END 22.06.13 - SAVE MEASURE IMAGE
	UINT32 SavePanelLayer(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle, IN const double &dResolution, IN const CString &strPathName);
	UINT32 SavePanelLayer_FeatureSub(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle, IN const double &dResolution, IN const CString &strPathName, int eLineOption, vector <LINEWIDTH> &vecLineWidth);
	UINT32 SavePanelLayer_FeatureSub_V3(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double &dResolution, IN const CString &strPathName, vector<vector <LINEWIDTH>> &vecvecLineWidth);
	UINT32 SavePanelAlignPoint(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle, IN const double &dResolution, IN const RECTD &rcRect, IN const CString &strPathName);
	UINT32 SaveUnitAlignPoint_In_Panel(IN const INT32 iStepIdx, IN const Orient &eOrient, IN const double &dResolution, IN const CString &strPathName);
	UINT32 SaveProfile(IN const BOOL m_bIs4Step, IN const INT32 iStepIdx, IN const Orient &eOrient, IN const CString &strPathName);

	UINT32 SaveUnitLayer_in_Panel(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const Orient &eOrient, IN const double &dResolution, IN const CString &strPathName);

	//Feature Select
	UINT32			ResetSelect();
	UINT32			SetSelect(vector<FEATURE_INFO> &vecFeatureInfo);
	UINT32			SetUnSelect(vector<FEATURE_INFO> &vecFeatureInfo);

	UINT32			UpdateSelectInfo();//선택된 Feature의 Status bar에 정보 출력을 위한 정보 취합

	//Feature Show/Hide
	UINT32			ResetHide();
	UINT32			SetShow(vector<FEATURE_INFO> &vecFeatureInfo);
	UINT32			SetHide(vector<FEATURE_INFO> &vecFeatureInfo);

	//Edit
	//Delete
	UINT32			SetDelete_Apply();//현재 선택된 Feature Delete
	//Move
	UINT32			SetMove_Apply();//Move Dialog 에서 Accept가 눌러졌을 때 들어 오는 함수
	UINT32			ManualMove(IN const PointDXY &point);
	//

	//Add
	UINT32			ClearTempPoint();
	UINT32			SetPointMode(IN const BOOL &bMode = TRUE, IN const int &nIndex = 0, IN const CString &strName = _T(""));
	BOOL			GetPointMode();
	UINT32			GetTempPoint(IN const int &nIndex, float &fX, float &fY);
	UINT32			GetTempPoint_Center(IN const int &nIndex, float &fX, float &fY);

	UINT32			SetAdd(IN const CString &strLayer, vector<CFeature*> &vecFeature);
	UINT32			SetAdd_Apply(IN const CString &strLayer, vector<CFeature*> &vecFeature);

	UINT32			SetCW();
	//
	UINT32			SetResize(IN const double &dResizePercent);
	UINT32			SetResize_Apply(IN const float &fResizePercent);
	UINT32			SetResize_Apply_SelectOnly(IN const double &dResizePercent);

	UINT32			SetCopy(IN const CPointD &dptPoint);
	UINT32			SetCopy_Repeat(IN const CPointD &dptPitch, IN const CPoint &ptCount);
	
	UINT32			SetCopy_Apply(IN const CPointD &dptPoint);
	UINT32			SetCopy_Repeat_Apply(IN const CPointD &dptPitch, IN const CPoint &ptCount);

	UINT32			SetRotate(IN const int&nIndex, IN const BOOL&bMirror);
	UINT32			SetRotate_Apply(IN const int&nIndex, IN const BOOL&bMirror);

	// Preview TempData Apply
	UINT32			SetTempData_Apply();

	UINT32			SetPictureBox(IN const D2D1_RECT_F & frtPictureBox);

	CString			GetSelectInfoString();
	SELECT_INFO*	GetSelectInfo();
	SELECT_INFO*	SetSelectInfo(D2D_POINT_2F fptPoint_mm, D2D_POINT_2F fptPoint_Pixel);

	D2D1::Matrix3x2F	GetViewScale();
	
	UINT32			SetEditMoveAxis(IN const int &nMoveAxis);

	//Mask Edit
	UINT32			SetMaskSelectMode(IN const MaskType &eSelectMode);
	MaskType		GetMaskSelectMode();
	UINT32			SetAdd_Mask(IN const MaskType eMaskType, IN const CString &strLayer, vector<CFeature*> vecFeature);
	UINT32			SetAdd_Apply_Mask(IN const MaskType eMaskType, IN const CString &strLayer, vector<CFeature*> vecFeature);
	UINT32			SetResize_Mask(IN const MaskType eMaskType, IN const float &fPercentWidth, IN const float &fPercentHeight);
	UINT32			SetResize_Apply_Mask(IN const MaskType eMaskType, IN const float &fPercentWidth, IN const float &fPercentHeight);
	UINT32			SetMove_Apply_Mask(IN const MaskType eMaskType);
	
	UINT32			SetDelete_Apply_Mask(IN const MaskType eMaskType);	
	UINT32			SetApply_Mask(IN const MaskType eMaskType);//EditTemp에 있는 내용을 Feature에 업데이트 한다.
	UINT32			SetResize_Manual_Mask(IN const MaskType eMaskType, IN const float &fPercentWidth, IN const float &fPercentHeight);//


	//Feaure compare
	BOOL			CompareFeature(IN CLayer *pLayer, IN const int &nFeatureIndex_1, IN const int &nFeatureIndex_2);


	BOOL			IsZoomRectMode() { return m_bRectZoom; }
	BOOL			IsMeasureMode() { return (m_bMeasureMode || m_bMeasureMode_Setting); }

	UINT32			Set_CW(IN BOOL CW);

	RECTD			GetViewRect();

	BOOL			m_bMoveMode = FALSE;

protected:
	virtual void OnRender();
	FeatureAddPadParam m_stParam;


private:
	void		_SetZoomRect(IN const CRect& rtRect);
	void		_SetZoomRect(IN const D2D1_RECT_F& frtRect_mm);
	void		SetViewScale();


	UINT32		_SetFeature_Move(vector<CFeatureData*>& vecFeatureData, CStep *pStep);
	UINT32		_SetFeature_Resize_LineArc(vector<CFeatureData*>& vecFeatureData, CStep *pStep);
	UINT32		_SetFeature_Resize_LineArc_SelectOnly(vector<CFeatureData*>& vecFeatureData, CStep *pStep);
	UINT32		_SetFeature_Resize_PadText(vector<CFeatureData*>& vecFeatureData, CStep *pStep);
	UINT32		_SetFeature_Resize_PadText_SelectOnly(vector<CFeatureData*>& vecFeatureData, CStep *pStep);
	UINT32		_SetFeature_Resize_Surface(vector<CFeatureData*>& vecFeatureData, CStep *pStep);

	UINT32		_SetFeature_Copy(vector<CFeatureData*>& vecFeatureData, CStep *pStep);


	UINT32		_SetFeature_Add(vector<CFeatureData*>& vecFeatureData, CStep *pStep);

	UINT32		_SetFeature_Rotate(vector<CFeatureData*>&vecFeatureData,CStep *pStep);

	// Preview Temp Data Set Feature
	UINT32		_SetFeature_TempData(vector<CFeatureData*>&vecFeatureData, CStep *pStep);

	UINT32		_SetFeature_Add_Mask(IN const MaskType &eMaskType, vector<CFeatureData*>& vecFeatureData, CStep *pStep);
	UINT32		_SetFeature_Resize_Mask(IN const MaskType &eMaskType, vector<CFeatureData*>& vecFeatureData, CStep *pStep);
	UINT32		_SetFeature_Move_Mask(IN const MaskType &eMaskType, vector<CFeatureData*>& vecFeatureData, CStep *pStep);

	//Edit Temp에서의 변경점을 Feature에 적용
	UINT32		_SetFeature_Edit_Mask(IN const MaskType &eMaskType, vector<CFeatureData*>& vecFeatureData, CStep *pStep);
	UINT32		GetFeatureData(vector<CFeatureData*>& vecFeatureData, CStep *pStep);

private:

	vector<Surfacepoint>				m_vecPoint;
	//int							m_nPointIndex = 0;
	
	CDirectView		*m_pDirectView = nullptr;
	CDrawProfile	*m_pDrawProfile = nullptr;

	CDrawEdit_Temp	*m_pDrawEdit_Temp = nullptr;
	CDrawFov		*m_pDrawFov = nullptr;

	CFeatureData	*m_pFeatureData = nullptr;;
	// 2022.06.22
	// 김준호 주석추가
	// Preview after Apply 시 원본 삭제 막기 위해 bool 변수 추가.
	BOOL bCheckRepeat = FALSE;

	CFeatureFilterFunction	*m_pFeatureFilter = nullptr;

	int				m_nSelectStep = -1;

	//PictureBox
	D2D1_RECT_F		m_frtPictureBox;

	//Mouse Control
	BOOL			m_bPanMode = FALSE;
	CPoint			m_ptPrevMove = CPoint(0, 0);

	BOOL			m_bRectZoom = FALSE;
	CRect			m_rtRectZoom = CRect(0, 0, 0, 0);//Alt+그리기

	//Point SetMode
	BOOL			m_bPointSetMode = FALSE;
	int				m_nPointIndex = -1;
	CString			m_strPointSet_Name = _T("");

	//EditMode
	UINT32			m_nEditMode = EditMode::enumMode_None;

	//Mask Selelect
	MaskType		m_eMaskSelectMode = MaskType::enumType_None;
	
	BOOL			m_bMeasureMode = FALSE;
	BOOL			m_bMeasureMode_Setting = FALSE;//메져 측정 진행 중
	D2D1_RECT_F		m_frtMeasure;
	//
	
	D2D1::Matrix3x2F m_matScaleView;//scale 정보
	D2D_POINT_2F	m_fptPan = { 0.f,0.f };//Panning 정보

	//외부 포인터
	//지우지 말것
	CJobFile		*m_pJobData = nullptr;

	

};

