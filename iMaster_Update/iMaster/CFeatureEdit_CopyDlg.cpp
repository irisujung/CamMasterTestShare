// CFeatureEdit_CopyDlg.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "MainFrm.h"
#include "CFeatureEdit_CopyDlg.h"
#include "afxdialogex.h"


// CFeatureEdit_CopyDlg 대화 상자

IMPLEMENT_DYNAMIC(CFeatureEdit_CopyDlg, CDialogEx)

CFeatureEdit_CopyDlg::CFeatureEdit_CopyDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDIT_COPY, pParent)
{

}

CFeatureEdit_CopyDlg::~CFeatureEdit_CopyDlg()
{
}

void CFeatureEdit_CopyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFeatureEdit_CopyDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CFeatureEdit_CopyDlg::OnBnClickedButtonApply)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_PREVIEW, &CFeatureEdit_CopyDlg::OnBnClickedButtonPreview)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &CFeatureEdit_CopyDlg::OnBnClickedButtonCancel)
END_MESSAGE_MAP()


// CFeatureEdit_CopyDlg 메시지 처리기
BOOL CFeatureEdit_CopyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	_InitValue();
	_UpdateUI();
	_UpdateMode(m_nMode);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CFeatureEdit_CopyDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow == TRUE)
	{
		_InitValue();
		_UpdateUI();
		_UpdateMode(m_nMode);
	}
	else
	{
		SetViewerEditMode(EditMode::enumMode_EditMode);
		AddApplicationLog(END_EDIT_COPY_TXT);
	}
}

void	CFeatureEdit_CopyDlg::_InitValue()
{
	m_nMode = enumModeCopy_Once;

	m_dptCopyXY = CPointD(0., 0.);
	m_dptRepeatPitchXY = CPointD(0., 0.);
	m_ptRepeatCount = CPoint(0, 0);
}

void	CFeatureEdit_CopyDlg::_UpdateUI()
{//Data to IU
	CString strTemp;

	strTemp.Format(_T("%.3lf"), m_dptCopyXY.x);
	GetDlgItem(IDC_EDIT_COPY_X)->SetWindowText(strTemp);

	strTemp.Format(_T("%.3lf"), m_dptCopyXY.y);
	GetDlgItem(IDC_EDIT_COPY_Y)->SetWindowText(strTemp);

	strTemp.Format(_T("%.3lf"), m_dptRepeatPitchXY.x);
	GetDlgItem(IDC_EDIT_PITCH_X)->SetWindowText(strTemp);

	strTemp.Format(_T("%.3lf"), m_dptRepeatPitchXY.y);
	GetDlgItem(IDC_EDIT_PITCH_Y)->SetWindowText(strTemp);

	strTemp.Format(_T("%d"), m_ptRepeatCount.x);
	GetDlgItem(IDC_EDIT_REPEAT_X)->SetWindowText(strTemp);
	
	strTemp.Format(_T("%d"), m_ptRepeatCount.y);
	GetDlgItem(IDC_EDIT_REPEAT_Y)->SetWindowText(strTemp);
}

void	CFeatureEdit_CopyDlg::_UpdateData()
{//UI To Data
	CString strTemp;

	GetDlgItem(IDC_EDIT_COPY_X)->GetWindowText(strTemp);
	m_dptCopyXY.x = _ttof(strTemp);

	GetDlgItem(IDC_EDIT_COPY_Y)->GetWindowText(strTemp);
	m_dptCopyXY.y = _ttof(strTemp);

	GetDlgItem(IDC_EDIT_PITCH_X)->GetWindowText(strTemp);
	m_dptRepeatPitchXY.x = _ttof(strTemp);

	GetDlgItem(IDC_EDIT_PITCH_Y)->GetWindowText(strTemp);
	m_dptRepeatPitchXY.y = _ttof(strTemp);

	GetDlgItem(IDC_EDIT_REPEAT_X)->GetWindowText(strTemp);
	m_ptRepeatCount.x = _ttoi(strTemp);

	GetDlgItem(IDC_EDIT_REPEAT_Y)->GetWindowText(strTemp);
	m_ptRepeatCount.y = _ttoi(strTemp);

	BOOL bOnceMode = ((CButton*)GetDlgItem(IDC_RADIO_COPY_ONCE))->GetCheck();
	if (bOnceMode == TRUE)
	{
		m_nMode = enumModeCopy_Once;
	}
	else
	{
		m_nMode = enumModeCopy_Repeat;
	}

}

BOOL	CFeatureEdit_CopyDlg::_IsValid()
{
	BOOL bValid = TRUE;

	if (m_nMode == enumModeCopy_Once)
	{
		if (m_dptCopyXY.x == 0.0 && m_dptCopyXY.y == 0.0)
		{
			bValid = FALSE;
		}
	}
	else
	{//Repeat Mode
		if (m_ptRepeatCount.x == 0 || m_ptRepeatCount.y == 0)
		{
			bValid = FALSE;
		}
		//else if ( )
	}

	return bValid;
}

void CFeatureEdit_CopyDlg::OnBnClickedButtonPreview()
{
	_UpdateData();

	//Valid Check
	if (_IsValid() == TRUE)
	{//Valid

		if (m_nMode == enumModeCopy_Once)
		{
			SetCopy(m_dptCopyXY);
		}
		if (m_nMode == enumModeCopy_Repeat)
		{
			SetCopy_Repeat(m_dptRepeatPitchXY, m_ptRepeatCount);
		}

	}
	else
	{//Not Valid
		AfxMessageBox(_T("Copy 작업을 수행하지 못하였습니다.\n 파라미터 재 확인 필요"));
	}
	
}

void CFeatureEdit_CopyDlg::OnBnClickedButtonApply()
{
	_UpdateData();
	
	//Valid Check
	if (_IsValid() == TRUE)
	{//Valid
		if (m_nMode == enumModeCopy_Once)
		{
			SetCopy_Apply(m_dptCopyXY);
		}
		if (m_nMode == enumModeCopy_Repeat)
		{
			SetCopy_Repeat_Apply(m_dptRepeatPitchXY, m_ptRepeatCount);
		}


		AddApplicationLog(APPLY_EDIT_COPY_TXT);
		Log_FeatureSelect();

		SetViewerEditMode(EditMode::enumMode_EditMode);

		ShowWindow(SW_HIDE);
	}
	else
	{//Not Valid
		AfxMessageBox(_T("Copy 작업을 수행하지 못하였습니다.\n 파라미터 재 확인 필요"));
	}
}



void CFeatureEdit_CopyDlg::OnBnClickedButtonCancel()
{
	SetViewerEditMode(EditMode::enumMode_EditMode);
	ShowWindow(FALSE);
}

BOOL CFeatureEdit_CopyDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

void	CFeatureEdit_CopyDlg::_UpdateMode(int nMode)
{
	m_nMode = nMode;

	switch (nMode)
	{
	default:
	case enumModeCopy_Once :
		((CButton*)GetDlgItem(IDC_RADIO_COPY_ONCE))->SetCheck(TRUE);
		((CButton*)GetDlgItem(IDC_RADIO_COPY_REPEAT))->SetCheck(FALSE);
		break;
	case enumModeCopy_Repeat:
		((CButton*)GetDlgItem(IDC_RADIO_COPY_ONCE))->SetCheck(FALSE);
		((CButton*)GetDlgItem(IDC_RADIO_COPY_REPEAT))->SetCheck(TRUE);
		break;
	}
}
