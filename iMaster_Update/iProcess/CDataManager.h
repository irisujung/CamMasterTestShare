#pragma once

#include "../iCommonIF/CommonIF-Struct.h"
#include "../iCommonIF/CommonIF-Error.h"
#include "../iCommonIF/CommonIF-Define.h"
#include "../iCommonIF/CommonIF-Path.h"
#include "../iUtilities/iUtilities.h"
#include "../iODB/iODB.h"
#include "../iDxDraw/iDxDraw.h"
#include "../iTinyXML/tinyxml.h"

#include <algorithm>

class __declspec(dllexport) CDataManager
{
public:
	CDataManager();
	virtual ~CDataManager();

	UINT32	SetSystemSpec(SystemSpec_t* pSystemSpec);
	UINT32	SetUserSetInfo(UserSetInfo_t *pUserSetInfo, UserLayerSet_t *pUserLayerSetInfo);
	UINT32	Initialize(CJobFile* pCAMData, CDrawManager* pDrawManager);
	UINT32  CheckStructure();

	BOOL	Is4Step() { return m_bIs4Step; }
	void	ClearData();
	static void	GetAngle(Orient ori, double &dAngle, bool &bMirror);
	static void	GetOrient(double dAngle, bool bMirror, Orient &ori);
	static Orient  AddOrient(Orient ori1, Orient ori2);
	static Orient  ReverseOrient(Orient ori);

	FovInfo_t*	GetFovInfo() { return &m_stFovInfo; }
	CellInfo_t*	GetCellInfo() { return &m_stCellInfo; }
	PanelInfo_t* GetProductInfo() { return	&m_stPanelInfo; }
	std::map<INT32, std::vector<CString>>* GetPTWidth_Pos() { return &m_vcPTWidth_Pos; }//사용하지 않음
	std::map<INT32, std::vector<CString>>* GetPTWidth_Neg() { return &m_vcPTWidth_Neg; }//사용하지 않음


	std::map<INT32, std::vector<CString>>* GetLineWidth(int eOption);

	//Step별 Line WidthData를 Step 구분하지 않는 Sort Data로 변환
	UINT32	SetSortWidthData();

public:
	virtual UINT32 Destory() = 0;
	virtual UINT32 MakeMasterData() = 0;
	virtual UINT32 GetMeasureData(MeasurePanel_t* pMeasure = nullptr, BOOL bSave = FALSE, BOOL bUpdate= FALSE) = 0;
	
	//Product Info
	virtual UINT32 EstimateProductInfo(IN INT32 iStepIdx, IN enum Orient eOrient) = 0;
	//Panel Info
	virtual UINT32 EstimatePanelInfo(IN INT32 iStepIdx, IN enum Orient eOrient) = 0;
	//Strip Info
	virtual UINT32 EstimateStripInfo(IN INT32 iStepIdx, IN enum Orient eOrient) = 0;
	virtual UINT32 CalculateStripNumOnSubStep(IN BOOL bFirstStep, IN CString strStripStepName, IN CSubStep* pSubStep, OUT UINT32 *pStripNum) = 0;
	virtual UINT32 CalculateStripCoord(IN BOOL bFirstStep, IN UINT iShapeIdx, IN INT32 iStepIdx, IN enum Orient eOrient, IN CString strStripStepName, IN CRotMirrArr *pRotMirrArr, IN CSubStep *pSubStep, OUT UINT32 *pRgnNum) = 0;
	virtual UINT32 CalculateOrient(IN enum Orient curOrient, OUT enum Orient* pTotOrient) = 0;
	//Unit Info
	virtual UINT32 EstimateUnitInfo(IN INT32 iStepIdx, IN enum Orient eOrient) = 0;
	virtual UINT32 CalculateUnitCoord(IN BOOL bFirstStep, IN UINT iShapeIdx, IN INT32 iStepIdx, IN enum Orient eOrient, IN CString strUnitStepName, IN CRotMirrArr *pRotMirrArr, IN CSubStep *pSubStep, OUT UINT32 *pRgnNum) = 0;
	virtual UINT32 RearrageIndexInfo(IN enum Orient eOrient) = 0;
	//Block Info
	virtual UINT32 EstimateBlockInfo() = 0;
	//Save 
	virtual UINT32 SaveProductInfo(IN INT32 iStepIdx, IN enum Orient eOrient) = 0;
	virtual UINT32 MakePanelAlignPointImage() = 0;
	virtual UINT32 SaveGrabInfo(INT32 iStepIdx, enum Orient eOrient) = 0;
	//FOV
	virtual UINT32 OptimizeFOV(IN BOOL bSave = FALSE, IN BOOL bUpdate = FALSE) = 0;

	//CrossPoint
	virtual UINT32 SetAlignCrossPoint(IN const INT32 & iStepIdx) = 0;
	
protected:
	UINT32 SetRect(IN enum Orient eOrient, IN CString strUnitStepName, IN CRotMirrArr *pRotMirrArr, IN CSubStep *pSubStep, IN const MaskType &eMaskType, OUT vector<vector<RECTD>> &vecRect);
	UINT32 SetRect_In_Panel(IN enum Orient eOrient, IN CString strUnitStepName, IN CRotMirrArr *pRotMirrArr, IN CSubStep *pSubStep, IN const MaskType &eMaskType, OUT vector<vector<RECTD>> &vecRect);

	UINT32 EstimateAlignInfo_in_Cell_V2();
	UINT32 EstimateMaskInfo_in_Cell_V2();

	//
	UINT32 GetRectInfo_In_Cell(IN const RECTD &drtInPanel, IN RECTD &drtCell, const Orient emOrient,
		IN const UserSetInfo_t	*pUserSetInfo, IN const double &dRedolution_mm, OUT CellAlignInfo_t &stAlignInfo);

	CLayer *GetLayerInfo(UserLayerSet_t	*pUserLayerSetInfo, CStep* pStep, CString strDefine);
	CLayer *GetLayer(IN CTypedPtrArray <CObArray, CLayer*> &arrLayer, IN const CString &strLayer);

	RECTD GetDrawRect(IN const RECTD &drtOutput, UserSetInfo_t *pUserSetInfo);


public:
	int				m_nSelectStep = -1;
	int				m_nPrevSelectStep = -1;
	enum Orient		m_ePrevOrient = Orient::OrientNone;
	enum eDirectionType	 m_ePrevMirrorDir = eDirectionType::DirectionTypeNone;
	BOOL			m_bIs4Step = FALSE;
	enum Orient		m_e4StepOrient = Orient::NoMir0Deg;
	enum Orient		m_eProfileOrient = Orient::OrientNone;

	SystemSpec_t	*m_pSystemSpec = nullptr;
	CJobFile		*m_pJobData = nullptr;
	CDrawManager	*m_pDrawManager = nullptr;
	UserSetInfo_t 	*m_pUserSetInfo = nullptr;
	UserLayerSet_t	*m_pUserLayerSetInfo = nullptr;	

	CString			m_strModelPath = L"";		// Model Path
	PanelInfo_t		m_stPanelInfo;				// Product Info
	
	//Swath, Cell Info
	FovInfo_t		m_stFovInfo;		//FOV로 명칭 (Line Camera : Swath, Area Camera : Cell)
	CellInfo_t		m_stCellInfo;	

	//Step별 Pattern Width 참조용
	std::map<INT32, std::vector<CString>> m_vcPTWidth_Pos;	// [Step, Widths]//사용하지 않음
	std::map<INT32, std::vector<CString>> m_vcPTWidth_Neg;	// [Step, Widths]//사용하지 않음

	std::vector<  std::map<INT32, vector<CString>>>	m_vecmapvecWidth;//[Line, Arc, LineArc(크기 고정)][Step][Widths]//Width 단위 ; mil


	//m_vecmapvecWidth_Pos의 Step을 구분하지 않고 Line, Arc, LineArc로 묶음.
	std::vector < std::vector <LINEWIDTH>>	m_vecvecWidthSort;

	CUtils			Utils;
};
