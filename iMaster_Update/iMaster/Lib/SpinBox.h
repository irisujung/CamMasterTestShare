#pragma once

extern UINT NEAR WM_SPINBOX_CHANGING;
extern UINT NEAR WM_SPINBOX_DELTA;

/////////////////////////////////////////////////////////////////////////////
// CSpinBox window

class CSpinBox : public CSpinButtonCtrl
{
// Construction
public:
	CSpinBox();

// Attributes
	// Singleton instance
	static CSpinBox* m_pXSpinBox;

// Operations
public:
	// Returns the instance of the class
	static CSpinBox* GetInstance();

	// Deletes the instance of the class
	static void DeleteInstance();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpinBox)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSpinBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSpinBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDeltapos(NMHDR *pNMHDR, LRESULT *pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
