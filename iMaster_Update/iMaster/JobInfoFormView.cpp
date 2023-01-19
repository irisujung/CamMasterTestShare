// CJobInfoFormView.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "JobInfoFormView.h"
#include "MainFrm.h"
#include "afxdialogex.h"

// CJobInfoFormView 대화 상자

IMPLEMENT_DYNAMIC(CJobInfoFormView, CFormView)

CJobInfoFormView::CJobInfoFormView()
	: CFormView(IDD_JOBINFO_FORM)
{
	m_bInit = FALSE;
	m_strInitDrv = L"";
}

CJobInfoFormView::~CJobInfoFormView()
{	
}

void CJobInfoFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STC_SELECTJOB, m_stcHeadText);
	DDX_Control(pDX, IDC_TREE_FILE, m_ctrFileTree);	
}


BEGIN_MESSAGE_MAP(CJobInfoFormView, CFormView)
	ON_COMMAND(TREE_NODE_CLICK, OnTreeNodeClicked)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CJobInfoFormView 메시지 처리기
void CJobInfoFormView::OnInitialUpdate()
{
	if (m_bInit)
		return;

	CFormView::OnInitialUpdate();

	TCHAR szPath[_MAX_PATH + 1];
	GetModuleFileName(AfxGetApp()->m_hInstance, szPath, _MAX_PATH);

	CString strCurAppDir = szPath;
	int nLen = strCurAppDir.ReverseFind('\\');
	if (nLen > 0)
		strCurAppDir = strCurAppDir.Left(nLen);
	
	//m_stcHeadText.SetPanelImage(strCurAppDir + L"\\Bin\\Img\\ModelName.bmp");
	//m_stcHeadText.SetPanelImage(strCurAppDir + L"\\Bin\\Img\\NodeMouseOver0.bmp");
	//m_stcHeadText.SetTextColor(RGB(255,255,255));
	m_stcHeadText.SetPanelImage(strCurAppDir + L"\\Bin\\Img\\ModelName3.bmp");
	m_stcHeadText.SetTextColor(RGB(0, 0, 0));
	//m_stcHeadText.SetFontWeight(PANEL_FONT_BOLD);
	m_stcHeadText.SetVerticalAlign(DT_TOP);
	m_stcHeadText.SetFontSize(20);
	
	m_stcHeadText.SetWindowTextW(L"Double Click on Work Model");	
	m_bInit = TRUE;
	UpdateDataPath();
}

UINT32 CJobInfoFormView::SetJobPath(CString strJobPath)
{
	if (strJobPath.GetLength() < 1)
		return RESULT_BAD;

	m_strInitDrv = strJobPath;

	return UpdateDataPath();	
}

CJobInfoFormView* CJobInfoFormView::CreateOne(CWnd* pParent)
{
	CJobInfoFormView* pFormView = new CJobInfoFormView;
	CCreateContext* pContext = NULL;

	if (!pFormView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 100, 100), pParent, 0, pContext))
	{
		TRACE0("Failed in creating CJobInfoFormView");
	}

	return pFormView;
}

void CJobInfoFormView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	_UpdateUI_Pos();
}

UINT32		CJobInfoFormView::_UpdateUI_Pos()
{
	if (!m_bInit) return RESULT_BAD;

	CRect rectView, rectTree, rectText, clRectPlayer;

	GetWindowRect(&rectView);
	m_ctrFileTree.GetWindowRect(&rectTree);

	rectText.top = 0;
	rectText.bottom = 45;
	rectText.left = 0;
	rectText.right = rectView.Width();

	m_stcHeadText.MoveWindow(&rectText);
	m_ctrFileTree.MoveWindow(0, 45, rectView.Width(), rectView.Height() - 45);

	Invalidate();

	return RESULT_GOOD;
}

UINT32 CJobInfoFormView::UpdateDataPath(CString strDataPath)
{
	if (!m_bInit)
		return RESULT_BAD;

	m_ctrFileTree.SetRecursiveSub(TRUE);
	//	m_ctrFileTree.SetRecursiveSub(FALSE);

	m_ctrFileTree.PopulateTree();

	if (strDataPath.GetLength() > 0)
		m_strInitDrv = strDataPath;

	m_ctrFileTree.SetRootFolder(m_strInitDrv);

	_UpdateUI_Pos();

	return RESULT_GOOD;
}

void CJobInfoFormView::OnTreeNodeClicked()
{
	LockJobTree(FALSE);

	AddApplicationLog(EXTRACT_CAM_TXT);
	ExtractCamData(m_ctrFileTree.m_sSelectFileName);
}

void CJobInfoFormView::LockJobTree(BOOL bEnable)
{
	m_ctrFileTree.EnableWindow(bEnable);
}

BOOL CJobInfoFormView::SetODBFile(CString csODBFile)
{
	if (csODBFile.GetLength() < 1)
		return FALSE;

	HTREEITEM hItem = NULL;
	CString Tempstr;

	Tempstr.Format(_T("%s\\%s.tgz"), m_ctrFileTree.m_sRootFolder, csODBFile);
	Tempstr.MakeUpper();
	hItem = m_ctrFileTree.PathToItem(Tempstr);

	return m_ctrFileTree.Open(hItem);
}

UINT32	CJobInfoFormView::RefreshList()
{
	return UpdateDataPath();
}