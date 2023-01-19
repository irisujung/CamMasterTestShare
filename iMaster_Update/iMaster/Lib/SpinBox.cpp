// XSpinBox.cpp : implementation file
//

#include "pch.h"
#include "SpinBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define SPIN_MODE_UP		1
#define SPIN_MODE_DOWN		0
#define SPIN_MODE_NOTUSE	-1

UINT NEAR WM_SPINBOX_CHANGING = ::RegisterWindowMessage(_T("WM_SPINBOX_CHANGING"));
UINT NEAR WM_SPINBOX_DELTA = ::RegisterWindowMessage(_T("WM_SPINBOX_DELTA"));

/////////////////////////////////////////////////////////////////////////////
// CSpinBox

CSpinBox* CSpinBox::m_pXSpinBox = NULL; 

CSpinBox::CSpinBox()
{
}

CSpinBox::~CSpinBox()
{
}

BEGIN_MESSAGE_MAP(CSpinBox, CSpinButtonCtrl)
	//{{AFX_MSG_MAP(CSpinBox)
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(UDN_DELTAPOS, OnDeltapos)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpinBox message handlers

CSpinBox* CSpinBox::GetInstance()
{
	if(m_pXSpinBox == NULL)
	{
		m_pXSpinBox = new CSpinBox;
	}
	return m_pXSpinBox;
}

void CSpinBox::DeleteInstance()
{
	if(m_pXSpinBox != NULL)
	{
		if(m_pXSpinBox->m_hWnd != NULL)
			m_pXSpinBox->DestroyWindow();

		delete m_pXSpinBox;
		m_pXSpinBox = NULL;
	}
}

void CSpinBox::OnDeltapos(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	*pResult = 1;

	int nLower=0, nUpper=0;
	GetRange(nLower, nUpper);

	int nPos = pNMUpDown->iPos;

	// Down
	if( pNMUpDown->iDelta < 0 )
	{
		//TRACE2("Down : iDelta(%d), iPos(%d)\n", pNMUpDown->iDelta, pNMUpDown->iPos);
		nPos -= 1;
		if( nPos < nLower )
			nPos = nLower;

		::PostMessage(GetParent()->m_hWnd, WM_SPINBOX_DELTA, (WPARAM)FALSE, (LPARAM)NULL);
	}
	// Up
	else if( pNMUpDown->iDelta > 0 )
	{
		//TRACE2("Up : iDelta(%d), iPos(%d)\n", pNMUpDown->iDelta, pNMUpDown->iPos);
		nPos += 1;
		if( nPos > nUpper )
			nPos = nUpper;

		::PostMessage(GetParent()->m_hWnd, WM_SPINBOX_DELTA, (WPARAM)TRUE, (LPARAM)NULL);
	}
	else /*if( pNMUpDown->iDelta == 0 ) */
	{
		return;
	}

	int nResult = 0;
	::SendMessage(GetParent()->m_hWnd, WM_SPINBOX_CHANGING, (WPARAM)nPos, (LPARAM)&nResult);

	if( nResult == 0 )
		SetPos(nPos);
}