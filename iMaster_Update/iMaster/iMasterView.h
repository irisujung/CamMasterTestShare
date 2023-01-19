
// iMasterView.h: CiMasterView 클래스의 인터페이스
//

#pragma once

#include "resource.h"
#include "../iCommonIF/CommonIF-Error.h"
#include "../iCommonIF/CommonIF-Define.h"
#include "../iCommonIF/CommonIF-Struct.h"
#include "FeatureEditFormView.h"
#include "FormViewDocPane.h"

class CiMasterView : public CFormView
{
	friend class CFeatureEditFormView;
protected: // serialization에서만 만들어집니다.
	CiMasterView();
	DECLARE_DYNCREATE(CiMasterView)

public:
	enum { IDD = IDD_DISPLAY_FORM };
	CFeatureEditFormView *m_pFeatureEditFormView;
	CFormViewDocPane *m_pFormViewDocPane;
	/*CMainFrame *m_pMainFrame;*/

// 특성입니다.
public:
	CiMasterDoc* GetDocument() const;

	PointDXY m_Move = CPointD(0, 0);
private:
	ViewInfo_t		m_stViewInfo;
	

// 	enum eMouseTool	m_enTool;
// 	BOOL			m_bMoveRMouse;
// 	BOOL			m_bZoomRect;
// 	CPoint			m_prevPt;
// 	CPoint			m_movePt;
// 	CPoint			m_cursorPt;
// 	RECT			m_rcZoomRect, m_rcZoomRectTmp;

// 작업입니다.
public:
	void		 ResetView();
	void		 InitCursorPt();

// 	void         SetMouseTool(eMouseTool eTool) { m_enTool = eTool; }
// 	eMouseTool	 GetMouseTool() { return m_enTool; }
	//void		 DrawZoomBox(CDC *pDC, UINT nFlags, CPoint point);
	
// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CiMasterView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate();

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // iMasterView.cpp의 디버그 버전
inline CiMasterDoc* CiMasterView::GetDocument() const
   { return reinterpret_cast<CiMasterDoc*>(m_pDocument); }
#endif

