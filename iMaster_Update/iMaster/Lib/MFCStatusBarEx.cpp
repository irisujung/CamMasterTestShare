#include "pch.h"
#include "MFCStatusBarEx.h"

const int nTextMargin = 4; // Gap between image and text

IMPLEMENT_DYNAMIC(CMFCStatusBarEx, CMFCStatusBar)


CMFCStatusBarEx::CMFCStatusBarEx()
{
}

CMFCStatusBarEx::~CMFCStatusBarEx()
{
}


void CMFCStatusBarEx::OnDrawPane(CDC* pDC, CMFCStatusBarPaneInfo* pPane)
{
	ASSERT_VALID(pDC);
	ENSURE(pPane != NULL);

	CRect rectPane = pPane->rect;
	if(rectPane.IsRectEmpty() || !pDC->RectVisible(rectPane))
	{
		return;
	}

	COLORREF defautclr = (COLORREF)-1;
	
	// Fill pane background:
	if(pPane->clrBackground != defautclr)
	{
		CDrawingManager dm(*pDC);
		dm.Fill4ColorsGradient(rectPane, RGB(179, 209, 245), RGB(215, 230, 249), RGB(215, 230, 249) ,RGB(179, 209, 245) );
	}
	
	// Draw pane border:
	if(!(pPane->nStyle & SBPS_NOBORDERS))
	{
		if(pPane->nStyle & SBPS_POPOUT)
		{
			CDrawingManager dm(*pDC);
			dm.HighlightRect(rectPane);
		}

		// Draw pane border:
		pDC->Draw3dRect(rectPane, RGB(86, 125, 176), RGB(179, 209, 245));
	}

	if(!(pPane->nStyle & SBPS_NOBORDERS)) // only adjust if there are borders
	{
		rectPane.DeflateRect(2 * AFX_CX_BORDER, AFX_CY_BORDER);
	}

	// Draw icon
	if(pPane->hImage != NULL && pPane->cxIcon > 0)
	{
		CRect rectIcon = rectPane;
		rectIcon.right = rectIcon.left + pPane->cxIcon;

		int x = max(0, (rectIcon.Width() - pPane->cxIcon) / 2);
		int y = max(0, (rectIcon.Height() - pPane->cyIcon) / 2);

		BOOL ret = ::ImageList_DrawEx(pPane->hImage, pPane->nCurrFrame, pDC->GetSafeHdc(), rectIcon.left + x, rectIcon.top + y,
			pPane->cxIcon, pPane->cyIcon, CLR_NONE, 0, ILD_NORMAL);
	}

	CRect rectText = rectPane;
	rectText.left += pPane->cxIcon;

	if(pPane->cxIcon > 0)
	{
		rectText.left += nTextMargin;
	}

	if(pPane->nProgressTotal > 0)
	{
		// Draw progress bar:
		CRect rectProgress = rectText;
		rectProgress.DeflateRect(1, 1);

		COLORREF clrBar = (pPane->clrProgressBar == defautclr)?GetGlobalData()->clrHilite:pPane->clrProgressBar;
		
		CMFCVisualManager::GetInstance()->OnDrawStatusBarProgress(pDC, this, rectProgress,
																															pPane->nProgressTotal, pPane->nProgressCurr, clrBar, pPane->clrProgressBarDest, pPane->clrProgressText, pPane->bProgressText);
	}
	else
	{
		// Draw text
		if(pPane->lpszText != NULL && pPane->cxText > 0)
		{
			COLORREF clrText = pDC->SetTextColor(CMFCVisualManager::GetInstance()->GetStatusBarPaneTextColor(this, pPane));

			pDC->DrawText(pPane->lpszText, static_cast<int>(_tcslen(pPane->lpszText)), rectText, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_NOPREFIX);
			pDC->SetTextColor(clrText);
		}
	}

}
