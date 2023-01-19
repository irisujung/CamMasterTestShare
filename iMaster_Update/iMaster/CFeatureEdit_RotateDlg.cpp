// CFeatureEdit_RotateDlg.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "CFeatureEdit_RotateDlg.h"
#include "MainFrm.h"
#include "afxdialogex.h"


// CFeatureEdit_RotateDlg 대화 상자

IMPLEMENT_DYNAMIC(CFeatureEdit_RotateDlg, CDialogEx)

CFeatureEdit_RotateDlg::CFeatureEdit_RotateDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDIT_ROTATE, pParent)
{

}

CFeatureEdit_RotateDlg::~CFeatureEdit_RotateDlg()
{
}

void CFeatureEdit_RotateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFeatureEdit_RotateDlg, CDialogEx)

	ON_BN_CLICKED(IDC_BUTTON_PREVIEW, &CFeatureEdit_RotateDlg::OnBnClickedButtonPreview)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_ANGLE_0, IDC_RADIO_ANGLE_270, &CFeatureEdit_RotateDlg::OnAngleButtonClicked)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_RADIO_MIRROR_NO,IDC_RADIO_MIRROR_YES, &CFeatureEdit_RotateDlg::OnMirrorButtonClicked)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CFeatureEdit_RotateDlg::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDCANCEL, &CFeatureEdit_RotateDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CFeatureEdit_RotateDlg 메시지 처리기
void CFeatureEdit_RotateDlg::OnAngleButtonClicked(UINT32 nID)
{
	if (nID <= 0) return;

	int nIndex = nID - IDC_RADIO_ANGLE_0;

	m_nAngleMode = nIndex;
}
void CFeatureEdit_RotateDlg::OnMirrorButtonClicked(UINT32 nID)
{
	if (nID <= 0) return;

	int nIndex = nID - IDC_RADIO_MIRROR_NO;

	if (nIndex == 0)
	{
		m_bMirror = FALSE;
	}
	else
	{
		m_bMirror = TRUE;
	}
}

void CFeatureEdit_RotateDlg ::_UpdateDataToUI()
{
	switch (m_nAngleMode)
	{
	default:
	case 0:
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_0))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_90))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_180))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_270))->SetCheck(FALSE);
		break;
	case 1:
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_0))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_90))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_180))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_270))->SetCheck(FALSE);
		break;
	case 2:
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_0))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_90))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_180))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_270))->SetCheck(FALSE);
		break;
	case 3:
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_0))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_90))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_180))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_ANGLE_270))->SetCheck(TRUE);
		break;
	}
	if (m_bMirror == TRUE)
	{
		((CButton*)GetDlgItem(IDC_RADIO_MIRROR_NO))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_MIRROR_YES))->SetCheck(TRUE);

	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO_MIRROR_NO))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_MIRROR_YES))->SetCheck(FALSE);
	}
}

BOOL CFeatureEdit_RotateDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	_UpdateDataToUI();

	return TRUE;  
}
void CFeatureEdit_RotateDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow == TRUE)
	{
		
	}
	else
	{
		SetViewerEditMode(EditMode::enumMode_EditMode);
		AddApplicationLog(END_EDIT_ROTATE_TXT);
	}
}


void CFeatureEdit_RotateDlg::OnBnClickedButtonPreview()
{
	_UpdateDataToUI();
	
	SetRotate(m_nAngleMode, m_bMirror);

}


void CFeatureEdit_RotateDlg::OnBnClickedButtonApply()
{
	_UpdateDataToUI();

	SetRotate_Apply(m_nAngleMode, m_bMirror);

	CString strLog;
	strLog.Format(APPLY_EDIT_ROTATE_TXT);
	AddApplicationLog(strLog);
	Log_FeatureSelect();
	SetViewerEditMode(EditMode::enumMode_EditMode);

	ShowWindow(SW_HIDE);
}


void CFeatureEdit_RotateDlg::OnBnClickedCancel()
{
	SetViewerEditMode(EditMode::enumMode_EditMode);
	ShowWindow(SW_HIDE);
}
