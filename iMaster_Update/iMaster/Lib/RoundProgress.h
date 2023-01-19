#pragma once
#include "afxwin.h"

#include <mutex>


#define FONT_NORMAL				FW_THIN
#define FONT_BOLD				FW_HEAVY
#define DEFAULT_FONT			_T("Arial")
#define DEFAULT_FONTSIZE		14

class CRoundProgress :	public CStatic
{
private:
	int m_iMinValue = 0;
	int m_iMaxValue = 0;
	int m_iCurValue = 0;

	unsigned int m_uiFontSize = DEFAULT_FONTSIZE;

	Gdiplus::FontStyle m_enFontStyle = Gdiplus::FontStyleRegular;

	CString m_strFontName = DEFAULT_FONT;
	Gdiplus::StringAlignment m_enHorizontalAlign = Gdiplus::StringAlignmentCenter;
	Gdiplus::StringAlignment m_enVerticalAlign = Gdiplus::StringAlignmentCenter;

	COLORREF m_colFontColor = RGB(128, 128, 128);


	bool m_bInfiniteProgress = true;
	int m_iInfiniteProgressInterval = 1000; //milli Seconds

	COLORREF m_colDraw = RGB(255, 255, 255);
	COLORREF m_colCircleColor = RGB(128, 128, 128);
	int m_iCircleBorderThickness = 4;

	int m_iInfiniteProgArcSize = 20; //In degrees
	int m_iInfiniteProgArcOffset = 0; //In degrees
	int m_iInfiniteProgArcOffsetInc = 1; //In degrees


	std::recursive_mutex m_AnimationLock;
	std::recursive_mutex m_ObjectLock;
	
	CString m_strText;
	bool m_bDisplayValue = false;
public:
	void SetRange(int iMin, int iMax);
	bool SetValue(int iValue);
	int GetValue() { return m_iCurValue; }
	int GetMinRange() { return m_iMinValue; }
	int GetMaxRange() { return m_iMaxValue; }
	void SetDisplayValue(bool bDisplay) { m_bDisplayValue = bDisplay; }
	bool IsDisplayValue() { return m_bDisplayValue; }
	void SetDisplayText(CString strTxt) { m_strText = strTxt; }
	CString GetDisplayText() { return m_strText; }

	void SetFontSize(unsigned int uiSize) { m_uiFontSize = uiSize; }
	unsigned int GetFontSize() { return m_uiFontSize; }

	void SetFontStyle(Gdiplus::FontStyle enStyle) { m_enFontStyle = enStyle; }

	void SetFontName(CString strFont) { m_strFontName = strFont; }
	CString GetFontName() { return m_strFontName; }

	void SetHorizontalAlign(Gdiplus::StringAlignment enStrHAlign) { m_enHorizontalAlign = enStrHAlign; }
	Gdiplus::StringAlignment GetHorizontalAlign() { return m_enHorizontalAlign; }
	void SetVerticalAlign(Gdiplus::StringAlignment enStrVAlign) { m_enVerticalAlign = enStrVAlign; }
	Gdiplus::StringAlignment GetVerticalAlign() { return m_enVerticalAlign; }

	void SetFontColor(COLORREF colFont) { m_colFontColor = colFont; }

	void SetProgDrawColor(COLORREF colDraw) { m_colDraw = colDraw; }
	void SetProgCircleColor(COLORREF colCircle) { m_colCircleColor = colCircle; }
	void SetCircleThickness(int iSize) { m_iCircleBorderThickness = iSize; }

	int GetCircleThickness() { return m_iCircleBorderThickness; }


	void SetInfiniteProgressArcSize(int iSize) { m_iInfiniteProgArcSize = iSize; }
	int GetInfiniteProgressArcSize() { return m_iInfiniteProgArcSize; }

	void SetInfiniteProgressArcOffsetIncrement(int iInc) { m_iInfiniteProgArcOffsetInc = iInc; }
	int GetInfiniteProgressArcOffsetIncrement() { return m_iInfiniteProgArcOffsetInc; }


	void SetInfiniteProgress(bool bInfinite) { m_bInfiniteProgress = bInfinite; }
	bool IsInfiniteProgress() { return m_bInfiniteProgress; }


	void SetInfiniteProgressInterval(int iInterval) { m_iInfiniteProgressInterval = iInterval; }
	int GetInfiniteProgressInterval() { return m_iInfiniteProgressInterval; }

	void StartInfiniteProgress();
	void StopInfiniteProgress();
	static void InfiniteProgressAnimationThread(CRoundProgress *pProgCtrl);

	void DrawProgressText(CDC *pDC, CRect clBoundRect, CString strTxt);

	CRoundProgress();
	virtual ~CRoundProgress();
	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT /*nCtlColor*/);
};

