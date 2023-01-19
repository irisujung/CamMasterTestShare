#include "pch.h"
#include "iMaster.h"
#include "LogViewDocPane.h"
#include "../iCommonIF/CommonIF-Error.h"
#include <thread>

IMPLEMENT_DYNCREATE(CLogViewDocPane, CDockablePane)

CLogViewDocPane::CLogViewDocPane()
{
	m_bLoadState = FALSE;
	m_bSaveToFile = true;
	m_bLogThreadLive = false;
	m_strLogFile = L"";
	m_pLogEvent = nullptr;
}

CLogViewDocPane::~CLogViewDocPane()
{
	m_bLogThreadLive = false;
	Sleep(1000);
	if (m_pLogEvent != nullptr)
	{
		m_pLogEvent->PulseEvent();
	}

	std::unique_lock <std::mutex> cLocker(m_LogThreadLock);
	m_vtLog.erase(m_vtLog.begin(), m_vtLog.end());
	cLocker.unlock();

	if (m_pLogEvent) delete m_pLogEvent;
	m_pLogEvent = nullptr;
}

BEGIN_MESSAGE_MAP(CLogViewDocPane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

void CLogViewDocPane::DoDataExchange(CDataExchange* pDX)
{
	CDockablePane::DoDataExchange(pDX);
	DDX_Control(pDX, ID_LOG_LISTBOX, m_lstLog);
}

int CLogViewDocPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect objRectDummy(0, 0, 600, 200);

	m_fraBack.Create(L"", WS_CHILD | WS_VISIBLE | SS_BITMAP, objRectDummy, this);

	BOOL bRes = m_lstLog.Create(WS_VSCROLL | WS_CHILD | WS_VISIBLE | WS_TABSTOP | LBS_OWNERDRAWVARIABLE | LBS_HASSTRINGS,
		objRectDummy, this, ID_LOG_LISTBOX);

	m_hList = m_lstLog.GetSafeHwnd();

	m_lstLog.UpdateBackColor(RGB(0, 0, 0));

	AdjustLayout();

	m_pLogEvent = new CEvent(FALSE, FALSE);
	m_bLogThreadLive = true;

	std::thread _TLogThread(LogThread, this);
	m_hThreadHandle = _TLogThread.native_handle();
	if (m_hThreadHandle)
		SetThreadPriority(m_hThreadHandle, THREAD_PRIORITY_LOWEST);

	_TLogThread.detach();

	return 0;
}

void CLogViewDocPane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CLogViewDocPane::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
		return;

	CRect rectClient;
	GetClientRect(rectClient);

	m_fraBack.SetWindowPos(NULL, rectClient.left, rectClient.top,
		rectClient.Width(), rectClient.Height(),
		SWP_NOACTIVATE | SWP_NOZORDER);

	m_lstLog.SetWindowPos(NULL, rectClient.left, rectClient.top, 
		rectClient.Width(), rectClient.Height(),
		SWP_NOACTIVATE | SWP_NOZORDER);
}

HBRUSH CLogViewDocPane::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDockablePane::OnCtlColor(pDC, pWnd, nCtlColor);
	pDC->SetBkColor(RGB(0, 0, 0));
	hbr = m_brBackColor;

	return hbr;
}

BOOL CLogViewDocPane::LoadState(LPCTSTR lpszProfileName, int nIndex, UINT uiID)
{
	m_bLoadState = CDockablePane::LoadState(lpszProfileName, nIndex, uiID);
	return m_bLoadState;
}

void CLogViewDocPane::AddLog(CString strLog, LogMsgType enMsgType)
{
	std::unique_lock <std::mutex> cLocker(m_LogThreadLock);
	
	Log_Msg stLog;
	stLog.strLog = strLog;
	stLog.enMsgType = enMsgType;
	m_vtLog.push_back(stLog);
	cLocker.unlock();

	if (m_pLogEvent)
		m_pLogEvent->PulseEvent();
}

UINT32 CLogViewDocPane::SetLogPath(CString strPath)
{
	if (strPath.GetLength() < 1) 
		return RESULT_BAD;

	m_strLogFile = strPath;
	return RESULT_GOOD;
}

BOOL CLogViewDocPane::OnShowControlBarMenu(CPoint pt)
{
	CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	if (rc.PtInRect(pt))
		return TRUE;//hide a pane contextmenu on client rea
					//show on caption bar
	return CDockablePane::OnShowControlBarMenu(pt);
}

bool CLogViewDocPane::LogThread(CLogViewDocPane* pParent)
{
	bool bIsFileOpen = false;
	FILE* fp = nullptr;
	int iPrevYear = 0;
	int iPrevMonth = 0;
	int iPrevHour = 0;

	while (pParent->m_bLogThreadLive)
	{

		if (pParent->m_pLogEvent)
			pParent->m_pLogEvent->Lock();
		else
			return 1;

		if (!pParent->m_bLogThreadLive)
		{
			if (bIsFileOpen)			
				fclose(fp);
			
			return 1;
		}

		while (pParent->m_vtLog.size() > 0)
		{
			if (!pParent->m_bLogThreadLive)
			{
				if (bIsFileOpen)
				{
					fflush(fp);
					fclose(fp);
				}
				return 1;
			}

			std::unique_lock <std::mutex> cLocker(pParent->m_LogThreadLock);
			
			Log_Msg lgMsg = pParent->m_vtLog.front();
			pParent->m_vtLog.erase(pParent->m_vtLog.begin());
			cLocker.unlock();

			CString strTxt;

			CTime clTime = CTime::GetCurrentTime();

			pParent->m_lstLog.SetRedraw(FALSE);
			if (pParent->m_lstLog.GetCount() >= MAX_LOG_COUNT)			
				pParent->m_lstLog.DeleteString(0);			

			strTxt = clTime.Format(L"%d-%b-%Y %H:%M:%S ");
			strTxt = strTxt + lgMsg.strLog;

			COLORREF clrBack = RGB(0, 0, 0);
			COLORREF clrForeground = RGB(255, 255, 255);
			switch (lgMsg.enMsgType)
			{
			case LogMsgType::EN_ERROR:
				clrForeground = RGB(255, 0, 0);
				break;
			case LogMsgType::EN_INFORMATION:
				break;
			case LogMsgType::EN_WARNING:
				clrForeground = RGB(255, 255, 0);
				break;
			}

			pParent->m_lstLog.AddString(clrBack, clrForeground, strTxt);
			pParent->m_lstLog.SetTopIndex(pParent->m_lstLog.GetCount() - 1);
			pParent->m_lstLog.SetRedraw(TRUE);

			if (pParent->m_bSaveToFile)
			{
				CString strPath, strFName;
				wchar_t* pcFName;
				INT32 iErrNo;

				if ((iPrevYear != clTime.GetYear()) ||
					(iPrevMonth != clTime.GetMonth()) ||
					(iPrevHour != clTime.GetHour()))
				{
					if (bIsFileOpen)
					{
						fflush(fp);
						fclose(fp);
						bIsFileOpen = false;
					}

					strFName = clTime.Format(L"\\%Y%m\\");
					strPath = pParent->m_strLogFile + strFName;
					CreateDirectory(strPath, NULL);

					strFName = clTime.Format(L"%d\\");
					strPath = strPath + strFName;
					CreateDirectory(strPath, NULL);

					strFName = clTime.Format(L"%H");
					strFName = strPath + L"LogView_" + strFName + L".txt";

					USES_CONVERSION;
					pcFName = T2W(strFName.LockBuffer());
					iErrNo = _wfopen_s(&fp, pcFName, L"a+");
					strFName.UnlockBuffer();

					iPrevYear = clTime.GetYear();
					iPrevMonth = clTime.GetMonth();
					iPrevHour = clTime.GetHour();

					if (iErrNo == 0)					
						bIsFileOpen = true;					
				}

				if (bIsFileOpen)				
					fwprintf(fp, L"%s\n", strTxt.GetBuffer());				
			}

			Sleep(1);
		}

		Sleep(50);
	}
	return true;
}