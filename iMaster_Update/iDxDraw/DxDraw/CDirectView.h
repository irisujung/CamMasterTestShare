#pragma once

#include "CDirect_Base.h"

#include <vector>
using namespace std;

#include <math.h>

#include "../../iODB/iODB.h"
#include "Def_Viewer.h"
#include "../SaveLayerInit.h"

#define ToRadian( degree )  ( (degree) * (PI / 180.0f) )
#define ToDegree( radian )  ( (radian) * (180.0f / PI) )



class CBackBuffer_Unit;
class CSaveLayer;
class CDrawProfile;
class CDrawLayer;
class CDrawFunction;
class CDrawManager;

class __declspec(dllexport) CDirectView
{
	//DECLARE_DYNAMIC(CDirectView)

	typedef struct _TempSelectSurface
	{
		CString strLayer;
		int nSurfaceIndex;
		int nObOeIndex;
		float fArea;

		_TempSelectSurface()
		{
			strLayer = _T("");
			nSurfaceIndex = -1;
			nObOeIndex = -1;
			fArea = FLT_MAX;
		}

	}TempSelectSurface;

	typedef struct _SaveRect
	{
		CString strFileName;
		D2D1_RECT_F frtRect;
	}SaveRect;

public:
	CDirectView(void);
	~CDirectView(void);
	
	void	SetLink(IN CDrawManager* pDrawManager);
	UINT32 SetSystemSpec(SystemSpec_t* pSystemSpec);
	UINT32 SetUserSetInfo(UserSetInfo_t *pUserSetInfo, UserLayerSet_t *pUserLayerSetInfo);
	UINT32 SetDrawOnly_MeasureUnit(IN const BOOL &bMeaureUnit);

	UINT32 Initialize(IN ID2D1Factory1 *m_pFactory, IN	ID2D1DeviceContext *m_pD2DContext, vector<ID2D1BitmapRenderTarget*> &vecRender,
		vector<ID2D1BitmapRenderTarget*> &vecRender_SubStep,
		IN CJobFile* pCAMData, IN ViewInfo_t* pViewInfo, IN PanelInfo_t* pstPanelInfo, IN FovInfo_t* pFovInfo, IN CellInfo_t* pCellInfo);

	UINT32 SetRenderPtr(vector<ID2D1BitmapRenderTarget*> &vecRender,
		vector<ID2D1BitmapRenderTarget*> &vecRender_SubStep);

	UINT32 ResetView();
	
	UINT32 SetStepIndex(int nStepIndex);
// 	UINT32 ZoomIn();
// 	UINT32 ZoomOut();
// 	UINT32 ResetZoom();

	UINT32 OnLButtonDown(UINT nFlags, D2D1_POINT_2F fptPoint);
	UINT32 OnLButtonUp(UINT nFlags, CPoint point);
	UINT32 OnMouseMove(UINT nFlags, CPoint point);
	UINT32 OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	UINT32 OnRButtonDblClk(UINT nFlags, CPoint point);

	UINT32 OnRButtonDown(UINT nFlags, CPoint point);
	UINT32 OnRButtonUp(UINT nFlags, CPoint point);
	
	//Save
	UINT32 SaveLayer_Test(IN const double &dResolution, IN const CString &strPathName);
	UINT32 SaveUnitLayer(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dAngle, IN const bool bMirror, IN const double &dResolution, IN const CString &strPathName);
	UINT32 SaveStripLayer(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dAngle, IN const bool bMirror, IN const double &dResolution, IN const CString &strPathName);
	
	UINT32 SaveUnitLayer_in_Panel(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const Orient &eOrient, IN const double &dResolution, IN const CString &strPathName);

	UINT32 SaveUnitAlignPoint(IN const INT32 iStepIdx, IN const UINT32 iUnitIndex, IN const UINT32 iIndex, IN const double dAngle, IN const bool bMirror, IN const double dCamAngle, 
		IN const double &dResolution, IN const RECTD &rcRect, IN const CString &strPathName);
	//YJD START 22.06.13 - SAVE MEASURE IMAGE
	UINT32 SaveMeasurePoint(IN const INT32 iStepIdx, IN const UINT32 iUnitIndex, IN const double dAngle, IN const bool bMirror, IN const double dCamAngle, IN const vector<double> &vecResolution, IN const vector<RECTD> &vecRect, IN const CString &strPathName);
	//YJD END 22.06.13 - SAVE MEASURE IMAGE
	UINT32 SavePanelLayer(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle, IN const double &dResolution, IN const CString &strPathName);
	UINT32 SavePanelAlignPoint(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle, IN const double &dResolution,
		IN const RECTD &rcRect, IN const CString &strMeasureLayer, IN const CString &strPathName);

	UINT32 SavePanelLayer_FeatureSub(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle, IN const double &dResolution, IN const CString &strPathName, int eLineOption, vector <LINEWIDTH> &vecLineWidth);
	UINT32 SavePanelLayer_FeatureSub_V3(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double &dResolution, IN const CString &strPathName, vector<vector <LINEWIDTH>> &vecvecLineWidth);

	UINT32 SaveUnitAlignPoint_In_Panel(IN const INT32 iStepIdx, IN const Orient &eOrient, IN const double &dResolution,
		IN const CString &strMeasureLayer, IN const CString &strPathName);

	UINT32 SaveProfile(IN const BOOL m_bIs4Step, IN const INT32 iStepIdx, IN const Orient &eOrient, IN const CString &strPathName);

	//Feature Select
	UINT32			ResetSelect();
	UINT32			SetSelect(vector<FEATURE_INFO> &vecFeatureInfo);
	UINT32			SetUnSelect(vector<FEATURE_INFO> &vecFeatureInfo);

	

	//
	UINT32			UpdateSelectInfo();//선택된 Feature의 Status bar에 정보 출력을 위한 정보 취합

	UINT32			ClearPreSelect();

	//Feature Show/Hide
	UINT32			ResetHide();
	UINT32			SetShow(vector<FEATURE_INFO> &vecFeatureInfo);
	UINT32			SetHide(vector<FEATURE_INFO> &vecFeatureInfo);

	//Edit
	//Delte
	UINT32			SetDelete();//현재 선택된 Feature Delete

	UINT32			SetPictureBox(IN const D2D1_RECT_F & frtPictureBox);

	CString			GetSelectInfoString();
	SELECT_INFO*	GetSelectInfo() { return &m_stSelectInfo; };
	SELECT_INFO*	SetSelectInfo(D2D_POINT_2F fptPoint_mm, D2D_POINT_2F fptPoint_Pixel);

	UINT32			SetRedrawGeo();
	
	void			OnRender();

	UINT32 GetFilteredFeature(vector < vector<CFeature*>> &vecvecFeature, CStep *pStep, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, int &eLineOption, vector<LINEWIDTH> &vecLineWidth, RECTD &drtView_mm);//Line Width에 맞는 Feature를 복사해온다.SubStep 포함

	UINT32 GetFilteredFeature_V3(vector < vector < vector<CFeature*>>> &vecvecvecFeature, CStep *pStep, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, vector < vector<LINEWIDTH>> &vecvecLineWidth, RECTD &drtView_mm);//Line Width에 맞는 Feature를 복사해온다.SubStep 포함
	
private:
	   
	void _OnInit();

	void _ClearDrawLayer();
	void _InitDrawLayer();

	BOOL _SetHighlight_Geo(IN D2D_POINT_2F fptPoint, IN const BOOL &bPreHighLight = FALSE);
	BOOL _SetHighlight_MaskGeo(IN const MaskType &eMaskType, IN D2D_POINT_2F fptPoint, IN const BOOL &bPreHighLight = FALSE);

	

// 	BOOL _CheckHighlight_LineFeature(IN D2D_POINT_2F fptPoint, IN CFeatureL* pFeatureL, IN CSymbol *pSymbol);
// 	BOOL _CheckHighlight_PadFeature(IN D2D_POINT_2F fptPoint, IN CFeatureP* pFeatureP, IN CSymbol *pSymbol);
// 	BOOL _CheckHighlight_ArcFeature(IN D2D_POINT_2F fptPoint, IN CFeatureA* pFeatureA, IN CSymbol *pSymbol);
// 	BOOL _CheckHighlight_TextFeature(IN D2D_POINT_2F fptPoint, IN CFeatureT* pFeatureT, IN CSymbol *pSymbol);
// 	BOOL _CheckHighlight_SurfaceFeature(IN D2D_POINT_2F fptPoint, IN CFeatureS* pFeatureS, IN CSymbol *pSymbol, OUT int &nSurfaceIndex);

	void _setHighlight_TpFeature(IN CFeatureZ* pFeatureZ, IN const BOOL &bPreHighLight = FALSE);
	void _SetHighlight_LineFeature(IN CFeatureL* pFeatureL, IN const BOOL &bPreHighLight = FALSE);
	void _SetHighlight_PadFeature(IN CFeatureP* pFeatureP, IN const BOOL &bPreHighLight = FALSE);
	void _SetHighlight_ArcFeature(IN CFeatureA* pFeatureA, IN const BOOL &bPreHighLight = FALSE);
	void _SetHighlight_TextFeature(IN CFeatureT* pFeatureT, IN const BOOL &bPreHighLight = FALSE);
	void _SetHighlight_SurfaceFeature(IN CFeatureS* pFeatureS, IN const int &nSurfaceIndex, IN const BOOL &bPreHighLight = FALSE);
	
	UINT32 _ClearGeo();

	//for NSIS
	UINT32 _SetMeasureUnitInfo(vector<MeasureUnit>& vecMeasureUnitInfo);
	CLayer *_GetTargetLayer(int nStepIndex);

	UINT32 SavePanelLayer_NSIS(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle,
		IN const double &dResolution, IN const CString &strPathName);
	
	UINT32 SavePanelLayer_AOI(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle,
		IN const double &dResolution, IN const CString &strPathName);

	UINT32 _SaveSplitImage(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle,
		IN const double &dResolution, IN const CString &strPathName);

	UINT32 _SaveSplitImage_V2(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle,
		IN const double &dResolution, IN const CString &strPathName);
	
	

	UINT32 _SaveSplitImage_LineArcSub_V2(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle,
		IN const double &dResolution, IN const CString &strPathName, int eLineOption, vector <LINEWIDTH> &vecLineWidth);

	UINT32 _SaveSplitImage_LineArcSub_V3(IN const INT32 iStepIdx, IN const UINT32 iIndex,
		IN const double &dResolution, IN const CString &strPathName, vector<vector <LINEWIDTH>> &vecvecLineWidth);

	UINT32 _MergeSplitImage(IN const INT32 iStepIdx, IN const double &dResolution, IN const CString &strPathName);

	//미사용
	UINT32 _ConvRegion(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle,
		IN const double &dResolution, IN const CString &strPathName);

	UINT32 _SaveRegion(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle,
		IN const double &dResolution, IN const CString &strPathName);


	//Filtered Feature
	UINT32 _MakeFilterFeatureBuffer(vector<vector < vector<CFeature*>>> &vecvecFeature, int nCellCount, int nLineWidthCount);
	UINT32 _DeleteFilterFeatureBuffer(vector<vector < vector<CFeature*>>> &vecvecFeature);

	UINT32 _MakeFilterFeatureBuffer_V3(vector<vector < vector<CFeature*>>> &vecvecFeature, vector<vector<LINEWIDTH>> &vecvecLineWidth);
	
	UINT32 _GetFeature(vector<vector<CFeature*>> &vecvecFeature, CStep *pStep, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, int &eLineOption, vector<LINEWIDTH> &vecLineWidth, RECTD &drtView_mm);
	UINT32 _GetFeature_SubStep(vector<vector<CFeature*>> &vecvecFeature, CSubStep *pSubStep, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, int &eLineOption, vector<LINEWIDTH> &vecLineWidth, RECTD &drtView_mm);
	UINT32 _SetFeatureData(vector<vector<CFeature*>> &vecvecFeature, CFeature *pFeature, CRotMirrArr *pRotMirrArr, int &eLineOption, vector<LINEWIDTH> &vecLineWidth);

	UINT32 _GetFeature_V3(vector < vector<vector<CFeature*>>> &vecvecvecFeature, CStep *pStep, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, vector< vector<LINEWIDTH>> &vecvecLineWidth, RECTD &drtView_mm);
	UINT32 _GetFeature_SubStep_V3(vector < vector<vector<CFeature*>>> &vecvecvecFeature, CSubStep *pSubStep, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, vector< vector<LINEWIDTH>> &vecvecLineWidth, RECTD &drtView_mm);
	UINT32 _SetFeatureData_V3(vector < vector<vector<CFeature*>>> &vecvecvecFeature, CFeature *pFeature, CRotMirrArr *pRotMirrArr, vector< vector<LINEWIDTH>> &vecvecLineWidth);


	//GetFeatureInfo(Clicked)
	//UINT32 _GetFeatureInfo(IN const D2D1_POINT_2F &fptPoint_pixel, SELECT_INFO_UNIT &stSelectInfo);
	UINT32 _GetFeatureInfo(IN D2D1_POINT_2F &fptClickCenter_mm, IN D2D1_POINT_2F &fptClickCenter_Pixel, SELECT_INFO_UNIT &stSelectInfo);

	//현재 화면에 보이는 Feature 포인터와 Rotate Mirror 정보를 가져온다.
	UINT32 _GetFeature_RotMirr_View(CStep *pStep, CRotMirrArr *pRotMirrArr, RECTD &drtView, vector<CFeature*> &vecFeature,
		vector<CRotMirrArr*> &vecRotMirr);
	UINT32 _GetFeature_RotMirr_View_SubStep(CSubStep *pSubStep, CRotMirrArr *pRotMirrArr, RECTD &drtView,
		vector<CFeature*> &vecFeature, vector<CRotMirrArr*> &vecRotMirr);
	UINT32 _GetCheckFeature_Touch(IN D2D1_POINT_2F &fptClickCenter_mm, IN const D2D1_POINT_2F &fptPoint_pixel, vector<CFeature*> &vecFeature,
		vector<vector<DrawFeatureGeo*>> &vecDrawFeatureGeo, OUT vector<vector<DrawFeatureGeo*>> &vecvecTouchArea);
	
	// 2022.10.26 KJH ADD
	// DirectView에서 fov size 계산 위해 추가
	SIZE GetFOVSize(bool bIs3D, double dLens, double dZoom);

private :

	CSaveLayer				*m_pSaveLayer = nullptr;

	CSaveLayerOp			*m_pSaveLayerOp = nullptr;

	IWICImagingFactory2				*m_pImageFactory = nullptr;

	//DrawLayer
	vector<CDrawLayer*>		m_vecDrawLayer;

	CDrawFunction			*m_pDrawFunction = nullptr;

	//PictureBox
	D2D1_RECT_F m_frtPictureBox;


	//
	int m_nSelectStep = -1;
	BOOL m_bNeedUpdate = FALSE;//Step 번호가 변경되었거나, 편집이 되었을 경우 새로 그리기 생성을 위한 플래그
	eDrawLevel m_eDrawLv = eDrawLevel::DrawLevelNone;

	SELECT_INFO				m_stSelectInfo;

	//Measure Unit
	BOOL					m_bMeasureUnitOnly = FALSE;//FALSE = 전체 그리기, TRUE = 측정유닛만 그린다.
	vector<MeasureUnit>		m_vecMeasureUnitInfo;
	//

	//외부 Ptr
	//지우지 말것
	ID2D1Factory1			*m_pFactory = nullptr;
	ID2D1DeviceContext		*m_pD2DContext = nullptr;

	SystemSpec_t			*m_pSystemSpec = nullptr;
	UserSetInfo_t			*m_pUserSetInfo = nullptr;
	UserLayerSet_t			*m_pUserLayerSetInfo = nullptr;
	CJobFile				*m_pJobData = nullptr;
	ViewInfo_t				*m_pViewInfo = nullptr;

	vector<ID2D1BitmapRenderTarget*> m_vecRender;
	vector<ID2D1BitmapRenderTarget*> m_vecRender_SubStep;

	CDrawManager			*m_pDrawManager = nullptr;
	PanelInfo_t				*m_pPanelInfo = nullptr;
	FovInfo_t				*m_pFovInfo = nullptr;
	CellInfo_t				*m_pCellInfo = nullptr;

	
	//

protected:
	

public:
	
	
};

