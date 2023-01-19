
#include "pch.h"
#include "CFeatureFilterFunction.h"
#include "CDrawMakePoint.h"
#include "CDrawFunction.h"

CFeatureFilterFunction::CFeatureFilterFunction()
{
	m_pDrawFunction = new CDrawFunction;
}

CFeatureFilterFunction::~CFeatureFilterFunction()
{
	if (m_pDrawFunction != nullptr)
	{
		delete m_pDrawFunction;
		m_pDrawFunction = nullptr;
	}
}

void CFeatureFilterFunction::SetLink(ID2D1Factory1 *pFactory, ID2D1DeviceContext* pD2DContext)
{
	m_pFactory = pFactory;
	m_pD2DContext = pD2DContext;

	m_pDrawFunction->_SetLink(pFactory, pD2DContext);
}

BOOL CFeatureFilterFunction::CompareFeature(IN CLayer *pLayer, IN const int &nFeatureIndex_1, IN const int &nFeatureIndex_2)
{
	BOOL bCompare = FALSE;

	if (pLayer == nullptr) return bCompare;

	CFeature *pFeaure_1 = pLayer->m_FeatureFile.m_arrFeature.GetAt(nFeatureIndex_1);
	if (pFeaure_1 == nullptr) return FALSE;
	CFeature *pFeaure_2 = pLayer->m_FeatureFile.m_arrFeature.GetAt(nFeatureIndex_2);
	if (pFeaure_2 == nullptr) return FALSE;

	if (pFeaure_1->m_eType != pFeaure_2->m_eType) return FALSE;

	

	switch (pFeaure_1->m_eType)
	{
	case FeatureType::L:
	{
	}
	break;
	case FeatureType::P:
	{
	}
	break;
	case FeatureType::A:
	{
	}
	break;
	case FeatureType::S:
	{
		CFeatureS *pFeatureS_1 = (CFeatureS*)pFeaure_1;
		CFeatureS *pFeatureS_2 = (CFeatureS*)pFeaure_2;

		bCompare = CompareSurface(pFeatureS_1, pFeatureS_2);
	}
	break;
	default:
	{
		bCompare = FALSE;
	}
	break;
	}

	return bCompare;
}

BOOL CFeatureFilterFunction::CompareSurface(CFeatureS *pFeaureS_1, CFeatureS *pFeaureS_2)
{
	if (m_pDrawFunction == nullptr) return FALSE;
	BOOL bCompare = FALSE;
	/*
	

	vector<CFeatureS *> vecFeatureS;//지우면 안됨
	vecFeatureS.push_back(pFeaureS_1);
	vecFeatureS.push_back(pFeaureS_2);
	int nCompareCount = static_cast<int>(vecFeatureS.size());


	CRotMirrArr RotMirr;

	vector<MakeFeatureData> vecGeoDraw;
	vector<vector<DrawFeatureGeo>> vecvecDrawFeatureGeo;
	vector<ID2D1PathGeometry *> vecGeo;
	vecvecDrawFeatureGeo.resize(nCompareCount);
	vecGeo.resize(nCompareCount);
	//Geo 생성
	for (int nfeno = 0 ; nfeno < nCompareCount ; nfeno++)
	{
		//Feature중심이 0,0이 되도록 그려주기 위함
		RotMirr.InputData(vecFeatureS[nfeno]->m_MinMaxRect.CenterX() * -1, vecFeatureS[nfeno]->m_MinMaxRect.CenterY() * -1.0, Orient::Mir0Deg);

		vecGeoDraw = CDrawMakePoint::MakeSurface(vecFeatureS[nfeno], &RotMirr, RECTD());
		int nCount = static_cast<int>(vecGeoDraw.size());

		vecvecDrawFeatureGeo[nfeno].resize(nCount);
		for (int i = 0; i < nCount; i++)
		{
			vecvecDrawFeatureGeo[nfeno][i].bIsUsed = true;
			vecvecDrawFeatureGeo[nfeno][i].bPolarity = vecGeoDraw[i].bPolarity;

			if (!vecGeoDraw[i].bIsUsed)
				continue;

			m_pDrawFunction->_SetGeometry(vecGeoDraw[i].vecPointData, &vecvecDrawFeatureGeo[nfeno][i].pGeo);
		}

		vecGeo[nfeno] = m_pDrawFunction->_CombineSurface(vecvecDrawFeatureGeo[nfeno], TRUE);

		RotMirr.ClearAll();
	}

	//Compare
	vector<ID2D1PathGeometry *> vecGeo_Temp;
	vector<ID2D1GeometrySink *> pSink_Temp;
	vecGeo_Temp.resize(nCompareCount);
	pSink_Temp.resize(nCompareCount);
	for (int nfeno = 0; nfeno < nCompareCount; nfeno++)
	{
		m_pFactory->CreatePathGeometry(&vecGeo_Temp[nfeno]);
	}

	HRESULT hr = S_FALSE;
	hr = vecGeo_Temp[0]->Open(&pSink_Temp[0]);
	vecGeo[0]->CombineWithGeometry(vecGeo[1], (D2D1_COMBINE_MODE)D2D1_COMBINE_MODE_XOR, NULL, NULL, pSink_Temp[0]);
	pSink_Temp[0]->Close();

	float fArea = 999.;
	hr = vecGeo_Temp[0]->ComputeArea(D2D1::Matrix3x2F(), &fArea);

	CString str;
	str.Format(_T("\nCompare Area : %.3lf\n"), fArea);
	TRACE(str);

	if (fArea < 1.0E-6)
	{
		bCompare = TRUE;
	}
	else
	{
		bCompare = FALSE;
	}

	//delete
	for (int nfeno = 0; nfeno < nCompareCount; nfeno++)
	{
		int nCount = static_cast<int>(vecvecDrawFeatureGeo[nfeno].size());
		for (int i = 0; i < nCount; i++)
		{
			SafeRelease(&vecvecDrawFeatureGeo[nfeno][i].pGeo);
		}

		SafeRelease(&vecGeo[nfeno]);

			SafeRelease(&pSink_Temp[nfeno]);
			SafeRelease(&vecGeo_Temp[nfeno]);
		
	}
	*/

	return bCompare;
}
