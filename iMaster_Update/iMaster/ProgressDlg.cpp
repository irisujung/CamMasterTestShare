// ProgressDlg.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "ProgressDlg.h"
#include "afxdialogex.h"
#include "resource.h"

// ProgressDlg 대화 상자

IMPLEMENT_DYNAMIC(CProgressDlg, CDialogEx)

CProgressDlg::CProgressDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROGRESS_DLG, pParent)
{
	m_strText = L"";
}

CProgressDlg::~CProgressDlg()
{
	m_RoundProgress.StopInfiniteProgress();
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STC_ROUNDPROGRESS, m_RoundProgress);
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialogEx)
END_MESSAGE_MAP()


// ProgressDlg 메시지 처리기
BOOL CProgressDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetBackgroundColor(RGB(1, 11, 21));
	LONG ExtendedStyle = GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE);
	SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, ExtendedStyle | WS_EX_LAYERED);
	::SetLayeredWindowAttributes(GetSafeHwnd(), RGB(1, 11, 21), 0, LWA_COLORKEY);

	m_RoundProgress.SetInfiniteProgressInterval(50);
	m_RoundProgress.SetInfiniteProgressArcOffsetIncrement(10);
	m_RoundProgress.SetInfiniteProgress(true);

	m_RoundProgress.SetCircleThickness(10);
	m_RoundProgress.SetProgCircleColor(RGB(107, 125, 189));

	m_RoundProgress.SetFontSize(18);
	m_RoundProgress.SetFontColor(RGB(255, 255, 255));
	m_RoundProgress.SetDisplayText(m_strText);

	m_RoundProgress.StartInfiniteProgress();
	return TRUE;
}

void CProgressDlg::OnDestroy()
{
	m_RoundProgress.StopInfiniteProgress();
	Sleep(1000);
	CDialogEx::OnDestroy();
}

void CProgressDlg::SetText(CString str)
{
	m_strText = str; 
}