#include "pch.h"
#include "iMasterCommon.h"
#include "SystemSpec.h"
#include "MainFrm.h"
#include "iMasterDoc.h"
#include "iMasterView.h"
#include "../iDxDraw/iDxDraw.h"
#include "../iProcess/iProcess.h"

#include "../iUtilities/PerfTimerEx.h"
#include "FeatureEditFormView.h"


// Main
CMainFrame*		g_pMainFrame;
CiMasterView*	g_pMainView;



// JobFile
CJobFile*		g_pJob;

// DLL
Version			g_ArchiveDLLVer;
Version			g_DrawDLLVer;

// Archive
bool			g_bArchiveInit;
bool			g_bFinishLoadCamThread;

//
CDrawManager*	g_pDrawManager = nullptr;
CDrawMinimap*	g_pDrawMinimap = nullptr;
CDrawEdit_Temp* g_pDrawEdit_Temp = nullptr;


CFeatureEditFormView* g_pFeatureEditFormView = nullptr;


//
//AddMode
int				g_nSelectIndex = -1;
//

// Draw
bool			g_bDrawInit;
bool			g_bFinishMakeMasterThread;

// Spec
bool			g_bSpecInit;
CSpec			g_cSpec;

// Data
CDataManager*	g_pDataManager = nullptr;


// UINT32	_SetShape(IN const double&dRadius, IN const double &dWidth, IN const double &dHeight)
// {
// 	/*g_pDrawManager-> */
// 	return RESULT_GOOD;
// }




FeatureSpec_t GetFeatureData()
{
	return g_pDrawManager->FeatureSpec;
}

UINT32 SetFeatureData()
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;
	return g_pDrawManager->SetFeatureData();
}

void SetLayerInfoView()
{
	g_pMainFrame->ShowHideLayerInfo();
}

void SetEdit_Feature()
{
	g_pMainFrame->ShowHideFeatureEdit();
}

void SetEdit_Apply()
{

	if (g_pDrawManager->GetEditMode() == EditMode::enumMode_EditMask || g_pDrawManager->GetEditMode() == EditMode::enumMode_EditMask_Add_FeatureBase) return;

	BOOL bSelect = IsSelect();
	BOOL bResized = IsResize();
	if (g_pDrawManager->m_bResizeMode == TRUE || bResized ==TRUE)
	{
		if (AfxMessageBox(_T("적용 하시겠습니까?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			if (bResized == TRUE )
			{
				g_pDrawManager->SetTempData_Apply();

			}

			g_pDrawManager->SetResize_Apply_SelectOnly(g_pDrawManager->m_dResizeRatio);
			g_pDrawManager->SetMove_Apply();
			Log_FeatureSelect();
			AddApplicationLog(APPLY_EDIT_RESIZE_TXT);
			g_pDrawManager->m_bResizeMode = FALSE;
			SetViewerEditMode(EditMode::enumMode_EditMode);
			Reset();
		}
		else
		{
			SetViewerEditMode(EditMode::enumMode_EditMode);
			g_pDrawManager->m_bResizeMode = FALSE;
			Reset();
		}
	}
	else if ( g_pDrawManager->IsMove()== TRUE )
	{
		if (AfxMessageBox(_T("적용 하시겠습니까?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			//g_pDrawManager->SetResize_Apply_SelectOnly(g_pDrawManager->m_dResizeRatio);
			g_pDrawManager->SetMove_Apply();
			Log_FeatureSelect();
			AddApplicationLog(APPLY_EDIT_MOVE_TXT);
			SetViewerEditMode(EditMode::enumMode_EditMode);
			g_pDrawManager->FeatureSpec.bDragMode = FALSE;
			Reset();
		}
		else
		{
			SetViewerEditMode(EditMode::enumMode_EditMode);
			g_pDrawManager->FeatureSpec.dMoveX = 0.0;
			g_pDrawManager->FeatureSpec.dMoveY = 0.0;			
			Reset();
		}
	}

	// 2022.06.07 
	// 김준호 주석추가
	// Edit 후 measureinfo update 위해 추가 일단 주석처리
	if (GetMachineType() == eMachineType::eNSIS)
	{
		g_pDataManager->GetMeasureData(g_pMainFrame->GetMeasureData(), FALSE, TRUE);
		g_pMainFrame->UpdataMeasureInfoView();
	}

}

UINT32 Reset()
{
	return g_pDrawManager->Reset();

}

UINT32 Initialize_SetMainFrm(void* pMainFrm)
{
	if (pMainFrm == NULL) return RESULT_BAD;
	g_pMainFrame = (CMainFrame*)pMainFrm;
	return RESULT_GOOD;
}

UINT32 Initialize_SetMainView(void *pMainView)
{
	if (pMainView == NULL) return RESULT_BAD;
	g_pMainView = (CiMasterView*)pMainView;
	return RESULT_GOOD;
}

UINT32 Initialize_Spec()
{
	return g_cSpec.LoadSystemSpec(GetCurAppPath());
}

SystemSpec_t* GetSystemSpec()
{
	return g_cSpec.GetSystemSpec();
}

FeatureSpec_t* GetFeatureSpec()
{
	return &g_pDrawManager->FeatureSpec;
}

UINT32 SaveSystemSpec()
{
	
	return g_cSpec.SaveSystemSpec(GetCurAppPath());
}

UINT32 Initialize_iArchive()
{
	CString str;

	g_bArchiveInit = false;
	g_bFinishLoadCamThread = true;

	UINT32 nRst = IArchive_Create(g_ArchiveDLLVer);
	if (nRst == RESULT_GOOD)
	{
		str.Format(_T("SmartArchive ver - Major:%d, Minor:%d, Release:%d, Build:%d"),
			g_ArchiveDLLVer.Major, g_ArchiveDLLVer.Minor, g_ArchiveDLLVer.Release, g_ArchiveDLLVer.Build);

		if (g_pMainFrame)
			g_pMainFrame->AddApplicationLog(str);

		nRst |= IArchive_Initiate();

		nRst |= IArchive_SetSystemSpec(GetSystemSpec());

		if (nRst == RESULT_GOOD)
			g_bArchiveInit = true;
	}
	return nRst;
}

UINT32			Initialize_DxDraw(ViewInfo_t *pViewInfo)
{
	if (pViewInfo == nullptr) return RESULT_BAD;
	if (g_pDataManager == nullptr) return RESULT_BAD;

	g_bFinishMakeMasterThread = true;
	g_bDrawInit = false;

	if (g_pDrawManager == nullptr)
	{
		g_pDrawManager = new CDrawManager();
	}

	g_pJob = nullptr;
	if (RESULT_GOOD != IArchive_GetJobFile((void**)&g_pJob))
		return RESULT_BAD;

	if (RESULT_GOOD != g_pDataManager->Initialize(g_pJob, g_pDrawManager))
		return RESULT_BAD;

	UINT32 nRst = g_pDrawManager->SetSystemSpec(GetSystemSpec());
	nRst |= g_pDrawManager->Initialize(g_pJob, pViewInfo, g_pDataManager->GetProductInfo(),
		g_pDataManager->GetFovInfo(), g_pDataManager->GetCellInfo());

	if (nRst == RESULT_GOOD)
	{
		g_bDrawInit = true;
		g_bFinishMakeMasterThread = true;

		ID2D1Factory1 * pFactory = g_pDrawManager->GetFactory();
		ID2D1DeviceContext* pContext = g_pDrawManager->GetDeviceContext();

	}

	if (g_pDrawMinimap != nullptr)
	{
		g_pDrawMinimap->SetLink(g_pDrawManager);
	}

	return nRst;

}

UINT32			Initialize_Minimap(IN const ViewInfo_t &stViewInfo)
{
	if (g_pDrawMinimap == nullptr)
	{
		g_pDrawMinimap = new CDrawMinimap();
	}

	if (g_pJob == nullptr)
	{
		IArchive_GetJobFile((void**)&g_pJob);
	}

	if (g_pJob == nullptr) return RESULT_BAD;

	g_pDrawMinimap->Initial(g_pJob, stViewInfo, g_pDrawManager);
	
	return RESULT_GOOD;
}

UINT32			Initialize_iProcess()
{
	SystemSpec_t* pSysSpec = GetSystemSpec();
	if (pSysSpec == nullptr)
		return RESULT_BAD;

	if (g_pDataManager == nullptr)
	{
		if (pSysSpec->sysBasic.McType == eMachineType::eNSIS)
			g_pDataManager = new CNSIS();
		else if (pSysSpec->sysBasic.McType == eMachineType::eAOI)
			g_pDataManager = new CAOI();
		else if (pSysSpec->sysBasic.McType == eMachineType::eAVI)
			g_pDataManager = new CAVI();
		else
			return RESULT_BAD;
	}

	UINT32 nRst = g_pDataManager->SetSystemSpec(GetSystemSpec());
	nRst |= g_pDataManager->SetUserSetInfo(g_pMainFrame->GetUserSetInfo(), g_pMainFrame->GetUserLayerSet());
	//	nRst |= g_pDataManager->Initialize(g_pJob, g_pDrawManager);

	if (pSysSpec->sysBasic.McType == eMachineType::eNSIS)
	{
		if (g_pMainFrame)
			g_pMainFrame->ShowHideMeasureInfo(TRUE);
	}

	return nRst;
}

CJobFile*		GetJobPtr()
{
	return g_pJob;
}

/*
UINT32 Initialize_iDraw(ViewInfo_t* pViewInfo, void* pDXpicBox)
{
	ASSERT(pViewInfo);

	g_bDrawInit = false;
	g_bFinishMakeMasterThread = true;

	g_pJobFile = nullptr;
	if (RESULT_GOOD != IArchive_GetJobFile((void**)&g_pJobFile))
		return RESULT_BAD;

	UINT32 nRst = IDraw_Create(g_DrawDLLVer);
	if (nRst == RESULT_GOOD)
	{
		CString str;
		str.Format(_T("SmartDraw ver - Major:%d, Minor:%d, Release:%d, Build:%d"), g_DrawDLLVer.Major, g_DrawDLLVer.Minor, g_DrawDLLVer.Release, g_DrawDLLVer.Build);
		AddApplicationLog(str);

		nRst |= IDraw_SetSystemSpec(GetSystemSpec());
		nRst |= IDraw_Initiate(g_pJobFile, pViewInfo, pDXpicBox);
		if (nRst == RESULT_GOOD)
			g_bDrawInit = true;
	}
	return nRst;
}

*/

void AddApplicationLog(CString strLog, LogMsgType enMsgType)
{
	if (g_pMainFrame)
		g_pMainFrame->AddApplicationLog(strLog, enMsgType);
}

UINT32 Destroy_iArchive()
{
	if (g_bArchiveInit)
	{
		while (!g_bFinishLoadCamThread) {}
		Sleep(WAIT_BEFORE_DESTORY);
	}

	return IArchive_Destory();
}

UINT32 Destroy_iDraw()
{
	if (g_bDrawInit)
	{
		while (!g_bFinishMakeMasterThread) {}
		Sleep(WAIT_BEFORE_DESTORY);
	}

	if (g_pDrawManager != nullptr)
	{
		delete g_pDrawManager;
		g_pDrawManager = nullptr;
	}

	if (g_pDrawMinimap != nullptr)
	{
		delete g_pDrawMinimap;
		g_pDrawMinimap = nullptr;
	}

	return RESULT_GOOD;
}

UINT32 Destroy_iProcess()
{
	if (g_pDataManager == nullptr) return RESULT_BAD;

	return g_pDataManager->Destory();
}

UINT32 ExtractCamData(CString fileName)
{
	CPerfTimerEx timerEx;
	timerEx.StartTimer();

	if (g_pMainFrame == nullptr)
		return RESULT_BAD;

	

	g_pDrawManager->ClearDisplay();
	g_pDataManager->ClearData();

	g_pMainFrame->AdjustDockPanes();
	g_pMainFrame->ShowPopupInitialize(L"Extracting Cam Data...");

	//1. Extract CAM 
	g_bFinishLoadCamThread = false;

	BOOL bCreateAlways = FALSE;

	SetViewerEditMode(EditMode::enumMode_None);

	g_pMainFrame->IsDoneExtractCam(FALSE);
	g_pMainFrame->IsDoneLoadLayers(FALSE);
	g_pMainFrame->IsDoneProcessMaster(FALSE);
	g_pMainFrame->IsAllLayerLoaded(FALSE);

	g_pMainFrame->ChangeSide(eProdSide::eTop);

	g_pMainFrame->AddApplicationLog(fileName);
	g_pMainFrame->AddApplicationLog(EXTRACT_CAM_TXT);

	UINT32 iRst = IArchive_Extract(bCreateAlways, fileName);

	if (iRst == RESULT_GOOD)
	{
		g_pMainFrame->HidePopupInitialize();
		g_pMainFrame->AddApplicationLog(SUCCEED_EXTRACT_CAM_TXT);

		iRst = LoadCamData();
		if (iRst == RESULT_GOOD)
		{
			g_pMainFrame->IsDoneExtractCam(TRUE);

			CString strModel = L"";
			if (RESULT_GOOD == IArchive_GetModelName(strModel))
				g_pMainFrame->SetWindowText(strModel);

			g_pMainFrame->UpdateStatusBar(MODEL_STATUS_POS, strModel);
			g_pMainFrame->AddApplicationLog(L"Model : " + strModel);
			g_pMainFrame->AddApplicationLog(SUCCEED_LOAD_CAM_TXT);

			iRst = SetCamInfo();
			if (iRst == RESULT_GOOD)
			{
				if (GetMachineType() == eMachineType::eNSIS)
				{
					g_pMainFrame->CheckLayerInfo();

					/*if (GetUserSite() == eUserSite::eSEM)
					{
						g_pMainFrame->m_strStartLayer = L"-1";
						g_pMainFrame->m_strEndLayer = L"-1";
					}*/
				}

				g_pMainFrame->AddApplicationLog(SUCCEED_SET_CAM_TXT);
				iRst = SetLayerInfo();
				if (iRst == RESULT_GOOD)
				{
					g_pMainFrame->AddApplicationLog(SUCCEED_SET_LAYER_INFO);
				}
				else
					g_pMainFrame->AddApplicationLog(FAILED_SET_LAYER_INFO, LogMsgType::EN_ERROR);
			}
			else
				g_pMainFrame->AddApplicationLog(FAILED_SET_CAM_TXT, LogMsgType::EN_ERROR);
		}
		else
			g_pMainFrame->AddApplicationLog(FAILED_LOAD_CAM_TXT, LogMsgType::EN_ERROR);
	}
	else
	{
		if (GetMachineType() == eMachineType::eNSIS)
			g_pMainFrame->SetFlag_MakeMasterResult(RESULT_BAD, (int)ODB_ERROR_CODE::ODB_COPY_FILE);
		g_pMainFrame->AddApplicationLog(FAILED_EXTRACT_CAM_TXT, LogMsgType::EN_ERROR);
	}

	g_bFinishLoadCamThread = true;
	g_pMainFrame->HidePopupInitialize();
	g_pMainFrame->LockJobTree(TRUE);

	UserLayerSet_t* pUserLayerSet = g_pMainFrame->GetUserLayerSet();
	if (pUserLayerSet == nullptr) return RESULT_BAD;

	if (pUserLayerSet->bIsPreview && GetMachineType() != eMachineType::eNSIS)
	{
		g_pMainFrame->LoadLayer();
		g_pMainFrame->IsAllLayerLoaded(TRUE);
	}

	g_pMainFrame->m_dLoadTime = timerEx.StopTimer(CTimerUnit_en::EN_MIL_SEC);

	return RESULT_GOOD;// iRst;
}

UINT32 LoadCamData()
{
	if (g_pMainFrame == nullptr)
		return RESULT_BAD;

	g_pMainFrame->AdjustDockPanes();
	g_pMainFrame->ShowPopupInitialize(L"Loading Cam Data...");

	//2. Load CAM
	AddApplicationLog(LOAD_CAM_TXT);

	//2.1 Load Misc
	if (IArchive_LoadMisc() == RESULT_GOOD)
	{
		AddApplicationLog(SUCCEED_LOAD_MISC_TXT);
	}
	else
	{
		AddApplicationLog(FAILED_LOAD_MISC_TXT, LogMsgType::EN_ERROR);
		return RESULT_BAD;
	}

	//2.2 Load Matrix
	if (IArchive_LoadMatrix() == RESULT_GOOD)
	{
		AddApplicationLog(SUCCEED_LOAD_MATRIX_TXT);
	}
	else
	{
		AddApplicationLog(FAILED_LOAD_MATRIX_TXT, LogMsgType::EN_ERROR);
		return RESULT_BAD;
	}

	//2.3 Load Step
	if (IArchive_LoadStep() == RESULT_GOOD)
	{
		AddApplicationLog(SUCCEED_LOAD_STEP_TXT);
	}
	else
	{
		AddApplicationLog(FAILED_LOAD_STEP_TXT, LogMsgType::EN_ERROR);
		return RESULT_BAD;
	}

	//2.4 Load Font
	if (IArchive_LoadFont() == RESULT_GOOD)
	{
		AddApplicationLog(SUCCEED_LOAD_FONT_TXT);
	}
	else
	{
		AddApplicationLog(FAILED_LOAD_FONT_TXT, LogMsgType::EN_ERROR);
		return RESULT_BAD;
	}

	//2.5 Load Symbol
	if (IArchive_LoadUserSymbols() == RESULT_GOOD)
	{
		AddApplicationLog(SUCCEED_LOAD_USERSYMBOLS_TXT);
	}
	else
	{
		AddApplicationLog(FAILED_LOAD_USERSYMBOLS_TXT, LogMsgType::EN_ERROR);
		return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 LoadLayerData()
{
	if (g_pMainFrame == nullptr)
		return RESULT_BAD;

	UserSetInfo_t* pstUserSetInfo = g_pMainFrame->GetUserSetInfo();
	if (pstUserSetInfo == nullptr)
		return RESULT_BAD;

	if (pstUserSetInfo->strWorkLayer.GetLength() < 1)
		pstUserSetInfo->strWorkLayer = g_pMainFrame->GetWorkLayer();


	
// 		SaveSystemSpec();
// 		g_pMainFrame->LoadSpecData();
	

	g_pDrawManager->ClearDisplay();
	DrawCamData();

	if (!g_pMainFrame->GetFlag_ExtractCam())
	{
		if (GetMachineType() == eMachineType::eNSIS)
			g_pMainFrame->SetFlag_MakeMasterResult(RESULT_BAD, (int)ODB_ERROR_CODE::EXTRACT_ODB_FAIL);

		AddApplicationLog(NOT_COMPLETED_EXTRACT_CAM_TXT, LogMsgType::EN_ERROR);
		return RESULT_BAD;
	}

	g_pMainFrame->AdjustDockPanes();
	g_bFinishLoadCamThread = false;

	if (GetMachineType() == eMachineType::eNSIS)
	{
		pstUserSetInfo->bShowStepRepeat = false;
		pstUserSetInfo->bShowSurface = true;		
		pstUserSetInfo->bShowPanelDummy = true;
	}
	// 2023.01.17 KJH ADD
	// AVI 모드일경우 Viewer 조건 추가
	else if (GetMachineType() == eMachineType::eAVI)
	{
		pstUserSetInfo->bShowStepRepeat = true;
		pstUserSetInfo->bShowSurface = true;
		pstUserSetInfo->bShowProfile = true;
	}
	else
	{
		pstUserSetInfo->bShowPanelDummy = true;
		pstUserSetInfo->bShowFOV = false;
		pstUserSetInfo->bShowCell = false;
		pstUserSetInfo->bShowUnitAlign = false;
		pstUserSetInfo->bShowInspMask = false;
	}

	AfxBeginThread(LoadLayer, g_pMainFrame->GetUserLayerSet());

	return RESULT_GOOD;
}

UINT32 LoadLayer(LPVOID Param)
{
	CPerfTimerEx timerEx;
	timerEx.StartTimer();

	g_pMainFrame->BeginWaitCursor();

	UserLayerSet_t* pLayerSet = (UserLayerSet_t*)Param;
	UINT32 iRst = RESULT_GOOD;

	if (pLayerSet->bIsPreview)
		iRst = IArchive_LoadLayer(pLayerSet);
	else
		iRst = IArchive_LoadLayer(pLayerSet, TRUE);

	if (iRst == RESULT_GOOD)
		AddApplicationLog(SUCCEED_LOAD_LAYER_TXT);
	else
		AddApplicationLog(FAILED_LOAD_LAYER_TXT, LogMsgType::EN_ERROR);

	//Load가 완료되었다면, View 초기 값으로 변경
	SystemSpec_t* pSysSpec = GetSystemSpec();
	UserSetInfo_t* pstUserSetInfo = g_pMainFrame->GetUserSetInfo();

	if (pSysSpec->sysBasic.emDirectionType == eDirectionType::DirectionTypeNone)
	{
		pstUserSetInfo->bMirror = false;
	}
	else
	{
		pstUserSetInfo->bMirror = true;
	}

	pstUserSetInfo->mirrorDir = pSysSpec->sysBasic.emDirectionType;
	if (pstUserSetInfo->prodSide == eProdSide::eBot)
	{
		pstUserSetInfo->bMirror = true;
		pstUserSetInfo->mirrorDir = eDirectionType::eHorizontal;
	}

	pstUserSetInfo->dAngle = pSysSpec->sysBasic.dAngle;

	if (GetMachineType() == eMachineType::eNSIS)
	{
		if (RESULT_GOOD == g_pDataManager->GetMeasureData(g_pMainFrame->GetMeasureData(), FALSE, TRUE))
		{			
			g_pMainFrame->GetUserSetInfo()->rcpMode = eRecipeMode::eRecipe_Monitor_Panel;
			//g_pMainFrame->GetUserSetInfo()->rcpMode = eRecipeMode::eRecipe_MeasureSR_Panel;

			//if (RESULT_GOOD != SetMeatureData())
			if (RESULT_GOOD != g_pMainFrame->UpdataMeasureInfoView())
			{
				g_pMainFrame->ClearMeasureData();
				g_pMainFrame->SetFlag_MakeMasterResult(RESULT_BAD, (int)ODB_ERROR_CODE::READ_FAIL);
			}
			else
			{//성공
// 				if (RESULT_GOOD == g_pMainFrame->SetMinimap(g_pDataManager->GetProductInfo(), g_pDataManager->Is4Step(), eViewType::eAllVT))
// 					iRst |= g_pMainFrame->ChangeViewType(eViewType::eAllVT);
			}
		}
		else
		{
			g_pMainFrame->ClearMeasureData();
			g_pMainFrame->SetFlag_MakeMasterResult(RESULT_BAD, (int)ODB_ERROR_CODE::GET_MEASUER_FAIL);
		}

		g_pMainFrame->IsDoneLoadLayers(TRUE);
		if (RESULT_GOOD != DrawCamData(TRUE, TRUE))
		{
			g_pMainFrame->ClearMeasureData();
			g_pMainFrame->SetFlag_MakeMasterResult(RESULT_BAD, (int)ODB_ERROR_CODE::READ_FAIL);
		}

		//g_pMainFrame->UpdataMeasureInfoView();
		
		//Minimap
		if (g_pDrawMinimap)
		{
			g_pDrawMinimap->SetPanelInfo(GetProductInfo());
			g_pDrawMinimap->ResetView();
			g_pDrawMinimap->SetMode(2);
			g_pDrawMinimap->SetUserSetInfo(g_pMainFrame->GetUserSetInfo(), g_pMainFrame->GetUserLayerSet());			
		}
	}
	else
	{


		g_pMainFrame->IsDoneLoadLayers(TRUE);
		DrawCamData(TRUE, TRUE);

		//Pos로만 호출
		g_pMainFrame->GetHisto_PatternWidth(eLineWidthOption::eLinePos, *g_pDataManager->GetLineWidth(eLineWidthOption::eLinePos), *g_pDataManager->GetLineWidth(eLineWidthOption::eLineNeg));
		g_pMainFrame->GetHisto_PatternWidth(eLineWidthOption::eArcPos, *g_pDataManager->GetLineWidth(eLineWidthOption::eArcPos), *g_pDataManager->GetLineWidth(eLineWidthOption::eArcNeg));
		g_pMainFrame->GetHisto_PatternWidth(eLineWidthOption::eLineArcPos, *g_pDataManager->GetLineWidth(eLineWidthOption::eLineArcPos), *g_pDataManager->GetLineWidth(eLineWidthOption::eLineArcNeg));
		g_pDataManager->SetSortWidthData();
		//
	}

	g_pMainFrame->m_dExtractTime = timerEx.StopTimer(CTimerUnit_en::EN_MIL_SEC);

	g_bFinishLoadCamThread = true;
	//	g_pMainFrame->IsDoneLoadLayers(TRUE);
	g_pMainFrame->EndWaitCursor();
	Sleep(EXTRACT_CLOSE_TIMER_DELAY);
	g_pMainFrame->EndExtractLayer();



	return iRst;
}

UINT32 MakeMasterData()
{
	if (g_pMainFrame == nullptr)
		return RESULT_BAD;

	if (!g_pMainFrame->GetFlag_LoadLayers())
	{
		if (GetMachineType() == eMachineType::eNSIS)
			g_pMainFrame->SetFlag_MakeMasterResult(RESULT_BAD, (int)ODB_ERROR_CODE::GET_LAYER_FAIL);

		AddApplicationLog(NOT_COMPLETED_LOAD_LAYER_TXT, LogMsgType::EN_ERROR);
		return RESULT_BAD;
	}

	g_pMainFrame->AdjustDockPanes();
	g_bFinishMakeMasterThread = false;

	AfxBeginThread(MakeMaster, NULL);

	return RESULT_GOOD;
}

UINT32 MakeMaster(LPVOID Param)
{
	CPerfTimerEx timerEx;
	timerEx.StartTimer();

	g_pMainFrame->BeginWaitCursor();

	//Master Data 출력시 Line, Arc, LineArc에 대한 정보 업데이트
	g_pMainFrame->GetHisto_PatternWidth(eLineWidthOption::eLinePos, *g_pDataManager->GetLineWidth(eLineWidthOption::eLinePos), *g_pDataManager->GetLineWidth(eLineWidthOption::eLineNeg));
	g_pMainFrame->GetHisto_PatternWidth(eLineWidthOption::eArcPos, *g_pDataManager->GetLineWidth(eLineWidthOption::eArcPos), *g_pDataManager->GetLineWidth(eLineWidthOption::eArcNeg));
	g_pMainFrame->GetHisto_PatternWidth(eLineWidthOption::eLineArcPos, *g_pDataManager->GetLineWidth(eLineWidthOption::eLineArcPos), *g_pDataManager->GetLineWidth(eLineWidthOption::eLineArcNeg));
	g_pDataManager->SetSortWidthData();
	//

	UINT32 iRst = g_pDataManager->SetUserSetInfo(g_pMainFrame->GetUserSetInfo(), g_pMainFrame->GetUserLayerSet());
	iRst |= g_pDataManager->MakeMasterData();


	if (GetMachineType() == eMachineType::eNSIS)
	{//Measure Info Update
		g_pMainFrame->UpdataMeasureInfoView();
	}


	g_pMainFrame->m_dMasterDataTime = timerEx.StopTimer(CTimerUnit_en::EN_MIL_SEC);

	CString str;
	str.Format(_T("\nLoadTime : %.2lf, Extract Time : %.2lf, Make Master Data : %.2lf\n"), g_pMainFrame->m_dLoadTime/1000./60.,
		g_pMainFrame->m_dExtractTime / 1000. / 60.,
		g_pMainFrame->m_dMasterDataTime / 1000. / 60.);
	OutputDebugString(str);


	if (iRst == RESULT_GOOD)
		AddApplicationLog(SUCCEED_MAKE_MASTER_TXT);
	else
		AddApplicationLog(FAILED_MAKE_MASTER_TXT, LogMsgType::EN_ERROR);

	g_bFinishMakeMasterThread = true;

	g_pMainFrame->EndWaitCursor();
	Sleep(MAKEMASTER_CLOSE_TIMER_DELAY);
	g_pMainFrame->EndMakeMaster();

	if (iRst == RESULT_GOOD)
		g_pMainFrame->IsDoneProcessMaster(TRUE);
	else
		g_pMainFrame->IsDoneProcessMaster(FALSE);

	iRst |= DrawCamData();

	if (iRst == RESULT_BAD)
	{
		if (GetMachineType() == eMachineType::eNSIS)
			g_pMainFrame->SetFlag_MakeMasterResult(RESULT_BAD, (int)ODB_ERROR_CODE::MAKE_ODB_FAIL);
	}

	return iRst;
}

UINT32 SetStepList()
{
	UINT32 iStepCnt = 0, iRst = RESULT_BAD;
	if (IArchive_GetStepCount(iStepCnt) == RESULT_GOOD)
	{
		std::vector<UINT32> vcStepRepeatCnt;
		vcStepRepeatCnt.assign(iStepCnt, 0);

		if (IArchive_GetStepRepeatCount(vcStepRepeatCnt) == RESULT_GOOD)
		{
			g_pMainFrame->SetStepCount(iStepCnt, vcStepRepeatCnt);

			std::vector<stStepInfo*>* pStepInfo = nullptr;
			g_pMainFrame->GetStepInfo(&pStepInfo);

			if (IArchive_GetStepInfo(pStepInfo) == RESULT_GOOD)
			{
				AddApplicationLog(SUCCEED_GET_STEP_INFO);
				iRst = RESULT_GOOD;
			}
			else
				AddApplicationLog(FAILED_GET_STEP_INFO, LogMsgType::EN_ERROR);
		}
		else
			AddApplicationLog(FAILED_GET_STEP_COUNT, LogMsgType::EN_ERROR);

		vcStepRepeatCnt.clear();
	}
	else
		AddApplicationLog(FAILED_GET_STEP_COUNT, LogMsgType::EN_ERROR);

	return iRst;
}

UINT32 SetLayerList()
{
	UINT32 iLayerCnt = 0, iRst = RESULT_BAD;
	if (IArchive_GetLayerCount(iLayerCnt) == RESULT_GOOD)
	{
		g_pMainFrame->SetLayerCount(iLayerCnt);

		std::vector<stLayerInfo*>* pLayerInfo = nullptr;
		vector<vector<stLayerInfo*>> pPatternLayerInfo;
		vector<vector<stLayerInfo*>> pSRLayerInfo;
		vector<vector<stLayerInfo*>> pMPLayerInfo;
		vector<vector<stLayerInfo*>> pTPLayerInfo;
		

		g_pMainFrame->GetLayerInfo(&pLayerInfo, pPatternLayerInfo, pSRLayerInfo, pMPLayerInfo, pTPLayerInfo);

		if (IArchive_GetLayerInfo(pLayerInfo) == RESULT_GOOD)
		{
			AddApplicationLog(SUCCEED_GET_LAYER_INFO);
			iRst = RESULT_GOOD;
		}
		else
			AddApplicationLog(FAILED_GET_LAYER_INFO, LogMsgType::EN_ERROR);
	}
	else
		AddApplicationLog(FAILED_GET_LAYER_COUNT, LogMsgType::EN_ERROR);

	return iRst;
}

UINT32 SetComponentList()
{
	UINT32 iStepCnt = 0, iRst = RESULT_BAD;
	if (IArchive_GetStepCount(iStepCnt) == RESULT_GOOD)
	{
		std::vector<UINT32> vcStepPkgCnt;
		vcStepPkgCnt.assign(iStepCnt, 0);

		if (IArchive_GetComponentCount(vcStepPkgCnt) == RESULT_GOOD)
		{
			g_pMainFrame->SetComponentCount(iStepCnt, vcStepPkgCnt);

			std::vector<stComponentInfo*>* pCompInfo = nullptr;
			g_pMainFrame->GetComponentInfo(&pCompInfo);

			if (IArchive_GetComponentInfo(pCompInfo) == RESULT_GOOD)
			{
				AddApplicationLog(SUCCEED_GET_COMP_INFO);
				iRst = RESULT_GOOD;
			}
			else
				AddApplicationLog(FAILED_GET_COMP_INFO, LogMsgType::EN_ERROR);
		}
		else
			AddApplicationLog(FAILED_GET_COMP_COUNT, LogMsgType::EN_ERROR);

		vcStepPkgCnt.clear();
	}
	else
		AddApplicationLog(FAILED_GET_STEP_COUNT, LogMsgType::EN_ERROR);

	return iRst;
}

UINT32 SetCamInfo()
{
	AddApplicationLog(SET_CAM_TXT);

	//1. Step
	if (SetStepList() != RESULT_GOOD)
		return RESULT_BAD;

	//2. Layer
	if (SetLayerList() != RESULT_GOOD)
		return RESULT_BAD;

	//3. Component
	if (SetComponentList() != RESULT_GOOD)
		return RESULT_BAD;

	return g_pMainFrame->DisplayCamInfo();
}

UINT32 SetLayerInfo()
{
	AddApplicationLog(SET_LAYER_TXT);

	if (g_pJob)
	{
		if (GetMachineType() != eMachineType::eNSIS)
		{
			CString strStrartLayerIdx, strBackLayerIdx;
			strStrartLayerIdx.Format(_T("%d"), g_pJob->m_Matrix.m_iStartLayerIdx);
			strBackLayerIdx.Format(_T("%d"), g_pJob->m_Matrix.m_iEndLayerIdx);

			return g_pMainFrame->DisplayLayerInfo(strStrartLayerIdx, strBackLayerIdx);
			/*return g_pMainFrame->DisplayLayerInfo(g_pJob->m_Matrix.m_iStartLayerIdx,
				g_pJob->m_Matrix.m_iEndLayerIdx);*/			
		}
		else
		{//NSIS 모드 일 경우
			/*if (g_pMainFrame->m_strStartLayer.IsEmpty() || g_pMainFrame->m_strEndLayer.IsEmpty())
			{
				return g_pMainFrame->DisplayLayerInfo();
			}*/

			if (GetUserSite() == eUserSite::eSEM)
			{
				//return g_pMainFrame->DisplayLayerInfo(g_pJob->m_Matrix.m_iStartLayerIdx, g_pJob->m_Matrix.m_iEndLayerIdx);				
				return g_pMainFrame->DisplayLayerInfo(g_pMainFrame->m_strStartLayer, _T(""));
			}
			else
			{
				CString strTemp;
				INT32 iStart, iEnd;

				strTemp = g_pMainFrame->m_strStartLayer;
				strTemp.Replace(_T("L"), _T(""));
				strTemp.Replace(_T("l"), _T(""));
				iStart = _ttoi(strTemp);

				strTemp = g_pMainFrame->m_strEndLayer;
				strTemp.Replace(_T("L"), _T(""));
				strTemp.Replace(_T("l"), _T(""));
				strTemp.Replace(_T("B"), _T(""));
				strTemp.Replace(_T("b"), _T(""));
				iEnd = _ttoi(strTemp);
				//return g_pMainFrame->DisplayLayerInfo(iStart, iEnd);
				return g_pMainFrame->DisplayLayerInfo(g_pMainFrame->m_strStartLayer, g_pMainFrame->m_strEndLayer);
			}
		}
	}
	else
		return RESULT_BAD;
}



CString GetCurAppPath()
{
	TCHAR szPath[_MAX_PATH + 1];
	GetModuleFileName(AfxGetApp()->m_hInstance, szPath, _MAX_PATH);

	CString strCurAppDir = szPath;
	int nLen = strCurAppDir.ReverseFind('\\');
	if (nLen > 0)
		strCurAppDir = strCurAppDir.Left(nLen);

	return strCurAppDir;
}

CString GetCamPath()
{
	return g_cSpec.GetCamPath();
}

eScanType GetScanType()
{
	return g_cSpec.GetScanType();
}

void SetScanType(eScanType scanType)
{
	return g_cSpec.SetScanType(scanType);
}

eUserSite GetUserSite()
{
	return g_cSpec.GetUserSite();
}

eMachineType GetMachineType()
{
	return g_cSpec.GetMachineType();
}

eProductType GetProductType()
{
	return g_cSpec.GetProductType();
}

void SetProductType(eProductType prdType)
{
	return g_cSpec.SetProductType(prdType);
}

CString GetModelName()
{
	CString strModel = L"";
	if (RESULT_GOOD == IArchive_GetModelName(strModel))
		return strModel;
	else
		return L"";
}

UINT32 GetMaxLayerNum()
{
	return g_cSpec.GetMaxLayerNum();
}

void ClearDisplay()
{//이거 안부를거 같은데..
	//IDraw_ClearDisplay();
	g_pDrawManager->ClearDisplay();
}

void InitCursorPt()
{
	if (g_pMainView)
		g_pMainView->InitCursorPt();
}

UINT32 DrawCamData(IN const BOOL &bScaleFitView, IN const BOOL &bResetViewer)
{
	if (g_bDrawInit && g_pMainFrame->GetFlag_ExtractCam())
	{
		HCURSOR hHandCursor = ::LoadCursor(NULL, IDC_WAIT);
		SetCursor(hHandCursor);

		InitCursorPt();

		UserSetInfo_t* pstUserSetInfo = g_pMainFrame->GetUserSetInfo();
		if (pstUserSetInfo == nullptr)
			return RESULT_BAD;

		pstUserSetInfo->iStepIdx = g_pMainFrame->GetCurStepIndex();

		if (bResetViewer == TRUE)
		{
			g_pDrawManager->ResetView();
		}

		if (GetFlag_LoadLayers())
		{
			if (GetMachineType() == eMachineType::eNSIS)
			{	
				SetDrawOnly_MeasureUnit(TRUE);				
			}
			else
			{
				if (pstUserSetInfo->bShowFOV || pstUserSetInfo->bShowCell ||
					pstUserSetInfo->bShowUnitAlign || pstUserSetInfo->bShowInspMask)
				{
					if (pstUserSetInfo->strWorkLayer.GetLength() < 1)
						pstUserSetInfo->strWorkLayer = g_pMainFrame->GetWorkLayer();

					g_pDataManager->OptimizeFOV(FALSE, TRUE);//Update TRUE
				}
			}
		}

		g_pDrawManager->SetUserSetInfo(g_pMainFrame->GetUserSetInfo(), g_pMainFrame->GetUserLayerSet());
		g_pDataManager->SetUserSetInfo(g_pMainFrame->GetUserSetInfo(), g_pMainFrame->GetUserLayerSet());

		if (bScaleFitView || pstUserSetInfo->bSideChanged)
		{
			g_pDrawManager->ResetZoom();

			if (pstUserSetInfo->bSideChanged)
				pstUserSetInfo->bSideChanged = false;
		}

		hHandCursor = ::LoadCursor(NULL, IDC_ARROW);
		SetCursor(hHandCursor);


		//Minimap
		if (g_pDrawMinimap)
		{
			if (bResetViewer == TRUE)
			{
				g_pDrawMinimap->ResetView();
			}

			if (bScaleFitView)
			{
				g_pDrawMinimap->ResetZoom();
			}

			g_pDrawMinimap->SetUserSetInfo(g_pMainFrame->GetUserSetInfo(), g_pMainFrame->GetUserLayerSet());

		}

		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
	
}

UINT32 DrawZoomInit()
{
	//return IDraw_ZoomInit();

	return g_pDrawManager->ResetZoom();
}


UINT32			DrawLBtuttonDblClk(UINT nFlags, CPoint point)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	if (g_pMainFrame->m_strType == "Surface")

	{
		CString LayerName;
		LayerName = g_pMainFrame->GetLayerName();
		g_pDrawManager->SetSurface(LayerName);
		g_pDrawManager->ClearTempPoint();
		SetPointMode(FALSE);
		AddApplicationLog(APPLY_ADD_SURFACE_TXT);
		g_pMainFrame->m_strType = _T("");
	}	

	if (g_pMainFrame->m_strType == "Arc")
	{
		CString LayerName;
		LayerName = g_pMainFrame->GetLayerName();
		g_pDrawManager->SetArc(LayerName);
		g_pDrawManager->ClearTempPoint();
		SetPointMode(FALSE);
		AddApplicationLog(APPLY_ADD_ARC_TXT);
		g_pMainFrame->m_strType = _T("");
	}

	
	//if (g_pDrawManager->GetEditMode() == EditMode::enumMode_EditTP)
	//{
	//	SetPointMode(TRUE, 0, _T("Cross Center"));
	//	g_pMainFrame->m_strType = _T("TP");
	//}
	//UINT32 bRet;
	//bRet = g_pDrawManager->OnLButtonUp(nFlags, point);

	//if (g_pDrawManager->GetPointMode() == TRUE)
	//{
	//	g_pDrawManager->SetPointMode(FALSE);

	//	UINT32 nMode = GetViewerEditMode();


	//	if (nMode == EditMode::enumMode_EditTP)
	//	{

	//		float fX, fY;
	//		CString LayerName;
	//		LayerName = g_pMainFrame->GetLayerName();

	//		if (g_pMainFrame->m_strType == "TP")
	//		{
	//			g_pDrawManager->GetTempPoint(0, fX, fY);
	//			g_pDrawManager->SetSelectPoint(fX, fY);
	//			g_pDrawManager->_SetTp(fX, fY);
	//			g_pDrawManager->SetTp(LayerName);
	//			g_pDrawManager->ClearTempPoint();
	//			g_pMainFrame->m_strType == _T("");
	//			AddApplicationLog(APPLY_ADD_TP_TXT);
	//		}

	//		// 2022.06.07 
	//		// 김준호 주석추가
	//		// setadd 후 measureinfo update 위해 추가 일단 주석처리
	//		if (GetMachineType() == eMachineType::eNSIS)
	//		{
	//			g_pDataManager->GetMeasureData(g_pMainFrame->GetMeasureData(), FALSE, TRUE);
	//			g_pMainFrame->UpdataMeasureInfoView();
	//		}

	//		//SetViewerEditMode(EditMode::enumMode_EditMode);
	//	}
	//}

	return RESULT_GOOD;
}

UINT32			DrawLButtonDown(UINT nFlags, CPoint point)
{
	// 2022.06.07
	// 김준호 주석추가
	// Layer 정보 없을때 Viewer 클릭시 에러 처리
	if (g_pMainFrame->GetWorkLayer() == "") return RESULT_BAD;

	if (g_pDrawManager == nullptr) return RESULT_BAD;
	
	UINT32 nRet = g_pDrawManager->OnLButtonDown(nFlags, point);

	//SetEdit_Feature();
	g_pDrawManager->m_dResizeRatio = 1.0;		
	g_pDrawManager->m_bWheelMode = FALSE;
	//Log_FeatureSelect();		
	//SetFeatureData();
	g_pMainFrame->LoadFeatureData();

	BOOL bSelect = IsSelect();
	if (bSelect == TRUE) { Log_FeatureSelect(); }



	//if (bSelect == TRUE && GetMachineType() == eMachineType::eNSIS)
	if ( GetMachineType() == eMachineType::eNSIS)

	{
		//선택된 Feature에 대한 List Update
		g_pMainFrame->SetMeasure_Select_UI();
	}

	//return nRet;
	return RESULT_GOOD;
}

UINT32			DrawLButtonUp(UINT nFlags, CPoint point)
{
	// 2022.06.07
	// 김준호 주석추가
	// Layer 정보 없을때 Viewer 클릭시 에러 처리
	if (g_pMainFrame->GetWorkLayer() == "") return RESULT_BAD;

	if (g_pDrawManager == nullptr) return RESULT_BAD;

	UserSetInfo_t* pUserSetInfo = GetUserSetInfo();

	if (g_pDrawManager->GetEditMode() == EditMode::enumMode_EditTP && g_pMainFrame->GetCheckSR() == TRUE)
	{
		ResetSelect();
		SetPointMode(TRUE, 0, _T("Cross"));
		g_pMainFrame->m_strType = _T("TP");
	}
	if (g_pDrawManager->GetEditMode() == EditMode::enumMode_EditTP_Center && g_pMainFrame->GetCheckSRAuto() == TRUE)
	{
		ResetSelect();
		SetPointMode(TRUE, 0, _T("Cross Center"));
		g_pMainFrame->m_strType = _T("TP");
	}

	UINT32 bRet;
	bRet = g_pDrawManager->OnLButtonUp(nFlags, point);

	if (g_pDrawManager->GetPointMode() == TRUE)
	{
		g_pDrawManager->SetPointMode(FALSE);

		UINT32 nMode = GetViewerEditMode();
		if (nMode == EditMode::enumMode_EditMask_Add_FeatureBase)
		{//hj.kim//Feature 기반의 Align 생성

			SELECT_INFO* pSelectInfo = g_pDrawManager->GetSelectInfo();

			//
			g_pMainFrame->SetSetlectFeatureInfo(pSelectInfo);
			//

			g_pDrawManager->ResetSelect();

			SetViewerEditMode(EditMode::enumMode_EditMask_Add_FeatureBase);

		}

		else if (nMode == EditMode::enumMode_EditTP)
		{
			float fX, fY;
			CString LayerName;
			LayerName = g_pMainFrame->GetLayerName();

			if (g_pMainFrame->m_strType == "TP")
			{
				g_pDrawManager->GetTempPoint(0, fX, fY);
				g_pDrawManager->SetSelectPoint(fX, fY);
				//g_pDrawManager->_SetTp(fX, fY);
				g_pDrawManager->SetTp(LayerName);
				g_pDrawManager->ClearTempPoint();
				g_pMainFrame->m_strType == _T("");
				AddApplicationLog(APPLY_ADD_TP_TXT);
			}

			// 2022.06.07 
			// 김준호 주석추가
			// setadd 후 measureinfo update 위해 추가 일단 주석처리
			if (GetMachineType() == eMachineType::eNSIS)
			{
				g_pDataManager->GetMeasureData(g_pMainFrame->GetMeasureData(), FALSE, TRUE);
				g_pMainFrame->UpdataMeasureInfoView();
			}

			//SetViewerEditMode(EditMode::enumMode_EditMode);
		}
		else if (nMode == EditMode::enumMode_EditTP_Center)
		{
			float fX, fY;
			CString LayerName;
			LayerName = g_pMainFrame->GetLayerName();

			if (g_pMainFrame->m_strType == "TP")
			{
				g_pDrawManager->GetTempPoint(0, fX, fY);
				g_pDrawManager->SetSelectPoint(fX, fY);
				g_pDrawManager->_SetTp(fX, fY);
				g_pDrawManager->SetTp(LayerName);
				g_pDrawManager->ClearTempPoint();
				g_pMainFrame->m_strType == _T("");
				AddApplicationLog(APPLY_ADD_TP_TXT);
			}

			// 2022.06.07 
			// 김준호 주석추가
			// setadd 후 measureinfo update 위해 추가 일단 주석처리
			if (GetMachineType() == eMachineType::eNSIS)
			{
				g_pDataManager->GetMeasureData(g_pMainFrame->GetMeasureData(), FALSE, TRUE);
				g_pMainFrame->UpdataMeasureInfoView();
			}

			//SetViewerEditMode(EditMode::enumMode_EditMode);
		}
		else if (nMode == EditMode::enumMode_EditMode)
		{
			float fX, fY;
			float fXs, fYs, fXe, fYe;			
			CString LayerName;
			LayerName = g_pMainFrame->GetLayerName();

			if (g_pMainFrame->m_strType == _T("")) return RESULT_BAD;

			if (g_pMainFrame->m_strType == "Surface")
			{
				if ((nFlags & MK_SHIFT) == MK_SHIFT)
				{
					g_nSelectIndex -= 1;
					g_pDrawManager->GetTempPoint_Center(g_nSelectIndex, fX, fY);
					g_pDrawManager->SetSelectPoint_Center(g_nSelectIndex, fX, fY);
					g_nSelectIndex++;

				}
				else
				{
					g_pDrawManager->GetTempPoint(g_nSelectIndex, fX, fY);
					g_pDrawManager->SetSelectPoint_Add(g_nSelectIndex, fX, fY);
					g_nSelectIndex++;
				}

			}
			else if (g_pMainFrame->m_strType == "Arc")
			{
				if ((nFlags & MK_SHIFT) == MK_SHIFT)
				{
					g_nSelectIndex -= 1;
					g_pDrawManager->GetTempPoint_Center(g_nSelectIndex, fX, fY);
					g_pDrawManager->SetSelectPoint_Center(g_nSelectIndex, fX, fY);
					g_nSelectIndex++;

				}
				else
				{
					g_pDrawManager->GetTempPoint(g_nSelectIndex, fX, fY);
					g_pDrawManager->SetSelectPoint_Add(g_nSelectIndex, fX, fY);
					if (g_nSelectIndex == 2)
					{
						g_pDrawManager->SetArc(LayerName);
						ClearTempPoint();
						g_pDrawManager->SetPointMode(FALSE);
					}
					g_nSelectIndex++;
				}
			}
			else if (g_pMainFrame->m_strType == _T("Line") || 
				g_pMainFrame->m_strType == _T("Line_X") ||
				g_pMainFrame->m_strType == _T("Line_Y") )
			{
				ResetSelect();

				int nLineMode = g_pMainFrame->m_nLineMode;

				g_pDrawManager->GetTempPoint(0, fXs, fYs);
				g_pDrawManager->SetSelect_StartPoint(fXs, fYs);

				SetPointMode(TRUE, 1, _T("Line End"));
				//g_pMainFrame->m_strType = _T("Line");
				g_pDrawManager->GetTempPoint(1, fXe, fYe);
				g_pDrawManager->SetSelect_EndPoint(fXe, fYe);

				if (fXe != 0.f && fYe != 0.f)
				{//두번째 클릭되었을때만 들어온다.

					g_pDrawManager->SetLine(LayerName, nLineMode);
					g_pDrawManager->ClearTempPoint();
					g_pDrawManager->SetPointMode(FALSE);
					AddApplicationLog(APPLY_ADD_LINE_TXT);
				}
			}
			else if (g_pMainFrame->m_strType == _T("Pad"))
			{
				g_pDrawManager->GetTempPoint(0, fX, fY);
				g_pDrawManager->SetSelectPoint(fX, fY);
				
				if (g_pMainFrame->m_strSymbol == _T("Circle"))
				{
					g_pDrawManager->SetCircle(LayerName);
					AddApplicationLog(APPLY_ADD_CIRCLE_TXT);


				}
				else if (g_pMainFrame->m_strSymbol == _T("Rectangle"))
				{
					g_pDrawManager->SetRect(LayerName);
					AddApplicationLog(APPLY_ADD_RECTAGNLE_TXT);
				}
				g_pDrawManager->ClearTempPoint();
			}

			/*else if (g_pMainFrame->m_strType == "TP")
			{
				g_pDrawManager->GetTempPoint(0, fX, fY);
				g_pDrawManager->SetSelectPoint(fX, fY);
				g_pDrawManager->_SetTp(fX, fY);
				g_pDrawManager->SetTp(LayerName);
				g_pDrawManager->ClearTempPoint();
				g_pMainFrame->m_strType == _T("");
				AddApplicationLog(APPLY_ADD_TP_TXT);
			}*/

			// 2022.06.07 
			// 김준호 주석추가
			// setadd 후 measureinfo update 위해 추가 일단 주석처리
			if (GetMachineType() == eMachineType::eNSIS)
			{
				g_pDataManager->GetMeasureData(g_pMainFrame->GetMeasureData(), FALSE, TRUE);
				g_pMainFrame->UpdataMeasureInfoView();
			}

		}
		else if (nMode == EditMode::enumMode_EditAdd)
		{
			float fX, fY;
			float fXs, fYs, fXe, fYe;
			float fxC = 0.f, fyC = 0.f, fWidth = 0.f, fHeight = 0.f;
			
			CString LayerName;
			LayerName = g_pMainFrame->GetLayerName();

			if (g_pMainFrame->m_strType == _T("")) return RESULT_BAD;

			
			SELECT_INFO* pSelectInfo = g_pDrawManager->GetSelectInfo();
			if (pSelectInfo->GetCount() <= 0)
			{
				AfxMessageBox(_T("Feature가 인식되지 않았습니다."));
				nMode = EditMode::enumMode_EditMode;
				SetViewerEditMode(nMode);

				return RESULT_BAD;
			}

			// 클릭된 Feature 정보 Log 남김
			CString strLog;
			strLog.Format(_T("Add Feature Base, Select Feature : %s"), 
				DefFeatueType::pName[static_cast<int>(pSelectInfo->vecSelectInfoUnit[0].emFeatureType)]);
			AddApplicationLog(strLog);


			float fcX = 0, fcY = 0;
			fcX = static_cast<float>(pSelectInfo->vecSelectInfoUnit[0].dCx);
			fcY = static_cast<float>(pSelectInfo->vecSelectInfoUnit[0].dCy);

			if (g_pMainFrame->m_strType == _T("Line_X_FEATURE") ||
				g_pMainFrame->m_strType == _T("Line_Y_FEATURE"))
			{
				int nLineMode = g_pMainFrame->m_nLineMode;
				
				SELECT_INFO* pSelectInfo = g_pDrawManager->GetSelectInfo();
				if (pSelectInfo->GetCount() > 0)
				{
					UINT32 nRet;

					SELECT_INFO_UNIT &stSelectInfoUnit = pSelectInfo->vecSelectInfoUnit[0];
					if (stSelectInfoUnit.emFeatureType == FeatureType::L)
					{
						nRet = g_pMainFrame->GetLineInfo_LineBase(stSelectInfoUnit, nLineMode, fXs, fYs, fXe, fYe);
						//자동으로 Line의 Start, End를 계산하였기 때문에 자유 모드로 강제로 변경
						nLineMode = AddLineMode::enumLineMode_XY;
					}
					else if (stSelectInfoUnit.emFeatureType == FeatureType::A)
					{
						nRet = g_pMainFrame->GetLineInfo_ArcBase(stSelectInfoUnit, nLineMode, fXs, fYs, fXe, fYe);
						//자동으로 Line의 Start, End를 계산하였기 때문에 자유 모드로 강제로 변경
						nLineMode = AddLineMode::enumLineMode_XY;
					}
					else
					{
						nRet = g_pMainFrame->GetLineInfo_PadBase(stSelectInfoUnit, nLineMode, fXs, fYs, fXe, fYe);
					}

					if (nRet != RESULT_GOOD)
					{
						AfxMessageBox(_T("Feature가 인식되지 않았습니다."));
						nMode = EditMode::enumMode_EditMode;
						SetViewerEditMode(nMode);

						return RESULT_BAD;
					}
				}
				else
				{
					AfxMessageBox(_T("Feature가 인식되지 않았습니다."));
					nMode = EditMode::enumMode_EditMode;
					SetViewerEditMode(nMode);

					return RESULT_BAD;
				}	

				
				
				g_pDrawManager->SetSelect_StartPoint(fXs, fYs);
				g_pDrawManager->SetSelect_EndPoint(fXe, fYe);
				SetPointMode(TRUE, 1, _T("Line End"));

				if (fXe != 0.f && fYe != 0.f)
				{
					g_pDrawManager->SetLine(LayerName, nLineMode);
					g_pDrawManager->ClearTempPoint();
					g_pDrawManager->SetPointMode(FALSE);
					AddApplicationLog(APPLY_ADD_LINE_TXT);
				}
			}
			else if (g_pMainFrame->m_strType == "Pad_FEATURE")
			{	
				double dWidth = 0., dHeight = 0.;

				SELECT_INFO* pSelectInfo = g_pDrawManager->GetSelectInfo();
				int nSelectCount = pSelectInfo->GetCount();
				if (nSelectCount > 0)
				{
					fX = static_cast<float>(pSelectInfo->vecSelectInfoUnit[0].dCx);
					fY = static_cast<float>(pSelectInfo->vecSelectInfoUnit[0].dCy);
					dWidth = pSelectInfo->vecSelectInfoUnit[0].dWidth;
					dHeight = pSelectInfo->vecSelectInfoUnit[0].dHeight;
				}

				if (nSelectCount <= 0 || fX == 0.f || fY == 0.f)
				{
					AfxMessageBox(_T("Feature가 인식되지 않았습니다."));
					nMode = EditMode::enumMode_EditMode;
					SetViewerEditMode(nMode);

					return RESULT_BAD;
				}
				g_pDrawManager->SetSelectPoint(fX, fY);
				
				if (g_pMainFrame->m_strSymbol == "Circle")
				{
					SELECT_INFO_UNIT &stSelectInfoUnit = pSelectInfo->vecSelectInfoUnit[0];

					double dDiameter = -999.;
					if (stSelectInfoUnit.emFeatureType == FeatureType::L)
					{
						if (stSelectInfoUnit.pFeature != nullptr &&
							stSelectInfoUnit.pFeature->m_pSymbol != nullptr &&
							static_cast<int>(stSelectInfoUnit.pFeature->m_pSymbol->m_vcParams.size()) > 0)
						{
							CFeatureL *pFeatureL = (CFeatureL *)stSelectInfoUnit.pFeature;
							if (pFeatureL->m_dXs == pFeatureL->m_dXe &&
								pFeatureL->m_dYs == pFeatureL->m_dYe)
							{//선에 대한 정보가 이상한 경우. UserDefineSymbol일때 디버깅 필요함.
								bRet = RESULT_BAD;
							}
							else
							{
								dDiameter = stSelectInfoUnit.pFeature->m_pSymbol->m_vcParams[0];
								bRet = RESULT_GOOD;
							}

							
						}
					}
					else if (stSelectInfoUnit.emFeatureType == FeatureType::A)
					{
						if (stSelectInfoUnit.pFeature != nullptr &&
							stSelectInfoUnit.pFeature->m_pSymbol != nullptr &&
							static_cast<int>(stSelectInfoUnit.pFeature->m_pSymbol->m_vcParams.size()) > 0)
						{
							dDiameter = stSelectInfoUnit.pFeature->m_pSymbol->m_vcParams[0];

							bRet = RESULT_GOOD;
						}
					}
					else
					{
						dDiameter = MIN(dWidth, dHeight);

						bRet = RESULT_GOOD;
					}

					if (bRet != RESULT_GOOD)
					{//NG
						AfxMessageBox(_T("Feature가 인식되지 않았습니다."));
						nMode = EditMode::enumMode_EditMode;
						SetViewerEditMode(nMode);
						return RESULT_BAD;
					}
					else
					{//OK
						g_pDrawManager->SetCircle(LayerName, dDiameter);
						AddApplicationLog(APPLY_ADD_CIRCLE_TXT);
					}
				} 

				if (g_pMainFrame->m_strSymbol == "Rectangle")
				{
					double dRectWidth = -999., dRectHeight = -999.;

					SELECT_INFO_UNIT &stSelectInfoUnit = pSelectInfo->vecSelectInfoUnit[0];
					if (stSelectInfoUnit.emFeatureType == FeatureType::L)
					{
						if (stSelectInfoUnit.pFeature != nullptr &&
							stSelectInfoUnit.pFeature->m_pSymbol != nullptr &&
							static_cast<int>(stSelectInfoUnit.pFeature->m_pSymbol->m_vcParams.size()) > 0)
						{
							CFeatureL *pFeatureL = (CFeatureL *)stSelectInfoUnit.pFeature;
							if (pFeatureL->m_dXs == pFeatureL->m_dXe &&
								pFeatureL->m_dYs == pFeatureL->m_dYe)
							{//선에 대한 정보가 이상한 경우. UserDefineSymbol일때 디버깅 필요함.
								bRet = RESULT_BAD;
							}
							else
							{
								dRectWidth = stSelectInfoUnit.pFeature->m_pSymbol->m_vcParams[0];
								dRectHeight = stSelectInfoUnit.pFeature->m_pSymbol->m_vcParams[0];

								bRet = RESULT_GOOD;
							}
						}
					}
					else if (stSelectInfoUnit.emFeatureType == FeatureType::A)
					{
						if (stSelectInfoUnit.pFeature != nullptr &&
							stSelectInfoUnit.pFeature->m_pSymbol != nullptr &&
							static_cast<int>(stSelectInfoUnit.pFeature->m_pSymbol->m_vcParams.size()) > 0 )
						{
							dRectWidth = stSelectInfoUnit.pFeature->m_pSymbol->m_vcParams[0];
							dRectHeight = stSelectInfoUnit.pFeature->m_pSymbol->m_vcParams[0];

							bRet = RESULT_GOOD;
						}
					}
					else
					{
						if (GetUserSetInfo()->dAngle == 90. || GetUserSetInfo()->dAngle == 270.)
						{
							double dTemp = 0.;
							SWAP(dWidth, dHeight, dTemp);
						}

						dRectWidth = dWidth;
						dRectHeight = dHeight;

						bRet = RESULT_GOOD;
					}
					if (bRet != RESULT_GOOD)
					{//NG
						AfxMessageBox(_T("Feature가 인식되지 않았습니다."));
						nMode = EditMode::enumMode_EditMode;
						SetViewerEditMode(nMode);
						return RESULT_BAD;
					}
					else
					{//OK
						g_pDrawManager->SetRect(LayerName, dRectWidth, dRectHeight);
						AddApplicationLog(APPLY_ADD_RECTAGNLE_TXT);
					}
				}

				g_pDrawManager->ClearTempPoint();
			}			

			// 2022.06.07 
			// 김준호 주석추가
			// setadd 후 measureinfo update 위해 추가 일단 주석처리
 			if (GetMachineType() == eMachineType::eNSIS)
			{
				g_pDataManager->GetMeasureData(g_pMainFrame->GetMeasureData(), FALSE, TRUE);
				g_pMainFrame->UpdataMeasureInfoView();
			}
		}

		if (g_pDrawManager->GetEditMode() != EditMode::enumMode_EditMask_Add_FeatureBase) // && 
			//(g_pDrawManager->GetEditMode() != EditMode::enumMode_EditTP ||
			 //g_pDrawManager->GetEditMode() != EditMode::enumMode_EditTP_Center ))
		{ 
			if ((g_pDrawManager->GetEditMode() == EditMode::enumMode_EditTP) ||
				(g_pDrawManager->GetEditMode() == EditMode::enumMode_EditTP_Center))
			{

			}
			else
			{
				SetViewerEditMode(EditMode::enumMode_EditMode);
			}
		}
	}

	bRet |= g_pMainFrame->LoadFeatureData();

	return bRet;

}

UINT32			DrawMouseMove(UINT nFlags, CPoint point)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = g_pDrawManager->OnMouseMove(nFlags, point);

	g_pDrawManager->IsMove();
	nRet |= g_pMainFrame->LoadMoveData();
	return nRet;
}

UINT32			DrawMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (g_pMainFrame->GetWorkLayer() == "") return RESULT_BAD;

	UINT32 nRet;

	nRet = g_pDrawManager->OnMouseWheel(nFlags, zDelta, pt);
	nRet |= g_pMainFrame->LoadFeatureData();
	return nRet;
}

UINT32			DrawRButtonDblClk(UINT nFlags, CPoint point)
{
	return g_pDrawManager->OnRButtonDblClk(nFlags, point);
}

UINT32			DrawRButtonDown(UINT nFlags, CPoint point)
{
	return g_pDrawManager->OnRButtonDown(nFlags, point);
}
UINT32			DrawRButtonUp(UINT nFlags, CPoint point)
{
	return g_pDrawManager->OnRButtonUp(nFlags, point);
}

/*
UINT32 DrawZoomInRect(RECT* rcZoomRect)
{
	return IDraw_ZoomInRect(rcZoomRect);
}

UINT32 MoveZoomInRect(POINT* ptMove)
{
	return IDraw_MoveZoomInRect(ptMove);
}

UINT32 DrawZoomInOut(eZoomType zoomType)
{
	return IDraw_ZoomInOut(zoomType);
}
*/

UINT32			DrawZoomIn()
{
	return g_pDrawManager->ZoomIn();
}

UINT32			DrawZoomOut()
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	return g_pDrawManager->ZoomOut();
}

UINT32			DrawZoomRect(IN const RECTD &drtRect)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	return g_pDrawManager->DrawZoomRect(drtRect);
}

UINT32			SetViewInfo(IN ViewInfo_t* pViewInfo)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	return g_pDrawManager->SetViewInfo(pViewInfo);
}

UINT32			SetMinimapSizeChange(IN ViewInfo_t* pViewInfo)
{
	if (g_pDrawMinimap == nullptr) return RESULT_BAD;

	return g_pDrawMinimap->ChangeViewSize(*pViewInfo);
}

void			SetViewerEditMode(IN const UINT32 &nEditMode)
{
	if (g_pDrawManager == nullptr) return;

	g_pDrawManager->SetEditMode(nEditMode);
}

UINT32			GetViewerEditMode()
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	return g_pDrawManager->GetEditMode();
}

UINT32			SetResize_Apply(IN const float &fResizePercent)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = g_pDrawManager->SetResize_Apply(fResizePercent);

	if (nRet == RESULT_GOOD && GetMachineType() == eMachineType::eNSIS)
	{
		//nRet |= SetMeatureData();
		nRet |= g_pMainFrame->UpdataMeasureInfoView();
	}

	return nRet;
}

UINT32 SetAttribute_Apply(IN const CString&strAttrName, IN const CString&strAttrString)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = g_pDrawManager->SetAttribute(strAttrName, strAttrString);

	UserSetInfo_t* pUserSetInfo = GetUserSetInfo();
	if (pUserSetInfo == nullptr)
		return RESULT_BAD;

	eRecipeMode rcpMode = pUserSetInfo->rcpMode;
	if (rcpMode == eRecipeMode::RecipeModeNone)
		return RESULT_GOOD;


	if (nRet == RESULT_GOOD && GetMachineType() == eMachineType::eNSIS)
	{
		// 2022.04.29 
		// 주석추가 김준호
		// MeasureData update 내용 갱신


		g_pDataManager->GetMeasureData(g_pMainFrame->GetMeasureData(), FALSE, TRUE);
		//nRet |= SetMeatureData(viewType);
		nRet |= g_pMainFrame->UpdataMeasureInfoView();

		//DrawCamData(FALSE);
	}

	return nRet;
	
}

UINT32 SetEdit_Shape(IN const double &dRadius, IN const double &dWidth, IN const double &dHeight, IN const CString &strShape)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = g_pDrawManager->SetEdit_Shape(dRadius, dWidth, dHeight, strShape);

	if (nRet == RESULT_GOOD && GetMachineType() == eMachineType::eNSIS)
	{
		nRet |= g_pMainFrame->UpdataMeasureInfoView();
	}

	return nRet;
}


UINT32			SetResize_Apply_SelectOnly(IN const double &dResizePercent)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;


	UINT32 nRet;
	nRet = g_pDrawManager->SetResize_Apply_SelectOnly(dResizePercent);

	if (dResizePercent != 100)
	{
		CString strLog;
		strLog.Format(_T("%s [Scale : %.1f%%]"), APPLY_EDIT_RESIZE_TXT, dResizePercent);
		AddApplicationLog(strLog);
	}

	if (nRet == RESULT_GOOD && GetMachineType() == eMachineType::eNSIS)
	{
		//nRet |= SetMeatureData();
		nRet |= g_pMainFrame->UpdataMeasureInfoView();
	}

	//if ()


	return nRet;
}

UINT32			SetDelete_Apply()
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = g_pDrawManager->SetDelete_Apply();

	UserSetInfo_t* pUserSetInfo = GetUserSetInfo();
	if (pUserSetInfo == nullptr)
		return RESULT_BAD;

	eRecipeMode rcpMode = pUserSetInfo->rcpMode;
	if (rcpMode == eRecipeMode::RecipeModeNone)
		return RESULT_GOOD;


	if (nRet == RESULT_GOOD && GetMachineType() == eMachineType::eNSIS)
	{
		// 2022.04.29 
		// 주석추가 김준호
		// MeasureData update 내용 갱신
		g_pDataManager->GetMeasureData(g_pMainFrame->GetMeasureData(), FALSE, TRUE);
		//nRet |= SetMeatureData();
		nRet |= g_pMainFrame->UpdataMeasureInfoView();

		//Delete 할때는 Measure Info에서 DrawCamData 함수가 호출되지 않아 강제 호출
		DrawCamData(FALSE);
	}

	return nRet;
}

UINT32			SetMove_Apply()
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = g_pDrawManager->SetMove_Apply();

	//if (nRet == RESULT_GOOD && GetMachineType() == eMachineType::eNSIS)
	//{
	//	//nRet |= SetMeatureData();
	//	g_pDataManager->GetMeasureData(g_pMainFrame->GetMeasureData(), FALSE, TRUE);
	//	nRet |= g_pMainFrame->UpdataMeasureInfoView();
	//}

	return nRet;
}


UINT32 GetMoveValue(PointDXY&point)
{
	if (g_pMainView == nullptr) return RESULT_BAD;

	g_pMainView->m_Move.x = point.x;
	g_pMainView->m_Move.y = point.y;


	return RESULT_GOOD;
}

UINT32 Feature_Move_KeyBoard(PointDXY &point, eKeyDir eKeyDirection)
{
	if (g_pDataManager == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = g_pDrawManager->Feature_Move_KeyBoard(point, eKeyDirection);
	nRet |= g_pMainFrame->LoadMove_KeyData();
	

	return nRet;

}

UINT32			SetAdd_Apply(IN const CString &strLayer, vector<CFeature*> &vecFeature)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = g_pDrawManager->SetAdd_Apply(strLayer, vecFeature);	

	// 2022.05.12
	// 주석추가 김준호
	// 쓰는 곳이 없어서 주석처리.
	/*UserSetInfo_t* pUserSetInfo = GetUserSetInfo();
	if (pUserSetInfo == nullptr)
		return RESULT_BAD;

	eRecipeMode rcpMode = pUserSetInfo->rcpMode;
	if (rcpMode == eRecipeMode::RecipeModeNone)
		return RESULT_GOOD;*/


	
	if (nRet == RESULT_GOOD && GetMachineType() == eMachineType::eNSIS)
	{
		// 2022.04.29 
		// 주석추가 김준호
		// MeasureData update 내용 갱신
		

		g_pDataManager->GetMeasureData(g_pMainFrame->GetMeasureData(), FALSE, TRUE);
		//nRet |= SetMeatureData(viewType);
		nRet |= g_pMainFrame->UpdataMeasureInfoView();
	}

	return nRet;
}


UINT32			SetRotate(IN const int&nIndex, IN const BOOL&bMirror)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	return g_pDrawManager->SetRotate(nIndex,bMirror);
}

UINT32			SetRotate_Apply(IN const int&nIndex, IN const BOOL&bMirror)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;
	if (nIndex != -1)
	{
		AddApplicationLog(APPLY_EDIT_ROTATE_TXT);
	}
	if (bMirror == TRUE || bMirror ==FALSE)
	{
		AddApplicationLog(APPLY_EDOT_MIRROR_TXT);
	}

	return g_pDrawManager->SetRotate_Apply(nIndex,bMirror);
}

UINT32			SetTempData_Apply()
{
	return g_pDrawManager->SetTempData_Apply();
}


UINT32			SetCopy(IN const CPointD &point)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	return g_pDrawManager->SetCopy(point);
}

UINT32			SetCopy_Repeat(IN const CPointD &dptPitch, IN const CPoint &ptCount)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	return g_pDrawManager->SetCopy_Repeat(dptPitch, ptCount);
}

UINT32			SetCopy_Apply(IN const CPointD &point)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = g_pDrawManager->SetCopy_Apply(point);

	if (nRet == RESULT_GOOD && GetMachineType() == eMachineType::eNSIS)
	{
		//nRet |= SetMeatureData();
		nRet |= g_pMainFrame->UpdataMeasureInfoView();
	}

	return nRet;
}

UINT32			SetCopy_Repeat_Apply(IN const CPointD &dptPitch, IN const CPoint &ptCount)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = g_pDrawManager->SetCopy_Repeat_Apply(dptPitch, ptCount);

	if (nRet == RESULT_GOOD && GetMachineType() == eMachineType::eNSIS)
	{
		//nRet |= SetMeatureData();
		nRet |= g_pMainFrame->UpdataMeasureInfoView();
	}

	return nRet;
}

UINT32			SetMoveAxis(IN const UINT32 &nEditAxis)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	return  g_pDrawManager->SetEditMoveAxis(nEditAxis);
}

UINT32			SetResize(IN const int &nResizePercent)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	return g_pDrawManager->SetResize(nResizePercent);
}

UINT32	Set_Preview(IN const PointDXY &point, double &fResizePercent, IN const int&nIndex, IN const BOOL&bMirror, IN const CPointD &point2, IN const CPointD &dptPitch,
	IN const CPoint &ptCount, IN  BOOL &bRepeat, IN  BOOL &bCopy, IN const double &dRadius, IN const double &dWidth, IN const double &dHeight, IN const CString&strShape, IN const double dLineWidth)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	return g_pDrawManager->SetPreview(point, fResizePercent, nIndex, bMirror, point2, dptPitch, ptCount, bRepeat, bCopy, dRadius, dWidth, dHeight, strShape, dLineWidth);
}



UINT32			ClearTempPoint()
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	return g_pDrawManager->ClearTempPoint();
}

UINT32			SetPointMode(IN const BOOL &bMode, int nIndex, IN const CString &strName)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	g_nSelectIndex = nIndex;
	return g_pDrawManager->SetPointMode(bMode, nIndex, strName);
}

UINT32			SetAdd(IN const CString &strLayer, vector<CFeature*> &vecFeature)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	return g_pDrawManager->SetAdd(strLayer, vecFeature);
}

UINT32			SaveLayer_Test(IN const double &dResolution, IN const CString &strPathName)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	return g_pDrawManager->SaveLayer_Test(dResolution, strPathName);
}

UINT32 Set_CW()
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;
	return g_pDrawManager->SetCW();

	return RESULT_GOOD;
}

RECTD			GetViewRect()
{
	if (g_pDrawManager == nullptr) return RECTD();

	RECTD drtView;

	return g_pDrawManager->GetViewRect();
}

//Feature Select
UINT32			ResetSelect()
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	UINT32 nRet = g_pDrawManager->ResetSelect();

	if (GetMachineType() == eMachineType::eNSIS)
	{
		//Reset Select
		g_pMainFrame->SetMeasure_ResetSelect();
	}

	DrawCamData(FALSE, FALSE);

	return nRet;
}


UINT32 ResetEdit()
{
	//g_pDrawManager->FeatureSpec.bDragMode = FALSE;
	//g_pDrawManager->FeatureSpec.bWheelMode = FALSE;
	g_pDrawManager->m_bResizeMode = FALSE;
	g_pDrawManager->m_bMoveMode = FALSE;

	SetPointMode(FALSE);
	ClearTempPoint();

	return RESULT_GOOD;
}

UINT32			SetSelect(vector<FEATURE_INFO> &vecFeatureInfo)
{
	UINT32 nRet;
	nRet = g_pDrawManager->SetSelect(vecFeatureInfo);

	if (nRet == RESULT_GOOD)
	{
		if (GetMachineType() == eMachineType::eNSIS)
		{
			//선택된 Feature에 대한 List Update
			g_pMainFrame->SetMeasure_Select_UI();
		}

		nRet = DrawCamData(FALSE);
	}

	return nRet;
}

UINT32			SetUnSelect(vector<FEATURE_INFO> &vecFeatureInfo)
{
	UINT32 nRet;
	nRet = g_pDrawManager->SetUnSelect(vecFeatureInfo);

	if (nRet == RESULT_GOOD)
	{
		nRet = DrawCamData(FALSE);
	}

	return nRet;
}

UINT32			UpdateSelectInfo()
{
	UINT32 nRet;
	nRet = g_pDrawManager->UpdateSelectInfo();

	UpdateStatusBar_FeatureSelect();

	return nRet;
}

UINT32			SetMaskSelectMode(IN const MaskType &eSelectMode)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	return g_pDrawManager->SetMaskSelectMode(eSelectMode);
}

MaskType		GetMaskSelectMode()
{
	if (g_pDrawManager == nullptr) return MaskType::enumType_None;

	return g_pDrawManager->GetMaskSelectMode();
}

UINT32			SetAdd_Apply_Mask(IN const MaskType eMaskType, IN const CString &strLayer, vector<CFeature*> &vecFeature)
{
	if (g_pDrawManager == nullptr) return FALSE;

	return g_pDrawManager->SetAdd_Apply_Mask(eMaskType, strLayer, vecFeature);
}

UINT32			SetDelete_Apply_Mask(IN const MaskType eMaskType)
{
	if (g_pDrawManager == nullptr) return FALSE;

	return g_pDrawManager->SetDelete_Apply_Mask(eMaskType);
}

UINT32			SetResize_Manual_Mask(IN const MaskType eMaskType, IN const float& fPercentX, IN const float& fPercentY)
{
	if (g_pDrawManager == nullptr) return FALSE;

	return g_pDrawManager->SetResize_Manual_Mask(eMaskType, fPercentX, fPercentY);
}

UINT32			SetEditApply_Mask(IN const MaskType eMaskType)
{//Align/Mask 변경된 내용을 최종 적용

	if (g_pDrawManager == nullptr) return FALSE;
		
	UINT32 nRet;
	nRet =  g_pDrawManager->SetApply_Mask(eMaskType);

	if (nRet == RESULT_GOOD && GetMachineType() == eMachineType::eNSIS)
	{
		g_pDataManager->GetMeasureData(g_pMainFrame->GetMeasureData(), FALSE, TRUE);		
		nRet |= g_pMainFrame->UpdataMeasureInfoView();
	}

	

	return RESULT_GOOD;
}


BOOL IsSelect()
{
	return g_pDrawManager->Hit_Test();
}

BOOL IsMove()
{
	return g_pDrawManager->IsMove();
}
BOOL IsResize()
{
	return g_pDrawManager->IsResize();
}

BOOL IsEditBox()
{
	return g_pDrawManager->IsEditBox();
}

//Feature Show/Hide
UINT32			ResetHide()
{
	UINT32 nRet;
	nRet = g_pDrawManager->ResetHide();

	if (nRet == RESULT_GOOD)
	{
		nRet = DrawCamData(FALSE);
	}

	return nRet;
}

UINT32			SetShow(vector<FEATURE_INFO> &vecFeatureInfo)
{
	UINT32 nRet;
	nRet = g_pDrawManager->SetShow(vecFeatureInfo);

	if (nRet == RESULT_GOOD)
	{
		nRet = DrawCamData(FALSE);
	}

	return nRet;
}
UINT32			SetHide(vector<FEATURE_INFO> &vecFeatureInfo)
{
	UINT32 nRet;
	nRet = g_pDrawManager->SetHide(vecFeatureInfo);

	if (nRet == RESULT_GOOD)
	{
		nRet = DrawCamData(FALSE);
	}

	return nRet;
}

UserLayerSet_t*	GetUserLayerSet()
{
	if (g_pMainFrame == nullptr) return nullptr;

	return g_pMainFrame->GetUserLayerSet();

}

UINT32 UpdateStatusBar(IN CPoint point)
{
	UserSetInfo_t* pstUserSetInfo = g_pMainFrame->GetUserSetInfo();
	if (pstUserSetInfo == nullptr)
		return RESULT_BAD;

	CString str = L"";
	PointDXY ptPos(CPointD(0, 0));
	if (g_pDrawManager->ConvertPos(point, ptPos.x, ptPos.y) == RESULT_GOOD)
	{
		double dTemp = 0.;

		if (pstUserSetInfo->bMirror)
		{
			if (pstUserSetInfo->dAngle == 0.0)
			{
				ptPos.x *= -1.0;
			}
			else if (pstUserSetInfo->dAngle == 90.0)
			{
				SWAP(ptPos.x, ptPos.y, dTemp);
				ptPos.x *= -1.0;
				ptPos.y *= -1.0;
			}
			else if (pstUserSetInfo->dAngle == 180.0)
			{
				ptPos.y *= -1.0;
			}
			else if (pstUserSetInfo->dAngle == 270.0)
			{
				SWAP(ptPos.x, ptPos.y, dTemp);
			}
		}
		else
		{
			if (pstUserSetInfo->dAngle == 90.0)
			{
				SWAP(ptPos.x, ptPos.y, dTemp);
				ptPos.y *= -1.0;
			}
			else if (pstUserSetInfo->dAngle == 180.0)
			{
				ptPos.x *= -1.0;
				ptPos.y *= -1.0;
			}
			else if (pstUserSetInfo->dAngle == 270.0)
			{
				SWAP(ptPos.x, ptPos.y, dTemp);
				ptPos.x *= -1.0;
			}
		}

		if (pstUserSetInfo->coordUnit == eCoordinateUnit::eMMToInch)
			str.Format(_T("X = %.6f mil,  Y = %.6f mil"), ptPos.x * 1000., ptPos.y * 1000.);
		else
			str.Format(_T("X = %.6f mm,  Y = %.6f mm"), ptPos.x, ptPos.y);

#ifdef _DEBUG
		float fPanX = 0.f, fPanY = 0.f;
		g_pDrawManager->GetPanPos_ForDebug(fPanX, fPanY);
		CString strDebug;
		strDebug.Format(_T(", PanX : %.3f, PanY : %.3f"), fPanX, fPanY);
		str += strDebug;
#endif

		g_pMainFrame->UpdateStatusBar(COORDINATE_STATUS_POS, str);
	}
	else
		return RESULT_BAD;

	if (pstUserSetInfo->bShowFOV)
	{
		FovInfo_t* pFovInfo = g_pDataManager->GetFovInfo();
		if (pFovInfo && pFovInfo->vcFovData.size() > 0)
		{
			str.Format(_T("[SWATH]\n Swath = %d\n, Width = %d\n, Height = %d\n, OverlapX = %d\n, OverlapY = %d\n"),
				pFovInfo->stBasic.swathNum, pFovInfo->stBasic.fovSize.cx, pFovInfo->stBasic.fovSize.cy, pFovInfo->stBasic.fovOverlap.cx, pFovInfo->stBasic.fovOverlap.cy);

			g_pMainFrame->UpdateStatusBar(FOV_STATUS_POS, str);
		}
	}
	else if (pstUserSetInfo->bShowCell)
	{
		CellInfo_t* pCellInfo = g_pDataManager->GetCellInfo();
		if (pCellInfo && pCellInfo->vcCellData.size() > 0)
		{
			str.Format(_T("[CELL]\n ColNum = %d\n, RowNum = %d\n, Width = %d\n, Height = %d\n, OverlapX = %d\n, OverlapY = %d\n"),
				pCellInfo->stBasic.cellNum.cx, pCellInfo->stBasic.cellNum.cy, pCellInfo->stBasic.cellSize.cx, pCellInfo->stBasic.cellSize.cy, pCellInfo->stBasic.cellOverlap.cx, pCellInfo->stBasic.cellOverlap.cy);

			g_pMainFrame->UpdateStatusBar(FOV_STATUS_POS, str);
		}
	}
	else
		UpdateStatusBar_FeatureSelect();

	return RESULT_GOOD;
}

UINT32			UpdateStatusBar_FeatureSelect()
{

	CString strInfo = g_pDrawManager->GetSelectInfoString();
	g_pMainFrame->UpdateStatusBar(FOV_STATUS_POS, strInfo);

	return RESULT_GOOD;
}

void IsDoneLoadLayers(BOOL b)
{
	if (g_pMainFrame)
		g_pMainFrame->IsDoneLoadLayers(b);
}

INT32 GetCurStepIndex()
{
	return g_pMainFrame->GetCurStepIndex();
}

MeasurePanel_t* GetMeasureData()
{
	if (g_pMainFrame)
		return g_pMainFrame->GetMeasureData();
	else
		return nullptr;
}

void Update_NSIS_MeasureData()
{
	if (g_pDataManager == nullptr) return;
	if (g_pMainFrame == nullptr) return;
	if (GetMachineType() == eMachineType::eNSIS)
	{
		g_pDataManager->GetMeasureData(g_pMainFrame->GetMeasureData(), FALSE, TRUE);
		g_pMainFrame->UpdataMeasureInfoView();
	}
}

UINT32 ChangeLayerInfo(CString strLayer)
{
	if (g_pMainFrame)
		return g_pMainFrame->ChangeLayerInfo(strLayer);
	else
		return RESULT_BAD;
}

UserSetInfo_t* GetUserSetInfo()
{
	if (g_pMainFrame)
		return g_pMainFrame->GetUserSetInfo();
	else
		return nullptr;
}

UINT32			ManualMove(IN const PointDXY &point)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	return  g_pDrawManager->ManualMove(point);
}

UINT32			Draw_ManualMove(UINT nFlags, PointDXY &point)
{
	if (g_pDrawManager == nullptr) return RESULT_BAD;

	UINT32 bRet;
	bRet = g_pDrawManager->Draw_ManualMove(nFlags, point);

	if (g_pDrawManager->GetPointMode() == TRUE)
	{
		g_pDrawManager->SetPointMode(FALSE);

		float fX, fY;
		g_pDrawManager->GetTempPoint(g_nSelectIndex, fX, fY);

		g_pMainFrame->SetSetlectPoint(g_nSelectIndex, fX, fY);
	}

	return bRet;
}

UINT32			Log_FeatureSelect()
{
	SELECT_INFO* pSelInfo = g_pDrawManager->GetSelectInfo();
	if (pSelInfo == nullptr) return RESULT_BAD;

	CString strLog;
	for (auto it : pSelInfo->vecSelectInfoUnit)
	{
		strLog.Format(_T("[SELECT] %s"), it.GetDataString());
		AddApplicationLog(strLog);
	}

	return RESULT_GOOD;
}

CFeatureEdit_AddDlg *GetFeature_AddDlg()
{
	if (g_pMainFrame == nullptr) return nullptr;

	return g_pMainFrame->GetFeature_AddDlg();
}

CAlignMaskEditDlg *GetAlignMaskEditDlg()
{
	if (g_pMainFrame == nullptr) return nullptr;

	return g_pMainFrame->GetAlignMaskEditDlg();
}

BOOL GetFlag_LoadLayers()
{
	if (g_pMainFrame)
		return g_pMainFrame->GetFlag_LoadLayers();

	return FALSE;
}

CString GetImageType()
{
	return g_cSpec.GetImageType();
}

BOOL GetFlag_IsAllLayerLoaded()
{
	if (g_pMainFrame)
		return g_pMainFrame->GetFlag_IsAllLayerLoaded();

	return FALSE;
}


UINT32			SaveCamData()
{
	AddApplicationLog(START_SAVE_LAYER_TXT);

	g_pMainFrame->ShowPopupInitialize(L"Save Layer Data...");

	CString strModelPath = CUtils::GetAppPath() + _T("\\job\\") + GetJobPtr()->GetModelName() + _T("\\");
	UINT32 nRet;
	nRet = IArchive_SaveLayer(GetUserLayerSet(), strModelPath);

	if (nRet == RESULT_GOOD)
	{
		AddApplicationLog(SUCCEED_SAVE_LAYER_TXT);
	}
	else
	{
		AddApplicationLog(FAILED__SAVE_LAYER_TXT);
	}

	g_pMainFrame->HidePopupInitialize();

	return nRet;
}

UINT32			SaveAsCamData(IN CString &strJobName)
{
	if (strJobName == _T("")) return RESULT_BAD;

	AddApplicationLog(START_SAVE_LAYER_TXT + strJobName);

	g_pMainFrame->ShowPopupInitialize(L"Save Layer Data...");

	CString strSrcPath = CUtils::GetAppPath() + _T("\\job\\") + GetJobPtr()->GetModelName() + _T("\\");
	CString strDstPath = CUtils::GetAppPath() + _T("\\job\\") + strJobName + _T("\\");

	UINT32 nRet;
	//기존 JobData Copy
	BOOL bRet = CUtils::CopyFolder(strSrcPath, strDstPath);
	if (bRet == TRUE)
	{
		//
		GetJobPtr()->SetModelName(strJobName);

		nRet = IArchive_SaveLayer(GetUserLayerSet(), strDstPath);
		if (nRet != RESULT_GOOD)
		{
			AddApplicationLog(_T("SaveAs Fail : Write File Fail"));
		}
		else
		{//Good case
			//압축하기
			CString strPath = _T("");
			IArchive_Compress(strPath);

			//Status bar : Model명 갱신
			g_pMainFrame->UpdateStatusBar(MODEL_STATUS_POS, strJobName);

			//Job List 갱신
			g_pMainFrame->Refresh_JobInfoList();

		}
	}
	else
	{
		AddApplicationLog(_T("SaveAs Fail : File Copy Fail"));
	}

	if (nRet == RESULT_GOOD)
	{
		AddApplicationLog(SUCCEED_SAVE_LAYER_TXT);
	}
	else
	{
		AddApplicationLog(FAILED__SAVE_LAYER_TXT);
	}

	g_pMainFrame->HidePopupInitialize();


	return nRet;
}

UINT32			SetAutoAlignCross()
{

	g_pDataManager->SetAlignCrossPoint(g_pMainFrame->GetCurStepIndex());

	return RESULT_GOOD;
}

UINT32			ClearEditTemp()
{
	return RESULT_GOOD;
}

UINT32			ClearLayers()
{
	return IArchive_ClearLayers();
}

PanelInfo_t*	GetProductInfo()
{
	if (g_pDataManager)
		return g_pDataManager->GetProductInfo();
	else
		return nullptr;
}
UINT32			SetDrawOnly_MeasureUnit(BOOL bMeaureUnit)
{
	if (g_pDrawManager)	
		return g_pDrawManager->SetDrawOnly_MeasureUnit(bMeaureUnit);
	else
		return RESULT_BAD;
}

BOOL			IsZoomRectMode()
{
	if (g_pDrawManager)
		return g_pDrawManager->IsZoomRectMode();
	else
		return FALSE;
}

BOOL			IsMeasureMode()
{
	if (g_pDrawManager)
		return g_pDrawManager->IsMeasureMode();
	else
		return FALSE;
}

UINT32			MinimapMouseMove(UINT nFlags, CPoint point)
{
	if (g_pDrawMinimap)
	{
		g_pDrawMinimap->OnMouseMove(nFlags, point);
	}

	return RESULT_GOOD;
}

UINT32			MinimapMouseLButtonDown(UINT nFlags, CPoint point)
{
	if (g_pDrawMinimap)
	{
		g_pDrawMinimap->OnLButtonDown(nFlags, point);
	}

	return RESULT_GOOD;
}

UINT32			MinimapMouseLButtonUp(UINT nFlags, CPoint point)
{
	if (g_pDrawMinimap)
	{
		g_pDrawMinimap->OnLButtonUp(nFlags, point);

		RECTD drtSelect;
		g_pDrawMinimap->GetSelectRect(drtSelect);
		if (drtSelect.IsRectNull() == false)
		{//선택되어 있다면.

			double dWidth = drtSelect.width();
			double dHeight = drtSelect.height();

			float fMargin = 0.05f;//%
			drtSelect.left -= (dWidth* fMargin);
			drtSelect.top -= (dHeight * fMargin);
			drtSelect.right += (dWidth * fMargin);
			drtSelect.bottom += (dHeight * fMargin);

			g_pDrawManager->DrawZoomRect(drtSelect);

			//Measure Info 업데이트
			g_pMainFrame->UpdataMeasureInfoView();
		}
		else
		{//선택 Reset이라면, reset Zoom
			g_pDrawManager->ResetZoom();
		}

		ResetSelect();
	}

	return RESULT_GOOD;
}

UINT32			MinimapMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (g_pDrawMinimap)
	{
		g_pDrawMinimap->OnMouseWheel(nFlags, zDelta, pt);
	}

	return RESULT_GOOD;
}

UINT32			MinimapResetZoom()
{
	if (g_pDrawMinimap)
	{
		g_pDrawMinimap->ResetZoom();
	}

	return RESULT_GOOD;
}

UINT32			Minimap_SetInspect()
{
	if (g_pDrawMinimap)
	{
		g_pDrawMinimap->SetInspect_Unit();
	}

	return RESULT_GOOD;
}

UINT32			Minimap_SetCopy(BOOL &bCopy)
{
	if (g_pDrawMinimap)
	{
		g_pDrawMinimap->SetCopy_Feature(bCopy);
	}

	return RESULT_GOOD;
}

UINT32			Minimap_SetPaste(BOOL &bCopy)
{
	if (g_pDrawMinimap)
	{
		g_pDrawMinimap->SetPaste_Feature();

		g_pDrawManager->ResetSelect();

		g_pDataManager->GetMeasureData(g_pMainFrame->GetMeasureData(), FALSE, TRUE);
		DrawCamData(FALSE, FALSE);

		g_pMainFrame->UpdataMeasureInfoView();

		bCopy = FALSE;
	}

	return RESULT_GOOD;
}

UINT32			Minimap_SetMode(UINT32 nMode)
{
	if (g_pDrawMinimap)
	{
		g_pDrawMinimap->SetMode(nMode);
	}
	else
	{
		return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32			Minimap_SetDelete()
{
	if (g_pDrawMinimap)
	{		
		g_pDrawMinimap->SetDelete_Feature();

		g_pDataManager->GetMeasureData(g_pMainFrame->GetMeasureData(), FALSE, TRUE);
		DrawCamData(FALSE, FALSE);

		g_pMainFrame->UpdataMeasureInfoView();
	}
	else
	{
		return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32			Minimap_GetSelectRect(OUT RECTD &drtSelect)
{
	UINT32 iRet = RESULT_GOOD;
	if (g_pDrawMinimap)
	{
		iRet = g_pDrawMinimap->GetSelectRect(drtSelect);
	}
	else
	{
		return RESULT_BAD;
	}

	return iRet;
}

CString	GetWorkLayer()
{
	return g_pMainFrame->GetWorkLayer();
}

AutoAlign_t* GetAutoAlignSpec()
{		
	return g_cSpec.GetAutoAlignSpec();	
}

UINT32 SetAutoFindAlign()
{	
	// 좌하단 Align x,y MM Value
	D2D1_POINT_2F fptPoint_mm;

	AutoAlign_t* AutoAlignSpec = g_cSpec.GetAutoAlignSpec();

	//fptPoint_mm.x = AutoAlignSpec->vecAutoAlign[0]->dAutoAlignX;
	//fptPoint_mm.y = AutoAlignSpec->vecAutoAlign[0]->dAutoAlignY;

	D2D1_POINT_2F fptReal_mm;
	fptReal_mm.x = 0;
	fptReal_mm.y = 0;

	// Panel Profile Value 	
	int PanelIdx = g_pMainFrame->GetUserLayerSet()->nPanelStepIdx;
	CStep *pStep = g_pJob->m_arrStep.GetAt(PanelIdx);

	D2D_POINT_2F fptMin_mm;
	D2D_POINT_2F fptMax_mm;

	fptMin_mm.x = static_cast<float>(pStep->m_Profile.m_MinMax.left);
	fptMin_mm.y = static_cast<float>(pStep->m_Profile.m_MinMax.bottom);
	fptMax_mm.x  = static_cast<float>(pStep->m_Profile.m_MinMax.right);
	fptMax_mm.y  = static_cast<float>(pStep->m_Profile.m_MinMax.top);

	SELECT_INFO* pSelectInfo;

	int nCount = 4;
	for (int i = 0; i < nCount; i++)
	{
		// INI AutoAlign Flag TRUE  일 경우 INI 값으로 찾기
		if (AutoAlignSpec->bAutoAlign == TRUE)
		{
			// 좌하단
			if (i == 0)
			{
				fptReal_mm.x = fptMin_mm.x + static_cast<float>(AutoAlignSpec->vecAutoAlign[0]->dAutoAlignX);
				fptReal_mm.y = fptMin_mm.y + static_cast<float>(AutoAlignSpec->vecAutoAlign[0]->dAutoAlignY);
			}
			// 좌상단 
			if (i == 1)
			{
				fptReal_mm.x = fptMin_mm.x + static_cast<float>(AutoAlignSpec->vecAutoAlign[1]->dAutoAlignX);
				fptReal_mm.y = fptMax_mm.y - static_cast<float>(AutoAlignSpec->vecAutoAlign[1]->dAutoAlignY);
			}
			// 우상단
			else if (i == 2)
			{
				fptReal_mm.x = fptMax_mm.x - static_cast<float>(AutoAlignSpec->vecAutoAlign[2]->dAutoAlignX);
				fptReal_mm.y = fptMax_mm.y - static_cast<float>(AutoAlignSpec->vecAutoAlign[2]->dAutoAlignY);
			}
			// 우하단
			else if (i == 3)
			{
				fptReal_mm.x = fptMax_mm.x - static_cast<float>(AutoAlignSpec->vecAutoAlign[3]->dAutoAlignX);
				fptReal_mm.y = fptMin_mm.y + static_cast<float>(AutoAlignSpec->vecAutoAlign[3]->dAutoAlignY);
			}
		}
		// FALSE 일 경우 X1, Y1 값으로 찾기
		else
		{
			fptPoint_mm.x = static_cast<float>(AutoAlignSpec->vecAutoAlign[0]->dAutoAlignX);
			fptPoint_mm.y = static_cast<float>(AutoAlignSpec->vecAutoAlign[0]->dAutoAlignY);

			// 좌하단
			if (i == 0)
			{
				fptReal_mm.x = fptMin_mm.x + fptPoint_mm.x;
				fptReal_mm.y = fptMin_mm.y + fptPoint_mm.y;
			}
			// 좌상단 
			if (i == 1)
			{
				fptReal_mm.x = fptMin_mm.x + fptPoint_mm.x;
				fptReal_mm.y = fptMax_mm.y - fptPoint_mm.y;
			}
			// 우상단
			else if (i == 2)
			{
				fptReal_mm.x = fptMax_mm.x - fptPoint_mm.x;
				fptReal_mm.y = fptMax_mm.y - fptPoint_mm.y;
			}
			// 우하단
			else if (i == 3)
			{
				fptReal_mm.x = fptMax_mm.x - fptPoint_mm.x;
				fptReal_mm.y = fptMin_mm.y + fptPoint_mm.y;
			}
		}

		pSelectInfo = g_pDrawManager->SetSelectInfo(fptReal_mm, D2D1::Point2F(-999.f, -999.f));
	
		g_pMainFrame->SetSetlectFeatureInfo(pSelectInfo);

		fptReal_mm.x = 0;
		fptReal_mm.y = 0;
	}

	return RESULT_GOOD;
}

UINT32			SetStartLayer(CString strTemp)
{	
	if (g_pMainFrame)
	{
		g_pMainFrame->m_strStartLayer = strTemp;				
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;	
}

UINT32			SetEndLayer(CString strTemp)
{
	if (g_pMainFrame)
	{
		g_pMainFrame->m_strEndLayer = strTemp;
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32			SetViewerLocation(CString strTemp)
{
	// strTemp 
	// 1. LT : LeftTop
	// 2. LB : LeftBottom
	// 3. RT : RightTop
	// 4. RB : RightBottom

	
	RECTD drtRect;
			
	AutoAlignRect(strTemp, drtRect);

	g_pDrawManager->DrawZoomRect(drtRect);	

	return RESULT_GOOD;
}

UINT32		AutoAlignRect(CString strTemp, RECTD &drtRect)
{
	AutoAlign_t* AutoAlignSpec = g_cSpec.GetAutoAlignSpec();	

	D2D1_POINT_2F Center;
	Center.x = 0;
	Center.y = 0;

	// Panel Profile Value 	
	int PanelIdx = g_pMainFrame->GetUserLayerSet()->nPanelStepIdx;
	CStep *pStep = g_pJob->m_arrStep.GetAt(PanelIdx);

	D2D_POINT_2F fptMin;
	D2D_POINT_2F fptMax;

	fptMin.x = static_cast<float>(pStep->m_Profile.m_MinMax.left);
	fptMin.y = static_cast<float>(pStep->m_Profile.m_MinMax.bottom);
	fptMax.x = static_cast<float>(pStep->m_Profile.m_MinMax.right);
	fptMax.y = static_cast<float>(pStep->m_Profile.m_MinMax.top);

	
	DOUBLE dAngle = g_pMainFrame->GetUserSetAngle();
	
 	if (strTemp.CompareNoCase(_T("LT")) == 0)
	{				
		Center.x = fptMin.x + static_cast<float>(AutoAlignSpec->vecAutoAlign[0]->dAutoAlignX);
		Center.y = fptMax.y - static_cast<float>(AutoAlignSpec->vecAutoAlign[0]->dAutoAlignY);
	}
	else if (strTemp.CompareNoCase(_T("LB")) == 0)
	{		
		Center.x = fptMin.x + static_cast<float>(AutoAlignSpec->vecAutoAlign[1]->dAutoAlignX);
		Center.y = fptMin.y + static_cast<float>(AutoAlignSpec->vecAutoAlign[1]->dAutoAlignY);
	}
	else if (strTemp.CompareNoCase(_T("RT")) == 0)
	{	
		Center.x = fptMax.x - static_cast<float>(AutoAlignSpec->vecAutoAlign[2]->dAutoAlignX);
		Center.y = fptMax.y - static_cast<float>(AutoAlignSpec->vecAutoAlign[2]->dAutoAlignY);
	}
	else if (strTemp.CompareNoCase(_T("RB")) == 0)
	{
		Center.x = fptMax.x - static_cast<float>(AutoAlignSpec->vecAutoAlign[3]->dAutoAlignX);
		Center.y = fptMin.y + static_cast<float>(AutoAlignSpec->vecAutoAlign[3]->dAutoAlignY);
	}

	float half_width = static_cast<float>(AutoAlignSpec->dRectWidth / 2);
	float half_height = static_cast<float>(AutoAlignSpec->dRectHeight / 2);

	drtRect.left = Center.x - half_width;
	drtRect.top = Center.y + half_height;
	drtRect.right = Center.x + half_width;
	drtRect.bottom = Center.y - half_height;

	drtRect.NormalizeRectD();

	return RESULT_GOOD;
}
