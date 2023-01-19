#pragma once


// CFeatureEdit_CopyDlg 대화 상자

class CFeatureEdit_CopyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFeatureEdit_CopyDlg)

	enum
	{
		enumModeCopy_Once = 0,
		enumModeCopy_Repeat,

		enumModeMax,
	};

public:
	CFeatureEdit_CopyDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFeatureEdit_CopyDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_COPY };
#endif

private:
	void	_InitValue();
	void	_UpdateUI();//Data to IU
	void	_UpdateMode(int nMode);

	void	_UpdateData();//UI To Data

	BOOL	_IsValid();

private :
	int			m_nMode = enumModeCopy_Once;
	CPointD		m_dptCopyXY			= CPointD(0., 0.);
	CPointD		m_dptRepeatPitchXY	= CPointD(0., 0.);
	CPoint		m_ptRepeatCount		= CPoint(0, 0);



protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonPreview();
	afx_msg void OnBnClickedButtonCancel();
	virtual BOOL OnInitDialog();
};
