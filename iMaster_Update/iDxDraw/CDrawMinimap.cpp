#include "pch.h"
#include "CDrawMinimap.h"

#include "CDrawFunction.h"
#include "CDrawMakePoint.h"
#include "CEditFunction.h"
#include "CDrawManager.h"

CDrawMinimap::CDrawMinimap()
{

}

CDrawMinimap::~CDrawMinimap()
{
	CDrawFunction::SafeRelease(&m_pGeoObject);
	CDrawFunction::SafeRelease(&m_pWhiteBrush);
	CDrawFunction::SafeRelease(&m_pRedBrush);
	CDrawFunction::SafeRelease(&m_pInspBrush);
	
	CDrawFunction::SafeRelease(&m_pGeoObject_Select);

	_ClearCopyBuffer();
}

void CDrawMinimap::Initial(IN CJobFile *pJobData, IN const ViewInfo_t &stViewInfo, IN CDrawManager *pDrawManager)
{
	m_pJobData = pJobData;
	m_pDrawManager = pDrawManager;

	//하나로 고정.
	m_stSystemSpec.sysBasic.iMaxLayerNum = 1;

	CDirect_Base::SetSystemSpec(&m_stSystemSpec);
	
	m_stView.pHwnd = stViewInfo.pHwnd;
	m_stView.ptSizeXY.x = stViewInfo.ptSizeXY.x;
	m_stView.ptSizeXY.y = stViewInfo.ptSizeXY.y;
	m_stView.rcRect = stViewInfo.rcRect;
	

	CDirect_Base::Initialize(pJobData, &m_stView);

	_Initial_Pen();
}

void CDrawMinimap::SetLink(IN CDrawManager *pDrawManager)
{
	if (pDrawManager != nullptr)
	{
		m_pDrawManager = pDrawManager;
	}
	
}

void CDrawMinimap::SetPanelInfo(PanelInfo_t		*pPanelInfo)
{
	m_pPanelInfo = pPanelInfo;
}

UINT32 CDrawMinimap::SetMode(IN const int &nMode)
{
	if (nMode == enumMode_Measure)
	{
		m_nMode = enumMode_Measure;		
		m_pInspBrush->SetColor(D2D1::ColorF(D2D1::ColorF::LightGreen));
	}
	else if(nMode == enumMode_Thickness)
	{
		m_nMode = enumMode_Thickness;		
		m_pInspBrush->SetColor(D2D1::ColorF(D2D1::ColorF::LightCyan));
	}
	else
	{
		m_nMode = enumMode_Max;		
		m_pInspBrush->SetColor(D2D1::ColorF(D2D1::ColorF::OrangeRed));
	}

	_ClearCopyBuffer();

	CDirect_Base::Render();

	return RESULT_GOOD;
}

UINT32 CDrawMinimap::ChangeViewSize(IN const ViewInfo_t &stViewInfo)
{
	UINT32 nRet;

	m_stView.ptSizeXY.x = stViewInfo.ptSizeXY.x;
	m_stView.ptSizeXY.y = stViewInfo.ptSizeXY.y;
	m_stView.rcRect = stViewInfo.rcRect;

	nRet = CDirect_Base::SetViewInfo(&m_stView);

	ResetZoom();

	return nRet;
}

UINT32		CDrawMinimap::_Initial_Pen()
{
	if (m_pD2DContext == nullptr) return RESULT_BAD;

	CDrawFunction::SafeRelease(&m_pWhiteBrush);

	m_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1), &m_pWhiteBrush);
	m_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(1, 0, 0), &m_pRedBrush);
	m_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LightGreen), &m_pInspBrush);	

	return RESULT_GOOD;
}

UINT32 CDrawMinimap::ResetView()
{
	SetStepIndex(-1);

	CDirect_Base::ClearDisplay();

	return RESULT_GOOD;
}

UINT32 CDrawMinimap::SetUserSetInfo(UserSetInfo_t *pUserSetInfo, UserLayerSet_t *pUserLayerSetInfo)
{
	CDirect_Base::SetUserSetInfo(pUserSetInfo, pUserLayerSetInfo);

	SetStepIndex(m_pUserSetInfo->iStepIdx);

	return RESULT_GOOD;
}

UINT32 CDrawMinimap::SetStepIndex(int nStepIndex)
{
	if (nStepIndex == -1) return RESULT_BAD;

	if (m_pJobData == nullptr) return RESULT_BAD;
	CStep *pStep = m_pJobData->m_arrStep.GetAt(nStepIndex);
	if (m_nSelectStep != nStepIndex)
	{
		//Step이 선택될때 강제로 현재 스텝을 선택하도록 한다.
		m_stSelectInfo.drtRect = pStep->m_Profile.m_MinMax;
		m_stSelectInfo.drtRect.NormalizeRectD();
		m_stSelectInfo.nStep = m_nSelectStep;

		_SetSelect_Once(m_stSelectInfo.drtRect);
		//

		if (pStep == nullptr) return RESULT_BAD;

		D2D1_RECT_F fptPicture;
		fptPicture = D2D1::RectF(DoubleToFloat(pStep->m_Profile.m_MinMax.left), DoubleToFloat(pStep->m_Profile.m_MinMax.top),
			DoubleToFloat(pStep->m_Profile.m_MinMax.right), DoubleToFloat(pStep->m_Profile.m_MinMax.bottom));
		_SetPictureBox(fptPicture);

		
	}
	m_nSelectStep = nStepIndex;
	
	//Clear
	if (m_pGeoObject != nullptr)
	{
		m_pGeoObject->Release();
		m_pGeoObject = nullptr;
	}

	m_vecGeo.clear();
	
	CDirect_Base::Render();

	return RESULT_GOOD;
}

UINT32 CDrawMinimap::ZoomIn()
{
	CPoint ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(*(m_pViewInfo->pHwnd), &ptMouse);

	float newScale = static_cast<float>(m_matScaleView.m11* 1.1f);

	float foldPanX_mm = m_fptPan.x / m_matScaleView.m11;
	float foldPanY_mm = m_fptPan.y / m_matScaleView.m11;
	D2D_POINT_2F fptMouse_mm;
	fptMouse_mm.x = ptMouse.x / m_matScaleView.m11;
	fptMouse_mm.y = ptMouse.y / m_matScaleView.m11;

	m_fptPan.x = (foldPanX_mm - fptMouse_mm.x) * newScale + ptMouse.x;
	m_fptPan.y = (foldPanY_mm - fptMouse_mm.y) * newScale + ptMouse.y;

	m_matScaleView = D2D1::Matrix3x2F::Scale(newScale, newScale, D2D1::Point2F(0.f, 0.f));

	return RESULT_GOOD;
}
UINT32 CDrawMinimap::ZoomOut()
{
	CPoint ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(*(m_pViewInfo->pHwnd), &ptMouse);

	float newScale = static_cast<float>(m_matScaleView.m11* 0.9f);

	float foldPanX_mm = m_fptPan.x / m_matScaleView.m11;
	float foldPanY_mm = m_fptPan.y / m_matScaleView.m11;
	D2D_POINT_2F fptMouse_mm;
	fptMouse_mm.x = ptMouse.x / m_matScaleView.m11;
	fptMouse_mm.y = ptMouse.y / m_matScaleView.m11;

	m_fptPan.x = (foldPanX_mm - fptMouse_mm.x) * newScale + ptMouse.x;
	m_fptPan.y = (foldPanY_mm - fptMouse_mm.y) * newScale + ptMouse.y;

	m_matScaleView = D2D1::Matrix3x2F::Scale(newScale, newScale, D2D1::Point2F(0.f, 0.f));

	return RESULT_GOOD;
}

UINT32 CDrawMinimap::ResetZoom()
{
	D2D1_RECT_F frtPictureBox = m_frtPictureBox;
	frtPictureBox = CDrawFunction::NormalizeRect(m_frtPictureBox);

	float fWidth = fabs(frtPictureBox.right - frtPictureBox.left);
	float fHeight = fabs(frtPictureBox.bottom - frtPictureBox.top);
	float fMargin = 0.05f;//%
	frtPictureBox.left -= (fWidth * fMargin);
	frtPictureBox.top -= (fHeight * fMargin);
	frtPictureBox.right += (fWidth * fMargin);
	frtPictureBox.bottom += (fHeight * fMargin);

	_SetZoomRect(frtPictureBox);

	return RESULT_GOOD;
}

void		CDrawMinimap::_SetZoomRect(IN const D2D1_RECT_F& frtRect_mm)
{
	if (m_pUserSetInfo == nullptr) return;

	CRect rtClient = m_pViewInfo->rcRect;

	float fRectWidth = fabs(frtRect_mm.right - frtRect_mm.left);
	float fRectHeight = fabs(frtRect_mm.bottom - frtRect_mm.top);

	//
	double dAngle = m_pUserSetInfo->dAngle;
	bool bMirror = m_pUserSetInfo->bMirror;
	
	Orient ori = Orient::NoMir0Deg;
	CDrawFunction::GetOrient(dAngle, bMirror, ori);

	CRotMirrArr rotMirrArr;
	rotMirrArr.InputData(0.0, 0.0, ori);

	double dRectWidth, dRectHeight;
	rotMirrArr.FinalPoint(&dRectWidth, &dRectHeight, (double)fRectWidth, (double)fRectHeight);
	fRectWidth = static_cast<float>(fabs(dRectWidth));
	fRectHeight = static_cast<float>(fabs(dRectHeight));
	//

	float fScaleX = fabs(rtClient.Width() / fRectWidth);
	float fScaleY = fabs(rtClient.Height() / fRectHeight);
	float fScale = fScaleX <= fScaleY ? fScaleX : fScaleY;

	m_matScaleView = D2D1::Matrix3x2F(fScale, 0, 0, fScale, 0, 0);	
	//m_matScaleView = D2D1::Matrix3x2F(fScaleX, 0, 0, fScaleY, 0, 0);
	_SetViewScale();
	   	
	D2D1_POINT_2F fptCenter = CDrawFunction::GetRectCenter(frtRect_mm);

	D2D1::Matrix3x2F matViewScale = D2D1::Matrix3x2F(m_ViewScale.m11, m_ViewScale.m12, m_ViewScale.m21, m_ViewScale.m22, 0, 0);
	D2D1_POINT_2F fptCenterT = matViewScale.TransformPoint(fptCenter);

	m_fptPan.x = (-1.0f*fptCenterT.x) + rtClient.Width() / 2.f;
	m_fptPan.y = (-1.0f*fptCenterT.y) + rtClient.Height() / 2.f;

	CDirect_Base::Render();

}

UINT32		CDrawMinimap::_SetPictureBox(IN const D2D1_RECT_F & frtPictureBox)
{
	m_frtPictureBox = frtPictureBox;

	ResetZoom();

	return RESULT_GOOD;
}

void		CDrawMinimap::_SetViewScale()
{
	if (m_pUserSetInfo == nullptr) return;

	double dAngle = m_pUserSetInfo->dAngle;
	bool bMirror = m_pUserSetInfo->bMirror;

	//GetStepOrient(dAngle, bMirror);

	D2D1::Matrix3x2F matRotate = D2D1::Matrix3x2F::Rotation(static_cast<float>(dAngle),
		CDrawFunction::GetRectCenter(m_frtPictureBox));

	D2D1::Matrix3x2F matTotal = m_matScaleView * matRotate;

	if (bMirror)
	{
		if (m_pUserSetInfo->mirrorDir == eDirectionType::eBoth)
		{
			m_ViewScale = D2D1::Matrix3x2F(matTotal.m11 * -1.f, matTotal.m12* -1.f, matTotal.m21*-1.0f, matTotal.m22*-1.0f, m_fptPan.x, m_fptPan.y);
			m_ViewScale = D2D1::Matrix3x2F(m_ViewScale.m11, m_ViewScale.m12, m_ViewScale.m21*-1.0f, m_ViewScale.m22*-1.0f, m_fptPan.x, m_fptPan.y);
		}
		else if (m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal)
		{
			if (dAngle == 90.0 || dAngle == 270.0)
				m_ViewScale = D2D1::Matrix3x2F(matTotal.m11, matTotal.m12, matTotal.m21*1.0f, matTotal.m22*1.0f, m_fptPan.x, m_fptPan.y);
			else
				m_ViewScale = D2D1::Matrix3x2F(matTotal.m11 * -1.f, matTotal.m12* -1.f, matTotal.m21*-1.0f, matTotal.m22*-1.0f, m_fptPan.x, m_fptPan.y);
		}
		else if (m_pUserSetInfo->mirrorDir == eDirectionType::eVertical)
		{
			if (dAngle == 90.0 || dAngle == 270.0)
				m_ViewScale = D2D1::Matrix3x2F(matTotal.m11 * -1.f, matTotal.m12* -1.f, matTotal.m21*-1.0f, matTotal.m22*-1.0f, m_fptPan.x, m_fptPan.y);
			else
			{
				// 				if (m_pUserSetInfo->mcType == eMachineType::eNSIS)
				// 				{
				// 					m_ViewScale = D2D1::Matrix3x2F(matTotal.m11, matTotal.m12, matTotal.m21*-1.0f, matTotal.m22*-1.0f, m_fptPan.x, m_fptPan.y);
				// 				}
				// 				else
				{
					m_ViewScale = D2D1::Matrix3x2F(matTotal.m11, matTotal.m12, matTotal.m21*1.0f, matTotal.m22*1.0f, m_fptPan.x, m_fptPan.y);
				}

			}
		}
		else
		{
			m_ViewScale = D2D1::Matrix3x2F(matTotal.m11, matTotal.m12, matTotal.m21*-1.0f, matTotal.m22*-1.0f, m_fptPan.x, m_fptPan.y);
		}
	}
	else
	{
		m_ViewScale = D2D1::Matrix3x2F(matTotal.m11, matTotal.m12, matTotal.m21*-1.0f, matTotal.m22*-1.0f, m_fptPan.x, m_fptPan.y);
	}

	
}


UINT32 CDrawMinimap::_SetGeometry(vector<GEODRAW> &vecGeoData, ID2D1PathGeometry **pGeoObject, BOOL bOpen)
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

void CDrawMinimap::OnRender()
{
	if (m_nSelectStep == -1)return ;
	if (static_cast<int>(m_vecRender.size()) <= 0) return;

	//Draw Insp
	_SetMeasureUnitInfo();
	//

	_SetViewScale();
	m_vecRender[0]->SetTransform(m_ViewScale);

	_DrawProfile(m_vecRender[0]);

	//Draw insp Unit(Measure Unit)
	_DrawInspUnit(m_vecRender[0]);

	//

	int nCount = 0;
	m_pCompositeEffect->SetInputCount(m_nMaxLayer);

	for (USHORT idx = 0; idx < m_nMaxLayer; idx++)
	{
		ID2D1Bitmap* pBitmap = nullptr;
		m_vecRender[idx]->GetBitmap(&pBitmap);

		if (pBitmap != nullptr)
		{
			m_pCompositeEffect->SetInput(nCount, pBitmap);
			nCount++;
		}
		pBitmap->Release();
		pBitmap = nullptr;
	}

}

UINT32		CDrawMinimap::_DrawProfile(ID2D1RenderTarget *pRender)
{
	if (m_pJobData == nullptr) return RESULT_BAD;
	if (pRender == nullptr) return RESULT_BAD;
	if (m_nSelectStep == -1)return RESULT_BAD;


	if (m_pGeoObject == nullptr)
	{
		SafeRelease(&m_pGeoObject);

		m_vecGeo.clear();
		m_vecvecRect.clear();

// 		//Make Buffer
// 		int nStepCount = static_cast<int>(m_pJobData->m_arrStep.GetCount());//스텝 갯수만큼 만들어서 관리
// 		m_vecvecRect.resize(nStepCount);
// 		//

		//
		m_vecvecRect = CDrawFunction::GetProfileData(m_pJobData, m_nSelectStep);

		CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
		if (pStep == nullptr) return RESULT_BAD;

		CLayer* pProfileLayer = pStep->m_Profile.m_pProfileLayer;
		RECTD drtPickture = pStep->m_Profile.m_MinMax;
		drtPickture.NormalizeRectD();
		m_frtPictureBox = D2D1::RectF(static_cast<float>(drtPickture.left), static_cast<float>(drtPickture.top),
			static_cast<float>(drtPickture.right), static_cast<float>(drtPickture.bottom));

		if (pProfileLayer == nullptr) return RESULT_BAD;
		int nFeatureCount = static_cast<int>(pProfileLayer->m_FeatureFile.m_arrFeature.GetCount());
		if (nFeatureCount > 0)
		{
			vector<RECTD> vecRectTemp;
			for (int i = 0; i < nFeatureCount; i++)
			{
				CFeature *pFeature = pProfileLayer->m_FeatureFile.m_arrFeature.GetAt(i);
				if (pFeature->m_eType == FeatureType::S)
				{
					CFeatureS *pFeatureS = (CFeatureS*)pFeature;
					//vector<RECTD> vecRectTempTemp = _AddProfileFeature(pFeatureS);

					_AddProfileFeature(pFeatureS);

// 					for (auto itRect : vecRectTempTemp)
// 					{
// 						vecRectTemp.push_back(itRect);
// 					}
				}
			}
// 
// 			//0 번째 Step의 Rect 정보
// 			DrawProfileData stTempData;
// 			for (auto itRect : vecRectTemp)
// 			{
// 				stTempData.drtRect = itRect;
// 				m_vecvecRect[m_nSelectStep].push_back(stTempData);
// 			}
		}

		for (UINT i = 0; i < pStep->m_nSubStepCnt; i++)
		{
			CSubStep* pSubStep = pStep->m_arrSubStep[i];
			//Draw SubStep
			_OnRender_SubStep(pSubStep, nullptr, i);
		}

		_SetGeometry(m_vecGeo, &m_pGeoObject, FALSE);


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

	//선택된 Rect
	if (m_vecGeo_Select.size() > 0)
	{
		if (m_pGeoObject_Select == nullptr)
		{//없으면 만들자

			_SetGeometry(m_vecGeo_Select, &m_pGeoObject_Select, FALSE);

		}

		if (m_pGeoObject_Select != nullptr)
		{//m_pGeoObject가 널이 아닐경우만 Draw

			pRender->DrawGeometry(m_pGeoObject_Select, m_pRedBrush, 1 / fScale);
		}
	}

	return RESULT_GOOD;
}


void CDrawMinimap::_OnRender_SubStep(CSubStep* pSubStep, CRotMirrArr *pRotMirrArr,
	IN const int &nSubIndex, IN int nSubLevel, IN int nParent_StepX, IN int nParent_StepY)
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

	//Check MeasureUnit
	std::vector<std::pair<CPoint, CPoint>> vcMeasure;
	for (auto it : m_vecMeasureUnitInfo)
	{
		if (nSubLevel >= 0)
		{
			if (it.nSubSubIndex == nSubIndex)
			{
				if (it.nSubIndex == nSubLevel && it.nStripX == nParent_StepX && it.nStripY == nParent_StepY)
					vcMeasure.emplace_back(std::make_pair(CPoint(it.nStripX, it.nStripY), CPoint(it.nUnitX, it.nUnitY)));
			}
		}
	}
	//

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

					_OnRender_SubStep(pSubStep->m_arrNextStep[j], rotMirrArr, j, nSubIndex, x_step, y_step);

				}
			}

			vector<RECTD> vecRectTemp;
			for (int feno = 0; feno < pSubStep->m_pSubStep->m_Profile.m_pProfileLayer->m_FeatureFile.m_arrFeature.GetSize(); feno++)	//feature drawing
			{
				CFeature* pFeatureTmp = pSubStep->m_pSubStep->m_Profile.m_pProfileLayer->m_FeatureFile.m_arrFeature.GetAt(feno);

				if (pFeatureTmp->m_eType == FeatureType::S)
				{
					CFeatureS *pFeatureS = (CFeatureS*)pFeatureTmp;
					vector<RECTD> vecRectTempTemp = _AddProfileFeature(pFeatureS, rotMirrArr);

					for (auto itRect : vecRectTempTemp)
					{
						vecRectTemp.push_back(itRect);
					}
				}
			}

			int nStep = pSubStep->m_pSubStep->m_nStepID;
			if (m_vecvecRect.size() > nStep)
			{
				for (auto itRect : vecRectTemp)
				{
					auto itExist = std::find(vcMeasure.begin(), vcMeasure.end(), std::make_pair(CPoint(nParent_StepX, nParent_StepY), CPoint(x_step, y_step)));
					if (itExist != vcMeasure.end())
					{
						int nProfileCount = static_cast<int>(m_vecvecRect[nStep].size());
						for (int i = 0; i < nProfileCount; i++)
						{
							if (m_vecvecRect[nStep][i].drtRect.IsPtInRectD(itRect.CenterX(), itRect.CenterY()) == TRUE)
							{
								m_vecvecRect[nStep][i].bInspect = TRUE;
								break;
							}
						}
					}

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


vector<RECTD> CDrawMinimap::_AddProfileFeature(CFeatureS *pFeatureS, CRotMirrArr *rotMirrArr)
{
	if (pFeatureS == nullptr) return vector<RECTD>();

	vector<RECTD> vecRect;

	int nCountS = static_cast<int>(pFeatureS->m_arrObOe.GetSize());
	for (int s = 0; s < nCountS; s++)
	{
		CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(s);

		if (m_pGeoObject == nullptr)
		{//m_pGeoObject가 널이 아닐경우 GeoPath를 만들어줌
			vector<RECTD> vecRectTemp = _AddProfile_ObOe(pObOe, rotMirrArr);

			for (auto itRect : vecRectTemp)
			{
				vecRect.push_back(itRect);
			}
		}
	}

	return vecRect;
}

vector<RECTD>  CDrawMinimap::_AddProfile_ObOe(CObOe* pSurface, CRotMirrArr *rotMirrArr)
{
	if (pSurface == nullptr) return vector<RECTD>();
	int nPtCount = static_cast<int>(pSurface->m_arrOsOc.GetSize());

	if (nPtCount < 1) return vector<RECTD>();

	BOOL bCreateRotMirr = FALSE;
	if (rotMirrArr == nullptr)
	{
		bCreateRotMirr = TRUE;
		rotMirrArr = new CRotMirrArr;
		rotMirrArr->InputData(0.0, 0.0, Orient::NoMir0Deg);
	}

	vector<GEODRAW> vecCurProfile;
	GEODRAW stGeo;
	double dNewX, dNewY;
	rotMirrArr->FinalPoint(&dNewX, &dNewY, pSurface->m_dXbs, pSurface->m_dYbs);
	stGeo.SetBasePoint(dNewX, dNewY);
	m_vecGeo.push_back(stGeo);
	vecCurProfile.push_back(stGeo);
	stGeo._Reset();

	
	for (int i = 0; i < nPtCount; i++)
	{
		COsOc* pOsOc = pSurface->m_arrOsOc.GetAt(i);


		rotMirrArr->FinalPoint(&dNewX, &dNewY, pOsOc->m_dX, pOsOc->m_dY);

		if (pOsOc->m_bOsOc == true)
		{//Line
			stGeo.SetLine(dNewX, dNewY);
			m_vecGeo.push_back(stGeo);
			vecCurProfile.push_back(stGeo);
			stGeo._Reset();
		}
		else
		{
			double dNew_CX, dNew_CY;
			rotMirrArr->FinalPoint(&dNew_CX, &dNew_CY, ((COc*)pOsOc)->m_dXc, ((COc*)pOsOc)->m_dYc);
			stGeo.SetArc(dNewX, dNewY, !((COc*)pOsOc)->m_bCw, dNew_CX, dNew_CY);
			m_vecGeo.push_back(stGeo);
			vecCurProfile.push_back(stGeo);
			stGeo._Reset();
		}

	}

	if (bCreateRotMirr == TRUE)
	{
		rotMirrArr->ClearAll();
		delete rotMirrArr;
		rotMirrArr = nullptr;
	}

	vector<RECTD> vecRect;
	//Rect로 가정한다.
	if (nPtCount == 4)
	{
		RECTD drcTemp;
		drcTemp.left = vecCurProfile[0].ptPoint.x;
		drcTemp.top = vecCurProfile[0].ptPoint.y;
		drcTemp.right = vecCurProfile[2].ptPoint.x;
		drcTemp.bottom = vecCurProfile[2].ptPoint.y;
		drcTemp.NormalizeRectD();

		vecRect.push_back(drcTemp);
	}
	else
	{
		return vector<RECTD>();
	}

	return vecRect;

}

void CDrawMinimap::_ClearProfileRect()
{
	int nStepCount = static_cast<int>(m_vecvecRect.size());
	for (int nStep = 0; nStep < nStepCount; nStep++)
	{
		m_vecvecRect[nStep].clear();
	}
	m_vecvecRect.clear();
}

UINT32			CDrawMinimap::_SetMeasureUnitInfo()
{
	if (m_pPanelInfo == nullptr) return RESULT_BAD;

	m_vecMeasureUnitInfo.clear();

	MeasureUnit stMeasureUnit;
	for (auto it : m_pPanelInfo->stMeasureInfo.vcUnits)
	{
		stMeasureUnit.nSubIndex = it->stStepRpt.StripStep;
		stMeasureUnit.nStripX = it->stStepRpt.StripIdx.x;
		stMeasureUnit.nStripY = it->stStepRpt.StripIdx.y;

		stMeasureUnit.nSubSubIndex = it->stStepRpt.UnitStep;
		stMeasureUnit.nUnitX = it->stStepRpt.UnitIdx.x;
		stMeasureUnit.nUnitY = it->stStepRpt.UnitIdx.y;

		if (m_nMode == enumMode_Measure && it->eMapMode == eMinimapMode::Measure)
		{
			m_vecMeasureUnitInfo.push_back(stMeasureUnit);
		}
		
		if (m_nMode == enumMode_Thickness && it->eMapMode == eMinimapMode::Thickness)
		{
			m_vecMeasureUnitInfo.push_back(stMeasureUnit);
		}
		
		if (m_nMode == enumMode_Max)
		{
			m_vecMeasureUnitInfo.push_back(stMeasureUnit);
		}
	}

	return RESULT_GOOD;
}

void			CDrawMinimap::_DrawInspUnit(ID2D1RenderTarget *pRender)
{//bInsp가 TRUE 인 영역만 색을 채운다
	
	if (pRender == nullptr) return;

	vector<GEODRAW> vecInsp;
	
	//bInsp가 TRUE 인 영역을 찾는다
	int nStepCount = static_cast<int>(m_vecvecRect.size());
	for (int nStep = 0; nStep < nStepCount; nStep++)
	{
		int nCount = static_cast<int>(m_vecvecRect[nStep].size());
		for (int nIndex = 0; nIndex < nCount; nIndex++)
		{
			if (m_vecvecRect[nStep][nIndex].bInspect == TRUE)
			{
				GEODRAW stTemp;
				RECTD *pRect = &m_vecvecRect[nStep][nIndex].drtRect;
				
				stTemp.SetBasePoint(pRect->left, pRect->top);
				vecInsp.push_back(stTemp);
				stTemp._Reset();

				stTemp.SetLine(pRect->right, pRect->top);
				vecInsp.push_back(stTemp);
				stTemp._Reset();

				stTemp.SetLine(pRect->right, pRect->bottom);
				vecInsp.push_back(stTemp);
				stTemp._Reset();

				stTemp.SetLine(pRect->left, pRect->bottom);
				vecInsp.push_back(stTemp);
				stTemp._Reset();

				stTemp.SetLine(pRect->left, pRect->top);
				vecInsp.push_back(stTemp);
				stTemp._Reset();
			}
			
		}
	}

	ID2D1PathGeometry *pInspGeo = nullptr;
	//Geo 생성
	_SetGeometry(vecInsp, &pInspGeo, FALSE);

	//Draw
	pRender->FillGeometry(pInspGeo, m_pInspBrush);
}

UINT32		 CDrawMinimap::_ResetSelect()
{
	m_vecGeo_Select.clear();
	CDrawFunction::SafeRelease(&m_pGeoObject_Select);

	return RESULT_GOOD;
}

UINT32		CDrawMinimap::_SetSelect_Once(IN const RECTD &drtSelect)
{
	m_vecGeo_Select.clear();
	CDrawFunction::SafeRelease(&m_pGeoObject_Select);

	GEODRAW stGeoDraw;
	stGeoDraw.SetBasePoint(drtSelect.left, drtSelect.top);
	m_vecGeo_Select.push_back(stGeoDraw);

	stGeoDraw.SetLine(drtSelect.right, drtSelect.top);
	m_vecGeo_Select.push_back(stGeoDraw);

	stGeoDraw.SetLine(drtSelect.right, drtSelect.bottom);
	m_vecGeo_Select.push_back(stGeoDraw);

	stGeoDraw.SetLine(drtSelect.left, drtSelect.bottom);
	m_vecGeo_Select.push_back(stGeoDraw);

	stGeoDraw.SetLine(drtSelect.left, drtSelect.top);
	m_vecGeo_Select.push_back(stGeoDraw);

	return RESULT_GOOD;
}


UINT32		CDrawMinimap::_SetSelect_Multi(IN const vector<MinimapSelect> &vecSelectInfo)
{
	m_vecGeo_Select.clear();
	CDrawFunction::SafeRelease(&m_pGeoObject_Select);

	GEODRAW stGeoDraw;

	int nSelectCnt = static_cast<int>(vecSelectInfo.size());
	for (int nCnt = 0; nCnt < nSelectCnt; nCnt++)
	{
		stGeoDraw.SetBasePoint(vecSelectInfo[nCnt].drtRect.left, vecSelectInfo[nCnt].drtRect.top);
		m_vecGeo_Select.push_back(stGeoDraw);

		stGeoDraw.SetLine(vecSelectInfo[nCnt].drtRect.right, vecSelectInfo[nCnt].drtRect.top);
		m_vecGeo_Select.push_back(stGeoDraw);

		stGeoDraw.SetLine(vecSelectInfo[nCnt].drtRect.right, vecSelectInfo[nCnt].drtRect.bottom);
		m_vecGeo_Select.push_back(stGeoDraw);

		stGeoDraw.SetLine(vecSelectInfo[nCnt].drtRect.left, vecSelectInfo[nCnt].drtRect.bottom);
		m_vecGeo_Select.push_back(stGeoDraw);

		stGeoDraw.SetLine(vecSelectInfo[nCnt].drtRect.left, vecSelectInfo[nCnt].drtRect.top);
		m_vecGeo_Select.push_back(stGeoDraw);
	}

	return RESULT_GOOD;
}

BOOL		 CDrawMinimap::_CheckSelect(IN const D2D1_POINT_2F &fptPoint, OUT MinimapSelect &stSelect)
{
	BOOL bSelect = FALSE;

	RECTD drtSelect;
	int nSelectStep;
	int nStepCount = static_cast<int>(m_vecvecRect.size());
	if (nStepCount == 0) return RESULT_BAD;
	for (int nStep = 0; nStep < nStepCount; nStep++)
	{
		//int nStep = 0;
		int nRectCount = static_cast<int>(m_vecvecRect[nStep].size());

		for (int nRect = 0; nRect < nRectCount; nRect++)
		{
			if (m_vecvecRect[nStep][nRect].drtRect.IsPtInRectD(fptPoint.x, fptPoint.y) == TRUE)
			{
				drtSelect = m_vecvecRect[nStep][nRect].drtRect;
				nSelectStep = nStep;

				bSelect = TRUE;
				break;
			}
		}

		if (bSelect == TRUE)
		{
			break;
		}
	}

	if (bSelect == TRUE)
	{
		stSelect.drtRect = drtSelect;
		stSelect.nStep = nSelectStep;
		stSelect.drtRect.NormalizeRectD();
	}
	//else
	//{//아무것도 선택되지 않았다면, Panel을 강제로 선택
	//	if (m_pUserLayerSetInfo != nullptr)
	//	{
	//		int nPanelStep = m_pUserLayerSetInfo->nPanelStepIdx;
	//
	//		if (m_pJobData == nullptr) return bSelect;
	//		if (m_pJobData->m_arrStep.GetCount() <= 0 ||
	//			m_pJobData->m_arrStep.GetCount() <= nPanelStep) return bSelect;
	//
	//		CStep *pStep = m_pJobData->m_arrStep.GetAt(nPanelStep);
	//		if (pStep == nullptr) return bSelect;
	//
	//
	//		bSelect = TRUE;
	//		stSelect.drtRect = pStep->m_Profile.m_MinMax;
	//		stSelect.nStep = nPanelStep;
	//		stSelect.drtRect.NormalizeRectD();
	//	}
	//}

	m_vecMultiSelectInfo.push_back(stSelect);

	return bSelect;
}


BOOL		 CDrawMinimap::_CheckSelect_Multi(IN const D2D1_POINT_2F &fptPoint, OUT vector<MinimapSelect> &stSelectInfo)
{
	BOOL bSelect = FALSE;

	RECTD drtSelect;
	int nSelectStep;
	int nStepCount = static_cast<int>(m_vecvecRect.size());
	if (nStepCount == 0) return RESULT_BAD;
	MinimapSelect stSelect;

	/*for (int nStep = 0; nStep < nStepCount; nStep++)
	{*/
	int nStep = m_stSelectInfo.nStep;
	if (nStep < 0) return FALSE;
	if (nStep >= static_cast<int>(m_vecvecRect.size())) return FALSE;

	int nRectCount = static_cast<int>(m_vecvecRect[nStep].size());
	for (int nRect = 0; nRect < nRectCount; nRect++)
	{
		if (m_vecvecRect[nStep][nRect].drtRect.IsPtInRectD(fptPoint.x, fptPoint.y) == TRUE)
		{
			drtSelect = m_vecvecRect[nStep][nRect].drtRect;
			nSelectStep = nStep;

			bSelect = TRUE;

			stSelect.drtRect = drtSelect;
			stSelect.nStep = nSelectStep;
			stSelect.drtRect.NormalizeRectD();

			stSelectInfo.push_back(stSelect);

			break;
		}
	}

	return bSelect;
}


UINT32 CDrawMinimap::GetSelectRect(OUT RECTD &drtRect)
{
	if (m_stSelectInfo.drtRect.IsRectNull())
	{
		//아무것도 선택되지 않았을 경우
		drtRect.SetRectEmpty();
		return RESULT_BAD;
	}
	else
	{//선택되어 있다
		drtRect = m_stSelectInfo.drtRect;
	}

	return RESULT_GOOD;
}

UINT32 CDrawMinimap::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_CONTROL) == MK_CONTROL)
	{//Panning
		HCURSOR hHandCursor = ::LoadCursor(NULL, IDC_HAND);
		SetCursor(hHandCursor);

		m_bPanMode = TRUE;
		m_ptPrevMove = point;
	}

	CDirect_Base::Render();

	return RESULT_GOOD;
}

UINT32 CDrawMinimap::OnLButtonUp(UINT nFlags, CPoint point)
{
	D2D1_POINT_2F fptPoint = D2D1::Point2F(static_cast<float>(point.x), static_cast<float>(point.y));
	D2D1_POINT_2F fptReal = CDrawFunction::DetansformPoint(fptPoint, m_ViewScale, m_pUserSetInfo);

	BOOL bSelect = FALSE;

	if (m_bPanMode == TRUE)
	{
		m_bPanMode = FALSE;
	}
	else if ((nFlags & MK_SHIFT) == MK_SHIFT)
	{			
		bSelect = _CheckSelect_Multi(fptReal, m_vecMultiSelectInfo);

		if (bSelect == TRUE)
		{
			_SetSelect_Multi(m_vecMultiSelectInfo);
		}
		else
		{
			m_vecMultiSelectInfo.clear();
			_ResetSelect();
		}
	}
	else
	{
		m_vecMultiSelectInfo.clear();
		bSelect = _CheckSelect(fptReal, m_stSelectInfo);
		
		if (bSelect == TRUE)
		{
			_SetSelect_Once(m_stSelectInfo.drtRect);
			
		}
		else
		{
			m_stSelectInfo._Reset();
			m_vecMultiSelectInfo.clear();
			_ResetSelect();
		}
	}

	

	CDirect_Base::Render();

	return RESULT_GOOD;
}


UINT32 CDrawMinimap::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bPanMode == TRUE)
	{
		HCURSOR hHandCursor = ::LoadCursor(NULL, IDC_HAND);
		SetCursor(hHandCursor);

		D2D_POINT_2F fptCurMove;
		//fptCurMove.x = (point.x - m_ptPrevMove.x) / m_matScaleView.m11;
		//fptCurMove.y = (point.y - m_ptPrevMove.y) / m_matScaleView.m11;

		fptCurMove.x = static_cast<float>(point.x - m_ptPrevMove.x);
		fptCurMove.y = static_cast<float>(point.y - m_ptPrevMove.y);

		//m_matTransView

		//m_matTransView = D2D1::Matrix3x2F::Translation(500, 300);
		m_fptPan.x += fptCurMove.x;
		m_fptPan.y += fptCurMove.y;

		m_ptPrevMove = point;

		CDirect_Base::Render();

	}

	return RESULT_GOOD;
}

UINT32 CDrawMinimap::OnRButtonDown(UINT nFlags, CPoint point)
{
	CDirect_Base::Render();

	return RESULT_GOOD;
}

UINT32 CDrawMinimap::OnRButtonUp(UINT nFlags, CPoint point)
{
	CDirect_Base::Render();

	return RESULT_GOOD;
}

UINT32 CDrawMinimap::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if ((nFlags & MK_CONTROL) == MK_CONTROL)
	{
		if (zDelta > 0)
			ZoomIn();
		else if (zDelta < 0)
			ZoomOut();

		CDirect_Base::Render();
	}

	return RESULT_GOOD;
}

//Clear Copy Buffer
UINT32		CDrawMinimap::_ClearCopyBuffer(int nMode)
{
	m_stCopyInfo._Reset();

	if (nMode == enumMode_Measure || nMode == enumMode_Max)
	{
		int nSize = static_cast<int>(m_vecCopyTemp.size());

		for (int i = 0; i < nSize; i++)
		{
			m_vecCopyTemp[i]->Reset();
		}
		m_vecCopyTemp.clear();


		int nAlignSize = static_cast<int>(m_vecCopyAlignTemp.size());

		for (int i = 0; i < nAlignSize; i++)
		{
			m_vecCopyAlignTemp[i]->Reset();
		}
		m_vecCopyAlignTemp.clear();
	}
	else if (nMode == enumMode_Thickness || nMode == enumMode_Max)
	{
		int nSrSize = static_cast<int>(m_vecCopyTemp_SR.size());

		for (int i = 0; i < nSrSize; i++)
		{
			m_vecCopyTemp_SR[i]->Reset();
		}
		m_vecCopyTemp_SR.clear();
	}


	return RESULT_GOOD;
}

CLayer*			CDrawMinimap::_GetEditLayer(OUT int &nLayer)
{//현재 편집 대상인 Layer의 포인터 반환

	nLayer = -1;

	if (m_pUserLayerSetInfo == nullptr) return nullptr;
	if (m_pJobData == nullptr) return nullptr;

	int nStep = m_pUserLayerSetInfo->nPanelStepIdx;
	if (nStep < 0) return nullptr;
	if (nStep >= m_pJobData->m_arrStep.GetSize()) return nullptr;

	CString strLayerName = _T("");
	//Get Layer Name
	int nCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());
	for (int i = 0; i < nCount; i++)
	{
		if (m_nMode == enumMode_Measure)
		{
			if (m_pUserLayerSetInfo->vcLayerSet[i]->strDefine == MEASURE_LAYER_TXT)
			{
				strLayerName = m_pUserLayerSetInfo->vcLayerSet[i]->strLayer;
				break;
			}
		}
		else
		{//SR
			if (m_pUserLayerSetInfo->vcLayerSet[i]->strDefine == THICKNESS_LAYER_TXT)
			{
				strLayerName = m_pUserLayerSetInfo->vcLayerSet[i]->strLayer;
				break;
			}
		}
	}
	if (strLayerName == _T("")) return nullptr;

	CStep *pStep = m_pJobData->m_arrStep.GetAt(nStep);
	if (pStep == nullptr) return nullptr;

	//대상 Layer 
	int nLayerIndex = 0;
	CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, strLayerName, nLayerIndex);
	if (pLayer == nullptr) return nullptr;

	nLayer = nLayerIndex;

	return pLayer;
}


//Inspect
UINT32		CDrawMinimap::SetInspect_Unit()
{
	int nLayer = 0;
	CLayer *pLayer = _GetEditLayer(nLayer);
	if (pLayer == nullptr) return RESULT_BAD;

	RECTD drtSelect;
	GetSelectRect(drtSelect);
	
	_SetInspect_Unit(pLayer, drtSelect);

	if (m_pDrawManager != nullptr)
	{
		m_pDrawManager->SetDrawOnly_MeasureUnit(TRUE);
	}

	return RESULT_GOOD;
}

//Copy
UINT32		CDrawMinimap::SetCopy_Feature(BOOL &bCopy)
{
	// 전체 MP, TP 복사 
	if (m_nMode == enumMode_Max)
	{
		m_nMode = enumMode_Measure;

		int nLayer = 0;
		CLayer *pLayer = _GetEditLayer(nLayer);
		if (pLayer == nullptr) return RESULT_BAD;

		//_ClearCopyBuffer();

		m_stSelectInfo.drtRect.NormalizeRectD();
		vector<int>	vecFeatureIndex = _GetInsideFeatureIndex(pLayer, m_stSelectInfo.drtRect);
		if (vecFeatureIndex.size() <= 0)
		{
			AfxMessageBox(_T("Measure 가 존재하지 않습니다."));
			return RESULT_BAD;
		}
		vector<int>	vecAlignIndex;
		if (m_nMode == enumMode_Measure)
		{
			vecAlignIndex = _GetInsideAlignIndex(pLayer, m_stSelectInfo.drtRect);
			if (vecAlignIndex.size() <= 0)
			{
				AfxMessageBox(_T("Align 이 존재하지 않습니다."));
				return RESULT_BAD;
			}
		}
		else
		{
			vecAlignIndex.clear();
		}

		_SetCopyBuffer(pLayer, nLayer, vecFeatureIndex, vecAlignIndex, m_nMode);

		m_nMode = enumMode_Thickness;

		nLayer = 0;
		pLayer = _GetEditLayer(nLayer);
		if (pLayer == nullptr) return RESULT_BAD;

		//_ClearCopyBuffer();

		m_stSelectInfo.drtRect.NormalizeRectD();
		vecFeatureIndex = _GetInsideFeatureIndex(pLayer, m_stSelectInfo.drtRect);
		if (vecFeatureIndex.size() <= 0)
		{
			AfxMessageBox(_T("Measure 가 존재하지 않습니다."));
			return RESULT_BAD;
		}		

		vecAlignIndex.clear();

		_SetCopyBuffer(pLayer, nLayer, vecFeatureIndex, vecAlignIndex, m_nMode);

		bCopy = TRUE;
		m_nMode = enumMode_Max;
	}
	else
	{// NSIS , SR 에 따라 각각 MP, TP 복사 
		int nLayer = 0;
		CLayer *pLayer = _GetEditLayer(nLayer);
		if (pLayer == nullptr) return RESULT_BAD;

		//_ClearCopyBuffer();
	
		m_stSelectInfo.drtRect.NormalizeRectD();
		vector<int>	vecFeatureIndex = _GetInsideFeatureIndex(pLayer, m_stSelectInfo.drtRect);
		if (vecFeatureIndex.size() <= 0)
		{
			AfxMessageBox(_T("Measure 가 존재하지 않습니다."));
			return RESULT_BAD;
		}	
		vector<int>	vecAlignIndex;
		if (m_nMode == enumMode_Measure)
		{
			vecAlignIndex = _GetInsideAlignIndex(pLayer, m_stSelectInfo.drtRect);
			if (vecAlignIndex.size() <= 0)
			{
				AfxMessageBox(_T("Align 이 존재하지 않습니다."));
				return RESULT_BAD;
			}
		}
		else
		{
			vecAlignIndex.clear();
		}

		_SetCopyBuffer(pLayer, nLayer, vecFeatureIndex, vecAlignIndex, m_nMode);

		bCopy = TRUE;
	}
	
	return RESULT_GOOD;
}

UINT32			CDrawMinimap::SetPaste_Feature()
{
	if (m_nMode == enumMode_Max)
	{
		m_nMode = enumMode_Measure;
		int nLayer = 0;
		CLayer *pLayer = _GetEditLayer(nLayer);
		if (pLayer == nullptr) return RESULT_BAD;

		int nCopyCount = GetCopyBufferCount(m_nMode);
		//CopyBuffer가 있는지 확인
		if (nCopyCount <= 0) return RESULT_BAD;

		int nSelectCnt = static_cast<int>(m_vecMultiSelectInfo.size());
		for (int nCnt = 0; nCnt < nSelectCnt; nCnt++)
		{

			if (m_vecMultiSelectInfo[nCnt].nStep == -1) return RESULT_BAD;
			if (m_stCopyInfo.nStep == -1) return RESULT_BAD;
			if (m_vecMultiSelectInfo[nCnt].nStep != m_stCopyInfo.nStep) return RESULT_BAD;

			//현재 선택되어 있는 내부 Feature를 지움
			_SetDelete_Feature(pLayer, m_vecMultiSelectInfo[nCnt].drtRect);

			//붙여 넣기할때의 이동량
			CPointD dptMove;
			m_vecMultiSelectInfo[nCnt].drtRect.NormalizeRectD();//Dest
			m_stCopyInfo.drtRect.NormalizeRectD();//Src

			dptMove.x = m_vecMultiSelectInfo[nCnt].drtRect.left - m_stCopyInfo.drtRect.left;
			dptMove.y = m_vecMultiSelectInfo[nCnt].drtRect.top - m_stCopyInfo.drtRect.top;
			//

			//Panel Step에서만 추가..
			int nStep = m_pUserLayerSetInfo->nPanelStepIdx;
			CStep *pStep = m_pJobData->m_arrStep.GetAt(nStep);

			_SetPaste_Feature(pStep, m_vecCopyTemp, dptMove);

			if (m_nMode == enumMode_Measure)
			{
				_SetPaste_AlignFeature(pStep, m_vecCopyAlignTemp, dptMove);
			}
		}

		m_nMode = enumMode_Thickness;
		nLayer = 0;
		pLayer = _GetEditLayer(nLayer);
		if (pLayer == nullptr) return RESULT_BAD;

		nCopyCount = GetCopyBufferCount(m_nMode);
		//CopyBuffer가 있는지 확인
		if (nCopyCount <= 0) return RESULT_BAD;


		nSelectCnt = static_cast<int>(m_vecMultiSelectInfo.size());
		for (int nCnt = 0; nCnt < nSelectCnt; nCnt++)
		{

			if (m_vecMultiSelectInfo[nCnt].nStep == -1) return RESULT_BAD;
			if (m_stCopyInfo.nStep == -1) return RESULT_BAD;
			if (m_vecMultiSelectInfo[nCnt].nStep != m_stCopyInfo.nStep) return RESULT_BAD;

			//현재 선택되어 있는 내부 Feature를 지움
			_SetDelete_Feature(pLayer, m_vecMultiSelectInfo[nCnt].drtRect);

			//붙여 넣기할때의 이동량
			CPointD dptMove;
			m_vecMultiSelectInfo[nCnt].drtRect.NormalizeRectD();//Dest
			m_stCopyInfo.drtRect.NormalizeRectD();//Src

			dptMove.x = m_vecMultiSelectInfo[nCnt].drtRect.left - m_stCopyInfo.drtRect.left;
			dptMove.y = m_vecMultiSelectInfo[nCnt].drtRect.top - m_stCopyInfo.drtRect.top;
			//

			//Panel Step에서만 추가..
			int nStep = m_pUserLayerSetInfo->nPanelStepIdx;
			CStep *pStep = m_pJobData->m_arrStep.GetAt(nStep);

			_SetPaste_Feature(pStep, m_vecCopyTemp_SR, dptMove);

		}

		m_nMode = enumMode_Max;
	}
	else
	{
		int nLayer = 0;
		CLayer *pLayer = _GetEditLayer(nLayer);
		if (pLayer == nullptr) return RESULT_BAD;

		int nCopyCount = GetCopyBufferCount(m_nMode);
		//CopyBuffer가 있는지 확인
		if (nCopyCount <= 0 ) return RESULT_BAD;


		int nSelectCnt = static_cast<int>(m_vecMultiSelectInfo.size());
		for (int nCnt = 0; nCnt < nSelectCnt; nCnt++)
		{

			if (m_vecMultiSelectInfo[nCnt].nStep == -1) return RESULT_BAD;
			if (m_stCopyInfo.nStep == -1) return RESULT_BAD;
			if (m_vecMultiSelectInfo[nCnt].nStep != m_stCopyInfo.nStep) return RESULT_BAD;

			//현재 선택되어 있는 내부 Feature를 지움
			_SetDelete_Feature(pLayer, m_vecMultiSelectInfo[nCnt].drtRect);
	
			//붙여 넣기할때의 이동량
			CPointD dptMove;
			m_vecMultiSelectInfo[nCnt].drtRect.NormalizeRectD();//Dest
			m_stCopyInfo.drtRect.NormalizeRectD();//Src

			dptMove.x = m_vecMultiSelectInfo[nCnt].drtRect.left - m_stCopyInfo.drtRect.left;
			dptMove.y = m_vecMultiSelectInfo[nCnt].drtRect.top - m_stCopyInfo.drtRect.top;
			//

			//Panel Step에서만 추가..
			int nStep = m_pUserLayerSetInfo->nPanelStepIdx;
			CStep *pStep = m_pJobData->m_arrStep.GetAt(nStep);
	
			if (m_nMode == enumMode_Measure)
			{
				_SetPaste_Feature(pStep, m_vecCopyTemp, dptMove);
			}
			else if (m_nMode == enumMode_Thickness)
			{
				_SetPaste_Feature(pStep, m_vecCopyTemp_SR, dptMove);
			}

			if (m_nMode == enumMode_Measure)
			{
				_SetPaste_AlignFeature(pStep, m_vecCopyAlignTemp, dptMove);
			}
		}

	}




	// 2022.06.17
	// 김준호 주석처리
	// 다중 선택 후 붙여넣기 위해  단일 붙여넣기 주석처리

	//if (m_stSelectInfo.nStep == -1) return RESULT_BAD;
	//if (m_stCopyInfo.nStep == -1) return RESULT_BAD;
	//if (m_stSelectInfo.nStep != m_stCopyInfo.nStep) return RESULT_BAD;


	////현재 선택되어 있는 내부 Feature를 지움
	//_SetDelete_Feature(pLayer, m_stSelectInfo.drtRect);
	//
	////붙여 넣기할때의 이동량
	//CPointD dptMove;
	//m_stSelectInfo.drtRect.NormalizeRectD();//Dest
	//m_stCopyInfo.drtRect.NormalizeRectD();//Src

	//dptMove.x = m_stSelectInfo.drtRect.left - m_stCopyInfo.drtRect.left;
	//dptMove.y = m_stSelectInfo.drtRect.top - m_stCopyInfo.drtRect.top;
	////

	////Panel Step에서만 추가..
	//int nStep = m_pUserLayerSetInfo->nPanelStepIdx;
	//CStep *pStep = m_pJobData->m_arrStep.GetAt(nStep);
	//
	//_SetPaste_Feature(pStep, m_vecCopyTemp, dptMove);



	return RESULT_GOOD;
}

UINT32			CDrawMinimap::SetDelete_Feature()
{
	// 전체보기 선택시 삭제할때 MP,TP 전체 삭제
	if (m_nMode == enumMode_Max)
	{
		m_nMode = enumMode_Measure;
 		int nLayer = 0;
		CLayer *pLayer = _GetEditLayer(nLayer);
		if (pLayer == nullptr) return RESULT_BAD;

		int nSelectCnt = static_cast<int>(m_vecMultiSelectInfo.size());
		for (int nCnt = 0; nCnt < nSelectCnt; nCnt++)
		{
			_SetDelete_Feature(pLayer, m_vecMultiSelectInfo[nCnt].drtRect);
		}
		m_nMode = enumMode_Thickness;		
		pLayer = _GetEditLayer(nLayer);
		if (pLayer == nullptr) return RESULT_BAD;

		nSelectCnt = static_cast<int>(m_vecMultiSelectInfo.size());
		for (int nCnt = 0; nCnt < nSelectCnt; nCnt++)
		{
			_SetDelete_Feature(pLayer, m_vecMultiSelectInfo[nCnt].drtRect);
		}
		m_nMode = enumMode_Max;
	}
	else 
	{// NSIS , SR 에 따라 각각 MP, TP 삭제
		int nLayer = 0;	
		CLayer *pLayer = _GetEditLayer(nLayer);
		if (pLayer == nullptr) return RESULT_BAD;

		int nSelectCnt = static_cast<int>(m_vecMultiSelectInfo.size());
		for (int nCnt = 0; nCnt < nSelectCnt; nCnt++)
		{
			_SetDelete_Feature(pLayer, m_vecMultiSelectInfo[nCnt].drtRect);
		}
	}

	// 2022.06.17
	// 김준호 주석처리
	// 다중 선택 후 삭제 기능 위해 단일 삭제 주석처리
	//_SetDelete_Feature(pLayer, m_stSelectInfo.drtRect);

	return RESULT_GOOD;
}

int				CDrawMinimap::GetCopyBufferCount(int nMode)
{
	int nCount = 0;
	if (nMode == enumMode_Measure)
	{
		nCount = static_cast<int>(m_vecCopyTemp.size());
	}
	else if (nMode == enumMode_Thickness)
	{
		nCount = static_cast<int>(m_vecCopyTemp_SR.size());
	}
	return nCount;
}

vector<int>			CDrawMinimap::_GetInsideFeatureIndex(IN CLayer* pLayer, IN const RECTD &drtRect_mm)
{
	if (pLayer == nullptr) return vector<int>();

	vector<int> vecFeatureIndex;
	
	RECTD drtTemp = drtRect_mm;
	RECTD drtFeature;
	int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());	
	for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
	{
		CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nfeno);
		
		drtFeature = pFeature->m_MinMaxRect;		
		if (drtFeature.IsIntersection(drtRect_mm) == FALSE)
		{
			continue;
		}
		
		vecFeatureIndex.push_back(nfeno);
	}	

	return vecFeatureIndex;
}


vector<int>			CDrawMinimap::_GetInsideAlignIndex(IN CLayer* pLayer, IN const RECTD &drtRect_mm)
{
	if (pLayer == nullptr) return vector<int>();

	vector<int> vecFeatureIndex;

	RECTD drtFeature;

	CAlignMask *pAlignMask = new CAlignMask;

	CTypedPtrArray<CObArray, CFeature*> *pMaskArr;
	//if (pAlignMask == nullptr) continue;


	int nArrayCount = static_cast<int>(pLayer->m_arrAlign.GetCount());

	for (int nAlign = 0; nAlign < nArrayCount; nAlign++)
	{
		pAlignMask = pLayer->m_arrAlign.GetAt(nAlign);
		pMaskArr = &pAlignMask->m_arrFeature;
		CFeature *pFeature = pMaskArr->GetAt(0);		

		drtFeature = pFeature->m_MinMaxRect;

		if (drtFeature.IsIntersection(drtRect_mm) == FALSE)
		{
			continue;
		}

		vecFeatureIndex.push_back(nAlign);
	}

	//}

	return vecFeatureIndex;
}

UINT32			CDrawMinimap::_SetCopyBuffer(IN const CLayer* pLayer, IN const int &nLayer, IN const vector<int> &vecFeatureIndex, IN const vector<int> &vecAlignIndex, IN int nMode)
{//입력 받은 Feature Index에 해당하는 Feature를 버퍼에 복사
	
	if (pLayer == nullptr) return RESULT_BAD;
	if (nLayer == -1) return RESULT_BAD;
	int nCount = static_cast<int>(vecFeatureIndex.size());
	if (nCount <= 0) return RESULT_BAD;


	_ClearCopyBuffer(nMode);

	//복사 소스 위치 정보 저장
	m_stCopyInfo.nStep = m_stSelectInfo.nStep;
	m_stCopyInfo.drtRect = m_stSelectInfo.drtRect;

	//임시 저장 시작
	int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());
	for (int i = 0; i < nCount; i++)
	{
		int nfeno = vecFeatureIndex[i];
		if ( nfeno < 0) continue;
		if ( nfeno >= nFeatureCount ) continue;

		CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nfeno);
		switch (pFeature->m_eType)
		{
		case FeatureType::Z:
		{
			CFeatureZ *pFeatureZ = (CFeatureZ*)pFeature;
			CFeatureData *pTempFeature = new CFeatureData();
			pTempFeature->SetTp(nLayer, nfeno, pFeatureZ, 0);
			m_vecCopyTemp_SR.push_back(pTempFeature);
		}
		break;
		case FeatureType::L:
		{
			CFeatureL *pFeatureL = (CFeatureL*)pFeature;
			CFeatureData *pTempFeature = new CFeatureData();
			pTempFeature->SetLine(nLayer, nfeno, pFeatureL, 0);
			m_vecCopyTemp.push_back(pTempFeature);
		}
			break;
		case FeatureType::P:
		{
			CFeatureP *pFeatureP = (CFeatureP*)pFeature;
			CFeatureData *pTempFeature = new CFeatureData();
			pTempFeature->SetPad(nLayer, nfeno, pFeatureP, 0);
			m_vecCopyTemp.push_back(pTempFeature);
		}
			break;
		case FeatureType::A:
		{
			CFeatureA *pFeatureA = (CFeatureA*)pFeature;
			CFeatureData *pTempFeature = new CFeatureData();
			pTempFeature->SetArc(nLayer, nfeno, pFeatureA, 0);
			m_vecCopyTemp.push_back(pTempFeature);
		}
			break;
		case FeatureType::S:
		{
			CFeatureS *pFeatureS = (CFeatureS*)pFeature;

			//전제 oboe정보를 복사하기 위함.
			//ObOe의 Highlight 값을 백업하고
			//Oboe의 Highlight를 강제로 셋한다.
			vector<bool> vecPreHighlight;
			int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
			vecPreHighlight.resize(nObOeCount);
			for (int nObOe = 0; nObOe < nObOeCount; nObOe++)
			{
				CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nObOe);
				if (pObOe == nullptr ) continue;

				vecPreHighlight[nObOe] = pObOe->m_bHighlight;

				pObOe->m_bHighlight = true;
			}
			
			CFeatureData *pTempFeature = new CFeatureData();
			pTempFeature->SetSurface(nLayer, nfeno, pFeatureS, 0);
			m_vecCopyTemp.push_back(pTempFeature);

			//강제로 true로 만든 값을 되돌려준다.
			for (int nObOe = 0; nObOe < nObOeCount; nObOe++)
			{
				CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nObOe);
				if (pObOe == nullptr) continue;

				pObOe->m_bHighlight = vecPreHighlight[nObOe];
			}
		}
			break;
		}
	}


	int nAlignCount = static_cast<int>(vecAlignIndex.size());
	if (nAlignCount <= 0) return RESULT_BAD;
	
	for (int i = 0; i < nAlignCount; i++)
	{
		int nfeno = vecAlignIndex[i];
		if (nfeno < 0) continue;
		//if (nfeno >= nAlignCount) continue;


		CTypedPtrArray<CObArray, CFeature*> *pMaskArr;

		CAlignMask *pAlignMask = pLayer->m_arrAlign.GetAt(nfeno);
		pMaskArr = &pAlignMask->m_arrFeature;

		if (pMaskArr == nullptr) continue;;

		//CFeature *pFeature = pMaskArr->GetAt(nfeno);

		//int nArrayCount = pLayer->m_arrAlign.GetCount();

		CFeature *pFeature = pMaskArr->GetAt(0);

		switch (pFeature->m_eType)
		{
		case FeatureType::P:
		{
			CFeatureP *pFeatureP = (CFeatureP*)pFeature;
			CFeatureData *pTempFeature = new CFeatureData();
			pTempFeature->SetPad(nLayer, nfeno, pFeatureP, 0);
			m_vecCopyAlignTemp.push_back(pTempFeature);
			break;
		}
		default:
		{
			break;
		}
		}			
		
	}


	return RESULT_GOOD;
}

UINT32			CDrawMinimap::_SetPaste_Feature(IN CStep* pStep, IN const vector<CFeatureData*> &vecFeatureData, IN const CPointD &dptPoint)
{
	if (pStep == nullptr) return RESULT_BAD;
	
	int nCopyCount = static_cast<int>(vecFeatureData.size());

	//신규로 FeaureData를 생성하면서 Move Point한다.
	vector<CFeatureData*> vecFeatureData_Moved;
	for (int i = 0; i < nCopyCount; i++)
	{
		int nLayer = vecFeatureData[i]->m_nLayer;
		int nFeatureIndex = vecFeatureData[i]->m_nFeatureIndex;
		CFeature *pFeature = vecFeatureData[i]->m_pFeature;

		switch (pFeature->m_eType)
		{
		case FeatureType::Z:
		{
			CFeatureZ *pFeatureZ = (CFeatureZ*)pFeature;
			CFeatureData *pTempFeature = new CFeatureData();

			pTempFeature->SetTp(nLayer, nFeatureIndex, pFeatureZ, 0);
			vecFeatureData_Moved.push_back(pTempFeature);

			pFeatureZ = (CFeatureZ*)pTempFeature->m_pFeature;
			//Move
			pFeatureZ->m_dX += dptPoint.x;
			pFeatureZ->m_dY += dptPoint.y;
			
		}
		break;
		case FeatureType::L:
		{
			CFeatureL *pFeatureL = (CFeatureL*)pFeature;
			CFeatureData *pTempFeature = new CFeatureData();
			
			pTempFeature->SetLine(nLayer, nFeatureIndex, pFeatureL, 0);
			vecFeatureData_Moved.push_back(pTempFeature);

			pFeatureL = (CFeatureL*)pTempFeature->m_pFeature;
			//Move
			pFeatureL->m_dXs += dptPoint.x;
			pFeatureL->m_dYs += dptPoint.y;
			pFeatureL->m_dXe += dptPoint.x;
			pFeatureL->m_dYe += dptPoint.y;
		}
		break;
		case FeatureType::P:
		{
			CFeatureP *pFeatureP = (CFeatureP*)pFeature;
			CFeatureData *pTempFeature = new CFeatureData();

			pTempFeature->SetPad(nLayer, nFeatureIndex, pFeatureP, 0);
			vecFeatureData_Moved.push_back(pTempFeature);

			pFeatureP = (CFeatureP*)pTempFeature->m_pFeature;
			//Move
			pFeatureP->m_dX += dptPoint.x;
			pFeatureP->m_dY += dptPoint.y;
		}
		break;
		case FeatureType::A:
		{
			CFeatureA *pFeatureA = (CFeatureA*)pFeature;
			CFeatureData *pTempFeature = new CFeatureData();

			pTempFeature->SetArc(nLayer, nFeatureIndex, pFeatureA, 0);
			vecFeatureData_Moved.push_back(pTempFeature);

			pFeatureA = (CFeatureA*)pTempFeature->m_pFeature;
			//Move
			pFeatureA->m_dXs += dptPoint.x;
			pFeatureA->m_dYs += dptPoint.y;
			pFeatureA->m_dXe += dptPoint.x;
			pFeatureA->m_dYe += dptPoint.y;
			pFeatureA->m_dXc += dptPoint.x;
			pFeatureA->m_dYc += dptPoint.y;
		}
		break;
		case FeatureType::S:
		{
			CFeatureS *pFeatureS = (CFeatureS*)pFeature;

			//전제 oboe정보를 복사하기 위함.
			//ObOe의 Highlight 값을 백업하고
			//Oboe의 Highlight를 강제로 셋한다.
			vector<bool> vecPreHighlight;
			int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
			vecPreHighlight.resize(nObOeCount);
			for (int nObOe = 0; nObOe < nObOeCount; nObOe++)
			{
				CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nObOe);
				if (pObOe == nullptr) continue;

				vecPreHighlight[nObOe] = pObOe->m_bHighlight;

				pObOe->m_bHighlight = true;
			}
			//
			
			CFeatureData *pTempFeature = new CFeatureData();
			pTempFeature->SetSurface(nLayer, nFeatureIndex, pFeatureS, 0);
			vecFeatureData_Moved.push_back(pTempFeature);

			//강제로 true로 만든 값을 되돌려준다.
			for (int nObOe = 0; nObOe < nObOeCount; nObOe++)
			{
				CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nObOe);
				if (pObOe == nullptr) continue;

				pObOe->m_bHighlight = vecPreHighlight[nObOe];
			}

			pFeatureS = (CFeatureS*)pTempFeature->m_pFeature;
			//Move
			for (int nObOe = 0; nObOe < nObOeCount; nObOe++)
			{
				CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nObOe);
				if (pObOe == nullptr) continue;

				pObOe->m_dXbs += dptPoint.x;
				pObOe->m_dYbs += dptPoint.y;

				int nOsOcCount = static_cast<int>(pObOe->m_arrOsOc.GetCount());
				for (int nOsOc = 0; nOsOc < nOsOcCount; nOsOc++)
				{
					COsOc *pOsOc = pObOe->m_arrOsOc.GetAt(nOsOc);
					if (pOsOc == nullptr) continue;

					pOsOc->m_dX += dptPoint.x;
					pOsOc->m_dY += dptPoint.y;

					if (pOsOc->m_bOsOc == false)
					{//Oc
						COc *pOc = (COc *)pOsOc;
						pOc->m_dXc += dptPoint.x;
						pOc->m_dYc += dptPoint.y;
					}
				}
			}
			//

			
		}
		break;
		default:
			break;
		}
	}

	CEditFunction::SetFeature_Add(vecFeatureData_Moved, pStep);

	return RESULT_GOOD;

}UINT32			CDrawMinimap::_SetPaste_AlignFeature(IN CStep* pStep, IN const vector<CFeatureData*> &vecFeatureData, IN const CPointD &dptPoint)
{
	if (pStep == nullptr) return RESULT_BAD;
	
	int nCopyCount = static_cast<int>(vecFeatureData.size());

	//신규로 FeaureData를 생성하면서 Move Point한다.
	vector<CFeatureData*> vecFeatureData_Moved;
	for (int i = 0; i < nCopyCount; i++)
	{
		int nLayer = vecFeatureData[i]->m_nLayer;
		int nFeatureIndex = vecFeatureData[i]->m_nFeatureIndex;
		CFeature *pFeature = vecFeatureData[i]->m_pFeature;

		switch (pFeature->m_eType)
		{
		case FeatureType::P:
		{
			CFeatureP *pFeatureP = (CFeatureP*)pFeature;
			CFeatureData *pTempFeature = new CFeatureData();

			pTempFeature->SetPad(nLayer, nFeatureIndex, pFeatureP, 0);
			vecFeatureData_Moved.push_back(pTempFeature);

			pFeatureP = (CFeatureP*)pTempFeature->m_pFeature;
			//Move
			pFeatureP->m_dX += dptPoint.x;
			pFeatureP->m_dY += dptPoint.y;
		}
		break;		
		default:
			break;
		}
	}

	CEditFunction::SetFeature_AlignAdd(vecFeatureData_Moved, pStep);

	return RESULT_GOOD;
}

UINT32			CDrawMinimap::_SetDelete_Feature(CLayer *pLayer, IN RECTD drtSelect)
{

	if (pLayer == nullptr) return RESULT_BAD;

	drtSelect.NormalizeRectD();

	if (m_nMode == enumMode_Measure || m_nMode == enumMode_Max)
	{
		vector<int>	vecFeatureAlignIndex = _GetInsideAlignIndex(pLayer, drtSelect);
		//if (vecFeatureAlignIndex.size() <= 0) return RESULT_BAD;

		//Delete
		int nAlignCount = static_cast<int>(vecFeatureAlignIndex.size());
		//if (nAlignCount <= 0) return RESULT_BAD;

		//임시 저장 시작
		int nFeatureAlignCount = static_cast<int>(pLayer->m_arrAlign.GetCount());
		for (int i = nAlignCount - 1; i >= 0; i--)
		{
			int nfeno = vecFeatureAlignIndex[i];
			if (nfeno < 0) continue;
			if (nfeno >= nFeatureAlignCount) continue;

			delete pLayer->m_arrAlign[nfeno];
			pLayer->m_arrAlign.RemoveAt(nfeno);
		}
	}


	vector<int>	vecFeatureIndex = _GetInsideFeatureIndex(pLayer, drtSelect);
	//if (vecFeatureIndex.size() <= 0) return RESULT_BAD;

	//Delete
	int nCount = static_cast<int>(vecFeatureIndex.size());
	//if (nCount <= 0) return RESULT_BAD;

	//임시 저장 시작
	int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());
	for (int i = nCount - 1; i >= 0; i--)
	{
		int nfeno = vecFeatureIndex[i];
		if (nfeno < 0) continue;
		if (nfeno >= nFeatureCount) continue;
		
		delete pLayer->m_FeatureFile.m_arrFeature[nfeno];
		pLayer->m_FeatureFile.m_arrFeature.RemoveAt(nfeno);
	}

	



	return RESULT_GOOD;
}



UINT32			CDrawMinimap::_SetInspect_Unit(CLayer *pLayer, IN RECTD drtSelect)
{
	if (pLayer == nullptr) return RESULT_BAD;

	drtSelect.NormalizeRectD();
	
	for (auto itStrip : m_pPanelInfo->vcStripInfo)
	{		
		RECTD tmpStripRect;
		tmpStripRect = itStrip->rcRect_In_Panel;
		tmpStripRect.NormalizeRectD();
		if (tmpStripRect.IsIntersection(drtSelect) == FALSE)
			continue;

		for (auto itUnit : itStrip->vcUnitInfo)
		{
			RECTD tmpUnitRect;

			tmpUnitRect = itUnit->rcRect_In_Panel;			
			tmpUnitRect.NormalizeRectD();
			if (tmpUnitRect.IsIntersection(drtSelect) == FALSE)
				continue;

			itUnit->bInspect = TRUE;
			
			if (itUnit->bInspect)
			{
				itStrip->bInspect = TRUE;
				AddMeasureUnit(itStrip->iStripIdx, itUnit->iUnitIdx_Ori, itUnit);			
				
				int nStepCnt = static_cast<int>(m_vecvecRect.size());

				for (int nStep = 0; nStep < nStepCnt; nStep++)
				{
					int nUnitCnt = static_cast<int>(m_vecvecRect[nStep].size());
					for (int nUnit = 0; nUnit < nUnitCnt; nUnit++)
					{
						if (tmpUnitRect.IsPtInRectD(m_vecvecRect[nStep][nUnit].drtRect.CenterX(), m_vecvecRect[nStep][nUnit].drtRect.CenterY()) == FALSE)
							continue;

						m_vecvecRect[nStep][nUnit].bInspect = TRUE;
					}
				}
			}
			
		}		
	}	

	CDirect_Base::Render();
	

	return RESULT_GOOD;
}

BOOL CDrawMinimap::AddMeasureUnit(INT32 iStripIdx, INT32 iUnitIdx, UnitInfo_t *pUnit)
{
	if (pUnit == nullptr) return FALSE;

	StripInfo_t *pStrip = m_pPanelInfo->vcStripInfo[iStripIdx];
	if (pStrip == nullptr)
		return FALSE;

	//	iUnitIdx += (iStripIdx * pStrip->iRowUnitNumInStrip * pStrip->iColUnitNumInStrip);

	bool bExist = false;
	for (auto it : m_pPanelInfo->stMeasureInfo.vcUnits)
	{
		if (it->iIndex != iUnitIdx)
			continue;

		bExist = true;
		break;
	}	

	if (!bExist)
	{
		MeasureUnit_t* pMeasureUnit = new MeasureUnit_t;

		pMeasureUnit->stStepRpt = pUnit->stStepRpt;
		pMeasureUnit->iStripIdx = pUnit->iStripIdx;
		pMeasureUnit->iIndex = pUnit->iUnitIdx_Ori;
		pMeasureUnit->iType = pUnit->iType;
		pMeasureUnit->UnitRect = pUnit->rcProfile;
		pMeasureUnit->UnitRect_In_Panel = pUnit->rcRect_In_Panel;

		if (m_nMode == enumMode_Measure)
		{
			pMeasureUnit->eMapMode = eMinimapMode::Measure;
		}
		else if (m_nMode == enumMode_Thickness)
		{
			pMeasureUnit->eMapMode = eMinimapMode::Thickness;
		}

		m_pPanelInfo->stMeasureInfo.vcUnits.emplace_back(pMeasureUnit);

	}
	return TRUE;
}