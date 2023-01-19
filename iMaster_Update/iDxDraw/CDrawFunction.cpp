#include "pch.h"
#include "CDrawFunction.h"

#include "CDrawMakeGeo.h"
#include "CDrawMakePoint.h"

CDrawFunction::CDrawFunction()
{
	m_pMakeGeo = new CDrawMakeGeo();
}

CDrawFunction::~CDrawFunction()
{
	if (m_pLayerColor_Hole != nullptr)
	{
		m_pLayerColor_Hole->Release();
		m_pLayerColor_Hole = nullptr;
	}

	if (m_pLayerColor != nullptr)
	{
		m_pLayerColor->Release();
		m_pLayerColor = nullptr;
	}

	if (m_pLayerColor_Mask != nullptr)
	{
		m_pLayerColor_Mask->Release();
		m_pLayerColor_Mask = nullptr;
	}

	if (m_pLayerColor_Mask_Select != nullptr)
	{
		m_pLayerColor_Mask_Select->Release();
		m_pLayerColor_Mask_Select = nullptr;
	}

	if (m_pLayerColor_Mask_PreSelect != nullptr)
	{
		m_pLayerColor_Mask_PreSelect->Release();
		m_pLayerColor_Mask_PreSelect = nullptr;
	}

	if (m_pLayerColor_Mask_Transparent != nullptr)
	{
		m_pLayerColor_Mask_Transparent->Release();
		m_pLayerColor_Mask_Transparent = nullptr;
	}

	if (m_pStrokeStyle_r != nullptr)
	{
		m_pStrokeStyle_r->Release();
		m_pStrokeStyle_r = nullptr;
	}

	if (m_pStrokeStyle_s != nullptr)
	{
		m_pStrokeStyle_s->Release();
		m_pStrokeStyle_s = nullptr;
	}

	if (m_pStrokeStyle_Dash != nullptr)
	{
		m_pStrokeStyle_Dash->Release();
		m_pStrokeStyle_Dash = nullptr;
	}

	if (m_pStrokeStyle_Dash_Dot != nullptr)
	{
		m_pStrokeStyle_Dash_Dot->Release();
		m_pStrokeStyle_Dash_Dot = nullptr;
	}

	if (m_pMakeGeo != nullptr)
	{
		delete m_pMakeGeo;
		m_pMakeGeo = nullptr;
	}
	
	_DeleteRenderItem();
	
}

UINT32 CDrawFunction::_SetGeometry(vector<GEODRAW> &vecGeoData, ID2D1PathGeometry **pGeoObject, BOOL bOpen)
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

CLayer *CDrawFunction::_GetLayerInfo(UserLayerSet_t *pUserLayerSetInfo, CStep* pStep, CString strDefine)
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

	int nLayerIndex = 0;
	return _GetLayerPrt(pStep->m_arrLayer, strLayer, nLayerIndex);
}

CLayer *CDrawFunction::_GetLayerPrt(IN CTypedPtrArray <CObArray, CLayer*> &arrLayer, IN CString strLayer, OUT int &nLayerIndex)
{
	//선택된 레이어와 같은 이름의 layer를 찾는다.
	int nLayerCount = static_cast<int>(arrLayer.GetCount());
	for (int j = 0; j < nLayerCount; j++)
	{
		CLayer *pLayerTmp = arrLayer.GetAt(j);
		if (pLayerTmp == nullptr) return pLayerTmp;

		CString strLayerName_Ori = pLayerTmp->m_strLayerName;
		CString strLayerTmp = strLayer;

		if (strLayerTmp.MakeLower().Compare(strLayerName_Ori.MakeLower()) == 0)
		{
			nLayerIndex = j;
			return pLayerTmp;
		}
	}

	return nullptr;
}

CStep *CDrawFunction::_GetStepPrt(IN CTypedPtrArray <CObArray, CStep*> &arrStep, IN CString strStepName, OUT int &nStepIndex)
{
	//선택된 레이어와 같은 이름의 layer를 찾는다.
	int nLayerCount = static_cast<int>(arrStep.GetCount());
	for (int j = 0; j < nLayerCount; j++)
	{
		CStep *pStepTmp = arrStep.GetAt(j);
		CString strStepName_Ori = pStepTmp->m_strStepName;
		if (pStepTmp == nullptr) return pStepTmp;

		if (strStepName_Ori.MakeLower() == strStepName.MakeLower())
		{
			nStepIndex = j;
			return pStepTmp;
		}
	}

	return nullptr;
}

D2D1_RECT_U CDrawFunction::NormalizeRect(IN const D2D1_RECT_U& rtRect)
{
	D2D1_RECT_U rtNormalize = rtRect;

	if (rtRect.left > rtRect.right)
	{
		rtNormalize.left = rtRect.right;
		rtNormalize.right = rtRect.left;
	}

	if (rtRect.top > rtRect.bottom)
	{
		rtNormalize.top = rtRect.bottom;
		rtNormalize.bottom = rtRect.top;
	}

	return rtNormalize;
}

D2D1_RECT_F CDrawFunction::NormalizeRect(IN const D2D1_RECT_F& frtRect)
{
	D2D1_RECT_F frtNormalize = frtRect;

	if (frtRect.left > frtRect.right)
	{
		frtNormalize.left = frtRect.right;
		frtNormalize.right = frtRect.left;
	}

	if (frtRect.top > frtRect.bottom)
	{
		frtNormalize.top = frtRect.bottom;
		frtNormalize.bottom = frtRect.top;
	}

	return frtNormalize;
}

D2D1_POINT_2F CDrawFunction::GetRectCenter(IN const D2D1_RECT_F& frtRect)
{
	D2D1_POINT_2F fptCenter;
	D2D1_RECT_F frtNormal = NormalizeRect(frtRect);

	fptCenter.x = (frtNormal.left + frtNormal.right)/2.f;
	fptCenter.y = (frtNormal.top + frtNormal.bottom)/2.f;

	return fptCenter;


}

D2D1_POINT_2F CDrawFunction::DetansformPoint(IN const D2D1_POINT_2F& fptOrg, IN const D2D1_MATRIX_3X2_F& matTrans, IN const UserSetInfo_t *pUserSetInfo)
{

	//x = point.x * _11 + point.y * _21 + _31,
	//y = point.x * _12 + point.y * _22 + _32
	float fDiv = matTrans.m11 * matTrans.m22 - matTrans.m12 * matTrans.m21;
	float m11 =  matTrans.m22 / fDiv;
	float m12 = -1*matTrans.m12 / fDiv;
	float m21 = -1*matTrans.m21 / fDiv;
	float m22 =  matTrans.m11 / fDiv;
	D2D1::Matrix3x2F matTrans_Inv = D2D1::Matrix3x2F(m11, m12, m21, m22, 0, 0);

	D2D1_POINT_2F fptNew;

	fptNew.x = (fptOrg.x - matTrans.dx) * matTrans_Inv.m11 + (fptOrg.y - matTrans.dy) * matTrans_Inv.m12;
	fptNew.y = (fptOrg.x - matTrans.dx) * matTrans_Inv.m21 + (fptOrg.y - matTrans.dy) * matTrans_Inv.m22;

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
					fptNew.x *= -1.0f;
					fptNew.y *= -1.0f;
				}
			}
			else if (pUserSetInfo->mirrorDir == eDirectionType::eVertical)
			{
				if (dAngle == 90.0 || dAngle == 270.0)
				{
					if (dAngle == 90.0 || dAngle == 270.0)
					{
						fptNew.x *= -1.0f;
						fptNew.y *= -1.0f;
					}
				}
			}
		}

	}
	
	return fptNew;
}

D2D1_RECT_F CDrawFunction::DetansformRect(IN const D2D1_RECT_F& fptOrg, IN const D2D1_MATRIX_3X2_F& matTrans, IN const UserSetInfo_t *pUserSetInfo, IN const BOOL &bNormalize)
{
	D2D1_RECT_F frtRect;

	D2D1_POINT_2F fptLT = D2D1::Point2F(fptOrg.left, fptOrg.top);
	D2D1_POINT_2F fptRB = D2D1::Point2F(fptOrg.right, fptOrg.bottom);

	fptLT = DetansformPoint(fptLT, matTrans, pUserSetInfo);
	fptRB = DetansformPoint(fptRB, matTrans, pUserSetInfo);

	frtRect = D2D1::RectF(fptLT.x, fptLT.y, fptRB.x, fptRB.y);

	if (bNormalize == TRUE)
	{
		frtRect = CDrawFunction::NormalizeRect(frtRect);
	}

	return frtRect;
}

CString		CDrawFunction::GetSymbolName(IN const SymbolName &eSymbolName, vector<double> &vecParam_mm)
{
	CString strSymbolName = _T("");

	int nParaSize = static_cast<int>(vecParam_mm.size());
	if (/*eSymbolName != SymbolName::null &&*/ nParaSize == 0) return strSymbolName;

	vector<double> vecMil;
	vecMil.resize(nParaSize);
	for (int i = 0; i < nParaSize; i++)
	{
		vecMil[i] = CJobFile::MMToMil(vecParam_mm[i]);
	}

	vector<CString> vecStrMil;
	vecStrMil.resize(nParaSize);
	for (int i = 0; i < nParaSize; i++)
	{
		vecStrMil[i].Format(_T("%.3lf"), vecMil[i]);

		//소수점 3째자리에 대한 검사
		//뒤부터 0이 있으면 제거한다.
		for (int j = 0; j < 3; j++)
		{
			if (vecStrMil[i].Mid(vecStrMil[i].GetLength() - 1, 1) == _T("0"))
			{
				vecStrMil[i] = vecStrMil[i].Left(vecStrMil[i].GetLength() - 1);
			}
			else
			{
				break;
			}
		}
	}

	switch (eSymbolName)
	{
	case SymbolName::r :
		strSymbolName.Format(_T("%s%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)], vecStrMil[0]);
		break;
	case SymbolName::s:
		strSymbolName.Format(_T("%s%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)], vecStrMil[0]);
		break;
	case SymbolName::rect:
		strSymbolName.Format(_T("%s%sx%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0], vecStrMil[1]);
		break;
	case SymbolName::rectxr:
		strSymbolName.Format(_T("%s%sx%sxr%sx%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0], vecStrMil[1], vecStrMil[2], vecStrMil[3]);
		break;
	case SymbolName::rectxc:
		strSymbolName.Format(_T("%s%sx%sxc%sx%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0], vecStrMil[1], vecStrMil[2], vecStrMil[3]);
		break;

	case SymbolName::oval:
		strSymbolName.Format(_T("%s%sx%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0], vecStrMil[1]);
		break;
	case SymbolName::di:
		strSymbolName.Format(_T("%s%sx%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0], vecStrMil[1]);
		break;
	case SymbolName::octagon:
		strSymbolName.Format(_T("%s%sx%sx%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0], vecStrMil[1], vecStrMil[2]);
		break;
	case SymbolName::donut_r:
		strSymbolName.Format(_T("%s%sx%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0], vecStrMil[1]);
		break;
	case SymbolName::donut_s:
		strSymbolName.Format(_T("%s%sx%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0], vecStrMil[1]);
		break;

	case SymbolName::hex_l:
		strSymbolName.Format(_T("%s%sx%sx%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0], vecStrMil[1], vecStrMil[2]);
		break;
	case SymbolName::hex_s:
		strSymbolName.Format(_T("%s%sx%sx%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0], vecStrMil[1], vecStrMil[2]);
		break;
	case SymbolName::bfr:
		strSymbolName.Format(_T("%s%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0]);
		break;
	case SymbolName::bfs:
		strSymbolName.Format(_T("%s%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0]);
		break;
	case SymbolName::tri:
		strSymbolName.Format(_T("%s%sx%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0], vecStrMil[1]);
		break;

	case SymbolName::oval_h:
		strSymbolName.Format(_T("%s%sx%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0], vecStrMil[1]);
		break;
	case SymbolName::thr:
		strSymbolName.Format(_T("%s%sx%sx%sx%sx%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0], vecStrMil[1], vecStrMil[2], vecStrMil[3], vecStrMil[4]);
		break;
	case SymbolName::ths:
		strSymbolName.Format(_T("%s%sx%sx%sx%sx%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0], vecStrMil[1], vecStrMil[2], vecStrMil[3], vecStrMil[4]);
		break;
	case SymbolName::s_ths:
		strSymbolName.Format(_T("%s%sx%sx%sx%sx%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0], vecStrMil[1], vecStrMil[2], vecStrMil[3], vecStrMil[4]);
		break;
	case SymbolName::s_tho:
		strSymbolName.Format(_T("%s%sx%sx%sx%sx%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0], vecStrMil[1], vecStrMil[2], vecStrMil[3], vecStrMil[4]);
		break;

	case SymbolName::sr_ths:
		strSymbolName.Format(_T("%s%sx%sx%sx%sx%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0], vecStrMil[1], vecStrMil[2], vecStrMil[3], vecStrMil[4]);
		break;
	case SymbolName::rc_ths:
		strSymbolName.Format(_T("%s%sx%sx%sx%sx%sx%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0], vecStrMil[1], vecStrMil[2], vecStrMil[3], vecStrMil[4], vecStrMil[5]);
		break;
	case SymbolName::rc_tho:
		strSymbolName.Format(_T("%s%sx%sx%sx%sx%sx%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0], vecStrMil[1], vecStrMil[2], vecStrMil[3], vecStrMil[4], vecStrMil[5]);
		break;
	case SymbolName::el:
		strSymbolName.Format(_T("%s%sx%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0], vecStrMil[1]);
		break;
	case SymbolName::moire:
		strSymbolName.Format(_T("%s%sx%sx%sx%sx%sx%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0], vecStrMil[1], vecStrMil[2], vecStrMil[3], vecStrMil[4], vecStrMil[5]);
		break;

	case SymbolName::hole:
		strSymbolName.Format(_T("%s%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)],
			vecStrMil[0]);
		break;

	case SymbolName::null:
		strSymbolName.Format(_T("%s"), DefSymbol::pName[static_cast<UINT8>(eSymbolName)], vecStrMil[0]);
		break;
	}


	return strSymbolName;
}

SymbolName   CDrawFunction::GetSymbolType(IN const CString &strSymbolName)
{
	SymbolName eSymbolType = SymbolName::SymbolNameNone;

	for (int i = 0; i < static_cast<int>(SymbolName::SymbolNameNone); i++)
	{
		if (DefSymbol::pName[i] != _T(""))
		{
			int nLength = DefSymbol::pName[i].GetLength();
			CString strCompSymbolName = strSymbolName.Mid(0, nLength);

			if (strCompSymbolName.Compare(DefSymbol::pName[i]) == 0)
			{
				eSymbolType = static_cast<SymbolName>(i);
				break;
			}
		}
	}

	return eSymbolType;
}

double		CDrawFunction::GetSymbolSize(IN const CString &strSymbolName)
{
	SymbolName eSymbolType = GetSymbolType(strSymbolName);
	if (eSymbolType == SymbolName::SymbolNameNone || eSymbolType == SymbolName::userdefined )
		//||eSymbolType == SymbolName::null)
	{
		return 0.0;
	}

	int nSymbolType = static_cast<int>(eSymbolType);
	int nLength = DefSymbol::pName[nSymbolType].GetLength();

	CString strParam = strSymbolName.Mid(nLength);
	strParam = strParam.MakeLower();
	CString strMasterSize = strParam.Left(strParam.GetLength() - 1 - strParam.Find('x'));

	return _ttof(strMasterSize);
}

void CDrawFunction::GetOrient(IN const double &dAngle, IN const bool &bMirror, OUT Orient &ori)
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

void CDrawFunction::GetAngleMirror(IN const Orient &ori, OUT double &dAngle, OUT bool &bMirror)
{
	dAngle = 0;
	bMirror = false;

	switch (ori)
	{
	case Orient::NoMir90Deg:
		dAngle = 90.;
		break;
	case Orient::NoMir180Deg:
		dAngle = 180.;
		break;
	case Orient::NoMir270Deg:
		dAngle = 270.;
		break;
	case Orient::Mir0Deg:
		bMirror = true;
		break;
	case Orient::Mir90Deg:
		dAngle = 90.;
		bMirror = true;
		break;
	case Orient::Mir180Deg:
		dAngle = 180.;
		bMirror = true;
		break;
	case Orient::Mir270Deg:
		dAngle = 270.;
		bMirror = true;
		break;
	}
}

CFeatureP *CDrawFunction::MakeRectFeature(IN RECTD &drtRect)
{
	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	double dCenterX, dCenterY;
	double dWidth, dHeight;

	dCenterX = drtRect.CenterX();
	dCenterY = drtRect.CenterY();
	dWidth = drtRect.width();
	dHeight = drtRect.height();

	pFeatureP->m_dX = dCenterX;
	pFeatureP->m_dY = dCenterY;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = true;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::rect;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;
	pSymbol->m_vcParams.resize(2);
	pSymbol->m_vcParams[0] = dWidth;
	pSymbol->m_vcParams[1] = dHeight;

	CString strSymbol = CDrawFunction::GetSymbolName(pSymbol->m_eSymbolName, pSymbol->m_vcParams);
	pSymbol->m_strUserSymbol = strSymbol;

	pFeatureP->m_pSymbol = pSymbol;

	pFeatureP->m_pSymbol->SetMinMaxRect();
	pFeatureP->SetMinMaxRect();

	return pFeatureP;
}






BOOL CDrawFunction::CheckHighlight_TpFeature(IN D2D_POINT_2F fptPoint, IN CFeatureZ* pFeatureZ, IN CSymbol *pSymbol,
	IN const D2D1_MATRIX_3X2_F& matViewScale, ID2D1PathGeometry *pGeo)
{
	if (pFeatureZ == nullptr) return FALSE;
	if (pFeatureZ->m_pGeoObject == nullptr) return FALSE;

	float fPenWidth = 1.0;
	ID2D1StrokeStyle *pStrokeStyle = nullptr;
	//UINT32 nRet = GetLineDrawInfo(pFeatureZ->m_pSymbol, fPenWidth, &pStrokeStyle);

	UINT32 nRet = GetPadDrawInfo(pFeatureZ->m_pSymbol, fPenWidth, &pStrokeStyle, nullptr);

	BOOL bHit = FALSE;
	if (nRet == RESULT_GOOD)
	{//Stroke
		if (pGeo == nullptr)
		{
			pFeatureZ->m_pGeoObject->StrokeContainsPoint(fptPoint, fPenWidth, pStrokeStyle, matViewScale, &bHit);
		}
		else
		{
			pGeo->StrokeContainsPoint(fptPoint, fPenWidth, pStrokeStyle, matViewScale, &bHit);
		}
	}
	else
	{//fill
		if (pGeo == nullptr)
		{
			pFeatureZ->m_pGeoObject->FillContainsPoint(fptPoint, matViewScale, &bHit);
		}
		else
		{
			pGeo->FillContainsPoint(fptPoint, matViewScale, &bHit);
		}
	}
	return bHit;
}






BOOL CDrawFunction::CheckHighlight_LineFeature(IN D2D_POINT_2F fptPoint, IN CFeatureL* pFeatureL, IN CSymbol *pSymbol,
	IN const D2D1_MATRIX_3X2_F& matViewScale, ID2D1PathGeometry *pGeo)
{
	if (pFeatureL == nullptr) return FALSE;
	if (pFeatureL->m_pGeoObject == nullptr) return FALSE;

	float fPenWidth = 1.0;
	ID2D1StrokeStyle *pStrokeStyle = nullptr;
	UINT32 nRet = GetLineDrawInfo(pFeatureL->m_pSymbol, fPenWidth, &pStrokeStyle);

	BOOL bHit = FALSE;
	if (nRet == RESULT_GOOD)
	{//Line은 Stroke만 존재함.
		if (pGeo == nullptr)
		{
			pFeatureL->m_pGeoObject->StrokeContainsPoint(fptPoint, fPenWidth, pStrokeStyle, matViewScale, &bHit);
		}
		else
		{
			pGeo->StrokeContainsPoint(fptPoint, fPenWidth, pStrokeStyle, matViewScale, &bHit);
		}
	}

	return bHit;
}

BOOL CDrawFunction::CheckHighlight_PadFeature(IN D2D_POINT_2F fptPoint, IN CFeatureP* pFeatureP, IN CSymbol *pSymbol, IN const D2D1_MATRIX_3X2_F& matViewScale, ID2D1PathGeometry *pGeo)
{
	if (pFeatureP == nullptr) return FALSE;
	if (pFeatureP->m_pGeoObject == nullptr) return FALSE;

	float fPenWidth = 1.0;
	ID2D1StrokeStyle *pStrokeStyle = nullptr;
	UINT32 nRet = GetPadDrawInfo(pFeatureP->m_pSymbol, fPenWidth, &pStrokeStyle, nullptr);

	BOOL bHit;
	/*BOOL bHit = TRUE;*/
	if (nRet == RESULT_GOOD)
	{//Stroke
		if (pGeo == nullptr)
		{
			pFeatureP->m_pGeoObject->StrokeContainsPoint(fptPoint, fPenWidth, pStrokeStyle, matViewScale, &bHit);
		}
		else
		{
			pGeo->StrokeContainsPoint(fptPoint, fPenWidth, pStrokeStyle, matViewScale, &bHit);
		}
	}
	else
	{//fill
		if (pGeo == nullptr)
		{
			pFeatureP->m_pGeoObject->FillContainsPoint(fptPoint, matViewScale, &bHit);
		}
		else
		{
			pGeo->FillContainsPoint(fptPoint, matViewScale, &bHit);
		}
	}

	return bHit;
}

BOOL CDrawFunction::CheckHighlight_ArcFeature(IN D2D_POINT_2F fptPoint, IN CFeatureA* pFeatureA, IN CSymbol *pSymbol, IN const D2D1_MATRIX_3X2_F& matViewScale, ID2D1PathGeometry *pGeo)
{
	if (pFeatureA == nullptr) return FALSE;
	if (pFeatureA->m_pGeoObject == nullptr) return FALSE;

	float fPenWidth = 1.0;
	ID2D1StrokeStyle *pStrokeStyle = nullptr;
	UINT32 nRet = GetArcDrawInfo(pFeatureA->m_pSymbol, fPenWidth, &pStrokeStyle);

	BOOL bHit = FALSE;
	if (nRet == RESULT_GOOD)
	{//Stroke
		if (pGeo == nullptr)
		{
			pFeatureA->m_pGeoObject->StrokeContainsPoint(fptPoint, fPenWidth, pStrokeStyle, matViewScale, &bHit);
		}
		else
		{
			pGeo->StrokeContainsPoint(fptPoint, fPenWidth, pStrokeStyle, matViewScale, &bHit);
		}
	}

	return bHit;
}

BOOL CDrawFunction::CheckHighlight_TextFeature(IN D2D_POINT_2F fptPoint, IN CFeatureT* pFeatureT, IN CSymbol *pSymbol, IN const D2D1_MATRIX_3X2_F& matViewScale, ID2D1PathGeometry *pGeo)
{
	return FALSE;
}

BOOL CDrawFunction::CheckHighlight_SurfaceFeature(IN D2D_POINT_2F fptPoint, IN CFeatureS* pFeatureS, IN CSymbol *pSymbol, IN const D2D1_MATRIX_3X2_F& matViewScale,
	OUT int &nObOeIndex, OUT float &fArea, IN vector<ID2D1PathGeometry *> vecGeo)
{
	nObOeIndex = -1;

	if (pFeatureS == nullptr) return FALSE;

	int nSurfaceCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());

	//마우스 Over된 개체 구하기
	vector<int> vecSelectIndex;
	if (vecGeo.size() == 0)
	{//(일반적인 경우)외부에서 Geo를 입력받이 않았을때,
		for (int i = nSurfaceCount - 1; i >= 0; i--)
		{
			CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(i);
			if (pObOe == nullptr) continue;

			BOOL bHit = FALSE;
			if (pObOe->m_pGeoObject == nullptr) continue;

			pObOe->m_pGeoObject->FillContainsPoint(fptPoint, matViewScale, &bHit);
			
			if (bHit == TRUE)
			{
				vecSelectIndex.push_back(i);
			}
		}

		int nSelectSize = static_cast<int>(vecSelectIndex.size());
		if (nSelectSize <= 0) return FALSE;

		float fMinArea = FLT_MAX;
		int nSelectIndex;
		//구해진 개체의 넓이가 가장작은 개체 선택
		for (int i = 0; i < nSelectSize; i++)
		{
			nSelectIndex = vecSelectIndex[i];

			CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nSelectIndex);
			if (pObOe == nullptr) continue;
			if (pObOe->m_pGeoObject == nullptr) continue;

			float fFeatureArea = 0;
			pObOe->m_pGeoObject->ComputeArea(matViewScale, &fFeatureArea);
			if (fMinArea >= fFeatureArea)
			{
				fMinArea = fFeatureArea;
				nObOeIndex = nSelectIndex;
				fArea = fMinArea;
			}
		}
	}
	else
	{
		int nObOeCount = static_cast<int>(vecGeo.size());
		for (int i = 0 ; i < nObOeCount ; i++)
		{
			BOOL bHit = FALSE;
			if (vecGeo[i] == nullptr) continue;

			vecGeo[i]->FillContainsPoint(fptPoint, matViewScale, &bHit);

			if (bHit == TRUE)
			{
				vecSelectIndex.push_back(i);
			}
		}

		int nSelectSize = static_cast<int>(vecSelectIndex.size());
		if (nSelectSize <= 0) return FALSE;

		float fMinArea = FLT_MAX;
		int nSelectIndex;
		//구해진 개체의 넓이가 가장작은 개체 선택
		for (int i = 0; i < nSelectSize; i++)
		{
			nSelectIndex = vecSelectIndex[i];
			if (vecGeo[nSelectIndex] == nullptr) continue;

			float fFeatureArea = 0;
			vecGeo[nSelectIndex]->ComputeArea(matViewScale, &fFeatureArea);
			if (fMinArea >= fFeatureArea)
			{
				fMinArea = fFeatureArea;
				nObOeIndex = nSelectIndex;
				fArea = fMinArea;
			}
		}
	}
	
	if (nObOeIndex != -1)
	{
		return TRUE;
	}

	return FALSE;
}

UINT32 CDrawFunction::_SetLink(ID2D1Factory1 *pFactory, ID2D1DeviceContext* pD2DContext)
{
	if (pFactory == nullptr) return RESULT_BAD;
	if (pD2DContext == nullptr) return RESULT_BAD;

	m_pFactory = pFactory;
	m_pD2DContext = pD2DContext;

	m_pMakeGeo->SetLink(m_pFactory);

	m_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(0), &m_pLayerColor_Hole);
	m_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1), &m_pLayerColor);//흰색으로 만들자.

	m_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(1.f, 1.f, 1.f, 0.6f), &m_pLayerColor_Mask);//Align 외곽선
	m_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(0.3f, 0.3f, 0.3f, 0.3f), &m_pLayerColor_Mask_PreSelect);
	m_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(0.3f, 0.3f, 0.3f, 0.6f), &m_pLayerColor_Mask_Select);
	m_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(0.3f, 0.3f, 0.3f, 0.0f), &m_pLayerColor_Mask_Transparent);
	
	m_pFactory->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_FLAT,
			D2D1_LINE_JOIN_MITER,
			10.0f,
			D2D1_DASH_STYLE_SOLID,
			0.0f),
		0,
		0,
		&m_pStrokeStyle_r);

	m_pFactory->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_SQUARE,
			D2D1_CAP_STYLE_SQUARE,
			D2D1_CAP_STYLE_FLAT,
			D2D1_LINE_JOIN_MITER,
			10.0f,
			D2D1_DASH_STYLE_SOLID,
			0.0f),
		0,
		0,
		&m_pStrokeStyle_s);

	m_pFactory->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_SQUARE,
			D2D1_CAP_STYLE_SQUARE,
			D2D1_CAP_STYLE_FLAT,
			D2D1_LINE_JOIN_MITER,
			10.0f,
			D2D1_DASH_STYLE_DASH,
			0.0f),
		0,
		0,
		&m_pStrokeStyle_Dash);

	m_pFactory->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_ROUND,
			D2D1_CAP_STYLE_FLAT,
			D2D1_LINE_JOIN_MITER,
			10.0f,
			D2D1_DASH_STYLE_DASH_DOT,
			0.0f),
		0,
		0,
		&m_pStrokeStyle_Dash_Dot);
	

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_GetBrush(IN const bool &bPolarity, IN const bool &bHighlight, IN const bool &bPreHighlight, IN const COLORREF color,
	ID2D1SolidColorBrush **pBrush, IN const BOOL &bSave)
{
	//Draw와 Save의 render가 다르기 때문에 color 는 따로 사용한다.

	if (bSave == FALSE)
	{
		if (bPolarity == true)
		{//island
			if (m_pLayerColor == nullptr)
			{
				m_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(GetRValue(color) / 255.f,
					GetGValue(color) / 255.f,
					GetBValue(color) / 255.f), &m_pLayerColor);
			}

			if (bHighlight == true)
			{
				COLORREF colorInvert = _InvertColor(color);
				m_pLayerColor->SetColor(D2D1::ColorF(GetRValue(colorInvert) / 255.f,
					GetGValue(colorInvert) / 255.f,
					GetBValue(colorInvert) / 255.f));
			}
			else if (bPreHighlight == true)
			{
				COLORREF colorInvert = _InvertColor(color);
				m_pLayerColor->SetColor(D2D1::ColorF(GetRValue(colorInvert) / 255.f,
					GetGValue(colorInvert) / 255.f,
					GetBValue(colorInvert) / 255.f, 0.3f));//Alpha 추가
			}
			else
			{
				m_pLayerColor->SetColor(D2D1::ColorF(GetRValue(color) / 255.f,
					GetGValue(color) / 255.f,
					GetBValue(color) / 255.f));
			}

			*pBrush = m_pLayerColor;

		}
		else
		{//Neg

			D2D1::ColorF fColor = D2D1::ColorF(0.2f, 0.2f, 0.2f);

			if (!m_bIsDrawMode)
				fColor = D2D1::ColorF(0.f, 0.f, 0.f);

			if (m_pLayerColor_Neg == nullptr)
			{
				m_pD2DContext->CreateSolidColorBrush(fColor, &m_pLayerColor_Neg);
			}

			if (bHighlight == true)
			{
				COLORREF colorInvert = _InvertColor(color);
				m_pLayerColor_Neg->SetColor(D2D1::ColorF(GetRValue(colorInvert) / 255.f,
					GetGValue(colorInvert) / 255.f,
					GetBValue(colorInvert) / 255.f));
			}
			else if (bPreHighlight == true)
			{
				COLORREF colorInvert = _InvertColor(color);
				m_pLayerColor_Neg->SetColor(D2D1::ColorF(GetRValue(colorInvert) / 255.f,
					GetGValue(colorInvert) / 255.f,
					GetBValue(colorInvert) / 255.f, 0.3f));//Alpha 추가
			}
			else
			{
				m_pLayerColor_Neg->SetColor(fColor);
			}

			*pBrush = m_pLayerColor_Neg;
		}
	}
	else
	{//Save에 사용
		if (bPolarity == true)
		{//island
			*pBrush = m_pLayerColor_Save;
		}
		else
		{
			*pBrush = m_pLayerColor_Hole_Save;
		}
	}

	return RESULT_GOOD;

}

UINT32 CDrawFunction::_GetBrush_Surface(IN const bool &bPolarity, IN const bool &bHighlight, IN const bool &bPreHighlight, IN const COLORREF color,
	ID2D1SolidColorBrush **pBrush, IN const BOOL &bSave)
{
	if (bSave == FALSE)
	{
		if (bPolarity == true)
		{//island
			if (m_pLayerColor == nullptr)
			{
				m_pD2DContext->CreateSolidColorBrush(D2D1::ColorF(GetRValue(color) / 255.f,
					GetGValue(color) / 255.f,
					GetBValue(color) / 255.f), &m_pLayerColor);
			}

			if (bHighlight == true)
			{
				COLORREF colorInvert = _InvertColor(color);
				m_pLayerColor->SetColor(D2D1::ColorF(GetRValue(colorInvert) / 255.f,
					GetGValue(colorInvert) / 255.f,
					GetBValue(colorInvert) / 255.f));
			}
			else if (bPreHighlight == true)
			{
				COLORREF colorInvert = _InvertColor(color);
				m_pLayerColor->SetColor(D2D1::ColorF(GetRValue(colorInvert) / 255.f,
					GetGValue(colorInvert) / 255.f,
					GetBValue(colorInvert) / 255.f, 0.7f));//Alpha 추가
			}
			else
			{


				m_pLayerColor->SetColor(D2D1::ColorF(GetRValue(color) / 255.f,
					GetGValue(color) / 255.f,
					GetBValue(color) / 255.f));


			}

			*pBrush = m_pLayerColor;

		}
		else
		{//hole
			D2D1::ColorF fColor = D2D1::ColorF(0.2f, 0.2f, 0.2f);

			if (!m_bIsDrawMode)
				fColor = D2D1::ColorF(0.f, 0.f, 0.f);

			if (m_pLayerColor_Hole == nullptr)
			{
				m_pD2DContext->CreateSolidColorBrush(fColor, &m_pLayerColor_Hole);
			}

			if (bHighlight == true)
			{
				COLORREF colorInvert = _InvertColor(color);
				m_pLayerColor_Hole->SetColor(D2D1::ColorF(GetRValue(colorInvert) / 255.f,
					GetGValue(colorInvert) / 255.f,
					GetBValue(colorInvert) / 255.f));
			}
			else if (bPreHighlight == true)
			{
				COLORREF colorInvert = _InvertColor(color);
				m_pLayerColor_Hole->SetColor(D2D1::ColorF(GetRValue(colorInvert) / 255.f,
					GetGValue(colorInvert) / 255.f,
					GetBValue(colorInvert) / 255.f, 0.7f));//Alpha 추가
			}
			else
			{
				m_pLayerColor_Hole->SetColor(fColor);
			}

			*pBrush = m_pLayerColor_Hole;
		}
	}
	else
	{//SaveMode
		if (bPolarity == true)
		{//island
			*pBrush = m_pLayerColor_Save;
		}
		else
		{
			*pBrush = m_pLayerColor_Hole_Save;
		}
	}

	return RESULT_GOOD;

}

COLORREF CDrawFunction::_InvertColor(IN const COLORREF &color)
{
	COLORREF colorInvert;

	colorInvert = RGB(255 - GetRValue(color), 255 - GetGValue(color), 255 - GetBValue(color));

	return colorInvert;
}

UINT32 CDrawFunction::GetTpDrawInfo(IN const CSymbol* pSymbol, OUT float &fPenWidth, ID2D1StrokeStyle	**pStrokeStyle)
{//return RESULT_GOOD :S troke Geometry, RESULT_BAD : Fill Geometry

	if (pSymbol == nullptr) return RESULT_BAD;

	int nSize = static_cast<int>(pSymbol->m_vcParams.size());
	if (nSize <= 0) return RESULT_BAD;

	
	fPenWidth = static_cast<float>(pSymbol->m_vcParams[0]);
	(*pStrokeStyle) = m_pStrokeStyle_r;

	return RESULT_GOOD;
}

UINT32 CDrawFunction::GetLineDrawInfo(IN const CSymbol* pSymbol, OUT float &fPenWidth, ID2D1StrokeStyle	**pStrokeStyle)
{//return RESULT_GOOD :S troke Geometry, RESULT_BAD : Fill Geometry

	if (pSymbol == nullptr) return RESULT_BAD;

	int nSize = static_cast<int>(pSymbol->m_vcParams.size());
	if (nSize <= 0) return RESULT_BAD;

	switch (pSymbol->m_eSymbolName)
	{
	default:
	case SymbolName::r:
	case SymbolName::hole:
		fPenWidth = static_cast<float>(pSymbol->m_vcParams[0]);
		(*pStrokeStyle) = m_pStrokeStyle_r;
		break;
	
	case SymbolName::s:
	case SymbolName::rect:
		fPenWidth = static_cast<float>(pSymbol->m_vcParams[0]);
		(*pStrokeStyle) = m_pStrokeStyle_s;
		break;
	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::GetPadDrawInfo(IN const CSymbol* pSymbol, OUT float &fPenWidth, ID2D1StrokeStyle	**pStrokeStyle, CRotMirrArr *pRotMirrArr)
{//return RESULT_GOOD :S troke Geometry, RESULT_BAD : Fill Geometry

	if (pSymbol == nullptr) return RESULT_BAD;

	switch (pSymbol->m_eSymbolName)
	{
	case SymbolName::donut_r:
	case SymbolName::thr:
		fPenWidth = static_cast<float>((pSymbol->m_vcParams[0] - pSymbol->m_vcParams[1]) / 2.f);
		(*pStrokeStyle) = m_pStrokeStyle_r;
		break;
	case SymbolName::donut_s:

		fPenWidth = static_cast<float>((pSymbol->m_vcParams[0] - pSymbol->m_vcParams[1]) / 2.f);
		(*pStrokeStyle) = m_pStrokeStyle_s;
		break;
	case SymbolName::oval:
	{
		enum Orient emOrient;
		if (pRotMirrArr != nullptr)
		{
			emOrient = pRotMirrArr->FinalOrient();
		}
		else
		{
			emOrient = Orient::NoMir0Deg;
		}
		double dParam;
		switch (emOrient)
		{
		default:
		case Orient::NoMir0Deg:
		case Orient::Mir0Deg:
		case Orient::NoMir180Deg:
		case Orient::Mir180Deg:
			dParam = pSymbol->m_vcParams[0];
			break;
		case Orient::NoMir90Deg:
		case Orient::Mir90Deg:
		case Orient::NoMir270Deg:
		case Orient::Mir270Deg:
			dParam = pSymbol->m_vcParams[1];
		}

		if (pSymbol->m_vcParams[0] < pSymbol->m_vcParams[1]) {
			dParam = pSymbol->m_vcParams[0];
		}
		else
		{
			dParam = pSymbol->m_vcParams[1];
		}

		fPenWidth = static_cast<float>(dParam);
		(*pStrokeStyle) = m_pStrokeStyle_r;
	}
		break;
	case SymbolName::moire:
		fPenWidth = static_cast<float>(pSymbol->m_vcParams[0]) ; // /2 제거 
		(*pStrokeStyle) = m_pStrokeStyle_r;
		break;
	default:
		return RESULT_BAD;
		break;
	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::GetArcDrawInfo(IN const CSymbol* pSymbol, OUT float &fPenWidth, ID2D1StrokeStyle	**pStrokeStyle)
{//return RESULT_GOOD :S troke Geometry, RESULT_BAD : Fill Geometry

	if (pSymbol == nullptr) return RESULT_BAD;

	switch (pSymbol->m_eSymbolName)
	{
	case SymbolName::r:			case SymbolName::s:			case SymbolName::donut_r:
	case SymbolName::donut_s:	case SymbolName::bfr:		case SymbolName::bfs:
	case SymbolName::thr:		case SymbolName::ths:		case SymbolName::s_ths:
	case SymbolName::s_tho:		case SymbolName::sr_ths:	case SymbolName::hole:
	{
		fPenWidth = static_cast<float>(pSymbol->m_vcParams[0]);
		(*pStrokeStyle) = m_pStrokeStyle_r;
	}
	break;

	case SymbolName::rect:			case SymbolName::rectxr: 	case SymbolName::rectxc:
	case SymbolName::oval:			case SymbolName::di:		case SymbolName::octagon:
	case SymbolName::hex_l:			case SymbolName::hex_s:		case SymbolName::tri:
	case SymbolName::oval_h:		case SymbolName::rc_ths:	case SymbolName::rc_tho:
	case SymbolName::el:
	{

		if (pSymbol->m_vcParams[0] > pSymbol->m_vcParams[1])
			fPenWidth = static_cast<float>(pSymbol->m_vcParams[1]);
		else
			fPenWidth = static_cast<float>(pSymbol->m_vcParams[0]);

		(*pStrokeStyle) = m_pStrokeStyle_s;
	}
	break;

	case SymbolName::moire:
	{
		fPenWidth = static_cast<float> (pSymbol->m_vcParams[4]);
		(*pStrokeStyle) = m_pStrokeStyle_r;
	}
	break;

	default:
	{
		fPenWidth = static_cast<float>(pSymbol->m_vcParams[0]);
		(*pStrokeStyle) = m_pStrokeStyle_r;
	}
	break;
	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawTempArc(IN ID2D1RenderTarget*pRender, IN D2D_POINT_2F fptStart, IN D2D_POINT_2F fptEnd, IN D2D_POINT_2F fptCenter, IN BOOL bCw)
{
	D2D1_MATRIX_3X2_F matTrans;
	pRender->GetTransform(&matTrans);

	float fLineWidth = static_cast<float>(fabs(1.f / matTrans.m11));

	ID2D1PathGeometry *pArcGeoMetry;
	m_pFactory->CreatePathGeometry(&pArcGeoMetry);

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData = CDrawMakePoint::MakeArc(fptStart, fptEnd, fptCenter, bCw, nullptr);
	if (vecFeatureData.size() <= 0) return RESULT_BAD;


	_SetGeometry(vecFeatureData[0].vecPointData, &pArcGeoMetry, TRUE);
	
	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	pRender->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 1.0f), &pLayerColor);
	pRender->DrawGeometry(pArcGeoMetry, pLayerColor, fLineWidth, m_pStrokeStyle_r);
	SafeRelease(&pArcGeoMetry);

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawTempLine(IN ID2D1RenderTarget* pRender, IN D2D_POINT_2F fptStart, IN D2D_POINT_2F fptEnd) 
{
	if (pRender == nullptr) return RESULT_BAD;

	D2D1_MATRIX_3X2_F matTrans;
	pRender->GetTransform(&matTrans);

	float fLineWidth = static_cast<float>(fabs(1.f / matTrans.m11));

	D2D1_POINT_2F Start = fptStart;
	D2D1_POINT_2F End = fptEnd;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	_GetBrush(true, false, false, RGB(255, 255, 255), &pLayerColor);
	if (pLayerColor == nullptr) return RESULT_BAD;

	pRender->DrawLine(Start,End, pLayerColor, fLineWidth, m_pStrokeStyle_r);
	return RESULT_GOOD;

}

UINT32 CDrawFunction::_DrawTempArcPoint(IN ID2D1RenderTarget* pRender, IN D2D_POINT_2F fptCenter)
{
	if (pRender == nullptr) return RESULT_BAD;

	D2D1_MATRIX_3X2_F matTrans;
	pRender->GetTransform(&matTrans);

	float fLineWidth = static_cast<float>(fabs(1.f / matTrans.m11));
	float fLineLength = static_cast<float>(fabs(5.f / matTrans.m11));
	float fLineLength_hs = fLineLength / 0.2f;

	D2D1_POINT_2F fptLine_1[2];
	D2D1_POINT_2F fptLine_2[2];

	fptLine_1[0].x = fptCenter.x - fLineLength_hs;
	fptLine_1[0].y = fptCenter.y - fLineLength_hs;
	fptLine_1[1].x = fptCenter.x + fLineLength_hs;
	fptLine_1[1].y = fptCenter.y + fLineLength_hs;

	fptLine_2[0].x = fptCenter.x + fLineLength_hs;
	fptLine_2[0].y = fptCenter.y - fLineLength_hs;
	fptLine_2[1].x = fptCenter.x - fLineLength_hs;
	fptLine_2[1].y = fptCenter.y + fLineLength_hs;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	_GetBrush(true, false, false, RGB(128, 128, 128), &pLayerColor);
	if (pLayerColor == nullptr) return RESULT_BAD;

	pRender->DrawLine(fptLine_1[0], fptLine_1[1], pLayerColor, fLineWidth, m_pStrokeStyle_r);
	pRender->DrawLine(fptLine_2[0], fptLine_2[1], pLayerColor, fLineWidth, m_pStrokeStyle_r);

	return RESULT_GOOD;
}



UINT32 CDrawFunction::_DrawTempPoint(IN ID2D1RenderTarget* pRender, IN D2D_POINT_2F fptCenter)
{
	if (pRender == nullptr) return RESULT_BAD;

	D2D1_MATRIX_3X2_F matTrans;
	pRender->GetTransform(&matTrans);
	float fScale = fabs(matTrans.m11);
	if (fScale == 0.f)
	{
		fScale = static_cast<float>(fabs(matTrans.m12));
	}

	float fLineWidth = static_cast<float>(fabs(1.f / fScale));
	float fLineLength = static_cast<float>(fabs(5.f / fScale));
	float fLineLength_hs = fLineLength / .2f;

	D2D1_POINT_2F fptLine_1[2];
	D2D1_POINT_2F fptLine_2[2];

	fptLine_1[0].x = fptCenter.x - fLineLength_hs;
	fptLine_1[0].y = fptCenter.y - fLineLength_hs;
	fptLine_1[1].x = fptCenter.x + fLineLength_hs;
	fptLine_1[1].y = fptCenter.y + fLineLength_hs;

	fptLine_2[0].x = fptCenter.x + fLineLength_hs;
	fptLine_2[0].y = fptCenter.y - fLineLength_hs;
	fptLine_2[1].x = fptCenter.x - fLineLength_hs;
	fptLine_2[1].y = fptCenter.y + fLineLength_hs;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	_GetBrush(true, false, false, RGB(255,255,255), &pLayerColor);
	if (pLayerColor == nullptr) return RESULT_BAD;

	pRender->DrawLine(fptLine_1[0], fptLine_1[1], pLayerColor, fLineWidth, m_pStrokeStyle_r);
	pRender->DrawLine(fptLine_2[0], fptLine_2[1], pLayerColor, fLineWidth, m_pStrokeStyle_r);

	return RESULT_GOOD;
}

UINT32 CDrawFunction::DrawEditPoint(IN ID2D1RenderTarget* pRender, IN const RECTD &drtRect)
{
	D2D1_RECT_F frtRect = D2D1::RectF(DoubleToFloat(drtRect.left), DoubleToFloat(drtRect.top),
		DoubleToFloat(drtRect.right), DoubleToFloat(drtRect.bottom));


	//Color 정의
	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	_GetBrush(true, false, false, RGB(50, 50, 50), &pLayerColor);
	if (pLayerColor == nullptr) return RESULT_BAD;

// 	D2D1_COLOR_F fColor = pLayerColor->GetColor();
// 	fColor.a = 0.5;//반투명
// 	pLayerColor->SetColor(fColor);
	
	pRender->FillRectangle(frtRect, pLayerColor);

	ID2D1SolidColorBrush		*pLayerColor_Frame = nullptr;
	_GetBrush(true, false, false, RGB(255, 255, 255), &pLayerColor_Frame);
	if (pLayerColor_Frame == nullptr) return RESULT_BAD;

// 	fColor = pLayerColor_Frame->GetColor();
// 	pLayerColor_Frame->SetColor(fColor);

	//Line Width
	D2D1_MATRIX_3X2_F matTrans;
	pRender->GetTransform(&matTrans);
	float fScale = fabs(matTrans.m11);
	if (fScale == 0.f)
	{
		fScale = static_cast<float>(fabs(matTrans.m12));
	}

	float fLineWidth = static_cast<float>(fabs(1.f / fScale));
	//
	
	pRender->DrawRectangle(frtRect, pLayerColor_Frame, fLineWidth, m_pStrokeStyle_r);

	return RESULT_GOOD;
}


UINT32 CDrawFunction::OnRender_Layer(IN ID2D1RenderTarget* pRender, IN CLayer* pLayer, IN const LayerSet_t* pLayerSet,
	CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate,
	IN const UserSetInfo_t *pUserSetInfo, IN const RECTD &drtViewRect_mm, IN const BOOL &bRedrawSelect)
{
	m_bIsDrawMode = true;

	if (pRender == nullptr) return RESULT_BAD;
	if (pLayer == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (pUserSetInfo == nullptr) return RESULT_BAD;

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetSize());

	CFeature*	pFeature = nullptr;
	CSymbol*	pSymbol = nullptr;

	for (int feno = 0; feno < nFeatureCount; feno++)
	{
		pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(feno);
		if (pFeature == nullptr)
			continue;

		if (drtViewRect_mm.left != 0. &&
			drtViewRect_mm.right != 0. &&
			drtViewRect_mm.top != 0. &&
			drtViewRect_mm.bottom != 0. )
		//교집합 체크 : Draw 시간을 줄이기 위함
		{

			RECTD drtFeature;
			if (pRotMirrArr == nullptr)
			{
				drtFeature = pFeature->m_MinMaxRect;
			}
			else
			{
				pRotMirrArr->FinalPoint(&drtFeature.left, &drtFeature.top, pFeature->m_MinMaxRect.left, pFeature->m_MinMaxRect.top);
				pRotMirrArr->FinalPoint(&drtFeature.right, &drtFeature.bottom, pFeature->m_MinMaxRect.right, pFeature->m_MinMaxRect.bottom);
				drtFeature.NormalizeRectD();
			}

			if (drtFeature.IsIntersection(drtViewRect_mm) == FALSE)
			{
				continue;
			}
		}

		switch (pFeature->m_eType)
		{
		case::FeatureType::Z:
		{
			CFeatureZ* pFeatureZ = (CFeatureZ*)pFeature;
			pSymbol = pFeatureZ->m_pSymbol;
			if (bRedrawSelect == static_cast<BOOL>(pFeatureZ->m_bHighlight))
			{

				_DrawTpFeature(pRender, pFeatureZ, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate);
			}
			else if (bRedrawSelect == static_cast<BOOL>(pFeatureZ->m_bPreHighlight))
			{
				_DrawTpFeature(pRender,pFeatureZ,pSymbol,pLayerSet,pRotMirrArr,bNeedUpdate);
			}
		}
		break;

		case FeatureType::L:		//Line
		{
			CFeatureL* pFeatureL = (CFeatureL*)pFeature;
			pSymbol = pFeatureL->m_pSymbol;
			if (bRedrawSelect == static_cast<BOOL>(pFeatureL->m_bHighlight))
			{
				_DrawLineFeature(pRender, pFeatureL, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate);
			}
			else if (bRedrawSelect == static_cast<BOOL>(pFeatureL->m_bPreHighlight))
			{
				_DrawLineFeature(pRender, pFeatureL, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate);
			}
		}
		break;
		case FeatureType::P:		//Pad
		{
			CFeatureP* pFeatureP = (CFeatureP*)pFeature;
			pSymbol = pFeatureP->m_pSymbol;
			if (bRedrawSelect == static_cast<BOOL>(pFeatureP->m_bHighlight))
			{
				_DrawPadFeature(pRender, pFeatureP, pSymbol, pLayerSet, pFontArr, pRotMirrArr, bNeedUpdate, pUserSetInfo, bRedrawSelect);
			}
			else if (bRedrawSelect == static_cast<BOOL>(pFeatureP->m_bPreHighlight))
			{
				_DrawPadFeature(pRender, pFeatureP, pSymbol, pLayerSet, pFontArr, pRotMirrArr, bNeedUpdate, pUserSetInfo, bRedrawSelect);
			}
		}
		break;
		case FeatureType::A:		//Arc								
		{
			CFeatureA* pFeatureA = (CFeatureA*)pFeature;
			pSymbol = pFeatureA->m_pSymbol;
			if (bRedrawSelect == static_cast<BOOL>(pFeatureA->m_bHighlight))
			{
				_DrawArcFeature(pRender, pFeatureA, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate);
			}
			else if (bRedrawSelect == static_cast<BOOL>(pFeatureA->m_bPreHighlight))
			{
				_DrawArcFeature(pRender, pFeatureA, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate);
			}
		}
		break;
		case FeatureType::T:		//Text			
		{

			CFeatureT* pFeatureT = (CFeatureT*)pFeature;
			pSymbol = pFeatureT->m_pSymbol;

			pRotMirrArr->InputData(pFeatureT->m_dX, pFeatureT->m_dY, pFeatureT->m_eOrient);		

			_DrawTextFeature(pRender, pFeatureT, pSymbol, pLayerSet, pFontArr, pRotMirrArr, bNeedUpdate);

			pRotMirrArr->RemoveTail();
		}
		break;
		case FeatureType::S:		//Surface	

			if (pUserSetInfo->bShowSurface)
			{
				pSymbol = ((CFeatureS*)pFeature)->m_pSymbol;
				_DrawSurfaceFeature(pRender, (CFeatureS*)pFeature, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bRedrawSelect);
			}

			break;
		
		}
	}

	//Draw Align / Mask
	int nCount = 0;
	nCount = static_cast<int>(pLayer->m_arrAlign.GetCount());
	for (int i = 0; i < nCount; i++)
	{
		CAlignMask *pAlignMask = pLayer->m_arrAlign.GetAt(i);
		int nFeatureCount = static_cast<int>(pAlignMask->m_arrFeature.GetCount());
		for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
		{
			CFeature *pFeature = pAlignMask->m_arrFeature.GetAt(nfeno);
			if (pFeature == nullptr) continue;

			pSymbol = pFeature->m_pSymbol;
			_DrawMask(pRender, MaskType::enumType_Align, pFeature, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bRedrawSelect);
		}
	}

	nCount = static_cast<int>(pLayer->m_arrMask.GetCount());
	for (int i = 0; i < nCount; i++)
	{
		CAlignMask *pAlignMask = pLayer->m_arrMask.GetAt(i);
		int nFeatureCount = static_cast<int>(pAlignMask->m_arrFeature.GetCount());
		for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
		{
			CFeature *pFeature = pAlignMask->m_arrFeature.GetAt(nfeno);
			if (pFeature == nullptr) continue;

			pSymbol = pFeature->m_pSymbol;
			_DrawMask(pRender, MaskType::enumType_Mask, pFeature, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bRedrawSelect);
		}
	}
	

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}


	return RESULT_GOOD;
}
UINT32 CDrawFunction::_DrawTpFeature(IN ID2D1RenderTarget* pRender, IN CFeatureZ* pFeatureZ, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave /* = FALSE */)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureZ == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureZ->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureZ->m_bPolarity : !pFeatureZ->m_bPolarity;
	_GetBrush(bPolarity, pFeatureZ->m_bHighlight, pFeatureZ->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);


	

	float fPenWidth = 0.003f;
	ID2D1StrokeStyle *pStrokeStyle = nullptr;
	UINT32 nRet = GetLineDrawInfo(pSymbol, fPenWidth, &pStrokeStyle);
	
	D2D1_MATRIX_3X2_F matTrans;
	pRender->GetTransform(&matTrans);

	float fScale = fabs(matTrans.m11);
	if (fScale == 0.f)
	{
		fScale = static_cast<float>(fabs(matTrans.m12));
	}

	//vector<MakeFeatureData> vecFeatureData;
	vector<DrawFeatureGeo> vecDrawFeatureGeo;
	vecDrawFeatureGeo.resize(1);
	if (bSave == TRUE)
	{
		m_pMakeGeo->MakeTp(pFeatureZ, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());
		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pStrokeStyle == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;
			float fPenWidth = 0.006f;
			pRender->DrawGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor, fPenWidth, pStrokeStyle);
		}
	}
	else
	{
		//if (bNeedUpdate == TRUE || pFeatureZ->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakeTp(pFeatureZ, pRotMirrArr, &pFeatureZ->m_pGeoObject);
		}

		if (pFeatureZ->m_pGeoObject == nullptr) return RESULT_BAD;
		if (pStrokeStyle == nullptr) return RESULT_BAD;
		if (pLayerColor == nullptr) return RESULT_BAD;
		float fPenWidth = 0.006f;

		//pRender->DrawGeometry(pFeatureZ->m_pGeoObject, pLayerColor, fPenWidth, pStrokeStyle);
		pRender->FillGeometry(pFeatureZ->m_pGeoObject, pLayerColor);

		{
			D2D1_MATRIX_3X2_F matTrans;
			pRender->GetTransform(&matTrans);

			D2D1_RECT_F frtBound;
			pFeatureZ->m_pGeoObject->GetBounds(matTrans, &frtBound);
			D2D1_RECT_F frtBound_mm = DetansformRect(frtBound, matTrans, nullptr, 1);
			
			float fCenterX = (frtBound_mm.right + frtBound_mm.left) / 2.f;
			float fCenterY = (frtBound_mm.bottom + frtBound_mm.top) / 2.f;
			float fWidth_hs = fabs(frtBound_mm.right - frtBound_mm.left) / 2.f;
			float fHeight_hs = fabs(frtBound_mm.bottom - frtBound_mm.top) / 2.f;

			D2D1_POINT_2F fptStart_1 = D2D1::Point2F(fCenterX - fWidth_hs, fCenterY);
			D2D1_POINT_2F fptEnd_1 = D2D1::Point2F(fCenterX + fWidth_hs, fCenterY);
			D2D1_POINT_2F fptStart_2 = D2D1::Point2F(fCenterX, fCenterY - fWidth_hs);
			D2D1_POINT_2F fptEnd_2 = D2D1::Point2F(fCenterX, fCenterY + fWidth_hs);

			
			_GetBrush(bPolarity, pFeatureZ->m_bHighlight, pFeatureZ->m_bPreHighlight, _InvertColor(pLayerSet->color), &pLayerColor, bSave);

			pRender->DrawLine(fptStart_1, fptEnd_1, pLayerColor, fPenWidth, pStrokeStyle);
			pRender->DrawLine(fptStart_2, fptEnd_2, pLayerColor, fPenWidth, pStrokeStyle);
		}

	}

	return RESULT_GOOD;
}
UINT32 CDrawFunction::_DrawLineFeature(IN ID2D1RenderTarget* pRender, IN CFeatureL* pFeatureL, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureL == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if ( pFeatureL->m_bShow == FALSE ) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureL->m_bPolarity : !pFeatureL->m_bPolarity;
	_GetBrush(bPolarity, pFeatureL->m_bHighlight, pFeatureL->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	float fPenWidth = 1.0;
	ID2D1StrokeStyle *pStrokeStyle = nullptr;
	UINT32 nRet = GetLineDrawInfo(pSymbol, fPenWidth, &pStrokeStyle);
	
	//vector<MakeFeatureData> vecFeatureData;
	vector<DrawFeatureGeo> vecDrawFeatureGeo;
	vecDrawFeatureGeo.resize(1);
	if (bSave == TRUE)
	{
		m_pMakeGeo->MakeLine(pFeatureL, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());
		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pStrokeStyle == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;

			pRender->DrawGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor, fPenWidth, pStrokeStyle);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureL->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakeLine(pFeatureL, pRotMirrArr, &pFeatureL->m_pGeoObject);
		}

		if (pFeatureL->m_pGeoObject == nullptr) return RESULT_BAD;
		if (pStrokeStyle == nullptr) return RESULT_BAD;
		if (pLayerColor == nullptr) return RESULT_BAD;

		pRender->DrawGeometry(pFeatureL->m_pGeoObject, pLayerColor, fPenWidth, pStrokeStyle);
	}

	return RESULT_GOOD;
}


UINT32 CDrawFunction::_DrawPadFeature(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr,
	IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo, IN const BOOL &bRedrawSelect, IN const BOOL &bSave, IN RECTD drtViewRect_mm)
{
	if (pSymbol == nullptr)
		return RESULT_BAD;
	
	UINT32 nRet = RESULT_BAD;
	switch (pSymbol->m_eSymbolName)
	{
	case SymbolName::r:		//circle
		nRet = _DrawPadCircle(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::hole:
		nRet = _DrawPadHole(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::s:		//square
	case SymbolName::rect:	//rectangle
		nRet = _DrawPadRect(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::rectxr: //Rounded Rectangle  
		nRet = _DrawPadRectxr(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::rectxc: //Rounded Rectangle  
		nRet = _DrawPadRectxc(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::oval:	//oval  
		nRet = _DrawPadOval(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::di:    //Diamond
		nRet = _DrawPadDi(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::octagon:
		nRet = _DrawPadOct(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::donut_r:
		nRet = _DrawPadDonut_r(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::donut_s:
		nRet = _DrawPadDonut_s(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::hex_l:
		nRet = _DrawPadHex_l(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::hex_s:
		nRet = _DrawPadHex_s(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::bfr: //Butterfly   
		nRet = _DrawPadBfr(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::bfs: //SQUARE Butterfly  
		nRet = _DrawPadBfs(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::tri:
		nRet = _DrawPadTriangle(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::oval_h: //Half Oval   
		nRet = _DrawPadOval_h(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::thr:
		nRet = _DrawPadThr(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::ths:
		nRet = _DrawPadThs(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::s_ths:
		nRet = _DrawPadS_ths(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::s_tho:
		nRet = _DrawPadS_tho(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::sr_ths:
		nRet = _DrawPadSr_ths(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::rc_ths:
		nRet = _DrawPadRc_ths(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::rc_tho:
		nRet = _DrawPadRc_tho(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::el:	//ellipse
		nRet = _DrawPadEllipse(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;
	case SymbolName::moire: //Moire  
		nRet = _DrawPadMoire(pRender, pFeatureP, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bSave);
		break;

	case SymbolName::userdefined://user defined symbol 인경우
	{
		ID2D1PathGeometry		*pGeoObject = nullptr;
		pGeoObject = _DrawPadUserdefined(pRender, pFeatureP, pSymbol, pLayerSet, pFontArr, pRotMirrArr, bNeedUpdate, pUserSetInfo, bRedrawSelect, drtViewRect_mm, bSave);
		if (pGeoObject != nullptr)
		{
			pFeatureP->m_pGeoObject = pGeoObject;
		}
	}
		break;
	default:
		break;
	}

	return nRet;
}

UINT32 CDrawFunction::_DrawPadCircle(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	vector<DrawFeatureGeo> vecDrawFeatureGeo;
	vecDrawFeatureGeo.resize(1);

	if (bSave == TRUE)
	{
		m_pMakeGeo->MakePadCircle(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);

		int nCount = static_cast<int>(vecDrawFeatureGeo.size());
		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;

			pRender->FillGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadCircle(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;

		pRender->FillGeometry(pFeatureP->m_pGeoObject, pLayerColor);

	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadRect(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	vector<DrawFeatureGeo> vecDrawFeatureGeo;
	vecDrawFeatureGeo.resize(1);
	if (bSave == TRUE)
	{
		m_pMakeGeo->MakePadRect(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());
		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;

			pRender->FillGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadRect(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;

		pRender->FillGeometry(pFeatureP->m_pGeoObject, pLayerColor);

	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadRectxr(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	if (bSave == TRUE)
	{
		vector<DrawFeatureGeo> vecDrawFeatureGeo;
		vecDrawFeatureGeo.resize(1);

		m_pMakeGeo->MakePadRectxr(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());

		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;

			pRender->FillGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadRectxr(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;

		pRender->FillGeometry(pFeatureP->m_pGeoObject, pLayerColor);

	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadRectxc(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{

	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);
	
	if (bSave == TRUE)
	{
		vector<DrawFeatureGeo> vecDrawFeatureGeo;
		vecDrawFeatureGeo.resize(1);

		m_pMakeGeo->MakePadRectxc(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());

		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;

			pRender->FillGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadRectxc(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;

		pRender->FillGeometry(pFeatureP->m_pGeoObject, pLayerColor);

	}
	
	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadOval(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	float fPenWidth = 1.0;
	ID2D1StrokeStyle *pStrokeStyle = nullptr;
	UINT32 nRet = GetPadDrawInfo(pSymbol, fPenWidth, &pStrokeStyle, pRotMirrArr);

	if (bSave == TRUE)
	{
		vector<DrawFeatureGeo> vecDrawFeatureGeo;
		vecDrawFeatureGeo.resize(1);

		m_pMakeGeo->MakePadOval(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());

		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;
			if (pStrokeStyle == nullptr) return RESULT_BAD;

			pRender->DrawGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor, fPenWidth, pStrokeStyle);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadOval(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;
		if (pLayerColor == nullptr) return RESULT_BAD;
		if (pStrokeStyle == nullptr) return RESULT_BAD;

		pRender->DrawGeometry(pFeatureP->m_pGeoObject, pLayerColor, fPenWidth, pStrokeStyle);

	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadDi(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	if (bSave == TRUE)
	{
		vector<DrawFeatureGeo> vecDrawFeatureGeo;
		vecDrawFeatureGeo.resize(1);

		m_pMakeGeo->MakePadDi(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());

		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;

			pRender->FillGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadDi(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;

		pRender->FillGeometry(pFeatureP->m_pGeoObject, pLayerColor);

	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadOct(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);
	
	if (bSave == TRUE)
	{
		vector<DrawFeatureGeo> vecDrawFeatureGeo;
		vecDrawFeatureGeo.resize(1);

		m_pMakeGeo->MakePadOct(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());

		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;

			pRender->FillGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadOct(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;

		pRender->FillGeometry(pFeatureP->m_pGeoObject, pLayerColor);

	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadDonut_r(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	float fPenWidth = 1.0;
	ID2D1StrokeStyle *pStrokeStyle = nullptr;
	UINT32 nRet = GetPadDrawInfo(pSymbol, fPenWidth, &pStrokeStyle, pRotMirrArr);

	if (bSave == TRUE)
	{
		vector<DrawFeatureGeo> vecDrawFeatureGeo;
		vecDrawFeatureGeo.resize(1);

		m_pMakeGeo->MakePadDonut_r(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());

		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;
			if (pStrokeStyle == nullptr) return RESULT_BAD;

			pRender->DrawGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor, fPenWidth, pStrokeStyle);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadDonut_r(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;
		if (pStrokeStyle == nullptr) return RESULT_BAD;

		pRender->DrawGeometry(pFeatureP->m_pGeoObject, pLayerColor, fPenWidth, pStrokeStyle);

	}
	
	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadDonut_s(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	if (pLayerColor == nullptr)	return RESULT_BAD;

	float fPenWidth = 1.0;
	ID2D1StrokeStyle *pStrokeStyle = nullptr;
	UINT32 nRet = GetPadDrawInfo(pSymbol, fPenWidth, &pStrokeStyle, pRotMirrArr);

	if (bSave == TRUE)
	{
		vector<DrawFeatureGeo> vecDrawFeatureGeo;
		vecDrawFeatureGeo.resize(1);

		m_pMakeGeo->MakePadDonut_s(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());

		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;
			if (pStrokeStyle == nullptr) return RESULT_BAD;

			pRender->DrawGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor, fPenWidth, pStrokeStyle);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadDonut_s(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;
		if (pStrokeStyle == nullptr) return RESULT_BAD;

		pRender->DrawGeometry(pFeatureP->m_pGeoObject, pLayerColor, fPenWidth, pStrokeStyle);

	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadHex_l(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	if (bSave == TRUE)
	{
		vector<DrawFeatureGeo> vecDrawFeatureGeo;
		vecDrawFeatureGeo.resize(1);

		m_pMakeGeo->MakePadHex_l(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());

		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;

			pRender->FillGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadHex_l(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;

		pRender->FillGeometry(pFeatureP->m_pGeoObject, pLayerColor);

	}	
		
	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadHex_s(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	if (bSave == TRUE)
	{
		vector<DrawFeatureGeo> vecDrawFeatureGeo;
		vecDrawFeatureGeo.resize(1);

		m_pMakeGeo->MakePadHex_s(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());

		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;

			pRender->FillGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadHex_s(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;

		pRender->FillGeometry(pFeatureP->m_pGeoObject, pLayerColor);

	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadBfr(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	if (bSave == TRUE)
	{
		vector<DrawFeatureGeo> vecDrawFeatureGeo;
		vecDrawFeatureGeo.resize(1);

		m_pMakeGeo->MakePadBfr(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());

		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;

			pRender->FillGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadBfr(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;

		pRender->FillGeometry(pFeatureP->m_pGeoObject, pLayerColor);

	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadBfs(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	if (bSave == TRUE)
	{
		vector<DrawFeatureGeo> vecDrawFeatureGeo;
		vecDrawFeatureGeo.resize(1);

		m_pMakeGeo->MakePadBfs(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());

		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;

			pRender->FillGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadBfs(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;

		pRender->FillGeometry(pFeatureP->m_pGeoObject, pLayerColor);

	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadTriangle(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	if (bSave == TRUE)
	{
		vector<DrawFeatureGeo> vecDrawFeatureGeo;
		vecDrawFeatureGeo.resize(1);

		m_pMakeGeo->MakePadTriangle(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());


		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;

			pRender->FillGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadTriangle(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;

		pRender->FillGeometry(pFeatureP->m_pGeoObject, pLayerColor);

	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadOval_h(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	if (bSave == TRUE)
	{
		vector<DrawFeatureGeo> vecDrawFeatureGeo;
		vecDrawFeatureGeo.resize(1);

		m_pMakeGeo->MakePadOval_h(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());

		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;

			pRender->FillGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadOval_h(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;

		pRender->FillGeometry(pFeatureP->m_pGeoObject, pLayerColor);

	}
	
	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadThr(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	if (pLayerColor == nullptr)	return RESULT_BAD;

	float fPenWidth = 1.0;
	ID2D1StrokeStyle *pStrokeStyle = nullptr;
	UINT32 nRet = GetPadDrawInfo(pSymbol, fPenWidth, &pStrokeStyle, pRotMirrArr);

	if (bSave == TRUE)
	{
		vector<DrawFeatureGeo> vecDrawFeatureGeo;
		vecDrawFeatureGeo.resize(1);

		m_pMakeGeo->MakePadThr(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());

		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;

			pRender->DrawGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor, fPenWidth, pStrokeStyle);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadThr(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;

		pRender->DrawGeometry(pFeatureP->m_pGeoObject, pLayerColor, fPenWidth, pStrokeStyle);

	}
	
	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadThs(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	if (bSave == TRUE)
	{
		vector<DrawFeatureGeo> vecDrawFeatureGeo;
		vecDrawFeatureGeo.resize(1);

		m_pMakeGeo->MakePadThs(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());


		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;

			pRender->FillGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadThs(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;

		pRender->FillGeometry(pFeatureP->m_pGeoObject, pLayerColor);


	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadS_ths(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	if (bSave == TRUE)
	{
		vector<DrawFeatureGeo> vecDrawFeatureGeo;
		vecDrawFeatureGeo.resize(1);

		m_pMakeGeo->MakePadS_ths(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());


		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;

			pRender->FillGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadS_ths(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;
		//if (pFeatureP->m_pGeoObject_Hole == nullptr) return RESULT_BAD;

		pRender->FillGeometry(pFeatureP->m_pGeoObject, pLayerColor);

// 		m_pLayerColor_Hole->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
// 		pRender->FillGeometry(pFeatureP->m_pGeoObject_Hole, m_pLayerColor_Hole);

	}
		
	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadS_tho(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	if (bSave == TRUE)
	{
		vector<DrawFeatureGeo> vecDrawFeatureGeo;
		vecDrawFeatureGeo.resize(1);

		m_pMakeGeo->MakePadS_tho(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());

		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;

			pRender->FillGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadS_tho(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;
		//if (pFeatureP->m_pGeoObject_Hole == nullptr) return RESULT_BAD;

		pRender->FillGeometry(pFeatureP->m_pGeoObject, pLayerColor);

// 		m_pLayerColor_Hole->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
// 		pRender->FillGeometry(pFeatureP->m_pGeoObject_Hole, m_pLayerColor_Hole);

	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadSr_ths(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	if (bSave == TRUE)
	{
		vector<DrawFeatureGeo> vecDrawFeatureGeo;
		vecDrawFeatureGeo.resize(1);

		m_pMakeGeo->MakePadSr_ths(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());

		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;

			pRender->FillGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadSr_ths(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;
		//if (pFeatureP->m_pGeoObject_Hole == nullptr) return RESULT_BAD;

		pRender->FillGeometry(pFeatureP->m_pGeoObject, pLayerColor);

// 		m_pLayerColor_Hole->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
// 		pRender->FillGeometry(pFeatureP->m_pGeoObject_Hole, m_pLayerColor_Hole);

	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadRc_ths(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;
	
	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	if (bSave == TRUE)
	{
		vector<DrawFeatureGeo> vecDrawFeatureGeo;
		vecDrawFeatureGeo.resize(1);

		m_pMakeGeo->MakePadRc_ths(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());

		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;

			pRender->FillGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadRc_ths(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;
		//if (pFeatureP->m_pGeoObject_Hole == nullptr) return RESULT_BAD;

		pRender->FillGeometry(pFeatureP->m_pGeoObject, pLayerColor);

// 		m_pLayerColor_Hole->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
// 		pRender->FillGeometry(pFeatureP->m_pGeoObject_Hole, m_pLayerColor_Hole);

	}
	
	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadRc_tho(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	if (bSave == TRUE)
	{
		vector<DrawFeatureGeo> vecDrawFeatureGeo;
		vecDrawFeatureGeo.resize(1);

		m_pMakeGeo->MakePadRc_tho(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());

		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;

			pRender->FillGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadRc_tho(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;
		//if (pFeatureP->m_pGeoObject_Hole == nullptr) return RESULT_BAD;

		pRender->FillGeometry(pFeatureP->m_pGeoObject, pLayerColor);

// 		m_pLayerColor_Hole->SetColor(D2D1::ColorF(D2D1::ColorF::Black));
// 		pRender->FillGeometry(pFeatureP->m_pGeoObject_Hole, m_pLayerColor_Hole);

	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadEllipse(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	if (bSave == TRUE)
	{
		vector<DrawFeatureGeo> vecDrawFeatureGeo;
		vecDrawFeatureGeo.resize(1);

		m_pMakeGeo->MakePadEllipse(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());

		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;

			pRender->FillGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadEllipse(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;

		pRender->FillGeometry(pFeatureP->m_pGeoObject, pLayerColor);

	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadMoire(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
	_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	if (pLayerColor == nullptr)	return RESULT_BAD;
	float fPenWidth = 1.0;
	ID2D1StrokeStyle *pStrokeStyle = nullptr;
	UINT32 nRet = GetPadDrawInfo(pSymbol, fPenWidth, &pStrokeStyle, pRotMirrArr);

	if (bSave == TRUE)
	{
		vector<DrawFeatureGeo> vecDrawFeatureGeo;
		vecDrawFeatureGeo.resize(1);

		m_pMakeGeo->MakePadMoire(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());

		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;

			pRender->DrawGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor, fPenWidth, pStrokeStyle);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakePadMoire(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;

		pRender->DrawGeometry(pFeatureP->m_pGeoObject, pLayerColor, fPenWidth, pStrokeStyle);
	}
	

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawPadHole(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)

	{
		if (pRender == nullptr) return RESULT_BAD;
		if (pFeatureP == nullptr) return RESULT_BAD;
		if (pSymbol == nullptr) return RESULT_BAD;
		if (pLayerSet == nullptr) return RESULT_BAD;
		if (m_pMakeGeo == nullptr) return RESULT_BAD;

		if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

		ID2D1SolidColorBrush		*pLayerColor = nullptr;
		bool bPolarity = false;// pLayerSet->bPolarity == true ? pFeatureP->m_bPolarity : !pFeatureP->m_bPolarity;
		_GetBrush(bPolarity, pFeatureP->m_bHighlight, pFeatureP->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);
		

		if (bSave == TRUE)
		{
			vector<DrawFeatureGeo> vecDrawFeatureGeo;
			vecDrawFeatureGeo.resize(1);

			m_pMakeGeo->MakePadHole(pFeatureP, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
			int nCount = static_cast<int>(vecDrawFeatureGeo.size());


			for (int i = 0; i < nCount; i++)
			{
				if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
				if (pLayerColor == nullptr) return RESULT_BAD;

				pRender->FillGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor);
			}
		}
		else
		{
			if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
			{
				m_pMakeGeo->MakePadHole(pFeatureP, pRotMirrArr, &pFeatureP->m_pGeoObject);
			}

			if (pLayerColor == nullptr)	return RESULT_BAD;
			if (pFeatureP->m_pGeoObject == nullptr) return RESULT_BAD;

			pRender->FillGeometry(pFeatureP->m_pGeoObject, pLayerColor);


		}

		return RESULT_GOOD;
	}




ID2D1PathGeometry* CDrawFunction::_DrawPadUserdefined(IN ID2D1RenderTarget* pRender, IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate,
	IN const UserSetInfo_t *pUserSetInfo, IN const BOOL &bRedrawSelect, IN const RECTD &drtViewRect_mm, IN const BOOL &bSave)
{
	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	if (pSymbol == nullptr) return nullptr;
	if (pSymbol->m_pUserSymbol == nullptr) return nullptr;

	CFeatureFile *pChildFeatureFile = &(pSymbol->m_pUserSymbol->m_FeatureFile);
	if (pChildFeatureFile == NULL)
		return nullptr;

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	//UserDefine Merge 추가
	vector<DrawFeatureGeo*> vecDrawFeatureGeo;


	CFeature		*pFeatureTmp = NULL;
	CSymbol			*pSymbolTmp = NULL;
	for (int feno = 0; feno < pChildFeatureFile->m_arrFeature.GetSize(); feno++)	//feature drawing
	{
		pFeatureTmp = pChildFeatureFile->m_arrFeature.GetAt(feno);

		if (pFeatureP->m_bPolarity != pFeatureTmp->m_bPolarity)
		{
			if (!pFeatureP->m_bPolarity || !pFeatureTmp->m_bPolarity)
				pFeatureTmp->m_bPolarity = false;// pObj->m_bPolarity;
		}

		//UserDefine Merge 추가
		DrawFeatureGeo* pFeatureGeo = new DrawFeatureGeo();

		switch (pFeatureTmp->m_eType)
		{
		case FeatureType::L:		//Line
		{
			CFeatureL* pFeatureL = (CFeatureL*)pFeatureTmp;
			pSymbol = pFeatureL->m_pSymbol;
			if (bRedrawSelect == static_cast<BOOL>(pFeatureL->m_bHighlight))
			{
				_DrawLineFeature(pRender, pFeatureL, pSymbol, pLayerSet, pRotMirrArr, TRUE, bSave);

				//UserDefine Merge 추가
				pFeatureGeo->bPolarity = pFeatureTmp->m_bPolarity;
				pFeatureGeo->pGeo = pFeatureL->m_pGeoObject;
				pFeatureL->m_pGeoObject = nullptr;
			}
		}
		break;
		case FeatureType::P:		//Pad
		{
			CFeatureP* pFeatureP = (CFeatureP*)pFeatureTmp;
			pSymbol = pFeatureP->m_pSymbol;
			if (bRedrawSelect == static_cast<BOOL>(pFeatureP->m_bHighlight))
			{
				_DrawPadFeature(pRender, pFeatureP, pSymbol, pLayerSet, pFontArr, pRotMirrArr, TRUE, pUserSetInfo, bRedrawSelect, bSave);

				//UserDefine Merge 추가
				pFeatureGeo->bPolarity = pFeatureTmp->m_bPolarity;
				pFeatureGeo->pGeo = pFeatureP->m_pGeoObject;
				pFeatureP->m_pGeoObject = nullptr;
			}
		}
		break;
		case FeatureType::A:		//Arc								
		{
			CFeatureA* pFeatureA = (CFeatureA*)pFeatureTmp;
			pSymbol = pFeatureA->m_pSymbol;
			if (bRedrawSelect == static_cast<BOOL>(pFeatureA->m_bHighlight))
			{
				_DrawArcFeature(pRender, pFeatureA, pSymbol, pLayerSet, pRotMirrArr, TRUE, bSave);

				//UserDefine Merge 추가
				pFeatureGeo->bPolarity = pFeatureTmp->m_bPolarity;
				pFeatureGeo->pGeo = pFeatureA->m_pGeoObject;
				pFeatureA->m_pGeoObject = nullptr;
			}
		}
		break;
		case FeatureType::T:		//Text			
		{
			CFeatureT* pFeatureT = (CFeatureT*)pFeatureTmp;
			pSymbol = pFeatureT->m_pSymbol;

			pRotMirrArr->InputData(pFeatureT->m_dX, pFeatureT->m_dY, pFeatureT->m_eOrient);

			_DrawTextFeature(pRender, pFeatureT, pSymbol, pLayerSet, pFontArr, pRotMirrArr, TRUE, bSave);

			pRotMirrArr->RemoveTail();
		}
		break;
		case FeatureType::S:		//Surface	
		{
			CFeatureS* pFeatureS = (CFeatureS*)pFeatureTmp;
 			pSymbol = pFeatureS->m_pSymbol;
 			_DrawSurfaceFeature(pRender, pFeatureS, pSymbol, pLayerSet, pRotMirrArr, TRUE, bRedrawSelect, bSave, drtViewRect_mm);

			//UserDefine Merge 추가
			pFeatureGeo->bPolarity = pFeatureTmp->m_bPolarity;
			pFeatureGeo->pGeo = pFeatureS->m_pGeoObject;
			pFeatureS->m_pGeoObject = nullptr;
		}
			break;
			//case FeatureType::B:		//Barcode								
			//	pSymbolTmp = ((CFeatureB*)pFeatureTmp)->m_pSymbol;
			//	break;
		default:	//unknown
			break;
		}

		if (pFeatureGeo->pGeo == nullptr)
		{
			delete pFeatureGeo;
		}
		else
		{
			vecDrawFeatureGeo.push_back(pFeatureGeo);
		}
	}

	ID2D1PathGeometry		*pGeoObject = nullptr;
	m_pMakeGeo->CombineFeature(vecDrawFeatureGeo, &pGeoObject);

	for (auto pDrawFeatureGeo : vecDrawFeatureGeo)
	{
		if (pDrawFeatureGeo != nullptr)
		{
			delete pDrawFeatureGeo;
			pDrawFeatureGeo = nullptr;
		}
	}

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return pGeoObject;
}


//////////////////////////////////////////////////////////////////////////
//Arc Feature

UINT32 CDrawFunction::_DrawArcFeature(IN ID2D1RenderTarget* pRender, IN CFeatureA* pFeatureA, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pFeatureA == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (m_pMakeGeo == nullptr) return RESULT_BAD;

	if (pFeatureA->m_bShow == FALSE) return RESULT_GOOD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureA->m_bPolarity : !pFeatureA->m_bPolarity;
	_GetBrush(bPolarity, pFeatureA->m_bHighlight, pFeatureA->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

	if (pLayerColor == nullptr)
	{
		return RESULT_BAD;
	}

	float fPenWidth = 1.0;
	ID2D1StrokeStyle *pStrokeStyle = nullptr;
	UINT32 nRet = GetArcDrawInfo(pSymbol, fPenWidth, &pStrokeStyle);

	vector<DrawFeatureGeo> vecDrawFeatureGeo;
	vecDrawFeatureGeo.resize(1);

	if (bSave == TRUE)
	{
		m_pMakeGeo->MakeArc(pFeatureA, pRotMirrArr, &vecDrawFeatureGeo[0].pGeo);
		int nCount = static_cast<int>(vecDrawFeatureGeo.size());
		for (int i = 0; i < nCount; i++)
		{
			if (vecDrawFeatureGeo[i].pGeo == nullptr) return RESULT_BAD;
			if (pLayerColor == nullptr) return RESULT_BAD;
			if (pStrokeStyle == nullptr) return RESULT_BAD;

			pRender->DrawGeometry(vecDrawFeatureGeo[i].pGeo, pLayerColor, fPenWidth, pStrokeStyle);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureA->m_pGeoObject == nullptr)
		{
			m_pMakeGeo->MakeArc(pFeatureA, pRotMirrArr, &pFeatureA->m_pGeoObject);
		}

		if (pLayerColor == nullptr)	return RESULT_BAD;
		if (pFeatureA->m_pGeoObject == nullptr) return RESULT_BAD;

		pRender->DrawGeometry(pFeatureA->m_pGeoObject, pLayerColor, fPenWidth, pStrokeStyle);
	}


	return RESULT_GOOD;
}


//////////////////////////////////////////////////////////////////////////
	//Text Feature
UINT32 CDrawFunction::_DrawTextFeature(IN ID2D1RenderTarget* pRender, IN CFeatureT* pFeatureT, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bSave)
{
	if (pFeatureT == nullptr)return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (pFontArr == nullptr) return RESULT_BAD;

	int nFontCount = static_cast<int>(pFontArr->GetCount());
	if ( nFontCount <= 0 ) return RESULT_BAD;

	//Select Font
	CStandardFont* pFont = nullptr;
	for (int i = 0; i < nFontCount; i++)
	{
		CStandardFont* pFontTemp = pFontArr->GetAt(i);
		
		if (pFontTemp->m_strFontName == pFeatureT->m_strFont)
		{
			pFont = pFontTemp;
			break;
		}
	}
	if (pFont == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecFeatureData;
	vector<DrawFeatureGeo> vecDrawFeatureGeo;
	vector<ID2D1PathGeometry *> vecGeo;

	if (bSave == TRUE)
	{
		vecFeatureData = CDrawMakePoint::MakeText(pFeatureT, pFont, pSymbol, pRotMirrArr);

		int nCount = static_cast<int>(vecFeatureData.size());
		vecDrawFeatureGeo.resize(nCount);
		for (int i = 0; i < nCount; i++)
		{
			_SetGeometry(vecFeatureData[i].vecPointData, &vecDrawFeatureGeo[i].pGeo, TRUE);
			vecGeo.push_back(vecDrawFeatureGeo[i].pGeo);
		}

		_DrawText(pRender, pFont, pFeatureT, pLayerSet, vecGeo, bSave);
		
	}
	else
	{
		if (bNeedUpdate == TRUE || static_cast<int>(pFeatureT->m_vecGeoObject.size()) == 0)
		{
			vecFeatureData = CDrawMakePoint::MakeText(pFeatureT, pFont, pSymbol, pRotMirrArr);

			int nGeoCount = static_cast<int>(pFeatureT->m_vecGeoObject.size());
			for (int i = 0; i < nGeoCount; i++)
			{
				SafeRelease(&pFeatureT->m_vecGeoObject[i]);
			}

			int nCount = static_cast<int>(vecFeatureData.size());
			pFeatureT->m_vecGeoObject.resize(nCount);
			for (int i = 0; i < nCount; i++)
			{
				_SetGeometry(vecFeatureData[i].vecPointData, &pFeatureT->m_vecGeoObject[i], TRUE);
			}
		}

		int nCount = static_cast<int>(pFeatureT->m_vecGeoObject.size());
		for (int i = 0; i < nCount; i++)
		{
			vecGeo.push_back(pFeatureT->m_vecGeoObject[i]);
		}

		_DrawText(pRender, pFont, pFeatureT, pLayerSet, vecGeo, bSave);
	
	}
	
	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawText(IN ID2D1RenderTarget* pRender, IN const CStandardFont* pFont, IN CFeatureT *pFeatureT,
	IN const LayerSet_t* pLayerSet, IN vector<ID2D1PathGeometry *> &vecGeo, IN const BOOL &bSave)
{
	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	ID2D1StrokeStyle *pStrokeStyle = nullptr;

	int nGeoIndex = 0;
	int nTextLength = pFeatureT->m_strText.GetLength();
	for (int i = 0; i < nTextLength; i++)
	{
		CString strChar = pFeatureT->m_strText.Mid(i, 1);



		wchar_t *pCharTemp = T2W(strChar.GetBuffer());
		wchar_t pChar = pCharTemp[0];

		//Select Font Char
		int nFontCharCount = static_cast<int>(pFont->m_arrFontChar.GetCount());
		CFontChar* pFontChar = nullptr;
		for (int nFont = 0; nFont < nFontCharCount; nFont++)
		{
			CFontChar* pFontCharTemp = pFont->m_arrFontChar.GetAt(nFont);

			if (pChar == pFontCharTemp->m_Char)
			{
				pFontChar = pFontCharTemp;
				break;
			}
		}

		if (pFontChar == nullptr)
			continue;

		int nFontLineCount = static_cast<int>(pFontChar->m_arrFontLine.GetCount());
		for (int nFontLine = 0; nFontLine < nFontLineCount; nFontLine++)
		{
			CFontLine* pFontLine = pFontChar->m_arrFontLine.GetAt(nFontLine);
			if (pFontLine == nullptr) continue;

			double dScaleLineWidth = pFontLine->m_dWidth * pFeatureT->m_dWidthFactor;

			bool bPolarity = pLayerSet->bPolarity == true ? pFeatureT->m_bPolarity : !pFeatureT->m_bPolarity;
			_GetBrush(bPolarity, pFeatureT->m_bHighlight, pFeatureT->m_bPreHighlight, pLayerSet->color, &pLayerColor, bSave);

			if (pFontLine->m_bShape == true)
			{//Round
				pStrokeStyle = m_pStrokeStyle_r;
			}
			else
			{//Square
				pStrokeStyle = m_pStrokeStyle_s;
			}

			if (static_cast<int>(vecGeo.size()) > nGeoIndex &&
				vecGeo[nGeoIndex] != nullptr)
			{
				pRender->DrawGeometry(vecGeo[nGeoIndex], pLayerColor, DoubleToFloat(dScaleLineWidth), pStrokeStyle);
				nGeoIndex++;
			}
		}
	}

	return RESULT_GOOD;
}

//////////////////////////////////////////////////////////////////////////
//Surface Feature
UINT32 CDrawFunction::_DrawSurfaceFeature(IN ID2D1RenderTarget* pRender, IN CFeatureS* pFeatureS, IN const CSymbol* pSymbol,
	IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bRedrawSelect, IN const BOOL &bSave, IN RECTD drtViewRect_mm)
{
	//Surface는 선택된 것 다시그릴때에 다시 그리지 않는다.
	if (bRedrawSelect == TRUE) return RESULT_GOOD;

	if (pFeatureS == nullptr)return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;

	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bPolarity = pLayerSet->bPolarity == true ? pFeatureS->m_bPolarity : !pFeatureS->m_bPolarity;
	//_GetBrush_Surface(bPolarity, pObOe->m_bHighlight, pObOe->m_bPreHighlight, pLayerSet->color, &pLayerColor);
	_GetBrush_Surface(bPolarity, FALSE, FALSE, pLayerSet->color, &pLayerColor, bSave);

	{//ObOe First element Check : Is island
		int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
		if (nObOeCount <= 0) return RESULT_BAD;

		CObOe* pObOe = pFeatureS->m_arrObOe.GetAt(0);
		if (pObOe != nullptr)
		{
			if (nObOeCount > 0 && pObOe->m_bPolyType == false)
				return RESULT_BAD;
		}
	}

	//
	vector<MakeFeatureData> vecGeoDraw;
	vector<DrawFeatureGeo> vecDrawFeatureGeo;
	if (bSave == TRUE)
	{
		vecGeoDraw = CDrawMakePoint::MakeSurface(pFeatureS, pRotMirrArr, drtViewRect_mm);
		int nCount = static_cast<int>(vecGeoDraw.size());

		vecDrawFeatureGeo.resize(nCount);
		for (int i = 0; i < nCount; i++)
		{
			vecDrawFeatureGeo[i].bIsUsed = vecGeoDraw[i].bIsUsed;
			vecDrawFeatureGeo[i].bPolarity = vecGeoDraw[i].bPolarity;

			if (!vecGeoDraw[i].bIsUsed)
				continue;

			_SetGeometry(vecGeoDraw[i].vecPointData, &vecDrawFeatureGeo[i].pGeo);

			CObOe* pObOe = pFeatureS->m_arrObOe.GetAt(i);
			if (pObOe == nullptr) continue;		
		}

	}
	else
	{
		//Check Need Make 
		//하나라도 안만들어져 있는것 이 있다면 다시 만들기
		BOOL bNeedMake = FALSE;
		int nSurfaceCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
		for (int i = 0; i < nSurfaceCount; i++)
		{
			CObOe* pObOe = pFeatureS->m_arrObOe.GetAt(i);
			if (pObOe == nullptr) continue;

			if (pObOe->m_pGeoObject == nullptr)
			{
				bNeedMake = TRUE;
				break;
			}
		}

		if (bNeedMake == TRUE || bNeedUpdate == TRUE)
		{
			vecGeoDraw = CDrawMakePoint::MakeSurface(pFeatureS, pRotMirrArr, drtViewRect_mm);
			int nCount = static_cast<int>(vecGeoDraw.size());

			for (int i = 0; i < nCount; i++)
			{
				CObOe* pObOe = pFeatureS->m_arrObOe.GetAt(i);
				if (pObOe == nullptr) continue;

				SafeRelease(&pObOe->m_pGeoObject);

				if (!vecGeoDraw[i].bIsUsed)				
					continue;
				
				_SetGeometry(vecGeoDraw[i].vecPointData, &pObOe->m_pGeoObject);
			}

			_MergePointGeo(vecGeoDraw, vecDrawFeatureGeo);
		}
	}	

	HRESULT hr = S_FALSE;

	ID2D1PathGeometry *pGeoS = nullptr;
	if (bSave == TRUE)
	{
		UINT32 nRet = m_pMakeGeo->CombineFeature(vecDrawFeatureGeo, &pGeoS);
		if (nRet == RESULT_GOOD && pGeoS != nullptr && pLayerColor != nullptr)
		{
			pRender->FillGeometry(pGeoS, pLayerColor);
		}
	}
	else
	{
		if (bNeedUpdate == TRUE || pFeatureS->m_pGeoObject == nullptr)
		{
			//_CombineSurface(pFeatureS, bNeedUpdate);
			m_pMakeGeo->CombineFeature(vecDrawFeatureGeo, &pFeatureS->m_pGeoObject);
		}

		if (pLayerColor != nullptr && pFeatureS->m_pGeoObject != nullptr)
		{
			pRender->FillGeometry(pFeatureS->m_pGeoObject, pLayerColor);
		}

		//SelectDraw
		int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
		for (int i = 0; i < nObOeCount; i++)
		{
			CObOe* pObOe = pFeatureS->m_arrObOe.GetAt(i);
			if (pObOe == nullptr) continue;
			if (pObOe->m_bHighlight == false && pObOe->m_bPreHighlight == false) continue;

			bool bPolarity = pLayerSet->bPolarity == true ? pFeatureS->m_bPolarity : !pFeatureS->m_bPolarity;
			bool bPolarityObOe = bPolarity == true ? pObOe->m_bPolyType : !pObOe->m_bPolyType;
			_GetBrush_Surface(bPolarityObOe, pObOe->m_bHighlight, pObOe->m_bPreHighlight, pLayerSet->color, &pLayerColor);

			if (pLayerColor == nullptr || pObOe->m_pGeoObject == nullptr) continue;

			pRender->FillGeometry(pObOe->m_pGeoObject, pLayerColor);
		}
		
	}

	int nCount = static_cast<int>(vecDrawFeatureGeo.size());
	for (int i = 0; i < nCount; i++)
	{
		SafeRelease(&vecDrawFeatureGeo[i].pGeo);
	}

	SafeRelease(&pGeoS);

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_CombineSurface(IN CFeatureS* pFeatureS, IN const BOOL &bNeedUpdate)
{
	if (pFeatureS == nullptr) return RESULT_BAD;

	HRESULT hr = S_FALSE;



	ID2D1PathGeometry *pLastGeo_Temp = nullptr;

	int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
	if (nObOeCount == 0) return RESULT_BAD;

	if (nObOeCount > 1)
	{//ObOe가 여러개일 경우
		D2D1_COMBINE_MODE emCombineMode;
		ID2D1GeometrySink *pSink_Temp[2] = { nullptr, };
		ID2D1PathGeometry *pGeo_Temp[2] = { nullptr, };
		m_pFactory->CreatePathGeometry(&pGeo_Temp[0]);
		m_pFactory->CreatePathGeometry(&pGeo_Temp[1]);
		
		int nSepIndex = 0;
		for (int i = 0; i < nObOeCount; i++)
		{
			CObOe* pObOe = pFeatureS->m_arrObOe.GetAt(i);
			if (pObOe == nullptr) continue;
			if (pObOe->m_pGeoObject == nullptr) continue;

			if (pObOe->m_bPolyType == TRUE)
			{
				emCombineMode = D2D1_COMBINE_MODE_UNION;
			}
			else
			{
				emCombineMode = D2D1_COMBINE_MODE_EXCLUDE;
			}

			nSepIndex = i % 2;
			if (nSepIndex == 0)
			{
				hr = pGeo_Temp[nSepIndex]->Open(&pSink_Temp[nSepIndex]);
				if (!SUCCEEDED(hr))
					continue;
				if (i == 0)
				{
					hr = pObOe->m_pGeoObject->CombineWithGeometry(pObOe->m_pGeoObject, (D2D1_COMBINE_MODE)D2D1_COMBINE_MODE_UNION, NULL, NULL, pSink_Temp[nSepIndex]);
				}
				else
				{
					hr = pGeo_Temp[1]->CombineWithGeometry(pObOe->m_pGeoObject, (D2D1_COMBINE_MODE)emCombineMode, NULL, NULL, pSink_Temp[nSepIndex]);

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

				hr = pGeo_Temp[0]->CombineWithGeometry(pObOe->m_pGeoObject, (D2D1_COMBINE_MODE)emCombineMode, NULL, NULL, pSink_Temp[nSepIndex]);

				pSink_Temp[nSepIndex]->Close();
				SafeRelease(&pSink_Temp[nSepIndex]);
				if (!SUCCEEDED(hr))
					continue;

				SafeRelease(&pGeo_Temp[0]);
				m_pFactory->CreatePathGeometry(&pGeo_Temp[0]);

				pLastGeo_Temp = pGeo_Temp[nSepIndex];
			}


		}


		if (pFeatureS->m_pGeoObject != nullptr)
		{
			pFeatureS->m_pGeoObject->Release();
			pFeatureS->m_pGeoObject = nullptr;
		}
		m_pFactory->CreatePathGeometry(&pFeatureS->m_pGeoObject);
		ID2D1GeometrySink *pSink_FeatureS = NULL;

		//
		hr = pFeatureS->m_pGeoObject->Open(&pSink_FeatureS);
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
			hr = pLastGeo_Temp->CombineWithGeometry(pLastGeo_Temp, D2D1_COMBINE_MODE_UNION, NULL, NULL, pSink_FeatureS);
		}


		pSink_FeatureS->Close();
		SafeRelease(&pSink_FeatureS);

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
	else
	{//ObOe가 한개 일경우
		SafeRelease(&pFeatureS->m_pGeoObject);

		CObOe* pObOe = pFeatureS->m_arrObOe.GetAt(0);
		if (pObOe == nullptr) return RESULT_BAD;

		m_pFactory->CreatePathGeometry(&pFeatureS->m_pGeoObject);
		ID2D1GeometrySink *pSink_FeatureS = NULL;


		hr = pFeatureS->m_pGeoObject->Open(&pSink_FeatureS);
		if (!SUCCEEDED(hr)) return RESULT_BAD;

		hr = pObOe->m_pGeoObject->CombineWithGeometry(pObOe->m_pGeoObject, D2D1_COMBINE_MODE_UNION, NULL, NULL, pSink_FeatureS);

		pSink_FeatureS->Close();
		SafeRelease(&pSink_FeatureS);

		if (!SUCCEEDED(hr)) return RESULT_BAD;
	}

	return RESULT_GOOD;
}

//Surface가 많을 때 Combine하는 시간이 오래 걸림
//Combine 하는 시간을 줄려주기 위하여 ObOe의 polarity가 연속적이라면, Merge하여 준다.
UINT32 CDrawFunction::_MergePointGeo(vector<MakeFeatureData> &vecFeaureSubData, vector<DrawFeatureGeo> &vecDrawFeatureGeo)
{
	vecDrawFeatureGeo.clear();


	int nDataCount = static_cast<int>(vecFeaureSubData.size());
	if (nDataCount <= 0) return RESULT_BAD;
		
	bool bPrePolarity = true;
	MakeFeatureData stTempMerge;
	vector<MakeFeatureData> vecMergePoint;
	int nStartIndex = 1;
	for (int nData = nStartIndex; nData < nDataCount; nData++)
	{
		if (bPrePolarity == vecFeaureSubData[nData].bPolarity)
		{
			stTempMerge.bPolarity = bPrePolarity;
			int nPointCount = static_cast<int>(vecFeaureSubData[nData].vecPointData.size());
			for (int nPoint = 0; nPoint < nPointCount; nPoint++)
			{
				stTempMerge.vecPointData.push_back(vecFeaureSubData[nData].vecPointData[nPoint]);
			}
		}
		else
		{
			if (nData != nStartIndex)
			{//처음은 비어있는 TempMerge이기 때문에 실행하지 않는다.
				vecMergePoint.push_back(stTempMerge);
			}

			stTempMerge.Clear();

			stTempMerge.bPolarity = vecFeaureSubData[nData].bPolarity;
			int nPointCount = static_cast<int>(vecFeaureSubData[nData].vecPointData.size());
			for (int nPoint = 0; nPoint < nPointCount; nPoint++)
			{
				stTempMerge.vecPointData.push_back(vecFeaureSubData[nData].vecPointData[nPoint]);
			}
		}

		bPrePolarity = vecFeaureSubData[nData].bPolarity;
	}

	if (stTempMerge.vecPointData.size() != 0)
	{
		vecMergePoint.push_back(stTempMerge);
	}

	
	int nMergeCount = static_cast<int>(vecMergePoint.size());

	vecDrawFeatureGeo.resize(nMergeCount + 1);

	vecDrawFeatureGeo[0].bPolarity = vecFeaureSubData[0].bPolarity;
	_SetGeometry(vecFeaureSubData[0].vecPointData, &vecDrawFeatureGeo[0].pGeo);

	for (int nData = 0; nData < nMergeCount; nData++)
	{
		vecDrawFeatureGeo[nData+1].bPolarity = vecMergePoint[nData].bPolarity;
		_SetGeometry(vecMergePoint[nData].vecPointData, &vecDrawFeatureGeo[nData + 1].pGeo);

		
	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DrawMask(IN ID2D1RenderTarget* pRender, IN const MaskType &eMaskType, IN CFeature* pFeature, IN const CSymbol* pSymbol, IN const LayerSet_t* pLayerSet, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const BOOL &bRedrawSelect)
{
	if (pRender == nullptr) return RESULT_BAD;
	if (pFeature == nullptr) return RESULT_BAD;
	if (pSymbol == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;

	ID2D1PathGeometry		*pGeoObject = nullptr;
	ID2D1SolidColorBrush		*pLayerColor = nullptr;
	bool bHighlight = false;
	bool bPreHighlight = false;
	if (pFeature->m_eType == FeatureType::P)
	{
		CFeatureP *pFeatureP = (CFeatureP *)pFeature;
		if (pFeatureP->m_bShow == FALSE) return RESULT_GOOD;

		if (bNeedUpdate == TRUE || pFeatureP->m_pGeoObject == nullptr)
		{
			if (pSymbol->m_eSymbolName == SymbolName::s)
			{
				_MakeMask_Square(pFeatureP, pSymbol, pRotMirrArr);
			}
			else if (pSymbol->m_eSymbolName == SymbolName::rect)
			{
				_MakeMask_Rect(pFeatureP, pSymbol, pRotMirrArr);
			}
			
		}

		bHighlight = pFeatureP->m_bHighlight;
		bPreHighlight = pFeatureP->m_bPreHighlight;
		pGeoObject = pFeatureP->m_pGeoObject;
	}
	
	if (pGeoObject == nullptr) return RESULT_BAD;

	float fPenWidth = 1.0;
	if (bHighlight == true)
	{
		pLayerColor = m_pLayerColor_Mask_Select;
	}
	else if (bPreHighlight == true)
	{
		pLayerColor = m_pLayerColor_Mask_PreSelect;
	}
	else
	{
		pLayerColor = m_pLayerColor_Mask_Transparent;
	}

	if (pLayerColor == nullptr)	return RESULT_BAD;

	D2D1_MATRIX_3X2_F matTransform;
	pRender->GetTransform(&matTransform);
	float fScale = fabs(matTransform.m11);
	if (fScale == 0.f)
	{
		fScale = static_cast<float>(fabs(matTransform.m12));
	}

	if (eMaskType == MaskType::enumType_Align)
	{
		pRender->DrawGeometry(pGeoObject, m_pLayerColor_Mask, 1 / fScale, m_pStrokeStyle_Dash);
		pRender->FillGeometry(pGeoObject, pLayerColor);
	}
	else if (eMaskType == MaskType::enumType_Mask)
	{
		pRender->DrawGeometry(pGeoObject, m_pLayerColor_Mask, 1 / fScale, m_pStrokeStyle_Dash_Dot);
		pRender->FillGeometry(pGeoObject, pLayerColor);
	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_MakeMask_Rect(IN CFeature* pFeature, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeature == nullptr)return RESULT_BAD;
	if (pSymbol == nullptr)return RESULT_BAD;

	if (pFeature->m_eType != FeatureType::P) return RESULT_BAD;

	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	double dHalfWidth;
	double dHalfHeight;
	
	dHalfWidth = pSymbol->m_vcParams[0] / 2.0;
	dHalfHeight = pSymbol->m_vcParams[1] / 2.0;

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;
	stGeoDrawTemp.SetBasePoint(-1.0f * dHalfWidth, -1.0f *  dHalfHeight);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.SetLine(dHalfWidth, -1.0f *  dHalfHeight);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.SetLine(dHalfWidth, dHalfHeight);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.SetLine(-1.0f *  dHalfWidth, dHalfHeight);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.SetLine(-1.0f * dHalfWidth, -1.0f * dHalfHeight);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	vector<GEODRAW> vecRotateData = CDrawMakePoint::SetMirrorRotate(vecGeoDraw, pRotMirrArr/*, pSymbol->m_dRotatedDeg*/);

	_SetGeometry(vecRotateData, &pFeatureP->m_pGeoObject);

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return RESULT_GOOD;
}


UINT32 CDrawFunction::_MakeMask_Square(IN CFeature* pFeature, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeature == nullptr)return RESULT_BAD;
	if (pSymbol == nullptr)return RESULT_BAD;

	if (pFeature->m_eType != FeatureType::P) return RESULT_BAD;

	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	double dHalfWidth;

	dHalfWidth = pSymbol->m_vcParams[0] / 2.0;

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;
	stGeoDrawTemp.SetBasePoint(-1.0f * dHalfWidth, -1.0f *  dHalfWidth);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.SetLine(dHalfWidth, -1.0f *  dHalfWidth);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.SetLine(dHalfWidth, dHalfWidth);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.SetLine(-1.0f *  dHalfWidth, dHalfWidth);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.SetLine(-1.0f * dHalfWidth, -1.0f * dHalfWidth);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	vector<GEODRAW> vecRotateData = CDrawMakePoint::SetMirrorRotate(vecGeoDraw, pRotMirrArr/*, pSymbol->m_dRotatedDeg*/);

	_SetGeometry(vecRotateData, &pFeatureP->m_pGeoObject);

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::OnMake_Layer(IN ID2D1RenderTarget* pRender, IN CLayer* pLayer, IN const LayerSet_t* pLayerSet,
	CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo,
	eDrawLevel drawLevel, IN const BOOL &bRedrawSelect)
{
	m_bIsDrawMode = false;
	m_eDrawLevel = drawLevel;

	if (pRender == nullptr) return RESULT_BAD;
	if (pLayer == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;
	if (pUserSetInfo == nullptr) return RESULT_BAD;

	int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetSize());

	CFeature*	pFeature = nullptr;
	CSymbol*	pSymbol = nullptr;

	bool bFindAlignSymbol = false;

	_MakeRenderItem(pRender);

	for (int feno = 0; feno < nFeatureCount; feno++)
	{
		pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(feno);
		if (pFeature == nullptr)
			continue;

		if (drawLevel == eDrawLevel::eSymbolLv)
		{
			CSymbol* pSym = ((CFeatureP*)pFeature)->m_pSymbol;
			if (pSym == nullptr || pSym->m_pUserSymbol == nullptr)
				continue;

			if (pSym->m_eSymbolName != SymbolName::userdefined ||
				pSym->m_pUserSymbol->m_strSymbolName.CompareNoCase(ALIGN_SYMBOL1_TXT) != 0)
				continue;

			bFindAlignSymbol = true;
		}

		switch (pFeature->m_eType)
		{
			case FeatureType::Z:
			{
				CFeatureZ* pFeatureZ = (CFeatureZ*)pFeature;
				pSymbol = pFeatureZ->m_pSymbol;

				_DrawTpFeature(pRender, pFeatureZ, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, TRUE);
;			}
			break;
			case FeatureType::L:		//Line
			{
				CFeatureL* pFeatureL = (CFeatureL*)pFeature;
				pSymbol = pFeatureL->m_pSymbol;
				
				_DrawLineFeature(pRender, pFeatureL, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, TRUE);
			}
			break;

			case FeatureType::P:		//Pad
			{
				CFeatureP* pFeatureP = (CFeatureP*)pFeature;
				pSymbol = pFeatureP->m_pSymbol;
			
				_DrawPadFeature(pRender, pFeatureP, pSymbol, pLayerSet, pFontArr, pRotMirrArr, bNeedUpdate, pUserSetInfo, bRedrawSelect, TRUE);
			}
			break;

			case FeatureType::A:		//Arc								
			{
				CFeatureA* pFeatureA = (CFeatureA*)pFeature;
				pSymbol = pFeatureA->m_pSymbol;
			
				_DrawArcFeature(pRender, pFeatureA, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, TRUE);
			}
			break;

			case FeatureType::S:		//Surface	
			{				
				CFeatureS* pFeatureS = (CFeatureS*)pFeature;
				pSymbol = pFeatureS->m_pSymbol;
				_DrawSurfaceFeature(pRender, pFeatureS, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bRedrawSelect, TRUE);
			}
			break;

			case FeatureType::T:		//Text			
			{
				CFeatureT* pFeatureT = (CFeatureT*)pFeature;
				pSymbol = pFeatureT->m_pSymbol;

				pRotMirrArr->InputData(pFeatureT->m_dX, pFeatureT->m_dY, pFeatureT->m_eOrient);

				_DrawTextFeature(pRender, pFeatureT, pSymbol, pLayerSet, pFontArr, pRotMirrArr, bNeedUpdate, TRUE);

				pRotMirrArr->RemoveTail();
			}
			break;

			case FeatureType::B:		//Barcode								
			{
			}
			break;
		}
	}

	if (!bFindAlignSymbol && drawLevel == eDrawLevel::eSymbolLv)
	{
		for (int feno = 0; feno < nFeatureCount; feno++)
		{
			pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(feno);
			if (pFeature == nullptr)
				continue;

			bool bFindFeature = false;
			for (int txtno = 0; txtno < pFeature->m_arrAttributeTextString.GetSize(); txtno++)
			{
				CAttributeTextString* pTextString = pFeature->m_arrAttributeTextString.GetAt(txtno);
				if (pTextString == nullptr || pTextString->m_strText.CompareNoCase(ALIGN_SYMBOL2_TXT) != 0)
					continue;

				bFindFeature = true;
				break;
			}

			if (!bFindFeature)
				continue;

			switch (pFeature->m_eType)
			{
			case FeatureType::Z:
			{
				CFeatureZ* pFeatureZ = (CFeatureZ*)pFeature;
				pSymbol = pFeatureZ->m_pSymbol;

				_DrawTpFeature(pRender, pFeatureZ, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, TRUE);
			}
			break;
			case FeatureType::L:		//Line
			{
				CFeatureL* pFeatureL = (CFeatureL*)pFeature;
				pSymbol = pFeatureL->m_pSymbol;

				_DrawLineFeature(pRender, pFeatureL, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, TRUE);
			}
			break;

			case FeatureType::P:		//Pad
			{
				CFeatureP* pFeatureP = (CFeatureP*)pFeature;
				pSymbol = pFeatureP->m_pSymbol;

				_DrawPadFeature(pRender, pFeatureP, pSymbol, pLayerSet, pFontArr, pRotMirrArr, bNeedUpdate, pUserSetInfo, bRedrawSelect, TRUE);
			}
			break;

			case FeatureType::A:		//Arc								
			{
				CFeatureA* pFeatureA = (CFeatureA*)pFeature;
				pSymbol = pFeatureA->m_pSymbol;

				_DrawArcFeature(pRender, pFeatureA, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, TRUE);
			}
			break;

			case FeatureType::S:		//Surface	
			{
				CFeatureS* pFeatureS = (CFeatureS*)pFeature;
				pSymbol = pFeatureS->m_pSymbol;
				_DrawSurfaceFeature(pRender, pFeatureS, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, bRedrawSelect, TRUE);
			}
			break;

			case FeatureType::T:		//Text			
			{
				CFeatureT* pFeatureT = (CFeatureT*)pFeature;
				pSymbol = pFeatureT->m_pSymbol;

				pRotMirrArr->InputData(pFeatureT->m_dX, pFeatureT->m_dY, pFeatureT->m_eOrient);

				_DrawTextFeature(pRender, pFeatureT, pSymbol, pLayerSet, pFontArr, pRotMirrArr, bNeedUpdate, TRUE);

				pRotMirrArr->RemoveTail();
			}
			break;

			case FeatureType::B:		//Barcode								
			{
			}
			break;
			}
		}
	}

	_DeleteRenderItem();

	return RESULT_GOOD;
}

RECTD CDrawFunction::GetViewRect_mm(IN ID2D1RenderTarget* pRender, const UserSetInfo_t *pUserSetInfo, IN const BOOL &bSaveMode)
{
	//Draw 시 Rotation과 Flip이 들어가 있을때 버그가 있음 : Draw 시 교집합하는 부분 주석처리
	//Save는 상관 없음.
	D2D1::Matrix3x2F matTransform;
	pRender->GetTransform(&matTransform);
	D2D1::Matrix3x2F matScale = D2D1::Matrix3x2F(matTransform.m11, matTransform.m12, matTransform.m21, matTransform.m22, 0, 0);

	float fScaleX = matTransform.m11;
	if (fScaleX == 0.f)
	{
		fScaleX = matTransform.m12;
	}

	float fScaleY = matTransform.m21;
	if (fScaleY == 0.f)
	{
		fScaleY = matTransform.m22;
	}
	RECTD drtDraw_mm;
	if (bSaveMode == TRUE)
	{
		D2D1_POINT_2F fptPan_mm = D2D1::Point2F(-1.0f*matTransform.dx / fScaleX, -1.0f * matTransform.dy / fScaleY);

		D2D1_POINT_2F fptSize = D2D1::Point2F(pRender->GetSize().width, pRender->GetSize().height);
		D2D1_POINT_2F fptSize_mm = D2D1::Point2F(fptSize.x / fScaleX, fptSize.y / fScaleY);

		drtDraw_mm = RECTD(fptPan_mm.x, fptPan_mm.y, fptPan_mm.x + fptSize_mm.x, fptPan_mm.y + fptSize_mm.y);
		drtDraw_mm.NormalizeRectD();
	}
	else
	{//Draw 모드

		D2D1_POINT_2F fptPan = D2D1::Point2F(matTransform.dx, matTransform.dy);
		D2D1_POINT_2F fptSize = D2D1::Point2F(pRender->GetSize().width, pRender->GetSize().height);
	
		//D2D1_POINT_2F fptPan_mm = CDrawFunction::DetansformPoint(fptPan, matTransform, pUserSetInfo);
		D2D1_POINT_2F fptPan_mm = CDrawFunction::DetansformPoint(D2D1::Point2F(0.f, 0.f), matTransform, pUserSetInfo);
		//D2D1_POINT_2F fptSize_mm = D2D1::Point2F(fptSize.x / fScaleX, fptSize.y / fScaleY); //CDrawFunction::DetansformPoint(fptSize, matTransform);

		D2D1::Matrix3x2F matScale = D2D1::Matrix3x2F(matTransform.m11, matTransform.m12, matTransform.m21, matTransform.m22, 0, 0);
		D2D1_POINT_2F fptSize_mm = CDrawFunction::DetansformPoint(fptSize, matScale, pUserSetInfo);
	
		drtDraw_mm = RECTD(fptPan_mm.x, fptPan_mm.y, fptPan_mm.x + fptSize_mm.x, fptPan_mm.y + fptSize_mm.y);
		
		drtDraw_mm.NormalizeRectD();
	}

	return drtDraw_mm;
}

UINT32 CDrawFunction::OnMake_Layer_V2(IN ID2D1RenderTarget* pRender, IN CLayer* pLayer, IN const LayerSet_t* pLayerSet,
	CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate,
	IN const UserSetInfo_t *pUserSetInfo, eDrawLevel drawLevel,
	IN const SaveMode eSaveMode, IN const RECTD &drtViewRect_mm, IN CString strLineArcWidth)
{
	m_bIsDrawMode = false;
	m_eDrawLevel = drawLevel;

	if (pRender == nullptr) return RESULT_BAD;
	if (pLayer == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;

	

//	RECTD drtViewRect_mm = GetViewRect_mm(pRender);

	int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetSize());

	CFeature*	pFeature = nullptr;
	CSymbol*	pSymbol = nullptr;

	bool bFindAlignSymbol = false;

	
	_MakeRenderItem(pRender);

	for (int feno = 0; feno < nFeatureCount; feno++)
	{
		pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(feno);
				
		//교집합 체크 : Draw 시간을 줄이기 위함
		{

			RECTD drtFeature;
			if (pRotMirrArr == nullptr)
			{
				drtFeature = pFeature->m_MinMaxRect;
			}
			else
			{
				pRotMirrArr->FinalPoint(&drtFeature.left, &drtFeature.top, pFeature->m_MinMaxRect.left, pFeature->m_MinMaxRect.top);
				pRotMirrArr->FinalPoint(&drtFeature.right, &drtFeature.bottom, pFeature->m_MinMaxRect.right, pFeature->m_MinMaxRect.bottom);
				drtFeature.NormalizeRectD();
			}

			if (drtFeature.IsIntersection(drtViewRect_mm) == FALSE)
			{
				//clLocker.unlock();
				continue;
			}
		}

		if (drawLevel == eDrawLevel::eSymbolLv)
		{
			CSymbol* pSym = ((CFeatureP*)pFeature)->m_pSymbol;
			if (pSym == nullptr || pSym->m_pUserSymbol == nullptr)
			{
				//clLocker.unlock();
				continue;
			}

			if (pSym->m_eSymbolName != SymbolName::userdefined ||
				pSym->m_pUserSymbol->m_strSymbolName.CompareNoCase(ALIGN_SYMBOL1_TXT) != 0)
			{
				//clLocker.unlock();
				continue;
			}

			bFindAlignSymbol = true;
		}
		
		
		switch (pFeature->m_eType)
		{
		case FeatureType::L:		//Line
		{
			CFeatureL* pFeatureL = (CFeatureL*)pFeature;
			pSymbol = pFeatureL->m_pSymbol;

			if (eSaveMode == SaveMode::SaveMode_All || eSaveMode == SaveMode::SaveMode_Line || eSaveMode == SaveMode::SaveMode_LineArc)
			{
				_DrawLineFeature(pRender, pFeatureL, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, TRUE);
			}
			else if (pFeatureL->m_bPolarity == true && (
				eSaveMode == SaveMode::SaveMode_Line_Pos_Sub || eSaveMode == SaveMode::SaveMode_LineArc_Pos_Sub ) )
			{//입력 받은 Line Width와 같은 것만 출력

				CString strSymbolWidth = pSymbol->m_strUserSymbol.Mid(1);
				if (strLineArcWidth.Compare(strSymbolWidth) == 0 )
				{
					_DrawLineFeature(pRender, pFeatureL, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, TRUE);
				}
			}
			else if (pFeatureL->m_bPolarity == false && (
				eSaveMode == SaveMode::SaveMode_Line_Neg_Sub || eSaveMode == SaveMode::SaveMode_LineArc_Neg_Sub))
			{//입력 받은 Line Width와 같은 것만 출력

				CString strSymbolWidth = pSymbol->m_strUserSymbol.Mid(1);
				if (strLineArcWidth.Compare(strSymbolWidth) == 0)
				{
					_DrawLineFeature(pRender, pFeatureL, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, TRUE);
				}
			}
		}
		break;

		case FeatureType::P:		//Pad
		{
			CFeatureP* pFeatureP = (CFeatureP*)pFeature;
			pSymbol = pFeatureP->m_pSymbol;

			if (eSaveMode == SaveMode::SaveMode_All || eSaveMode == SaveMode::SaveMode_Pad)
			{
				_DrawPadFeature(pRender, pFeatureP, pSymbol, pLayerSet, pFontArr, pRotMirrArr, bNeedUpdate, pUserSetInfo, FALSE, TRUE);
			}
		}
		break;

		case FeatureType::A:		//Arc								
		{
			CFeatureA* pFeatureA = (CFeatureA*)pFeature;
			pSymbol = pFeatureA->m_pSymbol;

			if (eSaveMode == SaveMode::SaveMode_All || eSaveMode == SaveMode::SaveMode_Arc|| eSaveMode == SaveMode::SaveMode_LineArc)
			{
				_DrawArcFeature(pRender, pFeatureA, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, TRUE);
			}
			else if (pFeatureA->m_bPolarity == true &&
				(eSaveMode == SaveMode::SaveMode_Arc_Pos_Sub || eSaveMode == SaveMode::SaveMode_LineArc_Pos_Sub))
			{//입력 받은 Line Width와 같은 것만 출력

				CString strSymbolWidth = pSymbol->m_strUserSymbol.Mid(1);
				if (strLineArcWidth.Compare(strSymbolWidth) == 0)
				{
					_DrawArcFeature(pRender, pFeatureA, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, TRUE);
				}
			}
			else if (pFeatureA->m_bPolarity == false && 
				(eSaveMode == SaveMode::SaveMode_Arc_Neg_Sub || eSaveMode == SaveMode::SaveMode_LineArc_Neg_Sub ))
			{//입력 받은 Line Width와 같은 것만 출력

				CString strSymbolWidth = pSymbol->m_strUserSymbol.Mid(1);
				if (strLineArcWidth.Compare(strSymbolWidth) == 0)
				{
					_DrawArcFeature(pRender, pFeatureA, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, TRUE);
				}
			}
		}
		break;

		case FeatureType::S:		//Surface	
		{
			CFeatureS* pFeatureS = (CFeatureS*)pFeature;
			pSymbol = pFeatureS->m_pSymbol;
			if (eSaveMode == SaveMode::SaveMode_All ||
				(pFeatureS->m_bPolarity == true && eSaveMode == SaveMode::SaveMode_Surface_Pos) ||
				(pFeatureS->m_bPolarity == false && eSaveMode == SaveMode::SaveMode_Surface_Neg))
			{
				_DrawSurfaceFeature(pRender, pFeatureS, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, FALSE, TRUE, drtViewRect_mm);
			}
		}
		break;

		case FeatureType::T:		//Text			
		{
// 			if (eSaveMode == SaveMode::SaveMode_All || eSaveMode == SaveMode::SaveMode_Text)
// 			{
// 				CFeatureT* pFeatureT = (CFeatureT*)pFeature;
// 				pSymbol = pFeatureT->m_pSymbol;
// 
// 				pRotMirrArr->InputData(pFeatureT->m_dX, pFeatureT->m_dY, pFeatureT->m_eOrient);
// 
// 				_DrawTextFeature(pRender, pFeatureT, pSymbol, pLayerSet, pFontArr, pRotMirrArr, bNeedUpdate);
// 
// 				pRotMirrArr->RemoveTail();
// 			}
		}
		break;

		case FeatureType::B:		//Barcode								
		{
		}
		break;
		}
	}
	
	

	if (!bFindAlignSymbol && drawLevel == eDrawLevel::eSymbolLv)
	{
		CString strAlignMark = ALIGN_SYMBOL2_TXT;
		CString strAlignFeature = _T("");

		if (pUserSetInfo->userSite == eUserSite::eDAEDUCK)
		{
			strAlignMark = ALIGN_SYMBOL3_TXT;
			strAlignFeature = ALIGN_FEATURE_TXT;
		}

		for (int feno = 0; feno < nFeatureCount; feno++)
		{
			pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(feno);
			if (pFeature == nullptr)
				continue;

			//교집합 체크 : Draw 시간을 줄이기 위함
			{

				RECTD drtFeature;
				if (pRotMirrArr == nullptr)
				{
					drtFeature = pFeature->m_MinMaxRect;
				}
				else
				{
					pRotMirrArr->FinalPoint(&drtFeature.left, &drtFeature.top, pFeature->m_MinMaxRect.left, pFeature->m_MinMaxRect.top);
					pRotMirrArr->FinalPoint(&drtFeature.right, &drtFeature.bottom, pFeature->m_MinMaxRect.right, pFeature->m_MinMaxRect.bottom);
					drtFeature.NormalizeRectD();
				}

				if (drtFeature.IsIntersection(drtViewRect_mm) == FALSE)
				{
					continue;
				}
			}


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

		/*	if (pUserSetInfo->userSite == eUserSite::eDAEDUCK)
			{
				if (pFeature->m_eType != FeatureType::P || !pFeature->m_bPolarity)
					continue;

				if (pFeature->m_pSymbol == nullptr)
					continue;

				if (pFeature->m_pSymbol->m_strUserSymbol.CompareNoCase(strAlignFeature) != 0)
					continue;
			}*/

			switch (pFeature->m_eType)
			{
			case FeatureType::L:		//Line
			{
				CFeatureL* pFeatureL = (CFeatureL*)pFeature;
				pSymbol = pFeatureL->m_pSymbol;

				_DrawLineFeature(pRender, pFeatureL, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, TRUE);
			}
			break;

			case FeatureType::P:		//Pad
			{
				CFeatureP* pFeatureP = (CFeatureP*)pFeature;
				pSymbol = pFeatureP->m_pSymbol;

				_DrawPadFeature(pRender, pFeatureP, pSymbol, pLayerSet, pFontArr, pRotMirrArr, bNeedUpdate, pUserSetInfo, FALSE, TRUE);
			}
			break;

			case FeatureType::A:		//Arc								
			{
				CFeatureA* pFeatureA = (CFeatureA*)pFeature;
				pSymbol = pFeatureA->m_pSymbol;

				_DrawArcFeature(pRender, pFeatureA, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, TRUE);
			}
			break;

			case FeatureType::S:		//Surface	
			{
				CFeatureS* pFeatureS = (CFeatureS*)pFeature;
				pSymbol = pFeatureS->m_pSymbol;
				_DrawSurfaceFeature(pRender, pFeatureS, pSymbol, pLayerSet, pRotMirrArr, bNeedUpdate, FALSE, TRUE, drtViewRect_mm);
			}
			break;

			case FeatureType::T:		//Text			
			{
				CFeatureT* pFeatureT = (CFeatureT*)pFeature;
				pSymbol = pFeatureT->m_pSymbol;

				pRotMirrArr->InputData(pFeatureT->m_dX, pFeatureT->m_dY, pFeatureT->m_eOrient);

				_DrawTextFeature(pRender, pFeatureT, pSymbol, pLayerSet, pFontArr, pRotMirrArr, bNeedUpdate, TRUE);

				pRotMirrArr->RemoveTail();
			}
			break;

			case FeatureType::B:		//Barcode								
			{
			}
			break;
			}
		}
	}

	_DeleteRenderItem();

	return RESULT_GOOD;
}

UINT32 CDrawFunction::OnMake_Layer_FilteredFeature(IN ID2D1RenderTarget* pRender, vector<CFeature*> vecFeature,
	IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
	CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo, IN const RECTD &drtViewRect_mm)
{
	m_bIsDrawMode = false;

	if (pRender == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;

	//	RECTD drtViewRect_mm = GetViewRect_mm(pRender);

	int nFeatureCount = static_cast<int>(vecFeature.size());

	CFeature*	pFeature = nullptr;
	CSymbol*	pSymbol = nullptr;


	_MakeRenderItem(pRender);

	for (int feno = 0; feno < nFeatureCount; feno++)
	{
		pFeature = vecFeature[feno];
		if (pFeature == nullptr) continue;;
		//교집합 체크 : Draw 시간을 줄이기 위함
// 		{
// 
// 			RECTD drtFeature;
// 			if (pRotMirrArr == nullptr)
// 			{
// 				drtFeature = pFeature->m_MinMaxRect;
// 			}
// 			else
// 			{
// 				pRotMirrArr->FinalPoint(&drtFeature.left, &drtFeature.top, pFeature->m_MinMaxRect.left, pFeature->m_MinMaxRect.top);
// 				pRotMirrArr->FinalPoint(&drtFeature.right, &drtFeature.bottom, pFeature->m_MinMaxRect.right, pFeature->m_MinMaxRect.bottom);
// 				drtFeature.NormalizeRectD();
// 			}
// 
// 			if (drtFeature.IsIntersection(drtViewRect_mm) == FALSE)
// 			{
// 				//clLocker.unlock();
// 				continue;
// 			}
// 		}
			   
		switch (pFeature->m_eType)
		{
		case FeatureType::L:		//Line
		{
			CFeatureL* pFeatureL = (CFeatureL*)pFeature;
			pSymbol = pFeatureL->m_pSymbol;

			_DrawLineFeature(pRender, pFeatureL, pSymbol, pLayerSet, nullptr, TRUE, TRUE);
		}
		break;

		case FeatureType::P:		//Pad
		{
			CFeatureP* pFeatureP = (CFeatureP*)pFeature;
			pSymbol = pFeatureP->m_pSymbol;

			_DrawPadFeature(pRender, pFeatureP, pSymbol, pLayerSet, pFontArr, nullptr, TRUE, pUserSetInfo, FALSE, TRUE);
		}
		break;

		case FeatureType::A:		//Arc								
		{
			CFeatureA* pFeatureA = (CFeatureA*)pFeature;
			pSymbol = pFeatureA->m_pSymbol;

			_DrawArcFeature(pRender, pFeatureA, pSymbol, pLayerSet, nullptr, TRUE, TRUE);
		}
		break;

		case FeatureType::S:		//Surface	
		{
			CFeatureS* pFeatureS = (CFeatureS*)pFeature;
			pSymbol = pFeatureS->m_pSymbol;

			_DrawSurfaceFeature(pRender, pFeatureS, pSymbol, pLayerSet, nullptr, TRUE, FALSE, TRUE, drtViewRect_mm);
		}
		break;

		case FeatureType::T:		//Text			
		{
		}
		break;

		case FeatureType::B:		//Barcode								
		{
		}
		break;
		}
	}

	_DeleteRenderItem();

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_MakeRenderItem(IN ID2D1RenderTarget* pRender)
{
	_DeleteRenderItem();

	if (pRender == nullptr) return RESULT_BAD;

	pRender->CreateSolidColorBrush(D2D1::ColorF(1, 1, 1), &m_pLayerColor_Save);//흰색으로 만들자.
	pRender->CreateSolidColorBrush(D2D1::ColorF(0), &m_pLayerColor_Hole_Save);

	return RESULT_GOOD;
}

UINT32 CDrawFunction::_DeleteRenderItem()
{
	SafeRelease(&m_pLayerColor_Save);
	SafeRelease(&m_pLayerColor_Hole_Save);

	return RESULT_GOOD;
}

void CDrawFunction::GetAngle(IN const Orient ori, OUT double &dAngle, OUT bool &bMirror)
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

Orient CDrawFunction::AddOrient(IN const Orient ori1, IN const Orient ori2)
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

vector<vector<DrawProfileData>> CDrawFunction::GetProfileData(CJobFile *pJobFile, int nPanelStepIndex)
{
	if (pJobFile == nullptr) return vector<vector<DrawProfileData>>();

	vector<vector<DrawProfileData>> vecProfileRect;

	int nStepCount = static_cast<int>(pJobFile->m_arrStep.GetCount());//스텝 갯수만큼 만들어서 관리

	if (nStepCount <= nPanelStepIndex) return vector<vector<DrawProfileData>>();//nPanelStepIndex  오류

	//Make Buffer
	vecProfileRect.resize(nStepCount);

	CStep *pStep = pJobFile->m_arrStep.GetAt(nPanelStepIndex);
	if (pStep == nullptr) return vector<vector<DrawProfileData>>();

	CLayer* pProfileLayer = pStep->m_Profile.m_pProfileLayer;
	
	if (pProfileLayer == nullptr) return vector<vector<DrawProfileData>>();
	int nFeatureCount = static_cast<int>(pProfileLayer->m_FeatureFile.m_arrFeature.GetCount());
	if (nFeatureCount > 0)
	{
		vector<DrawProfileData> vecRectTemp;
		for (int i = 0; i < nFeatureCount; i++)
		{
			CFeature *pFeature = pProfileLayer->m_FeatureFile.m_arrFeature.GetAt(i);
			if (pFeature->m_eType == FeatureType::S)
			{
				CFeatureS *pFeatureS = (CFeatureS*)pFeature;
				vector<DrawProfileData> vecRectTempTemp = _GetProfileRect(pFeatureS);

				for (auto itRect : vecRectTempTemp)
				{
					vecRectTemp.push_back(itRect);
				}
			}
		}

		//0 번째 Step의 Rect 정보
		DrawProfileData stTempData;
		for (auto itProfileData : vecRectTemp)
		{
			vecProfileRect[nPanelStepIndex].push_back(itProfileData);
		}
	}

	for (UINT i = 0; i < pStep->m_nSubStepCnt; i++)
	{
		CSubStep* pSubStep = pStep->m_arrSubStep[i];
		//Draw SubStep
		_SubStepRepeat(pSubStep, nullptr, vecProfileRect);
	}

	return vecProfileRect;
}

void CDrawFunction::_SubStepRepeat(CSubStep* pSubStep, CRotMirrArr *pRotMirrArr, vector<vector<DrawProfileData>> &vecProfileRect)
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

					_SubStepRepeat(pSubStep->m_arrNextStep[j], rotMirrArr, vecProfileRect);

				}
			}

			vector<DrawProfileData> vecRectTemp;
			for (int feno = 0; feno < pSubStep->m_pSubStep->m_Profile.m_pProfileLayer->m_FeatureFile.m_arrFeature.GetSize(); feno++)	//feature drawing
			{
				CFeature* pFeatureTmp = pSubStep->m_pSubStep->m_Profile.m_pProfileLayer->m_FeatureFile.m_arrFeature.GetAt(feno);

				if (pFeatureTmp->m_eType == FeatureType::S)
				{
					CFeatureS *pFeatureS = (CFeatureS*)pFeatureTmp;
					vector<DrawProfileData> vecRectTempTemp = _GetProfileRect(pFeatureS, rotMirrArr);

					for (auto itRect : vecRectTempTemp)
					{
						vecRectTemp.push_back(itRect);
					}
				}
			}

			int nStep = pSubStep->m_pSubStep->m_nStepID;
			if (vecProfileRect.size() > nStep)
			{
				DrawProfileData stTempData;
				for (auto itRect : vecRectTemp)
				{
					vecProfileRect[nStep].push_back(itRect);
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

vector<DrawProfileData> CDrawFunction::_GetProfileRect(CFeatureS *pFeatureS, CRotMirrArr *rotMirrArr)
{
	if (pFeatureS == nullptr) return vector<DrawProfileData>();

	vector<DrawProfileData> vecProfileRect;

	int nCountS = static_cast<int>(pFeatureS->m_arrObOe.GetSize());
	for (int s = 0; s < nCountS; s++)
	{
		CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(s);

		vector<DrawProfileData> vecRectTemp = _GetProfileRect_ObOe(pObOe, rotMirrArr);

		for (auto itProfileData : vecRectTemp)
		{
			vecProfileRect.push_back(itProfileData);
		}
	}

	return vecProfileRect;
}

vector<DrawProfileData> CDrawFunction::_GetProfileRect_ObOe(CObOe *pObOe, CRotMirrArr *rotMirrArr)
{
	if (pObOe == nullptr) return vector<DrawProfileData>();
	int nPtCount = static_cast<int>(pObOe->m_arrOsOc.GetSize());

	if (nPtCount < 1) return vector<DrawProfileData>();

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
	rotMirrArr->FinalPoint(&dNewX, &dNewY, pObOe->m_dXbs, pObOe->m_dYbs);
	stGeo.SetBasePoint(dNewX, dNewY);
	vecCurProfile.push_back(stGeo);
	stGeo._Reset();


	for (int i = 0; i < nPtCount; i++)
	{
		COsOc* pOsOc = pObOe->m_arrOsOc.GetAt(i);

		rotMirrArr->FinalPoint(&dNewX, &dNewY, pOsOc->m_dX, pOsOc->m_dY);

		if (pOsOc->m_bOsOc == true)
		{//Line
			stGeo.SetLine(dNewX, dNewY);
			vecCurProfile.push_back(stGeo);
			stGeo._Reset();
		}
		else
		{
			double dNew_CX, dNew_CY;
			rotMirrArr->FinalPoint(&dNew_CX, &dNew_CY, ((COc*)pOsOc)->m_dXc, ((COc*)pOsOc)->m_dYc);
			stGeo.SetArc(dNewX, dNewY, !((COc*)pOsOc)->m_bCw, dNew_CX, dNew_CY);
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

	vector<DrawProfileData> vecProfileRect;
	//Rect로 가정한다.
	/*if (nPtCount == 4)
	{
		DrawProfileData stProfileRect;
		stProfileRect.drtRect.left = vecCurProfile[0].ptPoint.x;
		stProfileRect.drtRect.top = vecCurProfile[0].ptPoint.y;
		stProfileRect.drtRect.right = vecCurProfile[2].ptPoint.x;
		stProfileRect.drtRect.bottom = vecCurProfile[2].ptPoint.y;
		stProfileRect.drtRect.NormalizeRectD();

		vecProfileRect.push_back(stProfileRect);
	}
	else
	{
		return vector<DrawProfileData>();
	}*/

	// 2022.08.04 KJH ADD
	// Strip 모양 Rect 아닌 경우(NSIS 삼성전기) 를 위해 Min, Max X,Y 계산 해서 사각형 만듬
	int nSize = static_cast<int>(vecCurProfile.size());
	double minX, minY = 0;
	double maxX, maxY = 0;

	minX = vecCurProfile[0].ptPoint.x;
	minY = vecCurProfile[0].ptPoint.y;
	maxX = vecCurProfile[0].ptPoint.x;
	maxY = vecCurProfile[0].ptPoint.y;
	for (int i = 0; i < nSize; i++)
	{
		if (minX > vecCurProfile[i].ptPoint.x)
			minX = vecCurProfile[i].ptPoint.x;
		if (maxX < vecCurProfile[i].ptPoint.x)
			maxX = vecCurProfile[i].ptPoint.x;

		if (minY > vecCurProfile[i].ptPoint.y)
			minY = vecCurProfile[i].ptPoint.y;
		if (maxY < vecCurProfile[i].ptPoint.y)
			maxY = vecCurProfile[i].ptPoint.y;
	}

	DrawProfileData stProfileRect;
	stProfileRect.drtRect.left = minX;
	stProfileRect.drtRect.top = maxY;
	stProfileRect.drtRect.right = maxX;
	stProfileRect.drtRect.bottom = minY;
	stProfileRect.drtRect.NormalizeRectD();

	vecProfileRect.push_back(stProfileRect);

	return vecProfileRect;
}

BOOL CDrawFunction::CheckPanelInOut(vector<vector<DrawProfileData>> vecvecRect, MeasureItem_t *tmpMeasure, int step)
{
	bool checkflag = FALSE;
	int RectCount = static_cast<int>(vecvecRect[step].size());
	for (int nRect = 0; nRect < RectCount; nRect++)
	{
		// Panel in
		if (vecvecRect[step][nRect].drtRect.IsPtInRectD(tmpMeasure->MinMaxRect_In_Panel.CenterX(), tmpMeasure->MinMaxRect_In_Panel.CenterY()) == TRUE)
		{
			checkflag = TRUE;
		}
		// Panel Out
		else
		{
			checkflag = FALSE;
		}
	}	

	return checkflag;
}


BOOL CDrawFunction::CheckStripInOut(vector<vector<DrawProfileData>> vecvecRect, MeasureItem_t *tmpMeasure, int &stripIndex, int step)
{	
	int nRectCount = static_cast<int>(vecvecRect[step].size());

	vector<bool> vectmp;
	vectmp.push_back(FALSE);
	vectmp.resize(nRectCount);	

	vector<INT> vectmp_index;
	vectmp_index.push_back(-1);
	vectmp_index.resize(nRectCount);
	
	for (int nRect = 0; nRect < nRectCount; nRect++)
	{
		// Strip in
		if (vecvecRect[step][nRect].drtRect.IsPtInRectD(tmpMeasure->MinMaxRect_In_Panel.CenterX(), tmpMeasure->MinMaxRect_In_Panel.CenterY()) == TRUE)
		{
			vectmp[nRect] = TRUE;
			vectmp_index[nRect] = nRect;
		}
		// Strip Out
		else
		{
			vectmp[nRect] = FALSE;
			vectmp_index[nRect] = -1;
		}
	}

	for (int i = 0; i < nRectCount; i++)
	{
		if (vectmp[i] == TRUE)
		{
			stripIndex = vectmp_index[i];
		}
	}
	

	if(find(vectmp.begin(), vectmp.end(), true) != vectmp.end())
	{
		return TRUE;
	}
	else  
	{
		return FALSE;
	}
}


BOOL CDrawFunction::CheckUnitInOut(vector<vector<DrawProfileData>> vecvecRect, MeasureItem_t *tmpMeasure, int &unitIndex, int step)
{
	int nRectCount = static_cast<int>(vecvecRect[step].size());

	vector<bool> vectmp;
	vectmp.push_back(FALSE);
	vectmp.resize(nRectCount);

	vector<INT> vectmp_index;
	vectmp_index.push_back(-1);
	vectmp_index.resize(nRectCount);

	for (int nRect = 0; nRect < nRectCount; nRect++)
	{
		// Unit in
		if (vecvecRect[step][nRect].drtRect.IsPtInRectD(tmpMeasure->MinMaxRect_In_Panel.CenterX(), tmpMeasure->MinMaxRect_In_Panel.CenterY()) == TRUE)
		{
			vectmp[nRect] = TRUE;
			vectmp_index[nRect] = nRect;
		}
		// Unit Out
		else
		{
			vectmp[nRect] = FALSE;
			vectmp_index[nRect] = -1;
		}
	}

	for (int i = 0; i < nRectCount; i++)
	{
		if (vectmp[i] == TRUE)
		{
			unitIndex = vectmp_index[i];
		}
	}

	if (find(vectmp.begin(), vectmp.end(), true) != vectmp.end())
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

UINT32 CDrawFunction::MakeFeatureGeo(vector<CFeature *> &vecFeature, vector<CRotMirrArr *> &vecRotMirrArr, OUT vector<vector<DrawFeatureGeo*>> &vecvecDrawFeatureGeo)
{
	int nFeatureCount = static_cast<int>(vecFeature.size());
	int nRotMirrCount = static_cast<int>(vecRotMirrArr.size());

	if (nFeatureCount != nRotMirrCount) return RESULT_BAD;

	vecvecDrawFeatureGeo.resize(nFeatureCount);
	for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
	{
		CFeature *pFeature = vecFeature[nfeno];
		if ( pFeature == nullptr ) continue;

		ID2D1PathGeometry *pGeo = nullptr;

		switch (pFeature->m_eType)
		{
		case FeatureType::Z:
		{
			CFeatureZ *pFeatureZ = (CFeatureZ*)pFeature;
			m_pMakeGeo->MakeTp(pFeatureZ, vecRotMirrArr[nfeno], &pGeo);

			DrawFeatureGeo* pDrawFeatureGeo = new DrawFeatureGeo();
			pDrawFeatureGeo->pGeo = pGeo;
			vecvecDrawFeatureGeo[nfeno].push_back(pDrawFeatureGeo);
		}
		break;
		case FeatureType::L:
		{
			CFeatureL *pFeatureL = (CFeatureL *)pFeature;
			m_pMakeGeo->MakeLine(pFeatureL, vecRotMirrArr[nfeno], &pGeo);
			
			DrawFeatureGeo* pDrawFeatureGeo = new DrawFeatureGeo();
			pDrawFeatureGeo->pGeo = pGeo;
			vecvecDrawFeatureGeo[nfeno].push_back(pDrawFeatureGeo);
		}
		break;
		case FeatureType::P:
		{
			CFeatureP *pFeatureP = (CFeatureP *)pFeature;
			MakeGeo_FeaturePad(pFeatureP, vecRotMirrArr[nfeno], &pGeo);

			DrawFeatureGeo* pDrawFeatureGeo = new DrawFeatureGeo();
			pDrawFeatureGeo->pGeo = pGeo;
			vecvecDrawFeatureGeo[nfeno].push_back(pDrawFeatureGeo);
		}
		break;
		case FeatureType::A:
		{
			CFeatureA *pFeatureA = (CFeatureA *)pFeature;
			m_pMakeGeo->MakeArc(pFeatureA, vecRotMirrArr[nfeno], &pGeo);

			DrawFeatureGeo* pDrawFeatureGeo = new DrawFeatureGeo();
			pDrawFeatureGeo->pGeo = pGeo;
			vecvecDrawFeatureGeo[nfeno].push_back(pDrawFeatureGeo);
		}
		break;
		case FeatureType::S:
		{
			CFeatureS *pFeatureS = (CFeatureS *)pFeature;

			vector<ID2D1PathGeometry *> vecGeo;
			MakeGeo_FeatureSurface(pFeatureS, vecRotMirrArr[nfeno], vecGeo);

			int nObOeCount = static_cast<int>(vecGeo.size());
			for (int nObOe = 0; nObOe < nObOeCount; nObOe++)
			{
				DrawFeatureGeo* pDrawFeatureGeo = new DrawFeatureGeo();
				pDrawFeatureGeo->pGeo = vecGeo[nObOe];
				vecvecDrawFeatureGeo[nfeno].push_back(pDrawFeatureGeo);
			}
		}
		break;
		default:
			break;
		}
		
	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::MakeGeo_FeaturePad(CFeatureP* pFeatureP, CRotMirrArr *pRotMirrArr, ID2D1PathGeometry **pGeo)
{
	if (pFeatureP == nullptr) 

	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	switch (pFeatureP->m_pSymbol->m_eSymbolName)
	{
	case SymbolName::r:
		m_pMakeGeo->MakePadCircle(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::s:
	case SymbolName::rect:
		m_pMakeGeo->MakePadRect(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::rectxr:
		m_pMakeGeo->MakePadRectxr(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::rectxc:
		m_pMakeGeo->MakePadRectxc(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::oval:
		m_pMakeGeo->MakePadOval(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::di:
		m_pMakeGeo->MakePadDi(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::octagon:
		m_pMakeGeo->MakePadOct(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::donut_r:
		m_pMakeGeo->MakePadDonut_r(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::donut_s:
		m_pMakeGeo->MakePadDonut_s(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::hex_l:
		m_pMakeGeo->MakePadHex_l(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::hex_s:
		m_pMakeGeo->MakePadHex_s(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::bfr:
		m_pMakeGeo->MakePadBfr(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::bfs:
		m_pMakeGeo->MakePadBfs(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::tri:
		m_pMakeGeo->MakePadTriangle(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::oval_h:
		m_pMakeGeo->MakePadOval_h(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::thr:
		m_pMakeGeo->MakePadThr(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::ths:
		m_pMakeGeo->MakePadThs(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::s_ths:
		m_pMakeGeo->MakePadS_ths(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::s_tho:
		m_pMakeGeo->MakePadS_tho(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::sr_ths:
		m_pMakeGeo->MakePadSr_ths(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::rc_ths:
		m_pMakeGeo->MakePadRc_ths(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::rc_tho:
		m_pMakeGeo->MakePadRc_tho(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::el:
		m_pMakeGeo->MakePadEllipse(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::moire:
		m_pMakeGeo->MakePadMoire(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::hole:
		m_pMakeGeo->MakePadHole(pFeatureP, pRotMirrArr, pGeo);
		break;
	case SymbolName::userdefined:
		break;
	default:
		break;
	}

	return RESULT_GOOD;
}

UINT32 CDrawFunction::MakeGeo_FeatureSurface(CFeatureS* pFeatureS, CRotMirrArr *pRotMirrArr, vector<ID2D1PathGeometry *> &vecGeo)
{
	if (pFeatureS == nullptr) return RESULT_BAD;

	vector<MakeFeatureData> vecGeoDraw = CDrawMakePoint::MakeSurface(pFeatureS, pRotMirrArr, RECTD() );
	int nCount = static_cast<int>(vecGeoDraw.size());

	vecGeo.resize(nCount);
	for (int i = 0; i < nCount; i++)
	{
		if (!vecGeoDraw[i].bIsUsed)
			continue;

		_SetGeometry(vecGeoDraw[i].vecPointData, &vecGeo[i]);
	}

	return RESULT_GOOD;
}