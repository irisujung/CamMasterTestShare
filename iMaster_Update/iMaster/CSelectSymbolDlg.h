#pragma once


// CSelectSymbolDlg 대화 상자

class CSelectSymbolDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSelectSymbolDlg)

	enum
	{//List column
		enumCol_No = 1,
		enumCol_Data,

		enumColMax,
	};

public:
	CSelectSymbolDlg(IN const BOOL &bPadMode, IN const CString &strLayerName, CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CSelectSymbolDlg();

	CString			GetSymbolName();
	vector<double>	GetSymbolPara();
	int				GetType();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_SELECT_SYMBOL };
#endif

private:
	void _InitSpread();
	void _UpdateSpread(BOOL bPadMode);

	UINT32 _MakeResult();

	CString _GetSelectSymbolName();

	vector<double> _GetSymbolPara(IN const CString strSymbolName);

private:

	BOOL				m_bPadMode = FALSE;
	CFpspread_dll		m_Spread;

	int					m_nSelectStep;
	CString				m_strSelectLayerName;

	//Out Data
	int					m_nSelectShape;
	vector<double>		m_vecPara_mm;
	CString				m_strSymbol;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonCancel();
};
