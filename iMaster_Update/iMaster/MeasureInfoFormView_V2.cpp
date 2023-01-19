#include "pch.h"
#include "MeasureInfoFormView_V2.h"
#include "MainFrm.h"
#include <vector>

using namespace std;

#define IDC_LISTCTRL_MEASURES   1990
#define IDC_STATICTEXT			1991
#define IDC_STATICCHECK			1992
#define IDC_MEASURE_INFO_BUTTON_ACCEPT	1993

#define IDC_MEASURE_INFO_STATIC_OOS	2000
#define IDC_MEASURE_INFO_STATIC_OOR	2001
#define IDC_MEASURE_INFO_STATIC_OOC	2002

#define IDC_MEASURE_INFO_BUTTON_SET_OOS	2010
#define IDC_MEASURE_INFO_BUTTON_SET_OOR	2011
#define IDC_MEASURE_INFO_BUTTON_SET_OOC	2012

#define IDC_MEASURE_INFO_EDIT_OOS_MIN	2020
#define IDC_MEASURE_INFO_EDIT_OOS_MAX	2021
#define IDC_MEASURE_INFO_EDIT_OOR_MIN	2022
#define IDC_MEASURE_INFO_EDIT_OOR_MAX	2023
#define IDC_MEASURE_INFO_EDIT_OOC_MIN	2024
#define IDC_MEASURE_INFO_EDIT_OOC_MAX	2025

CMeasureInfoFormView_V2::CMeasureInfoFormView_V2()
{

}

CMeasureInfoFormView_V2::~CMeasureInfoFormView_V2()
{
}

BEGIN_MESSAGE_MAP(CMeasureInfoFormView_V2, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_LISTCTRL_MEASURES, OnItemClick)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_LISTCTRL_MEASURES, OnItemChanging)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTCTRL_MEASURES, OnItemChangedList)
	ON_REGISTERED_MESSAGE(WM_LISTCTRL_COMBOBOX_CHANGE, OnComboBoxChange)
	ON_REGISTERED_MESSAGE(WM_LISTCTRL_EDIT_CHANGED, OnEditChanged)
	ON_COMMAND(IDC_MEASURE_INFO_BUTTON_ACCEPT, &CMeasureInfoFormView_V2::OnBnClickedButtonAccept)
	ON_UPDATE_COMMAND_UI(IDC_MEASURE_INFO_BUTTON_ACCEPT, &CMeasureInfoFormView_V2::OnUpdateButton)
	ON_COMMAND(IDC_MEASURE_INFO_BUTTON_SET_OOS, &CMeasureInfoFormView_V2::OnBnClickedButtonSetOOS)
	ON_COMMAND(IDC_MEASURE_INFO_BUTTON_SET_OOR, &CMeasureInfoFormView_V2::OnBnClickedButtonSetOOR)
	ON_COMMAND(IDC_MEASURE_INFO_BUTTON_SET_OOC, &CMeasureInfoFormView_V2::OnBnClickedButtonSetOOC)
	ON_UPDATE_COMMAND_UI(IDC_MEASURE_INFO_BUTTON_SET_OOS, &CMeasureInfoFormView_V2::OnUpdateButton)
	ON_UPDATE_COMMAND_UI(IDC_MEASURE_INFO_BUTTON_SET_OOR, &CMeasureInfoFormView_V2::OnUpdateButton)
	ON_UPDATE_COMMAND_UI(IDC_MEASURE_INFO_BUTTON_SET_OOC, &CMeasureInfoFormView_V2::OnUpdateButton)
END_MESSAGE_MAP()


int CMeasureInfoFormView_V2::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
	
// 	for (int i = 0; i < MeasureList_Sem::enumItemMax; i++)
// 	{
// 		m_MeasureList.InsertColumn(i, MeasureList_Sem::pName[i], LVCFMT_CENTER, 100);
// 	}

	m_MeasureList.InsertColumn(MeasureList_Sem::enumItem, MeasureList_Sem::pName[MeasureList_Sem::enumItem], LVCFMT_CENTER, 20);
	m_MeasureList.InsertColumn(MeasureList_Sem::enumType, MeasureList_Sem::pName[MeasureList_Sem::enumType], LVCFMT_CENTER, 100);
	m_MeasureList.InsertColumn(MeasureList_Sem::enumOptic, MeasureList_Sem::pName[MeasureList_Sem::enumOptic], LVCFMT_CENTER, 70);
	m_MeasureList.InsertColumn(MeasureList_Sem::enumInspSide, MeasureList_Sem::pName[MeasureList_Sem::enumInspSide], LVCFMT_CENTER, 70);
	m_MeasureList.InsertColumn(MeasureList_Sem::enumMag, MeasureList_Sem::pName[MeasureList_Sem::enumMag], LVCFMT_CENTER, 70);
	m_MeasureList.InsertColumn(MeasureList_Sem::enumZoom, MeasureList_Sem::pName[MeasureList_Sem::enumZoom], LVCFMT_CENTER, 70);
	m_MeasureList.InsertColumn(MeasureList_Sem::enumOOS_Min, MeasureList_Sem::pName[MeasureList_Sem::enumOOS_Min], LVCFMT_CENTER, 80);
	m_MeasureList.InsertColumn(MeasureList_Sem::enumOOS_Max, MeasureList_Sem::pName[MeasureList_Sem::enumOOS_Max], LVCFMT_CENTER, 80);
	m_MeasureList.InsertColumn(MeasureList_Sem::enumOOR_Min, MeasureList_Sem::pName[MeasureList_Sem::enumOOR_Min], LVCFMT_CENTER, 80);
	m_MeasureList.InsertColumn(MeasureList_Sem::enumOOR_Max, MeasureList_Sem::pName[MeasureList_Sem::enumOOR_Max], LVCFMT_CENTER, 80);
	m_MeasureList.InsertColumn(MeasureList_Sem::enumOOC_Min, MeasureList_Sem::pName[MeasureList_Sem::enumOOC_Min], LVCFMT_CENTER, 80);
	m_MeasureList.InsertColumn(MeasureList_Sem::enumOOC_Max, MeasureList_Sem::pName[MeasureList_Sem::enumOOC_Max], LVCFMT_CENTER, 80);
	m_MeasureList.InsertColumn(MeasureList_Sem::enumWidthRange, MeasureList_Sem::pName[MeasureList_Sem::enumWidthRange], LVCFMT_CENTER, 50);

	m_MeasureList.SetReadOnlyColumns(MeasureList_Sem::enumItem);
	m_MeasureList.SetReadOnlyColumns(MeasureList_Sem::enumType);
	m_MeasureList.SetReadOnlyColumns(MeasureList_Sem::enumOptic);
	m_MeasureList.SetReadOnlyColumns(MeasureList_Sem::enumWidthRange);
	m_MeasureList.SetReadOnlyColumns(MeasureList_Sem::enumInspSide);
// 	m_MeasureList.SetReadOnlyColumns(MeasureList_Sem::enumLeft);
// 	m_MeasureList.SetReadOnlyColumns(MeasureList_Sem::enumTop);
// 	m_MeasureList.SetReadOnlyColumns(MeasureList_Sem::enumRight);
// 	m_MeasureList.SetReadOnlyColumns(MeasureList_Sem::enumBottom);
//	m_MeasureList.SetReadOnlyColumns(MeasureList_Sem::enumFeatureType);

	m_MeasureList.SetComboColumns(MeasureList_Sem::enumMag);
	m_MeasureList.SetComboColumns(MeasureList_Sem::enumZoom);

	m_MeasureList.SetComboColumns(MeasureList_Sem::enumType);
	m_MeasureList.SetComboColumns(MeasureList_Sem::enumOptic);
	m_MeasureList.SetComboColumns(MeasureList_Sem::enumInspSide);

	m_MeasureList.SetDefaultTextColor(Foreground_Color);
	
	InitializeList();

	CImageList imagelist;
	imagelist.Create(22, 22, ILC_COLOR4, 10, 10);
	m_ItemList.SetImageList(&imagelist, LVSIL_SMALL);
	m_MeasureList.SetImageList(&imagelist, LVSIL_SMALL);

	//
	_InitialBtn();

	return 0;
}

void CMeasureInfoFormView_V2::ClearItemList()
{
	if (m_nItemListCount > 0)
		m_ItemList.DeleteAllItems();
	m_nItemListCount = 0;
}

void CMeasureInfoFormView_V2::ClearMeasureList()
{
	m_MeasureList.DeleteAllItems();
	
	m_vecMeasureItem.clear();
}

void CMeasureInfoFormView_V2::InitializeList()
{
	ClearItemList();
	ClearMeasureList();

	m_strOpticArr.RemoveAll();
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

	m_MeasureList.SetComboBoxList(MeasureList_Sem::enumMag, m_strMagArr);
	m_MeasureList.SetComboBoxList(MeasureList_Sem::enumZoom, m_strZoomArr);

	//Type
	m_strTypeArr.RemoveAll();
	int nStart = static_cast<int>(MeasureType_SEM::t_width);//Align은 제외하고..
	int nEnd = static_cast<int>(MeasureType_SEM::MeasureTypeNone);
	for (int i = nStart; i < nEnd; i++)
	{
		m_strTypeArr.Add(DefMeasureType_SEM::pName[i]);
	}

	//Type_SR
	m_strTypeArr_SR.RemoveAll();
	nStart = static_cast<int>(MeasureType_SR_SEM::probe);
	nEnd = static_cast<int>(MeasureType_SR_SEM::MeasureTypeNone);
	for (int i = nStart; i < nEnd; i++)
	{
		m_strTypeArr_SR.Add(DefMeasureType_SR_SEM::pName[i]);
	}
	
	//Side
	m_strSideArr.RemoveAll();
	nStart = static_cast<int>(MeasureSide::top);
	nEnd = static_cast<int>(MeasureSide::MeasureSideNone);
	for (int i = nStart; i < nEnd; i++)
	{
		m_strSideArr.Add(DefMeasureSide::pName[i]);
	}
	
	
	//m_MeasureList.SetComboBoxList(MeasureList_Sem::enumType, m_strTypeArr);	
	m_MeasureList.SetComboBoxList(MeasureList_Sem::enumOptic, m_strOpticArr);
	m_MeasureList.SetComboBoxList(MeasureList_Sem::enumInspSide, m_strSideArr);
}

void CMeasureInfoFormView_V2::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}


void CMeasureInfoFormView_V2::AdjustLayout()
{
	if (GetSafeHwnd() == NULL) return;

	_InitialSetSpec();

	CRect rectClient;
	GetClientRect(&rectClient);

	//SetSpec
	int nStaticWidth = static_cast<int>(rectClient.Width() / 3.);
	int nStaticHeight = 20;
	int nEditWidth = static_cast<int>(rectClient.Width() / 6.);
	int nEditHeight = 40;
	int nSetBtnHeight = 30;
	//

	int nStepListHeight = (m_nItemListCount + 1) * ITEM_LIST_HEIGHT;
	if (nStepListHeight == ITEM_LIST_HEIGHT)
		nStepListHeight = rectClient.Height() / 6;
	else if (nStepListHeight >= rectClient.Height() / 2)
		nStepListHeight = rectClient.Height() / 4;

	int nTop = 0;
	m_ItemList.SetWindowPos(NULL, rectClient.left, nTop, rectClient.Width(), nStepListHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	nTop += nStepListHeight;

	//SetSpec
	for (int i = 0; i < SpecSet_Sem::enumItemMax; i++)
	{//Static
		m_staticSpecSet[i]->SetWindowPos(nullptr, rectClient.left + (nStaticWidth*i), nTop, nStaticWidth, nStaticHeight, SWP_NOACTIVATE | SWP_NOZORDER);
		m_staticSpecSet[i]->Invalidate();
	}
	nTop += nStaticHeight;

	for (int i = 0; i < SpecSet_Sem::enumMax; i++)
	{//Edit
		m_editSpecSet[i]->SetWindowPos(nullptr, rectClient.left + (nEditWidth*i), nTop, nEditWidth, nEditHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	}
	nTop += nEditHeight;

	for (int i = 0; i < SpecSet_Sem::enumItemMax; i++)
	{//Button
		m_BtnSpecSet[i]->SetWindowPos(nullptr, rectClient.left + (nStaticWidth*i), nTop, nStaticWidth, nSetBtnHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	}
	nTop += nSetBtnHeight;

	m_MeasureList.SetWindowPos(NULL, rectClient.left, nTop, rectClient.Width(), rectClient.Height() - nTop - 30, SWP_NOACTIVATE | SWP_NOZORDER);
	nTop += rectClient.Height() - nTop - 30;

	if (m_pBtnAccept == nullptr)
	{
		_InitialBtn();
	}

	m_pBtnAccept->SetWindowPos(NULL, rectClient.left, nTop, rectClient.Width(), rectClient.Height() - nTop, 0);
	
}

void CMeasureInfoFormView_V2::OnItemClick(NMHDR* pNMHDR, LRESULT* pResult)
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
	// 2022.06.20
	// 김준호 주석처리
	// Item Column 뿐 아니라 Row 클릭시 위치 찾아가기 위해 주석처리
	/*if (iCol > 1)
		return;*/

	RECTD rcFeatureTemp(0,0,0,0);
	FeatureType featureType = FeatureType::FeatureTypeNone;

// 	CString strTemp = L"";
// 	strTemp = m_MeasureList.GetItemText(iRow, MeasureList_Sem::enumLeft);
// 	rcFeatureTemp.left = _ttof(strTemp);
// 
// 	strTemp = m_MeasureList.GetItemText(iRow, MeasureList_Sem::enumTop);
// 	rcFeatureTemp.top = _ttof(strTemp);
// 
// 	strTemp = m_MeasureList.GetItemText(iRow, MeasureList_Sem::enumRight);
// 	rcFeatureTemp.right = _ttof(strTemp);
// 
// 	strTemp = m_MeasureList.GetItemText(iRow, MeasureList_Sem::enumBottom);
// 	rcFeatureTemp.bottom = _ttof(strTemp);
// 
// 	strTemp = m_MeasureList.GetItemText(iRow, MeasureList_Sem::enumFeatureType);
// 	featureType = static_cast<FeatureType>(_ttoi(strTemp));

	if (m_vecMeasureItem.size() <= iRow) return;
	if (m_vecMeasureItem.size() <= 0) return;

	CFeature *pFeature = (CFeature *)m_vecMeasureItem[iRow]->pFeature;
	if (pFeature == nullptr) return;

	featureType = pFeature->m_eType;
	rcFeatureTemp = m_vecMeasureItem[iRow]->MinMaxRect_In_Panel;


	/*
	featureType = pFeature->m_eType;
	rcFeatureTemp = pFeature->m_MinMaxRect;
	
	// 2022.06.09 
	// 김준호 코드 추가
	// MeasureInfo List Item Click 시 reFeatureTemp 값 계산

	

	if ((pFeature->m_pSymbol == nullptr) || (pFeature->m_MinMaxRect.right != pFeature->m_MinMaxRect.left))
	{
		rcFeatureTemp.left = pFeature->m_MinMaxRect.left;
		rcFeatureTemp.right = pFeature->m_MinMaxRect.right;
		rcFeatureTemp.top = pFeature->m_MinMaxRect.top;
		rcFeatureTemp.bottom = pFeature->m_MinMaxRect.bottom;
	}
	else if ((pFeature->m_MinMaxRect.right == pFeature->m_MinMaxRect.left) || (pFeature->m_pSymbol->m_MinMaxRect.right == pFeature->m_pSymbol->m_MinMaxRect.left))
	{

		rcFeatureTemp.left = pFeature->m_MinMaxRect.left - pFeature->m_pSymbol->m_MinMaxRect.left;
		rcFeatureTemp.right = pFeature->m_MinMaxRect.right - pFeature->m_pSymbol->m_MinMaxRect.right;
		rcFeatureTemp.top = pFeature->m_MinMaxRect.top - pFeature->m_pSymbol->m_MinMaxRect.top;
		rcFeatureTemp.bottom = pFeature->m_MinMaxRect.bottom - pFeature->m_pSymbol->m_MinMaxRect.bottom;		
	}
	*/

//	UserSetInfo_t *pUserSetInfo = GetUserSetInfo();
//
//	//Front 90 도 조건 추가
//
//	double dAngle = pUserSetInfo->dAngle, dTemp;
//
//	if (dAngle == 90.0)
//	{
//		SWAP(rcFeatureTemp.top, rcFeatureTemp.left, dTemp);
//		SWAP(rcFeatureTemp.bottom, rcFeatureTemp.right, dTemp);
//		rcFeatureTemp.bottom *= -1;
//		rcFeatureTemp.top *= -1;
//	}
//
//	else if (dAngle == 180.0)
//	{
//// 		SWAP(rcFeatureTemp.top, rcFeatureTemp.left, dTemp);
//// 		SWAP(rcFeatureTemp.bottom, rcFeatureTemp.right, dTemp);
//		rcFeatureTemp.bottom *= -1;
//		rcFeatureTemp.top *= -1;
//		rcFeatureTemp.left *= -1;
//		rcFeatureTemp.right *= -1;
//	}
//
//	else if (dAngle == 270.0)
//	{
//		SWAP(rcFeatureTemp.top, rcFeatureTemp.left, dTemp);
//		SWAP(rcFeatureTemp.bottom, rcFeatureTemp.right, dTemp);
//
//// 		rcFeatureTemp.bottom *= -1;
//// 		rcFeatureTemp.top *= -1;
//		rcFeatureTemp.left *= -1;
//		rcFeatureTemp.right *= -1;
//	}
//
//	// NSIS Back 
//	
//
//
//
//	
//
//
//
//	if (pUserSetInfo->prodSide == eProdSide::eBot)
//	//if (m_stUserSetInfo.mirrorDir == eDirectionType::eVertical)
//	{
//// 		rcFeatureTemp.left *= -1;
//// 		rcFeatureTemp.right *= -1;
//		rcFeatureTemp.top *= -1;
//        rcFeatureTemp.bottom *= -1;
//	}
//	

	rcFeatureTemp.NormalizeRectD();

	ZoomRect(featureType, rcFeatureTemp);
}

void CMeasureInfoFormView_V2::OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = FALSE;

	if ((pNMListView->uNewState & LVIS_SELECTED))
	{
		*pResult = TRUE;
	}
}

void CMeasureInfoFormView_V2::OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult)
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

// 	if (pNMListView->hdr.hwndFrom == m_ItemList.m_hWnd)
// 		UpdateMeasureData(TRUE, nIndex,	pNMListView->iSubItem);
// 	else
// 		UpdateMeasureData(FALSE, nIndex, pNMListView->iSubItem);

	UpdateFeatureSelect(nIndex, bNewState);	
}

LRESULT CMeasureInfoFormView_V2::OnComboBoxChange(WPARAM wParam, LPARAM lParam)
{

	CUSTOM_NMITEM *pNMI = (CUSTOM_NMITEM*)wParam;

	if (pNMI->hwndFrom == m_ItemList)
	{
		if (pNMI->iSubItem == LIST_ITEM_MAG_COL ||
			pNMI->iSubItem == LIST_ITEM_ZOOM_COL)	// Magnification & Zoom
		{
			UpdateComboChange(TRUE, pNMI->iItem, pNMI->iSubItem);
			m_ItemList.Invalidate();
		}
	}
	else if (pNMI->hwndFrom == m_MeasureList)
	{
		if (pNMI->iSubItem == MeasureList_Sem::enumMag ||
			pNMI->iSubItem == MeasureList_Sem::enumZoom)		// Magnification & Zoom
		{			
			UpdateComboChange(FALSE, pNMI->iItem, pNMI->iSubItem);
		}

		m_MeasureList.Invalidate();
	}
	return 0L;
}

LRESULT CMeasureInfoFormView_V2::OnEditChanged(WPARAM wParam, LPARAM lParam)
{
	CUSTOM_NMITEM *pNMI = (CUSTOM_NMITEM*)wParam;

	return 0L;
}

UINT32 CMeasureInfoFormView_V2::SetMeasureData()
{
	if (GetMachineType() != eMachineType::eNSIS) return RESULT_BAD;
	
	ClearMeasureList();

	_SetMeasureData_Sem();

	return RESULT_GOOD;
}

UINT32 CMeasureInfoFormView_V2::_SetMeasureData_Sem()
{
	UINT32 nRet;

	//Minimap에서 선택된 좌표
	RECTD drtSelect(0, 0, 0, 0);
	nRet = Minimap_GetSelectRect(drtSelect);
	if (nRet != RESULT_GOOD)
	{
		return RESULT_BAD;
	}

	UserSetInfo_t* pUserSetInfo = GetUserSetInfo();
	if (pUserSetInfo == nullptr)
		return RESULT_BAD;

	SystemSpec_t* pSysSpec = GetSystemSpec();
	if (pSysSpec == nullptr)
		return RESULT_BAD;

	m_pPanelInfo = GetProductInfo();
	if (m_pPanelInfo == nullptr)
		return RESULT_BAD;

	UserLayerSet_t* pUserLayerSet = GetUserLayerSet();
	if (pUserLayerSet == nullptr)
		return RESULT_BAD;

	m_MeasureList.SetRedraw(FALSE);
	m_MeasureList.EnableWindow(FALSE);

// 	m_MeasureList.SetColumnWidth(MeasureList_Sem::enumType, MeasureList_Sem::enumListWidth_Type);
// 	for (int i = MeasureList_Sem::enumType + 1; i < MeasureList_Sem::enumItemMax; i++)
// 	{
// 		m_MeasureList.SetColumnWidth(i, MeasureList_Sem::enumListWidth_Other);
// 	}

// 	m_MeasureList.SetColumnWidth(MeasureList_Sem::enumType, MeasureList_Sem::enumListWidth_Type);
// 	m_MeasureList.SetColumnWidth(MeasureList_Sem::enumOOS_Min, MeasureList_Sem::enumListWidth_Other);
// 	m_MeasureList.SetColumnWidth(MeasureList_Sem::enumOOS_Max, MeasureList_Sem::enumListWidth_Other);
// 	m_MeasureList.SetColumnWidth(MeasureList_Sem::enumOOR_Min, MeasureList_Sem::enumListWidth_Other);
// 	m_MeasureList.SetColumnWidth(MeasureList_Sem::enumOOR_Max, MeasureList_Sem::enumListWidth_Other);
// 	m_MeasureList.SetColumnWidth(MeasureList_Sem::enumOOC_Min, MeasureList_Sem::enumListWidth_Other);
// 	m_MeasureList.SetColumnWidth(MeasureList_Sem::enumOOC_Max, MeasureList_Sem::enumListWidth_Other);

// 
// 	m_MeasureList.SetColumnWidth(LIST_OPTIC_COLUMN_SEM, LIST_COLUMN_WIDTH);
// 	m_MeasureList.SetColumnWidth(LIST_MAG_COLUMN_SEM, LIST_COLUMN_WIDTH);
// 	m_MeasureList.SetColumnWidth(LIST_ZOOM_COLUMN_SEM, LIST_COLUMN_WIDTH);
// 	m_MeasureList.SetColumnWidth(LIST_OOS_MIN_COLUMN_SEM, LIST_COLUMN_WIDTH);
// 	m_MeasureList.SetColumnWidth(LIST_OOS_MAX_COLUMN_SEM, LIST_COLUMN_WIDTH);
// 	m_MeasureList.SetColumnWidth(LIST_OOR_MIN_COLUMN_SEM, LIST_COLUMN_WIDTH);
// 	m_MeasureList.SetColumnWidth(LIST_OOR_MAX_COLUMN_SEM, LIST_COLUMN_WIDTH);
// 	m_MeasureList.SetColumnWidth(LIST_OOC_MIN_COLUMN_SEM, LIST_COLUMN_WIDTH);
// 	m_MeasureList.SetColumnWidth(LIST_OOC_MAX_COLUMN_SEM, LIST_COLUMN_WIDTH);
// 	m_MeasureList.SetColumnWidth(LIST_WIDTHRANGE_COLUMN_SEM, LIST_COLUMN_WIDTH);
// 	m_MeasureList.SetColumnWidth(LIST_SIDE_COLUMN_SEM, LIST_COLUMN_WIDTH);
	

	//m_vecMeasureItem.clear();


	_SetMeasureItem(m_pPanelInfo, drtSelect, pUserSetInfo->rcpMode, m_vecMeasureItem);

	//현재 상태 업데이트
	_UpdateMeasureList(m_vecMeasureItem);

	m_MeasureList.EnableWindow(TRUE);
	m_MeasureList.SetRedraw(TRUE);


	return RESULT_GOOD;
}

UINT32 CMeasureInfoFormView_V2::_SetMeasureItem(IN PanelInfo_t* pPanelInfo, IN RECTD &drtSelect, IN const eRecipeMode &emRecipeMode, vector<MeasureItem_t*> &vecMeasureItem)
{
	if (pPanelInfo == nullptr )  return RESULT_BAD;
	if (drtSelect.IsRectNull() == TRUE) return RESULT_BAD;
	if ( emRecipeMode == eRecipeMode::RecipeModeNone) return RESULT_BAD;
	
	MeasurePanel_t	*pMeasureInfo = &pPanelInfo->stMeasureInfo;
	if (pMeasureInfo == nullptr) return RESULT_BAD;


	vecMeasureItem.clear();

	//Align
	if (emRecipeMode == eRecipeMode::eRecipe_Monitor_Panel ||
		emRecipeMode == eRecipeMode::eRecipe_Align_Panel)
	{
		//Panel Align
		for (auto itAlign : pMeasureInfo->vcAlign)
		{
			if (_MeasureItemInRect(drtSelect, itAlign) == TRUE)
			{//선택된 영역안에 포함되어 있다면..				

				vecMeasureItem.push_back(itAlign);
			}
		}

		//Strip Dummy
		for (auto itStripDummy : pMeasureInfo->vcStripDummy)
		{
			//Strip Align
			for (auto itAlign : itStripDummy->vcAlign)
			{
				if (_MeasureItemInRect(drtSelect, itAlign) == TRUE)
				{//선택된 영역안에 포함되어 있다면..

					vecMeasureItem.push_back(itAlign);
				}
			}
		}
		//Unit
		for (auto itUnit : pMeasureInfo->vcUnits)
		{
			//Unit Align
			for (auto itAlign : itUnit->vcAlign)
			{
				if (_MeasureItemInRect(drtSelect, itAlign) == TRUE)
				{//선택된 영역안에 포함되어 있다면..

					vecMeasureItem.push_back(itAlign);
				}
			}
		}
	}
	
	//Measure
	if (emRecipeMode == eRecipeMode::eRecipe_Monitor_Panel ||
		emRecipeMode == eRecipeMode::eRecipe_MeasureNSIS_Panel)
	{
		//Strip Dummy
		for (auto itStripDummy : pMeasureInfo->vcStripDummy)
		{
			//Strip Measure
			for (auto itMeasure : itStripDummy->vcPoints)
			{
				if (_MeasureItemInRect(drtSelect, itMeasure) == TRUE)
				{//선택된 영역안에 포함되어 있다면..

					vecMeasureItem.push_back(itMeasure);
				}
			}
		}

		//Unit
		for (auto itUnit : pMeasureInfo->vcUnits)
		{
			//Unit Measure
			for (auto itMeasure : itUnit->vcPoints)
			{
				if (_MeasureItemInRect(drtSelect, itMeasure) == TRUE)
				{//선택된 영역안에 포함되어 있다면..

					vecMeasureItem.push_back(itMeasure);
				}
			}
		}
	}

	//Thickness
	if (emRecipeMode == eRecipeMode::eRecipe_Monitor_Panel ||
		emRecipeMode == eRecipeMode::eRecipe_MeasureSR_Panel)
	{
		//Strip Dummy
		for (auto itStripDummy : pMeasureInfo->vcStripDummy)
		{
			//Strip Thickness
			for (auto itThickness : itStripDummy->vcPoints_SR)
			{
				if (_MeasureItemInRect(drtSelect, itThickness) == TRUE)
				{//선택된 영역안에 포함되어 있다면..

					vecMeasureItem.push_back(itThickness);
				}
			}
		}

		//Unit
		for (auto itUnit : pMeasureInfo->vcUnits)
		{
			//Unit Thickness
			for (auto itThickness : itUnit->vcPoints_SR)
			{
				if (_MeasureItemInRect(drtSelect, itThickness) == TRUE)
				{//선택된 영역안에 포함되어 있다면..

					vecMeasureItem.push_back(itThickness);
				}
			}
		}
	}

	return RESULT_GOOD;
}

UINT32 CMeasureInfoFormView_V2::_UpdateMeasureList(vector<MeasureItem_t*> m_vecMeasureItem)
{

	UserSetInfo_t* pUserSetInfo = GetUserSetInfo();
	if (pUserSetInfo == nullptr) return RESULT_BAD;

	m_MeasureList.ClearComboBoxList();
	if (pUserSetInfo->rcpMode == eRecipeMode::eRecipe_Monitor_Panel)
	{
		m_MeasureList.SetComboBoxList(MeasureList_Sem::enumType, m_strTypeArr);
		m_MeasureList.SetComboBoxList(MeasureList_Sem::enumType, m_strTypeArr_SR);
	}
	else if (pUserSetInfo->rcpMode == eRecipeMode::eRecipe_MeasureNSIS_Panel)
	{		
		m_MeasureList.SetComboBoxList(MeasureList_Sem::enumType, m_strTypeArr);
	}
	else
	if (pUserSetInfo->rcpMode == eRecipeMode::eRecipe_MeasureSR_Panel)
	{		
		m_MeasureList.SetComboBoxList(MeasureList_Sem::enumType, m_strTypeArr_SR);	
	}

	m_MeasureList.SetComboBoxList(MeasureList_Sem::enumMag, m_strMagArr);
	m_MeasureList.SetComboBoxList(MeasureList_Sem::enumZoom, m_strZoomArr);

	m_MeasureList.SetComboBoxList(MeasureList_Sem::enumOptic, m_strOpticArr);
	m_MeasureList.SetComboBoxList(MeasureList_Sem::enumInspSide, m_strSideArr);


	int nCount = static_cast<int>(m_vecMeasureItem.size());

	CString strTxt = _T("");

	for (int i = 0; i < nCount; i++)
	{
		if (m_vecMeasureItem[i] == nullptr) continue;

		m_MeasureList.InsertItem(i, NULL_STRING);
		m_MeasureList.SetCheck(i, FALSE);
		m_MeasureList.SetItemText(i, MeasureList_Sem::enumItem, _T(""));		
		//Type
		m_MeasureList.SetItemText(i, MeasureList_Sem::enumType, m_vecMeasureItem[i]->strMeasureType);

		//Optic
		if (m_vecMeasureItem[i]->bIs3D == FALSE)
		{
			m_MeasureList.SetItemText(i, MeasureList_Sem::enumOptic, OPTIC_2D_TEXT);
		}
		else
		{
			m_MeasureList.SetItemText(i, MeasureList_Sem::enumOptic, OPTIC_3D_TEXT);
		}

		//SIDE(BOT, TOP)
		m_MeasureList.SetItemText(i, MeasureList_Sem::enumInspSide, GetMeasureSideString(static_cast<MeasureSide>(m_vecMeasureItem[i]->iMeasureSide)));

		//Mag
		m_MeasureList.SetItemText(i, MeasureList_Sem::enumMag, GetMagnificationString(m_vecMeasureItem[i]->dLens));

		//Zoom
		m_MeasureList.SetItemText(i, MeasureList_Sem::enumZoom, GetZoomString(m_vecMeasureItem[i]->dZoom));

		//OOS Min, Max
		strTxt.Format(_T("%.1f"), m_vecMeasureItem[i]->stSpec.dMin);
		m_MeasureList.SetItemText(i, MeasureList_Sem::enumOOS_Min, strTxt);

		strTxt.Format(_T("%.1f"), m_vecMeasureItem[i]->stSpec.dMax);
		m_MeasureList.SetItemText(i, MeasureList_Sem::enumOOS_Max, strTxt);

		//OOR Min, Max
		strTxt.Format(_T("%.1f"), m_vecMeasureItem[i]->stSpec.dMin_OOR);
		m_MeasureList.SetItemText(i, MeasureList_Sem::enumOOR_Min, strTxt);

		strTxt.Format(_T("%.1f"), m_vecMeasureItem[i]->stSpec.dMax_OOR);
		m_MeasureList.SetItemText(i, MeasureList_Sem::enumOOR_Max, strTxt);

		//OOC Min, Max
		strTxt.Format(_T("%.1f"), m_vecMeasureItem[i]->stSpec.dMin_OOC);
		m_MeasureList.SetItemText(i, MeasureList_Sem::enumOOC_Min, strTxt);

		strTxt.Format(_T("%.1f"), m_vecMeasureItem[i]->stSpec.dMax_OOC);
		m_MeasureList.SetItemText(i, MeasureList_Sem::enumOOC_Max, strTxt);

		//WR
		strTxt.Format(_T("%.1f"), m_vecMeasureItem[i]->stSpec.dRange);
		m_MeasureList.SetItemText(i, MeasureList_Sem::enumWidthRange, strTxt);
// 
// 		//POS(Left)
// 		strTxt.Format(_T("%.3f"), m_vecMeasureItem[i]->MinMaxRect_In_Panel.left);
// 		m_MeasureList.SetItemText(i, MeasureList_Sem::enumLeft, strTxt);
// 		//POS(Top)
// 		strTxt.Format(_T("%.3f"), m_vecMeasureItem[i]->MinMaxRect_In_Panel.top);
// 		m_MeasureList.SetItemText(i, MeasureList_Sem::enumTop, strTxt);
// 		//POS(Right)
// 		strTxt.Format(_T("%.3f"), m_vecMeasureItem[i]->MinMaxRect_In_Panel.right);
// 		m_MeasureList.SetItemText(i, MeasureList_Sem::enumRight, strTxt);
// 		//POS(Bottom)
// 		strTxt.Format(_T("%.3f"), m_vecMeasureItem[i]->MinMaxRect_In_Panel.bottom);
// 		m_MeasureList.SetItemText(i, MeasureList_Sem::enumBottom, strTxt);
// 
// 		//Feature Type
// 		strTxt.Format(_T("%d"), m_vecMeasureItem[i] ->iFeatureType);
// 		m_MeasureList.SetItemText(i, MeasureList_Sem::enumFeatureType, strTxt);
	}
	
	return RESULT_GOOD;
}

UINT32 CMeasureInfoFormView_V2::_UpdateMeasureList_Spec(vector<MeasureItem_t*> m_vecMeasureItem)//List 내용 업데이트
{
	int nCount = static_cast<int>(m_vecMeasureItem.size());

	CString strTxt = _T("");

	for (int i = 0; i < nCount; i++)
	{
		if (m_vecMeasureItem[i] == nullptr) continue;

		//OOS Min, Max
		strTxt.Format(_T("%.1f"), m_vecMeasureItem[i]->stSpec.dMin);
		m_MeasureList.SetItemText(i, MeasureList_Sem::enumOOS_Min, strTxt);

		strTxt.Format(_T("%.1f"), m_vecMeasureItem[i]->stSpec.dMax);
		m_MeasureList.SetItemText(i, MeasureList_Sem::enumOOS_Max, strTxt);

		//OOR Min, Max
		strTxt.Format(_T("%.1f"), m_vecMeasureItem[i]->stSpec.dMin_OOR);
		m_MeasureList.SetItemText(i, MeasureList_Sem::enumOOR_Min, strTxt);

		strTxt.Format(_T("%.1f"), m_vecMeasureItem[i]->stSpec.dMax_OOR);
		m_MeasureList.SetItemText(i, MeasureList_Sem::enumOOR_Max, strTxt);

		//OOC Min, Max
		strTxt.Format(_T("%.1f"), m_vecMeasureItem[i]->stSpec.dMin_OOC);
		m_MeasureList.SetItemText(i, MeasureList_Sem::enumOOC_Min, strTxt);

		strTxt.Format(_T("%.1f"), m_vecMeasureItem[i]->stSpec.dMax_OOC);
		m_MeasureList.SetItemText(i, MeasureList_Sem::enumOOC_Max, strTxt);
	}

	return RESULT_GOOD;
}

BOOL CMeasureInfoFormView_V2::_MeasureItemInRect(IN RECTD &drtSelect, IN MeasureItem_t *pMeasureItem)
{
	if (drtSelect.IsRectNull() == TRUE) return FALSE;
	if (pMeasureItem == nullptr) return FALSE;

	return drtSelect.IsPtInRectD(pMeasureItem->MinMaxRect_In_Panel.CenterX(), pMeasureItem->MinMaxRect_In_Panel.CenterY());
}

UINT32 CMeasureInfoFormView_V2::UpdateComboChange(BOOL bIsItemList, int iSelRow, int iSelCol)
{
	CMainFrame*	pMainFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	if (pMainFrame == nullptr)
		return RESULT_BAD;

	if (m_pPanelInfo == nullptr)
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
		
		if (iSelRow < 0 || iSelRow >= static_cast<int>(m_vecMeasureItem.size()))
			return RESULT_BAD;

		if (iSelCol != LIST_MAG_COLUMN && iSelCol != LIST_ZOOM_COLUMN)
			return RESULT_BAD;
	}
	
	CString strTemp = L"";
	
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

// 		if (iSelRow == 0)
// 		{
// 			if (pSysSpec->sysNSIS.sysAlignLens == 0. && pSysSpec->sysNSIS.sysAlignZoom == 0.)
// 			{
// 				for (auto it : m_pMeasureData->vcAlign)
// 				{
// 					CheckLensZoom(true, it, dMag, dZoom);
// 				}
// 			}
// 			else
// 			{
// 				strTemp.Format(_T("x%.1f"), pSysSpec->sysNSIS.sysAlignLens);
// 				m_ItemList.SetItemText(iSelRow, LIST_ITEM_MAG_COL, strTemp);
// 				strTemp.Format(_T("x%.2f"), pSysSpec->sysNSIS.sysAlignZoom);
// 				m_ItemList.SetItemText(iSelRow, LIST_ITEM_ZOOM_COL, strTemp);
// 			}
// 		}
// 		else
// 		{
// 			for (auto it : m_pMeasureData->vcUnits)
// 			{
// 				if (iSelRow == 1)
// 				{
// 					if (pSysSpec->sysNSIS.sysUnitAlignLens == 0. && pSysSpec->sysNSIS.sysUnitAlignZoom == 0.)
// 					{
// 						for (auto itAlign : it->vcAlign)
// 						{
// 							CheckLensZoom(true, itAlign, dMag, dZoom);
// 						}
// 					}
// 					else
// 					{
// 						strTemp.Format(_T("x%.1f"), pSysSpec->sysNSIS.sysUnitAlignLens);
// 						m_ItemList.SetItemText(iSelRow, LIST_ITEM_MAG_COL, strTemp);
// 						strTemp.Format(_T("x%.2f"), pSysSpec->sysNSIS.sysUnitAlignZoom);
// 						m_ItemList.SetItemText(iSelRow, LIST_ITEM_ZOOM_COL, strTemp);
// 					}
// 				}
// 				else if (iSelRow == 2)
// 				{
// 					for (auto itMeasure : it->vcPoints)
// 					{
// 						for (int i = 0; i < m_strZoomArr.GetCount(); i++)
// 						{
// 							strTemp = m_strZoomArr.GetAt(i);
// 							strTemp.Delete(0, 1);
// 							strTemp.Trim();
// 							dZoom = _ttof(strTemp);
// 
// 							if (RESULT_GOOD == CheckLensZoom(false, itMeasure, dMag, dZoom))
// 								break;
// 						}
// 					}
// 				}
// 				else if (iSelRow == 3)
// 				{
// 					for (auto itMeasure : it->vcPoints_SR)
// 					{
// 						for (int i = 0; i < m_strZoomArr.GetCount(); i++)
// 						{
// 							strTemp = m_strZoomArr.GetAt(i);
// 							strTemp.Delete(0, 1);
// 							strTemp.Trim();
// 							dZoom = _ttof(strTemp);
// 
// 							if (RESULT_GOOD == CheckLensZoom(false, itMeasure, dMag, dZoom))
// 								break;
// 						}
// 					}
// 				}
// 			}
// 		}
	}
	else
	{

		strTemp = m_MeasureList.GetItemText(iSelRow, LIST_MAG_COLUMN);
		strTemp.Delete(0, 1);
		strTemp.Trim();
		double dMag = _ttof(strTemp);

		strTemp = m_MeasureList.GetItemText(iSelRow, LIST_ZOOM_COLUMN);
		strTemp.Delete(0, 1);
		strTemp.Trim();
		double dZoom = _ttof(strTemp);

	
// 		if (rcpMode == eRecipeMode::eRecipe_Monitor_Panel ||
// 			rcpMode == eRecipeMode::eRecipe_Align_Panel )
// 		{
// 			if (iSelRow >= 0 && iSelRow < m_pMeasureData->vcAlign.size())
// 			{
// 				CheckLensZoom(true, m_pMeasureData->vcAlign[iSelRow], dMag, dZoom);
// 					
// 				strTemp.Format(_T("x%.1f"), m_pMeasureData->vcAlign[iSelRow]->dLens);
// 				m_MeasureList.SetItemText(iSelRow, LIST_MAG_COLUMN, strTemp);
// 				strTemp.Format(_T("x%.2f"), m_pMeasureData->vcAlign[iSelRow]->dZoom);
// 				m_MeasureList.SetItemText(iSelRow, LIST_ZOOM_COLUMN, strTemp);
// 			}
// 		}
// 		else
// 		{
// 			INT32 iStripIdx = -1, iBlockIdx = -1, iUnitIdx = -1;
// 			RECTD rcRect(0, 0, 0, 0);
// 
// 			if (m_eViewType == eViewType::eAllVT)
// 			{
// 				UnitInfo_t* pUnit = GetUnitIndex_In_Panel(m_iSelRow, m_iSelCol, iStripIdx, iBlockIdx, iUnitIdx, rcRect);
// 				if (pUnit == nullptr)
// 					return RESULT_BAD;
// 			}
// 			else if (m_eViewType == eViewType::ePanelVT)
// 			{
// 				iStripIdx = m_iSelCol * m_pPanelInfo->iRowStripNumInPanel + m_iSelRow;
// 				if (iStripIdx >= m_pPanelInfo->vcStripInfo.size())
// 					return RESULT_BAD;
// 
// 				rcRect = m_pPanelInfo->vcStripInfo[iStripIdx]->rcRect;
// 			}
// 			else if (m_eViewType == eViewType::eStripVT)
// 			{
// 				UnitInfo_t* pUnit = GetUnitIndex_In_Strip(m_iSelRow, m_iSelCol, iStripIdx, iBlockIdx, iUnitIdx, rcRect);
// 				if (pUnit == nullptr)
// 					return RESULT_BAD;
// 			}
// 
// 			for (auto it : m_pMeasureData->vcUnits)
// 			{
// 				if (m_eViewType == eViewType::eAllVT || m_eViewType == eViewType::eStripVT)
// 				{
// 					if (it->iIndex != iUnitIdx)
// 						continue;
// 				}
// 				else if (m_eViewType == eViewType::ePanelVT)
// 				{
// 					if (it->iStripIdx != iStripIdx)
// 						continue;
// 				}
// 
// 				if (rcpMode == eRecipeMode::eRecipe_Align_Panel)
// 				{
// 					if (iSelRow >= 0 && iSelRow < it->vcAlign.size())
// 					{
// 						CheckLensZoom(true, it->vcAlign[iSelRow], dMag, dZoom);
// 						
// 						strTemp.Format(_T("x%.1f"), it->vcAlign[iSelRow]->dLens);
// 						m_MeasureList.SetItemText(iSelRow, LIST_MAG_COLUMN, strTemp);
// 						strTemp.Format(_T("x%.2f"), it->vcAlign[iSelRow]->dZoom);
// 						m_MeasureList.SetItemText(iSelRow, LIST_ZOOM_COLUMN, strTemp);
// 					}
// 				}
// 				else if (rcpMode == eRecipeMode::eRecipe_MeasureNSIS_Panel)
// 				{
// 					if (iSelRow >= 0 && iSelRow < it->vcPoints.size())
// 					{
// 						for (int i = 0; i < m_strZoomArr.GetCount(); i++)
// 						{
// 							strTemp = m_strZoomArr.GetAt(i);
// 							strTemp.Delete(0, 1);
// 							strTemp.Trim();
// 							dZoom = _ttof(strTemp);
// 
// 							if (RESULT_GOOD == CheckLensZoom(true, it->vcPoints[iSelRow], dMag, dZoom))
// 								break;
// 						}
// 					}
// 				}
// 				else if (rcpMode == eRecipeMode::eRecipe_MeasureSR_Panel)
// 				{
// 					if (iSelRow >= 0 && iSelRow < it->vcPoints_SR.size())
// 					{
// 						for (int i = 0; i < m_strZoomArr.GetCount(); i++)
// 						{
// 							strTemp = m_strZoomArr.GetAt(i);
// 							strTemp.Delete(0, 1);
// 							strTemp.Trim();
// 							dZoom = _ttof(strTemp);
// 
// 							if (RESULT_GOOD == CheckLensZoom(true, it->vcPoints_SR[iSelRow], dMag, dZoom))
// 								break;
// 						}
// 					}
// 				}
// 			}
// 		}
	}

	return SetMeasureData();
}

CString CMeasureInfoFormView_V2::GetMeasureTypeString(MeasureType type)
{
	if (type == MeasureType::align)
		return PANEL_ALIGN_TXT;
	else if (type == MeasureType::align_plating)
		return PANEL_ALIGN_PLATING_TXT;
	else if (type == MeasureType::fm_p)
		return UNIT_ALIGN_TXT_P;
	else
	{
		if (type >= MeasureType::t_width &&
			type < MeasureType::MeasureTypeNone)//정의되어 있다면
		{
			return DefMeasureType::pName[static_cast<int>(type)];
		}
		else
		{
			return _T("UKNOWN");
		}
	}
}

CString CMeasureInfoFormView_V2::GetMeasureSideString(MeasureSide side)
{
	if (side == MeasureSide::top)
		return _T("TOP");
	else
		return _T("BOT");
}

CString CMeasureInfoFormView_V2::GetMagnificationString(BOOL bIs3D, INT8 iScanMode)
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

CString CMeasureInfoFormView_V2::GetMagnificationString(double dLens)
{
	CString strMag;
	strMag.Format(_T("x%.1lf"), dLens);

	return strMag;
}

CString CMeasureInfoFormView_V2::GetZoomString(BOOL bIs3D, INT8 iScanMode)
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

CString CMeasureInfoFormView_V2::GetZoomString(double dZoom)
{
	CString strMag;
	strMag.Format(_T("x%.2lf"), dZoom);

	return strMag;
}

INT8 CMeasureInfoFormView_V2::SetScanMode(BOOL bIs3D, CString strMag, CString strZoom)
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

void CMeasureInfoFormView_V2::UpdateFeatureSelect(IN const int &nLineIndex, IN const BOOL &bChecked)
{//List에 Check 정보를 Viewer에 보내어 Select 하도록 한다.

	UserSetInfo_t* pUserSetInfo = GetUserSetInfo();
	if (pUserSetInfo == nullptr)
		return;

	eRecipeMode rcpMode = pUserSetInfo->rcpMode;
	vector<CFeature *> vecFeature;

	int nCount = static_cast<int>(m_vecMeasureItem.size());
	vecFeature.resize(nCount, nullptr);
	for (int i = 0; i < nCount; i++)
	{

		if (m_vecMeasureItem[i]->pFeature == nullptr ) continue;

		vecFeature[i] = (CFeature *)m_vecMeasureItem[i]->pFeature;
	}

	CFeature *pFeature = nullptr;
	if (nLineIndex >= 0 && nLineIndex < vecFeature.size())
		pFeature = (CFeature*)(vecFeature[nLineIndex]);

	if (pFeature != nullptr)
	{//
		switch (pFeature->m_eType)
		{
		case FeatureType::Z:
		{
			CFeatureZ *pFeatureZ = (CFeatureZ*)pFeature;
			pFeatureZ->m_bHighlight = bChecked;
		}
		break;
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

UINT32 CMeasureInfoFormView_V2::ResetFeatureSelect_UI()
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

UINT32 CMeasureInfoFormView_V2::UpdateFeatureSelect_UI()
{
	if (m_pPanelInfo == nullptr) return RESULT_BAD;
	
	if (GetMachineType() != eMachineType::eNSIS) return RESULT_BAD;
	
	
	int nCount = static_cast<int>(m_vecMeasureItem.size());
	int nLightCount = m_MeasureList.GetItemCount();
	if (nCount != nLightCount) 
		return RESULT_BAD;

	m_MeasureList.SetRedraw(FALSE);
	m_MeasureList.EnableWindow(FALSE);

	BOOL bHighlight;
	for (int i = 0; i < nCount; i++)
	{
		if (m_vecMeasureItem[i]->pFeature == nullptr ) continue;
		CFeature *pFeature = (CFeature*)m_vecMeasureItem[i]->pFeature;

		bHighlight = FALSE;
		switch (pFeature->m_eType)
		{
		case FeatureType::Z:
		{
			CFeatureZ *pFeatureZ = (CFeatureZ*)pFeature;
			bHighlight = pFeatureZ->m_bHighlight;
		}
		break;
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

	UserSetInfo_t* pUserSetInfo = GetUserSetInfo();
	if (pUserSetInfo->rcpMode == eRecipeMode::eRecipe_Monitor_Panel)
	{
		m_MeasureList.EnableWindow(FALSE);
	}
	else
	{
		m_MeasureList.EnableWindow(TRUE);
	}

	m_MeasureList.SetRedraw(TRUE);

	return RESULT_GOOD;
}

UINT32 CMeasureInfoFormView_V2::UpdateFeatureSelect_UI_Sem()
{
	UserSetInfo_t* pUserSetInfo = GetUserSetInfo();
	if (pUserSetInfo == nullptr)
		return RESULT_BAD;

	eRecipeMode rcpMode = pUserSetInfo->rcpMode;

	return RESULT_GOOD;
}

CString CMeasureInfoFormView_V2::GetMeasureLayerName()
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

UnitInfo_t* CMeasureInfoFormView_V2::GetUnitIndex_In_Panel(INT32 iRow, INT32 iCol, INT32 &iStripIdx, INT32 &iBlockIdx, INT32 &iUnitIdx, RECTD &rcDrawRect)
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

void CMeasureInfoFormView_V2::ZoomRect(FeatureType &type, RECTD &rcRect)
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

	/*
	UserSetInfo_t* pstUserSetInfo = GetUserSetInfo();
	if (pstUserSetInfo == nullptr) return;
	RECTD drtFeature;
	
	Orient ori = Orient::Mir0Deg;
	double dAngle = pstUserSetInfo->dAngle, dTemp;
	bool bMirror = pstUserSetInfo->bMirror;	

	// 2022.07.05
	// Back 좌표 계산위해 prodSide bottom 일때 dangle, bMirror 수정
	if (pstUserSetInfo->prodSide == eProdSide::eBot)
	{	
		dAngle = 180.;		
	}
	
	CDataManager::GetOrient(dAngle, bMirror, ori);

	CRotMirrArr rotMirrArr;
	rotMirrArr.InputData(0, 0, CDataManager::ReverseOrient(ori), pstUserSetInfo->mirrorDir);
	rotMirrArr.FinalPoint(&drtFeature.left, &drtFeature.top, rcRect.left, rcRect.top);
	rotMirrArr.FinalPoint(&drtFeature.right, &drtFeature.bottom, rcRect.right, rcRect.bottom);
	rotMirrArr.ClearAll();

	if (bMirror)
	{
		if (dAngle == 0.0)
		{
			SWAP(drtFeature.left, drtFeature.right, dTemp);
			SWAP(drtFeature.top, drtFeature.bottom, dTemp);
		}
		else if (dAngle == 90.0)
		{
			drtFeature.left *= -1.0;
			drtFeature.right *= -1.0;

			SWAP(drtFeature.top, drtFeature.bottom, dTemp);
			drtFeature.top *= -1.0;
			drtFeature.bottom *= -1.0;
		}		
		// 2022.07.05
		// 김준호 주석추가
		// Back 면 일 경우 좌표 계산위해 추가.
		else if (dAngle == 180.0)
		{
			//SWAP(drtFeature.left, drtFeature.right, dTemp);
			drtFeature.left *= -1.0;
			drtFeature.right *= -1.0;
			//SWAP(drtFeature.top, drtFeature.bottom, dTemp);
			//drtFeature.top *= -1.0;
			//drtFeature.bottom *= -1.0;
		}
		else if (dAngle == 270.0)
		{
			SWAP(drtFeature.left, drtFeature.right, dTemp);
			drtFeature.left *= -1.0;
			drtFeature.right *= -1.0;

			drtFeature.top *= -1.0;
			drtFeature.bottom *= -1.0;
		}		
	}
	*/

	/*
	// 2022.07.05
	// JDJ Code Add
	// Front 인 경우 Item 좌표 계산하기 위해 추가.
	else
	{
		if (dAngle == 90.0)
		{
			SWAP(drtFeature.top, drtFeature.left, dTemp);
			SWAP(drtFeature.bottom, drtFeature.right, dTemp);
			drtFeature.bottom *= -1;
			drtFeature.top *= -1;
		}

		else if (dAngle == 180.0)
		{
			//          SWAP(drtFeature.top, drtFeature.left, dTemp);
			//          SWAP(drtFeature.bottom, drtFeature.right, dTemp);
			drtFeature.bottom *= -1;
			drtFeature.top *= -1;
			drtFeature.left *= -1;
			drtFeature.right *= -1;
		}

		else if (dAngle == 270.0)
		{
			SWAP(drtFeature.top, drtFeature.left, dTemp);
			SWAP(drtFeature.bottom, drtFeature.right, dTemp);
			//rcFeatureTemp.bottom *= -1;
			//rcFeatureTemp.top *= -1;
			drtFeature.left *= -1;
			drtFeature.right *= -1;
		}

	}
	*/

	DrawZoomRect(rcRect);
}

UnitInfo_t* CMeasureInfoFormView_V2::GetUnitIndex_In_Strip(INT32 iRow, INT32 iCol, INT32 &iStripIdx, INT32 &iBlockIdx, INT32 &iUnitIdx, RECTD &rcDrawRect)
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


UINT32 CMeasureInfoFormView_V2::CheckLensZoom(bool bIsAlign, MeasureItem_t* pItem, double &dMag, double &dZoom)
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

// 
// UINT32 CMeasureInfoFormView_V2::InsertMeasureRow(UINT32 &iRow, CString &strTitle, MeasureItem_t *itPoint)
// {
// 	if (itPoint == nullptr)
// 		return RESULT_BAD;
// 
// 	CString strTxt = _T("");
// 
// 	// 2022.04.28
// 	// 주석추가 김준호
// 	// measurelist daeduck, sem 으로 분리
// 	if (GetMachineType() == eMachineType::eNSIS && GetUserSite() == eUserSite::eSEM)
// 	{
// 
// 		m_MeasureList.InsertItem(iRow, NULL_STRING);
// 		m_MeasureList.SetCheck(iRow, FALSE);
// 		m_MeasureList.SetItemText(iRow, LIST_ITEM_COLUMN_SEM, strTitle);
// 		m_MeasureList.SetItemText(iRow, LIST_TYPE_COLUMN_SEM, itPoint->strMeasureType);
// 		m_MeasureList.SetItemText(iRow, LIST_OPTIC_COLUMN_SEM, OPTIC_3D_TEXT);
// 		m_MeasureList.SetItemText(iRow, LIST_MAG_COLUMN_SEM, GetMagnificationString(itPoint->dLens));
// 		m_MeasureList.SetItemText(iRow, LIST_ZOOM_COLUMN_SEM, GetZoomString(itPoint->dZoom));
// 
// 		strTxt.Format(_T("%.1f"), itPoint->stSpec.dMin);		
// 		m_MeasureList.SetItemText(iRow, LIST_OOS_MIN_COLUMN_SEM, strTxt);
// 		m_MeasureList.SetItemText(iRow, LIST_OOR_MIN_COLUMN_SEM, strTxt);
// 		m_MeasureList.SetItemText(iRow, LIST_OOC_MIN_COLUMN_SEM, strTxt);
// 
// 		strTxt.Format(_T("%.1f"), itPoint->stSpec.dMax);
// 		m_MeasureList.SetItemText(iRow, LIST_OOS_MAX_COLUMN_SEM, strTxt);
// 		m_MeasureList.SetItemText(iRow, LIST_OOR_MAX_COLUMN_SEM, strTxt);
// 		m_MeasureList.SetItemText(iRow, LIST_OOC_MAX_COLUMN_SEM, strTxt);
// 
// 		strTxt.Format(_T("%.1f"), itPoint->stSpec.dRange);
// 		m_MeasureList.SetItemText(iRow, LIST_WIDTHRANGE_COLUMN_SEM, strTxt);
// 		m_MeasureList.SetItemText(iRow, LIST_SIDE_COLUMN_SEM, GetMeasureSideString(static_cast<MeasureSide>(itPoint->iMeasureSide)));
// 
// 		strTxt.Format(_T("%.3f"), itPoint->MinMaxRect_In_Panel.left);
// 		m_MeasureList.SetItemText(iRow, LIST_LEFT_COLUMN_SEM, strTxt);
// 		strTxt.Format(_T("%.3f"), itPoint->MinMaxRect_In_Panel.top);
// 		m_MeasureList.SetItemText(iRow, LIST_TOP_COLUMN_SEM, strTxt);
// 		strTxt.Format(_T("%.3f"), itPoint->MinMaxRect_In_Panel.right);
// 		m_MeasureList.SetItemText(iRow, LIST_RIGHT_COLUMN_SEM, strTxt);
// 		strTxt.Format(_T("%.3f"), itPoint->MinMaxRect_In_Panel.bottom);
// 		m_MeasureList.SetItemText(iRow, LIST_BOT_COLUMN_SEM, strTxt);
// 
// 		strTxt.Format(_T("%d"), itPoint->iFeatureType);
// 		m_MeasureList.SetItemText(iRow, LIST_FEATURE_TYPE_SEM, strTxt);
// 	}
// 	else
// 	{
// 		m_MeasureList.InsertItem(iRow, NULL_STRING);
// 		m_MeasureList.SetCheck(iRow, FALSE);
// 		m_MeasureList.SetItemText(iRow, LIST_ITEM_COLUMN, strTitle);
// 		//m_MeasureList.SetItemText(iRow, LIST_TYPE_COLUMN, GetMeasureTypeString(static_cast<MeasureType>(itPoint->iMeasureType)));
// 		m_MeasureList.SetItemText(iRow, LIST_TYPE_COLUMN, itPoint->strMeasureType);
// 		m_MeasureList.SetItemText(iRow, LIST_OPTIC_COLUMN, OPTIC_3D_TEXT);
// 
// 		m_MeasureList.SetItemText(iRow, LIST_MAG_COLUMN, GetMagnificationString(itPoint->dLens));
// 		//m_MeasureList.SetItemText(iRow, LIST_MAG_COLUMN, GetMagnificationString(itPoint->bIs3D, itPoint->iScanMode));
// 		m_MeasureList.SetItemText(iRow, LIST_ZOOM_COLUMN, GetZoomString(itPoint->dZoom));
// 		//m_MeasureList.SetItemText(iRow, LIST_ZOOM_COLUMN, GetZoomString(itPoint->bIs3D, itPoint->iScanMode));
// 
// 		strTxt.Format(_T("%.1f"), itPoint->stSpec.dMin);
// 		m_MeasureList.SetItemText(iRow, LIST_MIN_COLUMN, strTxt);
// 		strTxt.Format(_T("%.1f"), itPoint->stSpec.dTarget);
// 		m_MeasureList.SetItemText(iRow, LIST_TARGET_COLUMN, strTxt);
// 		strTxt.Format(_T("%.1f"), itPoint->stSpec.dMax);
// 		m_MeasureList.SetItemText(iRow, LIST_MAX_COLUMN, strTxt);
// 
// 		strTxt.Format(_T("%.1f"), itPoint->stSpec.dRange);
// 		m_MeasureList.SetItemText(iRow, LIST_WIDTHRANGE_COLUMN, strTxt);
// 		m_MeasureList.SetItemText(iRow, LIST_SIDE_COLUMN, GetMeasureSideString(static_cast<MeasureSide>(itPoint->iMeasureSide)));
// 
// 		strTxt.Format(_T("%.3f"), itPoint->MinMaxRect_In_Panel.left);
// 		m_MeasureList.SetItemText(iRow, LIST_LEFT_COLUMN, strTxt);
// 		strTxt.Format(_T("%.3f"), itPoint->MinMaxRect_In_Panel.top);
// 		m_MeasureList.SetItemText(iRow, LIST_TOP_COLUMN, strTxt);
// 		strTxt.Format(_T("%.3f"), itPoint->MinMaxRect_In_Panel.right);
// 		m_MeasureList.SetItemText(iRow, LIST_RIGHT_COLUMN, strTxt);
// 		strTxt.Format(_T("%.3f"), itPoint->MinMaxRect_In_Panel.bottom);
// 		m_MeasureList.SetItemText(iRow, LIST_BOT_COLUMN, strTxt);
// 
// 		strTxt.Format(_T("%d"), itPoint->iFeatureType);
// 		m_MeasureList.SetItemText(iRow, LIST_FEATURE_TYPE, strTxt);
// 	}
// 
// 	iRow++;
// 
// 	return RESULT_GOOD;
// }
// 
// 
// UINT32 CMeasureInfoFormView_V2::InsertAlignRow(UINT32 &iRow, CString &strTitle, MeasureItem_t *itAlign)
// {
// 	if (itAlign == nullptr)
// 		return RESULT_BAD;
// 
// 	CString strTxt = _T("");
// 
// 	m_MeasureList.InsertItem(iRow, NULL_STRING);
// 	m_MeasureList.SetCheck(iRow, FALSE);
// 	
// 	// 2022.04.28
// 	// 주석추가 김준호
// 	// measurelist daeduck, sem 으로 분리
// 	if (GetMachineType() == eMachineType::eNSIS && GetUserSite() == eUserSite::eSEM)
// 	{
// 		m_MeasureList.SetItemText(iRow, LIST_ITEM_COLUMN_SEM, strTitle);		
// 		m_MeasureList.SetItemText(iRow, LIST_TYPE_COLUMN_SEM, itAlign->strMeasureType);
// 		m_MeasureList.SetItemText(iRow, LIST_OPTIC_COLUMN_SEM, OPTIC_2D_TEXT);
// 		m_MeasureList.SetItemText(iRow, LIST_MAG_COLUMN, GetMagnificationString(itAlign->dLens));
// 		m_MeasureList.SetItemText(iRow, LIST_ZOOM_COLUMN, GetZoomString(itAlign->dZoom));
// 
// 		strTxt.Format(_T("%.1f"), itAlign->stSpec.dMin);
// 		m_MeasureList.SetItemText(iRow, LIST_OOS_MIN_COLUMN_SEM, strTxt);
// 		m_MeasureList.SetItemText(iRow, LIST_OOR_MIN_COLUMN_SEM, strTxt);
// 		m_MeasureList.SetItemText(iRow, LIST_OOC_MIN_COLUMN_SEM, strTxt);
// 
// 		strTxt.Format(_T("%.1f"), itAlign->stSpec.dMax);
// 		m_MeasureList.SetItemText(iRow, LIST_OOS_MAX_COLUMN_SEM, strTxt);
// 		m_MeasureList.SetItemText(iRow, LIST_OOR_MAX_COLUMN_SEM, strTxt);
// 		m_MeasureList.SetItemText(iRow, LIST_OOC_MAX_COLUMN_SEM, strTxt);
// 
// 		strTxt.Format(_T("%.1f"), itAlign->stSpec.dRange);
// 		m_MeasureList.SetItemText(iRow, LIST_WIDTHRANGE_COLUMN_SEM, strTxt);
// 		m_MeasureList.SetItemText(iRow, LIST_SIDE_COLUMN_SEM, GetMeasureSideString(static_cast<MeasureSide>(itAlign->iMeasureSide)));
// 
// 		strTxt.Format(_T("%.3f"), itAlign->MinMaxRect_In_Panel.left);
// 		m_MeasureList.SetItemText(iRow, LIST_LEFT_COLUMN_SEM, strTxt);
// 		strTxt.Format(_T("%.3f"), itAlign->MinMaxRect_In_Panel.top);
// 		m_MeasureList.SetItemText(iRow, LIST_TOP_COLUMN_SEM, strTxt);
// 		strTxt.Format(_T("%.3f"), itAlign->MinMaxRect_In_Panel.right);
// 		m_MeasureList.SetItemText(iRow, LIST_RIGHT_COLUMN_SEM, strTxt);
// 		strTxt.Format(_T("%.3f"), itAlign->MinMaxRect_In_Panel.bottom);
// 		m_MeasureList.SetItemText(iRow, LIST_BOT_COLUMN_SEM, strTxt);
// 
// 		strTxt.Format(_T("%d"), itAlign->iFeatureType);
// 		m_MeasureList.SetItemText(iRow, LIST_FEATURE_TYPE_SEM, strTxt);
// 	}
// 	else
// 	{
// 
// 		m_MeasureList.SetItemText(iRow, LIST_ITEM_COLUMN, strTitle);
// 		//m_MeasureList.SetItemText(iRow, LIST_TYPE_COLUMN, GetMeasureTypeString(static_cast<MeasureType>(itAlign->iMeasureType)));
// 		m_MeasureList.SetItemText(iRow, LIST_TYPE_COLUMN, itAlign->strMeasureType);
// 		m_MeasureList.SetItemText(iRow, LIST_OPTIC_COLUMN, OPTIC_2D_TEXT);
// 
// 		//m_MeasureList.SetItemText(iRow, LIST_MAG_COLUMN, GetMagnificationString(itAlign->bIs3D, itAlign->iScanMode));
// 		//m_MeasureList.SetItemText(iRow, LIST_ZOOM_COLUMN, GetZoomString(itAlign->bIs3D, itAlign->iScanMode));
// 		m_MeasureList.SetItemText(iRow, LIST_MAG_COLUMN, GetMagnificationString(itAlign->dLens));
// 		m_MeasureList.SetItemText(iRow, LIST_ZOOM_COLUMN, GetZoomString(itAlign->dZoom));
// 
// 		strTxt.Format(_T("%.1f"), itAlign->stSpec.dMin);
// 		m_MeasureList.SetItemText(iRow, LIST_MIN_COLUMN, strTxt);
// 		strTxt.Format(_T("%.1f"), itAlign->stSpec.dTarget);
// 		m_MeasureList.SetItemText(iRow, LIST_TARGET_COLUMN, strTxt);
// 		strTxt.Format(_T("%.1f"), itAlign->stSpec.dMax);
// 		m_MeasureList.SetItemText(iRow, LIST_MAX_COLUMN, strTxt);
// 
// 		strTxt.Format(_T("%.1f"), itAlign->stSpec.dRange);
// 		m_MeasureList.SetItemText(iRow, LIST_WIDTHRANGE_COLUMN, strTxt);
// 		m_MeasureList.SetItemText(iRow, LIST_SIDE_COLUMN, GetMeasureSideString(static_cast<MeasureSide>(itAlign->iMeasureSide)));
// 
// 		strTxt.Format(_T("%.3f"), itAlign->MinMaxRect_In_Panel.left);
// 		m_MeasureList.SetItemText(iRow, LIST_LEFT_COLUMN, strTxt);
// 		strTxt.Format(_T("%.3f"), itAlign->MinMaxRect_In_Panel.top);
// 		m_MeasureList.SetItemText(iRow, LIST_TOP_COLUMN, strTxt);
// 		strTxt.Format(_T("%.3f"), itAlign->MinMaxRect_In_Panel.right);
// 		m_MeasureList.SetItemText(iRow, LIST_RIGHT_COLUMN, strTxt);
// 		strTxt.Format(_T("%.3f"), itAlign->MinMaxRect_In_Panel.bottom);
// 		m_MeasureList.SetItemText(iRow, LIST_BOT_COLUMN, strTxt);
// 
// 		strTxt.Format(_T("%d"), itAlign->iFeatureType);
// 		m_MeasureList.SetItemText(iRow, LIST_FEATURE_TYPE, strTxt);
// 	}
// 
// 	iRow++;
// 
// 	return RESULT_GOOD;
// }

void CMeasureInfoFormView_V2::_InitialBtn()
{
	if (m_pBtnAccept == nullptr)
	{
		m_pBtnAccept = new CButtonEx();
		m_pBtnAccept->Create(_T("Accept"), WS_CHILD | WS_VISIBLE, CRect(0, 0, 100, 100),
			this, IDC_MEASURE_INFO_BUTTON_ACCEPT);
	}
	
	m_pBtnAccept->SetFontSize(13);
	m_pBtnAccept->SetFontColor(BLACK_COLOR, BLACK_COLOR);
}

void CMeasureInfoFormView_V2::_InitialSetSpec()
{
	TCHAR szPath[_MAX_PATH + 1];
	GetModuleFileName(AfxGetApp()->m_hInstance, szPath, _MAX_PATH);

	CString strCurAppDir = szPath;
	int nLen = strCurAppDir.ReverseFind('\\');
	if (nLen > 0)
		strCurAppDir = strCurAppDir.Left(nLen);

	for (int i = 0; i < SpecSet_Sem::enumItemMax; i++)
	{
		if (m_staticSpecSet[i] == nullptr)
		{
			m_staticSpecSet[i] = new CStaticPanel();
			m_staticSpecSet[i]->Create(SpecSet_Sem::pSubjectName[i], WS_CHILD | WS_VISIBLE, CRect(0, 0, 100, 100),
				this, IDC_MEASURE_INFO_STATIC_OOS+i);

			m_staticSpecSet[i]->SetPanelImage(strCurAppDir + L"\\Bin\\Img\\ModelName3.bmp");
			m_staticSpecSet[i]->SetFontSize(13);
		}

		

		if (m_BtnSpecSet[i] == nullptr)
		{
			m_BtnSpecSet[i] = new CButtonEx();
			m_BtnSpecSet[i]->Create(SpecSet_Sem::pBtntName[i], WS_CHILD | WS_VISIBLE, CRect(0, 0, 100, 100),
				this, IDC_MEASURE_INFO_BUTTON_SET_OOS+i);

			m_BtnSpecSet[i]->SetFontSize(13);
		}

		

	}

	for (int i = 0; i < SpecSet_Sem::enumMax; i++)
	{
		if (m_editSpecSet[i] == nullptr)
		{
			m_editSpecSet[i] = new CEdit();
			m_editSpecSet[i]->Create(WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(0, 0, 100, 100),
				this, IDC_MEASURE_INFO_EDIT_OOS_MIN+i);
		}
	}
}

void CMeasureInfoFormView_V2::OnUpdateButton(CCmdUI *pCmdUI)
{//CDockablePane 의 경우 Button을 Disable함. 강제로 Enable 해야함
	
	UserSetInfo_t* pUserSetInfo = GetUserSetInfo();
	if (pUserSetInfo == nullptr)
	{
		pCmdUI->Enable(FALSE);
	}

	/*if (pUserSetInfo->rcpMode == eRecipeMode::eRecipe_MeasureNSIS_Panel ||
		pUserSetInfo->rcpMode == eRecipeMode::eRecipe_MeasureSR_Panel ||
		pUserSetInfo->rcpMode == eRecipeMode::eRecipe_Monitor_Panel
		)*/
	if (pUserSetInfo->rcpMode == eRecipeMode::eRecipe_MeasureNSIS_Panel ||
		pUserSetInfo->rcpMode == eRecipeMode::eRecipe_MeasureSR_Panel)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void CMeasureInfoFormView_V2::OnBnClickedButtonAccept()
{
	UserSetInfo_t* pUserSetInfo = GetUserSetInfo();
	if (pUserSetInfo == nullptr)
		return;

	/*if (pUserSetInfo->rcpMode == eRecipeMode::eRecipe_MeasureNSIS_Panel ||
		pUserSetInfo->rcpMode == eRecipeMode::eRecipe_MeasureSR_Panel || 
		pUserSetInfo->rcpMode == eRecipeMode::eRecipe_Monitor_Panel)*/
	if (pUserSetInfo->rcpMode == eRecipeMode::eRecipe_MeasureNSIS_Panel ||
		pUserSetInfo->rcpMode == eRecipeMode::eRecipe_MeasureSR_Panel)
	{
		_SetMeasureItem_Attribute(&m_MeasureList, m_vecMeasureItem);
	}

	Update_NSIS_MeasureData();
	

}

void CMeasureInfoFormView_V2::OnBnClickedButtonSetOOS()
{
	CString strMin, strMax;
	double dMin, dMax;

	m_editSpecSet[SpecSet_Sem::enumOOS_Min]->GetWindowText(strMin);

	if (CUtils::IsNumber(strMin) == FALSE)
	{
		AfxMessageBox(_T("유효한 숫자 입력 바랍니다."));

		return;
	}

	m_editSpecSet[SpecSet_Sem::enumOOS_Max]->GetWindowText(strMax);

	if (CUtils::IsNumber(strMin) == FALSE)
	{
		AfxMessageBox(_T("유효한 숫자 입력 바랍니다."));

		return;
	}

	dMin = _ttof(strMin);
	dMax = _ttof(strMax);

	int nCount = static_cast<int>(m_vecMeasureItem.size());
	for (int i = 0; i < nCount; i++)
	{
		MeasureItem_t* pMeasureItem = m_vecMeasureItem[i];
		if (pMeasureItem == nullptr ) continue;

		pMeasureItem->stSpec.dMin = dMin;
		pMeasureItem->stSpec.dMax = dMax;
	}

	_UpdateMeasureList_Spec(m_vecMeasureItem);
}

void CMeasureInfoFormView_V2::OnBnClickedButtonSetOOR()
{
	CString strMin, strMax;
	double dMin, dMax;

	m_editSpecSet[SpecSet_Sem::enumOOR_Min]->GetWindowText(strMin);

	if (CUtils::IsNumber(strMin) == FALSE)
	{
		AfxMessageBox(_T("유효한 숫자 입력 바랍니다."));

		return;
	}

	m_editSpecSet[SpecSet_Sem::enumOOR_Max]->GetWindowText(strMax);

	if (CUtils::IsNumber(strMin) == FALSE)
	{
		AfxMessageBox(_T("유효한 숫자 입력 바랍니다."));

		return;
	}

	dMin = _ttof(strMin);
	dMax = _ttof(strMax);

	int nCount = static_cast<int>(m_vecMeasureItem.size());
	for (int i = 0; i < nCount; i++)
	{
		MeasureItem_t* pMeasureItem = m_vecMeasureItem[i];
		if (pMeasureItem == nullptr) continue;

		pMeasureItem->stSpec.dMin_OOR = dMin;
		pMeasureItem->stSpec.dMax_OOR = dMax;
	}

	_UpdateMeasureList_Spec(m_vecMeasureItem);
}

void CMeasureInfoFormView_V2::OnBnClickedButtonSetOOC()
{
	CString strMin, strMax;
	double dMin, dMax;

	m_editSpecSet[SpecSet_Sem::enumOOC_Min]->GetWindowText(strMin);

	if (CUtils::IsNumber(strMin) == FALSE)
	{
		AfxMessageBox(_T("유효한 숫자 입력 바랍니다."));

		return;
	}

	m_editSpecSet[SpecSet_Sem::enumOOC_Max]->GetWindowText(strMax);

	if (CUtils::IsNumber(strMin) == FALSE)
	{
		AfxMessageBox(_T("유효한 숫자 입력 바랍니다."));

		return;
	}

	dMin = _ttof(strMin);
	dMax = _ttof(strMax);

	int nCount = static_cast<int>(m_vecMeasureItem.size());
	for (int i = 0; i < nCount; i++)
	{
		MeasureItem_t* pMeasureItem = m_vecMeasureItem[i];
		if (pMeasureItem == nullptr) continue;

		pMeasureItem->stSpec.dMin_OOC = dMin;
		pMeasureItem->stSpec.dMax_OOC = dMax;
	}

	_UpdateMeasureList_Spec(m_vecMeasureItem);
}

CLayer *CMeasureInfoFormView_V2::GetCurEditLayer()
{//현재 편집하는 Layer Pointer//MP, TP

	UserSetInfo_t* pUserSetInfo = GetUserSetInfo();
	if (pUserSetInfo == nullptr)
		return nullptr;

	UserLayerSet_t* pUserLayerSet = GetUserLayerSet();
	if (pUserLayerSet == nullptr)
		return nullptr;

	CJobFile* pJobData = GetJobPtr();
	if (pJobData == nullptr)
		return nullptr;

	CString strLayerName = _T("");
	//Get Layer Name
	int nCount = static_cast<int>(pUserLayerSet->vcLayerSet.size());
	for (int i = 0; i < nCount; i++)
	{
		if (pUserSetInfo->rcpMode == eRecipeMode::eRecipe_MeasureNSIS_Panel)
		{
			if (pUserLayerSet->vcLayerSet[i]->strDefine == MEASURE_LAYER_TXT)
			{
				strLayerName = pUserLayerSet->vcLayerSet[i]->strLayer;
				break;
			}
		}
		else if (pUserSetInfo->rcpMode == eRecipeMode::eRecipe_MeasureSR_Panel)
		{//SR
			if (pUserLayerSet->vcLayerSet[i]->strDefine == THICKNESS_LAYER_TXT)
			{
				strLayerName = pUserLayerSet->vcLayerSet[i]->strLayer;
				break;
			}
		}
	}
	if (strLayerName == _T("")) return nullptr;

	int nStep = pUserLayerSet->nPanelStepIdx;
	if (nStep < 0) return nullptr;
	if (nStep >= static_cast<int>(pJobData->m_arrStep.GetCount())) return nullptr;

	CStep *pStep = pJobData->m_arrStep.GetAt(nStep);
	if (pStep == nullptr) return nullptr;

	//대상 Layer 
	int nLayerIndex = 0;
	CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, strLayerName, nLayerIndex);
	if (pLayer == nullptr) return nullptr;
	
	return pLayer;

}

UINT32 CMeasureInfoFormView_V2::_SetMeasureItem_Attribute(CCustomListCtrl *pMeasureList, IN vector<MeasureItem_t*> &vecMeasureItem)
{
	int nItemCount = pMeasureList->GetItemCount();
	int nMeasureCount = static_cast<int>(vecMeasureItem.size());

	if (nItemCount != nMeasureCount) return RESULT_BAD;

	CString strType;
	CString strSide;
	CString strOOS_Min;
	CString strOOS_Max;
	CString strOOR_Min;
	CString strOOR_Max;
	CString strOOC_Min;
	CString strOOC_Max;
	CString strOptic;

	vector<CString> vecAttString;
	CString strTemp;
	//신규 Att String 생성
	for (int i = 0; i < nItemCount; i++)
	{
		//Type
		strType = pMeasureList->GetItemText(i, MeasureList_Sem::enumType);
		strType.MakeLower();

		//Side
		strSide = pMeasureList->GetItemText(i, MeasureList_Sem::enumInspSide);
		strSide.MakeLower();

		//OOS
		strOOS_Min = pMeasureList->GetItemText(i, MeasureList_Sem::enumOOS_Min);
		strOOS_Max = pMeasureList->GetItemText(i, MeasureList_Sem::enumOOS_Max);

		//OOR
		strOOR_Min = pMeasureList->GetItemText(i, MeasureList_Sem::enumOOR_Min);
		strOOR_Max = pMeasureList->GetItemText(i, MeasureList_Sem::enumOOR_Max);

		//OOC
		strOOC_Min = pMeasureList->GetItemText(i, MeasureList_Sem::enumOOC_Min);
		strOOC_Max = pMeasureList->GetItemText(i, MeasureList_Sem::enumOOC_Max);

		//Optic
		strOptic = pMeasureList->GetItemText(i, MeasureList_Sem::enumOptic);

		strTemp.Format(_T("%s,%s,%.1lf,%.1lf,%.1lf,%.1lf,%.1lf,%.1lf,%s"),
			strType, strSide, _ttof(strOOS_Min), _ttof(strOOS_Max),
			_ttof(strOOR_Min), _ttof(strOOR_Max), _ttof(strOOC_Min), _ttof(strOOC_Max), strOptic);

		vecAttString.push_back(strTemp);
	}

	if (nMeasureCount != static_cast<int>(vecAttString.size()))
		return RESULT_BAD;

	CLayer *pLayer = GetCurEditLayer();
	if (pLayer == nullptr) return RESULT_BAD;

	for (int i = 0; i < nMeasureCount; i++)
	{
		CFeature *pFeature = (CFeature*)vecMeasureItem[i]->pFeature;
		if (pFeature == nullptr) continue;

		if (vecMeasureItem[i]->strMeasureType.Find(_T("align")) >= 0 ||
			vecMeasureItem[i]->strMeasureType.Find(_T("ALIGN")) >= 0)
		{//Align은 Attribute를 수정하지 않는다.
			continue;
		}

		//Attribute는 0을 기준으로 한다.
		//Attribute가 하나라도 없다면, 추가
		CAttribute *pAtt_New = nullptr;
		if (pLayer->m_FeatureFile.m_arrAttribute.GetCount() == 0)
		{
			pAtt_New = new CAttribute();
			pAtt_New->m_iSerialNum = 0;
			pAtt_New->m_strName = _T(".string");

			pLayer->m_FeatureFile.m_arrAttribute.Add(pAtt_New);
			pFeature->m_arrAttribute.Add(pAtt_New);
		}
		else
		{//Layer의 att는 있지만, Feature에 연결되어 있는 att가 있는지 체크
			if (pFeature->m_arrAttribute.GetCount() == 0)
			{//feature에 연결되어 있는 att가 없다면, 0번째 에 있는 att 추가
				pFeature->m_arrAttribute.Add(pLayer->m_FeatureFile.m_arrAttribute.GetAt(0));
			}			
		}

		//Attribute String 은 0을 기준으로 한다.
		//추가하는 Attribute String 이 같은게 있는지 체크
		BOOL bCompare = FALSE;
		int nAttStringCount = static_cast<int>(pLayer->m_FeatureFile.m_arrAttributeTextString.GetCount());
		CAttributeTextString * pAttString;
		for (int nString = 0; nString < nAttStringCount; nString++)
		{
			CAttributeTextString *pAttStringTemp = pLayer->m_FeatureFile.m_arrAttributeTextString.GetAt(nString);

			if (pAttStringTemp->m_strText == vecAttString[i])
			{
				pAttString = pAttStringTemp;
				bCompare = TRUE;
				break;
			}
		}

		if (bCompare == FALSE)
		{//없다면 신규로 생성
			pAttString = new CAttributeTextString;
			pAttString->m_iSerialNum = static_cast<int>(pLayer->m_FeatureFile.m_arrAttributeTextString.GetCount());//마지막 번호 부여
			pAttString->m_strText = vecAttString[i];

			pLayer->m_FeatureFile.m_arrAttributeTextString.Add(pAttString);
		}

		if (pFeature->m_arrAttributeTextString.GetCount() == 0)
		{
			pFeature->m_arrAttributeTextString.Add(pAttString);
		}
		else
		{
			pFeature->m_arrAttributeTextString[0] = pAttString;
		}
	}

	return RESULT_GOOD;
}
