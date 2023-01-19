#pragma once

#include "../iCommonIF/CommonIF-Struct.h"
#include "../iCommonIF/CommonIF-Error.h"
#include "../iCommonIF/CommonIF-Path.h"
#include "../iUtilities/iUtilities.h"

#define MAX_SECTION_TXT_SIZE				50
#define MAX_LOAD_TXT_SIZE					500

class CSpec
{
public:
	CSpec() {};
	~CSpec() {};

private:
	SystemSpec_t	m_stSysSpec;
	FeatureSpec_t   m_stFeatureSpec;

public:
	UINT32 LoadSystemSpec(CString strPath);
	UINT32 SaveSystemSpec(CString strPath);	

public:
	SystemSpec_t* GetSystemSpec() { return &m_stSysSpec; }

	FeatureSpec_t* GetFeatureSpec() { return &m_stFeatureSpec; }

	bool IsSpecLoaded() { return m_stSysSpec.bLoaded; }
	BasicSpec_t* GetBasicSpec() { return &m_stSysSpec.sysBasic; }

	AutoAlign_t* GetAutoAlignSpec() { return &m_stSysSpec.sysAutoAlign;  }

	//Basic Spec
	bool IsShowAllSteps() { return m_stSysSpec.sysBasic.bShowAllSteps; };
	bool IsProfileMode() { return m_stSysSpec.sysBasic.bProfileMode; };
	bool IsExceptDummy() { return m_stSysSpec.sysBasic.bExceptDummy; };
	bool IsKeepLocalCopy() { return m_stSysSpec.sysBasic.bKeepLocalCopy; };
	eUserSite GetUserSite() { return m_stSysSpec.sysBasic.UserSite; };
	eMachineType GetMachineType() { return m_stSysSpec.sysBasic.McType; }
	ePlatFormType GetPlatFormType() { return m_stSysSpec.sysBasic.PlatType; };

	eProductType GetProductType() { return m_stSysSpec.sysBasic.ProdType; };
	void SetProductType(eProductType prdType) { m_stSysSpec.sysBasic.ProdType = prdType; }

	eCamType GetCamDataType() { return m_stSysSpec.sysBasic.CamType; };
	eMasterLevel GetMasterDataLevel() { return m_stSysSpec.sysBasic.MasterLevel; };	
	
	eScanType GetScanType() { return m_stSysSpec.sysBasic.ScanType; };
	void SetScanType(eScanType scanType) { m_stSysSpec.sysBasic.ScanType = scanType; };

	UINT32 GetMaxLayerNum() { return m_stSysSpec.sysBasic.iMaxLayerNum; }
	UINT32 GetIPUNum() { return m_stSysSpec.sysBasic.iIPUNum; }
	CString GetImageType() { return m_stSysSpec.sysBasic.strImageType; }

	// PATH
	CString	GetCamPath() { return m_stSysSpec.sysPath.strCamPath; };
	CString	GetMasterPath() { return m_stSysSpec.sysPath.strMasterPath; };
	CString	GetLogPath() { return m_stSysSpec.sysPath.strLogPath; };

private:
	UINT32 ReadStringINI(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpszFileName, UINT32& iOutput);
	UINT32 ReadStringINI(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpszFileName, double& dOutput);
	UINT32 ReadStringINI(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpszFileName, CString& strOutput);
	//YJD START 22.06.13 - SAVE MEASURE IMAGE
	UINT32 ReadStringINI(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpszFileName, bool& dOutput);
	//YJD END 22.06.13 - SAVE MEASURE IMAGE
	UINT32 WriteScalePRM(LPCTSTR lpszSection, LPCTSTR lpszKey, UINT32 iOutput, LPCTSTR lpszFileName, LPCTSTR lpszComment);
	UINT32 WriteScalePRM(LPCTSTR lpszSection, LPCTSTR lpszKey, double dOutput, LPCTSTR lpszFileName, LPCTSTR lpszComment);
	UINT32 WriteScalePRM(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR strOutput, LPCTSTR lpszFileName, LPCTSTR lpszComment);
	//YJD START 22.06.13 - SAVE MEASURE IMAGE
	UINT32 WriteScalePRM(LPCTSTR lpszSection, LPCTSTR lpszKey, bool iOutput, LPCTSTR lpszFileName, LPCTSTR lpszComment);
	//YJD END 22.06.13 - SAVE MEASURE IMAGE

	UINT32 LoadBasicSpec(CString fileName);
	UINT32 SaveBasicSpec(CString fileName);
	UINT32 LoadCameraSpec(bool bIsLine, CString fileName);
	UINT32 LoadCameraSpec_for_NSIS(bool bIs3D, CString fileName);
	UINT32 SaveCameraSpec(bool bIsLine, CString fileName);
	UINT32 SaveCameraSpec_for_NSIS(bool bIs3D, CString fileName);
	UINT32 LoadPathSpec(CString fileName);
	UINT32 SavePathSpec(CString fileName);
	UINT32 LoadStepSpec(CString fileName);
	UINT32 SaveStepSpec(CString fileName);
	UINT32 LoadLayerSpec(CString fileName);
	UINT32 SaveLayerSpec(CString fileName);

	//NSIS
	UINT32 LoadAutoAlignSpec(CString fileNmae);
	UINT32 LoadCameraOrientSpec(CString fileName);
	UINT32 SaveCameraOrientSpec(CString fileName);
	UINT32 LoadAlignRatioSpec(CString fileName);
	UINT32 SaveAlignRatioSpec(CString fileName);
	UINT32 LoadStaticOpticSpec(CString fileName);
	UINT32 SaveStaticOpticSpec(CString fileName);
	UINT32 LoadNetworkSpec(CString fileName);
	UINT32 SaveNetworkSpec(CString fileName);
	//YJD START 2022.04.08 - MEASURE TYPE SCALE
	UINT32 LoadMeasureScaleSpec(CString fileName);
	UINT32 SaveMeasureScaleSpec(CString fileName);
	CString GetMeasureTypeString(MeasureType type);
	//YJD END 2022.04.08 - MEASURE TYPE SCALE
	//YJD START 22.06.13 - SAVE MEASURE IMAGE
	UINT32 LoadMeasurePointSpec(CString fileName);
	UINT32 SaveMeasurePointSpec(CString fileName);
	//YJD END 22.06.13 - SAVE MEASURE IMAGE

	//Language 
	UINT32 LoadButtonName(CString fileName);
	CString GetLanguageString(CString strPreTemp, eLanguage eLan);

	UINT32 _SaveAlignSpec(CString fileName);
};