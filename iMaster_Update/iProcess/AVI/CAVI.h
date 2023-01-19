#pragma once

#include "../CDataManager.h"

class __declspec(dllexport) CAVI : public CDataManager
{
public:
	CAVI();
	~CAVI();

public:
	virtual UINT32 Destory();
	virtual UINT32 MakeMasterData();
	virtual UINT32 GetMeasureData(MeasurePanel_t* pMeasure = nullptr, BOOL bSave = FALSE, BOOL bUpdate = FALSE) { return RESULT_BAD; };
	
	//Product Info
	virtual UINT32 EstimateProductInfo(IN INT32 iStepIdx, IN enum Orient eOrient);
	//Panel Info
	virtual UINT32 EstimatePanelInfo(IN INT32 iStepIdx, IN enum Orient eOrient);
	//Strip Info
	virtual UINT32 EstimateStripInfo(IN INT32 iStepIdx, IN enum Orient eOrient);
	virtual UINT32 CalculateStripNumOnSubStep(IN BOOL bFirstStep, IN CString strStripStepName, IN CSubStep* pSubStep, OUT UINT32 *pStripNum);
	virtual UINT32 CalculateStripCoord(IN BOOL bFirstStep, IN UINT iShapeIdx, IN INT32 iStepIdx, IN enum Orient eOrient, IN CString strStripStepName, IN CRotMirrArr *pRotMirrArr, IN CSubStep *pSubStep, OUT UINT32 *pRgnNum);
	virtual UINT32 CalculateOrient(IN enum Orient curOrient, OUT enum Orient* pTotOrient);
	//Unit Info
	virtual UINT32 EstimateUnitInfo(IN INT32 iStepIdx, IN enum Orient eOrient);
	virtual UINT32 CalculateUnitCoord(IN BOOL bFirstStep, IN UINT iShapeIdx, IN INT32 iStepIdx, IN enum Orient eOrient, IN CString strUnitStepName, IN CRotMirrArr *pRotMirrArr, IN CSubStep *pSubStep, OUT UINT32 *pRgnNum);
	virtual UINT32 RearrageIndexInfo(IN enum Orient eOrient);

	//Block Info
	virtual UINT32 EstimateBlockInfo();
	//Save 
	virtual UINT32 SaveProductInfo(IN INT32 iStepIdx, IN enum Orient eOrient);
	virtual UINT32 MakePanelAlignPointImage();
	virtual UINT32 SaveGrabInfo(INT32 iStepIdx, enum Orient eOrient);

	//FOV
	virtual UINT32 OptimizeFOV(IN BOOL bSave = FALSE, IN BOOL bUpdate = FALSE);

	//CrossPoint
	virtual UINT32 SetAlignCrossPoint(IN const INT32 & iStepIdx) { return RESULT_GOOD; }
	
private:

	//Check if 4-Step (PNL-QUAD-STRIP-UNIT)
	UINT32 CheckIf_SubStripStep_Exist(IN INT32 iStepIdx);
	UINT32 GetStripNum_In_Panel(IN INT32 iStepIdx, OUT UINT32& iTotalStripNum);

	UINT32 GetStripIndex(IN BOOL bFirstStep, IN CString strStepName, IN CString strUnitStepName, IN RECTD rcStepRect, OUT IN INT32 &iStripIdx);

	//Panel Align
	UINT32 ExtractAlignMarkInfo(IN INT32 iStepIdx, IN enum Orient eOrient);

	//Fov
	UINT32 EstimateFOV_Based_On_Swath(INT32 iStepIdx, enum Orient eOrient);
	//Cell
	UINT32 EstimateCell_Based_On_Swath(INT32 iStepIdx, enum Orient eOrient);
	UINT32 EstimateCell_Based_On_Swath_V2(INT32 iStepIdx, enum Orient eOrient);
	//Unit in Cell
	UINT32 EstimateUnitInfo_in_Cell();

	UINT32 EstimateStripInfo_in_Cell();

	//Unit Master Image
	UINT32 MakeUnitAlignPointImage();

	//Make Master Image
	UINT32 MakeMasterImage();
	UINT32 MakeMasterImage_Panel();
	UINT32 MakeMasterImage_Strip();
	UINT32 MakeMasterImage_Unit();

	UINT32 SaveCellInfo();
};