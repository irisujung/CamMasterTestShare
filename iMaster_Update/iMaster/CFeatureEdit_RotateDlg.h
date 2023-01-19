#pragma once


// CFeatureEdit_RotateDlg ��ȭ ����

class CFeatureEdit_RotateDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFeatureEdit_RotateDlg)

public:
	CFeatureEdit_RotateDlg(CWnd* pParent = nullptr);   // ǥ�� �������Դϴ�.
	virtual ~CFeatureEdit_RotateDlg();

	
	
	
// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_ROTATE };
#endif

private:
	

private:
	void	_UpdateDataToUI();



public:
	int m_nAngleMode;
	bool m_bMirror;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnAngleButtonClicked(UINT32 nID);
	afx_msg void OnMirrorButtonClicked(UINT32 nID);
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonPreview();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedCancel();
};
