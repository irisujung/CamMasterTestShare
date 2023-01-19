#pragma once

class CDrawFunction;
#include "CSaveLayer.h"

#pragma warning( disable: 4805)//
#pragma comment( lib, "halconcppxl.lib" )
#include "HalconCpp.h"
using namespace HalconCpp;

#pragma comment( lib, "hdevenginecpp.lib" )
#include "HDevEngineCpp.h"
using namespace HDevEngineCpp;

/* 1 Bit PNG Test
#ifdef _DEBUG
#pragma comment( lib, "opencv_world3416d.lib" )
#else
#pragma comment( lib, "opencv_world3416.lib" )
#endif

#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;
*/
class CHalconFunction;

class CSaveLayer_AOI : public CSaveLayer
{

public:
	CSaveLayer_AOI();
	virtual ~CSaveLayer_AOI();
	
	virtual UINT32 SaveImage(IN const CString &strfilename, IN const double &dCamAngle,
		IN CStep *pStep, IN LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
		CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo, IN const BOOL &bSaveRegion = TRUE,
		IN const SaveMode eSaveMode = SaveMode::SaveMode_All, IN bool bExceptDummy = false, IN CString strLineArcWidth = _T(""),
		double dAngle = 0.f, bool bMirror = false);
	
	//Region 저장 : Feature 정보 기반
	UINT32 SaveMasterRegion(IN const CString &strfilename, IN const double &dCamAngle,
		IN CStep *pStep, IN LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
		CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo,
		IN const SaveMode eSaveMode = SaveMode::SaveMode_All,
		double dAngle = 0.f, bool bMirror = false);

	virtual UINT32 SaveImage_FilteredFeature(IN const CString &strFileName,
		vector<CFeature *> vecFeature,
		IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo);

	//Region 출력 : 저장된 원본 영상 기반
	UINT32 ConvRegion(IN const CString &strSrcPathName, IN const CString &strDstPath);
	
	virtual UINT32 _CreateMergeImage(IN const CString &strfilename, IN BOOL bIsSaveRaw = FALSE);

	UINT32 CreateConcatRegion(IN vector<CString> &vecSrcRegion, CString  &strDstPathName, IN const BOOL &bSrcDelete = TRUE);

	// 2023.01.18 KJH ADD
	// Strip Image Save Thread 
	BOOL _DrawRender_Split_Thread(ID2D1RenderTarget* pRender, IN const int &nSplitIndexX, IN const int &nSplitIndexY,
		IN CStep *pStep, IN const LayerSet_t* pLayerSet,
		CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo,
		IN const SaveMode eSaveMode, IN bool bExceptDummy, IN CString strLineArcWidth, double dAngle, bool bMirror);

public:


	
private:
	
	UINT32 _SaveMasterImage(IN const CString &strFilename, 
		IN CStep *pStep, IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
		CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo, IN const BOOL &bSaveRegion = TRUE,
		IN const SaveMode eSaveMode = SaveMode::SaveMode_All, IN bool bExceptDummy = false, IN CString strLineArcWidth = _T(""), double dAngle = 0.f, bool bMirror = false);

	// 2023.01.18 KJH ADD
	// Strip Image Save 속도 빠르게 하기 위해 추가 
	UINT32 _SaveMasterImage_V2(IN const CString &strFilename,
		IN CStep *pStep, IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
		CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo, IN const BOOL &bSaveRegion = TRUE,
		IN const SaveMode eSaveMode = SaveMode::SaveMode_All, IN bool bExceptDummy = false, IN CString strLineArcWidth = _T(""), double dAngle = 0.f, bool bMirror = false);
	
	UINT32 _SaveMasterRegion(IN const CString &strFilename,
		IN CStep *pStep, IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
		CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo,
		IN const SaveMode eSaveMode = SaveMode::SaveMode_All, double dAngle = 0.f, bool bMirror = false);

	UINT32	_DrawRender_Split(ID2D1RenderTarget* pRender, IN const int &nSplitIndexX, IN const int &nSplitIndexY,
		IN CStep *pStep, IN const LayerSet_t* pLayerSet,
		CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo,
		IN const SaveMode eSaveMode, IN bool bExceptDummy = false, IN CString strLineArcWidth = _T(""), double dAngle = 0.f, bool bMirror = false);

	UINT32 _DrawSubStep(ID2D1RenderTarget* pRender, IN CSubStep* pSubStep, IN const LayerSet_t* pLayerSet,
		CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo,
		IN const SaveMode eSaveMode, IN const RECTD &drtViewRect_mm, IN CString strLineArcWidth = _T(""));

	UINT32	_DrawRender_Split_FilteredFeature(ID2D1RenderTarget* pRender, IN const int &nSplitIndexX, IN const int &nSplitIndexY,
		vector<CFeature *> vecFeature,
		IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo);
	

	UINT32	_SaveRegion_Split(ID2D1RenderTarget* pRender, IN const int &nSplitIndexX, IN const int &nSplitIndexY,
		IN CStep *pStep, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo,
		IN const SaveMode eSaveMode, IN const CString &strFilename, double dAngle = 0.f, bool bMirror = false);

	UINT32 _SaveRegion_Split_SubStep(RECTD drtImage_mm, IN CSubStep* pSubStep, IN const LayerSet_t* pLayerSet,
		CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo, IN const SaveMode eSaveMode);

	UINT32 _SetSaveImage(IN const CString &strfilename, IN const int &nSplitIndexX, IN const int &nSplitIndexY, IN BOOL bDelSrc = FALSE, IN BOOL bSaveRaw = FALSE);
	
	//WIC Image를 사용하여 Halcon저장을 이용
	UINT32 _SetSaveImage_V2(IN const CString &strfilename, IN const int &nSplitIndexX, IN const int &nSplitIndexY, IN BOOL bSaveImg = TRUE, IN BOOL bSaveRegion = TRUE, IN BOOL bSaveRaw = FALSE);

	UINT32 _SetSplitImage_Halcon(IN IWICBitmap *pWicColor, IN const int &nSplitIndexX, IN const int &nSplitIndexY);

	UINT32 _SetSaveImage_DirectX(IN const CString &strfilename);
	UINT32 _SetSaveImage_Halcon(IN const CString &strfilename);
		
	UINT32 _SetSaveRegion(IN const CString &strfilename, IN const int &nSplitIndexX, IN const int &nSplitIndexY);

	void	_SetTransform_Split(ID2D1RenderTarget* pRender, IN const UserSetInfo_t *pUserSetInfo, IN const int nSplitX, IN const int nSplitY, double dAngle = 0.f, bool bMirror = false);
	void	_SetTransform(ID2D1RenderTarget* pRender, IN const UserSetInfo_t *pUserSetInfo, IN const int nSplitX, IN const int nSplitY, double dAngle = 0.f, bool bMirror = false);
	void	_SetViewScale(IN const UserSetInfo_t *pUserSetInfo, IN const float &fScale, IN const D2D_POINT_2F &fptPan, double dAngle = 0.f, bool bMirror = false);
	
	CString GetSplitFileName(IN const CString &strfilename, IN const int &nSplitIndexX, IN const int &nSplitIndexY);

//	static UINT32 _ConvertImage_1Bit(IN const CString strfilename, IN CEvent* pEvent);

private:
	mutex					m_mtx;

	CHalconFunction			*m_pHalconFunction = nullptr;
	CMakeRegionFunction		*m_pMakeRegionFunction = nullptr;	
};

