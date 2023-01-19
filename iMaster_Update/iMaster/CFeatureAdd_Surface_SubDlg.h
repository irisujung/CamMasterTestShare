#pragma once

#include <vector>
using namespace std;

typedef struct _FeatureAdd_Surface_Param 
{
	bool bPolytype;
	bool bCCW;
	bool bOSOC;
	double dStartX;
	double dStartY;
	double dEndX;
	double dEndY;
	double dCenterX;
	double dCenterY;
	double dAddX;
	double dAddY;


    //SymbolName eSymbolName;

	//CString strSymbol;

	CString strAttributeName;
	vector<CString> vecAttributeString;

	//vector<double> vecPara;

	_FeatureAdd_Surface_Param()
	{
		Reset();
	}

	void Reset()
	{
		bPolytype = true;
		bCCW = true;
		bOSOC = true;
		dStartX = 0.0;
		dStartY = 0.0;
		dCenterX = 0.0;
		dCenterY = 0.0;
		dEndX = 0.0;
		dEndY = 0.0;
		dAddX = 0.0;
		dAddY = 0.0;


		//strSymbol = _T("");

		strAttributeName = _T("");
		vecAttributeString.clear();

		//vecPara.clear();
	}
} FeatureAdd_SurfaceParam;

typedef struct _SurfacePoint
{
	bool bOsOc;
	bool bCCW;
	bool bPolytype;
	CPointD AddPoint;
	CPointD EndPoint;
	CPointD CenterPoint;
	CPointD StartPoint;
	

	_SurfacePoint()
	{
		_Reset();
	}

	void _Reset()
	{
		bPolytype = true;
		bOsOc = true;
		bCCW = true;
		StartPoint = CPointD(0.f, 0.f);
		AddPoint = CPointD(0.f, 0.f);
		EndPoint = CPointD(0.f, 0.f);
		CenterPoint = CPointD(0.f, 0.f);
	}

}SurfacePoint;



// CFeatureAdd_Surface_SubDlg 대화 상자

class CFeatureEdit_AddDlg;

class CFeatureAdd_Surface_SubDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFeatureAdd_Surface_SubDlg)

	 enum {
		enumSelectPoint_Start = 0,
		enumSelectPoint_End,
		enumSelectPoint_Center,
		enumSelectPoint_Add,
		enumSelectPoint1,
		enumSelectPoint2,
		enumSelectPoint3,
		enumSelectPoint4,
		enumSelectPoint5,

		enumSelectPointMax,
	};

	


public:
	CFeatureAdd_Surface_SubDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFeatureAdd_Surface_SubDlg();

	void SetLink(CFeatureEdit_AddDlg * pParent);

	UINT32 SetSelectPoint(IN const int&nIndex, IN const float &fX, IN const float &fY);
	UINT32 SetSelectPoint_Center(IN const int&nIndex, IN const float &fX, IN const float &fY);
	UINT32 MakeFeature(vector<CFeature*> &vecFeature);

	

	BOOL IsAllDataInput();

	UINT32 SetCW(IN BOOL bcw);

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_ADD_SUB_SURFACE };
#endif

private:
	void _InitValue();

	void _UpdateDataToUI();
	void _UpdateUIToData();

	UINT32 _InitSymbol();


private:

	FeatureAdd_SurfaceParam			m_stAddParam;
		
	CFeatureEdit_AddDlg				*m_pParent = nullptr;

	int								m_nPointIndex = 0;
	int								m_nPoint_Index = 1;
	vector<CPointD>					m_vecSurfacePoint;
	vector<SurfacePoint>			m_vecPoint;
	CPointD							m_CenterPoint;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPolyTypeButtonClicked(UINT32 nID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonSelectAttribute();
	afx_msg void OnBnClickedButtonAddPositon();

};
