#pragma once


// CFeatureEdit_DeleteDlg 대화 상자

class CFeatureEdit_DeleteDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFeatureEdit_DeleteDlg)

public:
	CFeatureEdit_DeleteDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFeatureEdit_DeleteDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_DELETE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
