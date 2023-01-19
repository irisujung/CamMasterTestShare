#pragma once


// CFeatureEdit_AddDlg 대화 상자

class CFeatureAdd_Pad_SubDlg;
class CFeatureAdd_Line_SubDlg;
class CFeatureAdd_Arc_SubDlg;
class CFeatureAdd_Surface_SubDlg;
class CFeatureAdd_TP_SubDlg;



class CFeatureEdit_AddDlg : public CDialogEx
{
	friend class CMakeSymbolDlg;
	DECLARE_DYNAMIC(CFeatureEdit_AddDlg)

	enum
	{
		enumSubDlg_Line = 0,
		enumSubDlg_Pad,		
		enumSubDlg_Arc,
		enumSubDlg_Surface,
		enumSubDlg_Tp,
		enumSubDlg_Hide,

		enumSubDlgMax,
	};

	enum
	{
		enumSubDlg_StartY = 120,
	};

public:
	CFeatureEdit_AddDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFeatureEdit_AddDlg();

	UINT32 SetSetlectPoint(IN const int &nIndex, IN const float &fX, IN const float &fY);
	UINT32 SetSetlectPoint_Center(IN const int &nIndex, IN const float &fX, IN const float &fY);
	CString GetLayerName();//Combo Box에서 선택된 Layer 이름 가져오기

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_ADD };
#endif

private:
	
	void _InitSubDlg();
	void _ShowSubDlg(int nSubMode);
	
	void _DeleteSubDlg();

	void _SetLayerName();//Combo Box에 선택된 Layer 이름 입력
	
	BOOL _IsAllDataInput();

	void _ClearFeatureData();
	UINT32 _MakeFeature();

private :

	CFeatureAdd_Pad_SubDlg		*m_pPadSubDlg = nullptr;
	CFeatureAdd_Line_SubDlg		*m_pLineSubDlg = nullptr;
	CFeatureAdd_Arc_SubDlg		*m_pArcSubDlg = nullptr;
	CFeatureAdd_Surface_SubDlg  *m_pSurfaceSubDlg = nullptr;
	CFeatureAdd_TP_SubDlg		*m_pTpSubDlg = nullptr;


	int m_nMode;

	vector<CFeature*>          m_vecFeatureData;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnModeButtonClicked(UINT32 nID);
private:
	CComboBox m_cbLayer;
public:
	afx_msg void OnBnClickedButtonSetdata();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
