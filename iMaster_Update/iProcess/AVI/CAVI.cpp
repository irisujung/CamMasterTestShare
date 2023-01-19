#include "../stdafx.h"
#include "CAVI.h"
#include "../../iUtilities/PerfTimerEx.h"

#include <float.h>

CAVI::CAVI()
{
	
}

CAVI::~CAVI()
{

}

UINT32 CAVI::Destory()
{
	return RESULT_GOOD;
}


UINT32 CAVI::MakeMasterData()
{
	if (m_pJobData == nullptr || m_pUserLayerSetInfo == nullptr ||
		m_pUserSetInfo == nullptr || m_pSystemSpec == nullptr)
		return RESULT_BAD;

	if (m_pJobData->m_Misc.GetModelName().GetLength() < 1)
		return RESULT_BAD;

	CString strWorkLayer = m_pUserSetInfo->strWorkLayer;

	CString strModelPath = L"";
	strModelPath.Format(L"%s\\%s", m_pSystemSpec->sysPath.strMasterPath, m_pJobData->m_Misc.GetModelName());
	CreateDirectory(strModelPath, NULL);

	strModelPath.Format(L"%s\\%s\\%s", m_pSystemSpec->sysPath.strMasterPath, m_pJobData->m_Misc.GetModelName(), strWorkLayer);
	CreateDirectory(strModelPath, NULL);
	m_strModelPath = strModelPath;

	if (m_strModelPath.GetLength() < 1)
		return RESULT_BAD;

	//선택된 스텝을 강제로 판넬스텝으로 변경
	int nSelectStep_Pre = m_nSelectStep;
	//Panel Step	
	if (m_nSelectStep != m_pUserLayerSetInfo->nPanelStepIdx)
		m_nSelectStep = m_pUserLayerSetInfo->nPanelStepIdx;

	if (RESULT_GOOD == OptimizeFOV(TRUE))
	{
		//Delete Folder
		CString strRemovePathName = m_strModelPath + _T("\\AlignMark");
		CUtils::RemoveDirectoryFile(strRemovePathName);	

		// 2023.01.17 KJH ADD
		// Time Check 
		CPerfTimerEx timerEx;
		timerEx.StartTimer();

		//Align Image 저장
		if (RESULT_GOOD != MakePanelAlignPointImage())
			return RESULT_BAD;

		double dMakePanelAlignPointImage = timerEx.StopTimer(CTimerUnit_en::EN_MIL_SEC);

		timerEx.StartTimer();

		//Unit Align : Master Image
		if (RESULT_GOOD != MakeUnitAlignPointImage())
			return RESULT_BAD;

		double dMakeUnitAlignPointImage = timerEx.StopTimer(CTimerUnit_en::EN_MIL_SEC);

		timerEx.StartTimer();

		if (m_pSystemSpec->sysBasic.bExceptImageData == false)
		{//ExceptImageData == false 일경우만 InspImage 저장

			//Master image 저장
 			if (RESULT_GOOD != MakeMasterImage())
 				return RESULT_BAD;
		}
		double dMakeMasterImage = timerEx.StopTimer(CTimerUnit_en::EN_MIL_SEC);

		CString str;
		str.Format(_T("\n MakePanelAlignPointImage : %.2lf, MakeUnitAlignPointImage : %.2lf, MakeMasterImage : %.2lf\n"), 
			dMakePanelAlignPointImage / 1000. / 60.,
			dMakeUnitAlignPointImage / 1000. / 60.,
			dMakeMasterImage / 1000. / 60.);
		OutputDebugString(str);
	}
	else
	{
		AfxMessageBox(L"Failed to Optimize FOV");
		return RESULT_BAD;
	}

	m_nSelectStep = nSelectStep_Pre;

	return RESULT_GOOD;
}

UINT32 CAVI::OptimizeFOV(IN BOOL bSave, IN BOOL bUpdate)
{
	if (m_pJobData == nullptr || m_pUserSetInfo == nullptr)
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
		if (RESULT_GOOD != EstimateFOV_Based_On_Swath(m_nSelectStep, orient))
			return RESULT_BAD;

		//Estimate Cell Info
		if (RESULT_GOOD != EstimateCell_Based_On_Swath(m_nSelectStep, orient))
			return RESULT_BAD;

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

		if (RESULT_GOOD != SaveCellInfo())
			return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 CAVI::EstimateProductInfo(IN INT32 iStepIdx, IN enum Orient eOrient)
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
	if (RESULT_GOOD != RearrageIndexInfo(eOrient))
		return RESULT_BAD;

	//5. Get Block Rect 
	if (RESULT_GOOD != EstimateBlockInfo())
		return RESULT_BAD;

	//6. Get Align Mark Info (SR_NEW_MANUAL_EXPO)
	if (RESULT_GOOD != ExtractAlignMarkInfo(iStepIdx, eOrient))
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CAVI::CheckIf_SubStripStep_Exist(IN INT32 iStepIdx)
{
	m_bIs4Step = FALSE;

	if (iStepIdx < 0 || m_pJobData == nullptr ||
		m_pUserLayerSetInfo == nullptr)
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
			CStepRepeat *pSubStepRepeat = &(m_pJobData->m_arrStep[iIdx]->m_arrSubStep[j]->m_arrStepRepeat);
			if (pSubStepRepeat == nullptr)
				continue;

			for (auto it : m_pUserLayerSetInfo->vcStripStepIdx)
			{
				CString strStripStepName = m_pJobData->m_arrStep[it]->m_strStepName;
				if (pSubStepRepeat->m_strName.CompareNoCase(strStripStepName) == 0)
				{
					m_bIs4Step = TRUE;
					break;
				}
			}

			if (m_bIs4Step)
				break;
		}
	}

	return RESULT_GOOD;
}

UINT32 CAVI::EstimatePanelInfo(IN INT32 iStepIdx, IN enum Orient eOrient)
{
	if (iStepIdx < 0 || m_pJobData == nullptr || m_pUserSetInfo == nullptr ||
		m_pUserLayerSetInfo == nullptr || m_pSystemSpec == nullptr)
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

UINT32 CAVI::EstimateStripInfo(IN INT32 iStepIdx, IN enum Orient eOrient)
{
	if (iStepIdx < 0 || m_pJobData == nullptr || m_pSystemSpec == nullptr ||
		m_pUserSetInfo == nullptr || m_pUserLayerSetInfo == nullptr)
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

	double dWidth = fabs(m_stPanelInfo.vcStripInfo[0]->rcProfile.right - m_stPanelInfo.vcStripInfo[0]->rcProfile.left);
	double dHeight = fabs(m_stPanelInfo.vcStripInfo[0]->rcProfile.top - m_stPanelInfo.vcStripInfo[0]->rcProfile.bottom);

	if (dWidth > dHeight)
		m_stPanelInfo.bIsStripVertical = false;
	else
		m_stPanelInfo.bIsStripVertical = true;

	return RESULT_GOOD;
}

UINT32 CAVI::GetStripNum_In_Panel(IN INT32 iStepIdx, OUT UINT32& iTotalStripNum)
{
	if (iStepIdx < 0 || m_pJobData == nullptr ||
		m_pUserLayerSetInfo == nullptr)
		return RESULT_BAD;

	if (iStepIdx != m_pUserLayerSetInfo->nPanelStepIdx)
		iStepIdx = m_pUserLayerSetInfo->nPanelStepIdx;

	for (auto it : m_pUserLayerSetInfo->vcStripStepIdx)
	{
		CString strStripStepName = m_pJobData->m_arrStep[it]->m_strStepName;
		UINT32 iStripNum = 0;

		for (UINT32 i = 0; i < m_pJobData->m_arrStep[iStepIdx]->m_nSubStepCnt; i++)
		{
			CStepRepeat *pSubStepRepeat = m_pJobData->m_arrStep[iStepIdx]->m_Stephdr.m_arrStepRepeat[i];
			if (pSubStepRepeat == nullptr)
				return FALSE;

			UINT32 iSubStepNum = pSubStepRepeat->m_nNY * pSubStepRepeat->m_nNX;
			CalculateStripNumOnSubStep(TRUE, strStripStepName, m_pJobData->m_arrStep[iStepIdx]->m_arrSubStep[i], &iStripNum);

			if (m_bIs4Step)
			{
				iStripNum *= iSubStepNum;
			}
			else
			{
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

UINT32 CAVI::CalculateStripNumOnSubStep(IN BOOL bFirstStep, IN CString strStripStepName, IN CSubStep* pSubStep, OUT UINT32 *pStripNum)
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

UINT32 CAVI::CalculateStripCoord(IN BOOL bFirstStep, IN UINT iShapeIdx, IN INT32 iStepIdx, IN enum Orient eOrient, IN CString strStripStepName, IN CRotMirrArr *pRotMirrArr, IN CSubStep *pSubStep, OUT UINT32 *pRgnNum)
{
	if (pSubStep == nullptr || m_pUserSetInfo == nullptr || m_pJobData == nullptr)
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
				pStrip->iType = iShapeIdx;
				pStrip->strStepName = strStripStepName;
				pStrip->iStripIdx = static_cast<UINT32>(m_stPanelInfo.vcStripInfo.size());//x_step * pSubStepRepeat->m_nNY + y_step;
				pStrip->rcRect = tmpMinMaxRect;
				pStrip->rcProfile = tmpProfileRect;

				//	if (m_pSystemSpec->sysBasic.bProfileMode)
				pStrip->rcRect_In_Panel = OrgProfileRect;
				//	else
				//		pStrip->rcRect_In_Panel = OrgMinMaxRect;

				pStrip->rcRect.left = fabs(pStrip->rcRect.left - m_stPanelInfo.rcRect.left);
				pStrip->rcRect.right = fabs(pStrip->rcRect.right - m_stPanelInfo.rcRect.left);
				pStrip->rcRect.top = fabs(pStrip->rcRect.top - m_stPanelInfo.rcRect.bottom);
				pStrip->rcRect.bottom = fabs(pStrip->rcRect.bottom - m_stPanelInfo.rcRect.bottom);

				if (pStrip->rcRect.right < pStrip->rcRect.left)	SWAP(pStrip->rcRect.left, pStrip->rcRect.right, dTmpx);
				if (pStrip->rcRect.top < pStrip->rcRect.bottom)	SWAP(pStrip->rcRect.top, pStrip->rcRect.bottom, dTmpy);

				pStrip->rcProfile.left = fabs(pStrip->rcProfile.left - m_stPanelInfo.rcRect.left);
				pStrip->rcProfile.right = fabs(pStrip->rcProfile.right - m_stPanelInfo.rcRect.left);
				pStrip->rcProfile.top = fabs(pStrip->rcProfile.top - m_stPanelInfo.rcRect.bottom);
				pStrip->rcProfile.bottom = fabs(pStrip->rcProfile.bottom - m_stPanelInfo.rcRect.bottom);

				if (pStrip->rcProfile.right < pStrip->rcProfile.left)	SWAP(pStrip->rcProfile.left, pStrip->rcProfile.right, dTmpx);
				if (pStrip->rcProfile.top < pStrip->rcProfile.bottom)	SWAP(pStrip->rcProfile.top, pStrip->rcProfile.bottom, dTmpy);

				enum Orient eStripOrient = AddOrient(eOrient, eSubOrient);
				enum Orient eTotOrient = Orient::NoMir0Deg;
				CalculateOrient(eStripOrient, &eTotOrient);
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

UINT32 CAVI::CalculateOrient(IN enum Orient curOrient, OUT enum Orient* pTotOrient)
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

UINT32 CAVI::EstimateUnitInfo(IN INT32 iStepIdx, IN enum Orient eOrient)
{
	//[TBD] 시간있을 때 간략하게 정리하자....

	if (iStepIdx < 0 || m_pJobData == nullptr ||
		m_pUserLayerSetInfo == nullptr)
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
			itStrip->vcUnitInfo.clear();

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
				//Block에 있는 데이터를 업데이트 했으니, 스트립에 있는 포인터도 업데이트
				itStrip->vcUnitInfo.clear();
				for (auto pBlock : itStrip->vcBlockInfo)
				{
					for (auto pUnitInfo : pBlock->vcUnitInfo)
					{
						itStrip->vcUnitInfo.push_back(pUnitInfo);
					}
				}
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

						//Block에 있는 데이터를 업데이트 했으니, 스트립에 있는 포인터도 업데이트
						itStrip->vcUnitInfo.clear();
						for (auto pBlock : itStrip->vcBlockInfo)
						{
							for (auto pUnitInfo : pBlock->vcUnitInfo)
							{
								itStrip->vcUnitInfo.push_back(pUnitInfo);
							}
						}
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

						//Block에 있는 데이터를 업데이트 했으니, 스트립에 있는 포인터도 업데이트
						itStrip->vcUnitInfo.clear();
						for (auto pBlock : itStrip->vcBlockInfo)
						{
							for (auto pUnitInfo : pBlock->vcUnitInfo)
							{
								itStrip->vcUnitInfo.push_back(pUnitInfo);
							}
						}
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

UINT32 CAVI::CalculateUnitCoord(IN BOOL bFirstStep, IN UINT iShapeIdx, IN INT32 iStepIdx, IN enum Orient eOrient, IN CString strUnitStepName, IN CRotMirrArr *pRotMirrArr, IN CSubStep *pSubStep, OUT UINT32 *pRgnNum)
{
	if (pSubStep == nullptr || pRgnNum == nullptr || m_pUserSetInfo == nullptr ||
		m_pJobData == nullptr || m_pUserLayerSetInfo == nullptr)
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
				UnitInfo_t *pUnit = new UnitInfo_t;
				pUnit->iType = iShapeIdx;
				pUnit->iStripIdx = iStepIdx;
				pUnit->iUnitIdx = *pRgnNum;
				pUnit->rcRect = tmpMinMaxRect;
				pUnit->rcProfile = tmpProfileRect;

				//		if (m_pSystemSpec->sysBasic.bProfileMode)
				pUnit->rcRect_In_Panel = OrgProfileRect;
				//		else
				//			pUnit->rcRect_In_Panel = OrgMinMaxRect;

				pUnit->rcRect.left = fabs(pUnit->rcRect.left - m_stPanelInfo.rcRect.left);
				pUnit->rcRect.right = fabs(pUnit->rcRect.right - m_stPanelInfo.rcRect.left);
				pUnit->rcRect.top = fabs(pUnit->rcRect.top - m_stPanelInfo.rcRect.bottom);
				pUnit->rcRect.bottom = fabs(pUnit->rcRect.bottom - m_stPanelInfo.rcRect.bottom);

				if (pUnit->rcRect.right < pUnit->rcRect.left)	SWAP(pUnit->rcRect.left, pUnit->rcRect.right, dTmpx);
				if (pUnit->rcRect.top < pUnit->rcRect.bottom)	SWAP(pUnit->rcRect.top, pUnit->rcRect.bottom, dTmpy);

				pUnit->rcProfile.left = fabs(pUnit->rcProfile.left - m_stPanelInfo.rcRect.left);
				pUnit->rcProfile.right = fabs(pUnit->rcProfile.right - m_stPanelInfo.rcRect.left);
				pUnit->rcProfile.top = fabs(pUnit->rcProfile.top - m_stPanelInfo.rcRect.bottom);
				pUnit->rcProfile.bottom = fabs(pUnit->rcProfile.bottom - m_stPanelInfo.rcRect.bottom);

				if (pUnit->rcProfile.right < pUnit->rcProfile.left)	SWAP(pUnit->rcProfile.left, pUnit->rcProfile.right, dTmpx);
				if (pUnit->rcProfile.top < pUnit->rcProfile.bottom)	SWAP(pUnit->rcProfile.top, pUnit->rcProfile.bottom, dTmpy);

				enum Orient eUnitOrient = AddOrient(eOrient, eSubOrient);
				enum Orient eTotOrient = Orient::NoMir0Deg;
				CalculateOrient(eUnitOrient, &eTotOrient);
				pUnit->iOrient = static_cast<UINT32>(eTotOrient);

				//SetUnitMask
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

UINT32 CAVI::GetStripIndex(IN BOOL bFirstStep, IN CString strStepName, IN CString strUnitStepName, IN RECTD rcStepRect, OUT IN INT32 &iStripIdx)
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

	RECTD rcProfile = rcStepRect;
	rcProfile.left = fabs(rcProfile.left - m_stPanelInfo.rcRect.left);
	rcProfile.right = fabs(rcProfile.right - m_stPanelInfo.rcRect.left);
	rcProfile.top = fabs(rcProfile.top - m_stPanelInfo.rcRect.bottom);
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

UINT32 CAVI::RearrageIndexInfo(IN enum Orient eOrient)
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

UINT32 CAVI::SaveProductInfo(IN INT32 iStepIdx, IN enum Orient eOrient)
{
	if (m_pJobData == nullptr || m_pUserLayerSetInfo == nullptr ||
		m_pUserSetInfo == nullptr || m_pSystemSpec == nullptr)
		return RESULT_BAD;

	if (m_pJobData->m_Misc.GetModelName().GetLength() < 1 ||
		m_strModelPath.GetLength() < 1)
		return RESULT_BAD;

	//INT32 iStepIdx = m_pUserSetInfo->iStepIdx;
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

	//0도 Reference
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
		m_pJobData->m_Misc.GetModelName(), pSpec->dResolution / 1000.0, m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror,
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

	//[[STRIP_TYPE]
	strValue.Format(_T("<strip_type count=\"%d\">\r\n"), m_stPanelInfo.vcStripType.size());
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	iIdx = 1;
	for (auto it : m_stPanelInfo.vcStripType)
	{
		strValue.Format(_T("<s%d count=\"%d\" profile_width=\"%.3f\" profile_height=\"%.3f\" feature_width=\"%.3f\" feature_height=\"%.3f\"></s%d>\r\n"),
			iIdx, it->nNum, it->dProfileSize.x, it->dProfileSize.y, it->dFeatureSize.x, it->dFeatureSize.y, iIdx);
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
		strValue.Format(_T("<u%d count=\"%d\" profile_width=\"%.3f\" profile_height=\"%.3f\" feature_width=\"%.3f\" feature_height=\"%.3f\"></u%d>\r\n"),
			iIdx, it->nNum, it->dProfileSize.x, it->dProfileSize.y, it->dFeatureSize.x, it->dFeatureSize.y, iIdx);
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
						iUnitIdx, itUnit->iUnitIdx_Ori/*iUnitIdx*/ + 1, itUnit->iType + 1, itUnit->rcProfile.left, itUnit->rcProfile.top, itUnit->rcProfile.right, itUnit->rcProfile.bottom, iUnitIdx);
				}
				else
				{
					strValue.Format(_T("<u%d index=\"%d\" type=\"%d\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\"></u%d>\r\n"),
						iUnitIdx, itUnit->iUnitIdx_Ori/*iUnitIdx*/ + 1, itUnit->iType + 1, itUnit->rcRect.left, itUnit->rcRect.top, itUnit->rcRect.right, itUnit->rcRect.bottom, iUnitIdx);
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

UINT32 CAVI::ExtractAlignMarkInfo(IN INT32 iStepIdx, IN enum Orient eOrient)
{
	if (iStepIdx < 0 || m_pJobData == nullptr || m_pUserSetInfo == nullptr ||
		m_pUserLayerSetInfo == nullptr || m_pSystemSpec == nullptr)
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

	eUserSite userSite = m_pSystemSpec->sysBasic.UserSite;

	double dResolMM = pSpec->dResolution / 1000.f;
	double dSwathWidthMM = pSpec->iFrameWidth * dResolMM;

	CLayer* pTargetLayer = nullptr;
	CString strTargetLayer = PATTERN_LAYER_TXT;
	CString strAlignMark = ALIGN_SYMBOL2_TXT;
	CString strAlignFeature = _T("");
	int iAlignFeatureNum = 7;
	double dAlignMarginMM = 5 * dResolMM;

	if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS &&
		userSite == eUserSite::eDAEDUCK)
	{
		strTargetLayer = SR_LAYER_TXT;
		strAlignMark = ALIGN_SYMBOL3_TXT;
		strAlignFeature = ALIGN_FEATURE_TXT;
		iAlignFeatureNum = 1;
	}

	for (auto it : m_pUserLayerSetInfo->vcLayerSet)
	{
		if (it->strDefine.CompareNoCase(strTargetLayer) != 0)
			continue;

		for (UINT32 lyno = 0; lyno < pStep->m_arrLayer.GetSize(); lyno++)
		{
			CLayer* pLayer = pStep->m_arrLayer.GetAt(lyno);
			if (pLayer == nullptr) continue;
			if (it->strLayer.CompareNoCase(pLayer->m_strLayerName) != 0) continue;

			pTargetLayer = pLayer;
		}
	}

	if (pTargetLayer == nullptr)
		return RESULT_BAD;

	double extend = 0;
	CRotMirrArr rotMirrArr;
	rotMirrArr.InputData(0.0, 0.0, eOrient, m_pUserSetInfo->mirrorDir);

	BOOL bFindAlignSymbol = FALSE;
	for (int feno = 0; feno < pTargetLayer->m_FeatureFile.m_arrFeature.GetSize(); feno++)
	{
		CFeature* pFeature = pTargetLayer->m_FeatureFile.m_arrFeature.GetAt(feno);
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
		int iFeatureNum = 0;

		for (int feno = 0; feno < pTargetLayer->m_FeatureFile.m_arrFeature.GetSize(); feno++)
		{
			CFeature* pFeature = pTargetLayer->m_FeatureFile.m_arrFeature.GetAt(feno);
			if (pFeature == nullptr)
				continue;

			bool bFindFeature = false;
			for (int txtno = 0; txtno < pFeature->m_arrAttributeTextString.GetSize(); txtno++)
			{
				CAttributeTextString* pTextString = pFeature->m_arrAttributeTextString.GetAt(txtno);
				if (pTextString == nullptr || pTextString->m_strText.CompareNoCase(strAlignMark) != 0)
					continue;

				bFindFeature = true;
				break;
			}

			if (!bFindFeature)
				continue;

			if (userSite == eUserSite::eDAEDUCK)
			{
				if (pFeature->m_eType != FeatureType::P || !pFeature->m_bPolarity)
					continue;

				if (pFeature->m_pSymbol == nullptr)
					continue;

				if (pFeature->m_pSymbol->m_strUserSymbol.CompareNoCase(strAlignFeature) != 0)
					continue;

				iFeatureNum = 0;
			}
			else
			{
				if (pFeature->m_eType == FeatureType::P)
					iFeatureNum = 0;
			}

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

				if (userSite == eUserSite::eDAEDUCK)
				{
					pAlignMark->left   -= dAlignMarginMM;
					pAlignMark->right  += dAlignMarginMM;

					if (pAlignMark->top > pAlignMark->bottom)
					{
						pAlignMark->top	   += dAlignMarginMM;
						pAlignMark->bottom -= dAlignMarginMM;
					}
					else
					{
						pAlignMark->top	   -= dAlignMarginMM;
						pAlignMark->bottom += dAlignMarginMM;
					}
				}

				m_stPanelInfo.vcAlignMarkInfo.emplace_back(pAlignMark);

				bFindAlignSymbol = TRUE;
			}
		}
	}

	rotMirrArr.ClearAll();

	std::sort(m_stPanelInfo.vcAlignMarkInfo.begin(), m_stPanelInfo.vcAlignMarkInfo.end(), tagDoubleRect::comp);

	if (!bFindAlignSymbol)
	{
		if (IDYES == AfxMessageBox(L"첫번째 Swath 내 판넬 얼라인 포인트가 없습니다. 계속 진행 하시겠습니까?", MB_YESNO))
			return RESULT_GOOD;
		else
			return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 CAVI::MakePanelAlignPointImage()
{
	if (m_pJobData == nullptr || m_pUserLayerSetInfo == nullptr ||
		m_pDrawManager == nullptr || m_pSystemSpec == nullptr ||
		m_pUserSetInfo == nullptr)
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
		// Panel 내의 Align Point 위치를 Unit 내 원본 좌표로 변환
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

UINT32 CAVI::EstimateFOV_Based_On_Swath(INT32 iStepIdx, enum Orient eOrient)
{
	m_stFovInfo.Clear();

	if (iStepIdx < 0)
		return RESULT_BAD;

	if (m_pJobData == nullptr || m_pUserSetInfo == nullptr ||
		m_pSystemSpec == nullptr)
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

	double dResolMM = pSpec->dResolution / 1000.f;
	int iFrameWidth = static_cast<int>(pSpec->iFrameWidth);
	int iFrameHeight = static_cast<int>(pSpec->iFrameHeight);
	int iMinOverlapX = static_cast<int>(pSpec->iMinOverlapX);
	int iMinOverlapY = static_cast<int>(pSpec->iMinOverlapY);
	int iMinMarginX = static_cast<int>(pSpec->iMinMarginX);
	int iMinMarginY = static_cast<int>(pSpec->iMinMarginY);

	if (m_pSystemSpec->sysBasic.ScanType == eScanType::eLineScan)
		iMinOverlapY = 0;

	if (dResolMM <= 0 || iFrameWidth <= 0 || iFrameHeight <= 0)
		return RESULT_BAD;

	if (iMinOverlapX < 0) iMinOverlapX = 0;
	if (iMinOverlapY < 0) iMinOverlapY = 0;
	if (iMinMarginX < 0) iMinMarginX = 0;
	if (iMinMarginY < 0) iMinMarginY = 0;

	RECTD rcWorkRect = m_pJobData->m_arrStep[iStepIdx]->m_ActiveRect;
	
	if (m_pSystemSpec->sysBasic.bExceptDummy)
	{
		rcWorkRect = m_stPanelInfo.rcRect_In_Panel;

		/*if (m_pSystemSpec->sysBasic.bProfileMode)
			rcWorkRect = m_stPanelInfo.rcProfile_wo_Dummy;
		else
			rcWorkRect = m_stPanelInfo.rcRect_wo_Dummy;*/
	}

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
	m_stFovInfo.stBasic.swathNum = iSwathNum;
	m_stFovInfo.stBasic.scanType = m_pSystemSpec->sysBasic.ScanType;
	m_stFovInfo.stBasic.fovType = m_pUserSetInfo->fovType;
	m_stFovInfo.stBasic.resolution = pSpec->dResolution;
	m_stFovInfo.stBasic.fovSize.cx = iFrameWidth;
	m_stFovInfo.stBasic.fovSize.cy = iProdHeight;
	m_stFovInfo.stBasic.fovOverlap.cx = iMinOverlapX;
	m_stFovInfo.stBasic.fovOverlap.cy = iMinOverlapY;
	m_stFovInfo.stBasic.fovMargin.cx = iMinMarginX;
	m_stFovInfo.stBasic.fovMargin.cy = iMinMarginY;
	m_stFovInfo.stBasic.prodSize.cx = iProdWidth;
	m_stFovInfo.stBasic.prodSize.cy = iProdHeight;

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

		pFov->rcRectPxl.left = i * (iFrameWidth - iMinOverlapX);
		pFov->rcRectPxl.right = pFov->rcRectPxl.left + iFrameWidth - 1;
		pFov->rcRectPxl.top = 0;
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

		str.Format(_T("Swath [%d] : (L,T,R,B) (%.2f,%.2f,%.2f,%.2f)\n"), i, pFov->rcDraw.left, pFov->rcDraw.top, pFov->rcDraw.right, pFov->rcDraw.bottom);
		OutputDebugString(str);
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

UINT32 CAVI::MakeUnitAlignPointImage()
{
	if (m_pDrawManager == nullptr) return RESULT_BAD;

	if (m_pJobData == nullptr || m_pUserLayerSetInfo == nullptr ||
		m_pDrawManager == nullptr || m_pSystemSpec == nullptr)
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

UINT32 CAVI::MakeMasterImage()
{	
	CString strRemovePathName = m_strModelPath + _T("\\Layers");
	CUtils::RemoveDirectoryFile(strRemovePathName);
	//Unit Step	
	CPerfTimerEx timerEx;
	timerEx.StartTimer();

	if (RESULT_GOOD != MakeMasterImage_Unit())
		return RESULT_BAD;

	double dMakeMasterImage_Unit = timerEx.StopTimer(CTimerUnit_en::EN_MIL_SEC);

	timerEx.StartTimer();
	//Strip Step
	if (RESULT_GOOD != MakeMasterImage_Strip())
		return RESULT_BAD;

	double dMakeMasterImage_Strip = timerEx.StopTimer(CTimerUnit_en::EN_MIL_SEC);

	CString str;
	str.Format(_T("\n MakeMasterImage_Unit : %.2lf \n MakeMasterImage_Strip : %.2lf"), 
		dMakeMasterImage_Unit/1000./60.,
		dMakeMasterImage_Strip / 1000. / 60.);
	OutputDebugString(str);

	//Panel Step	
//	if (RESULT_GOOD != MakeMasterImage_Panel())
//		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CAVI::MakeMasterImage_Panel()
{
	if (m_pDrawManager == nullptr) return RESULT_BAD;

	if (m_pJobData == nullptr || m_pUserLayerSetInfo == nullptr ||
		m_pDrawManager == nullptr || m_pSystemSpec == nullptr)
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

	if (RESULT_GOOD != m_pDrawManager->SavePanelLayer(iStepIdx, 0, dCamAngle, dResolMM, m_strModelPath))
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CAVI::MakeMasterImage_Strip()
{
	if (m_pDrawManager == nullptr) return RESULT_BAD;

	if (m_pJobData == nullptr || m_pUserLayerSetInfo == nullptr ||
		m_pDrawManager == nullptr || m_pSystemSpec == nullptr)
		return RESULT_BAD;

	INT32 iStepNum = static_cast<INT32>(m_pJobData->m_arrStep.GetSize());
	if (iStepNum <= 0)
		return RESULT_BAD;

	CameraSpec_t *pSpec = nullptr;
	if (m_pSystemSpec->sysBasic.ScanType == eScanType::eLineScan)
		pSpec = &m_pSystemSpec->sysCamera[true];
	else
		pSpec = &m_pSystemSpec->sysCamera[false];

	if (pSpec == nullptr) return RESULT_BAD;

	double dResolMM = pSpec->dResolution / 1000.f;

	CString strRootPath, strSavePath;
	strRootPath.Format(_T("%s\\Layers"), m_strModelPath);
	CreateDirectory(strRootPath, NULL);
	strRootPath.Format(_T("%s\\Layers\\Strip"), m_strModelPath);
	CreateDirectory(strRootPath, NULL);

	bool bMirror = false;
	double dAngle = 0.;
	UINT32 iStripIdx = 0;
	for (auto itType : m_stPanelInfo.vcStripType)
	{
		INT32 iStepIdx = -1;
		for (INT32 iIdx = 0; iIdx < iStepNum; iIdx++)
		{
			CStep *pStep = m_pJobData->m_arrStep[iIdx];
			if (pStep == nullptr)
				return RESULT_BAD;

			if (itType->strStepName.CompareNoCase(pStep->m_strStepName) == 0)
			{
				iStepIdx = iIdx;
				break;
			}
		}

		if (iStepIdx < 0) continue;

		strSavePath.Format(_T("%s\\S%d"), strRootPath, iStripIdx + 1);
		CreateDirectory(strSavePath, NULL);

		//		Orient ori = ReverseOrient(static_cast<Orient>(itType->iOrient));
		GetAngle(static_cast<Orient>(itType->iOrient), dAngle, bMirror);

		if (RESULT_GOOD != m_pDrawManager->SaveStripLayer(iStepIdx, iStripIdx, dAngle, bMirror, dResolMM, strSavePath))
			return RESULT_BAD;

		iStripIdx++;
	}

	return RESULT_GOOD;
}

UINT32 CAVI::MakeMasterImage_Unit()
{
	if (m_pDrawManager == nullptr) return RESULT_BAD;

	if (m_pJobData == nullptr || m_pUserLayerSetInfo == nullptr ||
		m_pDrawManager == nullptr || m_pSystemSpec == nullptr)
		return RESULT_BAD;

	INT32 iStepNum = static_cast<INT32>(m_pJobData->m_arrStep.GetSize());
	if (iStepNum <= 0)
		return RESULT_BAD;

	CameraSpec_t *pSpec = nullptr;
	if (m_pSystemSpec->sysBasic.ScanType == eScanType::eLineScan)
		pSpec = &m_pSystemSpec->sysCamera[true];
	else
		pSpec = &m_pSystemSpec->sysCamera[false];

	if (pSpec == nullptr) return RESULT_BAD;

	double dResolMM = pSpec->dResolution / 1000.f;

	CString strRootPath, strSavePath;
	strRootPath.Format(_T("%s\\Layers"), m_strModelPath);
	CreateDirectory(strRootPath, NULL);
	strRootPath.Format(_T("%s\\Layers\\Unit"), m_strModelPath);
	CreateDirectory(strRootPath, NULL);

	bool bMirror = false;
	double dAngle = 0.;
	UINT32 iUnitIdx = 0;
	for (auto itType : m_stPanelInfo.vcUnitType)
	{
		INT32 iStepIdx = -1;
		for (INT32 iIdx = 0; iIdx < iStepNum; iIdx++)
		{
			CStep *pStep = m_pJobData->m_arrStep[iIdx];
			if (pStep == nullptr)
				return RESULT_BAD;
			
			if (itType->strStepName.CompareNoCase(pStep->m_strStepName) == 0)
			{
				iStepIdx = iIdx;
				break;
			}
		}
		
		if (iStepIdx < 0) continue;

		strSavePath.Format(_T("%s\\U%d"), strRootPath, iUnitIdx + 1);
		CreateDirectory(strSavePath, NULL);

//		Orient ori = ReverseOrient(static_cast<Orient>(itType->iOrient));
		GetAngle(static_cast<Orient>(itType->iOrient), dAngle, bMirror);

		if (RESULT_GOOD != m_pDrawManager->SaveUnitLayer(iStepIdx, iUnitIdx, dAngle, bMirror, dResolMM, strSavePath))
			return RESULT_BAD;

		iUnitIdx++;
	}

	return RESULT_GOOD;
}

UINT32 CAVI::EstimateCell_Based_On_Swath(INT32 iStepIdx, enum Orient eOrient)
{
	m_stCellInfo.Clear();

	if (iStepIdx < 0)
		return RESULT_BAD;

	if (m_pJobData == nullptr || m_pUserSetInfo == nullptr ||
		m_pSystemSpec == nullptr)
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
		AfxMessageBox(_T("Cell 개수 입력 값이 올바르지 않습니다."));
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

				//1. Overlap b/w Cell in Swath 고려
//				pCell->rcRectPxl.left = rcRectPxl.left + iSwathMarginX + iCol * (iCellWidth - iCellMinOverlapX);
//				pCell->rcRectPxl.right = pCell->rcRectPxl.left + iCellWidth - 1;
//				pCell->rcRectPxl.top = rcRectPxl.top + iSwathMarginY + iRow * (iCellHeight - iCellMinOverlapY);
//				pCell->rcRectPxl.bottom = pCell->rcRectPxl.top + iCellHeight - 1;

//				pCell->rcRectMM.left = rcRectMM.left + (iSwathMarginX * dResolMM) + ((iCol * (iCellWidth - iCellMinOverlapX)) * dResolMM);
//				pCell->rcRectMM.right = pCell->rcRectMM.left + (iCellWidth * dResolMM);
//				pCell->rcRectMM.top = rcRectMM.top - (iSwathMarginY * dResolMM) - ((iRow * (iCellHeight - iCellMinOverlapY)) * dResolMM);
//				pCell->rcRectMM.bottom = pCell->rcRectMM.top - (iCellHeight * dResolMM);

				//2. Overlap b/w All Cells 고려
				pCell->rcRectPxl.left = rcRectPxl.left + (iSwathMarginX - iCellMinOverlapX_bw_Swath) + iCol * (iCellWidth - iCellMinOverlapX);
				pCell->rcRectPxl.right = pCell->rcRectPxl.left + iCellWidth - 1;
				pCell->rcRectPxl.top = rcRectPxl.top + iSwathMarginY + iRow * (iCellHeight - iCellMinOverlapY);
				pCell->rcRectPxl.bottom = pCell->rcRectPxl.top + iCellHeight - 1;

				pCell->rcRectMM.left = rcRectMM.left + ((iSwathMarginX - iCellMinOverlapX_bw_Swath) * dResolMM) + ((iCol * (iCellWidth - iCellMinOverlapX)) * dResolMM);
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
				str.Format(_T("Cell Swath[%d] Col[%d] Row[%d] : (L,T,R,B) (%d,%d,%d,%d)\n"), iSwath, iCol, iRow, pCell->rcRectPxl.left, pCell->rcRectPxl.top, pCell->rcRectPxl.right, pCell->rcRectPxl.bottom);
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


UINT32 CAVI::EstimateCell_Based_On_Swath_V2(INT32 iStepIdx, enum Orient eOrient)
{
	m_stCellInfo.Clear();

	if (iStepIdx < 0)
		return RESULT_BAD;

	if (m_pJobData == nullptr || m_pUserSetInfo == nullptr ||
		m_pSystemSpec == nullptr)
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
		AfxMessageBox(_T("Cell 개수 입력 값이 올바르지 않습니다."));
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

UINT32 CAVI::EstimateUnitInfo_in_Cell()
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

		RECTD rcCell = itCell->rcRectMM;
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
			rcStripMM.left = (LONG)(RoundReal(itStrip->rcRect_In_Panel.left));
			rcStripMM.top = (LONG)(RoundReal(itStrip->rcRect_In_Panel.top));
			rcStripMM.right = (LONG)(RoundReal(itStrip->rcRect_In_Panel.right));
			rcStripMM.bottom = (LONG)(RoundReal(itStrip->rcRect_In_Panel.bottom));
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
					RECTD rcUnit = itUnit->rcRect_In_Panel;
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

UINT32 CAVI::EstimateStripInfo_in_Cell()
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

UINT32 CAVI::SaveGrabInfo(INT32 iStepIdx, enum Orient eOrient)
{
	if (m_pJobData == nullptr || m_pUserLayerSetInfo == nullptr ||
		m_pUserSetInfo == nullptr || m_pSystemSpec == nullptr)
		return RESULT_BAD;

	if (m_pJobData->m_Misc.GetModelName().GetLength() < 1 ||
		m_strModelPath.GetLength() < 1)
		return RESULT_BAD;

//	INT32 iStepIdx = m_pUserSetInfo->iStepIdx;
	if (iStepIdx < 0 || m_pSystemSpec->sysPath.strMasterPath.GetLength() < 1)
		return RESULT_BAD;

	CameraSpec_t *pSpec = nullptr;
	if (m_pSystemSpec->sysBasic.ScanType == eScanType::eLineScan)
		pSpec = &m_pSystemSpec->sysCamera[true];
	else
		pSpec = &m_pSystemSpec->sysCamera[false];

	if (pSpec == nullptr) return RESULT_BAD;

	int iSwathNum = static_cast<int>(m_stFovInfo.stBasic.swathNum);
	int iCellCol = static_cast<int>(m_stCellInfo.stBasic.cellNum.cx) / iSwathNum;
	int iCellRow = static_cast<int>(m_stCellInfo.stBasic.cellNum.cy);
	if (iSwathNum <= 0 || iCellCol <= 0 || iCellRow <= 0)
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

	//[INFO]	
	if (m_pSystemSpec->sysBasic.bExceptDummy)
	{
		double dStartOffX = fabs(m_stFovInfo.vcFovData[0]->rcRectMM.left - m_stPanelInfo.rcRect.left);
		double dStartOffY = fabs(m_stFovInfo.vcFovData[0]->rcRectMM.top - m_stPanelInfo.rcRect.top);

		strValue.Format(_T("<info model=\"%s\" resolution=\"%.1f\" grab_width=\"%d\" grab_height=\"%d\" start_offsetx=\"%d\" start_offsety=\"%d\" ipu_count=\"%d\" total_cell_col_count=\"%d\" total_cell_row_count=\"%d\" unit=\"%s\"></info>\r\n"),
			m_pJobData->m_Misc.GetModelName(), m_stFovInfo.stBasic.resolution, m_stFovInfo.stBasic.prodSize.cx,
			m_stFovInfo.stBasic.prodSize.cy, (LONG)(dStartOffX / dResolMM + 0.5), (LONG)(dStartOffY / dResolMM + 0.5), iIPUNum,
			m_stCellInfo.stBasic.cellNum.cx, m_stCellInfo.stBasic.cellNum.cy, _T("pixel"));
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

	//[AlignMark]
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

		rcAlign.left -= dStartOffX;
		rcAlign.right -= dStartOffX;
		rcAlign.top -= dStartOffY;
		rcAlign.bottom -= dStartOffY;

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
		double dTmpx, dTmpy;
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

			strValue.Format(_T("<c%d insp=\"%d\" ipu_index=\"%d\" swath_index=\"%d\" index_x=\"%d\" index_y=\"%d\" left=\"%d\" top=\"%d\" right=\"%d\" bottom=\"%d\" overlap_left=\"%d\" overlap_top=\"%d\" overlap_right=\"%d\" overlap_bottom=\"%d\" unit_count=\"%d\">\r\n"),
				iCellIdx, 1, iIPUIdx + 1, itCell->swathIdx + 1, itCell->fovIdx.x + 1, itCell->fovIdx.y + 1, rcCell.left, rcCell.top, rcCell.right, rcCell.bottom,
				itCell->rcOverlapPxl.left, itCell->rcOverlapPxl.top, itCell->rcOverlapPxl.right, itCell->rcOverlapPxl.bottom,
				itUnit_in_Cell->second.size());

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
			{*/
			rcCellMM.left = fabs(rcCellMM.left - m_stPanelInfo.rcRect.left);
			rcCellMM.right = fabs(rcCellMM.right - m_stPanelInfo.rcRect.left);
			rcCellMM.top = fabs(rcCellMM.top - m_stPanelInfo.rcRect.bottom);
			rcCellMM.bottom = fabs(rcCellMM.bottom - m_stPanelInfo.rcRect.bottom);
			/*}*/

			if (rcCellMM.right < rcCellMM.left)	SWAP(rcCellMM.left, rcCellMM.right, dTmpx);
			if (rcCellMM.top < rcCellMM.bottom)	SWAP(rcCellMM.top, rcCellMM.bottom, dTmpy);

			//[UNIT]
			UINT32 iUnitIdx = 1;
			CRect rcUnit;
			for (auto itUnit : itUnit_in_Cell->second)
			{
				RECTD rcUnitRect = itUnit->rcRect;
				if (m_pSystemSpec->sysBasic.bProfileMode)
					rcUnitRect = itUnit->rcProfile;

//				if (fabs(rcUnitRect.right) > fabs(rcUnitRect.left))
//					rcUnit.left = (LONG)((rcUnitRect.left - itCell->rcRectMM.left) / dResolMM + 0.5);
//				else
//					rcUnit.left = (LONG)((rcUnitRect.left - itCell->rcRectMM.right) / dResolMM + 0.5);

				rcUnit.left = (LONG)((rcUnitRect.left - rcCellMM.left) / dResolMM + 0.5);
				rcUnit.right = (LONG)(rcUnit.left + m_stPanelInfo.vcUnitType[itUnit->iType]->szFeatureSize.cx - 1);

//				if (fabs(rcUnitRect.top) > fabs(rcUnitRect.bottom))
//					rcUnit.top = (LONG)((itCell->rcRectMM.top - rcUnitRect.top) / dResolMM + 0.5);
//				else
//					rcUnit.top = (LONG)((itCell->rcRectMM.top - rcUnitRect.bottom) / dResolMM + 0.5);

				rcUnit.top = (LONG)((rcCellMM.top - rcUnitRect.top) / dResolMM + 0.5);
				rcUnit.bottom = (LONG)(rcUnit.top + m_stPanelInfo.vcUnitType[itUnit->iType]->szFeatureSize.cy - 1);

				if (rcUnit.bottom - rcUnit.top >= m_stPanelInfo.vcUnitType[itUnit->iType]->szFeatureSize.cy)
					rcUnit.bottom = rcUnit.top + m_stPanelInfo.vcUnitType[itUnit->iType]->szFeatureSize.cy - 1;

				strValue.Format(_T("<u%d insp=\"%d\" index=\"%d\" left=\"%d\" top=\"%d\" right=\"%d\" bottom=\"%d\" stripidx=\"%d\" blockidx=\"%d\"></u%d>\r\n"),
					iUnitIdx, 1, itUnit->iUnitIdx_Ori + 1/*iUnitIdx*/, rcUnit.left, rcUnit.top, rcUnit.right, rcUnit.bottom, itUnit->iStripIdx + 1, itUnit->iBlockIdx + 1, iUnitIdx);

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

UINT32 CAVI::EstimateBlockInfo()
{
	double dTmpx, dTmpy;
	for (auto itStrip : m_stPanelInfo.vcStripInfo)
	{
		for (auto itBlock : itStrip->vcBlockInfo)
		{
			bool bIsFirst = true;
			RECTD rcBlockMinMax;
			for (auto itUnit : itBlock->vcUnitInfo)
			{
				if (bIsFirst)
				{
					rcBlockMinMax = itUnit->rcProfile;
					bIsFirst = false;
				}
				else
				{
					if (fabs(rcBlockMinMax.left) > fabs(itUnit->rcProfile.left))		rcBlockMinMax.left = itUnit->rcProfile.left;
					if (fabs(rcBlockMinMax.right) < fabs(itUnit->rcProfile.right))		rcBlockMinMax.right = itUnit->rcProfile.right;
					if (fabs(rcBlockMinMax.bottom) > fabs(itUnit->rcProfile.bottom))	rcBlockMinMax.bottom = itUnit->rcProfile.bottom;
					if (fabs(rcBlockMinMax.top) < fabs(itUnit->rcProfile.top))			rcBlockMinMax.top = itUnit->rcProfile.top;
				}
			}

			if (rcBlockMinMax.left > rcBlockMinMax.right)		SWAP(rcBlockMinMax.left, rcBlockMinMax.right, dTmpx);
			if (rcBlockMinMax.bottom > rcBlockMinMax.top)		SWAP(rcBlockMinMax.bottom, rcBlockMinMax.top, dTmpy);

			itBlock->rcRect_In_Panel = rcBlockMinMax;
		}
	}
	return RESULT_GOOD;
}

UINT32 CAVI::SaveCellInfo()
{
	if (m_pJobData == nullptr || m_pUserLayerSetInfo == nullptr ||
		m_pUserSetInfo == nullptr || m_pSystemSpec == nullptr)
		return RESULT_BAD;

	if (m_pJobData->m_Misc.GetModelName().GetLength() < 1 ||
		m_strModelPath.GetLength() < 1)
		return RESULT_BAD;

	INT32 iStepIdx = m_pUserSetInfo->iStepIdx;
	if (iStepIdx < 0 || m_pSystemSpec->sysPath.strMasterPath.GetLength() < 1)
		return RESULT_BAD;

	CameraSpec_t *pSpec = nullptr;
	if (m_pSystemSpec->sysBasic.ScanType == eScanType::eLineScan)
		pSpec = &m_pSystemSpec->sysCamera[true];
	else
		pSpec = &m_pSystemSpec->sysCamera[false];

	if (pSpec == nullptr) return RESULT_BAD;

	int iSwathNum = static_cast<int>(m_stFovInfo.stBasic.swathNum);
	int iCellCol = static_cast<int>(m_stCellInfo.stBasic.cellNum.cx) / iSwathNum;
	int iCellRow = static_cast<int>(m_stCellInfo.stBasic.cellNum.cy);
	if (iSwathNum <= 0 || iCellCol <= 0 || iCellRow <= 0)
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

	//[INFO]	
	strValue.Format(_T("<info model=\"%s\" resolution=\"%.1f\" grab_width=\"%d\" grab_height=\"%d\" start_offsetx=\"%d\" start_offsety=\"%d\" ipu_count=\"%d\" total_cell_col_count=\"%d\" total_cell_row_count=\"%d\" unit=\"%s\"></info>\r\n"),
		m_pJobData->m_Misc.GetModelName(), m_stFovInfo.stBasic.resolution, m_stFovInfo.stBasic.prodSize.cx,
		m_stFovInfo.stBasic.prodSize.cy, m_stFovInfo.stBasic.fovMargin.cx, m_stFovInfo.stBasic.fovMargin.cy, iIPUNum,
		m_stCellInfo.stBasic.cellNum.cx, m_stCellInfo.stBasic.cellNum.cy, _T("pixel"));

	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

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

			strValue.Format(_T("<c%d insp=\"%d\" ipu_index=\"%d\" swath_index=\"%d\" index_x=\"%d\" index_y=\"%d\" left=\"%d\" top=\"%d\" right=\"%d\" bottom=\"%d\" overlap_left=\"%d\" overlap_top=\"%d\" overlap_right=\"%d\" overlap_bottom=\"%d\" align_count=\"%d\" mask_count=\"%d\">\r\n"),
				iCellIdx, 1, iIPUIdx + 1, itCell->swathIdx + 1, itCell->fovIdx.x + 1, itCell->fovIdx.y + 1, rcCell.left, rcCell.top, rcCell.right, rcCell.bottom,
				itCell->rcOverlapPxl.left, itCell->rcOverlapPxl.top, itCell->rcOverlapPxl.right, itCell->rcOverlapPxl.bottom,
				nAlignCount_in_Cell, nMaskCount_in_Cell);

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
