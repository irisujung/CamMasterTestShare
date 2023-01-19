#include "pch.h"
#include "CDrawHighlightShowFunc.h"
#include "CDrawFunction.h"

CDrawHighlightShowFunc::CDrawHighlightShowFunc()
{

}

CDrawHighlightShowFunc::~CDrawHighlightShowFunc()
{

}

//Feature Select
UINT32			CDrawHighlightShowFunc::ResetSelect(IN CJobFile *pJobFile)
{
	if (pJobFile == nullptr) return RESULT_BAD;

	int nStepCount = static_cast<int>(pJobFile->m_arrStep.GetCount());
	for (int nStep = 0; nStep < nStepCount; nStep++)
	{
		CStep *pStep = pJobFile->m_arrStep.GetAt(nStep);
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
				case FeatureType::Z:
				{
					((CFeatureZ*)pFeature)->m_bHighlight = false;
				}
				break;

				case FeatureType::L:		//Line
					((CFeatureL*)pFeature)->m_bHighlight = false;
					break;
				case FeatureType::P:		//Pad
					((CFeatureP*)pFeature)->m_bHighlight = false;

					break;
				case FeatureType::A:		//Arc								
					((CFeatureA*)pFeature)->m_bHighlight = false;
					break;
				case FeatureType::T:		//Text			
					((CFeatureT*)pFeature)->m_bHighlight = false;
					break;
				case FeatureType::S:		//Surface	
				{
					CFeatureS *pFeatureS = (CFeatureS*)pFeature;
					int nSurfaceCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
					for (int nSurface = 0; nSurface < nSurfaceCount; nSurface++)
					{
						CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nSurface);
						if (pObOe == nullptr) continue;

						pObOe->m_bHighlight = false;

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

			int nAlignCount = static_cast<int>(pLayer->m_arrAlign.GetCount());
			for (int i = 0; i < nAlignCount; i++)
			{
				CAlignMask *pAlignMask = pLayer->m_arrAlign.GetAt(i);
				if (pAlignMask == nullptr) continue;
				int nFeatureCount = static_cast<int>(pAlignMask->m_arrFeature.GetCount());
				for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
				{
					CFeature *pFeature = pAlignMask->m_arrFeature.GetAt(nfeno);
					if (pFeature == nullptr) continue;

					switch (pFeature->m_eType)
					{
					case FeatureType::P:		//Pad
					{
						((CFeatureP*)pFeature)->m_bHighlight = false;
					}
					break;
					case FeatureType::S:		//Surface	
					{
						CFeatureS *pFeatureS = (CFeatureS*)pFeature;
						int nSurfaceCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
						for (int nSurface = 0; nSurface < nSurfaceCount; nSurface++)
						{
							CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nSurface);
							if (pObOe == nullptr) continue;

							pObOe->m_bHighlight = false;

						}

					}
					break;
					}
				}
			}

			int nMaskCount = static_cast<int>(pLayer->m_arrMask.GetCount());
			for (int i = 0; i < nMaskCount; i++)
			{
				CAlignMask *pAlignMask = pLayer->m_arrAlign.GetAt(i);
				if (pAlignMask == nullptr) continue;
				int nFeatureCount = static_cast<int>(pAlignMask->m_arrFeature.GetCount());
				for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
				{
					CFeature *pFeature = pAlignMask->m_arrFeature.GetAt(nfeno);
					if (pFeature == nullptr) continue;

					switch (pFeature->m_eType)
					{
					case FeatureType::P:		//Pad
					{
						((CFeatureP*)pFeature)->m_bHighlight = false;
					}
					break;
					case FeatureType::S:		//Surface	
					{
						CFeatureS *pFeatureS = (CFeatureS*)pFeature;
						int nSurfaceCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
						for (int nSurface = 0; nSurface < nSurfaceCount; nSurface++)
						{
							CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nSurface);
							if (pObOe == nullptr) continue;

							pObOe->m_bHighlight = false;

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

UINT32			CDrawHighlightShowFunc::SetSelect(IN CJobFile *pJobFile, IN const int &nStep, vector<FEATURE_INFO> &vecFeatureInfo, SELECT_INFO &stSelectInfo)
{
	if (pJobFile == nullptr) return RESULT_BAD;
	if (nStep < 0) return RESULT_BAD;
	if (nStep >= static_cast<int>(pJobFile->m_arrStep.GetCount())) return RESULT_BAD;
	
	int nInfoCount = static_cast<int>(vecFeatureInfo.size());
	if (nInfoCount <= 0) return RESULT_BAD;

	vector<int> vecSurfaceIndex;

	for (int nInfo = 0; nInfo < nInfoCount; nInfo++)
	{
		if (vecFeatureInfo[nInfo].strLayerName == _T("")) continue;

		CStep *pStep = pJobFile->m_arrStep.GetAt(nStep);
		if (pStep == nullptr) continue;

		BOOL bFindLayer = FALSE;
		int nLayerCount = static_cast<int>(pStep->m_arrLayer.GetCount());
		for (int nLayer = 0; nLayer < nLayerCount; nLayer++)
		{
			CLayer *pLayer = pStep->m_arrLayer.GetAt(nLayer);

			//같은 Layer 이름 찾기
			if (pLayer == nullptr) continue;
			if (vecFeatureInfo[nInfo].strLayerName != pLayer->m_strLayerName ) continue;
			
			bFindLayer = TRUE;
			
			if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Feature)
			{
				_SetFeature_byName(SurfaceMode::enumSurfaceMode_Select, pLayer, vecFeatureInfo[nInfo], stSelectInfo);
			}
			else if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Attribute)
			{
				_SetFeature_byAtt(SurfaceMode::enumSurfaceMode_Select, pLayer, vecFeatureInfo[nInfo], stSelectInfo);
			}
			else if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Symbol)
			{
				_SetFeature_bySymbol(SurfaceMode::enumSurfaceMode_Select, pLayer, vecFeatureInfo[nInfo], stSelectInfo);
			}

			//같은 이름의 Layer를 찾았으면 현재 Layer에서는 그만 찾기
			if (bFindLayer == TRUE)
			{
				break;
			}

		}
	}

	//Surface만 따로 찾기
	for (int nInfo = 0; nInfo < nInfoCount; nInfo++)
	{
		if (vecFeatureInfo[nInfo].eType != FeatureType::S) continue;
		if (vecFeatureInfo[nInfo].strLayerName == _T("")) continue;
				
		CStep *pStep = pJobFile->m_arrStep.GetAt(nStep);
		if (pStep == nullptr) continue;

		BOOL bFindLayer = FALSE;
		int nLayerCount = static_cast<int>(pStep->m_arrLayer.GetCount());
		for (int nLayer = 0; nLayer < nLayerCount; nLayer++)
		{
			CLayer *pLayer = pStep->m_arrLayer.GetAt(nLayer);

			//같은 Layer 이름 찾기
			if (pLayer == nullptr) continue;
			if (vecFeatureInfo[nInfo].strLayerName != pLayer->m_strLayerName) continue;

			bFindLayer = TRUE;

			if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Feature)
			{//이름으로 선택

				//Surface를 번호순으로 정렬
				_GetSurfaceIndex(pJobFile, nStep, vecFeatureInfo[nInfo].strLayerName, vecSurfaceIndex);

				int nSelectIndex = _ttoi(vecFeatureInfo[nInfo].strFeatureName) - 1;
				if (nSelectIndex < 0) continue;
				if (nSelectIndex >= static_cast<int>(vecSurfaceIndex.size())) continue;

				int nFeature = vecSurfaceIndex[nSelectIndex];

				CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nFeature);

				if (pFeature->m_eType == FeatureType::S)
				{
					CFeatureS *pFeatureS = (CFeatureS *)pFeature;
					_SetSurface_byName(enumSurfaceMode_Select, pFeatureS, nFeature, pLayer->m_strLayerName, SELECT_INFO());
				}
			}
			else if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Attribute)
			{
				int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());
				for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
				{
					CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nfeno);
					if (pFeature == nullptr) continue;

					if (pFeature->m_eType == FeatureType::S)
					{
						CFeatureS *pFeatureS = (CFeatureS *)pFeature;

						if (static_cast<int>(pFeatureS->m_arrAttributeTextString.GetCount()) == 0)
						{
							int nSize = static_cast<int>(vecFeatureInfo[nInfo].vecAttString.size());
							if (nSize == 1 &&
								vecFeatureInfo[nInfo].vecAttString[0] == DEF_HISTOGRAM::strAttNull)
							{

								_SetSurface_byName(enumSurfaceMode_Select, pFeatureS, nfeno, pLayer->m_strLayerName, SELECT_INFO());
							}
						}
						else if (vecFeatureInfo[nInfo].Compare_AttString(pFeatureS->m_arrAttributeTextString) == TRUE)
						{
							_SetSurface_byName(enumSurfaceMode_Select, pFeatureS, nfeno, pLayer->m_strLayerName, SELECT_INFO());
						}
					}
				}
			}
			else if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Symbol)
			{
				int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());
				for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
				{
					CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nfeno);
					if (pFeature == nullptr) continue;

					if (pFeature->m_eType == FeatureType::S)
					{
						CFeatureS *pFeatureS = (CFeatureS *)pFeature;
						if (pFeatureS->m_pSymbol == nullptr &&
							vecFeatureInfo[nInfo].strSymbolName == DEF_HISTOGRAM::strSymNull)
						{
							_SetSurface_byName(enumSurfaceMode_Select, pFeatureS, nfeno, pLayer->m_strLayerName, SELECT_INFO());
						}
						else if (pFeatureS->m_pSymbol != nullptr &&
							pFeatureS->m_pSymbol->m_strUserSymbol == vecFeatureInfo[nInfo].strSymbolName)
						{
							_SetSurface_byName(enumSurfaceMode_Select, pFeatureS, nfeno, pLayer->m_strLayerName, SELECT_INFO());
						}
					}
				}
			}

			//같은 이름의 Layer를 찾았으면 현재 Layer에서는 그만 찾기
			if (bFindLayer == TRUE)
			{
				break;
			}
		}

		
	}

	return RESULT_GOOD;
}

UINT32			CDrawHighlightShowFunc::SetUnSelect(IN CJobFile *pJobFile, IN const int &nStep, vector<FEATURE_INFO> &vecFeatureInfo, SELECT_INFO &stSelectInfo)
{
	if (pJobFile == nullptr) return RESULT_BAD;
	if (nStep < 0) return RESULT_BAD;
	if (nStep >= static_cast<int>(pJobFile->m_arrStep.GetCount())) return RESULT_BAD;

	int nInfoCount = static_cast<int>(vecFeatureInfo.size());
	if (nInfoCount <= 0) return RESULT_BAD;

	for (int nInfo = 0; nInfo < nInfoCount; nInfo++)
	{
		if (vecFeatureInfo[nInfo].strLayerName == _T("")) continue;

		CStep *pStep = pJobFile->m_arrStep.GetAt(nStep);
		if (pStep == nullptr) continue;

		BOOL bFindLayer = FALSE;
		int nLayerCount = static_cast<int>(pStep->m_arrLayer.GetCount());
		for (int nLayer = 0; nLayer < nLayerCount; nLayer++)
		{
			CLayer *pLayer = pStep->m_arrLayer.GetAt(nLayer);

			//같은 Layer 이름 찾기
			if (pLayer == nullptr) continue;
			if (vecFeatureInfo[nInfo].strLayerName != pLayer->m_strLayerName) continue;

			bFindLayer = TRUE;

			if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Feature)
			{
				_SetFeature_byName(SurfaceMode::enumSurfaceMode_UnSelect, pLayer, vecFeatureInfo[nInfo], stSelectInfo);
			}
			else if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Attribute)
			{
				_SetFeature_byAtt(SurfaceMode::enumSurfaceMode_UnSelect, pLayer, vecFeatureInfo[nInfo], stSelectInfo);
			}
			else if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Symbol)
			{
				_SetFeature_bySymbol(SurfaceMode::enumSurfaceMode_UnSelect, pLayer, vecFeatureInfo[nInfo], stSelectInfo);
			}

			//같은 이름의 Layer를 찾았으면 현재 Layer에서는 그만 찾기
			if (bFindLayer == TRUE)
			{
				break;
			}

		}
	}

	//Surface만 따로 찾기
	vector<int> vecSurfaceIndex;
	for (int nInfo = 0; nInfo < nInfoCount; nInfo++)
	{
		if (vecFeatureInfo[nInfo].strLayerName == _T("")) continue;

		_GetSurfaceIndex(pJobFile, nStep, vecFeatureInfo[nInfo].strLayerName, vecSurfaceIndex);

		CStep *pStep = pJobFile->m_arrStep.GetAt(nStep);
		if (pStep == nullptr) continue;

		BOOL bFindLayer = FALSE;
		int nLayerCount = static_cast<int>(pStep->m_arrLayer.GetCount());
		for (int nLayer = 0; nLayer < nLayerCount; nLayer++)
		{
			CLayer *pLayer = pStep->m_arrLayer.GetAt(nLayer);

			//같은 Layer 이름 찾기
			if (pLayer == nullptr) continue;
			if (vecFeatureInfo[nInfo].strLayerName != pLayer->m_strLayerName) continue;

			bFindLayer = TRUE;

			if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Feature)
			{//이름으로 선택
				int nSelectIndex = _ttoi(vecFeatureInfo[nInfo].strFeatureName) - 1;
				if (nSelectIndex < 0) continue;
				if (nSelectIndex >= static_cast<int>(vecSurfaceIndex.size())) continue;

				int nFeature = vecSurfaceIndex[nSelectIndex];

				CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nFeature);

				if (pFeature->m_eType == FeatureType::S)
				{
					CFeatureS *pFeatureS = (CFeatureS *)pFeature;
					_SetSurface_byName(enumSurfaceMode_UnSelect, pFeatureS, nFeature, pLayer->m_strLayerName, SELECT_INFO());
				}
			}
			else if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Attribute)
			{
				int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());
				for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
				{
					CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nfeno);
					if (pFeature == nullptr) continue;

					if (pFeature->m_eType == FeatureType::S)
					{
						CFeatureS *pFeatureS = (CFeatureS *)pFeature;

						if (static_cast<int>(pFeatureS->m_arrAttributeTextString.GetCount()) == 0)
						{
							int nSize = static_cast<int>(vecFeatureInfo[nInfo].vecAttString.size());
							if (nSize == 1 &&
								vecFeatureInfo[nInfo].vecAttString[0] == DEF_HISTOGRAM::strAttNull)
							{

								_SetSurface_byName(enumSurfaceMode_UnSelect, pFeatureS, nfeno, pLayer->m_strLayerName, SELECT_INFO());
							}
						}
						else if (vecFeatureInfo[nInfo].Compare_AttString(pFeatureS->m_arrAttributeTextString) == TRUE)
						{
							_SetSurface_byName(enumSurfaceMode_UnSelect, pFeatureS, nfeno, pLayer->m_strLayerName, SELECT_INFO());
						}
					}
				}
			}
			else if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Symbol)
			{
				int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());
				for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
				{
					CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nfeno);
					if (pFeature == nullptr) continue;

					if (pFeature->m_eType == FeatureType::S)
					{
						CFeatureS *pFeatureS = (CFeatureS *)pFeature;
						if (pFeatureS->m_pSymbol == nullptr &&
							vecFeatureInfo[nInfo].strSymbolName == DEF_HISTOGRAM::strSymNull)
						{
							_SetSurface_byName(enumSurfaceMode_UnSelect, pFeatureS, nfeno, pLayer->m_strLayerName, SELECT_INFO());
						}
						else if (pFeatureS->m_pSymbol != nullptr &&
							pFeatureS->m_pSymbol->m_strUserSymbol == vecFeatureInfo[nInfo].strSymbolName)
						{
							_SetSurface_byName(enumSurfaceMode_UnSelect, pFeatureS, nfeno, pLayer->m_strLayerName, SELECT_INFO());
						}
					}
				}
			}

			//같은 이름의 Layer를 찾았으면 현재 Layer에서는 그만 찾기
			if (bFindLayer == TRUE)
			{
				break;
			}
		}

		
	}

	return RESULT_GOOD;
}


//Feature Show/Hide
UINT32			CDrawHighlightShowFunc::ResetHide(IN CJobFile *pJobFile)
{
	if (pJobFile == nullptr) return RESULT_BAD;

	int nStepCount = static_cast<int>(pJobFile->m_arrStep.GetCount());
	for (int nStep = 0; nStep < nStepCount; nStep++)
	{
		CStep *pStep = pJobFile->m_arrStep.GetAt(nStep);
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
				case FeatureType::Z:
				{
					((CFeatureZ*)pFeature)->m_bShow = true;
				}

				case FeatureType::L:		//Line
				{
					((CFeatureL*)pFeature)->m_bShow = true;
				}
					break;
				case FeatureType::P:		//Pad
				{
					((CFeatureP*)pFeature)->m_bShow = true;
				}
					break;
				case FeatureType::A:		//Arc								
				{
					((CFeatureA*)pFeature)->m_bShow = true;
				}
					break;
				case FeatureType::T:		//Text			
				{
					((CFeatureT*)pFeature)->m_bShow = true;
				}
					break;
				case FeatureType::S:		//Surface	
				{
					CFeatureS *pFeatureS = (CFeatureS*)pFeature;
					int nSurfaceCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
					for (int nSurface = 0; nSurface < nSurfaceCount; nSurface++)
					{
						CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nSurface);
						if (pObOe == nullptr) continue;

						pObOe->m_bShow = true;

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

UINT32			CDrawHighlightShowFunc::SetShow(IN CJobFile *pJobFile, IN const int &nStep, vector<FEATURE_INFO> &vecFeatureInfo)
{
	if (pJobFile == nullptr) return RESULT_BAD;
	if (nStep < 0) return RESULT_BAD;
	if (nStep >= static_cast<int>(pJobFile->m_arrStep.GetCount())) return RESULT_BAD;

	int nInfoCount = static_cast<int>(vecFeatureInfo.size());
	if (nInfoCount <= 0) return RESULT_BAD;

	for (int nInfo = 0; nInfo < nInfoCount; nInfo++)
	{
		if (vecFeatureInfo[nInfo].strLayerName == _T("")) continue;

		CStep *pStep = pJobFile->m_arrStep.GetAt(nStep);
		if (pStep == nullptr) continue;

		BOOL bFindLayer = FALSE;
		int nLayerCount = static_cast<int>(pStep->m_arrLayer.GetCount());
		for (int nLayer = 0; nLayer < nLayerCount; nLayer++)
		{
			CLayer *pLayer = pStep->m_arrLayer.GetAt(nLayer);

			//같은 Layer 이름 찾기
			if (pLayer == nullptr) continue;
			if (vecFeatureInfo[nInfo].strLayerName != pLayer->m_strLayerName) continue;

			bFindLayer = TRUE;

			if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Feature)
			{
				_SetFeature_byName(SurfaceMode::enumSurfaceMode_Show, pLayer, vecFeatureInfo[nInfo], SELECT_INFO());
			}
			else if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Attribute)
			{
				_SetFeature_byAtt(SurfaceMode::enumSurfaceMode_Show, pLayer, vecFeatureInfo[nInfo], SELECT_INFO());
			}
			else if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Symbol)
			{
				_SetFeature_bySymbol(SurfaceMode::enumSurfaceMode_Show, pLayer, vecFeatureInfo[nInfo], SELECT_INFO());
			}

			//같은 이름의 Layer를 찾았으면 현재 Layer에서는 그만 찾기
			if (bFindLayer == TRUE)
			{
				break;
			}

		}
	}

	//Surface만 따로 찾기
	vector<int> vecSurfaceIndex;
	for (int nInfo = 0; nInfo < nInfoCount; nInfo++)
	{
		if (vecFeatureInfo[nInfo].strLayerName == _T("")) continue;

		_GetSurfaceIndex(pJobFile, nStep, vecFeatureInfo[nInfo].strLayerName, vecSurfaceIndex);

		CStep *pStep = pJobFile->m_arrStep.GetAt(nStep);
		if (pStep == nullptr) continue;

		BOOL bFindLayer = FALSE;
		int nLayerCount = static_cast<int>(pStep->m_arrLayer.GetCount());
		for (int nLayer = 0; nLayer < nLayerCount; nLayer++)
		{
			CLayer *pLayer = pStep->m_arrLayer.GetAt(nLayer);

			//같은 Layer 이름 찾기
			if (pLayer == nullptr) continue;
			if (vecFeatureInfo[nInfo].strLayerName != pLayer->m_strLayerName) continue;

			bFindLayer = TRUE;

			if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Feature)
			{//이름으로 선택
				int nSelectIndex = _ttoi(vecFeatureInfo[nInfo].strFeatureName) - 1;
				if (nSelectIndex < 0) continue;
				if (nSelectIndex >= static_cast<int>(vecSurfaceIndex.size())) continue;

				int nFeature = vecSurfaceIndex[nSelectIndex];

				CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nFeature);

				if (pFeature->m_eType == FeatureType::S)
				{
					CFeatureS *pFeatureS = (CFeatureS *)pFeature;
					_SetSurface_byName(enumSurfaceMode_Show, pFeatureS, nFeature, pLayer->m_strLayerName, SELECT_INFO());
				}
			}
			else if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Attribute)
			{
				int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());
				for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
				{
					CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nfeno);
					if (pFeature == nullptr) continue;

					if (pFeature->m_eType == FeatureType::S)
					{
						CFeatureS *pFeatureS = (CFeatureS *)pFeature;

						if (static_cast<int>(pFeatureS->m_arrAttributeTextString.GetCount()) == 0)
						{
							int nSize = static_cast<int>(vecFeatureInfo[nInfo].vecAttString.size());
							if (nSize == 1 &&
								vecFeatureInfo[nInfo].vecAttString[0] == DEF_HISTOGRAM::strAttNull)
							{

								_SetSurface_byName(enumSurfaceMode_Show, pFeatureS, nfeno, pLayer->m_strLayerName, SELECT_INFO());
							}
						}
						else if (vecFeatureInfo[nInfo].Compare_AttString(pFeatureS->m_arrAttributeTextString) == TRUE)
						{
							_SetSurface_byName(enumSurfaceMode_Show, pFeatureS, nfeno, pLayer->m_strLayerName, SELECT_INFO());
						}
					}
				}
			}
			else if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Symbol)
			{
				int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());
				for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
				{
					CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nfeno);
					if (pFeature == nullptr) continue;

					if (pFeature->m_eType == FeatureType::S)
					{
						CFeatureS *pFeatureS = (CFeatureS *)pFeature;
						if (pFeatureS->m_pSymbol == nullptr &&
							vecFeatureInfo[nInfo].strSymbolName == DEF_HISTOGRAM::strSymNull)
						{
							_SetSurface_byName(enumSurfaceMode_Show, pFeatureS, nfeno, pLayer->m_strLayerName, SELECT_INFO());
						}
						else if (pFeatureS->m_pSymbol != nullptr &&
							pFeatureS->m_pSymbol->m_strUserSymbol == vecFeatureInfo[nInfo].strSymbolName)
						{
							_SetSurface_byName(enumSurfaceMode_Show, pFeatureS, nfeno, pLayer->m_strLayerName, SELECT_INFO());
						}
					}
				}
			}

			//같은 이름의 Layer를 찾았으면 현재 Layer에서는 그만 찾기
			if (bFindLayer == TRUE)
			{
				break;
			}
		}

		
	}

	return RESULT_GOOD;
}

UINT32			CDrawHighlightShowFunc::SetHide(IN CJobFile *pJobFile, IN const int &nStep, vector<FEATURE_INFO> &vecFeatureInfo)
{
	if (pJobFile == nullptr) return RESULT_BAD;
	if (nStep < 0) return RESULT_BAD;
	if (nStep >= static_cast<int>(pJobFile->m_arrStep.GetCount())) return RESULT_BAD;

	int nInfoCount = static_cast<int>(vecFeatureInfo.size());
	if (nInfoCount <= 0) return RESULT_BAD;

	for (int nInfo = 0; nInfo < nInfoCount; nInfo++)
	{
		if (vecFeatureInfo[nInfo].strLayerName == _T("")) continue;

		CStep *pStep = pJobFile->m_arrStep.GetAt(nStep);
		if (pStep == nullptr) continue;

		BOOL bFindLayer = FALSE;
		int nLayerCount = static_cast<int>(pStep->m_arrLayer.GetCount());
		for (int nLayer = 0; nLayer < nLayerCount; nLayer++)
		{
			CLayer *pLayer = pStep->m_arrLayer.GetAt(nLayer);

			//같은 Layer 이름 찾기
			if (pLayer == nullptr) continue;
			if (vecFeatureInfo[nInfo].strLayerName != pLayer->m_strLayerName) continue;

			bFindLayer = TRUE;

			if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Feature)
			{
				_SetFeature_byName(SurfaceMode::enumSurfaceMode_Hide, pLayer, vecFeatureInfo[nInfo], SELECT_INFO());
			}
			else if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Attribute)
			{
				_SetFeature_byAtt(SurfaceMode::enumSurfaceMode_Hide, pLayer, vecFeatureInfo[nInfo], SELECT_INFO());
			}
			else if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Symbol)
			{
				_SetFeature_bySymbol(SurfaceMode::enumSurfaceMode_Hide, pLayer, vecFeatureInfo[nInfo], SELECT_INFO());
			}

			//같은 이름의 Layer를 찾았으면 현재 Layer에서는 그만 찾기
			if (bFindLayer == TRUE)
			{
				break;
			}

		}
	}

	//Surface만 따로 찾기
	vector<int> vecSurfaceIndex;
	for (int nInfo = 0; nInfo < nInfoCount; nInfo++)
	{
		if (vecFeatureInfo[nInfo].strLayerName == _T("")) continue;

		_GetSurfaceIndex(pJobFile, nStep, vecFeatureInfo[nInfo].strLayerName, vecSurfaceIndex);

		CStep *pStep = pJobFile->m_arrStep.GetAt(nStep);
		if (pStep == nullptr) continue;

		BOOL bFindLayer = FALSE;
		int nLayerCount = static_cast<int>(pStep->m_arrLayer.GetCount());
		for (int nLayer = 0; nLayer < nLayerCount; nLayer++)
		{
			CLayer *pLayer = pStep->m_arrLayer.GetAt(nLayer);

			//같은 Layer 이름 찾기
			if (pLayer == nullptr) continue;
			if (vecFeatureInfo[nInfo].strLayerName != pLayer->m_strLayerName) continue;

			bFindLayer = TRUE;

			if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Feature)
			{//이름으로 선택
				int nSelectIndex = _ttoi(vecFeatureInfo[nInfo].strFeatureName) - 1;
				if (nSelectIndex < 0) continue;
				if (nSelectIndex >= static_cast<int>(vecSurfaceIndex.size())) continue;

				int nFeature = vecSurfaceIndex[nSelectIndex];

				CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nFeature);

				if (pFeature->m_eType == FeatureType::S)
				{
					CFeatureS *pFeatureS = (CFeatureS *)pFeature;
					_SetSurface_byName(enumSurfaceMode_Hide, pFeatureS, nFeature, pLayer->m_strLayerName, SELECT_INFO());
				}
			}
			else if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Attribute)
			{
				int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());
				for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
				{
					CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nfeno);
					if ( pFeature == nullptr) continue;
					
					if (pFeature->m_eType == FeatureType::S)
					{
						CFeatureS *pFeatureS = (CFeatureS *)pFeature;

						if (static_cast<int>(pFeatureS->m_arrAttributeTextString.GetCount()) == 0 )
						{
							int nSize = static_cast<int>(vecFeatureInfo[nInfo].vecAttString.size());
							if (nSize == 1 &&
								vecFeatureInfo[nInfo].vecAttString[0] == DEF_HISTOGRAM::strAttNull)
							{

								_SetSurface_byName(enumSurfaceMode_Hide, pFeatureS, nfeno, pLayer->m_strLayerName, SELECT_INFO());
							}
						}
						else if (vecFeatureInfo[nInfo].Compare_AttString(pFeatureS->m_arrAttributeTextString) == TRUE )
						{
							_SetSurface_byName(enumSurfaceMode_Hide, pFeatureS, nfeno, pLayer->m_strLayerName, SELECT_INFO());
						}
					}
				}
			}
			else if (vecFeatureInfo[nInfo].nHistoSelectType == DEF_HISTOGRAM::Type_Symbol)
			{
				int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());
				for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
				{
					CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nfeno);
					if (pFeature == nullptr) continue;

					if (pFeature->m_eType == FeatureType::S)
					{
						CFeatureS *pFeatureS = (CFeatureS *)pFeature;
						if (pFeatureS->m_pSymbol == nullptr &&
							vecFeatureInfo[nInfo].strSymbolName == DEF_HISTOGRAM::strSymNull)
						{
							_SetSurface_byName(enumSurfaceMode_Hide, pFeatureS, nfeno, pLayer->m_strLayerName, SELECT_INFO());
						}
						else if (pFeatureS->m_pSymbol != nullptr &&
							pFeatureS->m_pSymbol->m_strUserSymbol == vecFeatureInfo[nInfo].strSymbolName)
						{
							_SetSurface_byName(enumSurfaceMode_Hide, pFeatureS, nfeno, pLayer->m_strLayerName, SELECT_INFO());
						}
					}
				}
			}

			//같은 이름의 Layer를 찾았으면 현재 Layer에서는 그만 찾기
			if (bFindLayer == TRUE)
			{
				break;
			}
		}

		
	}

	return RESULT_GOOD;
}


UINT32	CDrawHighlightShowFunc::_GetSurfaceIndex(IN CJobFile *pJobFile, IN const int &nStep, IN const CString &strLayerName, OUT vector<int> &verSurfaceIndex)
{
	if (pJobFile == nullptr) return RESULT_BAD;
	if (nStep < 0) return RESULT_BAD;
	if (nStep >= static_cast<int>(pJobFile->m_arrStep.GetCount())) return RESULT_BAD;

	verSurfaceIndex.clear();
	int nCount = 0;
	CStep *pStep = pJobFile->m_arrStep.GetAt(nStep);
	if (pStep == nullptr) return RESULT_BAD;

	BOOL bFindLayer = FALSE;
	int nLayerCount = static_cast<int>(pStep->m_arrLayer.GetCount());
	for (int nLayer = 0; nLayer < nLayerCount; nLayer++)
	{
		CLayer *pLayer = pStep->m_arrLayer.GetAt(nLayer);

		//같은 Layer 이름 찾기
		if (pLayer == nullptr) continue;
		if (strLayerName != pLayer->m_strLayerName) continue;

		bFindLayer = TRUE;

		//같은 이름의 Layer에서 Feature 선택
		int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());

		for (int nFeature = 0; nFeature < nFeatureCount; nFeature++)
		{
			CFeature *pFeaure = pLayer->m_FeatureFile.m_arrFeature.GetAt(nFeature);
			if (pFeaure == nullptr) continue;

			switch (pFeaure->m_eType)
			{
			case FeatureType::S:
				verSurfaceIndex.push_back(nFeature);
				break;
			default:
				break;
			}
		}
	}

	return RESULT_GOOD;
}

void CDrawHighlightShowFunc::UpdateSelectInfo(IN CJobFile *pJobFile, IN const int &nStep, IN const UserLayerSet_t *pUserLayerSet, SELECT_INFO &stSelectInfo)
{
	if (pJobFile == nullptr) return ;
	if (nStep < 0) return ;
	if (nStep >= static_cast<int>(pJobFile->m_arrStep.GetCount())) return ;

	CStep *pStep = pJobFile->m_arrStep.GetAt(nStep);
	if (pStep == nullptr) return;
	
	stSelectInfo._Reset();

	int nLayerSetCount = static_cast<int>(pUserLayerSet->vcLayerSet.size());
	for (int i = 0; i < nLayerSetCount; i++)
	{
		LayerSet_t* pLayerSet = pUserLayerSet->vcLayerSet[i];

		if (pLayerSet->bCheck == true)
		{
			int nLayerIndex;
			CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, pLayerSet->strLayer, nLayerIndex);
			if ( pLayer == nullptr ) continue;

			int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());
			for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
			{
				CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nfeno);
				if (pFeature == nullptr) continue;

				switch (pFeature->m_eType)
				{
				case FeatureType::Z:
				{
					CFeatureZ *pFeatureZ = (CFeatureZ *)pFeature;
					if (pFeatureZ->m_bHighlight == true)
					{
						SetSelectInfo(pFeatureZ, nfeno, pLayer->m_strLayerName, stSelectInfo);
					}
				}
				break;
				case FeatureType::L :
				{
					CFeatureL *pFeatureL = (CFeatureL *)pFeature;
					if (pFeatureL->m_bHighlight == true)
					{
						SetSelectInfo(pFeatureL, nfeno, pLayer->m_strLayerName, stSelectInfo);
					}
				}
				break;
				case FeatureType::P:
				{
					CFeatureP *pFeatureP = (CFeatureP *)pFeature;
					if (pFeatureP->m_bHighlight == true)
					{
						SetSelectInfo(pFeatureP, nfeno, pLayer->m_strLayerName, stSelectInfo);
					}
				}
				break;
				case FeatureType::A:
				{
					CFeatureA *pFeatureA = (CFeatureA *)pFeature;
					if (pFeatureA->m_bHighlight == true)
					{
						SetSelectInfo(pFeatureA, nfeno, pLayer->m_strLayerName, stSelectInfo);
					}
				}
				break;
				case FeatureType::T:
				{
					CFeatureT *pFeatureT = (CFeatureT *)pFeature;
					if (pFeatureT->m_bHighlight == true)
					{
						SetSelectInfo(pFeatureT, nfeno, pLayer->m_strLayerName, stSelectInfo);
					}
				}
				break;
				case FeatureType::S:
				{
					CFeatureS *pFeatureS = (CFeatureS *)pFeature;
					int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
					for (int nObOe = 0; nObOe < nObOeCount; nObOe++)
					{
						CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nObOe);
						if (pObOe->m_bHighlight == true)
						{
							SetSelectInfo(pObOe, nfeno, nObOe, pLayer->m_strLayerName, stSelectInfo);
						}
					}
					
				}
				break;
				break;
				default:
					break;

				}
			}
		}
	}


}


void CDrawHighlightShowFunc::SetSelectInfo(IN const CFeatureZ* pFeatureZ, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo)
{
	if (pFeatureZ == nullptr) return;

	SELECT_INFO_UNIT stInfoUnit;
	stInfoUnit.strLayer = strLayerName;
	stInfoUnit.nFeatureIndex = nIndex + 1;
	stInfoUnit.emFeatureType = pFeatureZ->m_eType;
	stInfoUnit.dCx = (pFeatureZ->m_dX + pFeatureZ->m_dX) / 2.0;
	stInfoUnit.dCy = (pFeatureZ->m_dY + pFeatureZ->m_dY) / 2.0;
	RECTD drtRect = pFeatureZ->m_MinMaxRect;
	stInfoUnit.dWidth = drtRect.width();
	stInfoUnit.dHeight = drtRect.height();
	if (pFeatureZ->m_pSymbol != nullptr)
	{
		stInfoUnit.strSymbolName = pFeatureZ->m_pSymbol->m_strUserSymbol;
	}

	//Att String
	stInfoUnit.vecAttTextString.clear();
	int nCount = static_cast<int>(pFeatureZ->m_arrAttributeTextString.GetCount());

	for (int i = 0; i < nCount; i++)
	{
		CAttributeTextString* pAttString = pFeatureZ->m_arrAttributeTextString.GetAt(i);
		if (pAttString == nullptr) continue;
		stInfoUnit.vecAttTextString.push_back(pAttString->m_strText);
	}

	stInfoUnit.bPolarity = pFeatureZ->m_bPolarity;

	stSelectInfo.vecSelectInfoUnit.push_back(stInfoUnit);
}


void CDrawHighlightShowFunc::SetSelectInfo(IN const CFeatureL* pFeatureL, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo)
{
	if (pFeatureL == nullptr) return;

	SELECT_INFO_UNIT stInfoUnit;
	stInfoUnit.strLayer = strLayerName;
	stInfoUnit.nFeatureIndex = nIndex + 1;
	stInfoUnit.emFeatureType = pFeatureL->m_eType;
	stInfoUnit.dCx = (pFeatureL->m_dXe + pFeatureL->m_dXs) / 2.0;
	stInfoUnit.dCy = (pFeatureL->m_dYe + pFeatureL->m_dYs) / 2.0;
	RECTD drtRect = pFeatureL->m_MinMaxRect;
	stInfoUnit.dWidth = drtRect.width();
	stInfoUnit.dHeight = drtRect.height();
	if (pFeatureL->m_pSymbol != nullptr)
	{
		stInfoUnit.strSymbolName = pFeatureL->m_pSymbol->m_strUserSymbol;
	}

	//Att String
	stInfoUnit.vecAttTextString.clear();
	int nCount = static_cast<int>(pFeatureL->m_arrAttributeTextString.GetCount());

	for (int i = 0; i < nCount; i++)
	{
		CAttributeTextString* pAttString = pFeatureL->m_arrAttributeTextString.GetAt(i);
		if (pAttString == nullptr ) continue;
		stInfoUnit.vecAttTextString.push_back(pAttString->m_strText);
	}

	stInfoUnit.bPolarity = pFeatureL->m_bPolarity;

	stSelectInfo.vecSelectInfoUnit.push_back(stInfoUnit);
}

void CDrawHighlightShowFunc::SetSelectInfo(IN const CFeatureP* pFeatureP, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo)
{
	if (pFeatureP == nullptr) return;

	SELECT_INFO_UNIT stInfoUnit;
	stInfoUnit.strLayer = strLayerName;
	stInfoUnit.nFeatureIndex = nIndex + 1;
	stInfoUnit.emFeatureType = pFeatureP->m_eType;
	stInfoUnit.dCx = pFeatureP->m_dX;
	stInfoUnit.dCy = pFeatureP->m_dY;
	RECTD drtRect = pFeatureP->m_MinMaxRect;
	stInfoUnit.dWidth = drtRect.width();
	stInfoUnit.dHeight = drtRect.height();
	if (pFeatureP->m_pSymbol != nullptr)
	{
		stInfoUnit.strSymbolName = pFeatureP->m_pSymbol->m_strUserSymbol;
	}

	//Att String
	stInfoUnit.vecAttTextString.clear();
	int nCount = static_cast<int>(pFeatureP->m_arrAttributeTextString.GetCount());

	for (int i = 0; i < nCount; i++)
	{
		CAttributeTextString* pAttString = pFeatureP->m_arrAttributeTextString.GetAt(i);
		if (pAttString == nullptr) continue;
		stInfoUnit.vecAttTextString.push_back(pAttString->m_strText);
	}

	stInfoUnit.bPolarity = pFeatureP->m_bPolarity;

	stSelectInfo.vecSelectInfoUnit.push_back(stInfoUnit);
}

void CDrawHighlightShowFunc::SetSelectInfo(IN const CFeatureA* pFeatureA, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo)
{
	if (pFeatureA == nullptr) return;

	SELECT_INFO_UNIT stInfoUnit;
	stInfoUnit.strLayer = strLayerName;
	stInfoUnit.nFeatureIndex = nIndex + 1;
	stInfoUnit.emFeatureType = pFeatureA->m_eType;
	stInfoUnit.dCx = pFeatureA->m_dXc;
	stInfoUnit.dCy = pFeatureA->m_dYc;
	RECTD drtRect = pFeatureA->m_MinMaxRect;
	stInfoUnit.dWidth = drtRect.width();
	stInfoUnit.dHeight = drtRect.height();
	if (pFeatureA->m_pSymbol != nullptr)
	{
		stInfoUnit.strSymbolName = pFeatureA->m_pSymbol->m_strUserSymbol;
	}

	//Att String
	stInfoUnit.vecAttTextString.clear();
	int nCount = static_cast<int>(pFeatureA->m_arrAttributeTextString.GetCount());

	for (int i = 0; i < nCount; i++)
	{
		CAttributeTextString* pAttString = pFeatureA->m_arrAttributeTextString.GetAt(i);
		if (pAttString == nullptr) continue;
		stInfoUnit.vecAttTextString.push_back(pAttString->m_strText);
	}

	stInfoUnit.bPolarity = pFeatureA->m_bPolarity;

	stSelectInfo.vecSelectInfoUnit.push_back(stInfoUnit);
}

void CDrawHighlightShowFunc::SetSelectInfo(IN const CFeatureT* pFeatureT, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo)
{
	if (pFeatureT == nullptr) return;

	SELECT_INFO_UNIT stInfoUnit;
	stInfoUnit.strLayer = strLayerName;
	stInfoUnit.nFeatureIndex = nIndex + 1;
	stInfoUnit.emFeatureType = pFeatureT->m_eType;
	stInfoUnit.dCx = pFeatureT->m_dX;
	stInfoUnit.dCy = pFeatureT->m_dY;
	RECTD drtRect = pFeatureT->m_MinMaxRect;
	stInfoUnit.dWidth = drtRect.width();
	stInfoUnit.dHeight = drtRect.height();
	if (pFeatureT->m_pSymbol != nullptr)
	{
		stInfoUnit.strSymbolName = pFeatureT->m_pSymbol->m_strUserSymbol;
	}

	//Att String
	stInfoUnit.vecAttTextString.clear();
	int nCount = static_cast<int>(pFeatureT->m_arrAttributeTextString.GetCount());

	for (int i = 0; i < nCount; i++)
	{
		CAttributeTextString* pAttString = pFeatureT->m_arrAttributeTextString.GetAt(i);
		if (pAttString == nullptr) continue;
		stInfoUnit.vecAttTextString.push_back(pAttString->m_strText);
	}

	stInfoUnit.bPolarity = pFeatureT->m_bPolarity;

	stSelectInfo.vecSelectInfoUnit.push_back(stInfoUnit);
}

void CDrawHighlightShowFunc::SetSelectInfo(IN const CFeatureS* pFeatureS, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo)
{
	if (pFeatureS == nullptr) return;

	SELECT_INFO_UNIT stInfoUnit;
	stInfoUnit.strLayer = strLayerName;
	stInfoUnit.nFeatureIndex = nIndex + 1;
	stInfoUnit.emFeatureType = pFeatureS->m_eType;
	stInfoUnit.dCx = (pFeatureS->m_MinMaxRect.left + pFeatureS->m_MinMaxRect.right) / 2.0;
	stInfoUnit.dCy = (pFeatureS->m_MinMaxRect.top + pFeatureS->m_MinMaxRect.bottom) / 2.0;
	
	RECTD drtRect = pFeatureS->m_MinMaxRect;
	stInfoUnit.dWidth = drtRect.width();
	stInfoUnit.dHeight = drtRect.height();

	stInfoUnit.strSymbolName = _T("");
	stInfoUnit.bPolarity = pFeatureS->m_bPolarity;

	stInfoUnit.dDx = fabs((pFeatureS->m_MinMaxRect.right - pFeatureS->m_MinMaxRect.left) / 2.0);
	stInfoUnit.dDy = fabs((pFeatureS->m_MinMaxRect.bottom - pFeatureS->m_MinMaxRect.top) / 2.0);
	
	int nTempHoleCount = 0;
	int nTempIslandCount = 0;
	int nCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
	for (int i = 0; i < nCount; i++)
	{
		CObOe* pObOe = pFeatureS->m_arrObOe.GetAt(i);
		if (pObOe->m_bPolyType == true)
		{
			nTempIslandCount++;
		}
		else
		{
			nTempHoleCount++;
		}
	}

	stInfoUnit.nIslandCount = nTempIslandCount;
	stInfoUnit.nHoleCount = nTempHoleCount;

	//Att String
	stInfoUnit.vecAttTextString.clear();
	nCount = static_cast<int>(pFeatureS->m_arrAttributeTextString.GetCount());

	for (int i = 0; i < nCount; i++)
	{
		CAttributeTextString* pAttString = pFeatureS->m_arrAttributeTextString.GetAt(i);
		if (pAttString == nullptr) continue;
		stInfoUnit.vecAttTextString.push_back(pAttString->m_strText);
	}

	stSelectInfo.vecSelectInfoUnit.push_back(stInfoUnit);
}

void CDrawHighlightShowFunc::SetSelectInfo(IN const CObOe* pObOe, IN const int &nFeatureIndex, IN const int &nObOeIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo)
{
	if (pObOe == nullptr) return;

	SELECT_INFO_UNIT stInfoUnit;
	stInfoUnit.strLayer = strLayerName;
	stInfoUnit.nFeatureIndex = nFeatureIndex + 1;
	stInfoUnit.emFeatureType = FeatureType::S;
	stInfoUnit.dCx = (pObOe->m_MinMaxRect.left + pObOe->m_MinMaxRect.right) / 2.0;
	stInfoUnit.dCy = (pObOe->m_MinMaxRect.top + pObOe->m_MinMaxRect.bottom) / 2.0;

	RECTD drtRect = pObOe->m_MinMaxRect;
	stInfoUnit.dWidth = drtRect.width();
	stInfoUnit.dHeight = drtRect.height();

	stInfoUnit.strSymbolName = _T("");
	stInfoUnit.bPolarity = pObOe->m_bPolyType;

	stInfoUnit.dDx = fabs((pObOe->m_MinMaxRect.right - pObOe->m_MinMaxRect.left) / 2.0);
	stInfoUnit.dDy = fabs((pObOe->m_MinMaxRect.bottom - pObOe->m_MinMaxRect.top) / 2.0);
	stInfoUnit.nObOeIndex = nObOeIndex;

	stSelectInfo.vecSelectInfoUnit.push_back(stInfoUnit);
}

//////////////////////////////////////////////////////////////////////////

void CDrawHighlightShowFunc::DeleteSelectInfo(IN const CFeatureZ* pFeatureZ, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo)
{
	if (pFeatureZ == nullptr) return;

	SELECT_INFO_UNIT stInfoUnit;
	stInfoUnit.strLayer = strLayerName;
	stInfoUnit.nFeatureIndex = nIndex + 1;
	stInfoUnit.emFeatureType = pFeatureZ->m_eType;
	stInfoUnit.dCx = (pFeatureZ->m_dX + pFeatureZ->m_dX) / 2.0;
	stInfoUnit.dCy = (pFeatureZ->m_dY + pFeatureZ->m_dY) / 2.0;

	RECTD drtRect = pFeatureZ->m_MinMaxRect;
	stInfoUnit.dWidth = drtRect.width();
	stInfoUnit.dHeight = drtRect.height();

	if (pFeatureZ->m_pSymbol != nullptr)
	{
		stInfoUnit.strSymbolName = pFeatureZ->m_pSymbol->m_strUserSymbol;
	}

	stInfoUnit.bPolarity = pFeatureZ->m_bPolarity;

	int nSize = static_cast<int>(stSelectInfo.vecSelectInfoUnit.size());
	for (int i = 0; i < nSize; i++)
	{
		if (stInfoUnit == stSelectInfo.vecSelectInfoUnit[i])
		{
			stSelectInfo.vecSelectInfoUnit.erase(stSelectInfo.vecSelectInfoUnit.begin() + i);
			break;
		}
	}
}



void CDrawHighlightShowFunc::DeleteSelectInfo(IN const CFeatureL* pFeatureL, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo)
{
	if (pFeatureL == nullptr) return;

	SELECT_INFO_UNIT stInfoUnit;
	stInfoUnit.strLayer = strLayerName;
	stInfoUnit.nFeatureIndex = nIndex + 1;
	stInfoUnit.emFeatureType = pFeatureL->m_eType;
	stInfoUnit.dCx = (pFeatureL->m_dXe + pFeatureL->m_dXs) / 2.0;
	stInfoUnit.dCy = (pFeatureL->m_dYe + pFeatureL->m_dYs) / 2.0;

	RECTD drtRect = pFeatureL->m_MinMaxRect;
	stInfoUnit.dWidth = drtRect.width();
	stInfoUnit.dHeight = drtRect.height();

	if (pFeatureL->m_pSymbol != nullptr)
	{
		stInfoUnit.strSymbolName = pFeatureL->m_pSymbol->m_strUserSymbol;
	}

	stInfoUnit.bPolarity = pFeatureL->m_bPolarity;

	int nSize = static_cast<int>(stSelectInfo.vecSelectInfoUnit.size());
	for (int i = 0; i < nSize; i++)
	{
		if (stInfoUnit == stSelectInfo.vecSelectInfoUnit[i])
		{
			stSelectInfo.vecSelectInfoUnit.erase(stSelectInfo.vecSelectInfoUnit.begin() + i);
			break;
		}
	}
}

void CDrawHighlightShowFunc::DeleteSelectInfo(IN const CFeatureP* pFeatureP, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo)
{
	if (pFeatureP == nullptr) return;

	SELECT_INFO_UNIT stInfoUnit;
	stInfoUnit.strLayer = strLayerName;
	stInfoUnit.nFeatureIndex = nIndex + 1;
	stInfoUnit.emFeatureType = pFeatureP->m_eType;
	stInfoUnit.dCx = pFeatureP->m_dX;
	stInfoUnit.dCy = pFeatureP->m_dY;

	RECTD drtRect = pFeatureP->m_MinMaxRect;
	stInfoUnit.dWidth = drtRect.width();
	stInfoUnit.dHeight = drtRect.height();

	if (pFeatureP->m_pSymbol != nullptr)
	{
		stInfoUnit.strSymbolName = pFeatureP->m_pSymbol->m_strUserSymbol;
	}

	stInfoUnit.bPolarity = pFeatureP->m_bPolarity;

	int nSize = static_cast<int>(stSelectInfo.vecSelectInfoUnit.size());
	for (int i = 0; i < nSize; i++)
	{
		if (stInfoUnit == stSelectInfo.vecSelectInfoUnit[i])
		{
			stSelectInfo.vecSelectInfoUnit.erase(stSelectInfo.vecSelectInfoUnit.begin() + i);
			break;
		}
	}
}

void CDrawHighlightShowFunc::DeleteSelectInfo(IN const CFeatureA* pFeatureA, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo)
{
	if (pFeatureA == nullptr) return;

	SELECT_INFO_UNIT stInfoUnit;
	stInfoUnit.strLayer = strLayerName;
	stInfoUnit.nFeatureIndex = nIndex + 1;
	stInfoUnit.emFeatureType = pFeatureA->m_eType;
	stInfoUnit.dCx = pFeatureA->m_dXc;
	stInfoUnit.dCy = pFeatureA->m_dYc;

	RECTD drtRect = pFeatureA->m_MinMaxRect;
	stInfoUnit.dWidth = drtRect.width();
	stInfoUnit.dHeight = drtRect.height();

	if (pFeatureA->m_pSymbol != nullptr)
	{
		stInfoUnit.strSymbolName = pFeatureA->m_pSymbol->m_strUserSymbol;
	}

	stInfoUnit.bPolarity = pFeatureA->m_bPolarity;

	int nSize = static_cast<int>(stSelectInfo.vecSelectInfoUnit.size());
	for (int i = 0; i < nSize; i++)
	{
		if (stInfoUnit == stSelectInfo.vecSelectInfoUnit[i])
		{
			stSelectInfo.vecSelectInfoUnit.erase(stSelectInfo.vecSelectInfoUnit.begin() + i);
			break;
		}
	}
}

void CDrawHighlightShowFunc::DeleteSelectInfo(IN const CFeatureT* pFeatureT, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo)
{
	if (pFeatureT == nullptr) return;

	SELECT_INFO_UNIT stInfoUnit;
	stInfoUnit.strLayer = strLayerName;
	stInfoUnit.nFeatureIndex = nIndex + 1;
	stInfoUnit.emFeatureType = pFeatureT->m_eType;
	stInfoUnit.dCx = pFeatureT->m_dX;
	stInfoUnit.dCy = pFeatureT->m_dY;

	RECTD drtRect = pFeatureT->m_MinMaxRect;
	stInfoUnit.dWidth = drtRect.width();
	stInfoUnit.dHeight = drtRect.height();

	if (pFeatureT->m_pSymbol != nullptr)
	{
		stInfoUnit.strSymbolName = pFeatureT->m_pSymbol->m_strUserSymbol;
	}

	stInfoUnit.bPolarity = pFeatureT->m_bPolarity;

	int nSize = static_cast<int>(stSelectInfo.vecSelectInfoUnit.size());
	for (int i = 0; i < nSize; i++)
	{
		if (stInfoUnit == stSelectInfo.vecSelectInfoUnit[i])
		{
			stSelectInfo.vecSelectInfoUnit.erase(stSelectInfo.vecSelectInfoUnit.begin() + i);
			break;
		}
	}
}

void CDrawHighlightShowFunc::DeleteSelectInfo(IN const CFeatureS* pFeatureS, IN const int &nIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo)
{
	if (pFeatureS == nullptr) return;

	SELECT_INFO_UNIT stInfoUnit;
	stInfoUnit.strLayer = strLayerName;
	stInfoUnit.nFeatureIndex = nIndex + 1;
	stInfoUnit.emFeatureType = pFeatureS->m_eType;
	stInfoUnit.dCx = (pFeatureS->m_MinMaxRect.left + pFeatureS->m_MinMaxRect.right) / 2.0;
	stInfoUnit.dCy = (pFeatureS->m_MinMaxRect.top + pFeatureS->m_MinMaxRect.bottom) / 2.0;

	RECTD drtRect = pFeatureS->m_MinMaxRect;
	stInfoUnit.dWidth = drtRect.width();
	stInfoUnit.dHeight = drtRect.height();

	stInfoUnit.strSymbolName = _T("");
	stInfoUnit.bPolarity = pFeatureS->m_bPolarity;

	stInfoUnit.dDx = fabs((pFeatureS->m_MinMaxRect.right - pFeatureS->m_MinMaxRect.left) / 2.0);
	stInfoUnit.dDy = fabs((pFeatureS->m_MinMaxRect.bottom - pFeatureS->m_MinMaxRect.top) / 2.0);

	int nTempHoleCount = 0;
	int nTempIslandCount = 0;
	int nCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
	for (int i = 0; i < nCount; i++)
	{
		CObOe* pObOe = pFeatureS->m_arrObOe.GetAt(i);
		if (pObOe->m_bPolyType == true)
		{
			nTempIslandCount++;
		}
		else
		{
			nTempHoleCount++;
		}
	}

	stInfoUnit.nIslandCount = nTempIslandCount;
	stInfoUnit.nHoleCount = nTempHoleCount;

	int nSize = static_cast<int>(stSelectInfo.vecSelectInfoUnit.size());
	for (int i = 0; i < nSize; i++)
	{
		if (stInfoUnit == stSelectInfo.vecSelectInfoUnit[i])
		{
			stSelectInfo.vecSelectInfoUnit.erase(stSelectInfo.vecSelectInfoUnit.begin() + i);
			break;
		}
	}
}

void CDrawHighlightShowFunc::DeleteSelectInfo(IN const CObOe* pObOe, IN const int &nFeatureIndex, IN const int &nObOeIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo)
{
	if (pObOe == nullptr) return;

	SELECT_INFO_UNIT stInfoUnit;
	stInfoUnit.strLayer = strLayerName;
	stInfoUnit.nFeatureIndex = nFeatureIndex + 1;
	stInfoUnit.emFeatureType = FeatureType::S;
	stInfoUnit.dCx = (pObOe->m_MinMaxRect.left + pObOe->m_MinMaxRect.right) / 2.0;
	stInfoUnit.dCy = (pObOe->m_MinMaxRect.top + pObOe->m_MinMaxRect.bottom) / 2.0;

	RECTD drtRect = pObOe->m_MinMaxRect;
	stInfoUnit.dWidth = drtRect.width();
	stInfoUnit.dHeight = drtRect.height();

	stInfoUnit.strSymbolName = _T("");
	stInfoUnit.bPolarity = pObOe->m_bPolyType;

	stInfoUnit.dDx = fabs((pObOe->m_MinMaxRect.right - pObOe->m_MinMaxRect.left) / 2.0);
	stInfoUnit.dDy = fabs((pObOe->m_MinMaxRect.bottom - pObOe->m_MinMaxRect.top) / 2.0);
	stInfoUnit.nObOeIndex = nObOeIndex;

	int nSize = static_cast<int>(stSelectInfo.vecSelectInfoUnit.size());
	for (int i = 0; i < nSize; i++)
	{
		if (stInfoUnit == stSelectInfo.vecSelectInfoUnit[i])
		{
			stSelectInfo.vecSelectInfoUnit.erase(stSelectInfo.vecSelectInfoUnit.begin() + i);
			break;
		}
	}

}

void CDrawHighlightShowFunc::_SetSurface_byName(IN const SurfaceMode &emMode, IN const CFeatureS* pFeatureS, IN const int &nFeatureIndex, IN const CString &strLayerName, SELECT_INFO &stSelectInfo)
{
	switch (emMode)
	{
	case enumSurfaceMode_Select:
	{
		int nSurface = static_cast<int>(pFeatureS->m_arrObOe.GetSize());
		for (int i = 0; i < nSurface; i++)
		{

			CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(i);


			if (pObOe->m_bPolyType == true)
			{
				pObOe->m_bHighlight = true;
				SetSelectInfo(pFeatureS, nFeatureIndex, strLayerName, stSelectInfo);
			}
		}
	}
		break;
	case enumSurfaceMode_UnSelect:
	{
		int nSurface = static_cast<int>(pFeatureS->m_arrObOe.GetSize());
		for (int i = 0; i < nSurface; i++)
		{

			CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(i);
			if (pObOe->m_bPolyType == true)
			{
				pObOe->m_bHighlight = false;
				DeleteSelectInfo(pFeatureS, nFeatureIndex, strLayerName, stSelectInfo);
			}
		}
	}
		break;
	case enumSurfaceMode_Show:
	{
		int nSurface = static_cast<int>(pFeatureS->m_arrObOe.GetSize());
		for (int i = 0; i < nSurface; i++)
		{

			CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(i);
			if (pObOe->m_bPolyType == true)
			{
				pObOe->m_bShow = true;
			}
		}
	}
		break;
	case enumSurfaceMode_Hide:
	{
		int nSurface = static_cast<int>(pFeatureS->m_arrObOe.GetSize());
		for (int i = 0; i < nSurface; i++)
		{

			CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(i);
			if (pObOe->m_bPolyType == true)
			{
				pObOe->m_bShow = false;
			}
		}
	}
		break;
	default:
		break;
	}
	
}

void		CDrawHighlightShowFunc::_SetFeature_byName(IN const SurfaceMode &emMode, IN const CLayer *pLayer, FEATURE_INFO &stFeatureInfo, SELECT_INFO &stSelectInfo)
{
	switch (emMode)
	{
	case enumSurfaceMode_Select:
	{
		//Feature 선택
		int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());

		for (int nFeature = 0; nFeature < nFeatureCount; nFeature++)
		{
			CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nFeature);
			if (pFeature == nullptr) continue;

			//비교 시작
			if (pFeature->m_eType == stFeatureInfo.eType)
			{

				if (stFeatureInfo.vecAttString.size() == 0 ||
					stFeatureInfo.Compare_AttString(pFeature->m_arrAttributeTextString) == TRUE)
				{

					if (stFeatureInfo.strSymbolName == _T("") ||
						(pFeature->m_pSymbol != nullptr &&
							stFeatureInfo.strSymbolName == pFeature->m_pSymbol->m_strUserSymbol))
					{
						switch (pFeature->m_eType)
						{
						case FeatureType::Z:
						{
							CFeatureZ *pFeatureZ = (CFeatureZ *)pFeature;
							pFeatureZ->m_bHighlight = true;
						}
						break;
						case FeatureType::L:
						{
							CFeatureL *pFeatureL = (CFeatureL *)pFeature;
							pFeatureL->m_bHighlight = true;
						}
						break;
						case FeatureType::P:
						{
							CFeatureP *pFeatureP = (CFeatureP *)pFeature;
							pFeatureP->m_bHighlight = true;
						}
						break;
						case FeatureType::A:
						{
							CFeatureA *pFeatureA = (CFeatureA *)pFeature;
							pFeatureA->m_bHighlight = true;;
						}
						break;
						case FeatureType::T:
						{
							CFeatureT *pFeatureT = (CFeatureT *)pFeature;
							pFeatureT->m_bHighlight = true;
						}
						break;
						default:
							break;
						}

					}
				}
			}


		}
	}
	break;
	case enumSurfaceMode_UnSelect:
	{
		//Feature 선택
		int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());

		for (int nFeature = 0; nFeature < nFeatureCount; nFeature++)
		{
			CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nFeature);
			if (pFeature == nullptr) continue;

			//비교 시작
			if (pFeature->m_eType == stFeatureInfo.eType)
			{

				if (stFeatureInfo.vecAttString.size() == 0 ||
					stFeatureInfo.Compare_AttString(pFeature->m_arrAttributeTextString) == TRUE)
				{

					if (stFeatureInfo.strSymbolName == _T("") ||
						(pFeature->m_pSymbol != nullptr &&
							stFeatureInfo.strSymbolName == pFeature->m_pSymbol->m_strUserSymbol))
					{
						switch (pFeature->m_eType)
						{
						case FeatureType::Z:
						{
							CFeatureZ *pFeatureZ = (CFeatureZ *)pFeature;
							pFeatureZ->m_bHighlight = true;
						}
						break;
						case FeatureType::L:
						{
							CFeatureL *pFeatureL = (CFeatureL *)pFeature;
							pFeatureL->m_bHighlight = false;
						}
						break;
						case FeatureType::P:
						{
							CFeatureP *pFeatureP = (CFeatureP *)pFeature;
							pFeatureP->m_bHighlight = false;
						}
						break;
						case FeatureType::A:
						{
							CFeatureA *pFeatureA = (CFeatureA *)pFeature;
							pFeatureA->m_bHighlight = false;
						}
						break;
						case FeatureType::T:
						{
							CFeatureT *pFeatureT = (CFeatureT *)pFeature;
							pFeatureT->m_bHighlight = false;
						}
						break;
						default:
							break;
						}

					}
				}
			}


		}
	}
	break;
	case enumSurfaceMode_Show:
	{
		//Feature 선택
		int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());

		for (int nFeature = 0; nFeature < nFeatureCount; nFeature++)
		{
			CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nFeature);
			if (pFeature == nullptr) continue;

			//비교 시작
			if (pFeature->m_eType == stFeatureInfo.eType)
			{

				if (stFeatureInfo.vecAttString.size() == 0 ||
					stFeatureInfo.Compare_AttString(pFeature->m_arrAttributeTextString) == TRUE)
				{

					if (stFeatureInfo.strSymbolName == _T("") ||
						(pFeature->m_pSymbol != nullptr &&
							stFeatureInfo.strSymbolName == pFeature->m_pSymbol->m_strUserSymbol))
					{
						switch (pFeature->m_eType)
						{
						case FeatureType::Z:
						{
							CFeatureZ *pFeatureZ = (CFeatureZ *)pFeature;
							pFeatureZ->m_bShow = true;
						}
						break;
						case FeatureType::L:
						{
							CFeatureL *pFeatureL = (CFeatureL *)pFeature;
							pFeatureL->m_bShow = true;
						}
						break;
						case FeatureType::P:
						{
							CFeatureP *pFeatureP = (CFeatureP *)pFeature;
							pFeatureP->m_bShow = true;
						}
						break;
						case FeatureType::A:
						{
							CFeatureA *pFeatureA = (CFeatureA *)pFeature;
							pFeatureA->m_bShow = true;
						}
						break;
						case FeatureType::T:
						{
							CFeatureT *pFeatureT = (CFeatureT *)pFeature;
							pFeatureT->m_bShow = true;
						}
						break;
						default:
							break;
						}

					}
				}
			}


		}
	}
	break;
	case enumSurfaceMode_Hide:
	{
		//Feature 선택
		int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());

		for (int nFeature = 0; nFeature < nFeatureCount; nFeature++)
		{
			CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nFeature);
			if (pFeature == nullptr) continue;

			//비교 시작
			if (pFeature->m_eType == stFeatureInfo.eType)
			{

				if (stFeatureInfo.vecAttString.size() == 0 ||
					stFeatureInfo.Compare_AttString(pFeature->m_arrAttributeTextString) == TRUE)
				{

					if (stFeatureInfo.strSymbolName == _T("") ||
						(pFeature->m_pSymbol != nullptr &&
							stFeatureInfo.strSymbolName == pFeature->m_pSymbol->m_strUserSymbol))
					{
						switch (pFeature->m_eType)
						{
						case FeatureType::Z:
						{
							CFeatureZ *pFeatureZ = (CFeatureZ *)pFeature;
							pFeatureZ->m_bShow = false;
						}
						break;
						case FeatureType::L:
						{
							CFeatureL *pFeatureL = (CFeatureL *)pFeature;
							pFeatureL->m_bShow = false;
						}
						break;
						case FeatureType::P:
						{
							CFeatureP *pFeatureP = (CFeatureP *)pFeature;
							pFeatureP->m_bShow = false;
						}
						break;
						case FeatureType::A:
						{
							CFeatureA *pFeatureA = (CFeatureA *)pFeature;
							pFeatureA->m_bShow = false;
						}
						break;
						case FeatureType::T:
						{
							CFeatureT *pFeatureT = (CFeatureT *)pFeature;
							pFeatureT->m_bShow = false;
						}
						break;

						default:
							break;
						}

					}
				}
			}


		}
	}
	break;
	default:
		break;
	}
}

void		CDrawHighlightShowFunc::_SetFeature_byAtt(IN const SurfaceMode &emMode, IN const CLayer *pLayer, FEATURE_INFO &stFeatureInfo, SELECT_INFO &stSelectInfo)
{
	switch (emMode)
	{
	case enumSurfaceMode_Select:
	{
		//Feature 선택
		int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());

		for (int nFeature = 0; nFeature < nFeatureCount; nFeature++)
		{
			CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nFeature);
			if (pFeature == nullptr) continue;

			//비교 시작
			if (pFeature->m_eType == stFeatureInfo.eType)
			{

				if (stFeatureInfo.Compare_AttString(pFeature->m_arrAttributeTextString) == TRUE)
				{


					{
						switch (pFeature->m_eType)
						{
						case FeatureType::Z:
						{
							CFeatureZ *pFeatureZ = (CFeatureZ *)pFeature;
							pFeatureZ->m_bHighlight = true;
						}
						break;
						case FeatureType::L:
						{
							CFeatureL *pFeatureL = (CFeatureL *)pFeature;
							pFeatureL->m_bHighlight = true;
						}
						break;
						case FeatureType::P:
						{
							CFeatureP *pFeatureP = (CFeatureP *)pFeature;
							pFeatureP->m_bHighlight = true;
						}
						break;
						case FeatureType::A:
						{
							CFeatureA *pFeatureA = (CFeatureA *)pFeature;
							pFeatureA->m_bHighlight = true;
						}
						break;
						case FeatureType::T:
						{
							CFeatureT *pFeatureT = (CFeatureT *)pFeature;
							pFeatureT->m_bHighlight = true;
						}
						break;

						break;
						default:
							break;
						}

					}
				}
			}


		}
	}
	break;
	case enumSurfaceMode_UnSelect:
	{
		//Feature 선택
		int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());

		for (int nFeature = 0; nFeature < nFeatureCount; nFeature++)
		{
			CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nFeature);
			if (pFeature == nullptr) continue;

			//비교 시작
			if (pFeature->m_eType == stFeatureInfo.eType)
			{

				if (stFeatureInfo.Compare_AttString(pFeature->m_arrAttributeTextString) == TRUE)
				{


					{
						switch (pFeature->m_eType)
						{
						case FeatureType::Z:
						{
							CFeatureZ *pFeatureZ = (CFeatureZ *)pFeature;
							pFeatureZ->m_bHighlight = false;
							break;
						}
						case FeatureType::L:
						{
							CFeatureL *pFeatureL = (CFeatureL *)pFeature;
							pFeatureL->m_bHighlight = false;
						}
						break;
						case FeatureType::P:
						{
							CFeatureP *pFeatureP = (CFeatureP *)pFeature;
							pFeatureP->m_bHighlight = false;
						}
						break;
						case FeatureType::A:
						{
							CFeatureA *pFeatureA = (CFeatureA *)pFeature;
							pFeatureA->m_bHighlight = false;
						}
						break;
						case FeatureType::T:
						{
							CFeatureT *pFeatureT = (CFeatureT *)pFeature;
							pFeatureT->m_bHighlight = false;
						}
						break;

						break;
						default:
							break;
						}

					}
				}
			}


		}
	}
	break;
	case enumSurfaceMode_Show:
	{
		//Feature 선택
		int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());

		for (int nFeature = 0; nFeature < nFeatureCount; nFeature++)
		{
			CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nFeature);
			if (pFeature == nullptr) continue;

			//비교 시작
			if (pFeature->m_eType == stFeatureInfo.eType)
			{

				if (stFeatureInfo.Compare_AttString(pFeature->m_arrAttributeTextString) == TRUE)
				{


					{
						switch (pFeature->m_eType)
						{
						case FeatureType::Z:
						{
							CFeatureZ *pFeatureZ = (CFeatureZ *)pFeature;
							pFeatureZ->m_bShow = true;
						}
						break;
						case FeatureType::L:
						{
							CFeatureL *pFeatureL = (CFeatureL *)pFeature;
							pFeatureL->m_bShow = true;
						}
						break;
						case FeatureType::P:
						{
							CFeatureP *pFeatureP = (CFeatureP *)pFeature;
							pFeatureP->m_bShow = true;
						}
						break;
						case FeatureType::A:
						{
							CFeatureA *pFeatureA = (CFeatureA *)pFeature;
							pFeatureA->m_bShow = true;
						}
						break;
						case FeatureType::T:
						{
							CFeatureT *pFeatureT = (CFeatureT *)pFeature;
							pFeatureT->m_bShow = true;
						}
						break;

						break;
						default:
							break;
						}

					}
				}
			}
		}
	}
	break;
	case enumSurfaceMode_Hide:
	{
		//Feature 선택
		int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());

		for (int nFeature = 0; nFeature < nFeatureCount; nFeature++)
		{
			CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nFeature);
			if (pFeature == nullptr) continue;

			//비교 시작
			if (pFeature->m_eType == stFeatureInfo.eType)
			{

				if (stFeatureInfo.Compare_AttString(pFeature->m_arrAttributeTextString) == TRUE)
				{


					{
						switch (pFeature->m_eType)
						{
						case FeatureType::Z:
						{
							CFeatureZ *pFeatureZ = (CFeatureZ *)pFeature;
							pFeatureZ->m_bShow = false;
						}
						break;
						case FeatureType::L:
						{
							CFeatureL *pFeatureL = (CFeatureL *)pFeature;
							pFeatureL->m_bShow = false;
						}
						break;
						case FeatureType::P:
						{
							CFeatureP *pFeatureP = (CFeatureP *)pFeature;
							pFeatureP->m_bShow = false;
						}
						break;
						case FeatureType::A:
						{
							CFeatureA *pFeatureA = (CFeatureA *)pFeature;
							pFeatureA->m_bShow = false;
						}
						break;
						case FeatureType::T:
						{
							CFeatureT *pFeatureT = (CFeatureT *)pFeature;
							pFeatureT->m_bShow = false;
						}
						break;

						break;
						default:
							break;
						}

					}
				}
			}
		}
	}
	break;
	}
}

void		CDrawHighlightShowFunc::_SetFeature_bySymbol(IN const SurfaceMode &emMode, IN const CLayer *pLayer, FEATURE_INFO &stFeatureInfo, SELECT_INFO &stSelectInfo)
{
	switch (emMode)
	{
	case enumSurfaceMode_Select:
	{
		//Feature 선택
		int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());

		for (int nFeature = 0; nFeature < nFeatureCount; nFeature++)
		{
			CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nFeature);
			if (pFeature == nullptr) continue;

			//비교 시작
			if (pFeature->m_eType == stFeatureInfo.eType)
			{

				if ( (pFeature->m_pSymbol == nullptr && stFeatureInfo.strSymbolName == DEF_HISTOGRAM::strSymNull )
					||
					(pFeature->m_pSymbol != nullptr && stFeatureInfo.strSymbolName == pFeature->m_pSymbol->m_strUserSymbol) )
				{


					{
						switch (pFeature->m_eType)
						{
						case FeatureType::Z:
						{
							CFeatureZ *pFeatureZ = (CFeatureZ *)pFeature;
							pFeatureZ->m_bHighlight = true;
						}
						break;
						case FeatureType::L:
						{
							CFeatureL *pFeatureL = (CFeatureL *)pFeature;
							pFeatureL->m_bHighlight = true;
						}
						break;
						case FeatureType::P:
						{
							CFeatureP *pFeatureP = (CFeatureP *)pFeature;
							pFeatureP->m_bHighlight = true;
						}
						break;
						case FeatureType::A:
						{
							CFeatureA *pFeatureA = (CFeatureA *)pFeature;
							pFeatureA->m_bHighlight = true;
						}
						break;
						case FeatureType::T:
						{
							CFeatureT *pFeatureT = (CFeatureT *)pFeature;
							pFeatureT->m_bHighlight = true;
						}
						break;

						break;
						default:
							break;
						}

					}
				}
			}


		}
	}
	break;
	case enumSurfaceMode_UnSelect:
	{
		//Feature 선택
		int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());

		for (int nFeature = 0; nFeature < nFeatureCount; nFeature++)
		{
			CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nFeature);
			if (pFeature == nullptr) continue;

			//비교 시작
			if (pFeature->m_eType == stFeatureInfo.eType)
			{

				if ((pFeature->m_pSymbol == nullptr && stFeatureInfo.strSymbolName == DEF_HISTOGRAM::strSymNull)
					||
					(pFeature->m_pSymbol != nullptr && stFeatureInfo.strSymbolName == pFeature->m_pSymbol->m_strUserSymbol))
				{


					{
						switch (pFeature->m_eType)
						{
						case FeatureType::Z:
						{
							CFeatureZ *pFeatureZ = (CFeatureZ *)pFeature;
							pFeatureZ->m_bHighlight = false;
						}
						break;
						case FeatureType::L:
						{
							CFeatureL *pFeatureL = (CFeatureL *)pFeature;
							pFeatureL->m_bHighlight = false;
						}
						break;
						case FeatureType::P:
						{
							CFeatureP *pFeatureP = (CFeatureP *)pFeature;
							pFeatureP->m_bHighlight = false;
						}
						break;
						case FeatureType::A:
						{
							CFeatureA *pFeatureA = (CFeatureA *)pFeature;
							pFeatureA->m_bHighlight = false;
						}
						break;
						case FeatureType::T:
						{
							CFeatureT *pFeatureT = (CFeatureT *)pFeature;
							pFeatureT->m_bHighlight = false;
						}
						break;

						break;
						default:
							break;
						}

					}
				}
			}
		}

	}
	break;
	case enumSurfaceMode_Show:
	{
		//Feature 선택
		int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());

		for (int nFeature = 0; nFeature < nFeatureCount; nFeature++)
		{
			CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nFeature);
			if (pFeature == nullptr) continue;

			//비교 시작
			if (pFeature->m_eType == stFeatureInfo.eType)
			{

				if ((pFeature->m_pSymbol == nullptr && stFeatureInfo.strSymbolName == DEF_HISTOGRAM::strSymNull)
					||
					(pFeature->m_pSymbol != nullptr && stFeatureInfo.strSymbolName == pFeature->m_pSymbol->m_strUserSymbol))
				{


					{
						switch (pFeature->m_eType)
						{
						case FeatureType::Z:
						{
							CFeatureZ *pFeatureZ = (CFeatureZ *)pFeature;
							pFeatureZ->m_bShow = true;
						}
						break;
						case FeatureType::L:
						{
							CFeatureL *pFeatureL = (CFeatureL *)pFeature;
							pFeatureL->m_bShow = true;
						}
						break;
						case FeatureType::P:
						{
							CFeatureP *pFeatureP = (CFeatureP *)pFeature;
							pFeatureP->m_bShow = true;
						}
						break;
						case FeatureType::A:
						{
							CFeatureA *pFeatureA = (CFeatureA *)pFeature;
							pFeatureA->m_bShow = true;
						}
						break;
						case FeatureType::T:
						{
							CFeatureT *pFeatureT = (CFeatureT *)pFeature;
							pFeatureT->m_bShow = true;
						}
						break;

						break;
						default:
							break;
						}

					}
				}
			}
		}

	}
	break;
	case enumSurfaceMode_Hide:
	{
		//Feature 선택
		int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());

		for (int nFeature = 0; nFeature < nFeatureCount; nFeature++)
		{
			CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nFeature);
			if (pFeature == nullptr) continue;

			//비교 시작
			if (pFeature->m_eType == stFeatureInfo.eType)
			{

				if ((pFeature->m_pSymbol == nullptr && stFeatureInfo.strSymbolName == DEF_HISTOGRAM::strSymNull)
					||
					(pFeature->m_pSymbol != nullptr && stFeatureInfo.strSymbolName == pFeature->m_pSymbol->m_strUserSymbol))
				{


					{
						switch (pFeature->m_eType)
						{
						case FeatureType::Z:
						{
							CFeatureZ *pFeatureZ = (CFeatureZ *)pFeature;
							pFeatureZ->m_bShow = false;
						}
						break;
						case FeatureType::L:
						{
							CFeatureL *pFeatureL = (CFeatureL *)pFeature;
							pFeatureL->m_bShow = false;
						}
						break;
						case FeatureType::P:
						{
							CFeatureP *pFeatureP = (CFeatureP *)pFeature;
							pFeatureP->m_bShow = false;
						}
						break;
						case FeatureType::A:
						{
							CFeatureA *pFeatureA = (CFeatureA *)pFeature;
							pFeatureA->m_bShow = false;
						}
						break;
						case FeatureType::T:
						{
							CFeatureT *pFeatureT = (CFeatureT *)pFeature;
							pFeatureT->m_bShow = false;
						}
						break;

						break;
						default:
							break;
						}

					}
				}
			}
		}
	}
	break;
	}
}