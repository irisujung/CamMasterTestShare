#pragma once

#include "DxDraw/CDirect_Base.h"
#include "DxDraw/Def_Viewer.h"
#include "CDrawEdit_Temp.h"

class CDrawManager;

//Minimap Menu
namespace MinimapMenu
{
	enum
	{
		enumID_Inspect = 91000,
		//enumID_Copy = 91000,
		enumID_Copy,
		enumID_Paste,
		enumID_Delete,
		enumID_ResetZoom,

		enumID_Max,
	};

	//const CString strName[enumID_Max - enumID_Copy] = {
	const CString strName[enumID_Max - enumID_Inspect] = {
		_T("Inspect"),
		_T("Copy"),
		_T("Paste"),
		_T("Delete"),
		_T("ResetZoom"),
	};
}

typedef struct _MinimapSelect
{
	int			nStep;
	RECTD		drtRect;

	_MinimapSelect()
	{
		_Reset();
	}

	void _Reset()
	{
		nStep = -1;
		drtRect.SetRectEmpty();
	}

}MinimapSelect;

class __declspec(dllexport) CDrawMinimap : public CDirect_Base
{
	enum //�ӽ� Enum
	{
		enumMode_Measure,
		enumMode_Thickness,

		enumMode_Max,
	};	

public:
	CDrawMinimap();
	~CDrawMinimap();

	void Initial(IN CJobFile *pJobData, IN const ViewInfo_t &stViewInfo, IN CDrawManager *pDrawManager);
	void SetLink(IN CDrawManager *pDrawManager);
	void SetPanelInfo(PanelInfo_t		*pPanelInfo);

	UINT32 ChangeViewSize(IN const ViewInfo_t &stViewInfo);

	UINT32 ResetView();

	UINT32 SetUserSetInfo(UserSetInfo_t *pUserSetInfo, UserLayerSet_t *pUserLayerSetInfo);
	UINT32 SetStepIndex(int nStepIndex);
	
	UINT32 ZoomIn();
	UINT32 ZoomOut();
	UINT32 ResetZoom();

	//Mouse Action
	UINT32 OnLButtonDown(UINT nFlags, CPoint point);
	UINT32 OnLButtonUp(UINT nFlags, CPoint point);
	UINT32 OnMouseMove(UINT nFlags, CPoint point);
	UINT32 OnRButtonDown(UINT nFlags, CPoint point);
	UINT32 OnRButtonUp(UINT nFlags, CPoint point);
	UINT32 OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

	//
	//���õ� ���� ��������
	UINT32 GetSelectRect(OUT RECTD &drtRect);

	UINT32 SetMode(IN const int &nMode);

	UINT32			SetInspect_Unit();
	UINT32			SetCopy_Feature(BOOL &bCopy);
	UINT32			SetPaste_Feature();
	UINT32			SetDelete_Feature();	

	int				GetCopyBufferCount(int nMode);
	
private:
	UINT32		_Initial_Pen();

	void		_SetViewScale();
	UINT32		_SetPictureBox(IN const D2D1_RECT_F & frtPictureBox);

	void		_SetZoomRect(IN const D2D1_RECT_F& frtRect_mm);

	UINT32		_SetGeometry(vector<GEODRAW> &vecGeoData, ID2D1PathGeometry **pGeoObject, BOOL bOpen);

	//
	UINT32		_DrawProfile(ID2D1RenderTarget *pRender);

	void _OnRender_SubStep(CSubStep* pSubStep, CRotMirrArr *pRotMirrArr, IN const int &nSubIndex, IN int nSubLevel = -1, IN int nParent_StepX = -1, IN int nParent_StepY = -1);
	vector<RECTD>  _AddProfileFeature(CFeatureS *pFeatureS, CRotMirrArr *rotMirrArr = nullptr);
	vector<RECTD>  _AddProfile_ObOe(CObOe *pSurface, CRotMirrArr *rotMirrArr = nullptr);

	//Mouse Action
	UINT32		_ResetSelect();
	BOOL		_CheckSelect(IN const D2D1_POINT_2F &fptPoint, OUT MinimapSelect &stSelect);
	BOOL		_CheckSelect_Multi(IN const D2D1_POINT_2F &fptPoint, OUT vector<MinimapSelect> &stSelectInfo);

	//

	UINT32		_SetSelect_Once(IN const RECTD &drtSelect);
	UINT32		_SetSelect_Multi(IN const vector<MinimapSelect> &vecSelectInfo);

	//Clear Copy Buffer
	UINT32			_ClearCopyBuffer(int nMode = enumMode_Max);

	CLayer*			_GetEditLayer(OUT int &nLayer);//���� ���� ����� Layer�� ������ ��ȯ

	//Insp Unit		
	BOOL			AddMeasureUnit(INT32 iStripIdx, INT32 iUnitIdx, UnitInfo_t *pUnit);
	UINT32			_SetInspect_Unit(CLayer *pLayer, IN RECTD drtSelect); //�������� Unit Inspect

	//Copy
	vector<int>		_GetInsideFeatureIndex(IN CLayer* pLayer, IN const RECTD &drtRect_mm);//�簢�� ���� Feature Index�� ��ȯ
	vector<int>		_GetInsideAlignIndex(IN CLayer* pLayer, IN const RECTD &drtRect_mm);// �簢�� ���� Align Index ��ȯ
	UINT32			_SetCopyBuffer(IN const CLayer* pLayer, IN const int &nLayer, IN const vector<int> &vecFeatureIndex, IN const vector<int> &vecAlignIndex, IN int nMode);//�Է� ���� Feature Index�� �ش��ϴ� Feature�� ���ۿ� ����

	//Paste
	UINT32			_SetPaste_Feature(IN CStep* pStep, IN const vector<CFeatureData*> &vecFeatureData, IN const CPointD &dptPoint);
	UINT32			_SetPaste_AlignFeature(IN CStep* pStep, IN const vector<CFeatureData*> &vecFeatureData, IN const CPointD &dptPoint);

	//Delete
	UINT32			_SetDelete_Feature(CLayer *pLayer, IN RECTD drtSelect);//�������� Feature Delete

	//Profile Rect
	void _ClearProfileRect();

	//
	
	UINT32			_SetMeasureUnitInfo();
	void			_DrawInspUnit(ID2D1RenderTarget *pRender);
		

protected:
	virtual void OnRender();



private:

	int				m_nSelectStep = -1;

	int				m_nMode = enumMode_Measure;//0 : MP, 1 : TP

	//PictureBox
	D2D1_RECT_F		m_frtPictureBox;

	//Draw Item
	ID2D1SolidColorBrush *m_pWhiteBrush = nullptr;
	ID2D1SolidColorBrush *m_pRedBrush = nullptr;
	ID2D1SolidColorBrush *m_pInspBrush = nullptr;
	ID2D1PathGeometry *m_pGeoObject = nullptr;
	vector<GEODRAW> m_vecGeo;
	vector<vector<DrawProfileData>>	m_vecvecRect;//[Step][Rect]

	//SelectInfo
	MinimapSelect		m_stSelectInfo;
	vector<MinimapSelect>	m_vecMultiSelectInfo;

	ID2D1PathGeometry *m_pGeoObject_Select = nullptr;
	vector<GEODRAW> m_vecGeo_Select;
	//

	//���� ���� �ε��� ����
	vector<MeasureUnit>		m_vecMeasureUnitInfo;

	//Mouse Control
	BOOL			m_bPanMode = FALSE;
	CPoint			m_ptPrevMove = CPoint(0, 0);

	D2D1::Matrix3x2F m_matScaleView;//scale ����
	D2D_POINT_2F	m_fptPan = { 0.f,0.f };//Panning ����


	ViewInfo_t		m_stView;
	SystemSpec_t	m_stSystemSpec;

	//���� ��� Rect
	MinimapSelect			m_stCopyInfo;
	//���� ��� Feature
	vector<CFeatureData*>	m_vecCopyTemp;
	vector<CFeatureData*>	m_vecCopyAlignTemp;
	vector<CFeatureData*>	m_vecCopyTemp_SR;


	//�ܺ� ������
	//������ ����
	CJobFile		*m_pJobData = nullptr;
	PanelInfo_t		*m_pPanelInfo = nullptr;
	CDrawManager	*m_pDrawManager = nullptr;
};

