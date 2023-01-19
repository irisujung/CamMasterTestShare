
#pragma once

class CInPlaceEdit : public CEdit
{

public:
	
// Implementation
	
	// Returns the instance of the class
	static CInPlaceEdit* GetInstance(); 

	// Deletes the instance of the class
	static void DeleteInstance(); 

	// Creates the Windows edit control and attaches it to the object
	// Shows the edit ctrl
	BOOL ShowEditCtrl(DWORD dwStyle, const RECT& rCellRect, CWnd* pParentWnd, 
					  UINT uiResourceID, int iRowIndex, int iColumnIndex,
					   CString& rstrCurSelection);

	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CInPlaceEdit* pInPlaceEdit;

	void SetMinNum(double nMinNum) { m_nMinNum = nMinNum; }
	void SetMaxNum(double nMaxNum) { m_nMaxNum = nMaxNum; }
protected:	
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnSpinDelta(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

private:

// Implementation

	// Constructor
	CInPlaceEdit();

	// Hide the copy constructor and operator =
	CInPlaceEdit (CInPlaceEdit&) {}

	int operator = (CInPlaceEdit) {}
	
	// Destructor
	virtual ~CInPlaceEdit();

// Attributes

	// Index of the item in the list control
	int m_iRowIndex;
	double m_nMinNum;
	double m_nMaxNum;

	int m_nVK;
	// Index of the subitem in the list control
	int m_iColumnIndex;

	// To indicate whether ESC key was pressed
	BOOL m_bESC;

	// Singleton instance
	static CInPlaceEdit* m_pInPlaceEdit;

	// Previous string value in the edit control
	CString m_strWindowText;
};
