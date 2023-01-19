
#include "pch.h"
#include "CDrawMakePoint.h"



double CDrawMakePoint::GetAngle(D2D_POINT_2F Point1, D2D_POINT_2F Point2)
{
	double Angle = ToDegree(atan2(Point2.y - Point1.y, Point2.x - Point1.x));
	if (Angle < 0)
		Angle += 360;
	return Angle;
}

double CDrawMakePoint::GetAngle(IN const double &dSx, IN const double &dSy, IN const double &dEx, IN const double &dEy)
{
	double Angle = ToDegree(atan2(dEy - dSy, dEx - dSx));
	if (Angle < 0)
		Angle += 360;
	return Angle;
}

double CDrawMakePoint::GetAngle(IN const float &fSx, IN const float &fSy, IN const float &fEx, IN const float &fEy)
{
	double Angle = ToDegree(atan2(fEy - fSy, fEx - fSx));
	if (Angle < 0)
		Angle += 360;
	return Angle;
}

D2D_POINT_2F CDrawMakePoint::Rotate(IN const D2D_POINT_2F &fptOrg, IN const D2D_POINT_2F &fptRotateCenter, IN const double &dAngle)
{
	D2D_POINT_2F fptRotated;

	D2D_POINT_2F fptTransOrg;
	fptTransOrg.x = fptOrg.x - fptRotateCenter.x;
	fptTransOrg.y = fptOrg.y - fptRotateCenter.y;

	fptRotated.x = static_cast<float>(fptTransOrg.x * cos(-1 * PI*dAngle / 180.0) - fptTransOrg.y * sin(-1 * PI*dAngle / 180.0)) + fptRotateCenter.x;
	fptRotated.y = static_cast<float>(fptTransOrg.x * sin(-1 * PI*dAngle / 180.0) + fptTransOrg.y * cos(-1 * PI*dAngle / 180.0)) + fptRotateCenter.y;

	return fptRotated;
}

double CDrawMakePoint::GetDistance(D2D_POINT_2F Point1, D2D_POINT_2F Point2)
{
	double dDist = 0.0;

	dDist = sqrt(pow(Point1.x - Point2.x, 2.0) + pow(Point1.y - Point2.y, 2.0));

	return dDist;
}

double CDrawMakePoint::GetDistance(CPointD dptPoint1, CPointD dptPoint2)
{
	double dDist = 0.0;

	dDist = sqrt(pow(dptPoint1.x - dptPoint2.x, 2.0) + pow(dptPoint1.y - dptPoint2.y, 2.0));

	return dDist;
}

double CDrawMakePoint::GetDistance_X(D2D_POINT_2F Point1, D2D_POINT_2F Point2)
{
	double dDist = 0.0;

	dDist = fabs(Point1.x - Point2.x);

	return dDist;
}

void		CDrawMakePoint::GetLine(IN const double &dSx, IN const double &dSy, IN const double &dEx, IN const double &dEy, OUT _Line2 &stLine)
{
	stLine = _Line2();//초기화

	double dAngle = GetAngle(dSx, dSy, dEx, dEy);
	double dCenterX = (dSx + dEx) / 2.;
	double dCenterY = (dSy + dEy) / 2.;

	stLine.dAngle = dAngle;
	stLine.fptCenter.x = static_cast<float>(dCenterX);
	stLine.fptCenter.y = static_cast<float>(dCenterY);
}

BOOL		CDrawMakePoint::GetCrossLine_LNtoPT(IN const _Line2& stLine, const D2D_POINT_2F& fptPoint, OUT _Line2& stCrossLine)
{
	stCrossLine = _Line2();//초기화

	// slope1 * x - y + fIntercept = 0;
	float fSlope = static_cast<float>(tan(stLine.dAngle*3.1415f / 180.f));

	if (fSlope == 0.) return FALSE;

	// 직교 -x + fslope1 * y + c = 0;  slope2 = 1/slope1 = tan(angle)
	//angle = tan-1(1/slope1) * 180.f / 3.1415f;

	stCrossLine.fptCenter.x = fptPoint.x;
	stCrossLine.fptCenter.y = fptPoint.y;
	stCrossLine.dAngle = -atan(1.f / fSlope) * 180.f / 3.1415f;

	return TRUE;
}

BOOL		CDrawMakePoint::GetCrossPoint_LNtoLN(IN const _Line2& stLine1, const _Line2& stLine2, OUT D2D_POINT_2F& fptCross)
{
	// ax - y + b = 0
	float fSlope1 = static_cast<float>(tan(stLine1.dAngle*3.1415f / 180.f));
	float fIntercept1 = -fSlope1 * stLine1.fptCenter.x + stLine1.fptCenter.y;

	float fSlope2 = static_cast<float>(tan(stLine2.dAngle*3.1415f / 180.f));
	float fIntercept2 = -fSlope2 * stLine2.fptCenter.x + stLine2.fptCenter.y;

	float det = -fSlope1 + fSlope2;

	if (fabs(det) < 1e-10)
		return FALSE;

	fptCross.x = (-fIntercept2 + fIntercept1) / det;
	fptCross.y = (fSlope2*fIntercept1 - fSlope1 * fIntercept2) / det;

	return TRUE;
}

BOOL		CDrawMakePoint::GetCrossPoint_LNtoPT(IN const _Line2& eLine, const D2D_POINT_2F& ePoint, OUT D2D_POINT_2F& eCrossPoint)
{
	_Line2 stCrossLine;
	
	if (GetCrossLine_LNtoPT(eLine, ePoint, stCrossLine) == FALSE)			return FALSE;
	if (GetCrossPoint_LNtoLN(eLine, stCrossLine, eCrossPoint) == FALSE)	return FALSE;
	
	return TRUE;
}

vector<GEODRAW> CDrawMakePoint::SetMirrorRotate(vector<GEODRAW> &vecGeoData, CRotMirrArr *pRotMirrArr, IN const double &dAngle)
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

		if (vecGeoData[i].nType == OdbPointType::enumArc)
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

			if (vecGeoData[i].nType == OdbPointType::enumArc)
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


vector<MakeFeatureData> CDrawMakePoint::MakeTp(IN  CFeatureZ* pFeatureZ, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureZ == nullptr) return vector<MakeFeatureData>();
		
	//float fDist = 0.3f;

	//D2D1_POINT_2F fptCenter = D2D1::Point2F(static_cast<float>(pFeatureZ->m_dX), static_cast<float>(pFeatureZ->m_dY));
	//D2D1_POINT_2F fptRight = D2D1::Point2F(static_cast<float>(pFeatureZ->m_dX)+ fDist, static_cast<float>(pFeatureZ->m_dY));
	//D2D1_POINT_2F fptLeft = D2D1::Point2F(static_cast<float>(pFeatureZ->m_dX)- fDist, static_cast<float>(pFeatureZ->m_dY));
	//D2D1_POINT_2F fptTop = D2D1::Point2F(static_cast<float>(pFeatureZ->m_dX), static_cast<float>(pFeatureZ->m_dY)- fDist);
	//D2D1_POINT_2F fptBottom = D2D1::Point2F(static_cast<float>(pFeatureZ->m_dX), static_cast<float>(pFeatureZ->m_dY)+ fDist);
		
	double dRadius = 4.75 / 2;	

	D2D1_POINT_2F fptCenter = D2D1::Point2F(static_cast<float>(pFeatureZ->m_dX), static_cast<float>(pFeatureZ->m_dY));
	D2D1_POINT_2F fptStart = D2D1::Point2F(DoubleToFloat(fptCenter.x - dRadius), fptCenter.y);
	D2D1_POINT_2F fptEnd = D2D1::Point2F(DoubleToFloat(fptCenter.x + dRadius), fptCenter.y);


	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;
	
	stGeoDrawTemp.SetBasePoint(fptStart.x, fptStart.y);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.SetArc(fptEnd.x, fptEnd.y, TRUE, fptCenter.x, fptCenter.y);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.SetArc(fptStart.x, fptStart.y, TRUE, fptCenter.x, fptCenter.y);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);


	// 2022.07.28 KJH ADD
	// 십자가 원으로 변경 위해 주석처리 
	/*
	//Surface 같은 십자가 정의
	float fWidth_hs = fabs(fptCenter.x - fptLeft.x);
	float fCrossWidth = fWidth_hs * 0.66f;//십자가 변의 길이
	float fCrossWidth_Sub = fWidth_hs * 0.33f;//십자가 변의 길이_Sub
	///

// 	1
// 		stGeoDrawTemp.nType = OdbPointType::enumBase;
// 		stGeoDrawTemp.ptPoint = D2D1::Point2F(fptCenter.x - fCrossWidth_Sub, fptCenter.y + fWidth_hs);
// 		vecGeoDraw.push_back(stGeoDrawTemp);
// 		stGeoDrawTemp._Reset();
// 	
// 		//2
// 		stGeoDrawTemp.nType = OdbPointType::enumLine;
// 		stGeoDrawTemp.ptPoint = D2D1::Point2F(fptCenter.x + fCrossWidth_Sub, fptCenter.y + fWidth_hs);
// 		vecGeoDraw.push_back(stGeoDrawTemp);
// 		stGeoDrawTemp._Reset();
// 	
// 		//3
// 		stGeoDrawTemp.nType = OdbPointType::enumLine;
// 		stGeoDrawTemp.ptPoint = D2D1::Point2F(fptCenter.x + fCrossWidth_Sub, fptCenter.y + fCrossWidth_Sub);
// 		vecGeoDraw.push_back(stGeoDrawTemp);
// 		stGeoDrawTemp._Reset();
// 	
// 		//4
// 		stGeoDrawTemp.nType = OdbPointType::enumLine;
// 		stGeoDrawTemp.ptPoint = D2D1::Point2F(fptCenter.x + fWidth_hs, fptCenter.y + fCrossWidth_Sub);
// 		vecGeoDraw.push_back(stGeoDrawTemp);
// 		stGeoDrawTemp._Reset();
// 	
// 		//5
// 		stGeoDrawTemp.nType = OdbPointType::enumLine;
// 		stGeoDrawTemp.ptPoint = D2D1::Point2F(fptCenter.x + fWidth_hs, fptCenter.y - fCrossWidth_Sub);
// 		vecGeoDraw.push_back(stGeoDrawTemp);
// 		stGeoDrawTemp._Reset();
// 	
// 		//6
// 		stGeoDrawTemp.nType = OdbPointType::enumLine;
// 		stGeoDrawTemp.ptPoint = D2D1::Point2F(fptCenter.x + fCrossWidth_Sub, fptCenter.y - fCrossWidth_Sub);
// 		vecGeoDraw.push_back(stGeoDrawTemp);
// 		stGeoDrawTemp._Reset();
// 	
// 		//7
// 		stGeoDrawTemp.nType = OdbPointType::enumLine;
// 		stGeoDrawTemp.ptPoint = D2D1::Point2F(fptCenter.x + fCrossWidth_Sub, fptCenter.y - fWidth_hs);
// 		vecGeoDraw.push_back(stGeoDrawTemp);
// 		stGeoDrawTemp._Reset();
// 	
// 		//8
// 		stGeoDrawTemp.nType = OdbPointType::enumLine;
// 		stGeoDrawTemp.ptPoint = D2D1::Point2F(fptCenter.x - fCrossWidth_Sub, fptCenter.y - fWidth_hs);
// 		vecGeoDraw.push_back(stGeoDrawTemp);
// 		stGeoDrawTemp._Reset();
// 	
// 		//9
// 		stGeoDrawTemp.nType = OdbPointType::enumLine;
// 		stGeoDrawTemp.ptPoint = D2D1::Point2F(fptCenter.x - fCrossWidth_Sub, fptCenter.y - fCrossWidth_Sub);
// 		vecGeoDraw.push_back(stGeoDrawTemp);
// 		stGeoDrawTemp._Reset();
// 	
// 		//10
// 		stGeoDrawTemp.nType = OdbPointType::enumLine;
// 		stGeoDrawTemp.ptPoint = D2D1::Point2F(fptCenter.x - fWidth_hs, fptCenter.y - fCrossWidth_Sub);
// 		vecGeoDraw.push_back(stGeoDrawTemp);
// 		stGeoDrawTemp._Reset();
// 	
// 		//11
// 		stGeoDrawTemp.nType = OdbPointType::enumLine;
// 		stGeoDrawTemp.ptPoint = D2D1::Point2F(fptCenter.x - fWidth_hs, fptCenter.y + fCrossWidth_Sub);
// 		vecGeoDraw.push_back(stGeoDrawTemp);
// 		stGeoDrawTemp._Reset();
// 	
// 		//12
// 		stGeoDrawTemp.nType = OdbPointType::enumLine;
// 		stGeoDrawTemp.ptPoint = D2D1::Point2F(fptCenter.x - fCrossWidth_Sub, fptCenter.y + fCrossWidth_Sub);
// 		vecGeoDraw.push_back(stGeoDrawTemp);
// 		stGeoDrawTemp._Reset();
// 	
// 		//13
// 		stGeoDrawTemp.nType = OdbPointType::enumLine;
// 		stGeoDrawTemp.ptPoint = D2D1::Point2F(fptCenter.x - fCrossWidth_Sub, fptCenter.y + fWidth_hs);
// 		vecGeoDraw.push_back(stGeoDrawTemp);
// 		stGeoDrawTemp._Reset();


	//Tp Circle 변경 22.07.28 DJ.J 

	
	stGeoDrawTemp.SetBasePoint(-1.0f * fRad, 0);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.SetArc(fRad, 0, TRUE, 0, 0);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.SetArc(-1.0f * fRad, 0, TRUE, 0, 0);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();


	//8
	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptCenter.x - fCrossWidth_Sub, fptCenter.y - fWidth_hs);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	//9
	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptCenter.x - fCrossWidth_Sub, fptCenter.y - fCrossWidth_Sub);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	//10
	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptCenter.x - fWidth_hs, fptCenter.y - fCrossWidth_Sub);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	//11
	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptCenter.x - fWidth_hs, fptCenter.y + fCrossWidth_Sub);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	//12
	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptCenter.x - fCrossWidth_Sub, fptCenter.y + fCrossWidth_Sub);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	//13
	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptCenter.x - fCrossWidth_Sub, fptCenter.y + fWidth_hs);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();
	*/

	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
	vecFeatureData[0].bPolarity = pFeatureZ->m_bPolarity;

	

	return vecFeatureData;

}

vector<MakeFeatureData> CDrawMakePoint::MakeLine(IN CFeatureL* pFeatureL, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureL == nullptr) return vector<MakeFeatureData>();

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);

	D2D1_POINT_2F fptStart = D2D1::Point2F(static_cast<float>(pFeatureL->m_dXs), static_cast<float>(pFeatureL->m_dYs));
	D2D1_POINT_2F fptEnd = D2D1::Point2F(static_cast<float>(pFeatureL->m_dXe), static_cast<float>(pFeatureL->m_dYe));

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;

	stGeoDrawTemp.nType = OdbPointType::enumBase;//좌상 Round 시작
	stGeoDrawTemp.ptPoint = fptStart;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;//좌상 Round 시작
	stGeoDrawTemp.ptPoint = fptEnd;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
	vecFeatureData[0].bPolarity = pFeatureL->m_bPolarity;

	return vecFeatureData;
}


vector<MakeFeatureData> CDrawMakePoint::MakePadCircle(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	double dRadius = pSymbol->m_vcParams[0] / 2.0;

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;
	stGeoDrawTemp.SetBasePoint(-1.0f * dRadius, 0);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.SetArc(dRadius, 0, TRUE, 0, 0);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.SetArc(-1.0f * dRadius, 0, TRUE, 0, 0);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);

	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr/*, pSymbol->m_dRotatedDeg*/);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}

vector<MakeFeatureData> CDrawMakePoint::MakePadRect(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	double dHalfWidth;
	double dHalfHeight;
	if (pSymbol->m_eSymbolName == SymbolName::s)	//sqare
	{
		dHalfWidth = pSymbol->m_vcParams[0] / 2.0;
		dHalfHeight = pSymbol->m_vcParams[0] / 2.0;
	}
	else
	{//rect
		dHalfWidth = pSymbol->m_vcParams[0] / 2.0;
		dHalfHeight = pSymbol->m_vcParams[1] / 2.0;
	}

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

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);

	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr/*, pSymbol->m_dRotatedDeg*/);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}

vector<MakeFeatureData> CDrawMakePoint::MakePadRectxr(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	double dHalfWidth;
	double dHalfHeight;
	if (pSymbol->m_eSymbolName == SymbolName::s)	//sqare
	{
		dHalfWidth = pSymbol->m_vcParams[0] / 2.0;
		dHalfHeight = pSymbol->m_vcParams[0] / 2.0;
	}
	else
	{//rect
		dHalfWidth = pSymbol->m_vcParams[0] / 2.0;
		dHalfHeight = pSymbol->m_vcParams[1] / 2.0;
	}

	double dRadius = pSymbol->m_vcParams[2];
	CString strRectMode;
	strRectMode.Format(_T("%d"), (int)(pSymbol->m_vcParams[3]));

	//
	D2D1_POINT_2F fptRectCenter = D2D1::Point2F(static_cast<float>(0), static_cast<float>(0));
	//기존 Rect Point(꼭지점)
	D2D1_POINT_2F fptLeftTop = D2D1::Point2F(static_cast<float>(-1.0f * dHalfWidth), static_cast<float>(-1.0f * dHalfHeight));
	D2D1_POINT_2F fptRightTop = D2D1::Point2F(static_cast<float>(dHalfWidth), static_cast<float>(-1.0f * dHalfHeight));
	D2D1_POINT_2F fptRightBot = D2D1::Point2F(static_cast<float>(dHalfWidth), static_cast<float>(dHalfHeight));
	D2D1_POINT_2F fptLeftBot = D2D1::Point2F(static_cast<float>(-1.0f * dHalfWidth), static_cast<float>(dHalfHeight));

	D2D1_POINT_2F fptLeftTop_r1 = D2D1::Point2F(static_cast<float>(fptLeftTop.x), static_cast<float>(fptLeftTop.y + dRadius));//좌상 r 시작
	D2D1_POINT_2F fptLeftTop_r2 = D2D1::Point2F(static_cast<float>(fptLeftTop.x + dRadius), static_cast<float>(fptLeftTop.y));//좌상 r 끝
	D2D1_POINT_2F fptLeftTop_rCenter = D2D1::Point2F(static_cast<float>(fptLeftTop.x + dRadius), static_cast<float>(fptLeftTop.y + dRadius));

	D2D1_POINT_2F fptRightTop_r1 = D2D1::Point2F(static_cast<float>(fptRightTop.x - dRadius), static_cast<float>(fptRightTop.y));//우상 r 시작
	D2D1_POINT_2F fptRightTop_r2 = D2D1::Point2F(static_cast<float>(fptRightTop.x), static_cast<float>(fptRightTop.y + dRadius));//우상 r 끝
	D2D1_POINT_2F fptRightTop_rCenter = D2D1::Point2F(static_cast<float>(fptRightTop.x - dRadius), static_cast<float>(fptRightTop.y + dRadius));

	D2D1_POINT_2F fptRightBot_r1 = D2D1::Point2F(static_cast<float>(fptRightBot.x), static_cast<float>(fptRightBot.y - dRadius));//우하 r 시작
	D2D1_POINT_2F fptRightBot_r2 = D2D1::Point2F(static_cast<float>(fptRightBot.x - dRadius), static_cast<float>(fptRightBot.y));//우하 r 끝
	D2D1_POINT_2F fptRightBot_rCenter = D2D1::Point2F(static_cast<float>(fptRightBot.x - dRadius), static_cast<float>(fptRightBot.y - dRadius));

	D2D1_POINT_2F fptLeftBot_r1 = D2D1::Point2F(static_cast<float>(fptLeftBot.x + dRadius), static_cast<float>(fptLeftBot.y));//좌하 r 시작
	D2D1_POINT_2F fptLeftBot_r2 = D2D1::Point2F(static_cast<float>(fptLeftBot.x), static_cast<float>(fptLeftBot.y - dRadius));//좌하 r 끝  // (+) -> (-)
	D2D1_POINT_2F fptLeftBot_rCenter = D2D1::Point2F(static_cast<float>(fptLeftBot.x + dRadius), static_cast<float>(fptLeftBot.y - dRadius));

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;

	stGeoDrawTemp.nType = OdbPointType::enumBase;//좌상 Round 시작
	stGeoDrawTemp.ptPoint = fptRightBot_r1;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();


	// 1 = 우상 
	if (strRectMode.Find('1') != -1 ||	
		strRectMode.Find('0') != -1)

	{// 우상

		stGeoDrawTemp.nType = OdbPointType::enumLine;//우하 Round 시작
		stGeoDrawTemp.ptPoint = fptRightBot_r1;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumArc;//우하 Round 끝
		stGeoDrawTemp.ptPoint = fptRightBot_r2;
		stGeoDrawTemp.ptCenter = fptRightBot_rCenter;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}
	else
	{
		stGeoDrawTemp.nType = OdbPointType::enumLine;//우하 꼭지점
		stGeoDrawTemp.ptPoint = fptRightBot;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}
	


	//2 = 좌상 
	if (strRectMode.Find('2') != -1 ||
		strRectMode.Find('0') != -1)
	{//좌상
		stGeoDrawTemp.nType = OdbPointType::enumLine;//좌하 Round 시작
		stGeoDrawTemp.ptPoint = fptLeftBot_r1;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumArc;//좌하 Round 끝
		stGeoDrawTemp.ptPoint = fptLeftBot_r2;
		stGeoDrawTemp.ptCenter = fptLeftBot_rCenter;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}
	else
	{
		stGeoDrawTemp.nType = OdbPointType::enumLine;//우하 꼭지점
		stGeoDrawTemp.ptPoint = fptLeftBot;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}

	

	// 3 = 좌하  
	if (strRectMode.Find('3') != -1 ||
		strRectMode.Find('0') != -1)
	{//좌하 
		stGeoDrawTemp.nType = OdbPointType::enumLine;//좌상 Round 시작
		stGeoDrawTemp.ptPoint = fptLeftTop_r1;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumArc;//좌상 Round 끝
		stGeoDrawTemp.ptPoint = fptLeftTop_r2;
		stGeoDrawTemp.ptCenter = fptLeftTop_rCenter;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}
	else
	{
		stGeoDrawTemp.nType = OdbPointType::enumLine;//좌상 꼭지점
		stGeoDrawTemp.ptPoint = fptLeftTop;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}

	// 4 = 우하
	if (strRectMode.Find('4') != -1 ||
		strRectMode.Find('0') != -1)
	{//우하

		stGeoDrawTemp.nType = OdbPointType::enumLine;//우상 Round 시작
		stGeoDrawTemp.ptPoint = fptRightTop_r1;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumArc;//우상 Round 끝
		stGeoDrawTemp.ptPoint = fptRightTop_r2;
		stGeoDrawTemp.ptCenter = fptRightTop_rCenter;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

	}
	else
	{
		stGeoDrawTemp.nType = OdbPointType::enumLine;//우상 꼭지점
		stGeoDrawTemp.ptPoint = fptRightTop;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}

	
	stGeoDrawTemp.nType = OdbPointType::enumLine;//시작점 좌상//다시 돌아옴
	stGeoDrawTemp.ptPoint = fptRightBot_r1;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();


	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);

	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr/*, pSymbol->m_dRotatedDeg*/);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}

vector<MakeFeatureData> CDrawMakePoint::MakePadRectxc(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	double dHalfWidth;
	double dHalfHeight;
	if (pSymbol->m_eSymbolName == SymbolName::s)	//sqare
	{
		dHalfWidth = pSymbol->m_vcParams[0] / 2.0;
		dHalfHeight = pSymbol->m_vcParams[0] / 2.0;
	}
	else
	{//rect
		dHalfWidth = pSymbol->m_vcParams[0] / 2.0;
		dHalfHeight = pSymbol->m_vcParams[1] / 2.0;
	}

	double dRadius = pSymbol->m_vcParams[2];
	CString strRectMode;
	strRectMode.Format(_T("%d"), (int)(pSymbol->m_vcParams[3]));

	//
	D2D1_POINT_2F fptRectCenter = D2D1::Point2F(static_cast<float>(0), static_cast<float>(0));
	//기존 Rect Point(꼭지점)
	D2D1_POINT_2F fptLeftTop = D2D1::Point2F(static_cast<float>(-1.0f * dHalfWidth), static_cast<float>(-1.0f * dHalfHeight));
	D2D1_POINT_2F fptRightTop = D2D1::Point2F(static_cast<float>(dHalfWidth), static_cast<float>(-1.0f * dHalfHeight));
	D2D1_POINT_2F fptRightBot = D2D1::Point2F(static_cast<float>(dHalfWidth), static_cast<float>(dHalfHeight));
	D2D1_POINT_2F fptLeftBot = D2D1::Point2F(static_cast<float>(-1.0f * dHalfWidth), static_cast<float>(dHalfHeight));

	D2D1_POINT_2F fptLeftTop_r1 = D2D1::Point2F(static_cast<float>(fptLeftTop.x), static_cast<float>(fptLeftTop.y + dRadius));//좌상 cut 시작
	D2D1_POINT_2F fptLeftTop_r2 = D2D1::Point2F(static_cast<float>(fptLeftTop.x + dRadius), static_cast<float>(fptLeftTop.y));//좌상 cut 끝
	D2D1_POINT_2F fptLeftTop_rCenter = D2D1::Point2F(static_cast<float>(fptLeftTop.x + dRadius), static_cast<float>(fptLeftTop.y + dRadius));

	D2D1_POINT_2F fptRightTop_r1 = D2D1::Point2F(static_cast<float>(fptRightTop.x - dRadius), static_cast<float>(fptRightTop.y));//우상 cut 시작
	D2D1_POINT_2F fptRightTop_r2 = D2D1::Point2F(static_cast<float>(fptRightTop.x), static_cast<float>(fptRightTop.y + dRadius));//우상 cut 끝
	D2D1_POINT_2F fptRightTop_rCenter = D2D1::Point2F(static_cast<float>(fptRightTop.x - dRadius), static_cast<float>(fptRightTop.y + dRadius));

	D2D1_POINT_2F fptRightBot_r1 = D2D1::Point2F(static_cast<float>(fptRightBot.x), static_cast<float>(fptRightBot.y - dRadius));//우하 cut 시작
	D2D1_POINT_2F fptRightBot_r2 = D2D1::Point2F(static_cast<float>(fptRightBot.x - dRadius), static_cast<float>(fptRightBot.y));//우하 cut 끝
	D2D1_POINT_2F fptRightBot_rCenter = D2D1::Point2F(static_cast<float>(fptRightBot.x - dRadius), static_cast<float>(fptRightBot.y - dRadius));

	D2D1_POINT_2F fptLeftBot_r1 = D2D1::Point2F(static_cast<float>(fptLeftBot.x + dRadius), static_cast<float>(fptLeftBot.y));//좌하 cut 시작
	D2D1_POINT_2F fptLeftBot_r2 = D2D1::Point2F(static_cast<float>(fptLeftBot.x), static_cast<float>(fptLeftBot.y - dRadius));//좌하 cut 끝
	D2D1_POINT_2F fptLeftBot_rCenter = D2D1::Point2F(static_cast<float>(fptLeftBot.x + dRadius), static_cast<float>(fptLeftBot.y - dRadius));

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;

	stGeoDrawTemp.nType = OdbPointType::enumBase;//좌상 cut 시작
	stGeoDrawTemp.ptPoint = fptRightBot_r1;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();


	if (strRectMode.Find('1') != -1 ||
		strRectMode.Find('0') != -1)
	{

		stGeoDrawTemp.nType = OdbPointType::enumLine;//우하 cut 시작
		stGeoDrawTemp.ptPoint = fptRightBot_r1;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;//우하 cut 끝
		stGeoDrawTemp.ptPoint = fptRightBot_r2;
		stGeoDrawTemp.ptCenter = fptRightBot_rCenter;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}
	else
	{
		stGeoDrawTemp.nType = OdbPointType::enumLine;//우하 꼭지점
		stGeoDrawTemp.ptPoint = fptRightBot;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}

	
	if (strRectMode.Find('2') != -1 ||
		strRectMode.Find('0') != -1)
	
	{//
		stGeoDrawTemp.nType = OdbPointType::enumLine;//좌하 cut 시작
		stGeoDrawTemp.ptPoint = fptLeftBot_r1;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;//좌하 cut 끝
		stGeoDrawTemp.ptPoint = fptLeftBot_r2;
		stGeoDrawTemp.ptCenter = fptLeftBot_rCenter;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}
	else
	{
		stGeoDrawTemp.nType = OdbPointType::enumLine;//우하 꼭지점
		stGeoDrawTemp.ptPoint = fptLeftBot;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}

	if (strRectMode.Find('3') != -1 ||
		strRectMode.Find('0') != -1)



	{
		stGeoDrawTemp.nType = OdbPointType::enumLine;//좌상 cut 시작
		stGeoDrawTemp.ptPoint = fptLeftTop_r1;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;//좌상 cut 끝
		stGeoDrawTemp.ptPoint = fptLeftTop_r2;
		stGeoDrawTemp.ptCenter = fptLeftTop_rCenter;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

	}
	else
	{
	stGeoDrawTemp.nType = OdbPointType::enumLine;//좌상 꼭지점
	stGeoDrawTemp.ptPoint = fptLeftTop;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();
	}


	
	if (strRectMode.Find('4') != -1 ||
		strRectMode.Find('0') != -1)
	{

		stGeoDrawTemp.nType = OdbPointType::enumLine;//우상 cut 시작
		stGeoDrawTemp.ptPoint = fptRightTop_r1;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;//우상 cut 끝
		stGeoDrawTemp.ptPoint = fptRightTop_r2;
		stGeoDrawTemp.ptCenter = fptRightTop_rCenter;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

	}
	else
	{
		stGeoDrawTemp.nType = OdbPointType::enumLine;//우상 꼭지점
		stGeoDrawTemp.ptPoint = fptRightTop;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptRightBot_r1;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);

	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr/*, pSymbol->m_dRotatedDeg*/);

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}

vector<MakeFeatureData> CDrawMakePoint::MakePadOval(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);



	D2D1_POINT_2F fptCenter = D2D1::Point2F(static_cast<float>(0), static_cast<float>(0));
	double dWidth = pSymbol->m_vcParams[0];
	double dHeight = pSymbol->m_vcParams[1];
	double	dLineWidth_hs = 0.;
	double	dLineHeight_hs = 0.;
	if (dWidth < dHeight) //width < height
	{
		//dLineWidth = dHeight - dWidth;
		dLineWidth_hs = dHeight / 2.;
		dLineHeight_hs = dWidth / 2.;
	}
	else											//width > height
	{
		dLineWidth_hs = dWidth / 2.;
		dLineHeight_hs = dHeight / 2.;
	}

	D2D1_POINT_2F fptStart;
	D2D1_POINT_2F fptEnd;

	if (dWidth < dHeight)
	{
		fptStart = D2D1::Point2F(static_cast<float>(fptCenter.x), static_cast<float>(fptCenter.y - dLineWidth_hs + dLineHeight_hs));
		fptEnd = D2D1::Point2F(static_cast<float>(fptCenter.x), static_cast<float>(fptCenter.y + dLineWidth_hs - dLineHeight_hs));
	}
	else
	{
		fptStart = D2D1::Point2F(static_cast<float>(fptCenter.x - dLineWidth_hs + dLineHeight_hs), static_cast<float>(fptCenter.y));
		fptEnd = D2D1::Point2F(static_cast<float>(fptCenter.x + dLineWidth_hs - dLineHeight_hs), static_cast<float>(fptCenter.y));
	}

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;

	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptStart;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptEnd;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);
	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr/*, pSymbol->m_dRotatedDeg*/);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}

vector<MakeFeatureData> CDrawMakePoint::MakePadDi(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	D2D1_POINT_2F fptCenter = D2D1::Point2F(static_cast<float>(0), static_cast<float>(0));
	double dHalfWidth = pSymbol->m_vcParams[0] / 2.;
	double dHalfHeight = pSymbol->m_vcParams[1] / 2.;

	D2D1_POINT_2F fptLeft = D2D1::Point2F(static_cast<float>(fptCenter.x - dHalfWidth), static_cast<float>(fptCenter.y));
	D2D1_POINT_2F fptTop = D2D1::Point2F(static_cast<float>(fptCenter.x), static_cast<float>(fptCenter.y - dHalfHeight));	//Width -> Height
	D2D1_POINT_2F fptRight = D2D1::Point2F(static_cast<float>(fptCenter.x + dHalfWidth), static_cast<float>(fptCenter.y));
	D2D1_POINT_2F fptBottom = D2D1::Point2F(static_cast<float>(fptCenter.x), static_cast<float>(fptCenter.y + dHalfHeight));	//Width -> Height

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;

	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptLeft;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptTop;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptRight;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptBottom;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptLeft;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);
	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr/*, pSymbol->m_dRotatedDeg*/);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}

vector<MakeFeatureData> CDrawMakePoint::MakePadOct(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{//RectXC와 다른점 확인 필요
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

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

	double dRadius = pSymbol->m_vcParams[2]; 
	CString strRectMode;
	strRectMode.Format(_T("%d"), (int)(pSymbol->m_vcParams[2])); // m_vcParams[3] -> m_vbParams[2]

	//
	D2D1_POINT_2F fptRectCenter = D2D1::Point2F(static_cast<float>(0), static_cast<float>(0));
	//기존 Rect Point(꼭지점)
	D2D1_POINT_2F fptLeftTop = D2D1::Point2F(static_cast<float>(fptRectCenter.x - dHalfWidth), static_cast<float>(fptRectCenter.y - dHalfHeight));
	D2D1_POINT_2F fptRightTop = D2D1::Point2F(static_cast<float>(fptRectCenter.x + dHalfWidth), static_cast<float>(fptRectCenter.y - dHalfHeight));
	D2D1_POINT_2F fptRightBot = D2D1::Point2F(static_cast<float>(fptRectCenter.x + dHalfWidth), static_cast<float>(fptRectCenter.y + dHalfHeight));
	D2D1_POINT_2F fptLeftBot = D2D1::Point2F(static_cast<float>(fptRectCenter.x - dHalfWidth), static_cast<float>(fptRectCenter.y + dHalfHeight));

	D2D1_POINT_2F fptLeftTop_r1 = D2D1::Point2F(static_cast<float>(fptLeftTop.x), static_cast<float>(fptLeftTop.y + dRadius));//좌상 cut 시작 // - -> +
	D2D1_POINT_2F fptLeftTop_r2 = D2D1::Point2F(static_cast<float>(fptLeftTop.x + dRadius), static_cast<float>(fptLeftTop.y));//좌상 cut 끝
	D2D1_POINT_2F fptLeftTop_rCenter = D2D1::Point2F(static_cast<float>(fptLeftTop.x + dRadius), static_cast<float>(fptLeftTop.y + dRadius));

	D2D1_POINT_2F fptRightTop_r1 = D2D1::Point2F(static_cast<float>(fptRightTop.x - dRadius), static_cast<float>(fptRightTop.y));//우상 cut 시작
	D2D1_POINT_2F fptRightTop_r2 = D2D1::Point2F(static_cast<float>(fptRightTop.x), static_cast<float>(fptRightTop.y + dRadius));//우상 cut 끝
	D2D1_POINT_2F fptRightTop_rCenter = D2D1::Point2F(static_cast<float>(fptRightTop.x - dRadius), static_cast<float>(fptRightTop.y + dRadius));

	D2D1_POINT_2F fptRightBot_r1 = D2D1::Point2F(static_cast<float>(fptRightBot.x), static_cast<float>(fptRightBot.y - dRadius));//우하 cut 시작
	D2D1_POINT_2F fptRightBot_r2 = D2D1::Point2F(static_cast<float>(fptRightBot.x - dRadius), static_cast<float>(fptRightBot.y));//우하 cut 끝
	D2D1_POINT_2F fptRightBot_rCenter = D2D1::Point2F(static_cast<float>(fptRightBot.x - dRadius), static_cast<float>(fptRightBot.y - dRadius));

	D2D1_POINT_2F fptLeftBot_r1 = D2D1::Point2F(static_cast<float>(fptLeftBot.x + dRadius), static_cast<float>(fptLeftBot.y));//좌하 cut 시작
	D2D1_POINT_2F fptLeftBot_r2 = D2D1::Point2F(static_cast<float>(fptLeftBot.x), static_cast<float>(fptLeftBot.y - dRadius));//좌하 cut 끝
	D2D1_POINT_2F fptLeftBot_rCenter = D2D1::Point2F(static_cast<float>(fptLeftBot.x + dRadius), static_cast<float>(fptLeftBot.y - dRadius));

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;

	stGeoDrawTemp.nType = OdbPointType::enumBase;//좌상 cut 시작
	stGeoDrawTemp.ptPoint = fptLeftTop_r1;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();


	stGeoDrawTemp.nType = OdbPointType::enumBase;//좌상 cut 시작
	stGeoDrawTemp.ptPoint = fptLeftTop_r1;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;//좌상 cut 끝
	stGeoDrawTemp.ptPoint = fptLeftTop_r2;
	stGeoDrawTemp.ptCenter = fptLeftTop_rCenter;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;//우상 cut 시작
	stGeoDrawTemp.ptPoint = fptRightTop_r1;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;//우상 cut 끝
	stGeoDrawTemp.ptPoint = fptRightTop_r2;
	stGeoDrawTemp.ptCenter = fptRightTop_rCenter;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;//우하 cut 시작
	stGeoDrawTemp.ptPoint = fptRightBot_r1;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;//우하 cut 끝
	stGeoDrawTemp.ptPoint = fptRightBot_r2;
	stGeoDrawTemp.ptCenter = fptRightBot_rCenter;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;//좌하 cut 시작
	stGeoDrawTemp.ptPoint = fptLeftBot_r1;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;//좌하 cut 끝
	stGeoDrawTemp.ptPoint = fptLeftBot_r2;
	stGeoDrawTemp.ptCenter = fptLeftBot_rCenter;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();


	stGeoDrawTemp.nType = OdbPointType::enumLine;//시작점 좌상//다시 돌아옴
	stGeoDrawTemp.ptPoint = fptLeftTop_r1;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);
	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr/*, pSymbol->m_dRotatedDeg*/);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}

vector<MakeFeatureData> CDrawMakePoint::MakePadDonut_r(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{//Pen으로 그려야함
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	D2D1_POINT_2F fptCenter = D2D1::Point2F(static_cast<float>(0.), static_cast<float>(0.));

	double dHalfWidth;
	double dHalfHeight;
	dHalfWidth = pSymbol->m_vcParams[0] / 2.0;
	dHalfHeight = pSymbol->m_vcParams[1] / 2.0;

	double dRadius = (dHalfWidth + dHalfHeight) / 2.;

	D2D1_POINT_2F fptLeft = D2D1::Point2F(static_cast<float>(fptCenter.x - dRadius), static_cast<float>(fptCenter.y));//좌 포인트
	D2D1_POINT_2F fptRight = D2D1::Point2F(static_cast<float>(fptCenter.x + dRadius), static_cast<float>(fptCenter.y));//우 포인트

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;

	stGeoDrawTemp.nType = OdbPointType::enumBase;//좌상 cut 시작
	stGeoDrawTemp.ptPoint = fptLeft;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumArc;//좌상 cut 시작
	stGeoDrawTemp.ptPoint = fptRight;
	stGeoDrawTemp.ptCenter = fptCenter;
	stGeoDrawTemp.bCw = TRUE;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumArc;//좌상 cut 시작
	stGeoDrawTemp.ptPoint = fptLeft;
	stGeoDrawTemp.ptCenter = fptCenter;
	stGeoDrawTemp.bCw = TRUE;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);
	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}

vector<MakeFeatureData> CDrawMakePoint::MakePadDonut_s(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{//펜으로 그릴것
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	D2D1_POINT_2F fptCenter = D2D1::Point2F(static_cast<float>(0.), static_cast<float>(0.));

	double dHalfWidth;
	double dHalfHeight;
	dHalfWidth = pSymbol->m_vcParams[0] / 2.0;
	dHalfHeight = pSymbol->m_vcParams[1] / 2.0;

	double dRadius = (dHalfWidth + dHalfHeight) / 2.;

	D2D1_POINT_2F fptLeftTop = D2D1::Point2F(static_cast<float>(fptCenter.x - dRadius), static_cast<float>(fptCenter.y - dRadius));//좌상 포인트
	D2D1_POINT_2F fptRightTop = D2D1::Point2F(static_cast<float>(fptCenter.x + dRadius), static_cast<float>(fptCenter.y - dRadius));//우상 포인트
	D2D1_POINT_2F fptRightBot = D2D1::Point2F(static_cast<float>(fptCenter.x + dRadius), static_cast<float>(fptCenter.y + dRadius));//우상 포인트
	D2D1_POINT_2F fptLeftBot = D2D1::Point2F(static_cast<float>(fptCenter.x - dRadius), static_cast<float>(fptCenter.y + dRadius));//우상 포인트

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;

	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptLeftTop;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptRightTop;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptRightBot;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptLeftBot;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptLeftTop;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);
	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}

vector<MakeFeatureData> CDrawMakePoint::MakePadHex_l(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;

	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[0] / 2.0f), 0.0f);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[0] / 2.0 - pSymbol->m_vcParams[2]), DoubleToFloat(pSymbol->m_vcParams[1] / 2.0));
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(-1.f * pSymbol->m_vcParams[0] / 2.0 + pSymbol->m_vcParams[2]), DoubleToFloat(pSymbol->m_vcParams[1] / 2.0f));
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(-1.f * pSymbol->m_vcParams[0] / 2.0), 0.0f);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(-1.f * pSymbol->m_vcParams[0] / 2.0f + pSymbol->m_vcParams[2]), DoubleToFloat(-1.f * pSymbol->m_vcParams[1] / 2.0f));
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[0] / 2.0f - pSymbol->m_vcParams[2]), DoubleToFloat(-1.f * pSymbol->m_vcParams[1] / 2.0f));
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[0] / 2.0f), 0.0f);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);
	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}

vector<MakeFeatureData> CDrawMakePoint::MakePadHex_s(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;

	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[0] / 2.0f), DoubleToFloat(pSymbol->m_vcParams[1] / 2.0f - pSymbol->m_vcParams[2]));
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(0.0f, DoubleToFloat(pSymbol->m_vcParams[1] / 2.0f));
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(-1.f * pSymbol->m_vcParams[0] / 2.0f), DoubleToFloat(pSymbol->m_vcParams[1] / 2.0f - pSymbol->m_vcParams[2]));
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(-1.f * pSymbol->m_vcParams[0] / 2.0f), DoubleToFloat(-1.f * pSymbol->m_vcParams[1] / 2.0f + pSymbol->m_vcParams[2]));
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(0.0f, DoubleToFloat(-1 * pSymbol->m_vcParams[1] / 2.0));
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[0] / 2.0f), DoubleToFloat(-1 * pSymbol->m_vcParams[1] / 2.0 + pSymbol->m_vcParams[2]));
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[0] / 2.0), DoubleToFloat(pSymbol->m_vcParams[1] / 2.0f - pSymbol->m_vcParams[2]));
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);
	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}

vector<MakeFeatureData> CDrawMakePoint::MakePadBfr(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{//Butter Fly
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	D2D1_POINT_2F fptCenter = D2D1::Point2F(0, 0);
	//날개 1
	D2D1_POINT_2F fptFly_1_1 = D2D1::Point2F(DoubleToFloat(-1.f * pSymbol->m_vcParams[0] / 2.0f), 0.f);
	D2D1_POINT_2F fptFly_1_2 = D2D1::Point2F(0.f, DoubleToFloat(pSymbol->m_vcParams[0] / 2.0f));
	//날개 2
	D2D1_POINT_2F fptFly_2_1 = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[0] / 2.0f), 0.f);
	D2D1_POINT_2F fptFly_2_2 = D2D1::Point2F(0.f, DoubleToFloat(-1.f * pSymbol->m_vcParams[0] / 2.0f));

	//회전 방향 설정 
	bool bTmpCw = true;
	bool bTmpCCw = false;
	enum Orient eFinalOrient = pRotMirrArr->FinalOrient();

	
	if (eFinalOrient >= Orient::Mir0Deg)
		bTmpCw = !bTmpCw;
	

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;

	//날개 2
	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptCenter;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();


	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptFly_2_1;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();


	stGeoDrawTemp.nType = OdbPointType::enumArc;
	stGeoDrawTemp.ptPoint = fptFly_2_2;
	stGeoDrawTemp.ptCenter = fptCenter;
	stGeoDrawTemp.bCw = bTmpCCw;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();


	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptCenter;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();
	
	//날개 1
	
	
	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptCenter;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();


	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptFly_1_1;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumArc;
	stGeoDrawTemp.ptPoint = fptFly_1_2;
	stGeoDrawTemp.ptCenter = fptCenter;
	stGeoDrawTemp.bCw = bTmpCCw;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();



	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptCenter;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();
	
	
	
	
	
	

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);
	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}

vector<MakeFeatureData> CDrawMakePoint::MakePadBfs(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{//Rect 모양 날개
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	D2D1_POINT_2F fptCenter = D2D1::Point2F(0, 0);

	D2D1_POINT_2F fptFly_1 = D2D1::Point2F(DoubleToFloat(-1.f * (pSymbol->m_vcParams[0]) / 2.0f), DoubleToFloat(pSymbol->m_vcParams[0] / 2.0f));
	D2D1_POINT_2F fptFly_2 = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[0] / 2.0f), DoubleToFloat((-1.f * pSymbol->m_vcParams[0]) / 2.0f));

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;

	//날개 1
	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptCenter;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptFly_1.x, fptCenter.y);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptFly_1;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptCenter.x, fptFly_1.y);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptCenter;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	//날개 2
	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptCenter;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptFly_2.x, fptCenter.y);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptFly_2;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptCenter.x, fptFly_2.y);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptCenter;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);
	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}

vector<MakeFeatureData> CDrawMakePoint::MakePadTriangle(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	D2D1_POINT_2F fptTop = D2D1::Point2F(0.0f, DoubleToFloat(pSymbol->m_vcParams[1] / 2.0f));
	D2D1_POINT_2F fptLeftBottom = D2D1::Point2F(DoubleToFloat(-1.0f *pSymbol->m_vcParams[0] / 2.0f), DoubleToFloat(-1.0f * pSymbol->m_vcParams[1] / 2.0f));
	D2D1_POINT_2F fptRightBottom = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[0] / 2.0f), DoubleToFloat(-1.0f * pSymbol->m_vcParams[1] / 2.0f));

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;

	//날개 1
	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptTop;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptLeftBottom;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptRightBottom;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptTop;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);
	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}

vector<MakeFeatureData> CDrawMakePoint::MakePadOval_h(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	bool bTmpCw = true;
	bool bTmpCCw = false;

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	// 전체적인 Code 수정 
	D2D1_POINT_2F fptCenter = D2D1::Point2F(0.f, 0.f);
	D2D1_POINT_2F fptRect_LeftTop = D2D1::Point2F(DoubleToFloat(-1.f* pSymbol->m_vcParams[0] / 2.0f), DoubleToFloat(pSymbol->m_vcParams[1] / 2.0f));
	D2D1_POINT_2F fptRect_RightBottom = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[0] / 2.0f - pSymbol->m_vcParams[1] / 2.0f), DoubleToFloat(-1.f * pSymbol->m_vcParams[1] / 2.0f));
	D2D1_POINT_2F fptElipse_Top = D2D1::Point2F(DoubleToFloat(fptRect_RightBottom.x), DoubleToFloat(pSymbol->m_vcParams[1] / 2.0f));
	D2D1_POINT_2F fptElipse_Bottom = fptRect_RightBottom;
	D2D1_POINT_2F fptElipse_Center = D2D1::Point2F( DoubleToFloat( pSymbol->m_vcParams[0]  / 2.0f - pSymbol->m_vcParams[1] / 2.0f),0.f);

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;


	stGeoDrawTemp.nType = OdbPointType::enumBase; 
	stGeoDrawTemp.ptPoint = fptRect_LeftTop;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptRect_RightBottom.x, fptRect_LeftTop.y);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumArc;
	stGeoDrawTemp.ptPoint = fptElipse_Top;
	stGeoDrawTemp.ptCenter = fptElipse_Center;
	stGeoDrawTemp.bCw = bTmpCCw;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumArc;
	stGeoDrawTemp.ptPoint = fptElipse_Bottom;
	stGeoDrawTemp.ptCenter = fptElipse_Center;
	stGeoDrawTemp.bCw = bTmpCCw;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptRect_LeftTop.x, fptRect_RightBottom.y);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptRect_LeftTop;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();


	/*
	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptRect_RightBottom;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	//Ellipse
	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptElipse_Left;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();


	stGeoDrawTemp.nType = OdbPointType::enumArc;
	stGeoDrawTemp.ptPoint = fptElipse_Right;
	stGeoDrawTemp.ptCenter = fptCenter;
	stGeoDrawTemp.bCw = TRUE;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumArc;
	stGeoDrawTemp.ptPoint = fptElipse_Left;
	stGeoDrawTemp.ptCenter = fptCenter;
	stGeoDrawTemp.bCw = TRUE;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();
	*/
	

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);
	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}

vector<MakeFeatureData> CDrawMakePoint::MakePadThr(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	D2D1_POINT_2F fptCenter = D2D1::Point2F(0.f, 0.f);

	
	
	//회전 방향 설정 
	bool bTmpCw = false;
	enum Orient eFinalOrient = Orient::NoMir0Deg;
	if (pRotMirrArr != NULL)
		eFinalOrient = pRotMirrArr->FinalOrient();

	if (eFinalOrient >= Orient::Mir0Deg)
		bTmpCw = !bTmpCw;

	D2D1_POINT_2F fptStart;
	D2D1_POINT_2F fptEnd;

	double dRadiusMM = (pSymbol->m_vcParams[0] + pSymbol->m_vcParams[1]) / 4.0;
	double dPenRadius = dRadiusMM / 2.;
	double dGapDeg = (pSymbol->m_vcParams[4] / dRadiusMM) *180.0 / PI;
	double dOriginDeg = 45 - pSymbol->m_vcParams[2];
	double dStartDeg;
	double dEndDeg;
	double dObjectDeg = 360.0 / pSymbol->m_vcParams[3] - dGapDeg - dPenRadius * 2;
	double dCurOriginDeg;

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;
	for (int i = 0; i < pSymbol->m_vcParams[3]; i++)
	{
		dCurOriginDeg = dOriginDeg + i * 90;
		dStartDeg = dCurOriginDeg - (dObjectDeg / 2.);
		dEndDeg = dStartDeg + dObjectDeg;

		fptStart.x = static_cast<float>(dRadiusMM * cos((dStartDeg)*PI / 180.0));
		fptStart.y = static_cast<float>(dRadiusMM * sin((dStartDeg)*PI / 180.0));

		fptEnd.x = static_cast<float>(dRadiusMM * cos((dEndDeg)*PI / 180.0));
 		fptEnd.y = static_cast<float>(dRadiusMM * sin((dEndDeg)*PI / 180.0));

		stGeoDrawTemp.nType = OdbPointType::enumBase;
		stGeoDrawTemp.ptPoint = fptStart;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumArc;
		stGeoDrawTemp.ptPoint = fptEnd;
		stGeoDrawTemp.ptCenter = fptCenter;
		stGeoDrawTemp.bCw = !bTmpCw;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);
	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}

vector<MakeFeatureData> CDrawMakePoint::MakePadThs(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	//OutCircle : Polarity = true
	D2D1_POINT_2F fptLargeCircle_Left = D2D1::Point2F(DoubleToFloat(-1.f * pSymbol->m_vcParams[0] / 2.0f), 0);
	D2D1_POINT_2F fptLargeCircle_Right = D2D1::Point2F(DoubleToFloat( pSymbol->m_vcParams[0] / 2.0f), 0);
	D2D1_POINT_2F fptCenter = D2D1::Point2F(0.f, 0.f);

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;

	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptLargeCircle_Left;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumArc;
	stGeoDrawTemp.ptPoint = fptLargeCircle_Right;
	stGeoDrawTemp.ptCenter = fptCenter;
	stGeoDrawTemp.bCw = TRUE;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumArc;
	stGeoDrawTemp.ptPoint = fptLargeCircle_Left;
	stGeoDrawTemp.ptCenter = fptCenter;
	stGeoDrawTemp.bCw = TRUE;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	

	//InnerCircle :hole
	D2D1_POINT_2F fptInnerCircle_Left = D2D1::Point2F(DoubleToFloat(-1.f * pSymbol->m_vcParams[1] / 2.0f), 0);
	D2D1_POINT_2F fptInnerCircle_Right = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[1] / 2.0f), 0);

	vector<GEODRAW> vecGeoDraw_Hole;

	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptInnerCircle_Left;
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumArc;
	stGeoDrawTemp.ptPoint = fptInnerCircle_Right;
	stGeoDrawTemp.ptCenter = fptCenter;
	stGeoDrawTemp.bCw = TRUE;
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumArc;
	stGeoDrawTemp.ptPoint = fptInnerCircle_Left;
	stGeoDrawTemp.ptCenter = fptCenter;
	stGeoDrawTemp.bCw = TRUE;
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();


	double dOneObjectDeg = 360.0 / pSymbol->m_vcParams[3];
	double dL = sqrt((double)2)*pSymbol->m_vcParams[0];
	double dGapRadian = atan(pSymbol->m_vcParams[4] / (2 * dL));
	double	MPointX, MPointY;
	double	P1PointX, P1PointY, P2PointX, P2PointY;
	double  P3PointX, P3PointY, P4PointX, P4PointY;

	for (int i = 0; i < pSymbol->m_vcParams[3]; i++)
	{

		MPointX = dL * cos((pSymbol->m_vcParams[2] + dOneObjectDeg * i)*PI / 180.0);
		MPointY = dL * sin((pSymbol->m_vcParams[2] + dOneObjectDeg * i)*PI / 180.0);

		P1PointX = MPointX * cos(dGapRadian) - MPointY * sin(dGapRadian);
		P1PointY = MPointX * sin(dGapRadian) + MPointY * cos(dGapRadian);

		P2PointX = MPointX * cos(-1 * dGapRadian) - MPointY * sin(-1 * dGapRadian);
		P2PointY = MPointX * sin(-1 * dGapRadian) + MPointY * cos(-1 * dGapRadian);

		P3PointX = P2PointX - MPointX;
		P3PointY = P2PointY - MPointY;

		P4PointX = P1PointX - MPointX;
		P4PointY = P1PointY - MPointY;

		stGeoDrawTemp.nType = OdbPointType::enumBase;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P4PointX), DoubleToFloat(P4PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P3PointX), DoubleToFloat(P3PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P2PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		/*stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();*/
	}

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(2);

	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	vecFeatureData[1].vecPointData = SetMirrorRotate(vecGeoDraw_Hole, pRotMirrArr);
	vecFeatureData[1].bPolarity = !pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}


vector<MakeFeatureData> CDrawMakePoint::MakePadS_ths(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	//OutRect : Polarity = true
	D2D1_POINT_2F fptLargeRect_LT = D2D1::Point2F(DoubleToFloat(-1.f * pSymbol->m_vcParams[0] / 2.0f), DoubleToFloat(pSymbol->m_vcParams[0] / 2.0f));
	D2D1_POINT_2F fptLargeRect_RB = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[0] / 2.0f), DoubleToFloat(-1.f * pSymbol->m_vcParams[0] / 2.0f));

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;

	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptLargeRect_LT;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptLargeRect_RB.x, fptLargeRect_LT.y);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptLargeRect_RB;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptLargeRect_LT.x, fptLargeRect_RB.y);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptLargeRect_LT;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	//InnerRect:hole
	D2D1_POINT_2F fptSmallRect_LT = D2D1::Point2F(DoubleToFloat(-1.f * pSymbol->m_vcParams[1] / 2.0f), DoubleToFloat(pSymbol->m_vcParams[1] / 2.0f));
	D2D1_POINT_2F fptSmallRect_RB = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[1] / 2.0f), DoubleToFloat(-1.f * pSymbol->m_vcParams[1] / 2.0f));

	vector<GEODRAW> vecGeoDraw_Hole;
	// vecGeoDraw -> vecGeoDraw_Hole 수정 
	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptSmallRect_LT;
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptSmallRect_RB.x, fptSmallRect_LT.y);
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptSmallRect_RB;
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptSmallRect_LT.x, fptSmallRect_RB.y);
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptSmallRect_LT;
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();


	
	double dOneObjectDeg = 360.0 / pSymbol->m_vcParams[3];
	double dL = sqrt((double)2)*pSymbol->m_vcParams[0];
	double dGapRadian = atan(pSymbol->m_vcParams[4] / (2 * dL));
	double	MPointX, MPointY;
	double	P1PointX, P1PointY, P2PointX, P2PointY;
	double  P3PointX, P3PointY, P4PointX, P4PointY;

	for (int i = 0; i < pSymbol->m_vcParams[3]; i++)
	{

		MPointX = dL * cos((pSymbol->m_vcParams[2] + dOneObjectDeg * i)*PI / 180.0);
		MPointY = dL * sin((pSymbol->m_vcParams[2] + dOneObjectDeg * i)*PI / 180.0);

		P1PointX = MPointX * cos(dGapRadian) - MPointY * sin(dGapRadian);
		P1PointY = MPointX * sin(dGapRadian) + MPointY * cos(dGapRadian);

		P2PointX = MPointX * cos(-1 * dGapRadian) - MPointY * sin(-1 * dGapRadian);
		P2PointY = MPointX * sin(-1 * dGapRadian) + MPointY * cos(-1 * dGapRadian);

		P3PointX = P2PointX - MPointX;
		P3PointY = P2PointY - MPointY;

		P4PointX = P1PointX - MPointX;
		P4PointY = P1PointY - MPointY;

		stGeoDrawTemp.nType = OdbPointType::enumBase;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P2PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P3PointX), DoubleToFloat(P3PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P4PointX), DoubleToFloat(P4PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}
	
	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(2);

	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	vecFeatureData[1].vecPointData = SetMirrorRotate(vecGeoDraw_Hole, pRotMirrArr);
	vecFeatureData[1].bPolarity = !pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}

vector<MakeFeatureData> CDrawMakePoint::MakePadS_tho(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	//OutRect : Polarity = true
	D2D1_POINT_2F fptLargeRect_LT = D2D1::Point2F(DoubleToFloat(-1.f * pSymbol->m_vcParams[0] / 2.0f), DoubleToFloat(pSymbol->m_vcParams[0] / 2.0f));
	D2D1_POINT_2F fptLargeRect_RB = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[0] / 2.0f), DoubleToFloat(-1.f * pSymbol->m_vcParams[0] / 2.0f));

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;

	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptLargeRect_LT;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptLargeRect_RB.x, fptLargeRect_LT.y);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptLargeRect_RB;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptLargeRect_LT.x, fptLargeRect_RB.y);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptLargeRect_LT;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	//InnerRect:hole
	D2D1_POINT_2F fptSmallRect_LT = D2D1::Point2F(DoubleToFloat(-1.f * pSymbol->m_vcParams[1] / 2.0f), DoubleToFloat(pSymbol->m_vcParams[1] / 2.0f));
	D2D1_POINT_2F fptSmallRect_RB = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[1] / 2.0f), DoubleToFloat(-1.f * pSymbol->m_vcParams[1] / 2.0f));

	vector<GEODRAW> vecGeoDraw_Hole;
	// vecGeoDraw -> vecGeoDraw_Hole 수정 // 사각형 좌표 순서 수정 
	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptSmallRect_LT;
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptSmallRect_RB.x, fptSmallRect_LT.y);
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptSmallRect_RB;
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptSmallRect_LT.x, fptSmallRect_RB.y);
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptSmallRect_LT;
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	//Corner Rect_1:hole 
	double dOffset = pSymbol->m_vcParams[4] / sqrt((double)2.0);
	double dL = pSymbol->m_vcParams[0];

	double MPointX = pSymbol->m_vcParams[1] / 2.0;
	double MPointY = pSymbol->m_vcParams[1] / 2.0;

	double P1PointX = MPointX - dOffset;
	double P1PointY = MPointY - dOffset;

	double P2PointX = P1PointX + dL;
	double P2PointY = P1PointY + dL;

	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P2PointY));
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P1PointY));
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P1PointY));
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P2PointY));
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P2PointY));
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	//2 or 4
	if (pSymbol->m_vcParams[3] > 1.0)
	{
		//좌측 하단 Corner 제거   
		MPointX = -1 * pSymbol->m_vcParams[1] / 2.0;
		MPointY = -1 * pSymbol->m_vcParams[1] / 2.0;

		P1PointX = MPointX + dOffset;
		P1PointY = MPointY + dOffset;

		P2PointX = P1PointX - dL;
		P2PointY = P1PointY - dL;

		stGeoDrawTemp.nType = OdbPointType::enumBase;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P2PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P2PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P2PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}
	if (pSymbol->m_vcParams[3] > 3.0)
	{
		//좌측 상단 Corner 제거   
		MPointX = -1 * pSymbol->m_vcParams[1] / 2.0;
		MPointY = pSymbol->m_vcParams[1] / 2.0;

		P1PointX = MPointX + dOffset;
		P1PointY = MPointY - dOffset;

		P2PointX = P1PointX - dL;
		P2PointY = P1PointY + dL;

		stGeoDrawTemp.nType = OdbPointType::enumBase;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P2PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P2PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		//우측 하단 Corner 제거   
		MPointX = pSymbol->m_vcParams[1] / 2.0;
		MPointY = -1 * pSymbol->m_vcParams[1] / 2.0;

		P1PointX = MPointX - dOffset;
		P1PointY = MPointY + dOffset;

		P2PointX = P1PointX + dL;
		P2PointY = P1PointY - dL;

		stGeoDrawTemp.nType = OdbPointType::enumBase;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P2PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P2PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(2);

	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	vecFeatureData[1].vecPointData = SetMirrorRotate(vecGeoDraw_Hole, pRotMirrArr);
	vecFeatureData[1].bPolarity = !pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}

vector<MakeFeatureData> CDrawMakePoint::MakePadSr_ths(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	//OutRect : Polarity = true
	D2D1_POINT_2F fptLargeRect_LT = D2D1::Point2F(DoubleToFloat(-1.f * pSymbol->m_vcParams[0] / 2.0f), DoubleToFloat(pSymbol->m_vcParams[0] / 2.0f));
	D2D1_POINT_2F fptLargeRect_RB = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[0] / 2.0f), DoubleToFloat(-1.f * pSymbol->m_vcParams[0] / 2.0f));

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;

	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptLargeRect_LT;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptLargeRect_RB.x, fptLargeRect_LT.y);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptLargeRect_RB;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptLargeRect_LT.x, fptLargeRect_RB.y);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptLargeRect_LT;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	//Innner Circle : Polarity = false
	D2D1_POINT_2F fptCenter = D2D1::Point2F(0.f, 0.f);
	D2D1_POINT_2F fptInnerCircle_Left = D2D1::Point2F(DoubleToFloat(-1.f * pSymbol->m_vcParams[1] / 2.0f), 0.f);
	D2D1_POINT_2F fptInnerCircle_Right = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[1] / 2.0f), 0.f);

	vector<GEODRAW> vecGeoDraw_Hole;

	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptInnerCircle_Left;
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumArc;
	stGeoDrawTemp.ptPoint = fptInnerCircle_Right;
	stGeoDrawTemp.ptCenter = fptCenter;
	stGeoDrawTemp.bCw = TRUE;
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptInnerCircle_Right;
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumArc;
	stGeoDrawTemp.ptPoint = fptInnerCircle_Left;
	stGeoDrawTemp.ptCenter = fptCenter;
	stGeoDrawTemp.bCw = TRUE;
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();
	
	
	double dOneObjectDeg = 360.0 / pSymbol->m_vcParams[3];
	double dL = sqrt((double)2)*pSymbol->m_vcParams[0];

	double dGapRadian = atan(pSymbol->m_vcParams[4] / (2* dL));

	double	MPointX, MPointY;
	double	P1PointX, P1PointY, P2PointX, P2PointY;
	double  P3PointX, P3PointY, P4PointX, P4PointY;

	for (int i = 0; i < pSymbol->m_vcParams[3]; i++)
	{
		MPointX = dL * cos((pSymbol->m_vcParams[2] + dOneObjectDeg * i)*PI / 180.0);
		MPointY = dL * sin((pSymbol->m_vcParams[2] + dOneObjectDeg * i)*PI / 180.0);

		P1PointX = MPointX * cos(dGapRadian) - MPointY * sin(dGapRadian);
		P1PointY = MPointX * sin(dGapRadian) + MPointY * cos(dGapRadian);

		P2PointX = MPointX * cos(-1 * dGapRadian) - MPointY * sin(-1 * dGapRadian);
		P2PointY = MPointX * sin(-1 * dGapRadian) + MPointY * cos(-1 * dGapRadian);

		P3PointX = P2PointX - MPointX;
		P3PointY = P2PointY - MPointY;

		P4PointX = P1PointX - MPointX;
		P4PointY = P1PointY - MPointY;

		stGeoDrawTemp.nType = OdbPointType::enumBase;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P4PointX), DoubleToFloat(P4PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P3PointX), DoubleToFloat(P3PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P2PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
		
		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
		
		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}
	
	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(2);

	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	vecFeatureData[1].vecPointData = SetMirrorRotate(vecGeoDraw_Hole, pRotMirrArr);
	vecFeatureData[1].bPolarity = !pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}

vector<MakeFeatureData> CDrawMakePoint::MakePadRc_ths(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	//OutRect : Polarity = true
	D2D1_POINT_2F fptLargeRect_LT = D2D1::Point2F(DoubleToFloat(-1.f * pSymbol->m_vcParams[0] / 2.0f),
		DoubleToFloat(pSymbol->m_vcParams[1] / 2.0f));
	D2D1_POINT_2F fptLargeRect_RB = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[0] / 2.0f),
		DoubleToFloat(-1.0f * pSymbol->m_vcParams[1] / 2.0f));

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;

	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptLargeRect_LT;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptLargeRect_RB.x, fptLargeRect_LT.y);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptLargeRect_RB;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptLargeRect_LT.x, fptLargeRect_RB.y);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptLargeRect_LT;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	D2D1_POINT_2F fptSmallRect_LT = D2D1::Point2F(DoubleToFloat(-1 * pSymbol->m_vcParams[0] / 2.0 + pSymbol->m_vcParams[5]),
		DoubleToFloat(pSymbol->m_vcParams[1] / 2.0 - pSymbol->m_vcParams[5]));
	D2D1_POINT_2F fptSmallRect_RB = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[0] / 2.0 - pSymbol->m_vcParams[5]),
		DoubleToFloat(-1 * pSymbol->m_vcParams[1] / 2.0 + pSymbol->m_vcParams[5]));

	vector<GEODRAW> vecGeoDraw_Hole;
	// vecGeoDraw -> vecGeoDraw_Hole 
	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptSmallRect_LT;
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptSmallRect_RB.x, fptSmallRect_LT.y);
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptSmallRect_RB;
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptSmallRect_LT.x, fptSmallRect_RB.y);
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptSmallRect_LT;
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	//
	double dOneObjectDeg = 90.0;//360.0/pSym->m_adParams[3]; 

	double dL = 0.00000000001;
	if (pSymbol->m_vcParams[0] >= pSymbol->m_vcParams[1])
		dL = sqrt((double)2)*pSymbol->m_vcParams[1];
	else
		dL = sqrt((double)2)*pSymbol->m_vcParams[0];

	if (dL <= 0.0)
		dL = 0.00000000001;

	double dGapRadian = atan(pSymbol->m_vcParams[4] / (2 * dL));

	double	dOffset, MPointX, MPointY;
	double	P1PointX, P1PointY, P2PointX, P2PointY;
	double  P3PointX, P3PointY, P4PointX, P4PointY;

	for (int i = 0; i < pSymbol->m_vcParams[3]; i++)
	{

		MPointX = dL * cos((pSymbol->m_vcParams[2] + dOneObjectDeg * i)*PI / 180.0);
		MPointY = dL * sin((pSymbol->m_vcParams[2] + dOneObjectDeg * i)*PI / 180.0);

		P1PointX = MPointX * cos(dGapRadian) - MPointY * sin(dGapRadian);
		P1PointY = MPointX * sin(dGapRadian) + MPointY * cos(dGapRadian);

		P2PointX = MPointX * cos(-1 * dGapRadian) - MPointY * sin(-1 * dGapRadian);
		P2PointY = MPointX * sin(-1 * dGapRadian) + MPointY * cos(-1 * dGapRadian);

		P3PointX = P2PointX - MPointX;
		P3PointY = P2PointY - MPointY;

		P4PointX = P1PointX - MPointX;
		P4PointY = P1PointY - MPointY;

		if (pSymbol->m_vcParams[0] >= pSymbol->m_vcParams[1])
		{
			dOffset = (pSymbol->m_vcParams[0] - pSymbol->m_vcParams[1]) / 2.0;
			switch (i)
			{
			case 0:
			case 3:
				P1PointX += dOffset;	P2PointX += dOffset;
				P3PointX += dOffset;	P4PointX += dOffset;
				break;
			case 1:
			case 2:
				P1PointX -= dOffset;	P2PointX -= dOffset;
				P3PointX -= dOffset;	P4PointX -= dOffset;
				break;
			}
		}
		else
		{
			dOffset = (pSymbol->m_vcParams[1] - pSymbol->m_vcParams[0]) / 2.0;
			switch (i)
			{
			case 0:
			case 1:
				P1PointY += dOffset;	P2PointY += dOffset;
				P3PointY += dOffset;	P4PointY += dOffset;
				break;
			case 2:
			case 3:
				P1PointY -= dOffset;	P2PointY -= dOffset;
				P3PointY -= dOffset;	P4PointY -= dOffset;
				break;
			}
		}

		stGeoDrawTemp.nType = OdbPointType::enumBase;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P2PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P3PointX), DoubleToFloat(P3PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P4PointX), DoubleToFloat(P4PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(2);

	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	vecFeatureData[1].vecPointData = SetMirrorRotate(vecGeoDraw_Hole, pRotMirrArr);
	vecFeatureData[1].bPolarity = !pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}

vector<MakeFeatureData> CDrawMakePoint::MakePadRc_tho(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	//OutRect : Polarity = true
	D2D1_POINT_2F fptLargeRect_LT = D2D1::Point2F(DoubleToFloat(-1.f * pSymbol->m_vcParams[0] / 2.0f),
		DoubleToFloat(pSymbol->m_vcParams[1] / 2.0f));
	D2D1_POINT_2F fptLargeRect_RB = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[0] / 2.0f),
		DoubleToFloat(-1.0f * pSymbol->m_vcParams[1] / 2.0f));

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;

	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptLargeRect_LT;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptLargeRect_RB.x, fptLargeRect_LT.y);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptLargeRect_RB;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptLargeRect_LT.x, fptLargeRect_RB.y);
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptLargeRect_LT;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	D2D1_POINT_2F fptSmallRect_LT = D2D1::Point2F(DoubleToFloat(-1 * pSymbol->m_vcParams[0] / 2.0 + pSymbol->m_vcParams[5]),
		DoubleToFloat(pSymbol->m_vcParams[1] / 2.0 - pSymbol->m_vcParams[5]));
	D2D1_POINT_2F fptSmallRect_RB = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[0] / 2.0 - pSymbol->m_vcParams[5]),
		DoubleToFloat(-1 * pSymbol->m_vcParams[1] / 2.0 + pSymbol->m_vcParams[5]));

	vector<GEODRAW> vecGeoDraw_Hole;
	// vecGeoDraw -> vecGodDraw_Hole 수정 
	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptSmallRect_LT;
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptSmallRect_RB.x, fptSmallRect_LT.y);
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptSmallRect_RB;
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(fptSmallRect_LT.x, fptSmallRect_RB.y);
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = fptSmallRect_LT;
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	double dOffset = pSymbol->m_vcParams[4] / sqrt((double)2);
	double dL = pSymbol->m_vcParams[0];

	//우측 상단 Corner 제거   // 사각형 좌표 순서 수정 
	double MPointX = pSymbol->m_vcParams[0] / 2.0 - pSymbol->m_vcParams[5];
	double MPointY = pSymbol->m_vcParams[1] / 2.0 - pSymbol->m_vcParams[5];

	double P1PointX = MPointX - dOffset;
	double P1PointY = MPointY - dOffset;

	double P2PointX = P1PointX + dL;
	double P2PointY = P1PointY + dL;

	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P2PointY));
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P1PointY));
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P1PointY));
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P2PointY));
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P2PointY));
	vecGeoDraw_Hole.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	if (pSymbol->m_vcParams[3] > 1.0)
	{
		//좌측 하단 Corner 제거  // 사각형 좌표 순서 수정    
		MPointX = -1 * pSymbol->m_vcParams[0] / 2.0 + pSymbol->m_vcParams[5];
		MPointY = -1 * pSymbol->m_vcParams[1] / 2.0 + pSymbol->m_vcParams[5];

		P1PointX = MPointX + dOffset;
		P1PointY = MPointY + dOffset;

		P2PointX = P1PointX - dL;
		P2PointY = P1PointY - dL;

		stGeoDrawTemp.nType = OdbPointType::enumBase;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P2PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P2PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P2PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}

	if (pSymbol->m_vcParams[3] > 3.0)
	{
		//좌측 상단 Corner 제거   
		MPointX = -1 * pSymbol->m_vcParams[0] / 2.0 + pSymbol->m_vcParams[5];
		MPointY = pSymbol->m_vcParams[1] / 2.0 - pSymbol->m_vcParams[5];

		P1PointX = MPointX + dOffset;
		P1PointY = MPointY - dOffset;

		P2PointX = P1PointX - dL;
		P2PointY = P1PointY + dL;

		stGeoDrawTemp.nType = OdbPointType::enumBase;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P2PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P2PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		//우측 하단 Corner 제거   
		MPointX = pSymbol->m_vcParams[0] / 2.0 - pSymbol->m_vcParams[5];
		MPointY = -1 * pSymbol->m_vcParams[1] / 2.0 + pSymbol->m_vcParams[5];

		P1PointX = MPointX - dOffset;
		P1PointY = MPointY + dOffset;

		P2PointX = P1PointX + dL;
		P2PointY = P1PointY - dL;

		stGeoDrawTemp.nType = OdbPointType::enumBase;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P2PointX), DoubleToFloat(P2PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P2PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumLine;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(P1PointX), DoubleToFloat(P1PointY));
		vecGeoDraw_Hole.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(2);

	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	vecFeatureData[1].vecPointData = SetMirrorRotate(vecGeoDraw_Hole, pRotMirrArr);
	vecFeatureData[1].bPolarity = !pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}

vector<MakeFeatureData> CDrawMakePoint::MakePadEllipse(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	D2D1_POINT_2F fptCenter = D2D1::Point2F(0.f, 0.f);
	D2D1_POINT_2F fptEllipseLeft = D2D1::Point2F(DoubleToFloat(-1.0f * pSymbol->m_vcParams[0] / 2.f), 0.f);
	D2D1_POINT_2F fptEllipseTop = D2D1::Point2F(0.f, DoubleToFloat(pSymbol->m_vcParams[1] / 2));
	D2D1_POINT_2F fptEllipseRight = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[0] / 2.f), 0.f);
	D2D1_POINT_2F fptEllipseBottom = D2D1::Point2F(0.f, DoubleToFloat(-1.0f * pSymbol->m_vcParams[1] / 2));

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;

	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptEllipseLeft;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumArc;
	stGeoDrawTemp.ptPoint = fptEllipseTop;
	stGeoDrawTemp.ptCenter = fptCenter;
	stGeoDrawTemp.bCw = TRUE;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumArc;
	stGeoDrawTemp.ptPoint = fptEllipseRight;
	stGeoDrawTemp.ptCenter = fptCenter;
	stGeoDrawTemp.bCw = TRUE;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumArc;
	stGeoDrawTemp.ptPoint = fptEllipseBottom;
	stGeoDrawTemp.ptCenter = fptCenter;
	stGeoDrawTemp.bCw = TRUE;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumArc;
	stGeoDrawTemp.ptPoint = fptEllipseLeft;
	stGeoDrawTemp.ptCenter = fptCenter;
	stGeoDrawTemp.bCw = TRUE;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);

	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}



vector<MakeFeatureData> CDrawMakePoint::MakePadUserdefined(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();


	return vector<MakeFeatureData>();
}

vector<MakeFeatureData> CDrawMakePoint::MakePadMoire(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	D2D1_POINT_2F fptCenter = D2D1::Point2F(0.f, 0.f);
	D2D1_POINT_2F fptCircleLeft;
	D2D1_POINT_2F fptCircleRight;
	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;
	
	for (int i = 0; i < static_cast<int>(pSymbol->m_vcParams[2]) ; i++)
	{
		float fRadius = static_cast<float>(pSymbol->m_vcParams[4] / 2) ;

		if (i == 0)
		{
			fRadius = DoubleToFloat(fRadius - pSymbol->m_vcParams[0] / 2);
		}

		else
		{
			fRadius = DoubleToFloat(fRadius  + pSymbol->m_vcParams[0] / 2 - static_cast<float>(pSymbol->m_vcParams[0] + pSymbol->m_vcParams[1] * (i) + pSymbol->m_vcParams[0] / 2 * i )) ;
		}


		fptCircleLeft = D2D1::Point2F(-1.0f * fRadius, 0.f);
		fptCircleRight = D2D1::Point2F(fRadius, 0.f);

		stGeoDrawTemp.nType = OdbPointType::enumBase;
		stGeoDrawTemp.ptPoint = fptCircleLeft;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumArc;
		stGeoDrawTemp.ptPoint = fptCircleRight;
		stGeoDrawTemp.ptCenter = fptCenter;
		stGeoDrawTemp.bCw = TRUE;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumArc;
		stGeoDrawTemp.ptPoint = fptCircleLeft;
		stGeoDrawTemp.ptCenter = fptCenter;
		stGeoDrawTemp.bCw = TRUE;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}

	//  회전: 점(x1,y1)을 중심점(0,0)을 기준으로 A도만큼 반시계방향으로 회전한 점(x,y)은 
	//    x = x1*cosA - y1*sinA
	//    y = x1*sinA + y1*cosA
	double dTempX = pSymbol->m_vcParams[4] / 2.0  ;
	double dTempY = 0.0;
	double dLine1SX = dTempX * cos(-1 * PI*pSymbol->m_vcParams[5] / 180.0) - dTempY * sin(-1 * PI*pSymbol->m_vcParams[5] / 180.0) - pSymbol->m_vcParams[0] / 2;
	double dLine1SY = dTempX * sin(-1 * PI*pSymbol->m_vcParams[5] / 180.0) + dTempY * cos(-1 * PI*pSymbol->m_vcParams[5] / 180.0) ;

	dTempX = -1 * pSymbol->m_vcParams[4] / 2.0;
	dTempY = 0.0; 
	double dLine1EX = dTempX * cos(-1 * PI*pSymbol->m_vcParams[5] / 180.0) - dTempY * sin(-1 * PI*pSymbol->m_vcParams[5] / 180.0) + pSymbol->m_vcParams[0]/2;
	double dLine1EY = dTempX * sin(-1 * PI*pSymbol->m_vcParams[5] / 180.0) + dTempY * cos(-1 * PI*pSymbol->m_vcParams[5] / 180.0);

	dTempX = 0.0;
	dTempY = 1 * pSymbol->m_vcParams[4] / 2.0;
	double dLine2SX = dTempX * cos(-1 * PI*pSymbol->m_vcParams[5] / 180.0) - dTempY * sin(-1 * PI*pSymbol->m_vcParams[5] / 180.0) ;
	double dLine2SY = dTempX * sin(-1 * PI*pSymbol->m_vcParams[5] / 180.0) + dTempY * cos(-1 * PI*pSymbol->m_vcParams[5] / 180.0) - pSymbol->m_vcParams[0]/2;

	dTempX = 0.0;
	dTempY = -1 * pSymbol->m_vcParams[4] / 2.0;
	double dLine2EX = dTempX * cos(-1 * PI*pSymbol->m_vcParams[5] / 180.0) - dTempY * sin(-1 * PI*pSymbol->m_vcParams[5] / 180.0) ;
	double dLine2EY = dTempX * sin(-1 * PI*pSymbol->m_vcParams[5] / 180.0) + dTempY * cos(-1 * PI*pSymbol->m_vcParams[5] / 180.0) + pSymbol->m_vcParams[0]/2;

	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(dLine1SX), DoubleToFloat(dLine1SY));
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(dLine1EX), DoubleToFloat(dLine1EY));
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(dLine2SX), DoubleToFloat(dLine2SY));
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumLine;
	stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(dLine2EX), DoubleToFloat(dLine2EY));
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);
	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;
	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;
}
vector<MakeFeatureData> CDrawMakePoint::MakePadHole(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureP == nullptr)return vector<MakeFeatureData>();
	if (pSymbol == nullptr)return vector<MakeFeatureData>();

	BOOL bMakeRotMirr = FALSE;
	if (pRotMirrArr == nullptr)
	{
		bMakeRotMirr = TRUE;
		pRotMirrArr = new CRotMirrArr();
	}

	pRotMirrArr->InputData(pFeatureP->m_dX, pFeatureP->m_dY, pFeatureP->m_eOrient);

	D2D1_POINT_2F fptCenter = D2D1::Point2F(0.f, 0.f);
	D2D1_POINT_2F fptCircle_Left = D2D1::Point2F(DoubleToFloat(-1.0f * pSymbol->m_vcParams[0] / 2.0f), 0.f);
	D2D1_POINT_2F fptCircle_Right = D2D1::Point2F(DoubleToFloat(pSymbol->m_vcParams[0]) / 2.0f, 0.f);

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;

	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptCircle_Left;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumArc;
	stGeoDrawTemp.ptPoint = fptCircle_Right;
	stGeoDrawTemp.ptCenter = fptCenter;
	stGeoDrawTemp.bCw = FALSE;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	stGeoDrawTemp.nType = OdbPointType::enumArc;
	stGeoDrawTemp.ptPoint = fptCircle_Left;
	stGeoDrawTemp.ptCenter = fptCenter;
	stGeoDrawTemp.bCw = FALSE;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);
	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
	vecFeatureData[0].bPolarity = pFeatureP->m_bPolarity;

	if (bMakeRotMirr == TRUE)
	{
		delete pRotMirrArr;
		pRotMirrArr = nullptr;
	}
	else
	{
		pRotMirrArr->RemoveTail();
	}

	return vecFeatureData;

}
vector <MakeFeatureData>CDrawMakePoint::MakeArc(IN const D2D1_POINT_2F &fptStart, IN const D2D1_POINT_2F &fptEnd, IN const D2D1_POINT_2F &fptCenter, IN const BOOL &bCW ,CRotMirrArr *pRotMirrArr)
{
	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;
	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptStart;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	if (fptStart.x == fptEnd.x &&
		fptStart.y == fptEnd.y )
	{//시작점과 끝점이 같은 원.

		D2D1_POINT_2F fptSidePoint = D2D1::Point2F(static_cast<float>(fptCenter.x * 2.0 - fptEnd.x),
			static_cast<float>(fptCenter.y * 2.0 - fptEnd.y));

		stGeoDrawTemp.nType = OdbPointType::enumArc;
		stGeoDrawTemp.ptPoint = fptSidePoint;
		stGeoDrawTemp.bCw = bCW;
		stGeoDrawTemp.ptCenter = fptCenter;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumArc;
		stGeoDrawTemp.ptPoint = fptEnd;
		stGeoDrawTemp.bCw = bCW;
		stGeoDrawTemp.ptCenter = fptCenter;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}
	else
	{
		stGeoDrawTemp.nType = OdbPointType::enumArc;
		stGeoDrawTemp.ptPoint = fptEnd;
		stGeoDrawTemp.ptCenter = fptCenter;
		stGeoDrawTemp.bCw = bCW;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);

	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
	vecFeatureData[0].bPolarity = true;

	return vecFeatureData;
}
vector<MakeFeatureData> CDrawMakePoint::MakeArc(IN CFeatureA* pFeatureA, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureA == nullptr)return vector<MakeFeatureData>();

	D2D1_POINT_2F fptStart = D2D1::Point2F(static_cast<float>(pFeatureA->m_dXs), static_cast<float>(pFeatureA->m_dYs));
	D2D1_POINT_2F fptEnd = D2D1::Point2F(static_cast<float>(pFeatureA->m_dXe), static_cast<float>(pFeatureA->m_dYe));
	D2D1_POINT_2F fptCenter = D2D1::Point2F(static_cast<float>(pFeatureA->m_dXc), static_cast<float>(pFeatureA->m_dYc));

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;
	stGeoDrawTemp.nType = OdbPointType::enumBase;
	stGeoDrawTemp.ptPoint = fptStart;
	vecGeoDraw.push_back(stGeoDrawTemp);
	stGeoDrawTemp._Reset();

	if (pFeatureA->m_dXs == pFeatureA->m_dXe &&
		pFeatureA->m_dYs == pFeatureA->m_dYe)
	{//시작점과 끝점이 같은 원.

		D2D1_POINT_2F fptSidePoint = D2D1::Point2F(static_cast<float>(fptCenter.x * 2.0 - pFeatureA->m_dXe),
			static_cast<float>(fptCenter.y * 2.0 - pFeatureA->m_dYe));

		stGeoDrawTemp.nType = OdbPointType::enumArc;
		stGeoDrawTemp.ptPoint = fptSidePoint;
		stGeoDrawTemp.bCw = !pFeatureA->m_bCw;
		stGeoDrawTemp.ptCenter = fptCenter;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		stGeoDrawTemp.nType = OdbPointType::enumArc;
		stGeoDrawTemp.ptPoint = fptEnd;
		stGeoDrawTemp.bCw = !pFeatureA->m_bCw;
		stGeoDrawTemp.ptCenter = fptCenter;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}
	else
	{
		stGeoDrawTemp.nType = OdbPointType::enumArc;
		stGeoDrawTemp.ptPoint = fptEnd;
		stGeoDrawTemp.ptCenter = fptCenter;
		stGeoDrawTemp.bCw = !pFeatureA->m_bCw;
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();
	}

	vector<MakeFeatureData> vecFeatureData;
	vecFeatureData.resize(1);

	vecFeatureData[0].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
	vecFeatureData[0].bPolarity = pFeatureA->m_bPolarity;

	return vecFeatureData;

}


vector<MakeFeatureData> CDrawMakePoint::MakeText(IN CFeatureT* pFeatureT, CStandardFont* pFont, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr)
{
	if (pFeatureT == nullptr)return vector<MakeFeatureData>();

	double dConvScaleX;//Font의 Scale 변환
	double dConvScaleY;
	double dUnitSizeX;//글자 한개당 크기
	double dUnitSizeY;
	double dUnitScaledSx;//글자 한개당 크기(변환 완료 크기)
	double dUnitScaledSy;
	double dUnitScaledEx;//글자 한개당 크기(변환 완료 크기)
	double dUnitScaledEy;

	int nTextLength = pFeatureT->m_strText.GetLength();

	vector<MakeFeatureData> vecFeatureData;
	MakeFeatureData stTempData;

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
			if (pFontCharTemp == nullptr) continue;

			if (pChar == pFontCharTemp->m_Char)
			{
				pFontChar = pFontCharTemp;
				break;
			}
		}

		//해당되는 폰트케릭터 없음
		if (pFontChar == nullptr)
		{
			continue;
		}

		int nFontLineCount = static_cast<int>(pFontChar->m_arrFontLine.GetCount());
		for (int nFontLine = 0; nFontLine < nFontLineCount; nFontLine++)
		{
			CFontLine* pFontLine = pFontChar->m_arrFontLine.GetAt(nFontLine);
			if (pFontLine == nullptr) continue;

			double dScaleLineWidth = pFontLine->m_dWidth * pFeatureT->m_dWidthFactor;

			//한줄에 있다는 가정
			dUnitSizeX = pFeatureT->m_dXSize;// / nTextLength;
			dUnitSizeY = pFeatureT->m_dYSize;
			dConvScaleX = (dUnitSizeX - dScaleLineWidth) / pFont->m_dXSize;
			dConvScaleY = (dUnitSizeY - dScaleLineWidth) / pFont->m_dYSize;

			double dStartX = /*pFeatureT->m_dX +*/ (dUnitSizeX*i);

			//
			dUnitScaledSx = dStartX + ((pFontLine->m_dXs - pFontChar->m_MinMaxRect.left) * dConvScaleX);
			dUnitScaledSy = /*pFeatureT->m_dY + */((pFontLine->m_dYs - pFontChar->m_MinMaxRect.bottom) * dConvScaleY);
			dUnitScaledEx = dStartX + ((pFontLine->m_dXe - pFontChar->m_MinMaxRect.left) * dConvScaleX);
			dUnitScaledEy = /*pFeatureT->m_dY + */((pFontLine->m_dYe - pFontChar->m_MinMaxRect.bottom) * dConvScaleY);

			//Font좌표와 Text좌표의 Offset값 계산					
			double dOffsetX = CJobFile::InchToMM(0.006)*pFeatureT->m_dWidthFactor;
			double dOffsetY = CJobFile::InchToMM(0.006)*pFeatureT->m_dWidthFactor;

			dUnitScaledSx += dOffsetX;
			dUnitScaledSy += dOffsetY;
			dUnitScaledEx += dOffsetX;
			dUnitScaledEy += dOffsetY;

			vector<GEODRAW> vecGeoDraw;
			GEODRAW stGeoDrawTemp;
			stGeoDrawTemp.nType = OdbPointType::enumBase;
			stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(dUnitScaledSx), DoubleToFloat(dUnitScaledSy));
			vecGeoDraw.push_back(stGeoDrawTemp);
			stGeoDrawTemp._Reset();

			stGeoDrawTemp.nType = OdbPointType::enumLine;
			stGeoDrawTemp.ptPoint = D2D1::Point2F(DoubleToFloat(dUnitScaledEx), DoubleToFloat(dUnitScaledEy));
			vecGeoDraw.push_back(stGeoDrawTemp);
			stGeoDrawTemp._Reset();

			stTempData.vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
			stTempData.bPolarity = pFeatureT->m_bPolarity;

			vecFeatureData.push_back(stTempData);
		}
	}

	return vecFeatureData;
}


vector<MakeFeatureData>  CDrawMakePoint::MakeSurface(IN CFeatureS* pFeatureS, CRotMirrArr *pRotMirrArr, IN const RECTD &drtViewRect_mm)
{
	if (pFeatureS == nullptr)return vector<MakeFeatureData>();

	int nSurfaceCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
	if (nSurfaceCount <= 0) return vector<MakeFeatureData>();

	bool bSkip = true;
	RECTD drtViewRect = drtViewRect_mm;
	if (drtViewRect.IsRectNull())
		bSkip = false;

	vector<GEODRAW> vecGeoDraw;
	GEODRAW stGeoDrawTemp;
	vector<MakeFeatureData> vecvecRotateData;
	vecvecRotateData.resize(nSurfaceCount);
	for (int i = 0; i < nSurfaceCount; i++)
	{
		CObOe* pObOe = pFeatureS->m_arrObOe.GetAt(i);
		if (pObOe == nullptr) continue;

		//Inrect Check Draw 시간을 줄이기 위함
		if (bSkip)
		{
			RECTD drtFeature;
			if (pRotMirrArr == nullptr)
			{
				drtFeature = pObOe->m_MinMaxRect;
			}
			else
			{
				pRotMirrArr->FinalPoint(&drtFeature.left, &drtFeature.top, pObOe->m_MinMaxRect.left, pObOe->m_MinMaxRect.top);
				pRotMirrArr->FinalPoint(&drtFeature.right, &drtFeature.bottom, pObOe->m_MinMaxRect.right, pObOe->m_MinMaxRect.bottom);
				drtFeature.NormalizeRectD();
			}

			if (drtFeature.IsIntersection(drtViewRect) == FALSE)
			{
				vecvecRotateData[i].bIsUsed = false;
				continue;
			}
		}

		vecGeoDraw.clear();
		stGeoDrawTemp._Reset();

		D2D1_POINT_2F fptLastPoint;
		//시작점 
		stGeoDrawTemp.nType = OdbPointType::enumBase;
		stGeoDrawTemp.ptPoint = D2D1::Point2F(static_cast<float>(pObOe->m_dXbs), static_cast<float>(pObOe->m_dYbs));
		vecGeoDraw.push_back(stGeoDrawTemp);
		stGeoDrawTemp._Reset();

		fptLastPoint = D2D1::Point2F(static_cast<float>(pObOe->m_dXbs), static_cast<float>(pObOe->m_dYbs));

		int nPointCount = static_cast<int>(pObOe->m_arrOsOc.GetCount());

		for (int j = 0; j < nPointCount; j++)
		{
			COsOc* pOsOc = pObOe->m_arrOsOc.GetAt(j);
			if (pOsOc == nullptr) continue;
			if (pOsOc->m_bOsOc == true)
			{//Line
				stGeoDrawTemp.nType = OdbPointType::enumLine;
				stGeoDrawTemp.ptPoint = D2D1::Point2F(static_cast<float>(pOsOc->m_dX), static_cast<float>(pOsOc->m_dY));
				vecGeoDraw.push_back(stGeoDrawTemp);
				stGeoDrawTemp._Reset();
			}
			else
			{//Arc

				if (fptLastPoint.x == static_cast<float>(pOsOc->m_dX) &&
					fptLastPoint.y == static_cast<float>(pOsOc->m_dY))
				{//시작점과 끝점이 같은 원.

					D2D1_POINT_2F fptSidePoint = D2D1::Point2F(static_cast<float>(((COc*)pOsOc)->m_dXc * 2.0 - pOsOc->m_dX),
						static_cast<float>(((COc*)pOsOc)->m_dYc * 2.0 - pOsOc->m_dY));

					stGeoDrawTemp.nType = OdbPointType::enumArc;
					stGeoDrawTemp.ptPoint = fptSidePoint;
					stGeoDrawTemp.bCw = !((COc*)pOsOc)->m_bCw;
					stGeoDrawTemp.ptCenter = D2D1::Point2F(static_cast<float>(((COc*)pOsOc)->m_dXc), static_cast<float>(((COc*)pOsOc)->m_dYc));
					vecGeoDraw.push_back(stGeoDrawTemp);
					stGeoDrawTemp._Reset();

					stGeoDrawTemp.nType = OdbPointType::enumArc;
					stGeoDrawTemp.ptPoint = D2D1::Point2F(static_cast<float>(pOsOc->m_dX), static_cast<float>(pOsOc->m_dY));
					stGeoDrawTemp.bCw = !((COc*)pOsOc)->m_bCw;
					stGeoDrawTemp.ptCenter = D2D1::Point2F(static_cast<float>(((COc*)pOsOc)->m_dXc), static_cast<float>(((COc*)pOsOc)->m_dYc));
					vecGeoDraw.push_back(stGeoDrawTemp);
					stGeoDrawTemp._Reset();
				}
				else
				{
					stGeoDrawTemp.nType = OdbPointType::enumArc;
					stGeoDrawTemp.ptPoint = D2D1::Point2F(static_cast<float>(pOsOc->m_dX), static_cast<float>(pOsOc->m_dY));
					stGeoDrawTemp.bCw = !((COc*)pOsOc)->m_bCw;
					stGeoDrawTemp.ptCenter = D2D1::Point2F(static_cast<float>(((COc*)pOsOc)->m_dXc), static_cast<float>(((COc*)pOsOc)->m_dYc));
					vecGeoDraw.push_back(stGeoDrawTemp);
					stGeoDrawTemp._Reset();
				}
			}

			fptLastPoint = D2D1::Point2F(static_cast<float>(pOsOc->m_dX), static_cast<float>(pOsOc->m_dY));
		}

		vecvecRotateData[i].vecPointData = SetMirrorRotate(vecGeoDraw, pRotMirrArr);
		vecvecRotateData[i].bPolarity = pObOe->m_bPolyType;
	}

	return vecvecRotateData;
}