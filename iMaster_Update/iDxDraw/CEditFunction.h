#pragma once

#include <vector>
using namespace std;

#include "../iODB/iODB.h"
#include "DxDraw/Def_Viewer.h"
#include "CDrawEdit_Temp.h"

class CEditFunction
{
public:
	CEditFunction();
	~CEditFunction();

	static UINT32 SetDelete(IN CJobFile *pJobFile, IN const int &nStep, IN const UserLayerSet_t* pUserLayerSet);
	static UINT32 SetDelete_Mask(IN const MaskType &eMaskType, IN CJobFile *pJobFile, IN const int &nStep, IN const UserLayerSet_t* pUserLayerSet);

	static UINT32 SetDeleteFeature(IN CTypedPtrArray <CObArray, CFeature*> &arrFeature, IN const int &nIndex);

	static UINT32 SetFeature_Add(vector<CFeatureData*>& vecFeatureData, CStep *pStep);
	static UINT32 SetFeature_AlignAdd(vector<CFeatureData*>& vecFeatureData, CStep *pStep);
};

