#include "../stdafx.h"
#include "CAOI.h"

#include <float.h>

CAOI::CAOI()
{
	
}

CAOI::~CAOI()
{

}

UINT32 CAOI::Destory()
{
	return RESULT_GOOD;
}

UINT32 CAOI::MakeMasterData()
{

	if (m_pJobData == nullptr || m_pUserLayerSetInfo == nullptr ||
		m_pUserSetInfo == nullptr || m_pSystemSpec == nullptr)
		return RESULT_BAD;

	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	if (m_pJobData->GetModelName().GetLength() < 1)
		return RESULT_BAD;

	m_strModelPath = GetModelPath(true);
	if (m_strModelPath.GetLength() < 1)
		return RESULT_BAD;

	if (RESULT_GOOD == OptimizeFOV(TRUE))
	{
		//Delete Folder
		CString strRemovePathName = m_strModelPath + _T("\\AlignMark");
		CUtils::RemoveDirectoryFile(strRemovePathName);	

		//Unit Image 历厘
		if (RESULT_GOOD != MakeMasterImage_Unit_in_Panel())
			return RESULT_BAD;

		//Align Image 历厘
 		if (RESULT_GOOD != MakePanelAlignPointImage())
 			return RESULT_BAD;

		//Unit Align : Master Image
		if (RESULT_GOOD != MakeUnitAlignPointImage())
			return RESULT_BAD;
			
		if (m_pSystemSpec->sysBasic.bExceptImageData == false)
		{//ExceptImageData == false 老版快父 InspImage 历厘

			//Master image 历厘
			if (RESULT_GOOD != MakeMasterImage())
				return RESULT_BAD;
		}
	}
	else
	{
		AfxMessageBox(L"Failed to Optimize FOV");
		return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 CAOI::OptimizeFOV(IN BOOL bSave, IN BOOL bUpdate)
{
	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	UINT32 iStepNum = static_cast<UINT32>(m_pJobData->m_arrStep.GetSize());
	if (iStepNum <= 0)
		return RESULT_BAD;

	m_stFovInfo.bChanged = false;
	m_stCellInfo.bChanged = false;

	enum Orient orient = Orient::NoMir0Deg;
	double dAngle = m_pUserSetInfo->dAngle;
	bool bMirror = m_pUserSetInfo->bMirror;

	if ((dAngle == 0.) && (bMirror == false))			orient = Orient::NoMir0Deg;
	else if ((dAngle == 90.) && (bMirror == false))		orient = Orient::NoMir90Deg;
	else if ((dAngle == 180.) && (bMirror == false))	orient = Orient::NoMir180Deg;
	else if ((dAngle == 270.) && (bMirror == false))	orient = Orient::NoMir270Deg;
	else if ((dAngle == 0.) && (bMirror == true))		orient = Orient::Mir0Deg;
	else if ((dAngle == 90.) && (bMirror == true))		orient = Orient::Mir90Deg;
	else if ((dAngle == 180.) && (bMirror == true))		orient = Orient::Mir180Deg;
	else if ((dAngle == 270.) && (bMirror == true))		orient = Orient::Mir270Deg;

	// Get Product Info
	if (bSave || bUpdate || (m_ePrevOrient != orient) ||
		(m_ePrevMirrorDir != m_pUserSetInfo->mirrorDir) ||
		(m_nPrevSelectStep != m_nSelectStep))
	{		
		m_ePrevOrient = orient;
		m_ePrevMirrorDir = m_pUserSetInfo->mirrorDir;
		m_nPrevSelectStep = m_nSelectStep;

		if (RESULT_GOOD != EstimateProductInfo(m_nSelectStep, orient))
			return RESULT_BAD;		

		// Estimate Swath Info
		if (m_pSystemSpec && m_pSystemSpec->sysBasic.bExceptDummy)
		{
			if (RESULT_GOOD != EstimateFOV_Based_On_Strip(m_nSelectStep, orient))
				return RESULT_BAD;

			//Estimate Cell Info
			if (RESULT_GOOD != EstimateCell_Based_On_Strip(m_nSelectStep, orient))
				return RESULT_BAD;
		}
		else
		{
			if (RESULT_GOOD != EstimateFOV_Based_On_Swath(m_nSelectStep, orient))
				return RESULT_BAD;

			//Estimate Cell Info
			if (RESULT_GOOD != EstimateCell_Based_On_Swath(m_nSelectStep, orient))
				return RESULT_BAD;
		}

		//Estimate Unit Info in Cell
		if (RESULT_GOOD != EstimateUnitInfo_in_Cell())
			return RESULT_BAD;

		//Estimate Unit Info in Cell
		if (RESULT_GOOD != EstimateStripInfo_in_Cell())
			return RESULT_BAD;

		//Estimate Align Info in Cell
		if (RESULT_GOOD != EstimateAlignInfo_in_Cell_V2())
			return RESULT_BAD;

		//Estimate Mask Info in Cell
		if (RESULT_GOOD != EstimateMaskInfo_in_Cell_V2())
			return RESULT_BAD;

		m_stFovInfo.bChanged = true;
		m_stCellInfo.bChanged = true;
	}
	else
	{		
		if (m_pUserSetInfo->bShowFOV)
			m_stFovInfo.bChanged = true;
		if (m_pUserSetInfo->bShowCell)
			m_stCellInfo.bChanged = true;
	}

	if (bSave)
	{
		if (RESULT_GOOD != SaveProductInfo(m_nSelectStep, orient))
			return RESULT_BAD;

		if (RESULT_GOOD != SaveGrabInfo(m_nSelectStep, orient))
			return RESULT_BAD;

		if (RESULT_GOOD != SaveCellInfo(m_nSelectStep, orient))
			return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 CAOI::EstimateProductInfo(IN INT32 iStepIdx, IN enum Orient eOrient)
{
	if (iStepIdx < 0)
		return RESULT_BAD;

	m_stPanelInfo.Clear();

	if (RESULT_GOOD != CheckIf_SubStripStep_Exist(iStepIdx))
		return RESULT_BAD;

	//1. Get Panel Info
	if (RESULT_GOOD != EstimatePanelInfo(iStepIdx, eOrient))
		return RESULT_BAD;

	//2. Get Strip Info
	if (RESULT_GOOD != EstimateStripInfo(iStepIdx, eOrient))
		return RESULT_BAD;

	//3. Get Unit Info
	if (RESULT_GOOD != EstimateUnitInfo(iStepIdx, eOrient))
		return RESULT_BAD;

	//4. Rearrage Index Info
//	if (RESULT_GOOD != RearrageIndexInfo(eOrient))
//		return RESULT_BAD;

	if (RESULT_GOOD != RearrageIndexInfo_Based_Strip(eOrient))
		return RESULT_BAD;

	//5. Get Block Rect 
	if (RESULT_GOOD != EstimateBlockInfo())
		return RESULT_BAD;

	//6. Get Align Mark Info (SR_NEW_MANUAL_EXPO)
	if (RESULT_GOOD != ExtractAlignMarkInfo(iStepIdx, eOrient))
		return RESULT_BAD;

	//7. Get Cross Point Info
	if (RESULT_GOOD != ExtractCrossPointInfo(iStepIdx, eOrient))
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CAOI::CheckIf_SubStripStep_Exist(IN INT32 iStepIdx)
{
	m_bIs4Step = FALSE;
	m_e4StepOrient = Orient::NoMir0Deg;

	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	if (iStepIdx != m_pUserLayerSetInfo->nPanelStepIdx)
		iStepIdx = m_pUserLayerSetInfo->nPanelStepIdx;

	CStep *pStep = m_pJobData->m_arrStep[iStepIdx];
	if (pStep == nullptr)
		return RESULT_BAD;

	UINT32 iSubStepNum = m_pJobData->m_arrStep[iStepIdx]->m_nSubStepCnt;

	for (UINT i = 0; i < iSubStepNum; i++)
	{
		CStepRepeat *pSubStepRepeat = &(m_pJobData->m_arrStep[iStepIdx]->m_arrSubStep[i]->m_arrStepRepeat);
		if (pSubStepRepeat == nullptr)
			continue;

		for (auto it : m_pUserLayerSetInfo->vcStripStepIdx)
		{
			CString strStripStepName = m_pJobData->m_arrStep[it]->m_strStepName;
			if (pSubStepRepeat->m_strName.CompareNoCase(strStripStepName) == 0)
				return RESULT_GOOD;
		}

		UINT32 iIdx = m_pJobData->m_arrStep[iStepIdx]->m_arrSubStep[i]->m_nStepID;
		UINT32 iStepNum = m_pJobData->m_arrStep[iIdx]->m_nSubStepCnt;
		if (iStepNum <= 0)
			continue;

		for (UINT j = 0; j < iStepNum; j++)
		{
			CStepRepeat *pSubSubStepRepeat = &(m_pJobData->m_arrStep[iIdx]->m_arrSubStep[j]->m_arrStepRepeat);
			if (pSubSubStepRepeat == nullptr)
				continue;

			for (auto it : m_pUserLayerSetInfo->vcStripStepIdx)
			{
				CString strStripStepName = m_pJobData->m_arrStep[it]->m_strStepName;
				if (pSubSubStepRepeat->m_strName.CompareNoCase(strStripStepName) == 0)
				{
					CDataManager::GetOrient(pSubStepRepeat->m_dAngle, pSubStepRepeat->m_bMirror, m_e4StepOrient);
					m_bIs4Step = TRUE;
					break;
				}
			}

			if (m_bIs4Step)
				break;
		}
		if (m_bIs4Step)
			break;
	}

	return RESULT_GOOD;
}

UINT32 CAOI::EstimatePanelInfo(IN INT32 iStepIdx, IN enum Orient eOrient)
{
	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	if (iStepIdx != m_pUserLayerSetInfo->nPanelStepIdx)
		iStepIdx = m_pUserLayerSetInfo->nPanelStepIdx;

	CStep *pStep = m_pJobData->m_arrStep[iStepIdx];
	if (pStep == nullptr)
		return RESULT_BAD;

	RECTD tmpMinMax, tmpRect;
	double extend = 0;

	CRotMirrArr RotMirrArr;
	RotMirrArr.InputData(0.0, 0.0, eOrient, m_pUserSetInfo->mirrorDir);

//	if (m_pSystemSpec->sysBasic.bProfileMode)
		tmpMinMax = pStep->m_Profile.m_MinMax;
//	else
//		tmpMinMax = pStep->m_FeatureMinMax;

	RotMirrArr.FinalPoint(&tmpRect.left, &tmpRect.top, tmpMinMax.left, tmpMinMax.top);
	RotMirrArr.FinalPoint(&tmpRect.right, &tmpRect.bottom, tmpMinMax.right, tmpMinMax.bottom);

	if (tmpRect.left > tmpRect.right) SWAP(tmpRect.left, tmpRect.right, extend);
	if (tmpRect.bottom > tmpRect.top) SWAP(tmpRect.bottom, tmpRect.top, extend);

	if (fabs(tmpRect.left) < EPSILON_DELTA1)	tmpRect.left = 0;
	if (fabs(tmpRect.right) < EPSILON_DELTA1)	tmpRect.right = 0;
	if (fabs(tmpRect.top) < EPSILON_DELTA1)		tmpRect.top = 0;
	if (fabs(tmpRect.bottom) < EPSILON_DELTA1)	tmpRect.bottom = 0;

	pStep->m_ActiveRect = tmpRect;

	return RESULT_GOOD;
}

UINT32 CAOI::EstimateStripInfo(IN INT32 iStepIdx, IN enum Orient eOrient)
{
	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	if (iStepIdx != m_pUserLayerSetInfo->nPanelStepIdx)
		iStepIdx = m_pUserLayerSetInfo->nPanelStepIdx;

	m_stPanelInfo.rcRect = m_pJobData->m_arrStep[iStepIdx]->m_ActiveRect;	

	UINT32 iTotalStripNum = 0;
	if (GetStripNum_In_Panel(iStepIdx, iTotalStripNum) != RESULT_GOOD)
		return RESULT_BAD;

	CameraSpec_t *pSpec = nullptr;
	if (m_pSystemSpec->sysBasic.ScanType == eScanType::eLineScan)
		pSpec = &m_pSystemSpec->sysCamera[true];
	else
		pSpec = &m_pSystemSpec->sysCamera[false];

	if (pSpec == nullptr) return RESULT_BAD;

	double dResolMM = pSpec->dResolution / 1000.f;

	UINT32 iStripShapeNo = 0, iStripRgnNum = 0, iPrevStripNum = 0;
	for (auto it : m_pUserLayerSetInfo->vcStripStepIdx)
	{
		CString strStripStepName = m_pJobData->m_arrStep[it]->m_strStepName;

		for (UINT32 i = 0; i < m_pJobData->m_arrStep[iStepIdx]->m_nSubStepCnt; i++)
		{
			if (RESULT_GOOD != CalculateStripCoord(TRUE, iStripShapeNo, i, eOrient, strStripStepName, nullptr, m_pJobData->m_arrStep[iStepIdx]->m_arrSubStep[i], &iStripRgnNum))
				return RESULT_BAD;

			//Strip Type
			if (m_stPanelInfo.vcStripInfo.size() != iPrevStripNum)
			{
				UINT32 iPrevCnt = static_cast<UINT32>(m_stPanelInfo.vcStripType.size());
				INT32 iRemain = static_cast<INT32>(m_stPanelInfo.vcStripInfo.size() - iPrevCnt);

				if (iRemain > 0)
				{
					bool bExistType = false;
					for (auto itType : m_stPanelInfo.vcStripType)
					{
						if (itType->strStepName.CompareNoCase(strStripStepName) == 0)
						{
							bExistType = true;
							if (m_pUserLayerSetInfo->vcStripStepIdx.size() > 1)
								itType->nNum += 1;
							else
								itType->nNum += (static_cast<UINT32>(m_stPanelInfo.vcStripInfo.size()) - iPrevStripNum);
							break;
						}
					}

					if (!bExistType)
					{
						TypeInfo_t *pStripType = new TypeInfo_t;
						pStripType->nNum = static_cast<UINT32>(m_stPanelInfo.vcStripInfo.size() - iPrevCnt);

						pStripType->dFeatureSize.x = (m_stPanelInfo.vcStripInfo[iPrevCnt]->rcRect.right - m_stPanelInfo.vcStripInfo[iPrevCnt]->rcRect.left);
						pStripType->dFeatureSize.y = (m_stPanelInfo.vcStripInfo[iPrevCnt]->rcRect.top - m_stPanelInfo.vcStripInfo[iPrevCnt]->rcRect.bottom);
						pStripType->dProfileSize.x = (m_stPanelInfo.vcStripInfo[iPrevCnt]->rcProfile.right - m_stPanelInfo.vcStripInfo[iPrevCnt]->rcProfile.left);
						pStripType->dProfileSize.y = (m_stPanelInfo.vcStripInfo[iPrevCnt]->rcProfile.top - m_stPanelInfo.vcStripInfo[iPrevCnt]->rcProfile.bottom);

						pStripType->szFeatureSize.cx = (LONG)((m_stPanelInfo.vcStripInfo[iPrevCnt]->rcRect.right - m_stPanelInfo.vcStripInfo[iPrevCnt]->rcRect.left) / dResolMM + 0.5);
						pStripType->szFeatureSize.cy = (LONG)((m_stPanelInfo.vcStripInfo[iPrevCnt]->rcRect.top - m_stPanelInfo.vcStripInfo[iPrevCnt]->rcRect.bottom) / dResolMM + 0.5);
						pStripType->szProfileSize.cx = (LONG)((m_stPanelInfo.vcStripInfo[iPrevCnt]->rcProfile.right - m_stPanelInfo.vcStripInfo[iPrevCnt]->rcProfile.left) / dResolMM + 0.5);
						pStripType->szProfileSize.cy = (LONG)((m_stPanelInfo.vcStripInfo[iPrevCnt]->rcProfile.top - m_stPanelInfo.vcStripInfo[iPrevCnt]->rcProfile.bottom) / dResolMM + 0.5);

						pStripType->strStepName = strStripStepName;
						pStripType->iOrient = m_stPanelInfo.vcStripInfo[iPrevCnt]->iOrient;
						m_stPanelInfo.vcStripType.emplace_back(pStripType);
					}

					iPrevStripNum = static_cast<UINT32>(m_stPanelInfo.vcStripInfo.size());
				}
			}
		}
		iStripShapeNo++;
	}	

	m_stPanelInfo.iStripTypeNum = iStripShapeNo;
	if (iStripRgnNum != m_stPanelInfo.vcStripInfo.size() ||
		m_stPanelInfo.vcStripInfo.size() <= 0)
		return RESULT_BAD;

	if (iStripShapeNo != m_stPanelInfo.vcStripType.size())
		return RESULT_BAD;

	// if Multi-Strip Step Case
	if (m_pUserLayerSetInfo->vcStripStepIdx.size() > 1)
	{
		for (auto itType : m_stPanelInfo.vcStripType)
		{
			UINT32 iNum = 0;
			for (auto itStrip : m_stPanelInfo.vcStripInfo)
			{
				if (itType->strStepName.CompareNoCase(itStrip->strStepName) == 0)
					iNum++;
			}
			itType->nNum = iNum;
		}
	}

	double dWidth	= fabs(m_stPanelInfo.vcStripInfo[0]->rcProfile.right - m_stPanelInfo.vcStripInfo[0]->rcProfile.left);
	double dHeight  = fabs(m_stPanelInfo.vcStripInfo[0]->rcProfile.top - m_stPanelInfo.vcStripInfo[0]->rcProfile.bottom);

	if (dWidth > dHeight)
		m_stPanelInfo.bIsStripVertical = false;
	else
		m_stPanelInfo.bIsStripVertical = true;

	return RESULT_GOOD;
}

UINT32 CAOI::GetStripNum_In_Panel(IN INT32 iStepIdx, OUT UINT32& iTotalStripNum)
{
	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	if (iStepIdx != m_pUserLayerSetInfo->nPanelStepIdx)
		iStepIdx = m_pUserLayerSetInfo->nPanelStepIdx;

	for (auto it : m_pUserLayerSetInfo->vcStripStepIdx)
	{
		CString strStripStepName = m_pJobData->m_arrStep[it]->m_strStepName;
		UINT32 iStripNum = 0, iStripSubNum = 0;

		for (UINT32 i = 0; i < m_pJobData->m_arrStep[iStepIdx]->m_nSubStepCnt; i++)
		{
			CStepRepeat *pSubStepRepeat = m_pJobData->m_arrStep[iStepIdx]->m_Stephdr.m_arrStepRepeat[i];
			if (pSubStepRepeat == nullptr)
				return FALSE;

			UINT32 iSubStepNum = pSubStepRepeat->m_nNY * pSubStepRepeat->m_nNX;
			
			if (m_bIs4Step)
			{
				iStripSubNum = 0;
				CalculateStripNumOnSubStep(TRUE, strStripStepName, m_pJobData->m_arrStep[iStepIdx]->m_arrSubStep[i], &iStripSubNum);

				if (iStripSubNum > 0)
					iStripNum += (iStripSubNum * iSubStepNum);
			}
			else
			{
				CalculateStripNumOnSubStep(TRUE, strStripStepName, m_pJobData->m_arrStep[iStepIdx]->m_arrSubStep[i], &iStripNum);

				if (pSubStepRepeat->m_strName.CompareNoCase(strStripStepName) != 0)
					continue;

				iStripNum += iSubStepNum;
			}
		}

		iTotalStripNum += iStripNum;
	}

	if (iTotalStripNum <= 0)
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CAOI::CalculateStripNumOnSubStep(IN BOOL bFirstStep, IN CString strStripStepName, IN CSubStep* pSubStep, OUT UINT32 *pStripNum)
{
	if (pSubStep == nullptr)
		return RESULT_BAD;

	CStepRepeat *pSubStepRepeat = &(pSubStep->m_arrStepRepeat);
	if (pSubStepRepeat == nullptr)
		return RESULT_BAD;

	if (pSubStep->m_nNextStepCnt == 0 || pSubStepRepeat->m_strName.CompareNoCase(strStripStepName) == 0)
	{
		if (!bFirstStep)
		{
			pSubStepRepeat = &(pSubStep->m_arrStepRepeat);
			if (pSubStepRepeat->m_strName.CompareNoCase(strStripStepName) == 0)
			{
				*pStripNum += pSubStepRepeat->m_nNY * pSubStepRepeat->m_nNX;
			}
		}
		return RESULT_GOOD;
	}

	for (UINT subno = 0; subno < pSubStep->m_nNextStepCnt; subno++)
		CalculateStripNumOnSubStep(FALSE, strStripStepName, pSubStep->m_arrNextStep[subno], pStripNum);

	return RESULT_GOOD;
}

UINT32 CAOI::CalculateStripCoord(IN BOOL bFirstStep, IN UINT iShapeIdx, IN INT32 iStepIdx, IN enum Orient eOrient, IN CString strStripStepName, IN CRotMirrArr *pRotMirrArr, IN CSubStep *pSubStep, OUT UINT32 *pRgnNum)
{
	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	if (pSubStep == nullptr)
		return RESULT_BAD;

	CStepRepeat *pSubStepRepeat = &(pSubStep->m_arrStepRepeat);
	if (pSubStepRepeat == nullptr)
		return RESULT_BAD;

	bool bCreatedRotMirr = false;
	CRotMirrArr *rotMirrArr = nullptr;
	if (pRotMirrArr) rotMirrArr = pRotMirrArr;
	else
	{
		rotMirrArr = new CRotMirrArr;
		rotMirrArr->InputData(0.0, 0.0, eOrient, m_pUserSetInfo->mirrorDir);
		bCreatedRotMirr = true;
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

	double		dRefx, dRefy, dTmpx, dTmpy;
	double		dParentX, dParentY;

	if (bFirstStep)
	{
		dRefx = pSubStep->m_dXDatum;
		dRefy = pSubStep->m_dYDatum;

		rotMirrArr->ConvertPoint(&dTmpx, &dTmpy, 0, 0, dRefx, dRefy, eSubOrient);
		dParentX = (pSubStepRepeat->m_dX - dTmpx);
		dParentY = (pSubStepRepeat->m_dY - dTmpy);

		rotMirrArr->InputData(0, 0, eSubOrient);
		rotMirrArr->FinalPoint(&dParentX, &dParentY, dParentX, dParentY);
		rotMirrArr->RemoveTail();
	}

	RECTD tmpMinMaxRect, tmpProfileRect;
	RECTD OrgMinMaxRect, OrgProfileRect;

	for (UINT32 y_step = 0; y_step < pSubStepRepeat->m_nNY; y_step++)
	{
		for (UINT32 x_step = 0; x_step < pSubStepRepeat->m_nNX; x_step++)
		{
			dRefx = pSubStep->m_dXDatum;
			dRefy = pSubStep->m_dYDatum;

			rotMirrArr->ConvertPoint(&dTmpx, &dTmpy, 0, 0, dRefx, dRefy, eSubOrient);
			dParentX = (pSubStepRepeat->m_dX - dTmpx) + pSubStepRepeat->m_dDX * x_step;
			dParentY = (pSubStepRepeat->m_dY - dTmpy) + pSubStepRepeat->m_dDY * y_step;

			rotMirrArr->InputData(dParentX, dParentY, eSubOrient);

			tmpProfileRect = m_pJobData->m_arrStep[pSubStep->m_nStepID]->m_Profile.m_MinMax;
			tmpMinMaxRect = m_pJobData->m_arrStep[pSubStep->m_nStepID]->m_FeatureMinMax;

			rotMirrArr->FinalPoint(&tmpProfileRect.left, &tmpProfileRect.top, tmpProfileRect.left, tmpProfileRect.top);
			rotMirrArr->FinalPoint(&tmpProfileRect.right, &tmpProfileRect.bottom, tmpProfileRect.right, tmpProfileRect.bottom);

			rotMirrArr->FinalPoint(&tmpMinMaxRect.left, &tmpMinMaxRect.top, tmpMinMaxRect.left, tmpMinMaxRect.top);
			rotMirrArr->FinalPoint(&tmpMinMaxRect.right, &tmpMinMaxRect.bottom, tmpMinMaxRect.right, tmpMinMaxRect.bottom);

			OrgProfileRect = tmpProfileRect;
			OrgMinMaxRect = tmpMinMaxRect;

			if (tmpProfileRect.left > tmpProfileRect.right)		SWAP(tmpProfileRect.left, tmpProfileRect.right, dTmpx);
			if (tmpProfileRect.bottom > tmpProfileRect.top)		SWAP(tmpProfileRect.bottom, tmpProfileRect.top, dTmpy);

			if (tmpMinMaxRect.left > tmpMinMaxRect.right)		SWAP(tmpMinMaxRect.left, tmpMinMaxRect.right, dTmpx);
			if (tmpMinMaxRect.bottom > tmpMinMaxRect.top)		SWAP(tmpMinMaxRect.bottom, tmpMinMaxRect.top, dTmpy);			

			if (m_bIs4Step && pSubStep->m_nNextStepCnt > 0)
			{
				for (UINT j = 0; j < pSubStep->m_nNextStepCnt; j++)
				{
					if (m_pJobData->m_arrStep[pSubStep->m_arrNextStep[j]->m_nStepID]->m_strStepName.CompareNoCase(strStripStepName) == 0)
						CalculateStripCoord(FALSE, iShapeIdx, iStepIdx, eOrient, strStripStepName, rotMirrArr, pSubStep->m_arrNextStep[j], pRgnNum);
				}
			}

			if (pSubStepRepeat->m_strName.CompareNoCase(strStripStepName) == 0)
			{
				StripInfo_t *pStrip = new StripInfo_t;
				pStrip->iType		= iShapeIdx;
				pStrip->strStepName = strStripStepName;
				pStrip->iStripIdx	= static_cast<UINT32>(m_stPanelInfo.vcStripInfo.size());//x_step * pSubStepRepeat->m_nNY + y_step;
				pStrip->rcRect		= tmpMinMaxRect;
				pStrip->rcProfile	= tmpProfileRect;

				if (m_pSystemSpec->sysBasic.bProfileMode)
					pStrip->rcRect_In_Panel = OrgProfileRect;
				else
					pStrip->rcRect_In_Panel = OrgMinMaxRect;

				pStrip->rcRect.left   = fabs(pStrip->rcRect.left - m_stPanelInfo.rcRect.left);
				pStrip->rcRect.right  = fabs(pStrip->rcRect.right - m_stPanelInfo.rcRect.left);
				pStrip->rcRect.top	  = fabs(pStrip->rcRect.top - m_stPanelInfo.rcRect.bottom);
				pStrip->rcRect.bottom = fabs(pStrip->rcRect.bottom - m_stPanelInfo.rcRect.bottom);

				if (pStrip->rcRect.right < pStrip->rcRect.left)	SWAP(pStrip->rcRect.left, pStrip->rcRect.right, dTmpx);
				if (pStrip->rcRect.top < pStrip->rcRect.bottom)	SWAP(pStrip->rcRect.top, pStrip->rcRect.bottom, dTmpy);

				pStrip->rcProfile.left   = fabs(pStrip->rcProfile.left - m_stPanelInfo.rcRect.left);
				pStrip->rcProfile.right  = fabs(pStrip->rcProfile.right - m_stPanelInfo.rcRect.left);
				pStrip->rcProfile.top	 = fabs(pStrip->rcProfile.top - m_stPanelInfo.rcRect.bottom);
				pStrip->rcProfile.bottom = fabs(pStrip->rcProfile.bottom - m_stPanelInfo.rcRect.bottom);

				if (pStrip->rcProfile.right < pStrip->rcProfile.left)	SWAP(pStrip->rcProfile.left, pStrip->rcProfile.right, dTmpx);
				if (pStrip->rcProfile.top < pStrip->rcProfile.bottom)	SWAP(pStrip->rcProfile.top, pStrip->rcProfile.bottom, dTmpy);

				enum Orient eStripOrient = AddOrient(eOrient, eSubOrient); 
				enum Orient eTotOrient = Orient::NoMir0Deg;
				CalculateOrient(eStripOrient, &eTotOrient);

				if (m_bIs4Step)				
					eTotOrient = AddOrient(eTotOrient, m_e4StepOrient);
					
				pStrip->iOrient = static_cast<UINT32>(eTotOrient);

				//SetStrip, Align/Mask
				SetRect(eOrient, strStripStepName, rotMirrArr, pSubStep, MaskType::enumType_Align, pStrip->vcAlign);
				SetRect(eOrient, strStripStepName, rotMirrArr, pSubStep, MaskType::enumType_Mask, pStrip->vcMask);
				SetRect_In_Panel(eOrient, strStripStepName, rotMirrArr, pSubStep, MaskType::enumType_Align, pStrip->vcAlign_In_Panel);
				SetRect_In_Panel(eOrient, strStripStepName, rotMirrArr, pSubStep, MaskType::enumType_Mask, pStrip->vcMask_In_Panel);

				m_stPanelInfo.vcStripInfo.emplace_back(pStrip);
				*pRgnNum += 1;
			}

			rotMirrArr->RemoveTail();
		}
	}

	if (bCreatedRotMirr)
	{
		rotMirrArr->ClearAll();
		delete rotMirrArr;
		rotMirrArr = nullptr;
	}

	if (bFirstStep)
	{
		if (m_stPanelInfo.vcStripInfo.size() > 0)
		{
			std::sort(m_stPanelInfo.vcStripInfo.begin(), m_stPanelInfo.vcStripInfo.end(), StripInfo_t::compY);

			UINT32 iLoop = 0;
			for (auto it : m_stPanelInfo.vcStripInfo)
				it->iStripIdx = iLoop++;

			UINT32 iRowStripNum = 0, iColStripNum = 0;
			double dLeftTop = m_stPanelInfo.vcStripInfo[0]->rcProfile.top;
			UINT32 iStripNumInPanel = static_cast<UINT32>(m_stPanelInfo.vcStripInfo.size());

			for (UINT32 iLoop = 1; iLoop < iStripNumInPanel; iLoop++)
			{
				if (fabs(dLeftTop - m_stPanelInfo.vcStripInfo[iLoop]->rcProfile.top) < EPSILON_DELTA2)
				{
					iRowStripNum = iLoop;
					iColStripNum = iStripNumInPanel / iLoop;
					break;
				}
			}

			if (iRowStripNum == 0)
			{
				iColStripNum = 1;
				iRowStripNum = iStripNumInPanel;
			}

			m_stPanelInfo.iColStripNumInPanel = iColStripNum;
			m_stPanelInfo.iRowStripNumInPanel = iRowStripNum;
		}
	}

	return RESULT_GOOD;
}

UINT32 CAOI::CalculateOrient(IN enum Orient curOrient, OUT enum Orient* pTotOrient)
{
	if (pTotOrient == nullptr)
		return RESULT_BAD;

	enum Orient eParentOrient = Orient::NoMir0Deg;
	UINT32 eTotOrient = static_cast<UINT32>(Orient::NoMir0Deg);

	eTotOrient = static_cast<UINT32>(AddOrient(eParentOrient, curOrient));
	UINT32 iMir0Deg = static_cast<UINT32>(Orient::Mir0Deg);
	UINT32 iMir270Deg = static_cast<UINT32>(Orient::Mir270Deg);

	if (curOrient <= Orient::NoMir270Deg)
	{
		if (eParentOrient <= Orient::NoMir270Deg)
		{
			if (eTotOrient >= iMir0Deg)
				eTotOrient -= iMir0Deg;
		}
		else //if (eParentOrient >  Orient::NoMir270Deg)
		{
			if (eTotOrient >= iMir270Deg + 1)
				eTotOrient -= iMir0Deg;
		}
	}
	else
	{
		if (eParentOrient <= Orient::NoMir270Deg)
		{
			if (eTotOrient >= iMir270Deg + 1)
				eTotOrient -= iMir0Deg;
		}
		else //if (eParentOrient >  Orient::NoMir270Deg)
		{
			if (eTotOrient >= static_cast<UINT32>(Orient::Mir270Deg) + 1)
			{
				eTotOrient -= (iMir270Deg + 1);
				if (eTotOrient >= iMir0Deg)
					eTotOrient -= iMir0Deg;
			}
		}
	}
	*pTotOrient = (Orient)eTotOrient;
	return RESULT_GOOD;
}

UINT32 CAOI::EstimateUnitInfo(IN INT32 iStepIdx, IN enum Orient eOrient)
{
	//[TBD] 矫埃乐阑 锭 埃帆窍霸 沥府窍磊....

	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	if (iStepIdx != m_pUserLayerSetInfo->nPanelStepIdx)
		iStepIdx = m_pUserLayerSetInfo->nPanelStepIdx;

	UINT32 iUnitShapeNo = 0, iUnitRgnNum = 0;

	for (auto itUnit : m_pUserLayerSetInfo->vcUnitStepIdx)
	{
		CString strUnitStepName = m_pJobData->m_arrStep[itUnit]->m_strStepName;

		for (UINT32 j = 0; j < m_pJobData->m_arrStep[iStepIdx]->m_nSubStepCnt; j++)
		{
			CalculateUnitCoord(TRUE, iUnitShapeNo, j, eOrient, strUnitStepName, nullptr,
				m_pJobData->m_arrStep[iStepIdx]->m_arrSubStep[j], &iUnitRgnNum);
		}

		iUnitShapeNo++;
	}
	
	bool bIsBlockOdd = false;
	UINT32 iPrevRowUnitNumInBlock = 0;
	for (auto itStrip : m_stPanelInfo.vcStripInfo)
	{
		for (auto itBlock : itStrip->vcBlockInfo)
		{
			if (iPrevRowUnitNumInBlock < itBlock->iRowUnitNumInBlock)
				iPrevRowUnitNumInBlock = itBlock->iRowUnitNumInBlock;
		}

		if (itStrip->vcBlockInfo.size() >= 3)	//Block is not properly calculated
		{
			bIsBlockOdd = true;
			break;
		}
	}

	bool bIsVert = false;
	UINT32 iRowUnitNum = 0, iColUnitNum = 0;
	if (m_pUserLayerSetInfo->vcUnitStepIdx.size() > 1 || bIsBlockOdd) // Multi-Unit Step Case
	{
		for (auto itStrip : m_stPanelInfo.vcStripInfo)
		{
			std::vector<UINT32> vcBlockIdx;
			std::vector<UnitInfo_t*> vcUnitInfo;
			for (auto itBlock : itStrip->vcBlockInfo)
			{
				for (auto itUnit : itBlock->vcUnitInfo)
				{
					UnitInfo_t *pUnitInfo = new UnitInfo_t;
					*pUnitInfo = *itUnit;
					vcUnitInfo.emplace_back(pUnitInfo);
				}
			}

			for (auto itBlock : itStrip->vcBlockInfo)
			{
				itBlock->Clear();
				delete itBlock;
				itBlock = nullptr;
			}
			itStrip->vcBlockInfo.clear();

			UINT32 iBlockNum = 1;
			std::sort(vcUnitInfo.begin(), vcUnitInfo.end(), UnitInfo_t::compY);
			if (vcUnitInfo.size() <= 0)
				return RESULT_BAD;

			double dLeftTop = vcUnitInfo[0]->rcProfile.top;
			UINT32 iUnitNumInStrip = static_cast<UINT32>(vcUnitInfo.size());

			for (UINT32 iLoop = 1; iLoop < iUnitNumInStrip; iLoop++)
			{
				if (fabs(dLeftTop - vcUnitInfo[iLoop]->rcProfile.top) < EPSILON_DELTA2)
				{
					iRowUnitNum = iLoop;
					iColUnitNum = iUnitNumInStrip / iLoop;
					break;
				}
			}

			if (iRowUnitNum == 0)
			{
				iColUnitNum = 1;
				iRowUnitNum = iUnitNumInStrip;
			}
			else
			{
				double dColPitch = 0.0;
				for (UINT32 iLoop = iRowUnitNum; iLoop < iUnitNumInStrip; iLoop += iRowUnitNum)
				{
					double dL1 = vcUnitInfo[iLoop - iRowUnitNum]->rcProfile.right;
					double dL2 = vcUnitInfo[iLoop]->rcProfile.left;
					dColPitch += fabs(dL2 - dL1);
				}

				dColPitch = (dColPitch / iColUnitNum);
				for (UINT32 iLoop = iRowUnitNum; iLoop < iUnitNumInStrip; iLoop += iRowUnitNum)
				{
					double dL1 = vcUnitInfo[iLoop - iRowUnitNum]->rcProfile.right;
					double dL2 = vcUnitInfo[iLoop]->rcProfile.left;
					if (fabs(dL2 - dL1) > dColPitch)
					{
						iBlockNum++;
						vcBlockIdx.emplace_back(iLoop);
					}
				}

				if (iBlockNum == iColUnitNum)
				{
					iBlockNum = 1;
					vcBlockIdx.clear();
					dColPitch = (dColPitch * iColUnitNum) / (iColUnitNum - 1) + 0.005;

					for (UINT32 iLoop = iRowUnitNum; iLoop < iUnitNumInStrip; iLoop += iRowUnitNum)
					{
						double dL1 = vcUnitInfo[iLoop - iRowUnitNum]->rcProfile.right;
						double dL2 = vcUnitInfo[iLoop]->rcProfile.left;
						if (fabs(dL2 - dL1) > dColPitch)
						{
							iBlockNum++;
							vcBlockIdx.emplace_back(iLoop);
						}
					}
				}
			}

			if (iColUnitNum == iBlockNum)
			{
				bIsVert = true;
				iBlockNum = 1;
				vcBlockIdx.clear();

				double dRowPitch = 0.0;
				for (UINT32 iLoop = 1; iLoop < iRowUnitNum; iLoop++)
				{
					double dL1 = vcUnitInfo[iLoop - 1]->rcProfile.bottom;
					double dL2 = vcUnitInfo[iLoop]->rcProfile.top;
					dRowPitch += fabs(dL2 - dL1);
				}

				dRowPitch /= iRowUnitNum;
				for (UINT32 iLoop = 1; iLoop < iRowUnitNum; iLoop++)
				{
					double dL1 = vcUnitInfo[iLoop - 1]->rcProfile.bottom;
					double dL2 = vcUnitInfo[iLoop]->rcProfile.top;
					if (fabs(dL2 - dL1) > dRowPitch)
					{
						iBlockNum++;
						vcBlockIdx.emplace_back(iLoop);
					}
				}
			}

			if (iBlockNum == 1)
			{
				UINT32 iLoop = 0;
				RECTD rcBlockRect = RECTD(LONG_MAX, LONG_MIN, LONG_MIN, LONG_MAX);
				RECTD rcBlockProfile = RECTD(LONG_MAX, LONG_MIN, LONG_MIN, LONG_MAX);
				for (auto it : vcUnitInfo)
				{
					it->iUnitIdx = iLoop++;
					it->iBlockIdx = 0;

					if (rcBlockRect.left > it->rcRect.left)				rcBlockRect.left = it->rcRect.left;
					if (rcBlockRect.right < it->rcRect.right)			rcBlockRect.right = it->rcRect.right;
					if (rcBlockRect.bottom > it->rcRect.bottom)			rcBlockRect.bottom = it->rcRect.bottom;
					if (rcBlockRect.top < it->rcRect.top)				rcBlockRect.top = it->rcRect.top;

					if (rcBlockProfile.left > it->rcProfile.left)		rcBlockProfile.left = it->rcProfile.left;
					if (rcBlockProfile.right < it->rcProfile.right)		rcBlockProfile.right = it->rcProfile.right;
					if (rcBlockProfile.bottom > it->rcProfile.bottom)	rcBlockProfile.bottom = it->rcProfile.bottom;
					if (rcBlockProfile.top < it->rcProfile.top)			rcBlockProfile.top = it->rcProfile.top;
				}

				if (vcUnitInfo.size() <= 0)
					return RESULT_BAD;

				UINT32 iRowUnitNum = 0, iColUnitNum = 0;
				double dLeftTop = vcUnitInfo[0]->rcProfile.top;
				UINT32 iUnitNumInStrip = static_cast<UINT32>(vcUnitInfo.size());

				for (UINT32 iLoop = 1; iLoop < iUnitNumInStrip; iLoop++)
				{
					if (fabs(dLeftTop - vcUnitInfo[iLoop]->rcProfile.top) < EPSILON_DELTA2)
					{
						iRowUnitNum = iLoop;
						iColUnitNum = iUnitNumInStrip / iLoop;
						break;
					}
				}

				if (iRowUnitNum == 0)
				{
					iColUnitNum = 1;
					iRowUnitNum = iUnitNumInStrip;
				}

				BlockInfo_t *pBlockInfo = new BlockInfo_t;
				pBlockInfo->iColUnitNumInBlock = iColUnitNum;
				pBlockInfo->iRowUnitNumInBlock = iRowUnitNum;
				pBlockInfo->iBlockIdx = 0;
				pBlockInfo->vcUnitInfo = vcUnitInfo;
				pBlockInfo->rcRect = rcBlockRect;
				pBlockInfo->rcProfile = rcBlockProfile;

				itStrip->vcBlockInfo.emplace_back(pBlockInfo);
			}
			else
			{
				if (bIsVert)
				{
					UINT32 iStart = 0, iEnd = 0;
					for (UINT32 iLoop = 0; iLoop < iBlockNum; iLoop++)
					{
						BlockInfo_t *pBlockInfo = new BlockInfo_t;

						if (iLoop == 0)
						{
							iStart = 0;
							iEnd = vcBlockIdx[iLoop];
						}
						else
						{
							iStart = vcBlockIdx[iLoop - 1];
							iEnd = iRowUnitNum;
						}

						UINT32 iLabel = 0;
						RECTD rcBlockRect = RECTD(LONG_MAX, LONG_MIN, LONG_MIN, LONG_MAX);
						RECTD rcBlockProfile = RECTD(LONG_MAX, LONG_MIN, LONG_MIN, LONG_MAX);
						for (UINT32 iCol = 0; iCol < iColUnitNum; iCol++)
						{
							for (UINT32 iRow = iStart; iRow < iEnd; iRow++)
							{
								UINT32 idx = iCol * iRowUnitNum + iRow;

								vcUnitInfo[idx]->iUnitIdx = iLabel++;
								vcUnitInfo[idx]->iBlockIdx = iLoop;
								pBlockInfo->vcUnitInfo.emplace_back(vcUnitInfo[idx]);

								if (rcBlockRect.left > vcUnitInfo[idx]->rcRect.left)			rcBlockRect.left = vcUnitInfo[idx]->rcRect.left;
								if (rcBlockRect.right < vcUnitInfo[idx]->rcRect.right)			rcBlockRect.right = vcUnitInfo[idx]->rcRect.right;
								if (rcBlockRect.bottom > vcUnitInfo[idx]->rcRect.bottom)		rcBlockRect.bottom = vcUnitInfo[idx]->rcRect.bottom;
								if (rcBlockRect.top < vcUnitInfo[idx]->rcRect.top)				rcBlockRect.top = vcUnitInfo[idx]->rcRect.top;

								if (rcBlockProfile.left > vcUnitInfo[idx]->rcProfile.left)		rcBlockProfile.left = vcUnitInfo[idx]->rcProfile.left;
								if (rcBlockProfile.right < vcUnitInfo[idx]->rcProfile.right)	rcBlockProfile.right = vcUnitInfo[idx]->rcProfile.right;
								if (rcBlockProfile.bottom > vcUnitInfo[idx]->rcProfile.bottom)	rcBlockProfile.bottom = vcUnitInfo[idx]->rcProfile.bottom;
								if (rcBlockProfile.top < vcUnitInfo[idx]->rcProfile.top)		rcBlockProfile.top = vcUnitInfo[idx]->rcProfile.top;
							}
						}

						if (pBlockInfo->vcUnitInfo.size() <= 0)
							return RESULT_BAD;

						UINT32 iRowUnitNumInBlock = 0, iColUnitNumInBlock = 0;
						double dLeftTop = pBlockInfo->vcUnitInfo[0]->rcProfile.top;
						UINT32 iUnitNumInStrip = static_cast<UINT32>(pBlockInfo->vcUnitInfo.size());

						for (UINT32 iLoop = 1; iLoop < iUnitNumInStrip; iLoop++)
						{
							if (fabs(dLeftTop - pBlockInfo->vcUnitInfo[iLoop]->rcProfile.top) < EPSILON_DELTA2)
							{
								iRowUnitNumInBlock = iLoop;
								iColUnitNumInBlock = iUnitNumInStrip / iLoop;
								break;
							}
						}

						if (iRowUnitNumInBlock == 0)
						{
							iColUnitNumInBlock = 1;
							iRowUnitNumInBlock = iUnitNumInStrip;
						}

						pBlockInfo->iColUnitNumInBlock = iColUnitNumInBlock;
						pBlockInfo->iRowUnitNumInBlock = iRowUnitNumInBlock;
						pBlockInfo->iBlockIdx = iLoop;
						pBlockInfo->rcRect = rcBlockRect;
						pBlockInfo->rcProfile = rcBlockProfile;

						itStrip->vcBlockInfo.emplace_back(pBlockInfo);
					}//for Block
				}
				else
				{
					UINT32 iStart = 0, iEnd = 0;
					for (UINT32 iLoop = 0; iLoop < iBlockNum; iLoop++)
					{
						BlockInfo_t *pBlockInfo = new BlockInfo_t;

						if (iLoop == 0)
						{
							iStart = 0;
							iEnd = vcBlockIdx[iLoop];
						}
						else
						{
							iStart = vcBlockIdx[iLoop - 1];
							iEnd = static_cast<UINT32>(vcUnitInfo.size());
						}

						UINT32 iLabel = 0;
						RECTD rcBlockRect = RECTD(LONG_MAX, LONG_MIN, LONG_MIN, LONG_MAX);
						RECTD rcBlockProfile = RECTD(LONG_MAX, LONG_MIN, LONG_MIN, LONG_MAX);
						for (UINT32 idx = iStart; idx < iEnd; idx++)
						{
							vcUnitInfo[idx]->iUnitIdx = iLabel++;
							vcUnitInfo[idx]->iBlockIdx = iLoop;
							pBlockInfo->vcUnitInfo.emplace_back(vcUnitInfo[idx]);

							if (rcBlockRect.left > vcUnitInfo[idx]->rcRect.left)			rcBlockRect.left = vcUnitInfo[idx]->rcRect.left;
							if (rcBlockRect.right < vcUnitInfo[idx]->rcRect.right)			rcBlockRect.right = vcUnitInfo[idx]->rcRect.right;
							if (rcBlockRect.bottom > vcUnitInfo[idx]->rcRect.bottom)		rcBlockRect.bottom = vcUnitInfo[idx]->rcRect.bottom;
							if (rcBlockRect.top < vcUnitInfo[idx]->rcRect.top)				rcBlockRect.top = vcUnitInfo[idx]->rcRect.top;

							if (rcBlockProfile.left > vcUnitInfo[idx]->rcProfile.left)		rcBlockProfile.left = vcUnitInfo[idx]->rcProfile.left;
							if (rcBlockProfile.right < vcUnitInfo[idx]->rcProfile.right)	rcBlockProfile.right = vcUnitInfo[idx]->rcProfile.right;
							if (rcBlockProfile.bottom > vcUnitInfo[idx]->rcProfile.bottom)	rcBlockProfile.bottom = vcUnitInfo[idx]->rcProfile.bottom;
							if (rcBlockProfile.top < vcUnitInfo[idx]->rcProfile.top)		rcBlockProfile.top = vcUnitInfo[idx]->rcProfile.top;
						}

						if (pBlockInfo->vcUnitInfo.size() <= 0)
							return RESULT_BAD;

						UINT32 iRowUnitNum = 0, iColUnitNum = 0;
						double dLeftTop = pBlockInfo->vcUnitInfo[0]->rcProfile.top;
						UINT32 iUnitNumInStrip = static_cast<UINT32>(pBlockInfo->vcUnitInfo.size());

						for (UINT32 iLoop = 1; iLoop < iUnitNumInStrip; iLoop++)
						{
							if (fabs(dLeftTop - pBlockInfo->vcUnitInfo[iLoop]->rcProfile.top) < EPSILON_DELTA2)
							{
								iRowUnitNum = iLoop;
								iColUnitNum = iUnitNumInStrip / iLoop;
								break;
							}
						}

						if (iRowUnitNum == 0)
						{
							iColUnitNum = 1;
							iRowUnitNum = iUnitNumInStrip;
						}

						pBlockInfo->iColUnitNumInBlock = iColUnitNum;
						pBlockInfo->iRowUnitNumInBlock = iRowUnitNum;
						pBlockInfo->iBlockIdx = iLoop;
						pBlockInfo->rcRect = rcBlockRect;
						pBlockInfo->rcProfile = rcBlockProfile;

						itStrip->vcBlockInfo.emplace_back(pBlockInfo);
					}//for Block
				}//bIsVert				
			}//Multi Block
		}//Strip Info
	}// Multi-Unit Step Case
	//////////////////////////////////////////////////////////////////////////////////
	if (iUnitShapeNo == 0 || iUnitShapeNo != m_stPanelInfo.vcUnitType.size())
		return RESULT_BAD;

	//Block Info
	RECTD rcStripRect = RECTD(LONG_MAX, LONG_MIN, LONG_MIN, LONG_MAX);
	RECTD rcStripProfile = RECTD(LONG_MAX, LONG_MIN, LONG_MIN, LONG_MAX);
	RECTD rcStripRect_In_Panel = RECTD(LONG_MAX, 0, 0, LONG_MAX);
	UINT32 iRowUnitNumInPanel = 0, iColUnitNumInPanel = 0;

	double extend = 0;
	for (auto itStrip : m_stPanelInfo.vcStripInfo)
	{
		UINT32 iBlockNumInStrip = static_cast<UINT32>(itStrip->vcBlockInfo.size());
		if (iBlockNumInStrip > 1)
		{
			std::sort(itStrip->vcBlockInfo.begin(), itStrip->vcBlockInfo.end(), BlockInfo_t::comp);

			UINT32 iLoop = 0;
			for (auto itBlock : itStrip->vcBlockInfo)
			{
				itBlock->iBlockIdx = iLoop;
				for (auto itUnit : itBlock->vcUnitInfo)
					itUnit->iBlockIdx = iLoop;

				iLoop++;
			}
		}

		UINT32 iRowBlockNum = 1, iColBlockNum = 1;
		double dLeftTopX = itStrip->vcBlockInfo[0]->rcProfile.left;
		double dLeftTopY = itStrip->vcBlockInfo[0]->rcProfile.top;
		for (UINT32 iLoop = 1; iLoop < iBlockNumInStrip; iLoop++)
		{
			if (fabs(dLeftTopX - itStrip->vcBlockInfo[iLoop]->rcProfile.left) < EPSILON_DELTA2)
				iRowBlockNum++;
			else if (fabs(dLeftTopY - itStrip->vcBlockInfo[iLoop]->rcProfile.top) < EPSILON_DELTA2)
				iColBlockNum++;
		}

		itStrip->iRowBlockNumInStrip = iRowBlockNum;
		itStrip->iColBlockNumInStrip = iColBlockNum;

		RECTD rcBlockRect = RECTD(LONG_MAX, LONG_MIN, LONG_MIN, LONG_MAX);
		RECTD rcBlockProfile = RECTD(LONG_MAX, LONG_MIN, LONG_MIN, LONG_MAX);
		UINT32 iRowUnitNumInStrip = 0, iColUnitNumInStrip = 0;
		for (auto itBlock : itStrip->vcBlockInfo)
		{
			iRowUnitNumInStrip += itBlock->iRowUnitNumInBlock;
			iColUnitNumInStrip += itBlock->iColUnitNumInBlock;

			if (rcBlockRect.left > itBlock->rcRect.left)			rcBlockRect.left = itBlock->rcRect.left;
			if (rcBlockRect.right < itBlock->rcRect.right)			rcBlockRect.right = itBlock->rcRect.right;
			if (rcBlockRect.bottom > itBlock->rcRect.bottom)		rcBlockRect.bottom = itBlock->rcRect.bottom;
			if (rcBlockRect.top < itBlock->rcRect.top)				rcBlockRect.top = itBlock->rcRect.top;

			if (rcBlockProfile.left > itBlock->rcProfile.left)		rcBlockProfile.left = itBlock->rcProfile.left;
			if (rcBlockProfile.right < itBlock->rcProfile.right)	rcBlockProfile.right = itBlock->rcProfile.right;
			if (rcBlockProfile.bottom > itBlock->rcProfile.bottom)	rcBlockProfile.bottom = itBlock->rcProfile.bottom;
			if (rcBlockProfile.top < itBlock->rcProfile.top)		rcBlockProfile.top = itBlock->rcProfile.top;
		}

		itStrip->rcRect_wo_Dummy = rcBlockRect;
		itStrip->rcProfile_wo_Dummy = rcBlockProfile;

		iRowUnitNumInStrip /= iColBlockNum;
		iColUnitNumInStrip /= iRowBlockNum;
		itStrip->iRowUnitNumInStrip = iRowUnitNumInStrip;
		itStrip->iColUnitNumInStrip = iColUnitNumInStrip;

		iRowUnitNumInPanel += iRowUnitNumInStrip;
		iColUnitNumInPanel += iColUnitNumInStrip;

		if (rcStripRect.left > itStrip->rcRect_wo_Dummy.left)			rcStripRect.left = itStrip->rcRect_wo_Dummy.left;
		if (rcStripRect.right < itStrip->rcRect_wo_Dummy.right)			rcStripRect.right = itStrip->rcRect_wo_Dummy.right;
		if (rcStripRect.bottom > itStrip->rcRect_wo_Dummy.bottom)		rcStripRect.bottom = itStrip->rcRect_wo_Dummy.bottom;
		if (rcStripRect.top < itStrip->rcRect_wo_Dummy.top)				rcStripRect.top = itStrip->rcRect_wo_Dummy.top;

		if (rcStripProfile.left > itStrip->rcProfile_wo_Dummy.left)		rcStripProfile.left = itStrip->rcProfile_wo_Dummy.left;
		if (rcStripProfile.right < itStrip->rcProfile_wo_Dummy.right)	rcStripProfile.right = itStrip->rcProfile_wo_Dummy.right;
		if (rcStripProfile.bottom > itStrip->rcProfile_wo_Dummy.bottom)	rcStripProfile.bottom = itStrip->rcProfile_wo_Dummy.bottom;
		if (rcStripProfile.top < itStrip->rcProfile_wo_Dummy.top)		rcStripProfile.top = itStrip->rcProfile_wo_Dummy.top;

		RECTD rcStrip_In_Panel = itStrip->rcRect_In_Panel;
		if (fabs(rcStrip_In_Panel.left) > fabs(rcStrip_In_Panel.right)) SWAP(rcStrip_In_Panel.left, rcStrip_In_Panel.right, extend);
		if (fabs(rcStrip_In_Panel.bottom) > fabs(rcStrip_In_Panel.top)) SWAP(rcStrip_In_Panel.bottom, rcStrip_In_Panel.top, extend);

		if (fabs(rcStripRect_In_Panel.left) > fabs(rcStrip_In_Panel.left))		rcStripRect_In_Panel.left = rcStrip_In_Panel.left;
		if (fabs(rcStripRect_In_Panel.right) < fabs(rcStrip_In_Panel.right))	rcStripRect_In_Panel.right = rcStrip_In_Panel.right;
		if (fabs(rcStripRect_In_Panel.bottom) > fabs(rcStrip_In_Panel.bottom))	rcStripRect_In_Panel.bottom = rcStrip_In_Panel.bottom;
		if (fabs(rcStripRect_In_Panel.top) < fabs(rcStrip_In_Panel.top))		rcStripRect_In_Panel.top = rcStrip_In_Panel.top;
	}

	if (rcStripRect_In_Panel.left > rcStripRect_In_Panel.right) SWAP(rcStripRect_In_Panel.left, rcStripRect_In_Panel.right, extend);
	if (rcStripRect_In_Panel.bottom > rcStripRect_In_Panel.top) SWAP(rcStripRect_In_Panel.bottom, rcStripRect_In_Panel.top, extend);

	//Panel Info
	iRowUnitNumInPanel /= m_stPanelInfo.iColStripNumInPanel;
	iColUnitNumInPanel /= m_stPanelInfo.iRowStripNumInPanel;

	m_stPanelInfo.iRowUnitNumInPanel = iRowUnitNumInPanel;
	m_stPanelInfo.iColUnitNumInPanel = iColUnitNumInPanel;
	m_stPanelInfo.rcRect_wo_Dummy = rcStripRect;
	m_stPanelInfo.rcProfile_wo_Dummy = rcStripProfile;
	m_stPanelInfo.rcRect_In_Panel = rcStripRect_In_Panel;

	return RESULT_GOOD;
}

UINT32 CAOI::CalculateUnitCoord(IN BOOL bFirstStep, IN UINT iShapeIdx, IN INT32 iStepIdx, IN enum Orient eOrient, IN CString strUnitStepName, IN CRotMirrArr *pRotMirrArr, IN CSubStep *pSubStep, OUT UINT32 *pRgnNum)
{
	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	if (pSubStep == nullptr || pRgnNum == nullptr)
		return RESULT_BAD;

	CStepRepeat *pSubStepRepeat = &(pSubStep->m_arrStepRepeat);
	if (pSubStepRepeat == nullptr)
		return RESULT_BAD;

	CameraSpec_t *pSpec = nullptr;
	if (m_pSystemSpec->sysBasic.ScanType == eScanType::eLineScan)
		pSpec = &m_pSystemSpec->sysCamera[true];
	else
		pSpec = &m_pSystemSpec->sysCamera[false];

	if (pSpec == nullptr) return RESULT_BAD;

	double dResolMM = pSpec->dResolution / 1000.f;

	bool bCreatedRotMirr = false;
	CRotMirrArr *rotMirrArr = nullptr;
	if (pRotMirrArr) rotMirrArr = pRotMirrArr;
	else
	{
		rotMirrArr = new CRotMirrArr;
		rotMirrArr->InputData(0.0, 0.0, eOrient, m_pUserSetInfo->mirrorDir);
		bCreatedRotMirr = true;
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

	double		dRefx, dRefy, dTmpx, dTmpy;
	double		dParentX, dParentY;

	if (bFirstStep)
	{
		dRefx = pSubStep->m_dXDatum;
		dRefy = pSubStep->m_dYDatum;

		rotMirrArr->ConvertPoint(&dTmpx, &dTmpy, 0, 0, dRefx, dRefy, eSubOrient);
		dParentX = (pSubStepRepeat->m_dX - dTmpx);
		dParentY = (pSubStepRepeat->m_dY - dTmpy);

		rotMirrArr->InputData(0, 0, eSubOrient);
		rotMirrArr->FinalPoint(&dParentX, &dParentY, dParentX, dParentY);
		rotMirrArr->RemoveTail();
	}

	RECTD tmpMinMaxRect, tmpProfileRect;
	RECTD OrgMinMaxRect, OrgProfileRect;
	std::vector<UnitInfo_t*> vcUnitInfo;

	for (UINT32 y_step = 0; y_step < pSubStepRepeat->m_nNY; y_step++)
	{
		for (UINT32 x_step = 0; x_step < pSubStepRepeat->m_nNX; x_step++)
		{
			dRefx = pSubStep->m_dXDatum;
			dRefy = pSubStep->m_dYDatum;

			rotMirrArr->ConvertPoint(&dTmpx, &dTmpy, 0, 0, dRefx, dRefy, eSubOrient);
			dParentX = (pSubStepRepeat->m_dX - dTmpx) + pSubStepRepeat->m_dDX * x_step;
			dParentY = (pSubStepRepeat->m_dY - dTmpy) + pSubStepRepeat->m_dDY * y_step;

			rotMirrArr->InputData(dParentX, dParentY, eSubOrient);

			tmpProfileRect = m_pJobData->m_arrStep[pSubStep->m_nStepID]->m_Profile.m_MinMax;
			tmpMinMaxRect = m_pJobData->m_arrStep[pSubStep->m_nStepID]->m_FeatureMinMax;

			rotMirrArr->FinalPoint(&tmpProfileRect.left, &tmpProfileRect.top, tmpProfileRect.left, tmpProfileRect.top);
			rotMirrArr->FinalPoint(&tmpProfileRect.right, &tmpProfileRect.bottom, tmpProfileRect.right, tmpProfileRect.bottom);

			rotMirrArr->FinalPoint(&tmpMinMaxRect.left, &tmpMinMaxRect.top, tmpMinMaxRect.left, tmpMinMaxRect.top);
			rotMirrArr->FinalPoint(&tmpMinMaxRect.right, &tmpMinMaxRect.bottom, tmpMinMaxRect.right, tmpMinMaxRect.bottom);

			OrgProfileRect = tmpProfileRect;
			OrgMinMaxRect = tmpMinMaxRect;

			if (tmpProfileRect.left > tmpProfileRect.right)		SWAP(tmpProfileRect.left, tmpProfileRect.right, dTmpx);
			if (tmpProfileRect.bottom > tmpProfileRect.top)		SWAP(tmpProfileRect.bottom, tmpProfileRect.top, dTmpy);

			if (tmpMinMaxRect.left > tmpMinMaxRect.right)		SWAP(tmpMinMaxRect.left, tmpMinMaxRect.right, dTmpx);
			if (tmpMinMaxRect.bottom > tmpMinMaxRect.top)		SWAP(tmpMinMaxRect.bottom, tmpMinMaxRect.top, dTmpy);

			if (RESULT_GOOD != GetStripIndex(bFirstStep, pSubStepRepeat->m_strName, strUnitStepName, tmpProfileRect, iStepIdx))
				return RESULT_BAD;

			if (pSubStep->m_nNextStepCnt > 0)
			{
				enum Orient eUnitOrient = AddOrient(eOrient, eSubOrient);
				enum Orient eTotOrient = Orient::NoMir0Deg;
				CalculateOrient(eUnitOrient, &eTotOrient);

				for (UINT j = 0; j < pSubStep->m_nNextStepCnt; j++)
				{
					CalculateUnitCoord(FALSE, iShapeIdx, iStepIdx, eTotOrient, strUnitStepName, rotMirrArr, pSubStep->m_arrNextStep[j], pRgnNum);
				}
			}
		
			if (pSubStepRepeat->m_strName.CompareNoCase(strUnitStepName) == 0)
			{
				UnitInfo_t *pUnit	= new UnitInfo_t;
				pUnit->iType		= iShapeIdx;
				pUnit->iStripIdx	= iStepIdx;
				pUnit->iUnitIdx		= *pRgnNum;
				pUnit->rcRect		= tmpMinMaxRect;
				pUnit->rcProfile	= tmpProfileRect;
				pUnit->rcProfile_In_Panel = tmpProfileRect;

				if (m_pSystemSpec->sysBasic.bProfileMode)
					pUnit->rcRect_In_Panel = OrgProfileRect;
				else
					pUnit->rcRect_In_Panel = OrgMinMaxRect;

				pUnit->rcRect.left	 = fabs(pUnit->rcRect.left - m_stPanelInfo.rcRect.left);
				pUnit->rcRect.right  = fabs(pUnit->rcRect.right - m_stPanelInfo.rcRect.left);
				pUnit->rcRect.top	 = fabs(pUnit->rcRect.top - m_stPanelInfo.rcRect.bottom);
				pUnit->rcRect.bottom = fabs(pUnit->rcRect.bottom - m_stPanelInfo.rcRect.bottom);

				if (pUnit->rcRect.right < pUnit->rcRect.left)	SWAP(pUnit->rcRect.left, pUnit->rcRect.right, dTmpx);
				if (pUnit->rcRect.top < pUnit->rcRect.bottom)	SWAP(pUnit->rcRect.top, pUnit->rcRect.bottom, dTmpy);

				pUnit->rcProfile.left	= fabs(pUnit->rcProfile.left - m_stPanelInfo.rcRect.left);
				pUnit->rcProfile.right	= fabs(pUnit->rcProfile.right - m_stPanelInfo.rcRect.left);
				pUnit->rcProfile.top	= fabs(pUnit->rcProfile.top - m_stPanelInfo.rcRect.bottom);
				pUnit->rcProfile.bottom = fabs(pUnit->rcProfile.bottom - m_stPanelInfo.rcRect.bottom);

				if (pUnit->rcProfile.right < pUnit->rcProfile.left)	SWAP(pUnit->rcProfile.left, pUnit->rcProfile.right, dTmpx);
				if (pUnit->rcProfile.top < pUnit->rcProfile.bottom)	SWAP(pUnit->rcProfile.top, pUnit->rcProfile.bottom, dTmpy);

				enum Orient eUnitOrient = AddOrient(eOrient, eSubOrient);
				enum Orient eTotOrient = Orient::NoMir0Deg;
				CalculateOrient(eUnitOrient, &eTotOrient);

				pUnit->iOrient = static_cast<UINT32>(eTotOrient);

				//SetUnit, Align/Mask
				SetRect(eOrient, strUnitStepName, /*nullptr*/pRotMirrArr, pSubStep, MaskType::enumType_Align, pUnit->vcAlign);
				SetRect(eOrient, strUnitStepName, /*nullptr*/pRotMirrArr, pSubStep, MaskType::enumType_Mask, pUnit->vcMask);
				SetRect_In_Panel(eOrient, strUnitStepName, pRotMirrArr, pSubStep, MaskType::enumType_Align, pUnit->vcAlign_In_Panel);
				SetRect_In_Panel(eOrient, strUnitStepName, pRotMirrArr, pSubStep, MaskType::enumType_Mask, pUnit->vcMask_In_Panel);

				vcUnitInfo.emplace_back(pUnit);

				//Unit Type
				UINT32 iUnitTypeNum = static_cast<UINT32>(m_stPanelInfo.vcUnitType.size());
				if (iUnitTypeNum == iShapeIdx + 1)
				{
					m_stPanelInfo.vcUnitType[iShapeIdx]->nNum++;
				}
				else if (iUnitTypeNum < iShapeIdx + 1)
				{
					bool bExistType = false;
					for (auto itType : m_stPanelInfo.vcUnitType)
					{
						if (itType->strStepName.CompareNoCase(strUnitStepName) == 0)
						{
							bExistType = true;
							break;
						}
					}

					if (!bExistType)
					{
						TypeInfo_t *pUnitType = new TypeInfo_t;
						pUnitType->nNum = 1;
						pUnitType->dFeatureSize.x = (pUnit->rcRect.right - pUnit->rcRect.left);
						pUnitType->dFeatureSize.y = (pUnit->rcRect.top - pUnit->rcRect.bottom);
						pUnitType->dProfileSize.x = (pUnit->rcProfile.right - pUnit->rcProfile.left);
						pUnitType->dProfileSize.y = (pUnit->rcProfile.top - pUnit->rcProfile.bottom);
					
						pUnitType->szFeatureSize.cx = (LONG)((pUnit->rcRect.right - pUnit->rcRect.left) / dResolMM + 0.5);
						pUnitType->szFeatureSize.cy = (LONG)((pUnit->rcRect.top - pUnit->rcRect.bottom) / dResolMM + 0.5);
						pUnitType->szProfileSize.cx = (LONG)((pUnit->rcProfile.right - pUnit->rcProfile.left) / dResolMM + 0.5);
						pUnitType->szProfileSize.cy = (LONG)((pUnit->rcProfile.top - pUnit->rcProfile.bottom) / dResolMM + 0.5);

						pUnitType->iOrient = pUnit->iOrient;
						pUnitType->strStepName = strUnitStepName;

						m_stPanelInfo.vcUnitType.emplace_back(pUnitType);
					}
				}

				*pRgnNum += 1;
			}

			rotMirrArr->RemoveTail();
		}
	}

	if (bCreatedRotMirr)
	{
		rotMirrArr->ClearAll();
		delete rotMirrArr;
		rotMirrArr = nullptr;
	}

	if (!bFirstStep)
	{
		if (m_bIs4Step && pSubStepRepeat->m_strName.CompareNoCase(strUnitStepName) != 0)
			return RESULT_GOOD;

		if (iStepIdx >= m_stPanelInfo.vcStripInfo.size())
			return RESULT_BAD;

		std::sort(vcUnitInfo.begin(), vcUnitInfo.end(), UnitInfo_t::compY);
		UINT32 iBlockIdx = static_cast<UINT32>(m_stPanelInfo.vcStripInfo[iStepIdx]->vcBlockInfo.size());

		UINT32 iLoop = 0;
		RECTD rcBlockRect = RECTD(LONG_MAX, LONG_MIN, LONG_MIN, LONG_MAX);
		RECTD rcBlockProfile = RECTD(LONG_MAX, LONG_MIN, LONG_MIN, LONG_MAX);

		for (auto it : vcUnitInfo)
		{
			it->iUnitIdx = iLoop++;
			it->iBlockIdx = iBlockIdx;

			if (rcBlockRect.left > it->rcRect.left)				rcBlockRect.left	= it->rcRect.left;
			if (rcBlockRect.right < it->rcRect.right)			rcBlockRect.right   = it->rcRect.right;
			if (rcBlockRect.bottom > it->rcRect.bottom)			rcBlockRect.bottom  = it->rcRect.bottom;
			if (rcBlockRect.top < it->rcRect.top)				rcBlockRect.top		= it->rcRect.top;

			if (rcBlockProfile.left > it->rcProfile.left)		rcBlockProfile.left   = it->rcProfile.left;
			if (rcBlockProfile.right < it->rcProfile.right)		rcBlockProfile.right  = it->rcProfile.right;
			if (rcBlockProfile.bottom > it->rcProfile.bottom)	rcBlockProfile.bottom = it->rcProfile.bottom;
			if (rcBlockProfile.top < it->rcProfile.top)			rcBlockProfile.top	  = it->rcProfile.top;
		}

		if (vcUnitInfo.size() <= 0)
			return RESULT_BAD;

		UINT32 iRowUnitNum = 0, iColUnitNum = 0;
		double dLeftTop = vcUnitInfo[0]->rcProfile.top;
		UINT32 iUnitNumInStrip = static_cast<UINT32>(vcUnitInfo.size());

		for (UINT32 iLoop = 1; iLoop < iUnitNumInStrip; iLoop++)
		{
			if (fabs(dLeftTop - vcUnitInfo[iLoop]->rcProfile.top) < EPSILON_DELTA2)
			{
				iRowUnitNum = iLoop;
				iColUnitNum = iUnitNumInStrip / iLoop;
				break;
			}
		}

		if (iRowUnitNum == 0)
		{
			iColUnitNum = 1;
			iRowUnitNum = iUnitNumInStrip;
		}

		m_stPanelInfo.vcStripInfo[iStepIdx]->iRowUnitNumInStrip = iRowUnitNum;
		m_stPanelInfo.vcStripInfo[iStepIdx]->iColUnitNumInStrip = iColUnitNum;

		if (m_stPanelInfo.vcStripInfo[iStepIdx]->vcUnitInfo.size() <= 0)
			m_stPanelInfo.vcStripInfo[iStepIdx]->vcUnitInfo = vcUnitInfo;
		else
		{
			std::vector<UnitInfo_t*> vcTempUnitInfo;
			for (auto itTemp : m_stPanelInfo.vcStripInfo[iStepIdx]->vcUnitInfo)
			{
				vcTempUnitInfo.emplace_back(itTemp);
			}
			for (auto itTemp : vcUnitInfo)
			{
				vcTempUnitInfo.emplace_back(itTemp);
			}
			m_stPanelInfo.vcStripInfo[iStepIdx]->vcUnitInfo = vcTempUnitInfo;
		}
		
		BlockInfo_t *pBlockInfo = new BlockInfo_t;
		pBlockInfo->iColUnitNumInBlock = iColUnitNum;
		pBlockInfo->iRowUnitNumInBlock = iRowUnitNum;
		pBlockInfo->iBlockIdx = iBlockIdx;
		pBlockInfo->vcUnitInfo = vcUnitInfo;
		pBlockInfo->rcRect = rcBlockRect;
		pBlockInfo->rcProfile = rcBlockProfile;

		m_stPanelInfo.vcStripInfo[iStepIdx]->vcBlockInfo.emplace_back(pBlockInfo);
	}

	return RESULT_GOOD;
}

UINT32 CAOI::GetStripIndex(IN BOOL bFirstStep, IN CString strStepName, IN CString strUnitStepName, IN RECTD rcStepRect, OUT IN INT32 &iStripIdx)
{
	if (m_bIs4Step)
	{
		if (bFirstStep)
			return RESULT_GOOD;

		if (strStepName.CompareNoCase(strUnitStepName) == 0)
			return RESULT_GOOD;
	}
	else
	{
		if (!bFirstStep)
			return RESULT_GOOD;
	}

	iStripIdx = 0;

	RECTD rcProfile  = rcStepRect;
	rcProfile.left	 = fabs(rcProfile.left - m_stPanelInfo.rcRect.left);
	rcProfile.right  = fabs(rcProfile.right - m_stPanelInfo.rcRect.left);
	rcProfile.top	 = fabs(rcProfile.top - m_stPanelInfo.rcRect.bottom);
	rcProfile.bottom = fabs(rcProfile.bottom - m_stPanelInfo.rcRect.bottom);

	double dTmpx, dTmpy;
	if (rcProfile.right < rcProfile.left)	SWAP(rcProfile.left, rcProfile.right, dTmpx);
	if (rcProfile.top < rcProfile.bottom)	SWAP(rcProfile.top, rcProfile.bottom, dTmpy);

	for (auto it : m_stPanelInfo.vcStripInfo)
	{
		if (fabs(it->rcProfile.left - rcProfile.left) < EPSILON_DELTA2 &&
			fabs(it->rcProfile.top - rcProfile.top) < EPSILON_DELTA2)
		{
			break;
		}
		iStripIdx++;
	}

	return RESULT_GOOD;
}

UINT32 CAOI::RearrageIndexInfo(IN enum Orient eOrient)
{
	if (m_pUserSetInfo == nullptr)
		return RESULT_BAD;

	CString strLog;
	int index = 0;

	for (auto it : m_stPanelInfo.vcStripInfo)
	{
		strLog.Format(_T("%d, %.3f, %.3f, %.3f, %.3f\n"), index++, it->rcProfile.left, it->rcProfile.top, it->rcProfile.right, it->rcProfile.bottom);
		OutputDebugString(strLog);
	}
	
	if (m_pUserSetInfo->mirrorDir == eDirectionType::eBoth)
		std::sort(m_stPanelInfo.vcStripInfo.begin(), m_stPanelInfo.vcStripInfo.end(), StripInfo_t::compX);
	else if (m_pUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone)
		std::sort(m_stPanelInfo.vcStripInfo.begin(), m_stPanelInfo.vcStripInfo.end(), StripInfo_t::comp_reverseY);
	else if (m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal)
		std::sort(m_stPanelInfo.vcStripInfo.begin(), m_stPanelInfo.vcStripInfo.end(), StripInfo_t::comp_reverseX);

	index = 0;
	for (auto it : m_stPanelInfo.vcStripInfo)
	{
		strLog.Format(_T("%d, %.3f, %.3f, %.3f, %.3f\n"), index++, it->rcProfile.left, it->rcProfile.top, it->rcProfile.right, it->rcProfile.bottom);
		OutputDebugString(strLog);
	}

	UINT32 iStripIdx = 0;
	for (auto itStrip : m_stPanelInfo.vcStripInfo)
	{
		UINT32 iStripStartIdx = iStripIdx * (itStrip->iColUnitNumInStrip * itStrip->iRowUnitNumInStrip);
		itStrip->iStripIdx = iStripIdx++;
		
		index = 0;
		for (auto it : itStrip->vcBlockInfo)
		{
			strLog.Format(_T("%d, %.3f, %.3f, %.3f, %.3f\n"), index++, it->rcProfile.left, it->rcProfile.top, it->rcProfile.right, it->rcProfile.bottom);
			OutputDebugString(strLog);
		}

		if (m_stPanelInfo.bIsStripVertical && m_pUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone)
			std::sort(itStrip->vcBlockInfo.begin(), itStrip->vcBlockInfo.end(), BlockInfo_t::comp_reverseY);			
		else if (m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal)
			std::sort(itStrip->vcBlockInfo.begin(), itStrip->vcBlockInfo.end(), BlockInfo_t::comp_reverseX);
		else if (!m_stPanelInfo.bIsStripVertical && m_pUserSetInfo->mirrorDir == eDirectionType::eBoth)
			std::sort(itStrip->vcBlockInfo.begin(), itStrip->vcBlockInfo.end(), BlockInfo_t::comp_reverseX);

		index = 0;
		for (auto it : itStrip->vcBlockInfo)
		{
			strLog.Format(_T("%d, %.3f, %.3f, %.3f, %.3f\n"), index++, it->rcProfile.left, it->rcProfile.top, it->rcProfile.right, it->rcProfile.bottom);
			OutputDebugString(strLog);
		}

		UINT32 iBlockIdx = 0;					
		for (auto itBlock : itStrip->vcBlockInfo)
		{
			UINT32 iBlockStartIdx = iBlockIdx * itBlock->iColUnitNumInBlock * itBlock->iRowUnitNumInBlock;
			itBlock->iBlockIdx = iBlockIdx++;

			index = 0;
			for (auto it : itBlock->vcUnitInfo)
			{
				strLog.Format(_T("%d, %.3f, %.3f, %.3f, %.3f\n"), index++, it->rcProfile.left, it->rcProfile.top, it->rcProfile.right, it->rcProfile.bottom);
				OutputDebugString(strLog);
			}

			if (m_pUserSetInfo->mirrorDir == eDirectionType::eBoth)
				std::sort(itBlock->vcUnitInfo.begin(), itBlock->vcUnitInfo.end(), UnitInfo_t::compX);
			else if (m_pUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone)
				std::sort(itBlock->vcUnitInfo.begin(), itBlock->vcUnitInfo.end(), UnitInfo_t::comp_reverseY);
			else if (m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal)
				std::sort(itBlock->vcUnitInfo.begin(), itBlock->vcUnitInfo.end(), UnitInfo_t::comp_reverseX);

			index = 0;
			for (auto it : itBlock->vcUnitInfo)
			{
				strLog.Format(_T("%d, %.3f, %.3f, %.3f, %.3f\n"), index++, it->rcProfile.left, it->rcProfile.top, it->rcProfile.right, it->rcProfile.bottom);
				OutputDebugString(strLog);
			}

			bool bPrevState = false;
			UINT32 iUnitIdx = 0, iTotUnitIdx = 0;
			for (auto itUnit : itBlock->vcUnitInfo)
			{
				itUnit->iStripIdx = itStrip->iStripIdx;
				itUnit->iBlockIdx = itBlock->iBlockIdx;

				UINT32 iColIdx = iTotUnitIdx / itBlock->iRowUnitNumInBlock;
				bool bIsEven = iColIdx % 2 == 0 ? true : false;

				if (bIsEven != bPrevState)
					iUnitIdx = 0;

				if (bIsEven)	// Sequential Order
				{
					itUnit->iUnitIdx_Ori = iStripStartIdx + iBlockStartIdx + itBlock->iRowUnitNumInBlock * iColIdx + iUnitIdx;
					bPrevState = bIsEven;
				}
				else           // Reverse Order
				{
					itUnit->iUnitIdx_Ori = iStripStartIdx + iBlockStartIdx + itBlock->iRowUnitNumInBlock * (iColIdx + 1) - 1 - iUnitIdx;
					bPrevState = bIsEven;
				}

				strLog.Format(L"%d,%d,%d,%.3f,%.3f,%.3f,%.3f\n", itUnit->iUnitIdx_Ori, itUnit->iStripIdx, itUnit->iBlockIdx, itUnit->rcProfile.left, itUnit->rcProfile.top, itUnit->rcProfile.right, itUnit->rcProfile.bottom);
				OutputDebugString(strLog);

				iTotUnitIdx++;
				iUnitIdx++;
			}
			if (m_pUserSetInfo->mirrorDir == eDirectionType::eBoth ||
				m_pUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone ||
				m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal)
				std::sort(itBlock->vcUnitInfo.begin(), itBlock->vcUnitInfo.end(), UnitInfo_t::compY);
		}		

		if (m_stPanelInfo.bIsStripVertical && m_pUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone)
			std::sort(itStrip->vcBlockInfo.begin(), itStrip->vcBlockInfo.end(), BlockInfo_t::comp);		
		else if (m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal)
			std::sort(itStrip->vcBlockInfo.begin(), itStrip->vcBlockInfo.end(), BlockInfo_t::comp);
		else if (!m_stPanelInfo.bIsStripVertical && m_pUserSetInfo->mirrorDir == eDirectionType::eBoth)
			std::sort(itStrip->vcBlockInfo.begin(), itStrip->vcBlockInfo.end(), BlockInfo_t::comp);
	} //No Mirror

	if (m_pUserSetInfo->mirrorDir == eDirectionType::eBoth ||
		m_pUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone ||
		m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal)
		std::sort(m_stPanelInfo.vcStripInfo.begin(), m_stPanelInfo.vcStripInfo.end(), StripInfo_t::compY);

	return RESULT_GOOD;
}

UINT32 CAOI::SaveProductInfo(IN INT32 iStepIdx, IN enum Orient eOrient)
{
	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	if (m_pJobData->GetModelName().GetLength() < 1 ||
		m_strModelPath.GetLength() < 1)
		return RESULT_BAD;

	if (iStepIdx != m_pUserLayerSetInfo->nPanelStepIdx)
		iStepIdx = m_pUserLayerSetInfo->nPanelStepIdx;

	if (iStepIdx < 0 || m_pSystemSpec->sysPath.strMasterPath.GetLength() < 1)
		return RESULT_BAD;

	CStep *pStep = m_pJobData->m_arrStep[iStepIdx];
	if (pStep == nullptr)
		return RESULT_BAD;

	CRotMirrArr RotMirrArr;
	RECTD tmpMinMax, tmpRect;
	tmpMinMax = pStep->m_Profile.m_MinMax;

	//0档 Reference
	RotMirrArr.InputData(0.0, 0.0, Orient::NoMir0Deg);
	RotMirrArr.FinalPoint(&tmpRect.left, &tmpRect.top, tmpMinMax.left, tmpMinMax.top);
	RotMirrArr.FinalPoint(&tmpRect.right, &tmpRect.bottom, tmpMinMax.right, tmpMinMax.bottom);
	RotMirrArr.ClearAll();

	if (fabs(tmpRect.left) < EPSILON_DELTA1)	tmpRect.left = 0;
	if (fabs(tmpRect.right) < EPSILON_DELTA1)	tmpRect.right = 0;
	if (fabs(tmpRect.top) < EPSILON_DELTA1)		tmpRect.top = 0;
	if (fabs(tmpRect.bottom) < EPSILON_DELTA1)	tmpRect.bottom = 0;

	if (eOrient == Orient::NoMir0Deg)
		tmpRect.SetRectEmpty();

	CameraSpec_t *pSpec = nullptr;
	if (m_pSystemSpec->sysBasic.ScanType == eScanType::eLineScan)
		pSpec = &m_pSystemSpec->sysCamera[true];
	else
		pSpec = &m_pSystemSpec->sysCamera[false];

	if (pSpec == nullptr) return RESULT_BAD;

	CString strWorkStep = m_pJobData->m_arrStep[iStepIdx]->m_strStepName;
	CString strWorkLayer = m_pUserSetInfo->strWorkLayer;

	CString strFileName = L"";
	strFileName.Format(L"%s\\%s", m_strModelPath, PRODUCT_INFO_FILE);

	CFile clResultFile;
	if (!clResultFile.Open(strFileName, CFile::modeCreate | CFile::modeReadWrite))
		return RESULT_BAD;

	CHAR *pcFiledata = nullptr;
	CString strValue = L"";

	strValue.Format(_T("<?xml version=\"1.0\" encoding=\"euc-kr\" standalone=\"yes\"?>\r\n\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	strValue.Format(_T("<data>\r\n\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	CTime TM = CTime::GetCurrentTime();
	strValue.Format(_T("<note date=\"%04d/%02d/%02d\" time=\"%02d:%02d:%02d\"></note>\r\n\r\n"),
		TM.GetYear(), TM.GetMonth(), TM.GetDay(), TM.GetHour(), TM.GetMinute(), TM.GetSecond());
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	//[INFO]
	strValue.Format(_T("<info model=\"%s\" resolution=\"%.4f\" rotation=\"%.1f\" mirror=\"%d\" workstep=\"%s\" worklayer=\"%s\" except_dummy=\"%d\" profile_mode=\"%d\" unit=\"%s\"></info>\r\n"),
		m_pJobData->GetModelName(), pSpec->dResolution / 1000.0, m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror,
		strWorkStep, strWorkLayer, m_pSystemSpec->sysBasic.bExceptDummy, m_pSystemSpec->sysBasic.bProfileMode, _T("mm"));

	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	//[LAYER]
	UINT32 iLayerNum = 0;
	for (auto it : m_pUserLayerSetInfo->vcLayerSet)
	{
		if (!it->bCheck || it->strDefine.GetLength() < 1)
			continue;

		iLayerNum++;
	}

	strValue.Format(_T("<layer count=\"%d\">\r\n"), iLayerNum);
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	UINT32 iIdx = 1;
	for (auto it : m_pUserLayerSetInfo->vcLayerSet)
	{
		if (!it->bCheck || it->strDefine.GetLength() < 1)
			continue;

		strValue.Format(_T("<l%d define=\"%s\" name=\"%s\" polarity=\"%d\" scale_x=\"%.1f\" scale_y=\"%.1f\"></l%d>\r\n"),
			iIdx, it->strDefine, it->strLayer, it->bPolarity, it->dScaleXY.x, it->dScaleXY.y, iIdx);
		pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
		clResultFile.Write(pcFiledata, strValue.GetLength());
		free(pcFiledata);
		iIdx++;
	}

	strValue.Format(_T("</layer>\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);


	//Print LineWidth Info
	INT32 iPTWidthCnt = 0;
	int nLineOptionCount = static_cast<int>(m_vecvecWidthSort.size());
	for (int nOption = 0; nOption < nLineOptionCount; nOption++)
	{
		iPTWidthCnt = static_cast<int>(m_vecvecWidthSort[nOption].size());

		strValue.Format(_T("<%s count=\"%d\">\r\n"), eLineWidthOption::pOptionName[nOption], iPTWidthCnt);
		pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
		clResultFile.Write(pcFiledata, strValue.GetLength());
		free(pcFiledata);

		iIdx = 1;
		for (auto stLineWidth : m_vecvecWidthSort[nOption])
		{
			strValue.Format(_T("<p%d index=\"%d\" step=\"%d\" width_um=\"%.3f\"></p%d>\r\n"),
				iIdx, iIdx - 1, stLineWidth.nStep, CJobFile::MilToMM(_ttof(stLineWidth.strLineWidth)) * 1000., iIdx);
			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);
			iIdx++;
		}

		strValue.Format(_T("</%s>\r\n"), eLineWidthOption::pOptionName[nOption]);
		pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
		clResultFile.Write(pcFiledata, strValue.GetLength());
		free(pcFiledata);
	}
	///[End]Print LineWidth Info

	//[STRIP_TYPE]
	strValue.Format(_T("<strip_type count=\"%d\">\r\n"), m_stPanelInfo.vcStripType.size());
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	iIdx = 1;
	for (auto it : m_stPanelInfo.vcStripType)
	{
		strValue.Format(_T("<s%d count=\"%d\" profile_width=\"%.3f\" profile_height=\"%.3f\" feature_width=\"%.3f\" feature_height=\"%.3f\">\r\n"),
			iIdx, it->nNum, it->dProfileSize.x, it->dProfileSize.y, it->dFeatureSize.x, it->dFeatureSize.y);
		pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
		clResultFile.Write(pcFiledata, strValue.GetLength());
		free(pcFiledata);

		//[AlignMark]
		StripInfo_t *pStrip = nullptr;
		for (auto itStrip : m_stPanelInfo.vcStripInfo)
		{
			if (iIdx == itStrip->iType + 1)
			{
				pStrip = itStrip;
				break;
			}
		}

		if (pStrip)
		{
			strValue.Format(_T("<alignmark count=\"%d\">\r\n"), static_cast<int>(pStrip->vcAlign.size()));
			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);

			UINT32 iMarkIdx = 1;
			for (auto itAlign : pStrip->vcAlign)
			{
				strValue.Format(_T("<m%d feature_count=\"%d\">\r\n"), iMarkIdx, itAlign.size());

				pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
				clResultFile.Write(pcFiledata, strValue.GetLength());
				free(pcFiledata);

				UINT32 iFeatureIdx = 1;
				for (auto itFeature : itAlign)
				{
					strValue.Format(_T("<f%d left=\"%.6f\" top=\"%.6f\" right=\"%.6f\" bottom=\"%.6f\" centerx=\"%.6f\" centery=\"%.6f\"></f%d>\r\n"),
						iFeatureIdx, itFeature.left, itFeature.top, itFeature.right, itFeature.bottom,
						(itFeature.left + itFeature.right) / 2.0, (itFeature.top + itFeature.bottom) / 2.0, iFeatureIdx);

					pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
					clResultFile.Write(pcFiledata, strValue.GetLength());
					free(pcFiledata);		
					iFeatureIdx++;
				}

				strValue.Format(_T("</m%d>\r\n"), iMarkIdx);
				pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
				clResultFile.Write(pcFiledata, strValue.GetLength());
				free(pcFiledata);
				iMarkIdx++;
			}

			strValue.Format(_T("</alignmark>\r\n"));
			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);
		}

		strValue.Format(_T("</s%d>\r\n"), iIdx);
		pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
		clResultFile.Write(pcFiledata, strValue.GetLength());
		free(pcFiledata);
		iIdx++;
	}

	strValue.Format(_T("</strip_type>\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	//[UNIT_TYPE]
	strValue.Format(_T("<unit_type count=\"%d\">\r\n"), m_stPanelInfo.vcUnitType.size());
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	iIdx = 1;
	for (auto it : m_stPanelInfo.vcUnitType)
	{
		strValue.Format(_T("<u%d count=\"%d\" profile_width=\"%.3f\" profile_height=\"%.3f\" feature_width=\"%.3f\" feature_height=\"%.3f\">\r\n"),
			iIdx, it->nNum, it->dProfileSize.x, it->dProfileSize.y, it->dFeatureSize.x, it->dFeatureSize.y);
		pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
		clResultFile.Write(pcFiledata, strValue.GetLength());
		free(pcFiledata);

		//[AlignMark]
		UnitInfo_t *pUnit = nullptr;
		for (auto itStrip : m_stPanelInfo.vcStripInfo)
		{
			for (auto itBlock : itStrip->vcBlockInfo)
			{
				for (auto itUnit : itBlock->vcUnitInfo)
				{
					if (iIdx == itUnit->iType + 1)
					{
						pUnit = itUnit;
						break;
					}
				}
				if (pUnit)
					break;
			}			
			if (pUnit)
				break;
		}

		if (pUnit)
		{
			strValue.Format(_T("<alignmark count=\"%d\">\r\n"), static_cast<int>(pUnit->vcAlign.size()));
			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);

			UINT32 iMarkIdx = 1;
			for (auto itAlign : pUnit->vcAlign)
			{
				strValue.Format(_T("<m%d feature_count=\"%d\">\r\n"), iMarkIdx, itAlign.size());

				pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
				clResultFile.Write(pcFiledata, strValue.GetLength());
				free(pcFiledata);

				UINT32 iFeatureIdx = 1;
				for (auto itFeature : itAlign)
				{
					strValue.Format(_T("<f%d left=\"%.6f\" top=\"%.6f\" right=\"%.6f\" bottom=\"%.6f\" centerx=\"%.6f\" centery=\"%.6f\"></f%d>\r\n"),
						iFeatureIdx, itFeature.left, itFeature.top, itFeature.right, itFeature.bottom,
						(itFeature.left + itFeature.right) / 2.0, (itFeature.top + itFeature.bottom) / 2.0, iFeatureIdx);

					pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
					clResultFile.Write(pcFiledata, strValue.GetLength());
					free(pcFiledata);
					iFeatureIdx++;
				}

				strValue.Format(_T("</m%d>\r\n"), iMarkIdx);
				pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
				clResultFile.Write(pcFiledata, strValue.GetLength());
				free(pcFiledata);
				iMarkIdx++;
			}

			strValue.Format(_T("</alignmark>\r\n"));
			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);
		}

		strValue.Format(_T("</u%d>\r\n"), iIdx);
		pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
		clResultFile.Write(pcFiledata, strValue.GetLength());
		free(pcFiledata);
		iIdx++;
	}

	strValue.Format(_T("</unit_type>\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	//[PANEL]
	double dTmp = 0;
	RECTD rcPanel = m_stPanelInfo.rcRect;

	rcPanel.left = fabs(rcPanel.left - m_stPanelInfo.rcRect.left);
	rcPanel.right = fabs(rcPanel.right - m_stPanelInfo.rcRect.left);
	rcPanel.top = fabs(rcPanel.top - m_stPanelInfo.rcRect.top);
	rcPanel.bottom = fabs(rcPanel.bottom - m_stPanelInfo.rcRect.top);	

	if (rcPanel.right < rcPanel.left)	SWAP(rcPanel.left, rcPanel.right, dTmp);
	if (rcPanel.top < rcPanel.bottom)	SWAP(rcPanel.top, rcPanel.bottom, dTmp);

	strValue.Format(_T("<panel left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\" width=\"%.3f\" height=\"%.3f\" thickness=\"%.3f\" strip_row_count=\"%d\" strip_col_count=\"%d\" unit_row_count=\"%d\" unit_col_count=\"%d\">\r\n"),
		rcPanel.left, rcPanel.top, rcPanel.right, rcPanel.bottom,
		fabs(m_stPanelInfo.rcRect.right - m_stPanelInfo.rcRect.left), fabs(m_stPanelInfo.rcRect.top - m_stPanelInfo.rcRect.bottom),
		m_pJobData->m_Misc.m_dThickness, m_stPanelInfo.iRowStripNumInPanel, m_stPanelInfo.iColStripNumInPanel, m_stPanelInfo.iRowUnitNumInPanel, m_stPanelInfo.iColUnitNumInPanel);
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	//[AlignMark]
	strValue.Format(_T("<alignmark count=\"%d\">\r\n"), m_stPanelInfo.vcAlignMarkInfo.size());
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	std::vector<RECTD> vcAlignInfo;
	for (auto itMark : m_stPanelInfo.vcAlignMarkInfo)
	{
		RECTD rcAlign = *itMark;

		//22.02.10, 0档 谅惑窜 扁霖栏肺 谅钎 拌魂 夸没 馆康
		rcAlign.left	= fabs(fabs(rcAlign.left) - tmpRect.left);
		rcAlign.right	= fabs(fabs(rcAlign.right) - tmpRect.left);
		rcAlign.top		= fabs(fabs(rcAlign.top) - tmpRect.top);
		rcAlign.bottom	= fabs(fabs(rcAlign.bottom) - tmpRect.top);

		/*if (m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal)
		{
			rcAlign.left = fabs(rcAlign.left - m_stPanelInfo.rcRect.right);
			rcAlign.right = fabs(rcAlign.right - m_stPanelInfo.rcRect.right);
		}
		else
		{
			rcAlign.left = fabs(rcAlign.left - m_stPanelInfo.rcRect.left);
			rcAlign.right = fabs(rcAlign.right - m_stPanelInfo.rcRect.left);
		}
		
		if (m_pUserSetInfo->mirrorDir == eDirectionType::eVertical)
		{
			rcAlign.top = fabs(rcAlign.top - m_stPanelInfo.rcRect.top);
			rcAlign.bottom = fabs(rcAlign.bottom - m_stPanelInfo.rcRect.top);
		}
		else
		{
			rcAlign.top = fabs(rcAlign.top - m_stPanelInfo.rcRect.bottom);
			rcAlign.bottom = fabs(rcAlign.bottom - m_stPanelInfo.rcRect.bottom);
		}
		
		if (rcAlign.right < rcAlign.left)	SWAP(rcAlign.left, rcAlign.right, dTmp);
		if (rcAlign.top < rcAlign.bottom)	SWAP(rcAlign.top, rcAlign.bottom, dTmp);
		*/

		vcAlignInfo.emplace_back(rcAlign);
	}

	std::sort(vcAlignInfo.begin(), vcAlignInfo.end(), tagDoubleRect::comp_var);

	UINT32 iMarkIdx = 1;	
	for (auto itMark : vcAlignInfo)
	{
		strValue.Format(_T("<m%d left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\" centerx=\"%.3f\" centery=\"%.3f\"></m%d>\r\n"),
			iMarkIdx, itMark.left, itMark.top, itMark.right, itMark.bottom,
			(itMark.left + itMark.right) / 2.0, (itMark.top + itMark.bottom) / 2.0, iMarkIdx);

		pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
		clResultFile.Write(pcFiledata, strValue.GetLength());
		free(pcFiledata);

		iMarkIdx++;
	}
	vcAlignInfo.clear();

	strValue.Format(_T("</alignmark>\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	//[STRIP]
	strValue.Format(_T("<strip count=\"%d\">\r\n"), m_stPanelInfo.vcStripInfo.size());
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	UINT32 iStripIdx = 1;
	for (auto itStrip : m_stPanelInfo.vcStripInfo)
	{
		if (m_pSystemSpec->sysBasic.bProfileMode)
		{
			strValue.Format(_T("<s%d index=\"%d\" type=\"%d\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\" block_row_count=\"%d\" block_col_count=\"%d\" unit_row_count=\"%d\" unit_col_count=\"%d\">\r\n"),
				iStripIdx, itStrip->iStripIdx + 1, itStrip->iType + 1, itStrip->rcProfile.left, itStrip->rcProfile.top, itStrip->rcProfile.right, itStrip->rcProfile.bottom,
				itStrip->iRowBlockNumInStrip, itStrip->iColBlockNumInStrip, itStrip->iRowUnitNumInStrip, itStrip->iColUnitNumInStrip);
		}
		else
		{
			strValue.Format(_T("<s%d index=\"%d\" type=\"%d\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\" block_row_count=\"%d\" block_col_count=\"%d\" unit_row_count=\"%d\" unit_col_count=\"%d\">\r\n"),
				iStripIdx, itStrip->iStripIdx + 1, itStrip->iType + 1, itStrip->rcRect.left, itStrip->rcRect.top, itStrip->rcRect.right, itStrip->rcRect.bottom,
				itStrip->iRowBlockNumInStrip, itStrip->iColBlockNumInStrip, itStrip->iRowUnitNumInStrip, itStrip->iColUnitNumInStrip);
		}

		pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
		clResultFile.Write(pcFiledata, strValue.GetLength());
		free(pcFiledata);

		//[BLOCK]
		UINT32 iBlockIdx = 1;
		for (auto itBlock : itStrip->vcBlockInfo)
		{
			if (m_pSystemSpec->sysBasic.bProfileMode)
			{
				strValue.Format(_T("<b%d index=\"%d\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\" unit_row_count=\"%d\" unit_col_count=\"%d\">\r\n"),
					iBlockIdx, itBlock->iBlockIdx + 1, itBlock->rcProfile.left, itBlock->rcProfile.top, itBlock->rcProfile.right, itBlock->rcProfile.bottom,
					itBlock->iRowUnitNumInBlock, itBlock->iColUnitNumInBlock);
			}
			else
			{
				strValue.Format(_T("<b%d index=\"%d\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\" unit_row_count=\"%d\" unit_col_count=\"%d\">\r\n"),
					iBlockIdx, itBlock->iBlockIdx + 1, itBlock->rcRect.left, itBlock->rcRect.top, itBlock->rcRect.right, itBlock->rcRect.bottom,
					itBlock->iRowUnitNumInBlock, itBlock->iColUnitNumInBlock);
			}

			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);

			//[UNIT]
			UINT32 iUnitIdx = 1;
			for (auto itUnit : itBlock->vcUnitInfo)
			{
				if (m_pSystemSpec->sysBasic.bProfileMode)
				{
					strValue.Format(_T("<u%d index=\"%d\" type=\"%d\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\"></u%d>\r\n"),
						iUnitIdx, itUnit->iUnitIdx_Ori/*iUnitIdx*/ /* + 1*/, itUnit->iType + 1, itUnit->rcProfile.left, itUnit->rcProfile.top, itUnit->rcProfile.right, itUnit->rcProfile.bottom, iUnitIdx);
				}
				else
				{
					strValue.Format(_T("<u%d index=\"%d\" type=\"%d\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\"></u%d>\r\n"),
						iUnitIdx, itUnit->iUnitIdx_Ori/*iUnitIdx*/ /*+ 1*/, itUnit->iType + 1, itUnit->rcRect.left, itUnit->rcRect.top, itUnit->rcRect.right, itUnit->rcRect.bottom, iUnitIdx);
				}

				pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
				clResultFile.Write(pcFiledata, strValue.GetLength());
				free(pcFiledata);
				iUnitIdx++;
			}
			strValue.Format(_T("</b%d>\r\n"), iBlockIdx);
			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);
			iBlockIdx++;
		}

		strValue.Format(_T("</s%d>\r\n"), iStripIdx);
		pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
		clResultFile.Write(pcFiledata, strValue.GetLength());
		free(pcFiledata);
		iStripIdx++;
	}

	strValue.Format(_T("</strip>\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	strValue.Format(_T("</panel>\r\n\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	strValue.Format(_T("</data>\r\n\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);
	clResultFile.Close();

	return RESULT_GOOD;
}

UINT32 CAOI::ExtractAlignMarkInfo(IN INT32 iStepIdx, IN enum Orient eOrient)
{
	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	if (iStepIdx != m_pUserLayerSetInfo->nPanelStepIdx)
		iStepIdx = m_pUserLayerSetInfo->nPanelStepIdx;

	CStep *pStep = m_pJobData->m_arrStep[iStepIdx];
	if (pStep == nullptr)
		return RESULT_BAD;

	CameraSpec_t *pSpec = nullptr;
	if (m_pSystemSpec->sysBasic.ScanType == eScanType::eLineScan)
		pSpec = &m_pSystemSpec->sysCamera[true];
	else
		pSpec = &m_pSystemSpec->sysCamera[false];

	if (pSpec == nullptr) return RESULT_BAD;

	double dResolMM = pSpec->dResolution / 1000.f;
	double dSwathWidthMM = pSpec->iFrameWidth * dResolMM;

	CLayer* pPatternLayer = nullptr;
	for (auto it : m_pUserLayerSetInfo->vcLayerSet)
	{
		if (it->strDefine.CompareNoCase(PATTERN_LAYER_TXT) != 0)
			continue;

		for (UINT32 lyno = 0; lyno < pStep->m_arrLayer.GetSize(); lyno++)
		{
			CLayer* pLayer = pStep->m_arrLayer.GetAt(lyno);
			if (pLayer == nullptr) continue;
			if (it->strLayer.CompareNoCase(pLayer->m_strLayerName) != 0) continue;

			pPatternLayer = pLayer;
		}
	}

	if (pPatternLayer == nullptr)
		return RESULT_BAD;

	double extend = 0;
	CRotMirrArr rotMirrArr;
	rotMirrArr.InputData(0.0, 0.0, eOrient, m_pUserSetInfo->mirrorDir);

	BOOL bFindAlignSymbol = FALSE;
	for (int feno = 0; feno < pPatternLayer->m_FeatureFile.m_arrFeature.GetSize(); feno++)
	{
		CFeature* pFeature = pPatternLayer->m_FeatureFile.m_arrFeature.GetAt(feno);
		if (pFeature == nullptr || pFeature->m_eType != FeatureType::P)
			continue;

		CSymbol* pSym = ((CFeatureP*)pFeature)->m_pSymbol;
		if (pSym == nullptr || pSym->m_pUserSymbol == nullptr)
			continue;

		if (pSym->m_eSymbolName != SymbolName::userdefined ||
			pSym->m_pUserSymbol->m_strSymbolName.CompareNoCase(ALIGN_SYMBOL1_TXT) != 0)
			continue;

		rotMirrArr.InputData(((CFeatureP*)pFeature)->m_dX, ((CFeatureP*)pFeature)->m_dY, ((CFeatureP*)pFeature)->m_eOrient);

		RECTD* pTmpMinMax = new RECTD;
		rotMirrArr.FinalPoint(&pTmpMinMax->left, &pTmpMinMax->top, pSym->m_MinMaxRect.left, pSym->m_MinMaxRect.top);
		rotMirrArr.FinalPoint(&pTmpMinMax->right, &pTmpMinMax->bottom, pSym->m_MinMaxRect.right, pSym->m_MinMaxRect.bottom);
		rotMirrArr.RemoveTail();

		if (pTmpMinMax->right >= (m_stPanelInfo.rcRect.left + dSwathWidthMM))
			continue;

		m_stPanelInfo.vcAlignMarkInfo.emplace_back(pTmpMinMax);
		bFindAlignSymbol = TRUE;
	}

	if (!bFindAlignSymbol)
	{
		rotMirrArr.ClearAll();
		rotMirrArr.InputData(0.0, 0.0, eOrient, m_pUserSetInfo->mirrorDir);

		RECTD rcTmpMinMax, rcAlignMark;
		int iAlignFeatureNum = 7, iFeatureNum = 0;		

		for (int feno = 0; feno < pPatternLayer->m_FeatureFile.m_arrFeature.GetSize(); feno++)
		{
			CFeature* pFeature = pPatternLayer->m_FeatureFile.m_arrFeature.GetAt(feno);
			if (pFeature == nullptr)
				continue;

			bool bFindFeature = false;
			for (int txtno = 0; txtno < pFeature->m_arrAttributeTextString.GetSize(); txtno++)
			{
				CAttributeTextString* pTextString = pFeature->m_arrAttributeTextString.GetAt(txtno);
				if (pTextString == nullptr || pTextString->m_strText.CompareNoCase(ALIGN_SYMBOL2_TXT) != 0)
					continue;

				bFindFeature = true;
				break;
			}

			if (!bFindFeature)
				continue;

			if (pFeature->m_eType == FeatureType::P)			
				iFeatureNum = 0;

			rotMirrArr.FinalPoint(&rcTmpMinMax.left, &rcTmpMinMax.top, pFeature->m_MinMaxRect.left, pFeature->m_MinMaxRect.top);
			rotMirrArr.FinalPoint(&rcTmpMinMax.right, &rcTmpMinMax.bottom, pFeature->m_MinMaxRect.right, pFeature->m_MinMaxRect.bottom);
			
			if (rcTmpMinMax.right >= (m_stPanelInfo.rcRect.left + dSwathWidthMM))
				continue;

			if (iFeatureNum == 0)
				rcAlignMark = rcTmpMinMax;
						
			if (fabs(rcAlignMark.left) > fabs(rcTmpMinMax.left))		rcAlignMark.left = rcTmpMinMax.left;
			if (fabs(rcAlignMark.right) < fabs(rcTmpMinMax.right))		rcAlignMark.right = rcTmpMinMax.right;
			if (fabs(rcAlignMark.bottom) > fabs(rcTmpMinMax.bottom))	rcAlignMark.bottom = rcTmpMinMax.bottom;
			if (fabs(rcAlignMark.top) < fabs(rcTmpMinMax.top))			rcAlignMark.top = rcTmpMinMax.top;			

			iFeatureNum++;
			if (iFeatureNum == iAlignFeatureNum)
			{
				RECTD *pAlignMark = new RECTD(rcAlignMark);
				m_stPanelInfo.vcAlignMarkInfo.emplace_back(pAlignMark);

				bFindAlignSymbol = TRUE;
			}
		}
	}

	rotMirrArr.ClearAll();

	std::sort(m_stPanelInfo.vcAlignMarkInfo.begin(), m_stPanelInfo.vcAlignMarkInfo.end(), tagDoubleRect::comp);

	if (!bFindAlignSymbol)
	{
		if (IDYES == AfxMessageBox(L"霉锅掳 Swath 郴 魄弛 倔扼牢 器牢飘啊 绝嚼聪促. 拌加 柳青 窍矫摆嚼聪鳖?", MB_YESNO))
			return RESULT_GOOD;
		else
			return RESULT_BAD;		
	}

	return RESULT_GOOD;
}

UINT32 CAOI::MakePanelAlignPointImage()
{
	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	//Panel Step	
	INT32 iStepIdx = m_nSelectStep;
	if (iStepIdx != m_pUserLayerSetInfo->nPanelStepIdx)
		iStepIdx = m_pUserLayerSetInfo->nPanelStepIdx;

	CStep *pStep = m_pJobData->m_arrStep[iStepIdx];
	if (pStep == nullptr)
		return RESULT_BAD;

	CameraSpec_t *pSpec = nullptr;
	if (m_pSystemSpec->sysBasic.ScanType == eScanType::eLineScan)
		pSpec = &m_pSystemSpec->sysCamera[true];
	else
		pSpec = &m_pSystemSpec->sysCamera[false];

	if (pSpec == nullptr) return RESULT_BAD;

	double dResolMM = pSpec->dResolution / 1000.f;

	Orient ori = Orient::Mir0Deg;
	GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, ori);

	double dTemp = 0.;
	UINT32 iAlignIdx = 0;
	for (auto align : m_stPanelInfo.vcAlignMarkInfo)
	{
		// Panel 郴狼 Align Point 困摹甫 Unit 郴 盔夯 谅钎肺 函券
		CRotMirrArr rotMirrArr;
		rotMirrArr.InputData(0, 0, ReverseOrient(ori), m_pUserSetInfo->mirrorDir);

		RECTD rcAlignRect;
		rotMirrArr.FinalPoint(&rcAlignRect.left, &rcAlignRect.top, align->left, align->top);
		rotMirrArr.FinalPoint(&rcAlignRect.right, &rcAlignRect.bottom, align->right, align->bottom);
		rotMirrArr.ClearAll();

		if (m_pUserSetInfo->bMirror)
		{
			if (m_pUserSetInfo->dAngle == 0.0)
			{
				SWAP(rcAlignRect.left, rcAlignRect.right, dTemp);
				SWAP(rcAlignRect.top, rcAlignRect.bottom, dTemp);
			}
			else if (m_pUserSetInfo->dAngle == 90.0)
			{
				rcAlignRect.left *= -1.0;
				rcAlignRect.right *= -1.0;

				SWAP(rcAlignRect.top, rcAlignRect.bottom, dTemp);
				rcAlignRect.top *= -1.0;
				rcAlignRect.bottom *= -1.0;
			}
			else if (m_pUserSetInfo->dAngle == 270.0)
			{
				SWAP(rcAlignRect.left, rcAlignRect.right, dTemp);
				rcAlignRect.left *= -1.0;
				rcAlignRect.right *= -1.0;

				rcAlignRect.top *= -1.0;
				rcAlignRect.bottom *= -1.0;
			}
		}

		CString strLog;
		strLog.Format(L"\nAlign,%d, %f, %f, %f, %f", iAlignIdx + 1, align->left, align->top, align->right, align->bottom);
		OutputDebugString(strLog);
		strLog.Format(L"\nAlign,%d, %f, %f, %f, %f", iAlignIdx + 1, rcAlignRect.left, rcAlignRect.top, rcAlignRect.right, rcAlignRect.bottom);
		OutputDebugString(strLog);

		if (RESULT_GOOD != m_pDrawManager->SavePanelAlignPoint(iStepIdx, iAlignIdx, 0.0, dResolMM, rcAlignRect, m_strModelPath))
			return RESULT_BAD;

		iAlignIdx++;
	}

	return RESULT_GOOD;
}

UINT32 CAOI::EstimateFOV_Based_On_Swath(INT32 iStepIdx, enum Orient eOrient)
{
	m_stFovInfo.Clear();

	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	if (iStepIdx < 0)
		return RESULT_BAD;

	if (m_pSystemSpec->sysBasic.ScanType != eScanType::eLineScan ||
		m_pUserSetInfo->fovType != eFovType::eVerticalJigjag)
		return RESULT_BAD;

	CStep *pStep = m_pJobData->m_arrStep[iStepIdx];
	if (pStep == nullptr)
		return RESULT_BAD;

	CameraSpec_t *pSpec = nullptr;
	if (m_pSystemSpec->sysBasic.ScanType == eScanType::eLineScan)
		pSpec = &m_pSystemSpec->sysCamera[true];
	else
		pSpec = &m_pSystemSpec->sysCamera[false];

	if (pSpec == nullptr) return RESULT_BAD;

	double dResolMM  = pSpec->dResolution / 1000.f;
	int iFrameWidth  = static_cast<int>(pSpec->iFrameWidth);
	int iFrameHeight = static_cast<int>(pSpec->iFrameHeight);
	int iMinOverlapX = static_cast<int>(pSpec->iMinOverlapX);
	int iMinOverlapY = static_cast<int>(pSpec->iMinOverlapY);
	int iMinMarginX  = static_cast<int>(pSpec->iMinMarginX);
	int iMinMarginY  = static_cast<int>(pSpec->iMinMarginY);

	if (m_pSystemSpec->sysBasic.ScanType == eScanType::eLineScan)
		iMinOverlapY = 0;

	if (dResolMM <= 0 || iFrameWidth <= 0 || iFrameHeight <= 0)
		return RESULT_BAD;

	if (iMinOverlapX < 0) iMinOverlapX = 0;
	if (iMinOverlapY < 0) iMinOverlapY = 0;
	if (iMinMarginX < 0) iMinMarginX = 0;
	if (iMinMarginY < 0) iMinMarginY = 0;

	RECTD rcWorkRect = m_pJobData->m_arrStep[iStepIdx]->m_ActiveRect;

/*	if (m_pSystemSpec->sysBasic.bExceptDummy)
	{
		rcWorkRect = m_stPanelInfo.rcRect_In_Panel;

//		if (m_pSystemSpec->sysBasic.bProfileMode)
//			rcWorkRect = m_stPanelInfo.rcProfile_wo_Dummy;
//		else
//			rcWorkRect = m_stPanelInfo.rcRect_wo_Dummy;
	}
*/
	if (rcWorkRect.IsRectNull())
		return RESULT_BAD;

	double dProdWidthMM = fabs(rcWorkRect.right - rcWorkRect.left);
	double dProdHeightMM = fabs(rcWorkRect.bottom - rcWorkRect.top);

	double dProdWidthPxl = Round(dProdWidthMM / dResolMM + 0.01);
	double dProdHeightPxl = Round(dProdHeightMM / dResolMM + 0.01);
	
	int iScanWidth = iFrameWidth - iMinOverlapX;
	int iScanHeight = iFrameHeight - iMinOverlapY;
	int iProdWidth = static_cast<int>(dProdWidthPxl) + 2 * iMinMarginX;
	int iProdHeight = static_cast<int>(dProdHeightPxl) + 2 * iMinMarginX;

	//Calculate # of Swath
	int iSwathNum = -1;
	if (iFrameWidth >= iProdWidth)
		iSwathNum = 1;
	else
	{
		double dNx = (double)iProdWidth / iFrameWidth;
		int iMinNx = (int)ceil(dNx);

		if (iProdWidth > (iMinNx - 1) * iScanWidth + iFrameWidth)
			iMinNx += 1;

		dNx = ((double)(iProdWidth - iFrameWidth) / iScanWidth) + 1;
		int iMaxNx = (int)ceil(dNx);

		if (iMaxNx < iMinNx) iMaxNx = iMinNx;

		for (int i = iMinNx; i <= iMaxNx; i++)
		{
			int iTotScanWidth = (i - 1) * iScanWidth + iFrameWidth;
			int iDiffWidth = iTotScanWidth - iProdWidth;

			if (iDiffWidth >= 0)
			{
				iSwathNum = i;
				break;
			}
		}
	}

	if (iSwathNum <= 0)	return RESULT_BAD;
	if (iFrameWidth < iProdWidth && iProdWidth >(iSwathNum - 1) * iScanWidth + iFrameWidth)
		iSwathNum += 1;

	// Set Swath Info
	m_stFovInfo.stBasic.swathNum		= iSwathNum;
	m_stFovInfo.stBasic.scanType		= m_pSystemSpec->sysBasic.ScanType;
	m_stFovInfo.stBasic.fovType			= m_pUserSetInfo->fovType;
	m_stFovInfo.stBasic.resolution		= pSpec->dResolution;
	m_stFovInfo.stBasic.fovSize.cx		= iFrameWidth;
	m_stFovInfo.stBasic.fovSize.cy		= iProdHeight;
	m_stFovInfo.stBasic.fovOverlap.cx	= iMinOverlapX;
	m_stFovInfo.stBasic.fovOverlap.cy	= iMinOverlapY;
	m_stFovInfo.stBasic.fovMargin.cx	= iMinMarginX;
	m_stFovInfo.stBasic.fovMargin.cy	= iMinMarginY;
	m_stFovInfo.stBasic.prodSize.cx		= iProdWidth;
	m_stFovInfo.stBasic.prodSize.cy		= iProdHeight;

	double dTemp = 0.;
	Orient ori = Orient::Mir0Deg;
	CDataManager::GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, ori);

	CRotMirrArr rotMirrArr;
	rotMirrArr.InputData(0, 0, CDataManager::ReverseOrient(ori), m_pUserSetInfo->mirrorDir);

	CString str;
	for (int i = 0; i < iSwathNum; i++)
	{
		FovData_t* pFov = new FovData_t;
		pFov->swathIdx = i;
		pFov->fovIdx.x = i;
		pFov->fovIdx.y = 0;

		pFov->rcRectPxl.left   = i * (iFrameWidth - iMinOverlapX);
		pFov->rcRectPxl.right  = pFov->rcRectPxl.left + iFrameWidth - 1;
		pFov->rcRectPxl.top    = 0;
		pFov->rcRectPxl.bottom = iProdHeight - 1;
				
		pFov->rcRectMM.left = rcWorkRect.left - (iMinMarginX * dResolMM) + ((i * (iFrameWidth - iMinOverlapX)) * dResolMM);
		pFov->rcRectMM.right = pFov->rcRectMM.left + (iFrameWidth * dResolMM);
		pFov->rcRectMM.top = rcWorkRect.top + (iMinMarginY * dResolMM);
		pFov->rcRectMM.bottom = pFov->rcRectMM.top - (iProdHeight * dResolMM);

		if (fabs(pFov->rcRectMM.left) < EPSILON_DELTA1)		pFov->rcRectMM.left = 0;
		if (fabs(pFov->rcRectMM.right) < EPSILON_DELTA1)	pFov->rcRectMM.right = 0;
		if (fabs(pFov->rcRectMM.top) < EPSILON_DELTA1)		pFov->rcRectMM.top = 0;
		if (fabs(pFov->rcRectMM.bottom) < EPSILON_DELTA1)	pFov->rcRectMM.bottom = 0;

		rotMirrArr.FinalPoint(&pFov->rcDraw.left, &pFov->rcDraw.top, pFov->rcRectMM.left, pFov->rcRectMM.top);
		rotMirrArr.FinalPoint(&pFov->rcDraw.right, &pFov->rcDraw.bottom, pFov->rcRectMM.right, pFov->rcRectMM.bottom);

		if (m_pUserSetInfo->bMirror)
		{
			if (m_pUserSetInfo->dAngle == 0.0)
			{
				SWAP(pFov->rcDraw.left, pFov->rcDraw.right, dTemp);
				SWAP(pFov->rcDraw.top, pFov->rcDraw.bottom, dTemp);
			}
			else if (m_pUserSetInfo->dAngle == 90.0)
			{				
				if (m_pUserSetInfo->mirrorDir != eDirectionType::eBoth)
				{
					pFov->rcDraw.left *= -1.0;
					pFov->rcDraw.right *= -1.0;

					SWAP(pFov->rcDraw.top, pFov->rcDraw.bottom, dTemp);
					pFov->rcDraw.top *= -1.0;
					pFov->rcDraw.bottom *= -1.0;
				}
			}
			else if (m_pUserSetInfo->dAngle == 270.0)
			{		
				if (m_pUserSetInfo->mirrorDir != eDirectionType::eBoth)
				{
					SWAP(pFov->rcDraw.left, pFov->rcDraw.right, dTemp);
					pFov->rcDraw.left *= -1.0;
					pFov->rcDraw.right *= -1.0;

					pFov->rcDraw.top *= -1.0;
					pFov->rcDraw.bottom *= -1.0;
				}
			}
		}

//		str.Format(_T("Swath [%d] : (L,T,R,B) (%.2f,%.2f,%.2f,%.2f)\n"), i, pFov->rcDraw.left, pFov->rcDraw.top, pFov->rcDraw.right, pFov->rcDraw.bottom);
//		OutputDebugString(str);
		str.Format(_T("Swath [%d] : (L,T,R,B) (%.2f,%.2f,%.2f,%.2f)\n"), i, pFov->rcRectMM.left, pFov->rcRectMM.top, pFov->rcRectMM.right, pFov->rcRectMM.bottom);
		OutputDebugString(str);

		m_stFovInfo.vcFovData.emplace_back(pFov);
	}

	if (m_stFovInfo.vcFovData.size() != iSwathNum) 
		return RESULT_BAD;

	// Calculate Overlap between Swath	
	BOOL bLeft = FALSE, bBot = FALSE;
	int optNx = iSwathNum;
	int optNy = 1;

	int iCol, iRow, rindex;
	for (iCol = 0; iCol < optNx; iCol++)
	{
		if (bLeft)
		{
			for (iRow = optNy - 1, rindex = 0; iRow >= 0; iRow--, rindex++)
			{
				int iCell = optNy * iCol + rindex;
				if (iRow > 0)
				{
					m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.top = m_stFovInfo.vcFovData[optNy * iCol + rindex + 1]->rcRectPxl.bottom - m_stFovInfo.vcFovData[iCell]->rcRectPxl.top + 1;
				}

				if (iRow + 1 < optNy)
				{
					m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.bottom = m_stFovInfo.vcFovData[iCell]->rcRectPxl.bottom + iProdHeight - m_stFovInfo.vcFovData[optNy * iCol + rindex - 1]->rcRectPxl.bottom;
				}

				if (iCol > 0)
				{
					m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.left = m_stFovInfo.vcFovData[optNy * (iCol - 1) + iRow]->rcRectPxl.right - m_stFovInfo.vcFovData[iCell]->rcRectPxl.left + 1;
				}

				if (iCol + 1 < optNx)
				{
					m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.right = m_stFovInfo.vcFovData[iCell]->rcRectPxl.right - m_stFovInfo.vcFovData[optNy * (iCol + 1) + iRow]->rcRectPxl.left + 1;
				}

				if (m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.top < 0)		m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.top = 0;
				if (m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.bottom < 0)	m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.bottom = 0;
				if (m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.left < 0)	m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.left = 0;
				if (m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.right < 0)	m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.right = 0;

				str.Format(_T("%d,%d,%d,%d\n"), m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.left, m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.top,
					m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.right, m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.bottom);
				OutputDebugString(str);
			}
			bLeft = FALSE;
		}
		else
		{
			for (iRow = 0, rindex = optNy - 1; iRow < optNy; iRow++, rindex)
			{
				int iCell = optNy * iCol + iRow;
				if (iRow > 0)
				{
					m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.top = m_stFovInfo.vcFovData[optNy * iCol + iRow - 1]->rcRectPxl.bottom - m_stFovInfo.vcFovData[iCell]->rcRectPxl.top + 1;
				}

				if (iRow + 1 < optNy)
				{
					m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.bottom = m_stFovInfo.vcFovData[iCell]->rcRectPxl.bottom + iProdHeight - m_stFovInfo.vcFovData[optNy * iCol + iRow + 1]->rcRectPxl.bottom;
				}

				if (iCol > 0)
				{
					m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.left = m_stFovInfo.vcFovData[optNy * (iCol - 1) + rindex]->rcRectPxl.right - m_stFovInfo.vcFovData[iCell]->rcRectPxl.left + 1;
				}

				if (iCol + 1 < optNx)
				{
					m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.right = m_stFovInfo.vcFovData[iCell]->rcRectPxl.right - m_stFovInfo.vcFovData[optNy * (iCol + 1) + rindex]->rcRectPxl.left + 1;
				}

				if (m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.top < 0)		m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.top = 0;
				if (m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.bottom < 0)	m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.bottom = 0;
				if (m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.left < 0)	m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.left = 0;
				if (m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.right < 0)	m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.right = 0;

				str.Format(_T("%d,%d,%d,%d\n"), m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.left, m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.top,
				m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.right, m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.bottom);
				OutputDebugString(str);
			}
			bLeft = TRUE;
		}
	}

	return RESULT_GOOD;
}

UINT32 CAOI::MakeUnitAlignPointImage()
{
	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	//Panel Step	
	INT32 iStepIdx = m_nSelectStep;
	if (iStepIdx != m_pUserLayerSetInfo->nPanelStepIdx)
		iStepIdx = m_pUserLayerSetInfo->nPanelStepIdx;

	CStep *pStep = m_pJobData->m_arrStep[iStepIdx];
	if (pStep == nullptr)
		return RESULT_BAD;

	CameraSpec_t *pSpec = nullptr;
	if (m_pSystemSpec->sysBasic.ScanType == eScanType::eLineScan)
		pSpec = &m_pSystemSpec->sysCamera[true];
	else
		pSpec = &m_pSystemSpec->sysCamera[false];

	if (pSpec == nullptr) return RESULT_BAD;

	double dResolMM = pSpec->dResolution / 1000.f;

	Orient eOrient = Orient::Mir0Deg;
	CDataManager::GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, eOrient);

	//for (auto itUnitStep : m_pUserLayerSetInfo->vcUnitStepIdx)
	{
		m_pDrawManager->SaveUnitAlignPoint_In_Panel(iStepIdx, eOrient, dResolMM, m_strModelPath);
	}

	return RESULT_GOOD;
}

UINT32 CAOI::MakeMasterImage()
{
	CString strRemovePathName = m_strModelPath + _T("\\Layers");
	CUtils::RemoveDirectoryFile(strRemovePathName);

	//Unit Step


	//Panel Step	
	if (RESULT_GOOD != MakeMasterImage_Panel())
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CAOI::MakeMasterImage_Panel()
{
	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	INT32 iStepIdx = m_nSelectStep;
	if (iStepIdx != m_pUserLayerSetInfo->nPanelStepIdx)
		iStepIdx = m_pUserLayerSetInfo->nPanelStepIdx;

	CStep *pStep = m_pJobData->m_arrStep[iStepIdx];
	if (pStep == nullptr)
		return RESULT_BAD;

	bool bMirror = false;
	double dCamAngle = 0.;
	CameraSpec_t *pSpec = nullptr;
	if (m_pSystemSpec->sysBasic.ScanType == eScanType::eLineScan)
		pSpec = &m_pSystemSpec->sysCamera[true];
	else
		pSpec = &m_pSystemSpec->sysCamera[false];

	if (pSpec == nullptr) return RESULT_BAD;

	double dResolMM = pSpec->dResolution / 1000.f;

	//Feaure Save
 	if (RESULT_GOOD != m_pDrawManager->SavePanelLayer(iStepIdx, 0, dCamAngle, dResolMM, m_strModelPath))
 		return RESULT_BAD;

	//V2
	if (m_pSystemSpec->sysBasic.bLineArc_SubSave == true)
	{
		//Feature SubSave
		int nOptionCount = static_cast<int>(m_vecvecWidthSort.size());
		for (int nOption = 0; nOption < nOptionCount; nOption++)
		{
			if (nOption != eLineWidthOption::eLineArcPos && nOption != eLineWidthOption::eLineArcNeg) continue;

			if (RESULT_GOOD != m_pDrawManager->SavePanelLayer_FeatureSub(iStepIdx, 0, dCamAngle, dResolMM, m_strModelPath,
				nOption, m_vecvecWidthSort[nOption]))
			{
				return RESULT_BAD;
			}
		}
	}

	//V3
// 	if (m_pSystemSpec->sysBasic.bLineArc_SubSave == true)
// 	{
// 		//Feature SubSave
// 		if (RESULT_GOOD != m_pDrawManager->SavePanelLayer_FeatureSub_V3(iStepIdx, 0, dResolMM, m_strModelPath,
// 			m_vecvecWidthSort))
// 		{
// 			return RESULT_BAD;
// 		}
// 
// 		
// 	}

	return RESULT_GOOD;
}

UINT32 CAOI::MakeMasterImage_Unit_in_Panel()
{
	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	INT32 iStepIdx = m_nSelectStep;
	if (iStepIdx != m_pUserLayerSetInfo->nPanelStepIdx)
		iStepIdx = m_pUserLayerSetInfo->nPanelStepIdx;

	CStep *pStep = m_pJobData->m_arrStep[iStepIdx];
	if (pStep == nullptr)
		return RESULT_BAD;

	bool bMirror = false;
	double dCamAngle = 0.;
	CameraSpec_t *pSpec = nullptr;
	if (m_pSystemSpec->sysBasic.ScanType == eScanType::eLineScan)
		pSpec = &m_pSystemSpec->sysCamera[true];
	else
		pSpec = &m_pSystemSpec->sysCamera[false];

	if (pSpec == nullptr) return RESULT_BAD;

	double dResolMM = pSpec->dResolution / 1000.f;

	Orient eOrient = Orient::Mir0Deg;
	CDataManager::GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, eOrient);

	if (RESULT_GOOD != m_pDrawManager->SaveUnitLayer_in_Panel(iStepIdx, 0, eOrient, dResolMM, m_strModelPath))
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CAOI::MakeProfileImage()
{
	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	Orient eOrient = Orient::Mir0Deg;
	CDataManager::GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, eOrient);

	if (eOrient == m_eProfileOrient)
		return RESULT_GOOD;

	if (m_strModelPath.GetLength() < 1)
	{
		m_strModelPath = GetModelPath(true);
		if (m_strModelPath.GetLength() < 1)
			return RESULT_BAD;
	}

	CString strRemovePathName = m_strModelPath + _T("\\Profile");
	CUtils::RemoveDirectoryFile(strRemovePathName);

	INT32 iStepIdx = m_nSelectStep;
	bool bIsPanel = (iStepIdx == m_pUserLayerSetInfo->nPanelStepIdx);

	if (bIsPanel)
	{
		Orient eOrient = Orient::Mir0Deg;
		CDataManager::GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, eOrient);
		m_pDrawManager->SaveProfile(m_bIs4Step, iStepIdx, eOrient, m_strModelPath);
	}

	for (auto itStrip : m_pUserLayerSetInfo->vcStripStepIdx)
	{
		Orient eOrient = Orient::Mir0Deg;
		CDataManager::GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, eOrient);

		if (!(bIsPanel || itStrip == iStepIdx))
			continue;

		Orient eStripOrient = Orient::NoMir0Deg;
		CString strStripStepName = m_pJobData->m_arrStep[itStrip]->m_strStepName;

		bool bFind = false;
		for (auto it : m_stPanelInfo.vcStripType)
		{
			if (it->strStepName.CompareNoCase(strStripStepName) == 0)
			{
				eStripOrient = static_cast<Orient>(it->iOrient);
				bFind = true;
				break;
			}
		}

		if (bFind)
		{
			eOrient = AddOrient(eOrient, eStripOrient);
			m_pDrawManager->SaveProfile(FALSE, itStrip, eOrient, m_strModelPath);
		}
	}

	//3.Unit
	/*for (auto itUnit : m_pUserLayerSetInfo->vcUnitStepIdx)
	{
		Orient eOrient = Orient::Mir0Deg;
		CDataManager::GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, eOrient);

		if (!(bIsPanel || itUnit == iStepIdx))
			continue;

		Orient eUnitOrient = Orient::NoMir0Deg;
		CString strUnitStepName = m_pJobData->m_arrStep[itUnit]->m_strStepName;

		bool bFind = false;
		for (auto it : m_stPanelInfo.vcUnitType)
		{
			if (it->strStepName.CompareNoCase(strUnitStepName) == 0)
			{
				eUnitOrient = static_cast<Orient>(it->iOrient);
				bFind = true;
				break;
			}
		}

		if (bFind)
		{
			eOrient = AddOrient(eOrient, eUnitOrient);
			m_pDrawManager->SaveProfile(FALSE, itUnit, eOrient, m_strModelPath);
		}
	}*/

	CDataManager::GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, m_eProfileOrient);

	return RESULT_GOOD;
}

UINT32 CAOI::EstimateCell_Based_On_Swath(INT32 iStepIdx, enum Orient eOrient)
{
	m_stCellInfo.Clear();

	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	if (iStepIdx < 0)
		return RESULT_BAD;
	
	if (m_stFovInfo.vcFovData.size() <= 0)
		return RESULT_BAD;

	CameraSpec_t *pSpec = nullptr;
	if (m_pSystemSpec->sysBasic.ScanType == eScanType::eLineScan)
		pSpec = &m_pSystemSpec->sysCamera[true];
	else
		pSpec = &m_pSystemSpec->sysCamera[false];

	if (pSpec == nullptr) return RESULT_BAD;

	double dResolMM = pSpec->dResolution / 1000.f;
	int iCellColNum = static_cast<int>(pSpec->iCellColNum);
	int iCellRowNum = static_cast<int>(pSpec->iCellRowNum);
	int iCellMinOverlapX = static_cast<int>(pSpec->iCellMinOverlapX);
	int iCellMinOverlapY = static_cast<int>(pSpec->iCellMinOverlapY);

	if (dResolMM <= 0 || iCellColNum <= 0 || iCellRowNum <= 0)
	{
		AfxMessageBox(_T("Cell 俺荐 涝仿 蔼捞 棵官福瘤 臼嚼聪促."));
		return RESULT_BAD;
	}

	if (iCellMinOverlapX < 0) iCellMinOverlapX = 0;
	if (iCellMinOverlapY < 0) iCellMinOverlapY = 0;

	int iSwathWidth = static_cast<int>(m_stFovInfo.stBasic.fovSize.cx);
	int iSwathHeight = static_cast<int>(m_stFovInfo.stBasic.fovSize.cy);
	if (iSwathWidth <= 0 || iSwathHeight <= 0)
		return RESULT_BAD;

	int iSwathMarginX = static_cast<int>(m_stFovInfo.stBasic.fovMargin.cx);
	int iSwathMarginY = static_cast<int>(m_stFovInfo.stBasic.fovMargin.cy);

	// Estimate Cell Size	
	int iTotCellWidth = iSwathWidth - (2 * iSwathMarginX);// +iCellMinOverlapX;
	int iTotCellHeight = iSwathHeight - (2 * iSwathMarginY);// +iCellMinOverlapY;
	if (iTotCellWidth <= 0 || iTotCellHeight <= 0)
		return RESULT_BAD;

	int iCellWidth = static_cast<int>(ceil((float)(iTotCellWidth - iCellMinOverlapX) / iCellColNum) + iCellMinOverlapX);
	int iCellHeight = static_cast<int>(ceil((float)(iTotCellHeight - iCellMinOverlapY) / iCellRowNum) + iCellMinOverlapY);

	int iDiffX = 0;
	if (iCellColNum > 1)
		iDiffX = ((iCellWidth - iCellMinOverlapX) * (iCellColNum - 1)) + iCellWidth - iTotCellWidth;
	else
		iDiffX = iCellWidth - iTotCellWidth;

	int iDiffY = 0;
	if (iCellRowNum > 1)
		iDiffY = ((iCellHeight - iCellMinOverlapY) * (iCellRowNum - 1)) + iCellHeight - iTotCellHeight;
	else
		iDiffY = iCellHeight - iTotCellHeight;

	if (iDiffX < 0 || iDiffY < 0)
		return RESULT_BAD;

	if (iDiffX > 0)
	{
		if (iCellColNum > 1)
			iDiffX = static_cast<int>(((float)iDiffX / (iCellColNum - 1)) + 0.5);
		iCellWidth += iDiffX;
	}

	if (iDiffY > 0)
	{
		if (iCellRowNum > 1)
			iDiffY = static_cast<int>(((float)iDiffY / (iCellRowNum - 1)) + 0.5);
		iCellHeight += iDiffY;
	}

	int iCellMinOverlapX_bw_Swath = iCellMinOverlapX / iCellColNum;
	iCellWidth += iCellMinOverlapX_bw_Swath;

	// Set Cell Info
	m_stCellInfo.stBasic.cellNum.cx = iCellColNum * m_stFovInfo.stBasic.swathNum;
	m_stCellInfo.stBasic.cellNum.cy = iCellRowNum;
	m_stCellInfo.stBasic.cellSize.cx = iCellWidth;
	m_stCellInfo.stBasic.cellSize.cy = iCellHeight;
	m_stCellInfo.stBasic.cellOverlap.cx = iCellMinOverlapX;
	m_stCellInfo.stBasic.cellOverlap.cy = iCellMinOverlapY;

	double dTemp = 0.;
	Orient ori = Orient::Mir0Deg;
	CDataManager::GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, ori);

	CRotMirrArr rotMirrArr;
	rotMirrArr.InputData(0, 0, CDataManager::ReverseOrient(ori), m_pUserSetInfo->mirrorDir);

	CString str;
	for (int iSwath = 0; iSwath < static_cast<int>(m_stFovInfo.stBasic.swathNum); iSwath++)
	{		
		RECTD	rcRectMM = m_stFovInfo.vcFovData[iSwath]->rcRectMM;
		RECT	rcRectPxl = m_stFovInfo.vcFovData[iSwath]->rcRectPxl;
			
		for (int iRow = 0; iRow < iCellRowNum; iRow++)
		{
			for (int iCol = 0; iCol < iCellColNum; iCol++)
			{				
				FovData_t* pCell = new FovData_t;
				pCell->swathIdx = iSwath;

				pCell->fovIdx.x = iCol;
				pCell->fovIdx.y = iRow;

				//1. Overlap b/w Cell in Swath 绊妨
//				pCell->rcRectPxl.left = rcRectPxl.left + iSwathMarginX + iCol * (iCellWidth - iCellMinOverlapX);
//				pCell->rcRectPxl.right = pCell->rcRectPxl.left + iCellWidth - 1;
//				pCell->rcRectPxl.top = rcRectPxl.top + iSwathMarginY + iRow * (iCellHeight - iCellMinOverlapY);
//				pCell->rcRectPxl.bottom = pCell->rcRectPxl.top + iCellHeight - 1;

//				pCell->rcRectMM.left = rcRectMM.left + (iSwathMarginX * dResolMM) + ((iCol * (iCellWidth - iCellMinOverlapX)) * dResolMM);
//				pCell->rcRectMM.right = pCell->rcRectMM.left + (iCellWidth * dResolMM);
//				pCell->rcRectMM.top = rcRectMM.top - (iSwathMarginY * dResolMM) - ((iRow * (iCellHeight - iCellMinOverlapY)) * dResolMM);
//				pCell->rcRectMM.bottom = pCell->rcRectMM.top - (iCellHeight * dResolMM);

				//2. Overlap b/w All Cells 绊妨
				pCell->rcRectPxl.left = rcRectPxl.left + (iSwathMarginX - iCellMinOverlapX_bw_Swath) + iCol * (iCellWidth - iCellMinOverlapX);
			//	pCell->rcRectPxl.left = rcRectPxl.left + (iSwathMarginX) + iCol * (iCellWidth - iCellMinOverlapX);
				pCell->rcRectPxl.right = pCell->rcRectPxl.left + iCellWidth - 1;
				pCell->rcRectPxl.top = rcRectPxl.top + iSwathMarginY + iRow * (iCellHeight - iCellMinOverlapY);
				pCell->rcRectPxl.bottom = pCell->rcRectPxl.top + iCellHeight - 1;

				pCell->rcRectMM.left = rcRectMM.left + ((iSwathMarginX - iCellMinOverlapX_bw_Swath) * dResolMM) + ((iCol * (iCellWidth - iCellMinOverlapX)) * dResolMM);
			//	pCell->rcRectMM.left = rcRectMM.left + (iSwathMarginX * dResolMM) + ((iCol * (iCellWidth - iCellMinOverlapX)) * dResolMM);
				pCell->rcRectMM.right = pCell->rcRectMM.left + (iCellWidth * dResolMM);		
				pCell->rcRectMM.top = rcRectMM.top - (iSwathMarginY * dResolMM) - ((iRow * (iCellHeight - iCellMinOverlapY)) * dResolMM);
				pCell->rcRectMM.bottom = pCell->rcRectMM.top - (iCellHeight * dResolMM);
		
				if (fabs(pCell->rcRectMM.left) < EPSILON_DELTA1)	pCell->rcRectMM.left = 0;
				if (fabs(pCell->rcRectMM.right) < EPSILON_DELTA1)	pCell->rcRectMM.right = 0;
				if (fabs(pCell->rcRectMM.top) < EPSILON_DELTA1)		pCell->rcRectMM.top = 0;
				if (fabs(pCell->rcRectMM.bottom) < EPSILON_DELTA1)	pCell->rcRectMM.bottom = 0;

				rotMirrArr.FinalPoint(&pCell->rcDraw.left, &pCell->rcDraw.top, pCell->rcRectMM.left, pCell->rcRectMM.top);
				rotMirrArr.FinalPoint(&pCell->rcDraw.right, &pCell->rcDraw.bottom, pCell->rcRectMM.right, pCell->rcRectMM.bottom);

				if (m_pUserSetInfo->bMirror)
				{
					if (m_pUserSetInfo->dAngle == 0.0)
					{
						SWAP(pCell->rcDraw.left, pCell->rcDraw.right, dTemp);
						SWAP(pCell->rcDraw.top, pCell->rcDraw.bottom, dTemp);
					}
					else if (m_pUserSetInfo->dAngle == 90.0)
					{
						if (m_pUserSetInfo->mirrorDir != eDirectionType::eBoth)
						{
							pCell->rcDraw.left *= -1.0;
							pCell->rcDraw.right *= -1.0;

							SWAP(pCell->rcDraw.top, pCell->rcDraw.bottom, dTemp);
							pCell->rcDraw.top *= -1.0;
							pCell->rcDraw.bottom *= -1.0;
						}
					}
					else if (m_pUserSetInfo->dAngle == 270.0)
					{
						if (m_pUserSetInfo->mirrorDir != eDirectionType::eBoth)
						{
							SWAP(pCell->rcDraw.left, pCell->rcDraw.right, dTemp);
							pCell->rcDraw.left *= -1.0;
							pCell->rcDraw.right *= -1.0;

							pCell->rcDraw.top *= -1.0;
							pCell->rcDraw.bottom *= -1.0;
						}
					}
				}

//				str.Format(_T("Cell Swath[%d] Col[%d] Row[%d] : (L,T,R,B) (%.2f,%.2f,%.2f,%.2f)\n"), iSwath, iCol, iRow, pCell->rcDraw.left, pCell->rcDraw.top, pCell->rcDraw.right, pCell->rcDraw.bottom);
//				OutputDebugString(str);
				str.Format(_T("Cell Swath[%d] Col[%d] Row[%d] : (L,T,R,B) (%.2f,%.2f,%.2f,%.2f)\n"), iSwath, iCol, iRow, pCell->rcRectMM.left, pCell->rcRectMM.top, pCell->rcRectMM.right, pCell->rcRectMM.bottom);
				OutputDebugString(str);

				m_stCellInfo.vcCellData.emplace_back(pCell);
			}
		}
	}

	if (m_stCellInfo.vcCellData.size() != m_stCellInfo.stBasic.cellNum.cx * m_stCellInfo.stBasic.cellNum.cy)
		return RESULT_BAD;

	int iCellNum = static_cast<int>(m_stCellInfo.vcCellData.size());

	// Calculate Overlap between Cell	
	for (auto itCell : m_stCellInfo.vcCellData)
	{
		int iCell = itCell->swathIdx * (iCellColNum * iCellRowNum) + (iCellColNum * itCell->fovIdx.y) + itCell->fovIdx.x;

		int iLeft = -1;
		if (itCell->fovIdx.x == 0 && itCell->swathIdx > 0)
			iLeft = (itCell->swathIdx - 1) * (iCellColNum * iCellRowNum) + (iCellColNum * itCell->fovIdx.y) + iCellColNum - 1;
		else if (itCell->fovIdx.x > 0 && itCell->fovIdx.x < iCellColNum)
			iLeft = itCell->swathIdx * (iCellColNum * iCellRowNum) + (iCellColNum * itCell->fovIdx.y) + itCell->fovIdx.x - 1;

		int iRight = -1;
		if (itCell->fovIdx.x + 1 == iCellColNum)
			iRight = (itCell->swathIdx + 1) * (iCellColNum * iCellRowNum) + (iCellColNum * itCell->fovIdx.y);
		else if (itCell->fovIdx.x < iCellColNum)
			iRight = itCell->swathIdx * (iCellColNum * iCellRowNum) + (iCellColNum * itCell->fovIdx.y) + itCell->fovIdx.x + 1;

		int iTop = -1;
		if (itCell->fovIdx.y > 0 && itCell->fovIdx.y < iCellRowNum)
			iTop = itCell->swathIdx * (iCellColNum * iCellRowNum) + (iCellColNum * (itCell->fovIdx.y - 1)) + itCell->fovIdx.x;

		int iBottom = -1;
		if (itCell->fovIdx.y + 1 < iCellRowNum)
			iBottom = itCell->swathIdx * (iCellColNum * iCellRowNum) + (iCellColNum * (itCell->fovIdx.y + 1)) + itCell->fovIdx.x;

		if (iLeft > 0 && iLeft < iCellNum)
			m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.left = m_stCellInfo.vcCellData[iLeft]->rcRectPxl.right - m_stCellInfo.vcCellData[iCell]->rcRectPxl.left + 1;
		if (iTop > 0 && iTop < iCellNum)
			m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.top = m_stCellInfo.vcCellData[iTop]->rcRectPxl.bottom - m_stCellInfo.vcCellData[iCell]->rcRectPxl.top + 1;
		if (iRight > 0 && iRight < iCellNum)
			m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.right = m_stCellInfo.vcCellData[iCell]->rcRectPxl.right - m_stCellInfo.vcCellData[iRight]->rcRectPxl.left + 1;
		if (iBottom > 0 && iBottom < iCellNum)
			m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.bottom = m_stCellInfo.vcCellData[iCell]->rcRectPxl.bottom - m_stCellInfo.vcCellData[iBottom]->rcRectPxl.top + 1;

		if (m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.left < 0)		m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.left = 0;
		if (m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.top < 0)		m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.top = 0;		
		if (m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.right < 0)		m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.right = 0;
		if (m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.bottom < 0)	m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.bottom = 0;

		str.Format(_T("%d,%d,%d,%d,%d\n"), iCell + 1, m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.left, m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.top,
			m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.right, m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.bottom);
		OutputDebugString(str);
	}

	return RESULT_GOOD;
}

UINT32 CAOI::EstimateCell_Based_On_Swath_V2(INT32 iStepIdx, enum Orient eOrient)
{
	m_stCellInfo.Clear();

	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	if (iStepIdx < 0)
		return RESULT_BAD;
	
	if (m_stFovInfo.vcFovData.size() <= 0)
		return RESULT_BAD;

	CameraSpec_t *pSpec = nullptr;
	if (m_pSystemSpec->sysBasic.ScanType == eScanType::eLineScan)
		pSpec = &m_pSystemSpec->sysCamera[true];
	else
		pSpec = &m_pSystemSpec->sysCamera[false];

	if (pSpec == nullptr) return RESULT_BAD;

	double dResolMM = pSpec->dResolution / 1000.f;
	int iCellColNum = static_cast<int>(pSpec->iCellColNum);
	int iCellRowNum = static_cast<int>(pSpec->iCellRowNum);
	int iCellMinOverlapX = static_cast<int>(pSpec->iCellMinOverlapX);
	int iCellMinOverlapY = static_cast<int>(pSpec->iCellMinOverlapY);

	if (dResolMM <= 0 || iCellColNum <= 0 || iCellRowNum <= 0)
	{
		AfxMessageBox(_T("Cell 俺荐 涝仿 蔼捞 棵官福瘤 臼嚼聪促."));
		return RESULT_BAD;
	}

	if (iCellMinOverlapX < 0) iCellMinOverlapX = 0;
	if (iCellMinOverlapY < 0) iCellMinOverlapY = 0;

	int iSwathWidth = static_cast<int>(m_stFovInfo.stBasic.fovSize.cx);
	int iSwathHeight = static_cast<int>(m_stFovInfo.stBasic.fovSize.cy);
	if (iSwathWidth <= 0 || iSwathHeight <= 0)
		return RESULT_BAD;

	int iSwathMarginX = static_cast<int>(m_stFovInfo.stBasic.fovMargin.cx);
	int iSwathMarginY = static_cast<int>(m_stFovInfo.stBasic.fovMargin.cy);

	// Estimate Cell Size	
	int iTotCellWidth = iSwathWidth - (2 * iSwathMarginX);// +iCellMinOverlapX;
	int iTotCellHeight = iSwathHeight - (2 * iSwathMarginY);// +iCellMinOverlapY;
	if (iTotCellWidth <= 0 || iTotCellHeight <= 0)
		return RESULT_BAD;

	int iCellWidth = static_cast<int>(ceil((float)(iTotCellWidth - iCellMinOverlapX) / iCellColNum) + iCellMinOverlapX);
	int iCellHeight = static_cast<int>(ceil((float)(iTotCellHeight - iCellMinOverlapY) / iCellRowNum) + iCellMinOverlapY);

	int iDiffX = 0;
	if (iCellColNum > 1)
		iDiffX = ((iCellWidth - iCellMinOverlapX) * (iCellColNum - 1)) + iCellWidth - iTotCellWidth;
	else
		iDiffX = iCellWidth - iTotCellWidth;

	int iDiffY = 0;
	if (iCellRowNum > 1)
		iDiffY = ((iCellHeight - iCellMinOverlapY) * (iCellRowNum - 1)) + iCellHeight - iTotCellHeight;
	else
		iDiffY = iCellHeight - iTotCellHeight;

	if (iDiffX < 0 || iDiffY < 0)
		return RESULT_BAD;

	if (iDiffX > 0)
	{
		if (iCellColNum > 1)
			iDiffX = static_cast<int>(((float)iDiffX / (iCellColNum - 1)) + 0.5);
		iCellWidth += iDiffX;
	}

	if (iDiffY > 0)
	{
		if (iCellRowNum > 1)
			iDiffY = static_cast<int>(((float)iDiffY / (iCellRowNum - 1)) + 0.5);
		iCellHeight += iDiffY;
	}

	// Set Cell Info
	m_stCellInfo.stBasic.cellNum.cx = iCellColNum * m_stFovInfo.stBasic.swathNum;
	m_stCellInfo.stBasic.cellNum.cy = iCellRowNum;
	m_stCellInfo.stBasic.cellSize.cx = iCellWidth;
	m_stCellInfo.stBasic.cellSize.cy = iCellHeight;
	m_stCellInfo.stBasic.cellOverlap.cx = iCellMinOverlapX;
	m_stCellInfo.stBasic.cellOverlap.cy = iCellMinOverlapY;

	double dTemp = 0.;
	Orient ori = Orient::Mir0Deg;
	CDataManager::GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, ori);

	CRotMirrArr rotMirrArr;
	rotMirrArr.InputData(0, 0, CDataManager::ReverseOrient(ori), m_pUserSetInfo->mirrorDir);

	CString str;
	for (int iSwath = 0; iSwath < static_cast<int>(m_stFovInfo.stBasic.swathNum); iSwath++)
	{
		RECTD	rcRectMM = m_stFovInfo.vcFovData[iSwath]->rcRectMM;
		RECT	rcRectPxl = m_stFovInfo.vcFovData[iSwath]->rcRectPxl;

		for (int iRow = 0; iRow < iCellRowNum; iRow++)
		{
			for (int iCol = 0; iCol < iCellColNum; iCol++)
			{
				FovData_t* pCell = new FovData_t;
				pCell->swathIdx = iSwath;

				pCell->fovIdx.x = iCol;
				pCell->fovIdx.y = iRow;

				pCell->rcRectPxl.left = rcRectPxl.left + iSwathMarginX + iCol * (iCellWidth - iCellMinOverlapX);
				pCell->rcRectPxl.right = pCell->rcRectPxl.left + iCellWidth - 1;
				pCell->rcRectPxl.top = rcRectPxl.top + iSwathMarginY + iRow * (iCellHeight - iCellMinOverlapY);
				pCell->rcRectPxl.bottom = pCell->rcRectPxl.top + iCellHeight - 1;

				pCell->rcRectMM.left = rcRectMM.left + (iSwathMarginX * dResolMM) + ((iCol * (iCellWidth - iCellMinOverlapX)) * dResolMM);
				pCell->rcRectMM.right = pCell->rcRectMM.left + (iCellWidth * dResolMM);
				pCell->rcRectMM.top = rcRectMM.top - (iSwathMarginY * dResolMM) - ((iRow * (iCellHeight - iCellMinOverlapY)) * dResolMM);
				pCell->rcRectMM.bottom = pCell->rcRectMM.top - (iCellHeight * dResolMM);

				if (fabs(pCell->rcRectMM.left) < EPSILON_DELTA1)	pCell->rcRectMM.left = 0;
				if (fabs(pCell->rcRectMM.right) < EPSILON_DELTA1)	pCell->rcRectMM.right = 0;
				if (fabs(pCell->rcRectMM.top) < EPSILON_DELTA1)		pCell->rcRectMM.top = 0;
				if (fabs(pCell->rcRectMM.bottom) < EPSILON_DELTA1)	pCell->rcRectMM.bottom = 0;

				rotMirrArr.FinalPoint(&pCell->rcDraw.left, &pCell->rcDraw.top, pCell->rcRectMM.left, pCell->rcRectMM.top);
				rotMirrArr.FinalPoint(&pCell->rcDraw.right, &pCell->rcDraw.bottom, pCell->rcRectMM.right, pCell->rcRectMM.bottom);

				if (m_pUserSetInfo->bMirror)
				{
					if (m_pUserSetInfo->dAngle == 0.0)
					{
						SWAP(pCell->rcDraw.left, pCell->rcDraw.right, dTemp);
						SWAP(pCell->rcDraw.top, pCell->rcDraw.bottom, dTemp);
					}
					else if (m_pUserSetInfo->dAngle == 90.0)
					{
						if (m_pUserSetInfo->mirrorDir != eDirectionType::eBoth)
						{
							pCell->rcDraw.left *= -1.0;
							pCell->rcDraw.right *= -1.0;

							SWAP(pCell->rcDraw.top, pCell->rcDraw.bottom, dTemp);
							pCell->rcDraw.top *= -1.0;
							pCell->rcDraw.bottom *= -1.0;
						}
					}
					else if (m_pUserSetInfo->dAngle == 270.0)
					{
						if (m_pUserSetInfo->mirrorDir != eDirectionType::eBoth)
						{
							SWAP(pCell->rcDraw.left, pCell->rcDraw.right, dTemp);
							pCell->rcDraw.left *= -1.0;
							pCell->rcDraw.right *= -1.0;

							pCell->rcDraw.top *= -1.0;
							pCell->rcDraw.bottom *= -1.0;
						}
					}
				}

				str.Format(_T("Cell Swath[%d] Col[%d] Row[%d] : (L,T,R,B) (%.2f,%.2f,%.2f,%.2f)\n"), iSwath, iCol, iRow, pCell->rcDraw.left, pCell->rcDraw.top, pCell->rcDraw.right, pCell->rcDraw.bottom);
				OutputDebugString(str);
				str.Format(_T("Cell Swath[%d] Col[%d] Row[%d] : (L,T,R,B) (%.2f,%.2f,%.2f,%.2f)\n"), iSwath, iCol, iRow, pCell->rcRectMM.left, pCell->rcRectMM.top, pCell->rcRectMM.right, pCell->rcRectMM.bottom);
				OutputDebugString(str);

				m_stCellInfo.vcCellData.emplace_back(pCell);
			}
		}
	}

	if (m_stCellInfo.vcCellData.size() != m_stCellInfo.stBasic.cellNum.cx * m_stCellInfo.stBasic.cellNum.cy)
		return RESULT_BAD;

	int iCellNum = static_cast<int>(m_stCellInfo.vcCellData.size());

	// Calculate Overlap between Cell	
	for (auto itCell : m_stCellInfo.vcCellData)
	{
		int iCell = itCell->swathIdx * (iCellColNum * iCellRowNum) + (iCellColNum * itCell->fovIdx.y) + itCell->fovIdx.x;

		int iLeft = -1;
		if (itCell->fovIdx.x == 0 && itCell->swathIdx > 0)
			iLeft = (itCell->swathIdx - 1) * (iCellColNum * iCellRowNum) + (iCellColNum * itCell->fovIdx.y) + iCellColNum - 1;
		else if (itCell->fovIdx.x > 0 && itCell->fovIdx.x < iCellColNum)
			iLeft = itCell->swathIdx * (iCellColNum * iCellRowNum) + (iCellColNum * itCell->fovIdx.y) + itCell->fovIdx.x - 1;

		int iRight = -1;
		if (itCell->fovIdx.x + 1 == iCellColNum)
			iRight = (itCell->swathIdx + 1) * (iCellColNum * iCellRowNum) + (iCellColNum * itCell->fovIdx.y);
		else if (itCell->fovIdx.x < iCellColNum)
			iRight = itCell->swathIdx * (iCellColNum * iCellRowNum) + (iCellColNum * itCell->fovIdx.y) + itCell->fovIdx.x + 1;

		int iTop = -1;
		if (itCell->fovIdx.y > 0 && itCell->fovIdx.y < iCellRowNum)
			iTop = itCell->swathIdx * (iCellColNum * iCellRowNum) + (iCellColNum * (itCell->fovIdx.y - 1)) + itCell->fovIdx.x;

		int iBottom = -1;
		if (itCell->fovIdx.y + 1 < iCellRowNum)
			iBottom = itCell->swathIdx * (iCellColNum * iCellRowNum) + (iCellColNum * (itCell->fovIdx.y + 1)) + itCell->fovIdx.x;

		if (iLeft > 0 && iLeft < iCellNum)
			m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.left = m_stCellInfo.vcCellData[iLeft]->rcRectPxl.right - m_stCellInfo.vcCellData[iCell]->rcRectPxl.left + 1;
		if (iTop > 0 && iTop < iCellNum)
			m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.top = m_stCellInfo.vcCellData[iTop]->rcRectPxl.bottom - m_stCellInfo.vcCellData[iCell]->rcRectPxl.top + 1;
		if (iRight > 0 && iRight < iCellNum)
			m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.right = m_stCellInfo.vcCellData[iCell]->rcRectPxl.right - m_stCellInfo.vcCellData[iRight]->rcRectPxl.left + 1;
		if (iBottom > 0 && iBottom < iCellNum)
			m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.bottom = m_stCellInfo.vcCellData[iCell]->rcRectPxl.bottom - m_stCellInfo.vcCellData[iBottom]->rcRectPxl.top + 1;

		if (m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.left < 0)		m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.left = 0;
		if (m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.top < 0)		m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.top = 0;
		if (m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.right < 0)		m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.right = 0;
		if (m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.bottom < 0)	m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.bottom = 0;

		str.Format(_T("%d,%d,%d,%d,%d\n"), iCell + 1, m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.left, m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.top,
			m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.right, m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.bottom);
		OutputDebugString(str);
	}

	return RESULT_GOOD;
}

UINT32 CAOI::EstimateUnitInfo_in_Cell()
{
	m_stCellInfo.vcUnit_in_Cell.clear();
	
	if (m_stCellInfo.vcCellData.size() <= 0)
		return RESULT_BAD;

	int iSwathNum = static_cast<int>(m_stFovInfo.stBasic.swathNum);
	int iCellCol = static_cast<int>(m_stCellInfo.stBasic.cellNum.cx) / iSwathNum;
	int iCellRow = static_cast<int>(m_stCellInfo.stBasic.cellNum.cy);
	if (iSwathNum <= 0 || iCellCol <= 0 || iCellRow <= 0)
		return RESULT_BAD;
	
	CString str;
	for (auto itCell : m_stCellInfo.vcCellData)
	{
		int iCell = itCell->swathIdx * (iCellCol * iCellRow) + (iCellCol * itCell->fovIdx.y) + itCell->fovIdx.x;

		RECTD rcCell	= itCell->rcRectMM;
		rcCell.NormalizeRectD();

		CRect rcCellMM;
		rcCellMM.left = (LONG)(RoundReal(itCell->rcRectMM.left));
		rcCellMM.top = (LONG)(RoundReal(itCell->rcRectMM.top));
		rcCellMM.right = (LONG)(RoundReal(itCell->rcRectMM.right));
		rcCellMM.bottom = (LONG)(RoundReal(itCell->rcRectMM.bottom));
		rcCellMM.NormalizeRect();

		std::vector<UnitInfo_t*> vcUnitInfo;

		CRect rcStripMM, rcIntersect;
		for (auto itStrip : m_stPanelInfo.vcStripInfo)
		{	
			rcStripMM.left = (LONG)(RoundReal(itStrip->rcProfile.left));
			rcStripMM.top = (LONG)(RoundReal(itStrip->rcProfile.top));
			rcStripMM.right = (LONG)(RoundReal(itStrip->rcProfile.right));
			rcStripMM.bottom = (LONG)(RoundReal(itStrip->rcProfile.bottom));
			rcStripMM.NormalizeRect();

			if (!rcIntersect.IntersectRect(rcStripMM, rcCellMM))
				continue;

			CRect rcBlockMM;
			for (auto itBlock : itStrip->vcBlockInfo)
			{	
				/*rcBlockMM.left	 = (LONG)(RoundReal(itBlock->rcRect_In_Panel.left));
				rcBlockMM.top	 = (LONG)(RoundReal(itBlock->rcRect_In_Panel.top));
				rcBlockMM.right  = (LONG)(RoundReal(itBlock->rcRect_In_Panel.right));
				rcBlockMM.bottom = (LONG)(RoundReal(itBlock->rcRect_In_Panel.bottom));
				rcBlockMM.NormalizeRect();

				if (!rcIntersect.IntersectRect(rcBlockMM, rcCellMM))
					continue;*/

				for (auto itUnit : itBlock->vcUnitInfo)
				{
					RECTD rcUnit = itUnit->rcProfile;
					rcUnit.NormalizeRectD();

 					if (rcCell.IsPtInRectD(rcUnit.left, rcUnit.top) ||
 						rcCell.IsPtInRectD(rcUnit.left, rcUnit.bottom) ||
 						rcCell.IsPtInRectD(rcUnit.right, rcUnit.top) ||
						rcCell.IsPtInRectD(rcUnit.right, rcUnit.bottom))
					{
						itUnit->iCellIdx = static_cast<UINT32>(iCell);
						vcUnitInfo.emplace_back(itUnit);
					}
				}
			}
		}

		m_stCellInfo.vcUnit_in_Cell.insert(std::make_pair(iCell, vcUnitInfo));

	}
	
	if (m_stCellInfo.vcUnit_in_Cell.size() != m_stCellInfo.vcCellData.size())
		return RESULT_BAD;
	
	return RESULT_GOOD;
}

UINT32 CAOI::EstimateStripInfo_in_Cell()
{
	m_stCellInfo.vcStrip_in_Cell.clear();

	if (m_stCellInfo.vcCellData.size() <= 0)
		return RESULT_BAD;

	int iSwathNum = static_cast<int>(m_stFovInfo.stBasic.swathNum);
	int iCellCol = static_cast<int>(m_stCellInfo.stBasic.cellNum.cx) / iSwathNum;
	int iCellRow = static_cast<int>(m_stCellInfo.stBasic.cellNum.cy);
	if (iSwathNum <= 0 || iCellCol <= 0 || iCellRow <= 0)
		return RESULT_BAD;

	CString str;

	for (auto itCell : m_stCellInfo.vcCellData)
	{
		int iCell = itCell->swathIdx * (iCellCol * iCellRow) + (iCellCol * itCell->fovIdx.y) + itCell->fovIdx.x;

		RECTD rcCell = itCell->rcRectMM;
		rcCell.NormalizeRectD();

		CRect rcCellMM;
		rcCellMM.left = (LONG)(RoundReal(itCell->rcRectMM.left));
		rcCellMM.top = (LONG)(RoundReal(itCell->rcRectMM.top));
		rcCellMM.right = (LONG)(RoundReal(itCell->rcRectMM.right));
		rcCellMM.bottom = (LONG)(RoundReal(itCell->rcRectMM.bottom));
		rcCellMM.NormalizeRect();

		std::vector<StripInfo_t*> vcStripInfo;

		CRect rcStripMM, rcIntersect;
		for (auto itStrip : m_stPanelInfo.vcStripInfo)
		{
			rcStripMM.left = (LONG)(RoundReal(itStrip->rcRect_In_Panel.left));
			rcStripMM.top = (LONG)(RoundReal(itStrip->rcRect_In_Panel.top));
			rcStripMM.right = (LONG)(RoundReal(itStrip->rcRect_In_Panel.right));
			rcStripMM.bottom = (LONG)(RoundReal(itStrip->rcRect_In_Panel.bottom));
			rcStripMM.NormalizeRect();

			if (!rcIntersect.IntersectRect(rcStripMM, rcCellMM))
				continue;

			vcStripInfo.emplace_back(itStrip);
		}

		m_stCellInfo.vcStrip_in_Cell.insert(std::make_pair(iCell, vcStripInfo));

	}

	if (m_stCellInfo.vcStrip_in_Cell.size() != m_stCellInfo.vcCellData.size())
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CAOI::SaveGrabInfo(INT32 iStepIdx, enum Orient eOrient)
{
	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	if (m_pJobData->m_Misc.GetModelName().GetLength() < 1 ||
		m_strModelPath.GetLength() < 1)
		return RESULT_BAD;

//	INT32 iStepIdx = m_pUserSetInfo->iStepIdx;
	if (iStepIdx < 0 || m_pSystemSpec->sysPath.strMasterPath.GetLength() < 1)
		return RESULT_BAD;

	CStep *pStep = m_pJobData->m_arrStep[iStepIdx];
	if (pStep == nullptr)
		return RESULT_BAD;

	int iRowStripNumInPanel = static_cast<int>(m_stPanelInfo.iRowStripNumInPanel);
	int iColStripNumInPanel = static_cast<int>(m_stPanelInfo.iColStripNumInPanel);

	if (iRowStripNumInPanel <= 0 || iColStripNumInPanel <= 0 ||
		m_stPanelInfo.vcStripInfo.size() != iRowStripNumInPanel * iColStripNumInPanel)
		return RESULT_BAD;

	int iRowBlockNumInStrip = static_cast<int>(m_stPanelInfo.vcStripInfo[0]->iRowBlockNumInStrip);
	int iColBlockNumInStrip = static_cast<int>(m_stPanelInfo.vcStripInfo[0]->iColBlockNumInStrip);

	if (iRowBlockNumInStrip <= 0 || iColBlockNumInStrip <= 0 ||
		m_stPanelInfo.vcStripInfo[0]->vcBlockInfo.size() != iRowBlockNumInStrip * iColBlockNumInStrip)
		return RESULT_BAD;

	CRotMirrArr RotMirrArr;
	RECTD tmpMinMax, tmpRect;
	tmpMinMax = pStep->m_Profile.m_MinMax;

	//0档 Reference
	RotMirrArr.InputData(0.0, 0.0, Orient::NoMir0Deg);
	RotMirrArr.FinalPoint(&tmpRect.left, &tmpRect.top, tmpMinMax.left, tmpMinMax.top);
	RotMirrArr.FinalPoint(&tmpRect.right, &tmpRect.bottom, tmpMinMax.right, tmpMinMax.bottom);
	RotMirrArr.ClearAll();

	if (fabs(tmpRect.left) < EPSILON_DELTA1)	tmpRect.left = 0;
	if (fabs(tmpRect.right) < EPSILON_DELTA1)	tmpRect.right = 0;
	if (fabs(tmpRect.top) < EPSILON_DELTA1)		tmpRect.top = 0;
	if (fabs(tmpRect.bottom) < EPSILON_DELTA1)	tmpRect.bottom = 0;

	CameraSpec_t *pSpec = nullptr;
	if (m_pSystemSpec->sysBasic.ScanType == eScanType::eLineScan)
		pSpec = &m_pSystemSpec->sysCamera[true];
	else
		pSpec = &m_pSystemSpec->sysCamera[false];

	if (pSpec == nullptr) return RESULT_BAD;

	int iSwathNum = static_cast<int>(m_stFovInfo.stBasic.swathNum);
	int iCellCol = static_cast<int>(m_stCellInfo.stBasic.cellNum.cx) / iSwathNum;
	int iCellRow = static_cast<int>(m_stCellInfo.stBasic.cellNum.cy);
	int iSwathNumInBlock = static_cast<int>(m_stFovInfo.stBasic.swathNumInBlock);
	int iCellColNumInBlock = static_cast<int>(pSpec->iCellColNum);
	int iCellRowNumInBlock = static_cast<int>(pSpec->iCellRowNum);
	if (iSwathNum <= 0 || iCellCol <= 0 || iCellRow <= 0 || 
		iSwathNumInBlock <= 0 || iCellRowNumInBlock <= 0 ||
		iCellColNumInBlock <= 0)
		return RESULT_BAD;

	if (m_stCellInfo.vcCellData.size() <= 0 || m_stCellInfo.vcUnit_in_Cell.size() <= 0)
		return RESULT_BAD;
	
	int iIPUNum = static_cast<int>(m_pSystemSpec->sysBasic.iIPUNum);
	if (iIPUNum <= 0) iIPUNum = 3;

	CString strWorkStep = m_pJobData->m_arrStep[iStepIdx]->m_strStepName;
	CString strWorkLayer = m_pUserSetInfo->strWorkLayer;

	double dResolMM = pSpec->dResolution / 1000.f;

	CString strFileName = L"";
	strFileName.Format(L"%s\\%s", m_strModelPath, GRABINSP_INFO_FILE);

	CFile clResultFile;
	if (!clResultFile.Open(strFileName, CFile::modeCreate | CFile::modeReadWrite))
		return RESULT_BAD;

	CHAR *pcFiledata = nullptr;
	CString strValue = L"", strLog = L"";

	strValue.Format(_T("<?xml version=\"1.0\" encoding=\"euc-kr\" standalone=\"yes\"?>\r\n\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	strValue.Format(_T("<data>\r\n\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	CTime TM = CTime::GetCurrentTime();
	strValue.Format(_T("<note date=\"%04d/%02d/%02d\" time=\"%02d:%02d:%02d\"></note>\r\n\r\n"),
		TM.GetYear(), TM.GetMonth(), TM.GetDay(), TM.GetHour(), TM.GetMinute(), TM.GetSecond());
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	//[MASTER]
	strValue.Format(_T("<master rotation=\"%.1f\" mirror=\"%d\" except_dummy=\"%d\" image_type=\"%s\" data_level=\"%d\" resolution=\"%.1f\" frame_width=\"%d\" frame_height=\"%d\" dummy_marginx=\"%.6f\" dummy_marginy=\"%.6f\" cell_col_count=\"%d\" cell_row_count=\"%d\" cell_overlapx=\"%d\" cell_overlapy=\"%d\" ></master>\r\n"),
		m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, m_pSystemSpec->sysBasic.bExceptDummy, m_pSystemSpec->sysBasic.strImageType, (int)m_pSystemSpec->sysBasic.MasterLevel,
		pSpec->dResolution, pSpec->iFrameWidth, pSpec->iFrameHeight, pSpec->dDummyMarginX, pSpec->dDummyMarginY, pSpec->iCellColNum, pSpec->iCellRowNum,
		pSpec->iCellMinOverlapX, pSpec->iCellMinOverlapY);

	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	bool bIsBlockMode = false;

	//[INFO]	
	if (m_pSystemSpec->sysBasic.bExceptDummy)
	{
//		double dStartOffX = fabs(m_stFovInfo.vcFovData[0]->rcRectMM.left - m_stPanelInfo.rcRect.left);
//		double dStartOffY = fabs(m_stFovInfo.vcFovData[0]->rcRectMM.top - m_stPanelInfo.rcRect.top);

		double dStartOffX = fabs(fabs(m_stFovInfo.vcFovData[0]->rcRectMM.left) - tmpRect.left);
		double dStartOffY = fabs(fabs(m_stFovInfo.vcFovData[0]->rcRectMM.top) - tmpRect.top);

		strValue.Format(_T("<info model=\"%s\" resolution=\"%.1f\" grab_width=\"%d\" grab_height=\"%d\" start_offsetx=\"%d\" start_offsety=\"%d\" ipu_count=\"%d\" total_cell_col_count=\"%d\" total_cell_row_count=\"%d\" unit=\"%s\"></info>\r\n"),
			m_pJobData->m_Misc.GetModelName(), m_stFovInfo.stBasic.resolution, m_stFovInfo.stBasic.prodSize.cx,
			m_stFovInfo.stBasic.prodSize.cy, (LONG)(dStartOffX / dResolMM + 0.5), (LONG)(dStartOffY / dResolMM + 0.5), iIPUNum,
			m_stCellInfo.stBasic.cellNum.cx, m_stCellInfo.stBasic.cellNum.cy, _T("pixel"));

		if (m_pSystemSpec->sysBasic.MasterLevel == eMasterLevel::eBlockLv)
			bIsBlockMode = true;
	}
	else
	{
		int iMinMarginX = static_cast<int>(pSpec->iMinMarginX);
		int iMinMarginY = static_cast<int>(pSpec->iMinMarginY);

		strValue.Format(_T("<info model=\"%s\" resolution=\"%.1f\" grab_width=\"%d\" grab_height=\"%d\" start_offsetx=\"%d\" start_offsety=\"%d\" ipu_count=\"%d\" total_cell_col_count=\"%d\" total_cell_row_count=\"%d\" unit=\"%s\"></info>\r\n"),
			m_pJobData->m_Misc.GetModelName(), m_stFovInfo.stBasic.resolution, m_stFovInfo.stBasic.prodSize.cx,
			m_stFovInfo.stBasic.prodSize.cy, -iMinMarginX, -iMinMarginY, iIPUNum,
			m_stCellInfo.stBasic.cellNum.cx, m_stCellInfo.stBasic.cellNum.cy, _T("pixel"));
	}

	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

/*	//[AlignMark]
	strValue.Format(_T("<alignmark count=\"%d\">\r\n"), m_stPanelInfo.vcAlignMarkInfo.size());
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	double dTmp = 0;
	std::vector<RECTD> vcAlignInfo;
	for (auto itMark : m_stPanelInfo.vcAlignMarkInfo)
	{
		RECTD rcAlign = *itMark;

		if (m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal)
		{
			rcAlign.left = fabs(rcAlign.left - m_stPanelInfo.rcRect.right);
			rcAlign.right = fabs(rcAlign.right - m_stPanelInfo.rcRect.right);
		}
		else
		{
			rcAlign.left = fabs(rcAlign.left - m_stPanelInfo.rcRect.left);
			rcAlign.right = fabs(rcAlign.right - m_stPanelInfo.rcRect.left);
		}

		if (m_pUserSetInfo->mirrorDir == eDirectionType::eVertical)
		{
			rcAlign.top = fabs(rcAlign.top - m_stPanelInfo.rcRect.top);
			rcAlign.bottom = fabs(rcAlign.bottom - m_stPanelInfo.rcRect.top);
		}
		else
		{
			rcAlign.top = fabs(rcAlign.top - m_stPanelInfo.rcRect.bottom);
			rcAlign.bottom = fabs(rcAlign.bottom - m_stPanelInfo.rcRect.bottom);
		}

		if (rcAlign.right < rcAlign.left)	SWAP(rcAlign.left, rcAlign.right, dTmp);
		if (rcAlign.top < rcAlign.bottom)	SWAP(rcAlign.top, rcAlign.bottom, dTmp);

		vcAlignInfo.emplace_back(rcAlign);
	}

	std::sort(vcAlignInfo.begin(), vcAlignInfo.end(), tagDoubleRect::comp_var);

	UINT32 iMarkIdx = 1;
	for (auto itMark : vcAlignInfo)
	{
		RECTD rcAlign = itMark;
		rcAlign.left = fabs(rcAlign.left - m_stPanelInfo.rcRect.left) / dResolMM;
		rcAlign.right = fabs(rcAlign.right - m_stPanelInfo.rcRect.left) / dResolMM;
		rcAlign.top = fabs(rcAlign.top - m_stPanelInfo.rcRect.top) / dResolMM;
		rcAlign.bottom = fabs(rcAlign.bottom - m_stPanelInfo.rcRect.top) / dResolMM;

		double dStartOffX = -static_cast<double>(pSpec->iMinMarginX);
		double dStartOffY = -static_cast<double>(pSpec->iMinMarginY);

		if (m_pSystemSpec->sysBasic.bExceptDummy)
		{
			dStartOffX = fabs(m_stFovInfo.vcFovData[0]->rcRectMM.left - m_stPanelInfo.rcRect.left) / dResolMM;
			dStartOffY = fabs(m_stFovInfo.vcFovData[0]->rcRectMM.top - m_stPanelInfo.rcRect.top) / dResolMM;
		}

		rcAlign.left	-= dStartOffX;
		rcAlign.right	-= dStartOffX;
		rcAlign.top		-= dStartOffY;
		rcAlign.bottom	-= dStartOffY;

		strValue.Format(_T("<m%d left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\" centerx=\"%.3f\" centery=\"%.3f\"></m%d>\r\n"),
			iMarkIdx, rcAlign.left, rcAlign.top, rcAlign.right, rcAlign.bottom,
			(rcAlign.left + rcAlign.right) / 2.0, (rcAlign.top + rcAlign.bottom) / 2.0, iMarkIdx);

		pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
		clResultFile.Write(pcFiledata, strValue.GetLength());
		free(pcFiledata);

		iMarkIdx++;
	}
	vcAlignInfo.clear();

	strValue.Format(_T("</alignmark>\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);
*/	
	//[SWATH_INFO]
	strValue.Format(_T("<swath_info width=\"%d\" height=\"%d\" overlapx=\"%d\" overlapy=\"%d\" marginx=\"%d\" marginy=\"%d\" cell_col_count=\"%d\" cell_row_count=\"%d\"></swath_info>\r\n"),
		m_stFovInfo.stBasic.fovSize.cx, m_stFovInfo.stBasic.fovSize.cy, m_stFovInfo.stBasic.fovOverlap.cx, m_stFovInfo.stBasic.fovOverlap.cy, 
		m_stFovInfo.stBasic.fovMargin.cx, m_stFovInfo.stBasic.fovMargin.cy, iCellCol, iCellRow);

	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	//[CELL_INFO]
	strValue.Format(_T("<cell_info width=\"%d\" height=\"%d\" overlapx=\"%d\" overlapy=\"%d\"></cell_info>\r\n\r\n"),
		m_stCellInfo.stBasic.cellSize.cx, m_stCellInfo.stBasic.cellSize.cy, m_stCellInfo.stBasic.cellOverlap.cx, m_stCellInfo.stBasic.cellOverlap.cy);

	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	//[BLOCK_INFO]
	m_stCellInfo.stBasic.cell_In_Block.cx = iCellColNumInBlock * iSwathNumInBlock;
	m_stCellInfo.stBasic.cell_In_Block.cy = iCellRowNumInBlock / (iRowStripNumInPanel * iRowBlockNumInStrip);

	if (bIsBlockMode)
		m_stCellInfo.stBasic.cell_In_Block.cy = iCellRowNumInBlock;

	strValue.Format(_T("<block_info swath_count=\"%d\" cell_col_count=\"%d\" cell_row_count=\"%d\"></block_info>\r\n\r\n"),
		iSwathNumInBlock, m_stCellInfo.stBasic.cell_In_Block.cx, m_stCellInfo.stBasic.cell_In_Block.cy);

	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	//[SWATH]
	strValue.Format(_T("<swath count=\"%d\">\r\n"), m_stFovInfo.vcFovData.size());
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	UINT32 iOffsetX = m_stFovInfo.vcFovData[0]->rcRectPxl.left;
	UINT32 iOffsetY = m_stFovInfo.vcFovData[0]->rcRectPxl.top;

	UINT32 iIdx = 1;
	int iTotCellIdx = 1;
	for (auto itSwath : m_stFovInfo.vcFovData)
	{
		strValue.Format(_T("<s%d insp=\"%d\" index=\"%d\" left=\"%d\" top=\"%d\" right=\"%d\" bottom=\"%d\" overlap_left=\"%d\" overlap_top=\"%d\" overlap_right=\"%d\" overlap_bottom=\"%d\" cell_col_count=\"%d\" cell_row_count=\"%d\">\r\n"),
			iIdx, 1, itSwath->fovIdx.x + 1, itSwath->rcRectPxl.left - iOffsetX, itSwath->rcRectPxl.top - iOffsetY, itSwath->rcRectPxl.right - iOffsetX, itSwath->rcRectPxl.bottom - iOffsetY,
			itSwath->rcOverlapPxl.left, itSwath->rcOverlapPxl.top, itSwath->rcOverlapPxl.right, itSwath->rcOverlapPxl.bottom, iCellCol, iCellRow);

		pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
		clResultFile.Write(pcFiledata, strValue.GetLength());
		free(pcFiledata);

		int iCellIdx = 1;

		//[CELL]
		CRect rcCell;
//		double dTmpx, dTmpy;
		for (auto itCell : m_stCellInfo.vcCellData)
		{
			if (itSwath->fovIdx.x != itCell->swathIdx)
				continue;

			rcCell.left		= itCell->rcRectPxl.left - iOffsetX;
			rcCell.top		= itCell->rcRectPxl.top - iOffsetY;
			rcCell.right	= itCell->rcRectPxl.right - iOffsetX;
			rcCell.bottom	= itCell->rcRectPxl.bottom - iOffsetY;

			int iCell = itCell->swathIdx * (iCellCol * iCellRow) + (iCellCol * itCell->fovIdx.y) + itCell->fovIdx.x;
			auto itUnit_in_Cell = m_stCellInfo.vcUnit_in_Cell.find(iCell);
			if (itUnit_in_Cell == m_stCellInfo.vcUnit_in_Cell.end())
				continue;

			int iIPUIdx = iTotCellIdx % iIPUNum;

			if (bIsBlockMode)
			{
				strValue.Format(_T("<c%d insp=\"%d\" ipu_index=\"%d\" swath_index=\"%d\" index_x=\"%d\" index_y=\"%d\" left=\"%d\" top=\"%d\" right=\"%d\" bottom=\"%d\" overlap_left=\"%d\" overlap_top=\"%d\" overlap_right=\"%d\" overlap_bottom=\"%d\" unit_count=\"%d\">\r\n"),
					iCellIdx, 1, iIPUIdx + 1, itCell->swathIdx + 1,/*((itCell->swathIdx * iCellColNumInBlock) + itCell->fovIdx.x) % (iSwathNumInBlock * iCellColNumInBlock) + 1,*/ itCell->fovIdx.x + 1,
					(itCell->fovIdx.y % iCellRowNumInBlock ) + 1, rcCell.left, rcCell.top, rcCell.right, rcCell.bottom,
					itCell->rcOverlapPxl.left, itCell->rcOverlapPxl.top, itCell->rcOverlapPxl.right, itCell->rcOverlapPxl.bottom,
					itUnit_in_Cell->second.size());
			}
			else
			{
				strValue.Format(_T("<c%d insp=\"%d\" ipu_index=\"%d\" swath_index=\"%d\" index_x=\"%d\" index_y=\"%d\" left=\"%d\" top=\"%d\" right=\"%d\" bottom=\"%d\" overlap_left=\"%d\" overlap_top=\"%d\" overlap_right=\"%d\" overlap_bottom=\"%d\" unit_count=\"%d\">\r\n"),
					iCellIdx, 1, iIPUIdx + 1, itCell->swathIdx + 1, itCell->fovIdx.x + 1, itCell->fovIdx.y + 1, rcCell.left, rcCell.top, rcCell.right, rcCell.bottom,
					itCell->rcOverlapPxl.left, itCell->rcOverlapPxl.top, itCell->rcOverlapPxl.right, itCell->rcOverlapPxl.bottom,
					itUnit_in_Cell->second.size());
			}

			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);

			RECTD rcCellMM = itCell->rcRectMM;			

			/*if (m_pSystemSpec->sysBasic.bExceptDummy)
			{
				rcCellMM.left = fabs(rcCellMM.left - m_stFovInfo.vcFovData[itCell->swathIdx]->rcRectMM.left);
				rcCellMM.right = fabs(rcCellMM.right - m_stFovInfo.vcFovData[itCell->swathIdx]->rcRectMM.left);
				rcCellMM.top = fabs(rcCellMM.top - m_stFovInfo.vcFovData[itCell->swathIdx]->rcRectMM.top);
				rcCellMM.bottom = fabs(rcCellMM.bottom - m_stFovInfo.vcFovData[itCell->swathIdx]->rcRectMM.top);
			}
			else
			{
				rcCellMM.left = fabs(rcCellMM.left - m_stPanelInfo.rcRect.left);
				rcCellMM.right = fabs(rcCellMM.right - m_stPanelInfo.rcRect.left);
				rcCellMM.top = fabs(rcCellMM.top - m_stPanelInfo.rcRect.bottom);
				rcCellMM.bottom = fabs(rcCellMM.bottom - m_stPanelInfo.rcRect.bottom);
			}
			
			if (rcCellMM.right < rcCellMM.left)	SWAP(rcCellMM.left, rcCellMM.right, dTmpx);
			if (rcCellMM.top < rcCellMM.bottom)	SWAP(rcCellMM.top, rcCellMM.bottom, dTmpy);
			*/
			//[UNIT]
			UINT32 iUnitIdx = 1;
			CRect rcUnit;
			for (auto itUnit : itUnit_in_Cell->second)
			{
				RECTD rcUnitRect = itUnit->rcRect;
				if (m_pSystemSpec->sysBasic.bProfileMode)
					rcUnitRect = itUnit->rcProfile;
				
//				if (fabs(rcUnitRect.right) > fabs(rcUnitRect.left))
//					rcUnit.left	= (LONG)((rcUnitRect.left - itCell->rcRectMM.left) / dResolMM + 0.5);
//				else
//					rcUnit.left = (LONG)((rcUnitRect.left - itCell->rcRectMM.right) / dResolMM + 0.5);

				rcUnit.left		= (LONG)((rcUnitRect.left - rcCellMM.left) / dResolMM + 0.5);
				rcUnit.right	= (LONG)(rcUnit.left + m_stPanelInfo.vcUnitType[itUnit->iType]->szFeatureSize.cx - 1);
			
//				if (fabs(rcUnitRect.top) > fabs(rcUnitRect.bottom))
//					rcUnit.top = (LONG)((itCell->rcRectMM.top - rcUnitRect.top) / dResolMM + 0.5);
//				else
//					rcUnit.top = (LONG)((itCell->rcRectMM.top - rcUnitRect.bottom) / dResolMM + 0.5);			

				rcUnit.top		= (LONG)((rcCellMM.top - rcUnitRect.top) / dResolMM + 0.5);
				rcUnit.bottom	= (LONG)(rcUnit.top + m_stPanelInfo.vcUnitType[itUnit->iType]->szFeatureSize.cy - 1);

				if (rcUnit.bottom - rcUnit.top >= m_stPanelInfo.vcUnitType[itUnit->iType]->szFeatureSize.cy)
					rcUnit.bottom = rcUnit.top + m_stPanelInfo.vcUnitType[itUnit->iType]->szFeatureSize.cy - 1;				

				strValue.Format(_T("<u%d insp=\"%d\" index=\"%d\" left=\"%d\" top=\"%d\" right=\"%d\" bottom=\"%d\" stripidx=\"%d\" blockidx=\"%d\"></u%d>\r\n"),
					iUnitIdx, 1, itUnit->iUnitIdx_Ori /*+ 1*//*iUnitIdx*/, rcUnit.left, rcUnit.top, rcUnit.right, rcUnit.bottom, itUnit->iStripIdx + 1, itUnit->iBlockIdx + 1, iUnitIdx);

				pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
				clResultFile.Write(pcFiledata, strValue.GetLength());
				free(pcFiledata);
				iUnitIdx++;

				strLog.Format(L"%d,%d,%d,%d\n", rcUnit.left, rcUnit.top, rcUnit.right, rcUnit.bottom);
				OutputDebugString(strLog);
			}

			strValue.Format(_T("</c%d>\r\n"), iCellIdx);
			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);
			iCellIdx++;
			iTotCellIdx++;
		}

		strValue.Format(_T("</s%d>\r\n"), iIdx);
		pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
		clResultFile.Write(pcFiledata, strValue.GetLength());
		free(pcFiledata);
		iIdx++;
	}

	strValue.Format(_T("</swath>\r\n\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	strValue.Format(_T("</data>\r\n\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);
	clResultFile.Close();

	return RESULT_GOOD;
}

UINT32 CAOI::EstimateBlockInfo()
{
	double extend = 0;
	for (auto itStrip : m_stPanelInfo.vcStripInfo)
	{
		for (auto itBlock : itStrip->vcBlockInfo)
		{
			RECTD rcBlockMinMax = RECTD(LONG_MAX, 0, 0, LONG_MAX);
			for (auto itUnit : itBlock->vcUnitInfo)
			{				
				RECTD rcUnit_In_Panel = itUnit->rcRect_In_Panel;
				if (fabs(rcUnit_In_Panel.left) > fabs(rcUnit_In_Panel.right)) SWAP(rcUnit_In_Panel.left, rcUnit_In_Panel.right, extend);
				if (fabs(rcUnit_In_Panel.bottom) > fabs(rcUnit_In_Panel.top)) SWAP(rcUnit_In_Panel.bottom, rcUnit_In_Panel.top, extend);

				if (fabs(rcBlockMinMax.left) > fabs(rcUnit_In_Panel.left))		rcBlockMinMax.left = rcUnit_In_Panel.left;
				if (fabs(rcBlockMinMax.right) < fabs(rcUnit_In_Panel.right))	rcBlockMinMax.right = rcUnit_In_Panel.right;
				if (fabs(rcBlockMinMax.bottom) > fabs(rcUnit_In_Panel.bottom))	rcBlockMinMax.bottom = rcUnit_In_Panel.bottom;
				if (fabs(rcBlockMinMax.top) < fabs(rcUnit_In_Panel.top))		rcBlockMinMax.top = rcUnit_In_Panel.top;
			}

			if (rcBlockMinMax.left > rcBlockMinMax.right)	SWAP(rcBlockMinMax.left, rcBlockMinMax.right, extend);
			if (rcBlockMinMax.bottom > rcBlockMinMax.top)	SWAP(rcBlockMinMax.bottom, rcBlockMinMax.top, extend);

			itBlock->rcRect_In_Panel = rcBlockMinMax;
		}
	}
	return RESULT_GOOD;
}

UINT32 CAOI::SaveCellInfo(INT32 iStepIdx, enum Orient eOrient)
{
	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	if (m_pJobData->m_Misc.GetModelName().GetLength() < 1 ||
		m_strModelPath.GetLength() < 1)
		return RESULT_BAD;

//	INT32 iStepIdx = m_pUserSetInfo->iStepIdx;
	if (iStepIdx < 0 || m_pSystemSpec->sysPath.strMasterPath.GetLength() < 1)
		return RESULT_BAD;

	CStep *pStep = m_pJobData->m_arrStep[iStepIdx];
	if (pStep == nullptr)
		return RESULT_BAD;

	CRotMirrArr RotMirrArr;
	RECTD tmpMinMax, tmpRect;
	tmpMinMax = pStep->m_Profile.m_MinMax;

	//0档 Reference
	RotMirrArr.InputData(0.0, 0.0, Orient::NoMir0Deg);
	RotMirrArr.FinalPoint(&tmpRect.left, &tmpRect.top, tmpMinMax.left, tmpMinMax.top);
	RotMirrArr.FinalPoint(&tmpRect.right, &tmpRect.bottom, tmpMinMax.right, tmpMinMax.bottom);
	RotMirrArr.ClearAll();

	if (fabs(tmpRect.left) < EPSILON_DELTA1)	tmpRect.left = 0;
	if (fabs(tmpRect.right) < EPSILON_DELTA1)	tmpRect.right = 0;
	if (fabs(tmpRect.top) < EPSILON_DELTA1)		tmpRect.top = 0;
	if (fabs(tmpRect.bottom) < EPSILON_DELTA1)	tmpRect.bottom = 0;

	CameraSpec_t *pSpec = nullptr;
	if (m_pSystemSpec->sysBasic.ScanType == eScanType::eLineScan)
		pSpec = &m_pSystemSpec->sysCamera[true];
	else
		pSpec = &m_pSystemSpec->sysCamera[false];

	if (pSpec == nullptr) return RESULT_BAD;

	int iSwathNum = static_cast<int>(m_stFovInfo.stBasic.swathNum);
	int iCellCol = static_cast<int>(m_stCellInfo.stBasic.cellNum.cx) / iSwathNum;
	int iCellRow = static_cast<int>(m_stCellInfo.stBasic.cellNum.cy);
	int iSwathNumInBlock = static_cast<int>(m_stFovInfo.stBasic.swathNumInBlock);
	int iCellColNumInBlock = static_cast<int>(pSpec->iCellColNum);
	int iCellRowNumInBlock = static_cast<int>(pSpec->iCellRowNum);
	if (iSwathNum <= 0 || iCellCol <= 0 || iCellRow <= 0 ||
		iSwathNumInBlock <= 0 || iCellRowNumInBlock <= 0 ||
		iCellColNumInBlock <= 0)
		return RESULT_BAD;

	if (m_stCellInfo.vcCellData.size() <= 0 || m_stCellInfo.vcUnit_in_Cell.size() <= 0)
		return RESULT_BAD;

	int iIPUNum = static_cast<int>(m_pSystemSpec->sysBasic.iIPUNum);
	if (iIPUNum <= 0) iIPUNum = 3;

	CString strWorkStep = m_pJobData->m_arrStep[iStepIdx]->m_strStepName;
	CString strWorkLayer = m_pUserSetInfo->strWorkLayer;

	double dResolMM = pSpec->dResolution / 1000.f;

	CString strFileName = L"";
	strFileName.Format(L"%s\\%s", m_strModelPath, CELL_INFO_FILE);

	CFile clResultFile;
	if (!clResultFile.Open(strFileName, CFile::modeCreate | CFile::modeReadWrite))
		return RESULT_BAD;

	CHAR *pcFiledata = nullptr;
	CString strValue = L"", strLog = L"";

	strValue.Format(_T("<?xml version=\"1.0\" encoding=\"euc-kr\" standalone=\"yes\"?>\r\n\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	strValue.Format(_T("<data>\r\n\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	CTime TM = CTime::GetCurrentTime();
	strValue.Format(_T("<note date=\"%04d/%02d/%02d\" time=\"%02d:%02d:%02d\"></note>\r\n\r\n"),
		TM.GetYear(), TM.GetMonth(), TM.GetDay(), TM.GetHour(), TM.GetMinute(), TM.GetSecond());
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	bool bIsBlockMode = false;

	//[INFO]	
	if (m_pSystemSpec->sysBasic.bExceptDummy)
	{
//		double dStartOffX = fabs(m_stFovInfo.vcFovData[0]->rcRectMM.left - m_stPanelInfo.rcRect.left);
//		double dStartOffY = fabs(m_stFovInfo.vcFovData[0]->rcRectMM.top - m_stPanelInfo.rcRect.top);

		double dStartOffX = fabs(fabs(m_stFovInfo.vcFovData[0]->rcRectMM.left) - tmpRect.left);
		double dStartOffY = fabs(fabs(m_stFovInfo.vcFovData[0]->rcRectMM.top) - tmpRect.top);

		strValue.Format(_T("<info model=\"%s\" resolution=\"%.1f\" grab_width=\"%d\" grab_height=\"%d\" start_offsetx=\"%d\" start_offsety=\"%d\" ipu_count=\"%d\" total_cell_col_count=\"%d\" total_cell_row_count=\"%d\" unit=\"%s\"></info>\r\n"),
			m_pJobData->m_Misc.GetModelName(), m_stFovInfo.stBasic.resolution, m_stFovInfo.stBasic.prodSize.cx,
			m_stFovInfo.stBasic.prodSize.cy, (LONG)(dStartOffX / dResolMM + 0.5), (LONG)(dStartOffY / dResolMM + 0.5), iIPUNum,
			m_stCellInfo.stBasic.cellNum.cx, m_stCellInfo.stBasic.cellNum.cy, _T("pixel"));

		if (m_pSystemSpec->sysBasic.MasterLevel == eMasterLevel::eBlockLv)
			bIsBlockMode = true;
	}
	else
	{
		int iMinMarginX = static_cast<int>(pSpec->iMinMarginX);
		int iMinMarginY = static_cast<int>(pSpec->iMinMarginY);

		strValue.Format(_T("<info model=\"%s\" resolution=\"%.1f\" grab_width=\"%d\" grab_height=\"%d\" start_offsetx=\"%d\" start_offsety=\"%d\" ipu_count=\"%d\" total_cell_col_count=\"%d\" total_cell_row_count=\"%d\" unit=\"%s\"></info>\r\n"),
			m_pJobData->m_Misc.GetModelName(), m_stFovInfo.stBasic.resolution, m_stFovInfo.stBasic.prodSize.cx,
			m_stFovInfo.stBasic.prodSize.cy, -iMinMarginX, -iMinMarginY, iIPUNum,
			m_stCellInfo.stBasic.cellNum.cx, m_stCellInfo.stBasic.cellNum.cy, _T("pixel"));
	}

	//[SWATH_INFO]
	strValue.Format(_T("<swath_info width=\"%d\" height=\"%d\" overlapx=\"%d\" overlapy=\"%d\" marginx=\"%d\" marginy=\"%d\" cell_col_count=\"%d\" cell_row_count=\"%d\"></swath_info>\r\n"),
		m_stFovInfo.stBasic.fovSize.cx, m_stFovInfo.stBasic.fovSize.cy, m_stFovInfo.stBasic.fovOverlap.cx, m_stFovInfo.stBasic.fovOverlap.cy,
		m_stFovInfo.stBasic.fovMargin.cx, m_stFovInfo.stBasic.fovMargin.cy, iCellCol, iCellRow);

	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	//[CELL_INFO]
	strValue.Format(_T("<cell_info width=\"%d\" height=\"%d\" overlapx=\"%d\" overlapy=\"%d\"></cell_info>\r\n\r\n"),
		m_stCellInfo.stBasic.cellSize.cx, m_stCellInfo.stBasic.cellSize.cy, m_stCellInfo.stBasic.cellOverlap.cx, m_stCellInfo.stBasic.cellOverlap.cy);

	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	//[BLOCK_INFO]
	strValue.Format(_T("<block_info swath_count=\"%d\" cell_col_count=\"%d\" cell_row_count=\"%d\"></block_info>\r\n\r\n"),
		iSwathNumInBlock, m_stCellInfo.stBasic.cell_In_Block.cx, m_stCellInfo.stBasic.cell_In_Block.cy);	

	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);	

	//[SWATH]
	strValue.Format(_T("<swath count=\"%d\">\r\n"), m_stFovInfo.vcFovData.size());
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	UINT32 iOffsetX = m_stFovInfo.vcFovData[0]->rcRectPxl.left;
	UINT32 iOffsetY = m_stFovInfo.vcFovData[0]->rcRectPxl.top;

	UINT32 iIdx = 1;
	int iTotCellIdx = 1;
	for (auto itSwath : m_stFovInfo.vcFovData)
	{
		strValue.Format(_T("<s%d insp=\"%d\" index=\"%d\" left=\"%d\" top=\"%d\" right=\"%d\" bottom=\"%d\" overlap_left=\"%d\" overlap_top=\"%d\" overlap_right=\"%d\" overlap_bottom=\"%d\" cell_col_count=\"%d\" cell_row_count=\"%d\">\r\n"),
			iIdx, 1, itSwath->fovIdx.x + 1, itSwath->rcRectPxl.left - iOffsetX, itSwath->rcRectPxl.top - iOffsetY, itSwath->rcRectPxl.right - iOffsetX, itSwath->rcRectPxl.bottom - iOffsetY,
			itSwath->rcOverlapPxl.left, itSwath->rcOverlapPxl.top, itSwath->rcOverlapPxl.right, itSwath->rcOverlapPxl.bottom, iCellCol, iCellRow);

		pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
		clResultFile.Write(pcFiledata, strValue.GetLength());
		free(pcFiledata);

		int iCellIdx = 1;

		//[CELL]
		CRect rcCell;
		for (auto itCell : m_stCellInfo.vcCellData)
		{
			if (itSwath->fovIdx.x != itCell->swathIdx)
				continue;

			rcCell.left = itCell->rcRectPxl.left - iOffsetX;
			rcCell.top = itCell->rcRectPxl.top - iOffsetY;
			rcCell.right = itCell->rcRectPxl.right - iOffsetX;
			rcCell.bottom = itCell->rcRectPxl.bottom - iOffsetY;

			int iCell = itCell->swathIdx * (iCellCol * iCellRow) + (iCellCol * itCell->fovIdx.y) + itCell->fovIdx.x;
			int iIPUIdx = iTotCellIdx % iIPUNum;

			int nAlignCount_in_Cell = 0;
			int nMaskCount_in_Cell = 0;

			auto itAlign_in_Cell = m_stCellInfo.vcAlign_in_Cell.find(iCell);
			auto itMask_in_Cell = m_stCellInfo.vcMask_in_Cell.find(iCell);

			if (itAlign_in_Cell != m_stCellInfo.vcAlign_in_Cell.end())
			{
				vector<CellAlignInfo_t> vecAlignInfo = itAlign_in_Cell->second;
				int nAlignCount = static_cast<int>(vecAlignInfo.size());
				for (int nAlign = 0; nAlign < nAlignCount; nAlign++)
				{
					nAlignCount_in_Cell += static_cast<int>(vecAlignInfo[nAlign].vcInCell_Pixel.size());
				}
			}

			if (itMask_in_Cell != m_stCellInfo.vcMask_in_Cell.end())
			{
				vector<CellMaskInfo_t> vecMaskInfo = itMask_in_Cell->second;
				int nMaskCount = static_cast<int>(vecMaskInfo.size());
				for (int nMask = 0; nMask < nMaskCount; nMask++)
				{
					nMaskCount_in_Cell += static_cast<int>(vecMaskInfo[nMask].vcInCell_Pixel.size());
				}
			}

			if (bIsBlockMode)
			{
				strValue.Format(_T("<c%d insp=\"%d\" ipu_index=\"%d\" swath_index=\"%d\" index_x=\"%d\" index_y=\"%d\" left=\"%d\" top=\"%d\" right=\"%d\" bottom=\"%d\" overlap_left=\"%d\" overlap_top=\"%d\" overlap_right=\"%d\" overlap_bottom=\"%d\" align_count=\"%d\" mask_count=\"%d\">\r\n"),
					iCellIdx, 1, iIPUIdx + 1, itCell->swathIdx + 1, /*((itCell->swathIdx * iCellColNumInBlock) + itCell->fovIdx.x) % (iSwathNumInBlock * iCellColNumInBlock) + 1,*/itCell->fovIdx.x + 1,
					(itCell->fovIdx.y % iCellRowNumInBlock) + 1, rcCell.left, rcCell.top, rcCell.right, rcCell.bottom,
					itCell->rcOverlapPxl.left, itCell->rcOverlapPxl.top, itCell->rcOverlapPxl.right, itCell->rcOverlapPxl.bottom,
					nAlignCount_in_Cell, nMaskCount_in_Cell);
			}
			else
			{
				strValue.Format(_T("<c%d insp=\"%d\" ipu_index=\"%d\" swath_index=\"%d\" index_x=\"%d\" index_y=\"%d\" left=\"%d\" top=\"%d\" right=\"%d\" bottom=\"%d\" overlap_left=\"%d\" overlap_top=\"%d\" overlap_right=\"%d\" overlap_bottom=\"%d\" align_count=\"%d\" mask_count=\"%d\">\r\n"),
					iCellIdx, 1, iIPUIdx + 1, itCell->swathIdx + 1, itCell->fovIdx.x + 1, itCell->fovIdx.y + 1, rcCell.left, rcCell.top, rcCell.right, rcCell.bottom,
					itCell->rcOverlapPxl.left, itCell->rcOverlapPxl.top, itCell->rcOverlapPxl.right, itCell->rcOverlapPxl.bottom,
					nAlignCount_in_Cell, nMaskCount_in_Cell);
			}

			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);

			//[ALIGN]
			if (nAlignCount_in_Cell > 0)
			{
				int iAlignIdx = 0;
				CRect rcAlign;
				vector<CellAlignInfo_t> vecAlignInfo = itAlign_in_Cell->second;
				int nCount = static_cast<int>(vecAlignInfo.size());
				for (int nAlign = 0; nAlign < nCount; nAlign++)
				{
					int nUnionCount = static_cast<int>(vecAlignInfo[nAlign].vcInCell_Pixel.size());
					for (int nUnion = 0; nUnion < nUnionCount; nUnion++)
					{
						rcAlign = vecAlignInfo[nAlign].vcInCell_Pixel[nUnion];
						rcAlign.NormalizeRect();

						strValue.Format(_T("<a%d type_index=\"%d\" union_index=\"%d\" left=\"%d\" top=\"%d\" right=\"%d\" bottom=\"%d\"></a%d>\r\n"),
							iAlignIdx + 1, nAlign + 1, nUnion + 1, rcAlign.left, rcAlign.top, rcAlign.right, rcAlign.bottom, iAlignIdx + 1);

						pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
						clResultFile.Write(pcFiledata, strValue.GetLength());
						free(pcFiledata);

						strLog.Format(L"%d,%d,%d,%d\n", rcAlign.left, rcAlign.top, rcAlign.right, rcAlign.bottom);
						OutputDebugString(strLog);

						iAlignIdx++;
					}
				}
			}

			//[MASK]
			if (nMaskCount_in_Cell > 0)
			{
				int iMaskIdx = 0;
				CRect rcAlign;
				vector<CellMaskInfo_t> vecMaskInfo = itMask_in_Cell->second;
				int nCount = static_cast<int>(vecMaskInfo.size());
				for (int nMask = 0; nMask < nCount; nMask++)
				{
					int nUnionCount = static_cast<int>(vecMaskInfo[nMask].vcInCell_Pixel.size());
					for (int nUnion = 0; nUnion < nUnionCount; nUnion++)
					{
						rcAlign = vecMaskInfo[nMask].vcInCell_Pixel[nUnion];
						rcAlign.NormalizeRect();

						strValue.Format(_T("<m%d type_index=\"%d\" union_index=\"%d\" left=\"%d\" top=\"%d\" right=\"%d\" bottom=\"%d\"></m%d>\r\n"),
							iMaskIdx + 1, nMask + 1, nUnion + 1, rcAlign.left, rcAlign.top, rcAlign.right, rcAlign.bottom, iMaskIdx + 1);

						pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
						clResultFile.Write(pcFiledata, strValue.GetLength());
						free(pcFiledata);

						strLog.Format(L"%d,%d,%d,%d\n", rcAlign.left, rcAlign.top, rcAlign.right, rcAlign.bottom);
						OutputDebugString(strLog);

						iMaskIdx++;
					}
				}
			}

			strValue.Format(_T("</c%d>\r\n"), iCellIdx);
			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);
			iCellIdx++;
			iTotCellIdx++;
		}

		strValue.Format(_T("</s%d>\r\n"), iIdx);
		pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
		clResultFile.Write(pcFiledata, strValue.GetLength());
		free(pcFiledata);
		iIdx++;
	}

	strValue.Format(_T("</swath>\r\n\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	strValue.Format(_T("</data>\r\n\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);
	clResultFile.Close();

	return RESULT_GOOD;
}

UINT32 CAOI::ExtractCrossPointInfo(IN INT32 iStepIdx, IN enum Orient eOrient)
{
	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	if (iStepIdx != m_pUserLayerSetInfo->nPanelStepIdx)
		iStepIdx = m_pUserLayerSetInfo->nPanelStepIdx;

	CStep *pStep = m_pJobData->m_arrStep[iStepIdx];
	if (pStep == nullptr)
		return RESULT_BAD;

	int iCrossPtNum = 0;	
	for (auto it : m_pUserLayerSetInfo->vcStripStepIdx)
	{
		CString strStripStepName = m_pJobData->m_arrStep[it]->m_strStepName;

		bool bFindCrossPt = false;
		for (UINT32 i = 0; i < pStep->m_nSubStepCnt; i++)
		{
			CStepRepeat *pSubStepRepeat = pStep->m_Stephdr.m_arrStepRepeat[i];
			if (pSubStepRepeat == nullptr)
				continue;
	
			if (pSubStepRepeat->m_strName.CompareNoCase(strStripStepName) != 0)
				continue;

			CLayer *pPatternLayer = GetLayerInfo(m_pUserLayerSetInfo, m_pJobData->m_arrStep[it], PATTERN_LAYER_TXT);
			if (pPatternLayer == nullptr) 
				return RESULT_BAD;

			//郴摸俊绰 粮犁窍瘤 臼澜
			CLayer *pSRLayer = GetLayerInfo(m_pUserLayerSetInfo, m_pJobData->m_arrStep[it], SR_LAYER_TXT);
			if (pSRLayer == nullptr)
				continue;

			iCrossPtNum = 0;

			//Cross Point 
			//1. Pad & construct
			for (int feno = 0; feno < pPatternLayer->m_FeatureFile.m_arrFeature.GetSize(); feno++)
			{
				CFeature* pFeature = pPatternLayer->m_FeatureFile.m_arrFeature.GetAt(feno);
				if (pFeature == nullptr || pFeature->m_eType != FeatureType::P)
					continue;

				CSymbol* pSym = ((CFeatureP*)pFeature)->m_pSymbol;
				if (pSym == nullptr || pSym->m_pUserSymbol == nullptr)
					continue;

				if (pSym->m_pUserSymbol->m_strSymbolName.Find(CROSS_POINT_SYMBOL_TXT) == -1)
					continue;

				//Cross Points
				iCrossPtNum++;
				//

				bFindCrossPt = true;
			}

			if (!bFindCrossPt)
			{
				//2. Surface
				double dRefWidth = -1;
				double dRefHeight = -1;
			
				for (int feno = 0; feno < pPatternLayer->m_FeatureFile.m_arrFeature.GetSize(); feno++)
				{
					CFeature* pFeature = pPatternLayer->m_FeatureFile.m_arrFeature.GetAt(feno);
					if (pFeature == nullptr || pFeature->m_eType != FeatureType::S)
						continue;

					CFeatureS *pFeatureS = (CFeatureS*)pFeature;
					int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
					if (nObOeCount != 1)
						continue;

					CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(0);
					if (pObOe == nullptr) continue;

					if (!pObOe->m_bAllOs || !pObOe->m_bPolyType)
						continue;

					int nOsOcCount = static_cast<int>(pObOe->m_arrOsOc.GetSize());
					if (nOsOcCount != CROSS_POINT_EDGES)
						continue;

					RECTD rcRect = pFeatureS->m_MinMaxRect;
					double dWidth = fabs(rcRect.right - rcRect.left);
					double dHeight = fabs(rcRect.bottom - rcRect.top);

					if (dWidth <= 0 || dHeight <= 0) continue;

					if (fabs(dWidth - dHeight) > EPSILON_DELTA1)
						continue;

					if (dRefWidth < 0 && dRefHeight < 0)
					{
						dRefWidth = dWidth;
						dRefHeight = dHeight;
					}
					else
					{
						if (fabs(dWidth - dRefWidth) > EPSILON_DELTA1)
							continue;

						if (fabs(dHeight - dRefHeight) > EPSILON_DELTA1)
							continue;
					}
										
					//Cross Points
					iCrossPtNum++;
					//

					bFindCrossPt = true;
				}
			}//Second Search

			if (bFindCrossPt)
				break;
		}//Sub Step
	}

	return RESULT_GOOD;
}


UINT32 CAOI::GetCrossPointIndex(IN INT32 iStepIdx, IN enum Orient eOrient, OUT vector<int> &vecFeatureIndex)
{
	vecFeatureIndex.clear();

	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	if (iStepIdx < 0)
		return RESULT_BAD;

	CStep *pStep = m_pJobData->m_arrStep[iStepIdx];
	if (pStep == nullptr)
		return RESULT_BAD;

	int iCrossPtNum = 0;
	

	CLayer *pPatternLayer = GetLayerInfo(m_pUserLayerSetInfo, pStep, PATTERN_LAYER_TXT);
	if (pPatternLayer == nullptr)
		return RESULT_BAD;

	iCrossPtNum = 0;

	bool bFindCrossPt = false;

	//Cross Point 
	//1. Pad & construct
	for (int feno = 0; feno < pPatternLayer->m_FeatureFile.m_arrFeature.GetSize(); feno++)
	{
		CFeature* pFeature = pPatternLayer->m_FeatureFile.m_arrFeature.GetAt(feno);
		if (pFeature == nullptr || pFeature->m_eType != FeatureType::P)
			continue;

		CSymbol* pSym = ((CFeatureP*)pFeature)->m_pSymbol;
		if (pSym == nullptr || pSym->m_pUserSymbol == nullptr)
			continue;

		if (pSym->m_pUserSymbol->m_strSymbolName.Find(CROSS_POINT_SYMBOL_TXT) == -1)
			continue;

		//Cross Points
		iCrossPtNum++;
		//

		vecFeatureIndex.push_back(feno);

		bFindCrossPt = true;
	}

	if (!bFindCrossPt)
	{
		//2. Surface
		double dRefWidth = -1;
		double dRefHeight = -1;

		for (int feno = 0; feno < pPatternLayer->m_FeatureFile.m_arrFeature.GetSize(); feno++)
		{
			CFeature* pFeature = pPatternLayer->m_FeatureFile.m_arrFeature.GetAt(feno);
			if (pFeature == nullptr || pFeature->m_eType != FeatureType::S)
				continue;

			CFeatureS *pFeatureS = (CFeatureS*)pFeature;
			int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
			if (nObOeCount != 1)
				continue;

			CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(0);
			if (pObOe == nullptr) continue;

			if (!pObOe->m_bAllOs || !pObOe->m_bPolyType)
				continue;

			int nOsOcCount = static_cast<int>(pObOe->m_arrOsOc.GetSize());
			if (nOsOcCount != CROSS_POINT_EDGES)
				continue;

			RECTD rcRect = pObOe->m_MinMaxRect;//pFeatureS->m_MinMaxRect;
			double dWidth = fabs(rcRect.right - rcRect.left);
			double dHeight = fabs(rcRect.bottom - rcRect.top);

			if (dWidth <= 0 || dHeight <= 0) continue;

			if (fabs(dWidth - dHeight) > EPSILON_DELTA1)
				continue;

			if (dRefWidth < 0 && dRefHeight < 0)
			{
				dRefWidth = dWidth;
				dRefHeight = dHeight;
			}
			else
			{
				if (fabs(dWidth - dRefWidth) > EPSILON_DELTA1)
					continue;

				if (fabs(dHeight - dRefHeight) > EPSILON_DELTA1)
					continue;
			}

			//Cross Points
			iCrossPtNum++;
			//

			vecFeatureIndex.push_back(feno);


			bFindCrossPt = true;
		}
	}//Second Search

	return RESULT_GOOD;
}



//CrossPoint
UINT32 CAOI::SetAlignCrossPoint(IN const INT32 & iStepIdx)
{
	enum Orient orient = Orient::NoMir0Deg;
	double dAngle = m_pUserSetInfo->dAngle;
	bool bMirror = m_pUserSetInfo->bMirror;

	GetOrient(dAngle, bMirror, orient);

	CStep *pStep = m_pJobData->m_arrStep[iStepIdx];
	if (pStep == nullptr) return RESULT_BAD;


	CLayer *pPatternLayer = GetLayerInfo(m_pUserLayerSetInfo, pStep, PATTERN_LAYER_TXT);
	if (pPatternLayer == nullptr) return RESULT_BAD;

	vector<int> vecCrossFeatureIndex;
	GetCrossPointIndex(iStepIdx, orient, vecCrossFeatureIndex);

	

	//茫篮 Pattern 吝 鞍篮 葛剧尝府 弓绢霖促.
	vector<vector<int>> vecvecAlignIndex;
	int nCrossFeatureCount = static_cast<int>(vecCrossFeatureIndex.size());
	for (int nCross = 0; nCross < nCrossFeatureCount; nCross++)
	{
		if (nCross == 0)
		{
			vector<int> vecIndex;
			vecIndex.push_back(vecCrossFeatureIndex[nCross]);
			vecvecAlignIndex.push_back(vecIndex);
		}
		else
		{
			BOOL bCompare = FALSE;
			int nAlignPattern = -1;
			int nAlignCount = static_cast<int>(vecvecAlignIndex.size());
			for (int nAlign = 0; nAlign < nAlignCount; nAlign++)
			{
				bCompare = m_pDrawManager->CompareFeature(pPatternLayer, vecvecAlignIndex[nAlign][0], vecCrossFeatureIndex[nCross]);

				if (bCompare == TRUE)
				{
					nAlignPattern = nAlign;
					break;
				}
			}

			if (bCompare == TRUE)
			{
				vecvecAlignIndex[nAlignPattern].push_back(vecCrossFeatureIndex[nCross]);
				
			}
			else
			{
				vector<int> vecIndex;
				vecIndex.push_back(vecCrossFeatureIndex[nCross]);
				vecvecAlignIndex.push_back(vecIndex);
			}

		}
	}

	//鞍篮 Feature 尝府 弓绢 林菌栏聪,
	int nAlignCount = static_cast<int>(vecvecAlignIndex.size());
	if (nAlignCount > 0)
	{
		CString str;
		str.Format(_T("Auto Search肺 茫篮 Align Point啊 乐嚼聪促.(Pattern:%d)\n 扁粮 Align 器牢飘绰 昏力窍绊 措眉 钦聪促."), nAlignCount);
		AfxMessageBox(str);

		//
		int nCount = static_cast<int>(pPatternLayer->m_arrAlign.GetCount());
		for (int i = 0 ; i < nCount ; i++)
		{
			if (pPatternLayer->m_arrAlign[i] != nullptr)
			{
				delete pPatternLayer->m_arrAlign[i];
				pPatternLayer->m_arrAlign[i] = nullptr;
			}
		}
		pPatternLayer->m_arrAlign.RemoveAll();
		//

		for (int nAlign = 0; nAlign < nAlignCount; nAlign++)
		{
			CAlignMask *pAlignMask = new CAlignMask;

			int nRegionCount = static_cast<int>(vecvecAlignIndex[nAlign].size());
			for (int nRegion = 0; nRegion < nRegionCount; nRegion++)
			{
				int nfeno = vecvecAlignIndex[nAlign][nRegion];

				RECTD drtFeature = pPatternLayer->m_FeatureFile.m_arrFeature.GetAt(nfeno)->m_MinMaxRect;
				CFeature *pFeature = CDrawFunction::MakeRectFeature(drtFeature);

				pAlignMask->m_arrFeature.Add(pFeature);
			}

			if (nRegionCount > 0)
			{
				pPatternLayer->m_arrAlign.Add(pAlignMask);
			}
		}
	}
	else
	{
		AfxMessageBox(_T("Auto Search肺 茫篮 Align Point啊 绝嚼聪促."));
	}

	return RESULT_GOOD; 
}

UINT32 CAOI::EstimateFOV_Based_On_Strip(INT32 iStepIdx, enum Orient eOrient)
{
	m_stFovInfo.Clear();
	m_stCellInfo.Clear();

	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	if (RESULT_GOOD != CheckParams())
		return RESULT_BAD;

	if (iStepIdx < 0)
		return RESULT_BAD;	

	CStep *pStep = m_pJobData->m_arrStep[iStepIdx];
	if (pStep == nullptr)
		return RESULT_BAD;

	CString strLog;
	double dResolMM = m_stFovInfo.stBasic.resolution / 1000.0f;
	int iRowStripNumInPanel = static_cast<int>(m_stPanelInfo.iRowStripNumInPanel);
	int iColStripNumInPanel = static_cast<int>(m_stPanelInfo.iColStripNumInPanel);

	if (iRowStripNumInPanel <= 0 || iColStripNumInPanel <= 0 ||
		m_stPanelInfo.vcStripInfo.size() != iRowStripNumInPanel * iColStripNumInPanel)
		return RESULT_BAD;
	
	int iRowBlockNumInStrip = static_cast<int>(m_stPanelInfo.vcStripInfo[0]->iRowBlockNumInStrip);
	int iColBlockNumInStrip = static_cast<int>(m_stPanelInfo.vcStripInfo[0]->iColBlockNumInStrip);
	
	if (iRowBlockNumInStrip <= 0 || iColBlockNumInStrip <= 0 ||
		m_stPanelInfo.vcStripInfo[0]->vcBlockInfo.size() != iRowBlockNumInStrip * iColBlockNumInStrip)
		return RESULT_BAD;	

	int iRowUnitNumInBlock = static_cast<int>(m_stPanelInfo.vcStripInfo[0]->vcBlockInfo[0]->iRowUnitNumInBlock);
	int iColUnitNumInBlock = static_cast<int>(m_stPanelInfo.vcStripInfo[0]->vcBlockInfo[0]->iColUnitNumInBlock);
	if (iRowUnitNumInBlock <= 0 || iColUnitNumInBlock <= 0 ||
		m_stPanelInfo.vcStripInfo[0]->vcBlockInfo[0]->vcUnitInfo.size() != iRowUnitNumInBlock * iColUnitNumInBlock)
		return RESULT_BAD;

	int iCellColNum = m_stCellInfo.stBasic.cellNum.cx;
	int iCellRowNum = m_stCellInfo.stBasic.cellNum.cy;

	int iRowCellNumInBlock = -1;
	if (m_pSystemSpec->sysBasic.MasterLevel == eMasterLevel::eBlockLv)
	{
		iRowCellNumInBlock = iCellRowNum;
	}
	else if (m_pSystemSpec->sysBasic.MasterLevel == eMasterLevel::ePanelLv)
	{
		iRowCellNumInBlock = iCellRowNum % (iRowBlockNumInStrip * iRowStripNumInPanel);
		if (iRowCellNumInBlock != 0)
		{
			strLog.Format(_T("Strip Row 俺荐绰 %d, Block Row 俺荐绰 %d涝聪促. Cell Row 俺荐绰 %d狼 硅荐啊 登绢具 钦聪促. 涝仿 蔼阑 犬牢窍咯 林绞矫坷."),
				iRowStripNumInPanel, iRowBlockNumInStrip, iRowBlockNumInStrip * iRowStripNumInPanel);
			AfxMessageBox(strLog);
			return RESULT_BAD;
		}

		iRowCellNumInBlock = iCellRowNum / (iRowBlockNumInStrip * iRowStripNumInPanel);
		if (iRowCellNumInBlock == 0)
		{
			strLog.Format(_T("Strip Row 俺荐绰 %d, Block Row 俺荐绰 %d涝聪促. Cell Row 俺荐绰 %d焊促 目具 钦聪促. 涝仿 蔼阑 犬牢窍咯 林绞矫坷."),
				iRowStripNumInPanel, iRowBlockNumInStrip, iRowBlockNumInStrip * iRowStripNumInPanel);
			AfxMessageBox(strLog);
			return RESULT_BAD;
		}
	}

	//Block 扁霖狼 Strip喊 Rect 拌魂
	RECTD rcWorkRect = m_stPanelInfo.vcStripInfo[0]->vcBlockInfo[0]->rcRect;
	if (m_pSystemSpec->sysBasic.bProfileMode)
		rcWorkRect = m_stPanelInfo.vcStripInfo[0]->vcBlockInfo[0]->rcProfile;

	if (rcWorkRect.IsRectNull())
		return RESULT_BAD;

	int iSwathNum = -1;
	if (RESULT_GOOD != CheckSwathNum(rcWorkRect, dResolMM, iSwathNum))
		return RESULT_BAD;

	/*int iColCellNumInBlock = -1;
	if (m_pSystemSpec->sysBasic.MasterLevel == eMasterLevel::eBlockLv)
	{
		iColCellNumInBlock = iCellColNum % iSwathNum;
		if (iColCellNumInBlock != 0)
		{
			strLog.Format(_T("Block Swath 俺荐绰 %d涝聪促. Cell Col 俺荐绰 %d狼 硅荐啊 登绢具 钦聪促. 涝仿 蔼阑 犬牢窍咯 林绞矫坷."),
				iSwathNum, iSwathNum);
			AfxMessageBox(strLog);
			return RESULT_BAD;
		}

		iColCellNumInBlock = iCellColNum / iSwathNum;
		if (iColCellNumInBlock == 0)
		{
			strLog.Format(_T("Block Swath 俺荐绰 %d涝聪促. Cell Col 俺荐绰 %d焊促 目具 钦聪促. 涝仿 蔼阑 犬牢窍咯 林绞矫坷."),
				iSwathNum, iSwathNum);
			AfxMessageBox(strLog);
			return RESULT_BAD;
		}
	}
	else if (m_pSystemSpec->sysBasic.MasterLevel == eMasterLevel::ePanelLv)
	{
		iColCellNumInBlock = iCellColNum;
		iCellColNum *= iSwathNum;
	}
	*/
	int iColCellNumInBlock = iCellColNum;
	iCellColNum *= iSwathNum;

	double dBlockWidthMM = fabs(rcWorkRect.right - rcWorkRect.left) + 2 * m_stFovInfo.stBasic.dummyMargin.x;
	double dBlockHeightMM = fabs(rcWorkRect.bottom - rcWorkRect.top) + 2 * m_stFovInfo.stBasic.dummyMargin.y;

	double dBlockWidthPxl = Round(dBlockWidthMM / dResolMM + 0.01);
	double dBlockHeightPxl = Round(dBlockHeightMM / dResolMM + 0.01);
	
	RECTD rcSwathRect = rcWorkRect;
	for (int j = 0; j < (int)m_stPanelInfo.iRowStripNumInPanel; j++)
	{
		int iBlockNum = m_stPanelInfo.vcStripInfo[j]->iRowBlockNumInStrip *  m_stPanelInfo.vcStripInfo[j]->iColBlockNumInStrip;
		for (int b = 0; b < iBlockNum; b++)
		{
			RECTD rcRect = m_stPanelInfo.vcStripInfo[j]->vcBlockInfo[b]->rcRect;
			if (m_pSystemSpec->sysBasic.bProfileMode)
				rcRect = m_stPanelInfo.vcStripInfo[j]->vcBlockInfo[b]->rcProfile;

			if (fabs(rcSwathRect.left) > fabs(rcRect.left))		rcSwathRect.left = rcRect.left;
			if (fabs(rcSwathRect.right) < fabs(rcRect.right))	rcSwathRect.right = rcRect.right;
			if (fabs(rcSwathRect.bottom) > fabs(rcRect.bottom))	rcSwathRect.bottom = rcRect.bottom;
			if (fabs(rcSwathRect.top) < fabs(rcRect.top))		rcSwathRect.top = rcRect.top;
		}
	}

	double dSwathWidthMM = fabs(rcSwathRect.right - rcSwathRect.left) + 2 * m_stFovInfo.stBasic.dummyMargin.x;
	double dSwathHeightMM = fabs(rcSwathRect.bottom - rcSwathRect.top) + 2 * m_stFovInfo.stBasic.dummyMargin.x;

	double dSwathWidthPxl = Round(dSwathWidthMM / dResolMM + 0.01);
	double dSwathHeightPxl = Round(dSwathHeightMM / dResolMM + 0.01);

	//AOI俊辑 夸没茄 荐侥
	int iCellWidthPxl = static_cast<int>(ceil((dBlockWidthPxl / iCellColNum) + ((double)m_stCellInfo.stBasic.cellOverlap.cx * ((iCellColNum - 1) / (double)iCellColNum))));
	int iCellHeightPxl = static_cast<int>(ceil((dBlockHeightPxl / iRowCellNumInBlock) + ((double)m_stCellInfo.stBasic.cellOverlap.cy * ((iRowCellNumInBlock - 1) / (double)iRowCellNumInBlock))));
	if (iCellWidthPxl <= 0 || iCellHeightPxl <= 0)
		return RESULT_BAD;

	if (m_stFovInfo.stBasic.fovSize.cx / iCellWidthPxl <= 0)
		return RESULT_BAD;

	m_stCellInfo.stBasic.cellSize.cx = iCellWidthPxl;
	m_stCellInfo.stBasic.cellSize.cy = iCellHeightPxl;
	m_stCellInfo.stBasic.cellNum.cx = m_stFovInfo.stBasic.fovSize.cx / iCellWidthPxl;
	m_stCellInfo.stBasic.cellNum.cy = iRowCellNumInBlock; // * (iRowBlockNumInStrip * iRowStripNumInPanel)

	if (m_stCellInfo.stBasic.cellNum.cx != iColCellNumInBlock)
		return RESULT_BAD;

	int iCellTotalWidthPxl = iCellWidthPxl * m_stCellInfo.stBasic.cellNum.cx;
	if (m_stCellInfo.stBasic.cellNum.cx > 1)
		iCellTotalWidthPxl -= (m_stCellInfo.stBasic.cellOverlap.cx * (m_stCellInfo.stBasic.cellNum.cx - 1));

	//Margin & Overlap 犁 拌魂
	m_stFovInfo.stBasic.swathNum = iSwathNum;
	m_stFovInfo.stBasic.swathNumInBlock = iSwathNum;
	m_stFovInfo.stBasic.fovMargin.cx = (m_stFovInfo.stBasic.fovSize.cx - iCellTotalWidthPxl) / 2;
	m_stFovInfo.stBasic.fovMargin.cy = 1000;
	m_stFovInfo.stBasic.fovOverlap.cx = (m_stFovInfo.stBasic.fovMargin.cx * 2) + m_stCellInfo.stBasic.cellOverlap.cx;
	m_stFovInfo.stBasic.fovOverlap.cy = 0;
	m_stFovInfo.stBasic.prodSize.cx = static_cast<int>(dSwathWidthPxl);
	m_stFovInfo.stBasic.prodSize.cy = static_cast<int>(dSwathHeightPxl) + (2 * m_stFovInfo.stBasic.fovMargin.cy);
	m_stFovInfo.stBasic.fovSize.cy = m_stFovInfo.stBasic.prodSize.cy;

	if (RESULT_GOOD != EstimateSwath())	
		return RESULT_BAD;

	SIZE szBlock;
	szBlock.cx = static_cast<int>(dBlockWidthPxl);
	szBlock.cy = static_cast<int>(dBlockHeightPxl) + (2 * m_stFovInfo.stBasic.fovMargin.cy);
		
	if (RESULT_GOOD != EstimateSwath_Based_On_Block(szBlock))
		return RESULT_BAD;
	
	m_stFovInfo.stBasic.swathNum = iSwathNum * (iColStripNumInPanel * iColBlockNumInStrip);
	if (RESULT_GOOD != CalculateOverlap_bw_Swath())
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CAOI::EstimateCell_Based_On_Strip(INT32 iStepIdx, enum Orient eOrient)
{
	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	if (iStepIdx < 0)
		return RESULT_BAD;

	if (m_stFovInfo.vcFovData.size() <= 0)
		return RESULT_BAD;

	int iCellColNum = m_stCellInfo.stBasic.cellNum.cx;
	int iCellRowNum = m_stCellInfo.stBasic.cellNum.cy;
	if (iCellColNum <= 0 || iCellRowNum <= 0)
		return RESULT_BAD;

	int iCellWidth = m_stCellInfo.stBasic.cellSize.cx;
	int iCellHeight = m_stCellInfo.stBasic.cellSize.cy;
	if (iCellWidth <= 0 || iCellHeight <= 0)
		return RESULT_BAD;

	int iCellMinOverlapX = m_stCellInfo.stBasic.cellOverlap.cx;
	if (iCellMinOverlapX < 0) iCellMinOverlapX = 0;
	int iCellMinOverlapY = m_stCellInfo.stBasic.cellOverlap.cy;
	if (iCellMinOverlapY < 0) iCellMinOverlapY = 0;

	double dResolMM = m_stFovInfo.stBasic.resolution / 1000.f;
	int iMinMarginX = m_stFovInfo.stBasic.fovMargin.cx;
	if (iMinMarginX < 0) iMinMarginX = 0;
	int iMinMarginY = m_stFovInfo.stBasic.fovMargin.cy;
	if (iMinMarginY < 0) iMinMarginY = 0;

	double dTemp = 0.;
	Orient ori = Orient::Mir0Deg;
	CDataManager::GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, ori);

	CRotMirrArr rotMirrArr;
	rotMirrArr.InputData(0, 0, CDataManager::ReverseOrient(ori), m_pUserSetInfo->mirrorDir);

	CString strLog;

	std::sort(m_stFovInfo.vcFovData_Block.begin(), m_stFovInfo.vcFovData_Block.end(), FovData_t::comp);

	for (int i = 0; i < (int)m_stFovInfo.stBasic.swathNum; i++)
	{
		for (auto itSwath : m_stFovInfo.vcFovData_Block)
		{
			if (i != itSwath->fovIdx.x)
				continue;

			RECTD	rcRectMM = itSwath->rcRectMM;
			RECT	rcRectPxl = itSwath->rcRectPxl;
			RECTD   rcRectMM_In_Panel = itSwath->rcRectMM_In_Panel;

			for (int iRow = 0; iRow < iCellRowNum; iRow++)
			{
				for (int iCol = 0; iCol < iCellColNum; iCol++)
				{
					FovData_t* pCell = new FovData_t;
					pCell->swathIdx = i;

					pCell->fovIdx.x = iCol;// +(itSwath->fovIdx.x * iCellColNum);
					pCell->fovIdx.y = iRow + (itSwath->fovIdx.y * iCellRowNum);

					//2. Overlap b/w All Cells 绊妨
					pCell->rcRectPxl.left = rcRectPxl.left + iCol * (iCellWidth - iCellMinOverlapX) + iMinMarginX;
					pCell->rcRectPxl.right = pCell->rcRectPxl.left + iCellWidth - 1;
					pCell->rcRectPxl.top = rcRectPxl.top + iRow * (iCellHeight - iCellMinOverlapY) + iMinMarginY;
					pCell->rcRectPxl.bottom = pCell->rcRectPxl.top + iCellHeight - 1;

					pCell->rcRectMM.left = rcRectMM.left + ((iCol * (iCellWidth - iCellMinOverlapX)) * dResolMM) + (iMinMarginX * dResolMM);
					pCell->rcRectMM.right = pCell->rcRectMM.left + (iCellWidth * dResolMM);
					pCell->rcRectMM.top = rcRectMM.top - ((iRow * (iCellHeight - iCellMinOverlapY)) * dResolMM) - (iMinMarginY * dResolMM);
					pCell->rcRectMM.bottom = pCell->rcRectMM.top - (iCellHeight * dResolMM);

					if (fabs(pCell->rcRectMM.left) < EPSILON_DELTA1)	pCell->rcRectMM.left = 0;
					if (fabs(pCell->rcRectMM.right) < EPSILON_DELTA1)	pCell->rcRectMM.right = 0;
					if (fabs(pCell->rcRectMM.top) < EPSILON_DELTA1)		pCell->rcRectMM.top = 0;
					if (fabs(pCell->rcRectMM.bottom) < EPSILON_DELTA1)	pCell->rcRectMM.bottom = 0;

					pCell->rcRectMM_In_Panel.left = rcRectMM_In_Panel.left + ((iCol * (iCellWidth - iCellMinOverlapX)) * dResolMM) + (iMinMarginX * dResolMM);
					pCell->rcRectMM_In_Panel.right = pCell->rcRectMM_In_Panel.left + (iCellWidth * dResolMM);
					pCell->rcRectMM_In_Panel.top = rcRectMM_In_Panel.top - ((iRow * (iCellHeight - iCellMinOverlapY)) * dResolMM) - (iMinMarginY * dResolMM);
					pCell->rcRectMM_In_Panel.bottom = pCell->rcRectMM_In_Panel.top - (iCellHeight * dResolMM);

					if (fabs(pCell->rcRectMM_In_Panel.left) < EPSILON_DELTA1)	pCell->rcRectMM_In_Panel.left = 0;
					if (fabs(pCell->rcRectMM_In_Panel.right) < EPSILON_DELTA1)	pCell->rcRectMM_In_Panel.right = 0;
					if (fabs(pCell->rcRectMM_In_Panel.top) < EPSILON_DELTA1)	pCell->rcRectMM_In_Panel.top = 0;
					if (fabs(pCell->rcRectMM_In_Panel.bottom) < EPSILON_DELTA1)	pCell->rcRectMM_In_Panel.bottom = 0;

					if (pCell->rcRectMM_In_Panel.IsRectNull())
					{
#ifdef _DEBUG
						AfxMessageBox(_T("伎 沥焊甫 犬牢窍咯 林技夸."));
#endif
						return RESULT_BAD;
					}

					rotMirrArr.FinalPoint(&pCell->rcDraw.left, &pCell->rcDraw.top, pCell->rcRectMM.left, pCell->rcRectMM.top);
					rotMirrArr.FinalPoint(&pCell->rcDraw.right, &pCell->rcDraw.bottom, pCell->rcRectMM.right, pCell->rcRectMM.bottom);

					if (RESULT_GOOD != ChangeToDrawCoordinate(pCell->rcDraw, m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, m_pUserSetInfo->mirrorDir))
						return RESULT_BAD;

//					strLog.Format(_T("Cell Swath[%d] Col[%d] Row[%d] : (L,T,R,B) (%d,%d,%d,%d)\n"), i, pCell->fovIdx.x, pCell->fovIdx.y,
//						pCell->rcRectPxl.left, pCell->rcRectPxl.top, pCell->rcRectPxl.right, pCell->rcRectPxl.bottom);
//					OutputDebugString(strLog);

					strLog.Format(_T("Cell Swath[%d] Col[%d] Row[%d] : (L,T,R,B) (%.2f,%.2f,%.2f,%.2f)\n"), i, pCell->fovIdx.x, pCell->fovIdx.y,
						pCell->rcRectMM_In_Panel.left, pCell->rcRectMM_In_Panel.top, pCell->rcRectMM_In_Panel.right, pCell->rcRectMM_In_Panel.bottom);
					OutputDebugString(strLog);

					strLog.Format(_T("Cell Swath[%d] Col[%d] Row[%d] : (L,T,R,B) (%.2f,%.2f,%.2f,%.2f)\n"), i, pCell->fovIdx.x, pCell->fovIdx.y,
						pCell->rcRectMM.left, pCell->rcRectMM.top, pCell->rcRectMM.right, pCell->rcRectMM.bottom);
					OutputDebugString(strLog);

					m_stCellInfo.vcCellData.emplace_back(pCell);
				}
			}
		}
	}
		
	int iRowBlockNumInStrip = static_cast<int>(m_stPanelInfo.vcStripInfo[0]->iRowBlockNumInStrip);
	int iRowStripNumInPanel = static_cast<int>(m_stPanelInfo.iRowStripNumInPanel);
	if (iRowBlockNumInStrip <= 0 || iRowStripNumInPanel <= 0)
		return RESULT_BAD;

	m_stCellInfo.stBasic.cellNum.cx *= m_stFovInfo.stBasic.swathNum;
	m_stCellInfo.stBasic.cellNum.cy *= (iRowBlockNumInStrip * iRowStripNumInPanel);
	if (m_stCellInfo.vcCellData.size() != m_stCellInfo.stBasic.cellNum.cx * m_stCellInfo.stBasic.cellNum.cy)
		return RESULT_BAD;

	if (RESULT_GOOD != CalculateOverlap_bw_Cell())
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CAOI::ChangeToDrawCoordinate(RECTD &rcDraw, DOUBLE dAngle, bool bMirror, eDirectionType mirType)
{
	double dTemp = 0.;

	if (dAngle == 90.0)
	{
		SWAP(rcDraw.left, rcDraw.right, dTemp);
		rcDraw.left *= -1.0;
		rcDraw.right *= -1.0;
	}
	else if (dAngle == 180.0)
	{
		SWAP(rcDraw.left, rcDraw.right, dTemp);
		rcDraw.left *= -1.0;
		rcDraw.right *= -1.0;

		SWAP(rcDraw.top, rcDraw.bottom, dTemp);
		rcDraw.top *= -1.0;
		rcDraw.bottom *= -1.0;
	}
	else if (dAngle == 270.0)
	{
		SWAP(rcDraw.top, rcDraw.bottom, dTemp);
		rcDraw.top *= -1.0;
		rcDraw.bottom *= -1.0;
	}

	if (bMirror)
	{
		if (mirType == eDirectionType::eBoth ||
			mirType == eDirectionType::eVertical)
		{
			SWAP(rcDraw.top, rcDraw.bottom, dTemp);
			rcDraw.top *= -1.0;
			rcDraw.bottom *= -1.0;
		}

		if (mirType == eDirectionType::eBoth ||
			mirType == eDirectionType::eHorizontal)
		{
			SWAP(rcDraw.left, rcDraw.right, dTemp);
			rcDraw.left *= -1.0;
			rcDraw.right *= -1.0;
		}
	}

	return RESULT_GOOD;
}

UINT32 CAOI::RearrageIndexInfo_Based_Strip(IN enum Orient eOrient)
{
	if (m_pUserSetInfo == nullptr)
		return RESULT_BAD;

	CString strLog;
	int index = 0;

	for (auto it : m_stPanelInfo.vcStripInfo)
	{
		strLog.Format(_T("%d, %.3f, %.3f, %.3f, %.3f\n"), index++, it->rcProfile.left, it->rcProfile.top, it->rcProfile.right, it->rcProfile.bottom);
		OutputDebugString(strLog);
	}

	if (m_pUserSetInfo->mirrorDir == eDirectionType::eBoth)
		std::sort(m_stPanelInfo.vcStripInfo.begin(), m_stPanelInfo.vcStripInfo.end(), StripInfo_t::compX);
	else if (m_pUserSetInfo->mirrorDir == eDirectionType::eVertical)
		std::sort(m_stPanelInfo.vcStripInfo.begin(), m_stPanelInfo.vcStripInfo.end(), StripInfo_t::comp_reverseY);
	else if (m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal)
		std::sort(m_stPanelInfo.vcStripInfo.begin(), m_stPanelInfo.vcStripInfo.end(), StripInfo_t::comp_reverseX);

	index = 0;
	for (auto it : m_stPanelInfo.vcStripInfo)
	{
		strLog.Format(_T("%d, %.3f, %.3f, %.3f, %.3f\n"), index++, it->rcProfile.left, it->rcProfile.top, it->rcProfile.right, it->rcProfile.bottom);
		OutputDebugString(strLog);
	}

	UINT32 iStripIdx = 0;
	for (auto itStrip : m_stPanelInfo.vcStripInfo)
	{
		UINT32 iStripStartIdx = iStripIdx * (itStrip->iColUnitNumInStrip * itStrip->iRowUnitNumInStrip);
		itStrip->iStripIdx = iStripIdx++;

		index = 0;
		for (auto it : itStrip->vcBlockInfo)
		{
			strLog.Format(_T("%d, %.3f, %.3f, %.3f, %.3f\n"), index++, it->rcProfile.left, it->rcProfile.top, it->rcProfile.right, it->rcProfile.bottom);
			OutputDebugString(strLog);
		}

		if (m_pUserSetInfo->mirrorDir == eDirectionType::eBoth)
			std::sort(itStrip->vcBlockInfo.begin(), itStrip->vcBlockInfo.end(), BlockInfo_t::compX);
		if (m_pUserSetInfo->mirrorDir == eDirectionType::eVertical)
			std::sort(itStrip->vcBlockInfo.begin(), itStrip->vcBlockInfo.end(), BlockInfo_t::comp_reverseY);
		else if (m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal)
			std::sort(itStrip->vcBlockInfo.begin(), itStrip->vcBlockInfo.end(), BlockInfo_t::comp_reverseX);

		index = 0;
		for (auto it : itStrip->vcBlockInfo)
		{
			strLog.Format(_T("%d, %.3f, %.3f, %.3f, %.3f\n"), index++, it->rcProfile.left, it->rcProfile.top, it->rcProfile.right, it->rcProfile.bottom);
			OutputDebugString(strLog);
		}

		UINT32 iBlockIdx = 0;
		for (auto itBlock : itStrip->vcBlockInfo)
		{
			UINT32 iBlockStartIdx = iBlockIdx * itBlock->iRowUnitNumInBlock;
			itBlock->iBlockIdx = iBlockIdx++;

			index = 0;
			for (auto it : itBlock->vcUnitInfo)
			{
				strLog.Format(_T("%d, %.3f, %.3f, %.3f, %.3f\n"), index++, it->rcProfile.left, it->rcProfile.top, it->rcProfile.right, it->rcProfile.bottom);
				OutputDebugString(strLog);
			}

			if (m_pUserSetInfo->mirrorDir == eDirectionType::eBoth)
				std::sort(itBlock->vcUnitInfo.begin(), itBlock->vcUnitInfo.end(), UnitInfo_t::compX);
			else if (m_pUserSetInfo->mirrorDir == eDirectionType::eVertical)
				std::sort(itBlock->vcUnitInfo.begin(), itBlock->vcUnitInfo.end(), UnitInfo_t::comp_reverseY);
			else if (m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal)
				std::sort(itBlock->vcUnitInfo.begin(), itBlock->vcUnitInfo.end(), UnitInfo_t::comp_reverseX);

			index = 0;
			for (auto it : itBlock->vcUnitInfo)
			{
				strLog.Format(_T("%d, %.3f, %.3f, %.3f, %.3f\n"), index++, it->rcProfile.left, it->rcProfile.top, it->rcProfile.right, it->rcProfile.bottom);
				OutputDebugString(strLog);
			}

			bool bPrevState = false;
			UINT32 iUnitIdx = 0, iTotUnitIdx = 0;
			for (auto itUnit : itBlock->vcUnitInfo)
			{
				itUnit->iStripIdx = itStrip->iStripIdx;
				itUnit->iBlockIdx = itBlock->iBlockIdx;

				UINT32 iColIdx = iTotUnitIdx / itBlock->iRowUnitNumInBlock;
				bool bIsEven = iColIdx % 2 == 0 ? true : false;

				if (bIsEven != bPrevState)
					iUnitIdx = 0;

				if (bIsEven)	// Sequential Order
				{
				//	itUnit->iUnitIdx_Ori = iStripStartIdx + iBlockStartIdx + itBlock->iRowUnitNumInBlock * iColIdx + iUnitIdx;
					itUnit->iUnitIdx_Ori = iStripStartIdx + iBlockStartIdx + (itStrip->iRowBlockNumInStrip * itBlock->iRowUnitNumInBlock) * iColIdx + iUnitIdx;
					bPrevState = bIsEven;
				}
				else           // Reverse Order
				{
				//	itUnit->iUnitIdx_Ori = iStripStartIdx + iBlockStartIdx + itBlock->iRowUnitNumInBlock * (iColIdx + 1) - 1 - iUnitIdx;
					itUnit->iUnitIdx_Ori = iStripStartIdx - iBlockStartIdx + (itStrip->iRowBlockNumInStrip * itBlock->iRowUnitNumInBlock) * (iColIdx + 1) - 1 - iUnitIdx;
					bPrevState = bIsEven;
				}

				strLog.Format(L"%d,%d,%d,%.3f,%.3f,%.3f,%.3f\n", itUnit->iUnitIdx_Ori, itUnit->iStripIdx, itUnit->iBlockIdx, itUnit->rcProfile.left, itUnit->rcProfile.top, itUnit->rcProfile.right, itUnit->rcProfile.bottom);
				OutputDebugString(strLog);

				iTotUnitIdx++;
				iUnitIdx++;
			}
			if (m_pUserSetInfo->mirrorDir != eDirectionType::DirectionTypeNone)
				std::sort(itBlock->vcUnitInfo.begin(), itBlock->vcUnitInfo.end(), UnitInfo_t::compY);
		}

		if (m_pUserSetInfo->mirrorDir != eDirectionType::DirectionTypeNone)
			std::sort(itStrip->vcBlockInfo.begin(), itStrip->vcBlockInfo.end(), BlockInfo_t::comp);
	} //No Mirror

	if (m_pUserSetInfo->mirrorDir != eDirectionType::DirectionTypeNone)
		std::sort(m_stPanelInfo.vcStripInfo.begin(), m_stPanelInfo.vcStripInfo.end(), StripInfo_t::compY);

	return RESULT_GOOD;
}

UINT32 CAOI::CheckParams()
{
	if (m_pSystemSpec->sysBasic.ScanType != eScanType::eLineScan ||
		m_pUserSetInfo->fovType != eFovType::eVerticalJigjag)
		return RESULT_BAD;

	CameraSpec_t *pSpec = nullptr;
	if (m_pSystemSpec->sysBasic.ScanType == eScanType::eLineScan)
		pSpec = &m_pSystemSpec->sysCamera[true];
	else
		pSpec = &m_pSystemSpec->sysCamera[false];

	if (pSpec == nullptr) return RESULT_BAD;

	double dResolution = pSpec->dResolution;
	if (dResolution <= 0)
	{
		AfxMessageBox(_T("秦惑档 涝仿 蔼捞 棵官福瘤 臼嚼聪促."));
		return RESULT_BAD;
	}

	double dMarginX = pSpec->dDummyMarginX;
	if (dMarginX < 0) dMarginX = 0;
	double dMarginY = pSpec->dDummyMarginY;
	if (dMarginY < 0) dMarginY = 0;

	int iFrameWidth = static_cast<int>(pSpec->iFrameWidth);
	if (iFrameWidth <= 0)
	{
		AfxMessageBox(_T("Frame Width 涝仿 蔼捞 棵官福瘤 臼嚼聪促."));
		return RESULT_BAD;
	}

	int iFrameHeight = static_cast<int>(pSpec->iFrameHeight);
	if (iFrameHeight < 0) iFrameHeight = 0;
	int iMinOverlapX = static_cast<int>(pSpec->iMinOverlapX);
	if (iMinOverlapX < 0) iMinOverlapX = 0;
	int iMinOverlapY = static_cast<int>(pSpec->iMinOverlapY);
	if (iMinOverlapY < 0) iMinOverlapY = 0;
	int iMinMarginX = static_cast<int>(pSpec->iMinMarginX);
	if (iMinMarginX < 0) iMinMarginX = 0;
	int iMinMarginY = static_cast<int>(pSpec->iMinMarginY);
	if (iMinMarginY < 0) iMinMarginY = 0;

	int iCellColNum = static_cast<int>(pSpec->iCellColNum);
	if (iCellColNum <= 0)
	{
		AfxMessageBox(_T("Cell Col 俺荐 涝仿 蔼捞 棵官福瘤 臼嚼聪促."));
		return RESULT_BAD;
	}

	int iCellRowNum = static_cast<int>(pSpec->iCellRowNum);
	if (iCellRowNum <= 0)
	{
		AfxMessageBox(_T("Cell Row 俺荐 涝仿 蔼捞 棵官福瘤 臼嚼聪促."));
		return RESULT_BAD;
	}

	int iCellMinOverlapX = static_cast<int>(pSpec->iCellMinOverlapX);
	if (iCellMinOverlapX < 0) iCellMinOverlapX = 0;
	int iCellMinOverlapY = static_cast<int>(pSpec->iCellMinOverlapY);
	if (iCellMinOverlapY < 0) iCellMinOverlapY = 0;

	m_stFovInfo.stBasic.scanType = m_pSystemSpec->sysBasic.ScanType;
	m_stFovInfo.stBasic.fovType = m_pUserSetInfo->fovType;
	m_stFovInfo.stBasic.resolution = dResolution;
	m_stFovInfo.stBasic.fovSize.cx = iFrameWidth;
	m_stFovInfo.stBasic.fovSize.cy = iFrameHeight;
	m_stFovInfo.stBasic.fovOverlap.cx = iMinOverlapX;
	m_stFovInfo.stBasic.fovOverlap.cy = iMinOverlapY;
	m_stFovInfo.stBasic.fovMargin.cx = iMinMarginX;
	m_stFovInfo.stBasic.fovMargin.cy = iMinMarginY;
	m_stFovInfo.stBasic.dummyMargin.x = dMarginX;
	m_stFovInfo.stBasic.dummyMargin.y = dMarginY;

	m_stCellInfo.stBasic.cellNum.cx = iCellColNum;
	m_stCellInfo.stBasic.cellNum.cy = iCellRowNum;
	m_stCellInfo.stBasic.cellOverlap.cx = iCellMinOverlapX;
	m_stCellInfo.stBasic.cellOverlap.cy = iCellMinOverlapY;

	return RESULT_GOOD;
}

UINT32 CAOI::CheckSwathNum(RECTD &rcWorkRect, double &dResolMM, int &iSwathNum)
{
	double extend = 0;
	if (rcWorkRect.left > rcWorkRect.right) SWAP(rcWorkRect.left, rcWorkRect.right, extend);
	if (rcWorkRect.bottom > rcWorkRect.top) SWAP(rcWorkRect.bottom, rcWorkRect.top, extend);

	double dProdWidthMM = fabs(rcWorkRect.right - rcWorkRect.left);
	double dProdHeightMM = fabs(rcWorkRect.bottom - rcWorkRect.top);

	double dProdWidthPxl = Round(dProdWidthMM / dResolMM + 0.01);
	double dProdHeightPxl = Round(dProdHeightMM / dResolMM + 0.01);

	int iScanWidth = m_stFovInfo.stBasic.fovSize.cx - m_stFovInfo.stBasic.fovOverlap.cx;
	int iProdWidth = static_cast<int>(dProdWidthPxl);
	int iProdHeight = static_cast<int>(dProdHeightPxl);

	//Calculate # of Swath

	if (m_stFovInfo.stBasic.fovSize.cx >= iProdWidth)
		iSwathNum = 1;
	else
	{
		double dNx = (double)iProdWidth / m_stFovInfo.stBasic.fovSize.cx;
		int iMinNx = (int)ceil(dNx);

		if (iProdWidth > (iMinNx - 1) * iScanWidth + m_stFovInfo.stBasic.fovSize.cx)
			iMinNx += 1;

		dNx = ((double)(iProdWidth - m_stFovInfo.stBasic.fovSize.cx) / iScanWidth) + 1;
		int iMaxNx = (int)ceil(dNx);

		if (iMaxNx < iMinNx) iMaxNx = iMinNx;

		for (int i = iMinNx; i <= iMaxNx; i++)
		{
			int iTotScanWidth = (i - 1) * iScanWidth + m_stFovInfo.stBasic.fovSize.cx;
			int iDiffWidth = iTotScanWidth - iProdWidth;

			if (iDiffWidth >= 0)
			{
				iSwathNum = i;
				break;
			}
		}
	}

	if (iSwathNum <= 0)	return RESULT_BAD;
	if (m_stFovInfo.stBasic.fovSize.cx < iProdWidth && iProdWidth >(iSwathNum - 1) * iScanWidth + m_stFovInfo.stBasic.fovSize.cx)
		iSwathNum += 1;

	return RESULT_GOOD;
}

UINT32 CAOI::CalculateOverlap_bw_Swath()
{
	CString strLog;

	BOOL bLeft = FALSE, bBot = FALSE;
	int optNx = m_stFovInfo.stBasic.swathNum;
	int optNy = 1;

	int iCol, iRow, rindex;
	for (iCol = 0; iCol < optNx; iCol++)
	{
		if (bLeft)
		{
			for (iRow = optNy - 1, rindex = 0; iRow >= 0; iRow--, rindex++)
			{
				int iCell = optNy * iCol + rindex;
				if (iRow > 0)
				{
					m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.top = m_stFovInfo.vcFovData[optNy * iCol + rindex + 1]->rcRectPxl.bottom - m_stFovInfo.vcFovData[iCell]->rcRectPxl.top + 1;
				}

				if (iRow + 1 < optNy)
				{
					m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.bottom = m_stFovInfo.vcFovData[iCell]->rcRectPxl.bottom + m_stFovInfo.stBasic.fovSize.cy - m_stFovInfo.vcFovData[optNy * iCol + rindex - 1]->rcRectPxl.bottom;
				}

				if (iCol > 0)
				{
					m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.left = m_stFovInfo.vcFovData[optNy * (iCol - 1) + iRow]->rcRectPxl.right - m_stFovInfo.vcFovData[iCell]->rcRectPxl.left + 1;
				}

				if (iCol + 1 < optNx)
				{
					m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.right = m_stFovInfo.vcFovData[iCell]->rcRectPxl.right - m_stFovInfo.vcFovData[optNy * (iCol + 1) + iRow]->rcRectPxl.left + 1;
				}

				if (m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.top < 0)		m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.top = 0;
				if (m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.bottom < 0)	m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.bottom = 0;
				if (m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.left < 0)	m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.left = 0;
				if (m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.right < 0)	m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.right = 0;

				strLog.Format(_T("%d,%d,%d,%d\n"), m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.left, m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.top,
					m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.right, m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.bottom);
				OutputDebugString(strLog);
			}
			bLeft = FALSE;
		}
		else
		{
			for (iRow = 0, rindex = optNy - 1; iRow < optNy; iRow++, rindex)
			{
				int iCell = optNy * iCol + iRow;
				if (iRow > 0)
				{
					m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.top = m_stFovInfo.vcFovData[optNy * iCol + iRow - 1]->rcRectPxl.bottom - m_stFovInfo.vcFovData[iCell]->rcRectPxl.top + 1;
				}

				if (iRow + 1 < optNy)
				{
					m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.bottom = m_stFovInfo.vcFovData[iCell]->rcRectPxl.bottom + m_stFovInfo.stBasic.fovSize.cy - m_stFovInfo.vcFovData[optNy * iCol + iRow + 1]->rcRectPxl.bottom;
				}

				if (iCol > 0)
				{
					m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.left = m_stFovInfo.vcFovData[optNy * (iCol - 1) + rindex]->rcRectPxl.right - m_stFovInfo.vcFovData[iCell]->rcRectPxl.left + 1;
				}

				if (iCol + 1 < optNx)
				{
					m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.right = m_stFovInfo.vcFovData[iCell]->rcRectPxl.right - m_stFovInfo.vcFovData[optNy * (iCol + 1) + rindex]->rcRectPxl.left + 1;
				}

				if (m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.top < 0)		m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.top = 0;
				if (m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.bottom < 0)	m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.bottom = 0;
				if (m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.left < 0)	m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.left = 0;
				if (m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.right < 0)	m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.right = 0;

				strLog.Format(_T("%d,%d,%d,%d\n"), m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.left, m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.top,
					m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.right, m_stFovInfo.vcFovData[iCell]->rcOverlapPxl.bottom);
				OutputDebugString(strLog);
			}
			bLeft = TRUE;
		}
	}

	return RESULT_GOOD;
}

UINT32 CAOI::EstimateSwath()
{
	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	double dResolMM = m_stFovInfo.stBasic.resolution / 1000.0f;

	Orient ori = Orient::Mir0Deg;
	CDataManager::GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, ori);

	CRotMirrArr rotMirrArr;
	rotMirrArr.InputData(0, 0, CDataManager::ReverseOrient(ori), m_pUserSetInfo->mirrorDir);

	CString strLog;
	RECTD rcRectLeftTop, rcWorkRect_In_Panel;

	for (int j = 0; j < (int)m_stPanelInfo.iColStripNumInPanel; j++)
	{
		StripInfo_t* pStripInfo = m_stPanelInfo.vcStripInfo[j * m_stPanelInfo.iRowStripNumInPanel];
		if (pStripInfo == nullptr)
			return RESULT_BAD;

		for (int b = 0; b < (int)pStripInfo->iColBlockNumInStrip; b++)
		{
			RECTD rcWorkRect = pStripInfo->vcBlockInfo[b * pStripInfo->iRowBlockNumInStrip]->rcRect;
			if (m_pSystemSpec->sysBasic.bProfileMode)
				rcWorkRect = pStripInfo->vcBlockInfo[b * pStripInfo->iRowBlockNumInStrip]->rcProfile;

			rcWorkRect_In_Panel = pStripInfo->vcBlockInfo[b * pStripInfo->iRowBlockNumInStrip]->rcRect_In_Panel;

			if (rcWorkRect.IsRectNull())
				return RESULT_BAD;

			for (int i = 0; i < (int)m_stFovInfo.stBasic.swathNum; i++)
			{
				FovData_t* pFov = new FovData_t;
				pFov->swathIdx = i + (j * m_stFovInfo.stBasic.swathNum * pStripInfo->iColBlockNumInStrip) + (b * m_stFovInfo.stBasic.swathNum);
				pFov->fovIdx.x = pFov->swathIdx;
				pFov->fovIdx.y = 0;

				pFov->rcRectMM.left = rcWorkRect.left - m_stFovInfo.stBasic.dummyMargin.x + ((i * (m_stFovInfo.stBasic.fovSize.cx - m_stFovInfo.stBasic.fovOverlap.cx)) * dResolMM) - (m_stFovInfo.stBasic.fovMargin.cx * dResolMM);
				pFov->rcRectMM.right = pFov->rcRectMM.left + (m_stFovInfo.stBasic.fovSize.cx * dResolMM);
				pFov->rcRectMM.top = rcWorkRect.top + m_stFovInfo.stBasic.dummyMargin.y + (m_stFovInfo.stBasic.fovMargin.cy * dResolMM);
				pFov->rcRectMM.bottom = pFov->rcRectMM.top - (m_stFovInfo.stBasic.fovSize.cy * dResolMM);

				pFov->rcRectMM_In_Panel.left = rcWorkRect_In_Panel.left - m_stFovInfo.stBasic.dummyMargin.x + ((i * (m_stFovInfo.stBasic.fovSize.cx - m_stFovInfo.stBasic.fovOverlap.cx)) * dResolMM) - (m_stFovInfo.stBasic.fovMargin.cx * dResolMM);
				pFov->rcRectMM_In_Panel.right = pFov->rcRectMM.left + (m_stFovInfo.stBasic.fovSize.cx * dResolMM);
				pFov->rcRectMM_In_Panel.top = rcWorkRect_In_Panel.top + m_stFovInfo.stBasic.dummyMargin.y + (m_stFovInfo.stBasic.fovMargin.cy * dResolMM);
				pFov->rcRectMM_In_Panel.bottom = pFov->rcRectMM.top - (m_stFovInfo.stBasic.fovSize.cy * dResolMM);

				if (j == 0 && i == 0)
					rcRectLeftTop = pFov->rcRectMM;

				pFov->rcRectPxl.left = static_cast<int>((fabs(pFov->rcRectMM.left - rcRectLeftTop.left) / dResolMM) + 0.5);
				pFov->rcRectPxl.right = pFov->rcRectPxl.left + m_stFovInfo.stBasic.fovSize.cx - 1;
				pFov->rcRectPxl.top = 0;
				pFov->rcRectPxl.bottom = m_stFovInfo.stBasic.fovSize.cy - 1;

				if (fabs(pFov->rcRectMM.left) < EPSILON_DELTA1)		pFov->rcRectMM.left = 0;
				if (fabs(pFov->rcRectMM.right) < EPSILON_DELTA1)	pFov->rcRectMM.right = 0;
				if (fabs(pFov->rcRectMM.top) < EPSILON_DELTA1)		pFov->rcRectMM.top = 0;
				if (fabs(pFov->rcRectMM.bottom) < EPSILON_DELTA1)	pFov->rcRectMM.bottom = 0;

				rotMirrArr.FinalPoint(&pFov->rcDraw.left, &pFov->rcDraw.top, pFov->rcRectMM.left, pFov->rcRectMM.top);
				rotMirrArr.FinalPoint(&pFov->rcDraw.right, &pFov->rcDraw.bottom, pFov->rcRectMM.right, pFov->rcRectMM.bottom);
				
				if (RESULT_GOOD != ChangeToDrawCoordinate(pFov->rcDraw, m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, m_pUserSetInfo->mirrorDir))
					return RESULT_BAD;

				//strLog.Format(_T("Swath [X%d, Y%d] : (L,T,R,B) (%d,%d,%d,%d)\n"),  pFov->fovIdx.x, pFov->fovIdx.y, pFov->rcRectPxl.left, pFov->rcRectPxl.top, pFov->rcRectPxl.right, pFov->rcRectPxl.bottom);
				//OutputDebugString(strLog);

				strLog.Format(_T("Swath [X%d, Y%d] : (L,T,R,B) (%.2f,%.2f,%.2f,%.2f)\n"), pFov->fovIdx.x, pFov->fovIdx.y, pFov->rcRectMM.left, pFov->rcRectMM.top, pFov->rcRectMM.right, pFov->rcRectMM.bottom);
				OutputDebugString(strLog);

				m_stFovInfo.vcFovData.emplace_back(pFov);
			}
		}
	}

	if (m_stFovInfo.vcFovData.size() != m_stFovInfo.stBasic.swathNum * (m_stPanelInfo.iColStripNumInPanel * m_stPanelInfo.vcStripInfo[0]->iColBlockNumInStrip))
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CAOI::EstimateSwath_Based_On_Block(SIZE &szBlock)
{
	if (RESULT_GOOD != CheckStructure())
		return RESULT_BAD;

	double dResolMM = m_stFovInfo.stBasic.resolution / 1000.0f;

	Orient ori = Orient::Mir0Deg;
	CDataManager::GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, ori);

	CRotMirrArr rotMirrArr;
	rotMirrArr.InputData(0, 0, CDataManager::ReverseOrient(ori), m_pUserSetInfo->mirrorDir);

	CString strLog;
	RECTD rcRectLeftTop;

	std::vector<RectInfo> vcBlockInfo;
	for (int j = 0; j < (int)(m_stPanelInfo.iColStripNumInPanel * m_stPanelInfo.iRowStripNumInPanel); j++)
	{
		StripInfo_t* pStripInfo = m_stPanelInfo.vcStripInfo[j];
		if (pStripInfo == nullptr)
			return RESULT_BAD;

		for (int b = 0; b < (int)(pStripInfo->iColBlockNumInStrip * pStripInfo->iRowBlockNumInStrip); b++)
		{
			RectInfo stBlockInfo;

			stBlockInfo.rcRect = pStripInfo->vcBlockInfo[b]->rcRect;
			if (m_pSystemSpec->sysBasic.bProfileMode)
				stBlockInfo.rcRect = pStripInfo->vcBlockInfo[b]->rcProfile;

			if (stBlockInfo.rcRect.IsRectNull())
				return RESULT_BAD;

			stBlockInfo.rcRect_In_Panel = pStripInfo->vcBlockInfo[b]->rcRect_In_Panel;

			vcBlockInfo.emplace_back(stBlockInfo);
		}
	}

	std::sort(vcBlockInfo.begin(), vcBlockInfo.end(), RectInfo::comp);

	int iColBlockNum = m_stPanelInfo.iColStripNumInPanel * m_stPanelInfo.vcStripInfo[0]->iColBlockNumInStrip;
	int iRowBlockNum = m_stPanelInfo.iRowStripNumInPanel * m_stPanelInfo.vcStripInfo[0]->iRowBlockNumInStrip;
	
	int iBlockIdx = 0;
	for (auto itBlock : vcBlockInfo)
	{
		RECTD rcWorkRect = itBlock.rcRect;
		RECTD rcWorkRect_In_Panel = itBlock.rcRect_In_Panel;

		int iSwathX = iBlockIdx / iRowBlockNum;
		int iSwathY = iBlockIdx % iRowBlockNum;
		for (int i = 0; i < (int)m_stFovInfo.stBasic.swathNum; i++)
		{
			FovData_t* pFov = new FovData_t;
			pFov->swathIdx = i + (iSwathX * m_stFovInfo.stBasic.swathNum);
			pFov->fovIdx.x = pFov->swathIdx;
			pFov->fovIdx.y = iSwathY;

			pFov->rcRectMM.left = rcWorkRect.left - m_stFovInfo.stBasic.dummyMargin.x + ((i * (m_stFovInfo.stBasic.fovSize.cx - m_stFovInfo.stBasic.fovOverlap.cx)) * dResolMM) - (m_stFovInfo.stBasic.fovMargin.cx * dResolMM);
			pFov->rcRectMM.right = pFov->rcRectMM.left + (m_stFovInfo.stBasic.fovSize.cx  * dResolMM);
			pFov->rcRectMM.top = rcWorkRect.top + m_stFovInfo.stBasic.dummyMargin.y + (m_stFovInfo.stBasic.fovMargin.cy * dResolMM);
			pFov->rcRectMM.bottom = pFov->rcRectMM.top - (szBlock.cy * dResolMM);

			pFov->rcRectMM_In_Panel.left = rcWorkRect_In_Panel.left - m_stFovInfo.stBasic.dummyMargin.x + ((i * (m_stFovInfo.stBasic.fovSize.cx - m_stFovInfo.stBasic.fovOverlap.cx)) * dResolMM) - (m_stFovInfo.stBasic.fovMargin.cx * dResolMM);
			pFov->rcRectMM_In_Panel.right = pFov->rcRectMM.left + (m_stFovInfo.stBasic.fovSize.cx  * dResolMM);
			pFov->rcRectMM_In_Panel.top = rcWorkRect_In_Panel.top + m_stFovInfo.stBasic.dummyMargin.y + (m_stFovInfo.stBasic.fovMargin.cy * dResolMM);
			pFov->rcRectMM_In_Panel.bottom = pFov->rcRectMM.top - (szBlock.cy * dResolMM);

			if (iBlockIdx == 0 && i == 0)
				rcRectLeftTop = pFov->rcRectMM;

			pFov->rcRectPxl.left = static_cast<int>((fabs(pFov->rcRectMM.left - rcRectLeftTop.left) / dResolMM) + 0.5);
			pFov->rcRectPxl.right = pFov->rcRectPxl.left + m_stFovInfo.stBasic.fovSize.cx - 1;
			pFov->rcRectPxl.top = static_cast<int>((fabs(pFov->rcRectMM.top - rcRectLeftTop.top) / dResolMM) + 0.5);
			pFov->rcRectPxl.bottom = pFov->rcRectPxl.top + szBlock.cy - 1;

			if (fabs(pFov->rcRectMM.left) < EPSILON_DELTA1)		pFov->rcRectMM.left = 0;
			if (fabs(pFov->rcRectMM.right) < EPSILON_DELTA1)	pFov->rcRectMM.right = 0;
			if (fabs(pFov->rcRectMM.top) < EPSILON_DELTA1)		pFov->rcRectMM.top = 0;
			if (fabs(pFov->rcRectMM.bottom) < EPSILON_DELTA1)	pFov->rcRectMM.bottom = 0;

			rotMirrArr.FinalPoint(&pFov->rcDraw.left, &pFov->rcDraw.top, pFov->rcRectMM.left, pFov->rcRectMM.top);
			rotMirrArr.FinalPoint(&pFov->rcDraw.right, &pFov->rcDraw.bottom, pFov->rcRectMM.right, pFov->rcRectMM.bottom);

			if (RESULT_GOOD != ChangeToDrawCoordinate(pFov->rcDraw, m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, m_pUserSetInfo->mirrorDir))
				return RESULT_BAD;

			strLog.Format(_T("Swath [X%d, Y%d] : (L,T,R,B) (%d,%d,%d,%d)\n"), pFov->fovIdx.x, pFov->fovIdx.y, pFov->rcRectPxl.left, pFov->rcRectPxl.top, pFov->rcRectPxl.right, pFov->rcRectPxl.bottom);
			OutputDebugString(strLog);

//			strLog.Format(_T("Swath [X%d, Y%d] : (L,T,R,B) (%.2f,%.2f,%.2f,%.2f)\n"), pFov->fovIdx.x, pFov->fovIdx.y, pFov->rcRectMM.left, pFov->rcRectMM.top, pFov->rcRectMM.right, pFov->rcRectMM.bottom);
//			OutputDebugString(strLog);

			m_stFovInfo.vcFovData_Block.emplace_back(pFov);
		}

		iBlockIdx++;
	}

	if (m_stFovInfo.vcFovData_Block.size() != m_stFovInfo.stBasic.swathNum * iColBlockNum * iRowBlockNum)
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CAOI::CalculateOverlap_bw_Cell()
{
	int iCellNum = static_cast<int>(m_stCellInfo.vcCellData.size());
	if (iCellNum <= 0 || m_stFovInfo.stBasic.swathNum <= 0) 
		return RESULT_BAD;

	int iCellColNum = m_stCellInfo.stBasic.cellNum.cx;	
	iCellColNum /= m_stFovInfo.stBasic.swathNum;
	if (iCellColNum <= 0) return RESULT_BAD;

	int iCellRowNum = m_stCellInfo.stBasic.cellNum.cy;
	if (iCellRowNum <= 0) return RESULT_BAD;

	CString strLog;

	// Calculate Overlap between Cell	
	for (auto itCell : m_stCellInfo.vcCellData)
	{
		int iCell = itCell->swathIdx * (iCellColNum * iCellRowNum) + (iCellColNum * itCell->fovIdx.y) + itCell->fovIdx.x;
		if (iCell >= iCellNum)
			return RESULT_BAD;

		int iLeft = -1;
		if (itCell->fovIdx.x == 0 && itCell->swathIdx > 0)
			iLeft = (itCell->swathIdx - 1) * (iCellColNum * iCellRowNum) + (iCellColNum * itCell->fovIdx.y) + iCellColNum - 1;
		else if (itCell->fovIdx.x > 0 && itCell->fovIdx.x < iCellColNum)
			iLeft = itCell->swathIdx * (iCellColNum * iCellRowNum) + (iCellColNum * itCell->fovIdx.y) + itCell->fovIdx.x - 1;

		int iRight = -1;
		if (itCell->fovIdx.x + 1 == iCellColNum)
			iRight = (itCell->swathIdx + 1) * (iCellColNum * iCellRowNum) + (iCellColNum * itCell->fovIdx.y);
		else if (itCell->fovIdx.x < iCellColNum)
			iRight = itCell->swathIdx * (iCellColNum * iCellRowNum) + (iCellColNum * itCell->fovIdx.y) + itCell->fovIdx.x + 1;

		int iTop = -1;
		if (itCell->fovIdx.y > 0 && itCell->fovIdx.y < iCellRowNum)
			iTop = itCell->swathIdx * (iCellColNum * iCellRowNum) + (iCellColNum * (itCell->fovIdx.y - 1)) + itCell->fovIdx.x;

		int iBottom = -1;
		if (itCell->fovIdx.y + 1 < iCellRowNum)
			iBottom = itCell->swathIdx * (iCellColNum * iCellRowNum) + (iCellColNum * (itCell->fovIdx.y + 1)) + itCell->fovIdx.x;

		if (iLeft >= 0 && iLeft < iCellNum)
			m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.left = m_stCellInfo.vcCellData[iLeft]->rcRectPxl.right - m_stCellInfo.vcCellData[iCell]->rcRectPxl.left + 1;
		if (iTop >= 0 && iTop < iCellNum)
			m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.top = m_stCellInfo.vcCellData[iTop]->rcRectPxl.bottom - m_stCellInfo.vcCellData[iCell]->rcRectPxl.top + 1;
		if (iRight >= 0 && iRight < iCellNum)
			m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.right = m_stCellInfo.vcCellData[iCell]->rcRectPxl.right - m_stCellInfo.vcCellData[iRight]->rcRectPxl.left + 1;
		if (iBottom >= 0 && iBottom < iCellNum)
			m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.bottom = m_stCellInfo.vcCellData[iCell]->rcRectPxl.bottom - m_stCellInfo.vcCellData[iBottom]->rcRectPxl.top + 1;

		if (m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.left < 0)		m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.left = 0;
		if (m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.top < 0)		m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.top = 0;
		if (m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.right < 0)		m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.right = 0;
		if (m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.bottom < 0)	m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.bottom = 0;

		strLog.Format(_T("%d,%d,%d,%d,%d\n"), iCell + 1, m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.left, m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.top,
			m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.right, m_stCellInfo.vcCellData[iCell]->rcOverlapPxl.bottom);
		OutputDebugString(strLog);
	}

	return RESULT_GOOD;
}

CString CAOI::GetModelPath(IN bool bCreateFolder)
{	
	if (RESULT_GOOD != CheckStructure())
		return L"";

	if (m_pJobData->m_Misc.GetModelName().GetLength() < 1)
		return L"";

	CString strWorkLayer = m_pUserSetInfo->strWorkLayer;

	CString strModelPath = L"";

	strModelPath.Format(L"%s\\%s", m_pSystemSpec->sysPath.strMasterPath, m_pJobData->m_Misc.GetModelName());
	if (bCreateFolder)
		CreateDirectory(strModelPath, NULL);

	strModelPath.Format(L"%s\\%s\\%s", m_pSystemSpec->sysPath.strMasterPath, m_pJobData->m_Misc.GetModelName(), strWorkLayer);
	if (bCreateFolder)
		CreateDirectory(strModelPath, NULL);

	if (strModelPath.GetLength() < 1)
		return L"";

	return strModelPath;
}
