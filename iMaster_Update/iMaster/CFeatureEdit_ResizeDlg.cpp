// CFeatureEdit_ResizeDlg.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "MainFrm.h"
#include "CFeatureEdit_ResizeDlg.h"
#include "afxdialogex.h"

#include "CFeatureEdit_SymbolSelectDlg.h"


// CFeatureEdit_ResizeDlg 대화 상자

IMPLEMENT_DYNAMIC(CFeatureEdit_ResizeDlg, CDialogEx)

CFeatureEdit_ResizeDlg::CFeatureEdit_ResizeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDIT_RESIZE, pParent)
{

}

CFeatureEdit_ResizeDlg::~CFeatureEdit_ResizeDlg()
{
}

void CFeatureEdit_ResizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFeatureEdit_ResizeDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CFeatureEdit_ResizeDlg::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CFeatureEdit_ResizeDlg::OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_PREVIEW, &CFeatureEdit_ResizeDlg::OnBnClickedButtonPreview)
END_MESSAGE_MAP()


// CFeatureEdit_ResizeDlg 메시지 처리기


BOOL CFeatureEdit_ResizeDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CFeatureEdit_ResizeDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow == TRUE)
	{	
		GetDlgItem(IDC_EDIT_FACTOR)->SetWindowText(_T("100.0"));
		GetDlgItem(IDC_STATIC_RESIZE_UNIT)->SetWindowText(_T("%"));
	}
	else
	{
		SetViewerEditMode(EditMode::enumMode_EditMode);//AddMode 에서 일반 Edit Mode로 변경
		AddApplicationLog(END_EDIT_RESIZE_TXT);
	}
}

float  CFeatureEdit_ResizeDlg::_GetResizeValue()
{
	CString strTemp;
	float fResize;
	GetDlgItem(IDC_EDIT_FACTOR)->GetWindowText(strTemp);
	fResize = static_cast<float>(_ttof(strTemp));

	return fResize;
}

void CFeatureEdit_ResizeDlg::OnBnClickedButtonPreview()
{
	float fResize = _GetResizeValue();

	int nResize = static_cast<int>(fResize);

	if (nResize <= 0)
	{
		AfxMessageBox(_T("입력 값이 올바르지 않습니다."));
		GetDlgItem(IDC_EDIT_FACTOR)->SetWindowText(_T("100.0"));
		return;
	}

	SetResize(nResize);	
}

void CFeatureEdit_ResizeDlg::OnBnClickedButtonApply()
{
	float fResize = _GetResizeValue();

	if (fResize <= 0.f)
	{
		AfxMessageBox(_T("입력 값이 올바르지 않습니다."));
		GetDlgItem(IDC_EDIT_FACTOR)->SetWindowText(_T("100.0"));
		return;
	}

	//Symbol Select 기능은 보이지 않고, Highlight된 Feature의 Symbol만 수정한다.
// 	CFeatureEdit_SymbolSelectDlg dlgSelect(fResize);
// 
// 	if (dlgSelect.DoModal() == IDOK)
// 	{
// 		ShowWindow(SW_HIDE);
// 	}

	//선택된 Feature만 변경
	SetResize_Apply_SelectOnly(fResize);

	CString strLog;
	strLog.Format(_T("%s [Scale : %.1f%%]"), APPLY_EDIT_RESIZE_TXT, fResize);
	AddApplicationLog(strLog);
	Log_FeatureSelect();

	SetViewerEditMode(EditMode::enumMode_EditMode);

	ShowWindow(SW_HIDE);
}


void CFeatureEdit_ResizeDlg::OnBnClickedButtonCancel()
{
	SetViewerEditMode(EditMode::enumMode_EditMode);

	ShowWindow(SW_HIDE);
}
