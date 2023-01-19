#pragma once

#include <afxdockablepane.h>
#include <mutex>
#include <vector>
#include "Lib/ListBoxEx.h"
#include "../iCommonIF/CommonIF-Struct.h"

#define MAX_LOG_COUNT	2000

class CLogViewDocPane : public CDockablePane
{
public:
	CLogViewDocPane();
	virtual ~CLogViewDocPane();

	BOOL OnShowControlBarMenu(CPoint pt);
	BOOL isPrevStateLoaded() { return m_bLoadState; }
	BOOL LoadState(LPCTSTR lpszProfileName, int nIndex, UINT uiID);

	void AddLog(CString strLog, LogMsgType enMsgType);
	UINT32 SetLogPath(CString strPath);

private:
	BOOL	m_bLoadState;
	bool	m_bSaveToFile;
	bool	m_bLogThreadLive;

	CString m_strLogFile;
	CEvent* m_pLogEvent = nullptr;

	std::vector<Log_Msg> m_vtLog;
	std::mutex m_LogThreadLock;
	
	CListBoxEx	m_lstLog;
	CBrush		m_brBackColor;

	CStatic		m_fraBack;
	HWND		m_hList;
	HANDLE		m_hThreadHandle;

private:
	void AdjustLayout();
	static bool LogThread(CLogViewDocPane* pParent);
	DECLARE_DYNCREATE(CLogViewDocPane)

protected:
	DECLARE_MESSAGE_MAP()

public:
	virtual void DoDataExchange(CDataExchange* pDX);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};