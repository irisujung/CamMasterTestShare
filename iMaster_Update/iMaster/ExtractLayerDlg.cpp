// ExtractLayerDlg.cpp : 구현 파일입니다.
//

#include "pch.h"
#include "iMaster.h"
#include "ExtractLayerDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h" 

// CExtractLayerDlg 대화 상자입니다.
IMPLEMENT_DYNAMIC(CExtractLayerDlg, CDialog)

CExtractLayerDlg::CExtractLayerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_EXTRACT_DLG, pParent)
{
	m_strModelName = _T("");
	m_strWorkStep = _T("");
	m_strPatternLayer = _T("");
	m_strDrillLayer = _T("");
	m_strViaLayer = _T("");
	m_strMeasureLayer = _T("");
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

CExtractLayerDlg::~CExtractLayerDlg()
{
	m_TitleFont.DeleteObject();
	m_SubFont.DeleteObject();
}

void CExtractLayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PICTURE, m_stcPix);
	DDX_Control(pDX, IDC_EXTRACT_PROGRESS_BAR, m_progressBar);
}


BEGIN_MESSAGE_MAP(CExtractLayerDlg, CDialog)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CExtractLayerDlg 메시지 처리기입니다.


BOOL CExtractLayerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	bool bIsNSIS = false;
	if (GetMachineType() == eMachineType::eNSIS)
		bIsNSIS = true;
	
	GetDlgItem(IDC_STATIC_TITLE)->SetFont(&m_TitleFont);
	GetDlgItem(IDC_STATIC_MODELNAME)->SetFont(&m_SubFont);
	GetDlgItem(IDC_STATIC_WORKSTEP)->SetFont(&m_SubFont);
	GetDlgItem(IDC_STATIC_PATTERN_LAYER)->SetFont(&m_SubFont);
	GetDlgItem(IDC_STATIC_DRILL_LAYER)->SetFont(&m_SubFont);
	GetDlgItem(IDC_STATIC_VIA_LAYER)->SetFont(&m_SubFont);
	GetDlgItem(IDC_STATIC_MODELNAME_TXT)->SetFont(&m_SubFont);
	GetDlgItem(IDC_STATIC_WORKSTEP_TXT)->SetFont(&m_SubFont);
	GetDlgItem(IDC_STATIC_PATTERN_LAYER_TXT)->SetFont(&m_SubFont);
	GetDlgItem(IDC_STATIC_DRILL_LAYER_TXT)->SetFont(&m_SubFont);
	GetDlgItem(IDC_STATIC_VIA_LAYER_TXT)->SetFont(&m_SubFont);
	GetDlgItem(IDC_STATIC_SUB_TITLE)->SetFont(&m_SubFont);
	GetDlgItem(IDC_STATIC_SUB_TITLE2)->SetFont(&m_SubFont);
	
	// 2022.11.02 KJH ADD
	// hBitmap Member 변수로 변경 및 GDI 객체 제거 위해 코드 수정
	// Origin Code
	/*
	HBITMAP hBitmap = NULL;
	hBitmap = (HBITMAP)LoadImage(NULL, _T("res\\Extraction.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	m_stcPix.SetBitmap(hBitmap);
	if (!hBitmap)::CloseHandle(hBitmap);
	*/

	// New Code 
	if (m_hBitmap != NULL)
	{
		DeleteObject(m_hBitmap);
	}
	m_hBitmap = (HBITMAP)LoadImage(NULL, _T("res\\Extraction.bmp"), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	m_stcPix.SetBitmap(m_hBitmap);
	if (!m_hBitmap)
	{
		::CloseHandle(m_hBitmap);
	}
	 
	GetDlgItem(IDC_STATIC_MODELNAME_TXT)->SetWindowTextW(m_strModelName);
	GetDlgItem(IDC_STATIC_WORKSTEP_TXT)->SetWindowTextW(m_strWorkStep);
	GetDlgItem(IDC_STATIC_PATTERN_LAYER_TXT)->SetWindowTextW(m_strPatternLayer);

	if (bIsNSIS)
	{
		if (GetUserSite() == eUserSite::eSEM)
		{
			GetDlgItem(IDC_STATIC_DRILL_LAYER)->SetWindowTextW(_T("Measure Layer"));
			GetDlgItem(IDC_STATIC_DRILL_LAYER_TXT)->SetWindowTextW(m_strMeasureLayer);
			GetDlgItem(IDC_STATIC_VIA_LAYER)->SetWindowTextW(_T("Thickness Layer"));
			GetDlgItem(IDC_STATIC_VIA_LAYER_TXT)->SetWindowTextW(m_strViaLayer);
		}
		else
		{
			GetDlgItem(IDC_STATIC_DRILL_LAYER)->SetWindowTextW(_T("Measure Layer"));
			GetDlgItem(IDC_STATIC_DRILL_LAYER_TXT)->SetWindowTextW(m_strMeasureLayer);
			GetDlgItem(IDC_STATIC_VIA_LAYER_TXT)->ShowWindow(FALSE);
			GetDlgItem(IDC_STATIC_VIA_LAYER)->ShowWindow(FALSE);
		}
		
	}
	else
	{
		GetDlgItem(IDC_STATIC_DRILL_LAYER)->SetWindowTextW(_T("Drill Layer"));
		GetDlgItem(IDC_STATIC_DRILL_LAYER_TXT)->SetWindowTextW(m_strDrillLayer);
		GetDlgItem(IDC_STATIC_VIA_LAYER_TXT)->SetWindowTextW(m_strViaLayer);
		GetDlgItem(IDC_STATIC_VIA_LAYER_TXT)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_VIA_LAYER)->ShowWindow(TRUE);
	}

	m_progressBar.SetRange(0, 100);
	m_progressBar.SetPos(0);
	m_progressBar.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_SUB_TITLE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_SUB_TITLE2)->ShowWindow(SW_HIDE);

	if (m_bIsPreview)
	{
		GetDlgItem(IDC_STATIC_PATTERN_LAYER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_DRILL_LAYER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_VIA_LAYER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_PATTERN_LAYER_TXT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_DRILL_LAYER_TXT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_VIA_LAYER_TXT)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_STATIC_PATTERN_LAYER)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_DRILL_LAYER)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_PATTERN_LAYER_TXT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_DRILL_LAYER_TXT)->ShowWindow(SW_SHOW);

		if (!bIsNSIS)
		{
			GetDlgItem(IDC_STATIC_VIA_LAYER)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_STATIC_VIA_LAYER_TXT)->ShowWindow(SW_SHOW);
		}
	}

	OnBnClickedOk();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


BOOL CExtractLayerDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;

	GetClientRect(rect);
	pDC->FillSolidRect(rect, RGB(45, 45, 48));
	return TRUE;

//	return CDialogEx::OnEraseBkgnd(pDC);
}


HBRUSH CExtractLayerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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

BOOL CExtractLayerDlg::SetData(CString strModelName, CString strWorkStep, CString strPattenLayer, CString strDrillLayer, CString strViaLayer, BOOL bIsPreview)
{
	m_strModelName = strModelName;
	m_strWorkStep = strWorkStep;
	m_strPatternLayer = strPattenLayer;

	if (GetMachineType() == eMachineType::eNSIS)
	{
		if (GetUserSite() == eUserSite::eSEM)
		{
			m_strMeasureLayer = strDrillLayer;
			m_strViaLayer = strViaLayer;
		}
		else
		{
			m_strMeasureLayer = strDrillLayer;
			m_strViaLayer = _T("");
		}
	}
	else
	{
		m_strDrillLayer = strDrillLayer;
		m_strViaLayer = strViaLayer;
	}

	m_bIsPreview = bIsPreview;
	return TRUE;
}

void CExtractLayerDlg::OnBnClickedOk()
{
	m_progressBar.SetPos(0);
	m_progressBar.ShowWindow(SW_SHOW);

	GetDlgItem(IDC_STATIC_SUB_TITLE)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_SUB_TITLE2)->ShowWindow(SW_HIDE);
 
	m_bProcessing = TRUE;	
	
	LONG_PTR style = GetWindowLongPtr(GetDlgItem(IDC_EXTRACT_PROGRESS_BAR)->GetSafeHwnd(), GWL_STYLE);
	style |= PBS_MARQUEE;
	SetWindowLongPtr(GetDlgItem(IDC_EXTRACT_PROGRESS_BAR)->GetSafeHwnd(), GWL_STYLE, style);
	SendDlgItemMessage(IDC_EXTRACT_PROGRESS_BAR, PBM_SETMARQUEE, TRUE, PROGRESS_SPEED);	
	//	CDialogEx::OnOK();
}

void CExtractLayerDlg::EndExtract()
{
	if (!m_bProcessing) return;

	GetDlgItem(IDC_STATIC_SUB_TITLE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_SUB_TITLE2)->ShowWindow(SW_SHOW);
 
	m_bProcessing = FALSE;

	SendDlgItemMessage(IDC_EXTRACT_PROGRESS_BAR, PBM_SETMARQUEE, FALSE, PROGRESS_SPEED);

	LONG_PTR style = GetWindowLongPtr(GetDlgItem(IDC_EXTRACT_PROGRESS_BAR)->GetSafeHwnd(), GWL_STYLE);
	style &= ~PBS_MARQUEE;
	SetWindowLongPtr(GetDlgItem(IDC_EXTRACT_PROGRESS_BAR)->GetSafeHwnd(), GWL_STYLE, style);
	m_progressBar.SetPos(100);
	
	SetTimer(EXTRACT_CLOSE_TIMER, EXTRACT_CLOSE_TIMER_DELAY, NULL);	
}

void CExtractLayerDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}

BOOL CExtractLayerDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) return TRUE;
	return CDialog::PreTranslateMessage(pMsg);
}


void CExtractLayerDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case EXTRACT_CLOSE_TIMER:
		KillTimer(EXTRACT_CLOSE_TIMER);

		EndDialog(0);
	}

	CDialog::OnTimer(nIDEvent);
}
