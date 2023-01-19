#include "pch.h"
#include "CDrawMakeGeo.h"

#include "CDrawMakePoint.h"

CDrawMakeGeo::CDrawMakeGeo()
{

}

CDrawMakeGeo::~CDrawMakeGeo()
{

}

void CDrawMakeGeo::SetLink(ID2D1Factory1 *pFactory)
{
	m_pFactory = pFactory;
}

UINT32 CDrawMakeGeo::SetGeometry(vector<GEODRAW> &vecGeoData, ID2D1PathGeometry **pGeoObject, BOOL bOpen)
{
	if (*pGeoObject != nullptr)
	{
		(*pGeoObject)->Release();
		(*pGeoObject) = nullptr;

	}
	m_pFactory->CreatePathGeometry(pGeoObject);

	ID2D1GeometrySink *pSink = NULL;

	HRESULT hr;
	hr = (*pGeoObject)->Open(&pSink);
	int nPtCount = static_cast<int>(vecGeoData.size());

	D2D_POINT_2F ptLastPoint;

	pSink->SetFillMode(D2D1_FILL_MODE_WINDING);
	for (int j = 0; j < nPtCount; j++)
	{
		switch (vecGeoData[j].nType)
		{
		case OdbPointType::enumBase:

			if (j != 0)
			{
				if (bOpen == TRUE)
				{
					pSink->EndFigure(D2D1_FIGURE_END_OPEN);
				}
				else
				{
					pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
				}
			}

			pSink->BeginFigure(
				D2D1::Point2F(vecGeoData[j].ptPoint.x, vecGeoData[j].ptPoint.y),
				D2D1_FIGURE_BEGIN_FILLED
			);


			break;
		case OdbPointType::enumLine:
			pSink->AddLine(D2D1::Point2F(vecGeoData[j].ptPoint.x, vecGeoData[j].ptPoint.y));
			break;
		case OdbPointType::enumArc:
		{
			double dStartAngle = CDrawMakePoint::GetAngle(vecGeoData[j].ptCenter, ptLastPoint);
			double dEndAngle = CDrawMakePoint::GetAngle(vecGeoData[j].ptCenter, vecGeoData[j].ptPoint);

			double dCurDistX = CDrawMakePoint::GetDistance_X(vecGeoData[j].ptCenter, ptLastPoint);
			double dPreDistX = CDrawMakePoint::GetDistance_X(vecGeoData[j].ptCenter, vecGeoData[j].ptPoint);
			double dCurDist = CDrawMakePoint::GetDistance(vecGeoData[j].ptCenter, ptLastPoint);
			double dPreDist = CDrawMakePoint::GetDistance(vecGeoData[j].ptCenter, vecGeoData[j].ptPoint);

			double dRadiusX;
			double dRadiusY;
			if (dCurDistX >= dPreDistX)
			{
				dRadiusX = dCurDist;
				dRadiusY = dPreDist;
			}
			else
			{
				dRadiusX = dPreDist;
				dRadiusY = dCurDist;
			}


			D2D1_ARC_SIZE nArcType;
			double dArcAngle;
			if (vecGeoData[j].bCw == TRUE)
			{
				if (dStartAngle > dEndAngle)
				{
					dArcAngle = (360. - dStartAngle + dEndAngle);
				}
				else
				{
					dArcAngle = dEndAngle - dStartAngle;
				}


			}
			else
			{
				if (dStartAngle > dEndAngle)
				{
					dArcAngle = dStartAngle - dEndAngle;
				}
				else
				{
					dArcAngle = (360. - dEndAngle + dStartAngle);
				}

			}
			if (dArcAngle >= 180.)
			{
				nArcType = D2D1_ARC_SIZE_LARGE;
			}
			else
			{
				nArcType = D2D1_ARC_SIZE_SMALL;
			}



			float fArcSize = static_cast<float>(sqrt(pow(vecGeoData[j].ptCenter.x - vecGeoData[j].ptPoint.x, 2.) +
				pow(vecGeoData[j].ptCenter.y - vecGeoData[j].ptPoint.y, 2.)));

			pSink->AddArc(
				D2D1::ArcSegment(
					D2D1::Point2F(vecGeoData[j].ptPoint.x, vecGeoData[j].ptPoint.y), // end point
					D2D1::SizeF(static_cast<float>(dRadiusX), static_cast<float>(dRadiusY)),
					0.0f, // rotation angle
					(vecGeoData[j].bCw == TRUE ? D2D1_SWEEP_DIRECTION_CLOCKWISE : D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE),
					nArcType
				));
		}
		break;
		default:
			break;
		}

		ptLastPoint = vecGeoData[j].ptPoint;
	}

	if (bOpen == TRUE)
	{
		pSink->EndFigure(D2D1_FIGURE_END_OPEN);
	}
	else
	{
		pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
	}


	hr = pSink->Close();

	pSink->Release();

	return RESULT_GOOD;
}


void CDrawMakeGeo::ClearDrawGeo(vector<DrawFeatureGeo>& vecDrawGeo)
{
	int nCount = static_cast<int>(vecDrawGeo.size());

	for (int i = 0; i < nCount; i++)
	{
		vecDrawGeo[i].Clear();
	}
}

UINT32 CDrawMakeGeo::MakeTp(IN CFeatureZ *pFeauteZ, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeauteZ == nullptr) return RESULT_BAD;
	if (pFeauteZ->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakeTp(pFeauteZ, pFeauteZ->m_pSymbol, pRotMirrArr);
	int nCount = static_cast<int>(vecFeatureData.size());
	if (nCount >= 1)
	{
		SetGeometry(vecFeatureData[0].vecPointData, pGeo, TRUE);
	}
	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakeLine(IN CFeatureL *pFeatureL, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureL == nullptr) return RESULT_BAD;
	if (pFeatureL->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakeLine(pFeatureL, pFeatureL->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	if (nCount >= 1)
	{
		SetGeometry(vecFeatureData[0].vecPointData, pGeo, TRUE);
	}

	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakePadCircle(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadCircle(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	if (nCount >= 1)
	{
		SetGeometry(vecFeatureData[0].vecPointData, pGeo);
	}

	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakePadRect(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadRect(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	if (nCount >= 1)
	{
		SetGeometry(vecFeatureData[0].vecPointData, pGeo);
	}

	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakePadRectxr(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadRectxr(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	if (nCount >= 1)
	{
		SetGeometry(vecFeatureData[0].vecPointData, pGeo);
	}

	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakePadRectxc(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadRectxc(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	if (nCount >= 1)
	{
		SetGeometry(vecFeatureData[0].vecPointData, pGeo);
	}

	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakePadOval(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadOval(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	if (nCount >= 1)
	{
		SetGeometry(vecFeatureData[0].vecPointData, pGeo, TRUE);
	}

	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakePadDi(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadDi(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	if (nCount >= 1)
	{
		SetGeometry(vecFeatureData[0].vecPointData, pGeo);
	}

	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakePadOct(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadOct(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	if (nCount >= 1)
	{
		SetGeometry(vecFeatureData[0].vecPointData, pGeo);
	}

	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakePadDonut_r(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadDonut_r(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	if (nCount >= 1)
	{
		SetGeometry(vecFeatureData[0].vecPointData, pGeo);
	}

	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakePadDonut_s(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadDonut_s(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	if (nCount >= 1)
	{
		SetGeometry(vecFeatureData[0].vecPointData, pGeo);
	}

	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakePadHex_l(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadHex_l(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	if (nCount >= 1)
	{
		SetGeometry(vecFeatureData[0].vecPointData, pGeo);
	}

	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakePadHex_s(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadHex_s(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	if (nCount >= 1)
	{
		SetGeometry(vecFeatureData[0].vecPointData, pGeo);
	}

	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakePadBfr(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadBfr(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	if (nCount >= 1)
	{
		SetGeometry(vecFeatureData[0].vecPointData, pGeo);
	}

	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakePadBfs(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadBfs(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	if (nCount >= 1)
	{
		SetGeometry(vecFeatureData[0].vecPointData, pGeo);
	}

	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakePadTriangle(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadTriangle(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	if (nCount >= 1)
	{
		SetGeometry(vecFeatureData[0].vecPointData, pGeo);
	}

	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakePadOval_h(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadOval_h(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	if (nCount >= 1)
	{
		SetGeometry(vecFeatureData[0].vecPointData, pGeo);
	}

	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakePadThr(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadThr(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	if (nCount >= 1)
	{
		SetGeometry(vecFeatureData[0].vecPointData, pGeo, TRUE);
	}

	return RESULT_GOOD;
}
//여기부터는 Hole  존재함
UINT32 CDrawMakeGeo::MakePadThs(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadThs(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	vector<DrawFeatureGeo> vecDrawFeatureGeo;
	vecDrawFeatureGeo.resize(nCount);
	for (int i = 0; i < nCount; i++)
	{
		vecDrawFeatureGeo[i].bPolarity = vecFeatureData[i].bPolarity;

		SetGeometry(vecFeatureData[i].vecPointData, &vecDrawFeatureGeo[i].pGeo);
	}

	CombineFeature(vecDrawFeatureGeo, pGeo);

	return RESULT_GOOD;
}
UINT32 CDrawMakeGeo::MakePadS_ths(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadS_ths(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	vector<DrawFeatureGeo> vecDrawFeatureGeo;
	vecDrawFeatureGeo.resize(nCount);
	for (int i = 0; i < nCount; i++)
	{
		vecDrawFeatureGeo[i].bPolarity = vecFeatureData[i].bPolarity;

		SetGeometry(vecFeatureData[i].vecPointData, &vecDrawFeatureGeo[i].pGeo);
	}

	CombineFeature(vecDrawFeatureGeo, pGeo);

	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakePadS_tho(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadS_tho(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	vector<DrawFeatureGeo> vecDrawFeatureGeo;
	vecDrawFeatureGeo.resize(nCount);
	for (int i = 0; i < nCount; i++)
	{
		vecDrawFeatureGeo[i].bPolarity = vecFeatureData[i].bPolarity;

		SetGeometry(vecFeatureData[i].vecPointData, &vecDrawFeatureGeo[i].pGeo);
	}

	CombineFeature(vecDrawFeatureGeo, pGeo);

	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakePadSr_ths(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadSr_ths(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	vector<DrawFeatureGeo> vecDrawFeatureGeo;
	vecDrawFeatureGeo.resize(nCount);
	for (int i = 0; i < nCount; i++)
	{
		vecDrawFeatureGeo[i].bPolarity = vecFeatureData[i].bPolarity;

		SetGeometry(vecFeatureData[i].vecPointData, &vecDrawFeatureGeo[i].pGeo);
	}

	CombineFeature(vecDrawFeatureGeo, pGeo);

	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakePadRc_ths(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadRc_ths(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	vector<DrawFeatureGeo> vecDrawFeatureGeo;
	vecDrawFeatureGeo.resize(nCount);
	for (int i = 0; i < nCount; i++)
	{
		vecDrawFeatureGeo[i].bPolarity = vecFeatureData[i].bPolarity;

		SetGeometry(vecFeatureData[i].vecPointData, &vecDrawFeatureGeo[i].pGeo);
	}

	CombineFeature(vecDrawFeatureGeo, pGeo);

	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakePadRc_tho(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadRc_tho(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	vector<DrawFeatureGeo> vecDrawFeatureGeo;
	vecDrawFeatureGeo.resize(nCount);
	for (int i = 0; i < nCount; i++)
	{
		vecDrawFeatureGeo[i].bPolarity = vecFeatureData[i].bPolarity;

		SetGeometry(vecFeatureData[i].vecPointData, &vecDrawFeatureGeo[i].pGeo);
	}

	CombineFeature(vecDrawFeatureGeo, pGeo);

	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakePadEllipse(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadEllipse(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	vector<DrawFeatureGeo> vecDrawFeatureGeo;
	vecDrawFeatureGeo.resize(nCount);
	for (int i = 0; i < nCount; i++)
	{
		vecDrawFeatureGeo[i].bPolarity = vecFeatureData[i].bPolarity;

		SetGeometry(vecFeatureData[i].vecPointData, &vecDrawFeatureGeo[i].pGeo);
	}

	CombineFeature(vecDrawFeatureGeo, pGeo);

	return RESULT_GOOD;
}

UINT32 CDrawMakeGeo::MakePadMoire(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadMoire(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	vector<DrawFeatureGeo> vecDrawFeatureGeo;
	vecDrawFeatureGeo.resize(nCount);
	//for (int i = 0; i < nCount; i++)
	if (nCount > 0)
	{
		int i = 0;
		vecDrawFeatureGeo[i].bPolarity = vecFeatureData[i].bPolarity;

		SetGeometry(vecFeatureData[i].vecPointData, pGeo, TRUE);
	}

	//CombineFeature(vecDrawFeatureGeo, pGeo);

	return RESULT_GOOD;
}
UINT32 CDrawMakeGeo::MakePadHole(IN CFeatureP *pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;
	
	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakePadHole(pFeatureP, pFeatureP->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	vector<DrawFeatureGeo> vecDrawFeatureGeo;
	vecDrawFeatureGeo.resize(nCount);
	//for (int i = 0; i < nCount; i++)
	if (nCount>0)
	{
		int i = 0;
		vecDrawFeatureGeo[i].bPolarity = vecFeatureData[i].bPolarity;

		SetGeometry(vecFeatureData[i].vecPointData, pGeo);
	}

	//CombineFeature(vecDrawFeatureGeo, pGeo);

	return RESULT_GOOD;
	
}



//Arc
UINT32 CDrawMakeGeo::MakeArc(IN CFeatureA *pFeatureA, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureA == nullptr) return RESULT_BAD;
	if (pFeatureA->m_pSymbol == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakeArc(pFeatureA, pFeatureA->m_pSymbol, pRotMirrArr);

	int nCount = static_cast<int>(vecFeatureData.size());
	if (nCount >= 1)
	{
		SetGeometry(vecFeatureData[0].vecPointData, pGeo, TRUE);
	}

	return RESULT_GOOD;
}

//Surface
UINT32 CDrawMakeGeo::MakeSurface(IN CFeatureS *pFeatureS, CRotMirrArr *pRotMirrArr, RECTD drtViewRect_mm, ID2D1PathGeometry **pGeo)
{
	SafeRelease(pGeo);

	vector<MakeFeatureData> vecGeoDraw;
	vector<DrawFeatureGeo> vecDrawFeatureGeo;
	vecGeoDraw = CDrawMakePoint::MakeSurface(pFeatureS, pRotMirrArr, drtViewRect_mm);
	int nCount = static_cast<int>(vecGeoDraw.size());

	vecDrawFeatureGeo.resize(nCount);
	for (int i = 0; i < nCount; i++)
	{
		vecDrawFeatureGeo[i].bIsUsed = vecGeoDraw[i].bIsUsed;
		vecDrawFeatureGeo[i].bPolarity = vecGeoDraw[i].bPolarity;

		if (!vecGeoDraw[i].bIsUsed)
			continue;

		SetGeometry(vecGeoDraw[i].vecPointData, &vecDrawFeatureGeo[i].pGeo);
	}

	CombineFeature(vecDrawFeatureGeo, pGeo);

	return RESULT_GOOD;
}




UINT32 CDrawMakeGeo::CombineFeature(vector<DrawFeatureGeo> &vecDrawFeatureGeo, ID2D1PathGeometry **ppFeatureGeo)
{
	SafeRelease(ppFeatureGeo);

	int nFeaureCount = static_cast<int>(vecDrawFeatureGeo.size());
	if (nFeaureCount == 0) return RESULT_BAD;

	HRESULT hr = S_FALSE;
	ID2D1PathGeometry *pLastGeo_Temp = nullptr;

	if (nFeaureCount > 1)
	{//ObOe가 여러개일 경우
		D2D1_COMBINE_MODE emCombineMode;
		ID2D1GeometrySink *pSink_Temp[2] = { nullptr, };
		ID2D1PathGeometry *pGeo_Temp[2] = { nullptr, };
		m_pFactory->CreatePathGeometry(&pGeo_Temp[0]);
		m_pFactory->CreatePathGeometry(&pGeo_Temp[1]);

		int nSepIndex = 0, nCurIndx = 0;
		for (int i = 0; i < nFeaureCount; i++)
		{
			if (!vecDrawFeatureGeo[i].bIsUsed)
				continue;

			if (vecDrawFeatureGeo[i].pGeo == nullptr) continue;

			if (vecDrawFeatureGeo[i].bPolarity == TRUE)
			{
				emCombineMode = D2D1_COMBINE_MODE_UNION;
			}
			else
			{
				emCombineMode = D2D1_COMBINE_MODE_EXCLUDE;
			}

			nSepIndex = nCurIndx % 2;// i % 2;
			if (nSepIndex == 0)
			{
				hr = pGeo_Temp[nSepIndex]->Open(&pSink_Temp[nSepIndex]);
				if (!SUCCEEDED(hr))
					continue;
				if (i == 0)
				{
					hr = vecDrawFeatureGeo[i].pGeo->CombineWithGeometry(vecDrawFeatureGeo[i].pGeo, (D2D1_COMBINE_MODE)D2D1_COMBINE_MODE_UNION, NULL, NULL, pSink_Temp[nSepIndex]);
				}
				else
				{
					hr = pGeo_Temp[1]->CombineWithGeometry(vecDrawFeatureGeo[i].pGeo, (D2D1_COMBINE_MODE)emCombineMode, NULL, NULL, pSink_Temp[nSepIndex]);

					SafeRelease(&pGeo_Temp[1]);
					m_pFactory->CreatePathGeometry(&pGeo_Temp[1]);
				}

				pSink_Temp[nSepIndex]->Close();
				SafeRelease(&pSink_Temp[nSepIndex]);
				if (!SUCCEEDED(hr))
					continue;

				pLastGeo_Temp = pGeo_Temp[nSepIndex];



			}
			else
			{
				hr = pGeo_Temp[nSepIndex]->Open(&pSink_Temp[nSepIndex]);
				if (!SUCCEEDED(hr))
					continue;

				hr = pGeo_Temp[0]->CombineWithGeometry(vecDrawFeatureGeo[i].pGeo, (D2D1_COMBINE_MODE)emCombineMode, NULL, NULL, pSink_Temp[nSepIndex]);

				pSink_Temp[nSepIndex]->Close();
				SafeRelease(&pSink_Temp[nSepIndex]);
				if (!SUCCEEDED(hr))
					continue;

				SafeRelease(&pGeo_Temp[0]);
				m_pFactory->CreatePathGeometry(&pGeo_Temp[0]);

				pLastGeo_Temp = pGeo_Temp[nSepIndex];
			}

			nCurIndx++;
		}

		m_pFactory->CreatePathGeometry(ppFeatureGeo);
		ID2D1GeometrySink *pSink_Feature = NULL;

		//
		hr = (*ppFeatureGeo)->Open(&pSink_Feature);
		if (!SUCCEEDED(hr))
		{
			return RESULT_BAD;
		}

		// 		if (nObOeCount % 2 == 0)
		// 		{
		// 			hr = pGeo_Temp[1]->CombineWithGeometry(pGeo_Temp[1], D2D1_COMBINE_MODE_UNION, NULL, NULL, pSink_FeatureS);
		// 		}
		// 		else
		// 		{
		// 			hr = pGeo_Temp[0]->CombineWithGeometry(pGeo_Temp[0], D2D1_COMBINE_MODE_UNION, NULL, NULL, pSink_FeatureS);
		// 		}

		if (pLastGeo_Temp != nullptr)
		{
			hr = pLastGeo_Temp->CombineWithGeometry(pLastGeo_Temp, D2D1_COMBINE_MODE_UNION, NULL, NULL, pSink_Feature);
		}


		pSink_Feature->Close();
		SafeRelease(&pSink_Feature);

		for (int i = 0; i < 2; i++)
		{
			SafeRelease(&pSink_Temp[i]);
			SafeRelease(&pGeo_Temp[i]);
		}

		if (!SUCCEEDED(hr))
		{
			return RESULT_BAD;
		}

		//
	}
	else if (nFeaureCount == 1)
	{//ObOe가 한개 일경우

		if (vecDrawFeatureGeo[0].bIsUsed)
		{
			m_pFactory->CreatePathGeometry(ppFeatureGeo);
			ID2D1GeometrySink *pSink_Feature = nullptr;

			hr = (*ppFeatureGeo)->Open(&pSink_Feature);
			if (!SUCCEEDED(hr)) return RESULT_BAD;

			hr = vecDrawFeatureGeo[0].pGeo->CombineWithGeometry(vecDrawFeatureGeo[0].pGeo, D2D1_COMBINE_MODE_UNION, NULL, NULL, pSink_Feature);

			pSink_Feature->Close();
			SafeRelease(&pSink_Feature);

			if (!SUCCEEDED(hr)) return RESULT_BAD;
		}
	}

	return RESULT_GOOD;
}


UINT32 CDrawMakeGeo::CombineFeature(vector<DrawFeatureGeo *> &vecDrawFeatureGeo, ID2D1PathGeometry **ppFeatureGeo)
{
	SafeRelease(ppFeatureGeo);

	int nFeaureCount = static_cast<int>(vecDrawFeatureGeo.size());
	if (nFeaureCount == 0) return RESULT_BAD;

	HRESULT hr = S_FALSE;
	ID2D1PathGeometry *pLastGeo_Temp = nullptr;

	if (nFeaureCount > 1)
	{//Feature가 여러개일 경우
		D2D1_COMBINE_MODE emCombineMode;
		ID2D1GeometrySink *pSink_Temp[2] = { nullptr, };
		ID2D1PathGeometry *pGeo_Temp[2] = { nullptr, };
		m_pFactory->CreatePathGeometry(&pGeo_Temp[0]);
		m_pFactory->CreatePathGeometry(&pGeo_Temp[1]);

		int nSepIndex = 0, nCurIndx = 0;
		for (int i = 0; i < nFeaureCount; i++)
		{
			if (vecDrawFeatureGeo[i] == nullptr) continue;;

			if (!vecDrawFeatureGeo[i]->bIsUsed)
				continue;

			if (vecDrawFeatureGeo[i]->pGeo == nullptr) continue;

			if (vecDrawFeatureGeo[i]->bPolarity == TRUE)
			{
				emCombineMode = D2D1_COMBINE_MODE_UNION;
			}
			else
			{
				emCombineMode = D2D1_COMBINE_MODE_EXCLUDE;
			}

			nSepIndex = nCurIndx % 2;// i % 2;
			if (nSepIndex == 0)
			{
				hr = pGeo_Temp[nSepIndex]->Open(&pSink_Temp[nSepIndex]);
				if (!SUCCEEDED(hr))
					continue;
				if (i == 0)
				{
					hr = vecDrawFeatureGeo[i]->pGeo->CombineWithGeometry(vecDrawFeatureGeo[i]->pGeo, (D2D1_COMBINE_MODE)D2D1_COMBINE_MODE_UNION, NULL, NULL, pSink_Temp[nSepIndex]);
				}
				else
				{
					hr = pGeo_Temp[1]->CombineWithGeometry(vecDrawFeatureGeo[i]->pGeo, (D2D1_COMBINE_MODE)emCombineMode, NULL, NULL, pSink_Temp[nSepIndex]);

					SafeRelease(&pGeo_Temp[1]);
					m_pFactory->CreatePathGeometry(&pGeo_Temp[1]);
				}

				pSink_Temp[nSepIndex]->Close();
				SafeRelease(&pSink_Temp[nSepIndex]);
				if (!SUCCEEDED(hr))
					continue;

				pLastGeo_Temp = pGeo_Temp[nSepIndex];



			}
			else
			{
				hr = pGeo_Temp[nSepIndex]->Open(&pSink_Temp[nSepIndex]);
				if (!SUCCEEDED(hr))
					continue;

				hr = pGeo_Temp[0]->CombineWithGeometry(vecDrawFeatureGeo[i]->pGeo, (D2D1_COMBINE_MODE)emCombineMode, NULL, NULL, pSink_Temp[nSepIndex]);

				pSink_Temp[nSepIndex]->Close();
				SafeRelease(&pSink_Temp[nSepIndex]);
				if (!SUCCEEDED(hr))
					continue;

				SafeRelease(&pGeo_Temp[0]);
				m_pFactory->CreatePathGeometry(&pGeo_Temp[0]);

				pLastGeo_Temp = pGeo_Temp[nSepIndex];
			}

			nCurIndx++;
		}

		m_pFactory->CreatePathGeometry(ppFeatureGeo);
		ID2D1GeometrySink *pSink_Feature = NULL;

		//
		hr = (*ppFeatureGeo)->Open(&pSink_Feature);
		if (!SUCCEEDED(hr))
		{
			return RESULT_BAD;
		}

		// 		if (nObOeCount % 2 == 0)
		// 		{
		// 			hr = pGeo_Temp[1]->CombineWithGeometry(pGeo_Temp[1], D2D1_COMBINE_MODE_UNION, NULL, NULL, pSink_FeatureS);
		// 		}
		// 		else
		// 		{
		// 			hr = pGeo_Temp[0]->CombineWithGeometry(pGeo_Temp[0], D2D1_COMBINE_MODE_UNION, NULL, NULL, pSink_FeatureS);
		// 		}

		if (pLastGeo_Temp != nullptr)
		{
			hr = pLastGeo_Temp->CombineWithGeometry(pLastGeo_Temp, D2D1_COMBINE_MODE_UNION, NULL, NULL, pSink_Feature);
		}


		pSink_Feature->Close();
		SafeRelease(&pSink_Feature);

		for (int i = 0; i < 2; i++)
		{
			SafeRelease(&pSink_Temp[i]);
			SafeRelease(&pGeo_Temp[i]);
		}

		if (!SUCCEEDED(hr))
		{
			return RESULT_BAD;
		}

		//
	}
	else if (nFeaureCount == 1)
	{//Feature가 한개 일경우

		if (vecDrawFeatureGeo[0]->bIsUsed)
		{
			m_pFactory->CreatePathGeometry(ppFeatureGeo);
			ID2D1GeometrySink *pSink_Feature = nullptr;

			hr = (*ppFeatureGeo)->Open(&pSink_Feature);
			if (!SUCCEEDED(hr)) return RESULT_BAD;

			hr = vecDrawFeatureGeo[0]->pGeo->CombineWithGeometry(vecDrawFeatureGeo[0]->pGeo, D2D1_COMBINE_MODE_UNION, NULL, NULL, pSink_Feature);

			pSink_Feature->Close();
			SafeRelease(&pSink_Feature);

			if (!SUCCEEDED(hr)) return RESULT_BAD;
		}
	}

	return RESULT_GOOD;
}
