#pragma once

#include <vector>
using namespace std;

#include "../iODB/iODB.h"
#include "DxDraw/Def_Viewer.h"

class CDrawFunction;

class CFeatureFilterFunction
{

public:
	CFeatureFilterFunction();
	~CFeatureFilterFunction();

	void SetLink(ID2D1Factory1 *pFactory, ID2D1DeviceContext* pD2DContext);

	BOOL CompareFeature(IN CLayer *pLayer, IN const int &nFeatureIndex_1, IN const int &nFeatureIndex_2);

	
private : 
	BOOL CompareSurface(CFeatureS *pFeaureS_1, CFeatureS *pFeaureS_2);

	CDrawFunction		*m_pDrawFunction = nullptr;

	//외부 Ptr
	//지우지 말것
	ID2D1Factory1		*m_pFactory = nullptr;
	ID2D1DeviceContext	*m_pD2DContext = nullptr;
};

