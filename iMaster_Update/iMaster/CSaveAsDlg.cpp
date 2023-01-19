// CSaveAsDlg.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "CSaveAsDlg.h"
#include "afxdialogex.h"


// CSaveAsDlg 대화 상자

IMPLEMENT_DYNAMIC(CSaveAsDlg, CDialogEx)

CSaveAsDlg::CSaveAsDlg(CString strJobName, CWnd* pParent /*=nullptr*/)
	: m_strJobName(strJobName),
	CDialogEx(IDD_DIALOG_SAVE_AS, pParent)
{

}

CSaveAsDlg::~CSaveAsDlg()
{
}

void CSaveAsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSaveAsDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CSaveAsDlg::OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CSaveAsDlg::OnBnClickedButtonApply)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CSaveAsDlg 메시지 처리기


BOOL CSaveAsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	GetDlgItem(IDC_EDIT_JOBNAME)->SetWindowText(m_strJobName);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CSaveAsDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow == TRUE)
	{
		GetDlgItem(IDC_EDIT_JOBNAME)->SetWindowText(m_strJobName);
	}
}

BOOL CSaveAsDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

CString CSaveAsDlg::GetJobName()
{
	return m_strJobName;
}

void CSaveAsDlg::OnBnClickedButtonApply()
{
	GetDlgItem(IDC_EDIT_JOBNAME)->GetWindowText(m_strJobName);

	if (m_strJobName == _T(""))
	{
		AfxMessageBox(_T("Job Name이 입력되지 않았습니다."));
	}
	else
	{
		CDialogEx::OnOK();
	}
}

void CSaveAsDlg::OnBnClickedButtonCancel()
{
	CDialogEx::OnCancel();
}





