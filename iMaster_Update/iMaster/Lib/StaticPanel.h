#pragma once
#include "afxwin.h"
#include <atlimage.h>

#define PANEL_DEFAULT_TRASPARENTCOLOR	RGB(255,255,255)
#define PANEL_DEFAULT_TEXTCOLOR			RGB(0,0,0)
#define DEFAULT_PANEL_FONT				_T("Arial")
#define DEFAULT_PANEL_FONTSIZE			14
#define PANEL_FONT_NORMAL				FW_THIN
#define PANEL_FONT_BOLD					FW_HEAVY

class CStaticPanel :
	public CStatic
{
private:
	CImage m_imgPanel;
	COLORREF m_colTransparent;
	COLORREF m_colText;
	INT32 m_iFontSize;
	INT32 m_iFontWeight;
	BOOL m_bItalic;
	UINT m_uiVAlign;
	UINT m_uiHAlign;

	CString m_strFontName;

	void DrawPanelText(CDC *DC, CRect R, CString strTxt, COLORREF TextColor);

public:
	BOOL SetPanelImage(CString strImg);

	void SetTransparentColor(COLORREF colTrans) { m_colTransparent = colTrans; }
	void SetTextColor(COLORREF colText) { m_colText = colText; }
	void SetFontName(CString strFontName) { m_strFontName = strFontName; }
	void SetFontSize(INT32 iFontSize) { m_iFontSize = iFontSize; }
	void SetFontWeight(INT32 iFontWeight) { m_iFontWeight = iFontWeight; }
	void SetFontItalic(BOOL bItalic) { m_bItalic = bItalic; }
	void SetVerticalAlign(UINT uiVAlign) { m_uiVAlign = uiVAlign; }
	void SetHorizontalAlign(UINT uiHAlign) { m_uiHAlign = uiHAlign; }

	CStaticPanel(void);
	~CStaticPanel(void);
protected:
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
public:
	DECLARE_MESSAGE_MAP()
};
