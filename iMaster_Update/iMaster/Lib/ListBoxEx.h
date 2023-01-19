
#pragma once

#include <vector>


#define LISTCTRLEX_FONT					_T("Arial")
#define LISTCTRLEX_FONTSIZE				15
#define LISTCTRLEX_NORMAL				FW_THIN
#define LISTCTRLEX_BOLD					FW_HEAVY

struct ItemInfo_t
{
	COLORREF colBkColor;
	COLORREF colForeColor;

	CString strFontName;

	int iFontSize;
	bool bIsItalic;
	bool bIsBold;
};

class CListBoxEx : public CListBox
{
private:
	std::vector <ItemInfo_t> m_vtItemInfo;
	CBrush m_brBackColor;

	DECLARE_DYNAMIC(CListBoxEx)
	DECLARE_MESSAGE_MAP()

public:
	int AddString(LPCTSTR lpszItem);
	int AddString(COLORREF clrBkColor, COLORREF clrForeColor, LPCTSTR lpszItem);
	int AddString(COLORREF clrBkColor, COLORREF clrForeColor, CString strFontName, bool bIsBold, bool bIsItalic, int iFontSize, LPCTSTR lpszItem);

	int DeleteString(UINT nIndex);
	int InsertString(int nIndex, LPCTSTR lpszItem);
	int InsertString(int nIndex, COLORREF clrBkColor, COLORREF clrForeColor, LPCTSTR lpszItem);
	int InsertString(int nIndex, COLORREF clrBkColor, COLORREF clrForeColor, CString strFontName, bool bIsBold, bool bIsItalic, int iFontSize, LPCTSTR lpszItem);
	void ResetContent();
	void UpdateBackColor(COLORREF clrBack = RGB(255, 255, 255));


	CListBoxEx();
	virtual ~CListBoxEx();
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT /*lpMeasureItemStruct*/);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};



