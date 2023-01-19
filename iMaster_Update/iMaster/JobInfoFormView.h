#pragma once

#include "Lib/StaticPanel.h"
#include "Lib/FileTreeCtrl.h"

// CJobInfoFormView 대화 상자

class CJobInfoFormView : public CFormView
{
	DECLARE_DYNAMIC(CJobInfoFormView)

public:
	CJobInfoFormView();   // 표준 생성자입니다.
	virtual ~CJobInfoFormView();
	virtual void OnInitialUpdate();
	void	OnTreeNodeClicked();
	void	LockJobTree(BOOL bEnable);
	BOOL	SetODBFile(CString csODBFile);

	UINT32	RefreshList();

private:
	UINT32		UpdateDataPath(CString strDataPath=L"");	
	UINT32		_UpdateUI_Pos();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JOBINFO_FORM };
#endif

private:
	CStaticPanel	m_stcHeadText;
	CFileTreeCtrl	m_ctrFileTree;

	BOOL			m_bInit = FALSE;
	CString			m_strInitDrv;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	static CJobInfoFormView* CreateOne(CWnd* pParent);
	UINT32 SetJobPath(CString strJobPath);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
