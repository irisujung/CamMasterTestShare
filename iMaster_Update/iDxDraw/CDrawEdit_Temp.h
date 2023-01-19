#pragma once

#include "../iODB/iODB.h"
#include "CDrawFunction.h"
#include "CFeatureData.h"


typedef struct _TempPoint
{
	CString strName;
	D2D1_POINT_2F fptPoint;
	D2D1_POINT_2F fptPoint2;
	bool bosoc;
	bool bCw;
	_TempPoint()
	{
		_Reset();
	}

	void _Reset()
	{
		strName = _T("");
		fptPoint = D2D1::Point2F(0.f, 0.f);
		fptPoint2 = D2D1::Point2F(0.f, 0.f);
		bosoc = true;
		bCw = true;

	}
	// 
	// 	_TempPoint& operator=(const _TempPoint& m)
	// 	{
	// 		strName = m.strName;
	// 		fptPoint = m.fptPoint;	
	// 	}


}TempPoint;

typedef struct _EditSelectInfo
{
	int nFeatureIndex;
	int nPointIndex;

	CPointD dptMove;

	_EditSelectInfo()
	{
		Reset();
	}

	void Reset()
	{
		nFeatureIndex = -1;
		nPointIndex = -1;

	}

}EditSelectInfo;

class CDrawFunction;
class CDrawManager;

class CDrawEdit_Temp
{
public:
	CDrawEdit_Temp();
	~CDrawEdit_Temp();

	void SetLink(IN ID2D1Factory1 *pFactory, IN ID2D1DeviceContext* p2DContext, IN CDrawManager *pDrawManager);
	UINT32 SetUserSetInfo(UserSetInfo_t *pUserSetInfo, UserLayerSet_t *pUserLayerSetInfo);
	UINT32 SetViewInfo(IN ViewInfo_t* pViewInfo);

	UINT32 SetResize(IN const float &fSizePersent);

	UINT32 SetLineArcResize(IN const double &dSizePersent);
	UINT32 SetPadTextResize(IN const double &dSizePersent);
	UINT32 SetSurfaceResize(IN const double &dSizePersent);
	UINT32 SetPadShape(IN const double &dRadius, IN const double &dWidth, IN const double &dHeight);
	UINT32 SetLineShape(IN const CString &strShape, IN const double &dLineWidth);

	

	UINT32 SetAttribute(IN const CString&strAttrName, IN const CString &strAttrString);

	UINT32 SetPadRotate(IN const int&nIndex, IN const BOOL&bMirror);


	UINT32 Set_CW(IN const int&nIndex);

	
	UINT32 SetMaskResize(IN const float &fPersentWidth, IN const float &fPersentHeight);

	UINT32 SetTransform(IN const D2D1::Matrix3x2F& matTrans);

	ID2D1BitmapRenderTarget * GetRender();

	//Feature Clear
	UINT32 ClearFeatureData();
	//Step에서 선택된 항목을 셋한다.
	UINT32 SetFeatureData(CStep* pStep);
	UINT32 SetMaskData(IN const MaskType &eMaskType, CStep* pStep);
	//개별 Feature를 넣는다.
	UINT32 AddFeatureData(IN const int &nLayer, IN const int &nFeatureIndex, CFeature *pFeature, IN const int &nMaskIndex = 0,
		IN const double &fMoveX_mm = 0., IN const double &fMoveY_mm = 0.);

	

	vector<CFeatureData*>& GetFeatureData();

	//
	UINT32 OnLButtonDown(UINT nFlags, CPoint point);
	UINT32 OnLButtonUp(UINT nFlags, CPoint point);
	UINT32 OnMouseMove(UINT nFlags, CPoint point);

	//
	UINT32 SetMoveAxis(IN const int &nMoveAxis);

	void ClearTempPoint();
	UINT32 SetTempPoint(vector<TempPoint> &vecTempPoint);
	UINT32 SetTempPoint(IN const int &nIndex, IN const CString &strName, IN const float &fX, IN const float &fY);
	UINT32 SetTempPoints(IN const int &nIndex, IN const CString &strName, IN const float &fX, IN const float &fY);
	vector<TempPoint> GetTempPoint();
	TempPoint GetTempPoint(IN const int &nIndex);

	//그리기 함수
	UINT32 OnRender(CTypedPtrArray <CObArray, CStandardFont*>* pFontArr);
	
	
	UINT32 _ManualMove_Feature(PointDXY point);

	UINT32 _SetPitch_Feateure(PointDXY point, int nRepeatX, int nRepeatY);
	
	// 22.6.13 장동진 수정 
	BOOL						m_bMoveMode = FALSE;

	D2D1_POINT_2F m_totalMove = D2D1::Point2F(0.f, 0.f);
	


	BOOL	m_bMoved = FALSE;
	BOOL    m_bResize = FALSE;

	BOOL	m_bEditPoint = FALSE;

private:

	BOOL _HitTest_Feature(vector<CFeatureData*> &vecFeature, D2D1_POINT_2F fptPoint, D2D1::Matrix3x2F &ViewScale);
	BOOL _HitTest_EditPoint(vector<CFeatureData*> &vecFeature, D2D1_POINT_2F fptPoint);


	UINT32 _Move_Feature(vector<CFeatureData*> &vecFeature, D2D1_POINT_2F fptPoint);

	UINT32 _Pitch_Feature(vector<CFeatureData*> &vecFeature, D2D1_POINT_2F fptPoint);

	UINT32 _DrawTempPoint();

	UINT32 _Draw_EditPoint();

	//같은 Feature가 있는지 체크
	BOOL   _IsOverlapFeature(vector<CFeatureData*> &vecFeature, IN const int &nLayer, IN const int &nFeatureIndex, CFeature *pFeature, IN const int &nMaskIndex = 0);

	UINT32 _DrawTempLine();

	

private:
	vector<CFeatureData*> m_vecFeature;

	//Render Draw
	ID2D1BitmapRenderTarget *m_pBmpRenderTarget = nullptr;
	D2D1_SIZE_F m_sizeTarget;
	D2D1::Matrix3x2F m_ViewScale;//Scale + Panning 정보

	CDrawFunction				*m_pDrawFunction = nullptr;

	//BOOL						m_bMoveMode = FALSE;
	CPoint						m_ptStartPoint = CPoint(0, 0);//Move 시작점
	int							m_nMoveAxis = Edit_MoveAxis::enumMoveAxis_None;

	
	//Temp Point
	const int					m_nMaxTempPoint = 100;//
	vector<TempPoint>			m_vecTempPoint;

	//EditSelectInfo
	
	EditSelectInfo				m_stEditSelectInfo;
	PointDXY					m_dptPreMoveTemp;
	//

	//외부 Ptr
	//지우지 말것
	ID2D1Factory1					*m_pFactory = nullptr;
	ID2D1DeviceContext				*m_pD2DContext = nullptr;

	CDrawManager					*m_pDrawManager = nullptr;

	UserSetInfo_t					*m_pUserSetInfo = nullptr;
	UserLayerSet_t					*m_pUserLayerSetInfo = nullptr;
	ViewInfo_t						*m_pViewInfo = nullptr;

};

