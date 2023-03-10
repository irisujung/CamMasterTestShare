
#pragma once
#include "atlimage.h"

#define DEFAULT_BUTTON_TEXTCOLOR		RGB(0,0,0)
#define DEFAULT_BUTTON_DISTEXTCOLOR		RGB(128,128,128)
#define DEFAULT_BUTTON_FONT				_T("Arial")
#define DEFAULT_BUTTON_FONTSIZE			14
#define BUTTON_FONT_NORMAL				FW_THIN
#define BUTTON_FONT_BOLD				FW_HEAVY
#define BUTTON_DEFAULT_TRASPARENTCOLOR	RGB(255,255,255)
#define BUTTON_GLOW_LEVEL				20
#define ICON_BORDER_PIXELS				4

class CButtonEx : public CButton
{
private:
	INT32			m_iFontSize;				//Caption font size 
	INT32			m_iFontWeight;				//Caption font weight
	BOOL			m_bItalic;					//Caption italic flag
	BOOL			m_bStickyState;				//Sticky button flag
	BOOL			m_bGlow;					//Glow flag
	SIZE			m_szIcon;

	COLORREF		m_colFontEnabled;			//Enabled caption color
	COLORREF		m_colFontDisabled;			//Disabled font color
	COLORREF		m_colTransparent;			//Button image transparent color.

	CString			m_strFontName;				//Caption font name

	CImage			m_imgPressed;				//Pressed state background image
	CImage			m_imgNormal;					//Normal state background image
	CImage			m_imgDisabled;				//Diabled state background image

	HICON			m_hIcon;					//Icon image file
	

	DECLARE_DYNAMIC(CButtonEx)

public:
	void SetImages(CString strPressImage, CString strNormalImage, CString strDisabledImage, CString strIcon, SIZE szIcon); //Sets the images
	void DrawButtonText(CDC *DC, CRect R, CString strTxt, COLORREF TextColor);	//Draw the caption text
	void SetFontColor(COLORREF colEnable, COLORREF colDisable);					//Sets the font color
	void SetFontName(CString strFontName) {m_strFontName = strFontName;}			//Sets the font name
	void SetFontSize(INT32 iFontSize){m_iFontSize = iFontSize;}					//Sets the font size
	void SetFontWeight(INT32 iFontWeight){m_iFontWeight = iFontWeight;}			//Sets the font weight
	void SetFontItalic(BOOL bItalic){m_bItalic = bItalic;}						//Sets the font italic
	void SetTransparentColor(COLORREF colTransparent);							//Sets the transparency color	
	void SetStickyState (BOOL bState);											//Sets the sticky state
	BOOL GetStickyState () { return m_bStickyState; }							//Gets the sticky state

	void ToggleStickyState ();

	CButtonEx();																//Constructor
	virtual ~CButtonEx();														//Destructor
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);					//Draw the button
	
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);					//Fired when control is repainted
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);						//Fired when mouse moves
	afx_msg void OnMouseLeave();												//Fired when mouse leaves the client area
};


