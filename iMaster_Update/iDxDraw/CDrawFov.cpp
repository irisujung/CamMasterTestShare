#include "pch.h"
#include "CDrawFov.h"
#include "CDrawFunction.h"
#include "CDrawMakePoint.h"

CDrawFov::CDrawFov()
{

}

CDrawFov::~CDrawFov()
{
	if (m_pBrush != nullptr)
	{
		m_pBrush->Release();
		m_pBrush = nullptr;
	}
	if (m_pGeoObject != nullptr)
	{
		m_pGeoObject->Release();
		m_pGeoObject = nullptr;
	}

	if (m_pBrush_Align != nullptr)
	{
		m_pBrush_Align->Release();
		m_pBrush_Align = nullptr;
	}

	if (m_pBrush_Mask != nullptr)
	{
		m_pBrush_Mask->Release();
		m_pBrush_Mask = nullptr;
	}
	
}

void CDrawFov::SetLink(ID2D1Factory1 *pFactory, ID2D1DeviceContext* pD2DContext, FovInfo_t* pFovInfo, CellInfo_t* pCellInfo)
{
	m_pFactory = pFactory;
	m_pD2DContext = pD2DContext;
	m_pFovInfo = pFovInfo;
	m_pCellInfo = pCellInfo;
	
	if (m_pD2DContext == nullptr) return;

	if (S_OK == m_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(1, 1, 0), &m_pBrush))
	{
		// 성공했으면 p_yellow_brush 객체에 사용가능한 Brush 객체의 주소가 저장되어 있다.
	}

	if (S_OK == m_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(0, 1, 0, 0.5f), &m_pBrush_Align))
	{
		// 성공했으면 p_yellow_brush 객체에 사용가능한 Brush 객체의 주소가 저장되어 있다.
	}

	if (S_OK == m_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(0, 0, 1, 0.5f), &m_pBrush_Mask))
	{
		// 성공했으면 p_yellow_brush 객체에 사용가능한 Brush 객체의 주소가 저장되어 있다.
	}
}

UINT32 CDrawFov::SetUserSetInfo(UserSetInfo_t *pUserSetInfo, UserLayerSet_t *pUserLayerSetInfo)
{
	m_pUserSetInfo = pUserSetInfo;
	m_pUserLayerSetInfo = pUserLayerSetInfo;

	return RESULT_GOOD;
}

void CDrawFov::ResetView()
{
	m_nCurStep = -1;
}

void CDrawFov::OnRender(const int &nStepIndex, ID2D1BitmapRenderTarget* pRender, bool bIsFovMode)
{
	if (bIsFovMode)
		OnRender_Fov(nStepIndex, pRender);
	else
		OnRender_Cell(nStepIndex, pRender);
}

void CDrawFov::_AddFovFeature(FovData_t* pFov)
{
	if (pFov == nullptr) return;	

	RECTD rcFov = pFov->rcDraw;
	GEODRAW stGeo;

	//Left Top
	stGeo.SetBasePoint(rcFov.left, rcFov.top);
	m_vecGeo.push_back(stGeo);
	stGeo._Reset();

	//Left Bot
	stGeo.SetLine(rcFov.left, rcFov.bottom);
	m_vecGeo.push_back(stGeo);
	stGeo._Reset();

	//Right Bot
	stGeo.SetLine(rcFov.right, rcFov.bottom);
	m_vecGeo.push_back(stGeo);
	stGeo._Reset();

	//Right Top
	stGeo.SetLine(rcFov.right, rcFov.top);
	m_vecGeo.push_back(stGeo);
	stGeo._Reset();

	//Left Top
	stGeo.SetLine(rcFov.left, rcFov.top);
	m_vecGeo.push_back(stGeo);
	stGeo._Reset();
}

UINT32 CDrawFov::_SetGeometry(vector<GEODRAW> &vecGeoData, ID2D1PathGeometry **pGeoObject, BOOL bOpen)
{
	if (*pGeoObject == nullptr)
	{
		m_pFactory->CreatePathGeometry(pGeoObject);
	}

	ID2D1GeometrySink *pSink = NULL;

	HRESULT hr;
	hr = (*pGeoObject)->Open(&pSink);
	if (FAILED(hr))
	{
		(*pGeoObject)->Release();
		(*pGeoObject) = nullptr;
		
		return RESULT_BAD;
	}

	if (pSink == nullptr)
	{
		(*pGeoObject)->Release();
		(*pGeoObject) = nullptr;

		return RESULT_BAD;
	}

	int nPtCount = static_cast<int>(vecGeoData.size());

	D2D_POINT_2F ptLastPoint;

	//pSink->SetFillMode(D2D1_FILL_MODE_WINDING);
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
					D2D1::SizeF(fArcSize, fArcSize),
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

void CDrawFov::OnRender_Fov(const int &nStepIndex, ID2D1BitmapRenderTarget* pRender)
{	
	if (pRender == nullptr) return;
	if (m_pFovInfo->vcFovData.size() <= 0) return;

	if (m_pFovInfo->bChanged ||
		m_nCurStep != nStepIndex)
	{
		m_nCurStep = nStepIndex;
		if (m_pGeoObject != nullptr)
		{
			m_pGeoObject->Release();
			m_pGeoObject = nullptr;
		}

		m_vecGeo.clear();
	}

	if (m_pGeoObject == nullptr)
	{
		for (auto it : m_pFovInfo->vcFovData)
		{
			_AddFovFeature(it);
		}
		_SetGeometry(m_vecGeo, &m_pGeoObject);
	}

	D2D1_MATRIX_3X2_F matTransform;
	pRender->GetTransform(&matTransform);
	float fScale = fabs(matTransform.m11);
	if (fScale == 0.f)
	{
		fScale = static_cast<float>(fabs(matTransform.m12));
	}

	if (m_pGeoObject != nullptr)
	{//m_pGeoObject가 널이 아닐경우만 Draw

		pRender->DrawGeometry(m_pGeoObject, m_pBrush, 1 / fScale);
	}

	m_pFovInfo->bChanged = false;
}

void CDrawFov::OnRender_Cell(const int &nStepIndex, ID2D1BitmapRenderTarget* pRender)
{
	if (pRender == nullptr) return;
	if (m_pCellInfo->vcCellData.size() <= 0) return;

	if (m_pCellInfo->bChanged ||
		m_nCurStep != nStepIndex)
	{
		m_nCurStep = nStepIndex;
		if (m_pGeoObject != nullptr)
		{
			m_pGeoObject->Release();
			m_pGeoObject = nullptr;
		}

		m_vecGeo.clear();
	}

	if (m_pGeoObject == nullptr)
	{
		for (auto it : m_pCellInfo->vcCellData)
		{
			_AddFovFeature(it);
		}
		_SetGeometry(m_vecGeo, &m_pGeoObject);
	}

	D2D1_MATRIX_3X2_F matTransform;
	pRender->GetTransform(&matTransform);
	float fScale = fabs(matTransform.m11);
	if (fScale == 0.f)
	{
		fScale = static_cast<float>(fabs(matTransform.m12));
	}

	if (m_pGeoObject != nullptr)
	{//m_pGeoObject가 널이 아닐경우만 Draw

		pRender->DrawGeometry(m_pGeoObject, m_pBrush, 1 / fScale);
	}

	//Test
	int iSwathNum = static_cast<int>(m_pFovInfo->stBasic.swathNum);
	if (iSwathNum == 0) return;
	int iCellCol = static_cast<int>(m_pCellInfo->stBasic.cellNum.cx) / iSwathNum;
	int iCellRow = static_cast<int>(m_pCellInfo->stBasic.cellNum.cy);

	UINT32 iOffsetX = m_pFovInfo->vcFovData[0]->rcRectPxl.left;
	UINT32 iOffsetY = m_pFovInfo->vcFovData[0]->rcRectPxl.top;

	//Strip
	auto itStip_in_Cell = m_pCellInfo->vcStrip_in_Cell.find(0);
	if (itStip_in_Cell != m_pCellInfo->vcStrip_in_Cell.end())
	{

		for ( auto itStrip : itStip_in_Cell->second)
		{//Align
			if (m_pUserSetInfo->bShowUnitAlign)
			{
				for (auto vcPattern : itStrip->vcAlign_In_Panel)
				{
					for (auto dRect : vcPattern)
					{
						D2D1_RECT_F frtRect = D2D1::RectF(DoubleToFloat(dRect.left),
							DoubleToFloat(dRect.top), DoubleToFloat(dRect.right), DoubleToFloat(dRect.bottom));
						//pRender->DrawRectangle(frtRect, m_pBrush_Align, 1 / fScale);
						pRender->FillRectangle(frtRect, m_pBrush_Align);
					}

				}
			}

			if (m_pUserSetInfo->bShowInspMask)
			{//Mask
				for (auto vcPattern : itStrip->vcMask_In_Panel)
				{
					for (auto dRect : vcPattern)
					{
						D2D1_RECT_F frtRect = D2D1::RectF(DoubleToFloat(dRect.left),
							DoubleToFloat(dRect.top), DoubleToFloat(dRect.right), DoubleToFloat(dRect.bottom));
						//pRender->DrawRectangle(frtRect, m_pBrush_Mask, 1 / fScale);
						pRender->FillRectangle(frtRect, m_pBrush_Mask);
					}
				}
			}

		}
	}

	//Unit
	auto itUnit_in_Cell = m_pCellInfo->vcUnit_in_Cell.find(0);
	if (itUnit_in_Cell != m_pCellInfo->vcUnit_in_Cell.end())
	{

		for (auto itUnit : itUnit_in_Cell->second)
		{//Align
			if (m_pUserSetInfo->bShowUnitAlign)
			{
				for (auto vcPattern : itUnit->vcAlign_In_Panel)
				{
					for (auto dRect : vcPattern)
					{
						D2D1_RECT_F frtRect = D2D1::RectF(DoubleToFloat(dRect.left),
							DoubleToFloat(dRect.top), DoubleToFloat(dRect.right), DoubleToFloat(dRect.bottom));
						//pRender->DrawRectangle(frtRect, m_pBrush_Align, 1 / fScale);
						pRender->FillRectangle(frtRect, m_pBrush_Align);
					}

				}
			}

			if (m_pUserSetInfo->bShowInspMask)
			{//Mask
				for (auto vcPattern : itUnit->vcMask_In_Panel)
				{
					for (auto dRect : vcPattern)
					{
						D2D1_RECT_F frtRect = D2D1::RectF(DoubleToFloat(dRect.left),
							DoubleToFloat(dRect.top), DoubleToFloat(dRect.right), DoubleToFloat(dRect.bottom));
						//pRender->DrawRectangle(frtRect, m_pBrush_Mask, 1 / fScale);
						pRender->FillRectangle(frtRect, m_pBrush_Mask);
					}
				}
			}

		}
	}

	CRect rcCell;
	for (auto itSwath : m_pFovInfo->vcFovData)
	{
		for (auto itCell : m_pCellInfo->vcCellData)
		{
			if (itSwath->fovIdx.x != itCell->swathIdx)
				continue;

			int iCell = itCell->swathIdx * (iCellCol * iCellRow) + (iCellCol * itCell->fovIdx.y) + itCell->fovIdx.x;
			
			//Cell 영역에 포함되어 있는 유효한 Align 영역만 표시
			auto itAlign_in_Cell = m_pCellInfo->vcAlign_in_Cell.find(iCell);
			if (itAlign_in_Cell == m_pCellInfo->vcAlign_in_Cell.end())	continue;
			
			for (auto vcAlign : itAlign_in_Cell->second)
			{
				if (m_pUserSetInfo->bShowUnitAlign)
				{
					int nCount = static_cast<int>(vcAlign.vcInPanel_mm.size());
					for ( auto drtAlign : vcAlign.vcInPanel_mm)
					{
						D2D1_RECT_F frtRect = D2D1::RectF(DoubleToFloat(drtAlign.left),
							DoubleToFloat(drtAlign.top), DoubleToFloat(drtAlign.right), DoubleToFloat(drtAlign.bottom));
						pRender->DrawRectangle(frtRect, m_pBrush, 3.f / fScale);
					}
				}
			}

		}
	}

	
	//

	m_pCellInfo->bChanged = false;
}