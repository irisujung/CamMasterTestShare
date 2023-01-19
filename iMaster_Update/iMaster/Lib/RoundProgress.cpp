#include "pch.h"
#include "RoundProgress.h"
#include <thread>

using namespace Gdiplus;

void CRoundProgress::SetRange(int iMin, int iMax)
{
	m_iMinValue = iMin; 
	m_iMaxValue = iMax; 
	m_iCurValue = m_iMinValue;
}

bool CRoundProgress::SetValue(int iValue)
{
	if ((iValue < m_iMinValue) || (iValue > m_iMaxValue))
	{
		m_iCurValue = m_iMinValue;
		return false;
	}

	m_iCurValue = iValue;
	return true;
}

CRoundProgress::CRoundProgress()
{
}


CRoundProgress::~CRoundProgress()
{
	std::lock_guard<std::recursive_mutex> clockerObj(m_ObjectLock);
	std::lock_guard<std::recursive_mutex> clockerAnim(m_AnimationLock);
	m_bInfiniteProgress = false;

}
BEGIN_MESSAGE_MAP(CRoundProgress, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()


HBRUSH CRoundProgress::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	Gdiplus::Graphics clGraphics(pDC->m_hDC);

	CRect clRectDisplay;

	GetClientRect(&clRectDisplay);


	int iSizeOffset = clRectDisplay.Height() - clRectDisplay.Width();

	if (iSizeOffset > 0)
	{
		clRectDisplay.bottom = clRectDisplay.bottom - iSizeOffset;
	}
	else if (iSizeOffset < 0)
	{
		clRectDisplay.right = clRectDisplay.right + iSizeOffset;
	}

	clRectDisplay.DeflateRect(m_iCircleBorderThickness, m_iCircleBorderThickness);

	Gdiplus::Pen clPen(Gdiplus::Color(255, GetRValue(m_colCircleColor), GetGValue(m_colCircleColor), GetBValue(m_colCircleColor)));
	
	clPen.SetWidth((Gdiplus::REAL)m_iCircleBorderThickness);

	clGraphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	clGraphics.DrawEllipse(&clPen, clRectDisplay.left, clRectDisplay.top, clRectDisplay.Width(), clRectDisplay.Height());

	if (m_bInfiniteProgress)
	{
		Gdiplus::Color clDrawColor(128,GetRValue(m_colDraw), GetGValue(m_colDraw), GetBValue(m_colDraw));
		clPen.SetColor(clDrawColor);
		clGraphics.DrawArc(&clPen, clRectDisplay.left, clRectDisplay.top, clRectDisplay.Width(), clRectDisplay.Height(), (Gdiplus::REAL)m_iInfiniteProgArcOffset, (Gdiplus::REAL)(m_iInfiniteProgArcOffset + m_iInfiniteProgArcSize));

		if (m_strText.GetLength() > 0)
		{
			DrawProgressText(pDC, clRectDisplay, m_strText);
		}
	}
	else
	{
		if (m_bDisplayValue)
		{
			CString strTxt;
			strTxt.Format(L"%d", m_iCurValue);
			DrawProgressText(pDC, clRectDisplay, strTxt);
		}
	}
	return NULL;
}


void CRoundProgress::DrawProgressText(CDC *pDC, CRect clBoundRect, CString strTxt)
{
	Gdiplus::Graphics clGraphics(pDC->m_hDC);
	Gdiplus::Font clGdiFont(m_strFontName, (Gdiplus::REAL)m_uiFontSize, m_enFontStyle, Gdiplus::UnitPoint);
	Gdiplus::RectF clGdiRect((Gdiplus::REAL)clBoundRect.left, (Gdiplus::REAL)clBoundRect.top, (Gdiplus::REAL)clBoundRect.Width(), (Gdiplus::REAL)clBoundRect.Height());
	Gdiplus::StringFormat clFormat;
	clFormat.SetAlignment(m_enHorizontalAlign);
	clFormat.SetLineAlignment(m_enVerticalAlign);

	Gdiplus::SolidBrush clFontColor(Gdiplus::Color(255, GetRValue(m_colFontColor), GetGValue(m_colFontColor), GetBValue(m_colFontColor)));


	clGraphics.DrawString(strTxt, strTxt.GetLength(), &clGdiFont, clGdiRect, &clFormat, &clFontColor);
}

void CRoundProgress::StartInfiniteProgress()
{
	std::thread _TProgressThread(InfiniteProgressAnimationThread, this);
	_TProgressThread.detach();
}

void CRoundProgress::StopInfiniteProgress()
{ 
	std::lock_guard<std::recursive_mutex> clockerAnim(m_AnimationLock);
	m_bInfiniteProgress = false; 
}

void CRoundProgress::InfiniteProgressAnimationThread(CRoundProgress *pProgCtrl)
{
	if (!pProgCtrl->m_bInfiniteProgress) return;


	while (pProgCtrl->m_bInfiniteProgress)
	{
		std::lock_guard<std::recursive_mutex> clockerObj(pProgCtrl->m_ObjectLock);
		Sleep(pProgCtrl->m_iInfiniteProgressInterval);

		std::lock_guard<std::recursive_mutex> clockerAnim(pProgCtrl->m_AnimationLock);

		if (!pProgCtrl->m_bInfiniteProgress) return;

		if (!pProgCtrl->GetSafeHwnd()) return;

		CDC *pDc = pProgCtrl->GetDC();

		if (pDc == nullptr) return;

		Gdiplus::Graphics clGraphics(pDc->m_hDC );

		CRect clRectDisplay;

		pProgCtrl->GetClientRect(&clRectDisplay);


		int iSizeOffset = clRectDisplay.Height() - clRectDisplay.Width();

		if (iSizeOffset > 0)
		{
			clRectDisplay.bottom = clRectDisplay.bottom - iSizeOffset;
		}
		else if (iSizeOffset < 0)
		{
			clRectDisplay.right = clRectDisplay.right + iSizeOffset;
		}

		clRectDisplay.DeflateRect(pProgCtrl->m_iCircleBorderThickness, pProgCtrl->m_iCircleBorderThickness);

		Gdiplus::Pen clPen(Gdiplus::Color(255, GetRValue(pProgCtrl->m_colCircleColor), GetGValue(pProgCtrl->m_colCircleColor), GetBValue(pProgCtrl->m_colCircleColor)));

		clPen.SetWidth((Gdiplus::REAL)pProgCtrl->m_iCircleBorderThickness);

		Gdiplus::Status clStatus =  clGraphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		clGraphics.DrawEllipse(&clPen, clRectDisplay.left, clRectDisplay.top, clRectDisplay.Width(), clRectDisplay.Height());

		if (pProgCtrl->m_bInfiniteProgress)
		{
			Gdiplus::Color clDrawColor(GetRValue(pProgCtrl->m_colDraw), GetGValue(pProgCtrl->m_colDraw), GetBValue(pProgCtrl->m_colDraw));
			clPen.SetColor(clDrawColor);
			clGraphics.DrawArc(&clPen, clRectDisplay.left, clRectDisplay.top, clRectDisplay.Width(), clRectDisplay.Height(), 
                (Gdiplus::REAL)pProgCtrl->m_iInfiniteProgArcOffset, (Gdiplus::REAL)pProgCtrl->m_iInfiniteProgArcSize);

			if (pProgCtrl->m_strText.GetLength() > 0)
			{
				pProgCtrl->DrawProgressText(pDc, clRectDisplay, pProgCtrl->m_strText);
			}
		}

		if (pDc && pProgCtrl->GetSafeHwnd())
			pProgCtrl->ReleaseDC(pDc);

		if (pProgCtrl->GetSafeHwnd())
			pProgCtrl->m_iInfiniteProgArcOffset = pProgCtrl->m_iInfiniteProgArcOffset + pProgCtrl->m_iInfiniteProgArcOffsetInc;
		
		if (pProgCtrl->GetSafeHwnd() || pProgCtrl->m_iInfiniteProgArcOffset > 360) 
			pProgCtrl->m_iInfiniteProgArcOffset = 0;
	}
}