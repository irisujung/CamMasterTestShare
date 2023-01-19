
#pragma once

class CHeaderCtrlEx : public CMFCHeaderCtrl
{
public:
	CHeaderCtrlEx();

public:
	virtual ~CHeaderCtrlEx();
	// Generated message map functions
protected:

	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	
};
