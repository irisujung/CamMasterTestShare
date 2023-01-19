#include "pch.h"
#include "FormViewDocPane.h"
#include "MainFrm.h"

BEGIN_MESSAGE_MAP(CFormViewDocPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()

int CFormViewDocPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_iFormViewIdx = GetDlgCtrlID();

	switch (m_iFormViewIdx)
	{
		case ID_JOBINFO_DOCPANE:
			m_pWndJobInfo = CJobInfoFormView::CreateOne(this);
			ASSERT(m_pWndJobInfo);
			break;
		case ID_CAMINFO_DOCPANE:
			m_pWndCamInfo = CCamInfoFormView::CreateOne(this);
			ASSERT(m_pWndCamInfo);
			break;
		case ID_SETTINGS_DOCPANE:
			m_pWndSettings = CSettingsFormView::CreateOne(this);
			ASSERT(m_pWndSettings);
			break;
// 		case ID_MINIMAP_V2_DOCPANE:
// 			m_pWndMinimap_V2 = CMiniMapFormView_V2::CreateOne(this);
// 			m_pWndMinimap_V2->OnInitialUpdate();
// 			ASSERT(m_pWndMinimap_V2);
// 			break;
		case ID_FEATURE_EDIT_DOCPANE:
			m_pWndFeatureEdit = CFeatureEditFormView::CreateOne(this);
			m_pWndFeatureEdit->OnInitialUpdate();
			ASSERT(m_pWndFeatureEdit);
			break;
		case ID_MINIMAP_DOCPANE_V2:
			m_pWndMinimap_V2 = CMiniMapFormView_V2::CreateOne(this);
			m_pWndMinimap_V2->OnInitialUpdate();
			ASSERT(m_pWndMinimap_V2);
			break;

	}


	return RESULT_GOOD;
}

void CFormViewDocPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	AdjustLayout();
	Invalidate();
}

void CFormViewDocPane::AdjustLayout()
{
	if (GetSafeHwnd() == NULL) return;

	CRect rectClient;
	GetClientRect(rectClient);

	switch (m_iFormViewIdx)
	{
		case ID_JOBINFO_DOCPANE:
			m_pWndJobInfo->SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
			break;	
		case ID_CAMINFO_DOCPANE:
			m_pWndCamInfo->SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
			break;
		case ID_SETTINGS_DOCPANE:
			m_pWndSettings->SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
			break;
		case ID_FEATURE_EDIT_DOCPANE:
			m_pWndFeatureEdit->SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
			break;

		case ID_MINIMAP_DOCPANE_V2:
			m_pWndMinimap_V2->SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
			break;
	}
}

int CFormViewDocPane::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	CFrameWnd* pParentFrame = GetParentFrame();

	if ((pParentFrame == pDesktopWnd) ||
		(pDesktopWnd->IsChild(pParentFrame)))
	{
		return CDockablePane::OnMouseActivate(pDesktopWnd, nHitTest, message);
	}

	return MA_NOACTIVATE;
}

void CFormViewDocPane::LockJobTree(BOOL bEnable)
{
	if (m_pWndJobInfo)
		m_pWndJobInfo->LockJobTree(bEnable);
}

void CFormViewDocPane::InitialUpdate()
{
	if (GetSafeHwnd() == NULL) return;

	switch (m_iFormViewIdx)
	{
	case ID_JOBINFO_DOCPANE:
		if (m_pWndJobInfo)
			m_pWndJobInfo->OnInitialUpdate();;
		break;
	case ID_CAMINFO_DOCPANE:
		if (m_pWndCamInfo)
			m_pWndCamInfo->OnInitialUpdate();
		break;
	case ID_SETTINGS_DOCPANE:
		if (m_pWndSettings)
			m_pWndSettings->OnInitialUpdate();
		break;
// 	case ID_FEATURE_EDIT_DOCPANE:
// 		if (m_pWndFeatureEdit)
// 			m_pWndFeatureEdit->OnInitialUpdate();
// 		break;
	}
}

UINT32 CFormViewDocPane::SetJobPath(CString strJobPath)
{
	if (m_pWndJobInfo)
		return m_pWndJobInfo->SetJobPath(strJobPath);
	else
		return RESULT_BAD;
}

UINT32 CFormViewDocPane::SetStepCount(UINT32& iStepCnt, std::vector<UINT32>& vcStepRepeatCnt)
{
	if (m_pWndCamInfo)	
		return m_pWndCamInfo->SetStepCount(iStepCnt, vcStepRepeatCnt);
	else
		return RESULT_BAD;
}

UINT32 CFormViewDocPane::GetStepInfo(std::vector<stStepInfo*>** v)
{
	if (m_pWndCamInfo)
	{
		*v = m_pWndCamInfo->GetStepInfo();
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CFormViewDocPane::DisplayCamInfo()
{
	if (m_pWndCamInfo)
		return m_pWndCamInfo->DisplayCamInfo();

	return RESULT_BAD;
}

UINT32	CFormViewDocPane::EstimateStepOrient()
{
	if (m_pWndCamInfo)
		return m_pWndCamInfo->EstimateStepOrient();

	return RESULT_BAD;
}

CString CFormViewDocPane::GetWorkStepName()
{
	if (m_pWndCamInfo)
		return m_pWndCamInfo->GetWorkStepName();

	return L"";
}

UINT32 CFormViewDocPane::SetLayerCount(UINT32& iLayerCnt)
{
	if (m_pWndCamInfo)
		return m_pWndCamInfo->SetLayerCount(iLayerCnt);
	else
		return RESULT_BAD;
}

UINT32 CFormViewDocPane::GetLayerInfo(std::vector<stLayerInfo*>** v)
{
	if (m_pWndCamInfo)
	{
		*v = m_pWndCamInfo->GetLayerInfo();
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 CFormViewDocPane::SetComponentCount(UINT32& iStepCnt, std::vector<UINT32>& vcCompCnt)
{
	if (m_pWndCamInfo)
		return m_pWndCamInfo->SetComponentCount(iStepCnt, vcCompCnt);
	else
		return RESULT_BAD;
}

UINT32 CFormViewDocPane::GetComponentInfo(std::vector<stComponentInfo*>** v)
{
	if (m_pWndCamInfo)
	{
		*v = m_pWndCamInfo->GetComponentInfo();
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

INT32 CFormViewDocPane::GetCurStepIndex()
{
	if (m_pWndCamInfo)
		return m_pWndCamInfo->GetCurStepIndex();
	else
		return -1;
}

BOOL CFormViewDocPane::SetODBFile(CString csODBFile)
{
	return m_pWndJobInfo->SetODBFile(csODBFile);
}

UINT32 CFormViewDocPane::CheckLayerInfo(std::vector<stLayerInfo*>** v,
	vector<vector<stLayerInfo*>> &vecPatternList,
	vector<vector<stLayerInfo*>> &vecSRList,
	vector<vector<stLayerInfo*>> &vecMPList,
	vector<vector<stLayerInfo*>> &vecTPList)
{
	SystemSpec_t *pSystemSpec = GetSystemSpec();
	if (pSystemSpec == nullptr) return RESULT_BAD;

	/*vector<stLayerInfo*> vecPatternList;
	vector<stLayerInfo*> vecMPList;
	vector<stLayerInfo*> vecTPList;
	m_pWndCamInfo->CheckLayerInfo_V2(*v, vecPatternList, vecMPList, vecTPList);*/

	if (m_pWndCamInfo)	
		//return m_pWndCamInfo->CheckLayerInfo(*v);	
		return m_pWndCamInfo->CheckLayerInfo_V2(*v, vecPatternList, vecSRList, vecMPList, vecTPList);
	else
		return RESULT_BAD;
}

CStringArray* CFormViewDocPane::GetLayerList(int idx)
{
	if (m_pWndCamInfo)
		return m_pWndCamInfo->GetLayerList(idx);
	else
		return nullptr;
}

UINT32 CFormViewDocPane::LoadSpecData()
{
	if (m_pWndSettings)
		return m_pWndSettings->LoadSpecData();
	else
		return RESULT_BAD;
}

UINT32	CFormViewDocPane::Refresh_JobList()
{
	if (m_pWndJobInfo)
		return m_pWndJobInfo->RefreshList();
	
	return RESULT_BAD;
}

UINT32	CFormViewDocPane::ChangeStep(int uiMenuCmd)
{
	if (m_pWndCamInfo)
		return m_pWndCamInfo->ChangeStep(uiMenuCmd);

	return RESULT_BAD;
}



UINT32 CFormViewDocPane::LoadFeatureData()
{
	if (m_pWndFeatureEdit)
	{
		return m_pWndFeatureEdit->LoadFeatureData();
	}
	else 
		return RESULT_BAD;
}

UINT32 CFormViewDocPane::LoadMoveData()
{
	if (m_pWndFeatureEdit)
	{
		return m_pWndFeatureEdit->LoadMoveData();
	}
	else
		return RESULT_BAD;
}

UINT CFormViewDocPane::LoadMove_KeyData()
{
	if (m_pWndFeatureEdit)
	{
		return m_pWndFeatureEdit->LoadMove_KeyData();
	}
	else
		return RESULT_BAD;
}
