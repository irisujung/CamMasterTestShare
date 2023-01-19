#pragma once

#include <vector>
using namespace std;

#include "../iODB/iODB.h"
#include "DxDraw/Def_Viewer.h"

typedef struct _Line_t
{
	double dSlope;
	double dB;

	_Line_t()
	{
		dSlope = 0.;
		dB = 0;

	}
}_Line;

typedef struct _Line2_t
{
	double dAngle;
	D2D_POINT_2F fptCenter;

	_Line2_t()
	{
		dAngle = 0.;
		fptCenter.x = 0.f;
		fptCenter.y = 0.f;

	}
}_Line2;

class __declspec(dllexport) CDrawMakePoint
{
public:

	static double GetAngle(D2D_POINT_2F Point1, D2D_POINT_2F Point2);
	static double GetAngle(IN const double &dSx, IN const double &dSy, IN const double &dEx, IN const double &dEy);
	static double GetAngle(IN const float &dSx, IN const float &dSy, IN const float &dEx, IN const float &dEy);
	static double GetDistance(D2D_POINT_2F Point1, D2D_POINT_2F Point2);
	static double GetDistance(CPointD dptPoint1, CPointD dptPoint2);
	static double GetDistance_X(D2D_POINT_2F Point1, D2D_POINT_2F Point2);

	static void		GetLine(IN const double &dSx, IN const double &dSy, IN const double &dEx, IN const double &dEy, OUT _Line2 &stLine);
	static BOOL		GetCrossLine_LNtoPT(IN const _Line2& stLine, const D2D_POINT_2F& fptPoint, OUT _Line2& stCrossLine);
	static BOOL		GetCrossPoint_LNtoLN(IN const _Line2& eLine1, const _Line2& eLine2, OUT D2D_POINT_2F& fptCross);
	static BOOL		GetCrossPoint_LNtoPT(IN const _Line2& eLine, const D2D_POINT_2F& ePoint, OUT D2D_POINT_2F& eCrossPoint);

	static D2D_POINT_2F Rotate(IN const D2D_POINT_2F &fptOrg, IN const D2D_POINT_2F &fptRotateCenter, IN const double &dAngle);

	static vector<GEODRAW> SetMirrorRotate(vector<GEODRAW> &vecGeoData, CRotMirrArr *pRotMirrArr, IN const double &dAngle = 0);

	static vector<MakeFeatureData> MakeTp(IN  CFeatureZ* pFeatureZ, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);

	static vector<MakeFeatureData> MakeLine(IN CFeatureL* pFeatureL, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);

	static vector<MakeFeatureData>  MakePadCircle(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadRect(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadRectxr(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadRectxc(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadOval(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadDi(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadOct(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadDonut_r(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadDonut_s(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadHex_l(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadHex_s(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadBfr(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadBfs(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadTriangle(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadOval_h(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadThr(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadThs(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadS_ths(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadS_tho(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadSr_ths(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadRc_ths(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadRc_tho(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadEllipse(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadMoire(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadHole(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector<MakeFeatureData>  MakePadUserdefined(IN CFeatureP* pFeatureP, IN const CSymbol* pSymbol, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr);

	static vector<MakeFeatureData> MakeArc(IN CFeatureA* pFeatureA, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);
	static vector <MakeFeatureData> MakeArc(IN const D2D1_POINT_2F &fptStart, IN const D2D1_POINT_2F &fptEnd, IN const D2D1_POINT_2F &fptCenter, IN  const BOOL &bCW, CRotMirrArr *pRotMirrArr);


	static vector<MakeFeatureData> MakeText(IN CFeatureT* pFeatureT, CStandardFont* pFont, IN const CSymbol* pSymbol, CRotMirrArr *pRotMirrArr);

	static vector<MakeFeatureData> MakeSurface(IN CFeatureS* pFeatureS, CRotMirrArr *pRotMirrArr, IN const RECTD &drtViewRect_mm);
};

