#pragma once
#include "Lib/RoundProgress.h"

// ProgressDlg 대화 상자

class CProgressDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CProgressDlg();

	void SetText(CString str);

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROGRESS_DLG };
#endif

private:
	CRoundProgress	m_RoundProgress;
	CString			m_strText;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
