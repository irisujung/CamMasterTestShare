#pragma once

#include "../iCommonIF/CommonIF-Define.h"
#include "../iCommonIF/CommonIF-Error.h"
#include "SystemSpec.h"
#include "CFeatureEdit_DeleteDlg.h"

#define COLOR_BLACK							RGB(0,0,0)
#define COLOR_WHITE							RGB(255,255,255)
#define COLOR_GREEN1						RGB(204, 255, 204)
#define COLOR_GREEN2						RGB(64, 96, 96)
#define COLOR_GRAY1							((COLORREF)-1)


#define	STRING_RESIZE						_T("Resize")
#define STRING_RESIZE_PERCENT				_T("Resize(%)")
#define	STRING_RESIZE_DESC					_T("Set Feature Size")
#define STRING_ROTATE						_T("Rotate")
#define STRING_MIRROR						_T("Mirror X")
#define STRING_ROTATE_DESC					_T("Set Feature Rotate")
#define STRING_MIRROR_DESC					_T("Set Feature Mirror X")
#define STRING_MOVE							_T("Move")
#define STRING_MOVE_DESC	                _T("Set Feature Position")
#define STRING_COPY							_T("Copy")
#define STRING_COPY_DESC					_T("Set Feature Copy")
#define STRING_DELETE						_T("Delete")
#define STRING_DELETE_DESC					_T("Set Feature Delete")
#define STRING_ROTATE_0						_T("0")
#define STRING_ROTATE_90					_T("90")
#define STRING_ROTATE_180					_T("180")
#define STRING_ROTATE_270					_T("270")
#define STRING_MIRROR_NO					_T("No")
#define STRING_MIRROR_YES					_T("Yes")
#define STRING_MOVE_X						_T("X축(mm)")
#define STRING_MOVE_Y						_T("Y축(mm)")
#define STRING_MOVE_VALUE					_T("키보드 이동거리(mm)")
#define STRING_COPY_TIME					_T("Repeat")
#define STRING_COPY_ONCE					_T("No")
#define STRING_COPY_REPEAT					_T("Yes")
#define STRING_COPY_PITCH_X					_T("Pitch X")
#define STRING_COPY_PITCH_Y					_T("Pitch Y")
#define STRING_EIDT_SHPAE					_T("Shape")
#define STRING_SHAPE_DESC					_T("Set Featuer Shape")
#define STRING_EDIT_ATTRIBUTE				_T("Attribute")
#define STRING_ATTRIBUTE_DESC				_T("Set Feature Attibute")
#define STRING_ATTRIBUTE_NAME				_T(".string")
#define STRING_ATTRIBUTE_STRING_FM_P		_T("fm_p")
#define STRING_ATTRIBUTE_STRING_TOP			_T("t_width,bot,20,30,40")
#define STRING_ATTRIBUTE_STRING_BOL         _T("bol_width,top,28,38,48")
#define STRING_ATTRIBUTE_STRING_BALL		_T("ball_width,bot,220,250,280")
#define STRING_ATTRIBUTE_STRING_ALIGN       _T("align")
#define STRING_EDIT_ATTRIBUTE_STRING        _T("Attribute String")
#define STRING_RADIUS						_T("Radius")
#define STRING_HEIGHT						_T("Height")
#define STRING_WIDTH						_T("Width")
#define STRING_SHAPE						_T("Shape")
#define STRING_SHAPE_ROUND					_T("Round")
#define STRING_SHAPE_SQUARE					_T("Square")
#define STRING_COPY_REPEAT_COUNT_X				_T("Reapeat X Count")
#define STRING_COPY_REPEAT_COUNT_Y				_T("Reapeat Y Count")
#define FEAUTRE_EDIT_LOAD_TIMER					70000
#define FEATURE_EDIT_LOAD_TIMER_DELAY			300

// FeatureEditFormView 폼 보기

class CFeatureEditFormView : public CFormView
{
	friend class CiMasterView;
	DECLARE_DYNCREATE(CFeatureEditFormView)

protected:
	CFeatureEditFormView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFeatureEditFormView();


public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_DLG };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
	
	
public:
	UINT32 SetFeatureSpec(FeatureSpec_t* pFeatureSpec);
	UINT32 SetMoveValue(FeatureSpec_t *pFeatureSpec);
	UINT32 SetMoveValue_Key(FeatureSpec_t* pFeatureSpec);
	UINT32 LoadFeatureData();
	UINT32 LoadMoveData();
	UINT32 LoadMove_KeyData();
	
	CFeatureEdit_DeleteDlg*					m_pDeleteDlg;

	CMFCPropertyGridProperty				*m_pResize[1];
	CMFCPropertyGridProperty				*m_pRotate[1], *m_pMirror[1], *m_pMove[3], *m_pCopy[5], *m_pDelete[1];
	CMFCPropertyGridProperty				*m_pShape[5], *m_pAttr[2];
	CSliderCtrl								m_ctrSlider;
	BOOL									m_bWheelMode = FALSE;
	BOOL									m_bDragMode = FALSE;
	BOOL									m_bPreview = FALSE;
	BOOL									m_bKeyMode = FALSE;


private:
	BOOL									m_bInit;
	CMFCPropertyGridCtrl					m_PropGridCtrl;
	CButton									m_btnPreview;
	CButton									m_btnApply;
	CButton									m_btnDelete;
	double									m_ResizeRatio;
	int										m_nAngleMode = -1;
	BOOL									m_bMirror;
	
	double									m_dRadius;
	double									m_dWidth;
	double									m_dHeight;
	CString									m_strShape;
	double									m_dLineWidth;

	double									m_dMouseMove_X, m_dMouseMove_Y, m_dKeyMove_X, m_dKeyMove_Y, m_dTotal_X, m_dTotal_Y;



	CString									m_strAttributeName;
	CString									m_strAttributeString;

	BOOL									m_bRepeat;
	BOOL									m_bCopy;
	

	
private:
	UINT32	_InitailzeResize();
	UINT32	InitailzeResize();
	UINT32	_InitailzeRotate();
	UINT32  _InitailizeMirror();
	UINT32	InitializeRotate();
	UINT32  InitailizeMove();
	UINT32  InitailizeCopy();
	UINT32  InitailizeShape();
	UINT32  InitailizeAttr();
	UINT32  InitalizeFeatureWindow();
	
	

	UINT32	SetResizeRatio();
	UINT32	SetRotate();
	UINT32  SetMirror();
	UINT32  SetMove();
	UINT32	_SetCopy();
	UINT32  SetPreviewCopy(CPointD &dptCopyXY, CPointD &dptRepeatPitchXY, CPoint &ptRepeatCount);
	UINT32  SetParam();
	UINT32	SetAttribute();

	UINT32  MovePreview();

	UINT32  CopyPreview();


	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	LRESULT OnPropertyChanged(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()

public:
	
	static CFeatureEditFormView* CreateOne(CWnd* pParent);
	afx_msg void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnApplyBtnClick();
	afx_msg void OnPreviewBtnClick();	
	afx_msg void OnDeleteBtnClick();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
};
