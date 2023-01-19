#include "pch.h"
#include "MeasureInfoFormView.h"
#include "MainFrm.h"
#include <vector>

using namespace std;

#define IDC_LISTCTRL_MEASURES   1990
#define IDC_STATICTEXT			1991
#define IDC_STATICCHECK			1992

CMeasureInfoFormView::CMeasureInfoFormView()
{

}


CMeasureInfoFormView::~CMeasureInfoFormView()
{

}

BEGIN_MESSAGE_MAP(CMeasureInfoFormView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_NOTIFY(NM_CLICK, IDC_LISTCTRL_MEASURES, OnItemClick)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_LISTCTRL_MEASURES, OnItemChanging)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTCTRL_MEASURES, OnItemChangedList)
	ON_REGISTERED_MESSAGE(WM_LISTCTRL_COMBOBOX_CHANGE, OnComboBoxChange)
END_MESSAGE_MAP()


int CMeasureInfoFormView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create List:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_EDITLABELS | LVTVIF_AUTOSIZE /*| WS_VSCROLL | WS_HSCROLL*/;
	
	if (!m_ItemList.Create(dwViewStyle, rectDummy, this, IDC_LISTCTRL_MEASURES))
	{
		TRACE0("Failed to create List control \n");
		return -1;      // fail to create
	}

	if (!m_MeasureList.Create(dwViewStyle, rectDummy, this, IDC_LISTCTRL_MEASURES))
	{
		TRACE0("Failed to create List control \n");
		return -1;      // fail to create
	}

	m_ItemHeaderCtrl.SubclassWindow(m_ItemList.GetHeaderCtrl()->m_hWnd);
	m_MeasureHeaderCtrl.SubclassWindow(m_MeasureList.GetHeaderCtrl()->m_hWnd);

	m_ItemList.SetExtendedStyle(m_ItemList.GetStyle() | LVS_EX_FULLROWSELECT | LVS_EX_BORDERSELECT | LVS_OWNERDRAWFIXED | LVS_EX_TWOCLICKACTIVATE | LVS_EX_DOUBLEBUFFER);
	m_ItemList.SetReadOnlyColumns(LIST_ITEM_NAME_COL);
	m_ItemList.InsertColumn(LIST_ITEM_NAME_COL, L"ITEM", LVCFMT_CENTER, 180);
	m_ItemList.SetReadOnlyColumns(LIST_ITEM_OPTIC_COL);
	m_ItemList.InsertColumn(LIST_ITEM_OPTIC_COL, L"OPTIC", LVCFMT_CENTER, 120);
	m_ItemList.InsertColumn(LIST_ITEM_MAG_COL, L"MAG", LVCFMT_CENTER, 150);
	m_ItemList.SetComboColumns(LIST_ITEM_MAG_COL);
	m_ItemList.InsertColumn(LIST_ITEM_ZOOM_COL, L"ZOOM", LVCFMT_CENTER, 150);
	m_ItemList.SetComboColumns(LIST_ITEM_ZOOM_COL);
	m_ItemList.SetDefaultTextColor(Foreground_Color);

	m_MeasureList.SetExtendedStyle(m_MeasureList.GetStyle() | LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_BORDERSELECT | LVS_OWNERDRAWFIXED | LVS_EX_TWOCLICKACTIVATE | LVS_EX_DOUBLEBUFFER);

	m_MeasureList.InsertColumn(LIST_ITEM_COLUMN, L"ITEM", LVCFMT_CENTER, 180);
	m_MeasureList.InsertColumn(LIST_TYPE_COLUMN, L"TYPE", LVCFMT_CENTER, 120);
	m_MeasureList.InsertColumn(LIST_OPTIC_COLUMN, L"OPTIC", LVCFMT_CENTER, 100);
	m_MeasureList.InsertColumn(LIST_MAG_COLUMN, L"MAG", LVCFMT_CENTER, 100);
	m_MeasureList.InsertColumn(LIST_ZOOM_COLUMN, L"ZOOM", LVCFMT_CENTER, 100);

	m_MeasureList.InsertColumn(LIST_MIN_COLUMN, L"MIN", LVCFMT_CENTER, 100);
	m_MeasureList.InsertColumn(LIST_TARGET_COLUMN, L"TARGET", LVCFMT_CENTER, 100);
	m_MeasureList.InsertColumn(LIST_MAX_COLUMN, L"MAX", LVCFMT_CENTER, 100);

	m_MeasureList.InsertColumn(LIST_WIDTHRANGE_COLUMN, L"WR", LVCFMT_CENTER, 100);
	m_MeasureList.InsertColumn(LIST_SIDE_COLUMN, L"SIDE", LVCFMT_CENTER, 80);
	m_MeasureList.InsertColumn(LIST_LEFT_COLUMN, L"LEFT", LVCFMT_CENTER, 150);
	m_MeasureList.InsertColumn(LIST_TOP_COLUMN, L"TOP", LVCFMT_CENTER, 150);
	m_MeasureList.InsertColumn(LIST_RIGHT_COLUMN, L"RIGHT", LVCFMT_CENTER, 150);
	m_MeasureList.InsertColumn(LIST_BOT_COLUMN, L"BOT", LVCFMT_CENTER, 150);
	m_MeasureList.InsertColumn(LIST_FEATURE_TYPE, L"FEATURE", LVCFMT_CENTER, 80);

	m_MeasureList.SetReadOnlyColumns(LIST_ITEM_COLUMN);
	m_MeasureList.SetReadOnlyColumns(LIST_TYPE_COLUMN);
	m_MeasureList.SetReadOnlyColumns(LIST_OPTIC_COLUMN);
	m_MeasureList.SetReadOnlyColumns(LIST_WIDTHRANGE_COLUMN);
	m_MeasureList.SetReadOnlyColumns(LIST_SIDE_COLUMN);
	m_MeasureList.SetReadOnlyColumns(LIST_LEFT_COLUMN);
	m_MeasureList.SetReadOnlyColumns(LIST_TOP_COLUMN);
	m_MeasureList.SetReadOnlyColumns(LIST_RIGHT_COLUMN);
	m_MeasureList.SetReadOnlyColumns(LIST_BOT_COLUMN);
	m_MeasureList.SetReadOnlyColumns(LIST_FEATURE_TYPE);

	m_MeasureList.SetComboColumns(LIST_MAG_COLUMN);
	m_MeasureList.SetComboColumns(LIST_ZOOM_COLUMN);

	m_MeasureList.SetDefaultTextColor(Foreground_Color);
	
	InitializeList();

	CImageList imagelist;
	imagelist.Create(22, 22, ILC_COLOR4, 10, 10);
	m_ItemList.SetImageList(&imagelist, LVSIL_SMALL);
	m_MeasureList.SetImageList(&imagelist, LVSIL_SMALL);

	return 0;
}

void CMeasureInfoFormView::ClearItemList()
{
	if (m_nItemListCount > 0)
		m_ItemList.DeleteAllItems();
	m_nItemListCount = 0;
}

void CMeasureInfoFormView::ClearMeasureList()
{
	if (m_nMeasureListCount > 0)
		m_MeasureList.DeleteAllItems();
	m_nMeasureListCount = 0;
}

void CMeasureInfoFormView::InitializeList()
{
	ClearItemList();
	ClearMeasureList();

	m_strOpticArr.RemoveAll();
	m_strOpticArr.Add(NULL_STRING);
	m_strOpticArr.Add(OPTIC_2D_TEXT);
	m_strOpticArr.Add(OPTIC_3D_TEXT);

	m_strMagArr.RemoveAll();
	m_strMagArr.Add(NSIS_MAG_MODE_1);
	m_strMagArr.Add(NSIS_MAG_MODE_2);
	m_strMagArr.Add(NSIS_MAG_MODE_3);
	m_strMagArr.Add(NSIS_MAG_MODE_4);
	m_strMagArr.Add(NSIS_MAG_MODE_5);

	m_strZoomArr.RemoveAll();
	m_strZoomArr.Add(NSIS_ZOOM_MODE_1);
	m_strZoomArr.Add(NSIS_ZOOM_MODE_2);
	m_strZoomArr.Add(NSIS_ZOOM_MODE_3);
	m_strZoomArr.Add(NSIS_ZOOM_MODE_4);

	m_ItemList.SetComboBoxList(LIST_ITEM_MAG_COL, m_strMagArr);
	m_ItemList.SetComboBoxList(LIST_ITEM_ZOOM_COL, m_strZoomArr);

	m_ItemList.InsertItem(0, NULL_STRING);
	m_ItemList.SetItemText(0, LIST_STEP_NAME_COL, _T("PANEL ALIGN"));
	m_ItemList.SetItemText(0, LIST_ITEM_OPTIC_COL, OPTIC_2D_TEXT);
	
	CString strTmp;
	SystemSpec_t* pSysSpec = GetSystemSpec();
	if (pSysSpec && pSysSpec->sysNSIS.sysAlignLens == 0. && 
		pSysSpec->sysNSIS.sysAlignZoom == 0.)
	{
		m_ItemList.SetItemText(0, LIST_ITEM_MAG_COL, NSIS_MAG_MODE_4);
		m_ItemList.SetItemText(0, LIST_ITEM_ZOOM_COL, NSIS_ZOOM_MODE_3);
	}
	else
	{
		strTmp.Format(_T("x%.1f"), pSysSpec->sysNSIS.sysAlignLens);
		m_ItemList.SetItemText(0, LIST_ITEM_MAG_COL, strTmp);
		strTmp.Format(_T("x%.2f"), pSysSpec->sysNSIS.sysAlignZoom);
		m_ItemList.SetItemText(0, LIST_ITEM_ZOOM_COL, strTmp);
	}

	m_ItemList.InsertItem(1, NULL_STRING);
	m_ItemList.SetItemText(1, LIST_STEP_NAME_COL, _T("UNIT ALIGN"));
	m_ItemList.SetItemText(1, LIST_ITEM_OPTIC_COL, OPTIC_2D_TEXT);
	if (pSysSpec && pSysSpec->sysNSIS.sysUnitAlignLens == 0. &&
		pSysSpec->sysNSIS.sysUnitAlignZoom == 0.)
	{
		m_ItemList.SetItemText(1, LIST_ITEM_MAG_COL, NSIS_MAG_MODE_4);
		m_ItemList.SetItemText(1, LIST_ITEM_ZOOM_COL, NSIS_ZOOM_MODE_1);
	}
	else
	{
		strTmp.Format(_T("x%.1f"), pSysSpec->sysNSIS.sysUnitAlignLens);
		m_ItemList.SetItemText(1, LIST_ITEM_MAG_COL, strTmp);
		strTmp.Format(_T("x%.2f"), pSysSpec->sysNSIS.sysUnitAlignZoom);
		m_ItemList.SetItemText(1, LIST_ITEM_ZOOM_COL, strTmp);
	}

	m_ItemList.InsertItem(2, NULL_STRING);
	m_ItemList.SetItemText(2, LIST_STEP_NAME_COL, _T("NSIS MEASURE"));
	m_ItemList.SetItemText(2, LIST_ITEM_OPTIC_COL, OPTIC_3D_TEXT);
	m_ItemList.SetItemText(2, LIST_ITEM_MAG_COL, NSIS_MAG_MODE_1);
	m_ItemList.SetItemText(2, LIST_ITEM_ZOOM_COL, NSIS_ZOOM_MODE_1);
	m_nItemListCount = 3;

	if (GetUserSite() == eUserSite::eSEM)
	{
		m_ItemList.InsertItem(3, NULL_STRING);
		m_ItemList.SetItemText(3, LIST_STEP_NAME_COL, _T("SR MEASURE"));
		m_ItemList.SetItemText(3, LIST_ITEM_OPTIC_COL, OPTIC_3D_TEXT);
		m_ItemList.SetItemText(3, LIST_ITEM_MAG_COL, NSIS_MAG_MODE_1);
		m_ItemList.SetItemText(3, LIST_ITEM_ZOOM_COL, NSIS_ZOOM_MODE_1);
		m_nItemListCount = 4;
	}

	m_MeasureList.SetComboBoxList(LIST_MAG_COLUMN, m_strMagArr);
	m_MeasureList.SetComboBoxList(LIST_ZOOM_COLUMN, m_strZoomArr);
	
}

void CMeasureInfoFormView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

BOOL CMeasureInfoFormView::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}

void CMeasureInfoFormView::OnPaint()
{
	CPaintDC dc(this);
}

void CMeasureInfoFormView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL) return;

	CRect rectClient;
	GetClientRect(&rectClient);

	int nStepListHeight = (m_nItemListCount + 1) * ITEM_LIST_HEIGHT;
	if (nStepListHeight == ITEM_LIST_HEIGHT)
		nStepListHeight = rectClient.Height() / 6;
	else if (nStepListHeight >= rectClient.Height() / 2)
		nStepListHeight = rectClient.Height() / 4;

	int layerListTop = nStepListHeight - 1;
	m_ItemList.SetWindowPos(NULL, rectClient.left, 0, rectClient.Width(), nStepListHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	m_MeasureList.SetWindowPos(NULL, rectClient.left, layerListTop, rectClient.Width(), rectClient.Height() - layerListTop, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CMeasureInfoFormView::OnItemClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (GetMachineType() != eMachineType::eNSIS)
		return;

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;

	if (pNMListView == nullptr)
		return;

	if (pNMListView->hdr.hwndFrom == m_ItemList.m_hWnd)
		return;

	INT32 iRow = pNMListView->iItem;
	INT32 iCol = pNMListView->iSubItem;
	if (iCol > 1)
		return;

	RECTD rcFeatureTemp(0,0,0,0);
	FeatureType featureType = FeatureType::FeatureTypeNone;

	CString strTemp = L"";
	strTemp = m_MeasureList.GetItemText(iRow, LIST_LEFT_COLUMN);
	rcFeatureTemp.left = _ttof(strTemp);

	strTemp = m_MeasureList.GetItemText(iRow, LIST_TOP_COLUMN);
	rcFeatureTemp.top = _ttof(strTemp);

	strTemp = m_MeasureList.GetItemText(iRow, LIST_RIGHT_COLUMN);
	rcFeatureTemp.right = _ttof(strTemp);

	strTemp = m_MeasureList.GetItemText(iRow, LIST_BOT_COLUMN);
	rcFeatureTemp.bottom = _ttof(strTemp);

	strTemp = m_MeasureList.GetItemText(iRow, LIST_FEATURE_TYPE);
	featureType = static_cast<FeatureType>(_ttoi(strTemp));

	ZoomRect(featureType, rcFeatureTemp);
}

void CMeasureInfoFormView::OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = FALSE;

	if ((pNMListView->uNewState & LVIS_SELECTED))
	{
		*pResult = TRUE;
	}
}

void CMeasureInfoFormView::OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;

	if (pNMListView == nullptr)
		return;

	if (pNMListView->uOldState == 0 && pNMListView->uNewState == 0)
		return;    // No change

	// Old check box state
	BOOL bOldState = (BOOL)(((pNMListView->uOldState & LVIS_STATEIMAGEMASK) >> 12) - 1);
	if (bOldState < 0)  bOldState = 0;// On startup there's no previous state, so assign as false (unchecked)		 

	// New check box state
	BOOL bNewState = (BOOL)(((pNMListView->uNewState & LVIS_STATEIMAGEMASK) >> 12) - 1);
	if (bNewState < 0) bNewState = 0;// On non-checkbox notifications assume false		

	if (bOldState == bNewState) return; // No change in check box

	// Now bNewState holds the new check box state
	int nIndex = pNMListView->iItem;

	if (pNMListView->hdr.hwndFrom == m_ItemList.m_hWnd)
		UpdateMeasureData(TRUE, nIndex,	pNMListView->iSubItem);
	else
		UpdateMeasureData(FALSE, nIndex, pNMListView->iSubItem);

	UpdateFeatureSelect(nIndex, bNewState);	
}

LRESULT CMeasureInfoFormView::OnComboBoxChange(WPARAM wParam, LPARAM lParam)
{

	CUSTOM_NMITEM *pNMI = (CUSTOM_NMITEM*)wParam;

	if (pNMI->hwndFrom == m_ItemList)
	{
		if (pNMI->iSubItem == LIST_ITEM_MAG_COL ||
			pNMI->iSubItem == LIST_ITEM_ZOOM_COL)	// Magnification & Zoom
		{
			UpdateMeasureData(TRUE, pNMI->iItem, pNMI->iSubItem);
			m_ItemList.Invalidate();
		}
	}
	else if (pNMI->hwndFrom == m_MeasureList)
	{
		if (pNMI->iSubItem == LIST_MAG_COLUMN ||
			pNMI->iSubItem == LIST_ZOOM_COLUMN)		// Magnification & Zoom
		{			
			UpdateMeasureData(FALSE, pNMI->iItem, pNMI->iSubItem);
			m_MeasureList.Invalidate();
		}
	}
	return 0L;
}

UINT32 CMeasureInfoFormView::SetMeasureData()
{
	if (GetMachineType() != eMachineType::eNSIS) return RESULT_BAD;
	
	ClearMeasureList();

	if (GetUserSite() == eUserSite::eDAEDUCK)
	{
		_SetMeasureData_Daeduck();
	}

	

	return RESULT_GOOD;
}

UINT32 CMeasureInfoFormView::_SetMeasureData_Daeduck()
{
	SystemSpec_t* pSysSpec = GetSystemSpec();
	if (pSysSpec == nullptr)
		return RESULT_BAD;

	m_pMeasureData = GetMeasureData();
	if (m_pMeasureData == nullptr)
		return RESULT_BAD;

	m_MeasureList.SetRedraw(FALSE);
	m_MeasureList.EnableWindow(FALSE);

	//YJD START 2021.09.30 - EXCEPTION 
	//	ClearMeasureList();
	//YJD END 2021.09.30 - EXCEPTION 

	m_rcFeature.SetRectEmpty();

	m_nMeasureListCount = 0;
	CString strTxt = _T("");

	//1. Panel Align
	for (auto it : m_pMeasureData->vcAlign)
	{
		m_MeasureList.InsertItem(m_nMeasureListCount, NULL_STRING);
		m_MeasureList.SetCheck(m_nMeasureListCount, FALSE);
		m_MeasureList.SetItemText(m_nMeasureListCount, LIST_ITEM_COLUMN, _T("PANEL ALIGN"));
		//m_MeasureList.SetItemText(m_nMeasureListCount, LIST_TYPE_COLUMN, GetMeasureTypeString(static_cast<MeasureType>(it->iMeasureType)));
		m_MeasureList.SetItemText(m_nMeasureListCount, LIST_TYPE_COLUMN, it->strMeasureType);
		m_MeasureList.SetItemText(m_nMeasureListCount, LIST_OPTIC_COLUMN, OPTIC_2D_TEXT);

		//		m_MeasureList.SetItemText(m_nMeasureListCount, LIST_MAG_COLUMN, GetMagnificationString(it->bIs3D, it->iScanMode));
		//		m_MeasureList.SetItemText(m_nMeasureListCount, LIST_ZOOM_COLUMN, GetZoomString(it->bIs3D, it->iScanMode));
		m_MeasureList.SetItemText(m_nMeasureListCount, LIST_MAG_COLUMN, GetMagnificationString(it->dLens));
		m_MeasureList.SetItemText(m_nMeasureListCount, LIST_ZOOM_COLUMN, GetZoomString(it->dZoom));

		strTxt.Format(_T("%.1f"), it->stSpec.dMin);
		m_MeasureList.SetItemText(m_nMeasureListCount, LIST_MIN_COLUMN, strTxt);
		strTxt.Format(_T("%.1f"), it->stSpec.dTarget);
		m_MeasureList.SetItemText(m_nMeasureListCount, LIST_TARGET_COLUMN, strTxt);
		strTxt.Format(_T("%.1f"), it->stSpec.dMax);
		m_MeasureList.SetItemText(m_nMeasureListCount, LIST_MAX_COLUMN, strTxt);
		strTxt.Format(_T("%.1f"), it->stSpec.dRange);
		m_MeasureList.SetItemText(m_nMeasureListCount, LIST_WIDTHRANGE_COLUMN, strTxt);
		m_MeasureList.SetItemText(m_nMeasureListCount, LIST_SIDE_COLUMN, GetMeasureSideString(static_cast<MeasureSide>(it->iMeasureSide)));

		strTxt.Format(_T("%.3f"), it->MinMaxRect_In_Panel.left);
		m_MeasureList.SetItemText(m_nMeasureListCount, LIST_LEFT_COLUMN, strTxt);
		strTxt.Format(_T("%.3f"), it->MinMaxRect_In_Panel.top);
		m_MeasureList.SetItemText(m_nMeasureListCount, LIST_TOP_COLUMN, strTxt);
		strTxt.Format(_T("%.3f"), it->MinMaxRect_In_Panel.right);
		m_MeasureList.SetItemText(m_nMeasureListCount, LIST_RIGHT_COLUMN, strTxt);
		strTxt.Format(_T("%.3f"), it->MinMaxRect_In_Panel.bottom);
		m_MeasureList.SetItemText(m_nMeasureListCount, LIST_BOT_COLUMN, strTxt);

		strTxt.Format(_T("%d"), it->iFeatureType);
		m_MeasureList.SetItemText(m_nMeasureListCount, LIST_FEATURE_TYPE, strTxt);

		m_nMeasureListCount++;
	}

	//2. Unit Align + Measure Points
	for (auto it : m_pMeasureData->vcUnits)
	{
		for (auto itAlign : it->vcAlign)
		{
			m_MeasureList.InsertItem(m_nMeasureListCount, NULL_STRING);
			m_MeasureList.SetCheck(m_nMeasureListCount, FALSE);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_ITEM_COLUMN, _T("UNIT ALIGN"));
			//m_MeasureList.SetItemText(m_nMeasureListCount, LIST_TYPE_COLUMN, GetMeasureTypeString(static_cast<MeasureType>(itAlign->iMeasureType)));
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_TYPE_COLUMN, itAlign->strMeasureType);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_OPTIC_COLUMN, OPTIC_2D_TEXT);

			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_MAG_COLUMN, GetMagnificationString(itAlign->dLens));
			//			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_MAG_COLUMN, GetMagnificationString(itAlign->bIs3D, itAlign->iScanMode));
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_ZOOM_COLUMN, GetZoomString(itAlign->dZoom));
			//			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_ZOOM_COLUMN, GetZoomString(itAlign->bIs3D, itAlign->iScanMode));

			strTxt.Format(_T("%.1f"), itAlign->stSpec.dMin);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_MIN_COLUMN, strTxt);
			strTxt.Format(_T("%.1f"), itAlign->stSpec.dTarget);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_TARGET_COLUMN, strTxt);
			strTxt.Format(_T("%.1f"), itAlign->stSpec.dMax);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_MAX_COLUMN, strTxt);
			strTxt.Format(_T("%.1f"), itAlign->stSpec.dRange);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_WIDTHRANGE_COLUMN, strTxt);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_SIDE_COLUMN, GetMeasureSideString(static_cast<MeasureSide>(itAlign->iMeasureSide)));

			strTxt.Format(_T("%.3f"), itAlign->MinMaxRect_In_Panel.left);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_LEFT_COLUMN, strTxt);
			strTxt.Format(_T("%.3f"), itAlign->MinMaxRect_In_Panel.top);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_TOP_COLUMN, strTxt);
			strTxt.Format(_T("%.3f"), itAlign->MinMaxRect_In_Panel.right);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_RIGHT_COLUMN, strTxt);
			strTxt.Format(_T("%.3f"), itAlign->MinMaxRect_In_Panel.bottom);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_BOT_COLUMN, strTxt);

			strTxt.Format(_T("%d"), itAlign->iFeatureType);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_FEATURE_TYPE, strTxt);

			m_nMeasureListCount++;
		}

		for (auto itPoint : it->vcPoints)
		{
			m_MeasureList.InsertItem(m_nMeasureListCount, NULL_STRING);
			m_MeasureList.SetCheck(m_nMeasureListCount, FALSE);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_ITEM_COLUMN, _T("MEASURE POINT"));
			//m_MeasureList.SetItemText(m_nMeasureListCount, LIST_TYPE_COLUMN, GetMeasureTypeString(static_cast<MeasureType>(itPoint->iMeasureType)));
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_TYPE_COLUMN, itPoint->strMeasureType);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_OPTIC_COLUMN, OPTIC_3D_TEXT);

			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_MAG_COLUMN, GetMagnificationString(itPoint->dLens));
			//			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_MAG_COLUMN, GetMagnificationString(itPoint->bIs3D, itPoint->iScanMode));
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_ZOOM_COLUMN, GetZoomString(itPoint->dZoom));
			//			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_ZOOM_COLUMN, GetZoomString(itPoint->bIs3D, itPoint->iScanMode));

			strTxt.Format(_T("%.1f"), itPoint->stSpec.dMin);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_MIN_COLUMN, strTxt);
			strTxt.Format(_T("%.1f"), itPoint->stSpec.dTarget);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_TARGET_COLUMN, strTxt);
			strTxt.Format(_T("%.1f"), itPoint->stSpec.dMax);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_MAX_COLUMN, strTxt);
			strTxt.Format(_T("%.1f"), itPoint->stSpec.dRange);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_WIDTHRANGE_COLUMN, strTxt);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_SIDE_COLUMN, GetMeasureSideString(static_cast<MeasureSide>(itPoint->iMeasureSide)));

			strTxt.Format(_T("%.3f"), itPoint->MinMaxRect_In_Panel.left);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_LEFT_COLUMN, strTxt);
			strTxt.Format(_T("%.3f"), itPoint->MinMaxRect_In_Panel.top);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_TOP_COLUMN, strTxt);
			strTxt.Format(_T("%.3f"), itPoint->MinMaxRect_In_Panel.right);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_RIGHT_COLUMN, strTxt);
			strTxt.Format(_T("%.3f"), itPoint->MinMaxRect_In_Panel.bottom);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_BOT_COLUMN, strTxt);

			strTxt.Format(_T("%d"), itPoint->iFeatureType);
			m_MeasureList.SetItemText(m_nMeasureListCount, LIST_FEATURE_TYPE, strTxt);

			m_nMeasureListCount++;
		}
	}

	m_MeasureList.EnableWindow(TRUE);
	m_MeasureList.SetRedraw(TRUE);

	if (m_nMeasureListCount <= 0)
		return RESULT_BAD;

	return RESULT_GOOD;
}
//
//UINT32 CMeasureInfoFormView::_SetMeasureData_Sem(eRecipeMode rcpMode, eViewType viewType, INT32 iSelRow, INT32 iSelCol)
//{
//	m_iSelRow = m_iSelCol = -1;
//	//YJD END 2021.09.30 - EXCEPTION 
//
//	if (m_eViewType != viewType ||
//		(iSelRow < 0 && iSelCol < 0))
//		DrawZoomInit();
//
//	UserSetInfo_t* pUserSetInfo = GetUserSetInfo();
//	if (pUserSetInfo == nullptr)
//		return RESULT_BAD;
//
//	SystemSpec_t* pSysSpec = GetSystemSpec();
//	if (pSysSpec == nullptr)
//		return RESULT_BAD;
//
//	m_pPanelInfo = GetProductInfo();
//	if (m_pPanelInfo == nullptr)
//		return RESULT_BAD;
//
//	m_pMeasureData = GetMeasureData();
//	if (m_pMeasureData == nullptr)
//		return RESULT_BAD;
//
//	UserLayerSet_t* pUserLayerSet = GetUserLayerSet();
//	if (pUserLayerSet == nullptr)
//		return RESULT_BAD;
//
//	m_MeasureList.SetRedraw(FALSE);
//	m_MeasureList.EnableWindow(FALSE);
//
//	//YJD START 2021.09.30 - EXCEPTION 
//	//	ClearMeasureList();
//	//YJD END 2021.09.30 - EXCEPTION 
//
//	m_MeasureList.SetColumnWidth(LIST_TYPE_COLUMN_SEM, LIST_TYPE_WIDTH);
//	m_MeasureList.SetColumnWidth(LIST_OPTIC_COLUMN_SEM, LIST_COLUMN_WIDTH);
//	m_MeasureList.SetColumnWidth(LIST_MAG_COLUMN_SEM, LIST_COLUMN_WIDTH);
//	m_MeasureList.SetColumnWidth(LIST_ZOOM_COLUMN_SEM, LIST_COLUMN_WIDTH);
//	m_MeasureList.SetColumnWidth(LIST_OOS_MIN_COLUMN_SEM, LIST_COLUMN_WIDTH);
//	m_MeasureList.SetColumnWidth(LIST_OOS_MAX_COLUMN_SEM, LIST_COLUMN_WIDTH);
//	m_MeasureList.SetColumnWidth(LIST_OOR_MIN_COLUMN_SEM, LIST_COLUMN_WIDTH);
//	m_MeasureList.SetColumnWidth(LIST_OOR_MAX_COLUMN_SEM, LIST_COLUMN_WIDTH);
//	m_MeasureList.SetColumnWidth(LIST_OOC_MIN_COLUMN_SEM, LIST_COLUMN_WIDTH);
//	m_MeasureList.SetColumnWidth(LIST_OOC_MAX_COLUMN_SEM, LIST_COLUMN_WIDTH);
//	m_MeasureList.SetColumnWidth(LIST_WIDTHRANGE_COLUMN_SEM, LIST_COLUMN_WIDTH);
//	m_MeasureList.SetColumnWidth(LIST_SIDE_COLUMN_SEM, LIST_COLUMN_WIDTH);
//
//	m_rcFeature.SetRectEmpty();
//
//	RECTD rcRect(0, 0, 0, 0);
//	FeatureType fType = FeatureType::FeatureTypeNone;
//
//	// 2022.05.02
//	// 주석추가 김준호
//	// setmeasureData 함수에서 rcpMode 같이 넘겨서 주석처리
//	//eRecipeMode rcpMode = pUserSetInfo->rcpMode;
//
//	// 2022.04.28
//	// 주석추가 김준호
//	// if문 rcpMode 경우 추가
//	if (rcpMode == eRecipeMode::RecipeModeNone ||
//		rcpMode <= eRecipeMode::eRecipe_Align_Panel || 
//		rcpMode <= eRecipeMode::eRecipe_Align_Strip ||
//		rcpMode <= eRecipeMode::eRecipe_Align_Unit)
//	{
//		Fill_AlignInfo(viewType, iSelRow, iSelCol, rcRect);
//	}	
//	else if (rcpMode <= eRecipeMode::eRecipe_MeasureNSIS_Panel || 
//			 rcpMode <= eRecipeMode::eRecipe_MeasureNSIS_Strip ||
//			 rcpMode <= eRecipeMode::eRecipe_MeasureNSIS_Unit)
//	{	
//		Fill_NSISMeasureInfo(viewType, iSelRow, iSelCol, rcRect);
//	}
//	else if (rcpMode <= eRecipeMode::eRecipe_MeasureSR_Panel || 
//			 rcpMode <= eRecipeMode::eRecipe_MeasureSR_Strip ||
//			 rcpMode <= eRecipeMode::eRecipe_MeasureSR_Unit)
//	{
//		Fill_SRMeasureInfo(viewType, iSelRow, iSelCol, rcRect);
//	}
//
//	SetDrawOnly_MeasureUnit(TRUE);
//
//	if (!rcRect.IsRectNull())
//		ZoomRect(fType, rcRect);
//
//	m_MeasureList.EnableWindow(TRUE);
//	m_MeasureList.SetRedraw(TRUE);
//
//	m_eViewType = viewType;
//	m_iSelRow = iSelRow;
//	m_iSelCol = iSelCol;
//	return RESULT_GOOD;
//}
//
UINT32 CMeasureInfoFormView::UpdateMeasureData(BOOL bIsItemList, int iSelRow, int iSelCol)
{
	CMainFrame*	pMainFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	if (pMainFrame == nullptr)
		return RESULT_BAD;

	if (m_pMeasureData == nullptr)
		return RESULT_BAD;

	if (bIsItemList)
	{
		if (iSelRow < 0 || iSelRow >= static_cast<int>(m_nItemListCount))
			return RESULT_BAD;

		if (iSelCol != LIST_ITEM_MAG_COL && iSelCol != LIST_ITEM_ZOOM_COL)
			return RESULT_BAD;		
	}
	else
	{
		if (iSelRow < 0 || iSelRow >= static_cast<int>(m_nMeasureListCount))
			return RESULT_BAD;

		if (iSelCol != LIST_MAG_COLUMN && iSelCol != LIST_ZOOM_COLUMN)
			return RESULT_BAD;
	}

	SystemSpec_t* pSysSpec = GetSystemSpec();
	if (pSysSpec == nullptr)
		return RESULT_BAD;

	CString strTemp = L"";

	// 2022.05.02
	// 주석추가 김준호
	// eRecipeMode 추가로 인해 상단으로 이동
	UserSetInfo_t* pUserSetInfo = GetUserSetInfo();
	if (pUserSetInfo == nullptr)
		return RESULT_BAD;

	eRecipeMode rcpMode = pUserSetInfo->rcpMode;
	if (rcpMode == eRecipeMode::RecipeModeNone)
		return RESULT_GOOD; 
	
	if (bIsItemList)
	{		
		strTemp = m_ItemList.GetItemText(iSelRow, LIST_ITEM_MAG_COL);
		strTemp.Delete(0, 1);
		strTemp.Trim();
		double dMag = _ttof(strTemp);

		strTemp = m_ItemList.GetItemText(iSelRow, LIST_ITEM_ZOOM_COL);
		strTemp.Delete(0, 1);
		strTemp.Trim();
		double dZoom = _ttof(strTemp);		

		if (iSelRow == 0)
		{
			if (pSysSpec->sysNSIS.sysAlignLens == 0. && pSysSpec->sysNSIS.sysAlignZoom == 0.)
			{
				for (auto it : m_pMeasureData->vcAlign)
				{
					CheckLensZoom(true, it, dMag, dZoom);
				}
			}
			else
			{
				strTemp.Format(_T("x%.1f"), pSysSpec->sysNSIS.sysAlignLens);
				m_ItemList.SetItemText(iSelRow, LIST_ITEM_MAG_COL, strTemp);
				strTemp.Format(_T("x%.2f"), pSysSpec->sysNSIS.sysAlignZoom);
				m_ItemList.SetItemText(iSelRow, LIST_ITEM_ZOOM_COL, strTemp);
			}
		}
		else
		{
			for (auto it : m_pMeasureData->vcUnits)
			{
				if (iSelRow == 1)
				{
					if (pSysSpec->sysNSIS.sysUnitAlignLens == 0. && pSysSpec->sysNSIS.sysUnitAlignZoom == 0.)
					{
						for (auto itAlign : it->vcAlign)
						{
							CheckLensZoom(true, itAlign, dMag, dZoom);
						}
					}
					else
					{
						strTemp.Format(_T("x%.1f"), pSysSpec->sysNSIS.sysUnitAlignLens);
						m_ItemList.SetItemText(iSelRow, LIST_ITEM_MAG_COL, strTemp);
						strTemp.Format(_T("x%.2f"), pSysSpec->sysNSIS.sysUnitAlignZoom);
						m_ItemList.SetItemText(iSelRow, LIST_ITEM_ZOOM_COL, strTemp);
					}
				}
				else if (iSelRow == 2)
				{
					for (auto itMeasure : it->vcPoints)
					{
						for (int i = 0; i < m_strZoomArr.GetCount(); i++)
						{
							strTemp = m_strZoomArr.GetAt(i);
							strTemp.Delete(0, 1);
							strTemp.Trim();
							dZoom = _ttof(strTemp);

							if (RESULT_GOOD == CheckLensZoom(false, itMeasure, dMag, dZoom))
								break;
						}
					}
				}
				else if (iSelRow == 3)
				{
					for (auto itMeasure : it->vcPoints_SR)
					{
						for (int i = 0; i < m_strZoomArr.GetCount(); i++)
						{
							strTemp = m_strZoomArr.GetAt(i);
							strTemp.Delete(0, 1);
							strTemp.Trim();
							dZoom = _ttof(strTemp);

							if (RESULT_GOOD == CheckLensZoom(false, itMeasure, dMag, dZoom))
								break;
						}
					}
				}
			}
		}
	}
	else
	{
		/*UserSetInfo_t* pUserSetInfo = GetUserSetInfo();
		if (pUserSetInfo == nullptr)
			return RESULT_BAD;

		eRecipeMode rcpMode = pUserSetInfo->rcpMode;
		if (rcpMode == eRecipeMode::RecipeModeNone)
			return RESULT_GOOD;*/

		strTemp = m_MeasureList.GetItemText(iSelRow, LIST_MAG_COLUMN);
		strTemp.Delete(0, 1);
		strTemp.Trim();
		double dMag = _ttof(strTemp);

		strTemp = m_MeasureList.GetItemText(iSelRow, LIST_ZOOM_COLUMN);
		strTemp.Delete(0, 1);
		strTemp.Trim();
		double dZoom = _ttof(strTemp);

		if (m_iSelRow < 0 && m_iSelCol < 0)
		{
			if (rcpMode == eRecipeMode::eRecipe_Align_Panel)
			{
				if (iSelRow >= 0 && iSelRow < m_pMeasureData->vcAlign.size())
				{
					CheckLensZoom(true, m_pMeasureData->vcAlign[iSelRow], dMag, dZoom);
					
					strTemp.Format(_T("x%.1f"), m_pMeasureData->vcAlign[iSelRow]->dLens);
					m_MeasureList.SetItemText(iSelRow, LIST_MAG_COLUMN, strTemp);
					strTemp.Format(_T("x%.2f"), m_pMeasureData->vcAlign[iSelRow]->dZoom);
					m_MeasureList.SetItemText(iSelRow, LIST_ZOOM_COLUMN, strTemp);
				}
			}
		}
		else
		{
			INT32 iStripIdx = -1, iBlockIdx = -1, iUnitIdx = -1;
			RECTD rcRect(0, 0, 0, 0);

			
				UnitInfo_t* pUnit = GetUnitIndex_In_Panel(m_iSelRow, m_iSelCol, iStripIdx, iBlockIdx, iUnitIdx, rcRect);
				if (pUnit == nullptr)
					return RESULT_BAD;
			

			for (auto it : m_pMeasureData->vcUnits)
			{
				

				if (rcpMode == eRecipeMode::eRecipe_Align_Panel)
				{
					if (iSelRow >= 0 && iSelRow < it->vcAlign.size())
					{
						CheckLensZoom(true, it->vcAlign[iSelRow], dMag, dZoom);
						
						strTemp.Format(_T("x%.1f"), it->vcAlign[iSelRow]->dLens);
						m_MeasureList.SetItemText(iSelRow, LIST_MAG_COLUMN, strTemp);
						strTemp.Format(_T("x%.2f"), it->vcAlign[iSelRow]->dZoom);
						m_MeasureList.SetItemText(iSelRow, LIST_ZOOM_COLUMN, strTemp);
					}
				}
				else if (rcpMode == eRecipeMode::eRecipe_MeasureNSIS_Panel)
				{
					if (iSelRow >= 0 && iSelRow < it->vcPoints.size())
					{
						for (int i = 0; i < m_strZoomArr.GetCount(); i++)
						{
							strTemp = m_strZoomArr.GetAt(i);
							strTemp.Delete(0, 1);
							strTemp.Trim();
							dZoom = _ttof(strTemp);

							if (RESULT_GOOD == CheckLensZoom(true, it->vcPoints[iSelRow], dMag, dZoom))
								break;
						}
					}
				}
				else if (rcpMode == eRecipeMode::eRecipe_MeasureSR_Panel)
				{
					if (iSelRow >= 0 && iSelRow < it->vcPoints_SR.size())
					{
						for (int i = 0; i < m_strZoomArr.GetCount(); i++)
						{
							strTemp = m_strZoomArr.GetAt(i);
							strTemp.Delete(0, 1);
							strTemp.Trim();
							dZoom = _ttof(strTemp);

							if (RESULT_GOOD == CheckLensZoom(true, it->vcPoints_SR[iSelRow], dMag, dZoom))
								break;
						}
					}
				}
			}
		}
	}
	
	return SetMeasureData();
}

CString CMeasureInfoFormView::GetMeasureTypeString(MeasureType type)
{
	if (type == MeasureType::align)
		return PANEL_ALIGN_TXT;
	else if (type == MeasureType::align_plating)
		return PANEL_ALIGN_PLATING_TXT;
	else if (type == MeasureType::fm_p)
		return UNIT_ALIGN_TXT_P;
	else
	{
		if (type == MeasureType::t_width)				return _T("T_WIDTH");
		else if (type == MeasureType::t_space)			return _T("T_SPACE");
		else if (type == MeasureType::bf_width)			return _T("BF_WIDTH");
		else if (type == MeasureType::bf_space)			return _T("BF_SPACE");
		else if (type == MeasureType::bol_width)		return _T("BOL_WIDTH");
		else if (type == MeasureType::bol_space)		return _T("BOL_SPACE");
		else if (type == MeasureType::thickness)		return _T("THICKNESS");
		else if (type == MeasureType::dimple)			return _T("DIMPLE");
		else if (type == MeasureType::ball_width)		return _T("BALL_WIDTH");
		else if (type == MeasureType::bump_width)		return _T("BUMP_WIDTH");
		else if (type == MeasureType::imp_space_A)		return _T("IMP_SPACE_A");
		else if (type == MeasureType::imp_space_B)		return _T("IMP_SPACE_B");
		else if (type == MeasureType::imp_width_A)		return _T("IMP_WIDTH_A");
		else if (type == MeasureType::imp_width_B)		return _T("IMP_WIDTH_B");
		else if (type == MeasureType::bol_width_x)		return _T("BOL_WIDTH_X");
		else if (type == MeasureType::bol_width_y)		return _T("BOL_WIDTH_Y");
		else if (type == MeasureType::bol_space_x)		return _T("BOL_SPACE_X");
		else if (type == MeasureType::bol_space_y)		return _T("BOL_SPACE_Y");
		else if (type == MeasureType::bol_pitch)		return _T("BOL_PITCH");
		else if (type == MeasureType::via_width)		return _T("VIA_WIDTH");
		else if (type == MeasureType::r_depth)			return _T("R_DEPTH");
		else if (type == MeasureType::pad)				return _T("PAD");
		else if (type == MeasureType::ball_pitch)		return _T("BALL_PITCH");
		else if (type == MeasureType::thickness_cu)		return _T("THICKNESS_CU");
		else if (type == MeasureType::thickness_pad)	return _T("THICKNESS_PAD");
		else
			return _T("UKNOWN");
	}
}

CString CMeasureInfoFormView::GetMeasureSideString(MeasureSide side)
{
	if (side == MeasureSide::top)
		return _T("TOP");
	else
		return _T("BOT");
}

CString CMeasureInfoFormView::GetMagnificationString(BOOL bIs3D, INT8 iScanMode)
{
	CString strMag = _T("");

	if (bIs3D)
	{
		if (iScanMode >= MAX_3D_MODE_NUM || iScanMode < 0)
			return strMag;

		switch (iScanMode)
		{
			case 0: case 1: case 2: strMag = NSIS_MAG_MODE_1; break;
			case 3: case 4: case 5: strMag = NSIS_MAG_MODE_2; break;
			case 6: case 7: case 8: strMag = NSIS_MAG_MODE_3; break;
		}
	}
	else
	{
		if (iScanMode >= MAX_2D_MODE_NUM || iScanMode < 0)
			return strMag;

		switch (iScanMode)
		{
			case 0: case 1: case 2: strMag = NSIS_MAG_MODE_4; break;
			case 3: case 4: case 5: strMag = NSIS_MAG_MODE_5; break;
		}
	}

	return strMag;
}

CString CMeasureInfoFormView::GetMagnificationString(double dLens)
{
	CString strMag;
	strMag.Format(_T("x%.1lf"), dLens);

	return strMag;
}

CString CMeasureInfoFormView::GetZoomString(BOOL bIs3D, INT8 iScanMode)
{
	CString strZoom = _T("");

	if (bIs3D)
	{
		if (iScanMode >= MAX_3D_MODE_NUM || iScanMode < 0)
			return strZoom;

		switch (iScanMode)
		{
			case 0: case 3: case 6: strZoom = NSIS_ZOOM_MODE_1; break;
			case 1: case 4: case 7: strZoom = NSIS_ZOOM_MODE_2; break;
			case 2: case 5: case 8: strZoom = NSIS_ZOOM_MODE_3; break;
		}
	}
	else
	{
		if (iScanMode >= MAX_2D_MODE_NUM || iScanMode < 0)
			return strZoom;

		switch (iScanMode)
		{
			case 0: case 3: strZoom = NSIS_ZOOM_MODE_1; break;
			case 1: case 4: strZoom = NSIS_ZOOM_MODE_2; break;
			case 2: case 5: strZoom = NSIS_ZOOM_MODE_3; break;
		}
	}

	return strZoom;
}

CString CMeasureInfoFormView::GetZoomString(double dZoom)
{
	CString strMag;
	strMag.Format(_T("x%.2lf"), dZoom);

	return strMag;
}

INT8 CMeasureInfoFormView::SetScanMode(BOOL bIs3D, CString strMag, CString strZoom)
{
	INT8 iScanMode = -1;

	if (bIs3D)
	{
		if (strMag.CompareNoCase(NSIS_MAG_MODE_1) == 0)
		{
			if (strZoom.CompareNoCase(NSIS_ZOOM_MODE_1) == 0) iScanMode = 0;
			else if (strZoom.CompareNoCase(NSIS_ZOOM_MODE_2) == 0) iScanMode = 1;
			else if (strZoom.CompareNoCase(NSIS_ZOOM_MODE_3) == 0) iScanMode = 2;
		}
		else if (strMag.CompareNoCase(NSIS_MAG_MODE_2) == 0)
		{
			if (strZoom.CompareNoCase(NSIS_ZOOM_MODE_1) == 0) iScanMode = 3;
			else if (strZoom.CompareNoCase(NSIS_ZOOM_MODE_2) == 0) iScanMode = 4;
			else if (strZoom.CompareNoCase(NSIS_ZOOM_MODE_3) == 0) iScanMode = 5;
		}
		else if (strMag.CompareNoCase(NSIS_MAG_MODE_3) == 0)
		{
			if (strZoom.CompareNoCase(NSIS_ZOOM_MODE_1) == 0) iScanMode = 6;
			else if (strZoom.CompareNoCase(NSIS_ZOOM_MODE_2) == 0) iScanMode = 7;
			else if (strZoom.CompareNoCase(NSIS_ZOOM_MODE_3) == 0) iScanMode = 8;
		}
	}
	else
	{
		if (strMag.CompareNoCase(NSIS_MAG_MODE_4) == 0)
		{
			if (strZoom.CompareNoCase(NSIS_ZOOM_MODE_1) == 0) iScanMode = 0;
			else if (strZoom.CompareNoCase(NSIS_ZOOM_MODE_2) == 0) iScanMode = 1;
			else if (strZoom.CompareNoCase(NSIS_ZOOM_MODE_3) == 0) iScanMode = 2;
		}
		else if (strMag.CompareNoCase(NSIS_MAG_MODE_5) == 0)
		{
			if (strZoom.CompareNoCase(NSIS_ZOOM_MODE_1) == 0) iScanMode = 3;
			else if (strZoom.CompareNoCase(NSIS_ZOOM_MODE_2) == 0) iScanMode = 4;
			else if (strZoom.CompareNoCase(NSIS_ZOOM_MODE_3) == 0) iScanMode = 5;
		}
	}

	return iScanMode;
}

void CMeasureInfoFormView::UpdateFeatureSelect(IN const int &nLineIndex, IN const BOOL &bChecked)
{//List에 Check 정보를 Viewer에 보내어 Select 하도록 한다.

	UserSetInfo_t* pUserSetInfo = GetUserSetInfo();
	if (pUserSetInfo == nullptr)
		return;

	eRecipeMode rcpMode = pUserSetInfo->rcpMode;
	vector<CFeature *> vecFeature;

// 	if (rcpMode <= eRecipeMode::eRecipe_Align_Unit)
// 	{
// 		Fill_AlignFeature(m_eViewType, m_iSelRow, m_iSelCol, vecFeature);
// 	}
// 	else if (rcpMode <= eRecipeMode::eRecipe_MeasureNSIS_Unit)
// 	{
// 		Fill_NSISMeasureFeature(m_eViewType, m_iSelRow, m_iSelCol, vecFeature);
// 	}
// 	else if (rcpMode <= eRecipeMode::eRecipe_MeasureSR_Unit)
// 	{
// 		Fill_SRMeasureFeature(m_eViewType, m_iSelRow, m_iSelCol, vecFeature);
// 	}

	CFeature *pFeature = nullptr;
	if (nLineIndex >= 0 && nLineIndex < vecFeature.size())
		pFeature = (CFeature*)(vecFeature[nLineIndex]);

	if (pFeature != nullptr)
	{//
		switch (pFeature->m_eType)
		{
		case FeatureType::L:
		{
			CFeatureL *pFeatureL = (CFeatureL*)pFeature;
			pFeatureL->m_bHighlight = bChecked;
		}
		break;
		case FeatureType::P:
		{
			CFeatureP *pFeatureP = (CFeatureP*)pFeature;
			pFeatureP->m_bHighlight = bChecked;
		}
		break;
		case FeatureType::A:
		{
			CFeatureA *pFeatureA = (CFeatureA*)pFeature;
			pFeatureA->m_bHighlight = bChecked;
		}
		break;
		case FeatureType::S:
		{
			CFeatureS *pFeatureS = (CFeatureS*)pFeature;
			int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
			for (int nObOe = 0; nObOe < nObOeCount; nObOe++)
			{
				CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nObOe);
				if (pObOe == nullptr) continue;
				pObOe->m_bHighlight = bChecked;
			}
		}
		break;
		default:
		{
			break;
		}
		}

		DrawCamData(FALSE);

// 		if (!m_rcFeature.IsRectNull())
// 			DrawZoomRect(m_rcFeature);

		UpdateSelectInfo();
	}	
}

UINT32 CMeasureInfoFormView::ResetFeatureSelect_UI()
{
	int nItemCount = m_MeasureList.GetItemCount();

	m_MeasureList.SetRedraw(FALSE);
	m_MeasureList.EnableWindow(FALSE);

	for (int i = 0; i < nItemCount; i++)
	{
		m_MeasureList.SetCheck(i, FALSE);
	}

	m_MeasureList.EnableWindow(TRUE);
	m_MeasureList.SetRedraw(TRUE);

	return RESULT_GOOD;
}

UINT32 CMeasureInfoFormView::UpdateFeatureSelect_UI()
{
	if (m_pMeasureData == nullptr) return RESULT_BAD;
	
	if (GetMachineType() != eMachineType::eNSIS) return RESULT_BAD;

	m_vecFeature.clear();

	if (GetUserSite() == eUserSite::eDAEDUCK)
	{
		UpdateFeatureSelect_UI_Daeduck();
	}

	int nCount = static_cast<int>(m_vecFeature.size());
	int nLightCount = m_MeasureList.GetItemCount();
	if (nCount != nLightCount) 
		return RESULT_BAD;

	m_MeasureList.SetRedraw(FALSE);
	m_MeasureList.EnableWindow(FALSE);

	BOOL bHighlight;
	for (int i = 0; i < nCount; i++)
	{
		if (m_vecFeature[i] == nullptr ) continue;
		CFeature *pFeature = m_vecFeature[i];

		bHighlight = FALSE;
		switch (pFeature->m_eType)
		{
		case FeatureType::L:
		{
			CFeatureL *pFeatureL = (CFeatureL*)pFeature;
			bHighlight = pFeatureL->m_bHighlight;
		}
		break;
		case FeatureType::P:
		{
			CFeatureP *pFeatureP = (CFeatureP*)pFeature;
			bHighlight = pFeatureP->m_bHighlight;
		}
		break;
		case FeatureType::A:
		{
			CFeatureA *pFeatureA = (CFeatureA*)pFeature;
			bHighlight = pFeatureA->m_bHighlight;
		}
		break;
		case FeatureType::S:
		{
			CFeatureS *pFeatureS = (CFeatureS*)pFeature;
			int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
			for (int nObOe = 0; nObOe < nObOeCount; nObOe++)
			{
				CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nObOe);
				if (pObOe == nullptr) continue;
				bHighlight |= (BOOL)(pObOe->m_bHighlight);
			}
		}
		break;
		default:
		{
			break;
		}
		}

		m_MeasureList.SetCheck(i, bHighlight);
	}

	m_MeasureList.EnableWindow(TRUE);
	m_MeasureList.SetRedraw(TRUE);

	return RESULT_GOOD;
}

UINT32 CMeasureInfoFormView::UpdateFeatureSelect_UI_Daeduck()
{
	//1. Panel Align
	for (auto it : m_pMeasureData->vcAlign)
	{
		if (it->pFeature == nullptr) continue;

		m_vecFeature.push_back((CFeature*)it->pFeature);
	}

	//2. Unit Align + Measure Points
	for (auto it : m_pMeasureData->vcUnits)
	{
		for (auto itAlign : it->vcAlign)
		{
			if (itAlign == nullptr) continue;

			m_vecFeature.push_back((CFeature*)itAlign->pFeature);
		}

		for (auto itPoint : it->vcPoints)
		{
			if (itPoint == nullptr) continue;

			m_vecFeature.push_back((CFeature*)itPoint->pFeature);
		}
	}

	return RESULT_GOOD;
}

CString CMeasureInfoFormView::GetMeasureLayerName()
{
	UserLayerSet_t* pUserLayerSet = GetUserLayerSet();
	if (pUserLayerSet == nullptr) return CString();

	SystemSpec_t* pSystemSpec = GetSystemSpec();
	if (pSystemSpec == nullptr) return CString();
	eMachineType eMcType = pSystemSpec->sysBasic.McType;
	
	CString strMeasureLayerName = _T("");
	int nCount = static_cast<int>(pUserLayerSet->vcLayerSet.size());
	for (int i = 0; i < nCount; i++)
	{
		//NSIS일 경우 Measure Layer만 편집
		if (eMcType == eMachineType::eNSIS &&
			pUserLayerSet->vcLayerSet[i]->strDefine != MEASURE_LAYER_TXT)
		{
			continue;
		}

		if (pUserLayerSet->vcLayerSet[i]->bCheck == true)
		{
			strMeasureLayerName = pUserLayerSet->vcLayerSet[i]->strLayer;

			break;
		}

	}

	return strMeasureLayerName;
}

UnitInfo_t* CMeasureInfoFormView::GetUnitIndex_In_Panel(INT32 iRow, INT32 iCol, INT32 &iStripIdx, INT32 &iBlockIdx, INT32 &iUnitIdx, RECTD &rcDrawRect)
{
	iStripIdx = -1;
	iBlockIdx = -1;
	iUnitIdx = -1;

	if (iRow < 0 || iCol < 0)
		return nullptr;

	if (m_pPanelInfo->iRowStripNumInPanel <= 0 || m_pPanelInfo->iColStripNumInPanel <= 0)
		return nullptr;

	INT32 iRowUnitNumInStrip = (INT32)(m_pPanelInfo->iRowUnitNumInPanel / m_pPanelInfo->iRowStripNumInPanel);
	INT32 iColUnitNumInStrip = (INT32)(m_pPanelInfo->iColUnitNumInPanel / m_pPanelInfo->iColStripNumInPanel);
	if (iRowUnitNumInStrip <= 0 || iColUnitNumInStrip <= 0)
		return nullptr;

	INT32 iStripRow = iRow / iRowUnitNumInStrip;
	INT32 iStripCol = iCol / iColUnitNumInStrip;

	int iSIdx = iStripCol * (int)m_pPanelInfo->iRowStripNumInPanel + iStripRow;
	if (iSIdx >= m_pPanelInfo->vcStripInfo.size())
		return nullptr;

	StripInfo_t *pStrip = m_pPanelInfo->vcStripInfo[iSIdx];
	if (pStrip == nullptr)
		return nullptr;

	int iUnitRowNumInStrip = static_cast<int>(pStrip->iRowUnitNumInStrip);
	int iUnitColNumInStrip = static_cast<int>(pStrip->iColUnitNumInStrip);

	if (iUnitRowNumInStrip <= 0 || iUnitColNumInStrip <= 0)
		return nullptr;

	iRow -= (iStripRow * iRowUnitNumInStrip);
	iCol -= (iStripCol * iColUnitNumInStrip);

	int iUIdx = iCol * iUnitRowNumInStrip + iRow;
	if (iUIdx >= pStrip->vcUnitInfo.size())
		return nullptr;

	if (pStrip->vcUnitInfo[iUIdx] == nullptr)
		return nullptr;

	iStripIdx = iSIdx;
	iUnitIdx = iUIdx + (iSIdx * iUnitRowNumInStrip * iUnitColNumInStrip);
	rcDrawRect = pStrip->vcUnitInfo[iUIdx]->rcRect;

	return pStrip->vcUnitInfo[iUIdx];
}

void CMeasureInfoFormView::ZoomRect(FeatureType &type, RECTD &rcRect)
{
	UserLayerSet_t* pUserLayerSet = GetUserLayerSet();
	if (pUserLayerSet == nullptr) return;

	if (GetCurStepIndex() != pUserLayerSet->nPanelStepIdx)
	{
//		AfxMessageBox(_T("Panel 뷰로 이동하여 주세요."), MB_OK | MB_ICONEXCLAMATION);
//		Strip & Unit Step 뷰 지원하자....
		return;
	}

	double dWidth = rcRect.width();
	double dHeight = rcRect.height();
	double dZoomMarginRatio = 7.0;

	SystemSpec_t* pSystemSpec = GetSystemSpec();
	if (pSystemSpec)
	{
		dZoomMarginRatio = pSystemSpec->sysNSIS.sysZoomMarginRatio;
		if (dZoomMarginRatio <= 0)
			dZoomMarginRatio = 7.0;
	}

	if (type == FeatureType::FeatureTypeNone)
		dZoomMarginRatio = 0.1;
	else if (type != FeatureType::L)
		dZoomMarginRatio /= 2.0;

	//Feature 크기의 10%를 더 보여준다.	
	rcRect.left = rcRect.left - (dWidth * dZoomMarginRatio);
	rcRect.top = rcRect.top - (dHeight * dZoomMarginRatio);
	rcRect.right = rcRect.right + (dWidth * dZoomMarginRatio);
	rcRect.bottom = rcRect.bottom + (dHeight * dZoomMarginRatio);

	UserSetInfo_t* pstUserSetInfo = GetUserSetInfo();
	if (pstUserSetInfo)
	{
		Orient ori = Orient::Mir0Deg;
		double dAngle = pstUserSetInfo->dAngle, dTemp;
		bool bMirror = pstUserSetInfo->bMirror;

		CDataManager::GetOrient(dAngle, bMirror, ori);

		CRotMirrArr rotMirrArr;
		rotMirrArr.InputData(0, 0, CDataManager::ReverseOrient(ori), pstUserSetInfo->mirrorDir);
		rotMirrArr.FinalPoint(&m_rcFeature.left, &m_rcFeature.top, rcRect.left, rcRect.top);
		rotMirrArr.FinalPoint(&m_rcFeature.right, &m_rcFeature.bottom, rcRect.right, rcRect.bottom);
		rotMirrArr.ClearAll();

		if (bMirror)
		{
			if (dAngle == 0.0)
			{
				SWAP(m_rcFeature.left, m_rcFeature.right, dTemp);
				SWAP(m_rcFeature.top, m_rcFeature.bottom, dTemp);
			}
			else if (dAngle == 90.0)
			{
				m_rcFeature.left *= -1.0;
				m_rcFeature.right *= -1.0;

				SWAP(m_rcFeature.top, m_rcFeature.bottom, dTemp);
				m_rcFeature.top *= -1.0;
				m_rcFeature.bottom *= -1.0;
			}
			else if (dAngle == 270.0)
			{
				SWAP(m_rcFeature.left, m_rcFeature.right, dTemp);
				m_rcFeature.left *= -1.0;
				m_rcFeature.right *= -1.0;

				m_rcFeature.top *= -1.0;
				m_rcFeature.bottom *= -1.0;
			}
		}
	}

	DrawZoomRect(m_rcFeature);
}

UnitInfo_t* CMeasureInfoFormView::GetUnitIndex_In_Strip(INT32 iRow, INT32 iCol, INT32 &iStripIdx, INT32 &iBlockIdx, INT32 &iUnitIdx, RECTD &rcDrawRect)
{
	iBlockIdx = -1;
	iUnitIdx = -1;

	if (iRow < 0 || iCol < 0)
		return nullptr;

	int iSIdx = m_pPanelInfo->iSelStripCol * (int)m_pPanelInfo->iRowStripNumInPanel + m_pPanelInfo->iSelStripRow;
	if (iSIdx >= m_pPanelInfo->vcStripInfo.size())
		return nullptr;

	StripInfo_t *pStrip = m_pPanelInfo->vcStripInfo[iSIdx];
	if (pStrip == nullptr)
		return nullptr;

	int iUnitRowNumInStrip = static_cast<int>(pStrip->iRowUnitNumInStrip);
	int iUnitColNumInStrip = static_cast<int>(pStrip->iColUnitNumInStrip);

	if (iUnitRowNumInStrip <= 0 || iUnitColNumInStrip <= 0)
		return nullptr;

	int iIdx = iCol * pStrip->iRowUnitNumInStrip + iRow;
	if (iIdx >= pStrip->vcUnitInfo.size())
		return nullptr;

	if (pStrip->vcUnitInfo[iIdx] == nullptr)
		return nullptr;

	iStripIdx = iSIdx;
	iUnitIdx = iIdx + (iSIdx * iUnitRowNumInStrip * iUnitColNumInStrip);
	rcDrawRect = pStrip->vcUnitInfo[iIdx]->rcRect;

	return pStrip->vcUnitInfo[iIdx];
}


UINT32 CMeasureInfoFormView::CheckLensZoom(bool bIsAlign, MeasureItem_t* pItem, double &dMag, double &dZoom)
{
	if (pItem == nullptr)
		return RESULT_BAD;

	SystemSpec_t* pSystemSpec = GetSystemSpec();
	if (pSystemSpec == nullptr)
		return RESULT_BAD;

	std::vector<NSIS_CameraSpec_t*>* pSysCamera = nullptr;
	if (pItem->bIs3D)
		pSysCamera = &pSystemSpec->sysNSIS.sys3DCamera;
	else
		pSysCamera = &pSystemSpec->sysNSIS.sys2DCamera;

	if (pSysCamera == nullptr)
		return RESULT_BAD;

	double dWidth = fabs(pItem->MinMaxRect.right - pItem->MinMaxRect.left) * 1000.0;
	double dHeight = fabs(pItem->MinMaxRect.top - pItem->MinMaxRect.bottom) * 1000.0;
	if (dWidth <= 0 || dHeight <= 0)
		return RESULT_BAD;

	if (bIsAlign)
	{
		dWidth *= 4.0;
		dHeight *= 4.0;
	}
	else
	{
		if (pItem->stSpec.dRange == 0)
		{
			dWidth += 100.;
			dHeight += 100.;
		}
		else
		{
			dWidth += pItem->stSpec.dRange;
			dHeight += pItem->stSpec.dRange;
		}

		// Pattern 및 여유공간 2배 이상 FOV 선정
		dWidth *= 2.0;
		dHeight *= 2.0;
	}

	int iSel = -1;
	for (int i = 0; i < pSysCamera->size(); i++)
	{
		NSIS_CameraSpec_t *pCameraSpec = pSysCamera->at(i);
		if (pCameraSpec == nullptr)
			continue;

		double diffX = pCameraSpec->iFovWidth - (dWidth);
		double diffY = pCameraSpec->iFovHeight - (dHeight);
		if (diffX < 0 || diffY < 0) continue;

		if (dWidth > dHeight)
		{
			if (diffX > 0)
			{
				iSel = i;
				break;
			}
		}
		else
		{
			if (diffY > 0)
			{
				iSel = i;
				break;
			}
		}
	}

	int iSet = -1;
	for (int i = 0; i < pSysCamera->size(); i++)
	{
		NSIS_CameraSpec_t *pCameraSpec = pSysCamera->at(i);
		if (pCameraSpec == nullptr)
			continue;

		if (pCameraSpec->dMagnification != dMag ||
			pCameraSpec->dZoom != dZoom)
			continue;
		
		double diffX = pCameraSpec->iFovWidth - (dWidth);
		double diffY = pCameraSpec->iFovHeight - (dHeight);
		if (diffX < 0 || diffY < 0) continue;

		if (dWidth > dHeight)
		{
			if (diffX > 0)
			{
				iSet = i;
				break;
			}
		}
		else
		{
			if (diffY > 0)
			{
				iSet = i;
				break;
			}
		}		
	}
	
	if (iSel < 0 || iSel >= pSysCamera->size())
		return RESULT_BAD;
	else
	{
		if (iSet < 0 || iSet >= pSysCamera->size())
			return RESULT_BAD;
		else
			iSel = iSet;
	}

	NSIS_CameraSpec_t *pCameraSpec = pSysCamera->at(iSel);
	if (pCameraSpec == nullptr)
		return RESULT_BAD;

	pItem->dLens = pCameraSpec->dMagnification;
	pItem->dZoom = pCameraSpec->dZoom;

	return RESULT_GOOD;
}

