#include "pch.h"
#include "CSaveLayer_AOI.h"
#include "../CDrawLayer.h"

#include "../MakeRegion/CHalconFunction.h"
#include "../MakeRegion/CMakeRegionFunction.h"
#include "../../iUtilities/PerfTimerEx.h"

#include <wrl.h>//Comptr
using namespace Microsoft::WRL;

#include "shlobj_core.h"

#include <thread>
#include <queue>
#include <future>
#include <mutex>

CSaveLayer_AOI::CSaveLayer_AOI()
{
	m_pHalconFunction = new CHalconFunction;
	m_pMakeRegionFunction = new CMakeRegionFunction;
	m_pMakeRegionFunction->SetLink(m_pHalconFunction);

}

CSaveLayer_AOI::~CSaveLayer_AOI()
{
	if (m_pHalconFunction != nullptr)
	{
		delete m_pHalconFunction;
		m_pHalconFunction = nullptr;
	}

	if (m_pMakeRegionFunction != nullptr)
	{
		delete m_pMakeRegionFunction;
		m_pMakeRegionFunction = nullptr;
	}
	
}

UINT32 CSaveLayer_AOI::SaveImage(IN const CString &strFilename, IN const double &dCamAngle,
	IN CStep *pStep, IN LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
	CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo, IN const BOOL &bSaveRegion,
	IN const SaveMode eSaveMode, IN bool bExceptDummy, IN CString strLineArcWidth,
	double dAngle, bool bMirror)
{
	UINT32 nRet = RESULT_GOOD;

	if (pUserSetInfo->mcType != eMachineType::eAVI)
	{
		nRet = _SaveMasterImage(strFilename, pStep, pLayerSet, pFontArr, pRotMirrArr, pUserSetInfo, bSaveRegion, eSaveMode, bExceptDummy, strLineArcWidth, dAngle, bMirror);
	}
	else
	{
		nRet = _SaveMasterImage_V2(strFilename, pStep, pLayerSet, pFontArr, pRotMirrArr, pUserSetInfo, bSaveRegion, eSaveMode, bExceptDummy, strLineArcWidth, dAngle, bMirror);
	}
	

	return nRet;
}

UINT32 CSaveLayer_AOI::SaveMasterRegion(IN const CString &strFilename, IN const double &dCamAngle,
	IN CStep *pStep, IN LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
	CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo,
	IN const SaveMode eSaveMode,
	double dAngle, bool bMirror)
{
	UINT32 nRet = RESULT_GOOD;

	if (
		// eSaveMode != SaveMode::SaveMode_All)
		//임시, Pad, Surface 구현 필요
		(pUserSetInfo->bMirror == false && pUserSetInfo->dAngle == 0.0) && //2021/12/26 현재는 0도만 가능
		(eSaveMode != SaveMode::SaveMode_All && 
			eSaveMode != SaveMode::SaveMode_Mask))
	{
		nRet = _SaveMasterRegion(strFilename, pStep, pLayerSet, pFontArr, pRotMirrArr, bNeedUpdate, pUserSetInfo, eSaveMode);
	}

	return nRet;
}

UINT32 CSaveLayer_AOI::SaveImage_FilteredFeature(IN const CString &strFileName,
	vector<CFeature *> vecFeature,
	IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo)
{
	UINT32 nRet = RESULT_GOOD;

	CPerfTimerEx clTimer;

	for (int i = 0; i < m_ptSplitCount.y; i++)
	{
		for (int j = 0; j < m_ptSplitCount.x; j++)
		{
			clTimer.StartTimer();

			_BeginDraw();

			nRet |= _DrawRender_Split_FilteredFeature(m_pImageRender, j, i, vecFeature, pLayerSet, pFontArr, nullptr,  pUserSetInfo);


			nRet = _EndDraw();

			if (nRet == RESULT_GOOD)
			{
				_SetSaveImage_V2(strFileName, j, i, FALSE, TRUE, FALSE);
			}
		}
	}
	
	

	return nRet;
}

UINT32 CSaveLayer_AOI::ConvRegion(IN const CString &strSrcPathName, IN const CString &strDstPath)
{
	UINT32 nRet = RESULT_GOOD;

	m_pHalconFunction->ConvRegion(strSrcPathName, strDstPath);

	return nRet;
}

UINT32 CSaveLayer_AOI::_SaveMasterImage(IN const CString &strFilename,
	IN CStep *pStep, IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
	CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo, IN const BOOL &bSaveRegion,
	IN const SaveMode eSaveMode, IN bool bExceptDummy, IN CString strLineArcWidth,
	double dAngle, bool bMirror)
{
	UINT32 nRet = RESULT_BAD;
	
	if (pStep == nullptr) return nRet;
	if (pLayerSet == nullptr) return nRet;
	if (pUserSetInfo == nullptr) return nRet;

	BOOL bSaveRaw = FALSE;
	if (pUserSetInfo->mcType == eMachineType::eAVI &&
		m_eDrawLevel == eDrawLevel::eUnitLv)
		bSaveRaw = TRUE;

	CPerfTimerEx clTimer;
	CString strLog;

	BOOL bNeedMerge = FALSE;
	if (( m_ptSplitCount.y == 1 &&
		m_ptSplitCount.x == 1 ) == FALSE )
	{//다중저장일 경우
		
		m_pHalconFunction->SetSplitSize(m_ptSplitCount.x, m_ptSplitCount.y);
		bNeedMerge = TRUE;
	}

	for (int i = 0; i < m_ptSplitCount.y; i++)
	{
		for (int j = 0; j < m_ptSplitCount.x; j++)
		{
			clTimer.StartTimer();

			_BeginDraw();

			_DrawRender_Split(m_pImageRender, j, i, pStep, pLayerSet, pFontArr, pRotMirrArr, TRUE, pUserSetInfo, eSaveMode, bExceptDummy, strLineArcWidth);

			nRet = _EndDraw();

			double dTime = clTimer.StopTimer(CTimerUnit_en::EN_MIL_SEC);
			strLog.Format(_T("\n%s : Split[R%d][C%d], Processing Time[%.2f ms]"), pLayerSet->strLayer, i, j, dTime);
			OutputDebugString(strLog);

			/*
			//V1
			if (nRet == RESULT_GOOD)
			{
				BOOL bDelImgFile = FALSE;
				if (pUserSetInfo->mcType == eMachineType::eAOI && m_eDrawLevel == eDrawLevel::ePanelLv)
				{
					bDelImgFile = TRUE;
					if (pLayerSet->strDefine.CompareNoCase(PATTERN_LAYER_TXT) == 0 && 
						eSaveMode == SaveMode::SaveMode_All)
						bDelImgFile = FALSE;
				}

				_SetSaveImage(strFilename, j, i, bDelImgFile, bSaveRaw);
			}
			*/
			//V2
			if (nRet == RESULT_GOOD)
			{
				BOOL bSaveImage = TRUE;
				if (pUserSetInfo->mcType == eMachineType::eAOI && m_eDrawLevel == eDrawLevel::ePanelLv)
				{
					bSaveImage = FALSE;
					if (pLayerSet->strDefine.CompareNoCase(PATTERN_LAYER_TXT) == 0 &&
						eSaveMode == SaveMode::SaveMode_All)
						bSaveImage = TRUE;
				}

				if (bNeedMerge == TRUE)
				{
					_SetSplitImage_Halcon(m_pWicBitmap, j, i);
				}
				else
				{
					_SetSaveImage_V2(strFilename, j, i, bSaveImage, bSaveRegion, bSaveRaw);
				}

				
			}
			
			else
			{
				break;
			}
		}

		if (nRet != RESULT_GOOD)
		{
			break;
		}
	}

	if (bNeedMerge == TRUE)
	{
		if (pUserSetInfo->mcType == eMachineType::eAVI)
		{
			bSaveRaw = TRUE;
		}
		
		//Make Merge Image
		m_pHalconFunction->MakeSwathImage(strFilename, m_vecvecSplitPos, m_nOverlapPixel,
			static_cast<int>(m_sizeTarget.width), static_cast<int>(m_sizeTarget.height), bSaveRaw);
		
	}
	//Clear Merge Image
	m_pHalconFunction->ClearSplitImage();
	
	//Memory 이슈로 전체 이미지 저장 후, 하자...
/*	if (nRet == RESULT_GOOD)
	{
		if (m_eDrawLevel == eDrawLevel::ePanelLv && m_pHalconFunction)
		{
			m_pHalconFunction->CreateMergeImage(strFilename, m_vecvecSplitPos, m_nOverlapPixel,
				static_cast<int>(m_sizeTarget.width), static_cast<int>(m_sizeTarget.height));
		}
	

		for (auto itY : m_vecvecSplitPos)
		{
			for (auto itX : itY)
			{
#ifdef _UNICODE
				DeleteFileW(itX.strPathName);
#else
				DeleteFileA(itX.strPathName);
#endif
			}
		}	

		//Delete Temp Folder
		// Change File Name
		fs::path fsPath;
		fsPath = std::string(CT2CA(strFilename));
		CString strPathName = CA2CT(fsPath.parent_path().string().c_str());
		
		strPathName += _T("\\Temp\\");

		CUtil::DeleteAllFiles(strPathName);//Temp 폴더내 파일 삭제
		RemoveDirectory(strPathName);//비어있는 Temp 폴더 삭제
	}*/

	return nRet;
}


// 2023.01.18 KJH ADD
// Strip Image Save 속도 빠르게 하기 위해 추가 
UINT32 CSaveLayer_AOI::_SaveMasterImage_V2(IN const CString &strFilename,
	IN CStep *pStep, IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
	CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo, IN const BOOL &bSaveRegion,
	IN const SaveMode eSaveMode, IN bool bExceptDummy, IN CString strLineArcWidth,
	double dAngle, bool bMirror)
{
	UINT32 nRet = RESULT_BAD;

	if (pStep == nullptr) return nRet;
	if (pLayerSet == nullptr) return nRet;
	if (pUserSetInfo == nullptr) return nRet;

	BOOL bSaveRaw = FALSE;
	if (pUserSetInfo->mcType == eMachineType::eAVI &&
		m_eDrawLevel == eDrawLevel::eUnitLv)
		bSaveRaw = TRUE;

	//CPerfTimerEx clTimer;
	//CString strLog;

	BOOL bNeedMerge = FALSE;
	if ((m_ptSplitCount.y == 1 &&
		m_ptSplitCount.x == 1) == FALSE)
	{//다중저장일 경우
		m_pHalconFunction->SetSplitSize(m_ptSplitCount.x, m_ptSplitCount.y);
		bNeedMerge = TRUE;
	}


	int nMaxThread = 7;
	vector<thread> vecThreads;	
	//vecThreads.resize(nMaxThread);
	vector<BOOL> vecFinish;
	vecFinish.resize(nMaxThread);

	vector<future<BOOL>> futures;
	futures.resize(nMaxThread);

	int nRowRpetCount = m_ptSplitCount.x / nMaxThread;
	int nCount = 0;
	
	for (int i = 0; i < m_ptSplitCount.y; i++)
	{
		for (int j = 0; j < m_ptSplitCount.x; j+= nMaxThread)		
		//for (int j = 0; j < m_ptSplitCount.x; j++)		
		{
			if (bNeedMerge == FALSE)
			{
				nRet = _BeginDraw();

				nRet = _DrawRender_Split(m_pImageRender, j, i, pStep, pLayerSet, pFontArr, pRotMirrArr, TRUE, pUserSetInfo, eSaveMode, bExceptDummy, strLineArcWidth, dAngle, bMirror);

				nRet = _EndDraw();
		
				nRet = _SetSaveImage_V2(strFilename, j, i, TRUE, FALSE, bSaveRaw);
				
			}
			else
			{
				vecThreads.clear();		

				if ((nRowRpetCount <= nCount) && (nMaxThread >= nCount))
				{
					nMaxThread = m_ptSplitCount.x - j;
				}

				for (int t = 0; t < nMaxThread; ++t)
				{
					int tj = t + j;
					
					vecThreads.push_back(thread(&CSaveLayer_AOI::_DrawRender_Split_Thread, this, m_pImageRender, tj, i, pStep, pLayerSet, pFontArr, pRotMirrArr, TRUE, pUserSetInfo, eSaveMode, bExceptDummy, strLineArcWidth, dAngle, bMirror));
					vecThreads[t].join();					
					//vecThreads[t].detach();					

				}

				/*vecThreads.push_back(thread(&CSaveLayer_AOI::_DrawRender_Split_Thread, this, m_pImageRender, j, i, pStep, pLayerSet, pFontArr, pRotMirrArr, TRUE, pUserSetInfo, eSaveMode, bExceptDummy, strLineArcWidth, dAngle, bMirror));
				vecThreads[nCount].c();*/

				nCount++;
				

				/*vecThreads.push_back(thread(&CSaveLayer_AOI::_DrawRender_Split_Thread, this, m_pImageRender, j, i, pStep, pLayerSet, pFontArr, pRotMirrArr, TRUE, pUserSetInfo, eSaveMode, bExceptDummy, strLineArcWidth, dAngle, bMirror));
				vecThreads.push_back(thread(&CSaveLayer_AOI::_DrawRender_Split_Thread, this, m_pImageRender, j+1, i, pStep, pLayerSet, pFontArr, pRotMirrArr, TRUE, pUserSetInfo, eSaveMode, bExceptDummy, strLineArcWidth, dAngle, bMirror));
				vecThreads.push_back(thread(&CSaveLayer_AOI::_DrawRender_Split_Thread, this, m_pImageRender, j+2, i, pStep, pLayerSet, pFontArr, pRotMirrArr, TRUE, pUserSetInfo, eSaveMode, bExceptDummy, strLineArcWidth, dAngle, bMirror));
				
				vecThreads[0].join();
				vecThreads[1].join();
				vecThreads[2].join();*/

				//vecThreads[0].detach();
				//vecThreads[1].detach();
				
				//for (int t = 0; t < 2; ++t)
				//{
				//	//futures[t] = async(launch::async, &CSaveLayer_AOI::_DrawRender_Split_Thread, this, m_pImageRender, j, i, pStep, pLayerSet, pFontArr, pRotMirrArr, TRUE, pUserSetInfo, eSaveMode, bExceptDummy, strLineArcWidth, dAngle, bMirror);
				//	futures[t] = async(&CSaveLayer_AOI::_DrawRender_Split_Thread, this, m_pImageRender, j, i, pStep, pLayerSet, pFontArr, pRotMirrArr, TRUE, pUserSetInfo, eSaveMode, bExceptDummy, strLineArcWidth, dAngle, bMirror);
				//}
				//
				//for (int t = 0; t < 2; ++t)
				//{
				//	futures[t].get();
				//}

			}			

		}
		
	}	

	//////////////////////////////////////////////////
	
	if (bNeedMerge == TRUE)
	{
		if (pUserSetInfo->mcType == eMachineType::eAVI)
		{
			bSaveRaw = TRUE;
		}

		//Make Merge Image
		m_pHalconFunction->MakeSwathImage(strFilename, m_vecvecSplitPos, m_nOverlapPixel,
			static_cast<int>(m_sizeTarget.width), static_cast<int>(m_sizeTarget.height), bSaveRaw);

	}

	//Clear Merge Image
	m_pHalconFunction->ClearSplitImage();
	
	return nRet;
}

BOOL CSaveLayer_AOI::_DrawRender_Split_Thread(ID2D1RenderTarget* pRender, IN const int &nSplitIndexX, IN const int &nSplitIndexY,
	IN CStep *pStep, IN const LayerSet_t* pLayerSet,
	CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo,
	IN const SaveMode eSaveMode, IN bool bExceptDummy, IN CString strLineArcWidth, double dAngle, bool bMirror)
{
	CPerfTimerEx clTimer;
	CString strLog;
	
	clTimer.StartTimer();

	//m_mtx.lock();

	_BeginDraw();
	
	_DrawRender_Split(pRender, nSplitIndexX, nSplitIndexY, pStep, pLayerSet, pFontArr, pRotMirrArr, TRUE, pUserSetInfo, eSaveMode, bExceptDummy, strLineArcWidth, dAngle, bMirror);	

	_EndDraw();


	BOOL nRet = _SetSplitImage_Halcon(m_pWicBitmap, nSplitIndexX, nSplitIndexY);

	//m_mtx.unlock();

	double dTime = clTimer.StopTimer(CTimerUnit_en::EN_MIL_SEC);
	strLog.Format(_T("\n%s : Split[R%d][C%d], Processing Time[%.2f ms]"), pLayerSet->strLayer, nSplitIndexX, nSplitIndexY, dTime);
	OutputDebugString(strLog);
	
	return nRet;
}

UINT32 CSaveLayer_AOI::_DrawRender_Split(ID2D1RenderTarget* pRender, IN const int &nSplitIndexX, IN const int &nSplitIndexY,
	IN CStep *pStep, IN const LayerSet_t* pLayerSet,
	CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo,
	IN const SaveMode eSaveMode, IN bool bExceptDummy, IN CString strLineArcWidth, double dAngle, bool bMirror)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pStep == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pDrawFunction == nullptr) return RESULT_BAD;
	
	int nLayerIndex = 0;//사용안함
	CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, pLayerSet->strLayer, nLayerIndex);
	if (pLayer == nullptr)
		return RESULT_BAD;

	if (m_ptSplitCount.x == 1 && m_ptSplitCount.y == 1)
	{
		_SetTransform(pRender, pUserSetInfo, nSplitIndexX, nSplitIndexY, dAngle, bMirror);
	}
	else
	{
		_SetTransform_Split(pRender, pUserSetInfo, nSplitIndexX, nSplitIndexY, dAngle, bMirror);
	}
	
	RECTD drtViewRect_mm = m_pDrawFunction->GetViewRect_mm(pRender, nullptr);
	drtViewRect_mm.NormalizeRectD();
	//현재 스텝 	

	UINT32 nRet = RESULT_GOOD;

	if (!bExceptDummy)
		nRet = m_pDrawFunction->OnMake_Layer_V2(pRender, pLayer, pLayerSet, pFontArr, pRotMirrArr, bNeedUpdate, pUserSetInfo, m_eDrawLevel, eSaveMode, drtViewRect_mm, strLineArcWidth);
	
	//서브 스텝
	//if (pUserSetInfo->bShowStepRepeat == true)
	{
		for (UINT i = 0; i < pStep->m_nSubStepCnt; i++)
		{
			CSubStep* pSubStep = pStep->m_arrSubStep[i];
			if (pSubStep == nullptr) continue;

			_DrawSubStep(pRender, pSubStep, pLayerSet, pFontArr, pRotMirrArr, TRUE, pUserSetInfo, eSaveMode, drtViewRect_mm, strLineArcWidth);
		}
	}

	return nRet;
}

UINT32 CSaveLayer_AOI::_DrawSubStep(ID2D1RenderTarget* pRender, IN CSubStep* pSubStep, IN const LayerSet_t* pLayerSet,
	CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate,
	IN const UserSetInfo_t *pUserSetInfo, IN const SaveMode eSaveMode, IN const RECTD &drtViewRect_mm, IN CString strLineArcWidth)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pSubStep == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;

	//현재 Draw 영역
	//D2D1::Matrix3x2F matTransform;
	//pRender->GetTransform(&matTransform);
	//D2D1::Matrix3x2F matScale = D2D1::Matrix3x2F(matTransform.m11, matTransform.m12, matTransform.m21, matTransform.m22, 0, 0);

	//float fScale = matTransform.m11;
	//if (fScale == 0.f)
	//{
	//	fScale = matTransform.m12;
	//}

	//D2D1_POINT_2F fptPan_mm = D2D1::Point2F(-1.0f*matTransform.dx / fScale, matTransform.dy / fScale);
	//
	//D2D1_POINT_2F fptSize = D2D1::Point2F(pRender->GetSize().width, pRender->GetSize().height);
	//D2D1_POINT_2F fptSize_mm = CDrawFunction::DetansformPoint(fptSize, matScale);
	//
	//RECTD drtViewRect_mm = RECTD(fptPan_mm.x, fptPan_mm.y, fptPan_mm.x + fptSize_mm.x, fptPan_mm.y + fptSize_mm.y);
	//drtViewRect_mm.NormalizeRectD();
	//

	CStepRepeat* pSubStepRepeat = &(pSubStep->m_arrStepRepeat);
	if (pSubStepRepeat == nullptr)
		return RESULT_BAD;

	CRotMirrArr *rotMirrArr = NULL;
	BOOL bCreateRotMirr = FALSE;
	if (pRotMirrArr) rotMirrArr = pRotMirrArr;
	else
	{
		bCreateRotMirr = TRUE;
		rotMirrArr = new CRotMirrArr;
		rotMirrArr->InputData(0.0, 0.0, Orient::NoMir0Deg);

	}

	enum Orient eSubOrient = Orient::NoMir0Deg;
	if ((pSubStepRepeat->m_dAngle == 0) && (pSubStepRepeat->m_bMirror == false))			eSubOrient = Orient::NoMir0Deg;
	else if ((pSubStepRepeat->m_dAngle == 90) && (pSubStepRepeat->m_bMirror == false))		eSubOrient = Orient::NoMir90Deg;
	else if ((pSubStepRepeat->m_dAngle == 180) && (pSubStepRepeat->m_bMirror == false))		eSubOrient = Orient::NoMir180Deg;
	else if ((pSubStepRepeat->m_dAngle == 270) && (pSubStepRepeat->m_bMirror == false))		eSubOrient = Orient::NoMir270Deg;
	else if ((pSubStepRepeat->m_dAngle == 0) && (pSubStepRepeat->m_bMirror == true))		eSubOrient = Orient::Mir0Deg;
	else if ((pSubStepRepeat->m_dAngle == 90) && (pSubStepRepeat->m_bMirror == true))		eSubOrient = Orient::Mir90Deg;
	else if ((pSubStepRepeat->m_dAngle == 180) && (pSubStepRepeat->m_bMirror == true))		eSubOrient = Orient::Mir180Deg;
	else if ((pSubStepRepeat->m_dAngle == 270) && (pSubStepRepeat->m_bMirror == true))		eSubOrient = Orient::Mir270Deg;
	else eSubOrient = Orient::NoMir0Deg;

	double		dRefx, dRefy;
	double		dParentX, dParentY;
	for (UINT32 y_step = 0; y_step < pSubStepRepeat->m_nNY; y_step++)
	{
		for (UINT32 x_step = 0; x_step < pSubStepRepeat->m_nNX; x_step++)
		{
			dRefx = pSubStep->m_dXDatum;
			dRefy = pSubStep->m_dYDatum;

			rotMirrArr->ConvertPoint(&dParentX, &dParentY, 0, 0, dRefx, dRefy, eSubOrient);
			dParentX = (pSubStepRepeat->m_dX - dParentX) + pSubStepRepeat->m_dDX * x_step;
			dParentY = (pSubStepRepeat->m_dY - dParentY) + pSubStepRepeat->m_dDY * y_step;

			rotMirrArr->InputData(dParentX, dParentY, eSubOrient);
			
			CStep *pStep = pSubStep->m_pSubStep;
			//if (pStep == nullptr) continue;
			if (pStep == nullptr) return RESULT_BAD;


			//Inrect Check Draw 시간을 줄이기 위함
			{
				

				RECTD drtFeature;
				if (pRotMirrArr == nullptr)
				{
					drtFeature = pStep->m_FeatureMinMax;//  pStep->m_Profile.m_MinMax;
				}
				else
				{
				//	pRotMirrArr->FinalPoint(&drtFeature.left, &drtFeature.top, pStep->m_Profile.m_MinMax.left, pStep->m_Profile.m_MinMax.top);
				//	pRotMirrArr->FinalPoint(&drtFeature.right, &drtFeature.bottom, pStep->m_Profile.m_MinMax.right, pStep->m_Profile.m_MinMax.bottom);
					pRotMirrArr->FinalPoint(&drtFeature.left, &drtFeature.top, pStep->m_FeatureMinMax.left, pStep->m_FeatureMinMax.top);
					pRotMirrArr->FinalPoint(&drtFeature.right, &drtFeature.bottom, pStep->m_FeatureMinMax.right, pStep->m_FeatureMinMax.bottom);
					drtFeature.NormalizeRectD();
				}

				if (drtFeature.IsIntersection(drtViewRect_mm) == FALSE)
				{
					rotMirrArr->RemoveTail();
					continue;
				}
			}

			int nLayerIndex;//사용안함
			CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, pLayerSet->strLayer, nLayerIndex);
			//if (pLayer == nullptr) continue;
			if (pLayer == nullptr) return RESULT_BAD;

			//Draw Layer
			m_pDrawFunction->OnMake_Layer_V2(pRender, pLayer, pLayerSet, pFontArr, pRotMirrArr, bNeedUpdate, pUserSetInfo, m_eDrawLevel, eSaveMode, drtViewRect_mm, strLineArcWidth);

			//SubSub
			for (UINT j = 0; j < pSubStep->m_nNextStepCnt; j++)
			{
				int nCurStripIndex = y_step * pSubStepRepeat->m_nNX + x_step;

				CSubStep* pSubSubStep = pSubStep->m_arrNextStep.GetAt(j);

				_DrawSubStep(pRender, pSubSubStep, pLayerSet, pFontArr, rotMirrArr, bNeedUpdate, pUserSetInfo, eSaveMode, drtViewRect_mm, strLineArcWidth);
			}

			rotMirrArr->RemoveTail();

		}
	}

	if (bCreateRotMirr == TRUE)
	{
		if (rotMirrArr != nullptr)
		{
			delete rotMirrArr;
			rotMirrArr = nullptr;
		}
	}

	return RESULT_GOOD;
}

UINT32	CSaveLayer_AOI::_DrawRender_Split_FilteredFeature(ID2D1RenderTarget* pRender, IN const int &nSplitIndexX, IN const int &nSplitIndexY,
	vector<CFeature *> vecFeature,
	IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pDrawFunction == nullptr) return RESULT_BAD;

// 	int nLayerIndex = 0;//사용안함
// 	CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, pLayerSet->strLayer, nLayerIndex);
// 	if (pLayer == nullptr)
// 		return RESULT_BAD;

	if (m_ptSplitCount.x == 1 && m_ptSplitCount.y == 1)
	{
		_SetTransform(pRender, pUserSetInfo, nSplitIndexX, nSplitIndexY);
	}
	else
	{
		_SetTransform_Split(pRender, pUserSetInfo, nSplitIndexX, nSplitIndexY);
	}

	RECTD drtViewRect_mm = m_pDrawFunction->GetViewRect_mm(pRender, nullptr);
	drtViewRect_mm.NormalizeRectD();

	//진행중
	m_pDrawFunction->OnMake_Layer_FilteredFeature(pRender, vecFeature, pLayerSet, pFontArr, pRotMirrArr, pUserSetInfo, RECTD(0, 0, 0, 0));

	return RESULT_GOOD;
}

void	CSaveLayer_AOI::_SetTransform(ID2D1RenderTarget* pRender, IN const UserSetInfo_t *pUserSetInfo,
	IN const int nSplitX, IN const int nSplitY, double dAngle, bool bMirror)
{
	D2D1_POINT_2F fptCenter;
	fptCenter.x = (m_frtPictureBox.left + (m_frtPictureBox.right - m_frtPictureBox.left) / 2.f);//rtClient.Width() / 2.f;
	fptCenter.y = (m_frtPictureBox.top + (m_frtPictureBox.bottom - m_frtPictureBox.top) / 2.f);
	//D2D1::Matrix3x2F matTrans = m_matScaleView.TransformPoint(fptCenter);

	D2D1_POINT_2F fptPan = D2D1::Point2F(0.f, 0.f);
	_SetViewScale(pUserSetInfo, m_fSplitScale, fptPan, dAngle, bMirror);

	D2D1::Matrix3x2F matViewScale = D2D1::Matrix3x2F(m_matDrawTransform.m11, m_matDrawTransform.m12, m_matDrawTransform.m21, m_matDrawTransform.m22, 0, 0);
	D2D1_POINT_2F fptCenterT = matViewScale.TransformPoint(fptCenter);

	fptPan.x = (-1.0f*fptCenterT.x) + m_sizeSplitImage.width / 2.f;
	fptPan.y = (-1.0f*fptCenterT.y) + m_sizeSplitImage.height / 2.f;

	_SetViewScale(pUserSetInfo, m_fSplitScale, fptPan, dAngle, bMirror);

	pRender->SetTransform(m_matDrawTransform);

}

void	CSaveLayer_AOI::_SetTransform_Split(ID2D1RenderTarget* pRender, IN const UserSetInfo_t *pUserSetInfo,
	IN const int nSplitX, IN const int nSplitY, double dAngle, bool bMirror)
{
	if (pRender == nullptr) return;
	if (pUserSetInfo == nullptr) return;

	D2D1_POINT_2F fptPan = D2D1::Point2F(0.f, 0.f);
	_SetViewScale(pUserSetInfo, m_fSplitScale, fptPan, dAngle, bMirror);

	D2D1::Matrix3x2F matViewScale = D2D1::Matrix3x2F(m_matDrawTransform.m11, m_matDrawTransform.m12, m_matDrawTransform.m21, m_matDrawTransform.m22, 0, 0);

	D2D1_RECT_F frtRect = CDrawFunction::NormalizeRect(m_frtPictureBox);
	D2D1_POINT_2F fptLT;
	fptLT.x = frtRect.left;
	fptLT.y = frtRect.top;

	D2D1_POINT_2F fptLT_T = matViewScale.TransformPoint(fptLT);

	fptPan.x = (-1 * fptLT_T.x)/*원점*/ - m_vecvecSplitPos[nSplitY][nSplitX].ptPos.x/*이동량*/;
	fptPan.y = (-1 * fptLT_T.y + pRender->GetSize().height)/*원점*/ + (m_vecvecSplitPos[nSplitY][nSplitX].ptPos.y)/*이동량*/;

	_SetViewScale(pUserSetInfo, m_fSplitScale, fptPan, dAngle, bMirror);

	pRender->SetTransform(m_matDrawTransform);
}

void CSaveLayer_AOI::_SetViewScale(IN const UserSetInfo_t *pUserSetInfo, IN const float &fScale, IN const D2D_POINT_2F &fptPan, double dAngle, bool bMirror)
{
// 	D2D1::Matrix3x2F matScale = D2D1::Matrix3x2F(fScale, 0, 0, fScale, 0, 0);
// 
// 	D2D1::Matrix3x2F matRotate = D2D1::Matrix3x2F::Rotation(static_cast<float>(pUserSetInfo->dAngle),
// 		CDrawFunction::GetRectCenter(m_frtPictureBox));
// 
// 	bool bIsMirror = pUserSetInfo->bMirror;
// 
// 	if (m_eDrawLevel == eDrawLevel::eUnitLv)
// 	{
// 		matRotate = D2D1::Matrix3x2F::Rotation(static_cast<float>(dAngle),
// 			CDrawFunction::GetRectCenter(m_frtPictureBox));
// 
// 		bIsMirror = bMirror;
// 	}
// 
// 	D2D1::Matrix3x2F matTotal = matScale *matRotate;
// 
// 	if (bIsMirror)
// 	{
// 		if (pUserSetInfo->mirrorDir == eDirectionType::eBoth)
// 		{
// 			m_matDrawTransform = D2D1::Matrix3x2F(matTotal.m11, matTotal.m12, matTotal.m21*1.0f, matTotal.m22*1.0f, fptPan.x, fptPan.y);
// 			m_matDrawTransform = D2D1::Matrix3x2F(m_matDrawTransform.m11 * -1.f, m_matDrawTransform.m12* -1.f, m_matDrawTransform.m21, m_matDrawTransform.m22, fptPan.x, fptPan.y);
// 		}
// 		else if (pUserSetInfo->mirrorDir == eDirectionType::eHorizontal)
// 		{
// 			m_matDrawTransform = D2D1::Matrix3x2F(matTotal.m11, matTotal.m12, matTotal.m21*1.0f, matTotal.m22*1.0f, fptPan.x, fptPan.y);
// 		}
// 		else if (pUserSetInfo->mirrorDir == eDirectionType::eVertical)
// 		{
// 			m_matDrawTransform = D2D1::Matrix3x2F(matTotal.m11 * -1.f, matTotal.m12* -1.f, matTotal.m21*-1.0f, matTotal.m22*-1.0f, fptPan.x, fptPan.y);
// 		}
// 		else
// 		{
// 			m_matDrawTransform = D2D1::Matrix3x2F(matTotal.m11, matTotal.m12, matTotal.m21*-1.0f, matTotal.m22*-1.0f, fptPan.x, fptPan.y);
// 		}
// 	}
// 	else
// 	{
// 		m_matDrawTransform = D2D1::Matrix3x2F(matTotal.m11, matTotal.m12, matTotal.m21*-1.0f, matTotal.m22*-1.0f, fptPan.x, fptPan.y);
// 	}

	D2D1::Matrix3x2F matScale = D2D1::Matrix3x2F(fScale, 0, 0, fScale, 0, 0);
	m_matDrawTransform = D2D1::Matrix3x2F(matScale.m11, matScale.m12, matScale.m21*-1.0f, matScale.m22*-1.0f, fptPan.x, fptPan.y);
}

UINT32 CSaveLayer_AOI::_SetSplitImage_Halcon(IN IWICBitmap *pWicColor, IN const int &nSplitIndexX, IN const int &nSplitIndexY)
{
	HRESULT hr;

	IWICBitmapSource *pWicGray;
	hr = WICConvertBitmapSource(GUID_WICPixelFormat8bppGray, pWicColor, &pWicGray);
	if (hr != S_OK)
	{
		return RESULT_BAD;
	}

	UINT uWidth, uHeight;
	pWicGray->GetSize(&uWidth, &uHeight);

	BYTE *pByte = new BYTE[uWidth*uHeight];

	WICRect rc;
	rc.X = 0;
	rc.Y = 0;
	rc.Width = uWidth;
	rc.Height = uHeight;

	hr = pWicGray->CopyPixels(&rc, uWidth, uWidth*uHeight, pByte);
	if (hr != S_OK)
	{
		return RESULT_BAD;
	}

	m_pHalconFunction->SetSplitImage(nSplitIndexX, nSplitIndexY, static_cast<int>(uWidth), static_cast<int>(uHeight), pByte);

	SafeRelease(&pWicGray);

	
	return RESULT_GOOD;
}


UINT32 CSaveLayer_AOI::_SetSaveImage_DirectX(IN const CString &strfilename)
{
	HRESULT hr;
	IWICBitmapSource *pWicGray;
	hr = WICConvertBitmapSource(GUID_WICPixelFormat8bppGray, m_pWicBitmap, &pWicGray);
	if (hr != S_OK)
	{
		return RESULT_BAD;
	}

	UINT32 iRet = RESULT_GOOD;
	iRet = _SaveImage(pWicGray, strfilename);

	SafeRelease(&pWicGray);

	return RESULT_GOOD;
}

UINT32 CSaveLayer_AOI::_SetSaveImage_Halcon(IN const CString &strfilename)
{
	HRESULT hr;
	IWICBitmapSource *pWicGray;
	hr = WICConvertBitmapSource(GUID_WICPixelFormat8bppGray, m_pWicBitmap, &pWicGray);
	if (hr != S_OK)
	{
		return RESULT_BAD;
	}

	UINT uWidth, uHeight;
	pWicGray->GetSize(&uWidth, &uHeight);

	BYTE *pByte = new BYTE[uWidth*uHeight];

	WICRect rc;
	rc.X = 0;
	rc.Y = 0;
	rc.Width = uWidth;
	rc.Height = uHeight;

	hr = pWicGray->CopyPixels(&rc, uWidth, uWidth*uHeight, pByte);
	if (hr != S_OK)
	{
		return RESULT_BAD;
	}

	fs::path fsPath;
	fsPath = std::string(CT2CA(strfilename));
	CString strExt = CA2CT(fsPath.extension().string().c_str());
	strExt = strExt.Mid(1);
	
	UINT32 iRet = RESULT_GOOD;
	iRet = m_pHalconFunction->SaveImage (strfilename, strExt, uWidth, uHeight, pByte);

	if (pByte != nullptr)
	{
		delete pByte;
		pByte = nullptr;
	}

	SafeRelease(&pWicGray);

	

	return RESULT_GOOD;
}

UINT32 CSaveLayer_AOI::_SetSaveImage(IN const CString &strfilename, IN const int &nSplitIndexX, IN const int &nSplitIndexY, IN BOOL bDelSrc, IN BOOL bSaveRaw)
{
	CString strFullPathName_New = GetSplitFileName(strfilename, nSplitIndexX, nSplitIndexY);
	if (strFullPathName_New.GetLength() < 1) return RESULT_BAD;

	UINT32 iRet = RESULT_GOOD;
	
// 	if (bUseHalcon == TRUE)
// 	{
// 		iRet = _SetSaveImage_Halcon(strFullPathName_New);
// 	}
// 	else
	{
		iRet = _SetSaveImage_DirectX(strFullPathName_New);
	}

	if (m_pHalconFunction && m_eDrawLevel == eDrawLevel::ePanelLv)
	{
		CString strDstPath = strFullPathName_New;

		fs::path fsPath;
		fsPath = std::string(CT2CA(strFullPathName_New));
		CString strExt = CA2CT(fsPath.extension().string().c_str());

		strDstPath.Replace(strExt, _T(".hobj"));
		iRet |= m_pHalconFunction->ConvRegion(strFullPathName_New, strDstPath);
				
		if (bDelSrc && iRet == RESULT_GOOD)
		{
#ifdef _UNICODE
			DeleteFileW(strFullPathName_New);
#else
			DeleteFileA(strFullPathName_New);
#endif
		}
	}

	if (m_pHalconFunction && m_eDrawLevel != eDrawLevel::ePanelLv)
	{

		if (bSaveRaw)
			iRet |= m_pHalconFunction->Save_As_Raw(strFullPathName_New, strFullPathName_New);

	}

	/*if (m_eDrawLevel == eDrawLevel::ePanelLv)
	{
		if (nSplitIndexY + 1 == m_ptSplitCount.y &&
			nSplitIndexX + 1 == m_ptSplitCount.x)
		{
			std::thread _TThread(_ConvertImage_1Bit, strFullPathName_New, &m_hFinishEvent);
			_TThread.detach();
		}
		else
		{
			std::thread _TThread(_ConvertImage_1Bit, strFullPathName_New, nullptr);
			_TThread.detach();
		}		
	}*/

	return iRet;
}

UINT32 CSaveLayer_AOI::_SetSaveImage_V2(IN const CString &strfilename, IN const int &nSplitIndexX, IN const int &nSplitIndexY, IN BOOL bSaveImg, IN BOOL bSaveRegion, IN BOOL bSaveRaw)
{
	CString strFullPathName_New = GetSplitFileName(strfilename, nSplitIndexX, nSplitIndexY);
	if (strFullPathName_New.GetLength() < 1) return RESULT_BAD;

	UINT32 iRet = RESULT_GOOD;

	HRESULT hr;
	IWICBitmapSource *pWicGray;
	hr = WICConvertBitmapSource(GUID_WICPixelFormat8bppGray, m_pWicBitmap, &pWicGray);
	if (hr != S_OK)
	{
		return RESULT_BAD;
	}

	if (bSaveImg == TRUE)
	{
		iRet = _SaveImage(pWicGray, strFullPathName_New);
	}

	if (m_pHalconFunction == nullptr) return RESULT_BAD;

	UINT uWidth, uHeight;
	pWicGray->GetSize(&uWidth, &uHeight);

	BYTE *pByte = new BYTE[uWidth*uHeight];

	WICRect rc;
	rc.X = 0;
	rc.Y = 0;
	rc.Width = uWidth;
	rc.Height = uHeight;

	hr = pWicGray->CopyPixels(&rc, uWidth, uWidth*uHeight, pByte);
	if (hr != S_OK)
	{
		return RESULT_BAD;
	}
	//

	if (bSaveRegion == TRUE)
	{
		m_pHalconFunction->ConvRegion(strFullPathName_New, uWidth, uHeight, pByte);
	}

	if (bSaveRaw == TRUE)
	{
		m_pHalconFunction->Save_As_Raw(strFullPathName_New, uWidth, uHeight, pByte);
	}

	//

	if (pByte != nullptr)
	{
		delete pByte;
		pByte = nullptr;
	}

	SafeRelease(&pWicGray);



	return RESULT_GOOD;
}

//UINT32 CSaveLayer_AOI::_ConvertImage_1Bit(IN const CString strfilename, IN CEvent* pEvent)
//{
//	if (strfilename.GetLength() < 1)
//		return RESULT_BAD;
//
//	//1 bit 변환 속도가 느려서 사용 불가
//   //Convert 24 bit to 1 bit
//	Mat matColor, matGray, matBinary;
//	String strLoad = String(CT2CA(strfilename));
//
//	matColor = imread(strLoad);
//	cvtColor(matColor, matGray, COLOR_BGR2GRAY);
//
//	threshold(matGray, matBinary, 150, 255, THRESH_BINARY);
//
//	vector<int> params;
//	params.push_back(IMWRITE_PNG_BILEVEL);
//	params.push_back(1);
//
//	CString strOutput = strfilename;
//
//	fs::path fsPath;
//	fsPath = std::string(CT2CA(strOutput));
//	CString strExt = CA2CT(fsPath.extension().string().c_str());
//
//	strOutput.Replace(strExt, _T(".png"));
//
//	strLoad = String(CT2CA(strOutput));
//	imwrite(strLoad, matBinary, params);
//
//#ifdef _UNICODE
//			DeleteFileW(strfilename);
//#else
//			DeleteFileA(strfilename);
//#endif		
//
//	if (pEvent)
//		pEvent->SetEvent();
//
//	return RESULT_GOOD;
//}

UINT32 CSaveLayer_AOI::_SetSaveRegion(IN const CString &strfilename, IN const int &nSplitIndexX, IN const int &nSplitIndexY)
{
	if (strfilename.GetLength() < 1)
		return RESULT_BAD;

	// Change File Name
	CString strFileName_New;
	fs::path fsPath;
	fsPath = std::string(CT2CA(strfilename));
	CString strExt = CA2CT(fsPath.extension().string().c_str());
	CString strPathName = CA2CT(fsPath.parent_path().string().c_str());
	CString strFileName = CA2CT(fsPath.filename().string().c_str());

	strFileName_New.Format(_T("%s.hobj"), strFileName.Mid(0, strFileName.GetLength() - strExt.GetLength()));

	//strPathName += _T("\\Temp\\");
	strPathName += _T("\\");
	CString strFullPathName_New = strPathName + strFileName_New;

	::SHCreateDirectoryEx(NULL, strPathName, NULL);

	if (m_pHalconFunction)
		m_pHalconFunction->SaveRegion(strFullPathName_New);
	else
		return RESULT_BAD;

	//Merge 를 위하여 Split Region path 저장
	m_vecvecSplitPos[nSplitIndexY][nSplitIndexX].strPathName_Region = strFullPathName_New;

	return RESULT_GOOD;
}


UINT32 CSaveLayer_AOI::_SaveMasterRegion(IN const CString &strFilename,
	IN CStep *pStep, IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
	CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo,
	IN const SaveMode eSaveMode, double dAngle, bool bMirror)
{
	UINT32 nRet = RESULT_GOOD;

	if (pStep == nullptr) return nRet;
	if (pLayerSet == nullptr) return nRet;

	try
	{
		SetSystem("clip_region", "false");
		SetSystem("width", static_cast<int>(m_sizeSplitImage.width + 0.5));
		SetSystem("height", static_cast<int>(m_sizeSplitImage.height + 0.5));
	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Error_SetSystem : %s\n"), strError);

		return RESULT_BAD;
	}

	
	for (int i = 0; i < m_ptSplitCount.y; i++)
	{
		for (int j = 0; j < m_ptSplitCount.x; j++)
		{
			if (m_pHalconFunction)
				m_pHalconFunction->ClearRegion();

			nRet |= _SaveRegion_Split(m_pImageRender, j, i, pStep, pLayerSet, pRotMirrArr, pUserSetInfo, eSaveMode, strFilename, dAngle, bMirror);
			if (nRet != RESULT_GOOD)
			{
				break;
			}
			else
			{//성공
				_SetSaveRegion(strFilename, j, i);
			}
		}
		if (nRet != RESULT_GOOD)
		{
			break;
		}
	}

	
// 	if (nRet == RESULT_GOOD)
// 	{//Region Merge
// 		if (m_pHalconFunction)
// 		{
// 			m_pHalconFunction->CreateMergeRegion(strFilename, m_vecvecSplitPos, m_nOverlapPixel,
// 				static_cast<int>(m_sizeTarget.width), static_cast<int>(m_sizeTarget.height));
// 		}
// 		else
// 			return RESULT_BAD;
// 	}
	

	return nRet;
}

UINT32	CSaveLayer_AOI::_SaveRegion_Split(ID2D1RenderTarget* pRender, IN const int &nSplitIndexX, IN const int &nSplitIndexY,
	IN CStep *pStep, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo,
	IN const SaveMode eSaveMode, IN const CString &strFilename, double dAngle, bool bMirror)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pStep == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;

	int nLayerIndex = 0;//사용안함
	CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, pLayerSet->strLayer, nLayerIndex);
	if (pLayer == nullptr) return RESULT_BAD;
	UINT32 nRet;

	int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());
		
	_BeginDraw();
	if (m_ptSplitCount.x == 1 && m_ptSplitCount.y == 1)
	{
		_SetTransform(pRender, pUserSetInfo, nSplitIndexX, nSplitIndexY, dAngle, bMirror);
	}
	else
	{
		_SetTransform_Split(pRender, pUserSetInfo, nSplitIndexX, nSplitIndexY, dAngle, bMirror);
	}

	RECTD drtViewRect = CDrawFunction::GetViewRect_mm(pRender, nullptr);
	drtViewRect.NormalizeRectD();
	for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
	{
		nRet = m_pMakeRegionFunction->OnMake_Feature(pLayer, drtViewRect, nfeno, pLayerSet, pRotMirrArr, pUserSetInfo, eSaveMode, m_dResolution);
	}
	
	
	//서브 스텝
	for (UINT i = 0; i < pStep->m_nSubStepCnt; i++)
	{
		CSubStep* pSubStep = pStep->m_arrSubStep[i];
		if (pSubStep == nullptr) continue;;

		_SaveRegion_Split_SubStep(drtViewRect, pSubStep, pLayerSet, pRotMirrArr, pUserSetInfo, eSaveMode);
	}

	_EndDraw();

	return RESULT_GOOD;
}

UINT32 CSaveLayer_AOI::_SaveRegion_Split_SubStep(RECTD drtImage_mm, IN CSubStep* pSubStep, IN const LayerSet_t* pLayerSet,
	CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo, IN const SaveMode eSaveMode)
{
	if (pSubStep == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;

	RECTD drtViewRect = drtImage_mm;
	drtViewRect.NormalizeRectD();

	CStepRepeat* pSubStepRepeat = &(pSubStep->m_arrStepRepeat);
	if (pSubStepRepeat == nullptr)
		return RESULT_BAD;

	CRotMirrArr *rotMirrArr = NULL;
	BOOL bCreateRotMirr = FALSE;
	if (pRotMirrArr) rotMirrArr = pRotMirrArr;
	else
	{
		bCreateRotMirr = TRUE;
		rotMirrArr = new CRotMirrArr;
		rotMirrArr->InputData(0.0, 0.0, Orient::NoMir0Deg);

	}

	enum Orient eSubOrient = Orient::NoMir0Deg;
	if ((pSubStepRepeat->m_dAngle == 0) && (pSubStepRepeat->m_bMirror == false))			eSubOrient = Orient::NoMir0Deg;
	else if ((pSubStepRepeat->m_dAngle == 90) && (pSubStepRepeat->m_bMirror == false))		eSubOrient = Orient::NoMir90Deg;
	else if ((pSubStepRepeat->m_dAngle == 180) && (pSubStepRepeat->m_bMirror == false))		eSubOrient = Orient::NoMir180Deg;
	else if ((pSubStepRepeat->m_dAngle == 270) && (pSubStepRepeat->m_bMirror == false))		eSubOrient = Orient::NoMir270Deg;
	else if ((pSubStepRepeat->m_dAngle == 0) && (pSubStepRepeat->m_bMirror == true))		eSubOrient = Orient::Mir0Deg;
	else if ((pSubStepRepeat->m_dAngle == 90) && (pSubStepRepeat->m_bMirror == true))		eSubOrient = Orient::Mir90Deg;
	else if ((pSubStepRepeat->m_dAngle == 180) && (pSubStepRepeat->m_bMirror == true))		eSubOrient = Orient::Mir180Deg;
	else if ((pSubStepRepeat->m_dAngle == 270) && (pSubStepRepeat->m_bMirror == true))		eSubOrient = Orient::Mir270Deg;
	else eSubOrient = Orient::NoMir0Deg;

	double		dRefx, dRefy;
	double		dParentX, dParentY;
	for (UINT32 y_step = 0; y_step < pSubStepRepeat->m_nNY; y_step++)
	{
		for (UINT32 x_step = 0; x_step < pSubStepRepeat->m_nNX; x_step++)
		{
			dRefx = pSubStep->m_dXDatum;
			dRefy = pSubStep->m_dYDatum;

			rotMirrArr->ConvertPoint(&dParentX, &dParentY, 0, 0, dRefx, dRefy, eSubOrient);
			dParentX = (pSubStepRepeat->m_dX - dParentX) + pSubStepRepeat->m_dDX * x_step;
			dParentY = (pSubStepRepeat->m_dY - dParentY) + pSubStepRepeat->m_dDY * y_step;

			rotMirrArr->InputData(dParentX, dParentY, eSubOrient);

			CStep *pStep = pSubStep->m_pSubStep;
			//if (pStep == nullptr) continue;
			if (pStep == nullptr)
			{
				rotMirrArr->RemoveTail();
				continue;
			}

			int nLayerIndex;//사용안함
			CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, pLayerSet->strLayer, nLayerIndex);
			//if (pLayer == nullptr) continue;
			if (pLayer == nullptr)
			{
				rotMirrArr->RemoveTail();
				continue;
			}

			//Inrect Check Draw 시간을 줄이기 위함
			{


				RECTD drtFeature;
				if (rotMirrArr == nullptr)
				{
					drtFeature = pStep->m_FeatureMinMax;//  pStep->m_Profile.m_MinMax;
				}
				else
				{
// 					pRotMirrArr->FinalPoint(&drtFeature.left, &drtFeature.top, pStep->m_Profile.m_MinMax.left, pStep->m_Profile.m_MinMax.top);
// 					pRotMirrArr->FinalPoint(&drtFeature.right, &drtFeature.bottom, pStep->m_Profile.m_MinMax.right, pStep->m_Profile.m_MinMax.bottom);

					rotMirrArr->FinalPoint(&drtFeature.left, &drtFeature.top, pStep->m_FeatureMinMax.left, pStep->m_FeatureMinMax.top);
					rotMirrArr->FinalPoint(&drtFeature.right, &drtFeature.bottom, pStep->m_FeatureMinMax.right, pStep->m_FeatureMinMax.bottom);
					drtFeature.NormalizeRectD();
				}

				if (drtFeature.IsIntersection(drtViewRect) == FALSE)
				{
					rotMirrArr->RemoveTail();
					continue;
				}
			}

			int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());
			for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
			{
				m_pMakeRegionFunction->OnMake_Feature(pLayer, drtViewRect, nfeno, pLayerSet, rotMirrArr, pUserSetInfo, eSaveMode, m_dResolution);
			}

			//SubSub
			for (UINT j = 0; j < pSubStep->m_nNextStepCnt; j++)
			{
				int nCurStripIndex = y_step * pSubStepRepeat->m_nNX + x_step;

				CSubStep* pSubSubStep = pSubStep->m_arrNextStep.GetAt(j);

				_SaveRegion_Split_SubStep(drtViewRect, pSubSubStep, pLayerSet, rotMirrArr, pUserSetInfo, eSaveMode);
			}

			rotMirrArr->RemoveTail();

		}
	}

	if (bCreateRotMirr == TRUE)
	{
		if (rotMirrArr != nullptr)
		{
			delete rotMirrArr;
			rotMirrArr = nullptr;
		}
	}

	return RESULT_GOOD;
}

CString CSaveLayer_AOI::GetSplitFileName(IN const CString &strfilename, IN const int &nSplitIndexX, IN const int &nSplitIndexY)
{
	if (strfilename.GetLength() < 1)
		return _T("");

	// Change File Name
	fs::path fsPath;
	fsPath = std::string(CT2CA(strfilename));
	CString strExt = CA2CT(fsPath.extension().string().c_str());
	CString strFileName = CA2CT(fsPath.filename().string().c_str());
	CString strPathName = CA2CT(fsPath.parent_path().string().c_str());
	CString strFileName_New, strFullPathName_New = _T("");

	::SHCreateDirectoryEx(NULL, strPathName, NULL);

	if (m_eDrawLevel == eDrawLevel::ePanelLv)
	{
		if (m_ptSplitCount.y == 1 && m_ptSplitCount.x == 1)
		{
			strFullPathName_New = strfilename;
		}
		else
		{
			strFileName_New.Format(_T("%s_R%d_C%d%s"), strFileName.Mid(0, strFileName.GetLength() - strExt.GetLength()), nSplitIndexY + 1, nSplitIndexX + 1, strExt);

			strFullPathName_New.Format(_T("%s\\%s"), strPathName, strFileName_New);
		}

	}
	else if (m_eDrawLevel == eDrawLevel::eSymbolLv)
	{
		strFullPathName_New = strfilename;
	}
	else if (m_eDrawLevel == eDrawLevel::eUnitLv)
	{
		strFullPathName_New = strfilename;
	}
	else if (m_eDrawLevel == eDrawLevel::eStripLv)
	{
		if (m_ptSplitCount.y == 1 && m_ptSplitCount.x == 1)
		{
			strFullPathName_New = strfilename;
		}
		else
		{
			strFileName_New.Format(_T("%s_R%d_C%d%s"), strFileName.Mid(0, strFileName.GetLength() - strExt.GetLength()), nSplitIndexY + 1, nSplitIndexX + 1, strExt);

			strFullPathName_New.Format(_T("%s\\%s"), strPathName, strFileName_New);
		}
	}

	if (strFullPathName_New.GetLength() < 1)
		return _T("");

	return strFullPathName_New;
}

UINT32 CSaveLayer_AOI::_CreateMergeImage(IN const CString &strfilename, IN BOOL bIsSaveRaw)
{
	if (m_pHalconFunction == nullptr) return RESULT_BAD;
	if (m_ptSplitCount.x <= 0 || m_ptSplitCount.y <= 0) return RESULT_BAD;

	for (int i = 0; i < m_ptSplitCount.y; i++)
	{
		for (int j = 0; j < m_ptSplitCount.x; j++)
		{
			CString strFullPathName_New = GetSplitFileName(strfilename, j, i);
			if (strFullPathName_New.GetLength() < 1)
				return RESULT_BAD;

			m_vecvecSplitPos[i][j].strPathName = strFullPathName_New;
		}
	}

	CString strOutput = strfilename;
	strOutput.Replace(_T("\\Cell"), _T("\\Swath"));

	if (RESULT_GOOD != m_pHalconFunction->CreateMergeImage(strOutput, m_vecvecSplitPos, m_nOverlapPixel,
		static_cast<int>(m_sizeTarget.width), static_cast<int>(m_sizeTarget.height), bIsSaveRaw))
	{
		return RESULT_BAD;
	}

	for (auto itY : m_vecvecSplitPos)
	{
		for (auto itX : itY)
		{			
#ifdef _UNICODE
			DeleteFileW(itX.strPathName);
#else
			DeleteFileA(itX.strPathName);
#endif	
		}
	}

	return RESULT_GOOD;
}

UINT32 CSaveLayer_AOI::CreateConcatRegion(IN vector<CString> &vecSrcRegion, CString  &strDstPathName, IN const BOOL &bSrcDelete)
{
	if(m_pHalconFunction == nullptr) return RESULT_BAD;

	return m_pHalconFunction->CreateConcatRegion(vecSrcRegion, strDstPathName, bSrcDelete);	
}


