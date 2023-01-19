#pragma once

#include "../CDataManager.h"

// Verified with ODB++ of Daeduck Site
class __declspec(dllexport) CNSIS : public CDataManager
{
public:
	CNSIS();
	~CNSIS();

public:
	virtual UINT32 Destory();
	virtual UINT32 MakeMasterData();
	virtual UINT32 GetMeasureData(MeasurePanel_t* pMeasure = nullptr, BOOL bSave = FALSE, BOOL bUpdate = FALSE);
	
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
	virtual UINT32 CalculateUnitCoord(IN BOOL bFirstStep, IN UINT iShapeIdx, IN INT32 iStepIdx, IN enum Orient eOrient, IN CString strUnitStepName, IN CRotMirrArr *pRotMirrArr, IN CSubStep *pSubStep, OUT UINT32 *pRgnNum) 
	{
		return RESULT_GOOD;
	}

	virtual UINT32 RearrageIndexInfo(IN enum Orient eOrient);
	//Block Info
	virtual UINT32 EstimateBlockInfo() { return RESULT_GOOD; }
	//Save 
	virtual UINT32 SaveProductInfo(IN INT32 iStepIdx, IN enum Orient eOrient);	
	virtual UINT32 MakePanelAlignPointImage();
	virtual UINT32 SaveGrabInfo(INT32 iStepIdx, enum Orient eOrient) { return RESULT_GOOD; }
	//FOV
	virtual UINT32 OptimizeFOV(IN BOOL bSave = FALSE, IN BOOL bUpdate = FALSE) {	return RESULT_GOOD;	}

	//CrossPoint
	virtual UINT32 SetAlignCrossPoint(IN const INT32 & iStepIdx) { return RESULT_GOOD; }
	
private:	
	UINT32 CalculateUnitCoord(IN BOOL bFirstStep, IN UINT iShapeIdx, IN INT32 iStepIdx, IN INT32 iStripIdx, IN enum Orient eOrient, IN CString strUnitStepName, IN CRotMirrArr *pRotMirrArr, IN CSubStep *pSubStep, OUT UINT32 *pRgnNum);

	//Measure Info
	UINT32 EstimateMeasureInfo(IN INT32 iStepIdx, IN enum Orient eOrient);
	UINT32 CheckLensZoom(bool bIsAlign, MeasureItem_t* pItem);	
	// from CamMaster SNU make function
	UINT32 CheckLensZoom_SNU(double dWidth, double dHeight, MeasureItem_t* tempItem);
	std::vector<std::string> CNSIS::SplitString(std::string s, std::string divid);
	MeasureType GetMeasureType(CString strType);
	CString GetMeasureTypeString(MeasureType type);
	MeasureSide GetMeasureSide(CString strType);
	CString GetMeasureSideString(MeasureSide side);
	CString GetMagnificationString(BOOL bIs3D, INT8 iScanMode);
	CString GetMagnificationString(double dLens);
	CString GetZoomString(BOOL bIs3D, INT8 iScanMode);
	CString GetZoomString(double dZoom);
	UINT32 SortMeasureUnit();
	INT32 CheckYCount();

	// 2022.06.29
	// 김준호 주석추가
	MeasureType_SEM GetMeasureType_SEM(CString strType);
	CString GetMeasureTypeString_SEM(MeasureType_SEM type);
	// for SR Mode 
	MeasureType_SR_SEM GetMeasureType_SR_SEM(CString strType);
	CString GetMeasureTypeString_SR_SEM(MeasureType_SR_SEM type);

	// 2022.05.03
	// 주석추가 김준호 
	// NSIS Save 	
	UINT32 SaveProductInfo_Daeduck(IN INT32 iStepIdx, IN enum Orient eOrient);
	UINT32 SaveProductInfo_Sem(IN INT32 iStepIdx, IN enum Orient eOrient);
	// 2022.05.10
	// 주석추가 김준호 
	// GetMeasuerInfo 분리 
	UINT32 CNSIS::EstimateMeasureInfo_Daeduck(IN INT32 iStepIdx, IN enum Orient eOrient);
	UINT32 CNSIS::EstimateMeasureInfo_Sem(IN INT32 iStepIdx, IN enum Orient eOrient);
		
	void CorrectPos(bool bIsPanel, Orient ori, RECTD rcRect, double &dCenterX, double &dCenterY);
	SIZE GetFOVSize(bool bIs3D, double dLens, double dZoom);	
	
	//Master Image
	UINT32 MakePanelLayerImage();

	UINT32 MakeUnitLayerImage();
	UINT32 MakeUnitAlignPointImage();	
	UINT32 MakeUnitAlignPointImage_Sem();
	UINT32 MakeProfileImage();

	//YJD START 22.06.13 - SAVE MEASURE IMAGE
	UINT32 MakeMeasurePointImage();
	//YJD END 22.06.13 - SAVE MEASURE IMAGE

	CString GetModelPath(IN bool bCreateFolder);

	vector<vector<DrawProfileData>>	m_vecvecRect;//[Step][Rect]
};