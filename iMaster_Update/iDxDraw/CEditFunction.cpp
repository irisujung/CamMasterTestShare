#include "pch.h"
#include "CEditFunction.h"

CEditFunction::CEditFunction()
{

}

CEditFunction::~CEditFunction()
{
}


UINT32 CEditFunction::SetDelete(IN CJobFile *pJobFile, IN const int &nStep, IN const UserLayerSet_t* pUserLayerSet)
{
	if (pJobFile == nullptr) return RESULT_BAD;
	if (pUserLayerSet == nullptr) return RESULT_BAD;
	if (nStep < 0) return RESULT_BAD;
	if (nStep >= static_cast<int>(pJobFile->m_arrStep.GetCount())) return RESULT_BAD;

	CStep* pStep = pJobFile->m_arrStep.GetAt(nStep);
	if (pStep == nullptr) return RESULT_BAD;

	int nLayerCount = static_cast<int>(pStep->m_arrLayer.GetCount());
	int nUserLayerCount = static_cast<int>(pUserLayerSet->vcLayerSet.size());
	for (int i = 0; i < nUserLayerCount; i++)
	{
		//Viewer에 보이는 것만 체크
		if (pUserLayerSet->vcLayerSet[i]->bCheck == false) continue;

		//Viwer에 보이는 레이어와 같은 Layer 선택
		for (int nLayer = 0; nLayer < nLayerCount; nLayer++)
		{
			CLayer* pLayer = pStep->m_arrLayer.GetAt(nLayer);
			if ( pLayer == nullptr ) continue;

			//Viwer에 보이는 레이어와 같은 Layer 선택
			if ( pUserLayerSet->vcLayerSet[i]->strLayer != pLayer->m_strLayerName ) continue;

			int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());
			for (int nfeno = nFeatureCount-1; nfeno >=0 ; nfeno--)
			{
				SetDeleteFeature(pLayer->m_FeatureFile.m_arrFeature, nfeno);

			}

			//이름이 같은것을 처리했으니까. step Layer loop는 탈출
			break;

		}

	}

	return RESULT_GOOD;
}


UINT32 CEditFunction::SetDelete_Mask(IN const MaskType &eMaskType, IN CJobFile *pJobFile, IN const int &nStep, IN const UserLayerSet_t* pUserLayerSet)
{
	if (pJobFile == nullptr) return RESULT_BAD;
	if (pUserLayerSet == nullptr) return RESULT_BAD;
	if (nStep < 0) return RESULT_BAD;
	if (nStep >= static_cast<int>(pJobFile->m_arrStep.GetCount())) return RESULT_BAD;

	CStep* pStep = pJobFile->m_arrStep.GetAt(nStep);
	if (pStep == nullptr) return RESULT_BAD;

	int nLayerCount = static_cast<int>(pStep->m_arrLayer.GetCount());
	int nUserLayerCount = static_cast<int>(pUserLayerSet->vcLayerSet.size());
	for (int i = 0; i < nUserLayerCount; i++)
	{
		//Viewer에 보이는 것만 체크
		if (pUserLayerSet->vcLayerSet[i]->bCheck == false) continue;

		//Viwer에 보이는 레이어와 같은 Layer 선택
		for (int nLayer = 0; nLayer < nLayerCount; nLayer++)
		{
			CLayer* pLayer = pStep->m_arrLayer.GetAt(nLayer);
			if (pLayer == nullptr) continue;

			//Viwer에 보이는 레이어와 같은 Layer 선택
			if (pUserLayerSet->vcLayerSet[i]->strLayer != pLayer->m_strLayerName) continue;

			if (eMaskType == MaskType::enumType_Align)
			{
				int nAlignMaskCount = static_cast<int>(pLayer->m_arrAlign.GetCount());
				for (int nAlignMask = nAlignMaskCount - 1; nAlignMask >= 0; nAlignMask--)
				{
					CAlignMask *pAlignMask = pLayer->m_arrAlign.GetAt(nAlignMask);
					int nFeatureCount = static_cast<int>(pAlignMask->m_arrFeature.GetCount());
					for (int nfeno = nFeatureCount - 1; nfeno >= 0; nfeno--)
					{
						SetDeleteFeature(pAlignMask->m_arrFeature, nfeno);
					}
					//남아있는 Feature 갯수가 0이라면 배열 삭제
					nFeatureCount = static_cast<int>(pAlignMask->m_arrFeature.GetCount());
					if (nFeatureCount == 0)
					{
						pLayer->m_arrAlign.RemoveAt(nAlignMask);
					}
				}
			}
			else if (eMaskType == MaskType::enumType_Mask)
			{
				int nAlignMaskCount = static_cast<int>(pLayer->m_arrMask.GetCount());
				for (int nAlignMask = nAlignMaskCount - 1; nAlignMask >= 0; nAlignMask--)
				{
					CAlignMask *pAlignMask = pLayer->m_arrMask.GetAt(nAlignMask);
					int nFeatureCount = static_cast<int>(pAlignMask->m_arrFeature.GetCount());
					for (int nfeno = nFeatureCount - 1; nfeno >= 0; nfeno--)
					{
						SetDeleteFeature(pAlignMask->m_arrFeature, nfeno);
					}
					//남아있는 Feature 갯수가 0이라면 배열 삭제
					nFeatureCount = static_cast<int>(pAlignMask->m_arrFeature.GetCount());
					if (nFeatureCount == 0)
					{
						pLayer->m_arrMask.RemoveAt(nAlignMask);
					}
				}
			}
		}

	}

	return RESULT_GOOD;
}

UINT32 CEditFunction::SetDeleteFeature(IN CTypedPtrArray <CObArray, CFeature*> &arrFeature, IN const int &nIndex)
{
	CFeature *pFeature = arrFeature.GetAt(nIndex);
	if (pFeature == nullptr) return RESULT_BAD;

	switch (pFeature->m_eType)
	{
	case FeatureType::Z:
	{
		CFeatureZ *pFeatureZ = (CFeatureZ*)pFeature;
		if (pFeatureZ->m_bHighlight == true)
		{
			delete arrFeature[nIndex];
			arrFeature.RemoveAt(nIndex);
		}
	}
	break;
	case FeatureType::L:
	{
		CFeatureL *pFeatureL = (CFeatureL*)pFeature;
		if (pFeatureL->m_bHighlight == true)
		{
			delete arrFeature[nIndex];
			arrFeature.RemoveAt(nIndex);
		}
	}
	break;
	case FeatureType::P:
	{
		CFeatureP *pFeatureP = (CFeatureP*)pFeature;
		if (pFeatureP->m_bHighlight == true)
		{
			delete arrFeature[nIndex];
			arrFeature.RemoveAt(nIndex);
		}
	}
	break;
	case FeatureType::A:
	{
		CFeatureA *pFeatureA = (CFeatureA*)pFeature;
		if (pFeatureA->m_bHighlight == true)
		{
			delete arrFeature[nIndex];
			arrFeature.RemoveAt(nIndex);
		}
	}
	break;
	case FeatureType::T:
	{
		CFeatureT *pFeatureT = (CFeatureT*)pFeature;
		if (pFeatureT->m_bHighlight == true)
		{
			delete arrFeature[nIndex];
			arrFeature.RemoveAt(nIndex);
		}
	}
	break;
	case FeatureType::S:
	{
		CFeatureS *pFeatureS = (CFeatureS*)pFeature;

		int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
		for (int nObOe = nObOeCount - 1; nObOe >= 0; nObOe--)
		{
			CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nObOe);
			if (pObOe == nullptr) continue;

			if (pObOe->m_bHighlight == true)
			{
				delete pFeatureS->m_arrObOe[nObOe];
				pFeatureS->m_arrObOe.RemoveAt(nObOe);
			}
		}

		//ObOe 삭제 후 버퍼 갯수가 0이라면 Feature 삭제
		nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
		if (nObOeCount == 0)
		{
			delete arrFeature[nIndex];
			arrFeature.RemoveAt(nIndex);
		}

	}
	break;
	default:
		break;
	}

	return RESULT_GOOD;
}

UINT32 CEditFunction::SetFeature_Add(vector<CFeatureData*>& vecFeatureData, CStep *pStep)
{
	int nDataSize = static_cast<int>(vecFeatureData.size());

	if (nDataSize <= 0) return RESULT_BAD;
	if (pStep == nullptr) return RESULT_BAD;

	for (int nfeno = 0; nfeno < nDataSize; nfeno++)
	{
		CFeatureData *pFeatureData = vecFeatureData[nfeno];

		CLayer *pLayer = pStep->m_arrLayer.GetAt(pFeatureData->m_nLayer);
		if (pLayer == nullptr) continue;

		if (pFeatureData->m_pFeature == nullptr) continue;

		CFeature *pFeature = nullptr;
		switch (pFeatureData->m_pFeature->m_eType)
		{
		case FeatureType::Z:
		{
			pFeature = new CFeatureZ();
			*pFeature = *pFeatureData->m_pFeature;
			*(CFeatureZ*)pFeature = *(CFeatureZ*)pFeatureData->m_pFeature;

			pFeature->SetMinMaxRect();

			pLayer->m_FeatureFile.m_arrFeature.Add(pFeature);

		}
		break;

		case FeatureType::P:
		{
			pFeature = new CFeatureP();
			*pFeature = *pFeatureData->m_pFeature;
			*(CFeatureP*)pFeature = *(CFeatureP*)pFeatureData->m_pFeature;

			pFeature->SetMinMaxRect();

			//Feature 추가
			pLayer->m_FeatureFile.m_arrFeature.Add(pFeature);
		}
		break;
		case FeatureType::L:
		{
			pFeature = new CFeatureL();
			*pFeature = *pFeatureData->m_pFeature;
			*(CFeatureL*)pFeature = *(CFeatureL*)pFeatureData->m_pFeature;

			pFeature->SetMinMaxRect();

			pLayer->m_FeatureFile.m_arrFeature.Add(pFeature);
		}
		break;
		case FeatureType::A:
		{
			pFeature = new CFeatureA();
			*pFeature = *pFeatureData->m_pFeature;
			*(CFeatureA*)pFeature = *(CFeatureA*)pFeatureData->m_pFeature;

			pFeature->SetMinMaxRect();

			pLayer->m_FeatureFile.m_arrFeature.Add(pFeature);
		}
		break;

		case FeatureType::S:
		{

			//O번째 ObOe가 nullptr 인지 확인,
			//nullptr이라면, 기존 Surface내에 신규 ObOe
			//nullptr이 아니라면, 신규 Surface 생성
			CFeatureS *pFeatureS_Src = (CFeatureS*)pFeatureData->m_pFeature;
			int nObOeCount = static_cast<int>(pFeatureS_Src->m_arrObOe.GetCount());
			if (nObOeCount == 0) continue;

			if (pFeatureS_Src->m_arrObOe.GetAt(0) == nullptr)
			{//기존 Surface내에 신규 ObOe
				CFeature *pFeature_Dst = pLayer->m_FeatureFile.m_arrFeature.GetAt(pFeatureData->m_nFeatureIndex);
				if (pFeature_Dst == nullptr) continue;
				CFeatureS *pFeatureS = (CFeatureS*)pFeature_Dst;

				for (int i = 0; i < nObOeCount; i++)
				{
					CObOe *pObOe_Src = pFeatureS_Src->m_arrObOe.GetAt(i);
					if (pObOe_Src == nullptr) continue;

					CObOe *pObOe = new CObOe();
					if (pObOe == nullptr) continue;
					*pObOe = *pObOe_Src;

					int nOsOcCount = static_cast<int>(pObOe_Src->m_arrOsOc.GetCount());
					for (int j = 0; j < nOsOcCount; j++)
					{
						COsOc *pOsOc_Src = pObOe_Src->m_arrOsOc.GetAt(j);
						if (pOsOc_Src == nullptr) continue;

						COsOc *pOsOc = nullptr;
						if (pOsOc_Src->m_bOsOc == true)
						{
							pOsOc = new COs();
						}
						else
						{
							pOsOc = new COc();
						}

						*pOsOc = *pOsOc_Src;

						//Arc 인경우 
						if (pOsOc_Src->m_bOsOc == false)
						{
							*((COc*)pOsOc) = *((COc*)pOsOc_Src);
						}

						pObOe->m_arrOsOc.Add(pOsOc);
					}

					pFeatureS->m_arrObOe.Add(pObOe);
				}

				pFeature = nullptr;
			}
			else
			{//신규 Surface 생성
				pFeature = new CFeatureS();

				*pFeature = *pFeatureData->m_pFeature;

				CFeatureS *pFeatureS = (CFeatureS*)pFeature;

				for (int i = 0; i < nObOeCount; i++)
				{
					CObOe *pObOe_Src = pFeatureS_Src->m_arrObOe.GetAt(i);
					if (pObOe_Src == nullptr) continue;

					CObOe *pObOe = new CObOe();
					if (pObOe == nullptr) continue;
					*pObOe = *pObOe_Src;

					int nOsOcCount = static_cast<int>(pObOe_Src->m_arrOsOc.GetCount());
					for (int j = 0; j < nOsOcCount; j++)
					{
						COsOc *pOsOc_Src = pObOe_Src->m_arrOsOc.GetAt(j);
						if (pOsOc_Src == nullptr) continue;

						COsOc *pOsOc = nullptr;
						if (pOsOc_Src->m_bOsOc == true)
						{
							pOsOc = new COs();
						}
						else
						{
							pOsOc = new COc();
						}

						*pOsOc = *pOsOc_Src;

						//Arc 인경우 
						if (pOsOc_Src->m_bOsOc == false)
						{
							*((COc*)pOsOc) = *((COc*)pOsOc_Src);
						}

						pObOe->m_arrOsOc.Add(pOsOc);
					}

					pFeatureS->m_arrObOe.Add(pObOe);
				}

				pFeature->SetMinMaxRect();
				pLayer->m_FeatureFile.m_arrFeature.Add(pFeature);
			}


		}
		break;

		}

		if (pFeature == nullptr) continue;

		if (pFeatureData->m_pFeature->m_pSymbol != nullptr)
		{
			CSymbol *pSymbol_New = new CSymbol();
			*pSymbol_New = *pFeatureData->m_pFeature->m_pSymbol;
			pSymbol_New->m_vcParams = pFeatureData->m_pFeature->m_pSymbol->m_vcParams;

			int nSymbolIndex = 0;
			//추가하는 Symbol이 같은게 있는지 체크
			BOOL bCompare = FALSE;
			int nSymbolCount = static_cast<int>(pLayer->m_FeatureFile.m_arrSymbol.GetCount());
			CSymbol *pSymbol = nullptr;
			for (int i = 0; i < nSymbolCount; i++)
			{
				CSymbol *pSymbolTemp = pLayer->m_FeatureFile.m_arrSymbol.GetAt(i);

				if (*pSymbolTemp == *pSymbol_New)
				{
					pSymbol = pSymbolTemp;
					bCompare = TRUE;
					break;
				}
			}

			if (bCompare == TRUE)
			{//같은 Symbol이 있을 경우 
				delete pSymbol_New;
				pSymbol_New = nullptr;

				pFeature->m_pSymbol = pSymbol;

				nSymbolIndex = pSymbol->m_iSerialNum;
			}
			else
			{//같은 Symbol이 없을 경우 
				int nMaxIndex = static_cast<int>(pLayer->m_FeatureFile.m_arrSymbol.GetCount());

				pSymbol_New->SetMinMaxRect();

				pFeature->m_pSymbol = pSymbol_New;
				//Symbol num : 새로 Add 할것이기 때문에 MaxIndex를 넣어준다.
				pFeature->m_pSymbol->m_iSerialNum = nMaxIndex;
				nSymbolIndex = nMaxIndex;

				pLayer->m_FeatureFile.m_arrSymbol.Add(pSymbol_New);

			}
			//Symbol Index 업데이트
			switch (pFeature->m_eType)
			{
			case FeatureType::Z:
			{
				CFeatureZ* pFeatureZ = (CFeatureZ*)pFeature;
				pFeatureZ->m_nSymNum = nSymbolIndex;
			}
			break;
			case FeatureType::P:
			{
				CFeatureP* pFeatureP = (CFeatureP*)pFeature;
				pFeatureP->m_nSymNum = nSymbolIndex;
			}
			break;
			case FeatureType::L:
			{
				CFeatureL* pFeatureL = (CFeatureL*)pFeature;
				pFeatureL->m_nSymNum = nSymbolIndex;
			}
			break;
			case FeatureType::A:
				CFeatureA* pFeatureA = (CFeatureA*)pFeature;
				pFeatureA->m_nSymNum = nSymbolIndex;
			}
		}

		pFeature->m_arrAttribute.RemoveAll();
		//Att Name 추가
		int nAttNameCount = static_cast<int>(pFeatureData->m_pFeature->m_arrAttribute.GetCount());
		for (int nAttName = 0; nAttName < nAttNameCount; nAttName++)
		{
			CAttribute *pAttName_New = new CAttribute();
			CAttribute *pAttName = pFeatureData->m_pFeature->m_arrAttribute.GetAt(nAttName);

			*pAttName_New = *pAttName;

			//추가하는 Attribute이 같은게 있는지 체크
			BOOL bCompare = FALSE;
			int nSymbolCount = static_cast<int>(pLayer->m_FeatureFile.m_arrAttribute.GetCount());
			pAttName = nullptr;
			for (int i = 0; i < nSymbolCount; i++)
			{
				CAttribute *pAttNameTemp = pLayer->m_FeatureFile.m_arrAttribute.GetAt(i);

				if (pAttNameTemp->m_strName == pAttName_New->m_strName)
				{
					pAttName = pAttNameTemp;
					bCompare = TRUE;
					break;
				}
			}

			if (bCompare == TRUE)
			{//같은 Att이 있을 경우 
				delete pAttName_New;
				pAttName_New = nullptr;

				pFeature->m_arrAttribute.Add(pAttName);
			}
			else
			{//같은 Att가 없을 경우 
				int nMaxIndex = static_cast<int>(pLayer->m_FeatureFile.m_arrAttribute.GetCount());

				pFeature->m_arrAttribute.Add(pAttName_New);

				//Symbol num : 새로 Add 할것이기 때문에 MaxIndex를 넣어준다.
				int nIndex = static_cast<int>(pFeature->m_arrAttribute.GetCount()) - 1;
				pFeature->m_arrAttribute[nIndex]->m_iSerialNum = nMaxIndex;

				pLayer->m_FeatureFile.m_arrAttribute.Add(pAttName_New);
			}
		}

		pFeature->m_arrAttributeTextString.RemoveAll();
		//Att String 추가
		int nAttStringCount = static_cast<int>(pFeatureData->m_pFeature->m_arrAttributeTextString.GetCount());
		for (int nAttString = 0; nAttString < nAttStringCount; nAttString++)
		{
			CAttributeTextString *pAttString_New = new CAttributeTextString();
			CAttributeTextString *pAttString = pFeatureData->m_pFeature->m_arrAttributeTextString.GetAt(nAttString);

			*pAttString_New = *pAttString;

			//추가하는 Attribute이 같은게 있는지 체크
			BOOL bCompare = FALSE;
			int nSymbolCount = static_cast<int>(pLayer->m_FeatureFile.m_arrAttributeTextString.GetCount());
			pAttString = nullptr;
			for (int i = 0; i < nSymbolCount; i++)
			{
				CAttributeTextString *pAttStringTemp = pLayer->m_FeatureFile.m_arrAttributeTextString.GetAt(i);

				if (pAttStringTemp->m_strText == pAttString_New->m_strText)
				{
					pAttString = pAttStringTemp;
					bCompare = TRUE;
					break;
				}
			}

			if (bCompare == TRUE)
			{//같은 Symbol이 있을 경우 
				delete pAttString_New;
				pAttString_New = nullptr;

				pFeature->m_arrAttributeTextString.Add(pAttString);
			}
			else
			{//같은 Symbol이 없을 경우 
				int nMaxIndex = static_cast<int>(pLayer->m_FeatureFile.m_arrAttributeTextString.GetCount());

				pFeature->m_arrAttributeTextString.Add(pAttString_New);

				//Symbol num : 새로 Add 할것이기 때문에 MaxIndex를 넣어준다.
				int nIndex = static_cast<int>(pFeature->m_arrAttributeTextString.GetCount()) - 1;
				pFeature->m_arrAttributeTextString[nIndex]->m_iSerialNum = nMaxIndex;

				pLayer->m_FeatureFile.m_arrAttributeTextString.Add(pAttString_New);
			}
		}

	}

	return RESULT_GOOD;
}



UINT32 CEditFunction::SetFeature_AlignAdd(vector<CFeatureData*>& vecFeatureData, CStep *pStep)
{
	int nDataSize = static_cast<int>(vecFeatureData.size());

	if (nDataSize <= 0) return RESULT_BAD;
	if (pStep == nullptr) return RESULT_BAD;


	CLayer *pLayer = nullptr;
	for (int nfeno = 0; nfeno < nDataSize; nfeno++)
	{
		CAlignMask *pAlignMask = new CAlignMask;
		CFeatureData *pFeatureData = vecFeatureData[nfeno];
		if (pFeatureData->m_pFeature == nullptr) continue;

		CFeature *pFeature = nullptr;
		switch (pFeatureData->m_pFeature->m_eType)
		{
		case FeatureType::P:
		{
			pFeature = new CFeatureP();
			*pFeature = *pFeatureData->m_pFeature;
			*(CFeatureP*)pFeature = *(CFeatureP*)pFeatureData->m_pFeature;

			pFeature->SetMinMaxRect();

			pAlignMask->m_arrFeature.Add(pFeature);
			
			//Add 확인용
			pLayer = pStep->m_arrLayer.GetAt(pFeatureData->m_nLayer);
			if (pLayer == nullptr) continue;

			break;
		}
		default:
			break;
		}

		if (pFeature == nullptr) continue;

		CSymbol *pSymbol_New = new CSymbol();
		*pSymbol_New = *pFeatureData->m_pFeature->m_pSymbol;
		pSymbol_New->m_vcParams = pFeatureData->m_pFeature->m_pSymbol->m_vcParams;

		pSymbol_New->SetMinMaxRect();

		pFeature->m_pSymbol = pSymbol_New;
		pFeature->SetMinMaxRect();

		pFeature->m_arrAttribute.RemoveAll();
		pFeature->m_arrAttributeTextString.RemoveAll();
	
		//Align Feature 추가	
		pLayer->m_arrAlign.Add(pAlignMask);
	}	

	return RESULT_GOOD;
}