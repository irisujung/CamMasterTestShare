#pragma once

#include "afxwin.h"
#include "afxcmn.h"
// CExtractLayerDlg ��ȭ �����Դϴ�.

#define PROGRESS_SPEED					20
#define MAKEMASTER_CLOSE_TIMER			50006
#define MAKEMASTER_CLOSE_TIMER_DELAY	2000

class CMakeMaterDlg : public CDialog
{
	DECLARE_DYNAMIC(CMakeMaterDlg)

public:
	CMakeMaterDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CMakeMaterDlg();

public:
	BOOL SetData(CString strModelName, CString strWorkStep, CString strWorkLayer, CString strResolution);
	void EndMakeMaster();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROCESS_DLG };
#endif

private:
	CFont		m_TitleFont, m_SubFont;
	LOGFONT		LogTitleFont, LogSubFont;

	BOOL		m_bProcessing;
	CString		m_strModelName;
	CString		m_strWorkStep;
	CString		m_strWorkLayer;
	CString		m_strResolution;

public:
	CStatic			m_stcPix;
	CProgressCtrl	m_progressBar;
	HBITMAP			m_hBitmap;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
