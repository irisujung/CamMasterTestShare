#pragma once

#include "afxwin.h"
#include "afxcmn.h"
// CExtractLayerDlg 대화 상자입니다.

#define PROGRESS_SPEED				20
#define EXTRACT_CLOSE_TIMER			50004
#define EXTRACT_CLOSE_TIMER_DELAY	2000

class CExtractLayerDlg : public CDialog
{
	DECLARE_DYNAMIC(CExtractLayerDlg)

public:
	CExtractLayerDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CExtractLayerDlg();

public:
	BOOL SetData(CString strModelName, CString strWorkStep, CString strPattenLayer, CString strDrillLayer, CString strViaLayer=L"", BOOL bIsPreview = FALSE);
	void EndExtract();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXTRACT_DLG };
#endif

private:
	CFont		m_TitleFont, m_SubFont;
	LOGFONT		LogTitleFont, LogSubFont;

	BOOL		m_bProcessing;
	BOOL		m_bIsPreview;
	CString		m_strModelName;
	CString		m_strWorkStep;
	CString		m_strPatternLayer;
	CString		m_strDrillLayer;
	CString		m_strViaLayer;
	CString		m_strMeasureLayer;
	CString		m_strThicknessLayer;

public:
	CStatic			m_stcPix;
	CProgressCtrl	m_progressBar;
	HBITMAP			m_hBitmap;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
