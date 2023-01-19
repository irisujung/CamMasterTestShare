#include "pch.h"
#include "SystemSpec.h"

UINT32 CSpec::LoadSystemSpec(CString strPath)
{
	m_stSysSpec.bLoaded = false;

	if (strPath.GetLength() < 1)
		return RESULT_BAD;

	CString fileName = strPath + SYSTEM_SPEC_FILE;
	CString LanguageFileName = strPath + LANGUAGE_SPEC_FILE;

	CFileFind file;
	if (!file.FindFile(fileName))
	{
		//Copy Spec File in FactorySetting
		CString factoryName = strPath + FACTORY_SYSTEM_SPEC_FILE;
		if (!file.FindFile(factoryName))
			return RESULT_BAD;

		CopyFile(factoryName, fileName, FALSE);
		if (!file.FindFile(fileName))
			return RESULT_BAD;
	}
	
	if (RESULT_GOOD != LoadBasicSpec(fileName))
		return RESULT_BAD;

	//Language 추가 
	if (RESULT_GOOD != LoadButtonName(LanguageFileName))
		return RESULT_BAD;

	if (eMachineType::eNSIS == m_stSysSpec.sysBasic.McType)
	{
		if (RESULT_GOOD != LoadCameraSpec_for_NSIS(true, fileName))
			return RESULT_BAD;

		if (RESULT_GOOD != LoadCameraSpec_for_NSIS(false, fileName))
			return RESULT_BAD;

		if (RESULT_GOOD != LoadCameraOrientSpec(fileName))
			return RESULT_BAD;

		if (RESULT_GOOD != LoadAlignRatioSpec(fileName))
			return RESULT_BAD;

		if (RESULT_GOOD != LoadNetworkSpec(fileName))
			return RESULT_BAD;

		if (RESULT_GOOD != LoadStaticOpticSpec(fileName))
			return RESULT_BAD;

		if (eUserSite::eSEM == m_stSysSpec.sysBasic.UserSite)
		{
			if (RESULT_GOOD != LoadAutoAlignSpec(fileName))
				return RESULT_BAD;
		}

		//YJD START 2022.04.08 - MEASURE TYPE SCALE
		if (RESULT_GOOD != LoadMeasureScaleSpec(fileName))
			return RESULT_BAD;
		//YJD END 2022.04.08 - MEASURE TYPE SCALE

		//YJD START 22.06.13 - SAVE MEASURE IMAGE	
		if (RESULT_GOOD != LoadMeasurePointSpec(fileName))
			return RESULT_BAD;
		//YJD END 22.06.13 - SAVE MEASURE IMAGE

	}
	else
	{
		if (RESULT_GOOD != LoadCameraSpec(true, fileName))
			return RESULT_BAD;

		if (RESULT_GOOD != LoadCameraSpec(false, fileName))
			return RESULT_BAD;
	}

	if (RESULT_GOOD != LoadPathSpec(fileName))
		return RESULT_BAD;

	if (RESULT_GOOD != LoadStepSpec(fileName))
		return RESULT_BAD;

	if (RESULT_GOOD != LoadLayerSpec(fileName))
		return RESULT_BAD;

	m_stSysSpec.bLoaded = true;
	return RESULT_GOOD;
}

UINT32 CSpec::SaveSystemSpec(CString strPath)
{
	if (strPath.GetLength() < 1)
		return RESULT_BAD;

	CString fileName = strPath + SYSTEM_SPEC_FILE;

	FILE* fp = NULL;
	_tfopen_s(&fp, fileName, _T("wt"));

	if (fp == NULL) return RESULT_BAD;
	if (fp) { fclose(fp);	fp = NULL; }

	if (RESULT_GOOD != SaveBasicSpec(fileName))
		return RESULT_BAD;

	if (m_stSysSpec.sysBasic.UserSite == eUserSite::eSEM && m_stSysSpec.sysBasic.McType == eMachineType::eNSIS)
	{
		if (RESULT_GOOD != _SaveAlignSpec(fileName))
			return RESULT_BAD;
	}


	if (eMachineType::eNSIS == m_stSysSpec.sysBasic.McType)
	{
		if (RESULT_GOOD != SaveCameraSpec_for_NSIS(true, fileName))
			return RESULT_BAD;

		if (RESULT_GOOD != SaveCameraSpec_for_NSIS(false, fileName))
			return RESULT_BAD;

		if (RESULT_GOOD != SaveCameraOrientSpec(fileName))
			return RESULT_BAD;

		if (RESULT_GOOD != SaveAlignRatioSpec(fileName))
			return RESULT_BAD;

		if (RESULT_GOOD != SaveNetworkSpec(fileName))
			return RESULT_BAD;

		if (RESULT_GOOD != SaveStaticOpticSpec(fileName))
			return RESULT_BAD;	

		if (RESULT_GOOD != SaveMeasureScaleSpec(fileName))
			return RESULT_BAD;

		//YJD START 22.06.13 - SAVE MEASURE IMAGE	
		if (RESULT_GOOD != SaveMeasurePointSpec(fileName))
			return RESULT_BAD;
		//YJD END 22.06.13 - SAVE MEASURE IMAGE
	}
	else
	{
		if (RESULT_GOOD != SaveCameraSpec(true, fileName))
			return RESULT_BAD;

		if (RESULT_GOOD != SaveCameraSpec(false, fileName))
			return RESULT_BAD;
	}

	if (RESULT_GOOD != SavePathSpec(fileName))
		return RESULT_BAD;

	if (RESULT_GOOD != SaveStepSpec(fileName))
		return RESULT_BAD;

	if (RESULT_GOOD != SaveLayerSpec(fileName))
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CSpec::LoadButtonName(CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	TCHAR strSection[MAX_SECTION_TXT_SIZE] = _T("");
	_stprintf_s(strSection, MAX_SECTION_TXT_SIZE, _T("LANGUAGE"));

	CString strTmp = _T("");
	CString strPre = _T("");
	
	CString strPost = _T("");

	int iErrCnt = 0;
	int nretval = 0;

	CString str = L"";

	eLanguage eLan = m_stSysSpec.sysBasic.LanguageType;

	//Category - CamMaster
	if (ReadStringINI(strSection, _T("CamMaster"), fileName, strTmp) != RESULT_GOOD) iErrCnt++;	
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecCamMaster.emplace_back(str);	
	
	//Category - View
	if (ReadStringINI(strSection, _T("Category"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;	
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecCategory.emplace_back(str);
	
	//Panel - Setting
	if (ReadStringINI(strSection, _T("Home"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecHome.emplace_back(str);
	
	if (ReadStringINI(strSection, _T("Refresh"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecRefresh.emplace_back(str);

	if (ReadStringINI(strSection, _T("Refresh_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecRefresh.emplace_back(str);

	if (ReadStringINI(strSection, _T("Mode"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecMode.emplace_back(str);
		
	if (ReadStringINI(strSection, _T("Setting"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecSetting.emplace_back(str);

	if (ReadStringINI(strSection, _T("Setting_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecSetting.emplace_back(str);
	
	if (ReadStringINI(strSection, _T("LineScan"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecLineScan.emplace_back(str);

	if (ReadStringINI(strSection, _T("LineScan_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecLineScan.emplace_back(str);

	if (ReadStringINI(strSection, _T("AreaScan"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecAreaScan.emplace_back(str);

	if (ReadStringINI(strSection, _T("AreaScan_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecAreaScan.emplace_back(str);

	if (ReadStringINI(strSection, _T("Panel"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecPanel.emplace_back(str);
	
	if (ReadStringINI(strSection, _T("Preview"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecPreview.emplace_back(str);
	
	if (ReadStringINI(strSection, _T("Manufacture"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecManufacture.emplace_back(str);
	
	if (ReadStringINI(strSection, _T("PreSR"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecPreSR.emplace_back(str);
	
	if (ReadStringINI(strSection, _T("PostSR"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecPostSR.emplace_back(str);
	
	if (ReadStringINI(strSection, _T("Side"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecSide.emplace_back(str);

	if (ReadStringINI(strSection, _T("Front"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecFront.emplace_back(str);

	if (ReadStringINI(strSection, _T("Back"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecBack.emplace_back(str);

	if (ReadStringINI(strSection, _T("Process"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecProcess.emplace_back(str);
	
	if (ReadStringINI(strSection, _T("LoadLayer"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecLoadLayer.emplace_back(str);

	if (ReadStringINI(strSection, _T("LoadLayer_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecLoadLayer.emplace_back(str);

	
	if (ReadStringINI(strSection, _T("MakeData"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecMakeData.emplace_back(str);

	if (ReadStringINI(strSection, _T("MakeData_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecMakeData.emplace_back(str);

	if (ReadStringINI(strSection, _T("TransferData"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecTransferData.emplace_back(str);

	if (ReadStringINI(strSection, _T("TransferData_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecTransferData.emplace_back(str);

	if (ReadStringINI(strSection, _T("MakeAlign"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecMakeAlign.emplace_back(str);

	if (ReadStringINI(strSection, _T("MakeAlign_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecMakeAlign.emplace_back(str);


	if (ReadStringINI(strSection, _T("Recipe"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecRecipe.emplace_back(str);

	if (ReadStringINI(strSection, _T("Recipe_Align"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecRecipeAlign.emplace_back(str);

	if (ReadStringINI(strSection, _T("Recipe_Align_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecRecipeAlign.emplace_back(str);

	if (ReadStringINI(strSection, _T("Recipe_Align_Auto"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecRecipeAlignAuto.emplace_back(str);

	if (ReadStringINI(strSection, _T("Recipe_Align_Manual"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecRecipeAlignManual.emplace_back(str);


	if (ReadStringINI(strSection, _T("Recipe_NSIS"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecRecipeNSIS.emplace_back(str);


	if (ReadStringINI(strSection, _T("Recipe_NSIS_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecRecipeNSIS.emplace_back(str);	

	if (ReadStringINI(strSection, _T("Recipe_SR"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecRecipeSR.emplace_back(str);

	if (ReadStringINI(strSection, _T("Recipe_SR_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecRecipeSR.emplace_back(str);
	
	if (ReadStringINI(strSection, _T("Recipe_ALL"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecRecipeALL.emplace_back(str);

	if (ReadStringINI(strSection, _T("Recipe_All_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecRecipeALL.emplace_back(str);
	
	if (ReadStringINI(strSection, _T("Edit"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecEdit.emplace_back(str);

	if (ReadStringINI(strSection, _T("Edit_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecEdit.emplace_back(str);

	if (ReadStringINI(strSection, _T("Edit_Align_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecEditAlign.emplace_back(strTmp);

	if (ReadStringINI(strSection, _T("ADDFeature"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecADDFeature.emplace_back(str);

	if (ReadStringINI(strSection, _T("ADD_X_LINE"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecADDFeature.emplace_back(str);

	if (ReadStringINI(strSection, _T("ADD_Y_LINE"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecADDFeature.emplace_back(str);

	if (ReadStringINI(strSection, _T("ADD_LINE"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecADDFeature.emplace_back(str);

	if (ReadStringINI(strSection, _T("ADD_X_LINE_FEATURE"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecADDFeature.emplace_back(str);

	if (ReadStringINI(strSection, _T("ADD_Y_LINE_FEATURE"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecADDFeature.emplace_back(str);

	if (ReadStringINI(strSection, _T("ADD_ARC"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecADDFeature.emplace_back(str);

	if (ReadStringINI(strSection, _T("ADD_CIRCLE"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecADDFeature.emplace_back(str);

	if (ReadStringINI(strSection, _T("ADD_RECTANGLE"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecADDFeature.emplace_back(str);

	if (ReadStringINI(strSection, _T("ADD_TP"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecADDFeature.emplace_back(str);

	if (ReadStringINI(strSection, _T("ADD_CIRCLE_FEATURE"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecADDFeature.emplace_back(str);

	if (ReadStringINI(strSection, _T("ADD_RECTANGLE_FEATURE"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecADDFeature.emplace_back(str);

	if (ReadStringINI(strSection, _T("ADD_SURFACE"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecADDFeature.emplace_back(str);

	if (ReadStringINI(strSection, _T("SHOW"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecShow.emplace_back(str);
	
	if (ReadStringINI(strSection, _T("Show_CAM_FOV"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecShowCAMFOV.emplace_back(str);

	if (ReadStringINI(strSection, _T("Show_CAM_CELL"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecShowCAMCELL.emplace_back(str);

	if (ReadStringINI(strSection, _T("Show_ALL"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecShowAll.emplace_back(str);

	if (ReadStringINI(strSection, _T("Show_ALL_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	m_stSysSpec.sysLanguage.vecShowAll.emplace_back(str);

	if (ReadStringINI(strSection, _T("Show_Profile"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecShowProfile.emplace_back(str);

	if (ReadStringINI(strSection, _T("Show_Profile_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecShowProfile.emplace_back(str);

	if (ReadStringINI(strSection, _T("Show_Dummy"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecShowDummy.emplace_back(str);

	if (ReadStringINI(strSection, _T("Show_Dummy_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecShowDummy.emplace_back(str);

	if (ReadStringINI(strSection, _T("Show_Surface"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecShowSurface.emplace_back(str);

	if (ReadStringINI(strSection, _T("Show_Surface_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecShowSurface.emplace_back(str);

	if (ReadStringINI(strSection, _T("Show_ALIGN"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecShowAlign.emplace_back(str);

	if (ReadStringINI(strSection, _T("Show_MASK"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecShowMask.emplace_back(str);
		
	if (ReadStringINI(strSection, _T("FOV"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecFOV.emplace_back(str);

	if (ReadStringINI(strSection, _T("WORK_DIR_HOR"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecWorkDirHor.emplace_back(str);

	if (ReadStringINI(strSection, _T("WORK_DIR_HOR2"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecWorkDirHor2.emplace_back(str);

	if (ReadStringINI(strSection, _T("WORK_DIR_VER"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecWorkDirVer.emplace_back(str);

	if (ReadStringINI(strSection, _T("VIEW_360"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecView360.emplace_back(str);

	if (ReadStringINI(strSection, _T("VIEW_90"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecView90.emplace_back(str);

	if (ReadStringINI(strSection, _T("VIEW_180"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecView180.emplace_back(str);

	if (ReadStringINI(strSection, _T("VIEW_270"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecView270.emplace_back(str);

	if (ReadStringINI(strSection, _T("VIEW_FOV2"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecViewFOV2.emplace_back(str);

	if (ReadStringINI(strSection, _T("SAVE_CATEGORY"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecSaveCategory.emplace_back(str);

	if (ReadStringINI(strSection, _T("SAVE"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecSave.emplace_back(str);

	if (ReadStringINI(strSection, _T("SAVEAS"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecSaveAs.emplace_back(str);

	if (ReadStringINI(strSection, _T("INFORMATION_CATEGORY"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecInformation.emplace_back(str);

	if (ReadStringINI(strSection, _T("RULER"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecRuler.emplace_back(str);

	if (ReadStringINI(strSection, _T("VERSION"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecVersion.emplace_back(str);

	if (ReadStringINI(strSection, _T("DOC_CATEGORY"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecDocCategory.emplace_back(str);
	
	if (ReadStringINI(strSection, _T("DOC_JOB"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecDocJob.emplace_back(str); 

	if (ReadStringINI(strSection, _T("DOC_JOB_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecDocJob.emplace_back(str);

	if (ReadStringINI(strSection, _T("DOC_CAM"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecDocCam.emplace_back(str); 

	if (ReadStringINI(strSection, _T("DOC_CAM_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecDocCam.emplace_back(str);

	if (ReadStringINI(strSection, _T("DOC_LAYER"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecDocLayer.emplace_back(str); 

	if (ReadStringINI(strSection, _T("DOC_LAYER_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecDocLayer.emplace_back(str);

	if (ReadStringINI(strSection, _T("DOC_MEASURE"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecDocMeasure.emplace_back(str); 

	if (ReadStringINI(strSection, _T("DOC_MEASURE_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecDocMeasure.emplace_back(str);

	if (ReadStringINI(strSection, _T("DOC_MINIMAP"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecDocMinimap.emplace_back(str); 

	if (ReadStringINI(strSection, _T("DOC_MINIMAP_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecDocMinimap.emplace_back(str);

	if (ReadStringINI(strSection, _T("DOC_LOG"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecDocLog.emplace_back(str);

	if (ReadStringINI(strSection, _T("DOC_LOG_Tooltip"), fileName, strTmp) != RESULT_GOOD)	iErrCnt++;
	str = GetLanguageString(strTmp, eLan);
	m_stSysSpec.sysLanguage.vecDocLog.emplace_back(str);

	return RESULT_GOOD;
}

CString CSpec::GetLanguageString(CString strPreTemp, eLanguage eLan)
{
	CString strTemp = L"";

	INT subIdx = 0;

	if (eLan == eLanguage::eKorea) subIdx = 0;
	else if (eLan == eLanguage::eEnglish) subIdx = 1;

	AfxExtractSubString(strTemp, strPreTemp, subIdx, ',');

	return strTemp;
}


UINT32 CSpec::LoadBasicSpec(CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	TCHAR	 strSection[MAX_SECTION_TXT_SIZE] = _T("");
	_stprintf_s(strSection, MAX_SECTION_TXT_SIZE, _T("BASIC"));

	UINT32 iValue = 0, iErrCnt = 0;
	if (ReadStringINI(strSection, _T("UserSite"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysBasic.UserSite = static_cast<eUserSite>(iValue);
	else
		iErrCnt++;

	if (ReadStringINI(strSection, _T("MachineType"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysBasic.McType = static_cast<eMachineType>(iValue);
	else
		iErrCnt++;

	if (ReadStringINI(strSection, _T("PlatFormType"), fileName, iValue) == RESULT_GOOD)	
		m_stSysSpec.sysBasic.PlatType = static_cast<ePlatFormType>(iValue);
	else
		iErrCnt++;
		
	if (ReadStringINI(strSection, _T("ProductType"), fileName, iValue) == RESULT_GOOD)		
		m_stSysSpec.sysBasic.ProdType = static_cast<eProductType>(iValue);
	else
		iErrCnt++;

	if (ReadStringINI(strSection, _T("CamDataType"), fileName, iValue) == RESULT_GOOD)		
		m_stSysSpec.sysBasic.CamType = static_cast<eCamType>(iValue);
	else 
		iErrCnt++;

	if (ReadStringINI(strSection, _T("MasterDataType"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysBasic.MasterLevel = static_cast<eMasterLevel>(iValue);
	else
		m_stSysSpec.sysBasic.MasterLevel = eMasterLevel::ePanelLv;

	if (ReadStringINI(strSection, _T("StepRepeatMode"), fileName, iValue) == RESULT_GOOD)	
		m_stSysSpec.sysBasic.bShowAllSteps = iValue ? true : false;
	else 
		iErrCnt++;

	if (ReadStringINI(strSection, _T("ProfileMode"), fileName, iValue) == RESULT_GOOD)	
		m_stSysSpec.sysBasic.bProfileMode = iValue ? true : false;
	else 
		iErrCnt++;

	if (ReadStringINI(strSection, _T("ExceptDummy"), fileName, iValue) == RESULT_GOOD)	
		m_stSysSpec.sysBasic.bExceptDummy = iValue ? true : false;
	else 
		iErrCnt++;

	if (ReadStringINI(strSection, _T("KeepLocalCopy"), fileName, iValue) == RESULT_GOOD)	
		m_stSysSpec.sysBasic.bKeepLocalCopy = iValue ? true : false;
	else 
		iErrCnt++;

	if (ReadStringINI(strSection, _T("ScanType"), fileName, iValue) == RESULT_GOOD)		
		m_stSysSpec.sysBasic.ScanType = static_cast<eScanType>(iValue);
	else 
		iErrCnt++;

	if (ReadStringINI(strSection, _T("MaxLayerNum"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysBasic.iMaxLayerNum = iValue;
	else
		iErrCnt++;

	if (ReadStringINI(strSection, _T("IPUNum"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysBasic.iIPUNum = iValue;
	else
		iErrCnt++;

	if (ReadStringINI(strSection, _T("ImageType"), fileName, m_stSysSpec.sysBasic.strImageType) != RESULT_GOOD)
	{
		m_stSysSpec.sysBasic.strImageType = _T("tif");
	}

	if (m_stSysSpec.sysBasic.McType == static_cast<eMachineType>(eMachineType::eNSIS))
	{
		m_stSysSpec.sysBasic.strImageType = _T("bmp");
		m_stSysSpec.sysBasic.bProfileMode = true;
	}
	else
		m_stSysSpec.sysBasic.bProfileMode = false;

	if (ReadStringINI(strSection, _T("ExceptImageData"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysBasic.bExceptImageData = iValue ? true : false;
	else
		m_stSysSpec.sysBasic.bExceptImageData = false;

	if (ReadStringINI(strSection, _T("LineArcSubSave"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysBasic.bLineArc_SubSave = iValue ? true : false;
	else
		m_stSysSpec.sysBasic.bLineArc_SubSave = false;
	
	if (ReadStringINI(strSection, _T("Language"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysBasic.LanguageType = static_cast<eLanguage>(iValue);

	//Load View 방향
	if (ReadStringINI(strSection, _T("View_MirrorDirection"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysBasic.emDirectionType = static_cast<eDirectionType>(iValue);
	else
		m_stSysSpec.sysBasic.emDirectionType = eDirectionType::DirectionTypeNone;

	if (ReadStringINI(strSection, _T("View_Rotate"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysBasic.dAngle = static_cast<double>(iValue);
	else
		m_stSysSpec.sysBasic.dAngle = 0;

	// 2022.10.26 KJH ADD
	// NSIS SEM Make Master Data 시 Back 면 방향 설정 위해 추가
	if (ReadStringINI(strSection, _T("Back_Mirror"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysBasic.bBackMirror = iValue ? true : false;
	else
		m_stSysSpec.sysBasic.bBackMirror = false;

	if (ReadStringINI(strSection, _T("Back_Angle"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysBasic.dBackAngle = static_cast<double>(iValue);
	else
		m_stSysSpec.sysBasic.dBackAngle = 0;

	// NSIS SEM Make Master Data 시 Image Save Pixel 기준 위해 추가
	if (ReadStringINI(strSection, _T("Save_Image_Width"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysBasic.dSaveImageWidth = static_cast<double>(iValue);
	else
		m_stSysSpec.sysBasic.dSaveImageWidth = 0;

	if (ReadStringINI(strSection, _T("Save_Image_Height"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysBasic.dSaveImageHeight = static_cast<double>(iValue);
	else
		m_stSysSpec.sysBasic.dSaveImageHeight = 0;

	if (iErrCnt) return RESULT_BAD;
	else return RESULT_GOOD;
}

UINT32 CSpec::SaveBasicSpec(CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	CString strSection = _T("BASIC");
	UINT32 rtn = 0, iErrCnt = 0;

	rtn = WriteScalePRM(strSection, _T("UserSite"), static_cast<UINT32>(m_stSysSpec.sysBasic.UserSite), fileName, _T("User Site"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("MachineType"), static_cast<UINT32>(m_stSysSpec.sysBasic.McType), fileName, _T("Machine Type"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("PlatFormType"), static_cast<UINT32>(m_stSysSpec.sysBasic.PlatType), fileName, _T("PlatForm Type"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("ProductType"), static_cast<UINT32>(m_stSysSpec.sysBasic.ProdType), fileName, _T("Product Type"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("CamDataType"), static_cast<UINT32>(m_stSysSpec.sysBasic.CamType), fileName, _T("CAM Data Type"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("MasterDataType"), static_cast<UINT32>(m_stSysSpec.sysBasic.MasterLevel), fileName, _T("Master Data Type"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("StepRepeatMode"), static_cast<UINT32>(m_stSysSpec.sysBasic.bShowAllSteps), fileName, _T("StepRepeat Mode"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("ProfileMode"), static_cast<UINT32>(m_stSysSpec.sysBasic.bProfileMode), fileName, _T("Profile Mode"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("ExceptDummy"), static_cast<UINT32>(m_stSysSpec.sysBasic.bExceptDummy), fileName, _T("Except Dummy"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("KeepLocalCopy"), static_cast<UINT32>(m_stSysSpec.sysBasic.bKeepLocalCopy), fileName, _T("Keep Local Copy"));
	if (rtn == RESULT_BAD) iErrCnt++;
	
	rtn = WriteScalePRM(strSection, _T("ScanType"), static_cast<UINT32>(m_stSysSpec.sysBasic.ScanType), fileName, _T("Scan Type"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("MaxLayerNum"), static_cast<UINT32>(m_stSysSpec.sysBasic.iMaxLayerNum), fileName, _T("Max Layer Num"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("IPUNum"), static_cast<UINT32>(m_stSysSpec.sysBasic.iIPUNum), fileName, _T("IPU Num"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("ImageType"), m_stSysSpec.sysBasic.strImageType, fileName, _T("Image Type"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("ExceptImageData"), static_cast<UINT32>(m_stSysSpec.sysBasic.bExceptImageData), fileName, _T("Except ImageData"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("LineArcSubSave"), static_cast<UINT32>(m_stSysSpec.sysBasic.bLineArc_SubSave), fileName, _T("Line Arc Save By Line Width"));
	if (rtn == RESULT_BAD) iErrCnt++;

	//Load View 방향
	rtn = WriteScalePRM(strSection, _T("View_MirrorDirection"), static_cast<UINT32>(m_stSysSpec.sysBasic.emDirectionType), fileName, _T(""));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("View_Rotate"), static_cast<UINT32>(m_stSysSpec.sysBasic.dAngle), fileName, _T(""));
	if (rtn == RESULT_BAD) iErrCnt++;

	//마스터 데이터 생성시 Back 방향
	rtn = WriteScalePRM(strSection, _T("Back_Mirror"), static_cast<UINT32>(m_stSysSpec.sysBasic.bBackMirror), fileName, _T(""));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("Back_Angle"), static_cast<UINT32>(m_stSysSpec.sysBasic.dBackAngle), fileName, _T(""));
	if (rtn == RESULT_BAD) iErrCnt++;

	// NSIS SEM Make Master Data 시 Image Save Pixel 기준 위해 추가
	rtn = WriteScalePRM(strSection, _T("Save_Image_Width"), static_cast<UINT32>(m_stSysSpec.sysBasic.dSaveImageWidth), fileName, _T(""));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("Save_Image_Height"), static_cast<UINT32>(m_stSysSpec.sysBasic.dSaveImageHeight), fileName, _T(""));
	if (rtn == RESULT_BAD) iErrCnt++;

	if (iErrCnt) return RESULT_BAD;
	else return RESULT_GOOD;
}

UINT32 CSpec::LoadCameraSpec(bool bIsLine, CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	TCHAR	 strSection[MAX_SECTION_TXT_SIZE] = _T("");

	if (bIsLine)
		_stprintf_s(strSection, MAX_SECTION_TXT_SIZE, _T("LINE_CAMERA"));
	else
		_stprintf_s(strSection, MAX_SECTION_TXT_SIZE, _T("AREA_CAMERA"));

	double dValue = 0.f;
	UINT32 iValue = 0, iErrCnt = 0;
	if (ReadStringINI(strSection, _T("Resolution"), fileName, dValue) == RESULT_GOOD)
		m_stSysSpec.sysCamera[bIsLine].dResolution = dValue;
	else
		iErrCnt++;

	if (ReadStringINI(strSection, _T("FrameWidth"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysCamera[bIsLine].iFrameWidth = iValue;
	else
		iErrCnt++;

	if (ReadStringINI(strSection, _T("FrameHeight"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysCamera[bIsLine].iFrameHeight = iValue;
	else
		iErrCnt++;

	if (ReadStringINI(strSection, _T("MinOverlapX"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysCamera[bIsLine].iMinOverlapX = iValue;
	else
		iErrCnt++;

	if (ReadStringINI(strSection, _T("MinOverlapY"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysCamera[bIsLine].iMinOverlapY = iValue;
	else
		iErrCnt++;

	if (ReadStringINI(strSection, _T("MinMarginX"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysCamera[bIsLine].iMinMarginX = iValue;
	else
		iErrCnt++;

	if (ReadStringINI(strSection, _T("MinMarginY"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysCamera[bIsLine].iMinMarginY = iValue;
	else
		iErrCnt++;

	if (ReadStringINI(strSection, _T("DummyMarginX"), fileName, dValue) == RESULT_GOOD)
		m_stSysSpec.sysCamera[bIsLine].dDummyMarginX = dValue;
	else
		m_stSysSpec.sysCamera[bIsLine].dDummyMarginX = 2.0;

	if (ReadStringINI(strSection, _T("DummyMarginY"), fileName, dValue) == RESULT_GOOD)
		m_stSysSpec.sysCamera[bIsLine].dDummyMarginY = dValue;
	else
		m_stSysSpec.sysCamera[bIsLine].dDummyMarginY = 2.0;

	if (ReadStringINI(strSection, _T("CellColNum"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysCamera[bIsLine].iCellColNum = iValue;
	else
		iErrCnt++;

	if (ReadStringINI(strSection, _T("CellRowNum"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysCamera[bIsLine].iCellRowNum = iValue;
	else
		iErrCnt++;

	if (ReadStringINI(strSection, _T("CellMinOverlapX"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysCamera[bIsLine].iCellMinOverlapX = iValue;
	else
		iErrCnt++;

	if (ReadStringINI(strSection, _T("CellMinOverlapY"), fileName, iValue) == RESULT_GOOD)
		m_stSysSpec.sysCamera[bIsLine].iCellMinOverlapY = iValue;
	else
		iErrCnt++;

	if (iErrCnt) return RESULT_BAD;
	else return RESULT_GOOD;
}

UINT32 CSpec::LoadCameraSpec_for_NSIS(bool bIs3D, CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	TCHAR	 strSection[MAX_SECTION_TXT_SIZE] = _T("");

	if (bIs3D)
	{
		m_stSysSpec.sysNSIS.Clear3DCamera();
		_stprintf_s(strSection, MAX_SECTION_TXT_SIZE, _T("3D_MEASURE"));
	}
	else
	{
		m_stSysSpec.sysNSIS.Clear2DCamera();
		_stprintf_s(strSection, MAX_SECTION_TXT_SIZE, _T("2D_MEASURE"));
	}

	CString strSub = _T("");
	double dValue = 0.f;
	UINT32 iValue = 0, iModeNum = 0, iErrCnt = 0;

	if (ReadStringINI(strSection, _T("ModeNum"), fileName, iValue) == RESULT_GOOD)
		iModeNum = iValue;
	else
		iErrCnt++;

	for (UINT32 iLoop = 0; iLoop < iModeNum; iLoop++)
	{
		NSIS_CameraSpec_t* pSpec = new NSIS_CameraSpec_t;

		strSub.Format(_T("Mode%02d_Mag"), iLoop + 1);
		if (ReadStringINI(strSection, strSub, fileName, dValue) == RESULT_GOOD)
			pSpec->dMagnification = dValue;
		else
			iErrCnt++;

		strSub.Format(_T("Mode%02d_Zoom"), iLoop + 1);
		if (ReadStringINI(strSection, strSub, fileName, dValue) == RESULT_GOOD)
			pSpec->dZoom = dValue;
		else
			iErrCnt++;

		strSub.Format(_T("Mode%02d_Resolution"), iLoop + 1);
		if (ReadStringINI(strSection, strSub, fileName, dValue) == RESULT_GOOD)
			pSpec->dResolution = dValue;
		else
			iErrCnt++;

		strSub.Format(_T("Mode%02d_FOV_Width"), iLoop + 1);
		if (ReadStringINI(strSection, strSub, fileName, iValue) == RESULT_GOOD)
			pSpec->iFovWidth = iValue;
		else
			iErrCnt++;

		strSub.Format(_T("Mode%02d_FOV_Height"), iLoop + 1);
		if (ReadStringINI(strSection, strSub, fileName, iValue) == RESULT_GOOD)
			pSpec->iFovHeight = iValue;
		else
			iErrCnt++;

		if (bIs3D)
			m_stSysSpec.sysNSIS.sys3DCamera.emplace_back(pSpec);
		else
			m_stSysSpec.sysNSIS.sys2DCamera.emplace_back(pSpec);
	}

	if (bIs3D)
	{
		if (iModeNum != static_cast<UINT32>(m_stSysSpec.sysNSIS.sys3DCamera.size()))
			return RESULT_BAD;
	}
	else
	{
		if (iModeNum != static_cast<UINT32>(m_stSysSpec.sysNSIS.sys2DCamera.size()))
			return RESULT_BAD;
	}

	if (iErrCnt) return RESULT_BAD;
	else
		return RESULT_GOOD;
}

UINT32 CSpec::LoadAutoAlignSpec(CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	TCHAR	 strSection[MAX_SECTION_TXT_SIZE] = _T("");
	_stprintf_s(strSection, MAX_SECTION_TXT_SIZE, _T("AUTO_ALIGN"));

	UINT32 iErrCnt = 0, iValue = 0;	
	if (ReadStringINI(strSection, _T("AUTOALIGN"), fileName, iValue) != RESULT_GOOD)
		iErrCnt++;

	m_stSysSpec.sysAutoAlign.bAutoAlign = iValue;

	double dValue = 0.f;

	if (ReadStringINI(strSection, _T("RECTWIDTH"), fileName, dValue) != RESULT_GOOD)
		iErrCnt++;	

	m_stSysSpec.sysAutoAlign.dRectWidth = dValue;

	if (ReadStringINI(strSection, _T("RECTHEIGHT"), fileName, dValue) != RESULT_GOOD)
	{
		m_stSysSpec.sysAutoAlign.dRectHeight = 20;
		iErrCnt++;
	}
	m_stSysSpec.sysAutoAlign.dRectHeight = dValue;

	CString strSub = _T("");
	CString strSub2 = _T("");
	for (UINT32 iLoop = 0; iLoop < 4; iLoop++)
	{
		NSIS_AutoAlign_t* pSpec = new NSIS_AutoAlign_t;

		strSub.Format(_T("AUTOALIGN_X%d"), iLoop + 1);
		if (ReadStringINI(strSection, strSub, fileName, dValue) != RESULT_GOOD)
			iErrCnt++;		

		pSpec->dAutoAlignX = dValue;

		strSub2.Format(_T("AUTOALIGN_Y%d"), iLoop + 1);
		if (ReadStringINI(strSection, strSub2, fileName, dValue) != RESULT_GOOD)
			iErrCnt++;

		pSpec->dAutoAlignY = dValue;

		if (iErrCnt <= 0 )
			m_stSysSpec.sysAutoAlign.vecAutoAlign.push_back(pSpec);
		
	}

	if (iErrCnt) 
		return RESULT_BAD;
	else
		return RESULT_GOOD;
}

UINT32 CSpec::SaveCameraSpec(bool bIsLine, CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	CString strSection = _T("AREA_CAMERA");
	if (bIsLine)
		strSection = _T("LINE_CAMERA");

	UINT32 rtn = 0, iErrCnt = 0;

	rtn = WriteScalePRM(strSection, _T("Resolution"), m_stSysSpec.sysCamera[bIsLine].dResolution, fileName, _T("Resolution"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("FrameWidth"), m_stSysSpec.sysCamera[bIsLine].iFrameWidth, fileName, _T("Frame Width"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("FrameHeight"), m_stSysSpec.sysCamera[bIsLine].iFrameHeight, fileName, _T("Frame Height"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("MinOverlapX"), m_stSysSpec.sysCamera[bIsLine].iMinOverlapX, fileName, _T("Min Overlap X"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("MinOverlapY"), m_stSysSpec.sysCamera[bIsLine].iMinOverlapY, fileName, _T("Min Overlap Y"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("MinMarginX"), m_stSysSpec.sysCamera[bIsLine].iMinMarginX, fileName, _T("Min Margin X"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("MinMarginY"), m_stSysSpec.sysCamera[bIsLine].iMinMarginY, fileName, _T("Min Margin Y"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("DummyMarginX"), m_stSysSpec.sysCamera[bIsLine].dDummyMarginX, fileName, _T("Dummy Margin X"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("DummyMarginY"), m_stSysSpec.sysCamera[bIsLine].dDummyMarginY, fileName, _T("Dummy Margin Y"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("CellColNum"), m_stSysSpec.sysCamera[bIsLine].iCellColNum, fileName, _T("Cell Col Num"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("CellRowNum"), m_stSysSpec.sysCamera[bIsLine].iCellRowNum, fileName, _T("Cell Row Num"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("CellMinOverlapX"), m_stSysSpec.sysCamera[bIsLine].iCellMinOverlapX, fileName, _T("Cell Min Overlap X"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("CellMinOverlapY"), m_stSysSpec.sysCamera[bIsLine].iCellMinOverlapY, fileName, _T("Cell Min Overlap Y"));
	if (rtn == RESULT_BAD) iErrCnt++;

	if (iErrCnt) return RESULT_BAD;
	else return RESULT_GOOD;
}

UINT32 CSpec::SaveCameraSpec_for_NSIS(bool bIs3D, CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	UINT32 rtn = 0, iErrCnt = 0, iLoop = 0;
	CString strSection = _T(""), strSub = _T("");

	if (bIs3D)
	{
		strSection = _T("3D_MEASURE");

		strSub.Format(_T("ModeNum"));
		rtn = WriteScalePRM(strSection, strSub, (UINT32)m_stSysSpec.sysNSIS.sys3DCamera.size(), fileName, _T("ModeNum"));
		if (rtn == RESULT_BAD) iErrCnt++;

		for (auto it : m_stSysSpec.sysNSIS.sys3DCamera)
		{
			strSub.Format(_T("Mode%02d_Mag"), iLoop + 1);
			rtn = WriteScalePRM(strSection, strSub, it->dMagnification, fileName, _T("Magnification"));
			if (rtn == RESULT_BAD) iErrCnt++;

			strSub.Format(_T("Mode%02d_Zoom"), iLoop + 1);
			rtn = WriteScalePRM(strSection, strSub, it->dZoom, fileName, _T("Zoom"));
			if (rtn == RESULT_BAD) iErrCnt++;

			strSub.Format(_T("Mode%02d_Resolution"), iLoop + 1);
			rtn = WriteScalePRM(strSection, strSub, it->dMagnification, fileName, _T("Resolution"));
			if (rtn == RESULT_BAD) iErrCnt++;

			strSub.Format(_T("Mode%02d_FOV_Width"), iLoop + 1);
			rtn = WriteScalePRM(strSection, strSub, it->iFovWidth, fileName, _T("FOV Width"));
			if (rtn == RESULT_BAD) iErrCnt++;

			strSub.Format(_T("Mode%02d_FOV_Height"), iLoop + 1);
			rtn = WriteScalePRM(strSection, strSub, it->iFovHeight, fileName, _T("FOV Height"));

			iLoop++;
		}
	}
	else
	{
		strSection = _T("2D_MEASURE");

		strSub.Format(_T("ModeNum"));
		rtn = WriteScalePRM(strSection, strSub, (UINT32)m_stSysSpec.sysNSIS.sys2DCamera.size(), fileName, _T("ModeNum"));
		if (rtn == RESULT_BAD) iErrCnt++;

		for (auto it : m_stSysSpec.sysNSIS.sys2DCamera)
		{
			strSub.Format(_T("Mode%02d_Mag"), iLoop + 1);
			rtn = WriteScalePRM(strSection, strSub, it->dMagnification, fileName, _T("Magnification"));
			if (rtn == RESULT_BAD) iErrCnt++;

			strSub.Format(_T("Mode%02d_Zoom"), iLoop + 1);
			rtn = WriteScalePRM(strSection, strSub, it->dZoom, fileName, _T("Zoom"));
			if (rtn == RESULT_BAD) iErrCnt++;

			strSub.Format(_T("Mode%02d_Resolution"), iLoop + 1);
			rtn = WriteScalePRM(strSection, strSub, it->dMagnification, fileName, _T("Resolution"));
			if (rtn == RESULT_BAD) iErrCnt++;

			strSub.Format(_T("Mode%02d_FOV_Width"), iLoop + 1);
			rtn = WriteScalePRM(strSection, strSub, it->iFovWidth, fileName, _T("FOV Width"));
			if (rtn == RESULT_BAD) iErrCnt++;

			strSub.Format(_T("Mode%02d_FOV_Height"), iLoop + 1);
			rtn = WriteScalePRM(strSection, strSub, it->iFovHeight, fileName, _T("FOV Height"));

			iLoop++;
		}
	}

	if (iErrCnt) return RESULT_BAD;
	else return RESULT_GOOD;
}


UINT32 CSpec::_SaveAlignSpec(CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	UINT32 rtn = 0, iErrCnt = 0, iLoop = 0;
	CString strSection = _T(""), strSub = _T("");
		
	strSection = _T("AUTO_ALIGN");

	strSub.Format(_T("AUTOALIGN"));
	rtn = WriteScalePRM(strSection, strSub, (UINT32)m_stSysSpec.sysAutoAlign.bAutoAlign, fileName, _T("AUTOALIGN"));
	if (rtn == RESULT_BAD) iErrCnt++;

	strSub.Format(_T("RECTWIDTH"));
	rtn = WriteScalePRM(strSection, strSub, (UINT32)m_stSysSpec.sysAutoAlign.dRectWidth, fileName, _T("RECTWIDTH"));
	if (rtn == RESULT_BAD) iErrCnt++;

	strSub.Format(_T("RECTHEIGHT"));
	rtn = WriteScalePRM(strSection, strSub, (UINT32)m_stSysSpec.sysAutoAlign.dRectHeight, fileName, _T("RECTHEIGHT"));
	if (rtn == RESULT_BAD) iErrCnt++;

	for (auto it : m_stSysSpec.sysAutoAlign.vecAutoAlign)
	{
		strSub.Format(_T("AUTOALIGN_X%d"), iLoop + 1);
		rtn = WriteScalePRM(strSection, strSub, it->dAutoAlignX, fileName, strSub);
		if (rtn == RESULT_BAD) iErrCnt++;

		strSub.Format(_T("AUTOALIGN_Y%d"), iLoop + 1);
		rtn = WriteScalePRM(strSection, strSub, it->dAutoAlignY, fileName, strSub);
		if (rtn == RESULT_BAD) iErrCnt++;

		iLoop++;
	}	

	if (iErrCnt) return RESULT_BAD;
	else return RESULT_GOOD;
}


UINT32 CSpec::LoadPathSpec(CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	TCHAR	 strSection[MAX_SECTION_TXT_SIZE] = _T("");
	_stprintf_s(strSection, MAX_SECTION_TXT_SIZE, _T("PATH"));

	UINT32 iErrCnt = 0;
	if (ReadStringINI(strSection, _T("Path_CamData"), fileName, m_stSysSpec.sysPath.strCamPath) != RESULT_GOOD)
		iErrCnt++;

	if (ReadStringINI(strSection, _T("Path_MasterData"), fileName, m_stSysSpec.sysPath.strMasterPath) != RESULT_GOOD)
		iErrCnt++;

	if (ReadStringINI(strSection, _T("Path_LogData"), fileName, m_stSysSpec.sysPath.strLogPath) != RESULT_GOOD)
		iErrCnt++;	

	if (iErrCnt) return RESULT_BAD;
	else
	{
		CUtils Utils;
		Utils.CheckCreateFolder(m_stSysSpec.sysPath.strCamPath);
		Utils.CheckCreateFolder(m_stSysSpec.sysPath.strMasterPath);
		Utils.CheckCreateFolder(m_stSysSpec.sysPath.strLogPath);
		return RESULT_GOOD;
	}
}

UINT32 CSpec::SavePathSpec(CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	CString strSection = _T("PATH");
	UINT32 rtn = 0, iErrCnt = 0;

	rtn = WriteScalePRM(strSection, _T("Path_CamData"), m_stSysSpec.sysPath.strCamPath, fileName, _T("Cam Data Path"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("Path_MasterData"), m_stSysSpec.sysPath.strMasterPath, fileName, _T("Master Data Path"));
	if (rtn == RESULT_BAD) iErrCnt++;

	rtn = WriteScalePRM(strSection, _T("Path_LogData"), m_stSysSpec.sysPath.strLogPath, fileName, _T("Log Data Path"));
	if (rtn == RESULT_BAD) iErrCnt++;

	if (iErrCnt) return RESULT_BAD;
	else return RESULT_GOOD;
}

UINT32 CSpec::LoadStepSpec(CString fileName)
{
	m_stSysSpec.sysStep.Clear();

	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	TCHAR	 strSection[MAX_SECTION_TXT_SIZE] = _T("");
	_stprintf_s(strSection, MAX_SECTION_TXT_SIZE, _T("STEP"));

	UINT32 iErrCnt = 0;

	UINT32 iStepNum = 0;
	if (ReadStringINI(strSection, _T("Ref_Panel_Num"), fileName, iStepNum) != RESULT_GOOD)
		iErrCnt++;

	CString strTmp = L"";
	if (ReadStringINI(strSection, _T("Ref_Panel_Name"), fileName, strTmp) != RESULT_GOOD)
		iErrCnt++;

	CString str = L"";
	while (strTmp.GetLength() > 0)
	{
		AfxExtractSubString(str, strTmp, 0, ',');
		m_stSysSpec.sysStep.vcPanelNames.emplace_back(str);
		strTmp.Delete(0, str.GetLength() + 1);
	}

	if (iStepNum != static_cast<UINT32>(m_stSysSpec.sysStep.vcPanelNames.size()))
		return RESULT_BAD;

	iStepNum = 0;
	if (ReadStringINI(strSection, _T("Ref_Strip_Num"), fileName, iStepNum) != RESULT_GOOD)
		iErrCnt++;

	strTmp = L"";
	if (ReadStringINI(strSection, _T("Ref_Strip_Name"), fileName, strTmp) != RESULT_GOOD)
		iErrCnt++;

	str = L"";
	while (strTmp.GetLength() > 0)
	{
		AfxExtractSubString(str, strTmp, 0, ',');
		m_stSysSpec.sysStep.vcStripNames.emplace_back(str);
		strTmp.Delete(0, str.GetLength() + 1);
	}

	if (iStepNum != static_cast<UINT32>(m_stSysSpec.sysStep.vcStripNames.size()))
		return RESULT_BAD;

	iStepNum = 0;
	if (ReadStringINI(strSection, _T("Ref_Unit_Num"), fileName, iStepNum) != RESULT_GOOD)
		iErrCnt++;

	strTmp = L"";
	if (ReadStringINI(strSection, _T("Ref_Unit_Name"), fileName, strTmp) != RESULT_GOOD)
		iErrCnt++;

	str = L"";
	while (strTmp.GetLength() > 0)
	{
		AfxExtractSubString(str, strTmp, 0, ',');
		m_stSysSpec.sysStep.vcUnitNames.emplace_back(str);
		strTmp.Delete(0, str.GetLength() + 1);
	}

	if (iStepNum != static_cast<UINT32>(m_stSysSpec.sysStep.vcUnitNames.size()))
		return RESULT_BAD;

	if (iErrCnt) return RESULT_BAD;
	else return RESULT_GOOD;
}

UINT32 CSpec::SaveStepSpec(CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	CString strSection = _T("STEP");
	UINT32 rtn = 0, iErrCnt = 0;

	UINT32 iStepNum = static_cast<UINT32>(m_stSysSpec.sysStep.vcPanelNames.size());
	rtn = WriteScalePRM(strSection, _T("Ref_Panel_Num"), iStepNum, fileName, _T("Reference Panel Num"));
	if (rtn == RESULT_BAD) iErrCnt++;
	 
	CString str = L"";
	for (auto it : m_stSysSpec.sysStep.vcPanelNames)
	{
		str += it;
		str += L",";
	}
	rtn = WriteScalePRM(strSection, _T("Ref_Panel_Name"), str, fileName, _T("Reference Panel Name"));
	if (rtn == RESULT_BAD) iErrCnt++;

	////////////////////////////////////////////////
	iStepNum = static_cast<UINT32>(m_stSysSpec.sysStep.vcStripNames.size());
	rtn = WriteScalePRM(strSection, _T("Ref_Strip_Num"), iStepNum, fileName, _T("Reference Strip Num"));
	if (rtn == RESULT_BAD) iErrCnt++;

	str = L"";
	for (auto it : m_stSysSpec.sysStep.vcStripNames)
	{
		str += it;
		str += L",";
	}
	rtn = WriteScalePRM(strSection, _T("Ref_Strip_Name"), str, fileName, _T("Reference Strip Name"));
	if (rtn == RESULT_BAD) iErrCnt++;

	////////////////////////////////////////////////
	iStepNum = static_cast<UINT32>(m_stSysSpec.sysStep.vcUnitNames.size());
	rtn = WriteScalePRM(strSection, _T("Ref_Unit_Num"), iStepNum, fileName, _T("Reference Unit Num"));
	if (rtn == RESULT_BAD) iErrCnt++;

	str = L"";
	for (auto it : m_stSysSpec.sysStep.vcUnitNames)
	{
		str += it;
		str += L",";
	}
	rtn = WriteScalePRM(strSection, _T("Ref_Unit_Name"), str, fileName, _T("Reference Unit Name"));
	if (rtn == RESULT_BAD) iErrCnt++;

	if (iErrCnt) return RESULT_BAD;
	else return RESULT_GOOD;
}

UINT32 CSpec::LoadLayerSpec(CString fileName)
{
	m_stSysSpec.sysLayer.Clear();

	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	TCHAR	 strSection[MAX_SECTION_TXT_SIZE] = _T("");
	_stprintf_s(strSection, MAX_SECTION_TXT_SIZE, _T("LAYER"));

	UINT32 iLayerCnt = 0, iValue = 0, iErrCnt = 0;
	CString strKey = L"", str = L"", strTmp = L"";
	if (ReadStringINI(strSection, _T("Ref_Layer_Num"), fileName, iLayerCnt) == RESULT_GOOD)
	{		
		for (UINT32 iLayer = 0; iLayer < iLayerCnt; iLayer++)
		{
			LayerInfo_t* pLayer = new LayerInfo_t();

			strKey.Format(_T("Ref_Layer%02d_Define"), iLayer + 1);
			if (ReadStringINI(strSection, strKey, fileName, pLayer->strDefine) != RESULT_GOOD)
				iErrCnt++;

			strKey.Format(_T("Ref_Layer%02d_Color"), iLayer + 1);
			if (ReadStringINI(strSection, strKey, fileName, iValue) != RESULT_GOOD)
				iErrCnt++;
			pLayer->layerColor = static_cast<COLORREF>(iValue);

			strKey.Format(_T("Ref_Layer%02d_FrontName"), iLayer + 1);
			if (ReadStringINI(strSection, strKey, fileName, strTmp) == RESULT_GOOD)
			{
				while (strTmp.GetLength() > 0)
				{
					AfxExtractSubString(str, strTmp, 0, ',');
					pLayer->vcFrontLayerNames.emplace_back(str);
					strTmp.Delete(0, str.GetLength() + 1);
				}
			}

			strKey.Format(_T("Ref_Layer%02d_BackName"), iLayer + 1);
			if (ReadStringINI(strSection, strKey, fileName, strTmp) == RESULT_GOOD)
			{
				while (strTmp.GetLength() > 0)
				{
					AfxExtractSubString(str, strTmp, 0, ',');
					pLayer->vcBackLayerNames.emplace_back(str);
					strTmp.Delete(0, str.GetLength() + 1);
				}
			}
			m_stSysSpec.sysLayer.vcLayer.emplace_back(pLayer);
		}
	}
	else
		iErrCnt++;

	if (iErrCnt) return RESULT_BAD;
	else return RESULT_GOOD;
}

UINT32 CSpec::SaveLayerSpec(CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	CString strSection = _T("LAYER");
	UINT32 rtn = 0, iErrCnt = 0;

	rtn = WriteScalePRM(strSection, _T("Ref_Layer_Num"), static_cast<UINT32>(m_stSysSpec.sysLayer.vcLayer.size()), fileName, _T("Reference Num"));
	if (rtn == RESULT_BAD) iErrCnt++;

	UINT32 iLayer = 0;
	CString strKey = _T(""), str = L"";
	for (auto it : m_stSysSpec.sysLayer.vcLayer)
	{
		strKey.Format(_T("Ref_Layer%02d_Define"), iLayer + 1);
		rtn = WriteScalePRM(strSection, strKey, it->strDefine, fileName, _T("Layer Define"));
		if (rtn == RESULT_BAD) iErrCnt++;

		strKey.Format(_T("Ref_Layer%02d_Color"), iLayer + 1);
		rtn = WriteScalePRM(strSection, strKey, static_cast<UINT32>(it->layerColor), fileName, _T("Layer Color"));
		if (rtn == RESULT_BAD) iErrCnt++;

		str = L"";
		for (auto itFront : it->vcFrontLayerNames)
		{
			str += itFront;
			str += L",";
		}

		strKey.Format(_T("Ref_Layer%02d_FrontName"), iLayer + 1);
		rtn = WriteScalePRM(strSection, strKey, str, fileName, _T("Front Layer Name"));
		if (rtn == RESULT_BAD) iErrCnt++;

		str = L"";
		for (auto itBack : it->vcBackLayerNames)
		{
			str += itBack;
			str += L",";
		}

		strKey.Format(_T("Ref_Layer%02d_BackName"), iLayer + 1);
		rtn = WriteScalePRM(strSection, strKey, str, fileName, _T("Back Layer Name"));
		if (rtn == RESULT_BAD) iErrCnt++;

		iLayer++;
	}

	if (iErrCnt) return RESULT_BAD;
	else return RESULT_GOOD;
}

UINT32 CSpec::LoadCameraOrientSpec(CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	TCHAR	 strSection[MAX_SECTION_TXT_SIZE] = _T("");
	_stprintf_s(strSection, MAX_SECTION_TXT_SIZE, _T("CAMERA_ORIENT"));

	UINT32 iErrCnt = 0, iCameraOrient = 0;

	if (ReadStringINI(strSection, _T("Camera_Orient_Top"), fileName, iCameraOrient) != RESULT_GOOD)
		iErrCnt++;

	m_stSysSpec.sysNSIS.sysCameraOrient_Top = iCameraOrient / 90;	

	if (ReadStringINI(strSection, _T("Camera_Orient_Bot"), fileName, iCameraOrient) != RESULT_GOOD)
		iErrCnt++;

	m_stSysSpec.sysNSIS.sysCameraOrient_Bot = iCameraOrient / 90;	

	if (iErrCnt) return RESULT_BAD;
	else return RESULT_GOOD;
}

UINT32 CSpec::SaveCameraOrientSpec(CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	CString strSection = _T("CAMERA_ORIENT");
	UINT32 rtn = 0, iErrCnt = 0, Orient;

	Orient = m_stSysSpec.sysNSIS.sysCameraOrient_Top * 90;
	rtn = WriteScalePRM(strSection, _T("Camera_Orient_Top"), Orient, fileName, _T("Camera_Orient_Top"));
	if (rtn == RESULT_BAD) iErrCnt++;

	Orient = m_stSysSpec.sysNSIS.sysCameraOrient_Bot * 90;
	rtn = WriteScalePRM(strSection, _T("Camera_Orient_Bot"), Orient, fileName, _T("Camera_Orient_Bot"));
	if (rtn == RESULT_BAD) iErrCnt++;

	if (iErrCnt) return RESULT_BAD;
	else return RESULT_GOOD;
}

UINT32 CSpec::LoadAlignRatioSpec(CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	TCHAR	 strSection[MAX_SECTION_TXT_SIZE] = _T("");
	_stprintf_s(strSection, MAX_SECTION_TXT_SIZE, _T("RATIO"));

	UINT32 iErrCnt = 0, Ratio = 0;
	double dValue = 0.f;

	if (ReadStringINI(strSection, _T("Align"), fileName, Ratio) != RESULT_GOOD)
		iErrCnt++;

	m_stSysSpec.sysNSIS.sysAlignRatio = Ratio;	

	if (ReadStringINI(strSection, _T("UnitAlign"), fileName, Ratio) != RESULT_GOOD)
		iErrCnt++;

	m_stSysSpec.sysNSIS.sysUnitAlignRatio = Ratio;

	if (ReadStringINI(strSection, _T("ZoomMargin"), fileName, dValue) != RESULT_GOOD)
		iErrCnt++;

	m_stSysSpec.sysNSIS.sysZoomMarginRatio = dValue;

	if (iErrCnt) return RESULT_BAD;
	else return RESULT_GOOD;
}

UINT32 CSpec::SaveAlignRatioSpec(CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	CString strSection = _T("RATIO");
	UINT32 rtn = 0, iErrCnt = 0, Ratio;
	double dValue;

	Ratio = m_stSysSpec.sysNSIS.sysAlignRatio;
	rtn = WriteScalePRM(strSection, _T("Align"), Ratio, fileName, _T("Align"));
	if (rtn == RESULT_BAD) iErrCnt++;

	Ratio = m_stSysSpec.sysNSIS.sysUnitAlignRatio;
	rtn = WriteScalePRM(strSection, _T("UnitAlign"), Ratio, fileName, _T("UnitAlign"));
	if (rtn == RESULT_BAD) iErrCnt++;

	dValue = m_stSysSpec.sysNSIS.sysZoomMarginRatio;
	rtn = WriteScalePRM(strSection, _T("ZoomMargin"), dValue, fileName, _T("ZoomMargin"));
	if (rtn == RESULT_BAD) iErrCnt++;

	if (iErrCnt) return RESULT_BAD;
	else return RESULT_GOOD;
}

UINT32 CSpec::LoadStaticOpticSpec(CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	TCHAR	 strSection[MAX_SECTION_TXT_SIZE] = _T("");
	_stprintf_s(strSection, MAX_SECTION_TXT_SIZE, _T("STATIC_OPTIC"));

	UINT32 iErrCnt = 0;
	double Optic = 0.;

	if (ReadStringINI(strSection, _T("Align_Lens"), fileName, Optic) != RESULT_GOOD)
		iErrCnt++;

	m_stSysSpec.sysNSIS.sysAlignLens = Optic;	

	if (ReadStringINI(strSection, _T("Align_Zoom"), fileName, Optic) != RESULT_GOOD)
		iErrCnt++;

	m_stSysSpec.sysNSIS.sysAlignZoom = Optic;
	
	if (ReadStringINI(strSection, _T("UnitAlign_Lens"), fileName, Optic) != RESULT_GOOD)
		iErrCnt++;

	m_stSysSpec.sysNSIS.sysUnitAlignLens = Optic;	

	if (ReadStringINI(strSection, _T("UnitAlign_Zoom"), fileName, Optic) != RESULT_GOOD)
		iErrCnt++;

	m_stSysSpec.sysNSIS.sysUnitAlignZoom = Optic;

	if (iErrCnt) return RESULT_BAD;
	else return RESULT_GOOD;
}

UINT32 CSpec::SaveStaticOpticSpec(CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	CString strSection = _T("STATIC_OPTIC");
	UINT32 rtn = 0, iErrCnt = 0;
	double Optic = 0.;

	Optic = m_stSysSpec.sysNSIS.sysAlignLens;
	rtn = WriteScalePRM(strSection, _T("Align_Lens"), Optic, fileName, _T("Align_Lens"));
	if (rtn == RESULT_BAD) iErrCnt++;

	Optic = m_stSysSpec.sysNSIS.sysAlignZoom;
	rtn = WriteScalePRM(strSection, _T("Align_Zoom"), Optic, fileName, _T("Align_Zoom"));
	if (rtn == RESULT_BAD) iErrCnt++;

	Optic = m_stSysSpec.sysNSIS.sysUnitAlignLens;
	rtn = WriteScalePRM(strSection, _T("UnitAlign_Lens"), Optic, fileName, _T("UnitAlign_Lens"));
	if (rtn == RESULT_BAD) iErrCnt++;

	Optic = m_stSysSpec.sysNSIS.sysUnitAlignZoom;
	rtn = WriteScalePRM(strSection, _T("UnitAlign_Zoom"), Optic, fileName, _T("UnitAlign_Zoom"));
	if (rtn == RESULT_BAD) iErrCnt++;

	if (iErrCnt) return RESULT_BAD;
	else return RESULT_GOOD;
}

UINT32 CSpec::LoadNetworkSpec(CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	TCHAR	 strSection[MAX_SECTION_TXT_SIZE] = _T("");
	_stprintf_s(strSection, MAX_SECTION_TXT_SIZE, _T("NETWORK"));

	UINT32 iErrCnt = 0, Port = 0;
	CString strIP;
	if (ReadStringINI(strSection, _T("NSIS_Port"), fileName, Port) != RESULT_GOOD)
		iErrCnt++;

	m_stSysSpec.sysNSIS.sysNSISPort = Port;	

	if (ReadStringINI(strSection, _T("Cam_Port"), fileName, Port) != RESULT_GOOD)
		iErrCnt++;
	
	m_stSysSpec.sysNSIS.sysCamPort = Port;
	
	if (ReadStringINI(strSection, _T("NSIS_IP"), fileName, strIP) != RESULT_GOOD)
		iErrCnt++;

	m_stSysSpec.sysNSIS.sysNSISIP = strIP;
	
	if (ReadStringINI(strSection, _T("CAM_IP"), fileName, strIP) != RESULT_GOOD)
		iErrCnt++;

	m_stSysSpec.sysNSIS.sysCamIP = strIP;
	
	if (iErrCnt) return RESULT_BAD;
	else return RESULT_GOOD;
}

UINT32 CSpec::SaveNetworkSpec(CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	CString strSection = _T("NETWORK");
	UINT32 rtn = 0, iErrCnt = 0, Port;
	CString strIP;

	Port = m_stSysSpec.sysNSIS.sysNSISPort;
	rtn = WriteScalePRM(strSection, _T("NSIS_Port"), Port, fileName, _T("NSIS_Port"));
	if (rtn == RESULT_BAD) iErrCnt++;

	Port = m_stSysSpec.sysNSIS.sysCamPort;
	rtn = WriteScalePRM(strSection, _T("Cam_Port"), Port, fileName, _T("Cam_Port"));
	if (rtn == RESULT_BAD) iErrCnt++;

	strIP = m_stSysSpec.sysNSIS.sysNSISIP;
	rtn = WriteScalePRM(strSection, _T("NSIS_IP"), strIP, fileName, _T("NSIS_IP"));
	if (rtn == RESULT_BAD) iErrCnt++;

	strIP = m_stSysSpec.sysNSIS.sysCamIP;
	rtn = WriteScalePRM(strSection, _T("CAM_IP"), strIP, fileName, _T("CAM_IP"));
	if (rtn == RESULT_BAD) iErrCnt++;

	if (iErrCnt) return RESULT_BAD;
	else return RESULT_GOOD;
}

UINT32 CSpec::ReadStringINI(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpszFileName, UINT32& iOutput)
{
	TCHAR lpstrRtn[MAX_LOAD_TXT_SIZE] = _T("");
	DWORD nSize = MAX_LOAD_TXT_SIZE;

	DWORD dwAck = GetPrivateProfileString(lpszSection, lpszKey, _T(""), lpstrRtn, nSize, lpszFileName);
	if (dwAck == -1) 
		return RESULT_BAD;

	int pos = 0;
	int length = (int)_tcslen(lpstrRtn);
	if (length == 0) 
		return RESULT_BAD;

	for (pos = 0; pos < length; pos++)
		if (lpstrRtn[pos] == ';') { lpstrRtn[pos] = 0; break; }
	
	iOutput = _ttoi(lpstrRtn);
	return RESULT_GOOD;
}

UINT32 CSpec::ReadStringINI(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpszFileName, double& dOutput)
{
	TCHAR lpstrRtn[MAX_LOAD_TXT_SIZE] = _T("");
	DWORD nSize = MAX_LOAD_TXT_SIZE;

	DWORD dwAck = GetPrivateProfileString(lpszSection, lpszKey, _T(""), lpstrRtn, nSize, lpszFileName);
	if (dwAck == -1)
		return RESULT_BAD;

	int pos = 0;
	int length = (int)_tcslen(lpstrRtn);
	if (length == 0)
		return RESULT_BAD;

	for (pos = 0; pos < length; pos++)
		if (lpstrRtn[pos] == ';') { lpstrRtn[pos] = 0; break; }

	dOutput = _ttof(lpstrRtn);
	return RESULT_GOOD;
}

#define BUFFERSIZE 1024
UINT32 CSpec::ReadStringINI(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpszFileName, CString& strOutput)
{
	strOutput = L"";

	TCHAR lpstrRtn[MAX_LOAD_TXT_SIZE] = _T("");
	DWORD nSize = MAX_LOAD_TXT_SIZE;

	

	CString str;
	// Section의 세션의 키는 key의 데이터를 가져온다. 키가 없을 경우에는 디폴트로 ""를 취득한다.  
	DWORD dwAck = GetPrivateProfileString(lpszSection, lpszKey, _T(""), lpstrRtn, nSize, lpszFileName);

	if (dwAck == -1)
		return RESULT_BAD;

	int pos = 0;
	int length = (int)_tcslen(lpstrRtn);
	if (length == 0)
		return RESULT_BAD;

	for (pos = 0; pos < length; pos++)
		if (lpstrRtn[pos] == ';') { lpstrRtn[pos] = 0; break; }

	strOutput.Format(L"%s", lpstrRtn);
	return RESULT_GOOD;
}

//YJD START 22.06.13 - SAVE MEASURE IMAGE
UINT32 CSpec::ReadStringINI(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR lpszFileName, bool& dOutput)
{
	TCHAR lpstrRtn[MAX_LOAD_TXT_SIZE] = _T("");
	DWORD nSize = MAX_LOAD_TXT_SIZE;

	DWORD dwAck = GetPrivateProfileString(lpszSection, lpszKey, _T(""), lpstrRtn, nSize, lpszFileName);
	if (dwAck == -1)
		return RESULT_BAD;

	int pos = 0;
	int length = (int)_tcslen(lpstrRtn);
	if (length == 0)
		return RESULT_BAD;

	for (pos = 0; pos < length; pos++)
		if (lpstrRtn[pos] == ';') { lpstrRtn[pos] = 0; break; }

	if (_ttoi(lpstrRtn) == 1)
	{
		dOutput = true;
	}
	else
	{
		dOutput = false;
	}
	return RESULT_GOOD;
}
//YJD END 22.06.13 - SAVE MEASURE IMAGE


UINT32 CSpec::WriteScalePRM(LPCTSTR lpszSection, LPCTSTR lpszKey, UINT32 iOutput, LPCTSTR lpszFileName, LPCTSTR lpszComment)
{
	CString strVal;
	strVal.Format(_T("%d"), iOutput);

	if (WritePrivateProfileString(lpszSection, lpszKey, strVal, lpszFileName) != 0)
		return RESULT_GOOD;
	else 
		return RESULT_BAD;
}

//YJD START 22.06.13 - SAVE MEASURE IMAGE
UINT32 CSpec::WriteScalePRM(LPCTSTR lpszSection, LPCTSTR lpszKey, bool iOutput, LPCTSTR lpszFileName, LPCTSTR lpszComment)
{
	CString strVal;
	if (iOutput == true)
	{
		strVal.Format(_T("1"));
	}
	else
	{
		strVal.Format(_T("0"));
	}

	if (WritePrivateProfileString(lpszSection, lpszKey, strVal, lpszFileName) != 0)
		return RESULT_GOOD;
	else
		return RESULT_BAD;
}
//YJD END 22.06.13 - SAVE MEASURE IMAGE

UINT32 CSpec::WriteScalePRM(LPCTSTR lpszSection, LPCTSTR lpszKey, double dOutput, LPCTSTR lpszFileName, LPCTSTR lpszComment)
{
	CString strVal;
	strVal.Format(_T("%f"), dOutput);

	if (WritePrivateProfileString(lpszSection, lpszKey, strVal, lpszFileName) != 0)
		return RESULT_GOOD;
	else
		return RESULT_BAD;
}

UINT32 CSpec::WriteScalePRM(LPCTSTR lpszSection, LPCTSTR lpszKey, LPCTSTR strOutput, LPCTSTR lpszFileName, LPCTSTR lpszComment)
{
	if (WritePrivateProfileString(lpszSection, lpszKey, strOutput, lpszFileName) != 0)
		return RESULT_GOOD;
	else
		return RESULT_BAD;
}

//YJD START 2022.04.08 - MEASURE TYPE SCALE
UINT32 CSpec::LoadMeasureScaleSpec(CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	CString TempString = _T("");
	TCHAR	 strSection[MAX_SECTION_TXT_SIZE] = _T("");
	_stprintf_s(strSection, MAX_SECTION_TXT_SIZE, _T("MEASURE_SCALE"));

	UINT32 iErrCnt = 0;
	double dDefaultRatio = 2.0;

	if (ReadStringINI(strSection, _T("DEFAULT_RATIO"), fileName, dDefaultRatio) != RESULT_GOOD)
		dDefaultRatio = 2.0;
	m_stSysSpec.sysNSIS.sysMeasureDefaultScale = dDefaultRatio;

	for (int i = 0; i < (int)MeasureType::MeasureTypeNone; i++)
	{
		m_stSysSpec.sysNSIS.sysMeasureTypeScale[i] = m_stSysSpec.sysNSIS.sysMeasureDefaultScale;
	}

	for (int i = 0; i < (int)MeasureType::MeasureTypeNone; i++)
	{
		double Scale = 2.0;

		TempString = GetMeasureTypeString((MeasureType)i);

		if (ReadStringINI(strSection, TempString, fileName, Scale) != RESULT_GOOD)
			Scale = m_stSysSpec.sysNSIS.sysMeasureDefaultScale;

		m_stSysSpec.sysNSIS.sysMeasureTypeScale[i] = Scale;
	}

	if (iErrCnt) return RESULT_BAD;
	else return RESULT_GOOD;
}

UINT32 CSpec::SaveMeasureScaleSpec(CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	CString strSection = _T("MEASURE_SCALE");
	UINT32 rtn = 0, iErrCnt = 0;

	//YJD START 2022.04.08 - MEASURE TYPE SCALE
	CString TempString = _T("");
	rtn = WriteScalePRM(strSection, _T("DEFAULT_RATIO"), m_stSysSpec.sysNSIS.sysMeasureDefaultScale, fileName, TempString);

	for (int i = 0; i < (int)MeasureType::MeasureTypeNone; i++)
	{
		double Scale = m_stSysSpec.sysNSIS.sysMeasureTypeScale[i];

		TempString = GetMeasureTypeString((MeasureType)i);

		rtn = WriteScalePRM(strSection, TempString, Scale, fileName, TempString);
	}
	//YJD END 2022.04.08 - MEASURE TYPE SCALE

	if (iErrCnt) return RESULT_BAD;
	else return RESULT_GOOD;
}

//YJD START 22.06.13 - SAVE MEASURE IMAGE
UINT32 CSpec::LoadMeasurePointSpec(CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	CString TempString = _T("");
	TCHAR	 strSection[MAX_SECTION_TXT_SIZE] = _T("");
	_stprintf_s(strSection, MAX_SECTION_TXT_SIZE, _T("MEASURE_POINT"));

	double dSaveMeasurePointScale = 2.0;
	bool dUseSaveMeasurePointImage = false;

	// 2022.11.02 KJH ADD
	// Unit Layer Image Save Parameter
	bool dUseSaveUnitLayerImage = false;

	if (ReadStringINI(strSection, _T("USE_SAVE_MEASURE_POINT"), fileName, dUseSaveMeasurePointImage) != RESULT_GOOD)
		dUseSaveMeasurePointImage = false;

	if (ReadStringINI(strSection, _T("SAVE_MEASURE_POINT_SCALE"), fileName, dSaveMeasurePointScale) != RESULT_GOOD)
		dSaveMeasurePointScale = 0.3;

	// 2022.11.02 KJH ADD
	// Unit Layer Image Save Parameter
	if (ReadStringINI(strSection, _T("USE_SAVE_UNIT_LAYER"), fileName, dUseSaveUnitLayerImage) != RESULT_GOOD)
		dUseSaveUnitLayerImage = false;

	m_stSysSpec.sysNSIS.sysUseSaveMeasurePoint = dUseSaveMeasurePointImage;
	m_stSysSpec.sysNSIS.sysSaveMeasureScale = dSaveMeasurePointScale;

	// 2022.11.02 KJH ADD
	// Unit Layer Image Save Parameter
	m_stSysSpec.sysNSIS.sysUseSaveUnitLayer = dUseSaveUnitLayerImage;

	return RESULT_GOOD;
}

UINT32 CSpec::SaveMeasurePointSpec(CString fileName)
{
	if (fileName.GetLength() < 1)
		return RESULT_BAD;

	CString strSection = _T("MEASURE_POINT");
	UINT32 rtn = 0, iErrCnt = 0;

	rtn = WriteScalePRM(strSection, _T("USE_SAVE_MEASURE_POINT"), m_stSysSpec.sysNSIS.sysUseSaveMeasurePoint, fileName, _T("Use Save Measure Point Image"));

	rtn = WriteScalePRM(strSection, _T("SAVE_MEASURE_POINT_SCALE"), m_stSysSpec.sysNSIS.sysSaveMeasureScale, fileName, _T("Save Measure Point Image Scale"));

	// 2022.11.02 KJH ADD
	// Unit Layer Image Save Parameter
	rtn = WriteScalePRM(strSection, _T("USE_SAVE_UNIT_LAYER"), m_stSysSpec.sysNSIS.sysUseSaveUnitLayer, fileName, _T("Use Save Unit Layer Image"));

	if (iErrCnt) return RESULT_BAD;
	else return RESULT_GOOD;
}
//YJD END 22.06.13 - SAVE MEASURE IMAG


CString CSpec::GetMeasureTypeString(MeasureType type)
{
	if (type == MeasureType::align)
		return PANEL_ALIGN_TXT;
	else if (type == MeasureType::align_plating)
		return PANEL_ALIGN_PLATING_TXT;
	else if (type == MeasureType::fm_p)
		return UNIT_ALIGN_TXT_P;
	else
	{
		if (type == MeasureType::t_width)		return _T("T_WIDTH");
		else if (type == MeasureType::t_space)		return _T("T_SPACE");
		else if (type == MeasureType::bf_width)		return _T("BF_WIDTH");
		else if (type == MeasureType::bf_space)		return _T("BF_SPACE");
		else if (type == MeasureType::bol_width)	return _T("BOL_WIDTH");
		else if (type == MeasureType::bol_space)	return _T("BOL_SPACE");
		else if (type == MeasureType::thickness)	return _T("THICKNESS");
		else if (type == MeasureType::dimple)		return _T("DIMPLE");
		else if (type == MeasureType::ball_width)	return _T("BALL_WIDTH");
		else if (type == MeasureType::bump_width)	return _T("BUMP_WIDTH");
		else if (type == MeasureType::imp_space_A)	return _T("IMP_SPACE_A");
		else if (type == MeasureType::imp_space_B)	return _T("IMP_SPACE_B");
		else if (type == MeasureType::imp_width_A)	return _T("IMP_WIDTH_A");
		else if (type == MeasureType::imp_width_B)	return _T("IMP_WIDTH_B");
		else if (type == MeasureType::bol_width_x)	return _T("BOL_WIDTH_X");
		else if (type == MeasureType::bol_width_y)	return _T("BOL_WIDTH_Y");
		else if (type == MeasureType::bol_space_x)	return _T("BOL_SPACE_X");
		else if (type == MeasureType::bol_space_y)	return _T("BOL_SPACE_Y");
		else if (type == MeasureType::bol_pitch)	return _T("BOL_PITCH");
		else if (type == MeasureType::via_width)	return _T("VIA_WIDTH");
		else if (type == MeasureType::r_depth)		return _T("R_DEPTH");
		else if (type == MeasureType::pad)			return _T("PAD");
		else if (type == MeasureType::ball_pitch)		return _T("BALL_PITCH");
		else if (type == MeasureType::thickness_cu)		return _T("THICKNESS_CU");
		else if (type == MeasureType::thickness_pad)	return _T("THICKNESS_PAD");
		else
			return _T("UKNOWN");
	}
}
//YJD END 2022.04.08 - MEASURE TYPE SCALE
