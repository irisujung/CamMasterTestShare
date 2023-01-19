#pragma once

#include <vector>
using namespace std;

#include "../iODB/iODB.h"
#include "CDrawFunction.h"

//#define PI               3.14159265358979323846

class CDrawFov
{

public:
	CDrawFov();
	~CDrawFov();

	void SetLink(ID2D1Factory1 *pFactory, ID2D1DeviceContext* pD2DContext, FovInfo_t* pFovInfo, CellInfo_t* pCellInfo);
	UINT32 SetUserSetInfo(UserSetInfo_t *pUserSetInfo, UserLayerSet_t *pUserLayerSetInfo);
	void OnRender(const int &nStepIndex, ID2D1BitmapRenderTarget* pRender, bool bIsFovMode);

	void ResetView();

private:
	UINT32 _SetGeometry(vector<GEODRAW> &vecGeoData, ID2D1PathGeometry **pGeoObject, BOOL bOpen = FALSE);

	void _AddFovFeature(FovData_t* pFov);
	void OnRender_Fov(const int &nStepIndex, ID2D1BitmapRenderTarget* pRender);
	void OnRender_Cell(const int &nStepIndex, ID2D1BitmapRenderTarget* pRender);
	
private:

	int						m_nCurStep = -1;

	ID2D1SolidColorBrush	*m_pBrush = nullptr;
	ID2D1SolidColorBrush	*m_pBrush_Valid = nullptr;
	ID2D1SolidColorBrush	*m_pBrush_Align = nullptr;
	ID2D1SolidColorBrush	*m_pBrush_Mask = nullptr;
	ID2D1PathGeometry		*m_pGeoObject = nullptr;
	vector<GEODRAW>			m_vecGeo;
	D2D1_RECT_F				m_frtPictureBox;
	
	//외부 ptr
	//지우지 말것
	ID2D1Factory1			*m_pFactory = nullptr;
	ID2D1DeviceContext		*m_pD2DContext = nullptr;
	UserSetInfo_t			*m_pUserSetInfo = nullptr;
	UserLayerSet_t			*m_pUserLayerSetInfo = nullptr;

	FovInfo_t				*m_pFovInfo = nullptr;
	CellInfo_t				*m_pCellInfo = nullptr;
};

