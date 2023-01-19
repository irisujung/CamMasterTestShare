#include "pch.h"

#include "CDrawLayer.h"
#include "DxDraw/CBackBuffer.h"

CDrawLayer::CDrawLayer()
{
}

CDrawLayer::~CDrawLayer()
{
	if (m_pDrawFunction != nullptr)
	{
		delete m_pDrawFunction;
		m_pDrawFunction = nullptr;
	}
	
	_ClearBackBuffer();
	
}

UINT32 CDrawLayer::SetLink(IN ID2D1Factory1 *pFactory, IN ID2D1DeviceContext* pD2DContext, IN SystemSpec_t *pSystemSpec)
{
	if (pFactory == nullptr) return RESULT_BAD;
	if (pD2DContext == nullptr) return RESULT_BAD;

	m_pFactory = pFactory;
	m_pD2DContext = pD2DContext;
	m_pSystemSpec = pSystemSpec;

	m_pDrawFunction = new CDrawFunction();
	m_pDrawFunction->_SetLink(m_pFactory, m_pD2DContext);
	
	return RESULT_GOOD;
}

UINT32 CDrawLayer::SetDrawOnly_MeasureUnit(IN const BOOL &bMeaureUnit, vector<MeasureUnit>& vecMeasureUnitInfo)
{
	m_bMeasureUnitOnly = bMeaureUnit;
	m_vecMeasureUnitInfo = vecMeasureUnitInfo;

	return RESULT_GOOD;
}

UINT32 CDrawLayer::ResetView()
{
	m_strLayerName = _T("");

	_ClearBackBuffer();

	return RESULT_GOOD;
}


UINT32 CDrawLayer::OnRender(IN ID2D1BitmapRenderTarget* pRender, IN CJobFile *pJobFile, const int &nStepIndex, IN const LayerSet_t* pLayerSet,
	IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo, IN const BOOL &bEditMode)
{
	if (pJobFile == nullptr) return RESULT_BAD;
	if (nStepIndex < 0) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;

	int nStepCount = static_cast<int>(pJobFile->m_arrStep.GetCount());
	if (nStepCount <= 0) return RESULT_BAD;
	if (nStepCount <= nStepIndex) return RESULT_BAD;

	CStep *pStep = pJobFile->m_arrStep[nStepIndex];
	if (pStep == nullptr) return RESULT_BAD;

	if (pUserSetInfo->bShowPanelDummy == false &&
		m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS &&
		pStep->m_strStepName == _T("PANEL") )
	{
		return RESULT_GOOD;
	}
	
	int nLayerIndex;//사용안함
	CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, pLayerSet->strLayer, nLayerIndex);
	if (pLayer == nullptr)
		return RESULT_BAD;

	RECTD drtViewRect_mm = m_pDrawFunction->GetViewRect_mm(pRender, pUserSetInfo, FALSE);
	drtViewRect_mm.NormalizeRectD();

	if (pLayer != nullptr)
	{//현재 Step의 Layer Draw
		m_pDrawFunction->OnRender_Layer(pRender, pLayer, pLayerSet, &pJobFile->m_arrFont, nullptr, bNeedUpdate, pUserSetInfo, drtViewRect_mm, FALSE);
		m_pDrawFunction->OnRender_Layer(pRender, pLayer, pLayerSet, &pJobFile->m_arrFont, nullptr, bNeedUpdate, pUserSetInfo, drtViewRect_mm, TRUE);//선택된 것만 나중에 한번더 그려야함.
	}

	return RESULT_GOOD;
}

UINT32 CDrawLayer::OnRender_SubStep(IN ID2D1BitmapRenderTarget* pRender, IN CJobFile *pJobFile, const int &nStepIndex, IN const LayerSet_t* pLayerSet,
	IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo, IN const BOOL &bEditMode)
{
	if (pJobFile == nullptr) return RESULT_BAD;
	if (nStepIndex < 0) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;

	int nStepCount = static_cast<int>(pJobFile->m_arrStep.GetCount());
	if (nStepCount <= 0) return RESULT_BAD;
	if (nStepCount <= nStepIndex) return RESULT_BAD;

	CStep *pStep = pJobFile->m_arrStep[nStepIndex];
	if (pStep == nullptr) return RESULT_BAD;
	
	//V2
	vector<SUBSTEP_INFO> vecSubStepInfo = _GetSubStepInfo(pStep);
	if (_IsNeedMaskBackBuffer(vecSubStepInfo) == TRUE ||
		(m_strLayerName != pLayerSet->strLayer) ||
		m_bDrawSurface != pUserSetInfo->bShowSurface ||
		m_nCurStepIndex != nStepIndex)//Layer가 변경되었다면.
	{
		if (RESULT_GOOD == _MakeBackBuffer_V2(vecSubStepInfo))
		{
			int nCount = static_cast<int>(vecSubStepInfo.size());
			for (int i = 0; i < nCount; i++)
			{
				_DrawBackBuffer_SubStep_V2(pJobFile, pLayerSet, &pJobFile->m_arrFont, pUserSetInfo, &m_vecDrawBackBuffer[i]);
			}

			//설정 업데이트
			m_strLayerName = pLayerSet->strLayer;
			m_bDrawSurface = pUserSetInfo->bShowSurface;
			m_nCurStepIndex = nStepIndex;
		}
	}

	//SubStep Draw
	for (UINT i = 0; i < pStep->m_nSubStepCnt; i++)
	{
		CSubStep* pSubStep = pStep->m_arrSubStep[i];
		if (pSubStep == nullptr) continue;

		//Draw SubStep
		_Render_SubStep_V2(pRender, pSubStep, i, pLayerSet, &pJobFile->m_arrFont, nullptr, bNeedUpdate, pUserSetInfo, bEditMode, 0);
	}
	//

	return RESULT_GOOD;
}

UINT32 CDrawLayer::_Render_SubStep(IN ID2D1RenderTarget* pRender, CSubStep* pSubStep, IN const int &nSubIndex, 
	IN const LayerSet_t* pLayerSet,	CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo,
	IN const BOOL &bEditMode, IN int &nSubLevel, SUBSTEP_BACKBUFFER *pSubStep_BackBuffer)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pSubStep == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (pSubStep_BackBuffer == nullptr) return RESULT_BAD;
	if (pSubStep_BackBuffer->pBackbuffer == nullptr) return RESULT_BAD;

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

	ID2D1Bitmap* pBitmap = pSubStep_BackBuffer->pBackbuffer->GetDrawBitmap();

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

			

			CStep *pStep = pSubStep->m_pSubStep;
			//if (pStep == nullptr) continue;
			if (pStep == nullptr) return RESULT_BAD;

			D2D1_RECT_F fptPicture = D2D1::RectF(DoubleToFloat(pStep->m_Profile.m_MinMax.left), DoubleToFloat(pStep->m_Profile.m_MinMax.top),
				DoubleToFloat(pStep->m_Profile.m_MinMax.right), DoubleToFloat(pStep->m_Profile.m_MinMax.bottom));
			
			//dParentX
			double dXs, dYs;
			double dXe, dYe;
			rotMirrArr->FinalPoint(&dXs, &dYs, fptPicture.left, fptPicture.top);
			rotMirrArr->FinalPoint(&dXe, &dYe, fptPicture.right, fptPicture.bottom);
			D2D1_RECT_F frtDst = D2D1::RectF(DoubleToFloat(dXs), DoubleToFloat(dYs), DoubleToFloat(dXe), DoubleToFloat(dYe));
			frtDst = CDrawFunction::NormalizeRect(frtDst);

			if (pBitmap != nullptr)
			{
				pRender->DrawBitmap(pBitmap, frtDst, 1, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
			}
				
			//SubSub
			for (UINT j = 0; j < pSubStep->m_nNextStepCnt; j++)
			{
				CSubStep* pSubSubStep = pSubStep->m_arrNextStep.GetAt(j);

				_Render_SubStep(pRender, pSubSubStep, j, pLayerSet, pFontArr, rotMirrArr, bNeedUpdate, pUserSetInfo, bEditMode, nSubLevel,
					&pSubStep_BackBuffer->vecSubStep[j]);

 			}//Sub Unit End


			
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

UINT32 CDrawLayer::_Render_SubStep_V2(IN ID2D1RenderTarget* pRender, CSubStep* pSubStep, IN const int &nSubIndex,
	IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo,
	IN const BOOL &bEditMode, IN int nSubLevel, IN int nParent_StepX, IN int nParent_StepY, IN int nParentSubStepIndex)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pSubStep == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;

	CStepRepeat* pSubStepRepeat = &(pSubStep->m_arrStepRepeat);
	if (pSubStepRepeat == nullptr)
		return RESULT_BAD;

	CStep *pStep = pSubStep->m_pSubStep;
	if (pStep == nullptr) return RESULT_BAD;

	CString strStepName = pStep->m_strStepName;

	BOOL bMeasureUnitOnly = m_bMeasureUnitOnly;
	if (pUserSetInfo->bShowStepRepeat)
		bMeasureUnitOnly = FALSE;

	std::vector<std::pair<INT32, INT32>> vcMeasure;
	if (bMeasureUnitOnly)
	{		
		bool bFind = false;
		for (auto it : m_vecMeasureUnitInfo)
		{
			if (nSubLevel > 0)
			{
				if (it.nSubSubIndex == nSubIndex)
				{
					if (it.nSubIndex == nParentSubStepIndex && it.nStripX == nParent_StepX && it.nStripY == nParent_StepY)
						vcMeasure.emplace_back(std::make_pair(it.nUnitX, it.nUnitY));
				}						
			}
			else
			{
				if (it.nSubIndex == nParentSubStepIndex)
					vcMeasure.emplace_back(std::make_pair(it.nStripX, it.nStripY));
			}
		}
// 		if (vcMeasure.size() <= 0)
// 			return RESULT_GOOD;
	}

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

	//
	enum Orient eTempOrient = rotMirrArr->FinalOrient();
	double dAngle_Temp;
	bool dMirror_Temp;
	CDrawFunction::GetAngleMirror(eTempOrient, dAngle_Temp, dMirror_Temp);
	
	double dFinal_Angle = dAngle_Temp + pSubStepRepeat->m_dAngle;
	bool dFinal_Mirror = dMirror_Temp == false ? pSubStepRepeat->m_bMirror : !pSubStepRepeat->m_bMirror;

	SUBSTEP_INFO stSubStepInfo(pSubStep->m_arrStepRepeat.m_strName, dFinal_Angle, dFinal_Mirror);
	int nBackBufferIndex = _GetBackBufferIndex(stSubStepInfo, m_vecDrawBackBuffer);
	if (nBackBufferIndex == -1) 
		return RESULT_BAD;

	ID2D1Bitmap* pBitmap = m_vecDrawBackBuffer[nBackBufferIndex].pBackbuffer->GetDrawBitmap();
	if (pBitmap == nullptr )
		return RESULT_BAD;

	for (int y_step = 0; y_step < static_cast<int>(pSubStepRepeat->m_nNY); y_step++)
	{
		for (int x_step = 0; x_step < static_cast<int>(pSubStepRepeat->m_nNX); x_step++)
		{
			if (pUserSetInfo->bShowStepRepeat == false)
			{
				if (nSubLevel > 0 && bMeasureUnitOnly)
				{
					auto itExist = std::find(vcMeasure.begin(), vcMeasure.end(), std::make_pair(x_step, y_step));
					if (itExist == vcMeasure.end())
						continue;
				}
			}

			dRefx = pSubStep->m_dXDatum;
			dRefy = pSubStep->m_dYDatum;

			rotMirrArr->ConvertPoint(&dParentX, &dParentY, 0, 0, dRefx, dRefy, eSubOrient);
			dParentX = (pSubStepRepeat->m_dX - dParentX) + pSubStepRepeat->m_dDX * x_step;
			dParentY = (pSubStepRepeat->m_dY - dParentY) + pSubStepRepeat->m_dDY * y_step;

			rotMirrArr->InputData(dParentX, dParentY, eSubOrient);	

			D2D1_RECT_F fptPicture = D2D1::RectF(DoubleToFloat(pStep->m_Profile.m_MinMax.left), DoubleToFloat(pStep->m_Profile.m_MinMax.top),
				DoubleToFloat(pStep->m_Profile.m_MinMax.right), DoubleToFloat(pStep->m_Profile.m_MinMax.bottom));

			//dParentX
			double dXs, dYs;
			double dXe, dYe;
			rotMirrArr->FinalPoint(&dXs, &dYs, fptPicture.left, fptPicture.top);
			rotMirrArr->FinalPoint(&dXe, &dYe, fptPicture.right, fptPicture.bottom);
			D2D1_RECT_F frtDst = D2D1::RectF(DoubleToFloat(dXs), DoubleToFloat(dYs), DoubleToFloat(dXe), DoubleToFloat(dYe));
			frtDst = CDrawFunction::NormalizeRect(frtDst);

			if (pBitmap != nullptr)
			{
				if (pUserSetInfo->bShowStepRepeat == true)
				{
					pRender->DrawBitmap(pBitmap, frtDst, 1, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
				}
				else if (pUserSetInfo->bShowPanelDummy == true )
				{
					pRender->DrawBitmap(pBitmap, frtDst, 1, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
				}
				else if (pUserSetInfo->bShowPanelDummy == false &&
					nSubLevel > 0)
				{
					pRender->DrawBitmap(pBitmap, frtDst, 1, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
				}		
			}

			//SubSub
			for (UINT j = 0; j < pSubStep->m_nNextStepCnt; j++)
			{
				CSubStep* pSubSubStep = pSubStep->m_arrNextStep.GetAt(j);
				if (pSubSubStep == nullptr) continue;				

				_Render_SubStep_V2(pRender, pSubSubStep, j, pLayerSet, pFontArr, rotMirrArr, bNeedUpdate, pUserSetInfo, bEditMode, ++nSubLevel, x_step, y_step, nSubIndex);
				nSubLevel--;
			}//Sub Unit End

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


BOOL CDrawLayer::_CheckBackBufferUpdate(IN const CStep *pStep)
{
	BOOL bUpdate = FALSE;

	int nStripCount = pStep->m_nSubStepCnt;

	int nStripCount_Buffer = static_cast<int>(m_vecSubStep_BackBuffer.size());
	if (nStripCount_Buffer != nStripCount) return TRUE;
	

	for (UINT i = 0; i < pStep->m_nSubStepCnt; i++)
	{
		CSubStep *pSubStep = pStep->m_arrSubStep.GetAt(i);
		SUBSTEP_BACKBUFFER *pSubSubStep_BackBuffer = &m_vecSubStep_BackBuffer[i];

		bUpdate = _CheckBackBufferCount(pSubStep, i, pSubSubStep_BackBuffer);
		if (bUpdate == TRUE) return bUpdate;
	}


	

	int nCount = static_cast<int>(m_vecSubStep_BackBuffer.size());
	for (int i = 0; i < nCount; i++)
	{
		bUpdate = _CheckBackBufferValid(&m_vecSubStep_BackBuffer[i]);
		if (bUpdate == TRUE) return bUpdate;
	}

	return FALSE;
}

BOOL CDrawLayer::_CheckBackBufferCount(CSubStep *pSubStep, IN const int &nSubIndex, SUBSTEP_BACKBUFFER *pSubStep_BackBuffer)
{
	if (pSubStep == nullptr) return TRUE;
	if (pSubStep_BackBuffer == nullptr) return TRUE;

	int nSubStepCount = pSubStep->m_nNextStepCnt;
	if (nSubStepCount != pSubStep_BackBuffer->vecSubStep.size())
	{
		return TRUE;
	}

	BOOL bRet = FALSE;
	for (int i = 0; i < nSubStepCount; i++)
	{
		CSubStep *pSubSubStep = pSubStep->m_arrNextStep.GetAt(i);
		SUBSTEP_BACKBUFFER *pSubSubStep_BackBuffer = &pSubStep_BackBuffer->vecSubStep[i];
		bRet = _CheckBackBufferCount(pSubSubStep, i, pSubSubStep_BackBuffer);
		if (bRet == TRUE)
		{//새로 만들어야 하는경우
			return bRet;
		}
	}

	return bRet;
}

BOOL CDrawLayer::_CheckBackBufferValid(SUBSTEP_BACKBUFFER *pSubStep_BackBuffer)
{
	if (pSubStep_BackBuffer == nullptr) return TRUE;


	if ( pSubStep_BackBuffer->pBackbuffer == nullptr ) return TRUE;
	if (pSubStep_BackBuffer->pBackbuffer->GetDrawBitmap() == nullptr) return TRUE;
	
	BOOL bRet = FALSE;
	int nCount = static_cast<int>(pSubStep_BackBuffer->vecSubStep.size());
	for (int i = 0; i < nCount; i++)
	{
		SUBSTEP_BACKBUFFER *pSubSubStep_BackBuffer = &pSubStep_BackBuffer->vecSubStep[i];
		bRet = _CheckBackBufferValid(pSubSubStep_BackBuffer);
		if (bRet == TRUE)
		{//새로 만들어야 하는경우
			return bRet;
		}
	}

	return bRet;
}

UINT32 CDrawLayer::_ClearBackBuffer()
{
	int nSize = static_cast<int>(m_vecSubStep_BackBuffer.size());

	for (int i = 0; i < nSize; i++)
	{
		m_vecSubStep_BackBuffer[i]._Reset();
	}
	m_vecSubStep_BackBuffer.clear();

	nSize = static_cast<int>(m_vecDrawBackBuffer.size());

	for (int i = 0; i < nSize; i++)
	{
		m_vecDrawBackBuffer[i]._Reset();
	}
	m_vecDrawBackBuffer.clear();

	
	return RESULT_GOOD;
}

UINT32 CDrawLayer::_MakeBackBuffer(IN const CStep *pStep)
{
	_ClearBackBuffer();

	int nStripCount = pStep->m_nSubStepCnt;
	m_vecSubStep_BackBuffer.resize(nStripCount);

	for (UINT i = 0; i < pStep->m_nSubStepCnt; i++)
	{
		CSubStep* pSubStep = pStep->m_arrSubStep[i];
		if (pSubStep == nullptr) continue;;

		if (m_vecSubStep_BackBuffer[i].SetBackBuffer(pSubStep->m_pSubStep->m_strStepName) == FALSE)
		{
			return RESULT_BAD;
		}

		if (pSubStep->m_nNextStepCnt > 0)
		{
			_MakeBackBuffer_SubStep(pSubStep, &m_vecSubStep_BackBuffer[i]);
		}

	}

	return RESULT_GOOD;
}

UINT32 CDrawLayer::_MakeBackBuffer_SubStep(IN const CSubStep *pSubStep, SUBSTEP_BACKBUFFER *pSubStep_BackBuffer)
{
	if (pSubStep == nullptr) return RESULT_BAD;

	for (UINT j = 0; j < pSubStep->m_nNextStepCnt; j++)
	{
		CSubStep* pSubSubStep = pSubStep->m_arrNextStep.GetAt(j);
		if (pSubStep_BackBuffer->AddSubStep(j, pSubSubStep->m_pSubStep->m_strStepName) == FALSE)
		{
			return RESULT_BAD;
		}

		int nCount = pSubSubStep->m_nNextStepCnt;
		if (nCount > 0)
		{
			_MakeBackBuffer_SubStep(pSubSubStep, &pSubStep_BackBuffer->vecSubStep[j]);
		}
	}

	return RESULT_GOOD;
}

UINT32 CDrawLayer::_DrawBackBuffer_SubStep(IN CSubStep *pSubStep, IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
	CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo, SUBSTEP_BACKBUFFER *pSubStep_BackBuffer)
{
	if (pSubStep == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (pUserSetInfo == nullptr) return RESULT_BAD;
	if (pSubStep_BackBuffer == nullptr) return RESULT_BAD;

	CRotMirrArr *rotMirrArr = NULL;
	BOOL bCreateRotMirr = FALSE;
	if (pRotMirrArr) rotMirrArr = pRotMirrArr;
	else
	{
		bCreateRotMirr = TRUE;
		rotMirrArr = new CRotMirrArr;
		rotMirrArr->InputData(0.0, 0.0, Orient::NoMir0Deg);

	}

	CStepRepeat* pSubStepRepeat = &(pSubStep->m_arrStepRepeat);
	if (pSubStepRepeat == nullptr)
		return RESULT_BAD;

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

	dRefx = pSubStep->m_dXDatum;
	dRefy = pSubStep->m_dYDatum;

	rotMirrArr->ConvertPoint(&dParentX, &dParentY, 0, 0, dRefx, dRefy, eSubOrient);

	dParentX = (pSubStepRepeat->m_dX - dParentX);
	dParentY = (pSubStepRepeat->m_dY - dParentY);

	rotMirrArr->InputData(dParentX, dParentY, eSubOrient);

	CStep *pStep = pSubStep->m_pSubStep;
	//if (pStep == nullptr) continue;
	if (pStep == nullptr) return RESULT_BAD;

	int nLayerIndex;//사용안함
	CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, pLayerSet->strLayer, nLayerIndex);
	//if (pLayer == nullptr) continue;
	if (pLayer == nullptr) return RESULT_BAD;

	D2D1_RECT_F fptPicture = D2D1::RectF(DoubleToFloat(pStep->m_Profile.m_MinMax.left), DoubleToFloat(pStep->m_Profile.m_MinMax.top),
		DoubleToFloat(pStep->m_Profile.m_MinMax.right), DoubleToFloat(pStep->m_Profile.m_MinMax.bottom));

	CRotMirrArr *pTempRotMirr = nullptr;
	pTempRotMirr = new CRotMirrArr();
	enum Orient eTempOrient = Orient::NoMir0Deg;
	eTempOrient = rotMirrArr->FinalOrient();

	pTempRotMirr->InputData(0, 0, eTempOrient);

	double dXs, dYs;
	double dXe, dYe;
	pTempRotMirr->FinalPoint(&dXs, &dYs, fptPicture.left, fptPicture.top);
	pTempRotMirr->FinalPoint(&dXe, &dYe, fptPicture.right, fptPicture.bottom);
	D2D1_RECT_F frtDst = D2D1::RectF(DoubleToFloat(dXs), DoubleToFloat(dYs), DoubleToFloat(dXe), DoubleToFloat(dYe));

	frtDst = CDrawFunction::NormalizeRect(frtDst);

	if (pSubStep_BackBuffer->pBackbuffer != nullptr)
	{
		pSubStep_BackBuffer->pBackbuffer->SetLink(m_pFactory, m_pD2DContext, m_pSystemSpec);
		pSubStep_BackBuffer->pBackbuffer->SetPictureBox(frtDst);

		//
		pSubStep_BackBuffer->pBackbuffer->OnRender(pLayer, pLayerSet, pFontArr, pTempRotMirr, pUserSetInfo);
	}
		

	delete pTempRotMirr;

	if (pSubStep->m_nNextStepCnt > 0)
	{
		for (UINT i = 0; i < pSubStep->m_nNextStepCnt; i++)
		{
			CSubStep* pSubSubStep = pSubStep->m_arrNextStep.GetAt(i);

// 			CStep *pStep = pSubSubStep->m_pSubStep;
// 			if (pStep == nullptr) continue;

// 			int nLayerIndex;//사용안함
// 			CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, pLayerSet->strLayer, nLayerIndex);
// 			if (pLayer == nullptr)
// 				continue;

			_DrawBackBuffer_SubStep(pSubSubStep, pLayerSet, pFontArr, rotMirrArr, pUserSetInfo, &pSubStep_BackBuffer->vecSubStep[i]);
		}
	}

	rotMirrArr->RemoveTail();

	if (bCreateRotMirr == TRUE)
	{
		rotMirrArr->ClearAll();
		delete rotMirrArr;
		rotMirrArr = nullptr;
	}

	return RESULT_GOOD;
}

UINT32 CDrawLayer::_DrawBackBuffer_SubStep_V2(IN CJobFile *pJobFile, IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
	IN const UserSetInfo_t *pUserSetInfo, DRAW_BACKBUFFER *pBackBuffer)
{
	if (pJobFile == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (pUserSetInfo == nullptr) return RESULT_BAD;
	if (pBackBuffer == nullptr) return RESULT_BAD;

	int nStepIndex;//사용안함
	CStep *pStep = CDrawFunction::_GetStepPrt(pJobFile->m_arrStep, pBackBuffer->stSubStepInfo.strStepName, nStepIndex);
	//if (pStep == nullptr) continue;
	if (pStep == nullptr) return RESULT_BAD;

	int nLayerIndex;//사용안함
	CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, pLayerSet->strLayer, nLayerIndex);
	//if (pLayer == nullptr) continue;
	if (pLayer == nullptr) return RESULT_BAD;

	D2D1_RECT_F fptPicture = D2D1::RectF(DoubleToFloat(pStep->m_Profile.m_MinMax.left), DoubleToFloat(pStep->m_Profile.m_MinMax.top),
		DoubleToFloat(pStep->m_Profile.m_MinMax.right), DoubleToFloat(pStep->m_Profile.m_MinMax.bottom));

	CRotMirrArr *pTempRotMirr = nullptr;
	pTempRotMirr = new CRotMirrArr();
	enum Orient eSubOrient = Orient::NoMir0Deg;
	if ((pBackBuffer->stSubStepInfo.dAngle == 0) && (pBackBuffer->stSubStepInfo.bMirror == false))			eSubOrient = Orient::NoMir0Deg;
	else if ((pBackBuffer->stSubStepInfo.dAngle == 90) && (pBackBuffer->stSubStepInfo.bMirror == false))		eSubOrient = Orient::NoMir90Deg;
	else if ((pBackBuffer->stSubStepInfo.dAngle == 180) && (pBackBuffer->stSubStepInfo.bMirror == false))		eSubOrient = Orient::NoMir180Deg;
	else if ((pBackBuffer->stSubStepInfo.dAngle == 270) && (pBackBuffer->stSubStepInfo.bMirror == false))		eSubOrient = Orient::NoMir270Deg;
	else if ((pBackBuffer->stSubStepInfo.dAngle == 0) && (pBackBuffer->stSubStepInfo.bMirror == true))		eSubOrient = Orient::Mir0Deg;
	else if ((pBackBuffer->stSubStepInfo.dAngle == 90) && (pBackBuffer->stSubStepInfo.bMirror == true))		eSubOrient = Orient::Mir90Deg;
	else if ((pBackBuffer->stSubStepInfo.dAngle == 180) && (pBackBuffer->stSubStepInfo.bMirror == true))		eSubOrient = Orient::Mir180Deg;
	else if ((pBackBuffer->stSubStepInfo.dAngle == 270) && (pBackBuffer->stSubStepInfo.bMirror == true))		eSubOrient = Orient::Mir270Deg;
	else eSubOrient = Orient::NoMir0Deg;

	pTempRotMirr->InputData(0, 0, eSubOrient);

	double dXs, dYs;
	double dXe, dYe;
	pTempRotMirr->FinalPoint(&dXs, &dYs, fptPicture.left, fptPicture.top);
	pTempRotMirr->FinalPoint(&dXe, &dYe, fptPicture.right, fptPicture.bottom);
	D2D1_RECT_F frtDst = D2D1::RectF(DoubleToFloat(dXs), DoubleToFloat(dYs), DoubleToFloat(dXe), DoubleToFloat(dYe));

	frtDst = CDrawFunction::NormalizeRect(frtDst);

	if (pBackBuffer->pBackbuffer != nullptr)
	{
		pBackBuffer->pBackbuffer->SetLink(m_pFactory, m_pD2DContext, m_pSystemSpec);
		pBackBuffer->pBackbuffer->SetPictureBox(frtDst);

		//
		pBackBuffer->pBackbuffer->OnRender(pLayer, pLayerSet, pFontArr, pTempRotMirr, pUserSetInfo);
	}

	delete pTempRotMirr;


	return RESULT_GOOD;

}

BOOL			CDrawLayer::_IsNeedMaskBackBuffer(IN const vector<SUBSTEP_INFO> &vecSubStepInfo)
{//vecBackBufferName 필요한 BackBuffer 이름
	BOOL bMake = FALSE;

	int nCount_Need = static_cast<int>(vecSubStepInfo.size());
	int nCount_BackBuffer = static_cast<int>(m_vecDrawBackBuffer.size());
	

	for (int i = 0; i < nCount_Need; i++)
	{
		BOOL bFind = FALSE;
		;
		for (int j = 0; j < nCount_BackBuffer; j++)
		{
			if (m_vecDrawBackBuffer[j].stSubStepInfo == vecSubStepInfo[i])
			{
				bFind = TRUE;
				break;
			}
		}
		if (bFind == FALSE)
		{
			bMake = TRUE;
			break;
		}
	}

	return bMake;
}

vector<SUBSTEP_INFO> CDrawLayer::_GetSubStepInfo(IN CStep *pStep)
{
	if (pStep == nullptr) return vector<SUBSTEP_INFO>();

	vector<SUBSTEP_INFO> vecSubStepInfo_All;
	
// 	int nStepCount = static_cast<int>(pJobFile->m_arrStep.GetCount());
// 	for (int i = 0; i < nStepCount; i++)
// 	{
// 		CStep *pStep = pJobFile->m_arrStep.GetAt(i);
// 		if ( pStep== nullptr ) continue;
// 
// 		vector<SUBSTEP_INFO> vecTemp;
// 		//개별 Step에서 사용하는 SubStep의 이름을 가져온다
// 		vecTemp = _GetSubStepName_Step(pStep);
// 
// 		for (auto Data : vecTemp)
// 		{
// 			vecSubStepInfo_All.push_back(Data);
// 		}
// 	}

	//현재 Step에서 사용하는 Sub스텝 정보
	vecSubStepInfo_All = _GetSubStepInfo_Step(pStep);

	vector<SUBSTEP_INFO> vecSubStepInfo;
	//리턴 버퍼 추가, 중복 된것은 넣지 않는다.
	int nCount_All = static_cast<int>(vecSubStepInfo_All.size());
	for (int i = 0; i < nCount_All; i++)
	{
		SUBSTEP_INFO stSubStepInfo = vecSubStepInfo_All[i];
		int nCount = static_cast<int>(vecSubStepInfo.size());

		if (nCount == 0)
		{
			vecSubStepInfo.push_back(stSubStepInfo);
		}
		else
		{
			//중복이 있는이 확인
			BOOL bFind = FALSE;
			for (int j = 0; j < nCount; j++)
			{
				if (vecSubStepInfo[j] == stSubStepInfo)
				{
					bFind = TRUE;
					break;
				}
			}

			//중복된것이 없다면 추가
			if (bFind == FALSE)
			{
				vecSubStepInfo.push_back(stSubStepInfo);
			}
		}
	}

	return vecSubStepInfo;
	
}

UINT32			CDrawLayer::_MakeBackBuffer_V2(IN const vector<SUBSTEP_INFO> &vecSubStepInfo)
{
	_ClearBackBuffer();

	int nCount = static_cast<int>(vecSubStepInfo.size());
	
	m_vecDrawBackBuffer.resize(nCount);
	
	for (int i = 0; i < nCount; i++)
	{
		m_vecDrawBackBuffer[i].SetBackBuffer(vecSubStepInfo[i]);
	}

	return RESULT_GOOD;
	
}

int				CDrawLayer::_GetBackBufferIndex(IN const SUBSTEP_INFO &stSubStepInfo, IN const vector<DRAW_BACKBUFFER> &vecBackBuffer)
{
	if (stSubStepInfo.strStepName == _T("")) return -1;

	int nIndex = -1;
	int nCount = static_cast<int>(vecBackBuffer.size());
	for (int i = 0; i < nCount; i++)
	{
		if (vecBackBuffer[i].stSubStepInfo == stSubStepInfo)
		{
			nIndex = i;
			break;
		}
	}

	return nIndex;
}

//
vector<SUBSTEP_INFO> CDrawLayer::_GetSubStepInfo_Step(IN CStep *pStep)
{
	if (pStep == nullptr) return vector<SUBSTEP_INFO>();

	vector<SUBSTEP_INFO> vecSubStepInfo;

	int nSubStepCount = static_cast<int>(pStep->m_arrSubStep.GetCount());
	for ( int i = 0 ; i < nSubStepCount ; i++ )
	{
		CSubStep *pSubStep = pStep->m_arrSubStep.GetAt(i);
		if (pSubStep == nullptr ) continue;

		vector<SUBSTEP_INFO> vecTemp;
		SUBSTEP_INFO stSubStepInfo;
		vecTemp = _GetSubStepInfo_SubStep(pSubStep, stSubStepInfo);

		for (auto Data : vecTemp)
		{
			vecSubStepInfo.push_back(Data);
		}
	}

	return vecSubStepInfo;
}

vector<SUBSTEP_INFO> CDrawLayer::_GetSubStepInfo_SubStep(IN CSubStep *pSubStep, IN SUBSTEP_INFO &in_stSubStepInfo)
{
	if (pSubStep == nullptr) return vector<SUBSTEP_INFO>();
	if ( pSubStep->m_pSubStep == nullptr ) return vector<SUBSTEP_INFO>();

	vector<SUBSTEP_INFO> vecSubStepInfo;
	SUBSTEP_INFO stSubStepInfo;
	stSubStepInfo.strStepName = pSubStep->m_arrStepRepeat.m_strName;
	stSubStepInfo.dAngle = in_stSubStepInfo.dAngle + pSubStep->m_arrStepRepeat.m_dAngle;
	stSubStepInfo.bMirror = pSubStep->m_arrStepRepeat.m_bMirror== false ? in_stSubStepInfo.bMirror : !in_stSubStepInfo.bMirror;


	int nSubSubCount = static_cast<int>(pSubStep->m_arrNextStep.GetCount());
	for (int i = 0; i < nSubSubCount; i++)
	{
		CSubStep *pSubSubStep = pSubStep->m_arrNextStep.GetAt(i);
		if (pSubSubStep == nullptr) continue;

		vector<SUBSTEP_INFO> vecTemp;
		vecTemp = _GetSubStepInfo_SubStep(pSubSubStep, stSubStepInfo);

		for (auto Data : vecTemp)
		{
			vecSubStepInfo.push_back(Data);
		}
	}

	vecSubStepInfo.push_back(stSubStepInfo);

	return vecSubStepInfo;
}