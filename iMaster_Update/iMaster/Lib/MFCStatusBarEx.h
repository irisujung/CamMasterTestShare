#pragma once



class CMFCStatusBarEx : public CMFCStatusBar
{
	DECLARE_DYNAMIC(CMFCStatusBarEx)

public:
	CMFCStatusBarEx();
	~CMFCStatusBarEx();

private:

protected:
	virtual void OnDrawPane(CDC* pDC, CMFCStatusBarPaneInfo* pPane);
};
