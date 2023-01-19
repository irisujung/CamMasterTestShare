
#include "pch.h"
#include "ListBoxEx.h"

IMPLEMENT_DYNAMIC(CListBoxEx, CListBox)


CListBoxEx::CListBoxEx()
{
	m_brBackColor.CreateSolidBrush(RGB(255,255,255));
}

 
CListBoxEx:: ~CListBoxEx()
{
	m_vtItemInfo.clear();
}



int CListBoxEx::AddString(LPCTSTR lpszItem)
{
	return AddString(RGB(255, 255, 255), RGB(0, 0, 0), lpszItem);
}

 
int CListBoxEx::AddString(COLORREF clrBkColor, COLORREF clrForeColor, LPCTSTR lpszItem)
{
	return AddString(clrBkColor, clrForeColor, LISTCTRLEX_FONT, false, false, LISTCTRLEX_FONTSIZE, lpszItem);
}
 
int CListBoxEx::AddString(COLORREF clrBkColor, COLORREF clrForeColor, CString strFontName, bool bIsBold, bool bIsItalic, int iFontSize, LPCTSTR lpszItem)
{
	ItemInfo_t stItemInfo;

	stItemInfo.colBkColor = clrBkColor;
	stItemInfo.colForeColor = clrForeColor;
	stItemInfo.bIsBold = bIsBold;
	stItemInfo.bIsItalic = bIsItalic;
	stItemInfo.iFontSize = iFontSize;
	stItemInfo.strFontName = strFontName;

	m_vtItemInfo.push_back(stItemInfo);

	return __super::AddString(lpszItem);

}

 
int CListBoxEx::DeleteString(UINT nIndex)
{
	m_vtItemInfo.erase(m_vtItemInfo.begin() + nIndex);

	return __super::DeleteString(nIndex);
}

 
int CListBoxEx::InsertString(int nIndex, LPCTSTR lpszItem)
{
	return InsertString(nIndex, RGB(255, 255, 255), RGB(0, 0, 0), lpszItem);
}


int CListBoxEx::InsertString(int nIndex, COLORREF clrBkColor, COLORREF clrForeColor, LPCTSTR lpszItem)
{
	return InsertString(nIndex, clrBkColor, clrForeColor, LISTCTRLEX_FONT, false, false, LISTCTRLEX_FONTSIZE, lpszItem);
}


 
int CListBoxEx::InsertString(int nIndex, COLORREF clrBkColor, COLORREF clrForeColor, CString strFontName, bool bIsBold, bool bIsItalic, int iFontSize, LPCTSTR lpszItem)
{
	ItemInfo_t stItemInfo;

	stItemInfo.colBkColor = clrBkColor;
	stItemInfo.colForeColor = clrForeColor;
	stItemInfo.bIsBold = bIsBold;
	stItemInfo.bIsItalic = bIsItalic;
	stItemInfo.iFontSize = iFontSize;
	stItemInfo.strFontName = strFontName;

	m_vtItemInfo.insert(m_vtItemInfo.begin() + nIndex, stItemInfo);

	return __super::InsertString(nIndex, lpszItem);
}


void CListBoxEx::ResetContent()
{
	m_vtItemInfo.clear();
	__super::ResetContent();
}

void CListBoxEx::UpdateBackColor(COLORREF clrBack)
{
	m_brBackColor.DeleteObject();
	m_brBackColor.CreateSolidBrush(clrBack);
}

BEGIN_MESSAGE_MAP(CListBoxEx, CListBox)
ON_WM_DRAWITEM_REFLECT()
ON_WM_MEASUREITEM_REFLECT()
ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



void CListBoxEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rItem;
	CDC* dc = CDC::FromHandle(lpDrawItemStruct->hDC);

	if ((int)lpDrawItemStruct->itemID < 0)
	{
		
		if ((lpDrawItemStruct->itemAction & ODA_FOCUS) && (lpDrawItemStruct->itemState & ODS_FOCUS))
		{
			dc->DrawFocusRect(&lpDrawItemStruct->rcItem);
		}
		else if ((lpDrawItemStruct->itemAction & ODA_FOCUS) && !(lpDrawItemStruct->itemState & ODS_FOCUS))
		{
			dc->DrawFocusRect(&lpDrawItemStruct->rcItem);
		}
		return;
	}

	CString strText;
	ItemInfo_t stItemInfo;

	stItemInfo = m_vtItemInfo[lpDrawItemStruct->itemID];



	// Get the item text.
	__super::GetText(lpDrawItemStruct->itemID, strText);

	//Initialize the CListBox Item's row size
	rItem = lpDrawItemStruct->rcItem;

	UINT nFormat = DT_LEFT | DT_SINGLELINE | DT_VCENTER;
	if (GetStyle() & LBS_USETABSTOPS)
		nFormat |= DT_EXPANDTABS;


	if ((lpDrawItemStruct->itemState & ODS_SELECTED) &&

	(lpDrawItemStruct->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{
		CBrush br(::GetSysColor(COLOR_HIGHLIGHT));
		dc->FillRect(&rItem, &br);
	}
	else 
	{
		CBrush br(stItemInfo.colBkColor);
		dc->FillRect(&rItem, &br);
	}


	if ((lpDrawItemStruct->itemAction & ODA_FOCUS) && (lpDrawItemStruct->itemState & ODS_FOCUS))
	{
		dc->DrawFocusRect(&rItem);
	}
	else if ((lpDrawItemStruct->itemAction & ODA_FOCUS) && !(lpDrawItemStruct->itemState & ODS_FOCUS))
	{
		dc->DrawFocusRect(&rItem);
	}

	int iBkMode = dc->SetBkMode(TRANSPARENT);

	COLORREF crText;
	CFont clFont;

	int iFontWeight = LISTCTRLEX_NORMAL;

	if (stItemInfo.bIsBold) iFontWeight = LISTCTRLEX_BOLD;

	clFont.CreateFont(
		stItemInfo.iFontSize,				  // nHeight
		0,                         // nWidth
		0,						  // nEscapement
		0,                         // nOrientation
		iFontWeight,        // nWeight
		stItemInfo.bIsItalic,					// bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		stItemInfo.strFontName);            // lpszFacename



	crText = stItemInfo.colForeColor;
	dc->SetTextColor(crText);
	CFont * pOldFont = dc->SelectObject(&clFont);

	//Draw the Text
	dc->TextOut(rItem.left, rItem.top, strText);
	dc->SetBkColor(RGB(0, 0 ,0));

	dc->SelectObject(pOldFont);

	clFont.DeleteObject();
}



void CListBoxEx::MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/)
{
	//Do not delete this function.
}


BOOL CListBoxEx::OnEraseBkgnd(CDC* pDC)
{
	if (m_brBackColor.GetSafeHandle())
	{
		CRect rcClient;
		GetClientRect(rcClient);
		rcClient.top = rcClient.Height() - 10;
		pDC->FillRect(&rcClient, &m_brBackColor);
	}
	return TRUE;
}