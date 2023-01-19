#pragma once

#include <vector>
using namespace std;

#include "../iODB/iODB.h"
#include "DxDraw/Def_Viewer.h"
#include <mutex>

class CDrawMakeGeo;

class __declspec(dllexport) CDrawFunction
{
private:
	//외부 Ptr
	ID2D1Factory1				*m_pFactory = nullptr;
	ID2D1DeviceContext			*m_pD2DContext = nullptr;
	//

	ID2D1SolidColorBrush		*m_pLayerColor = nullptr;
	ID2D1SolidColorBrush		*m_pLayerColor_Hole = nullptr;
	ID2D1SolidColorBrush		*m_pLayerColor_Neg = nullptr;

	ID2D1SolidColorBrush		*m_pLayerColor_Mask = nullptr;
	ID2D1SolidColorBrush		*m_pLayerColor_Mask_Select = nullptr;//선택
	ID2D1SolidColorBrush		*m_pLayerColor_Mask_PreSelect = nullptr;//마우스 오버
	ID2D1SolidColorBrush		*m_pLayerColor_Mask_Transparent = nullptr;//평상시 상태

private:
	ID2D1StrokeStyle			*m_pStrokeStyle_r = nullptr;
	ID2D1StrokeStyle			*m_pStrokeStyle_s = nullptr;
	ID2D1StrokeStyle			*m_pStrokeStyle_Dash = nullptr;
	ID2D1StrokeStyle			*m_pStrokeStyle_Dash_Dot = nullptr;
	bool						m_bIsDrawMode = true;
	eDrawLevel					m_eDrawLevel = eDrawLevel::eUnitLv;	

	CDrawMakeGeo				*m_pMakeGeo = nullptr;

public:
	CDrawFunction();
	~CDrawFunction();

	UINT32 _SetGeometry(vector<GEODRAW> &vecGeoData, ID2D1PathGeometry **pGeoObject, BOOL bOpen = FALSE);

	

	//Layer 이름에 맞는 Layer 포인터 가져오기, Step의 layer 순서와, SetLayer struct의 순서가 맞지 않음.
	static CLayer *_GetLayerInfo(UserLayerSet_t *pUserLayerSetInfo, CStep* pStep, CString strDefine);
	static CLayer *_GetLayerPrt(IN CTypedPtrArray <CObArray, CLayer*> &arrLayer, IN CString strLayer, OUT int &nLayerIndex);
	static CStep *_GetStepPrt(IN CTypedPtrArray <CObArray, CStep*> &arrStep, IN CString strStepName, OUT int &nStepIndex);

	static D2D1_RECT_U NormalizeRect(IN const D2D1_RECT_U& rtRect);
	static D2D1_RECT_F NormalizeRect(IN const D2D1_RECT_F& frtRect);

	static D2D1_POINT_2F GetRectCenter(IN const D2D1_RECT_F& frtRect);

	static D2D1_POINT_2F DetansformPoint(IN const D2D1_POINT_2F& fptOrg, IN const D2D1_MATRIX_3X2_F& matTrans,IN const UserSetInfo_t *pUserSetInfo);//화면 Pixel을 실제 좌표로 변환
	static D2D1_RECT_F DetansformRect(IN const D2D1_RECT_F& fptOrg, IN const D2D1_MATRIX_3X2_F& matTrans, IN const UserSetInfo_t *pUserSetInfo, IN const BOOL &bNormalize = TRUE);//화면 Pixel을 실제 좌표로 변환

	static CString		GetSymbolName(IN const SymbolName &eSymbolName, vector<double> &vecParam_mm);
	static SymbolName   GetSymbolType(IN const CString &strSymbolName);

	static double		GetSymbolSize(IN const CString &strSymbolName);

	static void GetOrient(IN const double &dAngle, IN const bool &bMirror, OUT Orient &ori);
	static void GetAngleMirror(IN const Orient &ori, OUT double &dAngle, OUT bool &bMirror);
	static void GetAngle(IN const Orient ori, OUT double &dAngle, OUT bool &bMirror);
	static Orient AddOrient(IN const Orient ori1, IN const Orient ori2);

	static CFeatureP *MakeRectFeature(IN RECTD &drtRect);

	BOOL CheckHighlight_TpFeature(IN D2D_POINT_2F fptPoint, IN CFeatureZ* pFeatureZ, IN CSymbol *pSymbol, IN const D2D1_MATRIX_3X2_F& matViewScale, ID2D1PathGeometry *pGeo = nullptr);
	BOOL CheckHighlight_LineFeature(IN D2D_POINT_2F fptPoint, IN CFeatureL* pFeatureL, IN CSymbol *pSymbol, IN const D2D1_MATRIX_3X2_F& matViewScale, ID2D1PathGeometry *pGeo = nullptr);
	BOOL CheckHighlight_PadFeature(IN D2D_POINT_2F fptPoint, IN CFeatureP* pFeatureP, IN CSymbol *pSymbol, IN const D2D1_MATRIX_3X2_F& matViewScale, ID2D1PathGeometry *pGeo = nullptr);
	BOOL CheckHighlight_ArcFeature(IN D2D_POINT_2F fptPoint, IN CFeatureA* pFeatureA, IN CSymbol *pSymbol, IN const D2D1_MATRIX_3X2_F& matViewScale, ID2D1PathGeometry *pGeo = nullptr);
	BOOL CheckHighlight_TextFeature(IN D2D_POINT_2F fptPoint, IN CFeatureT* pFeatureT, IN CSymbol *pSymbol, IN const D2D1_MATRIX_3X2_F& matViewScale, ID2D1PathGeometry *pGeo = nullptr);
	BOOL CheckHighlight_SurfaceFeature(IN D2D_POINT_2F fptPoint, IN CFeatureS* pFeatureS, IN CSymbol *pSymbol, IN const D2D1_MATRIX_3X2_F& matViewScale, OUT int &nSurfaceIndex, OUT float &fArea, IN vector<ID2D1PathGeometry *> vecGeo = vector<ID2D1PathGeometry *>());

	UINT32 GetTpDrawInfo(IN const CSymbol* pSymbol, OUT float &fPenWidth, ID2D1StrokeStyle	**pStrokeStyle);
	UINT32 GetLineDrawInfo(IN const CSymbol* pSymbol, OUT float &fPenWidth, ID2D1StrokeStyle	**pStrokeStyle);//return RESULT_GOOD :S troke Geometry, RESULT_BAD : Fill Geometry
	UINT32 GetPadDrawInfo(IN const CSymbol* pSymbol, OUT float &fPenWidth, ID2D1StrokeStyle	**pStrokeStyle, CRotMirrArr *pRotMirrArr);//return RESULT_GOOD :S troke Geometry, RESULT_BAD : Fill Geometry
	UINT32 GetArcDrawInfo(IN const CSymbol* pSymbol, OUT float &fPenWidth, ID2D1StrokeStyle	**pStrokeStyle);//return RESULT_GOOD :S troke Geometry, RESULT_BAD : Fill Geometry

	UINT32 _SetLink(ID2D1Factory1 *pFactory, ID2D1DeviceContext* pD2DContext);
	UINT32 _GetBrush(IN const bool &bPolarity, IN const bool &bHighlight, IN const bool &bPreHighlight, IN const COLORREF	color, ID2D1SolidColorBrush **pBrush, IN const BOOL &bSave = FALSE);
	UINT32 _GetBrush_Surface(IN const bool &bPolarity, IN const bool &bHighlight, IN const bool &bPreHighlight, IN const COLORREF	color, ID2D1SolidColorBrush **pBrush, IN const BOOL &bSave = FALSE);


	COLORREF _InvertColor(IN const COLORREF &color);

	//Draw Temp Point
	UINT32 _DrawTempPoint(IN ID2D1RenderTarget* pRender, IN D2D_POINT_2F fptCenter );
	UINT32 _DrawTempArcPoint(IN ID2D1RenderTarget* pRender, IN D2D_POINT_2F fptCenter);
	UINT32 _DrawTempLine(IN ID2D1RenderTarget* pRender, IN D2D_POINT_2F fptStart, IN D2D_POINT_2F fptEnd);
	UINT32 _DrawTempArc(IN ID2D1RenderTarget*pRender, IN D2D_POINT_2F fptStart, IN D2D_POINT_2F fptEnd, IN D2D_POINT_2F fptCenter,  IN BOOL bCW);
	
	//Draw Edit Point
	UINT32 DrawEditPoint(IN ID2D1RenderTarget* pRender, IN const RECTD &drtRect);
			

	//Draw Layer
	UINT32 OnRender_Layer(IN ID2D1RenderTarget* pRender, IN CLayer* pLayer, IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
		CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo, IN const RECTD &drtViewRect_mm = RECTD(0., 0., 0., 0.), IN const BOOL &bRedrawSelect = FALSE);

	//Make Layer
	UINT32 OnMake_Layer(IN ID2D1RenderTarget* pRender, IN CLayer* pLayer, IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
		CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo, eDrawLevel drawLevel, IN const BOOL &bRedrawSelect = FALSE);

	UINT32 OnMake_Layer_V2(IN ID2D1RenderTarget* pRender, IN CLayer* pLayer, IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
		CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo, eDrawLevel drawLevel,
		IN const SaveMode eSaveMode, IN const RECTD &drtViewRect_mm, IN CString strLineArcWidth = _T(""));

	UINT32 OnMake_Layer_FilteredFeature(IN ID2D1RenderTarget* pRender, vector<CFeature*> vecFeature,
		IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
		CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo, IN const RECTD &drtViewRect_mm);

	
	UINT32 _DrawTpFeature(IN ID2D1RenderTarget* pRender, IN CFeatureZ* pFeatureZ, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);

	//////////////////////////////////////////////////////////////////////////
	//Line Feature
	UINT32 _DrawLineFeature(IN ID2D1RenderTarget* pRender, IN CFeatureL* pFeatureL, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	

	//////////////////////////////////////////////////////////////////////////
	//Pad Feature
	UINT32 _DrawPadFeature(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo, IN const BOOL &bRedrawSelect, IN const BOOL &bSave = FALSE, IN RECTD drtViewRect_mm = RECTD(0, 0, 0, 0));
	UINT32 _DrawPadCircle(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadRect(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadRectxr(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadRectxc(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadOval(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadDi(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	//RectXC와 다른점 확인 필요
	UINT32 _DrawPadOct(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadDonut_r(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadDonut_s(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadHex_l(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadHex_s(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadBfr(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadBfs(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadTriangle(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadOval_h(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadThr(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadThs(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadS_ths(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadS_tho(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadSr_ths(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadRc_ths(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadRc_tho(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadEllipse(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadMoire(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawPadHole(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave);
	ID2D1PathGeometry		* _DrawPadUserdefined(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo, IN const BOOL &bRedrawSelect, IN const RECTD &drtViewRect_mm, IN const BOOL &bSave = FALSE);

	

	//////////////////////////////////////////////////////////////////////////
	//Arc Feature
	UINT32 _DrawArcFeature(IN ID2D1RenderTarget* pRender, IN CFeatureA* pFeatureA, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	

	//////////////////////////////////////////////////////////////////////////
	//Text Feature
	UINT32 _DrawTextFeature(IN ID2D1RenderTarget* pRender, IN CFeatureT* pFeatureT, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave = FALSE);
	UINT32 _DrawText(IN ID2D1RenderTarget* pRender, IN const CStandardFont* pFont, IN CFeatureT *pFeatureT,
		IN const LayerSet_t* pLayerSet, IN vector<ID2D1PathGeometry *> &vecGeo, IN const BOOL &bSave);
	

	//////////////////////////////////////////////////////////////////////////
	//Surface Feature
	UINT32 _DrawSurfaceFeature(IN ID2D1RenderTarget* pRender, IN CFeatureS* pFeatureS, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bRedrawSelect, IN const BOOL &bSave = FALSE, IN RECTD drtViewRect_mm = RECTD(0,0,0,0));
	UINT32 _CombineSurface(IN CFeatureS* pFeatureS, IN const BOOL &bNeedUpdate);

	//Surface가 많을 때 Combine하는 시간이 오래 걸림
	//Combine 하는 시간을 줄려주기 위하여 ObOe의 polarity가 연속적이라면, Merge하여 준다.
	UINT32 _MergePointGeo(vector<MakeFeatureData> &vecGeoDraw, vector<DrawFeatureGeo> &vecDrawFeatureGeo);

	//////////////////////////////////////////////////////////////////////////
	//Align / Mask
	UINT32 _DrawMask(IN ID2D1RenderTarget* pRender, IN const MaskType &eMaskType, IN CFeature* pFeature, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bRedrawSelect);
	UINT32 _MakeMask_Rect(IN CFeature* pFeature, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	UINT32 _MakeMask_Square(IN CFeature* pFeature, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);

	//Save할때 사용하는 Item;
	UINT32 _MakeRenderItem(IN ID2D1RenderTarget* pRender);
	UINT32 _DeleteRenderItem();
	//
	ID2D1SolidColorBrush		*m_pLayerColor_Save = nullptr;
	ID2D1SolidColorBrush		*m_pLayerColor_Hole_Save = nullptr;

	//
	static RECTD GetViewRect_mm(IN ID2D1RenderTarget* pRender, const UserSetInfo_t *pUserSetInfo, IN const BOOL &bSaveMode = TRUE);

	template<class Interface>
	static inline void SafeRelease(Interface **ppInterfaceToRelease) {
		if (*ppInterfaceToRelease != NULL) {
			(*ppInterfaceToRelease)->Release();
			(*ppInterfaceToRelease) = NULL;
		}
	}


	//Get Profile Data
public:
	static vector<vector<DrawProfileData>> GetProfileData(CJobFile *pJobFile, int nPanelStepIndex);

	static BOOL CheckPanelInOut(vector<vector<DrawProfileData>>, MeasureItem_t *tmpMeasure, int step = 2);
	static BOOL CheckStripInOut(vector<vector<DrawProfileData>>, MeasureItem_t *tmpMeasure, int &stripIndex, int step = 1);
	static BOOL CheckUnitInOut(vector<vector<DrawProfileData>>, MeasureItem_t *tmpMeasure, int &unitIndex, int step = 0);

private:
	
	static void _SubStepRepeat(CSubStep* pSubStep, CRotMirrArr *pRotMirrArr, vector<vector<DrawProfileData>> &vecProfileRect);
	static vector<DrawProfileData> _GetProfileRect(CFeatureS *pFeatureS, CRotMirrArr *rotMirrArr = nullptr);
	static vector<DrawProfileData>			_GetProfileRect_ObOe(CObOe *pSurface, CRotMirrArr *rotMirrArr = nullptr);
	//


	//Feature를 따로 생성하여 Geo를 만들때 사용
public:

	UINT32 MakeFeatureGeo(IN vector<CFeature *> &vecFeature, IN vector<CRotMirrArr *> &vecRotMirrArr, OUT vector<vector<DrawFeatureGeo*>> &vecvecDrawFeatureGeo);
	UINT32 MakeGeo_FeaturePad(CFeatureP* pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakeGeo_FeatureSurface(CFeatureS* pFeatureS, CRotMirrArr *pRotMirrArr, vector<ID2D1PathGeometry *> &vecGeo);
	//

};


