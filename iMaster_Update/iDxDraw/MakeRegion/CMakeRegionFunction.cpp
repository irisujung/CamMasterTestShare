/*#include "../pch.h"*/
#include "pch.h"
#include "CMakeRegionFunction.h"
#include "CHalconFunction.h"
#include "../CDrawMakePoint.h"
#include "../CDrawFunction.h"

CMakeRegionFunction::CMakeRegionFunction()
{

}

CMakeRegionFunction::~CMakeRegionFunction()
{

}

void CMakeRegionFunction::SetLink(IN CHalconFunction * pHalconFunction)
{
	m_pHalconFunction = pHalconFunction;
}

UINT32 CMakeRegionFunction::OnMake_Feature(IN CLayer* pLayer, RECTD drtImage_mm, IN const int nFeatureIndex, IN const LayerSet_t* pLayerSet,
	CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo, IN const SaveMode &eSaveMode, IN const double &dResolution)
{
	UINT32 nRet = RESULT_BAD;

	if (pLayer == nullptr) return RESULT_BAD;
	if (pLayerSet == nullptr) return RESULT_BAD;

	RECTD drtViewRect_mm = drtImage_mm;
	drtViewRect_mm.NormalizeRectD();

	int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetSize());
	if (nFeatureIndex < 0) return RESULT_BAD;
	if (nFeatureIndex >= nFeatureCount) return RESULT_BAD;

	CFeature*	pFeature = nullptr;
	CSymbol*	pSymbol = nullptr;

	pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nFeatureIndex);
	if (pFeature == nullptr) return RESULT_BAD;

	//±³ÁýÇÕ Ã¼Å© : Draw ½Ã°£À» ÁÙÀÌ±â À§ÇÔ
	RECTD drtFeature;
	{		
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
			return RESULT_GOOD;
		}
	}

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	vector<RegionData> vecRegionData;

	switch (pFeature->m_eType)
	{
	case FeatureType::Z:
	{
		if (eSaveMode == SaveMode::SaveMode_Tp)
		{
			CFeatureZ* pFeatureZ = (CFeatureZ*)pFeature;
			pSymbol = pFeatureZ->m_pSymbol;

			vecRegionData = _DrawTpFeature(pFeatureZ, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
			nRet = RESULT_GOOD;
		}
	}

	case FeatureType::L:		//Line
	{
		if (eSaveMode == SaveMode::SaveMode_Line || eSaveMode == SaveMode::SaveMode_LineArc)
		{
			CFeatureL* pFeatureL = (CFeatureL*)pFeature;
			pSymbol = pFeatureL->m_pSymbol;

			vecRegionData = _DrawLineFeature(pFeatureL, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);

			nRet = RESULT_GOOD;
		}
	}
	break;

	case FeatureType::P:		//Pad
	{
		if (eSaveMode == SaveMode::SaveMode_Pad)
		{
			CFeatureP* pFeatureP = (CFeatureP*)pFeature;
			pSymbol = pFeatureP->m_pSymbol;

			vecRegionData = _DrawPadFeature(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);

			nRet = RESULT_GOOD;
		}
	}
	break;

	case FeatureType::A:		//Arc								
	{
		if (eSaveMode == SaveMode::SaveMode_Arc || eSaveMode == SaveMode::SaveMode_LineArc)
		{
			CFeatureA* pFeatureA = (CFeatureA*)pFeature;
			pSymbol = pFeatureA->m_pSymbol;

			vecRegionData = _DrawArcFeature(pFeatureA, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);

			nRet = RESULT_GOOD;
		}
	}
	break;

	case FeatureType::S:		//Surface	
	{
		CFeatureS* pFeatureS = (CFeatureS*)pFeature;
		if (eSaveMode == SaveMode::SaveMode_All ||
			(pFeatureS->m_bPolarity == true && eSaveMode == SaveMode::SaveMode_Surface_Pos) ||
			(pFeatureS->m_bPolarity == false && eSaveMode == SaveMode::SaveMode_Surface_Neg))
		{
			pSymbol = pFeatureS->m_pSymbol;
			vecRegionData = _DrawSurfaceFeature(pFeatureS, pRotMirrArr, drtViewRect_mm, dResolution);

			nRet = RESULT_GOOD;
		}
	}
	break;

	case FeatureType::B:		//Barcode								
	{
	}
	break;
	}

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}

	//Set Add Region Data
	if (static_cast<int>(vecRegionData.size()) > 0)
	{
		m_pHalconFunction->AddRegion(vecRegionData);
	}
	

	return nRet;
}

UINT32 CMakeRegionFunction::_GetLineDrawInfo(IN const CSymbol* pSymbol, OUT float &fPenWidth, HStrokeStyle &eStrokeStyle)
{
	if (pSymbol == nullptr) return RESULT_BAD;

	int nSize = static_cast<int>(pSymbol->m_vcParams.size());
	if (nSize <= 0) return RESULT_BAD;

	switch (pSymbol->m_eSymbolName)
	{
	default:
	case SymbolName::r:
	case SymbolName::hole:
		fPenWidth = static_cast<float>(pSymbol->m_vcParams[0]);
		eStrokeStyle = HStrokeStyle::enumStroke_r;
		break;

	case SymbolName::s:
	case SymbolName::rect:
		fPenWidth = static_cast<float>(pSymbol->m_vcParams[0]);
		eStrokeStyle = HStrokeStyle::enumStroke_s;
		break;
	}

	return RESULT_GOOD;
}

UINT32 CMakeRegionFunction::_GetPadDrawInfo(IN const CSymbol* pSymbol, OUT float &fPenWidth, HStrokeStyle &eStrokeStyle, CRotMirrArr *pRotMirrArr)
{
	if (pSymbol == nullptr) return RESULT_BAD;

	switch (pSymbol->m_eSymbolName)
	{
	case SymbolName::donut_r:
	case SymbolName::thr:
		fPenWidth = static_cast<float>((pSymbol->m_vcParams[0] - pSymbol->m_vcParams[1]) / 2.f);
		eStrokeStyle = HStrokeStyle::enumStroke_r;
		break;
	case SymbolName::donut_s:
	case SymbolName::ths:
		fPenWidth = static_cast<float>((pSymbol->m_vcParams[0] - pSymbol->m_vcParams[1]) / 2.f);
		eStrokeStyle = HStrokeStyle::enumStroke_s;
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
		eStrokeStyle = HStrokeStyle::enumStroke_r;
	}
	break;
	case SymbolName::moire:
		fPenWidth = static_cast<float>(pSymbol->m_vcParams[0] / 2);
		eStrokeStyle = HStrokeStyle::enumStroke_r;
		break;
	default:
		return RESULT_BAD;
		break;
	}

	return RESULT_GOOD;
	return RESULT_GOOD;
}

UINT32 CMakeRegionFunction::_GetArcDrawInfo(IN const CSymbol* pSymbol, OUT float &fPenWidth, HStrokeStyle &eStrokeStyle)
{
	if (pSymbol == nullptr) return RESULT_BAD;

	int nSize = static_cast<int>(pSymbol->m_vcParams.size());
	if (nSize <= 0) return RESULT_BAD;

	switch (pSymbol->m_eSymbolName)
	{
	default:
	case SymbolName::r:
	case SymbolName::hole:
		fPenWidth = static_cast<float>(pSymbol->m_vcParams[0]);
		eStrokeStyle = HStrokeStyle::enumStroke_r;
		break;

	case SymbolName::s:
	case SymbolName::rect:
		fPenWidth = static_cast<float>(pSymbol->m_vcParams[0]);
		eStrokeStyle = HStrokeStyle::enumStroke_s;
		break;
	}

	return RESULT_GOOD;
}


UINT32 CMakeRegionFunction::MakeShape_Line(IN HObject &ho_Region, IN const double &dAngle, IN const float &fPenWidth,
	IN const HStrokeStyle &eStrokeStyle, OUT HObject &ho_Shaped)
{
	try
	{
		GenEmptyObj(&ho_Shaped);

		HObject ho_Unit_Shape;

		if (eStrokeStyle == HStrokeStyle::enumStroke_r)
		{
			GenCircle(&ho_Unit_Shape, fPenWidth + 0.5, fPenWidth + 0.5, fPenWidth / 2 + 0.5);
		}
		else
		{
			if ((dAngle <= 0.1 &&  dAngle > -0.1) ||
				(dAngle <= 90.1 &&  dAngle > -89.9) ||
				(dAngle <= 180.1 &&  dAngle > -179.9) ||
				(dAngle <= 270.1 &&  dAngle > -269.9))
			{//Rectangle 1
				GenRectangle1(&ho_Unit_Shape, 0, 0, fPenWidth + 0.5, fPenWidth + 0.5);
			}
			else
			{//Rotation ÇÊ¿ä? rectangle 2
				GenRectangle2(&ho_Unit_Shape, fPenWidth * 2 + 0.5, fPenWidth * 2 + 0.5, ToRadian(dAngle), fPenWidth + 0.5, fPenWidth + 0.5);
			}
		}

		Dilation1(ho_Region, ho_Unit_Shape, &ho_Shaped, 1);

	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Error_MakeShape_Line : %s\n"), strError);

		return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 CMakeRegionFunction::MakeShape_Arc(IN HObject &ho_Region, IN const float &fPenWidth, IN const HStrokeStyle &eStrokeStyle, OUT HObject &ho_Shaped)
{
	try
	{
		GenEmptyObj(&ho_Shaped);

		HObject ho_Unit_Shape;

		if (eStrokeStyle == HStrokeStyle::enumStroke_r)
		{
			GenCircle(&ho_Unit_Shape, fPenWidth + 0.5f, fPenWidth + 0.5f, fPenWidth / 2 + 0.5f);
		}
		else
		{
			GenRectangle1(&ho_Unit_Shape, 0, 0, fPenWidth + 0.5f, fPenWidth + 0.5f);
		}

		Dilation1(ho_Region, ho_Unit_Shape, &ho_Shaped, 1);

	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Error_MakeShape_Arc : %s\n"), strError);

		return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 CMakeRegionFunction::MakeShape_Pad(IN HObject &ho_Region, IN const float &fPenWidth, IN const HStrokeStyle &eStrokeStyle, OUT HObject &ho_Shaped)
{
	try
	{
		GenEmptyObj(&ho_Shaped);

		HObject ho_Unit_Shape;

		if (eStrokeStyle == HStrokeStyle::enumStroke_r)
		{
			GenCircle(&ho_Unit_Shape, fPenWidth + 0.5f, fPenWidth + 0.5f, fPenWidth / 2 + 0.5f);
		}
		else
		{
			GenRectangle1(&ho_Unit_Shape, 0, 0, fPenWidth + 0.5f, fPenWidth + 0.5f);
		}

		Dilation1(ho_Region, ho_Unit_Shape, &ho_Shaped, 1);

	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Error_MakeShape_Arc : %s\n"), strError);

		return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 CMakeRegionFunction::MakeRegion_Line(IN vector<MakeFeatureData>& vecPointData, IN const float &fPenWidth, IN const HStrokeStyle &eStrokeStyle,
	OUT vector<RegionData> &vecRegionData)
{
	try
	{
		HObject ho_Temp, ho_RegionLines;
		HObject ho_Temp1;
		HObject ho_Shaped;
		HTuple hv_Sx, hv_Sy;
		HTuple hv_Ex, hv_Ey;



		int nPointSetCount = static_cast<int>(vecPointData.size());
		vecRegionData.resize(nPointSetCount);

		for (int nPointSet = 0; nPointSet < nPointSetCount; nPointSet++)
		{
			GenEmptyObj(&vecRegionData[nPointSet].hObject);

			vecRegionData[nPointSet].bPolarity = vecPointData[nPointSet].bPolarity;

			HTuple nSx, nSy;
			HTuple nEx, nEy;

			vector<GEODRAW> &vecPoint = vecPointData[nPointSet].vecPointData;
			int nCount = static_cast<int>(vecPoint.size());
			for (int i = 0; i < nCount; i++)
			{

				switch (vecPoint[i].nType)
				{
				case OdbPointType::enumBase:
				{
					nSx = static_cast<int>(vecPoint[i].ptPoint.x + 0.5);
					nSy = static_cast<int>(vecPoint[i].ptPoint.y + 0.5);
					hv_Sx = vecPoint[i].ptPoint.x;
					hv_Sy = vecPoint[i].ptPoint.y;
				}
				break;
				case OdbPointType::enumLine:
				{
					nEx = static_cast<int>(vecPoint[i].ptPoint.x + 0.5);
					nEy = static_cast<int>(vecPoint[i].ptPoint.y + 0.5);
					hv_Ex = vecPoint[i].ptPoint.x;
					hv_Ey = vecPoint[i].ptPoint.y;

					GenRegionLine(&ho_Temp, hv_Sy, hv_Sx, hv_Ey, hv_Ex);

					double dAngle = CDrawMakePoint::GetAngle(hv_Sx.D(), hv_Sy.D(), hv_Ex.D(), hv_Ey.D());
					UINT32 nRet = MakeShape_Line(ho_Temp, dAngle, fPenWidth, eStrokeStyle, ho_Shaped);
					if (nRet == RESULT_GOOD)
					{
						ConcatObj(vecRegionData[nPointSet].hObject, ho_Shaped, &vecRegionData[nPointSet].hObject);
					}
				}
				break;
				default:
					break;

				}
			}
		}
	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Error_MakeRegion_Line : %s\n"), strError);

		return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 CMakeRegionFunction::MakeRegion_Arc(IN vector<MakeFeatureData>& vecPointData, IN const float &fPenWidth, IN const HStrokeStyle &eStrokeStyle,
	OUT vector<RegionData> &vecRegionData)
{
	try
	{
		HObject ho_Temp, ho_RegionLines;
		HObject ho_Temp1;
		HObject ho_Shaped;
		HTuple hv_Sx, hv_Sy;
		HTuple hv_Ex, hv_Ey;
		HTuple hv_Cx, hv_Cy;


		int nPointSetCount = static_cast<int>(vecPointData.size());
		vecRegionData.resize(nPointSetCount);

		for (int nPointSet = 0; nPointSet < nPointSetCount; nPointSet++)
		{
			GenEmptyObj(&vecRegionData[nPointSet].hObject);

			vecRegionData[nPointSet].bPolarity = vecPointData[nPointSet].bPolarity;

			HTuple nSx, nSy;
			HTuple nEx, nEy;

			vector<GEODRAW> &vecPoint = vecPointData[nPointSet].vecPointData;
			int nCount = static_cast<int>(vecPoint.size());
			for (int i = 0; i < nCount; i++)
			{

				switch (vecPoint[i].nType)
				{
				case OdbPointType::enumBase:
				{
					hv_Sx = vecPoint[i].ptPoint.x;
					hv_Sy = vecPoint[i].ptPoint.y;
				}
				break;
				case OdbPointType::enumArc:
				{
					hv_Ex = vecPoint[i].ptPoint.x;
					hv_Ey = vecPoint[i].ptPoint.y;
					hv_Cx = vecPoint[i].ptCenter.x;
					hv_Cy = vecPoint[i].ptCenter.y;

					ho_Temp = MakeArcRegion(hv_Sx, hv_Sy, hv_Ex, hv_Ey, hv_Cx, hv_Cy, vecPoint[i].bCw);

					UINT32 nRet = MakeShape_Arc(ho_Temp, fPenWidth, eStrokeStyle, ho_Shaped);
					if (nRet == RESULT_GOOD)
					{
						ConcatObj(vecRegionData[nPointSet].hObject, ho_Shaped, &vecRegionData[nPointSet].hObject);
					}
				}
				break;
				default:
					break;

				}
			}
		}
	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Error_MakeRegion_Arc : %s\n"), strError);

		return RESULT_BAD;
	}
	return RESULT_GOOD;
}

UINT32 CMakeRegionFunction::MakeRegion_Pad(IN vector<MakeFeatureData>& vecPointData, CFeature *pFeature, CRotMirrArr *pRotMirrArr, IN const double dResolution, OUT vector<RegionData> &vecRegionData)
{
	if (pFeature == nullptr) return RESULT_BAD;
	if (pRotMirrArr == nullptr) return RESULT_BAD;

	try
	{
		vector<D2D1_POINT_2F> vecRegionPoint;

		int nPointSetCount = static_cast<int>(vecPointData.size());

		vecRegionData.clear();
		vecRegionData.resize(nPointSetCount);

		vector<RegionData> vecTempSurface;
		vecTempSurface.resize(nPointSetCount);

		vector<D2D1_POINT_2F> vecRegionPoint_Temp;
		D2D1_POINT_2F fptLastPoint;
		for (int nPointSet = 0; nPointSet < nPointSetCount; nPointSet++)
		{
			vecRegionPoint.clear();

			vector<GEODRAW> &vecPoint = vecPointData[nPointSet].vecPointData;
			int nCount = static_cast<int>(vecPoint.size());
			for (int i = 0; i < nCount; i++)
			{

				switch (vecPoint[i].nType)
				{
				case OdbPointType::enumBase:
				{
					vecRegionPoint.push_back(vecPoint[i].ptPoint);
				}
				break;
				case OdbPointType::enumLine:
				{
					vecRegionPoint.push_back(vecPoint[i].ptPoint);
				}
				break;
				case OdbPointType::enumArc:
				{
					vecRegionPoint_Temp = _MakeArc_Point(fptLastPoint, vecPoint[i].ptPoint, vecPoint[i].ptCenter, vecPoint[i].bCw);
					int nCount = static_cast<int>(vecRegionPoint_Temp.size());
					for (int j = 0; j < nCount; j++)
					{
						vecRegionPoint.push_back(vecRegionPoint_Temp[j]);
					}

				}
				break;
				default:
					break;

				}

				fptLastPoint = vecPoint[i].ptPoint;
			}

			HTuple hv_Rows, hv_Cols;

			int nPointCount = static_cast<int>(vecRegionPoint.size());
			for (int i = 0; i < nPointCount; i++)
			{
				hv_Cols.Append(vecRegionPoint[i].x);
				hv_Rows.Append(vecRegionPoint[i].y);
			}

			HObject ho_Shaped;
			float fPenWidth;
			HStrokeStyle eStrokeStyle;

			switch (pFeature->m_eType)
			{
			case FeatureType::P:
			{
				vecRegionData[nPointSet].bPolarity = vecPointData[nPointSet].bPolarity;
				GenRegionPolygonFilled(&vecRegionData[nPointSet].hObject, hv_Rows, hv_Cols);

				if (pFeature->m_pSymbol == nullptr) break;

				switch (pFeature->m_pSymbol->m_eSymbolName)
				{
				case SymbolName::oval:
				case SymbolName::donut_r:
				case SymbolName::thr:
				case SymbolName::donut_s:
				case SymbolName::ths:
				case SymbolName::moire:
				{
					_GetPadDrawInfo(pFeature->m_pSymbol, fPenWidth, eStrokeStyle, pRotMirrArr);
					fPenWidth /= static_cast<float>(dResolution);

					MakeShape_Pad(vecRegionData[nPointSet].hObject, fPenWidth, eStrokeStyle, ho_Shaped);
					vecRegionData[nPointSet].hObject = ho_Shaped;
				}
				break;
				}
			}
			break;
			case FeatureType::S:
			{
				vecTempSurface[nPointSet].bPolarity = vecPointData[nPointSet].bPolarity;
				GenRegionPolygonFilled(&vecTempSurface[nPointSet].hObject, hv_Rows, hv_Cols);
			}
			case FeatureType::Z:
			{
				vecTempSurface[nPointSet].bPolarity = vecPointData[nPointSet].bPolarity;
				GenRegionPolygonFilled(&vecTempSurface[nPointSet].hObject, hv_Rows, hv_Cols);
			}
			break;
			default:
				break;

			}
		}//End PointSet

		if (pFeature->m_eType == FeatureType::S)
		{//SurfaceÀÏ °æ¿ì 1°³ ÀÇ Surface·Î ¸¸µé±â À§ÇÏ¿© Ã³¸®
			//DrawÇÒ¶§ CombineÇÏ´Â °Í°ú °°Àº È¿°ú¸¦ ÁÖ±â À§ÇÏ¿©

			vecRegionData.resize(1);

			int nCount = static_cast<int>(vecTempSurface.size());
			for (int i = 0; i < nCount; i++)
			{
				if (i == 0)
				{
					vecRegionData[0].hObject = vecTempSurface[i].hObject;
				}
				else
				{
					if (vecTempSurface[i].bPolarity == true)
					{
						ConcatObj(vecRegionData[0].hObject, vecTempSurface[i].hObject, &vecRegionData[0].hObject);
					}
					else
					{
						Difference(vecRegionData[0].hObject, vecTempSurface[i].hObject, &vecRegionData[0].hObject);
					}
				}
			}

			HObject ho_debug = vecRegionData[0].hObject;
			vecRegionData[0].bPolarity = pFeature->m_bPolarity;
			

		}


	}//
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Error_MakeRegion_Arc : %s\n"), strError);

		return RESULT_BAD;
	}

	return RESULT_GOOD;
}


HObject CMakeRegionFunction::MakeArcRegion(IN const HTuple &Sx, IN const HTuple &Sy, IN const HTuple &Ex, IN const HTuple &Ey,
	IN const HTuple &Cx, IN const HTuple &Cy, IN const bool &bCw)
{
	HObject hArc;
	try
	{
		D2D1_POINT_2F ptS = D2D1::Point2F(static_cast<float>(Sx.D()), static_cast<float>(Sy.D()));
		D2D1_POINT_2F ptE = D2D1::Point2F(static_cast<float>(Ex.D()), static_cast<float>(Ey.D()));
		D2D1_POINT_2F ptC = D2D1::Point2F(static_cast<float>(Cx.D()), static_cast<float>(Cy.D()));

		double dStart_Angle = CDrawMakePoint::GetAngle(ptC, ptS);
		double dEnd_Angle = CDrawMakePoint::GetAngle(ptC, ptE);

		D2D1_POINT_2F fptInnerPoint;
		vector<D2D1_POINT_2F> vecArcPoint;
		vecArcPoint.push_back(ptS);
		if (bCw == true)
		{//CW
			double dArcAngle;
			if (dStart_Angle > dEnd_Angle)
			{
				dArcAngle = (360. - dStart_Angle + dEnd_Angle);
			}
			else
			{

				dArcAngle = dEnd_Angle - dStart_Angle;
			}

			double dAngleStep = -0.5;
			int dAngleStepCount = static_cast<int>(fabs(dArcAngle / dAngleStep) + 0.5);

			double dSumAngle = 0.0;
			for (int i = 0; i < dAngleStepCount; i++)
			{
				dSumAngle += dAngleStep;

				fptInnerPoint = CDrawMakePoint::Rotate(ptS, ptC, dSumAngle);
				vecArcPoint.push_back(fptInnerPoint);
			}

		}
		else
		{
			double dArcAngle;
			if (dStart_Angle > dEnd_Angle)
			{
				dArcAngle = dStart_Angle - dEnd_Angle;
			}
			else
			{
				dArcAngle = (360. - dEnd_Angle + dStart_Angle);
			}
			double dAngleStep = 0.5;
			int dAngleStepCount = static_cast<int>(fabs(dArcAngle / dAngleStep) + 0.5);

			double dSumAngle = 0.0;
			for (int i = 0; i < dAngleStepCount; i++)
			{
				dSumAngle += dAngleStep;

				fptInnerPoint = CDrawMakePoint::Rotate(ptS, ptC, dSumAngle);
				vecArcPoint.push_back(fptInnerPoint);
			}
		}
		vecArcPoint.push_back(ptE);

		//
		int nPointCount = static_cast<int>(vecArcPoint.size());
		HTuple hv_Xs;
		HTuple hv_Ys;
		hv_Xs.Clear();
		hv_Ys.Clear();
		for (int i = 0; i < nPointCount; i++)
		{
			hv_Xs.Append(vecArcPoint[i].x);
			hv_Ys.Append(vecArcPoint[i].y);
		}


		GenRegionPoints(&hArc, hv_Ys, hv_Xs);

	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Error_MakeRegion_Arc : %s\n"), strError);

	}
	return hArc;
}

vector<D2D1_POINT_2F> CMakeRegionFunction::_MakeArc_Point(IN const D2D1_POINT_2F fptStart, IN const D2D1_POINT_2F fptEnd, IN const D2D1_POINT_2F fptCenter, IN const bool &bCw)
{
	HObject hArc;
	vector<D2D1_POINT_2F> vecArcPoint;
	try
	{

		double dStart_Angle = CDrawMakePoint::GetAngle(fptCenter, fptStart);
		double dEnd_Angle = CDrawMakePoint::GetAngle(fptCenter, fptEnd);

		D2D1_POINT_2F fptInnerPoint;

		vecArcPoint.push_back(fptStart);
		if (bCw == true)
		{//CW
			double dArcAngle;
			if (dStart_Angle > dEnd_Angle)
			{
				dArcAngle = (360. - dStart_Angle + dEnd_Angle);
			}
			else
			{

				dArcAngle = dEnd_Angle - dStart_Angle;
			}

			double dAngleStep = -0.5;
			int dAngleStepCount = static_cast<int>(fabs(dArcAngle / dAngleStep) + 0.5);

			double dSumAngle = 0.0;
			for (int i = 0; i < dAngleStepCount; i++)
			{
				dSumAngle += dAngleStep;

				fptInnerPoint = CDrawMakePoint::Rotate(fptStart, fptCenter, dSumAngle);
				vecArcPoint.push_back(fptInnerPoint);
			}

		}
		else
		{
			double dArcAngle;
			if (dStart_Angle > dEnd_Angle)
			{
				dArcAngle = dStart_Angle - dEnd_Angle;
			}
			else
			{
				dArcAngle = (360. - dEnd_Angle + dStart_Angle);
			}
			double dAngleStep = 0.5;
			int dAngleStepCount = static_cast<int>(fabs(dArcAngle / dAngleStep) + 0.5);

			double dSumAngle = 0.0;
			for (int i = 0; i < dAngleStepCount; i++)
			{
				dSumAngle += dAngleStep;

				fptInnerPoint = CDrawMakePoint::Rotate(fptStart, fptCenter, dSumAngle);
				vecArcPoint.push_back(fptInnerPoint);
			}
		}
		vecArcPoint.push_back(fptEnd);


	}
	catch (...)
	{
		TRACE(_T("Error : MakeArc_Point\n"));
	}

	return vecArcPoint;
}


vector<GEODRAW> CMakeRegionFunction::_SetMirrorRotate(vector<GEODRAW> &vecGeoData, CRotMirrArr *pRotMirrArr, IN const double &dAngle)
{
	vector<GEODRAW> vecNewGeoData = vecGeoData;

	if (pRotMirrArr == nullptr) return vecNewGeoData;

	int nDataCount = static_cast<int>(vecGeoData.size());
	if (nDataCount <= 0) return vecNewGeoData;

	double dSrcX, dSrcY;
	double dDstX, dDstY;
	for (int i = 0; i < nDataCount; i++)
	{
		dSrcX = vecGeoData[i].ptPoint.x;
		dSrcY = vecGeoData[i].ptPoint.y;
		pRotMirrArr->FinalPoint(&dDstX, &dDstY, dSrcX, dSrcY);

		vecNewGeoData[i].ptPoint.x = static_cast<float>(dDstX);
		vecNewGeoData[i].ptPoint.y = static_cast<float>(dDstY);

		if (vecGeoData[i].nType = OdbPointType::enumArc)
		{
			dSrcX = vecGeoData[i].ptCenter.x;
			dSrcY = vecGeoData[i].ptCenter.y;
			pRotMirrArr->FinalPoint(&dDstX, &dDstY, dSrcX, dSrcY);

			vecNewGeoData[i].ptCenter.x = static_cast<float>(dDstX);
			vecNewGeoData[i].ptCenter.y = static_cast<float>(dDstY);

			enum Orient eFinalOrient = pRotMirrArr->FinalOrient();

			if (eFinalOrient >= Orient::Mir0Deg &&
				eFinalOrient <= Orient::Mir270Deg)
			{
				enum eDirectionType eDirection = pRotMirrArr->FinalDirection();

				if (eDirection == eDirectionType::eHorizontal ||
					eDirection == eDirectionType::eVertical)
				{
					vecNewGeoData[i].bCw = !vecNewGeoData[i].bCw;
				}

			}

// 			enum Orient eFinalOrient = pRotMirrArr->FinalOrient();
// 			if (eFinalOrient >= Orient::Mir0Deg)
// 				vecNewGeoData[i].bCw = !vecNewGeoData[i].bCw;
		}

	}

	if (dAngle != 0.0)
	{
		for (int i = 0; i < nDataCount; i++)
		{
			double dTempX = vecNewGeoData[i].ptPoint.x;
			double dTempY = vecNewGeoData[i].ptPoint.y;

			vecNewGeoData[i].ptPoint.x = static_cast<float>(dTempX * cos(-1 * PI*dAngle / 180.0) - dTempY * sin(-1 * PI*dAngle / 180.0));
			vecNewGeoData[i].ptPoint.y = static_cast<float>(dTempX * sin(-1 * PI*dAngle / 180.0) + dTempY * cos(-1 * PI*dAngle / 180.0));

			if (vecGeoData[i].nType = OdbPointType::enumArc)
			{
				double dTempX = vecGeoData[i].ptCenter.x;
				double dTempY = vecGeoData[i].ptCenter.y;

				vecNewGeoData[i].ptCenter.x = static_cast<float>(dTempX * cos(-1 * PI*dAngle / 180.0) - dTempY * sin(-1 * PI*dAngle / 180.0));
				vecNewGeoData[i].ptCenter.y = static_cast<float>(dTempX * sin(-1 * PI*dAngle / 180.0) + dTempY * cos(-1 * PI*dAngle / 180.0));
			}
		}

	}

	return vecNewGeoData;
}

vector<RegionData> CMakeRegionFunction::_DrawTpFeature(IN CFeatureZ* pFeatureZ, IN const CSymbol* pSymbol,
	CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{

	vector<RegionData> vecRegionData;

	if (pFeatureZ == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakeTp(pFeatureZ, pSymbol, pRotMirrArr);
	// _MakeLine(pFeatureL, pSymbol, pRotMirrArr, dResolution);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	float fPenWidth = 1.0;
	HStrokeStyle eStrokeStyle;
	UINT32 nRet = _GetLineDrawInfo(pSymbol, fPenWidth, eStrokeStyle);
	fPenWidth /= static_cast<float>(dResolution);

	MakeRegion_Line(vecFeaureData, fPenWidth, eStrokeStyle, vecRegionData);

	return vecRegionData;

// 	vector<RegionData> vecRegionData;
// 
// 	if (pFeatureZ == nullptr) return vecRegionData;
// 	if (pSymbol == nullptr) return vecRegionData;
// 
// 	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakeTp(pFeatureZ, pSymbol, pRotMirrArr);
// 
// 	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
// 	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);
// 
// 	//Pixel ÁÂÇ¥ º¯È¯
// 	int nFeatureCount = static_cast<int>(vecFeaureData.size());
// 	for (int i = 0; i < nFeatureCount; i++)
// 	{
// 		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
// 		for (int j = 0; j < nPointCount; j++)
// 		{
// 			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
// 			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
// 			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
// 			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);
// 
// 			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
// 			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
// 			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
// 			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
// 		}
// 	}
// 
// 	MakeRegion_Line(vecFeaureData, pFeatureZ, pRotMirrArr, dResolution, vecRegionData);
// 
// 
// 	return vecRegionData;

}


vector<RegionData> CMakeRegionFunction::_DrawLineFeature(IN CFeatureL* pFeatureL, IN const CSymbol* pSymbol,
	CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureL == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakeLine(pFeatureL, pSymbol, pRotMirrArr);
		// _MakeLine(pFeatureL, pSymbol, pRotMirrArr, dResolution);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}
			
	float fPenWidth = 1.0;
	HStrokeStyle eStrokeStyle;
	UINT32 nRet = _GetLineDrawInfo(pSymbol, fPenWidth, eStrokeStyle);
	fPenWidth /= static_cast<float>(dResolution);

	MakeRegion_Line(vecFeaureData, fPenWidth, eStrokeStyle, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawArcFeature(IN CFeatureA* pFeatureA, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureA == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakeArc(pFeatureA, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	float fPenWidth = 1.0;
	HStrokeStyle eStrokeStyle;
	UINT32 nRet = _GetArcDrawInfo(pSymbol, fPenWidth, eStrokeStyle);
	fPenWidth /= static_cast<float>(dResolution);

	MakeRegion_Arc(vecFeaureData, fPenWidth, eStrokeStyle, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadFeature(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtViewRect_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;
	
	UINT32 nRet = RESULT_BAD;
	switch (pSymbol->m_eSymbolName)
	{
	case SymbolName::r:		//circle
	case SymbolName::hole:
		vecRegionData = _DrawPadCircle(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::s:		//square
	case SymbolName::rect:	//rectangle
		vecRegionData = _DrawPadRect(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::rectxr: //Rounded Rectangle  
		vecRegionData = _DrawPadRectxr(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::rectxc: //Rounded Rectangle  
		vecRegionData = _DrawPadRectxc(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::oval:	//oval  
		vecRegionData = _DrawPadOval(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::di:    //Diamond
		vecRegionData = _DrawPadDi(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::octagon:
		vecRegionData = _DrawPadOct(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::donut_r:
		vecRegionData = _DrawPadDonut_r(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::donut_s:
		vecRegionData = _DrawPadDonut_s(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::hex_l:
		vecRegionData = _DrawPadHex_l(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::hex_s:
		vecRegionData = _DrawPadHex_s(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::bfr: //Butterfly   
		vecRegionData = _DrawPadBfr(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::bfs: //SQUARE Butterfly  
		vecRegionData = _DrawPadBfs(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::tri:
		vecRegionData = _DrawPadTriangle(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::oval_h: //Half Oval   
		vecRegionData = _DrawPadOval_h(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::thr:
		vecRegionData = _DrawPadThr(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::ths:
		vecRegionData = _DrawPadThs(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::s_ths:
		vecRegionData = _DrawPadS_ths(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::s_tho:
		vecRegionData = _DrawPadS_tho(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::sr_ths:
		vecRegionData = _DrawPadSr_ths(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::rc_ths:
		vecRegionData = _DrawPadRc_ths(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::rc_tho:
		vecRegionData = _DrawPadRc_tho(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::el:	//ellipse
		vecRegionData = _DrawPadEllipse(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::moire: //Moire  
		vecRegionData = _DrawPadMoire(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	case SymbolName::userdefined://user defined symbol ÀÎ°æ¿ì
		vecRegionData = _DrawPadUserdefined(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		break;
	default:
		break;
	}

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadCircle(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadCircle(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);


	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadRect(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadRect(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadRectxr(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadRectxr(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadRectxc(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadRectxc(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadOval(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadOval(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadDi(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadDi(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadOct(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadOct(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadDonut_r(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadDonut_r(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadDonut_s(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadDonut_s(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadHex_l(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadHex_l(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadHex_s(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadHex_s(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadBfr(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadBfr(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadBfs(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadBfs(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadTriangle(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadTriangle(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadOval_h(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadOval_h(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadThr(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadThr(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadThs(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadThs(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadS_ths(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadS_ths(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadS_tho(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadS_tho(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadSr_ths(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadSr_ths(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadRc_ths(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadRc_ths(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadRc_tho(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadRc_tho(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadEllipse(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadEllipse(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadMoire(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureP == nullptr) return vecRegionData;
	if (pSymbol == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakePadMoire(pFeatureP, pSymbol, pRotMirrArr);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureP, pRotMirrArr, dResolution, vecRegionData);

	return vecRegionData;
}

vector<RegionData> CMakeRegionFunction::_DrawPadUserdefined(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr, IN RECTD &drtViewRect_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;
	
	drtViewRect_mm.NormalizeRectD();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	CFeatureFile *pChildFeatureFile = &(pSymbol->m_pUserSymbol->m_FeatureFile);
	if (pChildFeatureFile == NULL)
		return vecRegionData;

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

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
		switch (pFeatureTmp->m_eType)
		{
		case FeatureType::Z:
		{
			CFeatureZ* pFeatureZ = (CFeatureZ*)pFeatureTmp;
			pSymbol = pFeatureZ->m_pSymbol;

			vecRegionData = _DrawTpFeature(pFeatureZ, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		}

		case FeatureType::L:		//Line
		{
			CFeatureL* pFeatureL = (CFeatureL*)pFeatureTmp;
			pSymbol = pFeatureL->m_pSymbol;

			vecRegionData = _DrawLineFeature(pFeatureL, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);

		}
		break;

		case FeatureType::P:		//Pad
		{
			CFeatureP* pFeatureP = (CFeatureP*)pFeatureTmp;
			pSymbol = pFeatureP->m_pSymbol;

			vecRegionData = _DrawPadFeature(pFeatureP, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		}
		break;

		case FeatureType::A:		//Arc								
		{
			CFeatureA* pFeatureA = (CFeatureA*)pFeatureTmp;
			pSymbol = pFeatureA->m_pSymbol;

			vecRegionData = _DrawArcFeature(pFeatureA, pSymbol, pRotMirrArr, drtViewRect_mm, dResolution);
		}
		break;

		case FeatureType::S:		//Surface	
		{
			CFeatureS* pFeatureS = (CFeatureS*)pFeatureTmp;

			pSymbol = pFeatureS->m_pSymbol;
			vecRegionData = _DrawSurfaceFeature(pFeatureS, pRotMirrArr, drtViewRect_mm, dResolution);

		}
		break;
		}
	}

	int nRegionCount = static_cast<int>(vecRegionData.size());
	for (int i = 0; i < nRegionCount; i++)
	{
		vecRegionData[i].bPolarity = pFeatureP->m_bPolarity == true ? vecRegionData[i].bPolarity : !vecRegionData[i].bPolarity;
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

	return vecRegionData;
}


vector<RegionData> CMakeRegionFunction::_DrawSurfaceFeature(IN CFeatureS* pFeatureS, CRotMirrArr *pRotMirrArr, RECTD drtImage_mm, IN const double &dResolution)
{
	vector<RegionData> vecRegionData;

	if (pFeatureS == nullptr) return vecRegionData;

	vector<MakeFeatureData> vecFeaureData = CDrawMakePoint::MakeSurface(pFeatureS, pRotMirrArr, drtImage_mm);

	int nMoveX_Pixel = static_cast<int>(-1.0 * drtImage_mm.left / dResolution + 0.5);
	int nMoveY_Pixel = static_cast<int>(-1.0 * drtImage_mm.top / dResolution + 0.5);

	//Pixel ÁÂÇ¥ º¯È¯
	int nFeatureCount = static_cast<int>(vecFeaureData.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nPointCount = static_cast<int>(vecFeaureData[i].vecPointData.size());
		for (int j = 0; j < nPointCount; j++)
		{
			vecFeaureData[i].vecPointData[j].ptPoint.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptPoint.y /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.x /= static_cast<float>(dResolution);
			vecFeaureData[i].vecPointData[j].ptCenter.y /= static_cast<float>(dResolution);

			vecFeaureData[i].vecPointData[j].ptPoint.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptPoint.y += nMoveY_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.x += nMoveX_Pixel;
			vecFeaureData[i].vecPointData[j].ptCenter.y += nMoveY_Pixel;
		}
	}

	MakeRegion_Pad(vecFeaureData, pFeatureS, pRotMirrArr, dResolution, vecRegionData);


	return vecRegionData;
}