#pragma once


// CFeatureEdit_FeatureDlg ��ȭ ����

class CFeatureEdit_FeatureDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFeatureEdit_FeatureDlg)

public:
	CFeatureEdit_FeatureDlg(CWnd* pParent = nullptr);   // ǥ�� �������Դϴ�.
	virtual ~CFeatureEdit_FeatureDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EIDT_FEATURE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};
