#pragma once

#include "../../iODB/iODB.h"
#include "Def_MakeRegion.h"

class CHalconFunction;
class CMakeRegionFunction
{
public:
	CMakeRegionFunction();
	~CMakeRegionFunction();

	void SetLink(IN CHalconFunction *pHalconFunction);

	UINT32 OnMake_Feature(IN CLayer* pLayer, RECTD drtImage_mm,IN const int nFeatureIndex, IN const LayerSet_t* pLayerSet,
		CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo,	IN const SaveMode &eSaveMode, IN const double &dResolution);

	

private :
	vector<GEODRAW> _SetMirrorRotate(vector<GEODRAW> &vecGeoData, CRotMirrArr *pRotMirrArr, IN const double &dAngle = 0);

	UINT32 _GetLineDrawInfo(IN const CSymbol* pSymbol, OUT float &fPenWidth, HStrokeStyle &eStrokeStyle);//return RESULT_GOOD :S troke Geometry, RESULT_BAD : Fill Geometry
	UINT32 _GetPadDrawInfo(IN const CSymbol* pSymbol, OUT float &fPenWidth, HStrokeStyle &eStrokeStyle, CRotMirrArr *pRotMirrArr);//return RESULT_GOOD :S troke Geometry, RESULT_BAD : Fill Geometry
	UINT32 _GetArcDrawInfo(IN const CSymbol* pSymbol, OUT float &fPenWidth, HStrokeStyle &eStrokeStyle);//return RESULT_GOOD :S troke Geometry, RESULT_BAD : Fill Geometry

	//Make Line Region
	UINT32 MakeRegion_Line(IN vector<MakeFeatureData>& vecPointData, IN const float &fPenWidth, IN const HStrokeStyle &eStrokeStyle,
		OUT vector<RegionData> &vecRegionData);

	//Make Arc Region
	UINT32 MakeRegion_Arc(IN vector<MakeFeatureData>& vecPointData, IN const float &fPenWidth, IN const HStrokeStyle &eStrokeStyle,
		OUT vector<RegionData> &vecRegionData);

	UINT32 MakeRegion_Pad(IN vector<MakeFeatureData>& vecPointData, CFeature *pFeature, CRotMirrArr *pRotMirrArr, IN const double dResolution, OUT vector<RegionData> &vecRegion);

	UINT32 MakeShape_Line(IN HObject &ho_Region, IN const double &dAngle, IN const float &fPenWidth, IN const HStrokeStyle &eStrokeStyle, OUT HObject &ho_Shaped);
	UINT32 MakeShape_Arc(IN HObject &ho_Region, IN const float &fPenWidth, IN const HStrokeStyle &eStrokeStyle, OUT HObject &ho_Shaped);
	UINT32 MakeShape_Pad(IN HObject &ho_Region, IN const float &fPenWidth, IN const HStrokeStyle &eStrokeStyle, OUT HObject &ho_Shaped);

	HObject MakeArcRegion(IN const HTuple &Sx, IN const HTuple &Sy, IN const HTuple &Ex, IN const HTuple &Ey,
		IN const HTuple &Cx, IN const HTuple &Cy, IN const bool &bCw);

	vector<D2D1_POINT_2F> _MakeArc_Point(IN const D2D1_POINT_2F fptStart, IN const D2D1_POINT_2F fptEnd, IN const D2D1_POINT_2F fptCenter, IN const bool &bCw);

	vector<RegionData> _DrawTpFeature(IN CFeatureZ* pFeatureZ, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);

	vector<RegionData> _DrawLineFeature(IN CFeatureL* pFeatureL, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawArcFeature(IN CFeatureA* pFeatureA, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadFeature(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);

	vector<RegionData> _DrawPadCircle(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadRect(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadRectxr(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadRectxc(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadOval(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadDi(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadOct(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadDonut_r(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadDonut_s(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadHex_l(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadHex_s(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadBfr(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadBfs(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadTriangle(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadOval_h(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadThr(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadThs(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadS_ths(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadS_tho(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadSr_ths(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadRc_ths(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadRc_tho(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadEllipse(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadMoire(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);
	vector<RegionData> _DrawPadUserdefined(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, IN RECTD &drtViewRect_mm, IN const double &dResolution);
	
	vector<RegionData> _DrawSurfaceFeature(IN CFeatureS* pFeatureS, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution);


private:

	//외부 Prt
	//지우지 말것
	CHalconFunction			*m_pHalconFunction = nullptr;
};

