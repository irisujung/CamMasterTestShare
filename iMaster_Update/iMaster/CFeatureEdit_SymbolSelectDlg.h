#pragma once


// CFeatureEdit_SymbolSelectDlg 대화 상자

class CFeatureEdit_SymbolSelectDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFeatureEdit_SymbolSelectDlg)

public:
	CFeatureEdit_SymbolSelectDlg(IN const float &fResize, CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFeatureEdit_SymbolSelectDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_SYMBOL_SELECT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.


private:
	CFont		m_Font;
	LOGFONT		m_LogFont;

	float		m_iResizeValue = 0.f;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAllFeature();
	afx_msg void OnBnClickedButtonSelectOnly();
	afx_msg void OnBnClickedButtonCancel();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
