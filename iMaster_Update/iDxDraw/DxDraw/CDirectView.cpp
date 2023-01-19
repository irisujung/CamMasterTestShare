#include "pch.h"
#include "CDirectView.h"

#include "../iDxDraw.h"
#include "CBackBuffer.h"
#include "../CDrawProfile.h"
#include "../CDrawLayer.h"
#include "../CDrawFunction.h"

#include "CSaveLayer_NSIS.h"
#include "CSaveLayer_AOI.h"

#include "../CDrawHighlightShowFunc.h"
#include "../CEditFunction.h"
#include "../../iUtilities/PerfTimerEx.h"
#include "../SaveLayerOperation.h"

#include "shlobj_core.h" //::SHCreateDirectoryEx

#include "../MakeRegion/CHalconFunction.h"

#include <cmath>

//IMPLEMENT_DYNAMIC(CDirectView, CDirect_Base)

CDirectView::CDirectView(void)
{
	if (m_pSaveLayerOp == nullptr)
	{
		m_pSaveLayerOp = new CSaveLayerOperation();
		if (m_pSaveLayerOp)
			m_pSaveLayerOp->StopAllThreads();
	}
}

CDirectView::~CDirectView(void)
{
	_ClearDrawLayer();
	
	if (m_pSaveLayer != nullptr)
	{
		delete m_pSaveLayer;
		m_pSaveLayer = nullptr;
	}

	if (m_pDrawFunction != nullptr)
	{
		delete m_pDrawFunction;
		m_pDrawFunction = nullptr;
	}

	if (m_pSaveLayerOp)
	{
		delete (CSaveLayerOperation*)m_pSaveLayerOp;
		m_pSaveLayerOp = nullptr;
	}

	if (m_pImageFactory)
	{
		m_pImageFactory->Release();
		m_pImageFactory = nullptr;
	}

}

void	CDirectView::SetLink(IN CDrawManager* pDrawManager)
{
	m_pDrawManager = pDrawManager;
}

UINT32 CDirectView::SetSystemSpec(SystemSpec_t* pSystemSpec)
{
	m_pSystemSpec = pSystemSpec;

	return RESULT_GOOD;

}
UINT32 CDirectView::SetUserSetInfo(UserSetInfo_t *pUserSetInfo, UserLayerSet_t *pUserLayerSetInfo)
{
	m_pUserSetInfo = pUserSetInfo;
	m_pUserLayerSetInfo = pUserLayerSetInfo;
	
	//_OnUpdate();
	
	return RESULT_GOOD;

}

UINT32 CDirectView::SetDrawOnly_MeasureUnit(IN const BOOL &bMeaureUnit)
{
	m_bMeasureUnitOnly = bMeaureUnit;

	m_vecMeasureUnitInfo.clear();

	if (m_bMeasureUnitOnly == TRUE)
	{
		if (RESULT_GOOD != _SetMeasureUnitInfo(m_vecMeasureUnitInfo))
			return RESULT_BAD;
	}

	int nSize = static_cast<int>(m_vecDrawLayer.size());
	for (int i = 0; i < nSize; i++)
	{
		m_vecDrawLayer[i]->SetDrawOnly_MeasureUnit(bMeaureUnit, m_vecMeasureUnitInfo);
	}


	return RESULT_GOOD;
}

UINT32 CDirectView::Initialize(IN ID2D1Factory1 *pFactory, IN ID2D1DeviceContext *pD2DContext, vector<ID2D1BitmapRenderTarget*> &vecRender,
	vector<ID2D1BitmapRenderTarget*> &vecRender_SubStep,
	IN CJobFile* pCAMData, IN ViewInfo_t* pViewInfo, IN PanelInfo_t* pstPanelInfo, IN FovInfo_t* pFovInfo, IN CellInfo_t* pCellInfo)
{
	m_pFactory = pFactory;
	m_pD2DContext = pD2DContext;
	m_pPanelInfo = pstPanelInfo;
	m_pFovInfo = pFovInfo;
	m_pCellInfo = pCellInfo;

	m_vecRender = vecRender;
	m_vecRender_SubStep = vecRender_SubStep;

	m_pJobData = pCAMData;
	m_pViewInfo = pViewInfo;

	if (m_pImageFactory == nullptr)
	{
		CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory2),
			reinterpret_cast<void**>(&m_pImageFactory));
	}
	
	_OnInit();

	return RESULT_GOOD;

}

UINT32 CDirectView::SetRenderPtr(vector<ID2D1BitmapRenderTarget*> &vecRender, vector<ID2D1BitmapRenderTarget*> &vecRender_SubStep)
{
	m_vecRender = vecRender;
	m_vecRender_SubStep = vecRender_SubStep;



	return RESULT_GOOD;
}

UINT32 CDirectView::ResetView()
{
	int nSize = static_cast<int>(m_vecDrawLayer.size());
	for (int i = 0; i < nSize; i++)
	{
		m_vecDrawLayer[i]->ResetView();
	}

	SetStepIndex(-1);

	return RESULT_GOOD;
}

UINT32 CDirectView::SetStepIndex(int nStepIndex)
{
	m_eDrawLv = eDrawLevel::DrawLevelNone;

	if (m_pJobData == nullptr) return RESULT_BAD;

	if (m_nSelectStep != nStepIndex)
	{
		_ClearGeo();
		m_nSelectStep = nStepIndex;

		m_bNeedUpdate = TRUE;
	}

	if (m_pUserLayerSetInfo)
	{
		if (m_nSelectStep == m_pUserLayerSetInfo->nPanelStepIdx)
			m_eDrawLv = eDrawLevel::ePanelLv;
		else
		{
			for (auto it : m_pUserLayerSetInfo->vcStripStepIdx)
			{
				if (m_nSelectStep == it)
				{
					m_eDrawLv = eDrawLevel::eStripLv;
					break;
				}
			}

			for (auto it : m_pUserLayerSetInfo->vcUnitStepIdx)
			{
				if (m_nSelectStep == it)
				{
					m_eDrawLv = eDrawLevel::eUnitLv;
					break;
				}
			}
		}
	}	
	return RESULT_GOOD;
}

void CDirectView::_OnInit()
{
	int nRenderSize = static_cast<int>(m_vecRender.size());
	m_vecDrawLayer.resize(nRenderSize);
	for (int i = 0; i < nRenderSize; i++)
	{
		m_vecDrawLayer[i] = new CDrawLayer();
		m_vecDrawLayer[i]->SetLink(m_pFactory, m_pD2DContext, m_pSystemSpec);
	}
	
	if (m_pSaveLayer == nullptr )
	{
		if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS)
		{
			m_pSaveLayer = new CSaveLayer_NSIS();
		}
		else if (m_pSystemSpec->sysBasic.McType == eMachineType::eAOI)
		{
			m_pSaveLayer = new CSaveLayer_AOI();
		}
		else if (m_pSystemSpec->sysBasic.McType == eMachineType::eAVI)
		{
			m_pSaveLayer = new CSaveLayer_AOI();
		}
		else
		{
			m_pSaveLayer = nullptr;
		}
		if (m_pSaveLayer != nullptr)
		{
			m_pSaveLayer->SetLink(m_pImageFactory, m_pFactory, m_pD2DContext);
		}
		
	}

	if (m_pDrawFunction == nullptr)
	{
		m_pDrawFunction = new CDrawFunction();
		m_pDrawFunction->_SetLink(m_pFactory, m_pD2DContext);

	}

}

void CDirectView::OnRender()
{
	if (m_nSelectStep == -1 || m_pSystemSpec == nullptr ||
		m_pUserSetInfo == nullptr) return;


	int nMaxRender = static_cast<int>(m_vecRender.size());

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr) return;

	int nLayerSetCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());
	int nDrawLayerCount = 0;
	int nDrawLayerCount_Sub = 0;

	BOOL bEditMode = FALSE;
	UINT32 nEditMode = m_pDrawManager->GetEditMode();
	if (nEditMode >= EditMode::enumMode_EditMode && nEditMode < EditMode::enumMode_Max) bEditMode = TRUE;

	bool bIsAlignMaskMode = false;
	//if (m_pSystemSpec->sysBasic.McType == eMachineType::eAOI)
	{
		if (nEditMode == EditMode::enumMode_EditMask )
			bIsAlignMaskMode = true;

		if (m_pUserSetInfo->bShowUnitAlign || m_pUserSetInfo->bShowInspMask)
			bIsAlignMaskMode = true;
	}

	for (int i = 0; i < nLayerSetCount; i++)
	{
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];

		//[TBD] Ribbon Toolbar Option 추가 예정 : Pattern Drawing in Panel Dummy
		/*if (m_pUserSetInfo->bShowPanelDummy)
		{
			if ((m_eDrawLv != eDrawLevel::eUnitLv && pLayerSet->strDefine.CompareNoCase(MEASURE_LAYER_TXT) != 0) ||
				(m_eDrawLv != eDrawLevel::eUnitLv && pLayerSet->strDefine.CompareNoCase(THICKNESS_LAYER_TXT) != 0))
				continue;
		}*/		

		if (pLayerSet->bCheck == true)
		{
			if (m_pSystemSpec->sysBasic.McType == eMachineType::eAOI)
			{
				//Align or Mask Mode
				if (bIsAlignMaskMode && pLayerSet->strDefine.CompareNoCase(PATTERN_LAYER_TXT) != 0)
				{
					continue;
				}
			}

			if (nDrawLayerCount < static_cast<int>(m_vecRender.size()) &&
				nDrawLayerCount < static_cast<int>(m_vecDrawLayer.size()))
			{
				m_vecDrawLayer[nDrawLayerCount]->OnRender(m_vecRender[nDrawLayerCount], m_pJobData, m_nSelectStep,
					pLayerSet, m_bNeedUpdate, m_pUserSetInfo, bEditMode);
				nDrawLayerCount++;
			}	
		}
		
	}	

	BOOL bMeasureUnitOnly = m_bMeasureUnitOnly;
	if (m_eDrawLv == eDrawLevel::eStripLv)
		bMeasureUnitOnly = FALSE;
	
	if (m_bMeasureUnitOnly || m_pUserSetInfo->bShowStepRepeat == true)
 	{
// 		if (!m_bMeasureUnitOnly) || 
// 			!(m_pUserSetInfo->rcpMode == eRecipeMode::eRecipe_Align_Panel))
		{
			for (int i = 0; i < nLayerSetCount; i++)
			{
				LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];

				if (pLayerSet->bCheck == true)
				{
					//Align or Mask Mode
					if (bIsAlignMaskMode && pLayerSet->strDefine.CompareNoCase(PATTERN_LAYER_TXT) != 0)
					{
						continue;
					}

					if (nDrawLayerCount < static_cast<int>(m_vecRender_SubStep.size()) &&
						nDrawLayerCount < static_cast<int>(m_vecDrawLayer.size()))
					{
						if (m_vecRender_SubStep[nDrawLayerCount] == nullptr) continue;

						m_vecDrawLayer[nDrawLayerCount]->OnRender_SubStep(m_vecRender_SubStep[nDrawLayerCount], m_pJobData, m_nSelectStep,
							pLayerSet, m_bNeedUpdate, m_pUserSetInfo, bEditMode);
						nDrawLayerCount++;
					}
				}
			}
		}
	}

	//사용하지 않는 Drawlayer 내부 요소 삭제
	int nCount = static_cast<int>(m_vecDrawLayer.size());
	for (int i = nDrawLayerCount; i < nCount; i++)
	{
		m_vecDrawLayer[i]->ResetView();
	}

	//한번 그리기가 되었다면 Need 업데이트 변수 초기화.
	m_bNeedUpdate = FALSE;
}

UINT32 CDirectView::SaveLayer_Test(IN const double &dResolution, IN const CString &strPathName)
{
	if (m_nSelectStep == -1) return RESULT_BAD;
	if (m_pSaveLayer == nullptr) return RESULT_BAD;
	if (m_pSystemSpec == nullptr) return RESULT_BAD;

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr) return RESULT_BAD;

	if (m_pUserLayerSetInfo->vcUnitStepIdx.size() <= 0) return RESULT_BAD;

// 	if (m_nSelectStep != m_pUserLayerSetInfo->vcUnitStepIdx[0])
// 	{
// 		return RESULT_BAD;
// 	}

	CRotMirrArr *rotMirrArr = new CRotMirrArr;
	rotMirrArr->InputData(0.0, 0.0, Orient::NoMir0Deg);

	D2D1::Matrix3x2F matRotate = D2D1::Matrix3x2F::Rotation(static_cast<float>(m_pUserSetInfo->dAngle),
		CDrawFunction::GetRectCenter(m_frtPictureBox));

	D2D1_RECT_F frtTempBox = CDrawFunction::DetansformRect(m_frtPictureBox, matRotate, nullptr);

	frtTempBox = CDrawFunction::NormalizeRect(frtTempBox);

	//간단하게 Swath 구성해보자.
	//Temp : Swath Info
	vector<D2D1_RECT_F> vecSwath_Temp;
	double dWidth_mm = m_pSystemSpec->sysCamera[1/*LineScaen*/].iFrameWidth * dResolution;
	int nSwatchCount = static_cast<int>((frtTempBox.right - frtTempBox.left) / dWidth_mm) + 1;
	vecSwath_Temp.resize(nSwatchCount);
	for (int i = 0; i < nSwatchCount; i++)
	{
		vecSwath_Temp[i].left = static_cast<float>(frtTempBox.left + (dWidth_mm * i));
		vecSwath_Temp[i].right = static_cast<float>(vecSwath_Temp[i].left + dWidth_mm);
		vecSwath_Temp[i].top = frtTempBox.top;
		vecSwath_Temp[i].bottom = frtTempBox.bottom;
	}

	UINT32 nRet;
	//for (int nSwath = 0; nSwath < nSwatchCount; nSwath++)
	for (int nSwath = 0; nSwath < 1; nSwath++)
	{
		nRet = m_pSaveLayer->SetPictureBox(vecSwath_Temp[nSwath], dResolution);

		if (nRet != RESULT_GOOD)
		{
			AfxMessageBox(_T("Render 생성 실패"));

			if (rotMirrArr != nullptr) delete rotMirrArr;

			return RESULT_BAD;
		}

		int nLayerSetCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());
		int nDrawLayerCount = 0;

		for (int i = 0; i < nLayerSetCount; i++)
		{
			LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];

			if (pLayerSet->bCheck == true)
			{
				//한개의 Layer씩 저장

				{
					//for (int nSaveMode = 0; nSaveMode < static_cast<int>(SaveMode::SaveMode_Max) ; nSaveMode++)
					for (int nSaveMode = static_cast<int>(SaveMode::SaveMode_Arc); nSaveMode < static_cast<int>(SaveMode::SaveMode_Arc)+1; nSaveMode++)
					{
						SaveMode eSaveMode = static_cast<SaveMode>(nSaveMode);
						CString strFullPathName;
						strFullPathName.Format(_T("%s%s_%d%s.%s"), strPathName, pLayerSet->strLayer, nSwath, SaveMode_Name::strName[nSaveMode], m_pSystemSpec->sysBasic.strImageType);

						m_pSaveLayer->SaveImage(strFullPathName, 0., pStep, pLayerSet, &m_pJobData->m_arrFont, rotMirrArr, m_pUserSetInfo, TRUE, eSaveMode);
					}



				}
			}
		}
	}
	


	

	if (rotMirrArr != nullptr) delete rotMirrArr;

	return RESULT_GOOD;
}

UINT32 CDirectView::OnLButtonDown(UINT nFlags, D2D1_POINT_2F fptPoint)
{

	D2D_POINT_2F fptCurPoint;
	fptCurPoint.x = static_cast<float>(fptPoint.x);
	fptCurPoint.y = static_cast<float>(fptPoint.y);

	UINT32 nEditMode = m_pDrawManager->GetEditMode();
	if (nEditMode == EditMode::enumMode_EditMode ||
		nEditMode == EditMode::enumMode_EditTP ||
		nEditMode == EditMode::enumMode_EditTP_Center)
	{
		if (_SetHighlight_Geo(fptCurPoint, FALSE) == FALSE)
		{//선택되지 않았다면, 전체 클리어
			ResetSelect();		
		}		
	}
	else if (nEditMode == EditMode::enumMode_EditAdd)
	{
		ResetSelect();
		SELECT_INFO_UNIT stSelectInfo_Unit;

		// 2022.08.09 KJH ADD
		// Auto Align 위해 _GetFeatureInfo 계산을 mm 값으로 변경 했기 때문에 동일하게 mm 값으로 변경
		D2D1_POINT_2F fptCurPoint_mm = CDrawFunction::DetansformPoint(fptCurPoint, m_pDrawManager->GetViewScale(), m_pUserSetInfo);

		UINT32 nRet = _GetFeatureInfo(fptCurPoint_mm, fptCurPoint, stSelectInfo_Unit);

		if (nRet == RESULT_GOOD)
		{
			m_stSelectInfo.vecSelectInfoUnit.push_back(stSelectInfo_Unit);

			// 2022.10.26 KJH ADD
			// Save Image Pixel Size 계산 Alert 위해 추가 
			/*
			RECTD rcTempRect;
			double fAlignLens, fAlignZoom;
			fAlignLens = m_pSystemSpec->sysNSIS.sysAlignLens;
			fAlignZoom = m_pSystemSpec->sysNSIS.sysAlignZoom;
			SIZE szFov = GetFOVSize(0, fAlignLens, fAlignZoom);
			double dResolMM = (double)szFov.cx / 1000. / (double)NSIS_FOV_X; // mm/Pixel 단위

			double dRatio = m_pSystemSpec->sysNSIS.sysAlignRatio;
			dRatio /= 100.0;
			dRatio /= 2.0;

			// 정사각형 형태로 자르므로 Size 통일
			if (stSelectInfo_Unit.dWidth > stSelectInfo_Unit.dHeight)
			{
				rcTempRect.left = stSelectInfo_Unit.dCx - (stSelectInfo_Unit.dWidth / 2.);
				rcTempRect.top = stSelectInfo_Unit.dCy + (stSelectInfo_Unit.dWidth / 2.);
				rcTempRect.right = stSelectInfo_Unit.dCx + (stSelectInfo_Unit.dWidth / 2.);
				rcTempRect.bottom = stSelectInfo_Unit.dCy - (stSelectInfo_Unit.dWidth / 2.);
			}
			else
			{
				rcTempRect.left = stSelectInfo_Unit.dCx - (stSelectInfo_Unit.dHeight / 2.);
				rcTempRect.top = stSelectInfo_Unit.dCy + (stSelectInfo_Unit.dHeight / 2.);
				rcTempRect.right = stSelectInfo_Unit.dCx + (stSelectInfo_Unit.dHeight / 2.);
				rcTempRect.bottom = stSelectInfo_Unit.dCy - (stSelectInfo_Unit.dHeight / 2.);
			}

			// FOV X 방향 5% 여유공간 가져감
			rcTempRect.left -= (szFov.cx / 1000. * dRatio);
			rcTempRect.top += (szFov.cx / 1000. * dRatio);
			rcTempRect.right += (szFov.cx / 1000. * dRatio);
			rcTempRect.bottom -= (szFov.cx / 1000. * dRatio);


			double width_pixel = stSelectInfo_Unit.dWidth / dResolMM + 0.5;
			double height_pixel = stSelectInfo_Unit.dHeight / dResolMM + 0.5;

			if (width_pixel > m_pSystemSpec->sysBasic.dSaveImageWidth || height_pixel > m_pSystemSpec->sysBasic.dSaveImageHeight)
			{
				CString strTemp;
				strTemp.Format(L"Select Feature의 Size Width :%4d, Height :%4d 가\n기준 Size Width :%4d, Height :%4d 보다 큽니다.", width_pixel, height_pixel, static_cast<int>(m_pSystemSpec->sysBasic.dSaveImageWidth), static_cast<int>(m_pSystemSpec->sysBasic.dSaveImageHeight));
				AfxMessageBox(strTemp);
			}
			// KJH END
			*/

		}
	}
	else if (nEditMode == EditMode::enumMode_EditMask)
		
		/*
		m_pDrawManager->GetMaskSelectMode() != MaskType::enumType_None &&
		(m_pDrawManager->GetEditMode() == EditMode::enumMode_EditMask_Resize ||
			m_pDrawManager->GetEditMode() == EditMode::enumMode_EditMask_Move ||
			m_pDrawManager->GetEditMode() == EditMode::enumMode_EditMask_Delete))
			*/
	{

		if (_SetHighlight_MaskGeo(m_pDrawManager->GetMaskSelectMode(), fptCurPoint, FALSE) == FALSE)
		{//선택되지 않았다면, 전체 클리어
			ResetSelect();
		}
	}
	else if (nEditMode == EditMode::enumMode_EditMask_Add_FeatureBase)
	{
		// 2022.08.09 KJH ADD
		// Auto Align 위해 _GetFeatureInfo 계산을 mm 값으로 변경 했기 때문에 동일하게 mm 값으로 변경
		D2D1_POINT_2F fptCurPoint_mm = CDrawFunction::DetansformPoint(fptCurPoint, m_pDrawManager->GetViewScale(), m_pUserSetInfo);

		if (_SetHighlight_MaskGeo(m_pDrawManager->GetMaskSelectMode(), fptCurPoint, FALSE) == FALSE)
		{//선택되지 않았다면, 전체 클리어
			ResetSelect();
		}

		//ResetSelect();
		SELECT_INFO_UNIT stSelectInfo_Unit;

		UINT32 nRet = _GetFeatureInfo(fptCurPoint_mm, fptCurPoint, stSelectInfo_Unit);

		if (nRet == RESULT_GOOD)
		{
			m_stSelectInfo.vecSelectInfoUnit.push_back(stSelectInfo_Unit);			
		}				
	}
	   
	return RESULT_GOOD;
}


UINT32 CDirectView::OnLButtonUp(UINT nFlags, CPoint point)
{

	//if (m_pDrawManager->GetEditMode() == EditMode::enumMode_EditMode)
	//{

	//	D2D_POINT_2F fptCurPoint;
	//	fptCurPoint.x = static_cast<float>(point.x);
	//	fptCurPoint.y = static_cast<float>(point.y);

	//	if (_SetHighlight_Geo(fptCurPoint, FALSE) == FALSE)
	//	{//선택되지 않았다면, 전체 클리어
	//		ResetSelect();
	//	}
	//}

	return RESULT_GOOD;
}


UINT32 CDirectView::OnMouseMove(UINT nFlags, CPoint point)
{
	ClearPreSelect();

	D2D_POINT_2F fptCurPoint;
	fptCurPoint.x = static_cast<float>(point.x);
	fptCurPoint.y = static_cast<float>(point.y); 

	BOOL bRet = FALSE;

	if (m_pDrawManager->GetEditMode() == EditMode::enumMode_EditMode)
	{
		bRet = _SetHighlight_Geo(fptCurPoint, TRUE);
	}

// 	if (m_pDrawManager->GetEditMode() == EditMode::enumMode_EditMode)
// 	{
// 		_SetHighlight_Geo(fptCurPoint, TRUE);
// 	}

	if (m_pDrawManager->GetEditMode() == EditMode::enumMode_EditTP ||
		m_pDrawManager->GetEditMode() == EditMode::enumMode_EditTP_Center)
	{
		bRet = _SetHighlight_Geo(fptCurPoint, TRUE);
	}

	else if (m_pDrawManager->GetMaskSelectMode() != MaskType::enumType_None &&
		m_pDrawManager->GetEditMode() == EditMode::enumMode_EditMask || m_pDrawManager->GetEditMode() == EditMode::enumMode_EditMask_Add_FeatureBase)
	{
		bRet = _SetHighlight_MaskGeo(m_pDrawManager->GetMaskSelectMode(), fptCurPoint, TRUE);		
	}

	/*return RESULT_GOOD;*/
	return bRet;
}

UINT32 CDirectView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	

	//CDirect_Base::Render();

	return RESULT_GOOD;
}


UINT32 CDirectView::SetPictureBox(IN const D2D1_RECT_F & frtPictureBox)
{
	m_frtPictureBox = frtPictureBox;

	return RESULT_GOOD;
}

CString			CDirectView::GetSelectInfoString()
{
	return m_stSelectInfo.GetDataString();
}

UINT32			CDirectView::SetRedrawGeo()
{
	m_bNeedUpdate = TRUE;

	return RESULT_GOOD;
}

UINT32 CDirectView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	return RESULT_GOOD;
}

UINT32 CDirectView::OnRButtonDown(UINT nFlags, CPoint point)
{
	

	return RESULT_GOOD;
}

UINT32 CDirectView::OnRButtonUp(UINT nFlags, CPoint point)
{
	

	return RESULT_GOOD;
}

//Feature Select
UINT32			CDirectView::ResetSelect()
{
	if (m_pJobData == nullptr) return RESULT_BAD;

	UINT32 nRet = CDrawHighlightShowFunc::ResetSelect(m_pJobData);
	
	

	m_stSelectInfo._Reset();

	if ( m_nSelectStep == -1) return RESULT_BAD;
	
	return nRet;
}



UINT32			CDirectView::SetSelect(vector<FEATURE_INFO> &vecFeatureInfo)
{
	if (m_pJobData == nullptr) return RESULT_BAD;
	if (m_nSelectStep < 0) return RESULT_BAD;
	if (m_nSelectStep >= static_cast<int>(m_pJobData->m_arrStep.GetCount())) return RESULT_BAD;
	if (static_cast<int>(vecFeatureInfo.size()) <= 0) return RESULT_BAD;

	UINT32 nRet = CDrawHighlightShowFunc::SetSelect(m_pJobData, m_nSelectStep, vecFeatureInfo, m_stSelectInfo);

	UpdateSelectInfo();

	return nRet;
}

UINT32			CDirectView::SetUnSelect(vector<FEATURE_INFO> &vecFeatureInfo)
{
	if (m_pJobData == nullptr) return RESULT_BAD;
	if (m_nSelectStep < 0) return RESULT_BAD;
	if (m_nSelectStep >= static_cast<int>(m_pJobData->m_arrStep.GetCount())) return RESULT_BAD;
	if (static_cast<int>(vecFeatureInfo.size()) <= 0) return RESULT_BAD;

	UINT32 nRet = CDrawHighlightShowFunc::SetUnSelect(m_pJobData, m_nSelectStep, vecFeatureInfo, m_stSelectInfo);

	UpdateSelectInfo();
	
	return nRet;
}

UINT32			CDirectView::UpdateSelectInfo()
{
	if (m_pJobData == nullptr) return RESULT_BAD;
	if (m_nSelectStep < 0) return RESULT_BAD;
	if (m_nSelectStep >= static_cast<int>(m_pJobData->m_arrStep.GetCount())) return RESULT_BAD;
	if (m_pUserLayerSetInfo == nullptr ) return RESULT_BAD;

	//Align Feature Base로 추가시 선택된 정보를 업데이트하는 것을 막는다.
	if (m_pDrawManager->GetEditMode() == EditMode::enumMode_EditMask_Add_FeatureBase) return RESULT_GOOD;

	CDrawHighlightShowFunc::UpdateSelectInfo(m_pJobData, m_nSelectStep, m_pUserLayerSetInfo, m_stSelectInfo);

	return RESULT_GOOD;
}

//Feature Show/Hide
UINT32			CDirectView::ResetHide()
{
	if (m_pJobData == nullptr) return RESULT_BAD;

	UINT32 nRet = CDrawHighlightShowFunc::ResetHide(m_pJobData);
	
	return nRet;
}

UINT32			CDirectView::SetShow(vector<FEATURE_INFO> &vecFeatureInfo)
{
	if (m_pJobData == nullptr) return RESULT_BAD;
	if (m_nSelectStep < 0) return RESULT_BAD;
	if (m_nSelectStep >= static_cast<int>(m_pJobData->m_arrStep.GetCount())) return RESULT_BAD;
	if (static_cast<int>(vecFeatureInfo.size()) <= 0) return RESULT_BAD;

	UINT32 nRet = CDrawHighlightShowFunc::SetShow(m_pJobData, m_nSelectStep, vecFeatureInfo);
	
	return nRet;
}

UINT32			CDirectView::SetHide(vector<FEATURE_INFO> &vecFeatureInfo)
{
	if (m_pJobData == nullptr) return RESULT_BAD;
	if (m_nSelectStep < 0) return RESULT_BAD;
	if (m_nSelectStep >= static_cast<int>(m_pJobData->m_arrStep.GetCount())) return RESULT_BAD;
	if (static_cast<int>(vecFeatureInfo.size()) <= 0) return RESULT_BAD;

	UINT32 nRet = CDrawHighlightShowFunc::SetHide(m_pJobData, m_nSelectStep, vecFeatureInfo);
	
	return nRet;
}

UINT32			CDirectView::SetDelete()
{//현재 선택된 Feature Delete

	UINT32 nRet = CEditFunction::SetDelete(m_pJobData, m_nSelectStep, m_pUserLayerSetInfo);

	return nRet;
}

void CDirectView::_ClearDrawLayer()
{
	int nSize = static_cast<int>(m_vecDrawLayer.size());
	for (int i = 0; i < nSize; i++)
	{
		if (m_vecDrawLayer[i] != nullptr)
		{
			delete m_vecDrawLayer[i];
			m_vecDrawLayer[i] = nullptr;
		}
	}

	m_vecDrawLayer.clear();
	
}

void CDirectView::_InitDrawLayer()
{
	_ClearDrawLayer();

	
}


BOOL CDirectView::_SetHighlight_Geo(IN D2D_POINT_2F fptPoint, IN const BOOL &bPreHighLight)
{
	if (m_pDrawFunction == nullptr) return FALSE;
	if (m_nSelectStep == -1) return FALSE;
	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr) return FALSE;

	CSymbol *pSymbol = nullptr;

	int nLayerSetCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());
	int nDrawLayerCount = 0;
	BOOL bSelect = FALSE;
	for (int i = 0; i < nLayerSetCount; i++)
	{
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];

		if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS)
		{
			UINT32 nEditMode = m_pDrawManager->GetEditMode();
			if (nEditMode == EditMode::enumMode_EditMask_Add_FeatureBase || nEditMode == EditMode::enumMode_EditAdd)
			{//NSIS 모드 이지만, Feature Base의 Align Add 일경우 Pattern의 선택을 기준으로 해야함
				if (pLayerSet->strDefine != PATTERN_LAYER_TXT )
				{
					continue;
				}

			}
			else if (nEditMode == EditMode::enumMode_EditTP || nEditMode == EditMode::enumMode_EditTP_Center )
			{
				if (pLayerSet->strDefine != THICKNESS_LAYER_TXT)
				{
					continue;
				}
			}

			else
			{
				if (pLayerSet->strDefine != MEASURE_LAYER_TXT && pLayerSet->strDefine != THICKNESS_LAYER_TXT)
				{
					continue;
				}
			}
		}

		//Layer가 선택되었는지 체크
		if (pLayerSet->bCheck == false) continue;

		//선택된 Layer의 이름에 맞는 layer Ptr 가져오기
		int nLayerIndex;//사용안함
		CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, pLayerSet->strLayer, nLayerIndex);
		if (pLayer == nullptr) continue;


		//Layer의 Feature
		int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());
		for (int nfeno = nFeatureCount - 1; nfeno >= 0; nfeno--)
		{
			CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nfeno);
			if (pFeature == nullptr) continue;

			switch (pFeature->m_eType)
			{
			case FeatureType::Z:
			{
				CFeatureZ *pFeatureZ = (CFeatureZ*)pFeature;
				pSymbol = pFeatureZ->m_pSymbol;
				if (m_pDrawFunction->CheckHighlight_TpFeature(fptPoint, pFeatureZ, pSymbol, m_pDrawManager->GetViewScale()) == TRUE)
				{
					if (bPreHighLight == FALSE && (GetAsyncKeyState(VK_SHIFT) & 0x8000) == FALSE &&
						(GetAsyncKeyState(VK_SHIFT) & 0x8001) == FALSE)
					{
						ResetSelect();
					}

					_setHighlight_TpFeature(pFeatureZ, bPreHighLight);

					if (bPreHighLight == FALSE)
					{
						if (pFeatureZ->m_bHighlight == true)
						{
							CDrawHighlightShowFunc::SetSelectInfo(pFeatureZ, nfeno, pLayer->m_strLayerName, m_stSelectInfo);
						}
						else
						{
							CDrawHighlightShowFunc::DeleteSelectInfo(pFeatureZ, nfeno, pLayer->m_strLayerName, m_stSelectInfo);
						}
					}

					bSelect = TRUE;
				}
			}
			break;

			case FeatureType::L:		//Line
			{
				CFeatureL *pFeatureL = (CFeatureL*)pFeature;
				pSymbol = pFeatureL->m_pSymbol;
				if (m_pDrawFunction->CheckHighlight_LineFeature(fptPoint, pFeatureL, pSymbol, m_pDrawManager->GetViewScale()) == TRUE)
				{
					if (bPreHighLight == FALSE && (GetAsyncKeyState(VK_SHIFT) & 0x8000) == FALSE &&
					(GetAsyncKeyState(VK_SHIFT) & 0x8001) == FALSE)
					{
						ResetSelect();
					}

					_SetHighlight_LineFeature(pFeatureL, bPreHighLight);

					//
					if (bPreHighLight == FALSE)
					{
						if (pFeatureL->m_bHighlight == true)
						{
							CDrawHighlightShowFunc::SetSelectInfo(pFeatureL, nfeno, pLayer->m_strLayerName, m_stSelectInfo);
						}
						else
						{
							CDrawHighlightShowFunc::DeleteSelectInfo(pFeatureL, nfeno, pLayer->m_strLayerName, m_stSelectInfo);
						}
					}

					bSelect = TRUE;
				}
			}
			break;
			case FeatureType::P:		//Pad
			{
				CFeatureP *pFeatureP = (CFeatureP*)pFeature;
				pSymbol = pFeatureP->m_pSymbol;
				if (m_pDrawFunction->CheckHighlight_PadFeature(fptPoint, pFeatureP, pSymbol, m_pDrawManager->GetViewScale()) == TRUE)
				{
					if (bPreHighLight == FALSE && (GetAsyncKeyState(VK_SHIFT) & 0x8000) == FALSE &&
						(GetAsyncKeyState(VK_SHIFT) & 0x8001) == FALSE)
					{
						ResetSelect();
					}

					_SetHighlight_PadFeature(pFeatureP, bPreHighLight);

					if (bPreHighLight == FALSE)
					{
						if (pFeatureP->m_bHighlight == true)
						{
							CDrawHighlightShowFunc::SetSelectInfo(pFeatureP, nfeno, pLayer->m_strLayerName, m_stSelectInfo);
						}
						else
						{
							CDrawHighlightShowFunc::DeleteSelectInfo(pFeatureP, nfeno, pLayer->m_strLayerName, m_stSelectInfo);
						}
					}

					bSelect = TRUE;
				}
			}
			break;
			case FeatureType::A:		//Arc								
			{
				CFeatureA *pFeatureA = (CFeatureA*)pFeature;
				pSymbol = pFeatureA->m_pSymbol;
				if (m_pDrawFunction->CheckHighlight_ArcFeature(fptPoint, pFeatureA, pSymbol, m_pDrawManager->GetViewScale()) == TRUE)
				{
					if (bPreHighLight == FALSE && (GetAsyncKeyState(VK_SHIFT) & 0x8000) == FALSE &&
						(GetAsyncKeyState(VK_SHIFT) & 0x8001) == FALSE)
					{
						ResetSelect();
					}

					_SetHighlight_ArcFeature(pFeatureA, bPreHighLight);

					if (bPreHighLight == FALSE)
					{
						if (pFeatureA->m_bHighlight == true)
						{
							CDrawHighlightShowFunc::SetSelectInfo(pFeatureA, nfeno, pLayer->m_strLayerName, m_stSelectInfo);
						}
						else
						{
							CDrawHighlightShowFunc::DeleteSelectInfo(pFeatureA, nfeno, pLayer->m_strLayerName, m_stSelectInfo);
						}
					}

					bSelect = TRUE;
				}
			}
			break;
			case FeatureType::T:		//Text			
			{

			}
			break;

			default:	//unknown
				break;
			}

			if (bSelect == TRUE)
			{
				break;
			}
		}
	}
	
	//Surface이외에 선택된것이 없다면.Surface를 검색..
	if (bSelect == FALSE)
	{
		vector<TempSelectSurface> vecSelectSurface;
		for (int i = 0; i < nLayerSetCount; i++)
		{
			LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];

			if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS)

				if(pLayerSet->strDefine != THICKNESS_LAYER_TXT && pLayerSet->strDefine != MEASURE_LAYER_TXT)
			{
				continue;
			}

			//Layer가 선택되었는지 체크
			if (pLayerSet->bCheck == false) continue;

			//선택된 Layer의 이름에 맞는 layer Ptr 가져오기
			int nLayerIndex;//사용안함
			CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, pLayerSet->strLayer, nLayerIndex);
			if (pLayer == nullptr) continue;


			//Layer의 Feature

			//마우스 영역에 걸치는 Feature 산출
			int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());
			for (int nfeno = nFeatureCount - 1; nfeno >= 0; nfeno--)
			{
				CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nfeno);
				if (pFeature == nullptr) continue;

				switch (pFeature->m_eType)
				{
				case FeatureType::S:		//Surface	
				{
					CFeatureS *pFeatureS = (CFeatureS*)pFeature;
					pSymbol = pFeatureS->m_pSymbol;
					int nSelectIndex = -1;
					float fArea;
					if (m_pDrawFunction->CheckHighlight_SurfaceFeature(fptPoint, pFeatureS, pSymbol, m_pDrawManager->GetViewScale(), nSelectIndex, fArea) == TRUE)
					{
						TempSelectSurface stTemp;
						stTemp.strLayer = pLayerSet->strLayer;
						stTemp.nSurfaceIndex = nfeno;
						stTemp.nObOeIndex = nSelectIndex;
						stTemp.fArea = fArea;

						vecSelectSurface.push_back(stTemp);
					}
				}

				}
			}
		}

		int nSelectSize = static_cast<int>(vecSelectSurface.size());
		if (nSelectSize <= 0) return FALSE;

		bSelect = TRUE;

		float fMinArea = FLT_MAX;
		TempSelectSurface stMinAreaSurface;
		for (int i = 0; i < nSelectSize; i++)
		{
			if (fMinArea > vecSelectSurface[i].fArea)
			{
				fMinArea = vecSelectSurface[i].fArea;
				stMinAreaSurface = vecSelectSurface[i];
			}
		}

		if (bPreHighLight == FALSE && (GetAsyncKeyState(VK_SHIFT) & 0x8000) == FALSE &&
			(GetAsyncKeyState(VK_SHIFT) & 0x8001) == FALSE)
		{
			ResetSelect();
		}

		//선택된 Layer의 이름에 맞는 layer Ptr 가져오기
		int nLayerIndex;//사용안함
		CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, stMinAreaSurface.strLayer, nLayerIndex);
		if (pLayer == nullptr) return FALSE;

		CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(stMinAreaSurface.nSurfaceIndex);
		if (pFeature == nullptr) return FALSE;

		CFeatureS *pFeatureS = (CFeatureS*)pFeature;

		//Seletec Info는 Surface의 feature Highlight 함수 내부에서 처리
		_SetHighlight_SurfaceFeature(pFeatureS, stMinAreaSurface.nObOeIndex, bPreHighLight);

		if (bPreHighLight == FALSE)
		{
			CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(stMinAreaSurface.nObOeIndex);
			if (stMinAreaSurface.nObOeIndex == 0)
			{//Main ObOe 선택

				if (pObOe->m_bHighlight == true)
				{
					CDrawHighlightShowFunc::SetSelectInfo(pFeatureS, stMinAreaSurface.nSurfaceIndex, pLayer->m_strLayerName, m_stSelectInfo);
				}
				else
				{
					CDrawHighlightShowFunc::DeleteSelectInfo(pFeatureS, stMinAreaSurface.nSurfaceIndex, pLayer->m_strLayerName, m_stSelectInfo);
				}
			}
			else
			{//Sub ObOe 선택

				if (pObOe->m_bHighlight == true)
				{
					CDrawHighlightShowFunc::SetSelectInfo(pObOe, stMinAreaSurface.nSurfaceIndex, stMinAreaSurface.nObOeIndex, pLayer->m_strLayerName, m_stSelectInfo);
				}
				else
				{
					CDrawHighlightShowFunc::DeleteSelectInfo(pObOe, stMinAreaSurface.nSurfaceIndex, stMinAreaSurface.nObOeIndex, pLayer->m_strLayerName, m_stSelectInfo);
				}
			}
		}

	}

	
	return bSelect;

}


BOOL CDirectView::_SetHighlight_MaskGeo(IN const MaskType &eMaskType, IN D2D_POINT_2F fptPoint, IN const BOOL &bPreHighLight)
{
	if (m_pDrawFunction == nullptr) return FALSE;
	if (m_nSelectStep == -1) return FALSE;
	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr) return FALSE;

	CSymbol *pSymbol = nullptr;

	int nLayerSetCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());
	int nDrawLayerCount = 0;
	BOOL bSelect = FALSE;
	for (int i = 0; i < nLayerSetCount; i++)
	{
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];

		if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS &&
			pLayerSet->strDefine != MEASURE_LAYER_TXT)
		{
			continue;
		}

		//Layer가 선택되었는지 체크
		if (pLayerSet->bCheck == false) continue;

		//선택된 Layer의 이름에 맞는 layer Ptr 가져오기
		int nLayerIndex;//사용안함
		CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, pLayerSet->strLayer, nLayerIndex);
		if (pLayer == nullptr) continue;

		//Layer의 Feature
		int nAlignMaskCount;
		if (eMaskType == MaskType::enumType_Align)
		{
			nAlignMaskCount = static_cast<int>(pLayer->m_arrAlign.GetCount());
		}
		else
		{
			nAlignMaskCount = static_cast<int>(pLayer->m_arrMask.GetCount());
		}

		for (int nAlignMask = 0; nAlignMask < nAlignMaskCount; nAlignMask++)
		{
			CTypedPtrArray<CObArray, CFeature*> *pMaskArr;
			if (eMaskType == MaskType::enumType_Align)
			{
				CAlignMask *pAlignMask = pLayer->m_arrAlign.GetAt(nAlignMask);
				pMaskArr = &pAlignMask->m_arrFeature;
			}
			else
			{
				CAlignMask *pAlignMask = pLayer->m_arrMask.GetAt(nAlignMask);
				pMaskArr = &pAlignMask->m_arrFeature;
			}
			if (pMaskArr == nullptr) continue;;

			int nFeatureCount = static_cast<int>(pMaskArr->GetCount());

			for (int nfeno = nFeatureCount - 1; nfeno >= 0; nfeno--)
			{
				CFeature *pFeature = pMaskArr->GetAt(nfeno);
				if (pFeature == nullptr) continue;

				switch (pFeature->m_eType)
				{

				case FeatureType::P:		//Pad
				{
					CFeatureP *pFeatureP = (CFeatureP*)pFeature;
					pSymbol = pFeatureP->m_pSymbol;
					if (m_pDrawFunction->CheckHighlight_PadFeature(fptPoint, pFeatureP, pSymbol, m_pDrawManager->GetViewScale()) == TRUE)
					{
						if (bPreHighLight == FALSE && (GetAsyncKeyState(VK_SHIFT) & 0x8000) == FALSE &&
							(GetAsyncKeyState(VK_SHIFT) & 0x8001) == FALSE)
						{
							ResetSelect();
						}

						_SetHighlight_PadFeature(pFeatureP, bPreHighLight);

						bSelect = TRUE;
					}
				}
				break;

				if (bSelect == TRUE)
				{
					break;
				}
				}
			}
		}
	}

	//Surface이외에 선택된것이 없다면.Surface를 검색..
	if (bSelect == FALSE)
	{
		vector<TempSelectSurface> vecSelectSurface;
		for (int i = 0; i < nLayerSetCount; i++)
		{
			LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];

			if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS &&
				pLayerSet->strDefine != MEASURE_LAYER_TXT)
			{
				continue;
			}

			//Layer가 선택되었는지 체크
			if (pLayerSet->bCheck == false) continue;

			//선택된 Layer의 이름에 맞는 layer Ptr 가져오기
			int nLayerIndex;//사용안함
			CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, pLayerSet->strLayer, nLayerIndex);
			if (pLayer == nullptr) continue;


			//Layer의 Feature
			int nAlignMaskCount;
			if (eMaskType == MaskType::enumType_Align)
			{
				nAlignMaskCount = static_cast<int>(pLayer->m_arrAlign.GetCount());
			}
			else
			{
				nAlignMaskCount = static_cast<int>(pLayer->m_arrMask.GetCount());
			}
			
			for (int nAlignMask = 0; nAlignMask < nAlignMaskCount; nAlignMask++)
			{
				CTypedPtrArray<CObArray, CFeature*> *pMaskArr;
				if (eMaskType == MaskType::enumType_Align)
				{
					CAlignMask *pAlignMask = pLayer->m_arrAlign.GetAt(nAlignMask);
					pMaskArr = &pAlignMask->m_arrFeature;
				}
				else
				{
					CAlignMask *pAlignMask = pLayer->m_arrMask.GetAt(nAlignMask);
					pMaskArr = &pAlignMask->m_arrFeature;
				}
				if (pMaskArr == nullptr) continue;;

				int nFeatureCount = static_cast<int>(pMaskArr->GetCount());
				for (int nfeno = nFeatureCount - 1; nfeno >= 0; nfeno--)
				{
					CFeature *pFeature = pMaskArr->GetAt(nfeno);
					if (pFeature == nullptr) continue;

					switch (pFeature->m_eType)
					{
					case FeatureType::S:		//Surface	
					{
						CFeatureS *pFeatureS = (CFeatureS*)pFeature;
						pSymbol = pFeatureS->m_pSymbol;
						int nSelectIndex = -1;
						float fArea;
						if (m_pDrawFunction->CheckHighlight_SurfaceFeature(fptPoint, pFeatureS, pSymbol, m_pDrawManager->GetViewScale(), nSelectIndex, fArea) == TRUE)
						{
							TempSelectSurface stTemp;
							stTemp.strLayer = pLayerSet->strLayer;
							stTemp.nSurfaceIndex = nfeno;
							stTemp.nObOeIndex = nSelectIndex;
							stTemp.fArea = fArea;

							vecSelectSurface.push_back(stTemp);
						}
					}

					}
				}
			}
		}

		int nSelectSize = static_cast<int>(vecSelectSurface.size());
		if (nSelectSize <= 0) return FALSE;

		bSelect = TRUE;

		float fMinArea = FLT_MAX;
		TempSelectSurface stMinAreaSurface;
		for (int i = 0; i < nSelectSize; i++)
		{
			if (fMinArea > vecSelectSurface[i].fArea)
			{
				fMinArea = vecSelectSurface[i].fArea;
				stMinAreaSurface = vecSelectSurface[i];
			}
		}

		if (bPreHighLight == FALSE && (GetAsyncKeyState(VK_SHIFT) & 0x8000) == FALSE &&
			(GetAsyncKeyState(VK_SHIFT) & 0x8001) == FALSE)
		{
			ResetSelect();
		}

		//선택된 Layer의 이름에 맞는 layer Ptr 가져오기
		int nLayerIndex;//사용안함
		CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, stMinAreaSurface.strLayer, nLayerIndex);
		if (pLayer == nullptr) return FALSE;

		CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(stMinAreaSurface.nSurfaceIndex);
		if (pFeature == nullptr) return FALSE;

		CFeatureS *pFeatureS = (CFeatureS*)pFeature;

		//Seletec Info는 Surface의 feature Highlight 함수 내부에서 처리
		_SetHighlight_SurfaceFeature(pFeatureS, stMinAreaSurface.nObOeIndex, bPreHighLight);

	}


	return bSelect;

}

UINT32 CDirectView::ClearPreSelect()
{
	int nStepCount = static_cast<int>(m_pJobData->m_arrStep.GetCount());
	for ( int nStep = 0 ; nStep < nStepCount ; nStep++ )
	{
		CStep *pStep = m_pJobData->m_arrStep.GetAt(nStep);
		if (pStep == nullptr ) continue;

		int nLayerCount = static_cast<int>(pStep->m_arrLayer.GetCount());
		for (int nLayer = 0; nLayer < nLayerCount; nLayer++)
		{
			CLayer *pLayer = pStep->m_arrLayer.GetAt(nLayer);
			if (pLayer == nullptr) continue;

			int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());
			for (int nFeature = 0; nFeature < nFeatureCount; nFeature++)
			{
				CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nFeature);
				if (pFeature == nullptr) continue;

				switch (pFeature->m_eType)
				{
				case FeatureType::Z:
					((CFeatureZ*)pFeature)->m_bPreHighlight = false;
					break;

				case FeatureType::L:		//Line
					((CFeatureL*)pFeature)->m_bPreHighlight = false;
					break;
				case FeatureType::P:		//Pad
					((CFeatureP*)pFeature)->m_bPreHighlight = false;
					
					break;
				case FeatureType::A:		//Arc								
					((CFeatureA*)pFeature)->m_bPreHighlight = false;
					break;
				case FeatureType::T:		//Text			
					((CFeatureT*)pFeature)->m_bPreHighlight = false;
					break;
				case FeatureType::S:		//Surface	
				{
					CFeatureS *pFeatureS = (CFeatureS*)pFeature;
					int nSurfaceCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
					for (int nSurface = 0; nSurface < nSurfaceCount; nSurface++)
					{
						CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nSurface);
						if (pObOe == nullptr) continue;

						pObOe->m_bPreHighlight = false;

					}

				}

				break;
				//case FeatureType::B:		//Barcode								
				//	pSymbolTmp = ((CFeatureB*)pFeatureTmp)->m_pSymbol;
				//	break;
				default:	//unknown
					break;
				}
			}//Feature End

			int nAlignMaskCount = static_cast<int>(pLayer->m_arrAlign.GetCount());
			for (int nAlignMask = 0; nAlignMask < nAlignMaskCount; nAlignMask++)
			{

				CAlignMask *pAlignMask = pLayer->m_arrAlign.GetAt(nAlignMask);
				int nCount = static_cast<int>(pAlignMask->m_arrFeature.GetCount());
				for (int nfeno = 0; nfeno < nCount; nfeno++)
				{
					CFeature *pFeature = pAlignMask->m_arrFeature.GetAt(nfeno);
					if (pFeature == nullptr) continue;
					switch (pFeature->m_eType)
					{
					case FeatureType::P:		//Pad
						((CFeatureP*)pFeature)->m_bPreHighlight = false;
						break;
					case FeatureType::S:		//Surface	
					{
						CFeatureS *pFeatureS = (CFeatureS*)pFeature;
						int nSurfaceCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
						for (int nSurface = 0; nSurface < nSurfaceCount; nSurface++)
						{
							CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nSurface);
							if (pObOe == nullptr) continue;

							pObOe->m_bPreHighlight = false;
						}
					}

					break;
					}

				}
			}

			nAlignMaskCount = static_cast<int>(pLayer->m_arrMask.GetCount());
			for (int nAlignMask = 0; nAlignMask < nAlignMaskCount; nAlignMask++)
			{
				CAlignMask *pAlignMask = pLayer->m_arrMask.GetAt(nAlignMask);
				int nCount = static_cast<int>(pAlignMask->m_arrFeature.GetCount());
				for (int nfeno = 0; nfeno < nCount; nfeno++)
				{
					CFeature *pFeature = pAlignMask->m_arrFeature.GetAt(nfeno);
					if (pFeature == nullptr) continue;
					switch (pFeature->m_eType)
					{
					case FeatureType::P:		//Pad
						((CFeatureP*)pFeature)->m_bPreHighlight = false;
						break;
					case FeatureType::S:		//Surface	
					{
						CFeatureS *pFeatureS = (CFeatureS*)pFeature;
						int nSurfaceCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
						for (int nSurface = 0; nSurface < nSurfaceCount; nSurface++)
						{
							CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nSurface);
							if (pObOe == nullptr) continue;

							pObOe->m_bPreHighlight = false;
						}
					}

					break;
					}
				}

			}


		}

	}



	return RESULT_GOOD;
}
// 
// BOOL CDirectView::_CheckHighlight_LineFeature(IN D2D_POINT_2F fptPoint, IN CFeatureL* pFeatureL, IN CSymbol *pSymbol)
// {
// 	if (pFeatureL == nullptr) return FALSE;
// 	if (m_pDrawFunction == nullptr) return FALSE;
// 	if (pFeatureL->m_pGeoObject == nullptr) return FALSE;
// 
// 	float fPenWidth = 1.0;
// 	ID2D1StrokeStyle *pStrokeStyle = nullptr;
// 	UINT32 nRet = m_pDrawFunction->GetLineDrawInfo(pFeatureL->m_pSymbol, fPenWidth, &pStrokeStyle);
// 
// 	BOOL bHit = FALSE;
// 	if (nRet == RESULT_GOOD)
// 	{//Line은 Stroke만 존재함.
// 		//pFeatureL->m_pGeoObject->StrokeContainsPoint(fptPoint, fPenWidth, pStrokeStyle, m_ViewScale, &bHit);
// 		pFeatureL->m_pGeoObject->StrokeContainsPoint(fptPoint, fPenWidth, pStrokeStyle, nullptr, &bHit);
// 	}
// 
// 	return bHit;
// }
// 
// BOOL CDirectView::_CheckHighlight_PadFeature(IN D2D_POINT_2F fptPoint, IN CFeatureP* pFeatureP, IN CSymbol *pSymbol)
// {
// 	if (pFeatureP == nullptr) return FALSE;
// 	if (m_pDrawFunction == nullptr) return FALSE;
// 	if (pFeatureP->m_pGeoObject == nullptr) return FALSE;
// 	
// 	float fPenWidth = 1.0;
// 	ID2D1StrokeStyle *pStrokeStyle = nullptr;
// 	UINT32 nRet = m_pDrawFunction->GetPadDrawInfo(pFeatureP->m_pSymbol, fPenWidth, &pStrokeStyle, nullptr);
// 
// 	BOOL bHit;
// 	if (nRet == RESULT_GOOD)
// 	{//Stroke
// 		//pFeatureP->m_pGeoObject->StrokeContainsPoint(fptPoint, fPenWidth, pStrokeStyle, m_ViewScale, &bHit);
// 		pFeatureP->m_pGeoObject->StrokeContainsPoint(fptPoint, fPenWidth, pStrokeStyle, nullptr, &bHit);
// 	}
// 	else
// 	{//fill
// 		//pFeatureP->m_pGeoObject->FillContainsPoint(fptPoint, m_ViewScale, &bHit);
// 		pFeatureP->m_pGeoObject->FillContainsPoint(fptPoint, nullptr, &bHit);
// 	}
// 
// 	return bHit;
// }
// 
// BOOL CDirectView::_CheckHighlight_ArcFeature(IN D2D_POINT_2F fptPoint, IN CFeatureA* pFeatureA, IN CSymbol *pSymbol)
// {
// 	if (pFeatureA == nullptr) return FALSE;
// 	if (m_pDrawFunction == nullptr) return FALSE;
// 	if (pFeatureA->m_pGeoObject == nullptr) return FALSE;
// 
// 	float fPenWidth = 1.0;
// 	ID2D1StrokeStyle *pStrokeStyle = nullptr;
// 	UINT32 nRet = m_pDrawFunction->GetArcDrawInfo(pFeatureA->m_pSymbol, fPenWidth, &pStrokeStyle);
// 
// 	BOOL bHit = FALSE;
// 	if (nRet == RESULT_GOOD)
// 	{//Stroke
// 		//pFeatureA->m_pGeoObject->StrokeContainsPoint(fptPoint, fPenWidth, pStrokeStyle, m_ViewScale, &bHit);
// 		pFeatureA->m_pGeoObject->StrokeContainsPoint(fptPoint, fPenWidth, pStrokeStyle, nullptr, &bHit);
// 	}
// 
// 	return bHit;
// }
// 
// BOOL CDirectView::_CheckHighlight_TextFeature(IN D2D_POINT_2F fptPoint, IN CFeatureT* pFeatureT, IN CSymbol *pSymbol)
// {
// 	return FALSE;
// }
// 
// BOOL CDirectView::_CheckHighlight_SurfaceFeature(IN D2D_POINT_2F fptPoint, IN CFeatureS* pFeatureS, IN CSymbol *pSymbol, OUT int &nSurfaceIndex)
// {
// 	if (pFeatureS == nullptr) return FALSE;
// 
// 	int nSurfaceCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
// 	for (int i = nSurfaceCount - 1; i >=  0; i--)
// 	{
// 		CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(i);
// 		if (pObOe == nullptr) continue;
// 
// 		BOOL bHit = FALSE;
// 		if ( pObOe->m_pGeoObject == nullptr ) continue;
// 		//pObOe->m_pGeoObject->FillContainsPoint(fptPoint, m_ViewScale, &bHit);
// 		pObOe->m_pGeoObject->FillContainsPoint(fptPoint, nullptr, &bHit);
// 		if (bHit == TRUE)
// 		{
// 			nSurfaceIndex = i;
// 			return TRUE;
// 		}
// 
// 	}
// 
// 	return FALSE;
// }

void CDirectView::_setHighlight_TpFeature(IN CFeatureZ* pFeatureZ, IN const BOOL &bPreHighLight)
{
	if (pFeatureZ == nullptr) return;
	if (bPreHighLight == TRUE)
	{
		pFeatureZ->m_bPreHighlight = true;
	}
	else
	{
		pFeatureZ->m_bHighlight = !pFeatureZ->m_bHighlight;
	}
}




void CDirectView::_SetHighlight_LineFeature(IN CFeatureL* pFeatureL, IN const BOOL &bPreHighLight)
{
	if (pFeatureL == nullptr) return;
	if (bPreHighLight == TRUE)
	{
		pFeatureL->m_bPreHighlight = true;
	}
	else
	{
		pFeatureL->m_bHighlight = !pFeatureL->m_bHighlight;
	}
}

void CDirectView::_SetHighlight_PadFeature(IN CFeatureP* pFeatureP, IN const BOOL &bPreHighLight)
{
	if (pFeatureP == nullptr) return;
	if (bPreHighLight == TRUE)
	{
		pFeatureP->m_bPreHighlight = true;
	}
	else
	{
		pFeatureP->m_bHighlight = !pFeatureP->m_bHighlight;
	}
}

void CDirectView::_SetHighlight_ArcFeature(IN CFeatureA* pFeatureA, IN const BOOL &bPreHighLight)
{
	if (pFeatureA == nullptr) return;
	if (bPreHighLight == TRUE)
	{
		pFeatureA->m_bPreHighlight = true;
	}
	else
	{
		pFeatureA->m_bHighlight = !pFeatureA->m_bHighlight;
	}
}

void CDirectView::_SetHighlight_TextFeature(IN CFeatureT* pFeatureT, IN const BOOL &bPreHighLight)
{
	if (pFeatureT == nullptr) return;
	if (bPreHighLight == TRUE)
	{
		pFeatureT->m_bPreHighlight = true;
	}
	else
	{
		pFeatureT->m_bHighlight = !pFeatureT->m_bHighlight;
	}
}

void CDirectView::_SetHighlight_SurfaceFeature(IN CFeatureS* pFeatureS, IN const int &nSurfaceIndex, IN const BOOL &bPreHighLight)
{
	if (pFeatureS == nullptr) return;

	int nSurfaceCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
	if (nSurfaceIndex < 0) return;
	if (nSurfaceIndex >= nSurfaceCount) return;

	CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nSurfaceIndex);
	if (pObOe == nullptr) return;

	if (bPreHighLight == TRUE)
	{
		pObOe->m_bPreHighlight = true;
	}
	else
	{
		pObOe->m_bHighlight = !pObOe->m_bHighlight;
	}

	
}

UINT32 CDirectView::_ClearGeo()
{
	int nStepCount = static_cast<int>(m_pJobData->m_arrStep.GetCount());
	for (int nStep = 0; nStep < nStepCount; nStep++)
	{
		CStep *pStep = m_pJobData->m_arrStep.GetAt(nStep);
		if (pStep == nullptr) continue;

		int nLayerCount = static_cast<int>(pStep->m_arrLayer.GetCount());
		for (int nLayer = 0; nLayer < nLayerCount; nLayer++)
		{
			CLayer *pLayer = pStep->m_arrLayer.GetAt(nLayer);
			if (pLayer == nullptr) continue;

			int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());
			for (int nFeature = 0; nFeature < nFeatureCount; nFeature++)
			{
				CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nFeature);
				if (pFeature == nullptr) continue;

				switch (pFeature->m_eType)
				{
				case FeatureType::L:		//Line
					if (((CFeatureL*)pFeature)->m_pGeoObject != nullptr)
					{
						((CFeatureL*)pFeature)->m_pGeoObject->Release();
						((CFeatureL*)pFeature)->m_pGeoObject = nullptr;
					}
					break;
				case FeatureType::P:		//Pad
					if (((CFeatureP*)pFeature)->m_pGeoObject != nullptr)
					{
						((CFeatureP*)pFeature)->m_pGeoObject->Release();
						((CFeatureP*)pFeature)->m_pGeoObject = nullptr;
					}
					if (((CFeatureP*)pFeature)->m_pGeoObject_Hole != nullptr)
					{
						((CFeatureP*)pFeature)->m_pGeoObject_Hole->Release();
						((CFeatureP*)pFeature)->m_pGeoObject_Hole = nullptr;
					}
					break;
				case FeatureType::A:		//Arc						
					if (((CFeatureA*)pFeature)->m_pGeoObject != nullptr)
					{
						((CFeatureA*)pFeature)->m_pGeoObject->Release();
						((CFeatureA*)pFeature)->m_pGeoObject = nullptr;
					}
					break;
				case FeatureType::T:		//Text			
					
					break;
				case FeatureType::S:		//Surface	
				{
					CFeatureS *pFeatureS = (CFeatureS*)pFeature;
					int nSurfaceCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
					for (int nSurface = 0; nSurface < nSurfaceCount; nSurface++)
					{
						CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nSurface);
						if (pObOe == nullptr) continue;

						if (pObOe->m_pGeoObject != nullptr)
						{
							pObOe->m_pGeoObject->Release();
							pObOe->m_pGeoObject = nullptr;
						}

					}
					if (pFeatureS->m_pGeoObject != nullptr)
					{
						pFeatureS->m_pGeoObject->Release();
						pFeatureS->m_pGeoObject = nullptr;
					}
					
				}

				break;
				//case FeatureType::B:		//Barcode								
				//	pSymbolTmp = ((CFeatureB*)pFeatureTmp)->m_pSymbol;
				//	break;
				default:	//unknown
					break;
				}
			}
		}

	}

	return RESULT_GOOD;
}

CLayer *CDirectView::_GetTargetLayer(int nStepIndex)
{
	CLayer *pTargetLayer = nullptr;

	if (nStepIndex < 0 || m_pJobData == nullptr ||
		m_pUserLayerSetInfo == nullptr || m_pSystemSpec == nullptr)
		return pTargetLayer;

	//Panel Step	
	if (nStepIndex != m_pUserLayerSetInfo->nPanelStepIdx)
		nStepIndex = m_pUserLayerSetInfo->nPanelStepIdx;

	CStep *pStep = m_pJobData->m_arrStep[nStepIndex];
	if (pStep == nullptr)
		return pTargetLayer;

	for (auto it : m_pUserLayerSetInfo->vcLayerSet)
	{
		if (it->strDefine.CompareNoCase(MEASURE_LAYER_TXT) != 0)
			continue;

		for (UINT32 lyno = 0; lyno < pStep->m_arrLayer.GetSize(); lyno++)
		{
			CLayer* pLayer = pStep->m_arrLayer.GetAt(lyno);
			if (pLayer == nullptr) continue;
			if (it->strLayer.CompareNoCase(pLayer->m_strLayerName) != 0) continue;

			pTargetLayer = pLayer;
			break;
		}
	}

	return pTargetLayer;
}

UINT32 CDirectView::_SetMeasureUnitInfo(vector<MeasureUnit>& vecMeasureUnitInfo)
{
	if (m_pPanelInfo == nullptr) return RESULT_BAD;

	MeasureUnit stMeasureUnit;
	for (auto it : m_pPanelInfo->stMeasureInfo.vcUnits)
	{
		stMeasureUnit.nSubIndex = it->stStepRpt.StripStep;
		stMeasureUnit.nStripX = it->stStepRpt.StripIdx.x;
		stMeasureUnit.nStripY = it->stStepRpt.StripIdx.y;

		stMeasureUnit.nSubSubIndex = it->stStepRpt.UnitStep;
		stMeasureUnit.nUnitX = it->stStepRpt.UnitIdx.x;
		stMeasureUnit.nUnitY = it->stStepRpt.UnitIdx.y;

		vecMeasureUnitInfo.push_back(stMeasureUnit);
	}

	return RESULT_GOOD;
}

UINT32 CDirectView::SaveUnitLayer(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dAngle, IN const bool bMirror, IN const double &dResolution, IN const CString &strPathName)
{
	if (m_pSaveLayer == nullptr) return RESULT_BAD;
	if (m_pSystemSpec == nullptr) return RESULT_BAD;
	if (m_pUserLayerSetInfo->iSelectedNum <= 0) return RESULT_BAD;

	CStep *pStep = m_pJobData->m_arrStep.GetAt(iStepIdx);
	if (pStep == nullptr) return RESULT_BAD;

	D2D1_RECT_F fptPicture;
	if (m_pSystemSpec->sysBasic.bProfileMode)
	{
		fptPicture = D2D1::RectF(DoubleToFloat(pStep->m_Profile.m_MinMax.left), DoubleToFloat(pStep->m_Profile.m_MinMax.top),
			DoubleToFloat(pStep->m_Profile.m_MinMax.right), DoubleToFloat(pStep->m_Profile.m_MinMax.bottom));
	}
	else
	{
		fptPicture = D2D1::RectF(DoubleToFloat(pStep->m_FeatureMinMax.left), DoubleToFloat(pStep->m_FeatureMinMax.top),
			DoubleToFloat(pStep->m_FeatureMinMax.right), DoubleToFloat(pStep->m_FeatureMinMax.bottom));
	}

	Orient ori = Orient::NoMir0Deg;
	CDrawFunction::GetOrient(dAngle, bMirror, ori);

	CRotMirrArr *rotMirrArr = new CRotMirrArr;
	rotMirrArr->InputData(0.0, 0.0, ori, m_pUserSetInfo->mirrorDir);

// 	if (m_pUserSetInfo->mcType == eMachineType::eNSIS)
// 	{
// 		if (m_pUserSetInfo->mirrorDir == eDirectionType::eVertical ||
// 			m_pUserSetInfo->mirrorDir == eDirectionType::eBoth)
// 		{
// 			rotMirrArr->InputData(0, 0, Orient::Mir0Deg, eDirectionType::eVertical);
// 		}
// 	}

	RECTD drtPicture;
	rotMirrArr->FinalPoint(&drtPicture.left, &drtPicture.top, fptPicture.left, fptPicture.top);
	rotMirrArr->FinalPoint(&drtPicture.right, &drtPicture.bottom, fptPicture.right, fptPicture.bottom);

	D2D1_RECT_F frtTempBox = D2D1::RectF(DoubleToFloat(drtPicture.left), DoubleToFloat(drtPicture.top),
		DoubleToFloat(drtPicture.right), DoubleToFloat(drtPicture.bottom));	
	
	if (S_OK != m_pSaveLayer->SetPictureBox(frtTempBox, dResolution))
		return RESULT_BAD;

	int nLayerSetCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());

	for (int i = 0; i < nLayerSetCount; i++)
	{
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
		if (!pLayerSet->bCheck)
			continue;

		if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS)
		{
			if (pLayerSet->strDefine.CompareNoCase(PATTERN_LAYER_TXT) != 0)
				continue;
		}

		COLORREF	color = pLayerSet->color;
		pLayerSet->color = RGB(255, 255, 255);

		CString strFullPathName;
		strFullPathName.Format(_T("%s\\%s.%s"), strPathName, pLayerSet->strLayer, m_pSystemSpec->sysBasic.strImageType);

		m_pSaveLayer->SetDrawMode(eDrawLevel::eUnitLv);

		if (RESULT_GOOD != m_pSaveLayer->SaveImage(strFullPathName, 0,
			pStep, pLayerSet, &m_pJobData->m_arrFont, rotMirrArr, m_pUserSetInfo, TRUE, SaveMode::SaveMode_All, false, _T(""), dAngle, bMirror))
		{
			if (rotMirrArr != nullptr) delete rotMirrArr;

			pLayerSet->color = color;
			return RESULT_BAD;
		}

		pLayerSet->color = color;
	}

	if (rotMirrArr != nullptr) delete rotMirrArr;

	return RESULT_GOOD;
}


UINT32 CDirectView::SaveStripLayer(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dAngle, IN const bool bMirror, IN const double &dResolution, IN const CString &strPathName)
{
	if (m_pSaveLayer == nullptr) return RESULT_BAD;
	if (m_pSystemSpec == nullptr) return RESULT_BAD;
	if (m_pUserLayerSetInfo->iSelectedNum <= 0) return RESULT_BAD;

	CStep *pStep = m_pJobData->m_arrStep.GetAt(iStepIdx);
	if (pStep == nullptr) return RESULT_BAD;

	D2D1_RECT_F fptPicture;
	if (m_pSystemSpec->sysBasic.bProfileMode)
	{
		fptPicture = D2D1::RectF(DoubleToFloat(pStep->m_Profile.m_MinMax.left), DoubleToFloat(pStep->m_Profile.m_MinMax.top),
			DoubleToFloat(pStep->m_Profile.m_MinMax.right), DoubleToFloat(pStep->m_Profile.m_MinMax.bottom));
	}
	else
	{
		fptPicture = D2D1::RectF(DoubleToFloat(pStep->m_FeatureMinMax.left), DoubleToFloat(pStep->m_FeatureMinMax.top),
			DoubleToFloat(pStep->m_FeatureMinMax.right), DoubleToFloat(pStep->m_FeatureMinMax.bottom));
	}
	
	Orient ori = Orient::NoMir0Deg;
	CDrawFunction::GetOrient(dAngle, bMirror, ori);
		
	CRotMirrArr rotMirrArr;
	rotMirrArr.InputData(0.0, 0.0, ori, m_pUserSetInfo->mirrorDir);

	RECTD drtPicture;
	rotMirrArr.FinalPoint(&drtPicture.left, &drtPicture.top, fptPicture.left, fptPicture.top);
	rotMirrArr.FinalPoint(&drtPicture.right, &drtPicture.bottom, fptPicture.right, fptPicture.bottom);

	D2D1_RECT_F frtTempBox = D2D1::RectF(DoubleToFloat(drtPicture.left), DoubleToFloat(drtPicture.top),
		DoubleToFloat(drtPicture.right), DoubleToFloat(drtPicture.bottom));
	
	frtTempBox = CDrawFunction::NormalizeRect(frtTempBox);

	int nLayerSetCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());

	LONGLONG dImageMemory = static_cast<LONGLONG>(drtPicture.width() / dResolution * drtPicture.height() / dResolution * 4);
	double dNeededMemory = (dImageMemory / (pow(1024, 3.0)));

	MEMORYSTATUS memoryStatus;
	GlobalMemoryStatus(&memoryStatus);

	double dUsageMemory = (memoryStatus.dwAvailPhys / (pow(1024, 3.0)));
	UINT32 iMaxThreadNum = static_cast<UINT32>(max(1, int(dUsageMemory / dNeededMemory)));

	CUtils Utils;
	UINT32 iCoreNum;
	if (m_pUserSetInfo->mcType == eMachineType::eAVI)
	{
		iCoreNum = iMaxThreadNum;
	}
	else
	{
		iCoreNum = max((UINT32)1, Utils.GetCoreCount() - 1);
	}

	m_pSaveLayerOp->SetJobFile(m_pJobData);
	m_pSaveLayerOp->StartThreads(m_pUserLayerSetInfo->iSelectedNum, iCoreNum);

	UINT32 iLayerIdx = 0, iThreadIdx = 0;
	std::vector<COLORREF> vcColor;
	for (int i = 0; i < nLayerSetCount; i++)
	{
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
		if (pLayerSet == nullptr)
			continue;

		if (!pLayerSet->bCheck)
			continue;

		vcColor.emplace_back(pLayerSet->color);
		pLayerSet->color = RGB(255, 255, 255);

		if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS)
		{
			if (pLayerSet->strDefine.CompareNoCase(PATTERN_LAYER_TXT) != 0)
				continue;
		}

		SaveMode eSaveMode = SaveMode::SaveMode_All;

		m_pSaveLayer->SetDrawMode(eDrawLevel::eStripLv);

		CSaveLayerJob* pstJob = new CSaveLayerJob;
		if (RESULT_GOOD != pstJob->SetData_SaveLayer(_T(""), iLayerIdx, iThreadIdx, pStep, pLayerSet, frtTempBox, m_pUserSetInfo, &m_pJobData->m_arrFont,
			strPathName, m_pSystemSpec->sysBasic.strImageType, dResolution, m_pSystemSpec->sysBasic.McType, m_pFactory, m_pD2DContext, m_pImageFactory, eSaveMode, 
			eDrawLevel::eStripLv, false, OpMode::MakeLayer, TRUE, dAngle, bMirror, _T(""), _T(""), 0))		
		{
			continue;
		}

		m_pSaveLayerOp->AddJob(pstJob);
		iLayerIdx++;
		iThreadIdx++;
	}

	if (m_pSaveLayerOp->RunAllJobs())
	{
		m_pSaveLayerOp->WaitForJobFinish();
		m_pSaveLayerOp->ResetAllJobEvents();
	}

	if (!m_pSaveLayerOp->StopAllThreads())
		return RESULT_BAD;

	for (int i = 0, j = 0; i < nLayerSetCount; i++)
	{
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
		if (pLayerSet == nullptr || !pLayerSet->bCheck) continue;

		pLayerSet->color = vcColor[j++];
	}

	vcColor.clear();

	//Merge Strip
// 	double dNeededMemory = 15.0;
// 	if (dResolution <= 0.002) dNeededMemory = 21.0;
// 	else if (dResolution >= 0.008) dNeededMemory = 2.5;
// 
// 	MEMORYSTATUS memoryStatus;
// 	GlobalMemoryStatus(&memoryStatus);
// 
// 	double dUsageMemory = (memoryStatus.dwAvailPhys / (pow(1024, 3.0)));
// 	if (dUsageMemory < dNeededMemory) return RESULT_BAD;
// 
// 	UINT32 iMaxThreadNum = static_cast<UINT32>(max(1, int(dUsageMemory / dNeededMemory)));
// 
// 	iLayerIdx = 0, iThreadIdx = 0;
// 	m_pSaveLayerOp->SetJobFile(m_pJobData);
// 	m_pSaveLayerOp->StartThreads(m_pUserLayerSetInfo->iSelectedNum, iMaxThreadNum);
// 
// 	for (int i = 0; i < nLayerSetCount; i++)
// 	{
// 		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
// 		if (pLayerSet == nullptr || !pLayerSet->bCheck)
// 			continue;
// 
// 		if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS)
// 		{
// 			if (pLayerSet->strDefine.CompareNoCase(PATTERN_LAYER_TXT) != 0)
// 				continue;
// 		}
// 		
// 		SaveMode eSaveMode = SaveMode::SaveMode_All;
// 		m_pSaveLayer->SetDrawMode(eDrawLevel::eStripLv);
// 
// 		CSaveLayerJob* pstJob = new CSaveLayerJob;
// 		if (RESULT_GOOD != pstJob->SetData_SaveLayer(_T(""), iLayerIdx, iThreadIdx, pStep, pLayerSet, frtTempBox, m_pUserSetInfo, &m_pJobData->m_arrFont,
// 			strPathName, m_pSystemSpec->sysBasic.strImageType, dResolution, m_pSystemSpec->sysBasic.McType, m_pFactory, m_pD2DContext, m_pImageFactory, eSaveMode,
// 			eDrawLevel::eStripLv, false, OpMode::MergeSwath, TRUE, dAngle, bMirror, _T(""), 0))
// 		{
// 			continue;
// 		}
// 
// 		m_pSaveLayerOp->AddJob(pstJob);
// 		iLayerIdx++;
// 		iThreadIdx++;
// 	}
// 
// 	if (m_pSaveLayerOp->RunAllJobs())
// 	{
// 		m_pSaveLayerOp->WaitForJobFinish();
// 		m_pSaveLayerOp->ResetAllJobEvents();
// 	}
//
// 	if (!m_pSaveLayerOp->StopAllThreads())
// 		return RESULT_BAD;	

	return RESULT_GOOD;
}

UINT32 CDirectView::SaveUnitAlignPoint(IN const INT32 iStepIdx, IN const UINT32 iUnitIndex, IN const UINT32 iIndex, IN const double dAngle, IN const bool bMirror,
	IN const double dCamAngle, IN const double &dResolution, IN const RECTD &rcRect, IN const CString &strPathName)
{
	if (m_pSaveLayer == nullptr) return RESULT_BAD;
	if (m_pSystemSpec == nullptr) return RESULT_BAD;

	CStep *pStep = m_pJobData->m_arrStep.GetAt(iStepIdx);
	if (pStep == nullptr) return RESULT_BAD;

	D2D1_RECT_F fptPicture = D2D1::RectF(DoubleToFloat(rcRect.left), DoubleToFloat(rcRect.top),
		DoubleToFloat(rcRect.right), DoubleToFloat(rcRect.bottom));
		
	Orient ori = Orient::NoMir0Deg;
	CDrawFunction::GetOrient(dAngle, bMirror, ori);

	CRotMirrArr *rotMirrArr = new CRotMirrArr;
	rotMirrArr->InputData(0.0, 0.0, ori, m_pUserSetInfo->mirrorDir);

	RECTD drtPicture;
	rotMirrArr->FinalPoint(&drtPicture.left, &drtPicture.top, fptPicture.left, fptPicture.top);
	rotMirrArr->FinalPoint(&drtPicture.right, &drtPicture.bottom, fptPicture.right, fptPicture.bottom);

// 
	D2D1_RECT_F frtTempBox = D2D1::RectF(DoubleToFloat(drtPicture.left), DoubleToFloat(drtPicture.top),
		DoubleToFloat(drtPicture.right), DoubleToFloat(drtPicture.bottom));

	if (S_OK != m_pSaveLayer->SetPictureBox(fptPicture, dResolution))
		return RESULT_BAD;
	
/*	CRotMirrArr *rotMirrArr = new CRotMirrArr;
	rotMirrArr->InputData(0.0, 0.0, Orient::NoMir0Deg);

	D2D1::Matrix3x2F matRotate = D2D1::Matrix3x2F::Rotation(static_cast<float>(dAngle),
		CDrawFunction::GetRectCenter(fptPicture));

	D2D1_RECT_F frtTempBox = CDrawFunction::DetansformRect(fptPicture, matRotate);

	if (S_OK != m_pSaveLayer->SetPictureBox(frtTempBox, dResolution))
		return RESULT_BAD;
*/
	int nLayerSetCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());

	for (int i = 0; i < nLayerSetCount; i++)
	{
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
		if (!pLayerSet->bCheck)
			continue;

		if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS)
		{
			if (pLayerSet->strDefine.CompareNoCase(PATTERN_LAYER_TXT) != 0)
				continue;
		}

		m_pSaveLayer->SetDrawMode(eDrawLevel::eUnitLv);
		
		CString strFullPathName;
		
		strFullPathName.Format(_T("%s\\U%d\\ALIGN\\%s_%s_%02d_Unit.%s"), strPathName, iUnitIndex + 1, m_pJobData->m_Misc.GetModelName(), pLayerSet->strLayer, iIndex + 1, m_pSystemSpec->sysBasic.strImageType);
				
		if (RESULT_GOOD != m_pSaveLayer->SaveImage(strFullPathName, dCamAngle,		
			pStep, pLayerSet, &m_pJobData->m_arrFont, rotMirrArr, m_pUserSetInfo, TRUE, SaveMode::SaveMode_All, false, _T(""), dAngle, bMirror))
		
		{
			if (rotMirrArr != nullptr) delete rotMirrArr;
			return RESULT_BAD;
		}
	}

	if (rotMirrArr != nullptr) delete rotMirrArr;

	return RESULT_GOOD;
}

//YJD START 22.06.13 - SAVE MEASURE IMAGE
UINT32 CDirectView::SaveMeasurePoint(IN const INT32 iStepIdx, IN const UINT32 iUnitIndex, IN const double dAngle, IN const bool bMirror,
	IN const double dCamAngle, IN const vector<double> &vecResolution, IN const vector<RECTD> &vecRect, IN const CString &strPathName)
{
	if (m_pSaveLayer == nullptr) return RESULT_BAD;
	if (m_pSystemSpec == nullptr) return RESULT_BAD;

	CStep *pStep = m_pJobData->m_arrStep.GetAt(iStepIdx);
	if (pStep == nullptr) return RESULT_BAD;

	Orient ori = Orient::NoMir0Deg;
	CDrawFunction::GetOrient(dAngle, bMirror, ori);

	CRotMirrArr *rotMirrArr = new CRotMirrArr;
	rotMirrArr->InputData(0.0, 0.0, ori);

	CUtils Utils;
	UINT32 iCoreNum;
	iCoreNum = max((UINT32)1, Utils.GetCoreCount() - 1);

	//1. Make Layer Images
	m_pSaveLayerOp->SetJobFile(m_pJobData);
	m_pSaveLayerOp->StartThreads(static_cast<int>(vecRect.size()), iCoreNum);
	//

	UINT32 iLayerIdx = 0, iThreadIdx = 0;
	std::vector<COLORREF> vcColor;
	int nLayerSetCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());
	for (int i = 0; i < nLayerSetCount; i++)
	{
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
		if (!pLayerSet->bCheck)
			continue;

		vcColor.emplace_back(pLayerSet->color);
		pLayerSet->color = RGB(255, 255, 255);

		if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS)
		{
			if (pLayerSet->strDefine.CompareNoCase(PATTERN_LAYER_TXT) != 0)
				continue;
		}

		int nMeasureCount = static_cast<int>(vecRect.size());
		for (int nMeasureIndex = 0; nMeasureIndex < nMeasureCount; nMeasureIndex++)
		{

			CString strSavePathName;
			CString strSaveFileName;
			strSavePathName.Format(_T("%s\\U%d\\MEASURE\\"), strPathName, iUnitIndex + 1);
			strSaveFileName.Format(_T("%02d"), nMeasureIndex + 1);

			::SHCreateDirectoryEx(NULL, strSavePathName, NULL);

			D2D1_RECT_F fptPicture = D2D1::RectF(DoubleToFloat(vecRect[nMeasureIndex].left), DoubleToFloat(vecRect[nMeasureIndex].top),
				DoubleToFloat(vecRect[nMeasureIndex].right), DoubleToFloat(vecRect[nMeasureIndex].bottom));

			fptPicture = CDrawFunction::NormalizeRect(fptPicture);

			CSaveLayerJob* pstJob = new CSaveLayerJob;
			if (RESULT_GOOD != pstJob->SetData_SaveLayer_SNU(strSaveFileName, iLayerIdx, iThreadIdx, pStep, pLayerSet, fptPicture, m_pUserSetInfo, &m_pJobData->m_arrFont,
				strSavePathName, _T(".bmp"), vecResolution[nMeasureIndex], m_pSystemSpec->sysBasic.McType, m_pFactory, m_pD2DContext, m_pImageFactory,
				dCamAngle,
				eDrawLevel::eUnitLv, false, OpMode::MakeLayer, TRUE, dAngle, bMirror, _T("")))

			{
				continue;
			}
			m_pSaveLayerOp->AddJob(pstJob);
			iLayerIdx++;
			iThreadIdx++;

		}
	}

	if (m_pSaveLayerOp->RunAllJobs())
	{
		m_pSaveLayerOp->WaitForJobFinish();
		m_pSaveLayerOp->ResetAllJobEvents();
	}

	if (!m_pSaveLayerOp->StopAllThreads())
		return RESULT_BAD;

	for (int i = 0, j = 0; i < nLayerSetCount; i++)
	{
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
		if (pLayerSet == nullptr || !pLayerSet->bCheck) continue;

		pLayerSet->color = vcColor[j++];
	}

	vcColor.clear();

	if (rotMirrArr != nullptr) delete rotMirrArr;

	_ClearGeo();
	SetRedrawGeo();

	return RESULT_GOOD;
}
//YJD END 22.06.13 - SAVE MEASURE IMAGE

UINT32 CDirectView::SavePanelLayer(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle, IN const double &dResolution, IN const CString &strPathName)
{
	if (m_pSaveLayer == nullptr) return RESULT_BAD;
	if (m_pSystemSpec == nullptr) return RESULT_BAD;

	if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS)
	{
		SavePanelLayer_NSIS(iStepIdx, iIndex, dCamAngle, dResolution, strPathName);
	}
	else if (m_pSystemSpec->sysBasic.McType == eMachineType::eAOI)
	{
		SavePanelLayer_AOI(iStepIdx, iIndex, dCamAngle, dResolution, strPathName);
	}
	else if (m_pSystemSpec->sysBasic.McType == eMachineType::eAVI)
	{
		SavePanelLayer_AOI(iStepIdx, iIndex, dCamAngle, dResolution, strPathName);
	}

	return RESULT_GOOD;
}

UINT32 CDirectView::SavePanelAlignPoint(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle, IN const double &dResolution,
	IN const RECTD &rcRect, IN const CString &strMeasureLayer, IN const CString &strPathName)
{
	if (m_pSaveLayer == nullptr) return RESULT_BAD;
	if (m_pSystemSpec == nullptr) return RESULT_BAD;

	CStep *pStep = m_pJobData->m_arrStep.GetAt(iStepIdx);
	if (pStep == nullptr) return RESULT_BAD;

	D2D1_RECT_F fptPicture = D2D1::RectF(DoubleToFloat(rcRect.left), DoubleToFloat(rcRect.top),
		DoubleToFloat(rcRect.right), DoubleToFloat(rcRect.bottom));
	
	CRotMirrArr *rotMirrArr = new CRotMirrArr;
	rotMirrArr->InputData(0.0, 0.0, Orient::NoMir0Deg);

	D2D1::Matrix3x2F matRotate = D2D1::Matrix3x2F::Rotation(static_cast<float>(0),//D2D1::Matrix3x2F::Rotation(static_cast<float>(m_pUserSetInfo->dAngle),
		CDrawFunction::GetRectCenter(fptPicture));

	D2D1_RECT_F frtTempBox = CDrawFunction::DetansformRect(fptPicture, matRotate, nullptr);
	
	if (S_OK != m_pSaveLayer->SetPictureBox(frtTempBox, dResolution))
		return RESULT_BAD;

	int nLayerSetCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());

	CString strTargetLayer = PATTERN_LAYER_TXT;
	if (m_pSystemSpec->sysBasic.McType == eMachineType::eAVI &&
		m_pSystemSpec->sysBasic.UserSite == eUserSite::eDAEDUCK)
		strTargetLayer = SR_LAYER_TXT;

	// 2022.09.27 KJH ADD
	// NSIS SEM SR Layer Align Image Save 위해 추가
	CString strTargetLayer2;
	if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS &&
		m_pSystemSpec->sysBasic.UserSite == eUserSite::eSEM)
	{
		strTargetLayer2 = SR_LAYER_TXT;
	}
	// KJH END

	for (int i = 0; i < nLayerSetCount; i++)
	{
		BOOL bPatternLayer = FALSE;
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
		if (!pLayerSet->bCheck)
			continue;

		if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS  &&
			m_pSystemSpec->sysBasic.UserSite == eUserSite::eDAEDUCK)
		{
			
			//if (pLayerSet->strDefine.CompareNoCase(MEASURE_LAYER_TXT) != 0)
			//MP Layer -> Pattern Layer 변경
			if (pLayerSet->strDefine.CompareNoCase(strTargetLayer) != 0)
				continue;
		}
		// 2022.09.27 KJH ADD
		// NSIS SEM 모드 추가
		else if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS &&
			m_pSystemSpec->sysBasic.UserSite == eUserSite::eSEM)
		{
			/*if (pLayerSet->strDefine.CompareNoCase(strTargetLayer) != 0)
			{
				bPatternLayer = TRUE;
				if (pLayerSet->strDefine.CompareNoCase(strTargetLayer2) != 0)
				{
					bPatternLayer = FALSE;
					continue;
				}
			}*/
			if (pLayerSet->strDefine.CompareNoCase(strTargetLayer) != 0)
				continue;
		}
		else
		{
			if (pLayerSet->strDefine.CompareNoCase(strTargetLayer) != 0)
				continue;
		}

		COLORREF	color = pLayerSet->color;
		pLayerSet->color = RGB(255, 255, 255);

		if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS)
			m_pSaveLayer->SetDrawMode(eDrawLevel::ePanelLv);
		else
			m_pSaveLayer->SetDrawMode(eDrawLevel::eSymbolLv);

		CString strFullPathName;
		strFullPathName.Format(_T("%s\\AlignMark"), strPathName);
		CreateDirectory(strFullPathName, NULL);
		
		if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS &&
			m_pSystemSpec->sysBasic.UserSite == eUserSite::eDAEDUCK)
			strFullPathName.Format(_T("%s\\AlignMark\\%s_%s_Align%d.%s"), strPathName, m_pJobData->GetModelName(), strMeasureLayer, iIndex + 1, m_pSystemSpec->sysBasic.strImageType);
		else if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS &&
			m_pSystemSpec->sysBasic.UserSite == eUserSite::eSEM )
			//if (bPatternLayer == TRUE)
				//strFullPathName.Format(_T("%s\\AlignMark\\%s_Align_Pattern%d.%s"), strPathName, m_pJobData->GetModelName(), iIndex + 1, m_pSystemSpec->sysBasic.strImageType);
			//else
				//strFullPathName.Format(_T("%s\\AlignMark\\%s_Align_SR%d.%s"), strPathName, m_pJobData->GetModelName(), iIndex + 1, m_pSystemSpec->sysBasic.strImageType);
			strFullPathName.Format(_T("%s\\AlignMark\\%s_%s_Align%d.%s"), strPathName, m_pJobData->GetModelName(), strMeasureLayer, iIndex + 1, m_pSystemSpec->sysBasic.strImageType);

		else
		{
			strFullPathName.Format(_T("%s\\AlignMark\\Panel"), strPathName);
			CreateDirectory(strFullPathName, NULL);
			strFullPathName.Format(_T("%s\\AlignMark\\Panel\\Align_%d.%s"), strPathName, iIndex + 1, m_pSystemSpec->sysBasic.strImageType);
		}
		
		if (RESULT_GOOD != m_pSaveLayer->SaveImage(strFullPathName, dCamAngle,
			pStep, pLayerSet, &m_pJobData->m_arrFont, rotMirrArr, m_pUserSetInfo, TRUE))
		{
			if (rotMirrArr != nullptr) delete rotMirrArr;

			pLayerSet->color = color;
			return RESULT_BAD;
		}

		pLayerSet->color = color;
	}

	if (rotMirrArr != nullptr) delete rotMirrArr;

	return RESULT_GOOD;
}

UINT32 CDirectView::SaveUnitAlignPoint_In_Panel(IN const INT32 iStepIdx, IN const Orient &eOrient, IN const double &dResolution,
	IN const CString &strMeasureLayer, IN const CString &strPathName)
{
	if (m_pSaveLayer == nullptr) return RESULT_BAD;
	if (m_pSystemSpec == nullptr) return RESULT_BAD;

	CStep *pStep = m_pJobData->m_arrStep.GetAt(iStepIdx);
	if (pStep == nullptr) return RESULT_BAD;

	CLayer *pPatternLayer = nullptr;

	CRotMirrArr *rotMirrArr = new CRotMirrArr;
	rotMirrArr->InputData(0.0, 0.0, eOrient, m_pUserSetInfo->mirrorDir);

	CString strImageType = m_pSystemSpec->sysBasic.strImageType;

	CString strFullPathName;
	CString strPath;

	if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS)
	{
		strPath.Format(_T("%s\\Unit_AlignMark"), strPathName);
		SHCreateDirectoryEx(NULL, strPath, NULL);
	}
	else
	{
		strPath.Format(_T("%s\\AlignMark"), strPathName);
		SHCreateDirectoryEx(NULL, strPath, NULL);

		strPath.Format(_T("%s\\AlignMark\\Mpoint"), strPathName);
		SHCreateDirectoryEx(NULL, strPath, NULL);
	}
	
	LayerSet_t* pLayerSet_Pattern = nullptr;
	vector<RECTD> vcAlign;
	int nLayerSetCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());
	int nStripAlignCount;
	int nUnitAlignCount;
	for (int i = 0; i < nLayerSetCount; i++)
	{
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
		if (!pLayerSet->bCheck)
			continue;

		if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS)
		{
			if (pLayerSet->strDefine.CompareNoCase(MEASURE_LAYER_TXT) != 0)
				continue;
		}
		else
		{
			if (pLayerSet->strDefine.CompareNoCase(PATTERN_LAYER_TXT) != 0)
				continue;
		}

		pLayerSet_Pattern = pLayerSet;

		COLORREF	color = pLayerSet->color;
		pLayerSet->color = RGB(255, 255, 255);

		if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS)
			m_pSaveLayer->SetDrawMode(eDrawLevel::ePanelLv);
		else
			m_pSaveLayer->SetDrawMode(eDrawLevel::eUnitLv);

		//Get Valid UnitAlign
		vcAlign.clear();
		for (auto it : m_pCellInfo->vcStrip_in_Cell)
		{
			auto vcStripInfo = it.second;
			if (vcStripInfo.size() > 0)
			{
				int nCount = static_cast<int>(vcStripInfo.size());
				if (nCount <= 0) continue;
				for (auto vcInfo : vcStripInfo[0]->vcAlign)
				{
					int nCount = static_cast<int>(vcInfo.size());

					if (nCount > 0)
					{
						vcAlign.push_back(vcInfo[0]);
					}
				}
				//
				break;
			}
		}
		nStripAlignCount = static_cast<int>(vcAlign.size());

		for (int nAlign = 0; nAlign < nStripAlignCount; nAlign++)
		{

			//CFeature *pFeature = pLayer->m_arrAlign.GetAt(nAlign);
			RECTD frtRect = vcAlign[nAlign];
			// 			rotMirrArr->FinalPoint(&frtRect.left, &frtRect.top, frtRect.left, frtRect.top);
			// 			rotMirrArr->FinalPoint(&frtRect.right, &frtRect.bottom, frtRect.right, frtRect.bottom);

			frtRect.NormalizeRectD();

			D2D1_RECT_F fptPicture = D2D1::RectF(DoubleToFloat(frtRect.left), DoubleToFloat(frtRect.top),
				DoubleToFloat(frtRect.right), DoubleToFloat(frtRect.bottom));

			if (S_OK != m_pSaveLayer->SetPictureBox(fptPicture, dResolution))
				return RESULT_BAD;

			if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS)
				strFullPathName.Format(_T("%s\\%s_%s_Align%d.%s"), strPathName, m_pJobData->GetModelName(), strMeasureLayer, nAlign + 1, strImageType);
			else
			{
				strFullPathName.Format(_T("%s\\Align_%d.%s"), strPath, nAlign + 1, strImageType);
			}
			//strFullPathName.Format(_T("%s\\AlignMark\\%s_%s_Align%d.bmp"), strPathName, m_pJobData->m_Misc.GetModelName(), strMeasureLayer, iIndex + 1);
			if (RESULT_GOOD != m_pSaveLayer->SaveImage(strFullPathName, 0,
				pStep, pLayerSet, &m_pJobData->m_arrFont, rotMirrArr, m_pUserSetInfo, TRUE))
			{
				if (rotMirrArr != nullptr) delete rotMirrArr;

				pLayerSet->color = color;
				return RESULT_BAD;
			}
		}

		//Get Valid UnitAlign
		vcAlign.clear();
		for (auto it : m_pCellInfo->vcUnit_in_Cell)
		{
			auto vcUnitInfo = it.second;
			if (vcUnitInfo.size() > 0)
			{
				int nCount = static_cast<int>(vcUnitInfo.size());
				if (nCount <= 0) continue;
				for (auto vcInfo : vcUnitInfo[0]->vcAlign)
				{
					int nCount = static_cast<int>(vcInfo.size());

					if (nCount > 0)
					{
						vcAlign.push_back(vcInfo[0]);
					}
				}
				//
				break;
			}
		}
		nUnitAlignCount = static_cast<int>(vcAlign.size());

		for (int nAlign = 0; nAlign < nUnitAlignCount; nAlign++)
		{

			//CFeature *pFeature = pLayer->m_arrAlign.GetAt(nAlign);
			RECTD frtRect = vcAlign[nAlign];
// 			rotMirrArr->FinalPoint(&frtRect.left, &frtRect.top, frtRect.left, frtRect.top);
// 			rotMirrArr->FinalPoint(&frtRect.right, &frtRect.bottom, frtRect.right, frtRect.bottom);

			frtRect.NormalizeRectD();

			D2D1_RECT_F fptPicture = D2D1::RectF(DoubleToFloat(frtRect.left), DoubleToFloat(frtRect.top),
				DoubleToFloat(frtRect.right), DoubleToFloat(frtRect.bottom));

			if (S_OK != m_pSaveLayer->SetPictureBox(fptPicture, dResolution))
				return RESULT_BAD;

			if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS)
				strFullPathName.Format(_T("%s\\%s_%s_Align%d.%s"), strPathName, m_pJobData->GetModelName(), strMeasureLayer, nAlign + nStripAlignCount + 1, strImageType);
			else
			{
				strFullPathName.Format(_T("%s\\Align_%d.%s"), strPath, nAlign + nStripAlignCount + 1, strImageType);
			}
			//strFullPathName.Format(_T("%s\\AlignMark\\%s_%s_Align%d.bmp"), strPathName, m_pJobData->m_Misc.GetModelName(), strMeasureLayer, iIndex + 1);
			if (RESULT_GOOD != m_pSaveLayer->SaveImage(strFullPathName, 0,
				pStep, pLayerSet, &m_pJobData->m_arrFont, rotMirrArr, m_pUserSetInfo, TRUE))
			{
				if (rotMirrArr != nullptr) delete rotMirrArr;

				pLayerSet->color = color;
				return RESULT_BAD;
			}
		}

		pLayerSet->color = color;
	}

	int nAlignCount = nStripAlignCount + nUnitAlignCount;
	//Make Align Region
 	if (m_pSystemSpec->sysBasic.McType == eMachineType::eAOI &&
		nAlignCount > 0 &&
		pLayerSet_Pattern != nullptr)
 	{
		strFullPathName.Format(_T("%s\\AlignMark\\Cell"), strPathName);
		SHCreateDirectoryEx(NULL, strFullPathName, NULL);

		BOOL bIsBlockMode = FALSE;
		if (m_pSystemSpec->sysBasic.bExceptDummy && m_pSystemSpec->sysBasic.MasterLevel == eMasterLevel::eBlockLv)
			bIsBlockMode = TRUE;

		CHalconFunction::CreateUnitAlignRegion(pLayerSet_Pattern->strLayer, nAlignCount, m_pFovInfo, m_pCellInfo, strPath, strFullPathName, strImageType, bIsBlockMode);
 	}

	if (rotMirrArr != nullptr) delete rotMirrArr;
	
	_ClearGeo();
	SetRedrawGeo();

	return RESULT_GOOD;
}

UINT32 CDirectView::SaveProfile(IN const BOOL m_bIs4Step, IN const INT32 iStepIdx, IN const Orient &eOrient, IN const CString &strPathName)
{
	CStep *pStep = m_pJobData->m_arrStep.GetAt(iStepIdx);
	if (pStep == nullptr) return RESULT_BAD;

	CString strStepName = pStep->m_strStepName;
	bool bIsPanel = false;
	if (strStepName.CompareNoCase(_T("PANEL")) == 0)
		bIsPanel = true;

	CString strRootPath = strPathName + _T("\\Profile\\") + strStepName + _T("\\");
	::SHCreateDirectoryEx(NULL, strRootPath, NULL);

	RECTD drtPickture = pStep->m_Profile.m_MinMax;
	drtPickture.NormalizeRectD();

	CRotMirrArr *rotMirrArr = new CRotMirrArr;
	rotMirrArr->InputData(0.0, 0.0, eOrient, m_pUserSetInfo->mirrorDir);

	//Calculate Resolution
	rotMirrArr->FinalPoint(&drtPickture.left, &drtPickture.top, drtPickture.left, drtPickture.top);
	rotMirrArr->FinalPoint(&drtPickture.right, &drtPickture.bottom, drtPickture.right, drtPickture.bottom);
	drtPickture.NormalizeRectD();	

	double dPictureWidth = drtPickture.width();
	double dPictureHeight = drtPickture.height();	

	//10%만큼 큰사이즈로 저장해야 외곽이 보임
	double dMargin = (dPictureWidth > dPictureHeight) ? (dPictureWidth * 0.01) : (dPictureHeight * 0.01);
	drtPickture.left -= dMargin;
	drtPickture.top -= dMargin;
	drtPickture.right += dMargin;
	drtPickture.bottom += dMargin;

	//drtPickture.left -= (dPictureWidth * 0.1);
	//drtPickture.top -= (dPictureHeight * 0.1);
	//drtPickture.right += (dPictureWidth * 0.1);
	//drtPickture.bottom += (dPictureHeight * 0.1);

	dPictureWidth = drtPickture.width();
	dPictureHeight = drtPickture.height();

	double dMaxLength = max(dPictureWidth, dPictureHeight);
	// (MAX_RENDER_WIDTH - 1000) 기준, 고정 Scale 적용
//	double dResolution = dMaxLength / (MAX_RENDER_WIDTH - 1000);//-1000으로 해서 안정적으로 저장을 한다.
	double dResolution = dMaxLength / (MAX_RENDER_WIDTH / 4); // Saving Time to Create

	D2D1_RECT_F fptPicture;
	fptPicture = D2D1::RectF(DoubleToFloat(drtPickture.left), DoubleToFloat(drtPickture.top),
		DoubleToFloat(drtPickture.right), DoubleToFloat(drtPickture.bottom));

	m_pSaveLayer->SetPictureBox(fptPicture, dResolution, TRUE);	

	CString strFullPathName = _T("");

	UINT32 nRet;
	
	CString strImageType = _T("bmp");// m_pSystemSpec->sysBasic.strImageType

	//Only Current Step
//	strFullPathName.Format(_T("%sProfile.%s"), strRootPath, m_pSystemSpec->sysBasic.strImageType);
//	nRet = m_pSaveLayer->SaveProfile(strFullPathName, pStep, rotMirrArr, FALSE, FALSE);
	
	if (bIsPanel)
	{
		//Save SubStep
		strFullPathName.Format(_T("%sProfile_S1.%s"), strRootPath, strImageType);
		nRet |= m_pSaveLayer->SaveProfile(strFullPathName, pStep, rotMirrArr, TRUE, FALSE);

		//Save SubSubStep
		if (m_bIs4Step)
		{
			strFullPathName.Format(_T("%sProfile_S2.%s"), strRootPath, strImageType);
			nRet |= m_pSaveLayer->SaveProfile(strFullPathName, pStep, rotMirrArr, TRUE, TRUE, FALSE);

			strFullPathName.Format(_T("%sProfile_S3.%s"), strRootPath, strImageType);
			nRet |= m_pSaveLayer->SaveProfile(strFullPathName, pStep, rotMirrArr, TRUE, TRUE, TRUE);
		}
		else
		{

			strFullPathName.Format(_T("%sProfile_S2.%s"), strRootPath, strImageType);
			nRet |= m_pSaveLayer->SaveProfile(strFullPathName, pStep, rotMirrArr, TRUE, TRUE);
		}
	}
	else
	{
		if (strStepName.CompareNoCase(_T("STRIP")) == 0)
		{
			//Save SubStep
			strFullPathName.Format(_T("%sProfile_S1.%s"), strRootPath, strImageType);
			nRet |= m_pSaveLayer->SaveProfile(strFullPathName, pStep, rotMirrArr, TRUE, FALSE);
		}
	}

	return nRet;
}

UINT32 CDirectView::SaveUnitLayer_in_Panel(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const Orient &eOrient, IN const double &dResolution, IN const CString &strPathName)
{
	if (m_pSaveLayer == nullptr) return RESULT_BAD;
	if (m_pSystemSpec == nullptr) return RESULT_BAD;

	CStep *pStep = m_pJobData->m_arrStep.GetAt(iStepIdx);
	if (pStep == nullptr) return RESULT_BAD;

	CLayer *pPatternLayer = nullptr;

	CRotMirrArr *rotMirrArr = new CRotMirrArr;
	rotMirrArr->InputData(0.0, 0.0, eOrient, m_pUserSetInfo->mirrorDir);

	CString strImageType = m_pSystemSpec->sysBasic.strImageType;

	CString strFullPathName;
	CString strPath;

	if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS)
	{
		strPath.Format(_T("%s\\Unit_AlignMark"), strPathName);
		SHCreateDirectoryEx(NULL, strPath, NULL);
	}
	else
	{
		strPath.Format(_T("%s\\AlignMark"), strPathName);
		SHCreateDirectoryEx(NULL, strPath, NULL);
	}


	LayerSet_t* pLayerSet_Pattern = nullptr;
	vector<RECTD> vcAlign;
	int nLayerSetCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());
	for (int i = 0; i < nLayerSetCount; i++)
	{
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
		if (!pLayerSet->bCheck)
			continue;

		if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS)
		{
			if (pLayerSet->strDefine.CompareNoCase(MEASURE_LAYER_TXT) != 0)
				continue;
		}
		else
		{
			if (pLayerSet->strDefine.CompareNoCase(PATTERN_LAYER_TXT) != 0)
				continue;
		}

		pLayerSet_Pattern = pLayerSet;

		COLORREF	color = pLayerSet->color;
		pLayerSet->color = RGB(255, 255, 255);

		//Get Unit Rect
		RECTD drtRect;
		for (auto it : m_pCellInfo->vcUnit_in_Cell)
		{
			auto vcUnitInfo = it.second;
			if (vcUnitInfo.size() > 0)
			{
				drtRect = vcUnitInfo[0]->rcProfile_In_Panel;
				break;
			}
		}

		drtRect.NormalizeRectD();

		D2D1_RECT_F fptPicture = D2D1::RectF(DoubleToFloat(drtRect.left), DoubleToFloat(drtRect.top),
			DoubleToFloat(drtRect.right), DoubleToFloat(drtRect.bottom));

		if (S_OK != m_pSaveLayer->SetPictureBox(fptPicture, dResolution))
			return RESULT_BAD;

		if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS)
		{
			strFullPathName.Format(_T("%s\\%s_Unit.%s"), strPath, pLayerSet->strLayer, strImageType);
		}
		else
		{
			strFullPathName.Format(_T("%s\\%s_Unit.%s"), strPath, pLayerSet->strLayer, strImageType);
		}
		if (RESULT_GOOD != m_pSaveLayer->SaveImage(strFullPathName, 0,
			pStep, pLayerSet, &m_pJobData->m_arrFont, rotMirrArr, m_pUserSetInfo, FALSE))
		{
			if (rotMirrArr != nullptr) delete rotMirrArr;

			pLayerSet->color = color;
			return RESULT_BAD;
		}

		pLayerSet->color = color;
	}
	
	delete rotMirrArr;
	return RESULT_GOOD;
}

UINT32 CDirectView::SavePanelLayer_NSIS(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle,
	IN const double &dResolution, IN const CString &strPathName)
{
	if (m_pSystemSpec == nullptr) return RESULT_BAD;

	CStep *pStep = m_pJobData->m_arrStep.GetAt(iStepIdx);
	if (pStep == nullptr) return RESULT_BAD;

	CRotMirrArr *rotMirrArr = new CRotMirrArr;
	rotMirrArr->InputData(0.0, 0.0, Orient::NoMir0Deg);

	D2D1_RECT_F fptPicture;

	if (m_pSystemSpec->sysBasic.bProfileMode)
	{
		fptPicture = D2D1::RectF(DoubleToFloat(pStep->m_Profile.m_MinMax.left), DoubleToFloat(pStep->m_Profile.m_MinMax.top),
			DoubleToFloat(pStep->m_Profile.m_MinMax.right), DoubleToFloat(pStep->m_Profile.m_MinMax.bottom));
	}
	else
	{
		fptPicture = D2D1::RectF(DoubleToFloat(pStep->m_FeatureMinMax.left), DoubleToFloat(pStep->m_FeatureMinMax.top),
			DoubleToFloat(pStep->m_FeatureMinMax.right), DoubleToFloat(pStep->m_FeatureMinMax.bottom));
	}

	
	// 22-07-05 Viewer 각도 변환 후 Master Data 출력 

// 	D2D1::Matrix3x2F matRotate = D2D1::Matrix3x2F::Rotation(static_cast<float>(m_pUserSetInfo->dAngle),
// 		CDrawFunction::GetRectCenter(fptPicture)); 
	

	D2D1::Matrix3x2F matRotate = D2D1::Matrix3x2F::Rotation(static_cast<float>(0),
			CDrawFunction::GetRectCenter(fptPicture));

	D2D1_RECT_F frtTempBox = CDrawFunction::DetansformRect(fptPicture, matRotate, nullptr);

	if (S_OK != m_pSaveLayer->SetPictureBox(frtTempBox, dResolution))
		return RESULT_BAD;

	int nLayerSetCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());

	for (int i = 0; i < nLayerSetCount; i++)
	{
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
		if (!pLayerSet->bCheck)
			continue;

		if (pLayerSet->strDefine.CompareNoCase(PATTERN_LAYER_TXT) != 0)
			continue;

		m_pSaveLayer->SetDrawMode(eDrawLevel::ePanelLv);

		CString strFullPathName;
		strFullPathName.Format(_T("%s\\AlignMark\\Panel%d.%s"), strPathName, iIndex + 1, m_pSystemSpec->sysBasic.strImageType);
		if (RESULT_GOOD != m_pSaveLayer->SaveImage(strFullPathName, dCamAngle,
			pStep, pLayerSet, &m_pJobData->m_arrFont, rotMirrArr, m_pUserSetInfo, TRUE))
		{
			if (rotMirrArr != nullptr) delete rotMirrArr;
			return RESULT_BAD;
		}
	}
	

	if (rotMirrArr != nullptr) delete rotMirrArr;

	return RESULT_GOOD;
}

UINT32 CDirectView::SavePanelLayer_AOI(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle,
	IN const double &dResolution, IN const CString &strPathName)
{
	if (m_pSystemSpec == nullptr || m_pSaveLayerOp == nullptr || m_pJobData == nullptr) return RESULT_BAD;
	if (m_pUserLayerSetInfo == nullptr || m_pUserLayerSetInfo->iSelectedNum <= 0) return RESULT_BAD;
	
	CPerfTimerEx timerEx;
	timerEx.StartTimer();

//	if (RESULT_GOOD != _SaveSplitImage(iStepIdx, iIndex, dCamAngle, dResolution, strPathName))
//		return RESULT_BAD;
	
	if (RESULT_GOOD != _SaveSplitImage_V2(iStepIdx, iIndex, dCamAngle, dResolution, strPathName))
		return RESULT_BAD;

	CString strLog;
	double dTime = timerEx.StopTimer(CTimerUnit_en::EN_MIL_SEC);
	strLog.Format(_T("\n _SaveSplitImage_V2 : Processing Time[%.2f ms][%.2f min]"), dTime, dTime / 1000. / 60.);
	OutputDebugString(strLog);
	
	timerEx.StartTimer();

	//Merge 루틴 변경 테스트로 인하여 주석처리함
// 	if (RESULT_GOOD != _MergeSplitImage(iStepIdx, dResolution, strPathName))
// 		return RESULT_BAD;

	dTime = timerEx.StopTimer(CTimerUnit_en::EN_MIL_SEC);
	strLog.Format(_T("\n _MergeSplitImage : Processing Time[%.2f ms]"), dTime);
	OutputDebugString(strLog);

	timerEx.StartTimer();

//	if (RESULT_GOOD != _ConvRegion(iStepIdx, iIndex, dCamAngle, dResolution, strPathName))
//		return RESULT_BAD;

	//개별 Region 저장//속도 문제로 현재는 사용하지 않음
// 	if (RESULT_GOOD != _SaveRegion(iStepIdx, iIndex, dCamAngle, dResolution, strPathName))
// 		return RESULT_BAD;

	dTime = timerEx.StopTimer(CTimerUnit_en::EN_MIL_SEC);
	strLog.Format(_T("\n _ConvRegion : Processing Time[%.2f ms]"), dTime);
	OutputDebugString(strLog);
	
	return RESULT_GOOD;
}

UINT32 CDirectView::_SaveSplitImage(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle,
	IN const double &dResolution, IN const CString &strPathName)
{
	if (m_pUserSetInfo == nullptr) return RESULT_BAD;
	if (m_pSystemSpec == nullptr) return RESULT_BAD;

	bool bExceptDummy = m_pSystemSpec->sysBasic.bExceptDummy;

	bool bIsAVI = false;
	if (m_pUserSetInfo->mcType == eMachineType::eAVI)
		bIsAVI = true;

	CStep *pStep = m_pJobData->m_arrStep.GetAt(iStepIdx);
	if (pStep == nullptr) return RESULT_BAD;

	if (m_pUserLayerSetInfo->vcUnitStepIdx.size() <= 0) return RESULT_BAD;

	UINT32 nSwathCount;
	std::vector<SaveRect> vcSwath;
	SaveRect stTempSave;
	if (m_pUserSetInfo->mcType == eMachineType::eAOI)
	{//Cell 
		nSwathCount = static_cast<UINT32>(m_pCellInfo->vcCellData.size());
		if (nSwathCount <= 0) return RESULT_BAD;

		for (auto it : m_pCellInfo->vcCellData)
		{
			D2D1_RECT_F frtSwath = D2D1::RectF(DoubleToFloat(it->rcRectMM_In_Panel.left),
				DoubleToFloat(it->rcRectMM_In_Panel.top),
				DoubleToFloat(it->rcRectMM_In_Panel.right),
				DoubleToFloat(it->rcRectMM_In_Panel.bottom));

			frtSwath = CDrawFunction::NormalizeRect(frtSwath);

			stTempSave.frtRect = frtSwath;
			stTempSave.strFileName.Format(_T("S%d_R%d_C%d"), it->swathIdx + 1, it->fovIdx.y + 1, it->fovIdx.x + 1);

			vcSwath.emplace_back(stTempSave);
		}
	}
	else
	{//Swath
		nSwathCount = static_cast<UINT32>(m_pFovInfo->vcFovData.size());
		if (nSwathCount <= 0) return RESULT_BAD;

		for (auto it : m_pFovInfo->vcFovData)
		{
			D2D1_RECT_F frtSwath = D2D1::RectF(DoubleToFloat(it->rcRectMM_In_Panel.left),
				DoubleToFloat(it->rcRectMM_In_Panel.top),
				DoubleToFloat(it->rcRectMM_In_Panel.right),
				DoubleToFloat(it->rcRectMM_In_Panel.bottom));

			frtSwath = CDrawFunction::NormalizeRect(frtSwath);

			stTempSave.frtRect = frtSwath;
			stTempSave.strFileName.Format(_T("S%d"), it->swathIdx + 1);

			vcSwath.emplace_back(stTempSave);
		}
	}

	CString strRootPath = strPathName + _T("\\Layers");
	CreateDirectory(strRootPath, NULL);
	strRootPath = strPathName + _T("\\Layers\\Cell");
	CreateDirectory(strRootPath, NULL);

	CUtils Utils;
	UINT32 iCoreNum = max((UINT32)1, Utils.GetCoreCount() - 1);

	int iTotLayerCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());

	UINT32 iThreadIdx = 0;
	UINT32 iAddLayerNum = (UINT32)(static_cast<int>(SaveMode::SaveMode_Mask) - 1);

	if (bIsAVI) iAddLayerNum = 0;

	//1. Make Layer Images
	m_pSaveLayerOp->SetJobFile(m_pJobData);
	m_pSaveLayerOp->StartThreads(nSwathCount * (m_pUserLayerSetInfo->iSelectedNum + iAddLayerNum), iCoreNum);

	UINT32 iLayerIdx = 0;
	LayerSet_t* pPatternLayerSet = nullptr;
	std::vector<COLORREF> vcColor;
	for (UINT32 nSwath = 0; nSwath < nSwathCount; nSwath++)
	{
		iLayerIdx = 0;
		for (int i = 0; i < iTotLayerCount; i++)
		{
			LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
			if (pLayerSet == nullptr || !pLayerSet->bCheck) continue;

			SaveMode eSaveMode = SaveMode::SaveMode_All;

			if (pLayerSet->strDefine.CompareNoCase(PATTERN_LAYER_TXT) == 0)
			{
				pPatternLayerSet = pLayerSet;
				if (!bIsAVI)
					eSaveMode = SaveMode::SaveMode_LineArc;
			}

			CString strSavePath = strRootPath + _T("\\") + pLayerSet->strLayer;
			if (nSwath == 0)
			{
				vcColor.emplace_back(pLayerSet->color);
				pLayerSet->color = RGB(255, 255, 255);
			}

			CSaveLayerJob* pstJob = new CSaveLayerJob;
			if (RESULT_GOOD != pstJob->SetData_SaveLayer(vcSwath[nSwath].strFileName, iLayerIdx, iThreadIdx, pStep, pLayerSet, vcSwath[nSwath].frtRect, m_pUserSetInfo, &m_pJobData->m_arrFont,
				strSavePath, m_pSystemSpec->sysBasic.strImageType, dResolution, m_pSystemSpec->sysBasic.McType, m_pFactory, m_pD2DContext, m_pImageFactory, eSaveMode, eDrawLevel::ePanelLv, bExceptDummy))
			{
				continue;
			}

			m_pSaveLayerOp->AddJob(pstJob);
			iLayerIdx++;
			iThreadIdx++;
		}
	}

	if (!bIsAVI && pPatternLayerSet)
	{
		UINT32 iSubLayerIdx = iLayerIdx;
		for (UINT32 nSwath = 0; nSwath < nSwathCount; nSwath++)
		{
			iSubLayerIdx = iLayerIdx;
			for (int nSaveMode = static_cast<int>(SaveMode::SaveMode_Mask) - 1; nSaveMode >= static_cast<int>(SaveMode::SaveMode_All); nSaveMode--)
			{
				if (nSaveMode == static_cast<int>(SaveMode::SaveMode_LineArc))
					continue;

				CString strSavePath = strRootPath + _T("\\") + pPatternLayerSet->strLayer;

				CSaveLayerJob* pstJob = new CSaveLayerJob;
				if (RESULT_GOOD != pstJob->SetData_SaveLayer(vcSwath[nSwath].strFileName, iSubLayerIdx, iThreadIdx, pStep, pPatternLayerSet, vcSwath[nSwath].frtRect, m_pUserSetInfo, &m_pJobData->m_arrFont,
					strSavePath, m_pSystemSpec->sysBasic.strImageType, dResolution, m_pSystemSpec->sysBasic.McType, m_pFactory, m_pD2DContext, m_pImageFactory, static_cast<SaveMode>(nSaveMode), eDrawLevel::ePanelLv, bExceptDummy))
				{
					continue;
				}

				m_pSaveLayerOp->AddJob(pstJob);
				iSubLayerIdx++;
				iThreadIdx++;
			}
		}

		//All
// 		for (UINT32 nSwath = 0; nSwath < nSwathCount; nSwath++)
// 		{
// 			CString strSavePath = strRootPath + _T("\\") + pPatternLayerSet->strLayer;
// 
// 			CSaveLayerJob* pstJob = new CSaveLayerJob;
// 			if (RESULT_GOOD != pstJob->SetData_SaveLayer(vcSwath[nSwath].strFileName, iSubLayerIdx, iThreadIdx, pStep, pPatternLayerSet, vcSwath[nSwath].frtRect, m_pUserSetInfo, &m_pJobData->m_arrFont,
// 				strSavePath, m_pSystemSpec->sysBasic.strImageType, dResolution, m_pSystemSpec->sysBasic.McType, m_pFactory, m_pD2DContext, m_pImageFactory))
// 			{
// 				continue;
// 			}
// 
// 			m_pSaveLayerOp->AddJob(pstJob);
// 			iThreadIdx++;
// 		}
	}

	if (m_pSaveLayerOp->RunAllJobs())
	{
		m_pSaveLayerOp->WaitForJobFinish();
		m_pSaveLayerOp->ResetAllJobEvents();
	}

	if (!m_pSaveLayerOp->StopAllThreads())
		return RESULT_BAD;

	for (int i = 0, j = 0; i < iTotLayerCount; i++)
	{
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
		if (pLayerSet == nullptr || !pLayerSet->bCheck) continue;

		pLayerSet->color = vcColor[j++];
	}

	vcSwath.clear();
	vcColor.clear();

	_ClearGeo();
	SetRedrawGeo();

	return RESULT_GOOD;
}

UINT32 CDirectView::_SaveSplitImage_V2(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle,
	IN const double &dResolution, IN const CString &strPathName)
{
	if (m_pUserSetInfo == nullptr) return RESULT_BAD;
	if (m_pSystemSpec == nullptr) return RESULT_BAD;
	if (m_pFovInfo == nullptr) return RESULT_BAD;
	if (m_pCellInfo == nullptr) return RESULT_BAD;		

	int iSwathNumInBlock = static_cast<int>(m_pFovInfo->stBasic.swathNumInBlock);
	int	iCellRowInBlock = static_cast<int>(m_pCellInfo->stBasic.cell_In_Block.cy);
	if (iSwathNumInBlock <= 0 || iCellRowInBlock <= 0) return RESULT_BAD;

	bool bExceptDummy = m_pSystemSpec->sysBasic.bExceptDummy;

	bool bIsBlockMode = false;
	if (bExceptDummy && m_pSystemSpec->sysBasic.MasterLevel == eMasterLevel::eBlockLv)
		bIsBlockMode = true;

	bool bIsAVI = false;
	if (m_pUserSetInfo->mcType == eMachineType::eAVI)
		bIsAVI = true;

	CStep *pStep = m_pJobData->m_arrStep.GetAt(iStepIdx);
	if (pStep == nullptr) return RESULT_BAD;

	if (m_pUserLayerSetInfo->vcUnitStepIdx.size() <= 0) return RESULT_BAD;

	UINT32 nSwathCount;
	std::vector<SaveRect> vcSwath;
	SaveRect stTempSave;
	if (m_pUserSetInfo->mcType == eMachineType::eAOI)
	{//Cell 
		nSwathCount = static_cast<UINT32>(m_pCellInfo->vcCellData.size());
		if (nSwathCount <= 0) return RESULT_BAD;

		for (auto it : m_pCellInfo->vcCellData)
		{
			if (bIsBlockMode)
			{
				if ((int)it->swathIdx >= iSwathNumInBlock) 
					continue;

				if (it->fovIdx.y / iCellRowInBlock)	
					continue;
			}

			D2D1_RECT_F frtSwath = D2D1::RectF(DoubleToFloat(it->rcRectMM_In_Panel.left),
				DoubleToFloat(it->rcRectMM_In_Panel.top),
				DoubleToFloat(it->rcRectMM_In_Panel.right),
				DoubleToFloat(it->rcRectMM_In_Panel.bottom));

			if (fabs(frtSwath.right - frtSwath.left) < EPSILON_DELTA2 ||
				fabs(frtSwath.bottom - frtSwath.top) < EPSILON_DELTA2)
			{
#ifdef _DEBUG
				AfxMessageBox(_T("셀 정보를 확인하여 주세요."));
#endif
				return RESULT_BAD;
			}

			frtSwath = CDrawFunction::NormalizeRect(frtSwath);

			stTempSave.frtRect = frtSwath;
			stTempSave.strFileName.Format(_T("S%d_R%d_C%d"), it->swathIdx + 1, it->fovIdx.y + 1, it->fovIdx.x + 1);

			vcSwath.emplace_back(stTempSave);
		}

		nSwathCount = static_cast<UINT32>(vcSwath.size());
		if (nSwathCount <= 0) return RESULT_BAD;
	}
	else
	{//Swath
		nSwathCount = static_cast<UINT32>(m_pFovInfo->vcFovData.size());
		if (nSwathCount <= 0) return RESULT_BAD;

		for (auto it : m_pFovInfo->vcFovData)
		{
			D2D1_RECT_F frtSwath = D2D1::RectF(DoubleToFloat(it->rcRectMM_In_Panel.left),
				DoubleToFloat(it->rcRectMM_In_Panel.top),
				DoubleToFloat(it->rcRectMM_In_Panel.right),
				DoubleToFloat(it->rcRectMM_In_Panel.bottom));

			if (fabs(frtSwath.right - frtSwath.left) < EPSILON_DELTA2 ||
				fabs(frtSwath.bottom - frtSwath.top) < EPSILON_DELTA2)
			{
#ifdef _DEBUG
				AfxMessageBox(_T("셀 정보를 확인하여 주세요."));
#endif
				return RESULT_BAD;
			}

			frtSwath = CDrawFunction::NormalizeRect(frtSwath);

			stTempSave.frtRect = frtSwath;
			stTempSave.strFileName.Format(_T("S%d"), it->swathIdx + 1);

			vcSwath.emplace_back(stTempSave);
		}
	}

	CString strRootPath = strPathName + _T("\\Layers");
	CreateDirectory(strRootPath, NULL);
	strRootPath = strPathName + _T("\\Layers\\Cell");
	CreateDirectory(strRootPath, NULL);

	CUtils Utils;
	UINT32 iCoreNum;
	if (bIsAVI == true)
	{
		iCoreNum = 3;
	}
	else
	{
		iCoreNum = max((UINT32)1, Utils.GetCoreCount() - 1);
	}
	

	int iTotLayerCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());

	UINT32 iThreadIdx = 0;
	UINT32 iAddLayerNum = (UINT32)(static_cast<int>(SaveMode::SaveMode_Mask) - 1);

	if (bIsAVI) iAddLayerNum = 0;

	//1. Make Layer Images
	m_pSaveLayerOp->SetJobFile(m_pJobData);
	m_pSaveLayerOp->StartThreads(nSwathCount * (m_pUserLayerSetInfo->iSelectedNum + iAddLayerNum), iCoreNum);

	UINT32 iLayerIdx = 0;
	std::vector<COLORREF> vcColor;
	for (UINT32 nSwath = 0; nSwath < nSwathCount; nSwath++)
	{
		iLayerIdx = 0;
		for (int i = 0; i < iTotLayerCount; i++)
		{
			LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
			if (pLayerSet == nullptr || !pLayerSet->bCheck) continue;

			SaveMode eSaveMode = SaveMode::SaveMode_All;			

			CString strSavePath = strRootPath + _T("\\") + pLayerSet->strLayer;
			if (nSwath == 0)
			{
				vcColor.emplace_back(pLayerSet->color);
				pLayerSet->color = RGB(255, 255, 255);
			}

			if (pLayerSet->strDefine.CompareNoCase(PATTERN_LAYER_TXT) == 0 && !bIsAVI)
			{
				for (int nSaveMode = static_cast<int>(SaveMode::SaveMode_Mask) - 1; nSaveMode >= static_cast<int>(SaveMode::SaveMode_All); nSaveMode--)
				{
					CSaveLayerJob* pstJob = new CSaveLayerJob;
					if (RESULT_GOOD != pstJob->SetData_SaveLayer(vcSwath[nSwath].strFileName, iLayerIdx, iThreadIdx, pStep, pLayerSet, vcSwath[nSwath].frtRect, m_pUserSetInfo, &m_pJobData->m_arrFont,
						strSavePath, m_pSystemSpec->sysBasic.strImageType, dResolution, m_pSystemSpec->sysBasic.McType, m_pFactory, m_pD2DContext, m_pImageFactory, static_cast<SaveMode>(nSaveMode), eDrawLevel::ePanelLv, bExceptDummy))
					{
						continue;
					}

					m_pSaveLayerOp->AddJob(pstJob);
					iThreadIdx++;
				}

			}
			else
			{
				CSaveLayerJob* pstJob = new CSaveLayerJob;
				if (RESULT_GOOD != pstJob->SetData_SaveLayer(vcSwath[nSwath].strFileName, iLayerIdx, iThreadIdx, pStep, pLayerSet, vcSwath[nSwath].frtRect, m_pUserSetInfo, &m_pJobData->m_arrFont,
					strSavePath, m_pSystemSpec->sysBasic.strImageType, dResolution, m_pSystemSpec->sysBasic.McType, m_pFactory, m_pD2DContext, m_pImageFactory, eSaveMode, eDrawLevel::ePanelLv, bExceptDummy))
				{
					continue;
				}

				m_pSaveLayerOp->AddJob(pstJob);
				iThreadIdx++;
			}

			iLayerIdx++;
		}
	}
	
	if (m_pSaveLayerOp->RunAllJobs())
	{
		m_pSaveLayerOp->WaitForJobFinish();
		m_pSaveLayerOp->ResetAllJobEvents();
	}

	if (!m_pSaveLayerOp->StopAllThreads())
		return RESULT_BAD;

	for (int i = 0, j = 0; i < iTotLayerCount; i++)
	{
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
		if (pLayerSet == nullptr || !pLayerSet->bCheck) continue;

		pLayerSet->color = vcColor[j++];
	}

	vcSwath.clear();
	vcColor.clear();

	_ClearGeo();
	SetRedrawGeo();

	return RESULT_GOOD;
}

UINT32 CDirectView::SavePanelLayer_FeatureSub(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle,
	IN const double &dResolution, IN const CString &strPathName, int eLineOption, vector <LINEWIDTH> &vecLineWidth)
{
	//_SaveSplitImage_LineArcSub(iStepIdx, iIndex, dCamAngle, dResolution, strPathName, eLineOption, vecLineWidth);
	_SaveSplitImage_LineArcSub_V2(iStepIdx, iIndex, dCamAngle, dResolution, strPathName, eLineOption, vecLineWidth);

	return RESULT_GOOD;
}

UINT32 CDirectView::SavePanelLayer_FeatureSub_V3(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double &dResolution, IN const CString &strPathName, vector<vector <LINEWIDTH>> &vecvecLineWidth)
{
	_SaveSplitImage_LineArcSub_V3(iStepIdx, iIndex, dResolution, strPathName, vecvecLineWidth);

	return RESULT_GOOD;
}

UINT32 CDirectView::_SaveSplitImage_LineArcSub_V3(IN const INT32 iStepIdx, IN const UINT32 iIndex, 
	IN const double &dResolution, IN const CString &strPathName, vector<vector <LINEWIDTH>> &vecvecLineWidth)
{//vecvecLineWidth [LineOption][LineWidth]
	if (m_pUserSetInfo == nullptr) return RESULT_BAD;
	if (m_pSystemSpec == nullptr) return RESULT_BAD;
	if (m_pFovInfo == nullptr) return RESULT_BAD;
	if (m_pCellInfo == nullptr) return RESULT_BAD;

	int iSwathNumInBlock = static_cast<int>(m_pFovInfo->stBasic.swathNumInBlock);
	int	iCellRowInBlock = static_cast<int>(m_pCellInfo->stBasic.cell_In_Block.cy);
	if (iSwathNumInBlock <= 0 || iCellRowInBlock <= 0) return RESULT_BAD;

	bool bExceptDummy = m_pSystemSpec->sysBasic.bExceptDummy;

	bool bIsBlockMode = false;
	if (bExceptDummy && m_pSystemSpec->sysBasic.MasterLevel == eMasterLevel::eBlockLv)
		bIsBlockMode = true;

	bool bIsAVI = false;
	if (m_pUserSetInfo->mcType == eMachineType::eAVI)
		bIsAVI = true;

	CStep *pStep = m_pJobData->m_arrStep.GetAt(iStepIdx);
	if (pStep == nullptr) return RESULT_BAD;

	if (m_pUserLayerSetInfo->vcUnitStepIdx.size() <= 0) return RESULT_BAD;

	UINT32 nSwathCount;
	std::vector<SaveRect> vcSwath;
	SaveRect stTempSave;
	if (m_pUserSetInfo->mcType == eMachineType::eAOI)
	{//Cell 
		nSwathCount = static_cast<UINT32>(m_pCellInfo->vcCellData.size());
		if (nSwathCount <= 0) return RESULT_BAD;

		for (auto it : m_pCellInfo->vcCellData)
		{
			if (bIsBlockMode)
			{
				if ((int)it->swathIdx >= iSwathNumInBlock)
					continue;

				if (it->fovIdx.y / iCellRowInBlock)
					continue;
			}

			D2D1_RECT_F frtSwath = D2D1::RectF(DoubleToFloat(it->rcRectMM_In_Panel.left),
				DoubleToFloat(it->rcRectMM_In_Panel.top),
				DoubleToFloat(it->rcRectMM_In_Panel.right),
				DoubleToFloat(it->rcRectMM_In_Panel.bottom));

			if (fabs(frtSwath.right - frtSwath.left) < EPSILON_DELTA2 ||
				fabs(frtSwath.bottom - frtSwath.top) < EPSILON_DELTA2)
			{
#ifdef _DEBUG
				AfxMessageBox(_T("셀 정보를 확인하여 주세요."));
#endif
				return RESULT_BAD;
			}

			frtSwath = CDrawFunction::NormalizeRect(frtSwath);

			stTempSave.frtRect = frtSwath;
			stTempSave.strFileName.Format(_T("S%d_R%d_C%d"), it->swathIdx + 1, it->fovIdx.y + 1, it->fovIdx.x + 1);

			vcSwath.emplace_back(stTempSave);
		}

		nSwathCount = static_cast<UINT32>(vcSwath.size());
		if (nSwathCount <= 0) return RESULT_BAD;
	}
	else
	{//Swath
		nSwathCount = static_cast<UINT32>(m_pFovInfo->vcFovData.size());
		if (nSwathCount <= 0) return RESULT_BAD;

		for (auto it : m_pFovInfo->vcFovData)
		{
			D2D1_RECT_F frtSwath = D2D1::RectF(DoubleToFloat(it->rcRectMM_In_Panel.left),
				DoubleToFloat(it->rcRectMM_In_Panel.top),
				DoubleToFloat(it->rcRectMM_In_Panel.right),
				DoubleToFloat(it->rcRectMM_In_Panel.bottom));

			if (fabs(frtSwath.right - frtSwath.left) < EPSILON_DELTA2 ||
				fabs(frtSwath.bottom - frtSwath.top) < EPSILON_DELTA2)
			{
#ifdef _DEBUG
				AfxMessageBox(_T("셀 정보를 확인하여 주세요."));
#endif
				return RESULT_BAD;
			}

			frtSwath = CDrawFunction::NormalizeRect(frtSwath);

			stTempSave.frtRect = frtSwath;
			stTempSave.strFileName.Format(_T("S%d"), it->swathIdx + 1);

			vcSwath.emplace_back(stTempSave);
		}
	}


	CString strRootPath = strPathName + _T("\\Layers");
	CreateDirectory(strRootPath, NULL);
	strRootPath = strPathName + _T("\\Layers\\Cell");
	CreateDirectory(strRootPath, NULL);

	CUtils Utils;
	UINT32 iCoreNum = max((UINT32)1, Utils.GetCoreCount() - 1);

	int iTotLayerCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());

	UINT32 iThreadIdx = 0;
	UINT32 iAddLayerNum = (UINT32)(static_cast<int>(SaveMode::SaveMode_Mask) - 1);

	if (bIsAVI) iAddLayerNum = 0;

	//Get Pattern Layer Prt
	//////////////////////////////////////////////////////////////////////////
	LayerSet_t* pLayerSet_Pattern = nullptr;
	for (int i = 0; i < iTotLayerCount; i++)
	{
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
		if (pLayerSet == nullptr || !pLayerSet->bCheck) continue;

		SaveMode eSaveMode = SaveMode::SaveMode_All;

		if (pLayerSet->strDefine.CompareNoCase(PATTERN_LAYER_TXT) == 0)
		{
			pLayerSet_Pattern = pLayerSet;
			break;
		}
	}
	if (pLayerSet_Pattern == nullptr) return RESULT_BAD;
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	Orient		eOrient = Orient::NoMir0Deg;
	CDrawFunction::GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, eOrient);

	// 	CRotMirrArr *pRotMirrArr = new CRotMirrArr;
	// 	pRotMirrArr->InputData(0.0, 0.0, eOrient, m_pUserSetInfo->mirrorDir);

	

	vector<vector<vector<vector<CFeature *>>>> vecvecvecvecFilteredFeature;//[swath][LineOption][lineWidth][Feature]
	vecvecvecvecFilteredFeature.resize(nSwathCount);

	int nLineOptionCount = static_cast<int>(vecvecLineWidth.size());
	for (UINT32 nSwath = 0; nSwath < nSwathCount; nSwath++)
	{
		_MakeFilterFeatureBuffer_V3(vecvecvecvecFilteredFeature[nSwath], vecvecLineWidth);
	}


	m_pSaveLayerOp->SetJobFile(m_pJobData);
	m_pSaveLayerOp->StartThreads(nSwathCount, iCoreNum);

	iThreadIdx = 0;
	for (UINT32 nSwath = 0; nSwath < nSwathCount; nSwath++)
	{
		RECTD drtView_mm = RECTD(vcSwath[nSwath].frtRect.left, vcSwath[nSwath].frtRect.top, vcSwath[nSwath].frtRect.right, vcSwath[nSwath].frtRect.bottom);
		drtView_mm.NormalizeRectD();

		//_GetFilteredFeature(vecvecvecFilteredFeature[nSwath], pStep, pLayerSet_Pattern, pRotMirrArr, eLineOption, vecLineWidth, drtView_mm);
		CSaveLayerJob* pstJob = new CSaveLayerJob;

		if (RESULT_GOOD != pstJob->SetData_GetFilteredFeature_V3(this, &vecvecvecvecFilteredFeature[nSwath], pStep, pLayerSet_Pattern, m_pUserSetInfo,
			&vecvecLineWidth, drtView_mm, iThreadIdx))
		{
			continue;
		}
		m_pSaveLayerOp->AddJob(pstJob);
		iThreadIdx++;
	}

	// 	delete pRotMirrArr;
	// 	pRotMirrArr = nullptr;
		//////////////////////////////////////////////////////////////////////////

	if (m_pSaveLayerOp->RunAllJobs())
	{
		m_pSaveLayerOp->WaitForJobFinish();
		m_pSaveLayerOp->ResetAllJobEvents();
	}

	if (!m_pSaveLayerOp->StopAllThreads())
		return RESULT_BAD;

	//Cal thread Count
	int nThreadCount = 0;
	for (UINT32 nSwath = 0; nSwath < nSwathCount; nSwath++)
	{
		int nLineOptionCount = static_cast<int>(vecvecvecvecFilteredFeature[nSwath].size());
		for (int nLineOption = 0; nLineOption < nLineOptionCount; nLineOption++)
		{
			int nWidthCount = static_cast<int>(vecvecvecvecFilteredFeature[nSwath][nLineOption].size());
			for (int nWidth = 0; nWidth < nWidthCount; nWidth++)
			{
				nThreadCount++;
			}
		}
	}

	//1. Make Layer Images
	m_pSaveLayerOp->SetJobFile(m_pJobData);
	m_pSaveLayerOp->StartThreads(nThreadCount, iCoreNum);

	iThreadIdx = 0;
	UINT32 iLayerIdx = 0;
	std::vector<COLORREF> vcColor;
	CString strPatternLayerName = _T("");
	for (UINT32 nSwath = 0; nSwath < nSwathCount; nSwath++)
	{
		iLayerIdx = 0;
		for (int i = 0; i < iTotLayerCount; i++)
		{
			LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
			if (pLayerSet == nullptr || !pLayerSet->bCheck) continue;

			SaveMode eSaveMode = SaveMode::SaveMode_All;

			CString strSavePath = strRootPath + _T("\\") + pLayerSet->strLayer;
			if (nSwath == 0)
			{
				vcColor.emplace_back(pLayerSet->color);
				pLayerSet->color = RGB(255, 255, 255);
			}

			if (pLayerSet->strDefine.CompareNoCase(PATTERN_LAYER_TXT) == 0 && !bIsAVI)
			{
				strPatternLayerName = pLayerSet->strLayer;

				int nLineOptionCount = static_cast<int>(vecvecvecvecFilteredFeature[nSwath].size());
				for (int nLineOption = 0; nLineOption < nLineOptionCount; nLineOption++)
				{
					int nSaveMode;
					switch (nLineOption)
					{
					case eLineWidthOption::eLinePos:
						nSaveMode = static_cast<int>(SaveMode::SaveMode_Line_Pos_Sub);
						break;
					case eLineWidthOption::eLineNeg:
						nSaveMode = static_cast<int>(SaveMode::SaveMode_Line_Neg_Sub);
						break;
					case eLineWidthOption::eArcPos:
						nSaveMode = static_cast<int>(SaveMode::SaveMode_Arc_Pos_Sub);
						break;
					case eLineWidthOption::eArcNeg:
						nSaveMode = static_cast<int>(SaveMode::SaveMode_Arc_Neg_Sub);
						break;
					case eLineWidthOption::eLineArcPos:
						nSaveMode = static_cast<int>(SaveMode::SaveMode_LineArc_Pos_Sub);
						break;
					case eLineWidthOption::eLineArcNeg:
						nSaveMode = static_cast<int>(SaveMode::SaveMode_LineArc_Neg_Sub);
						break;
					default:
						continue;
					}

					int nWidthCount = static_cast<int>(vecvecvecvecFilteredFeature[nSwath][nLineOption].size());
					//Line, Arc SubMode
					for (int nWidth = 0; nWidth < nWidthCount; nWidth++)
					{
						CSaveLayerJob* pstJob = new CSaveLayerJob;

						if (RESULT_GOOD != pstJob->SetData_FilteredFeature(vecvecvecvecFilteredFeature[nSwath][nLineOption][nWidth], iThreadIdx, pLayerSet, m_pUserSetInfo, strSavePath, vcSwath[nSwath].strFileName,
							static_cast<SaveMode>(nSaveMode), m_pSystemSpec->sysBasic.strImageType, nWidth + 1,
							vcSwath[nSwath].frtRect, dResolution,
							m_pSystemSpec->sysBasic.McType, m_pFactory, m_pD2DContext, m_pImageFactory))
						{
							continue;
						}
						m_pSaveLayerOp->AddJob(pstJob);
						iThreadIdx++;
					}
				}
			}
			else
			{
				// 				CSaveLayerJob* pstJob = new CSaveLayerJob;
				// 				if (RESULT_GOOD != pstJob->SetData_SaveLayer(vcSwath[nSwath].strFileName, iLayerIdx, iThreadIdx, pStep, pLayerSet, vcSwath[nSwath].frtRect, m_pUserSetInfo, &m_pJobData->m_arrFont,
				// 					strSavePath, m_pSystemSpec->sysBasic.strImageType, dResolution, m_pSystemSpec->sysBasic.McType, m_pFactory, m_pD2DContext, m_pImageFactory, eSaveMode, _T(""), eDrawLevel::ePanelLv, bExceptDummy))
				// 				{
				// 					continue;
				// 				}
				// 
				// 				m_pSaveLayerOp->AddJob(pstJob);
				// 				iThreadIdx++;
			}

			iLayerIdx++;
		}
	}

	if (m_pSaveLayerOp->RunAllJobs())
	{
		m_pSaveLayerOp->WaitForJobFinish();
		m_pSaveLayerOp->ResetAllJobEvents();
	}

	if (!m_pSaveLayerOp->StopAllThreads())
		return RESULT_BAD;

	for (int nLineOption = 0; nLineOption < nLineOptionCount; nLineOption++)
	{
		_DeleteFilterFeatureBuffer(vecvecvecvecFilteredFeature[nLineOption]);
	}

	//Layer Color 원복
	for (int i = 0, j = 0; i < iTotLayerCount; i++)
	{
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
		if (pLayerSet == nullptr || !pLayerSet->bCheck) continue;

		pLayerSet->color = vcColor[j++];
	}


	if (strPatternLayerName != _T(""))
	{
		//Cal thread Count
		int nThreadCount = 0;
		for (UINT32 nSwath = 0; nSwath < nSwathCount; nSwath++)
		{
			int nLineOptionCount = eLineWidthOption::LineWidthOptionNone;//
			for (int nLineOption = 0; nLineOption < nLineOptionCount; nLineOption++)
			{
				nThreadCount++;
				
			}
		}

		m_pSaveLayerOp->SetJobFile(m_pJobData);
		m_pSaveLayerOp->StartThreads(nThreadCount, iCoreNum);

		iThreadIdx = 0;

		//Concat Region
		CString strFullPathName;
		CString strSavePath = strRootPath + _T("\\") + strPatternLayerName;
		for (UINT32 nSwath = 0; nSwath < nSwathCount; nSwath++)
		{
			vector<CString> vecSrcFullName;
			int nLineOptionCount = eLineWidthOption::LineWidthOptionNone;//
			for (int nLineOption = 0; nLineOption < nLineOptionCount; nLineOption++)
			{
				int nSaveMode;
				switch (nLineOption)
				{
				case eLineWidthOption::eLinePos:
					nSaveMode = static_cast<int>(SaveMode::SaveMode_Line_Pos_Sub);
					break;
				case eLineWidthOption::eLineNeg:
					nSaveMode = static_cast<int>(SaveMode::SaveMode_Line_Neg_Sub);
					break;
				case eLineWidthOption::eArcPos:
					nSaveMode = static_cast<int>(SaveMode::SaveMode_Arc_Pos_Sub);
					break;
				case eLineWidthOption::eArcNeg:
					nSaveMode = static_cast<int>(SaveMode::SaveMode_Arc_Neg_Sub);
					break;
				case eLineWidthOption::eLineArcPos:
					nSaveMode = static_cast<int>(SaveMode::SaveMode_LineArc_Pos_Sub);
					break;
				case eLineWidthOption::eLineArcNeg:
					nSaveMode = static_cast<int>(SaveMode::SaveMode_LineArc_Neg_Sub);
					break;
				default:
					continue;
				}

				vecSrcFullName.clear();
				int nWidthCount = static_cast<int>(vecvecLineWidth[nLineOption].size());
				for (int nWidth = 0; nWidth < nWidthCount; nWidth++)
				{
					strFullPathName.Format(_T("%s\\%s_%s%s_%d.%s"), strSavePath, strPatternLayerName,
						vcSwath[nSwath].strFileName, SaveMode_Name::strName[nSaveMode], nWidth + 1, _T("hobj"));
					vecSrcFullName.push_back(strFullPathName);
				}

				CString strDstPathName;
				strDstPathName.Format(_T("%s\\%s_%s%s.%s"), strSavePath, strPatternLayerName,
					vcSwath[nSwath].strFileName, SaveMode_Name::strName[nSaveMode], _T("hobj"));

				CSaveLayerJob* pstJob = new CSaveLayerJob;
				if (RESULT_GOOD != pstJob->SetData_ConcatRegion(vecSrcFullName, strDstPathName, iThreadIdx, eMachineType::eAOI, m_pFactory, m_pD2DContext, m_pImageFactory))
				{
					continue;
				}
				m_pSaveLayerOp->AddJob(pstJob);
				iThreadIdx++;
			}

			
		}

		if (m_pSaveLayerOp->RunAllJobs())
		{
			m_pSaveLayerOp->WaitForJobFinish();
			m_pSaveLayerOp->ResetAllJobEvents();
		}

		if (!m_pSaveLayerOp->StopAllThreads())
			return RESULT_BAD;

	}

	//
	vcSwath.clear();
	vcColor.clear();

	_ClearGeo();
	SetRedrawGeo();

	return RESULT_GOOD;
}


UINT32 CDirectView::_SaveSplitImage_LineArcSub_V2(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle,
	IN const double &dResolution, IN const CString &strPathName, int eLineOption, vector <LINEWIDTH> &vecLineWidth)
{
	if (m_pUserSetInfo == nullptr) return RESULT_BAD;
	if (m_pSystemSpec == nullptr) return RESULT_BAD;
	if (m_pFovInfo == nullptr) return RESULT_BAD;
	if (m_pCellInfo == nullptr) return RESULT_BAD;

	int iSwathNumInBlock = static_cast<int>(m_pFovInfo->stBasic.swathNumInBlock);
	int	iCellRowInBlock = static_cast<int>(m_pCellInfo->stBasic.cell_In_Block.cy);
	if (iSwathNumInBlock <= 0 || iCellRowInBlock <= 0) return RESULT_BAD;

	bool bExceptDummy = m_pSystemSpec->sysBasic.bExceptDummy;

	bool bIsBlockMode = false;
	if (bExceptDummy && m_pSystemSpec->sysBasic.MasterLevel == eMasterLevel::eBlockLv)
		bIsBlockMode = true;

	bool bIsAVI = false;
	if (m_pUserSetInfo->mcType == eMachineType::eAVI)
		bIsAVI = true;

	CStep *pStep = m_pJobData->m_arrStep.GetAt(iStepIdx);
	if (pStep == nullptr) return RESULT_BAD;

	if (m_pUserLayerSetInfo->vcUnitStepIdx.size() <= 0) return RESULT_BAD;

	UINT32 nSwathCount;
	std::vector<SaveRect> vcSwath;
	SaveRect stTempSave;
	if (m_pUserSetInfo->mcType == eMachineType::eAOI)
	{//Cell 
		nSwathCount = static_cast<UINT32>(m_pCellInfo->vcCellData.size());
		if (nSwathCount <= 0) return RESULT_BAD;

		for (auto it : m_pCellInfo->vcCellData)
		{
			if (bIsBlockMode)
			{
				if ((int)it->swathIdx >= iSwathNumInBlock)
					continue;

				if (it->fovIdx.y / iCellRowInBlock)
					continue;
			}

			D2D1_RECT_F frtSwath = D2D1::RectF(DoubleToFloat(it->rcRectMM_In_Panel.left),
				DoubleToFloat(it->rcRectMM_In_Panel.top),
				DoubleToFloat(it->rcRectMM_In_Panel.right),
				DoubleToFloat(it->rcRectMM_In_Panel.bottom));

			if (fabs(frtSwath.right - frtSwath.left) < EPSILON_DELTA2 ||
				fabs(frtSwath.bottom - frtSwath.top) < EPSILON_DELTA2)
			{
#ifdef _DEBUG
				AfxMessageBox(_T("셀 정보를 확인하여 주세요."));
#endif
				return RESULT_BAD;
			}

			frtSwath = CDrawFunction::NormalizeRect(frtSwath);

			stTempSave.frtRect = frtSwath;
			stTempSave.strFileName.Format(_T("S%d_R%d_C%d"), it->swathIdx + 1, it->fovIdx.y + 1, it->fovIdx.x + 1);

			vcSwath.emplace_back(stTempSave);
		}

		nSwathCount = static_cast<UINT32>(vcSwath.size());
		if (nSwathCount <= 0) return RESULT_BAD;
	}
	else
	{//Swath
		nSwathCount = static_cast<UINT32>(m_pFovInfo->vcFovData.size());
		if (nSwathCount <= 0) return RESULT_BAD;

		for (auto it : m_pFovInfo->vcFovData)
		{
			D2D1_RECT_F frtSwath = D2D1::RectF(DoubleToFloat(it->rcRectMM_In_Panel.left),
				DoubleToFloat(it->rcRectMM_In_Panel.top),
				DoubleToFloat(it->rcRectMM_In_Panel.right),
				DoubleToFloat(it->rcRectMM_In_Panel.bottom));

			if (fabs(frtSwath.right - frtSwath.left) < EPSILON_DELTA2 ||
				fabs(frtSwath.bottom - frtSwath.top) < EPSILON_DELTA2)
			{
#ifdef _DEBUG
				AfxMessageBox(_T("셀 정보를 확인하여 주세요."));
#endif
				return RESULT_BAD;
			}

			frtSwath = CDrawFunction::NormalizeRect(frtSwath);

			stTempSave.frtRect = frtSwath;
			stTempSave.strFileName.Format(_T("S%d"), it->swathIdx + 1);

			vcSwath.emplace_back(stTempSave);
		}
	}
	

	CString strRootPath = strPathName + _T("\\Layers");
	CreateDirectory(strRootPath, NULL);
	strRootPath = strPathName + _T("\\Layers\\Cell");
	CreateDirectory(strRootPath, NULL);

	CUtils Utils;
	UINT32 iCoreNum = max((UINT32)1, Utils.GetCoreCount() - 1);

	int iTotLayerCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());

	UINT32 iThreadIdx = 0;
	UINT32 iAddLayerNum = (UINT32)(static_cast<int>(SaveMode::SaveMode_Mask) - 1);

	if (bIsAVI) iAddLayerNum = 0;

	int nWidthCount = static_cast<int>(vecLineWidth.size());

	int nSaveMode;
	switch (eLineOption)
	{
	case eLineWidthOption::eLinePos:
		nSaveMode = static_cast<int>(SaveMode::SaveMode_Line_Pos_Sub);
		break;
	case eLineWidthOption::eLineNeg:
		nSaveMode = static_cast<int>(SaveMode::SaveMode_Line_Neg_Sub);
		break;
	case eLineWidthOption::eArcPos:
		nSaveMode = static_cast<int>(SaveMode::SaveMode_Arc_Pos_Sub);
		break;
	case eLineWidthOption::eArcNeg:
		nSaveMode = static_cast<int>(SaveMode::SaveMode_Arc_Neg_Sub);
		break;
	case eLineWidthOption::eLineArcPos:
		nSaveMode = static_cast<int>(SaveMode::SaveMode_LineArc_Pos_Sub);
		break;
	case eLineWidthOption::eLineArcNeg:
		nSaveMode = static_cast<int>(SaveMode::SaveMode_LineArc_Neg_Sub);
		break;
	default:
		return RESULT_BAD;
		break;
	}

	//Get Pattern Layer Prt
	//////////////////////////////////////////////////////////////////////////
	LayerSet_t* pLayerSet_Pattern = nullptr;
	for (int i = 0; i < iTotLayerCount; i++)
	{
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
		if (pLayerSet == nullptr || !pLayerSet->bCheck) continue;

		SaveMode eSaveMode = SaveMode::SaveMode_All;

		if (pLayerSet->strDefine.CompareNoCase(PATTERN_LAYER_TXT) == 0)
		{
			pLayerSet_Pattern = pLayerSet;
			break;
		}
	}
	if (pLayerSet_Pattern == nullptr) return RESULT_BAD;
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	Orient		eOrient = Orient::NoMir0Deg;
	CDrawFunction::GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, eOrient);

// 	CRotMirrArr *pRotMirrArr = new CRotMirrArr;
// 	pRotMirrArr->InputData(0.0, 0.0, eOrient, m_pUserSetInfo->mirrorDir);

	m_pSaveLayerOp->SetJobFile(m_pJobData);
	m_pSaveLayerOp->StartThreads(nSwathCount, iCoreNum);

	vector<vector<vector<CFeature *>>> vecvecvecFilteredFeature;//[swath][lineWidth][Feature]
	_MakeFilterFeatureBuffer(vecvecvecFilteredFeature, nSwathCount, static_cast<int>(vecLineWidth.size()));

	iThreadIdx = 0;
	for (UINT32 nSwath = 0; nSwath < nSwathCount; nSwath++)
	{
		RECTD drtView_mm = RECTD(vcSwath[nSwath].frtRect.left, vcSwath[nSwath].frtRect.top, vcSwath[nSwath].frtRect.right, vcSwath[nSwath].frtRect.bottom);
		drtView_mm.NormalizeRectD();

		//_GetFilteredFeature(vecvecvecFilteredFeature[nSwath], pStep, pLayerSet_Pattern, pRotMirrArr, eLineOption, vecLineWidth, drtView_mm);
		CSaveLayerJob* pstJob = new CSaveLayerJob;

		if (RESULT_GOOD != pstJob->SetData_GetFilteredFeature(this, &vecvecvecFilteredFeature[nSwath], pStep, pLayerSet_Pattern, m_pUserSetInfo,
			eLineOption, &vecLineWidth, drtView_mm, iThreadIdx))
		{
			continue;
		}
		m_pSaveLayerOp->AddJob(pstJob);
		iThreadIdx++;
	}

// 	delete pRotMirrArr;
// 	pRotMirrArr = nullptr;
	//////////////////////////////////////////////////////////////////////////

	if (m_pSaveLayerOp->RunAllJobs())
	{
		m_pSaveLayerOp->WaitForJobFinish();
		m_pSaveLayerOp->ResetAllJobEvents();
	}

	if (!m_pSaveLayerOp->StopAllThreads())
		return RESULT_BAD;


	//1. Make Layer Images
	m_pSaveLayerOp->SetJobFile(m_pJobData);
	m_pSaveLayerOp->StartThreads(nSwathCount * nWidthCount, iCoreNum);

	iThreadIdx = 0;
	UINT32 iLayerIdx = 0;
	std::vector<COLORREF> vcColor;
	CString strPatternLayerName = _T("");
	for (UINT32 nSwath = 0; nSwath < nSwathCount; nSwath++)
	{
		iLayerIdx = 0;
		for (int i = 0; i < iTotLayerCount; i++)
		{
			LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
			if (pLayerSet == nullptr || !pLayerSet->bCheck) continue;

			SaveMode eSaveMode = SaveMode::SaveMode_All;

			CString strSavePath = strRootPath + _T("\\") + pLayerSet->strLayer;
			if (nSwath == 0)
			{
				vcColor.emplace_back(pLayerSet->color);
				pLayerSet->color = RGB(255, 255, 255);
			}

			if (pLayerSet->strDefine.CompareNoCase(PATTERN_LAYER_TXT) == 0 && !bIsAVI)
			{
				strPatternLayerName = pLayerSet->strLayer;



				//Line, Arc SubMode
				for (int nWidth = 0; nWidth < nWidthCount; nWidth++)
				{
					CSaveLayerJob* pstJob = new CSaveLayerJob;

					if (RESULT_GOOD != pstJob->SetData_FilteredFeature(vecvecvecFilteredFeature[nSwath][nWidth], iThreadIdx, pLayerSet, m_pUserSetInfo, strSavePath, vcSwath[nSwath].strFileName,
						static_cast<SaveMode>(nSaveMode), m_pSystemSpec->sysBasic.strImageType, nWidth + 1, 
						vcSwath[nSwath].frtRect, dResolution,
						m_pSystemSpec->sysBasic.McType, m_pFactory, m_pD2DContext, m_pImageFactory) )
					{
						continue;
					}
					m_pSaveLayerOp->AddJob(pstJob);
					iThreadIdx++;
				}
			}
			else
			{
				// 				CSaveLayerJob* pstJob = new CSaveLayerJob;
				// 				if (RESULT_GOOD != pstJob->SetData_SaveLayer(vcSwath[nSwath].strFileName, iLayerIdx, iThreadIdx, pStep, pLayerSet, vcSwath[nSwath].frtRect, m_pUserSetInfo, &m_pJobData->m_arrFont,
				// 					strSavePath, m_pSystemSpec->sysBasic.strImageType, dResolution, m_pSystemSpec->sysBasic.McType, m_pFactory, m_pD2DContext, m_pImageFactory, eSaveMode, _T(""), eDrawLevel::ePanelLv, bExceptDummy))
				// 				{
				// 					continue;
				// 				}
				// 
				// 				m_pSaveLayerOp->AddJob(pstJob);
				// 				iThreadIdx++;
			}

			iLayerIdx++;
		}
	}

	if (m_pSaveLayerOp->RunAllJobs())
	{
		m_pSaveLayerOp->WaitForJobFinish();
		m_pSaveLayerOp->ResetAllJobEvents();
	}

	if (!m_pSaveLayerOp->StopAllThreads())
		return RESULT_BAD;

	_DeleteFilterFeatureBuffer(vecvecvecFilteredFeature);

	for (int i = 0, j = 0; i < iTotLayerCount; i++)
	{
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
		if (pLayerSet == nullptr || !pLayerSet->bCheck) continue;

		pLayerSet->color = vcColor[j++];
	}


	if (strPatternLayerName != _T(""))
	{
		m_pSaveLayerOp->SetJobFile(m_pJobData);
		m_pSaveLayerOp->StartThreads(nSwathCount, iCoreNum);

		iThreadIdx = 0;

		//Concat Region
		CString strFullPathName;
		CString strSavePath = strRootPath + _T("\\") + strPatternLayerName;
		for (UINT32 nSwath = 0; nSwath < nSwathCount; nSwath++)
		{
			vector<CString> vecSrcFullName;
			for (int nWidth = 0; nWidth < nWidthCount; nWidth++)
			{
				strFullPathName.Format(_T("%s\\%s_%s%s_%d.%s"), strSavePath, strPatternLayerName,
					vcSwath[nSwath].strFileName, SaveMode_Name::strName[nSaveMode], nWidth + 1, _T("hobj"));
				vecSrcFullName.push_back(strFullPathName);
			}

			CString strDstPathName;
			strDstPathName.Format(_T("%s\\%s_%s%s.%s"), strSavePath, strPatternLayerName,
				vcSwath[nSwath].strFileName, SaveMode_Name::strName[nSaveMode], _T("hobj"));

			CSaveLayerJob* pstJob = new CSaveLayerJob;
			if (RESULT_GOOD != pstJob->SetData_ConcatRegion(vecSrcFullName, strDstPathName, iThreadIdx, eMachineType::eAOI, m_pFactory, m_pD2DContext, m_pImageFactory))
			{
				continue;
			}
			m_pSaveLayerOp->AddJob(pstJob);
			iThreadIdx++;
		}

		if (m_pSaveLayerOp->RunAllJobs())
		{
			m_pSaveLayerOp->WaitForJobFinish();
			m_pSaveLayerOp->ResetAllJobEvents();
		}

		if (!m_pSaveLayerOp->StopAllThreads())
			return RESULT_BAD;

	}

	//

	vcSwath.clear();
	vcColor.clear();

	_ClearGeo();
	SetRedrawGeo();

	return RESULT_GOOD;
}

UINT32 CDirectView::_MergeSplitImage(IN const INT32 iStepIdx, IN const double &dResolution, IN const CString &strPathName)
{
	if (m_pUserSetInfo == nullptr) return RESULT_BAD;

	bool bIsAVI = false;
	if (m_pUserSetInfo->mcType == eMachineType::eAVI)
		bIsAVI = true;

	if (!bIsAVI) return RESULT_GOOD;
	
	BOOL bSaveRaw = TRUE;
	

	// for Merge Image per Swath 
	// 1) 32 bit case : Tif(12G), PNG(8.5G) Rough하게...
	// 2) 8 bit case : PNG(2.6G)
	double dNeededMemory = 12.0;

	MEMORYSTATUS memoryStatus;
	GlobalMemoryStatus(&memoryStatus);

	double dUsageMemory = (memoryStatus.dwAvailPhys / (pow(1024, 3.0)));
	if (dUsageMemory < dNeededMemory) return RESULT_BAD;

	UINT32 iMaxThreadNum = static_cast<UINT32>(max(1, int(dUsageMemory / dNeededMemory)));

	CStep *pStep = m_pJobData->m_arrStep.GetAt(iStepIdx);
	if (pStep == nullptr) return RESULT_BAD;

	if (m_pUserLayerSetInfo->vcUnitStepIdx.size() <= 0) return RESULT_BAD;

	UINT32 nSwathCount = static_cast<UINT32>(m_pFovInfo->vcFovData.size());
	if (nSwathCount <= 0) return RESULT_BAD;

	std::vector<SaveRect> vcSwath;
	SaveRect stTempSave;
	for (auto it : m_pFovInfo->vcFovData)
	{
		D2D1_RECT_F frtSwath = D2D1::RectF(DoubleToFloat(it->rcRectMM_In_Panel.left),
			DoubleToFloat(it->rcRectMM_In_Panel.top),
			DoubleToFloat(it->rcRectMM_In_Panel.right),
			DoubleToFloat(it->rcRectMM_In_Panel.bottom));

		frtSwath = CDrawFunction::NormalizeRect(frtSwath);
		
		stTempSave.frtRect = frtSwath;
		stTempSave.strFileName.Format(_T("S%d"), it->swathIdx + 1);

		vcSwath.emplace_back(stTempSave);
	}

	CString strRootPath = strPathName + _T("\\Layers");
	CreateDirectory(strRootPath, NULL);
	strRootPath = strPathName + _T("\\Layers\\Swath");
	CreateDirectory(strRootPath, NULL);

	strRootPath = strPathName + _T("\\Layers\\Cell");

	CUtils Utils;
	UINT32 iCoreNum = max((UINT32)1, Utils.GetCoreCount() - 1);

	int iTotLayerCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());

	UINT32 iThreadIdx = 0;
	UINT32 iAddLayerNum = (UINT32)(static_cast<int>(SaveMode::SaveMode_Mask) - 1);
	
	// 2. Merge Image - Memory 제약 : 32G에서 Thread 2개 한계
	m_pSaveLayerOp->SetJobFile(m_pJobData);
	m_pSaveLayerOp->StartThreads(nSwathCount * (m_pUserLayerSetInfo->iSelectedNum + iAddLayerNum), iMaxThreadNum);
	//m_pSaveLayerOp->StartThreads(nSwathCount * (m_pUserLayerSetInfo->iSelectedNum + iAddLayerNum), iCoreNum);

	iThreadIdx = 0;
	for (UINT32 nSwath = 0; nSwath < nSwathCount; nSwath++)
	{
		UINT32 iLayerIdx = 0;
		for (int i = 0; i < iTotLayerCount; i++)
		{
			LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
			if (pLayerSet == nullptr || !pLayerSet->bCheck) continue;

			SaveMode eSaveMode = SaveMode::SaveMode_All;
			CString strSavePath = strRootPath + _T("\\") + pLayerSet->strLayer;

			if (!bIsAVI && pLayerSet->strDefine.CompareNoCase(PATTERN_LAYER_TXT) == 0)
			{
				for (int nSaveMode = static_cast<int>(SaveMode::SaveMode_Mask) - 1; nSaveMode >= static_cast<int>(SaveMode::SaveMode_All); nSaveMode--)
				{
					CSaveLayerJob* pstJob = new CSaveLayerJob;
					if (RESULT_GOOD != pstJob->SetData_SaveLayer(vcSwath[nSwath].strFileName, iLayerIdx, iThreadIdx, pStep, pLayerSet, vcSwath[nSwath].frtRect, m_pUserSetInfo, &m_pJobData->m_arrFont,
						strSavePath, m_pSystemSpec->sysBasic.strImageType, dResolution, m_pSystemSpec->sysBasic.McType, m_pFactory, m_pD2DContext, m_pImageFactory, static_cast<SaveMode>(nSaveMode),
						eDrawLevel::ePanelLv, false, OpMode::MergeSwath, bSaveRaw))
					{
						continue;
					}
					m_pSaveLayerOp->AddJob(pstJob);
					iThreadIdx++;
				}
			}
			else
			{
				CSaveLayerJob* pstJob = new CSaveLayerJob;
				if (RESULT_GOOD != pstJob->SetData_SaveLayer(vcSwath[nSwath].strFileName, iLayerIdx, iThreadIdx, pStep, pLayerSet, vcSwath[nSwath].frtRect, m_pUserSetInfo, &m_pJobData->m_arrFont,
					strSavePath, m_pSystemSpec->sysBasic.strImageType, dResolution, m_pSystemSpec->sysBasic.McType, m_pFactory, m_pD2DContext, m_pImageFactory, eSaveMode, 
					eDrawLevel::ePanelLv, false, OpMode::MergeSwath, bSaveRaw))
				{
					continue;
				}
				m_pSaveLayerOp->AddJob(pstJob);
				iThreadIdx++;
			}
			iLayerIdx++;
		}
	}

	if (m_pSaveLayerOp->RunAllJobs())
	{
		m_pSaveLayerOp->WaitForJobFinish();
		m_pSaveLayerOp->ResetAllJobEvents();
	}

	if (!m_pSaveLayerOp->StopAllThreads())
		return RESULT_BAD;
	
	vcSwath.clear();

	return RESULT_GOOD;
}

UINT32 CDirectView::_ConvRegion(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle,
	IN const double &dResolution, IN const CString &strPathName)
{
	if (m_pUserSetInfo == nullptr) return RESULT_BAD;

	bool bIsAVI = false;
	if (m_pUserSetInfo->mcType == eMachineType::eAVI)
		bIsAVI = true;

	CStep *pStep = m_pJobData->m_arrStep.GetAt(iStepIdx);
	if (pStep == nullptr) return RESULT_BAD;

	if (m_pUserLayerSetInfo->vcUnitStepIdx.size() <= 0) return RESULT_BAD;

	UINT32 nSwathCount;
	std::vector<SaveRect> vcSwath;
	SaveRect stTempSave;
	if (m_pUserSetInfo->mcType == eMachineType::eAOI)
	{//Cell 
		nSwathCount = static_cast<UINT32>(m_pCellInfo->vcCellData.size());
		if (nSwathCount <= 0) return RESULT_BAD;

		for (auto it : m_pCellInfo->vcCellData)
		{
			D2D1_RECT_F frtSwath = D2D1::RectF(DoubleToFloat(it->rcRectMM_In_Panel.left),
				DoubleToFloat(it->rcRectMM_In_Panel.top),
				DoubleToFloat(it->rcRectMM_In_Panel.right),
				DoubleToFloat(it->rcRectMM_In_Panel.bottom));

			frtSwath = CDrawFunction::NormalizeRect(frtSwath);

			stTempSave.frtRect = frtSwath;
			stTempSave.strFileName.Format(_T("S%d_R%d_C%d"), it->swathIdx + 1, it->fovIdx.y + 1, it->fovIdx.x + 1);

			vcSwath.emplace_back(stTempSave);
		}
	}
	else
	{//Swath
		nSwathCount = static_cast<UINT32>(m_pFovInfo->vcFovData.size());
		if (nSwathCount <= 0) return RESULT_BAD;

		for (auto it : m_pFovInfo->vcFovData)
		{
			D2D1_RECT_F frtSwath = D2D1::RectF(DoubleToFloat(it->rcRectMM_In_Panel.left),
				DoubleToFloat(it->rcRectMM_In_Panel.top),
				DoubleToFloat(it->rcRectMM_In_Panel.right),
				DoubleToFloat(it->rcRectMM_In_Panel.bottom));

			frtSwath = CDrawFunction::NormalizeRect(frtSwath);

			stTempSave.frtRect = frtSwath;
			stTempSave.strFileName.Format(_T("S%d"), it->swathIdx + 1);

			vcSwath.emplace_back(stTempSave);
		}
	}

	CString strRootPath = strPathName + _T("\\Layers");

	CString strRootSrcPath = strPathName + _T("\\Layers\\Cell");
	CString strRootDstPath = strPathName + _T("\\Layers\\Cell_Region");
	::SHCreateDirectoryEx(NULL, strRootDstPath, NULL);
	
	int iTotLayerCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());

	//Thread Setting
	CUtils Utils;
	UINT32 iCoreNum = max((UINT32)1, Utils.GetCoreCount() - 1);

	UINT32 iThreadIdx = 0;
	UINT32 iAddLayerNum = (UINT32)(static_cast<int>(SaveMode::SaveMode_Mask) - 1);

	m_pSaveLayerOp->SetJobFile(m_pJobData);
	m_pSaveLayerOp->StartThreads(nSwathCount * (m_pUserLayerSetInfo->iSelectedNum + iAddLayerNum), iCoreNum);
	//

	UINT32 iLayerIdx = 0;
	LayerSet_t* pPatternLayerSet = nullptr;
	std::vector<COLORREF> vcColor;

	m_pSaveLayer->SetDrawMode(eDrawLevel::ePanelLv);

	iLayerIdx = 0;
	CString strFileName;
	CString strSrcPath;
	CString strDstPath;
	for (int i = 0; i < iTotLayerCount; i++)
	{
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
		if (pLayerSet == nullptr || !pLayerSet->bCheck) continue;
		if (pLayerSet->strDefine.CompareNoCase(PATTERN_LAYER_TXT) != 0) continue;

		strSrcPath = strRootSrcPath + _T("\\") + pLayerSet->strLayer + _T("\\");
		strDstPath = strRootDstPath + _T("\\") + pLayerSet->strLayer + _T("\\");
		::SHCreateDirectoryEx(NULL, strDstPath, NULL);

		for (UINT32 nSwath = 0; nSwath < nSwathCount; nSwath++)
		{

			for (int nSaveMode = static_cast<int>(SaveMode::SaveMode_Line); nSaveMode < static_cast<int>(SaveMode::SaveMode_Max); nSaveMode++)
			{
				CSaveLayerJob* pstJob = new CSaveLayerJob;
				if (RESULT_GOOD != pstJob->SetData_SaveLayer(vcSwath[nSwath].strFileName, iLayerIdx, iThreadIdx, pStep, pLayerSet, vcSwath[nSwath].frtRect, m_pUserSetInfo, &m_pJobData->m_arrFont,
					strSrcPath, m_pSystemSpec->sysBasic.strImageType, dResolution, m_pSystemSpec->sysBasic.McType, m_pFactory, m_pD2DContext, m_pImageFactory, static_cast<SaveMode>(nSaveMode), 
					eDrawLevel::ePanelLv, false, OpMode::ConvRegion, FALSE, 0, false, strDstPath))
				{
					continue;
				}
				m_pSaveLayerOp->AddJob(pstJob);
				iThreadIdx++;
			}
		}
	}

	if (m_pSaveLayerOp->RunAllJobs())
	{
		m_pSaveLayerOp->WaitForJobFinish();
		m_pSaveLayerOp->ResetAllJobEvents();
	}

	if (!m_pSaveLayerOp->StopAllThreads())
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CDirectView::_SaveRegion(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle,
	IN const double &dResolution, IN const CString &strPathName)
{
	if (m_pUserSetInfo == nullptr) return RESULT_BAD;

	bool bIsAVI = false;
	if (m_pUserSetInfo->mcType == eMachineType::eAVI)
		bIsAVI = true;

	CStep *pStep = m_pJobData->m_arrStep.GetAt(iStepIdx);
	if (pStep == nullptr) return RESULT_BAD;

	if (m_pUserLayerSetInfo->vcUnitStepIdx.size() <= 0) return RESULT_BAD;

	UINT32 nSwathCount;
	std::vector<SaveRect> vcSwath;
	SaveRect stTempSave;
	if (m_pUserSetInfo->mcType == eMachineType::eAOI)
	{//Cell 
		nSwathCount = static_cast<UINT32>(m_pCellInfo->vcCellData.size());
		if (nSwathCount <= 0) return RESULT_BAD;

		for (auto it : m_pCellInfo->vcCellData)
		{
			D2D1_RECT_F frtSwath = D2D1::RectF(DoubleToFloat(it->rcRectMM_In_Panel.left),
				DoubleToFloat(it->rcRectMM_In_Panel.top),
				DoubleToFloat(it->rcRectMM_In_Panel.right),
				DoubleToFloat(it->rcRectMM_In_Panel.bottom));

			frtSwath = CDrawFunction::NormalizeRect(frtSwath);

			stTempSave.frtRect = frtSwath;
			stTempSave.strFileName.Format(_T("S%d_R%d_C%d"), it->swathIdx + 1, it->fovIdx.y + 1, it->fovIdx.x + 1);

			vcSwath.emplace_back(stTempSave);
		}
	}
	else
	{//Swath
		nSwathCount = static_cast<UINT32>(m_pFovInfo->vcFovData.size());
		if (nSwathCount <= 0) return RESULT_BAD;

		for (auto it : m_pFovInfo->vcFovData)
		{
			D2D1_RECT_F frtSwath = D2D1::RectF(DoubleToFloat(it->rcRectMM_In_Panel.left),
				DoubleToFloat(it->rcRectMM_In_Panel.top),
				DoubleToFloat(it->rcRectMM_In_Panel.right),
				DoubleToFloat(it->rcRectMM_In_Panel.bottom));

			frtSwath = CDrawFunction::NormalizeRect(frtSwath);

			stTempSave.frtRect = frtSwath;
			stTempSave.strFileName.Format(_T("S%d"), it->swathIdx + 1);

			vcSwath.emplace_back(stTempSave);
		}
	}

	CString strRootPath = strPathName + _T("\\Layers");
	CreateDirectory(strRootPath, NULL);
	strRootPath = strPathName + _T("\\Layers\\Cell_Region\\");
	CreateDirectory(strRootPath, NULL);

	Orient		eOrient;
	CDrawFunction::GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, eOrient);
	CRotMirrArr *pRotMirrArr = new CRotMirrArr;
	pRotMirrArr->InputData(0.0, 0.0, eOrient, m_pUserSetInfo->mirrorDir);
		
	CUtils Utils;
	UINT32 iCoreNum = max((UINT32)1, Utils.GetCoreCount() - 1);

	int iTotLayerCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());

	UINT32 iThreadIdx = 0;
	UINT32 iAddLayerNum = (UINT32)(static_cast<int>(SaveMode::SaveMode_Mask) - 1);

	m_pSaveLayerOp->SetJobFile(m_pJobData);
	m_pSaveLayerOp->StartThreads(nSwathCount * (m_pUserLayerSetInfo->iSelectedNum + iAddLayerNum), iCoreNum);

	iThreadIdx = 0;
	for (UINT32 nSwath = 0; nSwath < nSwathCount; nSwath++)
	{
		UINT32 iLayerIdx = 0;
		for (int i = 0; i < iTotLayerCount; i++)
		{
			LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
			if (pLayerSet == nullptr || !pLayerSet->bCheck) continue;

			CString strSavePath = strRootPath + _T("\\") + pLayerSet->strLayer;

			if (pLayerSet->strDefine.CompareNoCase(PATTERN_LAYER_TXT) == 0)
			{
				for (int nSaveMode = static_cast<int>(SaveMode::SaveMode_Line); nSaveMode < static_cast<int>(SaveMode::SaveMode_Max); nSaveMode++)
				{
					CSaveLayerJob* pstJob = new CSaveLayerJob;
					if (RESULT_GOOD != pstJob->SetData_SaveLayer(vcSwath[nSwath].strFileName, iLayerIdx, iThreadIdx, pStep, pLayerSet, vcSwath[nSwath].frtRect, m_pUserSetInfo, &m_pJobData->m_arrFont,
						strSavePath, m_pSystemSpec->sysBasic.strImageType, dResolution, m_pSystemSpec->sysBasic.McType, m_pFactory, m_pD2DContext, m_pImageFactory, static_cast<SaveMode>(nSaveMode),
						eDrawLevel::ePanelLv, false, OpMode::MakeRegion))
					{
						continue;
					}
					m_pSaveLayerOp->AddJob(pstJob);
					iThreadIdx++;
				}
			}
			
			iLayerIdx++;
		}
	}

	if (m_pSaveLayerOp->RunAllJobs())
	{
		m_pSaveLayerOp->WaitForJobFinish();
		m_pSaveLayerOp->ResetAllJobEvents();
	}

	if (!m_pSaveLayerOp->StopAllThreads())
		return RESULT_BAD;

	vcSwath.clear();


	//////////////////////////////////////////////////////////////////////////
	//Save Region Test
	/*
	int iTotLayerCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());
	
	UINT32 iLayerIdx = 0;
	LayerSet_t* pPatternLayerSet = nullptr;
	std::vector<COLORREF> vcColor;
	
	m_pSaveLayer->SetDrawMode(eDrawLevel::ePanelLv);

	iLayerIdx = 0;
	CString strFileName;
	for (int i = 0; i < iTotLayerCount; i++)
	{
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];
		if (pLayerSet == nullptr || !pLayerSet->bCheck) continue;
		if (pLayerSet->strDefine.CompareNoCase(PATTERN_LAYER_TXT) != 0) continue;

		CString strSavePath = strRootPath + _T("\\") + pLayerSet->strLayer;

		SaveMode eSaveMode;
		//for (UINT32 nSwath = 0; nSwath < nSwathCount; nSwath++)
		for (UINT32 nSwath = 0; nSwath < 6; nSwath++)
		{
			m_pSaveLayer->SetPictureBox(vcSwath[nSwath].frtRect, dResolution);

			eSaveMode = SaveMode::SaveMode_Surface_Pos;
			
			strFileName.Format(_T("%s_%s%s"), 
				strSavePath, vcSwath[nSwath].strFileName, SaveMode_Name::strName[static_cast<int>(eSaveMode)]);

			((CSaveLayer_AOI*)m_pSaveLayer)->SaveMasterRegion(strFileName, 0, pStep, pLayerSet, &m_pJobData->m_arrFont, pRotMirrArr, TRUE, m_pUserSetInfo, eSaveMode);
		}
	}

	if (pRotMirrArr != nullptr)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}

	*/

	return RESULT_GOOD;
}

UINT32 CDirectView::_MakeFilterFeatureBuffer(vector<vector < vector<CFeature*>>> &vecvecFeature, int nCellCount, int nLineWidthCount)
{
	_DeleteFilterFeatureBuffer(vecvecFeature);

	vecvecFeature.resize(nCellCount);
	for (int nCell = 0; nCell < nCellCount; nCell++)
	{
		vecvecFeature[nCell].resize(nLineWidthCount);
	}

	return RESULT_GOOD;
}

UINT32 CDirectView::_DeleteFilterFeatureBuffer(vector<vector < vector<CFeature*>>> &vecvecvecFeature)
{
	int nCellCount = static_cast<int>(vecvecvecFeature.size());
	for (int nCell = 0; nCell < nCellCount; nCell++)
	{
		int nLineWidthCount = static_cast<int>(vecvecvecFeature[nCell].size());
		for (int nLineWidth = 0; nLineWidth < nLineWidthCount; nLineWidth++)
		{
			int nFeatureCount = static_cast<int>(vecvecvecFeature[nCell][nLineWidth].size());
			for (int nFeature = 0; nFeature < nFeatureCount; nFeature++)
			{
				if (vecvecvecFeature[nCell][nLineWidth][nFeature] != nullptr)
				{
					delete vecvecvecFeature[nCell][nLineWidth][nFeature];
					vecvecvecFeature[nCell][nLineWidth][nFeature] = nullptr;
				}
			}
			vecvecvecFeature[nCell][nLineWidth].clear();
		}
		vecvecvecFeature[nCell].clear();
	}
	vecvecvecFeature.clear();

	return RESULT_GOOD;
}

UINT32 CDirectView::_MakeFilterFeatureBuffer_V3(vector<vector < vector<CFeature*>>> &vecvecFeature, vector<vector<LINEWIDTH>> &vecvecLineWidth)
{
	_DeleteFilterFeatureBuffer(vecvecFeature);

	int nLineOptionCount = static_cast<int>(vecvecLineWidth.size());
	vecvecFeature.resize(nLineOptionCount);
	for (int nLineOption = 0; nLineOption < nLineOptionCount; nLineOption++)
	{
		int nLineWidthCount = static_cast<int>(vecvecLineWidth[nLineOption].size());

		vecvecFeature[nLineOption].resize(nLineWidthCount);
	}

	return RESULT_GOOD;
}


UINT32 CDirectView::GetFilteredFeature(vector < vector<CFeature*>> &vecvecFeature, CStep *pStep, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, int &eLineOption, vector<LINEWIDTH> &vecLineWidth, RECTD &drtView_mm)
{//Line Width에 맞는 Feature를 복사해온다.SubStep 포함
	

	//Step
	_GetFeature(vecvecFeature, pStep, pLayerSet, pRotMirrArr, eLineOption, vecLineWidth, drtView_mm);

	//SubStep
	for (UINT i = 0; i < pStep->m_nSubStepCnt; i++)
	{
		CSubStep* pSubStep = pStep->m_arrSubStep[i];
		if (pSubStep == nullptr) continue;

		_GetFeature_SubStep(vecvecFeature, pSubStep, pLayerSet, pRotMirrArr, eLineOption, vecLineWidth, drtView_mm);
	}

	return RESULT_GOOD;
}

UINT32 CDirectView::_GetFeature(vector<vector<CFeature*>> &vecvecFeature, CStep *pStep, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, int &eLineOption, vector<LINEWIDTH> &vecLineWidth, RECTD &drtView_mm)
{
	if (pStep == nullptr ) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;

	int nLayerIndex = 0;//사용안함
	CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, pLayerSet->strLayer, nLayerIndex);
	if (pLayer == nullptr)
		return RESULT_BAD;

	if (pRotMirrArr == nullptr) return RESULT_BAD;

	int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());
	for ( int nfeno = 0 ; nfeno < nFeatureCount ; nfeno++)
	{
		CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nfeno);
		if (pFeature == nullptr) continue;

		//교집합 체크 : Draw 시간을 줄이기 위함
		{

			RECTD drtFeature;
			if (pRotMirrArr == nullptr)
			{
				drtFeature = pFeature->m_MinMaxRect;
			}
			else
			{
				pRotMirrArr->FinalPoint(&drtFeature.left, &drtFeature.top, pFeature->m_MinMaxRect.left, pFeature->m_MinMaxRect.top);
				pRotMirrArr->FinalPoint(&drtFeature.right, &drtFeature.bottom, pFeature->m_MinMaxRect.right, pFeature->m_MinMaxRect.bottom);
				drtFeature.NormalizeRectD();
			}

			if (drtFeature.IsIntersection(drtView_mm) == FALSE)
			{
				continue;
			}
		}

		if (RESULT_BAD == _SetFeatureData(vecvecFeature, pFeature, pRotMirrArr, eLineOption, vecLineWidth))
		{
			continue;
		}

	}
	

	return RESULT_GOOD;
}

UINT32 CDirectView::_GetFeature_SubStep(vector<vector<CFeature*>> &vecvecFeature, CSubStep *pSubStep, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, int &eLineOption, vector<LINEWIDTH> &vecLineWidth, RECTD &drtView_mm)
{
	if (pSubStep == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;

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
					rotMirrArr->FinalPoint(&drtFeature.left, &drtFeature.top, pStep->m_FeatureMinMax.left, pStep->m_FeatureMinMax.top);
					rotMirrArr->FinalPoint(&drtFeature.right, &drtFeature.bottom, pStep->m_FeatureMinMax.right, pStep->m_FeatureMinMax.bottom);
					drtFeature.NormalizeRectD();
				}

				if (drtFeature.IsIntersection(drtView_mm) == FALSE)
				{
					rotMirrArr->RemoveTail();
					continue;
				}
			}

			_GetFeature(vecvecFeature, pStep, pLayerSet, pRotMirrArr, eLineOption, vecLineWidth, drtView_mm);

			//SubSub
			for (UINT j = 0; j < pSubStep->m_nNextStepCnt; j++)
			{
				CSubStep* pSubSubStep = pSubStep->m_arrNextStep.GetAt(j);

				_GetFeature_SubStep(vecvecFeature, pSubSubStep, pLayerSet, pRotMirrArr, eLineOption, vecLineWidth, drtView_mm);
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

UINT32 CDirectView::_SetFeatureData(vector<vector<CFeature*>> &vecvecFeature, CFeature *pFeature, CRotMirrArr *pRotMirrArr, int &eLineOption, vector<LINEWIDTH> &vecLineWidth)
{
	if (pFeature == nullptr) return RESULT_BAD;
	if (pRotMirrArr == nullptr) return RESULT_BAD;

	CFeature *pFeature_New = nullptr;
	double dTempX, dTempY;
	int nLineWidthSize = static_cast<int>(vecLineWidth.size());

	//feature type : P
	if (pFeature->m_eType == FeatureType::P) return RESULT_BAD;
	//feature type : T
	if (pFeature->m_eType == FeatureType::T) return RESULT_BAD;
	//feature type : S
	if (pFeature->m_eType == FeatureType::S) return RESULT_BAD;
	//feature type : B
	if (pFeature->m_eType == FeatureType::B) return RESULT_BAD;

	//pos pass
	if (pFeature->m_bPolarity == true && (eLineOption == eLineWidthOption::eLineNeg || eLineOption == eLineWidthOption::eArcNeg || eLineOption == eLineWidthOption::eLineArcNeg)) return RESULT_BAD;
	//Neg pass
	if (pFeature->m_bPolarity == false && (eLineOption == eLineWidthOption::eLinePos || eLineOption == eLineWidthOption::eArcPos || eLineOption == eLineWidthOption::eLineArcPos)) return RESULT_BAD;

	//feature type : L
	if (pFeature->m_eType == FeatureType::L && (eLineOption == eLineWidthOption::eArcPos || eLineOption == eLineWidthOption::eArcNeg)) return RESULT_BAD;
	//feature type : A
	if (pFeature->m_eType == FeatureType::A && (eLineOption == eLineWidthOption::eLinePos || eLineOption == eLineWidthOption::eLineNeg)) return RESULT_BAD;


	switch (pFeature->m_eType)
	{
	case FeatureType::L:
	{
		pFeature_New = new CFeatureL();
		*pFeature_New = *pFeature;
		pFeature_New->m_pSymbol = pFeature->m_pSymbol;

		CFeatureL *pFeatureL_New = (CFeatureL*)pFeature_New;
		*pFeatureL_New = *(CFeatureL*)pFeature;

		pRotMirrArr->FinalPoint(&dTempX, &dTempY, pFeatureL_New->m_dXs, pFeatureL_New->m_dYs);
		pFeatureL_New->m_dXs = dTempX;
		pFeatureL_New->m_dYs = dTempY;

		pRotMirrArr->FinalPoint(&dTempX, &dTempY, pFeatureL_New->m_dXe, pFeatureL_New->m_dYe);
		pFeatureL_New->m_dXe = dTempX;
		pFeatureL_New->m_dYe = dTempY;
	}
	break;
	case FeatureType::A:
	{
		pFeature_New = new CFeatureA();
		*pFeature_New = *pFeature;
		pFeature_New->m_pSymbol = pFeature->m_pSymbol;

		CFeatureA *pFeatureA_New = (CFeatureA*)pFeature_New;
		*pFeatureA_New = *(CFeatureA*)pFeature;

		pRotMirrArr->FinalPoint(&dTempX, &dTempY, pFeatureA_New->m_dXs, pFeatureA_New->m_dYs);
		pFeatureA_New->m_dXs = dTempX;
		pFeatureA_New->m_dYs = dTempY;

		pRotMirrArr->FinalPoint(&dTempX, &dTempY, pFeatureA_New->m_dXe, pFeatureA_New->m_dYe);
		pFeatureA_New->m_dXe = dTempX;
		pFeatureA_New->m_dYe = dTempY;

		pRotMirrArr->FinalPoint(&dTempX, &dTempY, pFeatureA_New->m_dXc, pFeatureA_New->m_dYc);
		pFeatureA_New->m_dXc = dTempX;
		pFeatureA_New->m_dYc = dTempY;
	}
	break;
	default:
		break;
	}

	if (pFeature_New == nullptr) return RESULT_BAD;

	int nSelectIndex = -1;
	for (int i = 0; i < nLineWidthSize; i++)
	{
		if (pFeature_New->m_pSymbol->m_strUserSymbol.Mid(1).CompareNoCase(vecLineWidth[i].strLineWidth) == 0)
		{
			nSelectIndex = i;
			break;
		}
	}

	if (nSelectIndex == -1)
	{
		delete pFeature_New;
		return RESULT_BAD;
	}

	vecvecFeature[nSelectIndex].push_back(pFeature_New);

	return RESULT_GOOD;
}

UINT32 CDirectView::GetFilteredFeature_V3(vector < vector < vector<CFeature*>>> &vecvecvecFeature, CStep *pStep, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, vector < vector<LINEWIDTH>> &vecvecLineWidth, RECTD &drtView_mm)
{//Line Width에 맞는 Feature를 복사해온다.SubStep 포함
	//Step
	_GetFeature_V3(vecvecvecFeature, pStep, pLayerSet, pRotMirrArr, vecvecLineWidth, drtView_mm);

	//SubStep
	for (UINT i = 0; i < pStep->m_nSubStepCnt; i++)
	{
		CSubStep* pSubStep = pStep->m_arrSubStep[i];
		if (pSubStep == nullptr) continue;

		_GetFeature_SubStep_V3(vecvecvecFeature, pSubStep, pLayerSet, pRotMirrArr, vecvecLineWidth, drtView_mm);
	}

	return RESULT_GOOD;
}

UINT32 CDirectView::_GetFeature_V3(vector < vector<vector<CFeature*>>> &vecvecvecFeature, CStep *pStep, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, vector< vector<LINEWIDTH>> &vecvecLineWidth, RECTD &drtView_mm)
{
	if (pStep == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;

	int nLayerIndex = 0;//사용안함
	CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, pLayerSet->strLayer, nLayerIndex);
	if (pLayer == nullptr)
		return RESULT_BAD;

	if (pRotMirrArr == nullptr) return RESULT_BAD;

	int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());
	for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
	{
		CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nfeno);
		if (pFeature == nullptr) continue;

		//교집합 체크 : Draw 시간을 줄이기 위함
		{

			RECTD drtFeature;
			if (pRotMirrArr == nullptr)
			{
				drtFeature = pFeature->m_MinMaxRect;
			}
			else
			{
				pRotMirrArr->FinalPoint(&drtFeature.left, &drtFeature.top, pFeature->m_MinMaxRect.left, pFeature->m_MinMaxRect.top);
				pRotMirrArr->FinalPoint(&drtFeature.right, &drtFeature.bottom, pFeature->m_MinMaxRect.right, pFeature->m_MinMaxRect.bottom);
				drtFeature.NormalizeRectD();
			}

			if (drtFeature.IsIntersection(drtView_mm) == FALSE)
			{
				continue;
			}
		}

		if (RESULT_BAD == _SetFeatureData_V3(vecvecvecFeature, pFeature, pRotMirrArr, vecvecLineWidth))
		{
			continue;
		}

	}


	return RESULT_GOOD;
}

UINT32 CDirectView::_GetFeature_SubStep_V3(vector < vector<vector<CFeature*>>> &vecvecvecFeature, CSubStep *pSubStep, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, vector< vector<LINEWIDTH>> &vecvecLineWidth, RECTD &drtView_mm)
{
	if (pSubStep == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;

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
					rotMirrArr->FinalPoint(&drtFeature.left, &drtFeature.top, pStep->m_FeatureMinMax.left, pStep->m_FeatureMinMax.top);
					rotMirrArr->FinalPoint(&drtFeature.right, &drtFeature.bottom, pStep->m_FeatureMinMax.right, pStep->m_FeatureMinMax.bottom);
					drtFeature.NormalizeRectD();
				}

				if (drtFeature.IsIntersection(drtView_mm) == FALSE)
				{
					rotMirrArr->RemoveTail();
					continue;
				}
			}

			_GetFeature_V3(vecvecvecFeature, pStep, pLayerSet, pRotMirrArr, vecvecLineWidth, drtView_mm);

			//SubSub
			for (UINT j = 0; j < pSubStep->m_nNextStepCnt; j++)
			{
				CSubStep* pSubSubStep = pSubStep->m_arrNextStep.GetAt(j);

				_GetFeature_SubStep_V3(vecvecvecFeature, pSubSubStep, pLayerSet, pRotMirrArr, vecvecLineWidth, drtView_mm);
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
UINT32 CDirectView::_SetFeatureData_V3(vector < vector<vector<CFeature*>>> &vecvecvecFeature, CFeature *pFeature, CRotMirrArr *pRotMirrArr, vector< vector<LINEWIDTH>> &vecvecLineWidth)
{
	//vecvecvecFeature : [LineOption][LineWidth][Feature]
	//vecvecLineWidth : [LineOption][LineWidth]

	if (pFeature == nullptr) return RESULT_BAD;
	if (pRotMirrArr == nullptr) return RESULT_BAD;

	CFeature *pFeature_New = nullptr;
	double dTempX, dTempY;
	

	//feature type : P
	if (pFeature->m_eType == FeatureType::P) return RESULT_BAD;
	//feature type : T
	if (pFeature->m_eType == FeatureType::T) return RESULT_BAD;
	//feature type : S
	if (pFeature->m_eType == FeatureType::S) return RESULT_BAD;
	//feature type : B
	if (pFeature->m_eType == FeatureType::B) return RESULT_BAD;

	//
	vector<int> vecSelectLineOption;
	
	if (pFeature->m_bPolarity == true)
	{
		if (pFeature->m_eType == FeatureType::L)
		{
			vecSelectLineOption.push_back(eLineWidthOption::eLinePos);
			vecSelectLineOption.push_back(eLineWidthOption::eLineArcPos);
		}
		else if (pFeature->m_eType == FeatureType::A)
		{
			vecSelectLineOption.push_back(eLineWidthOption::eArcPos);
			vecSelectLineOption.push_back(eLineWidthOption::eLineArcPos);
		}
	}
	else
	{
		if (pFeature->m_eType == FeatureType::L)
		{
			vecSelectLineOption.push_back(eLineWidthOption::eLineNeg);
			vecSelectLineOption.push_back(eLineWidthOption::eLineArcNeg);
		}
		else if (pFeature->m_eType == FeatureType::A)
		{
			vecSelectLineOption.push_back(eLineWidthOption::eArcNeg);
			vecSelectLineOption.push_back(eLineWidthOption::eLineArcNeg);
		}
	}
	int nLineOptionCount = static_cast<int>(vecSelectLineOption.size());
	if (nLineOptionCount == 0) return RESULT_BAD;

	
	for (int i = 0; i < nLineOptionCount; i++)
	{
		int nLineOption = vecSelectLineOption[i];

		switch (pFeature->m_eType)
		{
		case FeatureType::L:
		{
			pFeature_New = new CFeatureL();
			*pFeature_New = *pFeature;
			pFeature_New->m_pSymbol = pFeature->m_pSymbol;

			CFeatureL *pFeatureL_New = (CFeatureL*)pFeature_New;
			*pFeatureL_New = *(CFeatureL*)pFeature;

			pRotMirrArr->FinalPoint(&dTempX, &dTempY, pFeatureL_New->m_dXs, pFeatureL_New->m_dYs);
			pFeatureL_New->m_dXs = dTempX;
			pFeatureL_New->m_dYs = dTempY;

			pRotMirrArr->FinalPoint(&dTempX, &dTempY, pFeatureL_New->m_dXe, pFeatureL_New->m_dYe);
			pFeatureL_New->m_dXe = dTempX;
			pFeatureL_New->m_dYe = dTempY;
		}
		break;
		case FeatureType::A:
		{
			pFeature_New = new CFeatureA();
			*pFeature_New = *pFeature;
			pFeature_New->m_pSymbol = pFeature->m_pSymbol;

			CFeatureA *pFeatureA_New = (CFeatureA*)pFeature_New;
			*pFeatureA_New = *(CFeatureA*)pFeature;

			pRotMirrArr->FinalPoint(&dTempX, &dTempY, pFeatureA_New->m_dXs, pFeatureA_New->m_dYs);
			pFeatureA_New->m_dXs = dTempX;
			pFeatureA_New->m_dYs = dTempY;

			pRotMirrArr->FinalPoint(&dTempX, &dTempY, pFeatureA_New->m_dXe, pFeatureA_New->m_dYe);
			pFeatureA_New->m_dXe = dTempX;
			pFeatureA_New->m_dYe = dTempY;

			pRotMirrArr->FinalPoint(&dTempX, &dTempY, pFeatureA_New->m_dXc, pFeatureA_New->m_dYc);
			pFeatureA_New->m_dXc = dTempX;
			pFeatureA_New->m_dYc = dTempY;
		}
		break;
		default:
			break;
		}

		if (pFeature_New == nullptr) return RESULT_BAD;

		int nLineWidthSize = static_cast<int>(vecvecLineWidth.size());
		int nSelectLineWidth = -1;
		for (int nLineWidth = 0; nLineWidth < nLineWidthSize; nLineWidth++)
		{
			if (pFeature_New->m_pSymbol->m_strUserSymbol.Mid(1).CompareNoCase(vecvecLineWidth[nLineOption][nLineWidth].strLineWidth) == 0)
			{
				nSelectLineWidth = i;
				break;
			}
		}

		if (nSelectLineWidth == -1)
		{
			delete pFeature_New;
			return RESULT_BAD;
		}

		vecvecvecFeature[nLineOption][nSelectLineWidth].push_back(pFeature_New);
	}

	return RESULT_GOOD;
}
//////////////////////////////////////////////////////////////////////////

//
//UINT32 CDirectView::_GetFeatureInfo(IN const D2D1_POINT_2F &fptPoint_pixel, SELECT_INFO_UNIT &stSelectInfo)
UINT32 CDirectView::_GetFeatureInfo(IN D2D1_POINT_2F &fptClickCenter_mm, IN D2D1_POINT_2F &fptClickCenter_Pixel, SELECT_INFO_UNIT &stSelectInfo)
{
	stSelectInfo._Reset();

	if (m_pUserSetInfo == nullptr) return RESULT_BAD;
	if (m_pUserLayerSetInfo == nullptr) return RESULT_BAD;
	if (m_nSelectStep == -1 ) return RESULT_BAD;
	if (m_pJobData == nullptr) return RESULT_BAD;
	if (m_pDrawFunction == nullptr) return RESULT_BAD;

	vector<CFeature*> vecFeature;
	vector<CRotMirrArr*> vecRotMirr;

	CStep *pStep;
	if (m_vecRender.size() <= 0) return RESULT_BAD;
	if (m_vecRender[0] == nullptr) return RESULT_BAD;
	//RECTD drtView = m_pDrawFunction->GetViewRect_mm(m_vecRender[0], FALSE);
	
	//클릭된 좌표에서 10mm x 10mm 를 검색한다.
	double dWidth_hs = 5;

	//Cur Step
	if (m_pJobData->m_arrStep.GetCount() <= m_nSelectStep) return RESULT_BAD;
	pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr) return RESULT_BAD;

	Orient ori = Orient::NoMir0Deg;
	CDrawFunction::GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, ori);

	CRotMirrArr rotMirrArr;
	rotMirrArr.InputData(0.0, 0.0, Orient::NoMir0Deg, m_pUserSetInfo->mirrorDir);

 	RECTD drtView = RECTD(fptClickCenter_mm.x - dWidth_hs, fptClickCenter_mm.y - dWidth_hs,
 		fptClickCenter_mm.x + dWidth_hs, fptClickCenter_mm.y + dWidth_hs);
	drtView.NormalizeRectD();

	//CurStep
	_GetFeature_RotMirr_View(pStep, &rotMirrArr, drtView, vecFeature, vecRotMirr);

	//SubStep
	for (UINT i = 0; i < pStep->m_nSubStepCnt; i++)
	{
		CSubStep* pSubStep = pStep->m_arrSubStep[i];
		if (pSubStep == nullptr) continue; 

		_GetFeature_RotMirr_View_SubStep(pSubStep, &rotMirrArr, drtView, vecFeature, vecRotMirr);
	}
	
	//Geo를 만들자.
	vector<vector<DrawFeatureGeo*>> vecvecDrawFeatureGeo;//Surface내의 ObOe로 인하여 이중으로 배열 구성(다른 Feature 일경우 [nFeature][0], Surface일 경우 [nfeature][nObOe]
	m_pDrawFunction->MakeFeatureGeo(vecFeature, vecRotMirr, vecvecDrawFeatureGeo);
	
	//Point에 터치인지 체크
	vector<vector<DrawFeatureGeo*>> vecvecTouch;//

	_GetCheckFeature_Touch(fptClickCenter_mm, fptClickCenter_Pixel, vecFeature, vecvecDrawFeatureGeo, vecvecTouch);
	
	//터치 된 Geo중 가장 작은 크기인 피처 정보 가져오기
	int nMinArea_Index_1 = -1;//FeatureIndex
	int nMinArea_Index_2 = -1;//ObOeIndex
	double dMinArea = DBL_MAX;
	D2D1_RECT_F frtRect;
	int nFeatureCount = static_cast<int>(vecvecTouch.size());
	HRESULT hRet;

	float fViewWidth = static_cast<float>(drtView.width());
	float fViewHeight = static_cast<float>(drtView.height());
	float fViewArea = fViewWidth * fViewHeight;
	
	for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
	{
		if (vecFeature.size() <= 0) continue;
		if (vecFeature.size() <= nfeno) continue;
		if (vecFeature[nfeno] == nullptr) continue;

		//Line을 우선 순위를 둠
		if (vecFeature[nfeno]->m_eType == FeatureType::L)
		{//Line은 Area를 계산하지 않는다.//Line과의 Touch가 있다면 Line을 우선적으로 선택한다.

			//터치가 되었는지 체크
			if ( vecvecTouch[nfeno][0] == nullptr) continue;
			if (vecvecTouch[nfeno][0]->pGeo == nullptr) continue;

			CFeatureL *pFeatureL = (CFeatureL*)vecFeature[nfeno];
			if (pFeatureL->m_dXs != pFeatureL->m_dXe ||
				pFeatureL->m_dYs != pFeatureL->m_dYe)
			{

				nMinArea_Index_1 = nfeno;
				nMinArea_Index_2 = 0;

				break;
			}
		}
		else if (vecFeature[nfeno]->m_eType == FeatureType::A)
		{
			//터치가 되었는지 체크
			if (vecvecTouch[nfeno][0] == nullptr) continue;
			if (vecvecTouch[nfeno][0]->pGeo == nullptr) continue;

			nMinArea_Index_1 = nfeno;
			nMinArea_Index_2 = 0;

			break;
		}

		int nSubCount = static_cast<int>(vecvecTouch[nfeno].size());
		for (int nSub = 0; nSub < nSubCount; nSub++)
		{
			if (vecvecTouch[nfeno][nSub] == nullptr ) continue;
			if (vecvecTouch[nfeno][nSub]->pGeo == nullptr) continue;

			

			float fArea = 0;
			hRet = vecvecTouch[nfeno][nSub]->pGeo->ComputeArea(D2D1::Matrix3x2F().Identity(), &fArea);
			if (hRet != S_OK) continue;
			hRet = vecvecTouch[nfeno][nSub]->pGeo->GetBounds(D2D1::Matrix3x2F().Identity(), &frtRect);
			if (hRet != S_OK) continue;

			float fFeatureWidth = abs(frtRect.right - frtRect.left);
			float fFeatureHeight = abs(frtRect.bottom - frtRect.top);

			//View하는 영역보다 영역은 필터링
// 			if (fArea >= fViewArea) continue;
// 			if (fFeatureWidth >= fViewWidth) continue;
// 			if (fFeatureHeight >= fViewHeight) continue;
			if (fArea <= 0) continue;
			if (fFeatureWidth <= 0.f) continue;
			if (fFeatureHeight <= 0.f) continue;

			if (dMinArea > fArea)
			{
				dMinArea = fArea;
				nMinArea_Index_1 = nfeno;
				nMinArea_Index_2 = nSub;
			}
		}
	}

	BOOL bFind = FALSE;
	if (nMinArea_Index_1 != -1 && nMinArea_Index_2 != -1 &&
		nMinArea_Index_1 < vecvecTouch.size() &&
		nMinArea_Index_2 < vecvecTouch[nMinArea_Index_1].size())
	{
		//Line을 우선 순위를 둠
		if (vecFeature[nMinArea_Index_1]->m_eType == FeatureType::L)
		{//Line은 Area를 계산하지 않는다.//Line과의 Touch가 있다면 Line을 우선적으로 선택한다.

			stSelectInfo.pFeature = vecFeature[nMinArea_Index_1];
			stSelectInfo.arrRotMirr = *vecRotMirr[nMinArea_Index_1];

			stSelectInfo.emFeatureType = vecFeature[nMinArea_Index_1]->m_eType;

			stSelectInfo.dCx = fptClickCenter_mm.x;
			stSelectInfo.dCy = fptClickCenter_mm.y;

			//Test
// 			double dPointX = stSelectInfo.pFeature->m_MinMaxRect.CenterX();
// 			double dPointY = stSelectInfo.pFeature->m_MinMaxRect.CenterY();
// 			stSelectInfo.arrRotMirr.FinalPoint(&dPointX, &dPointY, dPointX, dPointY);
// 
// 			stSelectInfo.dCx = dPointX;
// 			stSelectInfo.dCy = dPointY;
			//

			bFind = TRUE;
		}
		else if (vecFeature[nMinArea_Index_1]->m_eType == FeatureType::A)
		{
			stSelectInfo.pFeature = vecFeature[nMinArea_Index_1];
			stSelectInfo.arrRotMirr = *vecRotMirr[nMinArea_Index_1];

			stSelectInfo.emFeatureType = vecFeature[nMinArea_Index_1]->m_eType;

			stSelectInfo.dCx = fptClickCenter_mm.x;
			stSelectInfo.dCy = fptClickCenter_mm.y;
			bFind = TRUE;
		}
		else
		{
			D2D1_RECT_F frtRect;
			vecvecTouch[nMinArea_Index_1][nMinArea_Index_2]->pGeo->GetBounds(D2D1::Matrix3x2F().Identity(), &frtRect);

			double dWidth = abs(frtRect.right - frtRect.left);
			double dHeight = abs(frtRect.bottom - frtRect.top);

			stSelectInfo.pFeature = vecFeature[nMinArea_Index_1];
			stSelectInfo.arrRotMirr = *vecRotMirr[nMinArea_Index_1];

			stSelectInfo.emFeatureType = vecFeature[nMinArea_Index_1]->m_eType;

			stSelectInfo.dWidth = dWidth;//MAX(dWidth, dHeight);
			stSelectInfo.dHeight = dHeight;
			stSelectInfo.dCx = (frtRect.right + frtRect.left) / 2.;
			stSelectInfo.dCy = (frtRect.bottom + frtRect.top) / 2.;

			bFind = TRUE;
		}
	}
	

	//Delete
	int nCount = static_cast<int>(vecRotMirr.size());
	for (int i = 0; i < nCount; i++)
	{
		delete vecRotMirr[i];
		vecRotMirr[i] = nullptr;
	}
	vecRotMirr.clear();

	nCount = static_cast<int>(vecvecDrawFeatureGeo.size());
	for (int i = 0; i < nCount; i++)
	{
		int nCount2 = static_cast<int>(vecvecDrawFeatureGeo[i].size());
		for (int j = 0; j < nCount2; j++)
		{
			vecvecDrawFeatureGeo[i][j]->Clear();
			delete vecvecDrawFeatureGeo[i][j];
			vecvecDrawFeatureGeo[i][j] = nullptr;
		}
		vecvecDrawFeatureGeo[i].clear();
	}
	vecvecDrawFeatureGeo.clear();
	//

	if (bFind == FALSE)
	{
		return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 CDirectView::_GetFeature_RotMirr_View(CStep *pStep, CRotMirrArr *pRotMirr, RECTD &drtView, vector<CFeature*> &vecFeature, vector<CRotMirrArr*> &vecRotMirr)
{
	CLayer *pPatternLayer = CDrawFunction::_GetLayerInfo(m_pUserLayerSetInfo, pStep, PATTERN_LAYER_TXT);
	int nFeatureCount = static_cast<int>(pPatternLayer->m_FeatureFile.m_arrFeature.GetCount());

	for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
	{
		CFeature *pFeature = pPatternLayer->m_FeatureFile.m_arrFeature.GetAt(nfeno);

		//교집합 체크 : 시간을 줄이기 위함
		{
			RECTD drtFeature;

			pRotMirr->FinalPoint(&drtFeature.left, &drtFeature.top, pFeature->m_MinMaxRect.left, pFeature->m_MinMaxRect.top);
			pRotMirr->FinalPoint(&drtFeature.right, &drtFeature.bottom, pFeature->m_MinMaxRect.right, pFeature->m_MinMaxRect.bottom);
			drtFeature.NormalizeRectD();

			if (drtFeature.IsIntersection(drtView) == FALSE)
			{
				continue;
			}
		}

		vecFeature.push_back(pFeature);
		CRotMirrArr *pTemp = new CRotMirrArr();
		*pTemp = *pRotMirr;
		vecRotMirr.push_back(pTemp);
	}

	return RESULT_GOOD;
}


UINT32 CDirectView::_GetFeature_RotMirr_View_SubStep(CSubStep *pSubStep, CRotMirrArr *pRotMirrArr, RECTD &drtView, vector<CFeature*> &vecFeature, vector<CRotMirrArr*> &vecRotMirr)
{
	if (pSubStep == nullptr) return RESULT_BAD;

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

	CStep *pStep = pSubStep->m_pSubStep;
	//if (pStep == nullptr) continue;
	if (pStep == nullptr) return RESULT_BAD;

	CLayer *pPatternLayer = CDrawFunction::_GetLayerInfo(m_pUserLayerSetInfo, pStep, PATTERN_LAYER_TXT);
	if (pPatternLayer == nullptr) return RESULT_BAD;

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

	for (int y_step = 0; y_step < static_cast<int>(pSubStepRepeat->m_nNY); y_step++)
	{
		for (int x_step = 0; x_step < static_cast<int>(pSubStepRepeat->m_nNX); x_step++)
		{
			dRefx = pSubStep->m_dXDatum;
			dRefy = pSubStep->m_dYDatum;

			rotMirrArr->ConvertPoint(&dParentX, &dParentY, 0, 0, dRefx, dRefy, eSubOrient);
			dParentX = (pSubStepRepeat->m_dX - dParentX) + pSubStepRepeat->m_dDX * x_step;
			dParentY = (pSubStepRepeat->m_dY - dParentY) + pSubStepRepeat->m_dDY * y_step;

			rotMirrArr->InputData(dParentX, dParentY, eSubOrient);

			RECTD dptPicture = pStep->m_Profile.m_MinMax;

			//교집합 체크 : 시간을 줄이기 위함
			{
				RECTD drtFeature;

				rotMirrArr->FinalPoint(&drtFeature.left, &drtFeature.top, dptPicture.left, dptPicture.top);
				rotMirrArr->FinalPoint(&drtFeature.right, &drtFeature.bottom, dptPicture.right, dptPicture.bottom);
				drtFeature.NormalizeRectD();

				if (drtFeature.IsIntersection(drtView) == FALSE)
				{
					rotMirrArr->RemoveTail();
					continue;
				}
			}

			//SubSub
			for (UINT j = 0; j < pSubStep->m_nNextStepCnt; j++)
			{
				CSubStep* pSubSubStep = pSubStep->m_arrNextStep.GetAt(j);

				_GetFeature_RotMirr_View_SubStep(pSubSubStep, rotMirrArr, drtView, vecFeature, vecRotMirr);

			}//Sub Unit End

			int nFeatureCount = static_cast<int>(pPatternLayer->m_FeatureFile.m_arrFeature.GetCount());
			for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
			{
				CFeature *pFeature = pPatternLayer->m_FeatureFile.m_arrFeature.GetAt(nfeno);

				//교집합 체크 : 시간을 줄이기 위함
				{

					RECTD drtFeature;

					rotMirrArr->FinalPoint(&drtFeature.left, &drtFeature.top, pFeature->m_MinMaxRect.left, pFeature->m_MinMaxRect.top);
					rotMirrArr->FinalPoint(&drtFeature.right, &drtFeature.bottom, pFeature->m_MinMaxRect.right, pFeature->m_MinMaxRect.bottom);
					drtFeature.NormalizeRectD();

					if (drtFeature.IsIntersection(drtView) == FALSE)
					{
						continue;
					}
				}

				vecFeature.push_back(pFeature);
				CRotMirrArr *pTemp = new CRotMirrArr();
				*pTemp = *rotMirrArr;
				vecRotMirr.push_back(pTemp);
			}



			rotMirrArr->RemoveTail();
		}
	}



	if (bCreateRotMirr == TRUE)
	{
		rotMirrArr->ClearAll();
		delete rotMirrArr;
		rotMirrArr = nullptr;
	}

	return RESULT_GOOD;
}

UINT32 CDirectView::_GetCheckFeature_Touch(IN D2D1_POINT_2F &fptClickCenter_mm, IN const D2D1_POINT_2F &fptPoint_pixel, vector<CFeature*> &vecFeature,
	vector<vector<DrawFeatureGeo*>> &vecDrawFeatureGeo,	OUT vector<vector<DrawFeatureGeo*>> &vecvecTouchFeatureGeo)
{
	int nFeatureCount = static_cast<int>(vecFeature.size());
	if (nFeatureCount == 0) return RESULT_BAD;
	
	//Surface
	vector<TempSelectSurface> vecSelectSurface;

	D2D1_POINT_2F fptPoint;
	D2D1_MATRIX_3X2_F matScale;
	if (fptPoint_pixel.x == -999.f && fptPoint_pixel.y == -999.f)
	{//뷰어 좌표가 없다면 실제 좌표계 사용
		fptPoint = fptClickCenter_mm;//실제 좌표
		matScale = D2D1::Matrix3x2F::Identity();
	}
	else
	{
		fptPoint = fptPoint_pixel;//뷰어 좌표
		matScale = m_pDrawManager->GetViewScale();
	}

	int nTouchCount = 0;
	vecvecTouchFeatureGeo.resize(nFeatureCount);
	for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
	{
		CFeature *pFeature = vecFeature[nfeno];
		
		switch (pFeature->m_eType)
		{
		case FeatureType::L:
		{
			CFeatureL *pFeatureL = (CFeatureL *)pFeature;
			CSymbol *pSymbol = pFeatureL->m_pSymbol;
			if (vecDrawFeatureGeo[nfeno][0]->pGeo != nullptr)
			{
				if (m_pDrawFunction->CheckHighlight_LineFeature(fptPoint, pFeatureL, pSymbol, matScale, vecDrawFeatureGeo[nfeno][0]->pGeo) == TRUE)
					//if (m_pDrawFunction->CheckHighlight_LineFeature(fptPoint_pixel, pFeatureL, pSymbol, matIdentity, vecDrawFeatureGeo[nfeno][0]->pGeo) == TRUE)
				{
					vecvecTouchFeatureGeo[nfeno].push_back(vecDrawFeatureGeo[nfeno][0]);
					nTouchCount++;
				}
				else
				{
					vecvecTouchFeatureGeo[nfeno].push_back(nullptr);
				}
			}
			else
			{
				vecvecTouchFeatureGeo[nfeno].push_back(nullptr);
			}
		}
		break;
		case FeatureType::P:
		{
			CFeatureP *pFeatureP = (CFeatureP *)pFeature;
			CSymbol *pSymbol = pFeatureP->m_pSymbol;

			if (vecDrawFeatureGeo[nfeno][0]->pGeo != nullptr)
			{
				if (m_pDrawFunction->CheckHighlight_PadFeature(fptPoint, pFeatureP, pSymbol, matScale, vecDrawFeatureGeo[nfeno][0]->pGeo) == TRUE)
					//if (m_pDrawFunction->CheckHighlight_PadFeature(fptPoint_pixel, pFeatureP, pSymbol, matIdentity, vecDrawFeatureGeo[nfeno][0]->pGeo) == TRUE)
				{
					vecvecTouchFeatureGeo[nfeno].push_back(vecDrawFeatureGeo[nfeno][0]);
					nTouchCount++;
				}
				else
				{
					vecvecTouchFeatureGeo[nfeno].push_back(nullptr);
				}
			}
			else
			{
				vecvecTouchFeatureGeo[nfeno].push_back(nullptr);
			}
		}
		break;
		case FeatureType::A:
		{
			CFeatureA *pFeatureA = (CFeatureA *)pFeature;
			CSymbol *pSymbol = pFeatureA->m_pSymbol;
			if (vecDrawFeatureGeo[nfeno][0]->pGeo != nullptr)
			{
				if (m_pDrawFunction->CheckHighlight_ArcFeature(fptPoint, pFeatureA, pSymbol, matScale, vecDrawFeatureGeo[nfeno][0]->pGeo) == TRUE)
					//if (m_pDrawFunction->CheckHighlight_ArcFeature(fptPoint_pixel, pFeatureA, pSymbol, matIdentity, vecDrawFeatureGeo[nfeno][0]->pGeo) == TRUE)
				{
					vecvecTouchFeatureGeo[nfeno].push_back(vecDrawFeatureGeo[nfeno][0]);
					nTouchCount++;
				}
				else
				{
					vecvecTouchFeatureGeo[nfeno].push_back(nullptr);
				}
			}
			else
			{
				vecvecTouchFeatureGeo[nfeno].push_back(nullptr);
			}
		}
		break;
		case FeatureType::S:
		{
			CFeatureS *pFeatureS = (CFeatureS *)pFeature;
			vector<ID2D1PathGeometry *> vecGeo;
			for (auto pDrawFeatureGeo : vecDrawFeatureGeo[nfeno])
			{
				vecGeo.push_back(pDrawFeatureGeo->pGeo);
			}

			int nObOe = -1;
			float fArea;
			if (m_pDrawFunction->CheckHighlight_SurfaceFeature(fptPoint, pFeatureS, nullptr, matScale, nObOe, fArea, vecGeo) == TRUE)
			//if (m_pDrawFunction->CheckHighlight_SurfaceFeature(fptPoint_pixel, pFeatureS, nullptr, matIdentity, nObOe, fArea, vecGeo) == TRUE)
			{
				int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
				for (int i = 0; i < nObOeCount; i++)
				{
					vecvecTouchFeatureGeo[nfeno].push_back(nullptr);
				}

				vecvecTouchFeatureGeo[nfeno][nObOe] = vecDrawFeatureGeo[nfeno][nObOe];
				nTouchCount++;
			}
			else
			{
				int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
				for (int i = 0; i < nObOeCount; i++)
				{
					vecvecTouchFeatureGeo[nfeno].push_back(nullptr);
				}
			}
		}
		break;
		default:
			break;
		}
	}

	return RESULT_GOOD;
}

SELECT_INFO*	CDirectView::SetSelectInfo(D2D_POINT_2F fptPoint_mm, D2D_POINT_2F fptPoint_Pixel)
{
	
	SELECT_INFO_UNIT stSelectInfo_Unit;	

	UINT32 nRet = _GetFeatureInfo(fptPoint_mm, fptPoint_Pixel, stSelectInfo_Unit);

	if (nRet == RESULT_GOOD)
	{
		m_stSelectInfo.vecSelectInfoUnit.push_back(stSelectInfo_Unit);
	}	

	return &m_stSelectInfo;
}


SIZE CDirectView::GetFOVSize(bool bIs3D, double dLens, double dZoom)
{
	SIZE fovSize;
	fovSize.cx = fovSize.cy = -1;

	std::vector<NSIS_CameraSpec_t*>* pSysCamera = nullptr;
	if (bIs3D)
		pSysCamera = &m_pSystemSpec->sysNSIS.sys3DCamera;
	else
		pSysCamera = &m_pSystemSpec->sysNSIS.sys2DCamera;

	if (pSysCamera == nullptr) return fovSize;

	bool bFind = false;
	for (int i = 0; i < pSysCamera->size(); i++)
	{
		NSIS_CameraSpec_t *pCameraSpec = pSysCamera->at(i);
		if (pCameraSpec == nullptr)
			continue;

		if (pCameraSpec->dMagnification == dLens && pCameraSpec->dZoom == dZoom)
		{
			bFind = true;
			fovSize.cx = pCameraSpec->iFovWidth;
			fovSize.cy = pCameraSpec->iFovHeight;
			break;
		}
	}

	return fovSize;
}