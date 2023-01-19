#pragma once

#include <vector>
using namespace std;

typedef struct _Feature_Arc_Param 
{
	BOOL bCw;
	BOOL bPolarity;
	double dStartX;
	double dStartY;
	double dCenterX;
	double dCenterY;
	double dEndX;
	double dEndY;

	SymbolName eSymbolName;
	
	CString strSymbol;

	CString strAttributeName;
	vector<CString> vecAttributeString;

	vector<double> vecPara;

	_Feature_Arc_Param()
	{
		Reset();
	}

	void Reset()
	{
		bCw = TRUE;
		bPolarity = TRUE;
		dStartX = 0.0;
		dStartY = 0.0;
		dCenterX = 0.0;
		dCenterY = 0.0;
		dEndX = 0.0;
		dEndY = 0.0;

		strSymbol = _T("");

		strAttributeName = _T("");
		vecAttributeString.clear();

		vecPara.clear();
	}
}FeatureAdd_ArcParam;

// CFeatureAdd_Arc_SubDlg 대화 상자

class CFeatureEdit_AddDlg;

class CFeatureAdd_Arc_SubDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFeatureAdd_Arc_SubDlg)

	enum {
		enumSelectPoint_Start = 0,
		enumSelectPoint_Center,
		enumSelectPoint_End,

		enumSelectPointMax,
	};


public:
	CFeatureAdd_Arc_SubDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFeatureAdd_Arc_SubDlg();

	void SetLink(CFeatureEdit_AddDlg *pParent);

	UINT SetSelectPoint(IN const int&nIndex, IN const float &fX, IN const float &fY);
	UINT MakeFeature(vector<CFeature*> &vecFeatureData);

	BOOL IsAllDataInput();


// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_ADD_SUB_ARC };
#endif

private:
	void _InitValue();

	void _UpdateDataToUI();
	void _UpdateUItoData();

	UINT _IntiSymbol();
	UINT _ManualMove(int nIndex);

private:
	FeatureAdd_ArcParam     m_stAddParam;

	CFeatureEdit_AddDlg		*m_pParent = nullptr;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSelectStartPos();
	afx_msg void OnBnClickedButtonSelectEndPos();
	afx_msg void OnBnClickedButtonSelectCenterPos();
	afx_msg void OnBnClickedButtonSelectAttribute();
	afx_msg void OnPolarityButtonClicked(UINT32 nID);
	afx_msg void OnBnClickedButtonMakeSymbol();
	afx_msg void OnEnChangeEditStartPosX();
	afx_msg void OnEnChangeEditStartPosY();
	afx_msg void OnEnChangeEditEndPosX();
	afx_msg void OnEnChangeEditEndPosY();
	afx_msg void OnEnChangeEditCenterPosX();
	afx_msg void OnEnChangeEditCenterPosY();
	afx_msg void OnDirectionButtonClicked(UINT32 nID);
	afx_msg void OnBnClickedButtonSelectSymbol();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
