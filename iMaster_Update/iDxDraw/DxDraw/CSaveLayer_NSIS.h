#pragma once


#ifdef _DEBUG
#pragma comment( lib, "opencv_world3416d.lib" )
#else
#pragma comment( lib, "opencv_world3416.lib" )
#endif

#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;


class CDrawFunction;

//mm로 변경시.
//#define CONVERT_MAG 700 //Render Target Size = Data Size x CONVERT_MAG

#include "CSaveLayer.h"

class CSaveLayer_NSIS : public CSaveLayer
{

public:
	CSaveLayer_NSIS();
	virtual ~CSaveLayer_NSIS();

	virtual UINT32 SaveImage(IN const CString &strfilename, IN const double &dCamAngle,
		IN CStep *pStep, IN LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
		CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo, IN const BOOL &bSaveRegion = FALSE,
		IN const SaveMode eSaveMode = SaveMode::SaveMode_All, IN bool bExceptDummy = false, IN CString strLineArcWidth = _T(""),
		double dAngle = 0.f, bool bMirror = false);

	virtual UINT32 _CreateMergeImage(IN const CString &strfilename, IN BOOL bIsSaveRaw = FALSE) { return RESULT_GOOD; }

	virtual UINT32 SaveImage_FilteredFeature(IN const CString &strFileName,
		vector<CFeature *> vecFeature,
		IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo) { return RESULT_GOOD; }
	

public:


	
private:

	UINT32	_DrawRender(ID2D1RenderTarget* pRender, IN  CLayer *pLayer, IN const LayerSet_t* pLayerSet,
		CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo,
		double dAngle = 0.f, bool bMirror = false);

	UINT32 _SetSaveImage(IN const CString &strfilename, IN const double &dAngle = 0.0);

	void	_SetViewScale(IN const UserSetInfo_t *pUserSetInfo, IN const float &fScale, IN const D2D_POINT_2F &fptPan, double dAngle = 0.f, bool bMirror = false);

private:

	
	
	
};

