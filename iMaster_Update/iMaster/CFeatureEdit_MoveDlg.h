#pragma once


// CFeatureEdit_MoveDlg 대화 상자

class CFeatureEdit_MoveDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFeatureEdit_MoveDlg)

public:
	CFeatureEdit_MoveDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFeatureEdit_MoveDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_MOVE };
#endif

	PointDXY _GetMoveValue();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedRadioMoveX();
	afx_msg void OnBnClickedRadioMoveY();
	afx_msg void OnBnClickedRadioMoveXy();
	afx_msg void OnBnClickedButtonPreview();
};
