// CFeatureEdit_SymbolSelectDlg.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "MainFrm.h"
#include "CFeatureEdit_SymbolSelectDlg.h"
#include "afxdialogex.h"


// CFeatureEdit_SymbolSelectDlg 대화 상자

IMPLEMENT_DYNAMIC(CFeatureEdit_SymbolSelectDlg, CDialogEx)

CFeatureEdit_SymbolSelectDlg::CFeatureEdit_SymbolSelectDlg(IN const float &fResize, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDIT_SYMBOL_SELECT, pParent),
	m_iResizeValue(fResize)
{
	::ZeroMemory(&m_LogFont, sizeof(m_LogFont));
	m_LogFont.lfHeight = -20;
	m_LogFont.lfWeight = FW_BOLD;
	_tcscpy_s(m_LogFont.lfFaceName, _T("MS Sans Serif"));
	m_Font.CreateFontIndirect(&m_LogFont);
}

CFeatureEdit_SymbolSelectDlg::~CFeatureEdit_SymbolSelectDlg()
{
	m_Font.DeleteObject();
}

void CFeatureEdit_SymbolSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFeatureEdit_SymbolSelectDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ALL_FEATURE, &CFeatureEdit_SymbolSelectDlg::OnBnClickedButtonAllFeature)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_ONLY, &CFeatureEdit_SymbolSelectDlg::OnBnClickedButtonSelectOnly)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CFeatureEdit_SymbolSelectDlg::OnBnClickedButtonCancel)
END_MESSAGE_MAP()


// CFeatureEdit_SymbolSelectDlg 메시지 처리기

BOOL CFeatureEdit_SymbolSelectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDC_STATIC)->SetFont(&m_Font);
	GetDlgItem(IDC_STATIC)->SetWindowText(_T("Feature의 Symbol과 연결되어 있는\n모든 Feature를 수정하시겠습니까?"));
	

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CFeatureEdit_SymbolSelectDlg::OnBnClickedButtonAllFeature()
{
	SetResize_Apply(m_iResizeValue);
 
 	SetViewerEditMode(EditMode::enumMode_EditMode);
 
	CDialogEx::OnOK();
}

void CFeatureEdit_SymbolSelectDlg::OnBnClickedButtonSelectOnly()
{
	SetResize_Apply_SelectOnly(m_iResizeValue);

	SetViewerEditMode(EditMode::enumMode_EditMode);

	CDialogEx::OnOK();
}

void CFeatureEdit_SymbolSelectDlg::OnBnClickedButtonCancel()
{
	CDialogEx::OnCancel();
}

BOOL CFeatureEdit_SymbolSelectDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}
