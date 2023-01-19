#include "pch.h"
#include "HeaderCtrlEx.h"


CHeaderCtrlEx::CHeaderCtrlEx()
{

}

CHeaderCtrlEx::~CHeaderCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CHeaderCtrlEx, CHeaderCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CHeaderCtrlEx::OnPaint()
{
	if (GetStyle() & HDS_FILTERBAR)
	{
		Default();
		return;
	}

	CPaintDC dc(this); // device context for painting
	CMemDC memDC(dc, this);
	CDC* pDC = &memDC.GetDC();

	CRect rectClip;
	dc.GetClipBox(rectClip);

	CRect rectClient;
	GetClientRect(rectClient);

	OnFillBackground(pDC);

	CFont* pOldFont = SelectFont(pDC);
	ASSERT_VALID(pOldFont);

	pDC->SetTextColor(GetGlobalData()->clrBtnText);
	pDC->SetBkMode(TRANSPARENT);

	CRect rect;
	GetClientRect(rect);

	CRect rectItem;
	int nCount = GetItemCount();

	int xMax = 0;

	for (int i = 0; i < nCount; i++)
	{
		

		GetItemRect(i, rectItem);

		CRgn rgnClip;
		rgnClip.CreateRectRgnIndirect(&rectItem);
		pDC->SelectClipRgn(&rgnClip);

		// 2022.11.02. KJH ADD
		// Rgn GID 객체 제거 위해 추가
		rgnClip.DeleteObject();

		// Draw item:
		OnDrawItem(pDC, i, rectItem, 0, 0);
		pDC->SelectClipRgn(NULL);

		xMax = max(xMax, rectItem.right);
	}

	// Draw "tail border":
	if (nCount == 0)
	{
		rectItem = rect;
		rectItem.right++;
	}
	else
	{
		rectItem.left = xMax;
		rectItem.right = rect.right + 1;
	}
	OnDrawItem(pDC, -1, rectItem, FALSE, FALSE);

	pDC->SelectObject(pOldFont);
}

BOOL CHeaderCtrlEx::OnEraseBkgnd(CDC* pDC)
{
	return FALSE;
}
