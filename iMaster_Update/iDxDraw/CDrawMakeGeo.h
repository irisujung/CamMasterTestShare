#pragma once

#include <vector>
using namespace std;

#include "../iODB/iODB.h"
#include "DxDraw/Def_Viewer.h"

class CDrawMakeGeo
{

public:
	CDrawMakeGeo();
	~CDrawMakeGeo();
	void SetLink(ID2D1Factory1 *pFactory);

	UINT32 SetGeometry(vector<GEODRAW> &vecGeoData, ID2D1PathGeometry **pGeoObject, BOOL bOpen = FALSE);

	void ClearDrawGeo(vector<DrawFeatureGeo>& vecDrawGeo);

	UINT32 MakeTp(IN CFeatureZ *pFeauteZ, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);

	//Line
	UINT32 MakeLine(IN CFeatureL *pFeatureL, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);

	//Pad
	UINT32 MakePadCircle(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadRect(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadRectxr(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadRectxc(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadOval(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadDi(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadOct(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadDonut_r(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadDonut_s(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadHex_l(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadHex_s(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadBfr(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadBfs(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadTriangle(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadOval_h(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadThr(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadThs(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	//여기부터는 Hole  존재함
	UINT32 MakePadS_ths(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadS_tho(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadSr_ths(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadRc_ths(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadRc_tho(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadEllipse(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadMoire(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);
	UINT32 MakePadHole(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);

	
	//Arc
	UINT32 MakeArc(IN CFeatureA *pFeatureA, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo);

	//Surface
	UINT32 MakeSurface(IN CFeatureS *pFeatureS, CRotMirrArr *pRotMirrArr, RECTD drtViewRect_mm, ID2D1PathGeometry **pGeo);

	//
	UINT32 CombineFeature(vector<DrawFeatureGeo> &vecDrawFeatureGeo, ID2D1PathGeometry **ppFeatureGeo);

	//
	UINT32 CombineFeature(vector<DrawFeatureGeo*> &vecDrawFeatureGeo, ID2D1PathGeometry **ppFeatureGeo);

private:


	//외부 Ptr
	//지우지 말것
	ID2D1Factory1				*m_pFactory = nullptr;

};

