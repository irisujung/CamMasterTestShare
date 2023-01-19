// CFeatureEdit_MoveDlg.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "MainFrm.h"
#include "CFeatureEdit_MoveDlg.h"
#include "afxdialogex.h"


// CFeatureEdit_MoveDlg 대화 상자

IMPLEMENT_DYNAMIC(CFeatureEdit_MoveDlg, CDialogEx)

CFeatureEdit_MoveDlg::CFeatureEdit_MoveDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDIT_MOVE, pParent)
{

}

CFeatureEdit_MoveDlg::~CFeatureEdit_MoveDlg()
{
}

void CFeatureEdit_MoveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFeatureEdit_MoveDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CFeatureEdit_MoveDlg::OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CFeatureEdit_MoveDlg::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_RADIO_MOVE_X, &CFeatureEdit_MoveDlg::OnBnClickedRadioMoveX)
	ON_BN_CLICKED(IDC_RADIO_MOVE_Y, &CFeatureEdit_MoveDlg::OnBnClickedRadioMoveY)
	ON_BN_CLICKED(IDC_RADIO_MOVE_XY, &CFeatureEdit_MoveDlg::OnBnClickedRadioMoveXy)
	ON_BN_CLICKED(IDC_BUTTON_PREVIEW, &CFeatureEdit_MoveDlg::OnBnClickedButtonPreview)
END_MESSAGE_MAP()


// CFeatureEdit_MoveDlg 메시지 처리기


BOOL CFeatureEdit_MoveDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CFeatureEdit_MoveDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow == TRUE)
	{
		((CButton*)GetDlgItem(IDC_RADIO_MOVE_XY))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_MOVE_X))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_MOVE_Y))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_BUTTON_PREVIEW))->SetCheck(FALSE);

		GetDlgItem(IDC_EDIT_MOVEX)->SetWindowText(_T("0.0"));
		GetDlgItem(IDC_EDIT_MOVEY)->SetWindowText(_T("0.0"));

		SetMoveAxis(Edit_MoveAxis::enumMoveAxis_XY);
	}
	else
	{
		SetViewerEditMode(EditMode::enumMode_EditMode);//AddMode 에서 일반 Edit Mode로 변경
		AddApplicationLog(END_EDIT_MOVE_TXT);
	}
}

PointDXY CFeatureEdit_MoveDlg::_GetMoveValue()
{
	PointDXY fMoveXY = CPointD(0, 0);

	CString strTemp = _T("");
	GetDlgItem(IDC_EDIT_MOVEX)->GetWindowText(strTemp);	
	if (strTemp.GetLength() > 0)
		fMoveXY.x = static_cast<float>(_ttof(strTemp));

	GetDlgItem(IDC_EDIT_MOVEY)->GetWindowText(strTemp);
	if (strTemp.GetLength() > 0)
		fMoveXY.y = static_cast<float>(_ttof(strTemp));

	return fMoveXY;
}

void CFeatureEdit_MoveDlg::OnBnClickedButtonPreview()
{
	PointDXY fMoveXY = _GetMoveValue();

	ManualMove(fMoveXY);
}

void CFeatureEdit_MoveDlg::OnBnClickedButtonApply()
{
	SetMove_Apply();

	AddApplicationLog(APPLY_EDIT_MOVE_TXT);
	Log_FeatureSelect();

	SetViewerEditMode(EditMode::enumMode_EditMode);

	ShowWindow(SW_HIDE);
}

void CFeatureEdit_MoveDlg::OnBnClickedButtonCancel()
{
	SetViewerEditMode(EditMode::enumMode_EditMode);

	ShowWindow(SW_HIDE);
}


void CFeatureEdit_MoveDlg::OnBnClickedRadioMoveX()
{
	SetMoveAxis(Edit_MoveAxis::enumMoveAxis_X);
}


void CFeatureEdit_MoveDlg::OnBnClickedRadioMoveY()
{
	SetMoveAxis(Edit_MoveAxis::enumMoveAxis_Y);
}


void CFeatureEdit_MoveDlg::OnBnClickedRadioMoveXy()
{
	SetMoveAxis(Edit_MoveAxis::enumMoveAxis_XY);
}
