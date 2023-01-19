
#include "pch.h"
#include "CustomListCtrl.h"
#include "InPlaceCombo.h"
#include "InPlaceEdit.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#defines
#define FIRST_COLUMN				0
#define MIN_COLUMN_WIDTH			10
#define MAX_DROP_DOWN_ITEM_COUNT	10

UINT NEAR WM_LISTCTRL_COMBOBOX_CHANGE = ::RegisterWindowMessage(_T("WM_LISTCTRL_COMBOBOX_CHANGE"));
UINT NEAR WM_LISTCTRL_LAYERCOLOR_CHANGE = ::RegisterWindowMessage(_T("WM_LISTCTRL_LAYERCOLOR_CHANGE"));
UINT NEAR WM_LISTCTRL_ITEM_RCLICK = ::RegisterWindowMessage(_T("WM_LISTCTRL_ITEM_RCLICK"));
UINT NEAR WM_LISTCTRL_EDIT_CHANGED = ::RegisterWindowMessage(_T("WM_LISTCTRL_EDIT_CHANGED"));



// CCustomListCtrl

CCustomListCtrl::CCustomListCtrl()
{
	m_ComboSupportColumnsList.RemoveAll();
	m_ReadOnlyColumnsList.RemoveAll();
	m_dwEditCtrlStyle = ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_LEFT | ES_NOHIDESEL;
	m_dwDropDownCtrlStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | 
							CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL;
	m_defaultTextColor = RGB(0, 0, 0);
}

CCustomListCtrl::~CCustomListCtrl()
{
	CInPlaceCombo::DeleteInstance();  
	CInPlaceEdit::DeleteInstance();
	m_nEditMinNum = -1;
	m_nEditMaxNum = -1;
	m_nComboItem = -1;
	m_nComboSubItem = -1;
}


BEGIN_MESSAGE_MAP(CCustomListCtrl, CListCtrl)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnNMDblclk)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginLabelEdit)	
	ON_REGISTERED_MESSAGE(WM_COMBOBOX_CHANGE, OnComboChange)
	ON_NOTIFY_REFLECT(NM_RCLICK, &CCustomListCtrl::OnNMRClick)
END_MESSAGE_MAP()

// CCustomListCtrl message handlers

CInPlaceCombo* CCustomListCtrl::ShowInPlaceList(int iRowIndex, int iColumnIndex, CStringList& rComboItemsList, 
											   CString strCurSelecetion , int iSel )
{
	// The returned obPointer should not be saved
	// Make sure that the item is visible
	if (!EnsureVisible(iRowIndex, TRUE))
	{
		return NULL;
	}

	//// Make sure that iColumnIndex is valid 
	//CHeaderCtrl* pHeader = static_cast<CHeaderCtrl*> (GetDlgItem(FIRST_COLUMN));

	//int iColumnCount = pHeader->GetItemCount();

	//if (iColumnIndex >= iColumnCount || GetColumnWidth(iColumnIndex) < MIN_COLUMN_WIDTH) 
	//{
	//	return NULL;
	//}

	// Calculate the rectangle specifications for the combo box
	CRect obCellRect(0, 0, 0, 0);
	CalculateCellRect(iColumnIndex, iRowIndex, obCellRect);

	int iHeight = obCellRect.Height();  
	int iCount = static_cast<int>(rComboItemsList.GetCount());

	iCount = (iCount < MAX_DROP_DOWN_ITEM_COUNT) ? 
		iCount + MAX_DROP_DOWN_ITEM_COUNT : (MAX_DROP_DOWN_ITEM_COUNT + 1); 

	obCellRect.bottom += iHeight * iCount; 

	// Create the in place combobox
	CInPlaceCombo::DeleteInstance();
	CInPlaceCombo* pInPlaceCombo = CInPlaceCombo::GetInstance();	
	pInPlaceCombo->ShowComboCtrl(m_dwDropDownCtrlStyle, obCellRect, this, 0, iRowIndex, iColumnIndex, &rComboItemsList, 
								 strCurSelecetion, iSel);

	m_nComboItem = iRowIndex;
	m_nComboSubItem = iColumnIndex;
	
	return pInPlaceCombo;
}

CInPlaceEdit* CCustomListCtrl::ShowInPlaceEdit(int iRowIndex, int iColumnIndex, CString& rstrCurSelection)
{
	
	// Calculate the rectangle specifications for the combo box
	CRect obCellRect(0, 0, 0, 0);
	CalculateCellRect(iColumnIndex, iRowIndex, obCellRect);

	obCellRect.bottom = obCellRect.bottom - 2;
	obCellRect.left = obCellRect.left + 2;

	CInPlaceEdit::DeleteInstance();
	CInPlaceEdit* pInPlaceEdit = CInPlaceEdit::GetInstance();

	pInPlaceEdit->SetMinNum(m_nEditMinNum);
	pInPlaceEdit->SetMaxNum(m_nEditMaxNum);

	pInPlaceEdit->ShowEditCtrl(m_dwEditCtrlStyle, obCellRect, this, 0,
		iRowIndex, iColumnIndex,
		 rstrCurSelection);


	return pInPlaceEdit;
}

void CCustomListCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	CColorDialog dlgColors;
	if (pNMItemActivate->iSubItem == m_iColorColumn)
	{
		if (pNMItemActivate->iItem != -1)
		{
			if (dlgColors.DoModal() == IDOK)
			{				
				COLORREF userColor = dlgColors.GetColor();
				SetItemColor(pNMItemActivate->iItem, userColor);
				GetParent()->SendMessage(WM_LISTCTRL_LAYERCOLOR_CHANGE, pNMItemActivate->iItem, userColor);
			}
		}
	}
	Invalidate();

	// TODO: Add your control notification handler code here
	*pResult = 0;	
}

	

void CCustomListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW lpLVCustomDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here

	int iCol = lpLVCustomDraw->iSubItem;
	int iRow = static_cast<int>(lpLVCustomDraw->nmcd.dwItemSpec);
	switch (lpLVCustomDraw->nmcd.dwDrawStage)
	{
	case CDDS_ITEMPREPAINT:
	case CDDS_PREPAINT:
	case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
		lpLVCustomDraw->clrText = m_defaultTextColor;
		lpLVCustomDraw->clrTextBk = LIST_BK_COLOR;
		
		for (size_t i = 0; i < m_vecColorRow.size(); i++)
		{
			if (iRow == m_vecColorRow[i])
			{
				if (iCol == m_iColorColumn)
				{
					lpLVCustomDraw->clrTextBk = m_vecItemColor[i];
				}				
			}
		}
		for (size_t i = 0; i < m_vecListCtrlItem.size(); i++)
		{
			if (iRow == m_vecListCtrlItem[i].iRowId && iCol == m_vecListCtrlItem[i].iColId)
			{				 
				 lpLVCustomDraw->clrText = m_vecListCtrlItem[i].textColor;
			}
		}
		break;

	}
	*pResult = 0;
	*pResult |= CDRF_DODEFAULT;
	*pResult |= CDRF_NOTIFYPOSTPAINT;
	*pResult |= CDRF_NOTIFYITEMDRAW;
	*pResult |= CDRF_NOTIFYSUBITEMDRAW;
}



void CCustomListCtrl::OnLButtonDown(UINT iFlags, CPoint obPoint) 
{
	// TODO: Add your message handler code here and/or call default

	int iColumnIndex = -1;
	int iRowIndex = -1;

	// Get the current column and row
	if (!HitTestEx(obPoint, &iRowIndex, &iColumnIndex))
	{
		return;
	}

	CListCtrl::OnLButtonDown(iFlags, obPoint);
	
	// If column is not read only then
	// If the SHIFT or CTRL key is down call the base class
	// Check the high bit of GetKeyState to determine whether SHIFT or CTRL key is down
	if ((GetKeyState(VK_SHIFT) & 0x80) || (GetKeyState(VK_CONTROL) & 0x80))
	{
		return;
	}

	// Get the current selection before creating the in place combo box
	CString strCurSelection = GetItemText(iRowIndex, iColumnIndex);
	
	if (-1 != iRowIndex)
	{
		UINT flag = LVIS_FOCUSED;
		
			// Add check for LVS_EDITLABELS
			if (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS)
			{
				// If combo box is supported
				// Create and show the in place combo box
				if (IsCombo(iColumnIndex))
				{

					CStringList obComboItemsList;						
					for (size_t i = 0; i < m_vecComboCol.size(); i++)
					{
						if (iColumnIndex == m_vecComboCol[i])
						{							
							obComboItemsList.AddTail(m_vecComboList[i]);
						}
					}					
					CInPlaceCombo* pInPlaceComboBox = ShowInPlaceList(iRowIndex, iColumnIndex, obComboItemsList, strCurSelection);
					ASSERT(pInPlaceComboBox); 
					
					// Set the selection to previous selection
					pInPlaceComboBox->SelectString(-1, strCurSelection);
				}
				// If combo box is not read only
				// Create and show the in place edit control
				else if (!IsReadOnly(iColumnIndex))
				{
					CInPlaceEdit* pInPlaceEdit = ShowInPlaceEdit(iRowIndex, iColumnIndex, strCurSelection);
				}				
			}
		
	}  
}

void CCustomListCtrl::ClearComboBoxList()
{
	m_vecComboCol.clear();
	m_vecComboList.clear();	
}

void CCustomListCtrl::SetComboBoxList(UINT iCol, CStringArray& strArray)
{
	for (int i = 0; i < strArray.GetCount(); i++)
	{
		m_vecComboCol.push_back(iCol);
		m_vecComboList.push_back(strArray.GetAt(i));
	}
}

void CCustomListCtrl::SetItemTextColor(UINT iRow, UINT iCol, COLORREF ulColor)
{
	BOOL bAddnew = TRUE;	
	for (size_t i = 0; i < m_vecListCtrlItem.size(); i++)
	{		
		if (iRow == m_vecListCtrlItem[i].iRowId && iCol == m_vecListCtrlItem[i].iColId)
		{
			m_vecListCtrlItem[i].textColor = ulColor;
			bAddnew = FALSE;
		}		
	}
	if (bAddnew)
	{
		st_ListCtrlItem stItem;
		stItem.iRowId = iRow;
		stItem.iColId = iCol;
		stItem.textColor = ulColor;		
		m_vecListCtrlItem.push_back(stItem);
	}
}

void CCustomListCtrl::SetItemColor(UINT iRow, COLORREF ulColor)
{
	BOOL bAddnew = TRUE;
	for (size_t i = 0; i < m_vecColorRow.size(); i++)
	{
		if (iRow == m_vecColorRow[i])
		{
			m_vecItemColor[i] = ulColor;
			bAddnew = FALSE;
		}
	}

	if (bAddnew)
	{
		m_vecColorRow.push_back(iRow);
		m_vecItemColor.push_back(ulColor);
	}
}

COLORREF CCustomListCtrl::GetItemColor(UINT iRow)
{
	COLORREF clrItem = LIST_BK_COLOR;
	for (size_t i = 0; i < m_vecColorRow.size(); i++)
	{
		if (iRow == m_vecColorRow[i])
		{
			clrItem = m_vecItemColor[i];			
		}
	}
	return  clrItem;
}


void CCustomListCtrl::SetComboBoxCurSel(UINT iRow, UINT iCol, UINT nIndex)
{
	UINT nComboIdx = 0;
	for (size_t i = 0; i < m_vecComboCol.size(); i++)
	{
		if (iCol == m_vecComboCol[i])
		{
			if (nComboIdx == nIndex)
			{
				CString sText = m_vecComboList[i];
				SetItemText(iRow, iCol, sText);

				CWnd *pWnd = GetParent();
				if (!pWnd)
					pWnd = GetOwner();

				if (pWnd && ::IsWindow(pWnd->m_hWnd))
				{
					CUSTOM_NMITEM NMI;
					NMI.hwndFrom = m_hWnd;
					NMI.iItem = iRow;
					NMI.iSubItem = iCol;
					NMI.cmbIndex = nIndex;
					NMI.sText = sText;
					pWnd->SendMessage(WM_LISTCTRL_COMBOBOX_CHANGE, (WPARAM)&NMI, NULL);
				}
				
			}
			nComboIdx++;
		}
	}
}

bool CCustomListCtrl::HitTestEx(CPoint &obPoint, int* pRowIndex, int* pColumnIndex) const
{
	if (!pRowIndex || !pColumnIndex)
	{
		return false;
	}

	// Get the row index
	*pRowIndex = HitTest(obPoint, NULL);

	if (pColumnIndex)
	{
		*pColumnIndex = 0;
	}

	// Make sure that the ListView is in LVS_REPORT
	if ((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT)
	{
		return false;
	}

	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);

	int iColumnCount = pHeader->GetItemCount();

	// Get bounding rect of item and check whether obPoint falls in it.
	CRect obCellRect;
	GetItemRect(*pRowIndex, &obCellRect, LVIR_BOUNDS);
	
	if (obCellRect.PtInRect(obPoint))
	{
		// Now find the column
		for (*pColumnIndex = 0; *pColumnIndex < iColumnCount; (*pColumnIndex)++)
		{
			int iColWidth = GetColumnWidth(*pColumnIndex);
			
			if (obPoint.x >= obCellRect.left && obPoint.x <= (obCellRect.left + iColWidth))
			{
				return true;
			}
			obCellRect.left += iColWidth;
		}
	}
	return false;
}

void CCustomListCtrl::SetComboColumns(int iColumnIndex, bool bSet /*= true*/)
{
	// If the Column Index is not present && Set flag is false
	// Then do nothing 
	// If the Column Index is present && Set flag is true
	// Then do nothing
	POSITION Pos = m_ComboSupportColumnsList.Find(iColumnIndex);

	// If the Column Index is not present && Set flag is true
	// Then Add to list
	if ((NULL == Pos) && bSet) 
	{
		m_ComboSupportColumnsList.AddTail(iColumnIndex); 
	}

	// If the Column Index is present && Set flag is false
	// Then Remove from list
	if ((NULL != Pos) && !bSet) 
	{
		m_ComboSupportColumnsList.RemoveAt(Pos); 
	}
}


void CCustomListCtrl::SetReadOnlyColumns(int iColumnIndex, bool bSet /*= true*/)
{
	// If the Column Index is not present && Set flag is false
	// Then do nothing 
	// If the Column Index is present && Set flag is true
	// Then do nothing
	POSITION Pos = m_ReadOnlyColumnsList.Find(iColumnIndex);

	// If the Column Index is not present && Set flag is true
	// Then Add to list
	if ((NULL == Pos) && bSet)
	{
		m_ReadOnlyColumnsList.AddTail(iColumnIndex);
	}

	// If the Column Index is present && Set flag is false
	// Then Remove from list
	if ((NULL != Pos) && !bSet)
	{
		m_ReadOnlyColumnsList.RemoveAt(Pos);
	}
}


bool CCustomListCtrl::IsReadOnly(int iColumnIndex)
{
	if (m_ReadOnlyColumnsList.Find(iColumnIndex))
	{
		return true;
	}
	return false;
}

bool CCustomListCtrl::IsCombo(int iColumnIndex)
{
	if (m_ComboSupportColumnsList.Find(iColumnIndex))
	{
		return true;
	}
	return false;
}


void CCustomListCtrl::CalculateCellRect(int iColumnIndex, int iRowIndex, CRect& robCellRect)
{
	GetItemRect(iRowIndex, &robCellRect, LVIR_BOUNDS);
	
	CRect rcClient;
	GetClientRect(&rcClient);

	if (robCellRect.right > rcClient.right) 
	{
		robCellRect.right = rcClient.right;
	}
	ScrollToView(iColumnIndex, robCellRect); 
}


void CCustomListCtrl::EnableHScroll(bool bEnable )
{
	if (bEnable)
	{
		m_dwDropDownCtrlStyle |= WS_HSCROLL;
	}
	else
	{
		m_dwDropDownCtrlStyle &= ~WS_HSCROLL;
	}	
}

void CCustomListCtrl::EnableVScroll(bool bEnable)
{
	if (bEnable)
	{
		m_dwDropDownCtrlStyle |= WS_VSCROLL;
	}
	else
	{
		m_dwDropDownCtrlStyle &= ~WS_VSCROLL;
	}
}

void CCustomListCtrl::ScrollToView(int iColumnIndex, CRect& robCellRect)
{
	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect(&rcClient);

	int iColumnWidth = GetColumnWidth(iColumnIndex);

	// Get the column iOffset
	int iOffSet = 0;
	for (int iIndex_ = 0; iIndex_ < iColumnIndex; iIndex_++)
	{
		iOffSet += GetColumnWidth(iIndex_);
	}

	// If x1 of cell rect is < x1 of ctrl rect or
	// If x1 of cell rect is > x1 of ctrl rect or **Should not ideally happen**
	// If the width of the cell extends beyond x2 of ctrl rect then
	// Scroll

	CSize obScrollSize(0, 0);

	if (((iOffSet + robCellRect.left) < rcClient.left) || 
		((iOffSet + robCellRect.left) > rcClient.right))
	{
		obScrollSize.cx = iOffSet + robCellRect.left;
	}
	else if ((iOffSet + robCellRect.left + iColumnWidth) > rcClient.right)
	{
		obScrollSize.cx = iOffSet + robCellRect.left + iColumnWidth - rcClient.right;
	}

	Scroll(obScrollSize);
	robCellRect.left -= obScrollSize.cx;
	
	// Set the width to the column width
	robCellRect.left += iOffSet;
	robCellRect.right = robCellRect.left + iColumnWidth;
}

void CCustomListCtrl::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here

	// Update the item text with the new text
	SetItemText(pDispInfo->item.iItem, pDispInfo->item.iSubItem, pDispInfo->item.pszText);

//	GetParent()->SendMessage(WM_VALIDATE, GetDlgCtrlID(), (LPARAM)pDispInfo);

	*pResult = 0;
}

void CCustomListCtrl::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	if (IsReadOnly(pDispInfo->item.iSubItem))
	{
		*pResult = 1;
		return;
	}

	*pResult = 0;
}


CString	CCustomListCtrl::GetComboText(int nItem, int nSubItem)
{
	ASSERT(nItem >= 0);
	ASSERT(nItem < GetItemCount());
	if ((nItem < 0) || nItem >= GetItemCount())
		return _T("");
	ASSERT(nSubItem >= 0);
	ASSERT(nSubItem < GetColumns());
	if ((nSubItem < 0) || nSubItem >= GetColumns())
		return _T("");

	CString str(_T(""));
	str = GetItemText(nItem, nSubItem);

	return str;
}

int CCustomListCtrl::GetColumns()
{	
	return GetHeaderCtrl()->GetItemCount();
}

LRESULT CCustomListCtrl::OnComboChange(WPARAM wParam, LPARAM lParam)
{
	if (m_nComboItem == -1 || m_nComboSubItem == -1)
		return 0L;

	CInPlaceCombo *pComboBox = CInPlaceCombo::GetInstance();
	if (pComboBox == NULL || pComboBox->m_hWnd == NULL)
		return 0L;

	int nIndex = pComboBox->GetCurSel();
	if (nIndex < 0)
		return 0L;

	CString sText(_T(""));
	pComboBox->GetLBText(nIndex, sText);
	SetItemText(m_nComboItem, m_nComboSubItem, sText);	

	CWnd *pWnd = GetParent();
	if (!pWnd)
		pWnd = GetOwner();

	if (pWnd && ::IsWindow(pWnd->m_hWnd))
	{
		CUSTOM_NMITEM NMI;
		NMI.hwndFrom = m_hWnd;
		NMI.iItem = m_nComboItem;
		NMI.iSubItem = m_nComboSubItem;
		NMI.cmbIndex = nIndex;
		NMI.sText = sText;
		pWnd->SendMessage(WM_LISTCTRL_COMBOBOX_CHANGE, (WPARAM)&NMI, NULL);
	}	

	m_nComboItem = -1;
	m_nComboSubItem = -1;
	return 0L;
}

void CCustomListCtrl::OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	/*
	CPoint CurrentPosition;
	::GetCursorPos(&CurrentPosition);

	INT nIndex = -1;
	CListCtrl &ListCtrl = GetListCtrl();

	ListCtrl.ScreenToClient(&CurrentPosition);
	nIndex = ListCtrl.HitTest(CurrentPosition);
	*/

	if (pNMItemActivate->iItem != -1)
	{
		GetParent()->SendMessage(WM_LISTCTRL_ITEM_RCLICK, pNMItemActivate->iItem, 0);
	}

	*pResult = 0;
}
