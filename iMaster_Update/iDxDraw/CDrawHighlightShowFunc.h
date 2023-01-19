#pragma once

#include <vector>
using namespace std;

#include "../iODB/iODB.h"
#include "DxDraw/Def_Viewer.h"

class CDrawHighlightShowFunc
{
	typedef enum _SurfaceMode {
		enumSurfaceMode_Select = 0,
		enumSurfaceMode_UnSelect,
		enumSurfaceMode_Show,
		enumSurfaceMode_Hide,

		enumSurfaceMode_Max
	}SurfaceMode;

public:
	CDrawHighlightShowFunc();
	~CDrawHighlightShowFunc();

	//Feature Select
	static UINT32			ResetSelect(IN CJobFile *pJobFile);
	static UINT32			SetSelect(IN CJobFile *pJobFile, IN const int &nStep, vector<FEATURE_INFO> &vecFeatureInfo, SELECT_INFO &stSelectInfo);
	static UINT32			SetUnSelect(IN CJobFile *pJobFile, IN const int &nStep, vector<FEATURE_INFO> &vecFeatureInfo, SELECT_INFO &stSelectInfo);

	//Feature Show/Hide
	static UINT32			ResetHide(IN CJobFile *pJobFile);
	static UINT32			SetShow(IN CJobFile *pJobFile, IN const int &nStep,vector<FEATURE_INFO> &vecFeatureInfo);
	static UINT32			SetHide(IN CJobFile *pJobFile, IN const int &nStep,vector<FEATURE_INFO> &vecFeatureInfo);

	static void UpdateSelectInfo(IN CJobFile *pJobFile, IN const int &nStep, IN const UserLayerSet_t *pUserLayerSet, SELECT_INFO &stSelectInfo);
	//Select Info : 선택된 개체의 정보의 모음
	static void SetSelectInfo(IN const CFeatureZ* pFeatureZ, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo);
	static void SetSelectInfo(IN const CFeatureL* pFeatureL, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo);
	static void SetSelectInfo(IN const CFeatureP* pFeatureP, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo);
	static void SetSelectInfo(IN const CFeatureA* pFeatureA, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo);
	static void SetSelectInfo(IN const CFeatureT* pFeatureT, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo);
	static void SetSelectInfo(IN const CFeatureS* pFeatureS, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo);
	static void SetSelectInfo(IN const CObOe* pObOe, IN const int &nFeatureIndex, IN const int &nObOeIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo);


	static void DeleteSelectInfo(IN const CFeatureZ* pFeatureZ, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo);
	static void DeleteSelectInfo(IN const CFeatureL* pFeatureL, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo);
	static void DeleteSelectInfo(IN const CFeatureP* pFeatureP, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo);
	static void DeleteSelectInfo(IN const CFeatureA* pFeatureA, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo);
	static void DeleteSelectInfo(IN const CFeatureT* pFeatureT, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo);
	static void DeleteSelectInfo(IN const CFeatureS* pFeatureS, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo);
	static void DeleteSelectInfo(IN const CObOe* pObOe, IN const int &nFeatureIndex, IN const int &nObOeIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo);

	

private:
	static UINT32	_GetSurfaceIndex(IN CJobFile *pJobFile, IN const int &nStep, IN const CString &strLayerName, OUT vector<int> &verSurfaceIndex);

	static void		_SetSurface_byName(IN const SurfaceMode &emMode, IN const CFeatureS* pFeatureS, IN const int &nFeatureIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo);

	static void		_SetFeature_byName(IN const SurfaceMode &emMode, IN const CLayer *pLayer, FEATURE_INFO &stFeatureInfo, SELECT_INFO &stSelectInfo);
	static void		_SetFeature_byAtt(IN const SurfaceMode &emMode, IN const CLayer *pLayer, FEATURE_INFO &stFeatureInfo, SELECT_INFO &stSelectInfo);
	static void		_SetFeature_bySymbol(IN const SurfaceMode &emMode, IN const CLayer *pLayer, FEATURE_INFO &stFeatureInfo, SELECT_INFO &stSelectInfo);

};

