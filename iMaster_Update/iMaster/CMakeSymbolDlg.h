#pragma once


class CFeatureAdd_Pad_SubDlg;
class CFeatureAdd_Line_SubDlg;
class CFeatureAdd_Arc_SubDlg;
class CFeatureAdd_Surface_SubDlg;
class CFeatureEdit_AddDlg;

// CMakeSymbolDlg 대화 상자

enum class eMakeSymbolMode
{//make Mode
	enumMode_Pad = 0,
	enumMode_Line,
	emumMode_Arc,
	enumMode_Surface,
	enumMode_Max,
};


class CMakeSymbolDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMakeSymbolDlg)

	enum 
	{//List column
		enumCol_No = 1,
		enumCol_Name,
		enumCol_Data,

		enumColMax,
	};

	enum
	{
		enumBtnCircle = 0,
		enumBtnSquare,
		enumBtnRectangle,
		enumBtnRounded_Rectangle,
		enumBtnChamfered_Rectangle,

		enumBtnOval,
		enumBtnDiamond,
		enumBtnOctagon,
		enumBtnRound_Donut,
		enumBtnSqaure_Donut,


		enumBtnHorizontal_Hexagon,
		enumtBtnVertical_Hexagon,
		enumBtnButterfly,
		enumBtnSquare_Butterfly,
		enumBtnTriangle,

		enumBtnHalf_Oval,
		enumBtnSquare_Round_Thermal_Rounded,
		enumBtnSquare_Round_Thermal_Squared,
		enumBtnSquare_Thermal,
		eunmBtnSquare_Thermal_Open_Corners,
		
		enumBtnSqaure_Round_Thermal,
		enumBtnRectangluar_Thermal,
		enumBtnRectangular_Thermal_Open_Corners,
		enumBtnEllipse,
		enumBtnMoire,

		enumBtnHole,
		enumBtnMax,
	};

	



public:
	CMakeSymbolDlg(enum eMakeSymbolMode eMode, CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CMakeSymbolDlg();

	CString			GetSymbolName();
	vector<double>	GetSymbolPara();
	int				GetType();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_MAKE_SYMBOL};
#endif

private:
	void _InitSpread();
	void _UpdateSpread(int nType);
	void _UpdatePara();//Spread -> Para Data;
	
	void _InitParaName();

	void _InitUI(enum eMakeSymbolMode eMode);
	
	BOOL _MakeResult();


private:
	CFpspread_dll		m_Spread;

	eMakeSymbolMode		m_eMode = eMakeSymbolMode::enumMode_Pad;

	vector<vector<CString>> m_vecvecParaName;

	int					m_nSelectShape;
	vector<double>		m_vecPara_mm;
	vector<double>		m_vecPara_Mil;
	CString				m_strSymbol;//최종 만들어진 Symbol

	CButton				m_btnSymbol[enumBtnMax];
private:
	CFeatureEdit_AddDlg			*m_pAddDlg = nullptr;
	CFeatureAdd_Pad_SubDlg		*m_pPadSubDlg = nullptr;
	CFeatureAdd_Line_SubDlg		*m_pLineSubDlg = nullptr;
	CFeatureAdd_Arc_SubDlg		*m_pArcSubDlg = nullptr;
	CFeatureAdd_Surface_SubDlg  *m_pSurfaceSubDlg = nullptr;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonCancel();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnModeButtonClicked(UINT32 nID);
	afx_msg void OnBnClickedSymbolPreview();
	afx_msg void OnBnClickedButtonPreview();
	

	
	
	CBitmapButton m_Icon_R;
	CBitmapButton m_Icon_S;
	CBitmapButton m_IconRect;
	CBitmapButton m_Icon__RonudRect;
	CBitmapButton m_IconChamferedRect;
	CBitmapButton m_IconOval;
	CBitmapButton m_IconDiamond;
	CBitmapButton m_IconOct;
	CBitmapButton m_IconDonut__R;
	CBitmapButton m_IconDonut__S;
	CBitmapButton m_IconHex_L;
	CBitmapButton m_Icon_Hex_S;
	CBitmapButton m_Icon_BFR;
	CBitmapButton m_IconBFS;
	CBitmapButton m_Icon_TRI;
	CBitmapButton m_Icon_Oval_H;
	CBitmapButton m_Icon_THR;
	CBitmapButton m_Icon_THS;
	CBitmapButton m_Icon_S_THS;
	CBitmapButton m_Icon_S_THO;
	CBitmapButton m_Icon_SR_THS;
	CBitmapButton m_Icon_RC_THS;
	CBitmapButton m_Icon_RC_THO;
	CBitmapButton m_Icon_EL;
	CBitmapButton m_Icon_Moire;
	CBitmapButton m_Icon_Hole;
};
