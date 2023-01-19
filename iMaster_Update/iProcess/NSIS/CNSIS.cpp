#include "../stdafx.h"
#include "CNSIS.h"
#include "../../iUtilities/PerfTimerEx.h"

#include <iostream>


bool SortAscendingX(MeasureUnit_t *a, MeasureUnit_t *b)
{
	return a->UnitRect.left < b->UnitRect.left;
}

bool SortDescendingX(MeasureUnit_t *a, MeasureUnit_t *b)
{
	return a->UnitRect.left > b->UnitRect.left;
}

bool SortAscendingY(std::vector<MeasureUnit_t*>a, std::vector<MeasureUnit_t*>b)
{
	MeasureUnit_t *A = a.at(0);
	MeasureUnit_t *B = b.at(0);
	return A->UnitRect.top < B->UnitRect.top;
}
bool SortDescendingY(std::vector<MeasureUnit_t*>a, std::vector<MeasureUnit_t*>b)
{
	MeasureUnit_t *A = a.at(0);
	MeasureUnit_t *B = b.at(0);
	return A->UnitRect.top > B->UnitRect.top;
}

CNSIS::CNSIS()
{
	
}

CNSIS::~CNSIS()
{

}

UINT32 CNSIS::Destory()
{
	return RESULT_GOOD;
}

UINT32 CNSIS::GetMeasureData(MeasurePanel_t* pMeasure, BOOL bSave, BOOL bUpdate)
{
	if (m_nSelectStep < 0 || m_pUserSetInfo == nullptr || m_pSystemSpec == nullptr)
		return RESULT_BAD;

	if (pMeasure)
		pMeasure->Clear();

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

	if (bSave || bUpdate || (m_ePrevOrient != orient) ||
		(m_ePrevMirrorDir != m_pUserSetInfo->mirrorDir) ||
		(m_nPrevSelectStep != m_nSelectStep)) 
	{
		m_ePrevOrient = orient;
		m_ePrevMirrorDir = m_pUserSetInfo->mirrorDir;
		m_nPrevSelectStep = m_nSelectStep;

		// Get Product Info
		if (RESULT_GOOD != EstimateProductInfo(m_nSelectStep, orient))
			return RESULT_BAD;

		// Get Measure Info
		if (RESULT_GOOD != EstimateMeasureInfo(m_nSelectStep, orient))
			return RESULT_BAD;		

		//Make Profile Image
 		/*if (RESULT_GOOD != MakeProfileImage())
 			return RESULT_BAD;*/
	}
	
	if (pMeasure)
		*pMeasure = m_stPanelInfo.stMeasureInfo;	

	m_ePrevOrient = orient;

	if (bSave)
	{	
		if (RESULT_GOOD != SaveProductInfo(m_nSelectStep, orient))
			return RESULT_BAD;		
	}

	return RESULT_GOOD;
}

UINT32 CNSIS::EstimateProductInfo(IN INT32 iStepIdx, IN enum Orient eOrient)
{
	if (iStepIdx < 0)
		return RESULT_BAD;

	m_stPanelInfo.Clear();

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

	return RESULT_GOOD;
}

UINT32 CNSIS::EstimatePanelInfo(IN INT32 iStepIdx, IN enum Orient eOrient)
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

	if (m_pSystemSpec->sysBasic.bProfileMode)
		tmpMinMax = pStep->m_Profile.m_MinMax;
	else
		tmpMinMax = pStep->m_FeatureMinMax;

	RotMirrArr.FinalPoint(&tmpRect.left, &tmpRect.top, tmpMinMax.left, tmpMinMax.top);
	RotMirrArr.FinalPoint(&tmpRect.right, &tmpRect.bottom, tmpMinMax.right, tmpMinMax.bottom);

	if (tmpRect.left > tmpRect.right) SWAP(tmpRect.left, tmpRect.right, extend);
	if (tmpRect.bottom > tmpRect.top) SWAP(tmpRect.bottom, tmpRect.top, extend);

	pStep->m_ActiveRect = tmpRect;

	return RESULT_GOOD;
}

UINT32 CNSIS::EstimateStripInfo(IN INT32 iStepIdx, IN enum Orient eOrient)
{
	if (iStepIdx < 0 || m_pJobData == nullptr || 
		m_pUserSetInfo == nullptr || m_pUserLayerSetInfo == nullptr)
		return RESULT_BAD;

	if (iStepIdx != m_pUserLayerSetInfo->nPanelStepIdx)
		iStepIdx = m_pUserLayerSetInfo->nPanelStepIdx;

	m_stPanelInfo.rcRect = m_pJobData->m_arrStep[iStepIdx]->m_ActiveRect;

	//Strip Step
	UINT32 iTotalStripNum = 0;
	for (auto it : m_pUserLayerSetInfo->vcStripStepIdx)
	{
		CString strStripStepName = m_pJobData->m_arrStep[it]->m_strStepName;
		UINT32 iStripNum = 0;

		for (UINT32 i = 0; i < m_pJobData->m_arrStep[iStepIdx]->m_nSubStepCnt; i++)
		{
			UINT32 iSubUnitNum = 0;
			CalculateStripNumOnSubStep(TRUE, strStripStepName, m_pJobData->m_arrStep[iStepIdx]->m_arrSubStep[i], &iStripNum);

			CStepRepeat *pSubStepRepeat = m_pJobData->m_arrStep[iStepIdx]->m_Stephdr.m_arrStepRepeat[i];
			if (iSubUnitNum == 0) // one step
			{
				if (pSubStepRepeat->m_strName.CompareNoCase(strStripStepName) != 0) continue;
				iStripNum += (pSubStepRepeat->m_nNY * pSubStepRepeat->m_nNX);
			}
			else
			{
				iStripNum += iSubUnitNum * (pSubStepRepeat->m_nNY * pSubStepRepeat->m_nNX);
			}
		}

		iTotalStripNum += iStripNum;
	}

	UINT32 iStripShapeNo = 0, iStripRgnNum = 0, iPrevStripNum = 0;

	if (iTotalStripNum > 0)
	{
		for (auto it : m_pUserLayerSetInfo->vcStripStepIdx)
		{
			CString strStripStepName = m_pJobData->m_arrStep[it]->m_strStepName;

			for (UINT32 i = 0; i < m_pJobData->m_arrStep[iStepIdx]->m_nSubStepCnt; i++)
			{
				UINT32 iSubStepIdx = m_pJobData->m_arrStep[iStepIdx]->m_arrSubStep[i]->m_nStepID;
				if (m_pJobData->m_arrStep[iSubStepIdx]->m_strStepName.CompareNoCase(strStripStepName) != 0)
					continue;

				if (RESULT_GOOD != CalculateStripCoord(TRUE, iStripShapeNo, i, eOrient, strStripStepName, nullptr, m_pJobData->m_arrStep[iStepIdx]->m_arrSubStep[i], &iStripRgnNum))
					return RESULT_BAD;

				//Strip Type
				if (m_stPanelInfo.vcStripInfo.size() != iPrevStripNum)
				{
					UINT32 iPrevCnt = static_cast<UINT32>(m_stPanelInfo.vcStripType.size());
					//if (iPrevCnt > 0)
					//	iPrevCnt = m_stPanelInfo.vcStripType[iPrevCnt - 1]->nNum;

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
	}

	m_stPanelInfo.iStripTypeNum = iStripShapeNo;
	if (iStripRgnNum != m_stPanelInfo.vcStripInfo.size())
		return RESULT_BAD;

	if (iStripShapeNo != m_stPanelInfo.vcStripType.size())
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CNSIS::CalculateStripNumOnSubStep(IN BOOL bFirstStep, IN CString strStripStepName, IN CSubStep* pSubStep, OUT UINT32 *pStripNum)
{
	if (pSubStep == nullptr || pStripNum == nullptr)
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

UINT32 CNSIS::CalculateStripCoord(IN BOOL bFirstStep, IN UINT iShapeIdx, IN INT32 iStepIdx, IN enum Orient eOrient, IN CString strStripStepName, IN CRotMirrArr *pRotMirrArr, IN CSubStep *pSubStep, OUT UINT32 *pRgnNum)
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

// 			if (pSubStep->m_nNextStepCnt > 0)
// 			{
// 				for (UINT j = 0; j < pSubStep->m_nNextStepCnt; j++)
// 				{
// 					CalculateStripCoord(FALSE, iShapeIdx, iStepIdx, eOrient, strStripStepName, rotMirrArr, pSubStep->m_arrNextStep[j], pRgnNum);
// 				}
// 			}

			if (pSubStepRepeat->m_strName.CompareNoCase(strStripStepName) == 0)
			{
				StripInfo_t *pStrip = new StripInfo_t;
				pStrip->iType = iShapeIdx;
				pStrip->stStepRpt.StripStep = iStepIdx;
				pStrip->stStepRpt.StripIdx.x = x_step;
				pStrip->stStepRpt.StripIdx.y = y_step;
				pStrip->iStripIdx = x_step * pSubStepRepeat->m_nNY + y_step;
				pStrip->rcRect = tmpMinMaxRect;
				pStrip->rcProfile = tmpProfileRect;

				if (m_pSystemSpec->sysBasic.bProfileMode)
					pStrip->rcRect_In_Panel = GetDrawRect(pStrip->rcProfile, m_pUserSetInfo);
				else
					pStrip->rcRect_In_Panel = GetDrawRect(pStrip->rcRect, m_pUserSetInfo);

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

UINT32 CNSIS::CalculateOrient(IN enum Orient curOrient, OUT enum Orient* pTotOrient)
{
	if (pTotOrient == nullptr)
		return RESULT_BAD;

	enum Orient eParentOrient = Orient::NoMir0Deg;
	UINT32 eTotOrient = static_cast<UINT32>(Orient::NoMir0Deg);

	eTotOrient = static_cast<UINT32>(eParentOrient) + static_cast<UINT32>(curOrient);
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

UINT32 CNSIS::EstimateUnitInfo(IN INT32 iStepIdx, IN enum Orient eOrient)
{
	if (iStepIdx < 0 || m_pJobData == nullptr || m_pUserLayerSetInfo == nullptr)
		return RESULT_BAD;

	if (iStepIdx != m_pUserLayerSetInfo->nPanelStepIdx)
		iStepIdx = m_pUserLayerSetInfo->nPanelStepIdx;

	UINT32 iUnitShapeNo = 0, iUnitRgnNum = 0;

	//Strip Step
	std::vector<CString> vcStripNames;
	for (auto itStrip : m_pUserLayerSetInfo->vcStripStepIdx)
	{
		CString strStripStepName = m_pJobData->m_arrStep[itStrip]->m_strStepName;

		for (UINT32 i = 0; i < m_pJobData->m_arrStep[iStepIdx]->m_nSubStepCnt; i++)
		{
			UINT32 iSubStepIdx = m_pJobData->m_arrStep[iStepIdx]->m_arrSubStep[i]->m_nStepID;
			if (m_pJobData->m_arrStep[iSubStepIdx]->m_strStepName.CompareNoCase(strStripStepName) != 0)
				continue;

			bool bIsDone = false;
			for (auto it : vcStripNames)
			{
				if (it.CompareNoCase(strStripStepName) == 0)
				{
					bIsDone = true;
					break;
				}
			}

			if (bIsDone)
				continue;

			vcStripNames.emplace_back(strStripStepName);

			//Unit Step			
			for (auto itUnit : m_pUserLayerSetInfo->vcUnitStepIdx)
			{
				CString strUnitStepName = m_pJobData->m_arrStep[itUnit]->m_strStepName;

				for (UINT32 j = 0; j < m_pJobData->m_arrStep[iStepIdx]->m_nSubStepCnt; j++)
				{
					if (m_pJobData->m_arrStep[iStepIdx]->m_arrSubStep[j]->m_arrStepRepeat.m_strName.CompareNoCase(strStripStepName) != 0)
						continue;

					CalculateUnitCoord(TRUE, iUnitShapeNo, j, -1,  eOrient, strUnitStepName, nullptr, m_pJobData->m_arrStep[iStepIdx]->m_arrSubStep[j], &iUnitRgnNum);
				}

				iUnitShapeNo++;
			}
		}
	}

	vcStripNames.clear();
	if (iUnitShapeNo != m_stPanelInfo.vcUnitType.size())
		return RESULT_BAD;

	//Unit Info
	RECTD rcStripRect = RECTD(LONG_MAX, LONG_MIN, LONG_MIN, LONG_MAX);
	RECTD rcStripProfile = RECTD(LONG_MAX, LONG_MIN, LONG_MIN, LONG_MAX);
	UINT32 iRowUnitNumInPanel = 0, iColUnitNumInPanel = 0;
	for (auto itStrip : m_stPanelInfo.vcStripInfo)
	{
		RECTD rcUnitRect = RECTD(LONG_MAX, LONG_MIN, LONG_MIN, LONG_MAX);
		RECTD rcUnitProfile = RECTD(LONG_MAX, LONG_MIN, LONG_MIN, LONG_MAX);

		for (auto itUnit : itStrip->vcUnitInfo)
		{
			if (rcUnitRect.left > itUnit->rcRect.left)				rcUnitRect.left = itUnit->rcRect.left;
			if (rcUnitRect.right < itUnit->rcRect.right)			rcUnitRect.right = itUnit->rcRect.right;
			if (rcUnitRect.bottom > itUnit->rcRect.bottom)			rcUnitRect.bottom = itUnit->rcRect.bottom;
			if (rcUnitRect.top < itUnit->rcRect.top)				rcUnitRect.top = itUnit->rcRect.top;

			if (rcUnitProfile.left > itUnit->rcProfile.left)		rcUnitProfile.left = itUnit->rcProfile.left;
			if (rcUnitProfile.right < itUnit->rcProfile.right)		rcUnitProfile.right = itUnit->rcProfile.right;
			if (rcUnitProfile.bottom > itUnit->rcProfile.bottom)	rcUnitProfile.bottom = itUnit->rcProfile.bottom;
			if (rcUnitProfile.top < itUnit->rcProfile.top)			rcUnitProfile.top = itUnit->rcProfile.top;
		}

		itStrip->rcRect_wo_Dummy = rcUnitRect;
		itStrip->rcProfile_wo_Dummy = rcUnitProfile;

		iRowUnitNumInPanel += itStrip->iRowUnitNumInStrip;
		iColUnitNumInPanel += itStrip->iColUnitNumInStrip;

		if (rcStripRect.left > itStrip->rcRect_wo_Dummy.left)			rcStripRect.left = itStrip->rcRect_wo_Dummy.left;
		if (rcStripRect.right < itStrip->rcRect_wo_Dummy.right)			rcStripRect.right = itStrip->rcRect_wo_Dummy.right;
		if (rcStripRect.bottom > itStrip->rcRect_wo_Dummy.bottom)		rcStripRect.bottom = itStrip->rcRect_wo_Dummy.bottom;
		if (rcStripRect.top < itStrip->rcRect_wo_Dummy.top)				rcStripRect.top = itStrip->rcRect_wo_Dummy.top;

		if (rcStripProfile.left > itStrip->rcProfile_wo_Dummy.left)		rcStripProfile.left = itStrip->rcProfile_wo_Dummy.left;
		if (rcStripProfile.right < itStrip->rcProfile_wo_Dummy.right)	rcStripProfile.right = itStrip->rcProfile_wo_Dummy.right;
		if (rcStripProfile.bottom > itStrip->rcProfile_wo_Dummy.bottom)	rcStripProfile.bottom = itStrip->rcProfile_wo_Dummy.bottom;
		if (rcStripProfile.top < itStrip->rcProfile_wo_Dummy.top)		rcStripProfile.top = itStrip->rcProfile_wo_Dummy.top;
	}

	//Panel Info
	iRowUnitNumInPanel /= m_stPanelInfo.iColStripNumInPanel;
	iColUnitNumInPanel /= m_stPanelInfo.iRowStripNumInPanel;

	m_stPanelInfo.iRowUnitNumInPanel = iRowUnitNumInPanel;
	m_stPanelInfo.iColUnitNumInPanel = iColUnitNumInPanel;
	m_stPanelInfo.rcRect_wo_Dummy = rcStripRect;
	m_stPanelInfo.rcProfile_wo_Dummy = rcStripProfile;

	return RESULT_GOOD;
}

UINT32 CNSIS::CalculateUnitCoord(IN BOOL bFirstStep, IN UINT iShapeIdx, IN INT32 iStepIdx, IN INT32 iStripIdx, IN enum Orient eOrient, IN CString strUnitStepName, IN CRotMirrArr *pRotMirrArr, IN CSubStep *pSubStep, OUT UINT32 *pRgnNum)
{
	if (pSubStep == nullptr || pRgnNum == nullptr || m_pUserSetInfo == nullptr ||
		m_pJobData == nullptr || m_pUserLayerSetInfo == nullptr)
		return RESULT_BAD;

	CStepRepeat *pSubStepRepeat = &(pSubStep->m_arrStepRepeat);
	if (pSubStepRepeat == nullptr)
		return RESULT_BAD;

	bool bCreatedRotMirr = false;
	CRotMirrArr *rotMirrArr = nullptr;
	if (pRotMirrArr)
	{
		rotMirrArr = pRotMirrArr;
	}
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

	if (!bFirstStep)
		vcUnitInfo = m_stPanelInfo.vcStripInfo[iStripIdx]->vcUnitInfo;

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

			if (bFirstStep)
			{
				iStripIdx = -1;

				RECTD rcProfile = tmpProfileRect;
				rcProfile.left = fabs(rcProfile.left - m_stPanelInfo.rcRect.left);
				rcProfile.right = fabs(rcProfile.right - m_stPanelInfo.rcRect.left);
				rcProfile.top = fabs(rcProfile.top - m_stPanelInfo.rcRect.bottom);
				rcProfile.bottom = fabs(rcProfile.bottom - m_stPanelInfo.rcRect.bottom);

				if (rcProfile.right < rcProfile.left)	SWAP(rcProfile.left, rcProfile.right, dTmpx);
				if (rcProfile.top < rcProfile.bottom)	SWAP(rcProfile.top, rcProfile.bottom, dTmpy);

				for (auto it : m_stPanelInfo.vcStripInfo)
				{
					if (fabs(it->rcProfile.left - rcProfile.left) < EPSILON_DELTA2 &&
						fabs(it->rcProfile.top - rcProfile.top) < EPSILON_DELTA2)
					{
						iStripIdx = it->iStripIdx;
						break;
					}
				}
			}

			if (pSubStep->m_nNextStepCnt > 0)
			{
				enum Orient eUnitOrient = AddOrient(eOrient, eSubOrient);
				enum Orient eTotOrient = Orient::NoMir0Deg;
				CalculateOrient(eUnitOrient, &eTotOrient);

				for (UINT j = 0; j < pSubStep->m_nNextStepCnt; j++)
				{
					CalculateUnitCoord(FALSE, iShapeIdx, j, iStripIdx, eTotOrient, strUnitStepName, rotMirrArr, pSubStep->m_arrNextStep[j], pRgnNum);
				}
			}

			if (pSubStepRepeat->m_strName.CompareNoCase(strUnitStepName) == 0)
			{
				UnitInfo_t *pUnit = new UnitInfo_t;
				pUnit->iType = iShapeIdx;
				pUnit->iStripIdx = iStripIdx;
				pUnit->stStepRpt.StripIdx = m_stPanelInfo.vcStripInfo[iStripIdx]->stStepRpt.StripIdx;
				pUnit->stStepRpt.StripStep = m_stPanelInfo.vcStripInfo[iStripIdx]->stStepRpt.StripStep;
				pUnit->stStepRpt.UnitStep = iStepIdx;// pSubStep->m_nStepID;
				pUnit->stStepRpt.UnitIdx.x = x_step;
				pUnit->stStepRpt.UnitIdx.y = y_step;
				pUnit->iUnitIdx = *pRgnNum;
				pUnit->rcRect = tmpMinMaxRect;
				pUnit->rcProfile = tmpProfileRect;
				pUnit->rcProfile_In_Panel = tmpProfileRect;

				if (m_pSystemSpec->sysBasic.bProfileMode)
					pUnit->rcRect_In_Panel = GetDrawRect(pUnit->rcProfile, m_pUserSetInfo);
				else
					pUnit->rcRect_In_Panel = GetDrawRect(pUnit->rcRect, m_pUserSetInfo);

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
		std::sort(vcUnitInfo.begin(), vcUnitInfo.end(), UnitInfo_t::compY);

		UINT32 iLoop = 0;
		for (auto it : vcUnitInfo)
			it->iUnitIdx = iLoop++;

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

		m_stPanelInfo.vcStripInfo[iStripIdx]->iRowUnitNumInStrip = iRowUnitNum;
		m_stPanelInfo.vcStripInfo[iStripIdx]->iColUnitNumInStrip = iColUnitNum;
		m_stPanelInfo.vcStripInfo[iStripIdx]->vcUnitInfo = vcUnitInfo;
	}

	return RESULT_GOOD;
}

UINT32 CNSIS::RearrageIndexInfo(IN enum Orient eOrient)
{
	CString strLog;

	if (eOrient > Orient::NoMir270Deg)
	{
		std::sort(m_stPanelInfo.vcStripInfo.begin(), m_stPanelInfo.vcStripInfo.end(), StripInfo_t::comp_reverseY);

		UINT32 iStripIdx = 0;
		for (auto itStrip : m_stPanelInfo.vcStripInfo)
		{
			UINT32 iStripStartIdx = iStripIdx * (itStrip->iColUnitNumInStrip * itStrip->iRowUnitNumInStrip);
			itStrip->iStripIdx = iStripIdx++;

			std::sort(itStrip->vcUnitInfo.begin(), itStrip->vcUnitInfo.end(), UnitInfo_t::comp_reverseY);

			UINT32 iUnitIdx = 0;
			for (auto itUnit : itStrip->vcUnitInfo)
			{
				itUnit->iStripIdx = itStrip->iStripIdx;
				itUnit->iUnitIdx_Ori = iStripStartIdx + iUnitIdx;

				//strLog.Format(L"%d,%d,%.3f,%.3f,%.3f,%.3f\n", itUnit->iUnitIdx_Ori, itUnit->iStripIdx, itUnit->rcProfile.left, itUnit->rcProfile.top, itUnit->rcProfile.right, itUnit->rcProfile.bottom);
				//OutputDebugString(strLog);

				iUnitIdx++;
			}
		}

		std::sort(m_stPanelInfo.vcStripInfo.begin(), m_stPanelInfo.vcStripInfo.end(), StripInfo_t::compY);
	}
	else
	{
		UINT32 iStripIdx = 0;
		for (auto itStrip : m_stPanelInfo.vcStripInfo)
		{
			UINT32 iStripStartIdx = iStripIdx * (itStrip->iColUnitNumInStrip * itStrip->iRowUnitNumInStrip);
			itStrip->iStripIdx = iStripIdx++;

			UINT32 iUnitIdx = 0;

			for (auto itUnit : itStrip->vcUnitInfo)
			{
				itUnit->iStripIdx = itStrip->iStripIdx;
				itUnit->iUnitIdx_Ori = iStripStartIdx + iUnitIdx;

				//strLog.Format(L"%d,%d,%.3f,%.3f,%.3f,%.3f\n", itUnit->iUnitIdx_Ori, itUnit->iStripIdx, itUnit->rcProfile.left, itUnit->rcProfile.top, itUnit->rcProfile.right, itUnit->rcProfile.bottom);
				//OutputDebugString(strLog);

				iUnitIdx++;
			}
		}
	}
	return RESULT_GOOD;
}

UINT32 CNSIS::EstimateMeasureInfo(IN INT32 iStepIdx, IN enum Orient eOrient)
{

	// 2022.05.03 
	// 주석추가 김준호
	// GetMeasureInfo daeduck, Sem 분리
	if (m_pUserSetInfo->userSite == eUserSite::eDAEDUCK)
	{
		if (RESULT_GOOD != EstimateMeasureInfo_Daeduck(iStepIdx, eOrient))
			return RESULT_BAD;
	}
	else if (m_pUserSetInfo->userSite == eUserSite::eSEM)
	{
		if (RESULT_GOOD != EstimateMeasureInfo_Sem(iStepIdx, eOrient))
			return RESULT_BAD;
	}

	return RESULT_GOOD;
}


	
UINT32 CNSIS::EstimateMeasureInfo_Daeduck(IN INT32 iStepIdx, IN enum Orient eOrient)
{
	// 2022.10.04 기존 코드 주석처리 
	// from SNU 0921 코드로 변경
	/*
	m_stPanelInfo.stMeasureInfo.Clear();

	if (iStepIdx < 0 || m_pJobData == nullptr || m_pUserSetInfo == nullptr ||
		m_pUserLayerSetInfo == nullptr || m_pSystemSpec == nullptr)
		return RESULT_BAD;

	//Panel Step	
	if (iStepIdx != m_pUserLayerSetInfo->nPanelStepIdx)
		iStepIdx = m_pUserLayerSetInfo->nPanelStepIdx;

	CStep *pStep = m_pJobData->m_arrStep[iStepIdx];
	if (pStep == nullptr)
		return RESULT_BAD;

	CLayer* pTargetLayer = nullptr;
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

	if (pTargetLayer == nullptr)
		return RESULT_BAD;

	std::vector<MeasureItem_t> vcMeasureItems;

	CRotMirrArr rotMirrArr;
	rotMirrArr.InputData(0.0, 0.0, eOrient, m_pUserSetInfo->mirrorDir);

	CString strText = L"";
	double dTmpx, dTmpy;
	for (int feno = 0; feno < pTargetLayer->m_FeatureFile.m_arrFeature.GetSize(); feno++)
	{
		CFeature* pFeature = pTargetLayer->m_FeatureFile.m_arrFeature.GetAt(feno);
		if (pFeature == nullptr)
			continue;

		//YJD START 2021.09.30 - EXCEPTION  
		if (pFeature->m_arrAttributeTextString.GetCount() <= 0)
		{
			continue;
		}
		//YJD END 2021.09.30 - EXCEPTION 

		CAttributeTextString* pTextString = pFeature->m_arrAttributeTextString.GetAt(0);
		if (pTextString == nullptr)
			continue;

		RECTD tmpMinMaxRect = pFeature->m_MinMaxRect;
		rotMirrArr.FinalPoint(&tmpMinMaxRect.left, &tmpMinMaxRect.top, tmpMinMaxRect.left, tmpMinMaxRect.top);
		rotMirrArr.FinalPoint(&tmpMinMaxRect.right, &tmpMinMaxRect.bottom, tmpMinMaxRect.right, tmpMinMaxRect.bottom);

		MeasureItem_t stMeasure;
		stMeasure.MinMaxRect_In_Panel = GetDrawRect(tmpMinMaxRect, m_pUserSetInfo);

		tmpMinMaxRect.left = fabs(tmpMinMaxRect.left - m_stPanelInfo.rcRect.left);
		tmpMinMaxRect.right = fabs(tmpMinMaxRect.right - m_stPanelInfo.rcRect.left);
		tmpMinMaxRect.top = fabs(tmpMinMaxRect.top - m_stPanelInfo.rcRect.bottom);
		tmpMinMaxRect.bottom = fabs(tmpMinMaxRect.bottom - m_stPanelInfo.rcRect.bottom);

		if (tmpMinMaxRect.left > tmpMinMaxRect.right)		SWAP(tmpMinMaxRect.left, tmpMinMaxRect.right, dTmpx);
		if (tmpMinMaxRect.bottom > tmpMinMaxRect.top)		SWAP(tmpMinMaxRect.bottom, tmpMinMaxRect.top, dTmpy);
				
		stMeasure.iFeatureType = static_cast<UINT8>(pFeature->m_eType);
		stMeasure.MinMaxRect = tmpMinMaxRect;

		

		std::vector<std::string> vcTextString = SplitString(std::string(CT2CA(pTextString->m_strText)), ",");

		stMeasure.iMeasureType = static_cast<UINT8>(GetMeasureType((CString)vcTextString.at(0).c_str()));
		stMeasure.strMeasureType.Format(_T("%s"), (CString)vcTextString.at(0).c_str());
		stMeasure.strMeasureType.MakeUpper();

		if (vcTextString.size() > 1)
		{
			stMeasure.iMeasureSide = static_cast<UINT8>(GetMeasureSide((CString)vcTextString.at(1).c_str()));

			if (vcTextString.size() == 5) //OOS_Min 없음
			{
				std::vector<std::string> vcSplit = SplitString(vcTextString.at(2).c_str(), ";");
				if (vcSplit.size() > 1)
				{
					stMeasure.stSpec.dRange = _ttof((CString)vcSplit.at(0).c_str());
					stMeasure.stSpec.dMin = _ttof((CString)vcSplit.at(1).c_str());
				}
				else
					stMeasure.stSpec.dMin = _ttof((CString)vcTextString.at(2).c_str());

				stMeasure.stSpec.dTarget = _ttof((CString)vcTextString.at(3).c_str());
				stMeasure.stSpec.dMax = _ttof((CString)vcTextString.at(4).c_str());
			}
			else
			{
				stMeasure.stSpec.dMin = 0.0;
				stMeasure.stSpec.dTarget = 0.0;
				stMeasure.stSpec.dMax = 0.0;
			}
		}

		if ((MeasureType)stMeasure.iMeasureType == MeasureType::align ||
			(MeasureType)stMeasure.iMeasureType == MeasureType::align_plating)
		{
			stMeasure.iUnitIndex = -1;
			stMeasure.pFeature = (void*)pFeature;
			vcMeasureItems.emplace_back(stMeasure);
			continue;
		}

		bool bFind = false;
		RECTD rcUnit;
		for (auto itStrip : m_stPanelInfo.vcStripInfo)
		{
			for (auto itUnit : itStrip->vcUnitInfo)
			{
				rcUnit = itUnit->rcRect;
				if (rcUnit.IsPtInRectD_R(tmpMinMaxRect.left, tmpMinMaxRect.top) &&
					rcUnit.IsPtInRectD_R(tmpMinMaxRect.right, tmpMinMaxRect.bottom))
				{
					stMeasure.iUnitIndex = itUnit->iUnitIdx_Ori;
					stMeasure.iStripIndex = itUnit->iStripIdx;
					stMeasure.stStepRpt = itUnit->stStepRpt;

					itUnit->bInspect = TRUE;
					bFind = true;
					break;
				}
			}
			if (bFind)
			{
				itStrip->bInspect = TRUE;
				break;
			}
		}

		if (!bFind)
		{
			// 측정 Point 가 Unit 영역에 없는 경우 Skip 처리
			continue;
			//rotMirrArr.ClearAll();
			//return RESULT_BAD;
		}
		stMeasure.pFeature = (void*)pFeature;
		vcMeasureItems.emplace_back(stMeasure);
	}

	//

	rotMirrArr.ClearAll();

	if (vcMeasureItems.size() <= 0)
		return RESULT_GOOD;

	std::vector<INT32> vcMeasureUnits;
	for (auto it : vcMeasureItems)
	{
		if ((MeasureType)it.iMeasureType == MeasureType::align ||
			(MeasureType)it.iMeasureType == MeasureType::align_plating)	// Panel Align
		{
			MeasureItem_t* pMeasure = new MeasureItem_t;
			*pMeasure = it;
			pMeasure->stSpec.dRange = 0.;
			pMeasure->bIs3D = FALSE;
			pMeasure->iScanMode = 5;

			if (m_pSystemSpec->sysNSIS.sysAlignLens == 0. && m_pSystemSpec->sysNSIS.sysAlignZoom == 0.)
			{
				CheckLensZoom(true, pMeasure);
			}
			else
			{
				pMeasure->dLens = m_pSystemSpec->sysNSIS.sysAlignLens;
				pMeasure->dZoom = m_pSystemSpec->sysNSIS.sysAlignZoom;
			}

			m_stPanelInfo.stMeasureInfo.vcAlign.emplace_back(pMeasure);
		}
		else
		{
			INT32 iUnitIndex = it.iUnitIndex;
			auto itInserted = std::find(vcMeasureUnits.begin(), vcMeasureUnits.end(), iUnitIndex);
			if (itInserted != vcMeasureUnits.end())
				continue;

			MeasureUnit_t* pUnit = new MeasureUnit_t;

			pUnit->stStepRpt = it.stStepRpt;
			pUnit->iStripIdx = it.iStripIndex;

			for (auto itUnit : vcMeasureItems)
			{
				if (iUnitIndex != itUnit.iUnitIndex)
					continue;

				MeasureItem_t* pMeasure = new MeasureItem_t;
				*pMeasure = itUnit;

				pUnit->iIndex = iUnitIndex;

				if ((MeasureType)itUnit.iMeasureType == MeasureType::fm_p)		// Unit Align
				{
					pMeasure->stSpec.dRange = 0.;
					pMeasure->bIs3D = FALSE;
					pMeasure->iScanMode = 0;

					if (m_pSystemSpec->sysNSIS.sysUnitAlignLens == 0. && m_pSystemSpec->sysNSIS.sysUnitAlignZoom == 0.)
					{
						CheckLensZoom(true, pMeasure);
					}
					else
					{
						pMeasure->dLens = m_pSystemSpec->sysNSIS.sysUnitAlignLens;
						pMeasure->dZoom = m_pSystemSpec->sysNSIS.sysUnitAlignZoom;
					}

					pUnit->vcAlign.emplace_back(pMeasure);

					bool bFind = false;
					for (auto itStripInfo : m_stPanelInfo.vcStripInfo)
					{
						for (auto itUnitInfo : itStripInfo->vcUnitInfo)
						{
							RECTD rcUnit;
							if (m_pSystemSpec->sysBasic.bProfileMode)
							{
								rcUnit = itUnitInfo->rcProfile;
							}
							else
							{
								rcUnit = itUnitInfo->rcRect;
							}

							if (rcUnit.IsPtInRectD_R(itUnit.MinMaxRect.left, itUnit.MinMaxRect.top) &&
								rcUnit.IsPtInRectD_R(itUnit.MinMaxRect.right, itUnit.MinMaxRect.bottom))
							{
								pUnit->iType = itUnitInfo->iType;
								pUnit->UnitRect = rcUnit;
								pUnit->UnitRect_In_Panel = itUnitInfo->rcRect_In_Panel;
								bFind = true;
								break;
							}
						}
						if (bFind)
							break;
					}
				}
				else // Unit Measure Points
				{
					pMeasure->bIs3D = TRUE;
					pMeasure->iScanMode = 1;

					CheckLensZoom(false, pMeasure);
					pUnit->vcPoints.emplace_back(pMeasure);
				}
			}

			vcMeasureUnits.emplace_back(iUnitIndex);
			m_stPanelInfo.stMeasureInfo.vcUnits.emplace_back(pUnit);
		}
	}

	// 측정 Unit 정렬
	if (RESULT_GOOD != SortMeasureUnit())
		return RESULT_BAD;

	vcMeasureUnits.clear();
	vcMeasureItems.clear();
	
	//Panel Align 정보가 없을 경우
	if (m_stPanelInfo.stMeasureInfo.vcAlign.size() <= 0)
		return RESULT_BAD;

	//측정 Unit 정보가 없을 경우
	if (m_stPanelInfo.stMeasureInfo.vcUnits.size() <= 0)
		return RESULT_BAD;

	return RESULT_GOOD;
	*/


	m_stPanelInfo.stMeasureInfo.Clear();

	if (iStepIdx < 0 || m_pJobData == nullptr ||
		m_pUserLayerSetInfo == nullptr || m_pSystemSpec == nullptr)
		return RESULT_BAD;

	//Panel Step	
	if (iStepIdx != m_pUserLayerSetInfo->nPanelStepIdx)
	iStepIdx = m_pUserLayerSetInfo->nPanelStepIdx;

	CStep *pStep = m_pJobData->m_arrStep[iStepIdx];
	if (pStep == nullptr)
	return RESULT_BAD;

	// Panel, Strip, Unit Rect 가져오기
	m_vecvecRect.clear();
	m_vecvecRect = CDrawFunction::GetProfileData(m_pJobData, iStepIdx);

	//
	//Panel좌표로 변환

	/*int nStepCount = static_cast<int>(m_vecvecRect.size());
	for (int i = 0; i < nStepCount; i++)
	{
		int nRectCount = static_cast<int>(m_vecvecRect[i].size());
		for (int j = 0; j < nRectCount; j++)
		{
			m_vecvecRect[i][j].drtRect.left = fabs(m_vecvecRect[i][j].drtRect.left - m_stPanelInfo.rcRect.left);
			m_vecvecRect[i][j].drtRect.right = fabs(m_vecvecRect[i][j].drtRect.right - m_stPanelInfo.rcRect.left);
			m_vecvecRect[i][j].drtRect.top = fabs(m_vecvecRect[i][j].drtRect.top - m_stPanelInfo.rcRect.bottom);
			m_vecvecRect[i][j].drtRect.bottom = fabs(m_vecvecRect[i][j].drtRect.bottom - m_stPanelInfo.rcRect.bottom);
		}
	}*/

	//

	CLayer* pTargetLayer = nullptr;
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

	if (pTargetLayer == nullptr)
	return RESULT_BAD;

	std::vector<MeasureItem_t> vcMeasureItems;

	CRotMirrArr rotMirrArr;
	rotMirrArr.InputData(0.0, 0.0, eOrient);

	CString strText = L"";
	double dTmpx, dTmpy;
	for (int feno = 0; feno < pTargetLayer->m_FeatureFile.m_arrFeature.GetSize(); feno++)
	{
		CFeature* pFeature = pTargetLayer->m_FeatureFile.m_arrFeature.GetAt(feno);
		if (pFeature == nullptr)
			continue;

		//YJD START 2021.09.30 - EXCEPTION  
		if (pFeature->m_arrAttributeTextString.GetCount() <= 0)
		{
			continue;
		}
		//YJD END 2021.09.30 - EXCEPTION 

		CAttributeTextString* pTextString = pFeature->m_arrAttributeTextString.GetAt(0);
		if (pTextString == nullptr)
			continue;

		RECTD tmpMinMaxRect = pFeature->m_MinMaxRect;
		rotMirrArr.FinalPoint(&tmpMinMaxRect.left, &tmpMinMaxRect.top, tmpMinMaxRect.left, tmpMinMaxRect.top);
		rotMirrArr.FinalPoint(&tmpMinMaxRect.right, &tmpMinMaxRect.bottom, tmpMinMaxRect.right, tmpMinMaxRect.bottom);

		MeasureItem_t stMeasure;
		//stMeasure.MinMaxRect_In_Panel = tmpMinMaxRect;
		stMeasure.MinMaxRect_In_Panel = GetDrawRect(tmpMinMaxRect, m_pUserSetInfo);

		tmpMinMaxRect.left = fabs(tmpMinMaxRect.left - m_stPanelInfo.rcRect.left);
		tmpMinMaxRect.right = fabs(tmpMinMaxRect.right - m_stPanelInfo.rcRect.left);
		tmpMinMaxRect.top = fabs(tmpMinMaxRect.top - m_stPanelInfo.rcRect.bottom);
		tmpMinMaxRect.bottom = fabs(tmpMinMaxRect.bottom - m_stPanelInfo.rcRect.bottom);

		if (tmpMinMaxRect.left > tmpMinMaxRect.right)		SWAP(tmpMinMaxRect.left, tmpMinMaxRect.right, dTmpx);
		if (tmpMinMaxRect.bottom > tmpMinMaxRect.top)		SWAP(tmpMinMaxRect.bottom, tmpMinMaxRect.top, dTmpy);

		stMeasure.iFeatureType = static_cast<UINT8>(pFeature->m_eType);
		stMeasure.MinMaxRect = tmpMinMaxRect;

		std::vector<std::string> vcTextString = SplitString(std::string(CT2CA(pTextString->m_strText)), ",");

		stMeasure.iMeasureType = static_cast<UINT8>(GetMeasureType((CString)vcTextString.at(0).c_str()));
		stMeasure.strMeasureType.Format(_T("%s"), (CString)vcTextString.at(0).c_str());
		stMeasure.strMeasureType.MakeUpper();

		if (vcTextString.size() > 1)
		{
			stMeasure.iMeasureSide = static_cast<UINT8>(GetMeasureSide((CString)vcTextString.at(1).c_str()));

			if (vcTextString.size() == 5) //OOS_Min 없음
			{
				std::vector<std::string> vcSplit = SplitString(vcTextString.at(2).c_str(), ";");
				if (vcSplit.size() > 1)
				{
					stMeasure.stSpec.dRange = _ttof((CString)vcSplit.at(0).c_str());
					stMeasure.stSpec.dMin = _ttof((CString)vcSplit.at(1).c_str());
				}
				else
					stMeasure.stSpec.dMin = _ttof((CString)vcTextString.at(2).c_str());

				stMeasure.stSpec.dTarget = _ttof((CString)vcTextString.at(3).c_str());
				stMeasure.stSpec.dMax = _ttof((CString)vcTextString.at(4).c_str());
			}
			else
			{
				stMeasure.stSpec.dMin = 0.0;
				stMeasure.stSpec.dTarget = 0.0;
				stMeasure.stSpec.dMax = 0.0;
			}
		}

		//YJD START 2021.11.29 - ALIGN EXPAND CAM
		/*
		if ((MeasureType)stMeasure.iMeasureType == MeasureType::align ||
			(MeasureType)stMeasure.iMeasureType == MeasureType::align_plating)
		*/
		if ((MeasureType)stMeasure.iMeasureType == MeasureType::align)
			//YJD END 2021.11.29 - ALIGN EXPAND CAM		
		{
			stMeasure.iUnitIndex = -1;
			stMeasure.pFeature = (void*)pFeature;
			vcMeasureItems.emplace_back(stMeasure);
			continue;
		}

		RECTD rcStrip;
		RECTD rcUnit;
		BOOL bFind = FALSE;
		BOOL bCheckUnit = FALSE;
		for (auto itStrip : m_stPanelInfo.vcStripInfo)
		{
			//rcStrip = itStrip->rcRect;

			for (auto itUnit : itStrip->vcUnitInfo)
			{
				rcUnit = itUnit->rcRect;

				if ((rcStrip.IsPtInRectD_R(tmpMinMaxRect.left, tmpMinMaxRect.top) == TRUE &&
					rcStrip.IsPtInRectD_R(tmpMinMaxRect.right, tmpMinMaxRect.bottom) == TRUE) &&
					(rcUnit.IsPtInRectD_R(tmpMinMaxRect.left, tmpMinMaxRect.top) == FALSE &&
					(rcUnit.IsPtInRectD_R(tmpMinMaxRect.right, tmpMinMaxRect.bottom) == FALSE)))
				{
					stMeasure.iUnitIndex = itUnit->iUnitIdx_Ori;
					stMeasure.iStripIndex = itStrip->iStripIdx;
					bFind = TRUE;
					break;
				}

				//rcUnit = itUnit->rcRect;				
				// Unit 인 경우 Check
				if (rcUnit.IsPtInRectD_R(tmpMinMaxRect.left, tmpMinMaxRect.top) &&
					rcUnit.IsPtInRectD_R(tmpMinMaxRect.right, tmpMinMaxRect.bottom))
				{
					stMeasure.iUnitIndex = itUnit->iUnitIdx_Ori;
					stMeasure.iStripIndex = itUnit->iStripIdx;
					bFind = TRUE;
					break;
				}
			}
			if (bFind)
			{
				bCheckUnit = TRUE;
				break;
			}
		}

		// Unit In 이 아닌 Strip In 경우를 위해 다시 For문
		if (bCheckUnit == FALSE)
		{
			for (auto itStrip : m_stPanelInfo.vcStripInfo)
			{
				for (auto itUnit : itStrip->vcUnitInfo)
				{
					rcStrip = itStrip->rcRect;
					rcUnit = itUnit->rcRect;

					// Strip Dummy 인 경우 Check
					if ((rcStrip.IsPtInRectD_R(tmpMinMaxRect.left, tmpMinMaxRect.top) == TRUE &&
						rcStrip.IsPtInRectD_R(tmpMinMaxRect.right, tmpMinMaxRect.bottom) == TRUE) &&
						(rcUnit.IsPtInRectD_R(tmpMinMaxRect.left, tmpMinMaxRect.top) == FALSE &&
							rcUnit.IsPtInRectD_R(tmpMinMaxRect.right, tmpMinMaxRect.bottom) == FALSE))
					{
						stMeasure.iUnitIndex = itUnit->iUnitIdx_Ori;
						stMeasure.iStripIndex = itStrip->iStripIdx;
						bFind = TRUE;
						break;
					}

				}
				if (bFind)
				{
					bCheckUnit = TRUE;
					break;
				}
			}
		}

		stMeasure.pFeature = (void*)pFeature;
		vcMeasureItems.emplace_back(stMeasure);

	}

	rotMirrArr.ClearAll();

	if (vcMeasureItems.size() <= 0)
	return RESULT_BAD;

	std::vector<INT32> vcMeasureUnits;
	std::vector<INT32> vcMeasureStrips;
	double width, height;

	int nPanelStep = m_pUserLayerSetInfo->nPanelStepIdx;
	vector<int> vecStripStep = m_pUserLayerSetInfo->vcStripStepIdx;
	vector<int> vecUnitStep = m_pUserLayerSetInfo->vcUnitStepIdx;

	for (auto it : vcMeasureItems)
	{
		// Check Position 
		BOOL bCheckPanel = FALSE;
		BOOL bCheckStrip = FALSE;
		BOOL bCheckUnit = FALSE;
		int stripidx, unitidx = -1;
		// True 면 In , False 면 Out
		bCheckPanel = CDrawFunction::CheckPanelInOut(m_vecvecRect, &it, nPanelStep);
		for (int nStep = 0; nStep < static_cast<int>(vecStripStep.size()); nStep++)
		{
			bCheckStrip = CDrawFunction::CheckStripInOut(m_vecvecRect, &it, stripidx, vecStripStep[nStep]);
			if (bCheckStrip == TRUE)
			{
				break;
			}
		}
		for (int nStep = 0; nStep < static_cast<int>(vecUnitStep.size()); nStep++)
		{
			bCheckUnit = CDrawFunction::CheckUnitInOut(m_vecvecRect, &it, unitidx, vecUnitStep[nStep]);
			if (bCheckUnit == TRUE)
			{
				break;
			}
		}

		// Panel In, Strip In, Unit In --> Unit Align, Point
		if (bCheckPanel == TRUE && bCheckStrip == TRUE && bCheckUnit == TRUE)
		{
			INT32 iUnitIndex = it.iUnitIndex;
			auto itInserted = std::find(vcMeasureUnits.begin(), vcMeasureUnits.end(), iUnitIndex);
			if (itInserted != vcMeasureUnits.end())
				continue;

			MeasureUnit_t* pUnit = new MeasureUnit_t;

			for (auto itUnit : vcMeasureItems)
			{
				if (iUnitIndex != itUnit.iUnitIndex)
					continue;

				for (int nStep = 0; nStep < static_cast<int>(vecUnitStep.size()); nStep++)
				{
					bCheckUnit = CDrawFunction::CheckUnitInOut(m_vecvecRect, &itUnit, unitidx, vecUnitStep[nStep]);
					if (bCheckUnit == TRUE)
					{
						break;
					}
				}
				if (bCheckUnit == FALSE)
					continue;

				// 2022.07.07 KJH ADD
				// SHIFT 예외 처리
				//if ((MeasureType)itUnit.iMeasureType == MeasureType::shift)
				//	continue;				

				MeasureItem_t* pMeasure = new MeasureItem_t;
				*pMeasure = itUnit;

				pUnit->iIndex = iUnitIndex;

				if ((MeasureType)itUnit.iMeasureType == MeasureType::fm_p)		// Unit Align
				{
					pMeasure->stSpec.dRange = 0.;
					pMeasure->bIs3D = FALSE;
					pMeasure->iScanMode = 0;

					if (m_pSystemSpec->sysNSIS.sysUnitAlignLens == 0. && m_pSystemSpec->sysNSIS.sysUnitAlignZoom == 0.)
					{
						width = abs(pMeasure->MinMaxRect.right - pMeasure->MinMaxRect.left);
						height = abs(pMeasure->MinMaxRect.top - pMeasure->MinMaxRect.bottom);
						// mm -> um 단위 변경
						width *= 1000.;
						height *= 1000.;
						CheckLensZoom_SNU(width * 4, height * 4, pMeasure);
						// Unit Align Optic 고정
						//pMeasure->dLens = 10.0;
						//pMeasure->dZoom = 0.5;
					}
					else
					{
						pMeasure->dLens = m_pSystemSpec->sysNSIS.sysUnitAlignLens;
						pMeasure->dZoom = m_pSystemSpec->sysNSIS.sysUnitAlignZoom;
					}

					pUnit->vcAlign.emplace_back(pMeasure);

				}
				else // Unit Measure Points
				{
					pMeasure->bIs3D = TRUE;
					pMeasure->iScanMode = 1;
					width = abs(pMeasure->MinMaxRect.right - pMeasure->MinMaxRect.left);
					height = abs(pMeasure->MinMaxRect.top - pMeasure->MinMaxRect.bottom);

					// mm -> um 단위 변환
					width *= 1000;
					height *= 1000;
					if (pMeasure->stSpec.dRange == 0)
					{
						width += 100.;
						height += 100.;
					}
					else
					{
						width += pMeasure->stSpec.dRange;
						height += pMeasure->stSpec.dRange;
					}

					// Pattern 및 여유공간 2배 이상 FOV 선정

					//YJD START 2022.04.08 - MEASURE TYPE SCALE
					/*
					width *= 2;
					height *= 2;
					*/
					if ((MeasureType)itUnit.iMeasureType < MeasureType::MeasureTypeNone)
					{
						width *= m_pSystemSpec->sysNSIS.sysMeasureTypeScale[itUnit.iMeasureType];
						height *= m_pSystemSpec->sysNSIS.sysMeasureTypeScale[itUnit.iMeasureType];
					}
					else
					{
						width *= m_pSystemSpec->sysNSIS.sysMeasureDefaultScale;
						height *= m_pSystemSpec->sysNSIS.sysMeasureDefaultScale;
					}
					//YJD END 2022.04.08 - MEASURE TYPE SCALE

					CheckLensZoom_SNU(width, height, pMeasure);
					pUnit->vcPoints.emplace_back(pMeasure);

					/*bool bFind = false;
					for (auto itStripInfo : m_stPanelInfo.vcStripInfo)
					{
						for (auto itUnitInfo : itStripInfo->vcUnitInfo)
						{
							RECTD rcUnit;
							if (m_pSystemSpec->sysBasic.bProfileMode)
							{
								rcUnit = itUnitInfo->rcProfile;
							}
							else
							{
								rcUnit = itUnitInfo->rcRect;
							}

							if (rcUnit.IsPtInRectD_R(itUnit.MinMaxRect.left, itUnit.MinMaxRect.top) &&
								rcUnit.IsPtInRectD_R(itUnit.MinMaxRect.right, itUnit.MinMaxRect.bottom))
							{
								pUnit->iType = itUnitInfo->iType;
								pUnit->UnitRect = rcUnit;
								pUnit->UnitRect_In_Panel = itUnitInfo->rcRect_In_Panel;
								bFind = true;
								break;
							}
						}
						if (bFind)
							break;
					}*/

				}

				for (auto itStripInfo : m_stPanelInfo.vcStripInfo)
				{
					bool bFind = false;
					RECTD rcUnit;
					for (auto itUnitInfo : itStripInfo->vcUnitInfo)
					{
						if (m_pSystemSpec->sysBasic.bProfileMode)
						{
							rcUnit = itUnitInfo->rcProfile;
						}
						else
						{
							rcUnit = itUnitInfo->rcRect;
						}

						if (rcUnit.IsPtInRectD_R(itUnit.MinMaxRect.left, itUnit.MinMaxRect.top) &&
							rcUnit.IsPtInRectD_R(itUnit.MinMaxRect.right, itUnit.MinMaxRect.bottom))
						{
							pUnit->iType = itUnitInfo->iType;
							pUnit->UnitRect = rcUnit;
							pUnit->UnitRect_In_Panel = itUnitInfo->rcRect_In_Panel;
							bFind = true;
							break;
						}
					}
					if (bFind)
						break;
				}
			}
			vcMeasureUnits.emplace_back(iUnitIndex);
			m_stPanelInfo.stMeasureInfo.vcUnits.emplace_back(pUnit);
		}
		// Panel In, Strip In, Unit Out --> Strip Dummy
		else if (bCheckPanel == TRUE && bCheckStrip == TRUE && bCheckUnit == FALSE)
		{
			INT32 iStripIndex = it.iStripIndex;
			auto itInserted = std::find(vcMeasureStrips.begin(), vcMeasureStrips.end(), iStripIndex);
			if (itInserted != vcMeasureStrips.end())
				continue;

			MeasureUnit_t* pStrip = new MeasureUnit_t;

			for (auto itStrip : vcMeasureItems)
			{
				if (iStripIndex != itStrip.iStripIndex)
					continue;

				for (int nStep = 0; nStep < static_cast<int>(vecStripStep.size()); nStep++)
				{
					bCheckStrip = CDrawFunction::CheckStripInOut(m_vecvecRect, &itStrip, stripidx, vecStripStep[nStep]);
					if (bCheckStrip == TRUE)
					{
						break;
					}
				}
				for (int nStep = 0; nStep < static_cast<int>(vecUnitStep.size()); nStep++)
				{
					bCheckUnit = CDrawFunction::CheckUnitInOut(m_vecvecRect, &itStrip, unitidx, vecUnitStep[nStep]);
					if (bCheckUnit == TRUE)
					{
						break;
					}
				}

				if (bCheckStrip == TRUE && bCheckUnit == TRUE)
					continue;

				MeasureItem_t* pMeasure = new MeasureItem_t;
				*pMeasure = itStrip;

				pStrip->iIndex = iStripIndex;

				pMeasure->bIs3D = TRUE;
				pMeasure->iScanMode = 1;
				width = abs(pMeasure->MinMaxRect.right - pMeasure->MinMaxRect.left);
				height = abs(pMeasure->MinMaxRect.top - pMeasure->MinMaxRect.bottom);

				// mm -> um 단위 변환
				width *= 1000;
				height *= 1000;
				if (pMeasure->stSpec.dRange == 0)
				{
					width += 100.;
					height += 100.;
				}
				else
				{
					width += pMeasure->stSpec.dRange;
					height += pMeasure->stSpec.dRange;
				}

				// Pattern 및 여유공간 2배 이상 FOV 선정

				//YJD START 2022.04.08 - MEASURE TYPE SCALE
				/*
				width *= 2;
				height *= 2;
				*/
				/*if ((MeasureType)itStrip.iMeasureType < MeasureType::MeasureTypeNone)
				{
					width *= m_pSystemSpec->sysNSIS.sysMeasureTypeScale[itStrip.iMeasureType];
					height *= m_pSystemSpec->sysNSIS.sysMeasureTypeScale[itStrip.iMeasureType];
				}
				else
				{
				width *= m_pSystemSpec->sysNSIS.sysMeasureDefaultScale;
					height *= m_pSystemSpec->sysNSIS.sysMeasureDefaultScale;
				}
				//YJD END 2022.04.08 - MEASURE TYPE SCALE*/
				//if ((MeasureType)itStrip.iMeasureType == MeasureType::shift)
				//{
				width *= m_pSystemSpec->sysNSIS.sysMeasureDefaultScale;
				height *= m_pSystemSpec->sysNSIS.sysMeasureDefaultScale;
				CheckLensZoom_SNU(width, height, pMeasure);
				pStrip->vcPoints.emplace_back(pMeasure);
				//}



				for (auto itStripInfo : m_stPanelInfo.vcStripInfo)
				{
					RECTD rcStrip;
					rcStrip = itStripInfo->rcRect;
					bool bFind = false;
					RECTD rcUnit;
					if (m_pSystemSpec->sysBasic.bProfileMode)
					{
						rcUnit = itStripInfo->rcProfile;
					}
					else
					{
						rcUnit = itStripInfo->rcRect;
					}

					if (rcUnit.IsPtInRectD_R(it.MinMaxRect.left, it.MinMaxRect.top) &&
						rcUnit.IsPtInRectD_R(it.MinMaxRect.right, it.MinMaxRect.bottom))
					{
						pStrip->iType = itStripInfo->iType;
						pStrip->UnitRect = rcUnit;
						pStrip->UnitRect_In_Panel = itStripInfo->rcRect;
						bFind = true;
						break;
					}
				}
			}
			vcMeasureStrips.emplace_back(iStripIndex);
			m_stPanelInfo.stMeasureInfo.vcStripDummy.emplace_back(pStrip);

		}
		// Panel In, Strip Out -->> Panel Align
		else if (bCheckPanel == TRUE && bCheckStrip == FALSE)
		{
			//if ((MeasureType)it.iMeasureType == MeasureType::align ||
			//	(MeasureType)it.iMeasureType == MeasureType::align_plating)	// Panel Align
			if ((MeasureType)it.iMeasureType == MeasureType::align)
			{
				MeasureItem_t* pMeasure = new MeasureItem_t;
				*pMeasure = it;
				pMeasure->stSpec.dRange = 0.;
				pMeasure->bIs3D = FALSE;
				pMeasure->iScanMode = 5;

				if (m_pSystemSpec->sysNSIS.sysAlignLens == 0. && m_pSystemSpec->sysNSIS.sysAlignZoom == 0.)
				{
					width = abs(pMeasure->MinMaxRect.right - pMeasure->MinMaxRect.left);
					height = abs(pMeasure->MinMaxRect.top - pMeasure->MinMaxRect.bottom);
					// mm -> um 단위 변환
					width *= 1000.;
					height *= 1000.;
					CheckLensZoom_SNU(width * 4, height * 4, pMeasure);
					// PCB Align Optic 고정
					//pMeasure->dLens = 2.5;
					//pMeasure->dZoom = 0.5;
				}
				else
				{
					pMeasure->dLens = m_pSystemSpec->sysNSIS.sysAlignLens;
					pMeasure->dZoom = m_pSystemSpec->sysNSIS.sysAlignZoom;
				}

				m_stPanelInfo.stMeasureInfo.vcAlign.emplace_back(pMeasure);

				//YJD START 2021.11.29 - ALIGN EXPAND CAM
				if (m_pSystemSpec->sysNSIS.sysUseExCam)
				{
					MeasureItem_t* pMeasureEx = new MeasureItem_t;
					*pMeasureEx = it;

					pMeasureEx->stSpec.dRange = 0.;
					pMeasureEx->bIs3D = FALSE;
					pMeasureEx->iScanMode = 5;
					pMeasureEx->dLens = m_pSystemSpec->sysNSIS.sysAlignExCamLens;
					pMeasureEx->dZoom = m_pSystemSpec->sysNSIS.sysAlignExCamZoom;
					pMeasureEx->bAlignExCam = true;

					m_stPanelInfo.stMeasureInfo.vcAlign.emplace_back(pMeasureEx);
				}
				//YJD END 2021.11.29 - ALIGN EXPAND CAM
			}
		}
		else if (bCheckPanel == FALSE)
		{
			continue;
		}
	}

	if (RESULT_GOOD != SortMeasureUnit())
	return RESULT_BAD;

	RECTD rcUnit;
	for (auto it : vcMeasureUnits)
	{
		bool bFind = false;
		for (auto itStrip : m_stPanelInfo.vcStripInfo)
		{
			for (auto itUnit : itStrip->vcUnitInfo)
			{
				if (it != itUnit->iUnitIdx_Ori)
					continue;

				rcUnit = itUnit->rcRect;
				bFind = true;
				break;
			}

			if (bFind)
				break;
		}
		if (!bFind)
		{
			vcMeasureUnits.clear();
			vcMeasureItems.clear();
			return RESULT_BAD;
		}

		RECTD* pRect = new RECTD;
		*pRect = rcUnit;
		m_stPanelInfo.stMeasureInfo.vcUnitRects.insert(std::make_pair(it, pRect));
	}

	// 측정 Unit 정렬
	if (RESULT_GOOD != SortMeasureUnit())
	return RESULT_BAD;

	vcMeasureUnits.clear();
	vcMeasureItems.clear();
	;
	//Panel Align 정보가 없을 경우
	if (m_stPanelInfo.stMeasureInfo.vcAlign.size() <= 0)
	return RESULT_BAD;

	//측정 Unit 정보가 없을 경우
	/*if (m_stPanelInfo.stMeasureInfo.vcUnits.size() <= 0 ||
		m_stPanelInfo.stMeasureInfo.vcUnitRects.size() != m_stPanelInfo.stMeasureInfo.vcUnits.size())
		return RESULT_BAD;*/
	if (m_stPanelInfo.stMeasureInfo.vcUnits.size() <= 0)
	return RESULT_BAD;


	return RESULT_GOOD;
}

UINT32 CNSIS::EstimateMeasureInfo_Sem(IN INT32 iStepIdx, IN enum Orient eOrient)
{
	m_stPanelInfo.stMeasureInfo.Clear();
	
	if (iStepIdx < 0 || m_pJobData == nullptr || m_pUserSetInfo == nullptr ||
		m_pUserLayerSetInfo == nullptr || m_pSystemSpec == nullptr)
		return RESULT_BAD;

	//Panel Step	
	if (iStepIdx != m_pUserLayerSetInfo->nPanelStepIdx)
		iStepIdx = m_pUserLayerSetInfo->nPanelStepIdx;

	CStep *pStep = m_pJobData->m_arrStep[iStepIdx];
	if (pStep == nullptr)
		return RESULT_BAD;

	CLayer* pTargetLayer_MP = nullptr;
	for (auto it : m_pUserLayerSetInfo->vcLayerSet)
	{
		if (it->strDefine.CompareNoCase(MEASURE_LAYER_TXT) != 0)
			continue;

		for (UINT32 lyno = 0; lyno < pStep->m_arrLayer.GetSize(); lyno++)
		{
			CLayer* pLayer = pStep->m_arrLayer.GetAt(lyno);
			if (pLayer == nullptr) continue;
			if (it->strLayer.CompareNoCase(pLayer->m_strLayerName) != 0) continue;

			pTargetLayer_MP = pLayer;
			break;
		}
	}

	if (pTargetLayer_MP == nullptr)
		return RESULT_BAD;
		
	std::vector<MeasureItem_t> vcMeasureItems;	

	CRotMirrArr rotMirrArr;
	rotMirrArr.InputData(0.0, 0.0, eOrient, m_pUserSetInfo->mirrorDir);
	
	CString strText = L"";
	double dTmpx, dTmpy;

	// Panel, Strip, Unit Rect 가져오기
	m_vecvecRect.clear();
	m_vecvecRect = CDrawFunction::GetProfileData(m_pJobData, iStepIdx);

	// Align Size
	int nAlignMaskCount = static_cast<int>(pTargetLayer_MP->m_arrAlign.GetCount());
	//// Align Check
	for (int nAlignMask = 0; nAlignMask < nAlignMaskCount; nAlignMask++)
	{
		CTypedPtrArray<CObArray, CFeature*> *pMaskArr;

		CAlignMask *pAlignMask = pTargetLayer_MP->m_arrAlign.GetAt(nAlignMask);
		pMaskArr = &pAlignMask->m_arrFeature;
		
		if (pMaskArr == nullptr) continue;;

		int nFeatureCount = static_cast<INT>(pMaskArr->GetCount());
		for (int nfeno = nFeatureCount - 1; nfeno >= 0; nfeno--)
		{
			CFeature *pFeature = pMaskArr->GetAt(nfeno);

			if (pFeature == nullptr) continue;

			RECTD tmpMinMaxRect = pFeature->m_MinMaxRect;
			rotMirrArr.FinalPoint(&tmpMinMaxRect.left, &tmpMinMaxRect.top, tmpMinMaxRect.left, tmpMinMaxRect.top);
			rotMirrArr.FinalPoint(&tmpMinMaxRect.right, &tmpMinMaxRect.bottom, tmpMinMaxRect.right, tmpMinMaxRect.bottom);

			MeasureItem_t stMeasure;
			stMeasure.MinMaxRect_In_Panel = GetDrawRect(tmpMinMaxRect, m_pUserSetInfo);

			tmpMinMaxRect.left = fabs(tmpMinMaxRect.left - m_stPanelInfo.rcRect.left);
			tmpMinMaxRect.right = fabs(tmpMinMaxRect.right - m_stPanelInfo.rcRect.left);
			tmpMinMaxRect.top = fabs(tmpMinMaxRect.top - m_stPanelInfo.rcRect.bottom);
			tmpMinMaxRect.bottom = fabs(tmpMinMaxRect.bottom - m_stPanelInfo.rcRect.bottom);

			if (tmpMinMaxRect.left > tmpMinMaxRect.right)		SWAP(tmpMinMaxRect.left, tmpMinMaxRect.right, dTmpx);
			if (tmpMinMaxRect.bottom > tmpMinMaxRect.top)		SWAP(tmpMinMaxRect.bottom, tmpMinMaxRect.top, dTmpy);

			tmpMinMaxRect.NormalizeRectD();

			stMeasure.iFeatureType = static_cast<UINT8>(pFeature->m_eType);
			stMeasure.MinMaxRect = tmpMinMaxRect;						
			

			stMeasure.iMeasureSide = static_cast<UINT8>(m_pUserSetInfo->prodSide);

			// oos, oor, ooc 값 0.0f 로 Set
			stMeasure.stSpec.dMin = 0.0;
			stMeasure.stSpec.dMax = 0.0;
			stMeasure.stSpec.dMin_OOC = 0.0;
			stMeasure.stSpec.dMax_OOC = 0.0;
			stMeasure.stSpec.dMin_OOR = 0.0;
			stMeasure.stSpec.dMax_OOR = 0.0;

			//UNIT in check
			RECTD rcStrip;
			RECTD rcUnit;		
			BOOL bFind = FALSE;
			for (auto itStrip : m_stPanelInfo.vcStripInfo)
			{
				for (auto itUnit : itStrip->vcUnitInfo)
				{
					rcStrip = itStrip->rcRect;
					rcUnit = itUnit->rcRect;

					// Unit 인 경우 Check
					if (rcUnit.IsPtInRectD_R(tmpMinMaxRect.left, tmpMinMaxRect.top) &&
						rcUnit.IsPtInRectD_R(tmpMinMaxRect.right, tmpMinMaxRect.bottom))
					{
						stMeasure.iUnitIndex = itUnit->iUnitIdx_Ori;
						stMeasure.iStripIndex = itUnit->iStripIdx;
						stMeasure.stStepRpt = itUnit->stStepRpt;

						itUnit->bInspect = TRUE;
						bFind = TRUE;
						break;
					}
				}

				if (bFind == TRUE)
				{
					break;
				}
			}

			if (bFind == FALSE)
			{
				//Strip in check
				RECTD rcStrip;
				RECTD rcUnit;
				for (auto itStrip : m_stPanelInfo.vcStripInfo)
				{
					for (auto itUnit : itStrip->vcUnitInfo)
					{

						rcStrip = itStrip->rcRect;
						rcUnit = itUnit->rcRect;

						// Strip 인 경우 Check
						if ((rcStrip.IsPtInRectD_R(tmpMinMaxRect.left, tmpMinMaxRect.top) == TRUE &&
							rcStrip.IsPtInRectD_R(tmpMinMaxRect.right, tmpMinMaxRect.bottom) == TRUE) &&
							(rcUnit.IsPtInRectD_R(tmpMinMaxRect.left, tmpMinMaxRect.top) == FALSE &&
								rcUnit.IsPtInRectD_R(tmpMinMaxRect.right, tmpMinMaxRect.bottom) == FALSE))
						{
							stMeasure.iUnitIndex = itUnit->iUnitIdx_Ori;
							stMeasure.iStripIndex = itStrip->iStripIdx;
							stMeasure.stStepRpt = itStrip->stStepRpt;

							itStrip->bInspect = TRUE;
							bFind = TRUE;
							break;
						}
					}

					if (bFind == TRUE)
					{
						break;
					}
				}
			}

			
			// Check Position 
			bool bCheckPanel = FALSE;
			bool bCheckStrip = FALSE;
			bool bCheckUnit = FALSE;
			int stripidx, unitidx = -1;
			int nStripStep;
			int nUnitStep;

			//
			int nPanelStepIndex = m_pUserLayerSetInfo->nPanelStepIdx;
			vector<int> nStripStepIndex = m_pUserLayerSetInfo->vcStripStepIdx;
			vector<int> nUnitStepIndex = m_pUserLayerSetInfo->vcUnitStepIdx;

			// True 면 In , False 면 Out
			//Panel Step Check
			bCheckPanel = CDrawFunction::CheckPanelInOut(m_vecvecRect, &stMeasure, nPanelStepIndex);
			//Strip Step Check
			for (nStripStep = 0; nStripStep < static_cast<int>(nStripStepIndex.size()); nStripStep++)
			{
				bCheckStrip = CDrawFunction::CheckStripInOut(m_vecvecRect, &stMeasure, stripidx, nStripStepIndex[nStripStep]);
				if (bCheckStrip == TRUE)
				{
					break;
				}
			}
			//Unit Step Check
			for (nUnitStep = 0; nUnitStep < static_cast<int>(nUnitStepIndex.size()); nUnitStep++)
			{
				bCheckUnit = CDrawFunction::CheckUnitInOut(m_vecvecRect, &stMeasure, unitidx, nUnitStepIndex[nUnitStep]);
				if (bCheckUnit == TRUE)
				{
					break;
				}
			}

			// Panel In, Strip In, Unit In
			if (bCheckPanel == TRUE && bCheckStrip == TRUE && bCheckUnit == TRUE)
			{
				// Unit Align				
				stMeasure.iMeasureType = static_cast<UINT8>(MeasureType::fm_p);
				stMeasure.strMeasureType.Format(_T("fm_p"));
				stMeasure.strMeasureType.MakeUpper();

				//stMeasure.iUnitIndex = -1;
				stMeasure.pFeature = (void*)pFeature;
				vcMeasureItems.emplace_back(stMeasure);
				continue;

			}					
			// Panel In, Strip In, Unit Out --> Strip Dummy
			else if (bCheckPanel == TRUE && bCheckStrip == TRUE && bCheckUnit == FALSE)					
			{				
				// Strip Align							
				stMeasure.iMeasureType = static_cast<UINT8>(MeasureType::fm_p);
				stMeasure.strMeasureType.Format(_T("fm_p"));
				stMeasure.strMeasureType.MakeUpper();

				//stMeasure.iUnitIndex = -1;				
				stMeasure.pFeature = (void*)pFeature;
				vcMeasureItems.emplace_back(stMeasure);
				continue;						
			}
				
			// Strip Out --> Panel Align
			else if (bCheckPanel == TRUE && bCheckStrip == FALSE && bCheckUnit == FALSE)
			{
				// Panel Align				
				// iMeasureType, strMeasureType Align Set
				stMeasure.iMeasureType = static_cast<UINT8>(MeasureType::align);
				stMeasure.strMeasureType.Format(_T("align"));
				stMeasure.strMeasureType.MakeUpper();

				//stMeasure.iUnitIndex = -1;
				stMeasure.pFeature = (void*)pFeature;
				vcMeasureItems.emplace_back(stMeasure);
				continue;					
			}
			
			// Panel Out
			else if (bCheckPanel == FALSE)
			{
				continue;
			}

		}	
	}

	// Feature Size
	int nFeatureCount = static_cast<int>(pTargetLayer_MP->m_FeatureFile.m_arrFeature.GetCount());
	// Feature Check	
	for (int nFeature = 0; nFeature < nFeatureCount; nFeature++)
	{
		CFeature* pFeature = pTargetLayer_MP->m_FeatureFile.m_arrFeature.GetAt(nFeature);
		if (pFeature == nullptr)
			continue;

		CAttributeTextString* pTextString = new CAttributeTextString();

		if (pFeature->m_arrAttributeTextString.GetCount() > 0)
		{
			pTextString = pFeature->m_arrAttributeTextString.GetAt(0);
			if (pTextString == nullptr)
				continue;
		}

		RECTD tmpMinMaxRect = pFeature->m_MinMaxRect;
		rotMirrArr.FinalPoint(&tmpMinMaxRect.left, &tmpMinMaxRect.top, tmpMinMaxRect.left, tmpMinMaxRect.top);
		rotMirrArr.FinalPoint(&tmpMinMaxRect.right, &tmpMinMaxRect.bottom, tmpMinMaxRect.right, tmpMinMaxRect.bottom);

		MeasureItem_t stMeasure;
		stMeasure.MinMaxRect_In_Panel = GetDrawRect(tmpMinMaxRect, m_pUserSetInfo);

		tmpMinMaxRect.left = fabs(tmpMinMaxRect.left - m_stPanelInfo.rcRect.left);
		tmpMinMaxRect.right = fabs(tmpMinMaxRect.right - m_stPanelInfo.rcRect.left);
		tmpMinMaxRect.top = fabs(tmpMinMaxRect.top - m_stPanelInfo.rcRect.bottom);
		tmpMinMaxRect.bottom = fabs(tmpMinMaxRect.bottom - m_stPanelInfo.rcRect.bottom);

		if (tmpMinMaxRect.left > tmpMinMaxRect.right)		SWAP(tmpMinMaxRect.left, tmpMinMaxRect.right, dTmpx);
		if (tmpMinMaxRect.bottom > tmpMinMaxRect.top)		SWAP(tmpMinMaxRect.bottom, tmpMinMaxRect.top, dTmpy);

		stMeasure.iFeatureType = static_cast<UINT8>(pFeature->m_eType);
		stMeasure.MinMaxRect = tmpMinMaxRect;


		std::vector<std::string> vcTextString;
		if (pTextString->m_strText == "")
		{
			stMeasure.iMeasureType = static_cast<UINT8>(MeasureType::MeasureTypeNone);
			stMeasure.strMeasureType.Format(_T("NONE"));
		}
		else
		{
			vcTextString = SplitString(std::string(CT2CA(pTextString->m_strText)), ",");
			stMeasure.iMeasureType = static_cast<UINT8>(GetMeasureType_SEM((CString)vcTextString.at(0).c_str()));
			stMeasure.strMeasureType.Format(_T("%s"), (CString)vcTextString.at(0).c_str());
		}
		stMeasure.strMeasureType.MakeUpper();
		
		if (vcTextString.size() > 1)
		{
			stMeasure.iMeasureSide = static_cast<UINT8>(GetMeasureSide((CString)vcTextString.at(1).c_str()));

			// ex) t_width, bot, 20, 30, 40 
			//	   --> t_width, bot, min , target, max
			//	   --> t_width, bot, min, max, ooc_min, ooc_max, oor_min, oor_max
			if (vcTextString.size() == 9)
			{
				std::vector<std::string> vcsplit = SplitString(vcTextString.at(2).c_str(), ";");
				stMeasure.stSpec.dMin = _ttof((CString)vcTextString.at(2).c_str());
				stMeasure.stSpec.dMax = _ttof((CString)vcTextString.at(3).c_str());
				stMeasure.stSpec.dMin_OOR = _ttof((CString)vcTextString.at(4).c_str());
				stMeasure.stSpec.dMax_OOR = _ttof((CString)vcTextString.at(5).c_str());
				stMeasure.stSpec.dMin_OOC = _ttof((CString)vcTextString.at(6).c_str());
				stMeasure.stSpec.dMax_OOC = _ttof((CString)vcTextString.at(7).c_str());
// 				stMeasure.stSpec.dMin_OOC = _ttof((CString)vcTextString.at(4).c_str());
// 				stMeasure.stSpec.dMax_OOC = _ttof((CString)vcTextString.at(5).c_str());
// 				stMeasure.stSpec.dMin_OOR = _ttof((CString)vcTextString.at(6).c_str());
// 				stMeasure.stSpec.dMax_OOR = _ttof((CString)vcTextString.at(7).c_str());
			}
			else
			{
				stMeasure.stSpec.dMin = 0.0;
				stMeasure.stSpec.dMax = 0.0;
				stMeasure.stSpec.dMin_OOC = 0.0;
				stMeasure.stSpec.dMax_OOC = 0.0;
				stMeasure.stSpec.dMin_OOR = 0.0;
				stMeasure.stSpec.dMax_OOR = 0.0;
			}
		}
		else
		{
			stMeasure.iMeasureSide = static_cast<UINT8>(m_pUserSetInfo->prodSide);
			stMeasure.stSpec.dMin = 0.0;
			stMeasure.stSpec.dMax = 0.0;
			stMeasure.stSpec.dMin_OOC = 0.0;
			stMeasure.stSpec.dMax_OOC = 0.0;
			stMeasure.stSpec.dMin_OOR = 0.0;
			stMeasure.stSpec.dMax_OOR = 0.0;
		}
		RECTD rcStrip;
		RECTD rcUnit;
		BOOL bFind = FALSE;
		for (auto itStrip : m_stPanelInfo.vcStripInfo)
		{
			for (auto itUnit : itStrip->vcUnitInfo)
			{	
				rcStrip = itStrip->rcRect;
				rcUnit = itUnit->rcRect;
				
				// Unit 인 경우 Check
				if (rcUnit.IsPtInRectD_R(tmpMinMaxRect.left, tmpMinMaxRect.top) &&
					rcUnit.IsPtInRectD_R(tmpMinMaxRect.right, tmpMinMaxRect.bottom))
				{
					stMeasure.iUnitIndex = itUnit->iUnitIdx_Ori;
					stMeasure.iStripIndex = itUnit->iStripIdx;					
					stMeasure.stStepRpt = itUnit->stStepRpt;
					
					itUnit->bInspect = TRUE;
					bFind = TRUE;
					break;
				}

			}
			if (bFind)
			{
				break;
			}
		}

		// Strip Check
		if (bFind == FALSE)
		{
			for (auto itStrip : m_stPanelInfo.vcStripInfo)
			{
				for (auto itUnit : itStrip->vcUnitInfo)
				{
					rcStrip = itStrip->rcRect;
					rcUnit = itUnit->rcRect;

					// Strip Dummy 인 경우 Check
					if ((rcStrip.IsPtInRectD_R(tmpMinMaxRect.left, tmpMinMaxRect.top) == TRUE &&
						rcStrip.IsPtInRectD_R(tmpMinMaxRect.right, tmpMinMaxRect.bottom) == TRUE) &&
						(rcUnit.IsPtInRectD_R(tmpMinMaxRect.left, tmpMinMaxRect.top) == FALSE &&
							rcUnit.IsPtInRectD_R(tmpMinMaxRect.right, tmpMinMaxRect.bottom) == FALSE))
					{
						stMeasure.iUnitIndex = itUnit->iUnitIdx_Ori;
						stMeasure.iStripIndex = itStrip->iStripIdx;
						stMeasure.stStepRpt = itStrip->stStepRpt;

						itStrip->bInspect = TRUE;
						bFind = true;
						break;
					}

				}
				if (bFind == TRUE )
				{
					break;
				}
			}
		}




		stMeasure.pFeature = (void*)pFeature;
		vcMeasureItems.emplace_back(stMeasure);
		
	}

	// 밀어넣기	
	std::vector<INT32> vcMeasureUnits;
	std::vector<INT32> vcMeasureStrips;
	double width, height;

	for (auto it : vcMeasureItems)
	{
		// Check Position 
		bool bCheckPanel = FALSE;
		bool bCheckStrip = FALSE;
		bool bCheckUnit = FALSE;
		int stripidx, unitidx = -1;
		int nStripStep;
		int nUnitStep;

		//
		int nPanelStepIndex = m_pUserLayerSetInfo->nPanelStepIdx;
		vector<int> nStripStepIndex = m_pUserLayerSetInfo->vcStripStepIdx;
		vector<int> nUnitStepIndex = m_pUserLayerSetInfo->vcUnitStepIdx;

		// True 면 In , False 면 Out
		//Panel Step Check
		bCheckPanel = CDrawFunction::CheckPanelInOut(m_vecvecRect, &it, nPanelStepIndex);
		//Strip Step Check
		for (nStripStep = 0; nStripStep < static_cast<int>(nStripStepIndex.size()); nStripStep++)
		{
			bCheckStrip = CDrawFunction::CheckStripInOut(m_vecvecRect, &it, stripidx, nStripStepIndex[nStripStep]);
			if (bCheckStrip == TRUE)
			{
				break;
			}
		}
		//Unit Step Check
		for (nUnitStep = 0; nUnitStep < static_cast<int>(nUnitStepIndex.size()); nUnitStep++)
		{
			bCheckUnit = CDrawFunction::CheckUnitInOut(m_vecvecRect, &it, unitidx, nUnitStepIndex[nUnitStep]);
			if (bCheckUnit == TRUE)
			{
				break;
			}
		}

		// Panel In , Strip In, Unit In --> Unit Align, Point
		if (bCheckPanel == TRUE && bCheckStrip == TRUE && bCheckUnit == TRUE)
		{		
			INT32 iUnitIndex = it.iUnitIndex;
			auto itInserted = std::find(vcMeasureUnits.begin(), vcMeasureUnits.end(), iUnitIndex);
			if (itInserted != vcMeasureUnits.end())
				continue;

			MeasureUnit_t* pUnit = new MeasureUnit_t;			

			pUnit->stStepRpt = it.stStepRpt;
			pUnit->iStripIdx = it.iStripIndex;

			// 2022.07.27 KJH TEST
			pUnit->eMapMode = eMinimapMode::Measure;

			for (auto itUnit : vcMeasureItems)
			{
				if (iUnitIndex != itUnit.iUnitIndex)
					continue;

				for (nUnitStep = 0; nUnitStep < static_cast<int>(nUnitStepIndex.size()); nUnitStep++)
				{
					bCheckUnit = CDrawFunction::CheckUnitInOut(m_vecvecRect, &itUnit, unitidx, nUnitStepIndex[nUnitStep]);
					if (bCheckUnit == TRUE)
					{
						break;
					}
				}
				if (bCheckUnit == FALSE)
					continue;
				

				MeasureItem_t* pMeasure = new MeasureItem_t;
				*pMeasure = itUnit;
				
				pUnit->iIndex = iUnitIndex;

				if ((MeasureType)itUnit.iMeasureType == MeasureType::fm_p)		// Unit Align
				{
					pMeasure->stSpec.dRange = 0.;
					pMeasure->bIs3D = FALSE;
					pMeasure->iScanMode = 0;

					if (m_pSystemSpec->sysNSIS.sysUnitAlignLens == 0. && m_pSystemSpec->sysNSIS.sysUnitAlignZoom == 0.)
					{
						// 2022.10.04 KJH ADD
						// 기존 CheckLensZoom 함수 주석 처리 
						// from SNU Code 적용
						//CheckLensZoom(true, pMeasure);

						width = abs(pMeasure->MinMaxRect.right - pMeasure->MinMaxRect.left);
						height = abs(pMeasure->MinMaxRect.top - pMeasure->MinMaxRect.bottom);
						// mm -> um 단위 변경
						width *= 1000.;
						height *= 1000.;
						
						CheckLensZoom_SNU(width * 4, height * 4, pMeasure);
					}
					else
					{
						pMeasure->dLens = m_pSystemSpec->sysNSIS.sysUnitAlignLens;
						pMeasure->dZoom = m_pSystemSpec->sysNSIS.sysUnitAlignZoom;
					}

					pUnit->vcAlign.emplace_back(pMeasure);
				}
				else // Unit Measure Points
				{
					pMeasure->bIs3D = TRUE;
					pMeasure->iScanMode = 1;

					// 2022.10.04 KJH ADD
					// 기존 CheckLensZoom 함수 주석 처리 
					// from SNU Code 적용
					//CheckLensZoom(false, pMeasure);

					width = abs(pMeasure->MinMaxRect.right - pMeasure->MinMaxRect.left);
					height = abs(pMeasure->MinMaxRect.top - pMeasure->MinMaxRect.bottom);

					// mm -> um 단위 변환
					width *= 1000.;
					height *= 1000.;
					if (pMeasure->stSpec.dRange == 0)
					{
						width += 100.;
						height += 100.;
					}
					else
					{
						width += pMeasure->stSpec.dRange;
						height += pMeasure->stSpec.dRange;
					}

					// Pattern 및 여유공간 2배 이상 FOV 선정

					//YJD START 2022.04.08 - MEASURE TYPE SCALE
					/*
					width *= 2;
					height *= 2;
					*/
					if ((MeasureType)itUnit.iMeasureType < MeasureType::MeasureTypeNone)
					{
						width *= m_pSystemSpec->sysNSIS.sysMeasureTypeScale[itUnit.iMeasureType];
						height *= m_pSystemSpec->sysNSIS.sysMeasureTypeScale[itUnit.iMeasureType];
					}
					else
					{
						width *= m_pSystemSpec->sysNSIS.sysMeasureDefaultScale;
						height *= m_pSystemSpec->sysNSIS.sysMeasureDefaultScale;
					}
					//YJD END 2022.04.08 - MEASURE TYPE SCALE

					CheckLensZoom_SNU(width, height, pMeasure);

					pUnit->vcPoints.emplace_back(pMeasure);
				}

				for (auto itStripInfo : m_stPanelInfo.vcStripInfo)
				{
					bool bFind = false;
					RECTD rcUnit;
					for (auto itUnitInfo : itStripInfo->vcUnitInfo)
					{
						if (m_pSystemSpec->sysBasic.bProfileMode)
						{
							rcUnit = itUnitInfo->rcProfile;
						}
						else
						{
							rcUnit = itUnitInfo->rcRect;
						}

						if (rcUnit.IsPtInRectD_R(itUnit.MinMaxRect.left, itUnit.MinMaxRect.top) &&
							rcUnit.IsPtInRectD_R(itUnit.MinMaxRect.right, itUnit.MinMaxRect.bottom))
						{
							pUnit->iType = itUnitInfo->iType;
							pUnit->UnitRect = rcUnit;
							pUnit->UnitRect_In_Panel = itUnitInfo->rcRect_In_Panel;
							bFind = true;
							break;
						}
					}
					if (bFind)					
						break;					
				}
			}
			vcMeasureUnits.emplace_back(iUnitIndex);
			m_stPanelInfo.stMeasureInfo.vcUnits.emplace_back(pUnit);
		}
		// Panel In, Strip In, Unit Out --> Strip Dummy
		else if (bCheckPanel == TRUE && bCheckStrip == TRUE && bCheckUnit == FALSE)
		{
			INT32 iStripIndex = it.iStripIndex;
			auto itInserted = std::find(vcMeasureStrips.begin(), vcMeasureStrips.end(), iStripIndex);
			if (itInserted != vcMeasureStrips.end())
				continue;

			MeasureUnit_t* pStrip = new MeasureUnit_t;

			for (auto itStrip : vcMeasureItems)
			{
				if (iStripIndex != itStrip.iStripIndex)
					continue;

				//Strip Step Check
				for (nStripStep = 0; nStripStep < static_cast<int>(nStripStepIndex.size()); nStripStep++)
				{
					bCheckStrip = CDrawFunction::CheckStripInOut(m_vecvecRect, &itStrip, stripidx, nStripStepIndex[nStripStep]);
					if (bCheckStrip == TRUE)
					{
						break;
					}
				}
				//Unit Step Check
				for (nUnitStep = 0; nUnitStep < static_cast<int>(nUnitStepIndex.size()); nUnitStep++)
				{
					bCheckUnit = CDrawFunction::CheckUnitInOut(m_vecvecRect, &itStrip, unitidx, nUnitStepIndex[nUnitStep]);
					if (bCheckUnit == TRUE)
					{
						break;
					}
				}

				if (bCheckStrip == TRUE && bCheckUnit == TRUE)
					continue;

				// Strip Dummy			
				pStrip->stStepRpt = itStrip.stStepRpt;
				pStrip->iStripIdx = itStrip.iStripIndex;

				// 2022.07.27 KJH TEST
				pStrip->eMapMode = eMinimapMode::Measure;

				MeasureItem_t* pMeasure = new MeasureItem_t;
				*pMeasure = itStrip;

				if ((MeasureType)pMeasure->iMeasureType == MeasureType::fm_p)		// Strip Align
				{
					pMeasure->stSpec.dRange = 0.;
					pMeasure->bIs3D = FALSE;
					pMeasure->iScanMode = 0;

					if (m_pSystemSpec->sysNSIS.sysUnitAlignLens == 0. && m_pSystemSpec->sysNSIS.sysUnitAlignZoom == 0.)
					{
						// 2022.10.04 KJH ADD
						// 기존 CheckLensZoom 함수 주석 처리 
						// from SNU Code 적용
						//CheckLensZoom(true, pMeasure);

						width = abs(pMeasure->MinMaxRect.right - pMeasure->MinMaxRect.left);
						height = abs(pMeasure->MinMaxRect.top - pMeasure->MinMaxRect.bottom);
						// mm -> um 단위 변경
						width *= 1000.;
						height *= 1000.;

						CheckLensZoom_SNU(width * 4, height * 4, pMeasure);
					}
					else
					{
						pMeasure->dLens = m_pSystemSpec->sysNSIS.sysUnitAlignLens;
						pMeasure->dZoom = m_pSystemSpec->sysNSIS.sysUnitAlignZoom;
					}

					pStrip->vcAlign.emplace_back(pMeasure);
				}
				else // Strip Measure Points
				{
					pMeasure->bIs3D = TRUE;
					pMeasure->iScanMode = 1;

					// 2022.10.04 KJH ADD
					// 기존 CheckLensZoom 함수 주석 처리 
					// from SNU Code 적용
					//CheckLensZoom(false, pMeasure);

					width = abs(pMeasure->MinMaxRect.right - pMeasure->MinMaxRect.left);
					height = abs(pMeasure->MinMaxRect.top - pMeasure->MinMaxRect.bottom);

					// mm -> um 단위 변환
					width *= 1000;
					height *= 1000;
					if (pMeasure->stSpec.dRange == 0)
					{
						width += 100.;
						height += 100.;
					}
					else
					{
						width += pMeasure->stSpec.dRange;
						height += pMeasure->stSpec.dRange;
					}

					width *= m_pSystemSpec->sysNSIS.sysMeasureDefaultScale;
					height *= m_pSystemSpec->sysNSIS.sysMeasureDefaultScale;
					CheckLensZoom_SNU(width, height, pMeasure);

					pStrip->vcPoints.emplace_back(pMeasure);
				}

				for (auto itStripInfo : m_stPanelInfo.vcStripInfo)
				{						
					RECTD rcStrip;
					rcStrip = itStripInfo->rcRect;
					bool bFind = false;
					RECTD rcUnit;
					if (m_pSystemSpec->sysBasic.bProfileMode)
					{
						rcUnit = itStripInfo->rcProfile;
					}
					else
					{
						rcUnit = itStripInfo->rcRect;
					}

					if (rcUnit.IsPtInRectD_R(it.MinMaxRect.left, it.MinMaxRect.top) &&
						rcUnit.IsPtInRectD_R(it.MinMaxRect.right, it.MinMaxRect.bottom))					
					{
						pStrip->iType = itStripInfo->iType;
						pStrip->UnitRect = rcUnit;
						pStrip->UnitRect_In_Panel = itStripInfo->rcRect_In_Panel;
						bFind = true;
						break;
					}
				}
			}
			vcMeasureStrips.emplace_back(iStripIndex);
			m_stPanelInfo.stMeasureInfo.vcStripDummy.emplace_back(pStrip);
		}
		// Panel In, Strip Out --> Panel Align
		else if (bCheckPanel == TRUE && bCheckStrip == FALSE)
		{
			MeasureItem_t* pMeasure = new MeasureItem_t;
			*pMeasure = it;
			pMeasure->stSpec.dRange = 0.;
			pMeasure->bIs3D = FALSE;
			pMeasure->iScanMode = 5;

			if (m_pSystemSpec->sysNSIS.sysAlignLens == 0. && m_pSystemSpec->sysNSIS.sysAlignZoom == 0.)
			{
				// 2022.10.04 KJH ADD
				// 기존 CheckLensZoom 함수 주석 처리 
				// from SNU Code 적용
				//CheckLensZoom(true, pMeasure);

				width = abs(pMeasure->MinMaxRect.right - pMeasure->MinMaxRect.left);
				height = abs(pMeasure->MinMaxRect.top - pMeasure->MinMaxRect.bottom);
				// mm -> um 단위 변환
				width *= 1000.;
				height *= 1000.;
				CheckLensZoom_SNU(width * 4, height * 4, pMeasure);
			}
			else
			{
				pMeasure->dLens = m_pSystemSpec->sysNSIS.sysAlignLens;
				pMeasure->dZoom = m_pSystemSpec->sysNSIS.sysAlignZoom;
			}

			m_stPanelInfo.stMeasureInfo.vcAlign.emplace_back(pMeasure);

			//YJD START 2021.11.29 - ALIGN EXPAND CAM
			if (m_pSystemSpec->sysNSIS.sysUseExCam)
			{
				MeasureItem_t* pMeasureEx = new MeasureItem_t;
				*pMeasureEx = it;

				pMeasureEx->stSpec.dRange = 0.;
				pMeasureEx->bIs3D = FALSE;
				pMeasureEx->iScanMode = 5;
				pMeasureEx->dLens = m_pSystemSpec->sysNSIS.sysAlignExCamLens;
				pMeasureEx->dZoom = m_pSystemSpec->sysNSIS.sysAlignExCamZoom;
				pMeasureEx->bAlignExCam = true;

				m_stPanelInfo.stMeasureInfo.vcAlign.emplace_back(pMeasureEx);
			}
			//YJD END 2021.11.29 - ALIGN EXPAND CAM

		}
		// Panel Out
		else if (bCheckPanel == FALSE)
		{
			continue;
		}
	}
	
	// TP_Feature Check
	CLayer* pTargetLayer_TP = nullptr;
	for (auto it : m_pUserLayerSetInfo->vcLayerSet)
	{
		if (it->strDefine.CompareNoCase(THICKNESS_LAYER_TXT) != 0)
			continue;

		for (UINT32 lyno = 0; lyno < pStep->m_arrLayer.GetSize(); lyno++)
		{
			CLayer* pLayer = pStep->m_arrLayer.GetAt(lyno);
			if (pLayer == nullptr) continue;
			if (it->strLayer.CompareNoCase(pLayer->m_strLayerName) != 0) continue;

			pTargetLayer_TP = pLayer;
			break;
		}
	}

	if (pTargetLayer_TP == nullptr)
		return RESULT_BAD;

	// TP_Feature Size
	int nFeatureCount_TP = static_cast<int>(pTargetLayer_TP->m_FeatureFile.m_arrFeature.GetCount());
	std::vector<MeasureItem_t> vcMeasureItems_tp;
	for (int nFeature = 0; nFeature < nFeatureCount_TP; nFeature++)
	{
		CFeature* pFeature = pTargetLayer_TP->m_FeatureFile.m_arrFeature.GetAt(nFeature);
		if (pFeature == nullptr)
			continue;

		CAttributeTextString* pTextString = new CAttributeTextString();

		if (pFeature->m_arrAttributeTextString.GetCount() > 0)
		{
			pTextString = pFeature->m_arrAttributeTextString.GetAt(0);
			if (pTextString == nullptr)
				continue;
		}
		

		RECTD tmpMinMaxRect = pFeature->m_MinMaxRect;
		rotMirrArr.FinalPoint(&tmpMinMaxRect.left, &tmpMinMaxRect.top, tmpMinMaxRect.left, tmpMinMaxRect.top);
		rotMirrArr.FinalPoint(&tmpMinMaxRect.right, &tmpMinMaxRect.bottom, tmpMinMaxRect.right, tmpMinMaxRect.bottom);

		MeasureItem_t stMeasure;
		stMeasure.MinMaxRect_In_Panel = GetDrawRect(tmpMinMaxRect, m_pUserSetInfo);

		tmpMinMaxRect.left = fabs(tmpMinMaxRect.left - m_stPanelInfo.rcRect.left);
		tmpMinMaxRect.right = fabs(tmpMinMaxRect.right - m_stPanelInfo.rcRect.left);
		tmpMinMaxRect.top = fabs(tmpMinMaxRect.top - m_stPanelInfo.rcRect.bottom);
		tmpMinMaxRect.bottom = fabs(tmpMinMaxRect.bottom - m_stPanelInfo.rcRect.bottom);

		if (tmpMinMaxRect.left > tmpMinMaxRect.right)		SWAP(tmpMinMaxRect.left, tmpMinMaxRect.right, dTmpx);
		if (tmpMinMaxRect.bottom > tmpMinMaxRect.top)		SWAP(tmpMinMaxRect.bottom, tmpMinMaxRect.top, dTmpy);

		stMeasure.iFeatureType = static_cast<UINT8>(pFeature->m_eType);
		stMeasure.MinMaxRect = tmpMinMaxRect;


		std::vector<std::string> vcTextString;
		if (pTextString->m_strText == "") 
		{
			stMeasure.iMeasureType = static_cast<UINT8>(MeasureType_SR_SEM::probe);
			stMeasure.strMeasureType.Format(_T("PROBE"));
		}
		else
		{
			vcTextString = SplitString(std::string(CT2CA(pTextString->m_strText)), ",");
			stMeasure.iMeasureType = static_cast<UINT8>(GetMeasureType_SR_SEM((CString)vcTextString.at(0).c_str()));
			stMeasure.strMeasureType.Format(_T("%s"), (CString)vcTextString.at(0).c_str());
		}

		if (stMeasure.iMeasureType == 1)
		{
			stMeasure.iMeasureType = static_cast<UINT8>(MeasureType_SR_SEM::probe);
			stMeasure.strMeasureType.Format(_T("PROBE"));
		}

		stMeasure.strMeasureType.MakeUpper();

		if (vcTextString.size() > 1)
		{
			stMeasure.iMeasureSide = static_cast<UINT8>(GetMeasureSide((CString)vcTextString.at(1).c_str()));

			// ex) t_width, bot, 20, 30, 40 
			//	   --> t_width, bot, min , target, max
			//	   --> t_width, bot, min, max, ooc_min, ooc_max, oor_min, oor_max
			if (vcTextString.size() == 8)
			{
				std::vector<std::string> vcsplit = SplitString(vcTextString.at(2).c_str(), ";");
				stMeasure.stSpec.dMin = _ttof((CString)vcTextString.at(0).c_str());
				stMeasure.stSpec.dMax = _ttof((CString)vcTextString.at(1).c_str());
				stMeasure.stSpec.dMin_OOC = _ttof((CString)vcTextString.at(2).c_str());
				stMeasure.stSpec.dMax_OOC = _ttof((CString)vcTextString.at(3).c_str());
				stMeasure.stSpec.dMin_OOR = _ttof((CString)vcTextString.at(4).c_str());
				stMeasure.stSpec.dMax_OOR = _ttof((CString)vcTextString.at(5).c_str());
			}
			else
			{
				stMeasure.stSpec.dMin = 0.0;
				stMeasure.stSpec.dMax = 0.0;
				stMeasure.stSpec.dMin_OOC = 0.0;
				stMeasure.stSpec.dMax_OOC = 0.0;
				stMeasure.stSpec.dMin_OOR = 0.0;
				stMeasure.stSpec.dMax_OOR = 0.0;
			}
		}
		else
		{
			stMeasure.iMeasureSide = static_cast<UINT8>(m_pUserSetInfo->prodSide);
			stMeasure.stSpec.dMin = 0.0;
			stMeasure.stSpec.dMax = 0.0;
			stMeasure.stSpec.dMin_OOC = 0.0;
			stMeasure.stSpec.dMax_OOC = 0.0;
			stMeasure.stSpec.dMin_OOR = 0.0;
			stMeasure.stSpec.dMax_OOR = 0.0;
		}
			   
		RECTD rcStrip;
		RECTD rcUnit;
		BOOL bFind = FALSE;
		BOOL bCheckUnit = FALSE;
		for (auto itStrip : m_stPanelInfo.vcStripInfo)
		{
			for (auto itUnit : itStrip->vcUnitInfo)
			{
				rcStrip = itStrip->rcRect;
				rcUnit = itUnit->rcRect;

				// Unit 인 경우 Check
				if (rcUnit.IsPtInRectD_R(tmpMinMaxRect.left, tmpMinMaxRect.top) &&
					rcUnit.IsPtInRectD_R(tmpMinMaxRect.right, tmpMinMaxRect.bottom))
				{
					stMeasure.iUnitIndex = itUnit->iUnitIdx_Ori;
					stMeasure.iStripIndex = itUnit->iStripIdx;
					stMeasure.stStepRpt = itUnit->stStepRpt;

					itUnit->bInspect = TRUE;
					bFind = TRUE;
					break;
				}
			}
			if (bFind == TRUE)
			{
				break;
			}
		}

		// Unit In 이 아닌 Strip In 경우를 위해 다시 For문
		if (bFind == FALSE)
		{
			for (auto itStrip : m_stPanelInfo.vcStripInfo)
			{
				for (auto itUnit : itStrip->vcUnitInfo)
				{
					rcStrip = itStrip->rcRect;
					rcUnit = itUnit->rcRect;

					// Strip Dummy 인 경우 Check
					if ((rcStrip.IsPtInRectD_R(tmpMinMaxRect.left, tmpMinMaxRect.top) == TRUE &&
						rcStrip.IsPtInRectD_R(tmpMinMaxRect.right, tmpMinMaxRect.bottom) == TRUE) &&
						(rcUnit.IsPtInRectD_R(tmpMinMaxRect.left, tmpMinMaxRect.top) == FALSE &&
							rcUnit.IsPtInRectD_R(tmpMinMaxRect.right, tmpMinMaxRect.bottom) == FALSE))
					{
						stMeasure.iUnitIndex = itUnit->iUnitIdx_Ori;
						stMeasure.iStripIndex = itStrip->iStripIdx;
						stMeasure.stStepRpt = itStrip->stStepRpt;

						itStrip->bInspect = TRUE;
						bFind = TRUE;
						break;
					}

				}
				if (bFind == TRUE )
				{
					break;
				}
			}
		}

		// Check Position 
		bool bCheckPanel = FALSE;
		int nPanelStepIndex = m_pUserLayerSetInfo->nPanelStepIdx;
		// True 면 In , False 면 Out
		//Panel Step Check
		bCheckPanel = CDrawFunction::CheckPanelInOut(m_vecvecRect, &stMeasure, nPanelStepIndex);

		// Panel In, Strip In, Unit In
		if (bCheckPanel == TRUE)
		{
			if ((MeasureType)stMeasure.iMeasureType == MeasureType::align ||
				(MeasureType)stMeasure.iMeasureType == MeasureType::align_plating)
			{
				// Unit Align				
				stMeasure.iUnitIndex = -1;
				stMeasure.pFeature = (void*)pFeature;
				vcMeasureItems_tp.emplace_back(stMeasure);
			}
			else
			{
				stMeasure.pFeature = (void*)pFeature;
				vcMeasureItems_tp.emplace_back(stMeasure);
			}

		}

		// Panel Out
		else if (bCheckPanel == FALSE)
		{
			continue;
		}

	}


	rotMirrArr.ClearAll();
	   	
	/*if (vcMeasureItems.size() <= 0)
		return RESULT_GOOD;*/
		

	// TP 밀어넣기
	std::vector<INT32> vcMeasureUnits_tp;
	std::vector<INT32> vcMeasureStrips_tp;
	for (auto it : vcMeasureItems_tp)
	{
		// Check Position 
		bool bCheckPanel = FALSE;
		bool bCheckStrip = FALSE;
		bool bCheckUnit = FALSE;
		int stripidx, unitidx = -1;
		int nStripStep;
		int nUnitStep;

		//
		int nPanelStepIndex = m_pUserLayerSetInfo->nPanelStepIdx;
		vector<int> nStripStepIndex = m_pUserLayerSetInfo->vcStripStepIdx;
		vector<int> nUnitStepIndex = m_pUserLayerSetInfo->vcUnitStepIdx;

		// True 면 In , False 면 Out
		//Panel Step Check
		bCheckPanel = CDrawFunction::CheckPanelInOut(m_vecvecRect, &it, nPanelStepIndex);
		//Strip Step Check
		for (nStripStep = 0; nStripStep < static_cast<int>(nStripStepIndex.size()); nStripStep++)
		{
			bCheckStrip = CDrawFunction::CheckStripInOut(m_vecvecRect, &it, stripidx, nStripStepIndex[nStripStep]);
			if (bCheckStrip == TRUE)
			{
				break;
			}
		}
		//Unit Step Check
		for (nUnitStep = 0; nUnitStep < static_cast<int>(nUnitStepIndex.size()); nUnitStep++)
		{
			bCheckUnit = CDrawFunction::CheckUnitInOut(m_vecvecRect, &it, unitidx, nUnitStepIndex[nUnitStep]);
			if (bCheckUnit == TRUE)
			{
				break;
			}
		}

		// Panel In , Strip In, Unit In --> Unit SR Point
		if (bCheckPanel == TRUE && bCheckStrip == TRUE && bCheckUnit == TRUE)
		{
			INT32 iUnitIndex = it.iUnitIndex;
			auto itInserted = std::find(vcMeasureUnits_tp.begin(), vcMeasureUnits_tp.end(), iUnitIndex);
 			if (itInserted != vcMeasureUnits_tp.end())
 				continue;

			MeasureUnit_t* pUnit = new MeasureUnit_t;

			pUnit->stStepRpt = it.stStepRpt;
			pUnit->iStripIdx = it.iStripIndex;
			
			// 2022.07.27 KJH TEST
			pUnit->eMapMode = eMinimapMode::Thickness;

			for (auto itUnit : vcMeasureItems_tp)
			{
				if (iUnitIndex != itUnit.iUnitIndex)
					continue;

				//Unit Step Check
				for (nUnitStep = 0; nUnitStep < static_cast<int>(nUnitStepIndex.size()); nUnitStep++)
				{
					bCheckUnit = CDrawFunction::CheckUnitInOut(m_vecvecRect, &itUnit, unitidx, nUnitStepIndex[nUnitStep]);
					if (bCheckUnit == TRUE)
					{
						break;
					}
				}
				if (bCheckUnit == FALSE)
					continue;

				MeasureItem_t* pMeasure = new MeasureItem_t;
				*pMeasure = itUnit;

				pUnit->iIndex = iUnitIndex;

				if ((MeasureType_SEM)it.iMeasureType != MeasureType_SEM::MeasureTypeNone)	// Unit SR Measure Points
				{
					pMeasure->bIs3D = TRUE;
					pMeasure->iScanMode = 1;

					// 2022.10.04 KJH ADD
					// from SNU Code 로 변경
					//CheckLensZoom(false, pMeasure);

					width = abs(pMeasure->MinMaxRect.right - pMeasure->MinMaxRect.left);
					height = abs(pMeasure->MinMaxRect.top - pMeasure->MinMaxRect.bottom);

					// mm -> um 단위 변환
					width *= 1000.;
					height *= 1000.;
					if (pMeasure->stSpec.dRange == 0)
					{
						width += 100.;
						height += 100.;
					}
					else
					{
						width += pMeasure->stSpec.dRange;
						height += pMeasure->stSpec.dRange;
					}

					// Pattern 및 여유공간 2배 이상 FOV 선정

					//YJD START 2022.04.08 - MEASURE TYPE SCALE					
					if ((MeasureType)itUnit.iMeasureType < MeasureType::MeasureTypeNone)
					{
						width *= m_pSystemSpec->sysNSIS.sysMeasureTypeScale[itUnit.iMeasureType];
						height *= m_pSystemSpec->sysNSIS.sysMeasureTypeScale[itUnit.iMeasureType];
					}
					else
					{
						width *= m_pSystemSpec->sysNSIS.sysMeasureDefaultScale;
						height *= m_pSystemSpec->sysNSIS.sysMeasureDefaultScale;
					}
					//YJD END 2022.04.08 - MEASURE TYPE SCALE

					CheckLensZoom_SNU(width, height, pMeasure);


					pUnit->vcPoints_SR.emplace_back(pMeasure);
				}

				for (auto itStripInfo : m_stPanelInfo.vcStripInfo)
				{
					bool bFind = false;
					RECTD rcUnit;
					for (auto itUnitInfo : itStripInfo->vcUnitInfo)
					{
						if (m_pSystemSpec->sysBasic.bProfileMode)
						{
							//rcUnit = itUnitInfo->rcProfile;
							rcUnit = itUnitInfo->rcRect;
						}
						else
						{
							rcUnit = itUnitInfo->rcRect;
						}

						if (rcUnit.IsPtInRectD_R(itUnit.MinMaxRect.left, itUnit.MinMaxRect.top) &&
							rcUnit.IsPtInRectD_R(itUnit.MinMaxRect.right, itUnit.MinMaxRect.bottom))						
						{
							pUnit->iType = itUnitInfo->iType;
							pUnit->UnitRect = rcUnit;
							pUnit->UnitRect_In_Panel = itUnitInfo->rcRect_In_Panel;
							bFind = true;
							break;
						}
					}
					if (bFind)
						break;
				}
			}
			vcMeasureUnits_tp.emplace_back(iUnitIndex);
			m_stPanelInfo.stMeasureInfo.vcUnits.emplace_back(pUnit);

		}
				
		// Panel In, Strip In, Unit Out --> Strip Dummy
		// Strip Dummy 는 For 문 돌릴 필요 없음.
		else if (bCheckPanel == TRUE && bCheckStrip == TRUE && bCheckUnit == FALSE)
		{	
			INT32 iStripIndex = it.iStripIndex;
			auto itInserted = std::find(vcMeasureStrips_tp.begin(), vcMeasureStrips_tp.end(), iStripIndex);
			if (itInserted != vcMeasureStrips_tp.end())
				continue;

			MeasureUnit_t* pStrip = new MeasureUnit_t;

			for (auto itStrip : vcMeasureItems_tp)
			{
				if (iStripIndex != itStrip.iStripIndex)
					continue;

				//Strip Step Check
				for (nStripStep = 0; nStripStep < static_cast<int>(nStripStepIndex.size()); nStripStep++)
				{
					bCheckStrip = CDrawFunction::CheckStripInOut(m_vecvecRect, &itStrip, stripidx, nStripStepIndex[nStripStep]);
					if (bCheckStrip == TRUE)
					{
						break;
					}
				}
				//Unit Step Check
				for (nUnitStep = 0; nUnitStep < static_cast<int>(nUnitStepIndex.size()); nUnitStep++)
				{
					bCheckUnit = CDrawFunction::CheckUnitInOut(m_vecvecRect, &itStrip, unitidx, nUnitStepIndex[nUnitStep]);
					if (bCheckUnit == TRUE)
					{
						break;
					}
				}
				if (bCheckStrip == TRUE && bCheckUnit == TRUE)
					continue;

				// Strip Dummy			
				pStrip->stStepRpt = itStrip.stStepRpt;
				pStrip->iStripIdx = itStrip.iStripIndex;
				
				// 2022.07.27 KJH TEST
				pStrip->eMapMode = eMinimapMode::Thickness;

				MeasureItem_t* pMeasure = new MeasureItem_t;
				*pMeasure = itStrip;
				pMeasure->iUnitIndex = -1;
				
				pMeasure->iMeasureType = static_cast<UINT32>(MeasureType_SR_SEM::probe);
				pMeasure->bIs3D = TRUE;
				pMeasure->iScanMode = 1;
							   
				// 2022.10.04 KJH ADD
				// from SNU Code 로 변경
				//CheckLensZoom(false, pMeasure);

				width = abs(pMeasure->MinMaxRect.right - pMeasure->MinMaxRect.left);
				height = abs(pMeasure->MinMaxRect.top - pMeasure->MinMaxRect.bottom);

				// mm -> um 단위 변환
				width *= 1000.;
				height *= 1000.;
				if (pMeasure->stSpec.dRange == 0)
				{
					width += 100.;
					height += 100.;
				}
				else
				{
					width += pMeasure->stSpec.dRange;
					height += pMeasure->stSpec.dRange;
				}

				width *= m_pSystemSpec->sysNSIS.sysMeasureDefaultScale;
				height *= m_pSystemSpec->sysNSIS.sysMeasureDefaultScale;
				CheckLensZoom_SNU(width, height, pMeasure);
							   				 
				pStrip->vcPoints_SR.emplace_back(pMeasure);				

				for (auto itStripInfo : m_stPanelInfo.vcStripInfo)
				{
					RECTD rcStrip;
					rcStrip = itStripInfo->rcRect;
					bool bFind = false;
					RECTD rcUnit;
					if (m_pSystemSpec->sysBasic.bProfileMode)
					{
						rcStrip = itStripInfo->rcProfile;
					}
					else
					{
						rcStrip = itStripInfo->rcRect;
					}

					if (rcStrip.IsPtInRectD_R(it.MinMaxRect.left, it.MinMaxRect.top) &&
						rcStrip.IsPtInRectD_R(it.MinMaxRect.right, it.MinMaxRect.bottom))
					{
						pStrip->iType = itStripInfo->iType;						
						pStrip->UnitRect = rcStrip;
						pStrip->UnitRect_In_Panel = itStripInfo->rcRect_In_Panel;
						bFind = true;
						break;
					}
				}
			}
			vcMeasureStrips_tp.emplace_back(iStripIndex);
			m_stPanelInfo.stMeasureInfo.vcStripDummy.emplace_back(pStrip);


	
		}		
		
	}

	// 측정 Unit 정렬
	if (RESULT_GOOD != SortMeasureUnit())
		return RESULT_BAD;

	vcMeasureUnits.clear();
	vcMeasureItems.clear();
	vcMeasureUnits_tp.clear();
	vcMeasureItems_tp.clear();

	////Panel Align 정보가 없을 경우
	//if (m_stPanelInfo.stMeasureInfo.vcAlign.size() <= 0)
	//	return RESULT_BAD;

	////측정 Unit 정보가 없을 경우
	//if (m_stPanelInfo.stMeasureInfo.vcUnits.size() <= 0)
	//	return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CNSIS::CheckLensZoom(bool bIsAlign, MeasureItem_t* pItem)
{
	if (pItem == nullptr)
		return RESULT_BAD;
	
	std::vector<NSIS_CameraSpec_t*>* pSysCamera = nullptr;
	if (pItem->bIs3D)
		pSysCamera = &m_pSystemSpec->sysNSIS.sys3DCamera;
	else
		pSysCamera = &m_pSystemSpec->sysNSIS.sys2DCamera;

	if (pSysCamera == nullptr) return RESULT_BAD;

	double dWidth = fabs(pItem->MinMaxRect.right - pItem->MinMaxRect.left) * 1000.0;
	double dHeight = fabs(pItem->MinMaxRect.top - pItem->MinMaxRect.bottom) * 1000.0;
	if (dWidth <= 0 || dHeight <= 0)
		return RESULT_BAD;

	if (bIsAlign)
	{
		dWidth *= 4.0;
		dHeight *= 4.0;
	}
	else
	{
		if (pItem->stSpec.dRange == 0)
		{
			dWidth += 100.;
			dHeight += 100.;
		}
		else
		{
			dWidth += pItem->stSpec.dRange;
			dHeight += pItem->stSpec.dRange;
		}

		// Pattern 및 여유공간 2배 이상 FOV 선정
		dWidth *= 2.0;
		dHeight *= 2.0;
	}
	
	int iSel = -1;
	for (int i = 0; i < pSysCamera->size(); i++)
	{
		NSIS_CameraSpec_t *pCameraSpec = pSysCamera->at(i);
		if (pCameraSpec == nullptr)
			continue;
		
		double diffX = pCameraSpec->iFovWidth - (dWidth);
		double diffY = pCameraSpec->iFovHeight - (dHeight);
		if (diffX < 0 || diffY < 0) continue;

		if (dWidth > dHeight)
		{
			if (diffX > 0)
			{
				iSel = i;
				break;
			}
		}
		else
		{
			if (diffY > 0)
			{
				iSel = i;
				break;
			}
		}
	}

	if (iSel < 0 || iSel >= pSysCamera->size())
		return RESULT_BAD;

	NSIS_CameraSpec_t *pCameraSpec = pSysCamera->at(iSel);
	if (pCameraSpec == nullptr)
		return RESULT_BAD;

	pItem->dLens = pCameraSpec->dMagnification;
	pItem->dZoom = pCameraSpec->dZoom;

	return RESULT_GOOD;
}

UINT32 CNSIS::CheckLensZoom_SNU(double dWidth, double dHeight, MeasureItem_t* tempItem)
{
	if (tempItem == nullptr)
		return RESULT_BAD;

	std::vector<NSIS_CameraSpec_t*>* pSysCamera = nullptr;
	if (tempItem->bIs3D)
		pSysCamera = &m_pSystemSpec->sysNSIS.sys3DCamera;
	else
		pSysCamera = &m_pSystemSpec->sysNSIS.sys2DCamera;

	if (pSysCamera == nullptr) 
		return RESULT_BAD;

	vector<PointDXY> vcDiff;
	NSIS_CameraSpec_t* pCameraSpec = nullptr;
	for (int i = 0; i < pSysCamera->size(); i++)
	{
		PointDXY pt(CPointD(-1.f, -1.f));

		pCameraSpec = pSysCamera->at(i);
		if (pCameraSpec == nullptr)
		{
			vcDiff.emplace_back(pt);
			continue;
		}

		pt.x = pCameraSpec->iFovWidth - (dWidth);
		pt.y = pCameraSpec->iFovHeight - (dHeight);
		vcDiff.emplace_back(pt);
	}

	int iIndex = 0;
	bool bSearch = false;
	for (int i = 0; i < pSysCamera->size(); i++)
	{
		if (vcDiff[i].x < 0) continue;
		if (vcDiff[i].y < 0) continue;

		if (dWidth > dHeight)
		{
			if (vcDiff[i].x > 0)
			{
				iIndex = i;
				bSearch = true;
				break;
			}
		}
		else
		{
			if (vcDiff[i].y > 0)
			{
				iIndex = i;
				bSearch = true;
				break;
			}
		}
	}

	if (bSearch)
		pCameraSpec = pSysCamera->at(iIndex);
	else
		pCameraSpec = pSysCamera->at(pSysCamera->size() - 1);


	tempItem->dLens = pCameraSpec->dMagnification;
	tempItem->dZoom = pCameraSpec->dZoom;

	return RESULT_GOOD;
}


std::vector<std::string> CNSIS::SplitString(std::string s, std::string divid)
{
	std::vector<std::string> v;
	int start = 0;
	int d = static_cast<int>(s.find(divid));

	while (d != -1)
	{
		v.push_back(s.substr(start, d - start));
		start = d + 1;
		d = static_cast<int>(s.find(divid, start));
	}
	v.push_back(s.substr(start, d - start));

	return v;
}

MeasureType CNSIS::GetMeasureType(CString strType)
{
	MeasureType eType = MeasureType::MeasureTypeNone;

	if (strType.CompareNoCase(PANEL_ALIGN_TXT) == 0)
	{
		eType = MeasureType::align;
	}
	else if (strType.CompareNoCase(PANEL_ALIGN_PLATING_TXT) == 0)
	{
		eType = MeasureType::align_plating;
	}
	else if (strType.CompareNoCase(UNIT_ALIGN_TXT_P) == 0)
	{
		eType = MeasureType::fm_p;
	}
	else  // MEASURE_POINTs
	{
		if (strType.CompareNoCase(_T("T_WIDTH")) == 0)			eType = MeasureType::t_width;
		else if (strType.CompareNoCase(_T("T_SPACE")) == 0)			eType = MeasureType::t_space;
		else if (strType.CompareNoCase(_T("BF_WIDTH")) == 0)		eType = MeasureType::bf_width;
		else if (strType.CompareNoCase(_T("BF_SPACE")) == 0)		eType = MeasureType::bf_space;
		else if (strType.CompareNoCase(_T("BOL_WIDTH")) == 0)		eType = MeasureType::bol_width;
		else if (strType.CompareNoCase(_T("BOL_SPACE")) == 0)		eType = MeasureType::bol_space;
		else if (strType.CompareNoCase(_T("THICKNESS")) == 0)		eType = MeasureType::thickness;
		else if (strType.CompareNoCase(_T("DIMPLE")) == 0)			eType = MeasureType::dimple;
		else if (strType.CompareNoCase(_T("BALL_WIDTH")) == 0)		eType = MeasureType::ball_width;
		else if (strType.CompareNoCase(_T("BUMP_WIDTH")) == 0)		eType = MeasureType::bump_width;
		else if (strType.CompareNoCase(_T("IMP_SPACE_A")) == 0)		eType = MeasureType::imp_space_A;
		else if (strType.CompareNoCase(_T("IMP_SPACE_B")) == 0)		eType = MeasureType::imp_space_B;
		else if (strType.CompareNoCase(_T("IMP_WIDTH_A")) == 0)		eType = MeasureType::imp_width_A;
		else if (strType.CompareNoCase(_T("IMP_WIDTH_B")) == 0)		eType = MeasureType::imp_width_B;
		else if (strType.CompareNoCase(_T("BOL_WIDTH_X")) == 0)		eType = MeasureType::bol_width_x;
		else if (strType.CompareNoCase(_T("BOL_WIDTH_Y")) == 0)		eType = MeasureType::bol_width_y;
		else if (strType.CompareNoCase(_T("BOL_SPACE_X")) == 0)		eType = MeasureType::bol_space_x;
		else if (strType.CompareNoCase(_T("BOL_SPACE_Y")) == 0)		eType = MeasureType::bol_space_y;
		else if (strType.CompareNoCase(_T("BOL_PITCH")) == 0)		eType = MeasureType::bol_pitch;
		else if (strType.CompareNoCase(_T("VIA_WIDTH")) == 0)		eType = MeasureType::via_width;
		else if (strType.CompareNoCase(_T("R_DEPTH")) == 0)			eType = MeasureType::r_depth;
		else if (strType.CompareNoCase(_T("PAD")) == 0)				eType = MeasureType::pad;
		else if (strType.CompareNoCase(_T("BALL_PITCH")) == 0)		eType = MeasureType::ball_pitch;
		else if (strType.CompareNoCase(_T("THICKNESS_CU")) == 0)	eType = MeasureType::thickness_cu;
		else if (strType.CompareNoCase(_T("THICKNESS_PAD")) == 0)	eType = MeasureType::thickness_pad;
		else
			eType = MeasureType::MeasureTypeNone;
	}
	return eType;
}

CString CNSIS::GetMeasureTypeString(MeasureType type)
{
	if (type == MeasureType::align)
		return PANEL_ALIGN_TXT;
	else if (type == MeasureType::align_plating)
		return PANEL_ALIGN_PLATING_TXT;
	else if (type == MeasureType::fm_p)
		return UNIT_ALIGN_TXT_P;
	else
	{
		if (type == MeasureType::t_width)				return _T("T_WIDTH");
		else if (type == MeasureType::t_space)			return _T("T_SPACE");
		else if (type == MeasureType::bf_width)			return _T("BF_WIDTH");
		else if (type == MeasureType::bf_space)			return _T("BF_SPACE");
		else if (type == MeasureType::bol_width)		return _T("BOL_WIDTH");
		else if (type == MeasureType::bol_space)		return _T("BOL_SPACE");
		else if (type == MeasureType::thickness)		return _T("THICKNESS");
		else if (type == MeasureType::dimple)			return _T("DIMPLE");
		else if (type == MeasureType::ball_width)		return _T("BALL_WIDTH");
		else if (type == MeasureType::bump_width)		return _T("BUMP_WIDTH");
		else if (type == MeasureType::imp_space_A)		return _T("IMP_SPACE_A");
		else if (type == MeasureType::imp_space_B)		return _T("IMP_SPACE_B");
		else if (type == MeasureType::imp_width_A)		return _T("IMP_WIDTH_A");
		else if (type == MeasureType::imp_width_B)		return _T("IMP_WIDTH_B");
		else if (type == MeasureType::bol_width_x)		return _T("BOL_WIDTH_X");
		else if (type == MeasureType::bol_width_y)		return _T("BOL_WIDTH_Y");
		else if (type == MeasureType::bol_space_x)		return _T("BOL_SPACE_X");
		else if (type == MeasureType::bol_space_y)		return _T("BOL_SPACE_Y");
		else if (type == MeasureType::bol_pitch)		return _T("BOL_PITCH");
		else if (type == MeasureType::via_width)		return _T("VIA_WIDTH");
		else if (type == MeasureType::r_depth)			return _T("R_DEPTH");
		else if (type == MeasureType::pad)				return _T("PAD");
		else if (type == MeasureType::ball_pitch)		return _T("BALL_PITCH");
		else if (type == MeasureType::thickness_cu)		return _T("THICKNESS_CU");
		else if (type == MeasureType::thickness_pad)	return _T("THICKNESS_PAD");
		else
			return _T("UKNOWN");
	}
}



MeasureType_SEM CNSIS::GetMeasureType_SEM(CString strType)
{
	MeasureType_SEM eType = MeasureType_SEM::MeasureTypeNone;

	if (strType.CompareNoCase(PANEL_ALIGN_TXT) == 0)
	{
		eType = MeasureType_SEM::align;
	}
	else if (strType.CompareNoCase(PANEL_ALIGN_PLATING_TXT) == 0)
	{
		eType = MeasureType_SEM::align_plating;
	}
	else if (strType.CompareNoCase(UNIT_ALIGN_TXT_P) == 0)
	{
		eType = MeasureType_SEM::fm_p;
	}
	else  // MEASURE_POINTs
	{
		if (strType.CompareNoCase(_T("T_WIDTH")) == 0)			eType = MeasureType_SEM::t_width;
		else if (strType.CompareNoCase(_T("T_SPACE")) == 0)			eType = MeasureType_SEM::t_space;
		else if (strType.CompareNoCase(_T("BF_WIDTH")) == 0)		eType = MeasureType_SEM::bf_width;
		else if (strType.CompareNoCase(_T("BF_SPACE")) == 0)		eType = MeasureType_SEM::bf_space;
		else if (strType.CompareNoCase(_T("BOL_WIDTH")) == 0)		eType = MeasureType_SEM::bol_width;
		else if (strType.CompareNoCase(_T("BOL_SPACE")) == 0)		eType = MeasureType_SEM::bol_space;
		else if (strType.CompareNoCase(_T("THICKNESS")) == 0)		eType = MeasureType_SEM::thickness;
		else if (strType.CompareNoCase(_T("DIMPLE")) == 0)			eType = MeasureType_SEM::dimple;
		else if (strType.CompareNoCase(_T("BALL_WIDTH")) == 0)		eType = MeasureType_SEM::ball_width;
		else if (strType.CompareNoCase(_T("BUMP_WIDTH")) == 0)		eType = MeasureType_SEM::bump_width;
		else if (strType.CompareNoCase(_T("IMP_SPACE_A")) == 0)		eType = MeasureType_SEM::imp_space_A;
		else if (strType.CompareNoCase(_T("IMP_SPACE_B")) == 0)		eType = MeasureType_SEM::imp_space_B;
		else if (strType.CompareNoCase(_T("IMP_WIDTH_A")) == 0)		eType = MeasureType_SEM::imp_width_A;
		else if (strType.CompareNoCase(_T("IMP_WIDTH_B")) == 0)		eType = MeasureType_SEM::imp_width_B;
		else if (strType.CompareNoCase(_T("BOL_WIDTH_X")) == 0)		eType = MeasureType_SEM::bol_width_x;
		else if (strType.CompareNoCase(_T("BOL_WIDTH_Y")) == 0)		eType = MeasureType_SEM::bol_width_y;
		else if (strType.CompareNoCase(_T("BOL_SPACE_X")) == 0)		eType = MeasureType_SEM::bol_space_x;
		else if (strType.CompareNoCase(_T("BOL_SPACE_Y")) == 0)		eType = MeasureType_SEM::bol_space_y;
		else if (strType.CompareNoCase(_T("BOL_PITCH")) == 0)		eType = MeasureType_SEM::bol_pitch;
		else if (strType.CompareNoCase(_T("VIA_WIDTH")) == 0)		eType = MeasureType_SEM::via_width;
		else if (strType.CompareNoCase(_T("R_DEPTH")) == 0)			eType = MeasureType_SEM::r_depth;
		else if (strType.CompareNoCase(_T("PAD")) == 0)				eType = MeasureType_SEM::pad;
		else if (strType.CompareNoCase(_T("BALL_PITCH")) == 0)		eType = MeasureType_SEM::ball_pitch;
		else if (strType.CompareNoCase(_T("THICKNESS_CU")) == 0)	eType = MeasureType_SEM::thickness_cu;
		else if (strType.CompareNoCase(_T("THICKNESS_PAD")) == 0)	eType = MeasureType_SEM::thickness_pad;
		else if (strType.CompareNoCase(_T("ROUGHNESS")) == 0)		eType = MeasureType_SEM::roughness;
		else
			eType = MeasureType_SEM::MeasureTypeNone;
	}
	return eType;
}

CString CNSIS::GetMeasureTypeString_SEM(MeasureType_SEM type)
{
	if (type == MeasureType_SEM::align)
		return PANEL_ALIGN_TXT;
	else if (type == MeasureType_SEM::align_plating)
		return PANEL_ALIGN_PLATING_TXT;
	else if (type == MeasureType_SEM::fm_p)
		return UNIT_ALIGN_TXT_P;
	else
	{
		if (type == MeasureType_SEM::t_width)				return _T("T_WIDTH");
		else if (type == MeasureType_SEM::t_space)			return _T("T_SPACE");
		else if (type == MeasureType_SEM::bf_width)			return _T("BF_WIDTH");
		else if (type == MeasureType_SEM::bf_space)			return _T("BF_SPACE");
		else if (type == MeasureType_SEM::bol_width)		return _T("BOL_WIDTH");
		else if (type == MeasureType_SEM::bol_space)		return _T("BOL_SPACE");
		else if (type == MeasureType_SEM::thickness)		return _T("THICKNESS");
		else if (type == MeasureType_SEM::dimple)			return _T("DIMPLE");
		else if (type == MeasureType_SEM::ball_width)		return _T("BALL_WIDTH");
		else if (type == MeasureType_SEM::bump_width)		return _T("BUMP_WIDTH");
		else if (type == MeasureType_SEM::imp_space_A)		return _T("IMP_SPACE_A");
		else if (type == MeasureType_SEM::imp_space_B)		return _T("IMP_SPACE_B");
		else if (type == MeasureType_SEM::imp_width_A)		return _T("IMP_WIDTH_A");
		else if (type == MeasureType_SEM::imp_width_B)		return _T("IMP_WIDTH_B");
		else if (type == MeasureType_SEM::bol_width_x)		return _T("BOL_WIDTH_X");
		else if (type == MeasureType_SEM::bol_width_y)		return _T("BOL_WIDTH_Y");
		else if (type == MeasureType_SEM::bol_space_x)		return _T("BOL_SPACE_X");
		else if (type == MeasureType_SEM::bol_space_y)		return _T("BOL_SPACE_Y");
		else if (type == MeasureType_SEM::bol_pitch)		return _T("BOL_PITCH");
		else if (type == MeasureType_SEM::via_width)		return _T("VIA_WIDTH");
		else if (type == MeasureType_SEM::r_depth)			return _T("R_DEPTH");
		else if (type == MeasureType_SEM::pad)				return _T("PAD");
		else if (type == MeasureType_SEM::ball_pitch)		return _T("BALL_PITCH");
		else if (type == MeasureType_SEM::thickness_cu)		return _T("THICKNESS_CU");
		else if (type == MeasureType_SEM::thickness_pad)	return _T("THICKNESS_PAD");
		else if (type == MeasureType_SEM::roughness)		return _T("ROUGHNESS");
		else
			return _T("UKNOWN");
	}
}


MeasureType_SR_SEM CNSIS::GetMeasureType_SR_SEM(CString strType)
{
	MeasureType_SR_SEM eType = MeasureType_SR_SEM::MeasureTypeNone;
	if (strType.CompareNoCase(_T("PROBE")) == 0)			eType = MeasureType_SR_SEM::probe;
	else if (strType.CompareNoCase(_T("")) == 0)			eType = MeasureType_SR_SEM::MeasureTypeNone;
	
	return eType;
}

CString CNSIS::GetMeasureTypeString_SR_SEM(MeasureType_SR_SEM type)
{	
	if (type == MeasureType_SR_SEM::probe)					return _T("PROBE");
	else if (type == MeasureType_SR_SEM::MeasureTypeNone)	return _T("NONE");	
	else return _T("NONE");
}

MeasureSide CNSIS::GetMeasureSide(CString strType)
{
	MeasureSide eSide = MeasureSide::MeasureSideNone;

	if (strType.CompareNoCase(_T("TOP")) == 0)
	{
		eSide = MeasureSide::top;
	}
	else if (strType.CompareNoCase(_T("BOT")) == 0)
	{
		eSide = MeasureSide::bot;
	}

	return eSide;
}

CString CNSIS::GetMeasureSideString(MeasureSide side)
{
	if (side == MeasureSide::top)
		return _T("TOP");
	else
		return _T("BOT");
}

CString CNSIS::GetMagnificationString(BOOL bIs3D, INT8 iScanMode)
{
	CString strMag = _T("");

	if (bIs3D)
	{
		if (iScanMode >= MAX_3D_MODE_NUM || iScanMode < 0)
			return strMag;

		switch (iScanMode)
		{
		case 0: case 1: case 2: strMag = NSIS_MAG_MODE_1; break;
		case 3: case 4: case 5: strMag = NSIS_MAG_MODE_2; break;
		case 6: case 7: case 8: strMag = NSIS_MAG_MODE_3; break;
		}
	}
	else
	{
		if (iScanMode >= MAX_2D_MODE_NUM || iScanMode < 0)
			return strMag;

		switch (iScanMode)
		{
		case 0: case 1: case 2: strMag = NSIS_MAG_MODE_4; break;
		case 3: case 4: case 5: strMag = NSIS_MAG_MODE_5; break;
		}
	}

	return strMag;
}

CString CNSIS::GetMagnificationString(double dLens)
{
	CString strMag;
	strMag.Format(_T("x%.1f"), dLens);

	return strMag;
}

CString CNSIS::GetZoomString(BOOL bIs3D, INT8 iScanMode)
{
	CString strZoom = _T("");

	if (bIs3D)
	{
		if (iScanMode >= MAX_3D_MODE_NUM || iScanMode < 0)
			return strZoom;

		switch (iScanMode)
		{
		case 0: case 3: case 6: strZoom = NSIS_ZOOM_MODE_1; break;
		case 1: case 4: case 7: strZoom = NSIS_ZOOM_MODE_2; break;
		case 2: case 5: case 8: strZoom = NSIS_ZOOM_MODE_3; break;
		}
	}
	else
	{
		if (iScanMode >= MAX_2D_MODE_NUM || iScanMode < 0)
			return strZoom;

		switch (iScanMode)
		{
		case 0: case 3: strZoom = NSIS_ZOOM_MODE_1; break;
		case 1: case 4: strZoom = NSIS_ZOOM_MODE_2; break;
		case 2: case 5: strZoom = NSIS_ZOOM_MODE_3; break;
		}
	}

	return strZoom;
}

CString CNSIS::GetZoomString(double dZoom)
{
	CString strMag;
	strMag.Format(_T("x%.2f"), dZoom);

	return strMag;
}

UINT32 CNSIS::SortMeasureUnit()
{
	std::vector<MeasureUnit_t*> *vcUnits = &m_stPanelInfo.stMeasureInfo.vcUnits;
	if (vcUnits == nullptr) return RESULT_BAD;

	// Y 축 수량 확인
	INT32 iYCount = CheckYCount();
	if (iYCount < 0) return RESULT_BAD;

	std::vector<MeasureUnit_t*> *vcSortUnit = nullptr;
	vcSortUnit = new std::vector<MeasureUnit_t*>[vcUnits->size()];// [iYCount];
	if (vcSortUnit == nullptr) return RESULT_BAD;

	BOOL bSortDirection = TRUE;
	/*if (m_pUserSetInfo->prodSide == eProdSide::eTop)
		bSortDirection = TRUE;
	else
		bSortDirection = FALSE;*/

	// Y 축 같은 Unit 끼리 묶음
	int iCount = 0;
	MeasureUnit_t* pUnit = nullptr;
	for (auto Unit : *vcUnits)
	{
		BOOL bFind = FALSE;
		for (int i = 0; i < iCount; i++)
		{
			for (auto SortUnit : vcSortUnit[i])
			{
				if (SortUnit->UnitRect.top == Unit->UnitRect.top)
				{
					bFind = TRUE;
					break;
				}
			}

			if (bFind)
			{
				pUnit = new MeasureUnit_t(*Unit);
				vcSortUnit[i].emplace_back(pUnit);
				break;
			}
		}

		if (!bFind)
		{
			pUnit = new MeasureUnit_t(*Unit);
			vcSortUnit[iCount].emplace_back(pUnit);
			iCount++;
		}
	}

	if (iYCount != iCount)
		iYCount = iCount;

	// Y 정렬
	sort(vcSortUnit, vcSortUnit + iYCount, SortDescendingY);

	// 각 X 정렬
	for (int i = 0; i < iYCount; i++)
	{
		if (bSortDirection)
			sort(vcSortUnit[i].begin(), vcSortUnit[i].end(), SortDescendingX);			
		else
			sort(vcSortUnit[i].begin(), vcSortUnit[i].end(), SortAscendingX);

		bSortDirection = !bSortDirection;
	}

	// 기존 저장된 vector clear
	for (auto Unit : *vcUnits)
	{
		Unit->Clear();
		delete Unit;
		Unit = nullptr;
	}
	vcUnits->clear();

	// 정렬된 Unit 기입
	for (int i = 0; i < iYCount; i++)
	{
		for (auto SortUnit : vcSortUnit[i])
		{
			vcUnits->emplace_back(SortUnit);
		}
	}

	return RESULT_GOOD;
}

INT32 CNSIS::CheckYCount()
{	
	std::vector<MeasureUnit_t*> *vcUnits = &m_stPanelInfo.stMeasureInfo.vcUnits;
	if (vcUnits == nullptr)
		return -1;

	std::vector<INT32> vcYCount;
	for (auto Unit : *vcUnits)
	{
		INT32 iTop = static_cast<INT32>(Unit->UnitRect.top + 0.5);

		BOOL bFind = FALSE;
		for (auto Y : vcYCount)
		{
			if (Y == iTop)
			{
				bFind = TRUE;
				break;
			}
		}

		if (!bFind)
			vcYCount.emplace_back(iTop);
	}

	INT32 iResult = static_cast<INT32>(vcYCount.size());
	vcYCount.clear();

	return iResult;
}

UINT32 CNSIS::MakeMasterData()
{
	//CPerfTimerEx clTimer;
	//CString strLog;

	//clTimer.StartTimer();

	if (m_pJobData == nullptr || m_pUserLayerSetInfo == nullptr ||
		m_pUserSetInfo == nullptr || m_pSystemSpec == nullptr)
		return RESULT_BAD;

	if (m_pJobData->GetModelName().GetLength() < 1)
		return RESULT_BAD;

	m_strModelPath = GetModelPath(true);
	if (m_strModelPath.GetLength() < 1)
		return RESULT_BAD;

	//1. Get Measure Data
	if (RESULT_GOOD != GetMeasureData(nullptr, TRUE))
		return RESULT_BAD;
	
	//2. Panel Image 저장
	if (RESULT_GOOD != MakePanelLayerImage())
		return RESULT_BAD;

	//3. Panel Align Point Image 저장
	if (RESULT_GOOD != MakePanelAlignPointImage())
		return RESULT_BAD;
	
	//4. Unit Image 저장

	// 2022.11.02 KJH ADD 	
	// Unit Layer Image Save Parameter
	if (m_pSystemSpec->sysNSIS.sysUseSaveUnitLayer == true)
	{
		if (RESULT_GOOD != MakeUnitLayerImage())
			return RESULT_BAD;
	}

	//5. Unit Align Point Image 저장
	if (RESULT_GOOD != MakeUnitAlignPointImage_Sem())
		return RESULT_BAD;

	//YJD START 22.06.13 - SAVE MEASURE IMAGE
	//CPerfTimerEx timerEx;
	//CString str;
	//CString strWorkLayer = m_pUserSetInfo->strWorkLayer;
	//double testTime = 0.0;
	//timerEx.StartTimer();
	if (m_pSystemSpec->sysNSIS.sysUseSaveMeasurePoint && RESULT_GOOD != MakeMeasurePointImage())
		return RESULT_BAD;
	//testTime = timerEx.StopTimer(CTimerUnit_en::EN_MIL_SEC);
	//str.Format(_T("\nstrWorkLayer : %s,  Unit Image  저장 : %.2lf\n"), strWorkLayer, testTime);
	//OutputDebugString(str);
	//YJD END 22.06.13 - SAVE MEASURE IMAGE

	if (m_pDrawManager)
	{		
		if (RESULT_GOOD != m_pDrawManager->SetStepIndex(m_nSelectStep))
			return RESULT_BAD;
	}

	//double dTime = clTimer.StopTimer(CTimerUnit_en::EN_MIL_SEC);
	//strLog.Format(_T("\n MakeMasterData : Processing Time[%.2f ms]"), dTime);
	//OutputDebugString(strLog);

	return RESULT_GOOD;
}

UINT32 CNSIS::SaveProductInfo(IN INT32 iStepIdx, IN enum Orient eOrient)
{
	// 2022.05.03 
	// 주석추가 김준호
	// SaveProductInfo daeduck, Sem 분리
	if (m_pUserSetInfo->userSite == eUserSite::eDAEDUCK)
	{		
		if (RESULT_GOOD != SaveProductInfo_Daeduck(iStepIdx, eOrient))
			return RESULT_BAD;	
	}
	else if (m_pUserSetInfo->userSite == eUserSite::eSEM)
	{
		if (RESULT_GOOD != SaveProductInfo_Sem(iStepIdx, eOrient))
			return RESULT_BAD;			
	}

	return RESULT_GOOD;
}

UINT32 CNSIS::SaveProductInfo_Daeduck(IN INT32 iStepIdx, IN enum Orient eOrient)
{
	if (m_pJobData == nullptr || m_pUserLayerSetInfo == nullptr ||
		m_pUserSetInfo == nullptr || m_pSystemSpec == nullptr)
		return RESULT_BAD;

	if (m_pJobData->GetModelName().GetLength() < 1 ||
		m_strModelPath.GetLength() < 1)
		return RESULT_BAD;

	//INT32 iStepIdx = m_pUserSetInfo->iStepIdx;
	if (iStepIdx != m_pUserLayerSetInfo->nPanelStepIdx)
		iStepIdx = m_pUserLayerSetInfo->nPanelStepIdx;

	if (iStepIdx < 0 || m_pSystemSpec->sysPath.strMasterPath.GetLength() < 1)
		return RESULT_BAD;

	CString strWorkStep = m_pJobData->m_arrStep[iStepIdx]->m_strStepName;
	CString strWorkLayer = m_pUserSetInfo->strWorkLayer;
	CString strPairLayer = m_pUserSetInfo->strPairLayer;
	CString Side;

	CString strFileName = L"";
	strFileName.Format(L"%s\\%s", m_strModelPath, PRODUCT_INFO_FILE);

	CFile clResultFile;
	if (!clResultFile.Open(strFileName, CFile::modeCreate | CFile::modeReadWrite))
		return RESULT_BAD;

	CHAR *pcFiledata = nullptr;
	CString strValue = L"", strMag = L"", strZoom = L"";

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
	if (m_pUserSetInfo->prodSide == eProdSide::eBot)
		Side = _T("BOT");
	else
		Side = _T("TOP");
	strValue.Format(_T("<info model=\"%s\" rotation=\"%.1f\" mirror=\"%d\" workstep=\"%s\" worklayer=\"%s\" except_dummy=\"%d\" profile_mode=\"%d\" unit=\"%s\" side=\"%s\" pair=\"%s\"></info>\r\n"),
		m_pJobData->GetModelName(), m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror,
		strWorkStep, strWorkLayer, m_pSystemSpec->sysBasic.bExceptDummy, m_pSystemSpec->sysBasic.bProfileMode, _T("mm"), Side, strPairLayer);

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

	UINT32 iIdx = 1, iAlignIdx = 1, iPtIdx = 1;
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
	double dAngle = 0.;
	bool bMirror = false;
	for (auto it : m_stPanelInfo.vcStripType)
	{
		GetAngle((Orient)it->iOrient, dAngle, bMirror);
		strValue.Format(_T("<s%d count=\"%d\" angle=\"%1.f\" mirror=\"%d\" profile_width=\"%.3f\" profile_height=\"%.3f\" feature_width=\"%.3f\" feature_height=\"%.3f\"></s%d>\r\n"),
			iIdx, it->nNum, dAngle, bMirror, it->dProfileSize.x, it->dProfileSize.y, it->dFeatureSize.x, it->dFeatureSize.y, iIdx);
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
			strValue.Format(_T("<s%d index=\"%d\" type=\"%d\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\" unit_row_count=\"%d\" unit_col_count=\"%d\">\r\n"),
				iStripIdx, itStrip->iStripIdx + 1, itStrip->iType + 1, itStrip->rcProfile.left, itStrip->rcProfile.top, itStrip->rcProfile.right, itStrip->rcProfile.bottom,
				itStrip->iRowUnitNumInStrip, itStrip->iColUnitNumInStrip);
		}
		else
		{
			strValue.Format(_T("<s%d index=\"%d\" type=\"%d\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\" unit_row_count=\"%d\" unit_col_count=\"%d\">\r\n"),
				iStripIdx, itStrip->iStripIdx + 1, itStrip->iType + 1, itStrip->rcRect.left, itStrip->rcRect.top, itStrip->rcRect.right, itStrip->rcRect.bottom,
				itStrip->iRowUnitNumInStrip, itStrip->iColUnitNumInStrip);
		}

		pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
		clResultFile.Write(pcFiledata, strValue.GetLength());
		free(pcFiledata);

		//[UNIT]
		UINT32 iUnitIdx = 1;
		for (auto itUnit : itStrip->vcUnitInfo)
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

	//NSIS 얼라인 및 측정 정보
	strValue.Format(_T("<align count=\"%d\">\r\n"), m_stPanelInfo.stMeasureInfo.vcAlign.size());
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	iIdx = 1;
	for (auto it : m_stPanelInfo.stMeasureInfo.vcAlign)
	{
		strMag = GetMagnificationString(it->dLens);
		//		strMag = GetMagnificationString(it->bIs3D, it->iScanMode);
		strZoom = GetZoomString(it->dZoom);
		//		strZoom = GetZoomString(it->bIs3D, it->iScanMode);

		//		strValue.Format(_T("<m%d type=\"%s\" is3D=\"%d\" mag=\"%s\" zoom=\"%s\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\"></m%d>\r\n"),
		//			iIdx, GetMeasureTypeString(static_cast<MeasureType>(it->iMeasureType)), it->bIs3D, strMag, strZoom,
		//			it->MinMaxRect.left, it->MinMaxRect.top, it->MinMaxRect.right, it->MinMaxRect.bottom, iIdx);
		strValue.Format(_T("<m%d type=\"%s\" is3D=\"%d\" mag=\"%s\" zoom=\"%s\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\"></m%d>\r\n"),
			iIdx, it->strMeasureType, it->bIs3D, strMag, strZoom,
			it->MinMaxRect.left, it->MinMaxRect.top, it->MinMaxRect.right, it->MinMaxRect.bottom, iIdx);
		pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
		clResultFile.Write(pcFiledata, strValue.GetLength());
		free(pcFiledata);
		iIdx++;
	}

	strValue.Format(_T("</align>\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	strValue.Format(_T("<measure_unit count=\"%d\">\r\n"), m_stPanelInfo.stMeasureInfo.vcUnits.size());
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	iIdx = 1;
	for (auto it : m_stPanelInfo.stMeasureInfo.vcUnits)
	{
		strValue.Format(_T("<u%d align_count=\"%d\" measure_count=\"%d\" type=\"%d\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\">\r\n"),
			iIdx, it->vcAlign.size(), it->vcPoints.size(), it->iType + 1,
			it->UnitRect.left, it->UnitRect.top, it->UnitRect.right, it->UnitRect.bottom);
		pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
		clResultFile.Write(pcFiledata, strValue.GetLength());
		free(pcFiledata);

		iAlignIdx = 1;
		for (auto itAlign : it->vcAlign)
		{
			strMag = GetMagnificationString(itAlign->dLens);
			//			strMag = GetMagnificationString(itAlign->bIs3D, itAlign->iScanMode);
			strZoom = GetZoomString(itAlign->dZoom);
			//			strZoom = GetZoomString(itAlign->bIs3D, itAlign->iScanMode);

			//			strValue.Format(_T("<m%d type=\"%s\" is3D=\"%d\" mag=\"%s\" zoom=\"%s\" index=\"%d\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\"></m%d>\r\n"),
			//				iAlignIdx, GetMeasureTypeString(static_cast<MeasureType>(itAlign->iMeasureType)), itAlign->bIs3D, strMag, strZoom,
			//				itAlign->iUnitIndex, itAlign->MinMaxRect.left, itAlign->MinMaxRect.top, itAlign->MinMaxRect.right, itAlign->MinMaxRect.bottom, iAlignIdx);
			strValue.Format(_T("<m%d type=\"%s\" is3D=\"%d\" mag=\"%s\" zoom=\"%s\" index=\"%d\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\"></m%d>\r\n"),
				iAlignIdx, itAlign->strMeasureType, itAlign->bIs3D, strMag, strZoom,
				itAlign->iUnitIndex, itAlign->MinMaxRect.left, itAlign->MinMaxRect.top, itAlign->MinMaxRect.right, itAlign->MinMaxRect.bottom, iAlignIdx);
			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);
			iAlignIdx++;
		}

		iPtIdx = 1;
		for (auto itPoint : it->vcPoints)
		{
			strMag = GetMagnificationString(itPoint->dLens);
			//			strMag = GetMagnificationString(itPoint->bIs3D, itPoint->iScanMode);
			strZoom = GetZoomString(itPoint->dZoom);
			//			strZoom = GetZoomString(itPoint->bIs3D, itPoint->iScanMode);

			//			strValue.Format(_T("<p%d type=\"%s\" is3D=\"%d\" mag=\"%s\" zoom=\"%s\" side=\"%s\" index=\"%d\" width_range=\"%.3f\" min=\"%.3f\" target=\"%.3f\" max=\"%.3f\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\"></p%d>\r\n"),
			//				iPtIdx, GetMeasureTypeString(static_cast<MeasureType>(itPoint->iMeasureType)), itPoint->bIs3D, strMag, strZoom,
			//				GetMeasureSideString(static_cast<MeasureSide>(itPoint->iMeasureSide)),
			//				itPoint->iUnitIndex, itPoint->stSpec.dRange, itPoint->stSpec.dMin, itPoint->stSpec.dTarget, itPoint->stSpec.dMax,
			//				itPoint->MinMaxRect.left, itPoint->MinMaxRect.top, itPoint->MinMaxRect.right, itPoint->MinMaxRect.bottom, iPtIdx);
			strValue.Format(_T("<p%d type=\"%s\" is3D=\"%d\" mag=\"%s\" zoom=\"%s\" side=\"%s\" index=\"%d\" width_range=\"%.3f\" min=\"%.3f\" target=\"%.3f\" max=\"%.3f\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\"></p%d>\r\n"),
				iPtIdx, itPoint->strMeasureType, itPoint->bIs3D, strMag, strZoom,
				GetMeasureSideString(static_cast<MeasureSide>(itPoint->iMeasureSide)),
				itPoint->iUnitIndex, itPoint->stSpec.dRange, itPoint->stSpec.dMin, itPoint->stSpec.dTarget, itPoint->stSpec.dMax,
				itPoint->MinMaxRect.left, itPoint->MinMaxRect.top, itPoint->MinMaxRect.right, itPoint->MinMaxRect.bottom, iPtIdx);
			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);
			iPtIdx++;
		}

		strValue.Format(_T("</u%d>\r\n"), iIdx);
		pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
		clResultFile.Write(pcFiledata, strValue.GetLength());
		free(pcFiledata);
		iIdx++;
	}

	strValue.Format(_T("</measure_unit>\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	// Image 정보 출력 (CSP 정보)
	strValue.Format(_T("<measure_unit_type count=\"%d\">\r\n"), m_stPanelInfo.vcUnitType.size());
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	RECTD PatternRect;
	CString csFileName = L"";
	double Center_X, Center_Y;
	double PatternSize_X, PatternSize_Y;

	std::vector<bool> vcUnitType;
	vcUnitType.assign(m_stPanelInfo.vcUnitType.size(), false);

	for (auto it : m_stPanelInfo.stMeasureInfo.vcUnits)
	{
		if (!vcUnitType[it->iType])
		{
			strValue.Format(_T("<u%d measure_point_count=\"%d\">\r\n"), it->iType + 1, it->vcPoints.size());
			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);

			UINT32 iPoint = 0;
			for (auto Measure : it->vcPoints)
			{
				// Optic, FOV Size, Resolution 확인
				SIZE FovSize = GetFOVSize(Measure->bIs3D, Measure->dLens, Measure->dZoom);
				double dResol_X = (double)FovSize.cx / (double)NSIS_FOV_X;
				double dResol_Y = (double)FovSize.cy / (double)NSIS_FOV_Y;

				Orient ori = Orient::NoMir0Deg;
				if (m_pUserSetInfo->bMirror)
					ori = Orient::Mir0Deg;

				if (m_pUserSetInfo->prodSide == eProdSide::eTop)
					ori = static_cast<Orient>(static_cast<INT32>(ori) + m_pSystemSpec->sysNSIS.sysCameraOrient_Top);
				else
					ori = static_cast<Orient>(static_cast<INT32>(ori) + m_pSystemSpec->sysNSIS.sysCameraOrient_Bot);

				CRotMirrArr rotMirrArr;
				rotMirrArr.InputData(0.0, 0.0, ori, m_pUserSetInfo->mirrorDir);

				// Pattern 좌표 입력
				switch (static_cast<FeatureType>(Measure->iFeatureType))
				{
				case FeatureType::L:
				{	// Line 인 경우 시작점 끝점 이용
					// Left, Top 시작점
					// Right, Bottom 끝점
					PatternRect.left = ((CFeatureL*)Measure->pFeature)->m_dXs * 1000.;
					PatternRect.top = ((CFeatureL*)Measure->pFeature)->m_dYs * 1000.;
					PatternRect.right = ((CFeatureL*)Measure->pFeature)->m_dXe * 1000.;
					PatternRect.bottom = ((CFeatureL*)Measure->pFeature)->m_dYe * 1000.;

					// 측정 영역 중심 확인
					Center_X = fabs(PatternRect.left + PatternRect.right) / 2;
					Center_Y = fabs(PatternRect.top + PatternRect.bottom) / 2;

					// 시작 점, 끝 점 중심좌표로부터 얼마나 떨어져 있는지 계산
					PatternRect.left -= Center_X;
					PatternRect.top -= Center_Y;
					PatternRect.right -= Center_X;
					PatternRect.bottom -= Center_Y;

					// Mirror 반영
					rotMirrArr.FinalPoint(&PatternRect.left, &PatternRect.top, PatternRect.left, PatternRect.top);
					rotMirrArr.FinalPoint(&PatternRect.right, &PatternRect.bottom, PatternRect.right, PatternRect.bottom);

					// Pixel 단위 변환
					PatternRect.left /= dResol_X;
					PatternRect.top /= dResol_Y;
					PatternRect.right /= dResol_X;
					PatternRect.bottom /= dResol_Y;

					// FOV 중심대비 Pixel 위치 계산
					PatternRect.left += NSIS_FOV_X / 2;
					PatternRect.right += NSIS_FOV_X / 2;
					// Image 좌표 좌표계 Top -> Bottom 이므로 - 연산
					PatternRect.top = (NSIS_FOV_Y / 2) - PatternRect.top;
					PatternRect.bottom = (NSIS_FOV_Y / 2) - PatternRect.bottom;
				}
				break;

				default:
				{
					PatternRect = Measure->MinMaxRect;

					// Mirror 반영 먼저함
					rotMirrArr.FinalPoint(&PatternRect.left, &PatternRect.top, PatternRect.left, PatternRect.top);
					rotMirrArr.FinalPoint(&PatternRect.right, &PatternRect.bottom, PatternRect.right, PatternRect.bottom);

					// 측정 영역 중심 확인
					Center_X = fabs(PatternRect.left + PatternRect.left) / 2;
					Center_Y = fabs(PatternRect.top + PatternRect.bottom) / 2;

					// Pattern Size 확인 및 단위 변환 mm->um
					PatternSize_X = fabs(PatternRect.left - PatternRect.right) * 1000.;
					PatternSize_Y = fabs(PatternRect.top - PatternRect.bottom) * 1000.;

					// Pattern Size Pixel 변환
					PatternSize_X /= dResol_X;
					PatternSize_Y /= dResol_Y;

					// FOV 중심 Pixel 초기화
					PatternRect.left = NSIS_FOV_X / 2;
					PatternRect.top = NSIS_FOV_Y / 2;
					PatternRect.right = NSIS_FOV_X / 2;
					PatternRect.bottom = NSIS_FOV_Y / 2;

					// Pixel 단위 Pattern Size 입력
					PatternRect.left -= (PatternSize_X / 2);
					PatternRect.top -= (PatternSize_Y / 2);
					PatternRect.right += (PatternSize_X / 2);
					PatternRect.bottom += (PatternSize_Y / 2);
				}
				break;
				}

				csFileName.Format(_T("%s_%s_U%02d_P%02d_MEASURE"), m_pJobData->GetModelName(), strWorkLayer, it->iType + 1, iPoint + 1);
				strMag = GetMagnificationString(Measure->dLens);
				strZoom = GetZoomString(Measure->dZoom);

				// Pixel 단위 이므로 소수점 표기 하지 않음
				//strValue.Format(_T("<p%d type=\"%s\" mag=\"%s\" zoom=\"%s\" target=\"%.3f\" width_range=\"%.3f\" left=\"%.0f\" top=\"%.0f\" right=\"%.0f\" bottom=\"%.0f\" cspfilename=\"%s\"></p%d>\r\n"),
				//	iPoint + 1, GetMeasureTypeString(static_cast<MeasureType>(Measure->iMeasureType)), strMag, strZoom, Measure->stSpec.dTarget, Measure->stSpec.dRange,
				//	PatternRect.left, PatternRect.top, PatternRect.right, PatternRect.bottom, csFileName.GetBuffer(), iPoint + 1);
				strValue.Format(_T("<p%d type=\"%s\" mag=\"%s\" zoom=\"%s\" min=\"%.3f\" target=\"%.3f\" max=\"%.3f\" width_range=\"%.3f\" left=\"%.0f\" top=\"%.0f\" right=\"%.0f\" bottom=\"%.0f\" cspfilename=\"%s\"></p%d>\r\n"),
					iPoint + 1, Measure->strMeasureType, strMag, strZoom, Measure->stSpec.dMin, Measure->stSpec.dTarget, Measure->stSpec.dMax, Measure->stSpec.dRange,
					PatternRect.left, PatternRect.top, PatternRect.right, PatternRect.bottom, csFileName.GetBuffer(), iPoint + 1);
				pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
				clResultFile.Write(pcFiledata, strValue.GetLength());
				free(pcFiledata);

				iPoint++;
			}

			strValue.Format(_T("</u%d>\r\n"), it->iType + 1);
			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);

			vcUnitType[it->iType] = true;
		}
	}

	vcUnitType.clear();

	strValue.Format(_T("</measure_unit_type>\r\n"));
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

UINT32 CNSIS::SaveProductInfo_Sem(IN INT32 iStepIdx, IN enum Orient eOrient)
{
	if (m_pJobData == nullptr || m_pUserLayerSetInfo == nullptr ||
		m_pUserSetInfo == nullptr || m_pSystemSpec == nullptr)
		return RESULT_BAD;

	if (m_pJobData->GetModelName().GetLength() < 1 ||
		m_strModelPath.GetLength() < 1)
		return RESULT_BAD;

	//INT32 iStepIdx = m_pUserSetInfo->iStepIdx;
	if (iStepIdx != m_pUserLayerSetInfo->nPanelStepIdx)
		iStepIdx = m_pUserLayerSetInfo->nPanelStepIdx;

	if (iStepIdx < 0 || m_pSystemSpec->sysPath.strMasterPath.GetLength() < 1)
		return RESULT_BAD;

	CString strWorkStep = m_pJobData->m_arrStep[iStepIdx]->m_strStepName;
	CString strWorkLayer = m_pUserSetInfo->strWorkLayer;
	CString strPairLayer = m_pUserSetInfo->strPairLayer;
	CString Side;

	CString strFileName = L"";
	strFileName.Format(L"%s\\%s", m_strModelPath, PRODUCT_INFO_FILE);

	CFile clResultFile;
	if (!clResultFile.Open(strFileName, CFile::modeCreate | CFile::modeReadWrite))
		return RESULT_BAD;

	CHAR *pcFiledata = nullptr;
	CString strValue = L"", strMag = L"", strZoom = L"";

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
	if (m_pUserSetInfo->prodSide == eProdSide::eBot)
		Side = _T("BOT");
	else
		Side = _T("TOP");
	strValue.Format(_T("<info model=\"%s\" rotation=\"%.1f\" mirror=\"%d\" workstep=\"%s\" worklayer=\"%s\" except_dummy=\"%d\" profile_mode=\"%d\" unit=\"%s\" side=\"%s\" pair=\"%s\"></info>\r\n"),
		m_pJobData->GetModelName(), m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror,
		strWorkStep, strWorkLayer, m_pSystemSpec->sysBasic.bExceptDummy, m_pSystemSpec->sysBasic.bProfileMode, _T("mm"), Side, strPairLayer);

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

	UINT32 iIdx = 1, iAlignIdx = 1, iPtIdx = 1;
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
	double dAngle = 0.;
	bool bMirror = false;
	for (auto it : m_stPanelInfo.vcStripType)
	{
		GetAngle((Orient)it->iOrient, dAngle, bMirror);
		strValue.Format(_T("<s%d count=\"%d\" angle=\"%1.f\" mirror=\"%d\" profile_width=\"%.3f\" profile_height=\"%.3f\" feature_width=\"%.3f\" feature_height=\"%.3f\"></s%d>\r\n"),
			iIdx, it->nNum, dAngle, bMirror, it->dProfileSize.x, it->dProfileSize.y, it->dFeatureSize.x, it->dFeatureSize.y, iIdx);
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
			strValue.Format(_T("<s%d index=\"%d\" type=\"%d\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\" unit_row_count=\"%d\" unit_col_count=\"%d\">\r\n"),
				iStripIdx, itStrip->iStripIdx + 1, itStrip->iType + 1, itStrip->rcProfile.left, itStrip->rcProfile.top, itStrip->rcProfile.right, itStrip->rcProfile.bottom,
				itStrip->iRowUnitNumInStrip, itStrip->iColUnitNumInStrip);
		}
		else
		{
			strValue.Format(_T("<s%d index=\"%d\" type=\"%d\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\" unit_row_count=\"%d\" unit_col_count=\"%d\">\r\n"),
				iStripIdx, itStrip->iStripIdx + 1, itStrip->iType + 1, itStrip->rcRect.left, itStrip->rcRect.top, itStrip->rcRect.right, itStrip->rcRect.bottom,
				itStrip->iRowUnitNumInStrip, itStrip->iColUnitNumInStrip);
		}

		pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
		clResultFile.Write(pcFiledata, strValue.GetLength());
		free(pcFiledata);

		//[UNIT]
		UINT32 iUnitIdx = 1;
		for (auto itUnit : itStrip->vcUnitInfo)
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

	//NSIS 얼라인 및 측정 정보
	strValue.Format(_T("<align count=\"%d\">\r\n"), m_stPanelInfo.stMeasureInfo.vcAlign.size());
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);
	
	iIdx = 1;
	for (auto it : m_stPanelInfo.stMeasureInfo.vcAlign)
	{
		strMag = GetMagnificationString(it->dLens);		
		strZoom = GetZoomString(it->dZoom);		
		strValue.Format(_T("<m%d type=\"%s\" is3D=\"%d\" mag=\"%s\" zoom=\"%s\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\"></m%d>\r\n"),
			iIdx, it->strMeasureType, it->bIs3D, strMag, strZoom,
			it->MinMaxRect.left, it->MinMaxRect.top, it->MinMaxRect.right, it->MinMaxRect.bottom, iIdx);
		pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
		clResultFile.Write(pcFiledata, strValue.GetLength());
		free(pcFiledata);
		iIdx++;
	}

	strValue.Format(_T("</align>\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	// Measure Dummy Strip Count	
	UINT32 vcDummy_Cnt = 0;
	for (auto it : m_stPanelInfo.stMeasureInfo.vcStripDummy)
	{
		//int cnt = static_cast<int>(it->vcPoints_SR.size());
		int cnt = static_cast<int>(it->vcPoints.size());
		//if (cnt == 0) vcDummy_Cnt++;
		vcDummy_Cnt += cnt;
	}

	//strValue.Format(_T("<measure_dummy count=\"%d\">\r\n"), m_stPanelInfo.stMeasureInfo.vcStripDummy.size());	
	strValue.Format(_T("<measure_dummy count=\"%d\">\r\n"), vcDummy_Cnt);
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	iIdx = 1;
	for (auto it : m_stPanelInfo.stMeasureInfo.vcStripDummy)
	{		
		if (it->vcAlign.size() > 0)
		{
			strValue.Format(_T("<s%d align_count=\"%d\" measure_count=\"%d\" type=\"%d\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\">\r\n"),
				iIdx, it->vcAlign.size(), it->vcPoints.size(), it->iType + 1,
				it->UnitRect.left, it->UnitRect.top, it->UnitRect.right, it->UnitRect.bottom);
			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);
		

			iAlignIdx = 1;
			for (auto itAlign : it->vcAlign)
			{
				strMag = GetMagnificationString(itAlign->dLens);
				strZoom = GetZoomString(itAlign->dZoom);
				strValue.Format(_T("<m%d type=\"%s\" is3D=\"%d\" mag=\"%s\" zoom=\"%s\" index=\"%d\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\"></m%d>\r\n"),
					iAlignIdx, itAlign->strMeasureType, itAlign->bIs3D, strMag, strZoom,
					itAlign->iStripIndex, itAlign->MinMaxRect.left, itAlign->MinMaxRect.top, itAlign->MinMaxRect.right, itAlign->MinMaxRect.bottom, iAlignIdx);
				pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
				clResultFile.Write(pcFiledata, strValue.GetLength());
				free(pcFiledata);
				iAlignIdx++;
			}

			iPtIdx = 1;
			for (auto itPoint : it->vcPoints)
			{
				strMag = GetMagnificationString(itPoint->dLens);
				strZoom = GetZoomString(itPoint->dZoom);
				strValue.Format(_T("<p%d type=\"%s\" is3D=\"%d\" mag=\"%s\" zoom=\"%s\" side=\"%s\" index=\"%d\" width_range=\"%.3f\" min_oos=\"%.3f\" max_oos=\"%.3f\" min_ooc=\"%.3f\" max_ooc=\"%.3f\" min_oor=\"%.3f\" max_oor=\"%.3f\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\"></p%d>\r\n"),
					iPtIdx, itPoint->strMeasureType, itPoint->bIs3D, strMag, strZoom,
					GetMeasureSideString(static_cast<MeasureSide>(itPoint->iMeasureSide)),
					itPoint->iUnitIndex, itPoint->stSpec.dRange, itPoint->stSpec.dMin, itPoint->stSpec.dMax, itPoint->stSpec.dMin_OOC, itPoint->stSpec.dMax_OOC, itPoint->stSpec.dMin_OOR, itPoint->stSpec.dMax_OOR,
					itPoint->MinMaxRect.left, itPoint->MinMaxRect.top, itPoint->MinMaxRect.right, itPoint->MinMaxRect.bottom, iPtIdx);
				pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
				clResultFile.Write(pcFiledata, strValue.GetLength());
				free(pcFiledata);
				iPtIdx++;
			}

		
			strValue.Format(_T("</s%d>\r\n"), iIdx);
			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);
			iIdx++;
		}
	}

	strValue.Format(_T("</measure_dummy>\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);



	// Measure Dummy SR		
	// vcPoints_SR Count
	UINT32 vcPoints_SR_Cnt = 0;
	for (auto it : m_stPanelInfo.stMeasureInfo.vcStripDummy)
	{
		int cnt = static_cast<int>(it->vcPoints_SR.size());
		vcPoints_SR_Cnt += cnt;
	}
	
	//strValue.Format(_T("<measure_dummy_sr count=\"%d\">\r\n"), m_stPanelInfo.stMeasureInfo.vcStripDummy.size());
	strValue.Format(_T("<measure_dummy_sr count=\"%d\">\r\n"), vcPoints_SR_Cnt);

	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	iIdx = 1;
	iPtIdx = 1;
	for (auto it : m_stPanelInfo.stMeasureInfo.vcStripDummy)
	{
		for (auto itPoint : it->vcPoints_SR)
		{
			strMag = GetMagnificationString(itPoint->dLens);
			strZoom = GetZoomString(itPoint->dZoom);
			strValue.Format(_T("<p%d type=\"%s\" is3D=\"%d\" mag=\"%s\" zoom=\"%s\" side=\"%s\" index=\"%d\" width_range=\"%.3f\" min_oos=\"%.3f\" max_oos=\"%.3f\" min_ooc=\"%.3f\" max_ooc=\"%.3f\" min_oor=\"%.3f\" max_oor=\"%.3f\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\"></p%d>\r\n"),
				iPtIdx, itPoint->strMeasureType, itPoint->bIs3D, strMag, strZoom,
				GetMeasureSideString(static_cast<MeasureSide>(itPoint->iMeasureSide)),
				itPoint->iUnitIndex, itPoint->stSpec.dRange, itPoint->stSpec.dMin, itPoint->stSpec.dMax, itPoint->stSpec.dMin_OOC, itPoint->stSpec.dMax_OOC, itPoint->stSpec.dMin_OOR, itPoint->stSpec.dMax_OOR,
				itPoint->MinMaxRect.left, itPoint->MinMaxRect.top, itPoint->MinMaxRect.right, itPoint->MinMaxRect.bottom, iPtIdx);
			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);
			iPtIdx++;
		}
		
	}

	strValue.Format(_T("</measure_dummy_sr>\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);



	//// vcPoints Count
	UINT32 vcPoints_Cnt = 0;
	UINT32 vcUnits_Cnt = 0;
	for (auto it : m_stPanelInfo.stMeasureInfo.vcUnits)
	{
		int cnt = static_cast<int>(it->vcPoints.size());
		if (cnt > 0) vcUnits_Cnt++ ;

		vcPoints_Cnt += cnt;
	}
	//strValue.Format(_T("<measure_unit count=\"%d\">\r\n"), vcPoints_Cnt);
	strValue.Format(_T("<measure_unit count=\"%d\">\r\n"), vcUnits_Cnt);
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);



	iIdx = 1;
	for (auto it : m_stPanelInfo.stMeasureInfo.vcUnits)
	{
		//if (iIdx > vcPoints_Cnt) break;
		
		bool bWrite = false;
		if (it->vcAlign.size() > 0 || it->vcPoints.size() > 0)
		{
			bWrite = true;

			strValue.Format(_T("<u%d align_count=\"%d\" measure_count=\"%d\" type=\"%d\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\">\r\n"),
				iIdx, it->vcAlign.size(), it->vcPoints.size(), it->iType + 1,
				it->UnitRect.left, it->UnitRect.top, it->UnitRect.right, it->UnitRect.bottom);
			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);
		}

		if (it->vcAlign.size() > 0)
		{
			iAlignIdx = 1;
			for (auto itAlign : it->vcAlign)
			{
				strMag = GetMagnificationString(itAlign->dLens);
				//			strMag = GetMagnificationString(itAlign->bIs3D, itAlign->iScanMode);
				strZoom = GetZoomString(itAlign->dZoom);
				//			strZoom = GetZoomString(itAlign->bIs3D, itAlign->iScanMode);

				//			strValue.Format(_T("<m%d type=\"%s\" is3D=\"%d\" mag=\"%s\" zoom=\"%s\" index=\"%d\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\"></m%d>\r\n"),
				//				iAlignIdx, GetMeasureTypeString(static_cast<MeasureType>(itAlign->iMeasureType)), itAlign->bIs3D, strMag, strZoom,
				//				itAlign->iUnitIndex, itAlign->MinMaxRect.left, itAlign->MinMaxRect.top, itAlign->MinMaxRect.right, itAlign->MinMaxRect.bottom, iAlignIdx);
				strValue.Format(_T("<m%d type=\"%s\" is3D=\"%d\" mag=\"%s\" zoom=\"%s\" index=\"%d\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\"></m%d>\r\n"),
					iAlignIdx, itAlign->strMeasureType, itAlign->bIs3D, strMag, strZoom,
					itAlign->iUnitIndex, itAlign->MinMaxRect.left, itAlign->MinMaxRect.top, itAlign->MinMaxRect.right, itAlign->MinMaxRect.bottom, iAlignIdx);
				pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
				clResultFile.Write(pcFiledata, strValue.GetLength());
				free(pcFiledata);
				iAlignIdx++;
			}
		}

		if (it->vcPoints.size() > 0)
		{

			iPtIdx = 1;
			for (auto itPoint : it->vcPoints)
			{
				strMag = GetMagnificationString(itPoint->dLens);
				//			strMag = GetMagnificationString(itPoint->bIs3D, itPoint->iScanMode);
				strZoom = GetZoomString(itPoint->dZoom);
				//			strZoom = GetZoomString(itPoint->bIs3D, itPoint->iScanMode);

				//			strValue.Format(_T("<p%d type=\"%s\" is3D=\"%d\" mag=\"%s\" zoom=\"%s\" side=\"%s\" index=\"%d\" width_range=\"%.3f\" min=\"%.3f\" target=\"%.3f\" max=\"%.3f\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\"></p%d>\r\n"),
				//				iPtIdx, GetMeasureTypeString(static_cast<MeasureType>(itPoint->iMeasureType)), itPoint->bIs3D, strMag, strZoom,
				//				GetMeasureSideString(static_cast<MeasureSide>(itPoint->iMeasureSide)),
				//				itPoint->iUnitIndex, itPoint->stSpec.dRange, itPoint->stSpec.dMin, itPoint->stSpec.dTarget, itPoint->stSpec.dMax,
				//				itPoint->MinMaxRect.left, itPoint->MinMaxRect.top, itPoint->MinMaxRect.right, itPoint->MinMaxRect.bottom, iPtIdx);

				// 김준호 TEST
				//strValue.Format(_T("<p%d type=\"%s\" is3D=\"%d\" mag=\"%s\" zoom=\"%s\" side=\"%s\" index=\"%d\" width_range=\"%.3f\" min=\"%.3f\" target=\"%.3f\" max=\"%.3f\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\"></p%d>\r\n"),
				strValue.Format(_T("<p%d type=\"%s\" is3D=\"%d\" mag=\"%s\" zoom=\"%s\" side=\"%s\" index=\"%d\" width_range=\"%.3f\" min_oos=\"%.3f\" max_oos=\"%.3f\" min_ooc=\"%.3f\" max_ooc=\"%.3f\" min_oor=\"%.3f\" max_oor=\"%.3f\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\"></p%d>\r\n"),
					iPtIdx, itPoint->strMeasureType, itPoint->bIs3D, strMag, strZoom,
					GetMeasureSideString(static_cast<MeasureSide>(itPoint->iMeasureSide)),
					itPoint->iUnitIndex, itPoint->stSpec.dRange, itPoint->stSpec.dMin, itPoint->stSpec.dMax, itPoint->stSpec.dMin_OOC, itPoint->stSpec.dMax_OOC, itPoint->stSpec.dMin_OOR, itPoint->stSpec.dMax_OOR,
					itPoint->MinMaxRect.left, itPoint->MinMaxRect.top, itPoint->MinMaxRect.right, itPoint->MinMaxRect.bottom, iPtIdx);
				pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
				clResultFile.Write(pcFiledata, strValue.GetLength());
				free(pcFiledata);
				iPtIdx++;
			}
		}

		if (bWrite == true)
		{
			strValue.Format(_T("</u%d>\r\n"), iIdx);
			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);
			iIdx++;
		}
	}

	strValue.Format(_T("</measure_unit>\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);


	// Measure Unit SR	
	// vcPoints_SR Count
	vcPoints_SR_Cnt = 0;
	for (auto it : m_stPanelInfo.stMeasureInfo.vcUnits)
	{
		int cnt = static_cast<int>(it->vcPoints_SR.size());
		if (it->vcPoints_SR.size() > 0)
		{
			vcPoints_SR_Cnt++;
		}		
	}

	//strValue.Format(_T("<measure_unit_sr count=\"%d\">\r\n"), m_stPanelInfo.stMeasureInfo.vcUnits.size());
	strValue.Format(_T("<measure_unit_sr count=\"%d\">\r\n"), vcPoints_SR_Cnt);
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	iIdx = 1;
	for (auto it : m_stPanelInfo.stMeasureInfo.vcUnits)
	{
		
		if (it->vcPoints_SR.size() > 0)
		{
			strValue.Format(_T("<u%d align_count=\"%d\" measure_count=\"%d\" type=\"%d\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\">\r\n"),
				//iIdx, it->vcAlign.size(), it->vcPoints_SR.size(), it->iType + 1,
				iIdx, it->vcAlign.size(), it->vcPoints_SR.size(), 1,
				it->UnitRect.left, it->UnitRect.top, it->UnitRect.right, it->UnitRect.bottom);
			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);

		}
			iPtIdx = 1;
			for (auto itPoint : it->vcPoints_SR)
			{
				strMag = GetMagnificationString(itPoint->dLens);
				strZoom = GetZoomString(itPoint->dZoom);
				strValue.Format(_T("<p%d type=\"%s\" is3D=\"%d\" mag=\"%s\" zoom=\"%s\" side=\"%s\" index=\"%d\" width_range=\"%.3f\" min_oos=\"%.3f\" max_oos=\"%.3f\" min_ooc=\"%.3f\" max_ooc=\"%.3f\" min_oor=\"%.3f\" max_oor=\"%.3f\" left=\"%.3f\" top=\"%.3f\" right=\"%.3f\" bottom=\"%.3f\"></p%d>\r\n"),
					iPtIdx, itPoint->strMeasureType, itPoint->bIs3D, strMag, strZoom,
					GetMeasureSideString(static_cast<MeasureSide>(itPoint->iMeasureSide)),
					itPoint->iUnitIndex, itPoint->stSpec.dRange, itPoint->stSpec.dMin, itPoint->stSpec.dMax, itPoint->stSpec.dMin_OOC, itPoint->stSpec.dMax_OOC, itPoint->stSpec.dMin_OOR, itPoint->stSpec.dMax_OOR,
					itPoint->MinMaxRect.left, itPoint->MinMaxRect.top, itPoint->MinMaxRect.right, itPoint->MinMaxRect.bottom, iPtIdx);
				pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
				clResultFile.Write(pcFiledata, strValue.GetLength());
				free(pcFiledata);
				iPtIdx++;
			}	

		if (it->vcPoints_SR.size() > 0)
		{
			strValue.Format(_T("</u%d>\r\n"), iIdx);
			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);
			iIdx++;
		}

	}

	strValue.Format(_T("</measure_unit_sr>\r\n"));
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);


	// Image 정보 출력 (CSP 정보)
	strValue.Format(_T("<measure_unit_type count=\"%d\">\r\n"), m_stPanelInfo.vcUnitType.size());
	pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
	clResultFile.Write(pcFiledata, strValue.GetLength());
	free(pcFiledata);

	RECTD PatternRect;
	CString csFileName = L"";
	double Center_X, Center_Y;
	double PatternSize_X, PatternSize_Y;

	std::vector<bool> vcUnitType;
	vcUnitType.assign(m_stPanelInfo.vcUnitType.size(), false);

	for (auto it : m_stPanelInfo.stMeasureInfo.vcUnits)
	{
		if (it->vcPoints.size() <= 0) continue;
		if (!vcUnitType[it->iType])		
		{
			strValue.Format(_T("<u%d measure_point_count=\"%d\">\r\n"), it->iType + 1, it->vcPoints.size());
			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);

			UINT32 iPoint = 0;
			for (auto Measure : it->vcPoints)
			{
				// Optic, FOV Size, Resolution 확인
				SIZE FovSize = GetFOVSize(Measure->bIs3D, Measure->dLens, Measure->dZoom);
				double dResol_X = (double)FovSize.cx / (double)NSIS_FOV_X;
				double dResol_Y = (double)FovSize.cy / (double)NSIS_FOV_Y;

				Orient ori = Orient::NoMir0Deg;
				if (m_pUserSetInfo->bMirror)
					ori = Orient::Mir0Deg;

				if (m_pUserSetInfo->prodSide == eProdSide::eTop)
					ori = static_cast<Orient>(static_cast<INT32>(ori) + m_pSystemSpec->sysNSIS.sysCameraOrient_Top);
				else
					ori = static_cast<Orient>(static_cast<INT32>(ori) + m_pSystemSpec->sysNSIS.sysCameraOrient_Bot);

				CRotMirrArr rotMirrArr;
				rotMirrArr.InputData(0.0, 0.0, ori, m_pUserSetInfo->mirrorDir);

				// Pattern 좌표 입력
				switch (static_cast<FeatureType>(Measure->iFeatureType))
				{
				case FeatureType::L:
				{	// Line 인 경우 시작점 끝점 이용
					// Left, Top 시작점
					// Right, Bottom 끝점
					PatternRect.left = ((CFeatureL*)Measure->pFeature)->m_dXs * 1000.;
					PatternRect.top = ((CFeatureL*)Measure->pFeature)->m_dYs * 1000.;
					PatternRect.right = ((CFeatureL*)Measure->pFeature)->m_dXe * 1000.;
					PatternRect.bottom = ((CFeatureL*)Measure->pFeature)->m_dYe * 1000.;

					// 측정 영역 중심 확인
					Center_X = fabs(PatternRect.left + PatternRect.right) / 2;
					Center_Y = fabs(PatternRect.top + PatternRect.bottom) / 2;

					// 시작 점, 끝 점 중심좌표로부터 얼마나 떨어져 있는지 계산
					PatternRect.left -= Center_X;
					PatternRect.top -= Center_Y;
					PatternRect.right -= Center_X;
					PatternRect.bottom -= Center_Y;

					// Mirror 반영
					rotMirrArr.FinalPoint(&PatternRect.left, &PatternRect.top, PatternRect.left, PatternRect.top);
					rotMirrArr.FinalPoint(&PatternRect.right, &PatternRect.bottom, PatternRect.right, PatternRect.bottom);

					// Pixel 단위 변환
					PatternRect.left /= dResol_X;
					PatternRect.top /= dResol_Y;
					PatternRect.right /= dResol_X;
					PatternRect.bottom /= dResol_Y;

					// FOV 중심대비 Pixel 위치 계산
					PatternRect.left += NSIS_FOV_X / 2;
					PatternRect.right += NSIS_FOV_X / 2;
					// Image 좌표 좌표계 Top -> Bottom 이므로 - 연산
					PatternRect.top = (NSIS_FOV_Y / 2) - PatternRect.top;
					PatternRect.bottom = (NSIS_FOV_Y / 2) - PatternRect.bottom;
				}
				break;

				default:
				{
					PatternRect = Measure->MinMaxRect;

					// Mirror 반영 먼저함
					rotMirrArr.FinalPoint(&PatternRect.left, &PatternRect.top, PatternRect.left, PatternRect.top);
					rotMirrArr.FinalPoint(&PatternRect.right, &PatternRect.bottom, PatternRect.right, PatternRect.bottom);

					// 측정 영역 중심 확인
					Center_X = fabs(PatternRect.left + PatternRect.left) / 2;
					Center_Y = fabs(PatternRect.top + PatternRect.bottom) / 2;

					// Pattern Size 확인 및 단위 변환 mm->um
					PatternSize_X = fabs(PatternRect.left - PatternRect.right) * 1000.;
					PatternSize_Y = fabs(PatternRect.top - PatternRect.bottom) * 1000.;

					// Pattern Size Pixel 변환
					PatternSize_X /= dResol_X;
					PatternSize_Y /= dResol_Y;

					// FOV 중심 Pixel 초기화
					PatternRect.left = NSIS_FOV_X / 2;
					PatternRect.top = NSIS_FOV_Y / 2;
					PatternRect.right = NSIS_FOV_X / 2;
					PatternRect.bottom = NSIS_FOV_Y / 2;

					// Pixel 단위 Pattern Size 입력
					PatternRect.left -= (PatternSize_X / 2);
					PatternRect.top -= (PatternSize_Y / 2);
					PatternRect.right += (PatternSize_X / 2);
					PatternRect.bottom += (PatternSize_Y / 2);
				}
				break;
				}

				csFileName.Format(_T("%s_%s_U%02d_P%02d_MEASURE"), m_pJobData->GetModelName(), strWorkLayer, it->iType + 1, iPoint + 1);
				strMag = GetMagnificationString(Measure->dLens);
				strZoom = GetZoomString(Measure->dZoom);

				// Pixel 단위 이므로 소수점 표기 하지 않음
				//strValue.Format(_T("<p%d type=\"%s\" mag=\"%s\" zoom=\"%s\" target=\"%.3f\" width_range=\"%.3f\" left=\"%.0f\" top=\"%.0f\" right=\"%.0f\" bottom=\"%.0f\" cspfilename=\"%s\"></p%d>\r\n"),
				//	iPoint + 1, GetMeasureTypeString(static_cast<MeasureType>(Measure->iMeasureType)), strMag, strZoom, Measure->stSpec.dTarget, Measure->stSpec.dRange,
				//	PatternRect.left, PatternRect.top, PatternRect.right, PatternRect.bottom, csFileName.GetBuffer(), iPoint + 1);
				
				// 원본 코드
				//strValue.Format(_T("<p%d type=\"%s\" mag=\"%s\" zoom=\"%s\" min=\"%.3f\" target=\"%.3f\" max=\"%.3f\" width_range=\"%.3f\" left=\"%.0f\" top=\"%.0f\" right=\"%.0f\" bottom=\"%.0f\" cspfilename=\"%s\"></p%d>\r\n"),
				
				// 김준호 TEST
				//	   --> t_width, bot, min , target, max
				//	   --> t_width, bot, min, max, ooc_min, ooc_max, oor_min, oor_max
				strValue.Format(_T("<p%d type=\"%s\" mag=\"%s\" zoom=\"%s\" min_oos=\"%.3f\" max_oos=\"%.3f\" min_ooc=\"%.3f\" max_ooc=\"%.3f\" min_oor=\"%.3f\" max_oor=\"%.3f\" width_range=\"%.3f\" left=\"%.0f\" top=\"%.0f\" right=\"%.0f\" bottom=\"%.0f\" cspfilename=\"%s\"></p%d>\r\n"),
					iPoint + 1, Measure->strMeasureType, strMag, strZoom, Measure->stSpec.dMin, Measure->stSpec.dMax, Measure->stSpec.dMin_OOC, Measure->stSpec.dMax_OOC, Measure->stSpec.dMin_OOR, Measure->stSpec.dMax_OOR, Measure->stSpec.dRange,
					PatternRect.left, PatternRect.top, PatternRect.right, PatternRect.bottom, csFileName.GetBuffer(), iPoint + 1);
				pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
				clResultFile.Write(pcFiledata, strValue.GetLength());
				free(pcFiledata);

				iPoint++;
			}

			strValue.Format(_T("</u%d>\r\n"), it->iType + 1);
			pcFiledata = Utils.ConvertWideCharToAnsiString(strValue);
			clResultFile.Write(pcFiledata, strValue.GetLength());
			free(pcFiledata);

			vcUnitType[it->iType] = true;
		}
	}

	vcUnitType.clear();

	strValue.Format(_T("</measure_unit_type>\r\n"));
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

SIZE CNSIS::GetFOVSize(bool bIs3D, double dLens, double dZoom)
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

UINT32 CNSIS::MakeUnitLayerImage()
{
	if (m_pJobData == nullptr || m_pUserLayerSetInfo == nullptr ||
		m_pDrawManager == nullptr || m_pSystemSpec == nullptr)
		return RESULT_BAD;

	bool bMirror = false;
	bool bProfileMode = m_pSystemSpec->sysBasic.bProfileMode;
	UINT32 iShapeIdx = 0;
	double dAngle = 0.;
	double dResolMM = 1.0f / 1000.f;

	double dWidthMM = 0, dHeightMM = 0;
	for (auto itStep : m_pUserLayerSetInfo->vcUnitStepIdx)
	{
		if (m_pJobData->m_arrStep[itStep] == nullptr)
			return RESULT_BAD;

		auto itUnitType = m_stPanelInfo.vcUnitType[iShapeIdx];

		GetAngle((Orient)itUnitType->iOrient, dAngle, bMirror);

		if (bProfileMode)
		{
			dWidthMM = itUnitType->dProfileSize.x;
			dHeightMM = itUnitType->dProfileSize.y;
		}
		else
		{
			dWidthMM = itUnitType->dFeatureSize.x;
			dHeightMM = itUnitType->dFeatureSize.y;
		}

		if (dWidthMM > dHeightMM)
			dResolMM = dWidthMM / 1000;
		else
			dResolMM = dHeightMM / 1000;

		CString strFullPathName;
		strFullPathName.Format(_T("%s\\U%d"), m_strModelPath, iShapeIdx + 1);
		CreateDirectory(strFullPathName, NULL);

		if (RESULT_GOOD != m_pDrawManager->SaveUnitLayer(itStep, iShapeIdx, dAngle, bMirror, dResolMM, strFullPathName))
			return RESULT_BAD;

		iShapeIdx++;
	}

	return RESULT_GOOD;
}

//YJD START 22.06.13 - SAVE MEASURE IMAGE
UINT32 CNSIS::MakeMeasurePointImage()
{
	if (m_pJobData == nullptr || m_pUserLayerSetInfo == nullptr ||
		m_pDrawManager == nullptr || m_pSystemSpec == nullptr ||
		m_pUserSetInfo == nullptr)
		return RESULT_BAD;

	bool bMirror = false;
	bool bProfileMode = m_pSystemSpec->sysBasic.bProfileMode;
	double dAngle = 0., dCamAngle = 0.;
	double dResolMM = 1.0f / 1000.f, dTemp = 0;

	if (m_pUserSetInfo->prodSide == eProdSide::eTop)
		GetAngle((Orient)m_pSystemSpec->sysNSIS.sysCameraOrient_Top, dCamAngle, bMirror);
	else
		GetAngle((Orient)m_pSystemSpec->sysNSIS.sysCameraOrient_Bot, dCamAngle, bMirror);

	RECTD rcTempRect;
	UINT32 iShapeIdx = 0;

	for (auto itStep : m_pUserLayerSetInfo->vcUnitStepIdx)
	{
		if (m_pJobData->m_arrStep[itStep] == nullptr)
			return RESULT_BAD;

		auto itUnitType = m_stPanelInfo.vcUnitType[iShapeIdx];

		MeasureUnit_t *pUnit = nullptr;
		for (auto itUnit : m_stPanelInfo.stMeasureInfo.vcUnits)
		{
			if (itUnit->eMapMode == eMinimapMode::Thickness) continue;
			if (iShapeIdx == itUnit->iType)
			{
				pUnit = itUnit;
				break;
			}
		}
		if (pUnit == nullptr)
			return RESULT_BAD;

		GetAngle((Orient)itUnitType->iOrient, dAngle, bMirror);

		UINT32 iMeasurePoint = 0;

		vector<RECTD> vecMeasureRect;
		vector<double> vecMeasureResolution;
		for (auto measure : pUnit->vcPoints)
		{
			//RECTD rcAlign = measure->MinMaxRect_In_Panel;
			RECTD rcAlign = measure->MinMaxRect;

			double dWidthMM = abs(rcAlign.right - rcAlign.left);
			double dHeightMM = abs(rcAlign.top - rcAlign.bottom);

			double Center_X = (rcAlign.left + rcAlign.right) / 2.;
			double Center_Y = (rcAlign.top + rcAlign.bottom) / 2.;

			// 			Center_X -= ((pUnit->UnitRect_In_Panel.left + pUnit->UnitRect_In_Panel.right) / 2.0);
			// 			Center_Y -= ((pUnit->UnitRect_In_Panel.top + pUnit->UnitRect_In_Panel.bottom) / 2.0);

			Center_X -= ((pUnit->UnitRect.left + pUnit->UnitRect.right) / 2.0);
			Center_Y -= ((pUnit->UnitRect.top + pUnit->UnitRect.bottom) / 2.0);

			SIZE szFov = GetFOVSize(measure->bIs3D, measure->dLens, measure->dZoom);
			dResolMM = (double)szFov.cx / 1000. / (double)NSIS_FOV_X;

			//Measure 영역을 한번에 저장하기 때문에
			//Resolution도 각각 저장 후 Save 해야함
			vecMeasureResolution.push_back(dResolMM);

			// Pattern Size 긴 방향 정사각형으로 자름
			if (dWidthMM > dHeightMM)
			{
				rcTempRect.left = Center_X - (dWidthMM / 2.);
				rcTempRect.top = Center_Y + (dWidthMM / 2.);
				rcTempRect.right = Center_X + (dWidthMM / 2.);
				rcTempRect.bottom = Center_Y - (dWidthMM / 2.);
			}
			else
			{
				rcTempRect.left = Center_X - (dHeightMM / 2.);
				rcTempRect.top = Center_Y + (dHeightMM / 2.);
				rcTempRect.right = Center_X + (dHeightMM / 2.);
				rcTempRect.bottom = Center_Y - (dHeightMM / 2.);
			}

			// FOV X 방향 5% 여유공간 가져감
			rcTempRect.left -= (szFov.cx / 1000. * 0.2);
			rcTempRect.top += (szFov.cx / 1000. * 0.2);
			rcTempRect.right += (szFov.cx / 1000. * 0.2);
			rcTempRect.bottom -= (szFov.cx / 1000. * 0.2);


			vecMeasureRect.push_back(rcTempRect);

			//// Panel 내의 Align Point 위치를 Unit 내 원본 좌표로 변환
			//CRotMirrArr rotMirrArr;
			//rotMirrArr.InputData(0, 0, ReverseOrient((Orient)itUnitType->iOrient));
			//rotMirrArr.FinalPoint(&rcMeasureRect.left, &rcMeasureRect.top, rcTempRect.left, rcTempRect.top);
			//rotMirrArr.FinalPoint(&rcMeasureRect.right, &rcMeasureRect.bottom, rcTempRect.right, rcTempRect.bottom);
			//rotMirrArr.ClearAll();

			//if (bMirror)
			//{
			//	if (dAngle == 0.0)
			//	{
			//		SWAP(rcMeasureRect.left, rcMeasureRect.right, dTemp);
			//		SWAP(rcMeasureRect.top, rcMeasureRect.bottom, dTemp);
			//	}
			//	else if (dAngle == 90.0)
			//	{
			//		rcMeasureRect.left *= -1.0;
			//		rcMeasureRect.right *= -1.0;

			//		SWAP(rcMeasureRect.top, rcMeasureRect.bottom, dTemp);
			//		rcMeasureRect.top *= -1.0;
			//		rcMeasureRect.bottom *= -1.0;
			//	}
			//	else if (dAngle == 270.0)
			//	{
			//		SWAP(rcMeasureRect.left, rcMeasureRect.right, dTemp);
			//		rcMeasureRect.left *= -1.0;
			//		rcMeasureRect.right *= -1.0;

			//		rcMeasureRect.top *= -1.0;
			//		rcMeasureRect.bottom *= -1.0;
			//	}
			//}


		}

		if (RESULT_GOOD != m_pDrawManager->SaveMeasurePoint(itStep, iShapeIdx, dAngle, bMirror, dCamAngle, vecMeasureResolution, vecMeasureRect, m_strModelPath))
			return RESULT_BAD;

		iShapeIdx++;
	}

	return RESULT_GOOD;
}
//YJD END 22.06.13 - SAVE MEASURE IMAGE


UINT32 CNSIS::MakeUnitAlignPointImage()
{
	if (m_pJobData == nullptr || m_pUserLayerSetInfo == nullptr ||
		m_pDrawManager == nullptr || m_pSystemSpec == nullptr ||
		m_pUserSetInfo == nullptr)
		return RESULT_BAD;

	bool bMirror = false;

	bool bProfileMode = m_pSystemSpec->sysBasic.bProfileMode;
	double dAngle = 0., dCamAngle = 0.;	
	double dResolMM = 1.0f / 1000.f, dTemp = 0;
	double dRatio = m_pSystemSpec->sysNSIS.sysUnitAlignRatio;
	Orient ori = Orient::Mir0Deg;

	dRatio /= 100.0;
	dRatio /= 2.0;

	//double dAngle = m_pUserSetInfo->dAngle;

	if (m_pUserSetInfo->prodSide == eProdSide::eTop)
		GetAngle((Orient)m_pSystemSpec->sysNSIS.sysCameraOrient_Top, dCamAngle, bMirror);
	else
		GetAngle((Orient)m_pSystemSpec->sysNSIS.sysCameraOrient_Bot, dCamAngle, bMirror);

	RECTD rcTempRect, rcAlignRect;
	UINT32 iShapeIdx = 0;
	for (auto itStep : m_pUserLayerSetInfo->vcUnitStepIdx)
	{
		if (m_pJobData->m_arrStep[itStep] == nullptr)
			return RESULT_BAD;

		auto itUnitType = m_stPanelInfo.vcUnitType[iShapeIdx];

		MeasureUnit_t *pUnit = nullptr;
		for (auto itUnit : m_stPanelInfo.stMeasureInfo.vcUnits)
		{
			if (iShapeIdx == itUnit->iType)
			{
				pUnit = itUnit;
				break;
			}
		}
		if (pUnit == nullptr)
			return RESULT_BAD;

		GetAngle((Orient)itUnitType->iOrient, dAngle, bMirror);		

		//if (m_pUserSetInfo->prodSide == eProdSide::eTop)
		//{
		//	//GetAngle((Orient)m_pSystemSpec->sysNSIS.sysCameraOrient_Top, dCamAngle, bMirror);
		//	GetOrient(dAngle, bMirror, ori);
		//	GetAngle((Orient)ori, dCamAngle, bMirror);
		//}
		//else
		//{
		//	//GetAngle((Orient)m_pSystemSpec->sysNSIS.sysCameraOrient_Bot, dCamAngle, bMirror);		
		//	dAngle = 180.;
		//	GetOrient(dAngle, bMirror, ori);
		//	GetAngle((Orient)ori, dCamAngle, bMirror);
		//}

		
		UINT32 iAlignIdx = 0;
		for (auto align : pUnit->vcAlign)
		{
			RECTD rcAlign = align->MinMaxRect_In_Panel;

			double dWidthMM = abs(rcAlign.right - rcAlign.left);
			double dHeightMM = abs(rcAlign.top - rcAlign.bottom);

			double Center_X = (rcAlign.left + rcAlign.right) / 2.;
			double Center_Y = (rcAlign.top + rcAlign.bottom) / 2.;
			
			//Center_X -= ((pUnit->UnitRect_In_Panel.left + pUnit->UnitRect_In_Panel.right) / 2.0);
			//Center_Y -= ((pUnit->UnitRect_In_Panel.top + pUnit->UnitRect_In_Panel.bottom) / 2.0);

			Center_X -= ((pUnit->UnitRect.left + pUnit->UnitRect.right) / 2.0);
			Center_Y -= ((pUnit->UnitRect.top + pUnit->UnitRect.bottom) / 2.0);

			SIZE szFov = GetFOVSize(align->bIs3D, align->dLens, align->dZoom);
			dResolMM = (double)szFov.cx / 1000. / (double)NSIS_FOV_X; // mm/Pixel 단위

//			CorrectPos(false, (Orient)itUnitType->iOrient, rcUnitRect, Center_X, Center_Y);

			// Pattern Size 긴 방향 정사각형으로 자름
			if (dWidthMM > dHeightMM)
			{
				rcTempRect.left		= Center_X - (dWidthMM / 2.);
				rcTempRect.top		= Center_Y + (dWidthMM / 2.);
				rcTempRect.right	= Center_X + (dWidthMM / 2.);
				rcTempRect.bottom	= Center_Y - (dWidthMM / 2.);
			}
			else
			{
				rcTempRect.left		= Center_X - (dHeightMM / 2.);
				rcTempRect.top		= Center_Y + (dHeightMM / 2.);
				rcTempRect.right	= Center_X + (dHeightMM / 2.);
				rcTempRect.bottom	= Center_Y - (dHeightMM / 2.);
			}

			// FOV X 방향 5% 여유공간 가져감
			rcTempRect.left		-= (szFov.cx / 1000. * dRatio);
			rcTempRect.top		+= (szFov.cx / 1000. * dRatio);
			rcTempRect.right	+= (szFov.cx / 1000. * dRatio);
			rcTempRect.bottom	-= (szFov.cx / 1000. * dRatio);

			rcAlignRect = rcTempRect;
			/*

			// Panel 내의 Align Point 위치를 Unit 내 원본 좌표로 변환
			CRotMirrArr rotMirrArr;
			rotMirrArr.InputData(0, 0, ReverseOrient((Orient)itUnitType->iOrient), m_pUserSetInfo->mirrorDir);			
			rotMirrArr.FinalPoint(&rcAlignRect.left, &rcAlignRect.top, rcTempRect.left, rcTempRect.top);
			rotMirrArr.FinalPoint(&rcAlignRect.right, &rcAlignRect.bottom, rcTempRect.right, rcTempRect.bottom);
			rotMirrArr.ClearAll();

			if (bMirror)
			{
				//Mirror 옵션 시, 좌표계간 불일치가 있는 것으로 파악 중
				//180도의 경우, 기본 좌표계와 동일
				//그 외 회전일 경우, 좌표계 일치 작업 필요
				//추가적인 검증 필요

				if (dAngle == 0.0)
				{
					SWAP(rcAlignRect.left, rcAlignRect.right, dTemp);
					SWAP(rcAlignRect.top, rcAlignRect.bottom, dTemp);
				}
				else if (dAngle == 90.0)
				{
					rcAlignRect.left *= -1.0;
					rcAlignRect.right *= -1.0;

					SWAP(rcAlignRect.top, rcAlignRect.bottom, dTemp);
					rcAlignRect.top		*= -1.0;
					rcAlignRect.bottom	*= -1.0;
				}
				// 2022.07.05
				// 김준호 주석추가
				// Back 면 일 경우 좌표 계산위해 추가.
				else if (dAngle == 180.0)
				{
					//SWAP(drtFeature.left, drtFeature.right, dTemp);
					rcAlignRect.left *= -1.0;
					rcAlignRect.right *= -1.0;
					//SWAP(rcAlignRect.top, rcAlignRect.bottom, dTemp);
					//drtFeature.top *= -1.0;
					//drtFeature.bottom *= -1.0;
				}
				else if (dAngle == 270.0)
				{
					SWAP(rcAlignRect.left, rcAlignRect.right, dTemp);
					rcAlignRect.left *= -1.0;
					rcAlignRect.right *= -1.0;
					
					rcAlignRect.top		*= -1.0;
					rcAlignRect.bottom	*= -1.0;
				}
			}
			// 2022.07.05
			// JDJ Code Add
			// Front 인 경우 Item 좌표 계산하기 위해 추가.
			//else
			//{
			//	if (dAngle == 90.0)
			//	{
			//		SWAP(rcAlignRect.top, rcAlignRect.left, dTemp);
			//		rcAlignRect.left *= -1;

			//		SWAP(rcAlignRect.bottom, rcAlignRect.right, dTemp);
			//		rcAlignRect.right *= -1;
			//	}

			//	else if (dAngle == 180.0)
			//	{
			//		//          SWAP(drtFeature.top, drtFeature.left, dTemp);
			//		//          SWAP(drtFeature.bottom, drtFeature.right, dTemp);
			//		rcAlignRect.bottom *= -1;
			//		rcAlignRect.top *= -1;
			//		rcAlignRect.left *= -1;
			//		rcAlignRect.right *= -1;
			//	}

			//	else if (dAngle == 270.0)
			//	{
			//		SWAP(rcAlignRect.top, rcAlignRect.left, dTemp);
			//		rcAlignRect.top *= -1;

			//		SWAP(rcAlignRect.bottom, rcAlignRect.right, dTemp);
			//		rcAlignRect.bottom *= -1;
			//	}
			//}
			*/

			CString strLog;
			strLog.Format(L"\nUnit Align,%d, %f, %f, %f, %f", iAlignIdx + 1, rcAlign.left, rcAlign.top, rcAlign.right, rcAlign.bottom);
			OutputDebugString(strLog);
			strLog.Format(L"\nUnit AlignRect,%d, %f, %f, %f, %f", iAlignIdx + 1, rcAlignRect.left, rcAlignRect.top, rcAlignRect.right, rcAlignRect.bottom);
			OutputDebugString(strLog);
			
			if (RESULT_GOOD != m_pDrawManager->SaveUnitAlignPoint(itStep, iShapeIdx, iAlignIdx, dAngle, bMirror, dCamAngle, dResolMM, rcAlignRect, m_strModelPath))
				return RESULT_BAD;
			

			iAlignIdx++;
		}

		iShapeIdx++;
	}

	return RESULT_GOOD;
}

UINT32 CNSIS::MakeUnitAlignPointImage_Sem()
{
	if (m_pJobData == nullptr || m_pUserLayerSetInfo == nullptr ||
		m_pDrawManager == nullptr || m_pSystemSpec == nullptr ||
		m_pUserSetInfo == nullptr)
		return RESULT_BAD;

	bool bMirror = false;

	bool bProfileMode = m_pSystemSpec->sysBasic.bProfileMode;
	double dAngle = 0., dCamAngle = 0.;
	double dResolMM = 1.0f / 1000.f, dTemp = 0;
	double dRatio = m_pSystemSpec->sysNSIS.sysUnitAlignRatio;
	Orient ori = Orient::Mir0Deg;

	dRatio /= 100.0;
	dRatio /= 2.0;

	//double dAngle = m_pUserSetInfo->dAngle;

	if (m_pUserSetInfo->prodSide == eProdSide::eTop)
	{
		GetAngle((Orient)m_pSystemSpec->sysNSIS.sysCameraOrient_Top, dCamAngle, bMirror);
	}
	else
	{
		GetAngle((Orient)m_pSystemSpec->sysNSIS.sysCameraOrient_Bot, dCamAngle, bMirror);
	}

	

	RECTD rcTempRect, rcAlignRect;
	UINT32 iShapeIdx = 0;
	for (auto itStep : m_pUserLayerSetInfo->vcUnitStepIdx)
	{
		if (m_pJobData->m_arrStep[itStep] == nullptr)
			return RESULT_BAD;

		auto itUnitType = m_stPanelInfo.vcUnitType[iShapeIdx];

		MeasureUnit_t *pUnit = nullptr;
		for (auto itUnit : m_stPanelInfo.stMeasureInfo.vcUnits)
		{
			if (itUnit->eMapMode == eMinimapMode::Thickness) continue;
			if (iShapeIdx == itUnit->iType)
			{
				pUnit = itUnit;
				break;
			}
		}
		if (pUnit == nullptr)
			return RESULT_BAD;

		GetAngle((Orient)itUnitType->iOrient, dAngle, bMirror);

		UINT32 iAlignIdx = 0;
		for (auto align : pUnit->vcAlign)
		{
			RECTD rcAlign = align->MinMaxRect;

			double dWidthMM = abs(rcAlign.right - rcAlign.left);
			double dHeightMM = abs(rcAlign.top - rcAlign.bottom);

			double Center_X = (rcAlign.left + rcAlign.right) / 2.;
			double Center_Y = (rcAlign.top + rcAlign.bottom) / 2.;

			//Center_X -= ((pUnit->UnitRect_In_Panel.left + pUnit->UnitRect_In_Panel.right) / 2.0);
			//Center_Y -= ((pUnit->UnitRect_In_Panel.top + pUnit->UnitRect_In_Panel.bottom) / 2.0);

			Center_X -= ((pUnit->UnitRect.left + pUnit->UnitRect.right) / 2.0);
			Center_Y -= ((pUnit->UnitRect.top + pUnit->UnitRect.bottom) / 2.0);

			SIZE szFov = GetFOVSize(align->bIs3D, align->dLens, align->dZoom);
			dResolMM = (double)szFov.cx / 1000. / (double)NSIS_FOV_X; // mm/Pixel 단위

//			CorrectPos(false, (Orient)itUnitType->iOrient, rcUnitRect, Center_X, Center_Y);

			// Pattern Size 긴 방향 정사각형으로 자름
			if (dWidthMM > dHeightMM)
			{
				rcTempRect.left = Center_X - (dWidthMM / 2.);
				rcTempRect.top = Center_Y + (dWidthMM / 2.);
				rcTempRect.right = Center_X + (dWidthMM / 2.);
				rcTempRect.bottom = Center_Y - (dWidthMM / 2.);
			}
			else
			{
				rcTempRect.left = Center_X - (dHeightMM / 2.);
				rcTempRect.top = Center_Y + (dHeightMM / 2.);
				rcTempRect.right = Center_X + (dHeightMM / 2.);
				rcTempRect.bottom = Center_Y - (dHeightMM / 2.);
			}

			// FOV X 방향 5% 여유공간 가져감
			rcTempRect.left -= (szFov.cx / 1000. * dRatio);
			rcTempRect.top += (szFov.cx / 1000. * dRatio);
			rcTempRect.right += (szFov.cx / 1000. * dRatio);
			rcTempRect.bottom -= (szFov.cx / 1000. * dRatio);

			rcAlignRect = rcTempRect;

			CString strLog;
			strLog.Format(L"\nUnit Align,%d, %f, %f, %f, %f", iAlignIdx + 1, rcAlign.left, rcAlign.top, rcAlign.right, rcAlign.bottom);
			OutputDebugString(strLog);
			strLog.Format(L"\nUnit AlignRect,%d, %f, %f, %f, %f", iAlignIdx + 1, rcAlignRect.left, rcAlignRect.top, rcAlignRect.right, rcAlignRect.bottom);
			OutputDebugString(strLog);

			if (RESULT_GOOD != m_pDrawManager->SaveUnitAlignPoint(itStep, iShapeIdx, iAlignIdx, dAngle, bMirror, dCamAngle, dResolMM, rcAlignRect, m_strModelPath))
				return RESULT_BAD;


			iAlignIdx++;
		}

		iShapeIdx++;
	}

	return RESULT_GOOD;
}

void CNSIS::CorrectPos(bool bIsPanel, Orient ori, RECTD rcRect, double &dCenterX, double &dCenterY)
{
	double dTemp = 0;

	switch (ori)
	{
		case Orient::NoMir0Deg:
		break;

		case Orient::NoMir90Deg:
		{
			dTemp = dCenterX;
			dCenterX = -dCenterY;
			dCenterY = dTemp;
		}
		break;

		case Orient::NoMir180Deg:
		{
			dCenterX = -dCenterX;
			dCenterY = -dCenterY;
		}
		break;

		case Orient::NoMir270Deg:
		{
			dTemp = dCenterX;
			dCenterX = dCenterY;
			dCenterY = -dTemp;
		}
		break;

		case Orient::Mir0Deg:
		{
			// X 축 반전
			if (bIsPanel)
			{
				dCenterX = fabs(rcRect.right - rcRect.left) - dCenterX;
				dCenterX *= -1;
			}
			else
				dCenterX = ((rcRect.right + rcRect.left) / 2) - dCenterX;
		}
		break;

		case Orient::Mir90Deg:
		{
			dTemp = dCenterX;
			dCenterX = -dCenterY;
			dCenterY = dTemp;
			
			// Y 축 반전
			if (bIsPanel)
			{
				dCenterY = fabs(rcRect.top - rcRect.bottom) - dCenterY;
				dCenterY *= -1;
			}
			else
				dCenterY = ((rcRect.top + rcRect.bottom) / 2) - dCenterY;
		}
		break;

		case Orient::Mir180Deg:
		{
			dCenterX = -dCenterX;
			dCenterY = -dCenterY;

			// X 축 반전
			if (bIsPanel)
			{
				dCenterX = fabs(rcRect.right - rcRect.left) - dCenterX;
				dCenterX *= -1;
			}
			else
				dCenterX = ((rcRect.right + rcRect.left) / 2) - dCenterX;
		}
		break;

		case Orient::Mir270Deg:
		{
			dTemp = dCenterX;
			dCenterX = dCenterY;
			dCenterY = -dTemp;

			// Y 축 반전
			if (bIsPanel)
			{
				dCenterY = fabs(rcRect.top - rcRect.bottom) - dCenterY;
				dCenterY *= -1;
			}
			else
				dCenterY = ((rcRect.top + rcRect.bottom) / 2) - dCenterY;
		}
		break;
	}
}

UINT32 CNSIS::MakePanelLayerImage()
{
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

	bool bMirror = false;
	double dCamAngle = 0.;
	double dResolMM = 2000.f / 1000.f;

	if (m_pUserSetInfo->prodSide == eProdSide::eTop)
		GetAngle((Orient)m_pSystemSpec->sysNSIS.sysCameraOrient_Top, dCamAngle, bMirror);
	else
		GetAngle((Orient)m_pSystemSpec->sysNSIS.sysCameraOrient_Bot, dCamAngle, bMirror);

	for (int i = 0; i < static_cast<int>(m_stPanelInfo.iStripTypeNum); i++)
	{
		if (RESULT_GOOD != m_pDrawManager->SavePanelLayer(iStepIdx, i, dCamAngle, dResolMM, m_strModelPath))
			return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 CNSIS::MakePanelAlignPointImage()
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

	//bool bMirror = false;
	bool bMirror = m_pUserSetInfo->bMirror;
	double dCamAngle = 0., dTemp = 0.;
	double dResolMM = 50.0f / 1000.f;
	double dRatio = m_pSystemSpec->sysNSIS.sysAlignRatio;
	double dAngle = m_pUserSetInfo->dAngle;
	Orient ori = Orient::Mir0Deg;

	dRatio /= 100.0;
	dRatio /= 2.0;

	if (m_pUserSetInfo->prodSide == eProdSide::eTop)	
	{			
		GetAngle((Orient)m_pSystemSpec->sysNSIS.sysCameraOrient_Top, dCamAngle, bMirror);
	}	
	else
	{			
		GetAngle((Orient)m_pSystemSpec->sysNSIS.sysCameraOrient_Bot, dCamAngle, bMirror);		
	}

	//bMirror = m_pUserSetInfo->bMirror;
	//GetOrient(dAngle, bMirror, ori);

	RECTD rcTempRect, rcAlignRect;
	
	UINT32 iAlignIdx = 0;
	for (auto align : m_stPanelInfo.stMeasureInfo.vcAlign)
	{
		RECTD rcAlign = align->MinMaxRect_In_Panel;

		double dWidthMM = abs(rcAlign.right - rcAlign.left);
		double dHeightMM = abs(rcAlign.top - rcAlign.bottom);

		double Center_X = (rcAlign.left + rcAlign.right) / 2.;
		double Center_Y = (rcAlign.top + rcAlign.bottom) / 2.;
		
//		CorrectPos(true, ori, m_stPanelInfo.rcRect, Center_X, Center_Y);

		SIZE szFov = GetFOVSize(align->bIs3D, align->dLens, align->dZoom);
		dResolMM = (double)szFov.cx / 1000. / (double)NSIS_FOV_X; // mm/Pixel 단위
				
		// 정사각형 형태로 자르므로 Size 통일
		if (dWidthMM > dHeightMM)
		{
			rcTempRect.left		= Center_X - (dWidthMM / 2.);
			rcTempRect.top		= Center_Y + (dWidthMM / 2.);
			rcTempRect.right	= Center_X + (dWidthMM / 2.);
			rcTempRect.bottom	= Center_Y - (dWidthMM / 2.);
		}
		else
		{
			rcTempRect.left		= Center_X - (dHeightMM / 2.);
			rcTempRect.top		= Center_Y + (dHeightMM / 2.);
			rcTempRect.right	= Center_X + (dHeightMM / 2.);
			rcTempRect.bottom	= Center_Y - (dHeightMM / 2.);
		}

		// FOV X 방향 5% 여유공간 가져감
		rcTempRect.left		-= (szFov.cx / 1000. * dRatio);
		rcTempRect.top		+= (szFov.cx / 1000. * dRatio);
		rcTempRect.right	+= (szFov.cx / 1000. * dRatio);
		rcTempRect.bottom	-= (szFov.cx / 1000. * dRatio);

		rcAlignRect = rcTempRect;

		// 2022.10.26 KJH ADD
		// Save Image Pixel Size 계산 Alert 위해 추가 
		double width_pixel = rcTempRect.width() / dResolMM + 0.5;
		double height_pixel = rcTempRect.height() / dResolMM + 0.5;

		if (width_pixel > m_pSystemSpec->sysBasic.dSaveImageWidth || height_pixel > m_pSystemSpec->sysBasic.dSaveImageHeight)
		{
			CString strTemp;
			//strTemp.Format(L"Align %d 의 Image Pixel Size 가\n기준 Size Width :%4d, Height :%4d 보다 큽니다.", iAlignIdx +1, static_cast<int>(m_pSystemSpec->sysBasic.dSaveImageWidth), static_cast<int>(m_pSystemSpec->sysBasic.dSaveImageHeight));
			strTemp.Format(L"Align %d 의 Size Width :%4d, Height :%4d 가\n기준 Size Width :%4d, Height :%4d 보다 큽니다.", iAlignIdx+1, static_cast<int>(width_pixel), static_cast<int>(height_pixel), static_cast<int>(m_pSystemSpec->sysBasic.dSaveImageWidth), static_cast<int>(m_pSystemSpec->sysBasic.dSaveImageHeight));
			AfxMessageBox(strTemp);
		}
		// KJH END

		/*
		// Panel 내의 Align Point 위치를 Unit 내 원본 좌표로 변환
		CRotMirrArr rotMirrArr;
		rotMirrArr.InputData(0, 0, ReverseOrient(ori), m_pUserSetInfo->mirrorDir);
		rotMirrArr.FinalPoint(&rcAlignRect.left, &rcAlignRect.top, rcTempRect.left, rcTempRect.top);
		rotMirrArr.FinalPoint(&rcAlignRect.right, &rcAlignRect.bottom, rcTempRect.right, rcTempRect.bottom);
		rotMirrArr.ClearAll();
		
		
		// Back 인 경우
		if (bMirror)
		{
			if (dAngle == 0.0)
			{
				SWAP(rcAlignRect.left, rcAlignRect.right, dTemp);
				SWAP(rcAlignRect.top, rcAlignRect.bottom, dTemp);
			}
			else if (dAngle == 90.0)
			{
				rcAlignRect.left *= -1.0;
				rcAlignRect.right *= -1.0;

				SWAP(rcAlignRect.top, rcAlignRect.bottom, dTemp);
				rcAlignRect.top *= -1.0;
				rcAlignRect.bottom *= -1.0;
			}
			// 2022.07.05
			// 김준호 주석추가
			// Back 면 일 경우 좌표 계산위해 추가.
			else if (dAngle == 180.0)
			{
				//SWAP(drtFeature.left, drtFeature.right, dTemp);
				rcAlignRect.left *= -1.0;
				rcAlignRect.right *= -1.0;
				//SWAP(drtFeature.top, drtFeature.bottom, dTemp);
				//drtFeature.top *= -1.0;
				//drtFeature.bottom *= -1.0;
			}
			else if (dAngle == 270.0)
			{
				SWAP(rcAlignRect.left, rcAlignRect.right, dTemp);
				rcAlignRect.left *= -1.0;
				rcAlignRect.right *= -1.0;

				rcAlignRect.top *= -1.0;
				rcAlignRect.bottom *= -1.0;
			}
		}
		// 2022.07.05
	    // JDJ Code Add
	    // Front 인 경우 Item 좌표 계산하기 위해 추가.
		else
		{
			if (dAngle == 90.0)
			{
				SWAP(rcAlignRect.top, rcAlignRect.left, dTemp);
				rcAlignRect.left *= -1;

				SWAP(rcAlignRect.bottom, rcAlignRect.right, dTemp);
				rcAlignRect.right *= -1;
			}

			else if (dAngle == 180.0)
			{
				//          SWAP(drtFeature.top, drtFeature.left, dTemp);
				//          SWAP(drtFeature.bottom, drtFeature.right, dTemp);
				rcAlignRect.bottom *= -1;
				rcAlignRect.top *= -1;
				rcAlignRect.left *= -1;
				rcAlignRect.right *= -1;
			}

			else if (dAngle == 270.0)
			{
				SWAP(rcAlignRect.top, rcAlignRect.left, dTemp);
				rcAlignRect.top *= -1;
				
				SWAP(rcAlignRect.bottom, rcAlignRect.right, dTemp);
				rcAlignRect.bottom *= -1;
			}
		}
		*/
		
		CString strLog;
		//strLog.Format(L"\nAlign,%d, %f, %f, %f, %f", iAlignIdx + 1, rcAlign.left, rcAlign.top, rcAlign.right, rcAlign.bottom);
		//OutputDebugString(strLog);		
		strLog.Format(L"\nAlign Rect,%d, %f, %f, %f, %f", iAlignIdx + 1, rcAlignRect.left, rcAlignRect.top, rcAlignRect.right, rcAlignRect.bottom);
		OutputDebugString(strLog);

		if (RESULT_GOOD != m_pDrawManager->SavePanelAlignPoint(iStepIdx, iAlignIdx, dCamAngle, dResolMM, rcAlignRect, m_strModelPath))
			return RESULT_BAD;

		iAlignIdx++;
	}

	return RESULT_GOOD;
}

CString CNSIS::GetModelPath(IN bool bCreateFolder)
{
	if (RESULT_GOOD != CheckStructure())
		return L"";

	if (m_pJobData->GetModelName().GetLength() < 1)
		return L"";

	CString strWorkLayer = m_pUserSetInfo->strWorkLayer;
	CString strModelPath = L"";

	strModelPath.Format(L"%s\\%s", m_pSystemSpec->sysPath.strMasterPath, m_pJobData->GetModelName());
	if (bCreateFolder)
		CreateDirectory(strModelPath, NULL);

	strModelPath.Format(L"%s\\%s\\%s", m_pSystemSpec->sysPath.strMasterPath, m_pJobData->GetModelName(), strWorkLayer);
	if (m_pUserSetInfo->prodSide == eProdSide::eTop)
		strModelPath += L"_T";
	else if (m_pUserSetInfo->prodSide == eProdSide::eBot)
		strModelPath += L"_B";

	if (bCreateFolder)
		CreateDirectory(strModelPath, NULL);

	if (strModelPath.GetLength() < 1)
		return L"";

	return strModelPath;
}

UINT32 CNSIS::MakeProfileImage()
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