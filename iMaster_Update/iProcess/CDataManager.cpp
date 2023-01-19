
#include "stdafx.h"
#include "CDataManager.h"

//Doxygen
/**
 * @brief 
 * @details
 * @author 
 * @date
 * @param
 * @param
 * @return
 */

CDataManager::CDataManager()
{
	m_vecmapvecWidth.resize(static_cast<int>(eLineWidthOption::LineWidthOptionNone));
	m_vecvecWidthSort.resize(static_cast<int>(eLineWidthOption::LineWidthOptionNone));
	
}

CDataManager::~CDataManager()
{
}

UINT32 CDataManager::SetSystemSpec(SystemSpec_t* pSystemSpec)
{
	if (pSystemSpec == nullptr) return RESULT_BAD;

	m_pSystemSpec = pSystemSpec;

	return RESULT_GOOD;
}

UINT32 CDataManager::SetUserSetInfo(UserSetInfo_t *pUserSetInfo, UserLayerSet_t *pUserLayerSetInfo)
{
	if (pUserSetInfo == nullptr) return RESULT_BAD;
	if (pUserLayerSetInfo == nullptr) return RESULT_BAD;

	m_pUserSetInfo = pUserSetInfo;
	m_pUserLayerSetInfo = pUserLayerSetInfo;

	m_nSelectStep = m_pUserSetInfo->iStepIdx;

	return RESULT_GOOD;
}

void CDataManager::ClearData()
{
	m_nSelectStep = -1;
	m_ePrevOrient = Orient::OrientNone;
	m_ePrevMirrorDir = eDirectionType::DirectionTypeNone;

	m_stPanelInfo.Clear();
	m_stFovInfo.Clear();
	m_stCellInfo.Clear();
}

UINT32 CDataManager::Initialize(CJobFile* pCAMData, CDrawManager* pDrawManager)
{
	if (pCAMData == nullptr ||
		pDrawManager == nullptr)
		return RESULT_BAD;

	m_pJobData = pCAMData;
	m_pDrawManager = pDrawManager;

	return RESULT_GOOD;
}

void CDataManager::GetAngle(Orient ori, double &dAngle, bool &bMirror)
{
	dAngle = 0.f;
	bMirror = false;

	switch (ori)
	{
	case Orient::NoMir0Deg:		dAngle = 0.;	bMirror = false;	break;
	case Orient::Mir0Deg:		dAngle = 0.;	bMirror = true;		break;

	case Orient::NoMir45Deg:	dAngle = 45.;	bMirror = false;	break;
	case Orient::Mir45Deg:		dAngle = 45.;	bMirror = true;		break;

	case Orient::NoMir90Deg:	dAngle = 90.;	bMirror = false;	break;
	case Orient::Mir90Deg:		dAngle = 90.;	bMirror = true;		break;

	case Orient::NoMir135Deg:	dAngle = 135.;	bMirror = false;	break;
	case Orient::Mir135Deg:		dAngle = 135.;	bMirror = true;		break;

	case Orient::NoMir180Deg:	dAngle = 180.;	bMirror = false;	break;
	case Orient::Mir180Deg:		dAngle = 180.;	bMirror = true;		break;

	case Orient::NoMir225Deg:	dAngle = 225.;	bMirror = false;	break;
	case Orient::Mir225Deg:		dAngle = 225.;	bMirror = true;		break;

	case Orient::NoMir270Deg:	dAngle = 270.;	bMirror = false;	break;
	case Orient::Mir270Deg:		dAngle = 270.;	bMirror = true;		break;

	case Orient::NoMir315Deg:	dAngle = 315.;	bMirror = false;	break;
	case Orient::Mir315Deg:		dAngle = 315.;	bMirror = true;		break;
	}
}

void CDataManager::GetOrient(double dAngle, bool bMirror, Orient &ori)
{
	if (bMirror)
	{
		ori = Orient::Mir0Deg;

		if (dAngle == 45.0)  ori = Orient::Mir45Deg;
		else if (dAngle == 90.0)  ori = Orient::Mir90Deg;
		else if (dAngle == 135.0) ori = Orient::Mir135Deg;
		else if (dAngle == 180.0) ori = Orient::Mir180Deg;
		else if (dAngle == 225.0) ori = Orient::Mir225Deg;
		else if (dAngle == 270.0) ori = Orient::Mir270Deg;
		else if (dAngle == 315.0) ori = Orient::Mir315Deg;
	}
	else
	{
		ori = Orient::NoMir0Deg;

		if (dAngle == 45.0)  ori = Orient::NoMir45Deg;
		else if (dAngle == 90.0)  ori = Orient::NoMir90Deg;
		else if (dAngle == 135.0) ori = Orient::NoMir135Deg;
		else if (dAngle == 180.0) ori = Orient::NoMir180Deg;
		else if (dAngle == 225.0) ori = Orient::NoMir225Deg;
		else if (dAngle == 270.0) ori = Orient::NoMir270Deg;
		else if (dAngle == 315.0) ori = Orient::NoMir315Deg;
	}
}

Orient CDataManager::AddOrient(Orient ori1, Orient ori2)
{
	Orient oriRet = Orient::Mir0Deg;

	bool bMirror1 = false, bMirror2 = false;
	double dAngle1 = 0, dAngle2 = 0;

	GetAngle(ori1, dAngle1, bMirror1);
	GetAngle(ori2, dAngle2, bMirror2);

	dAngle1 += dAngle2;
	if (dAngle1 < 0)
		dAngle1 += 360.0;

	int iMult = static_cast<int>(dAngle1 / 360.0);
	dAngle1 -= (iMult * 360.0);

	bMirror1 |= bMirror2;

	GetOrient(dAngle1, bMirror1, oriRet);
	return oriRet;
}

Orient CDataManager::ReverseOrient(Orient ori)
{
	Orient ret = Orient::Mir0Deg;

	switch (ori)
	{
	case Orient::NoMir0Deg:		ret = Orient::NoMir0Deg;	break;
	case Orient::NoMir90Deg:	ret = Orient::NoMir270Deg;	break;
	case Orient::NoMir180Deg:	ret = Orient::NoMir180Deg;	break;
	case Orient::NoMir270Deg:	ret = Orient::NoMir90Deg;	break;
	case Orient::Mir0Deg:		ret = Orient::Mir0Deg;		break;
	case Orient::Mir90Deg:		ret = Orient::Mir270Deg;	break;
	case Orient::Mir180Deg:		ret = Orient::Mir180Deg;	break;
	case Orient::Mir270Deg:		ret = Orient::Mir90Deg;		break;
	}
	return ret;
}

UINT32 CDataManager::CheckStructure()
{
	if (m_pJobData == nullptr)			return RESULT_BAD;
	if (m_pUserLayerSetInfo == nullptr) return RESULT_BAD;
	if (m_pUserSetInfo == nullptr)		return RESULT_BAD;
	if (m_pSystemSpec == nullptr)		return RESULT_BAD;
	if (m_pDrawManager == nullptr)		return RESULT_BAD;

	return RESULT_GOOD;
}

std::map<INT32, std::vector<CString>>* CDataManager::GetLineWidth(int eOption)
{
	int nOption = static_cast<int>(eOption);
	if (nOption < 0) return nullptr;
	if (nOption >= static_cast<int>(eLineWidthOption::LineWidthOptionNone)) return nullptr;

	return &m_vecmapvecWidth[nOption];
}

UINT32	CDataManager::SetSortWidthData()
{
	//Sort Buffer Clear
	m_vecvecWidthSort.clear();
	m_vecvecWidthSort.resize(static_cast<int>(eLineWidthOption::LineWidthOptionNone));

	std::vector<  std::map<INT32, vector<CString>>> *pWidthPos = nullptr;
	int nSize;
	LINEWIDTH stLineWidth;
	
	pWidthPos =  &m_vecmapvecWidth;
	nSize = static_cast<int>(pWidthPos->size());
	if (nSize != static_cast<int>(eLineWidthOption::LineWidthOptionNone)) return RESULT_BAD;

	for (int i = 0 ; i < nSize ; i++ )
	{
		for (auto itStep : (*pWidthPos)[i])
		{
			stLineWidth.nStep = itStep.first;
			for (auto itData : itStep.second)
			{
				stLineWidth.strLineWidth = itData;

				m_vecvecWidthSort[i].push_back(stLineWidth);
			}
		}

		//중복된 데이터 삭제
		for (int j = 0; j < static_cast<int>(m_vecvecWidthSort[i].size()); j++)
		{
			CString strData = m_vecvecWidthSort[i][j].strLineWidth;
			for (int k = static_cast<int>(m_vecvecWidthSort[i].size()) - 1; k > j; k--)
			{
				if (m_vecvecWidthSort[i][k].strLineWidth.Compare(strData) == 0)
				{
					m_vecvecWidthSort[i].erase(m_vecvecWidthSort[i].begin() + k);
				}
			}
		}

		//정렬
		for (int j = 0; j < static_cast<int>(m_vecvecWidthSort[i].size()); j++)
		{
			for (int k = j + 1; k < static_cast<int>(m_vecvecWidthSort[i].size()); k++)
			{
				if (_ttof(m_vecvecWidthSort[i][j].strLineWidth) > _ttof(m_vecvecWidthSort[i][k].strLineWidth))
				{
					iter_swap(m_vecvecWidthSort[i].begin() + j, m_vecvecWidthSort[i].begin() + k);
				}
			}
		}

	}

	
	return RESULT_GOOD;

}


UINT32 CDataManager::SetRect_In_Panel(IN enum Orient eOrient, IN CString strUnitStepName, IN CRotMirrArr *pRotMirrArr, IN CSubStep *pSubStep, IN const MaskType &eMaskType, OUT vector<vector<RECTD>> &vecRect)
{
	vecRect.clear();

	bool bCreatedRotMirr = false;
	CRotMirrArr *rotMirrArr = nullptr;
	if (pRotMirrArr) rotMirrArr = pRotMirrArr;
	else
	{
		rotMirrArr = new CRotMirrArr;
		rotMirrArr->InputData(0.0, 0.0, eOrient, m_pUserSetInfo->mirrorDir);
		bCreatedRotMirr = true;
	}

	//Reverse User Orientation
	CRotMirrArr *rotUserMirrArr = new CRotMirrArr;

	Orient ori = Orient::Mir0Deg;
	GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, ori);
	rotUserMirrArr->InputData(0, 0, ReverseOrient(ori), m_pUserSetInfo->mirrorDir);

	CLayer *pPatternLayer = GetLayerInfo(m_pUserLayerSetInfo, pSubStep->m_pSubStep, PATTERN_LAYER_TXT);
	if (pPatternLayer == nullptr) return RESULT_BAD;

	CTypedPtrArray<CObArray, CAlignMask*> *pArrMask;
	if (eMaskType == MaskType::enumType_Align)
	{
		pArrMask = &pPatternLayer->m_arrAlign;
	}
	else
	{
		pArrMask = &pPatternLayer->m_arrMask;
	}

	RECTD tmpMinMaxRect;
	vector<RECTD> vcRect_temp;
	int nAlignMaskCount;
	double dTemp;
	nAlignMaskCount = static_cast<int>(pArrMask->GetCount());
	for (int nAlign = 0; nAlign < nAlignMaskCount; nAlign++)
	{
		CAlignMask *pAlignMask = pArrMask->GetAt(nAlign);

		vcRect_temp.clear();

		int nFeatureCount = static_cast<int>(pAlignMask->m_arrFeature.GetCount());
		for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
		{
			CFeature *pFeature = pAlignMask->m_arrFeature.GetAt(nfeno);
			if (pFeature == nullptr)continue;
			tmpMinMaxRect = pFeature->m_MinMaxRect;
			rotMirrArr->FinalPoint(&tmpMinMaxRect.left, &tmpMinMaxRect.top, tmpMinMaxRect.left, tmpMinMaxRect.top);
			rotMirrArr->FinalPoint(&tmpMinMaxRect.right, &tmpMinMaxRect.bottom, tmpMinMaxRect.right, tmpMinMaxRect.bottom);

			//Draw를 위하여 Reverser 해준다.
			rotUserMirrArr->FinalPoint(&tmpMinMaxRect.left, &tmpMinMaxRect.top, tmpMinMaxRect.left, tmpMinMaxRect.top);
			rotUserMirrArr->FinalPoint(&tmpMinMaxRect.right, &tmpMinMaxRect.bottom, tmpMinMaxRect.right, tmpMinMaxRect.bottom);

			if (m_pUserSetInfo->bMirror)
			{
				if (m_pUserSetInfo->dAngle == 0.0)
				{
					SWAP(tmpMinMaxRect.left, tmpMinMaxRect.right, dTemp);
					SWAP(tmpMinMaxRect.top, tmpMinMaxRect.bottom, dTemp);
				}
				else if (m_pUserSetInfo->dAngle == 90.0)
				{
					if (m_pUserSetInfo->mirrorDir != eDirectionType::eBoth)
					{
						tmpMinMaxRect.left *= -1.0;
						tmpMinMaxRect.right *= -1.0;

						SWAP(tmpMinMaxRect.top, tmpMinMaxRect.bottom, dTemp);
						tmpMinMaxRect.top *= -1.0;
						tmpMinMaxRect.bottom *= -1.0;
					}
				}
				else if (m_pUserSetInfo->dAngle == 270.0)
				{
					if (m_pUserSetInfo->mirrorDir != eDirectionType::eBoth)
					{
						SWAP(tmpMinMaxRect.left, tmpMinMaxRect.right, dTemp);
						tmpMinMaxRect.left *= -1.0;
						tmpMinMaxRect.right *= -1.0;

						tmpMinMaxRect.top *= -1.0;
						tmpMinMaxRect.bottom *= -1.0;
					}
				}
			}
			vcRect_temp.push_back(tmpMinMaxRect);

		}
		vecRect.push_back(vcRect_temp);
	}

	if (bCreatedRotMirr == true)
	{
		delete rotMirrArr;
		rotMirrArr = nullptr;
	}

	if (rotUserMirrArr != nullptr)
	{
		delete rotUserMirrArr;
		rotUserMirrArr = nullptr;
	}

	return RESULT_GOOD;
}

UINT32 CDataManager::SetRect(IN enum Orient eOrient, IN CString strUnitStepName, IN CRotMirrArr *pRotMirrArr, IN CSubStep *pSubStep, IN const MaskType &eMaskType, OUT vector<vector<RECTD>> &vecRect)
{
	vecRect.clear();

	bool bCreatedRotMirr = false;
	CRotMirrArr *rotMirrArr = nullptr;
	if (pRotMirrArr) rotMirrArr = pRotMirrArr;
	else
	{
		rotMirrArr = new CRotMirrArr;
		rotMirrArr->InputData(0.0, 0.0, eOrient, m_pUserSetInfo->mirrorDir);
		bCreatedRotMirr = true;
	}

	CLayer *pPatternLayer = GetLayerInfo(m_pUserLayerSetInfo, pSubStep->m_pSubStep, PATTERN_LAYER_TXT);
	if (pPatternLayer == nullptr) return RESULT_BAD;

	CTypedPtrArray<CObArray, CAlignMask*> *pArrMask;
	if (eMaskType == MaskType::enumType_Align)
	{
		pArrMask = &pPatternLayer->m_arrAlign;
	}
	else
	{
		pArrMask = &pPatternLayer->m_arrMask;
	}

	RECTD tmpMinMaxRect;
	vector<RECTD> vcRect_temp;
	int  nAlignMaskCount = static_cast<int>(pArrMask->GetCount());
	for (int nAlign = 0; nAlign < nAlignMaskCount; nAlign++)
	{
		CAlignMask *pAlignMask = pArrMask->GetAt(nAlign);

		vcRect_temp.clear();

		int nFeatureCount = static_cast<int>(pAlignMask->m_arrFeature.GetCount());
		for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
		{
			CFeature *pFeature = pAlignMask->m_arrFeature.GetAt(nfeno);
			if (pFeature == nullptr)continue;
			tmpMinMaxRect = pFeature->m_MinMaxRect;
			rotMirrArr->FinalPoint(&tmpMinMaxRect.left, &tmpMinMaxRect.top, tmpMinMaxRect.left, tmpMinMaxRect.top);
			rotMirrArr->FinalPoint(&tmpMinMaxRect.right, &tmpMinMaxRect.bottom, tmpMinMaxRect.right, tmpMinMaxRect.bottom);

			vcRect_temp.push_back(tmpMinMaxRect);
		}

		vecRect.push_back(vcRect_temp);

	}

	if (bCreatedRotMirr == true)
	{
		delete rotMirrArr;
		rotMirrArr = nullptr;
	}

	return RESULT_GOOD;
}

UINT32 CDataManager::EstimateMaskInfo_in_Cell_V2()
{

	return RESULT_GOOD;
}


UINT32 CDataManager::EstimateAlignInfo_in_Cell_V2()
{
	m_stCellInfo.vcAlign_in_Cell.clear();

	if (m_stCellInfo.vcCellData.size() <= 0)
		return RESULT_BAD;

	int iSwathNum = static_cast<int>(m_stFovInfo.stBasic.swathNum);
	int iCellCol = static_cast<int>(m_stCellInfo.stBasic.cellNum.cx) / iSwathNum;
	int iCellRow = static_cast<int>(m_stCellInfo.stBasic.cellNum.cy);
	if (iSwathNum <= 0 || iCellCol <= 0 || iCellRow <= 0)
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

	CString str;
	for (auto itCell : m_stCellInfo.vcCellData)
	{
		int iCell = itCell->swathIdx * (iCellCol * iCellRow) + (iCellCol * itCell->fovIdx.y) + itCell->fovIdx.x;

		RECTD drtCell = itCell->rcRectMM_In_Panel;// itCell->rcRectMM;
		drtCell.NormalizeRectD();

		RECTD drtTemp;

		vector<CellAlignInfo_t> vecAlignInfo;
		CellAlignInfo_t stAlignInfo;

		auto itStripInfo = m_stCellInfo.vcStrip_in_Cell.find(iCell);
		if (itStripInfo == m_stCellInfo.vcStrip_in_Cell.end()) continue;

		auto itUnitInfo = m_stCellInfo.vcUnit_in_Cell.find(iCell);
		if (itUnitInfo == m_stCellInfo.vcUnit_in_Cell.end()) continue;

		std::vector<StripInfo_t*> vcStripInfo = itStripInfo->second;
		int nStripInfoCount = static_cast<int>(vcStripInfo.size());

		std::vector<UnitInfo_t*> vcUnitInfo = itUnitInfo->second;
		int nUnitInfoCount = static_cast<int>(vcUnitInfo.size());

		int nStripPatternCount = 0;
		int nUnitPatternCount = 0;
		if (nStripInfoCount > 0)
		{
			nStripPatternCount = static_cast<int>(vcStripInfo[0]->vcAlign.size());
		}
		if (nUnitInfoCount > 0)
		{
			nUnitPatternCount = static_cast<int>(vcUnitInfo[0]->vcAlign.size());
		}

		int nAlignPatternCount = nStripPatternCount + nUnitPatternCount;

		vecAlignInfo.resize(nAlignPatternCount);

		{
			//Strip
			for (int nAlign = 0; nAlign < nStripPatternCount; nAlign++)
			{
				stAlignInfo.Clear();
				int nRegionCount = static_cast<int>(vcStripInfo[0]->vcAlign[nAlign].size());
				for (int nRegion = 0; nRegion < nRegionCount; nRegion++)
				{

					for (int nUnitInfo = 0; nUnitInfo < nStripInfoCount; nUnitInfo++)
					{
						drtTemp = vcStripInfo[nUnitInfo]->vcAlign[nAlign][nRegion];
						drtTemp.NormalizeRectD();

						//Align 영역전체가 포함되어 있는지 체크
						if (drtCell.IsRectInRectD(drtTemp) == FALSE) continue;

						GetRectInfo_In_Cell(drtTemp, drtCell, ori, m_pUserSetInfo, dResolMM, stAlignInfo);
					}

					vecAlignInfo[nAlign] = stAlignInfo;
				}
			}

		}

		//Unit
		{

			for (int nAlign = 0; nAlign < nUnitPatternCount; nAlign++)
			{
				stAlignInfo.Clear();
				int nRegionCount = static_cast<int>(vcUnitInfo[0]->vcAlign[nAlign].size());
				for (int nRegion = 0; nRegion < nRegionCount; nRegion++)
				{

					for (int nUnitInfo = 0; nUnitInfo < nUnitInfoCount; nUnitInfo++)
					{
						drtTemp = vcUnitInfo[nUnitInfo]->vcAlign[nAlign][nRegion];
						drtTemp.NormalizeRectD();

						//Align 영역전체가 포함되어 있는지 체크
						if (drtCell.IsRectInRectD(drtTemp) == FALSE) continue;

						GetRectInfo_In_Cell(drtTemp, drtCell, ori, m_pUserSetInfo, dResolMM, stAlignInfo);
					}
				}

				vecAlignInfo[nAlign + nStripPatternCount] = stAlignInfo;
			}
		}


		m_stCellInfo.vcAlign_in_Cell.insert(std::make_pair(iCell, vecAlignInfo));
	}//End Cell

	return RESULT_GOOD;
}


UINT32 CDataManager::GetRectInfo_In_Cell(IN const RECTD &drtInPanel, IN RECTD &drtCell, const Orient emOrient,
	IN const UserSetInfo_t	*pUserSetInfo, IN const double &dRedolution_mm, OUT CellAlignInfo_t &stAlignInfo)
{
	if (pUserSetInfo == nullptr) return RESULT_BAD;

	double dTemp;
	RECTD drtTemp = drtInPanel;
	RECTD drtPixel;
	CRotMirrArr rotMirrArr;
	rotMirrArr.InputData(0, 0, ReverseOrient(emOrient), m_pUserSetInfo->mirrorDir);

	RECTD rcAlignRect;
	rotMirrArr.FinalPoint(&rcAlignRect.left, &rcAlignRect.top, drtTemp.left, drtTemp.top);
	rotMirrArr.FinalPoint(&rcAlignRect.right, &rcAlignRect.bottom, drtTemp.right, drtTemp.bottom);
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
			if (m_pUserSetInfo->mirrorDir != eDirectionType::eBoth)
			{
				rcAlignRect.left *= -1.0;
				rcAlignRect.right *= -1.0;

				SWAP(rcAlignRect.top, rcAlignRect.bottom, dTemp);
				rcAlignRect.top *= -1.0;
				rcAlignRect.bottom *= -1.0;
			}
		}
		else if (m_pUserSetInfo->dAngle == 270.0)
		{
			if (m_pUserSetInfo->mirrorDir != eDirectionType::eBoth)
			{
				SWAP(rcAlignRect.left, rcAlignRect.right, dTemp);
				rcAlignRect.left *= -1.0;
				rcAlignRect.right *= -1.0;

				rcAlignRect.top *= -1.0;
				rcAlignRect.bottom *= -1.0;
			}
		}
	}

	stAlignInfo.vcInPanel_mm.push_back(rcAlignRect);

	drtTemp = drtInPanel;
	drtTemp.OffsetRect(-1.0*drtCell.left, -1.0*drtCell.top);
	drtTemp.NormalizeRectD();

	stAlignInfo.vcInCell_mm.push_back(drtTemp);

	//Pixel좌표로 변환//Y Flip
	drtPixel = drtTemp;
	drtPixel.top = drtCell.height() - drtTemp.bottom;
	drtPixel.bottom = drtCell.height() - drtTemp.top;
	drtPixel.ScaleRect(1. / dRedolution_mm, 1. / dRedolution_mm);

	RECT rtPixel;
	rtPixel.left = static_cast<int>(drtPixel.left + 0.5);
	rtPixel.top = static_cast<int>(drtPixel.top + 0.5);
	rtPixel.right = static_cast<int>(drtPixel.right + 0.5);
	rtPixel.bottom = static_cast<int>(drtPixel.bottom + 0.5);

	stAlignInfo.vcInCell_Pixel.push_back(rtPixel);

	return RESULT_GOOD;
}


CLayer *CDataManager::GetLayerInfo(UserLayerSet_t *pUserLayerSetInfo, CStep* pStep, CString strDefine)
{
	if (pStep == nullptr) return nullptr;
	if (pUserLayerSetInfo == nullptr) return nullptr;
	if (strDefine.GetLength() < 1) return nullptr;

	CString strLayer = _T("");
	for (auto pLayerSet : pUserLayerSetInfo->vcLayerSet)
	{
		if (pLayerSet->strDefine == strDefine)
		{
			strLayer = pLayerSet->strLayer;
			break;
		}
	}

	if (strLayer == _T("")) return nullptr;

	return GetLayer(pStep->m_arrLayer, strLayer);
}

CLayer *CDataManager::GetLayer(IN CTypedPtrArray <CObArray, CLayer*> &arrLayer, IN const CString &strLayer)
{
	CLayer *pLayer = nullptr;

	pLayer = nullptr;
	int nLayerCount = static_cast<int>(arrLayer.GetCount());
	for (int nLayer = 0; nLayer < nLayerCount; nLayer++)
	{
		CLayer *pTempLayer = arrLayer.GetAt(nLayer);
		if (pTempLayer == nullptr) continue;

		if (pTempLayer->m_strLayerName == strLayer)
		{
			pLayer = pTempLayer;
			break;
		}
	}

	return pLayer;
}

RECTD CDataManager::GetDrawRect(IN const RECTD &drtOutput, UserSetInfo_t *pUserSetInfo)
{
	if (pUserSetInfo == nullptr) return RECTD();

	Orient ori = Orient::NoMir0Deg;

	double dAngle = pUserSetInfo->dAngle;
	bool bMirror = pUserSetInfo->bMirror;

	CDataManager::GetOrient(dAngle, bMirror, ori);

	CRotMirrArr rotMirrArr;
	rotMirrArr.InputData(0, 0, CDataManager::ReverseOrient(ori), pUserSetInfo->mirrorDir);
	
	RECTD drtDraw = drtOutput;
	
	rotMirrArr.FinalPoint(&drtDraw.left, &drtDraw.top, drtDraw.left, drtDraw.top);
	rotMirrArr.FinalPoint(&drtDraw.right, &drtDraw.bottom, drtDraw.right, drtDraw.bottom);

	if (pUserSetInfo != nullptr)
	{
		double dAngle = pUserSetInfo->dAngle;
		bool bMirror = pUserSetInfo->bMirror;

		if (bMirror)
		{
			if (pUserSetInfo->mirrorDir == eDirectionType::eHorizontal)
			{
				if (dAngle == 90.0 || dAngle == 270.0)
				{
					drtDraw.left *= -1.0f;
					drtDraw.top *= -1.0f;
					drtDraw.right *= -1.0f;
					drtDraw.bottom *= -1.0f;
				}
			}
			else if (pUserSetInfo->mirrorDir == eDirectionType::eVertical)
			{
				if (dAngle == 90.0 || dAngle == 270.0)
				{
					if (dAngle == 90.0 || dAngle == 270.0)
					{
						drtDraw.left *= -1.0f;
						drtDraw.top *= -1.0f;
						drtDraw.right *= -1.0f;
						drtDraw.bottom *= -1.0f;
					}
				}
			}
		}

	}

	drtDraw.NormalizeRectD();

	return drtDraw;
}