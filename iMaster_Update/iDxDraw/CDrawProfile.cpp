#include "pch.h"
#include "CDrawProfile.h"
#include "CDrawFunction.h"
#include "CDrawMakePoint.h"

CDrawProfile::CDrawProfile()
{

}

CDrawProfile::~CDrawProfile()
{
	if (m_pWhiteBrush != nullptr)
	{
		m_pWhiteBrush->Release();
		m_pWhiteBrush = nullptr;
	}
	if (m_pGeoObject != nullptr)
	{
		m_pGeoObject->Release();
		m_pGeoObject = nullptr;
	}

	_ClearTextUtil();
}

void CDrawProfile::SetLink(ID2D1Factory1 *pFactory, ID2D1DeviceContext* pD2DContext)
{
	m_pFactory = pFactory;
	m_pD2DContext = pD2DContext;

	if (m_pD2DContext == nullptr) return;

	if (S_OK == m_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1), &m_pWhiteBrush))
	{
		// 성공했으면 p_yellow_brush 객체에 사용가능한 Brush 객체의 주소가 저장되어 있다.
	}

	_InitTextUtil();
}

UINT32 CDrawProfile::SetUserSetInfo(UserSetInfo_t *pUserSetInfo, UserLayerSet_t *pUserLayerSetInfo)
{
	m_pUserSetInfo = pUserSetInfo;
	m_pUserLayerSetInfo = pUserLayerSetInfo;

	return RESULT_GOOD;
}

void CDrawProfile::ResetView()
{
	m_nCurStep = -1;
}

void CDrawProfile::OnRender(IN CJobFile *pjobFile, const int &nStepIndex, ID2D1BitmapRenderTarget* pRender)
{
	if (pjobFile == nullptr) return;
	if (pRender == nullptr) return;

	if (m_nCurStep != nStepIndex)
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
		CStep *pStep = pjobFile->m_arrStep.GetAt(nStepIndex);
		if (pStep == nullptr) return;

		CLayer* pProfileLayer = pStep->m_Profile.m_pProfileLayer;
		RECTD drtPickture= pStep->m_Profile.m_MinMax;
		drtPickture.NormalizeRectD();

		m_frtPictureBox = D2D1::RectF(static_cast<float>(drtPickture.left), static_cast<float>(drtPickture.top),
			static_cast<float>(drtPickture.right), static_cast<float>(drtPickture.bottom));

		if (pProfileLayer == nullptr) return;
		int nFeatureCount = static_cast<int>(pProfileLayer->m_FeatureFile.m_arrFeature.GetCount());

		for (int i = 0; i < nFeatureCount; i++)
		{
			CFeature *pFeature = pProfileLayer->m_FeatureFile.m_arrFeature.GetAt(i);
			if (pFeature->m_eType == FeatureType::S)
			{
				CFeatureS *pFeatureS = (CFeatureS*)pFeature;
				AddProfileFeature(pFeatureS);
			}


			
		}

		for (UINT i = 0; i < pStep->m_nSubStepCnt; i++)
		{
			CSubStep* pSubStep = pStep->m_arrSubStep[i];
			//Draw SubStep
			OnRender_SubStep(pSubStep);
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

		pRender->DrawGeometry(m_pGeoObject, m_pWhiteBrush, 1 / fScale);
	}

}

void CDrawProfile::OnRender_SubStep(CSubStep* pSubStep, CRotMirrArr *pRotMirrArr)
{
	if (pSubStep == nullptr)return;

	CStepRepeat* pSubStepRepeat = &(pSubStep->m_arrStepRepeat);
	if (pSubStepRepeat == nullptr)
		return;

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
	for (UINT32 y_step = 0; y_step < pSubStepRepeat->m_nNY; y_step++)
	{
		for (UINT32 x_step = 0; x_step < pSubStepRepeat->m_nNX; x_step++)
		{
			dRefx = pSubStep->m_dXDatum;
			dRefy = pSubStep->m_dYDatum;

			rotMirrArr->ConvertPoint(&dParentX, &dParentY, 0, 0, dRefx, dRefy, eSubOrient);
			dParentX = (pSubStepRepeat->m_dX - dParentX) + pSubStepRepeat->m_dDX * x_step;
			dParentY = (pSubStepRepeat->m_dY - dParentY) + pSubStepRepeat->m_dDY * y_step;

			rotMirrArr->InputData(dParentX, dParentY, eSubOrient);

			if (pSubStep->m_nNextStepCnt > 0)
			{
				for (UINT j = 0; j < pSubStep->m_nNextStepCnt; j++)
				{
					CStepRepeat* pNextStepRepeat = &(pSubStep->m_arrNextStep[j]->m_arrStepRepeat);
					if (pNextStepRepeat == nullptr)
						return;

					UINT32 nNextStepRepeatX = pNextStepRepeat->m_nNX;
					UINT32 nNextStepRepeatY = pNextStepRepeat->m_nNY;

					OnRender_SubStep(pSubStep->m_arrNextStep[j], rotMirrArr);

				}
			}

			for (int feno = 0; feno < pSubStep->m_pSubStep->m_Profile.m_pProfileLayer->m_FeatureFile.m_arrFeature.GetSize(); feno++)	//feature drawing
			{
				CFeature* pFeatureTmp = pSubStep->m_pSubStep->m_Profile.m_pProfileLayer->m_FeatureFile.m_arrFeature.GetAt(feno);

				if (pFeatureTmp->m_eType == FeatureType::S)
				{
					CFeatureS *pFeatureS = (CFeatureS*)pFeatureTmp;
					AddProfileFeature(pFeatureS, rotMirrArr);
				}
			}

			rotMirrArr->RemoveTail();

		}
	}

	if (bCreateRotMirr == TRUE)
	{
		rotMirrArr->ClearAll();
		delete rotMirrArr;
		rotMirrArr = nullptr;
	}

}

void CDrawProfile::OnRender_Save(IN CStep *pStep, CRotMirrArr *pRotMirrArr, IN const BOOL bSubDraw, IN const BOOL &bSubSubDraw, IN const BOOL &bSubSubSubDraw, IN ID2D1RenderTarget* pRender)
{
	if (pStep == nullptr) return;
	ID2D1PathGeometry *pGeoObject = nullptr;

	m_vecGeo.clear();

	CLayer* pProfileLayer = pStep->m_Profile.m_pProfileLayer;
// 	RECTD drtPickture = pStep->m_Profile.m_MinMax;
// 	drtPickture.NormalizeRectD();
// 	m_frtPictureBox = D2D1::RectF(static_cast<float>(drtPickture.left), static_cast<float>(drtPickture.top),
// 		static_cast<float>(drtPickture.right), static_cast<float>(drtPickture.bottom));

	if (pProfileLayer == nullptr) return;
	int nFeatureCount = static_cast<int>(pProfileLayer->m_FeatureFile.m_arrFeature.GetCount());

	for (int i = 0; i < nFeatureCount; i++)
	{
		CFeature *pFeature = pProfileLayer->m_FeatureFile.m_arrFeature.GetAt(i);
		if (pFeature->m_eType == FeatureType::S)
		{
			CFeatureS *pFeatureS = (CFeatureS*)pFeature;
			AddProfileFeature(pFeatureS, pRotMirrArr);
		}
	}

	if (bSubDraw == TRUE)
	{
		for (UINT i = 0; i < pStep->m_nSubStepCnt; i++)
		{
			CSubStep* pSubStep = pStep->m_arrSubStep[i];
			//Draw SubStep
			OnRender_SubStep_Save(pSubStep, bSubSubDraw, bSubSubSubDraw, pRotMirrArr);
		}
	}

	if (m_vecGeo.size() > 0)
	{
		_SetGeometry(m_vecGeo, &pGeoObject);

		D2D1_MATRIX_3X2_F matTransform;
		pRender->GetTransform(&matTransform);
		float fScale = fabs(matTransform.m11);
		if (fScale == 0.f)
		{
			fScale = static_cast<float>(fabs(matTransform.m12));
		}

		if (pGeoObject != nullptr)
		{//m_pGeoObject가 널이 아닐경우만 Draw
			ID2D1SolidColorBrush *pWhiteBrush;
			if (S_OK == pRender->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1), &pWhiteBrush))
			{
				pRender->DrawGeometry(pGeoObject, pWhiteBrush, 10 / fScale);

				SafeRelease(&pWhiteBrush);
			}
		}
	}

	m_vecGeo.clear();
	SafeRelease(&pGeoObject);
	SafeRelease(&m_pGeoObject);
}

void CDrawProfile::OnRender_SubStep_Save(CSubStep* pSubStep, IN const BOOL &bSubSubDraw, IN const BOOL &bSubSubSubDraw, CRotMirrArr *pRotMirrArr)
{
	if (pSubStep == nullptr)return;

	CStepRepeat* pSubStepRepeat = &(pSubStep->m_arrStepRepeat);
	if (pSubStepRepeat == nullptr)
		return;

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
	for (UINT32 y_step = 0; y_step < pSubStepRepeat->m_nNY; y_step++)
	{
		for (UINT32 x_step = 0; x_step < pSubStepRepeat->m_nNX; x_step++)
		{
			dRefx = pSubStep->m_dXDatum;
			dRefy = pSubStep->m_dYDatum;

			rotMirrArr->ConvertPoint(&dParentX, &dParentY, 0, 0, dRefx, dRefy, eSubOrient);
			dParentX = (pSubStepRepeat->m_dX - dParentX) + pSubStepRepeat->m_dDX * x_step;
			dParentY = (pSubStepRepeat->m_dY - dParentY) + pSubStepRepeat->m_dDY * y_step;

			rotMirrArr->InputData(dParentX, dParentY, eSubOrient);

			if (pSubStep->m_nNextStepCnt > 0 && bSubSubDraw == TRUE )
			{
				for (UINT j = 0; j < pSubStep->m_nNextStepCnt; j++)
				{
					CStepRepeat* pNextStepRepeat = &(pSubStep->m_arrNextStep[j]->m_arrStepRepeat);
					if (pNextStepRepeat == nullptr)
						return;

					UINT32 nNextStepRepeatX = pNextStepRepeat->m_nNX;
					UINT32 nNextStepRepeatY = pNextStepRepeat->m_nNY;

					OnRender_SubStep_Save(pSubStep->m_arrNextStep[j], bSubSubSubDraw, bSubSubSubDraw, rotMirrArr);

				}
			}

			for (int feno = 0; feno < pSubStep->m_pSubStep->m_Profile.m_pProfileLayer->m_FeatureFile.m_arrFeature.GetSize(); feno++)	//feature drawing
			{
				CFeature* pFeatureTmp = pSubStep->m_pSubStep->m_Profile.m_pProfileLayer->m_FeatureFile.m_arrFeature.GetAt(feno);

				if (pFeatureTmp->m_eType == FeatureType::S)
				{
					CFeatureS *pFeatureS = (CFeatureS*)pFeatureTmp;
					AddProfileFeature(pFeatureS, rotMirrArr);
				}
			}

			rotMirrArr->RemoveTail();

		}
	}

	if (bCreateRotMirr == TRUE)
	{
		rotMirrArr->ClearAll();
		delete rotMirrArr;
		rotMirrArr = nullptr;
	}
}

void CDrawProfile::AddProfileFeature(CFeatureS *pFeatureS, CRotMirrArr *rotMirrArr)
{
	if (pFeatureS == nullptr) return;

	int nCountS = static_cast<int>(pFeatureS->m_arrObOe.GetSize());
	for (int s = 0; s < nCountS; s++)
	{
		CObOe *pSurface = pFeatureS->m_arrObOe.GetAt(s);

		if (m_pGeoObject == nullptr)
		{//m_pGeoObject가 널이 아닐경우 GeoPath를 만들어줌
			_AddProfileFeature(pSurface, rotMirrArr);
		}
	}
}

void CDrawProfile::_AddProfileFeature(CObOe* pSurface, CRotMirrArr *rotMirrArr)
{
	if (pSurface == nullptr) return;
	int nPtCount = static_cast<int>(pSurface->m_arrOsOc.GetSize());

	if (nPtCount < 1) return;

	BOOL bCreateRotMirr = FALSE;
	if (rotMirrArr == nullptr)
	{
		bCreateRotMirr = TRUE;
		rotMirrArr = new CRotMirrArr;
		rotMirrArr->InputData(0.0, 0.0, Orient::NoMir0Deg);
	}


	GEODRAW stGeo;
	double dNewX, dNewY;
	rotMirrArr->FinalPoint(&dNewX, &dNewY, pSurface->m_dXbs, pSurface->m_dYbs);
	stGeo.SetBasePoint(dNewX, dNewY);
	m_vecGeo.push_back(stGeo);
	stGeo._Reset();

	for (int i = 0; i < nPtCount; i++)
	{
		COsOc* pOsOc = pSurface->m_arrOsOc.GetAt(i);


		rotMirrArr->FinalPoint(&dNewX, &dNewY, pOsOc->m_dX, pOsOc->m_dY);

		if (pOsOc->m_bOsOc == true)
		{//Line
			stGeo.SetLine(dNewX, dNewY);
			m_vecGeo.push_back(stGeo);
			stGeo._Reset();
		}
		else
		{
			double dNew_CX, dNew_CY;
			rotMirrArr->FinalPoint(&dNew_CX, &dNew_CY, ((COc*)pOsOc)->m_dXc, ((COc*)pOsOc)->m_dYc);
			stGeo.SetArc(dNewX, dNewY, !((COc*)pOsOc)->m_bCw, dNew_CX, dNew_CY);
			m_vecGeo.push_back(stGeo);
			stGeo._Reset();
		}

	}

	if (bCreateRotMirr == TRUE)
	{
		rotMirrArr->ClearAll();
		delete rotMirrArr;
		rotMirrArr = nullptr;
	}
}

UINT32 CDrawProfile::_SetGeometry(vector<GEODRAW> &vecGeoData, ID2D1PathGeometry **pGeoObject, BOOL bOpen)
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

void CDrawProfile::DrawRect(ID2D1BitmapRenderTarget* pRender, CRect &rtRect)
{
	if (pRender == nullptr) return;

	D2D1_MATRIX_3X2_F matTransform;
	pRender->GetTransform(&matTransform);
	float fScaleX = static_cast<float>(fabs(matTransform.m11));
	if (fScaleX == 0.f)
	{
		fScaleX = static_cast<float>(fabs(matTransform.m12));
	}


	D2D1_RECT_F frtRect = D2D1::RectF(static_cast<float>(rtRect.left), static_cast<float>(rtRect.top),
		static_cast<float>(rtRect.right), static_cast<float>(rtRect.bottom));

	D2D1_RECT_F frtTransRect = CDrawFunction::DetansformRect(frtRect, matTransform, m_pUserSetInfo);

	frtTransRect = CDrawFunction::NormalizeRect(frtTransRect);

	fScaleX = fabs(fScaleX);
	pRender->DrawRectangle(frtTransRect, m_pWhiteBrush, 1 / fScaleX);
}


void CDrawProfile::_InitTextUtil()
{
	//if (m_pFactory == nullptr) return;

	DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(m_pWriteFactory), reinterpret_cast<IUnknown**>(&m_pWriteFactory));
	if (m_pWriteFactory == nullptr) return;

	m_pWriteFactory->CreateTextFormat(_T("Verdana"), NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		10/*FontSize*/, _T("")/*localeName*/, &m_pTextFormat);

	if (m_pTextFormat == nullptr) return;

	m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);//H-Align
	m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);//V-Align
	
}

void CDrawProfile::_ClearTextUtil()
{
	if (m_pTextFormat != nullptr)
	{
		m_pTextFormat->Release();
		m_pTextFormat = nullptr;
	}

	if (m_pWriteFactory != nullptr)
	{
		m_pWriteFactory->Release();
		m_pWriteFactory = nullptr;
	}
}

void CDrawProfile::DrawMeasure(ID2D1BitmapRenderTarget* pRender, IN const D2D1_RECT_F &frtRect, IN const BOOL &bDispInch)
{
	if (pRender == nullptr) return;
	if (m_pUserSetInfo == nullptr) return;

	D2D1_MATRIX_3X2_F matTransform;
	pRender->GetTransform(&matTransform);
		
	float fScaleX = static_cast<float>(fabs(matTransform.m11));
	if (fScaleX == 0.f)
	{
		fScaleX = static_cast<float>(fabs(matTransform.m12));
	}

	

	//Rect
	pRender->DrawRectangle(frtRect, m_pWhiteBrush, 1 / fScaleX);
	
	//대각선
	D2D1_POINT_2F frtStart = D2D1::Point2F(frtRect.left, frtRect.top);
	D2D1_POINT_2F frtEnd = D2D1::Point2F(frtRect.right, frtRect.bottom);
	pRender->DrawLine(frtStart, frtEnd, m_pWhiteBrush, 1 / fScaleX);

	//Text	
	if (m_pWriteFactory == nullptr) return;

	IDWriteTextFormat	*pTextFormat;
	m_pWriteFactory->CreateTextFormat(_T("Verdana"), NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		10.f / fScaleX/*FontSize*/, _T("")/*localeName*/, &pTextFormat);

	if (pTextFormat == nullptr) return;
	pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);//H-Align
	pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);//V-Align
	
	double dDistX = fabs(frtRect.right - frtRect.left);
	double dDistY = fabs(frtRect.bottom - frtRect.top);
	double dDist = sqrt(pow(dDistX, 2) + pow(dDistY, 2));

	//Text Position
	double dCenterX = (frtRect.right + frtRect.left) / 2.;
	double dCenterY = -1.0f * (frtRect.bottom + frtRect.top) / 2.;
	
 	double dTemp;
	if (m_pUserSetInfo->bMirror == TRUE)
	{
		if (m_pUserSetInfo->dAngle == 0.0)
		{
			dCenterX *= -1.0;
		}
		else if (m_pUserSetInfo->dAngle == 90.0)
		{
			SWAP(dCenterX, dCenterY, dTemp);
			dCenterX *= -1.0;
			dCenterY *= -1.0;

			SWAP(dDistX, dDistY, dTemp);
			
		}
		else if (m_pUserSetInfo->dAngle == 180.0)
		{
			dCenterY *= -1.0;
		}
		else if (m_pUserSetInfo->dAngle == 270.0)
		{
			SWAP(dCenterX, dCenterY, dTemp);
			SWAP(dDistX, dDistY, dTemp);
		}
	}
	else
	{
		if (m_pUserSetInfo->dAngle == 90.0)
		{
			SWAP(dCenterX, dCenterY, dTemp);
			dCenterX *= -1.0;

			SWAP(dDistX, dDistY, dTemp);
		}
		else if (m_pUserSetInfo->dAngle == 180.0)
		{
			dCenterX *= -1.0;
			dCenterY *= -1.0;
		}
		else if (m_pUserSetInfo->dAngle == 270.0)
		{
			SWAP(dCenterX, dCenterY, dTemp);
			dCenterY *= -1.0;

			SWAP(dDistX, dDistY, dTemp);
		}
	}

	CString str;
	if (m_pUserSetInfo->coordUnit == eCoordinateUnit::eMMToInch)
	{
		str.Format(_T(" X:%.3lf mil,Y:%.3lf mil"",D:%.3lf mil"), CJobFile::MMToInch(dDistX) * 1000., CJobFile::MMToInch(dDistY) * 1000., CJobFile::MMToInch(dDist) * 1000.);
	}
	else
	{
		str.Format(_T(" X:%.3lf mm,Y:%.3lf mm,D:%.3lf mm"), dDistX, dDistY, dDist);
	}


	D2D1_RECT_F frtTextRegion = D2D1::RectF(static_cast<float>(dCenterX),
		static_cast<float>(dCenterY),
		static_cast<float>(dCenterX + (str.GetLength() * pTextFormat->GetFontSize())),
		static_cast<float>(dCenterY + pTextFormat->GetFontSize()));

	//Text Display 시 Tranform 변경
	//기존 그대로 출력시 텍스트가 뒤집어져서 나옴
	D2D1::Matrix3x2F matTransform_Swap;
	D2D1::Matrix3x2F matTransform_Scale;

	matTransform_Scale = D2D1::Matrix3x2F::Scale(fScaleX, fScaleX, D2D1::Point2F(0.f, 0.f));

	matTransform_Swap = D2D1::Matrix3x2F(matTransform_Scale.m11, matTransform_Scale.m12,
		matTransform_Scale.m21, matTransform_Scale.m22, matTransform.dx, matTransform.dy);

	pRender->SetTransform(matTransform_Swap);
	//

	TCHAR *pString = (TCHAR*)(LPCTSTR)str;
	pRender->DrawText(pString, str.GetLength(), pTextFormat, frtTextRegion, m_pWhiteBrush/*, D2D1_DRAW_TEXT_OPTIONS_NONE,
		DWRITE_MEASURING_MODE_NATURAL*/);
	
	pTextFormat->Release();
	
	//원래 Tranform 복원
	pRender->SetTransform(matTransform);
	
	
	//
}