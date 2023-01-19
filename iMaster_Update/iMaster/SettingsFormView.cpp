// SettingsFormView.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "SettingsFormView.h"
#include "MainFrm.h"

// CSettingsFormView

IMPLEMENT_DYNCREATE(CSettingsFormView, CFormView)

CSettingsFormView::CSettingsFormView()
	: CFormView(IDD_SETTINGS_DLG)
{
	m_bInit = FALSE;
}

CSettingsFormView::~CSettingsFormView()
{
}

void CSettingsFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSettingsFormView, CFormView)
	ON_WM_SIZE()
	ON_COMMAND(IDC_SAVENAPPLY_BTN, &OnSaveBtnClick)
	ON_COMMAND(IDC_RESET_BTN, &OnResetBtnClick)
END_MESSAGE_MAP()


// CSettingsFormView 진단

#ifdef _DEBUG
void CSettingsFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CSettingsFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CSettingsFormView 메시지 처리기
CSettingsFormView* CSettingsFormView::CreateOne(CWnd* pParent)
{
	CSettingsFormView* pFormView = new CSettingsFormView;
	CCreateContext* pContext = NULL;
	if (!pFormView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 100, 100), pParent, 0, pContext))
	{
		TRACE0("Failed in creating CSettingsFormView");
	}
	return pFormView;
}

void CSettingsFormView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	_UpdateIU_Pos();
}

UINT32 CSettingsFormView::_UpdateIU_Pos()
{
	if (!m_bInit) return RESULT_BAD;

	CRect rectClient;
	GetClientRect(&rectClient);

	m_PropGridCtrl.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height() - 70, NULL);

	m_btnReset.MoveWindow(rectClient.left, rectClient.Height() - 60 + 45 / 4, 120, 30);
	m_btnReset.RedrawWindow();
	m_btnReset.ShowWindow(FALSE);

	m_btnSaveApply.MoveWindow(rectClient.Width() - 120, rectClient.Height() - 60 + 45 / 4, 120, 30);
	m_btnSaveApply.RedrawWindow();

	Invalidate();

	return RESULT_GOOD;
}

void CSettingsFormView::OnInitialUpdate()
{
	if (m_bInit == TRUE) return;

	CFormView::OnInitialUpdate();

	CRect rectDummy(0, 0, 0, 0);
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER;
	if (!m_PropGridCtrl.Create(dwViewStyle, rectDummy, this, IDC_SYSTEMSPEC_GRID))
	{
		TRACE0("Failed to create List control \n");
		return;
	}
	m_PropGridCtrl.EnableHeaderCtrl(FALSE);
	m_PropGridCtrl.EnableDescriptionArea();
	m_PropGridCtrl.SetVSDotNetLook();
	m_PropGridCtrl.MarkModifiedProperties();
	m_PropGridCtrl.SetCustomColors(COLOR_GRAY1, COLOR_GRAY1, COLOR_GRAY1, COLOR_GREEN2, COLOR_GRAY1, COLOR_GRAY1, COLOR_GREEN1);

	m_btnSaveApply.Create(L"Apply && Save", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, IDC_SAVENAPPLY_BTN);
	m_btnReset.Create(L"Reset", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, IDC_RESET_BTN);

	InitializeSpecWindow();
	LoadSpecData();

	m_bInit = TRUE;
	_UpdateIU_Pos();
}

UINT32 CSettingsFormView::InitializeUserSite()
{
	m_pUserSite = new CMFCPropertyGridProperty(STRING_USER_SITE, L"", STRING_USER_SITE_DESC);
	if (m_pUserSite)
	{
		m_pUserSite->AddOption(STRING_USER_SITE_LGIT);
		m_pUserSite->AddOption(STRING_USER_SITE_DAEDUCK);
		m_pUserSite->AddOption(STRING_USER_SITE_SEM);
		m_pUserSite->AllowEdit(FALSE);
		m_pUserSite->Enable(FALSE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeMachineType()
{
	m_pMcType = new CMFCPropertyGridProperty(STRING_MC_TYPE, L"", STRING_MC_TYPE_DESC);
	if (m_pMcType)
	{
		m_pMcType->AddOption(STRING_MC_TYPE_NSIS);
		m_pMcType->AddOption(STRING_MC_TYPE_AOI);
		m_pMcType->AddOption(STRING_MC_TYPE_AVI);
		m_pMcType->AllowEdit(FALSE);
		m_pMcType->Enable(FALSE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializePlatformType()
{
	m_pPlatType = new CMFCPropertyGridProperty(STRING_PLATFORM_TYPE, L"", STRING_PLATFORM_TYPE_DESC);
	if (m_pPlatType)
	{
		m_pPlatType->AddOption(STRING_PLATFORM_TYPE_BGA);
		m_pPlatType->AddOption(STRING_PLATFORM_TYPE_FCB);
		m_pPlatType->AddOption(STRING_PLATFORM_TYPE_HDI);
		m_pPlatType->AllowEdit(FALSE);
		m_pPlatType->Enable(FALSE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeProductType()
{
	m_pProdType = new CMFCPropertyGridProperty(STRING_PRODUCT_TYPE, L"", STRING_PRODUCT_TYPE_DESC);
	if (m_pProdType)
	{
		m_pProdType->AddOption(STRING_PRODUCT_TYPE_STRIP);
		m_pProdType->AddOption(STRING_PRODUCT_TYPE_PANEL);
		m_pProdType->AllowEdit(FALSE);
		m_pProdType->Enable(FALSE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeCamType()
{
	m_pCamType = new CMFCPropertyGridProperty(STRING_CAM_DATA_TYPE, L"", STRING_CAM_DATA_TYPE_DESC);
	if (m_pCamType)
	{
		m_pCamType->AddOption(STRING_CAM_DATA_TYPE_ODB);
		m_pCamType->AddOption(STRING_CAM_DATA_TYPE_GERBER);
		m_pCamType->AddOption(STRING_CAM_DATA_TYPE_DXF);
		m_pCamType->AllowEdit(FALSE);
		m_pCamType->Enable(FALSE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeMasterLevel()
{
	m_pMasterLevel = new CMFCPropertyGridProperty(STRING_MASTER_DATA_LEVEL, L"", STRING_MASTER_DATA_LEVEL_DESC);
	if (m_pMasterLevel)
	{
		m_pMasterLevel->AddOption(STRING_MASTER_DATA_BLOCK_LEVEL);
		m_pMasterLevel->AddOption(STRING_MASTER_DATA_PANEL_LEVEL);
		m_pMasterLevel->AllowEdit(TRUE);
		m_pMasterLevel->Enable(TRUE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeStepRepeatMode()
{
	m_pStepRepeatMode = new CMFCPropertyGridProperty(STRING_STEP_REPEAT_MODE, L"", STRING_STEP_REPEAT_MODE_DESC);
	if (m_pStepRepeatMode)
	{
		m_pStepRepeatMode->AddOption(STRING_STEP_REPEAT_MODE_SHOW_ONE);
		m_pStepRepeatMode->AddOption(STRING_STEP_REPEAT_MODE_SHOW_ALL);
		m_pStepRepeatMode->AllowEdit(FALSE);
		m_pStepRepeatMode->Show(FALSE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeReferenceMode()
{
	m_pReferenceMode = new CMFCPropertyGridProperty(STRING_REFERANCE_REGION, L"", STRING_REFERANCE_REGION_DESC);
	if (m_pReferenceMode)
	{
		m_pReferenceMode->AddOption(STRING_REFERANCE_REGION_FEATURE);
		m_pReferenceMode->AddOption(STRING_REFERANCE_REGION_PROFILE);
		m_pReferenceMode->AllowEdit(FALSE);
		m_pReferenceMode->Enable(FALSE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeDummyMode()
{
	m_pDummyMode = new CMFCPropertyGridProperty(STRING_EXCEPT_DUMMY, L"", STRING_EXCEPT_DUMMY_DESC);
	if (m_pDummyMode)
	{
		m_pDummyMode->AddOption(STRING_EXCEPT_DUMMY_TRUE);
		m_pDummyMode->AddOption(STRING_EXCEPT_DUMMY_FALSE);
		m_pDummyMode->AllowEdit(FALSE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeKeepLocalCopy()
{
	m_pKeepLocalCopy = new CMFCPropertyGridProperty(STRING_KEEP_LOCAL_COPY, L"", STRING_KEEP_LOCAL_DESC);
	if (m_pKeepLocalCopy)
	{
		m_pKeepLocalCopy->AddOption(STRING_KEEP_LOCAL_COPY_TRUE);
		m_pKeepLocalCopy->AddOption(STRING_KEEP_LOCAL_COPY_FALSE);
		m_pKeepLocalCopy->AllowEdit(FALSE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeScanType()
{
	m_pScanType = new CMFCPropertyGridProperty(STRING_SCAN_TYPE, L"", STRING_SCAN_TYPE_DESC);
	if (m_pScanType)
	{
		m_pScanType->AddOption(STRING_SCAN_TYPE_LINE);
		m_pScanType->AddOption(STRING_SCAN_TYPE_AREA);
		m_pScanType->AllowEdit(FALSE);
		m_pScanType->Enable(FALSE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeExceptImageMode()
{
	m_pExceptImageData = new CMFCPropertyGridProperty(STRING_EXCEPT_IMAGE, L"", STRING_EXCEPT_IMAGE_DESC);
	if (m_pExceptImageData)
	{
		m_pExceptImageData->AddOption(STRING_EXCEPT_IMAGE_TRUE);
		m_pExceptImageData->AddOption(STRING_EXCEPT_IMAGE_FALSE);
		m_pExceptImageData->AllowEdit(FALSE);
		m_pExceptImageData->Show(FALSE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeLineArc_SubSave()
{
	m_pLineArcSubSave = new CMFCPropertyGridProperty(STRING_LINEARC_SUBSAVE, L"", STRING_LINEARC_SUBSAVE_DESC);
	if (m_pLineArcSubSave)
	{
		m_pLineArcSubSave->AddOption(STRING_LINEARC_SUBSAVE_TRUE);
		m_pLineArcSubSave->AddOption(STRING_LINEARC_SUBSAVE_FALSE);
		m_pLineArcSubSave->AllowEdit(FALSE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeMaxLayerNum()
{
	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	m_pMaxLayerNum = new CMFCPropertyGridProperty(STRING_MAX_LAYER_NUM, var.lVal, STRING_USER_EDITABLE);
	if (m_pMaxLayerNum)
	{
		m_pMaxLayerNum->AllowEdit(FALSE);
		m_pMaxLayerNum->Enable(FALSE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeBasicSpec()
{
	CMFCPropertyGridProperty* pBasicGroup = new CMFCPropertyGridProperty(STRING_BASIC);
	if (pBasicGroup == nullptr)
		return RESULT_BAD;

	//1. User Site
	if (RESULT_GOOD == InitializeUserSite())
		pBasicGroup->AddSubItem(m_pUserSite);
	else
		return RESULT_BAD;

	//2. Machine Type
	if (RESULT_GOOD == InitializeMachineType())
		pBasicGroup->AddSubItem(m_pMcType);
	else
		return RESULT_BAD;

	//3. Platform Type
	if (RESULT_GOOD == InitializePlatformType())
		pBasicGroup->AddSubItem(m_pPlatType);
	else
		return RESULT_BAD;

	//4. Product Type
	if (RESULT_GOOD == InitializeProductType())
		pBasicGroup->AddSubItem(m_pProdType);
	else
		return RESULT_BAD;

	//5. Cam Type
	if (RESULT_GOOD == InitializeCamType())
		pBasicGroup->AddSubItem(m_pCamType);
	else
		return RESULT_BAD;

	//6. Master Level
	if (RESULT_GOOD == InitializeMasterLevel())
		pBasicGroup->AddSubItem(m_pMasterLevel);
	else
		return RESULT_BAD;

	//7. Step Repeat Mode
	if (RESULT_GOOD == InitializeStepRepeatMode())
		pBasicGroup->AddSubItem(m_pStepRepeatMode);

	//8. Reference Mode
	if (RESULT_GOOD == InitializeReferenceMode())
		pBasicGroup->AddSubItem(m_pReferenceMode);
	else
		return RESULT_BAD;

	//9. Except Dummy
	if (RESULT_GOOD == InitializeDummyMode())
		pBasicGroup->AddSubItem(m_pDummyMode);
	else
		return RESULT_BAD;

	//10. Keep Local Copy
	if (RESULT_GOOD == InitializeKeepLocalCopy())
		pBasicGroup->AddSubItem(m_pKeepLocalCopy);
	else
		return RESULT_BAD;

	//11. Camera Type
	if (RESULT_GOOD == InitializeScanType())
		pBasicGroup->AddSubItem(m_pScanType);
	else
		return RESULT_BAD;

	//12. Max Layer Num
	if (RESULT_GOOD == InitializeMaxLayerNum())
		pBasicGroup->AddSubItem(m_pMaxLayerNum);
	else
		return RESULT_BAD;

	//13. IPU Num
	if (RESULT_GOOD == InitializeIPUNum())
		pBasicGroup->AddSubItem(m_pIPUNum);
	else
		return RESULT_BAD;

	//14. Image Type
	if (RESULT_GOOD == InitializeImageType())
		pBasicGroup->AddSubItem(m_pImageType);
	else
		return RESULT_BAD;

	//15. Except Image Master Data
	if (RESULT_GOOD == InitializeExceptImageMode())
		pBasicGroup->AddSubItem(m_pExceptImageData);
	else
		return RESULT_BAD;

	//16. Line Arc Save By Line Width
	if (RESULT_GOOD == InitializeLineArc_SubSave())
		pBasicGroup->AddSubItem(m_pLineArcSubSave);
	else
		return RESULT_BAD;

	m_PropGridCtrl.AddProperty(pBasicGroup);
	pBasicGroup->Expand(TRUE);
	return RESULT_GOOD;
}

UINT32 CSettingsFormView::InitializeResolution(bool bIsLine)
{
	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	m_pResolution[bIsLine] = new CMFCPropertyGridProperty(STRING_CAM_RESOLUTION, var.dblVal, STRING_USER_EDITABLE);
	if (m_pResolution[bIsLine])
	{
		m_pResolution[bIsLine]->AllowEdit(TRUE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeFrameWidth(bool bIsLine)
{
	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	m_pFrameWidth[bIsLine] = new CMFCPropertyGridProperty(STRING_FRAME_WIDTH, var.lVal, STRING_USER_EDITABLE);
	if (m_pFrameWidth[bIsLine])
	{
		m_pFrameWidth[bIsLine]->AllowEdit(TRUE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeFrameHeight(bool bIsLine)
{
	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	m_pFrameHeight[bIsLine] = new CMFCPropertyGridProperty(STRING_FRAME_HEIGHT, var.lVal, STRING_USER_EDITABLE);
	if (m_pFrameHeight[bIsLine])
	{
		m_pFrameHeight[bIsLine]->AllowEdit(TRUE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeOverlapX(bool bIsLine)
{
	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	m_pOverlapX[bIsLine] = new CMFCPropertyGridProperty(STRING_MIN_OVRLP_X, var.lVal, STRING_USER_EDITABLE);
	if (m_pOverlapX[bIsLine])
	{
		m_pOverlapX[bIsLine]->AllowEdit(TRUE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeOverlapY(bool bIsLine)
{
	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	m_pOverlapY[bIsLine] = new CMFCPropertyGridProperty(STRING_MIN_OVRLP_Y, var.lVal, STRING_USER_EDITABLE);
	if (m_pOverlapY[bIsLine])
	{
		m_pOverlapY[bIsLine]->AllowEdit(TRUE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeMarginX(bool bIsLine)
{
	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	m_pMarginX[bIsLine] = new CMFCPropertyGridProperty(STRING_MIN_MARGIN_X, var.lVal, STRING_USER_EDITABLE);
	if (m_pMarginX[bIsLine])
	{
		m_pMarginX[bIsLine]->AllowEdit(TRUE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeMarginY(bool bIsLine)
{
	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	m_pMarginY[bIsLine] = new CMFCPropertyGridProperty(STRING_MIN_MARGIN_Y, var.lVal, STRING_USER_EDITABLE);
	if(m_pMarginY[bIsLine])
	{
		m_pMarginY[bIsLine]->AllowEdit(TRUE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeCellColNum(bool bIsLine)
{
	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	m_pCellX[bIsLine] = new CMFCPropertyGridProperty(STRING_CELL_COL_NUM, var.lVal, STRING_USER_EDITABLE);
	if (m_pCellX[bIsLine])
	{
		m_pCellX[bIsLine]->AllowEdit(TRUE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeCellRowNum(bool bIsLine)
{
	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	m_pCellY[bIsLine] = new CMFCPropertyGridProperty(STRING_CELL_ROW_NUM, var.lVal, STRING_USER_EDITABLE);
	if (m_pCellY[bIsLine])
	{
		m_pCellY[bIsLine]->AllowEdit(TRUE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeCellOverlapX(bool bIsLine)
{
	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	m_pCellOverlapX[bIsLine] = new CMFCPropertyGridProperty(STRING_CELL_MIN_OVERLAP_X, var.lVal, STRING_USER_EDITABLE);
	if (m_pCellOverlapX[bIsLine])
	{
		m_pCellOverlapX[bIsLine]->AllowEdit(TRUE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeCellOverlapY(bool bIsLine)
{
	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	m_pCellOverlapY[bIsLine] = new CMFCPropertyGridProperty(STRING_CELL_MIN_OVERLAP_Y, var.lVal, STRING_USER_EDITABLE);
	if (m_pCellOverlapY[bIsLine])
	{
		m_pCellOverlapY[bIsLine]->AllowEdit(TRUE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeCameraSpec_for_NSIS(bool bIs3D)
{
	CMFCPropertyGridProperty* pCamera = nullptr;

	UINT32 iModeNum = 0;
	if (bIs3D)
	{
		iModeNum = MAX_3D_MODE_NUM;
		pCamera = new CMFCPropertyGridProperty(STRING_3D_MEASURE);
	}
	else
	{
		iModeNum = MAX_2D_MODE_NUM;
		pCamera = new CMFCPropertyGridProperty(STRING_2D_MEASURE);
	}

	if (pCamera == nullptr)	return RESULT_BAD;

	CString strMode = _T("");
	stGridCamSpec *pSpec = nullptr;
	for (UINT32 iLoop = 0; iLoop < iModeNum; iLoop++)
	{
		strMode.Format(_T("Mode %02d"), iLoop + 1);
		if (bIs3D) pSpec = &m_st3DCamProp[iLoop];
		else pSpec = &m_st2DCamProp[iLoop];

		pSpec->pMode = new CMFCPropertyGridProperty(strMode);
		if (RESULT_GOOD == InitializeMagnification(bIs3D, iLoop))
			pSpec->pMode->AddSubItem(pSpec->pMagnification);
		else
			return RESULT_BAD;

		if (RESULT_GOOD == InitializeZoom(bIs3D, iLoop))
			pSpec->pMode->AddSubItem(pSpec->pZoom);
		else
			return RESULT_BAD;

		if (RESULT_GOOD == InitializeResolution(bIs3D, iLoop))
			pSpec->pMode->AddSubItem(pSpec->pResolution);
		else
			return RESULT_BAD;

		if (RESULT_GOOD == InitializeFovWidth(bIs3D, iLoop))
			pSpec->pMode->AddSubItem(pSpec->pFovWidth);
		else
			return RESULT_BAD;

		if (RESULT_GOOD == InitializeFovHeight(bIs3D, iLoop))
			pSpec->pMode->AddSubItem(pSpec->pFovHeight);
		else
			return RESULT_BAD;

		pSpec->pMode->Expand(FALSE);
		pCamera->AddSubItem(pSpec->pMode);
	}

	m_PropGridCtrl.AddProperty(pCamera);
	pCamera->Expand(FALSE);

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::InitializeCameraOrient()
{
	CMFCPropertyGridProperty* pGroup = new CMFCPropertyGridProperty(STRING_CAMERA_ORIENT);
	if (pGroup == nullptr)
		return RESULT_BAD;
	
	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	//Orient Top
	m_pCamOrient[0] = new CMFCPropertyGridProperty(STRING_CAMERA_ORIENT_TOP, var.lVal, STRING_USER_EDITABLE);
	if (m_pCamOrient[0])
	{
		m_pCamOrient[0]->AllowEdit(TRUE);
		pGroup->AddSubItem(m_pCamOrient[0]);
	}
	else
		return RESULT_BAD;

	//Orient Bottom
	m_pCamOrient[1] = new CMFCPropertyGridProperty(STRING_CAMERA_ORIENT_BOT, var.lVal, STRING_USER_EDITABLE);
	if (m_pCamOrient[1])
	{
		m_pCamOrient[1]->AllowEdit(TRUE);
		pGroup->AddSubItem(m_pCamOrient[1]);
	}
	else
		return RESULT_BAD;

	m_PropGridCtrl.AddProperty(pGroup);
	pGroup->Expand(TRUE);

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::InitializeAlignRatio()
{
	CMFCPropertyGridProperty* pGroup = new CMFCPropertyGridProperty(STRING_ALIGN_RATIO);
	if (pGroup == nullptr)
		return RESULT_BAD;

	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	//Align
	m_pAlignRatio[0] = new CMFCPropertyGridProperty(STRING_ALIGN, var.lVal, STRING_USER_EDITABLE);
	if (m_pAlignRatio[0])
	{
		m_pAlignRatio[0]->AllowEdit(TRUE);
		pGroup->AddSubItem(m_pAlignRatio[0]);
	}
	else
		return RESULT_BAD;

	//Unit Align
	m_pAlignRatio[1] = new CMFCPropertyGridProperty(STRING_UNIT_ALIGN, var.lVal, STRING_USER_EDITABLE);
	if (m_pAlignRatio[1])
	{
		m_pAlignRatio[1]->AllowEdit(TRUE);
		pGroup->AddSubItem(m_pAlignRatio[1]);
	}
	else
		return RESULT_BAD;

	//Zoom Margin
	m_pZoomMarginRatio = new CMFCPropertyGridProperty(STRING_ZOOM_MARGIN, var.dblVal, STRING_USER_EDITABLE);
	if (m_pZoomMarginRatio)
	{
		m_pZoomMarginRatio->AllowEdit(TRUE);
		pGroup->AddSubItem(m_pZoomMarginRatio);
	}
	else
		return RESULT_BAD;
	
	m_PropGridCtrl.AddProperty(pGroup);
	pGroup->Expand(TRUE);

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::InitializeNetwork()
{
	CMFCPropertyGridProperty* pGroup = new CMFCPropertyGridProperty(STRING_NETWORK);
	if (pGroup == nullptr)
		return RESULT_BAD;

	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	//NSIS Port
	m_pNetwork[0] = new CMFCPropertyGridProperty(STRING_NSIS_PORT, var.lVal, STRING_USER_EDITABLE);
	if (m_pNetwork[0])
	{
		m_pNetwork[0]->AllowEdit(TRUE);
		pGroup->AddSubItem(m_pNetwork[0]);
	}
	else
		return RESULT_BAD;

	//Camera Port
	m_pNetwork[1] = new CMFCPropertyGridProperty(STRING_CAMERA_PORT, var.lVal, STRING_USER_EDITABLE);
	if (m_pNetwork[1])
	{
		m_pNetwork[1]->AllowEdit(TRUE);
		pGroup->AddSubItem(m_pNetwork[1]);
	}
	else
		return RESULT_BAD;

	//NSIS IP
	m_pNetwork[2] = new CMFCPropertyGridProperty(STRING_NSIS_IP, L"");
	if (m_pNetwork[2])
	{
		m_pNetwork[2]->AllowEdit(TRUE);
		pGroup->AddSubItem(m_pNetwork[2]);
	}
	else
		return RESULT_BAD;

	//Camera IP
	m_pNetwork[3] = new CMFCPropertyGridProperty(STRING_CAMERA_IP, L"");
	if (m_pNetwork[3])
	{
		m_pNetwork[3]->AllowEdit(TRUE);
		pGroup->AddSubItem(m_pNetwork[3]);
	}
	else
		return RESULT_BAD;

	m_PropGridCtrl.AddProperty(pGroup);
	pGroup->Expand(TRUE);

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::InitializeStaticOptic()
{
	CMFCPropertyGridProperty* pGroup = new CMFCPropertyGridProperty(STRING_STATIC_OPTIC);
	if (pGroup == nullptr)
		return RESULT_BAD;

	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	//Align Mag
	m_pStaticOptic[0] = new CMFCPropertyGridProperty(STRING_ALIGN_MAG, var.dblVal, STRING_USER_EDITABLE);
	if (m_pStaticOptic[0])
	{
		m_pStaticOptic[0]->AllowEdit(FALSE);
		pGroup->AddSubItem(m_pStaticOptic[0]);
	}
	else
		return RESULT_BAD;

	//Align Zoom
	m_pStaticOptic[1] = new CMFCPropertyGridProperty(STRING_ALIGN_ZOOM, var.dblVal, STRING_USER_EDITABLE);
	if (m_pStaticOptic[1])
	{
		m_pStaticOptic[1]->AllowEdit(FALSE);
		pGroup->AddSubItem(m_pStaticOptic[1]);
	}
	else
		return RESULT_BAD;

	//Unit Align Mag
	m_pStaticOptic[2] = new CMFCPropertyGridProperty(STRING_UNIT_ALIGN_MAG, var.dblVal, STRING_USER_EDITABLE);
	if (m_pStaticOptic[2])
	{
		m_pStaticOptic[2]->AllowEdit(FALSE);
		pGroup->AddSubItem(m_pStaticOptic[2]);
	}
	else
		return RESULT_BAD;

	//Unit Align Zoom
	m_pStaticOptic[3] = new CMFCPropertyGridProperty(STRING_UNIT_ALIGN_ZOOM, var.dblVal, STRING_USER_EDITABLE);
	if (m_pStaticOptic[3])
	{
		m_pStaticOptic[3]->AllowEdit(FALSE);
		pGroup->AddSubItem(m_pStaticOptic[3]);
	}
	else
		return RESULT_BAD;

	m_PropGridCtrl.AddProperty(pGroup);
	pGroup->Expand(FALSE);

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::InitializeMagnification(bool bIs3D, UINT32 idx)
{
	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	if (bIs3D)
	{
		m_st3DCamProp[idx].pMagnification = new CMFCPropertyGridProperty(STRING_MAGNIFICATION, var.dblVal, STRING_USER_EDITABLE);
		if (m_st3DCamProp[idx].pMagnification)
		{
			m_st3DCamProp[idx].pMagnification->AllowEdit(TRUE);
			return RESULT_GOOD;
		}
		else
			return RESULT_BAD;
	}
	else
	{
		m_st2DCamProp[idx].pMagnification = new CMFCPropertyGridProperty(STRING_MAGNIFICATION, var.dblVal, STRING_USER_EDITABLE);
		if (m_st2DCamProp[idx].pMagnification)
		{
			m_st2DCamProp[idx].pMagnification->AllowEdit(TRUE);
			return RESULT_GOOD;
		}
		else
			return RESULT_BAD;
	}
}

UINT32 CSettingsFormView::InitializeZoom(bool bIs3D, UINT32 idx)
{
	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	if (bIs3D)
	{
		m_st3DCamProp[idx].pZoom = new CMFCPropertyGridProperty(STRING_ZOOM, var.dblVal, STRING_USER_EDITABLE);
		if (m_st3DCamProp[idx].pZoom)
		{
			m_st3DCamProp[idx].pZoom->AllowEdit(TRUE);
			return RESULT_GOOD;
		}
		else
			return RESULT_BAD;
	}
	else
	{
		m_st2DCamProp[idx].pZoom = new CMFCPropertyGridProperty(STRING_ZOOM, var.dblVal, STRING_USER_EDITABLE);
		if (m_st2DCamProp[idx].pZoom)
		{
			m_st2DCamProp[idx].pZoom->AllowEdit(TRUE);
			return RESULT_GOOD;
		}
		else
			return RESULT_BAD;
	}
}

UINT32 CSettingsFormView::InitializeResolution(bool bIs3D, UINT32 idx)
{
	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	if (bIs3D)
	{
		m_st3DCamProp[idx].pResolution = new CMFCPropertyGridProperty(STRING_RESOLUTION, var.dblVal, STRING_USER_EDITABLE);
		if (m_st3DCamProp[idx].pResolution)
		{
			m_st3DCamProp[idx].pResolution->AllowEdit(TRUE);
			return RESULT_GOOD;
		}
		else
			return RESULT_BAD;
	}
	else
	{
		m_st2DCamProp[idx].pResolution = new CMFCPropertyGridProperty(STRING_RESOLUTION, var.dblVal, STRING_USER_EDITABLE);
		if (m_st2DCamProp[idx].pResolution)
		{
			m_st2DCamProp[idx].pResolution->AllowEdit(TRUE);
			return RESULT_GOOD;
		}
		else
			return RESULT_BAD;
	}
}

UINT32 CSettingsFormView::InitializeFovWidth(bool bIs3D, UINT32 idx)
{
	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	if (bIs3D)
	{
		m_st3DCamProp[idx].pFovWidth = new CMFCPropertyGridProperty(STRING_FOV_WIDTH, var.lVal, STRING_USER_EDITABLE);
		if (m_st3DCamProp[idx].pFovWidth)
		{
			m_st3DCamProp[idx].pFovWidth->AllowEdit(TRUE);
			return RESULT_GOOD;
		}
		else
			return RESULT_BAD;
	}
	else
	{
		m_st2DCamProp[idx].pFovWidth = new CMFCPropertyGridProperty(STRING_FOV_WIDTH, var.lVal, STRING_USER_EDITABLE);
		if (m_st2DCamProp[idx].pFovWidth)
		{
			m_st2DCamProp[idx].pFovWidth->AllowEdit(TRUE);
			return RESULT_GOOD;
		}
		else
			return RESULT_BAD;
	}
}

UINT32 CSettingsFormView::InitializeFovHeight(bool bIs3D, UINT32 idx)
{
	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	if (bIs3D)
	{
		m_st3DCamProp[idx].pFovHeight = new CMFCPropertyGridProperty(STRING_FOV_HEIGHT, var.lVal, STRING_USER_EDITABLE);
		if (m_st3DCamProp[idx].pFovHeight)
		{
			m_st3DCamProp[idx].pFovHeight->AllowEdit(TRUE);
			return RESULT_GOOD;
		}
		else
			return RESULT_BAD;
	}
	else
	{
		m_st2DCamProp[idx].pFovHeight = new CMFCPropertyGridProperty(STRING_FOV_HEIGHT, var.lVal, STRING_USER_EDITABLE);
		if (m_st2DCamProp[idx].pFovHeight)
		{
			m_st2DCamProp[idx].pFovHeight->AllowEdit(TRUE);
			return RESULT_GOOD;
		}
		else
			return RESULT_BAD;
	}
}

UINT32 CSettingsFormView::InitializeCameraSpec(bool bIsLine)
{
	CMFCPropertyGridProperty* pCamera = nullptr;
	
	if (bIsLine) 
		pCamera = new CMFCPropertyGridProperty(STRING_LINE_CAMERA);
	else
		pCamera = new CMFCPropertyGridProperty(STRING_AREA_CAMERA);

	if (pCamera == nullptr)
		return RESULT_BAD;

	//1. Resolution
	if (RESULT_GOOD == InitializeResolution(bIsLine))
		pCamera->AddSubItem(m_pResolution[bIsLine]);
	else
		return RESULT_BAD;	
	
	//2. FrameWidth
	if (RESULT_GOOD == InitializeFrameWidth(bIsLine))
		pCamera->AddSubItem(m_pFrameWidth[bIsLine]);	
	else
		return RESULT_BAD;

	//3. FrameHeight
	if (RESULT_GOOD == InitializeFrameHeight(bIsLine))
		pCamera->AddSubItem(m_pFrameHeight[bIsLine]);	
	else
		return RESULT_BAD;

	//4. OverlapX 
	if (RESULT_GOOD == InitializeOverlapX(bIsLine))
	{
	//	pCamera->AddSubItem(m_pOverlapX[bIsLine]);
	}
	else
		return RESULT_BAD;

	//5. OverlapY 
	if (RESULT_GOOD == InitializeOverlapY(bIsLine))
	{
	//	pCamera->AddSubItem(m_pOverlapY[bIsLine]);
	}
	else
		return RESULT_BAD;

	//6. MarginX 
	if (RESULT_GOOD == InitializeMarginX(bIsLine))
	{
	//	pCamera->AddSubItem(m_pMarginX[bIsLine]);
	}
	else
		return RESULT_BAD;

	//7. MarginY 
	if (RESULT_GOOD == InitializeMarginY(bIsLine))
	{
	//	pCamera->AddSubItem(m_pMarginY[bIsLine]);
	}
	else
		return RESULT_BAD;

	//8. Dummy MarginX
	if (RESULT_GOOD == InitializeDummyMarginX(bIsLine))
		pCamera->AddSubItem(m_pDummyMarginX[bIsLine]);
	else
		return RESULT_BAD;

	//9. Dummy MarginY
	if (RESULT_GOOD == InitializeDummyMarginY(bIsLine))
		pCamera->AddSubItem(m_pDummyMarginY[bIsLine]);
	else
		return RESULT_BAD;

	//10. Cell Col Num
	if (RESULT_GOOD == InitializeCellColNum(bIsLine))
		pCamera->AddSubItem(m_pCellX[bIsLine]);
	else
		return RESULT_BAD;

	//11. Cell Row Num
	if (RESULT_GOOD == InitializeCellRowNum(bIsLine))
		pCamera->AddSubItem(m_pCellY[bIsLine]);
	else
		return RESULT_BAD;

	//12. Cell OverlapX
	if (RESULT_GOOD == InitializeCellOverlapX(bIsLine))
		pCamera->AddSubItem(m_pCellOverlapX[bIsLine]);
	else
		return RESULT_BAD;

	//13. Cell OverlapY
	if (RESULT_GOOD == InitializeCellOverlapY(bIsLine))
		pCamera->AddSubItem(m_pCellOverlapY[bIsLine]);
	else
		return RESULT_BAD;

	m_PropGridCtrl.AddProperty(pCamera);

	if (bIsLine)	pCamera->Expand(TRUE);	
	else pCamera->Expand(FALSE);

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::InitializePathSpec()
{
	CMFCPropertyGridProperty* pPathGroup = new CMFCPropertyGridProperty(STRING_PATH);
	if (pPathGroup == nullptr)
		return RESULT_BAD;

	m_pCamPath = new CMFCPropertyGridFileProperty(STRING_CAM_DATA_PATH, L"");
	if (m_pCamPath == nullptr)
		return RESULT_BAD;

	m_pCamPath->AllowEdit(FALSE);
	pPathGroup->AddSubItem(m_pCamPath);

	m_pMasterPath = new CMFCPropertyGridFileProperty(STRING_MASTER_DATA_PATH, L"");
	if (m_pMasterPath == nullptr)
		return RESULT_BAD;

	m_pMasterPath->AllowEdit(FALSE);
	pPathGroup->AddSubItem(m_pMasterPath);

	m_pLogPath = new CMFCPropertyGridFileProperty(STRING_LOG_PATH, L"");
	if (m_pLogPath == nullptr)
		return RESULT_BAD;

	m_pLogPath->AllowEdit(FALSE);
	pPathGroup->AddSubItem(m_pLogPath);

	m_PropGridCtrl.AddProperty(pPathGroup);
	pPathGroup->Expand(TRUE);

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::InitializeStepSpec()
{
	CMFCPropertyGridProperty* pStepGroup = new CMFCPropertyGridProperty(STRING_STEP_DEFINE);
	if (pStepGroup == nullptr)
		return RESULT_BAD;

	m_pPnlStepName = new CMFCPropertyGridProperty(STRING_PNL_STEP, L"");
	if (m_pPnlStepName == nullptr)
		return RESULT_BAD;

	m_pPnlStepName->AllowEdit(TRUE);
	pStepGroup->AddSubItem(m_pPnlStepName);

	m_pStripStepName = new CMFCPropertyGridProperty(STRING_STRIP_STEP, L"");
	if (m_pStripStepName == nullptr)
		return RESULT_BAD;

	m_pStripStepName->AllowEdit(TRUE);
	pStepGroup->AddSubItem(m_pStripStepName);

	m_pUnitStepName = new CMFCPropertyGridProperty(STRING_UNIT_STEP, L"");
	if (m_pUnitStepName == nullptr)
		return RESULT_BAD;

	m_pUnitStepName->AllowEdit(TRUE);
	pStepGroup->AddSubItem(m_pUnitStepName);

	m_PropGridCtrl.AddProperty(pStepGroup);
	pStepGroup->Expand(TRUE);

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::InitializeLayerSpec()
{
	CMFCPropertyGridProperty* pLayerDefine = new CMFCPropertyGridProperty(STRING_LAYER_DEFINE);
	if (pLayerDefine == nullptr)
		return RESULT_BAD;

	UINT32 iLayerCnt = static_cast<UINT32>(eRefLayer::EN_MAX_LAYER_NUM);

	for (UINT32 i = 0; i < iLayerCnt; i++)
	{
		CMFCPropertyGridProperty* pLayer = new CMFCPropertyGridProperty(GetLayerDefine(i));

		m_pFrontLayerName[i] = new CMFCPropertyGridProperty(L"Front Name", L"");
		m_pFrontLayerName[i]->AllowEdit(TRUE);
		pLayer->AddSubItem(m_pFrontLayerName[i]);

		m_pBackLayerName[i] = new CMFCPropertyGridProperty(L"Back Name", L"");
		m_pBackLayerName[i]->AllowEdit(TRUE);
		pLayer->AddSubItem(m_pBackLayerName[i]);

		m_pLayerColor[i] = new CMFCPropertyGridColorProperty(L"Color", RGB(255, 255, 255), NULL, _T("Specifies the default color"));
		m_pLayerColor[i]->EnableOtherButton(_T("OTHER"));
		m_pLayerColor[i]->EnableAutomaticButton(_T("DEFAULT"), ::GetSysColor(COLOR_3DFACE));
		pLayer->AddSubItem(m_pLayerColor[i]);
		
		pLayerDefine->AddSubItem(pLayer);
	}

	m_PropGridCtrl.AddProperty(pLayerDefine);
	pLayerDefine->Expand(FALSE);

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::InitializeSpecWindow()
{
	SystemSpec_t* pSysSpec = GetSystemSpec();
	if (pSysSpec == nullptr)
		return RESULT_BAD;

	if (RESULT_GOOD != InitializeBasicSpec())
		return RESULT_BAD;

	if (eMachineType::eNSIS == pSysSpec->sysBasic.McType)
	{
		if (eUserSite::eSEM == pSysSpec->sysBasic.UserSite)
		{
			if (RESULT_GOOD != InitializeAutoAlignSpec())
				return RESULT_BAD;
		}

		if (RESULT_GOOD != InitializeCameraSpec_for_NSIS(true))
			return RESULT_BAD;

		if (RESULT_GOOD != InitializeCameraSpec_for_NSIS(false))
			return RESULT_BAD;

		if (RESULT_GOOD != InitializeCameraOrient())
			return RESULT_BAD;

		if (RESULT_GOOD != InitializeAlignRatio())
			return RESULT_BAD;

		if (RESULT_GOOD != InitializeNetwork())
			return RESULT_BAD;

		if (RESULT_GOOD != InitializeStaticOptic())
			return RESULT_BAD;

		if (RESULT_GOOD != InitializeMeasureRatioSpec())
			return RESULT_BAD;

		if (RESULT_GOOD != InitializeSaveMeasurePointSpec())
			return RESULT_BAD;
	}
	else
	{
		if (RESULT_GOOD != InitializeCameraSpec(true))
			return RESULT_BAD;

		if (RESULT_GOOD != InitializeCameraSpec(false))
			return RESULT_BAD;
	}

	if (RESULT_GOOD != InitializePathSpec())
		return RESULT_BAD;

	if (RESULT_GOOD != InitializeStepSpec())
		return RESULT_BAD;

	if (RESULT_GOOD != InitializeLayerSpec())
		return RESULT_BAD;

	return RESULT_GOOD;
}

eUserSite CSettingsFormView::GetUserSite()
{
	eUserSite eSite = eUserSite::UserSiteNone;
	CString str = m_pUserSite->GetValue().bstrVal;

	if (str == STRING_USER_SITE_LGIT) {
		eSite = eUserSite::eLGIT;
	}
	else if (str == STRING_USER_SITE_DAEDUCK) {
		eSite = eUserSite::eDAEDUCK;
	}
	else if (str == STRING_USER_SITE_SEM) {
		eSite = eUserSite::eSEM;
	}
	return eSite;
}

void CSettingsFormView::SetUserSite(INT32 iData)
{
	switch (iData)
	{
	case 0:
		m_pUserSite->SetValue((_variant_t)STRING_USER_SITE_LGIT);
		break;
	case 1:
		m_pUserSite->SetValue((_variant_t)STRING_USER_SITE_DAEDUCK);
		break;
	case 2:
		m_pUserSite->SetValue((_variant_t)STRING_USER_SITE_SEM);
		break;
	default:
		break;
	}
}

eMachineType CSettingsFormView::GetMachineType()
{
	eMachineType eType = eMachineType::MachineTypeNone;
	CString str = m_pMcType->GetValue().bstrVal;

	if (str == STRING_MC_TYPE_NSIS) {
		eType = eMachineType::eNSIS;
	}
	if (str == STRING_MC_TYPE_AOI) {
		eType = eMachineType::eAOI;
	}
	if (str == STRING_MC_TYPE_AVI) {
		eType = eMachineType::eAVI;
	}
	return eType;
}

void CSettingsFormView::SetMachineType(INT32 iData)
{
	switch (iData)
	{
	case 0:
		m_pMcType->SetValue((_variant_t)STRING_MC_TYPE_NSIS);
		break;
	case 1:
		m_pMcType->SetValue((_variant_t)STRING_MC_TYPE_AOI);
		break;
	case 2:
		m_pMcType->SetValue((_variant_t)STRING_MC_TYPE_AVI);
		break;
	default:
		break;
	}
}

ePlatFormType CSettingsFormView::GetPlatformType()
{
	ePlatFormType ePtType = ePlatFormType::PlatFormTypeNone;
	CString str = m_pPlatType->GetValue().bstrVal;
	if (str == STRING_PLATFORM_TYPE_BGA) {
		ePtType = ePlatFormType::eBGA;
	}
	if (str == STRING_PLATFORM_TYPE_FCB) {
		ePtType = ePlatFormType::eFCB;
	}
	if (str == STRING_PLATFORM_TYPE_HDI) {
		ePtType = ePlatFormType::eHDI;
	}
	return ePtType;
}

void CSettingsFormView::SetPlatformType(INT32 iData)
{
	switch (iData)
	{
	case 0:
		m_pPlatType->SetValue((_variant_t)STRING_PLATFORM_TYPE_BGA);
		break;
	case 1:
		m_pPlatType->SetValue((_variant_t)STRING_PLATFORM_TYPE_FCB);
		break;
	case 2:
		m_pPlatType->SetValue((_variant_t)STRING_PLATFORM_TYPE_HDI);
		break;
	default:
		break;
	}
}

eProductType CSettingsFormView::GetProductType()
{
	eProductType ePdType = eProductType::ProductTypeNone;
	CString str = m_pProdType->GetValue().bstrVal;
	if (str == STRING_PRODUCT_TYPE_STRIP) {
		ePdType = eProductType::eStrip;
	}
	if (str == STRING_PRODUCT_TYPE_PANEL) {
		ePdType = eProductType::ePanel;
	}
	return ePdType;
}

void CSettingsFormView::SetProductType(INT32 iData)
{
	switch (iData)
	{
	case 0:
		m_pProdType->SetValue((_variant_t)STRING_PRODUCT_TYPE_STRIP);
		break;
	case 1:
		m_pProdType->SetValue((_variant_t)STRING_PRODUCT_TYPE_PANEL);
		break;
	default:
		break;
	}
}

eCamType CSettingsFormView::GetCamType()
{
	eCamType eCamdataType = eCamType::CamTypeNone;
	CString str = m_pCamType->GetValue().bstrVal;
	if (str == STRING_CAM_DATA_TYPE_ODB) {
		eCamdataType = eCamType::eODB;
	}
	if (str == STRING_CAM_DATA_TYPE_GERBER) {
		eCamdataType = eCamType::eGerber;
	}
	if (str == STRING_CAM_DATA_TYPE_DXF) {
		eCamdataType = eCamType::eDxf;
	}
	return eCamdataType;
}

void CSettingsFormView::SetCamType(INT32 iData)
{
	switch (iData)
	{
	case 0:
		m_pCamType->SetValue((_variant_t)STRING_CAM_DATA_TYPE_ODB);
		break;
	case 1:
		m_pCamType->SetValue((_variant_t)STRING_CAM_DATA_TYPE_GERBER);
		break;
	case 2:
		m_pCamType->SetValue((_variant_t)STRING_CAM_DATA_TYPE_DXF);
		break;
	default:
		break;
	}
}

eMasterLevel CSettingsFormView::GetMasterLevel()
{
	eMasterLevel eMasterLv = eMasterLevel::MasterLevelNone;
	CString str = m_pMasterLevel->GetValue().bstrVal;
	if (str == STRING_MASTER_DATA_BLOCK_LEVEL) {
		eMasterLv = eMasterLevel::eBlockLv;
	}
	if (str == STRING_MASTER_DATA_PANEL_LEVEL) {
		eMasterLv = eMasterLevel::ePanelLv;
	}
	return eMasterLv;
}

void CSettingsFormView::SetMasterLevel(INT32 iData)
{
	switch (iData)
	{
	case 0:
		m_pMasterLevel->SetValue((_variant_t)STRING_MASTER_DATA_BLOCK_LEVEL);
		break;
	case 1:
		m_pMasterLevel->SetValue((_variant_t)STRING_MASTER_DATA_PANEL_LEVEL);
		break;
	default:
		break;
	}
}

eShowStepRepeatType CSettingsFormView::GetStepRepeatMode()
{
	eShowStepRepeatType eStpRepMode = eShowStepRepeatType::ShowStepRepeatTypeNone;
	CString str = m_pStepRepeatMode->GetValue().bstrVal;
	if (str == STRING_STEP_REPEAT_MODE_SHOW_ONE) {
		eStpRepMode = eShowStepRepeatType::eShowJustOne;
	}
	if (str == STRING_STEP_REPEAT_MODE_SHOW_ALL) {
		eStpRepMode = eShowStepRepeatType::eShowAll;
	}
	return eStpRepMode;
}

void CSettingsFormView::SetStepRepeatMode(INT32 iData)
{
	switch (iData)
	{
	case 0:
		m_pStepRepeatMode->SetValue((_variant_t)STRING_STEP_REPEAT_MODE_SHOW_ONE);
		break;
	case 1:
		m_pStepRepeatMode->SetValue((_variant_t)STRING_STEP_REPEAT_MODE_SHOW_ALL);
		break;
	default:
		break;
	}
}

eRefRegionMode CSettingsFormView::GetReferenceRegion()
{
	eRefRegionMode eRefRegion = eRefRegionMode::RefRegionModeNone;
	CString str = m_pReferenceMode->GetValue().bstrVal;
	if (str == STRING_REFERANCE_REGION_FEATURE) {
		eRefRegion = eRefRegionMode::eFeature;
	}
	if (str == STRING_REFERANCE_REGION_PROFILE) {
		eRefRegion = eRefRegionMode::eProfile;
	}
	return eRefRegion;
}

void CSettingsFormView::SetReferenceRegion(INT32 iData)
{
	switch (iData)
	{
	case 0:
		m_pReferenceMode->SetValue((_variant_t)STRING_REFERANCE_REGION_FEATURE);
		break;
	case 1:
		m_pReferenceMode->SetValue((_variant_t)STRING_REFERANCE_REGION_PROFILE);
		break;
	default:
		break;
	}
}

bool CSettingsFormView::GetExceptDummy()
{
	CString str = m_pDummyMode->GetValue().bstrVal;
	if (str == STRING_EXCEPT_DUMMY_TRUE) {
		return true;
	}
	else if (str == STRING_EXCEPT_DUMMY_FALSE) {
		return false;
	}
	return false;
}

void CSettingsFormView::SetExceptDummy(INT32 iData)
{
	switch (iData)
	{
	case 0:
		m_pDummyMode->SetValue((_variant_t)STRING_EXCEPT_DUMMY_FALSE);
		break;
	case 1:
		m_pDummyMode->SetValue((_variant_t)STRING_EXCEPT_DUMMY_TRUE);
		break;
	default:
		break;
	}
}

bool CSettingsFormView::GetKeepLocalCopy()
{
	CString str = m_pKeepLocalCopy->GetValue().bstrVal;
	if (str == STRING_KEEP_LOCAL_COPY_TRUE) {
		return true;
	}
	else if (str == STRING_KEEP_LOCAL_COPY_FALSE) {
		return false;
	}
	else
		return false;
}

void CSettingsFormView::SetKeepLocalCopy(INT32 iData)
{
	switch (iData)
	{
	case 0:
		m_pKeepLocalCopy->SetValue((_variant_t)STRING_KEEP_LOCAL_COPY_FALSE);
		break;
	case 1:
		m_pKeepLocalCopy->SetValue((_variant_t)STRING_KEEP_LOCAL_COPY_TRUE);
		break;
	default:
		break;
	}
}

eScanType CSettingsFormView::GetScanType()
{
	eScanType eSType = eScanType::ScanTypeNone;
	CString str = m_pScanType->GetValue().bstrVal;
	if (str == STRING_SCAN_TYPE_LINE) {
		eSType = eScanType::eLineScan;
	}
	if (str == STRING_SCAN_TYPE_AREA) {
		eSType = eScanType::eAreaScan;
	}
	return eSType;
}

void CSettingsFormView::SetScanType(INT32 iData)
{
	switch (iData)
	{
	case 0:
		m_pScanType->SetValue((_variant_t)STRING_SCAN_TYPE_LINE);
		break;
	case 1:
		m_pScanType->SetValue((_variant_t)STRING_SCAN_TYPE_AREA);
		break;
	default:
		break;
	}
}

bool CSettingsFormView::GetExceptImageData()
{
	CString str = m_pExceptImageData->GetValue().bstrVal;
	if (str == STRING_EXCEPT_IMAGE_TRUE) {
		return true;
	}
	else if (str == STRING_EXCEPT_IMAGE_FALSE) {
		return false;
	}
	return false;
}


void CSettingsFormView::SetExceptImageData(INT32 iData)
{
	switch (iData)
	{
	case 0:
		m_pExceptImageData->SetValue((_variant_t)STRING_EXCEPT_IMAGE_FALSE);
		break;
	default:
	case 1:
		m_pExceptImageData->SetValue((_variant_t)STRING_EXCEPT_IMAGE_TRUE);
		break;
	}
}

void CSettingsFormView::SetLineArcSubSave(INT32 iData)
{
	switch (iData)
	{
	case 0:
		m_pLineArcSubSave->SetValue((_variant_t)STRING_LINEARC_SUBSAVE_FALSE);
		break;
	default:
	case 1:
		m_pLineArcSubSave->SetValue((_variant_t)STRING_LINEARC_SUBSAVE_TRUE);
		break;
	}
}

bool CSettingsFormView::GetLineArc_SubSave()
{
	CString str = m_pLineArcSubSave->GetValue().bstrVal;
	if (str == STRING_LINEARC_SUBSAVE_TRUE) {
		return true;
	}
	else if (str == STRING_LINEARC_SUBSAVE_FALSE) {
		return false;
	}
	return false;
}

void CSettingsFormView::SetMaxLayerNum(LONG iData)
{
	m_pMaxLayerNum->SetValue((_variant_t)iData);
}

UINT32 CSettingsFormView::GetMaxLayerNum()
{
	return static_cast<UINT32>(m_pMaxLayerNum->GetValue().lVal);
}

double CSettingsFormView::GetCamResolution(bool bIsLine)
{
	return (m_pResolution[bIsLine]->GetValue().dblVal);
}

void CSettingsFormView::SetCamResolution(bool bIsLine, double dData)
{
	m_pResolution[bIsLine]->SetValue((_variant_t)dData);
}

UINT32 CSettingsFormView::GetCamFrameWidth(bool bIsLine)
{
	return static_cast<UINT32>(m_pFrameWidth[bIsLine]->GetValue().lVal);
}

void CSettingsFormView::SetCamFrameWidth(bool bIsLine, LONG iData)
{
	m_pFrameWidth[bIsLine]->SetValue((_variant_t)iData);
}

UINT32 CSettingsFormView::GetCamFrameHeight(bool bIsLine)
{
	return static_cast<UINT32>(m_pFrameHeight[bIsLine]->GetValue().lVal);
}

void CSettingsFormView::SetCamFrameHeight(bool bIsLine, LONG iData)
{
	m_pFrameHeight[bIsLine]->SetValue((_variant_t)iData);
}

UINT32 CSettingsFormView::GetCamMinOverlapX(bool bIsLine)
{
	return static_cast<UINT32>(m_pOverlapX[bIsLine]->GetValue().lVal);
}

void CSettingsFormView::SetCamMinOverlapX(bool bIsLine, LONG iData)
{
	m_pOverlapX[bIsLine]->SetValue((_variant_t)iData);
}

UINT32 CSettingsFormView::GetCamMinOverlapY(bool bIsLine)
{
	return static_cast<UINT32>(m_pOverlapY[bIsLine]->GetValue().lVal);
}

void CSettingsFormView::SetCamMinOverlapY(bool bIsLine, LONG iData)
{
	m_pOverlapY[bIsLine]->SetValue((_variant_t)iData);
}

UINT32 CSettingsFormView::GetCamMinMarginX(bool bIsLine)
{
	return static_cast<UINT32>(m_pMarginX[bIsLine]->GetValue().lVal);
}

void CSettingsFormView::SetCamMinMarginX(bool bIsLine, LONG iData)
{
	m_pMarginX[bIsLine]->SetValue((_variant_t)iData);
}

UINT32 CSettingsFormView::GetCamMinMarginY(bool bIsLine)
{
	return static_cast<UINT32>(m_pMarginY[bIsLine]->GetValue().lVal);
}

void CSettingsFormView::SetCamMinMarginY(bool bIsLine, LONG iData)
{
	m_pMarginY[bIsLine]->SetValue((_variant_t)iData);
}

UINT32 CSettingsFormView::GetCamCellColNum(bool bIsLine)
{
	return static_cast<UINT32>(m_pCellX[bIsLine]->GetValue().lVal);
}

void CSettingsFormView::SetCamCellColNum(bool bIsLine, LONG iData)
{
	m_pCellX[bIsLine]->SetValue((_variant_t)iData);
}

UINT32 CSettingsFormView::GetCamCellRowNum(bool bIsLine)
{
	return static_cast<UINT32>(m_pCellY[bIsLine]->GetValue().lVal);
}

void CSettingsFormView::SetCamCellRowNum(bool bIsLine, LONG iData)
{
	m_pCellY[bIsLine]->SetValue((_variant_t)iData);
}

UINT32 CSettingsFormView::GetCamCellOverlapX(bool bIsLine)
{
	return static_cast<UINT32>(m_pCellOverlapX[bIsLine]->GetValue().lVal);
}

void CSettingsFormView::SetCamCellOverlapX(bool bIsLine, LONG iData)
{
	m_pCellOverlapX[bIsLine]->SetValue((_variant_t)iData);
}

UINT32 CSettingsFormView::GetCamCellOverlapY(bool bIsLine)
{
	return static_cast<UINT32>(m_pCellOverlapY[bIsLine]->GetValue().lVal);
}

void CSettingsFormView::SetCamCellOverlapY(bool bIsLine, LONG iData)
{
	m_pCellOverlapY[bIsLine]->SetValue((_variant_t)iData);
}

CString CSettingsFormView::GetCamDataPath()
{
	return(m_pCamPath->GetValue().bstrVal);
}

void CSettingsFormView::SetCamDataPath(CString strData)
{
	m_pCamPath->SetValue((_variant_t)strData);
}

CString CSettingsFormView::GetMasterDataPath()
{
	return(m_pMasterPath->GetValue().bstrVal);
}

void CSettingsFormView::SetMasterDataPath(CString strData)
{
	m_pMasterPath->SetValue((_variant_t)strData);
}

CString CSettingsFormView::GetLogDataPath()
{
	return(m_pLogPath->GetValue().bstrVal);
}

void CSettingsFormView::SetLogDataPath(CString strData)
{
	m_pLogPath->SetValue((_variant_t)strData);
}

CString CSettingsFormView::GetPanelStepName()
{
	return(m_pPnlStepName->GetValue().bstrVal);
}

void CSettingsFormView::SetPanelStepName(CString strData)
{
	m_pPnlStepName->SetValue((_variant_t)strData);
}

CString CSettingsFormView::GetStripStepName()
{
	return(m_pStripStepName->GetValue().bstrVal);
}

void CSettingsFormView::SetStripStepName(CString strData)
{
	m_pStripStepName->SetValue((_variant_t)strData);
}

CString CSettingsFormView::GetUnitStepName()
{
	return(m_pUnitStepName->GetValue().bstrVal);
}

void CSettingsFormView::SetUnitStepName(CString strData)
{
	m_pUnitStepName->SetValue((_variant_t)strData);
}

COLORREF CSettingsFormView::GetLayerColor(UINT32 index)
{
	return m_pLayerColor[index]->GetColor();
}

void CSettingsFormView::SetLayerColor(UINT32 index, COLORREF color)
{
	m_pLayerColor[index]->SetColor(color);
}

CString CSettingsFormView::GetFrontLayerName(UINT32 index)
{
	return(m_pFrontLayerName[index]->GetValue().bstrVal);
}

void CSettingsFormView::SetFrontLayerName(UINT32 index, CString strData)
{
	m_pFrontLayerName[index]->SetValue((_variant_t)strData);
}

CString CSettingsFormView::GetBackLayerName(UINT32 index)
{
	return(m_pBackLayerName[index]->GetValue().bstrVal);
}

void CSettingsFormView::SetBackLayerName(UINT32 index, CString strData)
{
	m_pBackLayerName[index]->SetValue((_variant_t)strData);
}

UINT32 CSettingsFormView::LoadSpecData()
{
	SystemSpec_t* pSysSpec = GetSystemSpec();
	if (pSysSpec == nullptr)
		return RESULT_BAD;

	if (RESULT_GOOD != SetBasicSpec(&pSysSpec->sysBasic))
		return RESULT_BAD;
		
	if (eMachineType::eNSIS == pSysSpec->sysBasic.McType)
	{
		if (eUserSite::eSEM == pSysSpec->sysBasic.UserSite)
		{
			if (RESULT_GOOD != SetAutoAlignSpec(&pSysSpec->sysAutoAlign))
				return RESULT_BAD;
		}

		if (RESULT_GOOD != SetCameraSpec_for_NSIS(true, pSysSpec->sysNSIS.sys3DCamera))
			return RESULT_BAD;

		if (RESULT_GOOD != SetCameraSpec_for_NSIS(false, pSysSpec->sysNSIS.sys2DCamera))
			return RESULT_BAD;

		if (RESULT_GOOD != SetCameraOrient(&pSysSpec->sysNSIS))
			return RESULT_BAD;

		if (RESULT_GOOD != SetAlignRatio(&pSysSpec->sysNSIS))
			return RESULT_BAD;

		if (RESULT_GOOD != SetNetwork(&pSysSpec->sysNSIS))
			return RESULT_BAD;

		if (RESULT_GOOD != SetStaticOptic(&pSysSpec->sysNSIS))
			return RESULT_BAD;

		if (RESULT_GOOD != SetMeasureRatioSpec(&pSysSpec->sysNSIS))
			return RESULT_BAD;

		if (RESULT_GOOD != SetSaveMeasurePointSpec(&pSysSpec->sysNSIS))
			return RESULT_BAD;

	}
	else
	{
		bool bIsLine = true;
		if (RESULT_GOOD != SetCameraSpec(bIsLine, &pSysSpec->sysCamera[bIsLine]))
			return RESULT_BAD;

		bIsLine = false;
		if (RESULT_GOOD != SetCameraSpec(bIsLine, &pSysSpec->sysCamera[bIsLine]))
			return RESULT_BAD;
	}

	if (RESULT_GOOD != SetPathSpec(&pSysSpec->sysPath))
		return RESULT_BAD;

	if (RESULT_GOOD != SetStepSpec(&pSysSpec->sysStep))
		return RESULT_BAD;

	if (RESULT_GOOD != SetLayerSpec(&pSysSpec->sysLayer))
		return RESULT_BAD;

	AddApplicationLog(L"Succed to Load System Spec");
	return RESULT_GOOD;
}

UINT32 CSettingsFormView::SetBasicSpec(BasicSpec_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	SetUserSite(static_cast<INT32>(pSpec->UserSite));
	SetMachineType(static_cast<INT32>(pSpec->McType));
	SetPlatformType(static_cast<INT32>(pSpec->PlatType));
	SetProductType(static_cast<INT32>(pSpec->ProdType));
	SetCamType(static_cast<INT32>(pSpec->CamType));
	SetMasterLevel(static_cast<INT32>(pSpec->MasterLevel));
	SetStepRepeatMode(static_cast<INT32>(pSpec->bShowAllSteps));
	SetReferenceRegion(static_cast<INT32>(pSpec->bProfileMode));
	SetExceptDummy(static_cast<INT32>(pSpec->bExceptDummy));
	SetKeepLocalCopy(static_cast<INT32>(pSpec->bKeepLocalCopy));
	SetScanType(static_cast<INT32>(pSpec->ScanType));
	SetMaxLayerNum(static_cast<LONG>(pSpec->iMaxLayerNum));
	SetIPUNum(static_cast<LONG>(pSpec->iIPUNum));
	SetImageType(pSpec->strImageType);
	SetExceptImageData(static_cast<INT32>(pSpec->bExceptImageData));
	SetLineArcSubSave(static_cast<INT32>(pSpec->bLineArc_SubSave));

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::SetCameraSpec(bool bIsLine, CameraSpec_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	SetCamResolution(bIsLine, pSpec->dResolution);
	SetDummyMarginX(bIsLine, pSpec->dDummyMarginX);
	SetDummyMarginY(bIsLine, pSpec->dDummyMarginY);

	SetCamFrameWidth(bIsLine, static_cast<LONG>(pSpec->iFrameWidth));
	SetCamFrameHeight(bIsLine, static_cast<LONG>(pSpec->iFrameHeight));
	SetCamMinOverlapX(bIsLine, static_cast<LONG>(pSpec->iMinOverlapX));
	SetCamMinOverlapY(bIsLine, static_cast<LONG>(pSpec->iMinOverlapY));
	SetCamMinMarginX(bIsLine, static_cast<LONG>(pSpec->iMinMarginX));
	SetCamMinMarginY(bIsLine, static_cast<LONG>(pSpec->iMinMarginY));
	SetCamCellColNum(bIsLine, static_cast<LONG>(pSpec->iCellColNum));
	SetCamCellRowNum(bIsLine, static_cast<LONG>(pSpec->iCellRowNum));
	SetCamCellOverlapX(bIsLine, static_cast<LONG>(pSpec->iCellMinOverlapX));
	SetCamCellOverlapY(bIsLine, static_cast<LONG>(pSpec->iCellMinOverlapY));

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::SetCameraSpec_for_NSIS(bool bIs3D, std::vector<NSIS_CameraSpec_t*>& pSpec)
{
	UINT32 iLoop = 0;
	for (auto it : pSpec)
	{
		SetCamMagnification(bIs3D, iLoop, it->dMagnification);
		SetCamZoom(bIs3D, iLoop, it->dZoom);
		SetCamResolution(bIs3D, iLoop, it->dResolution);
		SetCamFovWidth(bIs3D, iLoop, static_cast<LONG>(it->iFovWidth));
		SetCamFovHeight(bIs3D, iLoop, static_cast<LONG>(it->iFovHeight));

		iLoop++;
	}

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::SetPathSpec(PathSpec_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	SetCamDataPath(pSpec->strCamPath);
	SetMasterDataPath(pSpec->strMasterPath);
	SetLogDataPath(pSpec->strLogPath);

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::SetStepSpec(StepSpec_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;
	
	CString str = L"";
	for (auto it : pSpec->vcPanelNames)
	{
		str += it;
		str += L",";
	}
	SetPanelStepName(str);

	str = L"";
	for (auto it : pSpec->vcStripNames)
	{
		str += it;
		str += L",";
	}
	SetStripStepName(str);

	str = L"";
	for (auto it : pSpec->vcUnitNames)
	{
		str += it;
		str += L",";
	}
	SetUnitStepName(str);

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::SetLayerSpec(LayerSpec_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	UINT32 index = 0;
	CString str = L"";
	for (auto it : pSpec->vcLayer)
	{
		SetLayerColor(index, it->layerColor);

		str = L"";
		for (auto itFront : it->vcFrontLayerNames)
		{
			str += itFront;
			str += L",";
		}
		SetFrontLayerName(index, str);

		str = L"";
		for (auto itBack : it->vcBackLayerNames)
		{
			str += itBack;
			str += L",";
		}
		SetBackLayerName(index, str);
		index++;
	}

	return RESULT_GOOD;
}

CString CSettingsFormView::GetLayerDefine(UINT32 layerIdx)
{
	CString strDefine = L"";

	eRefLayer layer = static_cast<eRefLayer>(layerIdx);

	switch (layer)
	{
	case eRefLayer::EN_PATTERN_LAYER:	strDefine = PATTERN_LAYER_TXT;		break;
	case eRefLayer::EN_SR_LAYER:		strDefine = SR_LAYER_TXT;			break;
	case eRefLayer::EN_DOUBLE_SR_LAYER:	strDefine = DOUBLE_SR_LAYER_TXT;	break;
	case eRefLayer::EN_VIA_LAYER:		strDefine = VIA_LAYER_TXT;			break;
	case eRefLayer::EN_VIA2_LAYER:		strDefine = VIA2_LAYER_TXT;			break;
	case eRefLayer::EN_DRILL_LAYER:		strDefine = DRILL_LAYER_TXT;		break;
	case eRefLayer::EN_DRILL2_LAYER:	strDefine = DRILL2_LAYER_TXT;		break;
	case eRefLayer::EN_MASK_LAYER:		strDefine = MASK_LAYER_TXT;			break;
	case eRefLayer::EN_UNIT_OUT_LAYER:  strDefine = UNIT_OUT_LAYER_TXT;     break;
	case eRefLayer::EN_INSIDE_PATTERN_LAYER: strDefine = INSIDE_PATTERN_LAYER_TXT;     break;
	case eRefLayer::EN_MEASURE_LAYER:	strDefine = MEASURE_LAYER_TXT;		break;
	case eRefLayer::EN_SR_DAM_LAYER:	strDefine = SR_DAM_LAYER_TXT;		break;
	case eRefLayer::EN_AU_LAYER:		strDefine = AU_LAYER_TXT;			break;
	case eRefLayer::EN_ETCH_LAYER:		strDefine = ETCH_LAYER_TXT;			break;	
	case eRefLayer::EN_USER_LAYER_1:	strDefine = USER_LAYER1_TXT;		break;
	case eRefLayer::EN_USER_LAYER_2:	strDefine = USER_LAYER2_TXT;		break;
	case eRefLayer::EN_USER_LAYER_3:	strDefine = USER_LAYER3_TXT;		break;
	case eRefLayer::EN_PROFILE_LAYER:	strDefine = PROFILE_LAYER_TXT;		break;
	case eRefLayer::EN_MEASURE_SR_LAYER:	strDefine = THICKNESS_LAYER_TXT;		break;
	default:
		break;
	}

	return strDefine;
}

UINT32 CSettingsFormView::SaveSpecData()
{
	SystemSpec_t* pSysSpec = GetSystemSpec();
	if (pSysSpec == nullptr)
		return RESULT_BAD;

	if (RESULT_GOOD != GetBasicSpec(&pSysSpec->sysBasic))
		return RESULT_BAD;

	if (eMachineType::eNSIS == pSysSpec->sysBasic.McType)
	{

		if (eUserSite::eSEM == pSysSpec->sysBasic.UserSite)
		{
			if (RESULT_GOOD != GetAutoAlignSpec(&pSysSpec->sysAutoAlign))
				return RESULT_BAD;
		}

		if (RESULT_GOOD != GetCameraSpec_for_NSIS(true, pSysSpec->sysNSIS.sys3DCamera))
			return RESULT_BAD;

		if (RESULT_GOOD != GetCameraSpec_for_NSIS(false, pSysSpec->sysNSIS.sys2DCamera))
			return RESULT_BAD;

		if (RESULT_GOOD != GetCameraOrient(&pSysSpec->sysNSIS))
			return RESULT_BAD;

		if (RESULT_GOOD != GetAlignRatio(&pSysSpec->sysNSIS))
			return RESULT_BAD;

		if (RESULT_GOOD != GetNetwork(&pSysSpec->sysNSIS))
			return RESULT_BAD;

		if (RESULT_GOOD != GetStaticOptic(&pSysSpec->sysNSIS))
			return RESULT_BAD;

		if (RESULT_GOOD != GetMeasureRatioSpec(&pSysSpec->sysNSIS))
			return RESULT_BAD;

		if (RESULT_GOOD != GetSaveMeasurePointSpec(&pSysSpec->sysNSIS))
			return RESULT_BAD;	
	}
	else
	{
		bool bIsLine = true;
		if (RESULT_GOOD != GetCameraSpec(bIsLine, &pSysSpec->sysCamera[bIsLine]))
			return RESULT_BAD;

		bIsLine = false;
		if (RESULT_GOOD != GetCameraSpec(bIsLine, &pSysSpec->sysCamera[bIsLine]))
			return RESULT_BAD;
	}

	if (RESULT_GOOD != GetPathSpec(&pSysSpec->sysPath))
		return RESULT_BAD;

	if (RESULT_GOOD != GetStepSpec(&pSysSpec->sysStep))
		return RESULT_BAD;

	if (RESULT_GOOD != GetLayerSpec(&pSysSpec->sysLayer))
		return RESULT_BAD;

	AddApplicationLog(L"Succed to Save System Spec");
	return RESULT_GOOD;
}

UINT32 CSettingsFormView::GetBasicSpec(BasicSpec_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	pSpec->UserSite = GetUserSite();
	pSpec->McType = GetMachineType();
	pSpec->PlatType = GetPlatformType();
	pSpec->ProdType = GetProductType();
	pSpec->CamType = GetCamType();
	pSpec->MasterLevel = GetMasterLevel();
	pSpec->bShowAllSteps = GetStepRepeatMode() == eShowStepRepeatType::eShowAll ? true : false;
	pSpec->bProfileMode = GetReferenceRegion() == eRefRegionMode::eProfile ? true : false;
	pSpec->bExceptDummy = GetExceptDummy();
	pSpec->bKeepLocalCopy = GetKeepLocalCopy();
	pSpec->ScanType = GetScanType();
	pSpec->strImageType = GetImageType();
	pSpec->bExceptImageData = GetExceptImageData();
	pSpec->bLineArc_SubSave = GetLineArc_SubSave();

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::GetCameraSpec(bool bIsLine, CameraSpec_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	pSpec->dResolution = GetCamResolution(bIsLine);
	pSpec->dDummyMarginX = GetDummyMarginX(bIsLine);
	pSpec->dDummyMarginY = GetDummyMarginY(bIsLine);
	pSpec->iFrameWidth = GetCamFrameWidth(bIsLine);
	pSpec->iFrameHeight = GetCamFrameHeight(bIsLine);
	pSpec->iMinOverlapX = GetCamMinOverlapX(bIsLine);
	pSpec->iMinOverlapY = GetCamMinOverlapY(bIsLine);
	pSpec->iMinMarginX = GetCamMinMarginX(bIsLine);
	pSpec->iMinMarginY = GetCamMinMarginY(bIsLine);
	pSpec->iCellColNum = GetCamCellColNum(bIsLine);
	pSpec->iCellRowNum = GetCamCellRowNum(bIsLine);
	pSpec->iCellMinOverlapX = GetCamCellOverlapX(bIsLine);
	pSpec->iCellMinOverlapY = GetCamCellOverlapY(bIsLine);

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::GetCameraSpec_for_NSIS(bool bIs3D, std::vector<NSIS_CameraSpec_t*>& pSpec)
{
	UINT32 iLoop = 0;
	for (auto it : pSpec)
	{
		it->dMagnification	= GetCamMagnification(bIs3D, iLoop);
		it->dZoom			= GetCamZoom(bIs3D, iLoop);
		it->dResolution		= GetCamResolution(bIs3D, iLoop);
		it->iFovWidth		= GetCamFovWidth(bIs3D, iLoop);
		it->iFovHeight		= GetCamFovHeight(bIs3D, iLoop);

		iLoop++;
	}
	return RESULT_GOOD;
}

double CSettingsFormView::GetCamMagnification(bool bIs3D, UINT32 idx)
{
	if (bIs3D)
	{
		if (idx >= MAX_3D_MODE_NUM) return 0.0;
		return (m_st3DCamProp[idx].pMagnification->GetValue().dblVal);
	}
	else
	{
		if (idx >= MAX_2D_MODE_NUM) return 0.0;
		return (m_st2DCamProp[idx].pMagnification->GetValue().dblVal);
	}
}

void CSettingsFormView::SetCamMagnification(bool bIs3D, UINT32 idx, double dData)
{
	if (bIs3D)
	{
		if (idx >= MAX_3D_MODE_NUM) return;
		m_st3DCamProp[idx].pMagnification->SetValue((_variant_t)dData);
	}
	else
	{
		if (idx >= MAX_2D_MODE_NUM) return;
		m_st2DCamProp[idx].pMagnification->SetValue((_variant_t)dData);
	}
}

double CSettingsFormView::GetCamZoom(bool bIs3D, UINT32 idx)
{
	if (bIs3D)
	{
		if (idx >= MAX_3D_MODE_NUM) return 0.0;
		return (m_st3DCamProp[idx].pZoom->GetValue().dblVal);
	}
	else
	{
		if (idx >= MAX_2D_MODE_NUM) return 0.0;
		return (m_st2DCamProp[idx].pZoom->GetValue().dblVal);
	}
}

void CSettingsFormView::SetCamZoom(bool bIs3D, UINT32 idx, double dData)
{
	if (bIs3D)
	{
		if (idx >= MAX_3D_MODE_NUM) return;
		m_st3DCamProp[idx].pZoom->SetValue((_variant_t)dData);
	}
	else
	{
		if (idx >= MAX_2D_MODE_NUM) return;
		m_st2DCamProp[idx].pZoom->SetValue((_variant_t)dData);
	}
}

double CSettingsFormView::GetCamResolution(bool bIs3D, UINT32 idx)
{
	if (bIs3D)
	{
		if (idx >= MAX_3D_MODE_NUM) return 0.0;
		return (m_st3DCamProp[idx].pResolution->GetValue().dblVal);
	}
	else
	{
		if (idx >= MAX_2D_MODE_NUM) return 0.0;
		return (m_st2DCamProp[idx].pResolution->GetValue().dblVal);
	}
}

void CSettingsFormView::SetCamResolution(bool bIs3D, UINT32 idx, double dData)
{
	if (bIs3D)
	{
		if (idx >= MAX_3D_MODE_NUM) return;
		m_st3DCamProp[idx].pResolution->SetValue((_variant_t)dData);
	}
	else
	{
		if (idx >= MAX_2D_MODE_NUM) return;
		m_st2DCamProp[idx].pResolution->SetValue((_variant_t)dData);
	}
}

UINT32 CSettingsFormView::GetCamFovWidth(bool bIs3D, UINT32 idx)
{
	if (bIs3D)
	{
		if (idx >= MAX_3D_MODE_NUM) return 0;
		return (m_st3DCamProp[idx].pFovWidth->GetValue().lVal);
	}
	else
	{
		if (idx >= MAX_2D_MODE_NUM) return 0;
		return (m_st2DCamProp[idx].pFovWidth->GetValue().lVal);
	}
}

void CSettingsFormView::SetCamFovWidth(bool bIs3D, UINT32 idx, LONG iData)
{
	if (bIs3D)
	{
		if (idx >= MAX_3D_MODE_NUM) return;
		m_st3DCamProp[idx].pFovWidth->SetValue((_variant_t)iData);
	}
	else
	{
		if (idx >= MAX_2D_MODE_NUM) return;
		m_st2DCamProp[idx].pFovWidth->SetValue((_variant_t)iData);
	}
}

UINT32 CSettingsFormView::GetCamFovHeight(bool bIs3D, UINT32 idx)
{
	if (bIs3D)
	{
		if (idx >= MAX_3D_MODE_NUM) return 0;
		return (m_st3DCamProp[idx].pFovHeight->GetValue().lVal);
	}
	else
	{
		if (idx >= MAX_2D_MODE_NUM) return 0;
		return (m_st2DCamProp[idx].pFovHeight->GetValue().lVal);
	}
}

void CSettingsFormView::SetCamFovHeight(bool bIs3D, UINT32 idx, LONG iData)
{
	if (bIs3D)
	{
		if (idx >= MAX_3D_MODE_NUM) return;
		m_st3DCamProp[idx].pFovHeight->SetValue((_variant_t)iData);
	}
	else
	{
		if (idx >= MAX_2D_MODE_NUM) return;
		m_st2DCamProp[idx].pFovHeight->SetValue((_variant_t)iData);
	}
}

UINT32 CSettingsFormView::GetPathSpec(PathSpec_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	pSpec->strCamPath = GetCamDataPath();
	pSpec->strMasterPath = GetMasterDataPath();
	pSpec->strLogPath = GetLogDataPath();

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::GetStepSpec(StepSpec_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	pSpec->vcPanelNames.clear();

	CString str = L"", strTmp = GetPanelStepName();
	while (strTmp.GetLength() > 0)
	{
		AfxExtractSubString(str, strTmp, 0, ',');
		pSpec->vcPanelNames.emplace_back(str);
		strTmp.Delete(0, str.GetLength() + 1);
	}

	pSpec->vcStripNames.clear();
	str = L"", strTmp = GetStripStepName();
	while (strTmp.GetLength() > 0)
	{
		AfxExtractSubString(str, strTmp, 0, ',');
		pSpec->vcStripNames.emplace_back(str);
		strTmp.Delete(0, str.GetLength() + 1);
	}

	pSpec->vcUnitNames.clear();
	str = L"", strTmp = GetUnitStepName();
	while (strTmp.GetLength() > 0)
	{
		AfxExtractSubString(str, strTmp, 0, ',');
		pSpec->vcUnitNames.emplace_back(str);
		strTmp.Delete(0, str.GetLength() + 1);
	}
	
	return RESULT_GOOD;
}

UINT32 CSettingsFormView::GetLayerSpec(LayerSpec_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	pSpec->Clear();
	for (UINT32 iLayer = 0; iLayer < static_cast<UINT32>(eRefLayer::EN_MAX_LAYER_NUM); iLayer++)
	{
		LayerInfo_t* pLayer = new LayerInfo_t;
		
		pLayer->layerColor = GetLayerColor(iLayer);
		pLayer->strDefine = GetLayerDefine(iLayer);

		CString str = L"", strTmp = GetFrontLayerName(iLayer);
		while (strTmp.GetLength() > 0)
		{
			AfxExtractSubString(str, strTmp, 0, ',');
			pLayer->vcFrontLayerNames.emplace_back(str);
			strTmp.Delete(0, str.GetLength() + 1);
		}

		str = L"", strTmp = GetBackLayerName(iLayer);
		while (strTmp.GetLength() > 0)
		{
			AfxExtractSubString(str, strTmp, 0, ',');
			pLayer->vcBackLayerNames.emplace_back(str);
			strTmp.Delete(0, str.GetLength() + 1);
		}

		pSpec->vcLayer.emplace_back(pLayer);
	}

	return RESULT_GOOD;
}

void CSettingsFormView::OnSaveBtnClick()
{
	SaveSpecData();
	SaveSystemSpec();
	RemoveBtnFocus(&m_btnSaveApply);

	AfxMessageBox(L"설정값이 저장되었습니다.");
}

void CSettingsFormView::OnResetBtnClick()
{
	LoadSpecData();
	RemoveBtnFocus(&m_btnReset);

	_UpdateIU_Pos();
	
	AfxMessageBox(L"설정값이 초기화되었습니다.");
}

void CSettingsFormView::RemoveBtnFocus(CButton* btn)
{
	DWORD style = btn->GetStyle();
	// remove default push button style
	style &= ~BS_DEFPUSHBUTTON;
	SetFocus();

	// set the style
	::SendMessage(btn->GetSafeHwnd(), BM_SETSTYLE, (WPARAM)style, (LPARAM)TRUE);
}

UINT32 CSettingsFormView::SetCameraOrient(NSISSpec_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	if (m_pCamOrient[0])
		m_pCamOrient[0]->SetValue((_variant_t)static_cast<LONG>(pSpec->sysCameraOrient_Top * 90));
	else
		return RESULT_BAD;

	if (m_pCamOrient[1])
		m_pCamOrient[1]->SetValue((_variant_t)static_cast<LONG>(pSpec->sysCameraOrient_Bot * 90));
	else
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::SetAlignRatio(NSISSpec_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	if (m_pAlignRatio[0])
		m_pAlignRatio[0]->SetValue((_variant_t)static_cast<LONG>(pSpec->sysAlignRatio));
	else
		return RESULT_BAD;

	if (m_pAlignRatio[1])
		m_pAlignRatio[1]->SetValue((_variant_t)static_cast<LONG>(pSpec->sysUnitAlignRatio));
	else
		return RESULT_BAD;

	if (m_pZoomMarginRatio)
		m_pZoomMarginRatio->SetValue((_variant_t)static_cast<double>(pSpec->sysZoomMarginRatio));
	else
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::SetNetwork(NSISSpec_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	if (m_pNetwork[0])
		m_pNetwork[0]->SetValue((_variant_t)static_cast<LONG>(pSpec->sysNSISPort));
	else
		return RESULT_BAD;

	if (m_pNetwork[1])
		m_pNetwork[1]->SetValue((_variant_t)static_cast<LONG>(pSpec->sysCamPort));
	else
		return RESULT_BAD;

	if (m_pNetwork[2])
		m_pNetwork[2]->SetValue((_variant_t)pSpec->sysNSISIP);	
	else
		return RESULT_BAD;

	if (m_pNetwork[3])	
		m_pNetwork[3]->SetValue((_variant_t)pSpec->sysCamIP);	
	else
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::SetMeasureRatioSpec(NSISSpec_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	int i, index;
	if (m_pMeasureDefaultRatio)
		m_pMeasureDefaultRatio->SetValue((_variant_t)static_cast<double>(pSpec->sysMeasureDefaultScale));
	else
		return RESULT_BAD;

	index = 0;
	for (i = 0; i < static_cast<UINT32>(MeasureType::MeasureTypeNone); i++)
	{
		CString TempString = GetMeasureDefine(i);

		if (TempString != "")
		{
			if (m_pMeasureRatio[index])
				m_pMeasureRatio[index]->SetValue((_variant_t)static_cast<double>(pSpec->sysMeasureTypeScale[i]));
			else
				return RESULT_BAD;

			index++;
		}
	}

	return RESULT_GOOD;
}

//YJD START 22.06.13 - SAVE MEASURE IMAGE
UINT32 CSettingsFormView::SetSaveMeasurePointSpec(NSISSpec_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	// 2022.11.02 KJH ADD
	// Unit Layer Image Save Parameter
	if (m_pUseSaveUnitLayer)
	{
		switch (pSpec->sysUseSaveUnitLayer)
		{
		case true:
			m_pUseSaveUnitLayer->SetValue((_variant_t)_T("TRUE"));
			break;
		default:
			m_pUseSaveUnitLayer->SetValue((_variant_t)_T("FALSE"));
			break;
		}
	}
	else
		return RESULT_BAD;

	if (m_pUseSaveMeasurePoint)
	{
		switch (pSpec->sysUseSaveMeasurePoint)
		{
		case true:
			m_pUseSaveMeasurePoint->SetValue((_variant_t)_T("TRUE"));
			break;
		default:
			m_pUseSaveMeasurePoint->SetValue((_variant_t)_T("FALSE"));
			break;
		}
	}
	else
		return RESULT_BAD;

	if (m_pSaveMeasurePointScale)
		m_pSaveMeasurePointScale->SetValue((_variant_t)static_cast<double>(pSpec->sysSaveMeasureScale));
	else
		return RESULT_BAD;


	return RESULT_GOOD;
}
//YJD END 22.06.13 - SAVE MEASURE IMAGE

UINT32 CSettingsFormView::SetStaticOptic(NSISSpec_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	if (m_pStaticOptic[0])
		m_pStaticOptic[0]->SetValue((_variant_t)static_cast<double>(pSpec->sysAlignLens));
	else
		return RESULT_BAD;

	if (m_pStaticOptic[1])
		m_pStaticOptic[1]->SetValue((_variant_t)static_cast<double>(pSpec->sysAlignZoom));
	else
		return RESULT_BAD;

	if (m_pStaticOptic[2])
		m_pStaticOptic[2]->SetValue((_variant_t)static_cast<double>(pSpec->sysUnitAlignLens));
	else
		return RESULT_BAD;

	if (m_pStaticOptic[3])
		m_pStaticOptic[3]->SetValue((_variant_t)static_cast<double>(pSpec->sysUnitAlignZoom));
	else
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::GetCameraOrient(NSISSpec_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	if (m_pCamOrient[0])
		pSpec->sysCameraOrient_Top = static_cast<INT32>(m_pCamOrient[0]->GetValue().lVal / 90);
	else
		return RESULT_BAD;

	if (m_pCamOrient[1])
		pSpec->sysCameraOrient_Bot = static_cast<INT32>(m_pCamOrient[1]->GetValue().lVal / 90);
	else
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::GetAlignRatio(NSISSpec_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	if (m_pAlignRatio[0])
		pSpec->sysAlignRatio = static_cast<INT32>(m_pAlignRatio[0]->GetValue().lVal);
	else
		return RESULT_BAD;

	if (m_pAlignRatio[1])
		pSpec->sysUnitAlignRatio = static_cast<INT32>(m_pAlignRatio[1]->GetValue().lVal);
	else
		return RESULT_BAD;

	if (m_pZoomMarginRatio)
		pSpec->sysZoomMarginRatio = static_cast<double>(m_pZoomMarginRatio->GetValue().dblVal);

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::GetNetwork(NSISSpec_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	if (m_pNetwork[0])
		pSpec->sysNSISPort = static_cast<INT32>(m_pNetwork[0]->GetValue().lVal);
	else
		return RESULT_BAD;

	if (m_pNetwork[1])
		pSpec->sysCamPort = static_cast<INT32>(m_pNetwork[1]->GetValue().lVal);
	else
		return RESULT_BAD;

	if (m_pNetwork[2])
		pSpec->sysNSISIP = m_pNetwork[2]->GetValue().bstrVal;
	else
		return RESULT_BAD;

	if (m_pNetwork[3])
		pSpec->sysCamIP = m_pNetwork[3]->GetValue().bstrVal;
	else
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::GetMeasureRatioSpec(NSISSpec_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	int i, index;
	if (m_pMeasureDefaultRatio)
		pSpec->sysMeasureDefaultScale = static_cast<double>(m_pMeasureDefaultRatio->GetValue().dblVal);
	else
		return RESULT_BAD;

	index = 0;
	for (i = 0; i < static_cast<UINT32>(MeasureType::MeasureTypeNone); i++)
	{
		CString TempString = GetMeasureDefine(i);

		if (TempString != "")
		{
			if (m_pMeasureRatio[index])
				pSpec->sysMeasureTypeScale[i] = static_cast<double>(m_pMeasureRatio[index]->GetValue().dblVal);
			else
				return RESULT_BAD;

			index++;
		}
	}

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::GetSaveMeasurePointSpec(NSISSpec_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	// 2022.11.02 KJH ADD
	// Unit Layer Image Save Parameter
	CString strValue;
	if (m_pUseSaveUnitLayer)
	{
		strValue = m_pUseSaveUnitLayer->GetValue().bstrVal;

		if (strValue == _T("TRUE"))
		{
			pSpec->sysUseSaveUnitLayer = true;
		}
		else
		{
			pSpec->sysUseSaveUnitLayer = false;
		}
	}
	else
	{
		pSpec->sysUseSaveUnitLayer = false;
	}


	if (m_pUseSaveMeasurePoint)
	{
		CString strValue = m_pUseSaveMeasurePoint->GetValue().bstrVal;

		if (strValue == _T("TRUE"))
		{
			pSpec->sysUseSaveMeasurePoint = true;
		}
		else
		{
			pSpec->sysUseSaveMeasurePoint = false;
		}
	}
	else
	{
		pSpec->sysUseSaveMeasurePoint = false;
	}

	if (m_pSaveMeasurePointScale)
		pSpec->sysSaveMeasureScale = static_cast<double>(m_pSaveMeasurePointScale->GetValue().dblVal);
	else
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CSettingsFormView::GetStaticOptic(NSISSpec_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	if (m_pStaticOptic[0])
		pSpec->sysAlignLens = static_cast<double>(m_pStaticOptic[0]->GetValue().dblVal);
	else
		return RESULT_BAD;

	if (m_pStaticOptic[1])
		pSpec->sysAlignZoom = static_cast<double>(m_pStaticOptic[1]->GetValue().dblVal);
	else
		return RESULT_BAD;

	if (m_pStaticOptic[2])
		pSpec->sysUnitAlignLens = static_cast<double>(m_pStaticOptic[2]->GetValue().dblVal);
	else
		return RESULT_BAD;

	if (m_pStaticOptic[3])
		pSpec->sysUnitAlignZoom = static_cast<double>(m_pStaticOptic[3]->GetValue().dblVal);
	else
		return RESULT_BAD;

	return RESULT_GOOD;
}

void CSettingsFormView::SetIPUNum(LONG iData)
{
	m_pIPUNum->SetValue((_variant_t)iData);
}

UINT32 CSettingsFormView::GetIPUNum()
{
	return static_cast<UINT32>(m_pIPUNum->GetValue().lVal);
}

UINT32 CSettingsFormView::InitializeIPUNum()
{
	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	m_pIPUNum = new CMFCPropertyGridProperty(STRING_MAX_IPU_NUM, var.lVal, STRING_USER_EDITABLE);
	if (m_pIPUNum)
	{
		m_pIPUNum->AllowEdit(FALSE);
		m_pIPUNum->Enable(FALSE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeImageType()
{
	m_pImageType = new CMFCPropertyGridProperty(STRING_IMAGE_TYPE, L"", STRING_IMAGE_TYPE_DESC);
	if (m_pImageType)
	{
		m_pImageType->AddOption(STRING_IMAGE_TYPE_BMP);
		m_pImageType->AddOption(STRING_IMAGE_TYPE_TIF);
		m_pImageType->AddOption(STRING_IMAGE_TYPE_PNG);
		m_pImageType->AddOption(STRING_IMAGE_TYPE_JPG);
		m_pImageType->AllowEdit(TRUE);
		m_pImageType->Enable(TRUE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

CString CSettingsFormView::GetImageType()
{
	return(m_pImageType->GetValue().bstrVal);
}

void CSettingsFormView::SetImageType(CString strType)
{
	m_pImageType->SetValue((_variant_t)strType);
}

double CSettingsFormView::GetDummyMarginX(bool bIsLine)
{
	return (m_pDummyMarginX[bIsLine]->GetValue().dblVal);
}

void CSettingsFormView::SetDummyMarginX(bool bIsLine, double dData)
{
	m_pDummyMarginX[bIsLine]->SetValue((_variant_t)dData);
}

double CSettingsFormView::GetDummyMarginY(bool bIsLine)
{
	return (m_pDummyMarginY[bIsLine]->GetValue().dblVal);
}

void CSettingsFormView::SetDummyMarginY(bool bIsLine, double dData)
{
	m_pDummyMarginY[bIsLine]->SetValue((_variant_t)dData);
}

UINT32 CSettingsFormView::InitializeDummyMarginX(bool bIsLine)
{
	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	m_pDummyMarginX[bIsLine] = new CMFCPropertyGridProperty(STRING_DUMMY_MARGIN_X, var.dblVal, STRING_USER_EDITABLE);
	if (m_pDummyMarginX[bIsLine])
	{
		m_pDummyMarginX[bIsLine]->AllowEdit(TRUE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CSettingsFormView::InitializeDummyMarginY(bool bIsLine)
{
	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	m_pDummyMarginY[bIsLine] = new CMFCPropertyGridProperty(STRING_DUMMY_MARGIN_Y, var.dblVal, STRING_USER_EDITABLE);
	if (m_pDummyMarginY[bIsLine])
	{
		m_pDummyMarginY[bIsLine]->AllowEdit(TRUE);
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

//YJD START 22.06.13 - SAVE MEASURE IMAGE
UINT32 CSettingsFormView::InitializeSaveMeasurePointSpec()
{
	//CMFCPropertyGridProperty *m_pUseSaveMeasurePoint, *m_pSaveMeasurePointScale;
	CMFCPropertyGridProperty* pMeasureDefine = new CMFCPropertyGridProperty(STRING_SAVE_MEASURE_POINT_DEFINE);
	if (pMeasureDefine == nullptr)
		return RESULT_BAD;

	VARIANT var;
	var.dblVal = 4.0;
	//int index;

	// 2022.11.02 KJH ADD
	// Unit Layer Image Save Parameter
	m_pUseSaveUnitLayer = new CMFCPropertyGridProperty(_T("USE SAVE UNIT LAYER IMAGE"), L"", L"SAVE IMAGE -> TRUE / NOT SAVE IMAGE -> FALSE");
	m_pUseSaveUnitLayer->AddOption(_T("TRUE"));
	m_pUseSaveUnitLayer->AddOption(_T("FALSE"));
	m_pUseSaveUnitLayer->AllowEdit(FALSE);
	pMeasureDefine->AddSubItem(m_pUseSaveUnitLayer);

	m_pUseSaveMeasurePoint = new CMFCPropertyGridProperty(_T("USE SAVE MEASURE POINT IMAGE"), L"", L"SAVE IMAGE -> TRUE / NOT SAVE IMAGE -> FALSE");
	m_pUseSaveMeasurePoint->AddOption(_T("TRUE"));
	m_pUseSaveMeasurePoint->AddOption(_T("FALSE"));
	m_pUseSaveMeasurePoint->AllowEdit(FALSE);
	pMeasureDefine->AddSubItem(m_pUseSaveMeasurePoint);

	m_pSaveMeasurePointScale = new CMFCPropertyGridProperty(_T("SAVE MEASURE POINT SCALE"), var.dblVal, STRING_USER_EDITABLE);
	m_pSaveMeasurePointScale->AllowEdit(TRUE);
	pMeasureDefine->AddSubItem(m_pSaveMeasurePointScale);
	pMeasureDefine->Expand(TRUE);
	m_PropGridCtrl.AddProperty(pMeasureDefine);

	return RESULT_GOOD;
}

//YJD END 22.06.13 - SAVE MEASURE IMAGE
UINT32 CSettingsFormView::InitializeMeasureRatioSpec()
{
	CMFCPropertyGridProperty* pMeasureDefine = new CMFCPropertyGridProperty(STRING_MEASURE_RATIO_DEFINE);
	if (pMeasureDefine == nullptr)
		return RESULT_BAD;

	UINT32 iMeasureCnt = static_cast<UINT32>(MeasureType::MeasureTypeNone);

	VARIANT var;
	var.dblVal = 4.0;
	int index;

	m_pMeasureDefaultRatio = new CMFCPropertyGridProperty(_T("DEFAULT MEASURE"), var.dblVal, STRING_USER_EDITABLE);
	m_pMeasureDefaultRatio->AllowEdit(TRUE);
	pMeasureDefine->AddSubItem(m_pMeasureDefaultRatio);

	index = 0;
	for (UINT32 i = 0; i < iMeasureCnt; i++)
	{
		CString TempString = GetMeasureDefine(i);

		if (TempString != "")
		{
			m_pMeasureRatio[index] = new CMFCPropertyGridProperty(TempString, var.dblVal, STRING_USER_EDITABLE);
			if (m_pMeasureRatio[index])
			{
				m_pMeasureRatio[index]->AllowEdit(TRUE);
				pMeasureDefine->AddSubItem(m_pMeasureRatio[index]);
			}
			else
				return RESULT_BAD;
			index++;
		}
	}

	m_PropGridCtrl.AddProperty(pMeasureDefine);
	pMeasureDefine->Expand(TRUE);

	return RESULT_GOOD;
}


CString CSettingsFormView::GetMeasureDefine(UINT32 index)
{
	MeasureType type = static_cast<MeasureType>(index);

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
		return _T("");
}


UINT32 CSettingsFormView::InitializeAutoAlignSpec()
{

	VARIANT var;
	var.dblVal = 0.0;
	var.lVal = 0;

	CMFCPropertyGridProperty* pAutoAlignGroup = new CMFCPropertyGridProperty(STRING_AUTO_ALIGN);
	if (pAutoAlignGroup == nullptr)
		return RESULT_BAD;

	//1. AutoAlign	
	m_pAutoAlign = new CMFCPropertyGridProperty(STRING_AUTO_ALIGN, L"");
	if (m_pAutoAlign)
	{
		m_pAutoAlign->AddOption(_T("TRUE"));
		m_pAutoAlign->AddOption(_T("FALSE"));
		m_pAutoAlign->AllowEdit(TRUE);
		m_pAutoAlign->Enable(TRUE);		
	}
	pAutoAlignGroup->AddSubItem(m_pAutoAlign);
	

	//2. RectWidth
	int nSize = 2;
	for (int i = 0; i < nSize; i++)
	{
		if(i == 0)
			m_pAutoAlignRect[i] = new CMFCPropertyGridProperty(STRING_AUTO_ALIGN_RECT_WIDTH, var.lVal, STRING_USER_EDITABLE);
		else
			m_pAutoAlignRect[i] = new CMFCPropertyGridProperty(STRING_AUTO_ALIGN_RECT_HEIGHT, var.lVal, STRING_USER_EDITABLE);

		if (m_pAutoAlignRect[i])	
		{
			m_pAutoAlignRect[i]->AllowEdit(TRUE);
			pAutoAlignGroup->AddSubItem(m_pAutoAlignRect[i]);
		}
	}

	//3. Auto Align Pos
	nSize = 4;
	CString strTemp = _T("");
	for (int i = 0; i < 4; i++)
	{
		strTemp.Format(_T("AUTO ALIGN X%d"), i+1);
		m_pAutoAlignPosX[i] = new CMFCPropertyGridProperty(strTemp, var.lVal, STRING_USER_EDITABLE);
		if (m_pAutoAlignPosX[i])
		{
			m_pAutoAlignPosX[i]->AllowEdit(TRUE);
		}
		pAutoAlignGroup->AddSubItem(m_pAutoAlignPosX[i]);

		strTemp.Format(_T("AUTO ALIGN Y%d"), i+1);		
		m_pAutoAlignPosY[i] = new CMFCPropertyGridProperty(strTemp, var.lVal, STRING_USER_EDITABLE);
		if (m_pAutoAlignPosY[i])
		{
			m_pAutoAlignPosY[i]->AllowEdit(TRUE);
		}
		pAutoAlignGroup->AddSubItem(m_pAutoAlignPosY[i]);

	}
	
	
	
	m_PropGridCtrl.AddProperty(pAutoAlignGroup);
	pAutoAlignGroup->Expand(TRUE);
	return RESULT_GOOD;
}


UINT32 CSettingsFormView::SetAutoAlignSpec(AutoAlign_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	if (pSpec->bAutoAlign == TRUE)
		m_pAutoAlign->SetValue((_variant_t)_T("TRUE"));
	else
		m_pAutoAlign->SetValue((_variant_t)_T("FALSE"));


	if (m_pAutoAlignRect[0])
		m_pAutoAlignRect[0]->SetValue((_variant_t)static_cast<LONG>(pSpec->dRectWidth));
	
	if (m_pAutoAlignRect[1])
		m_pAutoAlignRect[1]->SetValue((_variant_t)static_cast<LONG>(pSpec->dRectHeight));

	int nSize = static_cast<int>(pSpec->vecAutoAlign.size());

	for (int i = 0; i < nSize; i++)
	{
		m_pAutoAlignPosX[i]->SetValue((_variant_t)static_cast<LONG>(pSpec->vecAutoAlign[i]->dAutoAlignX));
		m_pAutoAlignPosY[i]->SetValue((_variant_t)static_cast<LONG>(pSpec->vecAutoAlign[i]->dAutoAlignY));
	}
		
	return RESULT_GOOD;
}

UINT32 CSettingsFormView::GetAutoAlignSpec(AutoAlign_t* pSpec)
{
	if (pSpec == nullptr)
		return RESULT_BAD;

	CString strTemp = m_pAutoAlign->GetValue().bstrVal;

	// 1. Auto Align
	if (strTemp == "TRUE")
		pSpec->bAutoAlign = 1;
	else
		pSpec->bAutoAlign = 0;

	// 2. Rect Width , Rect Height
	// 0 --> width, 1 --> height
	int nSize = 2; 
	for (int i = 0; i < nSize; i++)
	{
		if (i == 0 )
			pSpec->dRectWidth = static_cast<INT32>(m_pAutoAlignRect[i]->GetValue().lVal);
		else
			pSpec->dRectHeight = static_cast<INT32>(m_pAutoAlignRect[i]->GetValue().lVal);
	}

	// 3. Auto Align Pos X, Y 
	nSize = 4;
	for (int i = 0; i < nSize; i++)
	{		
		pSpec->vecAutoAlign[i]->dAutoAlignX = static_cast<INT32>(m_pAutoAlignPosX[i]->GetValue().lVal);
		pSpec->vecAutoAlign[i]->dAutoAlignY = static_cast<INT32>(m_pAutoAlignPosY[i]->GetValue().lVal);
	}

	return RESULT_GOOD;
}