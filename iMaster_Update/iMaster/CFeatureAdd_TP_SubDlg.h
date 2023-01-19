#pragma once

#include <vector>
using namespace std;

typedef struct _SurfaceCross
{
	bool bOsOc;
	CPointD AddPoint;
	CPointD CenterPoint;


	_SurfaceCross()
	{
		_Reset();
	}

	void _Reset()
	{
		bOsOc = true;
		AddPoint = CPointD(0.f, 0.f);
		CenterPoint = CPointD(0.f, 0.f);
	}

}SurfaceCross;






// CFeatureAdd_TP_SubDlg ��ȭ ����

class CFeatureAdd_TP_SubDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFeatureAdd_TP_SubDlg)

public:
	CFeatureAdd_TP_SubDlg(CWnd* pParent = nullptr);   // ǥ�� �������Դϴ�.
	virtual ~CFeatureAdd_TP_SubDlg();

	void SetLink(CFeatureEdit_AddDlg * pParent);

	BOOL IsAllDataInput();
	UINT32 MakeFeature(vector<CFeature*> &vecFeatureData);
	UINT32 SetCross();
	UINT32 SetSetlectPoint(IN const float &fX, IN const float &fY);


// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDIT_ADD_SUB_TP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	DECLARE_MESSAGE_MAP()

private:
	CFeatureEdit_AddDlg				*m_pParent = nullptr;
	vector<CPointD>					m_vecSurfacePoint;
	vector<SurfaceCross>			m_vecPoint;

	int								m_nPointIndex = 0;
	vector<CFeature*>				m_vecFeatureData;
	CPointD							m_CenterPoint;
	
public:
	afx_msg void OnBnClickedButtonAddCross();
	afx_msg void OnBnClickedButtonSelectCrossCenter();
};
