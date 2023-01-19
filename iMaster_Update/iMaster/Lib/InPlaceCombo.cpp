
#include "pch.h" 
#include "InPlaceCombo.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CInPlaceCombo

CInPlaceCombo* CInPlaceCombo::m_pInPlaceCombo = NULL; 
UINT NEAR WM_COMBOBOX_CHANGE = ::RegisterWindowMessage(_T("WM_COMBOBOX_CHANGE"));

CInPlaceCombo::CInPlaceCombo()
{
	m_iRowIndex = -1;
	m_iColumnIndex = -1;
	m_bESC = FALSE;
}

CInPlaceCombo::~CInPlaceCombo()
{
}

BEGIN_MESSAGE_MAP(CInPlaceCombo, CComboBox)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceCombo message handlers

int CInPlaceCombo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Set the proper font
	CFont* pFont = GetParent()->GetFont();
	SetFont(pFont);
	
	SetFocus();

	ResetContent(); 
	for (POSITION Pos_ = m_DropDownList.GetHeadPosition(); Pos_ != NULL;)
	{
		AddString((LPCTSTR) (m_DropDownList.GetNext(Pos_)));
	}

	return 0;
}

BOOL CInPlaceCombo::PreTranslateMessage(MSG* pMsg) 
{
	// If the message if for "Enter" or "Esc"
	// Do not process
	if (pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			// DO NOT process further
			return TRUE;						
		}
	}
	
	return CComboBox::PreTranslateMessage(pMsg);
}

void CInPlaceCombo::OnKillFocus(CWnd* pNewWnd) 
{
	CComboBox::OnKillFocus(pNewWnd);	

	// Close the control
	::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
	::SendMessage(GetParent()->m_hWnd, WM_COMBOBOX_CHANGE, (WPARAM)NULL, (LPARAM)NULL);
}

void CInPlaceCombo::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// If the key is "Esc" set focus back to the list control
	if (nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if (nChar == VK_ESCAPE)
		{
			m_bESC = TRUE;
		}

		GetParent()->SetFocus();
		return;
	}
	
	CComboBox::OnChar(nChar, nRepCnt, nFlags);
}

void CInPlaceCombo::OnCloseup() 
{
	// Set the focus to the parent list control
	GetParent()->SetFocus();
	::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
	::SendMessage(::GetParent(m_hWnd), WM_COMBOBOX_CHANGE, (WPARAM)NULL, (LPARAM)NULL);
}

CInPlaceCombo* CInPlaceCombo::GetInstance()
{
	if(m_pInPlaceCombo == NULL)
	{
		m_pInPlaceCombo = new CInPlaceCombo;
	}
	return m_pInPlaceCombo;
}

void CInPlaceCombo::DeleteInstance()
{
	if (m_pInPlaceCombo != NULL)
	{
		if (m_pInPlaceCombo->m_hWnd != NULL)
			m_pInPlaceCombo->DestroyWindow();

		delete m_pInPlaceCombo;
		m_pInPlaceCombo = NULL;
	}
}

BOOL CInPlaceCombo::ShowComboCtrl(DWORD dwStyle, const CRect &rCellRect, CWnd* pParentWnd, UINT uiResourceID,
								  int iRowIndex, int iColumnIndex, CStringList* pDropDownList, 
								  CString strCurSelecetion, int iCurSel )
{

	m_iRowIndex = iRowIndex;
	m_iColumnIndex = iColumnIndex;
	m_bESC = FALSE;
	
	m_DropDownList.RemoveAll(); 
	m_DropDownList.AddTail(pDropDownList);

	BOOL bRetVal = TRUE;

	if (-1 != iCurSel)
	{
		GetLBText(iCurSel, m_strWindowText);
	}
	else if (!strCurSelecetion.IsEmpty()) 
	{
		m_strWindowText = strCurSelecetion;
	}
	
	if (NULL == m_pInPlaceCombo->m_hWnd) 
	{
		bRetVal = m_pInPlaceCombo->Create(dwStyle, rCellRect, pParentWnd, uiResourceID); 
	}

	SetCurSel(iCurSel);

	return bRetVal;
}