#pragma once


// CFeatureEdit_FeatureDlg 대화 상자

class CFeatureEdit_FeatureDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFeatureEdit_FeatureDlg)

public:
	CFeatureEdit_FeatureDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CFeatureEdit_FeatureDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EIDT_FEATURE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
};
