#pragma once


// CSaveAsDlg 대화 상자

class CSaveAsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSaveAsDlg)

public:
	CSaveAsDlg(CString strJobName, CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CSaveAsDlg();

	CString GetJobName();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SAVE_AS };
#endif

private :
	CString		m_strJobName = _T("");

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonApply();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
