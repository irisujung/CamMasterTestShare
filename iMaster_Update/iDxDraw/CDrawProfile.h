#pragma once

#include <vector>
using namespace std;

#include "../iODB/iODB.h"
#include "CDrawFunction.h"

//#define PI               3.14159265358979323846


class CDrawProfile
{

public:
	CDrawProfile();
	~CDrawProfile();

	void SetLink(ID2D1Factory1 *pFactory, ID2D1DeviceContext* pD2DContext);
	UINT32 SetUserSetInfo(UserSetInfo_t *pUserSetInfo, UserLayerSet_t *pUserLayerSetInfo);
	void OnRender(IN CJobFile *pjobFile, const int &nStepIndex, ID2D1BitmapRenderTarget* pRender);

	//Profile Save시에 호출
	void OnRender_Save(IN CStep *pStep, CRotMirrArr *pRotMirrArr, IN const BOOL bSubDraw, IN const BOOL &bSubSubDraw, IN const BOOL &bSubSubSubDraw, IN ID2D1RenderTarget* pRender);



	void ResetView();

	//Zoom Rect
	void DrawRect(ID2D1BitmapRenderTarget* pRender, CRect &rtRect);

	//Measure Rect
	void DrawMeasure(ID2D1BitmapRenderTarget* pRender, IN const D2D1_RECT_F &frtRect, IN const BOOL &bDispInch = FALSE);


private:
	void OnRender_SubStep(CSubStep* pSubStep, CRotMirrArr *pRotMirrArr = nullptr);
	UINT32 _SetGeometry(vector<GEODRAW> &vecGeoData, ID2D1PathGeometry **pGeoObject, BOOL bOpen = FALSE);

	void OnRender_SubStep_Save(CSubStep* pSubStep, IN const BOOL &bSubSubDraw, IN const BOOL &bSubSubSubDraw, CRotMirrArr *pRotMirrArr = nullptr);


	void AddProfileFeature(CFeatureS *pFeatureS, CRotMirrArr *rotMirrArr = nullptr);
	void _AddProfileFeature(CObOe *pSurface, CRotMirrArr *rotMirrArr = nullptr);
	//void DrawStepRepeatProfile(CStep *pStep, ID2D1BitmapRenderTarget* pRender);

	void _InitTextUtil();
	void _ClearTextUtil();

private:

	int m_nCurStep = -1;

	ID2D1SolidColorBrush *m_pWhiteBrush = nullptr;
	ID2D1PathGeometry *m_pGeoObject = nullptr;
	vector<GEODRAW> m_vecGeo;
	D2D1_RECT_F				m_frtPictureBox;

	//Text
	IDWriteFactory		*m_pWriteFactory = nullptr;
	IDWriteTextFormat	*m_pTextFormat = nullptr;

	//외부 ptr
	//지우지 말것
	ID2D1Factory1 *m_pFactory = nullptr;
	ID2D1DeviceContext* m_pD2DContext = nullptr;
	UserSetInfo_t *m_pUserSetInfo = nullptr;
	UserLayerSet_t *m_pUserLayerSetInfo = nullptr;
};

