
#include "pch.h"
#include "ButtonEx.h"


// CButtonEx

IMPLEMENT_DYNAMIC(CButtonEx, CButton)


CButtonEx::CButtonEx()
{
	m_strFontName = DEFAULT_BUTTON_FONT; 
	m_colFontEnabled = DEFAULT_BUTTON_TEXTCOLOR;
	m_colFontDisabled = DEFAULT_BUTTON_DISTEXTCOLOR;
	m_iFontSize = DEFAULT_BUTTON_FONTSIZE;
	m_iFontWeight = BUTTON_FONT_NORMAL;
	m_colTransparent = BUTTON_DEFAULT_TRASPARENTCOLOR;
	m_bItalic = FALSE;
	m_szIcon.cx = 0;
	m_szIcon.cy = 0;

	m_bStickyState = FALSE;

	m_bGlow = FALSE;
}
CButtonEx::~CButtonEx()
{
	m_imgPressed.Destroy();
	m_imgNormal.Destroy();
	m_imgDisabled.Destroy(); 

	/*If icon is created destroy it*/
	if (m_hIcon) 
	{
		DestroyIcon(m_hIcon);
	}
}


BEGIN_MESSAGE_MAP(CButtonEx, CButton)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
END_MESSAGE_MAP()

void CButtonEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CString strTxt;
	CDC dc;
	CRect rt;
	INT32 iMode, iGlowMode, iImageWidth, iImageHeight;//, iConSize;
	UINT state;
	CImage imgGlow;
	HDC hGlowDc, hNormalDc;
	COLORADJUSTMENT stColorParam;
	BOOL bRes;
	COLORREF colText;

	POINT stpntIcon;

	GetWindowText(strTxt);

	dc.Attach(lpDrawItemStruct->hDC);		//Get device context object
	rt = lpDrawItemStruct->rcItem;		//Get button rect
	state = lpDrawItemStruct->itemState;

	stpntIcon.x = rt.Width() / 2 - m_szIcon.cx / 2;
	
	if (strTxt.GetLength() > 0)
	{
		stpntIcon.y = rt.Height() * 3 / 4 / 2 - m_szIcon.cy / 2;
	}
	else
	{
		stpntIcon.y = rt.Height() / 2 - m_szIcon.cy / 2;
	}

	iMode = dc.SetStretchBltMode(HALFTONE);
	colText = m_colFontEnabled;

	/*If button in sticky state draw as pressed*/
	if (TRUE == m_bStickyState)
	{
		if (!m_imgPressed.IsNull())
		{
			TransparentBlt(dc.m_hDC, 0, 0, rt.Width(), rt.Height(), m_imgPressed.GetDC(), 0, 0, m_imgPressed.GetWidth(), m_imgPressed.GetHeight(), m_colTransparent);
			m_imgPressed.ReleaseDC();
		}
		colText = m_colFontDisabled; 
	}
	else if ((state & ODS_SELECTED) > 0)		// If it is pressed
	{
		if (!m_imgPressed.IsNull())
		{
			TransparentBlt(dc.m_hDC, 0, 0, rt.Width(), rt.Height(), m_imgPressed.GetDC(), 0, 0, m_imgPressed.GetWidth(), m_imgPressed.GetHeight(), m_colTransparent);
			m_imgPressed.ReleaseDC();
		}

		stpntIcon.x++;
		stpntIcon.y++;
		colText = m_colFontDisabled; 
	}
    else if ((state & ODS_DISABLED) > 0)	//if disabled draw as disbled state
	{
		if (!m_imgDisabled.IsNull())
		{
			TransparentBlt(dc.m_hDC, 0, 0, rt.Width(), rt.Height(), m_imgDisabled.GetDC(), 0, 0, m_imgDisabled.GetWidth(), m_imgDisabled.GetHeight(), m_colTransparent);
			m_imgDisabled.ReleaseDC();
		}
		colText = m_colFontDisabled; 
    }
	else
	{
		/*If mouse over m_bGlow will be set so draw with more brightness*/
		if (m_bGlow && (!m_imgNormal.IsNull()))
		{

			iImageWidth = m_imgNormal.GetWidth();
			iImageHeight = m_imgNormal.GetHeight();

			bRes = imgGlow.Create(iImageWidth, iImageHeight, 24);// m_imgNormal.GetBPP());


			hGlowDc = imgGlow.GetDC(); 

			GetColorAdjustment(hGlowDc, &stColorParam);
			stColorParam.caBrightness = BUTTON_GLOW_LEVEL;
	
			SetColorAdjustment(hGlowDc, &stColorParam);

			iGlowMode = SetStretchBltMode(hGlowDc, HALFTONE);

			hNormalDc = m_imgNormal.GetDC();  
			bRes = StretchBlt(hGlowDc, 0,0,iImageWidth,iImageHeight,hNormalDc, 0,0,iImageWidth,iImageHeight,SRCCOPY);
		
			bRes = TransparentBlt(dc.m_hDC,0,0,rt.Width(),rt.Height(),hGlowDc,0,0,iImageWidth,iImageHeight ,m_colTransparent);  
			
			imgGlow.ReleaseDC();  
			m_imgNormal.ReleaseDC();  


			SetStretchBltMode(hGlowDc, iGlowMode);
			imgGlow.Destroy(); 

		}
		else
		{	/*Draw as normal*/
			if (!m_imgNormal.IsNull())
			{
				TransparentBlt(dc.m_hDC, 0, 0, rt.Width(), rt.Height(), m_imgNormal.GetDC(), 0, 0, m_imgNormal.GetWidth(), m_imgNormal.GetHeight(), m_colTransparent);
				m_imgNormal.ReleaseDC();
			}
		}

		colText = m_colFontEnabled; 
	}

	/*If Icon handle is valid draw it*/
	if (m_hIcon) 
	{
		DrawIconEx(dc.m_hDC, stpntIcon.x, stpntIcon.y, m_hIcon, 0, 0, 0,NULL,DI_NORMAL | DI_COMPAT   );

		rt.top = stpntIcon.y + m_szIcon.cy - 5; 
	}
	DrawButtonText(&dc, rt,strTxt,colText);	


	dc.SetStretchBltMode(iMode); 
	dc.Detach();

}

void CButtonEx::SetImages(CString strPressImage, CString strNormalImage, CString strDisabledImage, CString strIcon, SIZE szIcon)
{
	m_imgPressed.Load(strPressImage);
	m_imgNormal.Load(strNormalImage); 
	m_imgDisabled.Load(strDisabledImage);
	
	/*If icon path is null*/
	if (strIcon.GetLength() < 1)
	{
		m_hIcon = NULL;
	}
	else
	{
		/*Load Icon*/
		m_szIcon = szIcon;
		if (szIcon.cx == 0)
		{
			m_szIcon.cx = 32;
		}
		if (szIcon.cy == 0)
		{
			m_szIcon.cy = 32;
		}
		m_hIcon = (HICON)::LoadImage(NULL, strIcon, IMAGE_ICON, m_szIcon.cx, m_szIcon.cy, LR_LOADFROMFILE);  

	}
	 
}

void CButtonEx::SetTransparentColor(COLORREF colTransparent)
{
	m_colTransparent = colTransparent;

}


void CButtonEx::SetStickyState (BOOL bState) 
{ 
	m_bStickyState = bState; 
	RedrawWindow();
}
void CButtonEx::ToggleStickyState () 
{ 
	m_bStickyState = !m_bStickyState; 
	RedrawWindow();
}

void CButtonEx::DrawButtonText(CDC *DC, CRect R, CString strTxt, 
					COLORREF TextColor)
 {
	CFont clFont, *pOldFont;
	COLORREF prevColor;

	clFont.CreateFont(
	   m_iFontSize,				  // nHeight
	   0,                         // nWidth
	   0,						  // nEscapement
	   0,                         // nOrientation
	   m_iFontWeight ,            // nWeight
	   m_bItalic,                 // bItalic
	   FALSE,                     // bUnderline
	   0,                         // cStrikeOut
	   ANSI_CHARSET,              // nCharSet
	   OUT_DEFAULT_PRECIS,        // nOutPrecision
	   CLIP_DEFAULT_PRECIS,       // nClipPrecision
	   DEFAULT_QUALITY,           // nQuality
	   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
	   m_strFontName);            // lpszFacename


    prevColor = DC->SetTextColor(TextColor);
    DC->SetBkMode(TRANSPARENT);
	pOldFont = DC->SelectObject(&clFont); 
	
    DC->DrawText(strTxt, strTxt.GetLength() , R, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    DC->SetTextColor(prevColor);
	DC->SelectObject(pOldFont); 
 }

void CButtonEx::SetFontColor(COLORREF colEnable, COLORREF colDisable)
{
	m_colFontEnabled = colEnable;
	m_colFontDisabled = colDisable;
}

HBRUSH CButtonEx::CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/)
{
	return (HBRUSH)::GetStockObject(NULL_BRUSH);
}

void CButtonEx::OnMouseMove(UINT nFlags, CPoint point)
{
	int iResult;
	TRACKMOUSEEVENT Tme;

	Tme.cbSize = sizeof(TRACKMOUSEEVENT);
	Tme.dwFlags = TME_LEAVE;
	Tme.hwndTrack = m_hWnd;
	iResult = TrackMouseEvent(&Tme);

	/*If mouse button is pressed return*/
	if (nFlags == MK_LBUTTON)
	{
		CButton::OnMouseMove(nFlags, point);
		return;
	}

	/*If glow state return*/
	if (m_bGlow)
	{
		CButton::OnMouseMove(nFlags, point);
		return;
	}

	m_bGlow = TRUE;

	Invalidate();

	CButton::OnMouseMove(nFlags, point);
}

void CButtonEx::OnMouseLeave()
{
	/*If not glow state return*/
	if (!m_bGlow)
	{
		CButton::OnMouseLeave();
		return;
	}

	m_bGlow = FALSE;
	Invalidate();
	CButton::OnMouseLeave();
}
