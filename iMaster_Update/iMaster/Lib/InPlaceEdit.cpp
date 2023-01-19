
#include "pch.h" 
#include "InPlaceEdit.h"
#include "../Lib/SpinBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CTRL_C	0x3
#define CTRL_V	0x16
#define CTRL_X	0x18

// CInPlaceEdit
UINT NEAR WM_EDITBOX_CHANGE = ::RegisterWindowMessage(_T("WM_EDITBOX_CHANGE"));
CInPlaceEdit* CInPlaceEdit::m_pInPlaceEdit = NULL;  

CInPlaceEdit::CInPlaceEdit()
{
	m_iRowIndex= -1;
	m_iColumnIndex = -1;
	m_bESC = FALSE;

	m_nMinNum = -1;
	m_nMaxNum = -1;

	m_nVK = 0;
}

CInPlaceEdit::~CInPlaceEdit()
{
	CSpinBox::DeleteInstance();
}

BEGIN_MESSAGE_MAP(CInPlaceEdit, CEdit)
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_REGISTERED_MESSAGE(WM_SPINBOX_DELTA, OnSpinDelta)
	ON_WM_CREATE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CInPlaceEdit message handlers

LRESULT CInPlaceEdit::OnSpinDelta(WPARAM wParam, LPARAM lParam)
{
	BOOL bUp = (BOOL)wParam;

	CString sText(_T(""));
	GetWindowText(sText);
	double nValue = _tstof(sText);

	if (bUp)
	{
		nValue += 0.1;
		if (nValue <= m_nMaxNum)
		{
			sText.Format(_T("%.1f"), nValue);
			SetWindowText(sText);
		}
	}
	else
	{
		nValue -= 0.1;
		if (nValue >= m_nMinNum)
		{
			sText.Format(_T("%.1f"), nValue);
			SetWindowText(sText);
		}
	}

	return 0L;
}


void CInPlaceEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here

	::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
	//::SendMessage(GetParent()->m_hWnd, WM_EDITBOX_CHANGE, (WPARAM)m_nVK, (LPARAM)NULL);

	// Get the text in the edit ctrl
	CString strEdit;
	GetWindowText(strEdit);

	// Send Notification to parent of edit ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

//	double dValid = _ttof(strEdit);
//	if (dValid >= 0.5 && dValid <= 1.5)
//	{
		dispinfo.item.mask = LVIF_TEXT;
		dispinfo.item.iItem = m_iRowIndex;
		dispinfo.item.iSubItem = m_iColumnIndex;
		dispinfo.item.pszText = m_bESC ? LPTSTR((LPCTSTR)m_strWindowText) : LPTSTR((LPCTSTR)strEdit);
		dispinfo.item.cchTextMax = m_bESC ? m_strWindowText.GetLength() : strEdit.GetLength();

		GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo);

		PostMessage(WM_CLOSE);
//	}
//	else
//	{
//		MessageBox(L"Only Accept Value Between 0.5 & 1.5", L"Waring", MB_OK | MB_ICONEXCLAMATION);
//		return;
//	}
}

BOOL CInPlaceEdit::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (WM_KEYDOWN == pMsg->message && (VK_ESCAPE == pMsg->wParam || VK_RETURN == pMsg->wParam))
	{
		if (VK_ESCAPE == pMsg->wParam)
		{
			m_bESC = TRUE;
		}

		GetParent()->SetFocus();
		return TRUE;
	}
	else if(pMsg->message == WM_KEYDOWN)
	{
		// Enter Key
		if (pMsg->wParam == VK_RETURN ||
			pMsg->wParam == VK_ESCAPE ||
			pMsg->wParam == VK_TAB)
		{
			m_nVK = static_cast<int>(pMsg->wParam);
			::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}

int CInPlaceEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Set the proper font
	CFont* pFont = GetParent()->GetFont();
	SetFont(pFont);

	if (m_nMinNum >= 0.5 && m_nMaxNum <= 1.5)
	{
		ModifyStyle(0, WS_CLIPCHILDREN);
		CSpinBox::DeleteInstance();
		CSpinBox *pSpinBox = CSpinBox::GetInstance();
		if (pSpinBox)
		{
			CRect rc;
			GetClientRect(&rc);
			rc.left = rc.right - 16;
			pSpinBox->Create(UDS_WRAP | UDS_SETBUDDYINT | UDS_AUTOBUDDY | UDS_NOTHOUSANDS |
				WS_CHILD | WS_VISIBLE, rc, this, NULL);
			pSpinBox->SetRange(0, 1);
		}
	}

	ShowWindow(SW_SHOW);
	SetWindowText(m_strWindowText);
	SetFocus();
	  
	return 0;
}

CInPlaceEdit* CInPlaceEdit::GetInstance()
{
	if(m_pInPlaceEdit == NULL)
	{
		m_pInPlaceEdit = new CInPlaceEdit;
	}
	return m_pInPlaceEdit;
}


void CInPlaceEdit::DeleteInstance()
{
	if (m_pInPlaceEdit != NULL)
	{
		if (m_pInPlaceEdit->m_hWnd != NULL)
			m_pInPlaceEdit->DestroyWindow();

		delete m_pInPlaceEdit;
		m_pInPlaceEdit = NULL;
	}
}

BOOL CInPlaceEdit::ShowEditCtrl(DWORD dwStyle, const RECT &rCellRect, CWnd* pParentWnd, 
								UINT uiResourceID, int iRowIndex, int iColumnIndex,
								 CString& rstrCurSelection)
{
	m_iRowIndex = iRowIndex;
	m_iColumnIndex = iColumnIndex;
	m_strWindowText = rstrCurSelection;
	m_bESC = FALSE;

	if (NULL == m_pInPlaceEdit->m_hWnd) 
	{
		return m_pInPlaceEdit->Create(dwStyle, rCellRect, pParentWnd, uiResourceID); 
	}	

	return TRUE;
}

void CInPlaceEdit::OnPaint()
{
	Default();

	if (m_nMinNum >= 0 && m_nMaxNum >= 0 &&
		CSpinBox::m_pXSpinBox)
	{
		CSpinBox::m_pXSpinBox->RedrawWindow();
	}
}
