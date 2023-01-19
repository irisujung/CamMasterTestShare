

#pragma once

extern UINT NEAR WM_COMBOBOX_CHANGE;

class CInPlaceCombo : public CComboBox
{
public:

	// Returns the instance of the class
	static CInPlaceCombo* GetInstance(); 

	// Deletes the instance of the class
	static void DeleteInstance(); 

	// Creates the Windows combo control and attaches it to the object, if needed and shows the combo ctrl
	BOOL ShowComboCtrl(DWORD dwStyle, const CRect& rCellRect, CWnd* pParentWnd, UINT uiResourceID,
					   int iRowIndex, int iColumnIndex, CStringList* pDropDownList, CString strCurSelecetion = L"", int iCurSel = -1);
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnCloseup();	
	DECLARE_MESSAGE_MAP()

private:

// Implementation
	// Constructor
	CInPlaceCombo();

	// Hide the copy constructor and operator =
	CInPlaceCombo (CInPlaceCombo&) {}

	int operator = (CInPlaceCombo) {}

	// Destructor
	virtual ~CInPlaceCombo();

// Attributes

	// Index of the item in the list control
	int m_iRowIndex;

	// Index of the subitem in the list control
	int m_iColumnIndex;

	// To indicate whether ESC key was pressed
	BOOL m_bESC;
	
	// Singleton instance
	static CInPlaceCombo* m_pInPlaceCombo;

	// Previous selected string value in the combo control
	CString m_strWindowText;

	// List of items to be shown in the drop down
	CStringList m_DropDownList;
};