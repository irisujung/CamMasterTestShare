#pragma once


// CFeatureAdd_Pad_SubDlg 대화 상자
#include <vector>
using namespace std;

typedef struct _FeatureAdd_Pad_Param
{
	BOOL bPolariry;
	double dCenterX;
	double dCenterY;

	SymbolName	eSymbolName;

	CString strSymbol;//Symbol Text

	CString strAttributeName;
	vector<CString> vecAttributeString;

	int nAngleMode;//0, 90, 180, 270
	BOOL bMirror;

	vector<double> vecPara;

	_FeatureAdd_Pad_Param()
	{
		Reset();
	}

	void Reset()
	{
		bPolariry = TRUE;
		dCenterX = 0.0;
		dCenterY = 0.0;

		strSymbol = _T("");

		strAttributeName = _T("");
		vecAttributeString.clear();

		nAngleMode = 0;
		bMirror = FALSE;

		vecPara.clear();
	}

}FeatureAdd_PadParam;

class CFeatureEdit_AddDlg;

class CFeatureAdd_Pad_SubDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFeatureAdd_Pad_SubDlg)

public:
	CFeatureAdd_Pad_SubDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFeatureAdd_Pad_SubDlg();

	void	SetLink(CFeatureEdit_AddDlg *pParent);

	UINT32	SetSetlectPoint(IN const float &fX, IN const float &fY);
	UINT32	MakeFeature(vector<CFeature*> &vecFeatureData);

	BOOL	IsAllDataInput();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_ADD_SUB_PAD };
#endif

private:
	void	_InitValue();

	void	_UpdateDataToUI();
	void	_UpdateUIToData();

	UINT32  _InitSymbol();
	UINT32  _ManualMove();

private:

	FeatureAdd_PadParam		m_stAddParam;

	//외부 Prt
	//지우지 말것.
	CFeatureEdit_AddDlg			*m_pParent = nullptr;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonSelectPos();
	afx_msg void OnBnClickedButtonSelectAttribute();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPolarityButtonClicked(UINT32 nID);
	afx_msg void OnAngleButtonClicked(UINT32 nID);
	afx_msg void OnMirrorButtonClicked(UINT32 nID);
	afx_msg void OnBnClickedButtonSelectSymbol();
	afx_msg void OnBnClickedButtonMakeSymbol();
	afx_msg void OnEnChangeEditCenterPosX();
	afx_msg void OnEnChangeEditCenterPosY();
};
