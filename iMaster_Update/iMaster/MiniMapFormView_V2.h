#pragma once



// CMiniMapFormView_V2 폼 보기

class CMiniMapFormView_V2 : public CFormView
{
	DECLARE_DYNCREATE(CMiniMapFormView_V2)

public:
	CMiniMapFormView_V2();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CMiniMapFormView_V2();

	static CMiniMapFormView_V2 * CreateOne(CWnd* pParent);


public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MINIMAP_FORM_V2 };
#endif

private:
	BOOL	m_bInit = FALSE;
	BOOL	bCopy	= FALSE;	

public:
	virtual void OnInitialUpdate();
	//static CMiniMapFormView_V2 *CreateOne(CWnd *pParent);

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};


