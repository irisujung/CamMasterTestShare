// ExtractLayerDlg.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "iMaster.h"
#include "MakeMasterDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h" 

// CExtractLayerDlg 대화 상자입니다.
IMPLEMENT_DYNAMIC(CMakeMaterDlg, CDialog)

CMakeMaterDlg::CMakeMaterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_PROCESS_DLG, pParent)
{
	m_strModelName = _T("");
	m_strWorkStep = _T("");
	m_strWorkLayer = _T("");
	m_strResolution = _T("");
	m_bProcessing = FALSE;

	::ZeroMemory(&LogTitleFont, sizeof(LogTitleFont));
	LogTitleFont.lfHeight = -32;
	LogTitleFont.lfWeight = FW_BOLD;
	_tcscpy_s(LogTitleFont.lfFaceName, _T("MS Sans Serif"));
	m_TitleFont.CreateFontIndirect(&LogTitleFont);

	::ZeroMemory(&LogSubFont, sizeof(LogSubFont));
	LogSubFont.lfHeight = -24;
	LogSubFont.lfWeight = FW_NORMAL;
	_tcscpy_s(LogSubFont.lfFaceName, _T("MS Sans Serif"));
	m_SubFont.CreateFontIndirect(&LogSubFont);
}

CMakeMaterDlg::~CMakeMaterDlg()
{
	m_TitleFont.DeleteObject();
	m_SubFont.DeleteObject();
}

void CMakeMaterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PICTURE, m_stcPix);
	DDX_Control(pDX, IDC_MAKEMASTER_PROGRESS_BAR, m_progressBar);
}


BEGIN_MESSAGE_MAP(CMakeMaterDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CExtractLayerDlg 메시지 처리기입니다.


BOOL CMakeMaterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	GetDlgItem(IDC_STATIC_TITLE)->SetFont(&m_TitleFont);
	GetDlgItem(IDC_STATIC_MODELNAME)->SetFont(&m_SubFont);
	GetDlgItem(IDC_STATIC_WORKSTEP)->SetFont(&m_SubFont);
	GetDlgItem(IDC_STATIC_WORKLAYER)->SetFont(&m_SubFont);
	GetDlgItem(IDC_STATIC_RESOLUTION)->SetFont(&m_SubFont);
	GetDlgItem(IDC_STATIC_MODELNAME_TXT)->SetFont(&m_SubFont);
	GetDlgItem(IDC_STATIC_WORKSTEP_TXT)->SetFont(&m_SubFont);
	GetDlgItem(IDC_STATIC_WORKLAYER_TXT)->SetFont(&m_SubFont);
	GetDlgItem(IDC_STATIC_RESOLUTION_TXT)->SetFont(&m_SubFont);
	GetDlgItem(IDC_STATIC_SUB_TITLE)->SetFont(&m_SubFont);
	GetDlgItem(IDC_STATIC_SUB_TITLE2)->SetFont(&m_SubFont);
	
	// 2022.11.02 KJH ADD
	// hBitmap Member 변수로 변경 및 GDI 객체 제거 위해 코드 수정
	// Origin Code
	/*
	HBITMAP hBitmap = NULL;
	hBitmap = (HBITMAP)LoadImage(NULL, _T("res\\ProcessMaster.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	m_stcPix.SetBitmap(hBitmap);
	if (!hBitmap)::CloseHandle(hBitmap);
	*/

	// New Code 
	if (m_hBitmap != NULL)
	{
		DeleteObject(m_hBitmap);
	}
	m_hBitmap = (HBITMAP)LoadImage(NULL, _T("res\\ProcessMaster.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	m_stcPix.SetBitmap(m_hBitmap);
	if (!m_hBitmap)
	{
		::CloseHandle(m_hBitmap);
	}
 
	GetDlgItem(IDC_STATIC_MODELNAME_TXT)->SetWindowTextW(m_strModelName);
	GetDlgItem(IDC_STATIC_WORKSTEP_TXT)->SetWindowTextW(m_strWorkStep);
	GetDlgItem(IDC_STATIC_WORKLAYER_TXT)->SetWindowTextW(m_strWorkLayer);

	if (GetMachineType() == eMachineType::eNSIS)
		GetDlgItem(IDC_STATIC_RESOLUTION)->SetWindowTextW(L"Work Side");
	else
		GetDlgItem(IDC_STATIC_RESOLUTION)->SetWindowTextW(L"Resolution");
	GetDlgItem(IDC_STATIC_RESOLUTION_TXT)->SetWindowTextW(m_strResolution);
	
	m_progressBar.SetRange(0, 100);
	m_progressBar.SetPos(0);
	m_progressBar.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_SUB_TITLE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_SUB_TITLE2)->ShowWindow(SW_HIDE);

	OnBnClickedOk();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CMakeMaterDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;

	GetClientRect(rect);
	pDC->FillSolidRect(rect, RGB(45, 45, 48));
	return TRUE;

//	return CDialogEx::OnEraseBkgnd(pDC);
}


HBRUSH CMakeMaterDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	switch (nCtlColor)
	{
		case CTLCOLOR_STATIC:
		{
			pDC->SetTextColor(RGB(255, 255, 255));
			pDC->SetBkMode(TRANSPARENT);
			return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
	}
	return hbr;
}

BOOL CMakeMaterDlg::SetData(CString strModelName, CString strWorkStep, CString strWorkLayer, CString strResolution)
{
	m_strModelName = strModelName;
	m_strWorkStep = strWorkStep;
	m_strWorkLayer = strWorkLayer;
	m_strResolution = strResolution;

	return TRUE;
}

void CMakeMaterDlg::OnBnClickedOk()
{
	m_progressBar.SetPos(0);
	m_progressBar.ShowWindow(SW_SHOW);

	GetDlgItem(IDC_STATIC_SUB_TITLE)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_SUB_TITLE2)->ShowWindow(SW_HIDE);
 
	m_bProcessing = TRUE;	
	
	LONG_PTR style = GetWindowLongPtr(GetDlgItem(IDC_MAKEMASTER_PROGRESS_BAR)->GetSafeHwnd(), GWL_STYLE);
	style |= PBS_MARQUEE;
	SetWindowLongPtr(GetDlgItem(IDC_MAKEMASTER_PROGRESS_BAR)->GetSafeHwnd(), GWL_STYLE, style);
	SendDlgItemMessage(IDC_MAKEMASTER_PROGRESS_BAR, PBM_SETMARQUEE, TRUE, PROGRESS_SPEED);
	//	CDialogEx::OnOK();
}

void CMakeMaterDlg::EndMakeMaster()
{
	if (!m_bProcessing) return;

	GetDlgItem(IDC_STATIC_SUB_TITLE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_SUB_TITLE2)->ShowWindow(SW_SHOW);
 
	m_bProcessing = FALSE;

	SendDlgItemMessage(IDC_MAKEMASTER_PROGRESS_BAR, PBM_SETMARQUEE, FALSE, PROGRESS_SPEED);

	LONG_PTR style = GetWindowLongPtr(GetDlgItem(IDC_MAKEMASTER_PROGRESS_BAR)->GetSafeHwnd(), GWL_STYLE);
	style &= ~PBS_MARQUEE;
	SetWindowLongPtr(GetDlgItem(IDC_MAKEMASTER_PROGRESS_BAR)->GetSafeHwnd(), GWL_STYLE, style);
	m_progressBar.SetPos(100);
	
	SetTimer(MAKEMASTER_CLOSE_TIMER, MAKEMASTER_CLOSE_TIMER_DELAY, NULL);
}

void CMakeMaterDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}

BOOL CMakeMaterDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}


void CMakeMaterDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case MAKEMASTER_CLOSE_TIMER:
		KillTimer(MAKEMASTER_CLOSE_TIMER);

		EndDialog(0);
	}

	CDialog::OnTimer(nIDEvent);
}
