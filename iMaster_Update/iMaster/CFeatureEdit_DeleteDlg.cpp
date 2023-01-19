// CFeatureEdit_DeleteDlg.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "MainFrm.h"
#include "CFeatureEdit_DeleteDlg.h"
#include "afxdialogex.h"


// CFeatureEdit_DeleteDlg 대화 상자

IMPLEMENT_DYNAMIC(CFeatureEdit_DeleteDlg, CDialogEx)

CFeatureEdit_DeleteDlg::CFeatureEdit_DeleteDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDIT_DELETE, pParent)
{

}

CFeatureEdit_DeleteDlg::~CFeatureEdit_DeleteDlg()
{
}

void CFeatureEdit_DeleteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFeatureEdit_DeleteDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CFeatureEdit_DeleteDlg::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CFeatureEdit_DeleteDlg::OnBnClickedButtonCancel)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CFeatureEdit_DeleteDlg 메시지 처리기


void CFeatureEdit_DeleteDlg::OnBnClickedButtonApply()
{
	SetDelete_Apply();

	AddApplicationLog(APPLY_EDIT_DELETE_TXT);
	Log_FeatureSelect();

	ShowWindow(SW_HIDE);
}


void CFeatureEdit_DeleteDlg::OnBnClickedButtonCancel()
{
	ShowWindow(SW_HIDE);	
}


BOOL CFeatureEdit_DeleteDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CFeatureEdit_DeleteDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow == TRUE)
	{
	}
	else
	{
		SetViewerEditMode(EditMode::enumMode_EditMode);//AddMode 에서 일반 Edit Mode로 변경
		AddApplicationLog(END_EDIT_DELETE_TXT);
	}
}
