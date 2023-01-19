#pragma once

#include "../iCommonIF/CommonIF-Define.h"
#include "../iCommonIF/CommonIF-Struct.h"
#include "../iODB/iODB.h"
#include "DxDraw/Def_Viewer.h"
#include "DxDraw/CSaveLayer_AOI.h"
#include "DxDraw/CSaveLayer_NSIS.h"
#include "CDrawFunction.h"
#include <mutex>

class CDirectView;

class CSaveLayerJob
{
private:
	JobMode		m_eJobMode = JobMode::SaveLayer;

	CString		m_strLineArcWidth = _T("");//LineSubMode ArcSubMode, LineArcSubMode 일때의 해당 Width만 선택적으로 저장
	int			m_nWidthIndex = 0;
	bool		m_bExceptDummy = false;
	bool		m_bMirror = false;
	BOOL		m_bNeedUpdate = TRUE;
	BOOL		m_bSaveRaw = FALSE;
	//UINT32		m_iSwathIdx = 0;
	CString		m_strFileName = _T("");//확장자를 제외한 파일이름
	UINT32		m_iLayerIdx = 0;
	UINT32		m_iThreadIdx = 0;
	Orient		m_eOrient = Orient::NoMir0Deg;
	eMachineType m_eMcType = eMachineType::MachineTypeNone;
	SaveMode	m_eSaveMode = SaveMode::SaveMode_All;
	OpMode		m_eOpMode = OpMode::MakeLayer;
	eDrawLevel  m_eDrawLevel = eDrawLevel::ePanelLv;
	CString		m_strDstPath;

	double		m_dResolution = 0.0;
	double		m_dAngle = 0.0;
	D2D1_RECT_F m_frtSwath = D2D1::RectF(0.f, 0.f, 0.f, 0.f);
	CString		m_strFilePath = _T("");
	CString		m_strImageType = _T("");

	CStep		*m_pStep = nullptr;
	LayerSet_t	*m_pLayerSet = nullptr;
	CRotMirrArr *m_pRotMirrArr = nullptr;
	UserSetInfo_t *m_pUserSetInfo = nullptr;
	CTypedPtrArray <CObArray, CStandardFont*>* m_pFontArr = nullptr;

	CSaveLayer	*m_pSaveLayer = nullptr;

	//NSIS
	double		m_dCamAngle;

	//Concat Mode
	vector<CString> m_vecSrcPathName;

	//Get Filtered Feature
	CDirectView *m_pDirectView;
	vector < vector<CFeature*>> *m_pvecvecFilterdFeature;
	int m_eLineOption;
	vector<LINEWIDTH> *m_pvecLineWidth;
	RECTD			m_drtView_mm;
	//

	//Get Filtered Feature_V3
	vector < vector < vector<CFeature*>>> *m_pvecvecvecFilterdFeature_V3;
	vector < vector<LINEWIDTH>> *m_pvecvecLineWidth_V3;
	//

	//Save Filtered Feature
	vector<CFeature *> m_vecFeaure;

	
	

public:
	CSaveLayerJob() {}
	~CSaveLayerJob() { Clear(); }

	// 2022.10.04 KJH ADD
	// from SNU Code - MeasureItem Save Image 위해 주석처리	
	UINT32 SetData_SaveLayer(CString strFileName, UINT32 iLayerIdx, UINT32 iThreadIdx, CStep *pStep, LayerSet_t *pLayerSet, D2D1_RECT_F frtSwath,
		UserSetInfo_t *pUserSetInfo, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
		CString strFilePath, CString strImageType, double dResolution, eMachineType mcType, ID2D1Factory1 *pFactory,
		ID2D1DeviceContext*	pD2DContext, IWICImagingFactory2 *pImageFactory, SaveMode eSaveMode = SaveMode::SaveMode_All, eDrawLevel drawlevel = eDrawLevel::ePanelLv,
		bool bExceptDummy = false, OpMode eOpMode = OpMode::MakeLayer,	BOOL bSaveRaw = FALSE, double dAngle = UNKNOWN_ANGLE, bool bMirror = false, CString strDstPath = _T(""), 
		CString strLineArcWidth = _T(""), int nWidthIndex = 0)
	{
		if (pStep == nullptr || pLayerSet == nullptr || pUserSetInfo == nullptr) return RESULT_BAD;
		if (pFontArr == nullptr || strFilePath.GetLength() < 1 || strImageType.GetLength() < 1) return RESULT_BAD;
		if (pFactory == nullptr || pD2DContext == nullptr) return RESULT_BAD;

		m_eJobMode = JobMode::SaveLayer;

		//m_iSwathIdx = iSwathIdx;
		m_strFileName = strFileName;
		m_iLayerIdx = iLayerIdx;
		m_iThreadIdx = iThreadIdx;
		m_pStep = pStep;
		m_pLayerSet = pLayerSet;
		m_frtSwath = frtSwath;
		m_pUserSetInfo = pUserSetInfo;
		m_pFontArr = pFontArr;
		m_strFilePath = strFilePath;
		m_strImageType = strImageType;
		m_dResolution = dResolution;
		m_eMcType = mcType;
		m_eSaveMode = eSaveMode;
		m_bExceptDummy = bExceptDummy;
		m_eOpMode = eOpMode;
		m_eDrawLevel = drawlevel;
		m_dAngle = dAngle;
		m_bSaveRaw = bSaveRaw;

		m_bMirror = bMirror;		m_strDstPath = strDstPath;
		m_strLineArcWidth = strLineArcWidth;
		m_nWidthIndex = nWidthIndex;

		if (dAngle != UNKNOWN_ANGLE)
		{
			CDrawFunction::GetOrient(m_dAngle, m_bMirror, m_eOrient);

			m_pRotMirrArr = new CRotMirrArr;
			m_pRotMirrArr->InputData(0.0, 0.0, m_eOrient, m_pUserSetInfo->mirrorDir);
		}
		else
		{
			CDrawFunction::GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, m_eOrient);

			m_pRotMirrArr = new CRotMirrArr;
			m_pRotMirrArr->InputData(0.0, 0.0, m_eOrient, m_pUserSetInfo->mirrorDir);
		}

		return Init(pFactory, pD2DContext, pImageFactory, drawlevel);
	}
	
	
	UINT32 SetData_SaveLayer_SNU(CString strFileName, UINT32 iLayerIdx, UINT32 iThreadIdx, CStep *pStep, LayerSet_t *pLayerSet, D2D1_RECT_F frtSwath,
		UserSetInfo_t *pUserSetInfo, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
		CString strFilePath, CString strImageType, double dResolution, eMachineType mcType, ID2D1Factory1 *pFactory,
		ID2D1DeviceContext*	pD2DContext, IWICImagingFactory2 *pImageFactory, double dCamAngle, eDrawLevel drawlevel = eDrawLevel::ePanelLv,
		bool bExceptDummy = false, OpMode eOpMode = OpMode::MakeLayer, BOOL bSaveRaw = FALSE, double dAngle = UNKNOWN_ANGLE, bool bMirror = false, CString strDstPath = _T(""))
	{
		if (pStep == nullptr || pLayerSet == nullptr || pUserSetInfo == nullptr) return RESULT_BAD;
		if (pFontArr == nullptr || strFilePath.GetLength() < 1 || strImageType.GetLength() < 1) return RESULT_BAD;
		if (pFactory == nullptr || pD2DContext == nullptr) return RESULT_BAD;

		m_eJobMode = JobMode::SaveLayer;

		//m_iSwathIdx = iSwathIdx;
		m_strFileName = strFileName;
		m_iLayerIdx = iLayerIdx;
		m_iThreadIdx = iThreadIdx;
		m_pStep = pStep;
		m_pLayerSet = pLayerSet;
		m_frtSwath = frtSwath;
		m_pUserSetInfo = pUserSetInfo;
		m_pFontArr = pFontArr;
		m_strFilePath = strFilePath;
		m_strImageType = strImageType;
		m_dResolution = dResolution;
		m_eMcType = mcType;
		m_bExceptDummy = bExceptDummy;
		m_eOpMode = eOpMode;
		m_eDrawLevel = drawlevel;
		m_dAngle = dAngle;
		m_bSaveRaw = bSaveRaw;

		m_bMirror = bMirror;		m_strDstPath = strDstPath;

		m_dCamAngle = dCamAngle;

		if (dAngle != UNKNOWN_ANGLE)
		{
			CDrawFunction::GetOrient(m_dAngle, m_bMirror, m_eOrient);

			m_pRotMirrArr = new CRotMirrArr;
			m_pRotMirrArr->InputData(0.0, 0.0, m_eOrient);
		}
		else
		{
			CDrawFunction::GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, m_eOrient);

			m_pRotMirrArr = new CRotMirrArr;
			m_pRotMirrArr->InputData(0.0, 0.0, m_eOrient);
		}

		return Init(pFactory, pD2DContext, pImageFactory, drawlevel);
	}
	

	UINT32 SetData_ConcatRegion(vector<CString> vecSrcPathName, CString strDstPathName,
		UINT32 iThreadIdx,
		eMachineType mcType, ID2D1Factory1 *pFactory,
		ID2D1DeviceContext*	pD2DContext, IWICImagingFactory2 *pImageFactory)
	{
		m_eJobMode = JobMode::ConcatRegion;
		
		m_iThreadIdx = iThreadIdx;
		m_eMcType = mcType;

		m_vecSrcPathName = vecSrcPathName;
		m_strDstPath = strDstPathName;

		return Init(pFactory, pD2DContext, pImageFactory, eDrawLevel::ePanelLv);
	}

	UINT32 SetData_GetFilteredFeature(CDirectView *pDirectView, vector < vector<CFeature*>> *vecvecFeature, CStep *pStep, IN LayerSet_t* pLayerSet,
		UserSetInfo_t *pUserSetInfo, int &eLineOption, vector<LINEWIDTH> *pvecLineWidth, RECTD &drtView_mm,
		UINT32 iThreadIdx)
	{
		m_eJobMode = JobMode::GetFilteredFeature;

		m_iThreadIdx = iThreadIdx;

		m_pDirectView = pDirectView;

		m_pvecvecFilterdFeature = vecvecFeature;
		m_pStep = pStep;
		m_pLayerSet = pLayerSet;
		m_pUserSetInfo = pUserSetInfo;

		m_eLineOption = eLineOption;
		m_pvecLineWidth = pvecLineWidth;
		m_drtView_mm = drtView_mm;

		{
			CDrawFunction::GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, m_eOrient);

			m_pRotMirrArr = new CRotMirrArr;
			m_pRotMirrArr->InputData(0.0, 0.0, m_eOrient, m_pUserSetInfo->mirrorDir);
		}

		return RESULT_GOOD;
	}

	UINT32 SetData_GetFilteredFeature_V3(CDirectView *pDirectView, vector < vector < vector<CFeature*>>> *vecvecvecFeature, CStep *pStep, IN LayerSet_t* pLayerSet,
		UserSetInfo_t *pUserSetInfo, vector < vector<LINEWIDTH>> *pvecvecLineWidth, RECTD &drtView_mm,
		UINT32 iThreadIdx)
	{
		m_eJobMode = JobMode::GetFilteredFeature_V3;

		m_iThreadIdx = iThreadIdx;

		m_pDirectView = pDirectView;

		m_pvecvecvecFilterdFeature_V3 = vecvecvecFeature;
		m_pStep = pStep;
		m_pLayerSet = pLayerSet;
		m_pUserSetInfo = pUserSetInfo;

		m_pvecvecLineWidth_V3 = pvecvecLineWidth;
		m_drtView_mm = drtView_mm;

		{
			CDrawFunction::GetOrient(m_pUserSetInfo->dAngle, m_pUserSetInfo->bMirror, m_eOrient);

			m_pRotMirrArr = new CRotMirrArr;
			m_pRotMirrArr->InputData(0.0, 0.0, m_eOrient, m_pUserSetInfo->mirrorDir);
		}

		return RESULT_GOOD;
	}

	UINT32 SetData_FilteredFeature(vector<CFeature*> &vecFeature, 
		UINT32 iThreadIdx, LayerSet_t *pLayerSet, UserSetInfo_t *pUserSetInfo, CString strFilePath, CString strFileName,
		SaveMode eSaveMode, CString strImageType, int nImageIndex,
		D2D1_RECT_F frtSwath, double dResolution,
		eMachineType mcType, ID2D1Factory1 *pFactory,
		ID2D1DeviceContext*	pD2DContext, IWICImagingFactory2 *pImageFactory)
	{
		m_eJobMode = JobMode::SaveFilteredFeature;

		m_iThreadIdx = iThreadIdx;
		m_eMcType = mcType;

		m_pLayerSet = pLayerSet;
		m_pUserSetInfo = pUserSetInfo;
		m_strFilePath = strFilePath;
		m_strFileName = strFileName;
		m_eSaveMode = eSaveMode;
		m_strImageType = strImageType;
		m_nWidthIndex = nImageIndex;

		m_frtSwath = frtSwath;
		m_dResolution = dResolution;

		m_vecFeaure = vecFeature;

		return Init(pFactory, pD2DContext, pImageFactory, eDrawLevel::ePanelLv);
	}

	void Clear()
	{
		if (m_pRotMirrArr)
			delete m_pRotMirrArr;
		m_pRotMirrArr = nullptr;

		if (m_pSaveLayer)
			delete m_pSaveLayer;
		m_pSaveLayer = nullptr;
	}

	UINT32 Init(ID2D1Factory1 *pFactory,
		ID2D1DeviceContext*	pD2DContext, IWICImagingFactory2 *pImageFactory, eDrawLevel drawlevel)
	{
		if (m_pSaveLayer == nullptr)
		{
			if (m_eMcType == eMachineType::eNSIS)			
				m_pSaveLayer = new CSaveLayer_NSIS();
			else if (m_eMcType == eMachineType::eAOI)			
				m_pSaveLayer = new CSaveLayer_AOI();
			else if (m_eMcType == eMachineType::eAVI)
				m_pSaveLayer = new CSaveLayer_AOI();
			
			if (m_pSaveLayer == nullptr) 
				return RESULT_BAD;
		}

		m_pSaveLayer->SetLink(pImageFactory, pFactory, pD2DContext);
		m_pSaveLayer->SetDrawMode(drawlevel);
		return RESULT_GOOD;
	}
	
	JobMode GetJobMode() { return m_eJobMode; }

	//UINT32 GetSwathIdx() { return m_iSwathIdx; }
	CString GetFileName() { return m_strFileName; }
	UINT32 GetLayerIdx() { return m_iLayerIdx; }
	UINT32 GetThreadIdx() { return m_iThreadIdx; }
	CStep* GetStep() { return m_pStep; }
	LayerSet_t* GetLayerSet() { return m_pLayerSet; }
	D2D1_RECT_F GetSwathRect() { return m_frtSwath; }
	UserSetInfo_t* GetUserSetInfo() { return m_pUserSetInfo; }
	CRotMirrArr* GetRotMirrArr() { return m_pRotMirrArr; }
	CTypedPtrArray <CObArray, CStandardFont*>* GetFont() { return m_pFontArr; }
	CString GetFilePath() { return m_strFilePath; }
	double GetResolution() { return m_dResolution; }
	eMachineType GetMcType() { return m_eMcType; }
	double GetAngle() { return m_dAngle; }
	BOOL GetNeedUpdate() { return m_bNeedUpdate; }
	CSaveLayer* GetSaveLayer() { return m_pSaveLayer; }
	SaveMode GetSaveMode() { return	m_eSaveMode; }
	eDrawLevel GetDrawLevel() {	return m_eDrawLevel; }
	OpMode GetOpMode() { return m_eOpMode; }
	CString GetImageType() { return m_strImageType; }
	BOOL IsSaveRaw() { return m_bSaveRaw; }
	bool GetMirror() { return m_bMirror; }
	CString GetDestPath() {	return m_strDstPath;}
	bool IsExceptDummy() { return m_bExceptDummy; }
	CString GetLineArcWidth() { return m_strLineArcWidth; }
	int GetWidthIndex() { return m_nWidthIndex; }

	//Concat Region
	vector<CString> GetSrcPathName_Vector() { return m_vecSrcPathName; }

	//Get
	CDirectView *					GetDirectView() {	return m_pDirectView;}
	vector < vector<CFeature*>> *GetFilteredFeature_vecvec() { return m_pvecvecFilterdFeature; }
	int								GetLineOption() { return m_eLineOption; }
	vector<LINEWIDTH> *				GetLineWidth() { return m_pvecLineWidth; }
	RECTD							GetView_mm() { return m_drtView_mm; }
	//

	//V3
	vector < vector < vector<CFeature*>>> *GetFilteredFeature_V3() { return m_pvecvecvecFilterdFeature_V3; }
	vector < vector<LINEWIDTH>> *				GetLineWidth_V3() { return m_pvecvecLineWidth_V3; }

	//Save Filtered Feature
	vector<CFeature *> GetFilteredFeature() { return m_vecFeaure; }
};

class CSaveLayerOp
{
public:
	virtual bool AddJob(CSaveLayerJob* stJob) = 0;
	virtual bool RunJob(UINT32 nWorkIdx) = 0;
	virtual bool RunAllJobs() = 0;
	virtual void WaitForJobFinish() = 0;

	virtual void RemoveAllJobs() = 0;
	virtual bool ResetAllJobEvents() = 0;

	virtual bool StartThreads(UINT32 iLayerCount, UINT32 iThreadCount) = 0;
	virtual bool StopAllThreads() = 0;

	virtual void SetJobFile(CJobFile* pJobFile) = 0;
	virtual CJobFile* GetJobFile() = 0;

	virtual bool GetThreadStatus() = 0;
	virtual UINT32 GetThreadCount() = 0;

	virtual void SetParentInitThread(UINT32 iThreadCnt) = 0;
	virtual void CloseParentEvents() = 0;
	virtual void CreateParentEvents(UINT32 iLayerCount) = 0;
};

class CSaveLayerBase : public CSaveLayerOp
{
protected:
	UINT32 m_iThreadCount = 0;
	UINT32 m_iLayerCount = 0;

	CRITICAL_SECTION m_stJobCritcalSection;
	CJobFile* m_pJobFile = nullptr;

	std::mutex	m_Mutex;

public:
	bool m_bThreadStatus = false;
	HANDLE* m_phThreadEvent = nullptr;
	HANDLE* m_phJobComplete = nullptr;
	HANDLE** m_phParentFinish = nullptr;

public:
	void SetJobFile(CJobFile* pJobFile) { m_pJobFile = pJobFile; }
	CJobFile* GetJobFile() { return m_pJobFile; }
};