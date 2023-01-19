
#pragma once

#include <afxtempl.h>
#include <vector>

using namespace std;

class CInPlaceEdit;
class CInPlaceCombo;


//
typedef struct CUSTOM_tagNMITEM
{
	HWND hwndFrom;
	BOOL bChecked;
	int	iItem;
	int	iSubItem;
	int cmbIndex;
	CString sText;
	COLORREF color;
} CUSTOM_NMITEM;



typedef struct ListCtrlItem
{	
	UINT	iRowId;
	UINT	iColId;
	COLORREF textColor;
} st_ListCtrlItem;

// User define message 
// This message is posted to the parent

extern UINT NEAR WM_LISTCTRL_COMBOBOX_CHANGE;
extern UINT NEAR WM_LISTCTRL_LAYERCOLOR_CHANGE;
extern UINT NEAR WM_LISTCTRL_ITEM_RCLICK;
extern UINT NEAR WM_LISTCTRL_EDIT_CHANGED;

#define LIST_BK_COLOR			RGB(255, 255, 255)

class CCustomListCtrl : public CListCtrl
{
private:
	UINT					m_iColorColumn;
	vector<UINT>			m_vecColorRow;
	vector<UINT>			m_vecItemColor;	
	vector<st_ListCtrlItem>	m_vecListCtrlItem;	
	vector<UINT>			m_vecComboCol;
	vector<CString>			m_vecComboList;
	double					m_nEditMinNum;
	double					m_nEditMaxNum;
	COLORREF				m_defaultTextColor;
	int						m_nComboItem;
	int						m_nComboSubItem;

	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);

public:
	void SetColorColumn(UINT iCol) {
		m_iColorColumn = iCol;
	};
	void SetDefaultTextColor(COLORREF ulColor) {
		m_defaultTextColor = ulColor;
	};
	void SetEditBoxMinMax(double dMin, double dMax) {
		m_nEditMinNum = dMin;
		m_nEditMaxNum = dMax;
	}
	void SetItemColor(UINT iRow, COLORREF ulColor);
	void SetItemTextColor(UINT iRow, UINT iCol, COLORREF ulColor);
	void SetComboBoxList(UINT iCol, CStringArray& strArray);
	void SetComboBoxCurSel(UINT iRow, UINT iCol, UINT nIndex);
	

	void ClearComboBoxList();

	CString	 GetComboText(int nItem, int nSubItem);
	COLORREF GetItemColor(UINT iRow);

// Implementation
	// Constructor
	CCustomListCtrl();

	// Destructor
	virtual ~CCustomListCtrl();

	// Sets/Resets the column which support the in place combo box
	void SetComboColumns(int iColumnIndex, bool bSet = true);

	// Sets/Resets the column which support the in place edit control
	void SetReadOnlyColumns(int iColumnIndex, bool bSet = true);

	// Sets the valid characters for the edit ctrl

	// Sets the vertical scroll
	void EnableVScroll(bool bEnable = true);

	// Sets the horizontal scroll
	void EnableHScroll(bool bEnable = true);
	int GetColumns();
protected:

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);	
	afx_msg LRESULT OnComboChange(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
			
// Implementation

	// Returns the row & column index of the column on which mouse click event has occured
	bool HitTestEx(CPoint& rHitPoint, int* pRowIndex, int* pColumnIndex) const;

	// Creates and displays the in place combo box
	CInPlaceCombo* ShowInPlaceList(int iRowIndex, int iColumnIndex, CStringList& rComboItemsList, 
								   CString strCurSelecetion = L"", int iSel = -1);

	// Creates and displays the in place Edit box
	CInPlaceEdit* ShowInPlaceEdit(int iRowIndex, int iColumnIndex, CString& rstrCurSelection);

	// Checks whether column is read only
	bool IsReadOnly(int iColumnIndex);

	// Calculates the cell rect
	void CalculateCellRect(int iColumnIndex, int iRowIndex, CRect& robCellRect);

	// Checks whether column supports in place combo box
	bool IsCombo(int iColumnIndex);

	

	// Scrolls the list ctrl to bring the in place ctrl to the view
	void ScrollToView(int iColumnIndex, /*int iOffSet, */CRect& obCellRect);

// Attributes
	
	// List of columns that support the in place combo box
	CList<int, int> m_ComboSupportColumnsList;

	// List of columns that are read only
	CList<int, int> m_ReadOnlyColumnsList;


	// The window style of the in place edit ctrl
	DWORD m_dwEditCtrlStyle;

	// The window style of the in place combo ctrl
	DWORD m_dwDropDownCtrlStyle;
public:
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
};
