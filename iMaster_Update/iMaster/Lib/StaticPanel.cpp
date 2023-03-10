#include "pch.h"
#include "StaticPanel.h"

CStaticPanel::CStaticPanel(void)
{
	m_colTransparent = PANEL_DEFAULT_TRASPARENTCOLOR;
	m_colText = PANEL_DEFAULT_TEXTCOLOR;

	m_strFontName = DEFAULT_PANEL_FONT;
	m_iFontSize = DEFAULT_PANEL_FONTSIZE;
	m_iFontWeight = PANEL_FONT_NORMAL;
	m_bItalic = FALSE;

	m_uiVAlign = DT_VCENTER;
	m_uiHAlign = DT_CENTER;
}

CStaticPanel::~CStaticPanel(void)
{
	m_imgPanel.Destroy();
}

BOOL CStaticPanel::SetPanelImage(CString strImg)
{
	HRESULT hRes;

	hRes = m_imgPanel.Load(strImg);

	/*Check the status of image load*/
	if (FAILED(hRes))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void CStaticPanel::PreSubclassWindow()
{
	CStatic::PreSubclassWindow();

	ModifyStyle(0, SS_OWNERDRAW);
}

BEGIN_MESSAGE_MAP(CStaticPanel, CStatic)
	ON_WM_DRAWITEM_REFLECT()
END_MESSAGE_MAP()

void CStaticPanel::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	INT32 iMode;
	CString strTxt;

	CDC *pDc = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect clRect = lpDrawItemStruct->rcItem;

	iMode = pDc->SetStretchBltMode(HALFTONE);

	m_imgPanel.TransparentBlt(lpDrawItemStruct->hDC, 0, 0, clRect.Width(), clRect.Height(), m_colTransparent);

	pDc->SetStretchBltMode(iMode);

	GetWindowText(strTxt);

	clRect.left = 20;
	clRect.top = 8;
	DrawPanelText(pDc, clRect, strTxt, m_colText);
}

void CStaticPanel::DrawPanelText(CDC *DC, CRect R, CString strTxt, COLORREF TextColor)
{
	CFont clFont, *pOldFont;
	COLORREF prevColor;
	INT32 iMode;
	//CRect clRect;
	SIZE stStringSize;
	POINT stStringPoint;

	//GetClientRect(&clRect);

	clFont.CreateFont(
		m_iFontSize,				  // nHeight
		0,                         // nWidth
		0,						  // nEscapement
		0,                         // nOrientation
		m_iFontWeight,            // nWeight
		m_bItalic,                 // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		m_strFontName);            // lpszFacename

	iMode = DC->SetBkMode(TRANSPARENT);
	prevColor = DC->SetTextColor(TextColor);
	pOldFont = DC->SelectObject(&clFont);

	GetTextExtentPoint32(DC->GetSafeHdc(), strTxt, strTxt.GetLength(), &stStringSize);

	if (m_uiHAlign == DT_CENTER)
	{
		stStringPoint.x = R.Width() / 2 - stStringSize.cx / 2;
	}
	else
	{
		stStringPoint.x = 10;
	}

	stStringPoint.y = R.Height() / 2 - stStringSize.cy / 2;

	//DC->DrawText(strTxt, strTxt.GetLength() , R, m_uiHAlign|m_uiVAlign|DT_SINGLELINE);
	DC->TextOut(stStringPoint.x, stStringPoint.y, strTxt);

	DC->SetBkMode(iMode);
	DC->SetTextColor(prevColor);
	DC->SelectObject(pOldFont);
}