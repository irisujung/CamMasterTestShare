#pragma once

#include <vector>
using namespace std;

typedef struct _FeatureAdd_Line_Param
{
	BOOL bPolariry;
	double dStartX;
	double dStartY;
	double dEndX;
	double dEndY;

	SymbolName	eSymbolName;

	CString strSymbol;//Symbol Text

	CString strAttributeName;
	vector<CString> vecAttributeString;


	vector<double> vecPara;

	_FeatureAdd_Line_Param()
	{
		Reset();
	}

	void Reset()
	{
		bPolariry = TRUE;
		dStartX = 0.0;
		dStartY = 0.0;
		dEndX = 0.0;
		dEndY = 0.0;

		strSymbol = _T("");

		strAttributeName = _T("");
		vecAttributeString.clear();

		vecPara.clear();
	}

}FeatureAdd_LineParam;

// CFeatureAdd_Line_SubDlg 대화 상자

class CFeatureEdit_AddDlg;

class CFeatureAdd_Line_SubDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFeatureAdd_Line_SubDlg)

	enum {
		enumSelectPoint_Start = 0, 
		enumSelectPoint_End,

		enumSelectPointMax,
	};

public:
	CFeatureAdd_Line_SubDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFeatureAdd_Line_SubDlg();

	void	SetLink(CFeatureEdit_AddDlg *pParent);

	UINT32	SetSetlectPoint(IN const int&nIndex, IN const float &fX, IN const float &fY);
	UINT32	MakeFeature(vector<CFeature*> &vecFeatureData);

	BOOL	IsAllDataInput();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_ADD_SUB_LINE };
#endif

private:
	void	_InitValue();

	void	_UpdateDataToUI();
	void	_UpdateUIToData();

	UINT32  _InitSymbol();
	UINT32 _ManualMove(int nIndex);

private:

	FeatureAdd_LineParam		m_stAddParam;

	//외부 Prt
	//지우지 말것.
	CFeatureEdit_AddDlg			*m_pParent = nullptr;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSelectStartPos();
	afx_msg void OnBnClickedButtonSelectEndPos();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonSelectAttribute();
	afx_msg void OnPolarityButtonClicked(UINT32 nID);
	afx_msg void OnBnClickedButtonSelectSymbol();
	afx_msg void OnBnClickedButtonMakeSymbol();
	afx_msg void OnEnChangeEditStartPosX();
	afx_msg void OnEnChangeEditStartPosY();
	afx_msg void OnEnChangeEditEndPosX();
	afx_msg void OnEnChangeEditEndPosY();
};
