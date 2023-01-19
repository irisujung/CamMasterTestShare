#pragma once


// CMakeAttributeDlg 대화 상자

class CMakeAttributeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMakeAttributeDlg)

	enum
	{
		enumSpread_Name = 0,
		enumSpread_String,

		enumSpread_Max,
	};

	enum
	{//List column
		enumCol_No = 1,
		enumCol_Data,

		enumColMax,
	};

public:
	CMakeAttributeDlg(IN const CString &strLayerName, CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CMakeAttributeDlg();

	CString GetAttributeName();
	CString GetAttributeString();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_SELECT_ATTRIBUTE };
#endif

private:
	void _InitUI();
	void _InitSpread();
	void _UpdateSpread_Name();
	void _UpdateSpread_String();

	UINT32 _SetAttributeName();
	UINT32 _SetAttributeString();

private:
	CFpspread_dll		m_pSpread[enumSpread_Max];

	int					m_nSelectStep;
	CString				m_strSelectLayerName;

	CString				m_strAttributeName = _T("");
	CString				m_strAttributeString = _T("");

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonCancel();
};
