// CMiniMapFormView_V2.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "MainFrm.h"
#include "MiniMapFormView_V2.h"


// CMiniMapFormView_V2

IMPLEMENT_DYNCREATE(CMiniMapFormView_V2, CFormView)

CMiniMapFormView_V2::CMiniMapFormView_V2()
	: CFormView(IDD_MINIMAP_FORM_V2)
{

}

CMiniMapFormView_V2::~CMiniMapFormView_V2()
{
}

BEGIN_MESSAGE_MAP(CMiniMapFormView_V2, CFormView)
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CMiniMapFormView_V2 메시지 처리기

CMiniMapFormView_V2* CMiniMapFormView_V2::CreateOne(CWnd* pParent)
{
	CMiniMapFormView_V2* pFormView = new CMiniMapFormView_V2;
	CCreateContext* pContext = NULL;
	if (!pFormView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 300, 300), pParent, 0, pContext))
	{
		TRACE0("Failed in creating CMiniMapFormView_V2");
	}
	return pFormView;
}

int CMiniMapFormView_V2::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (m_bInit == TRUE) return 0;

	CRect rect;
	GetClientRect(rect);

	ViewInfo_t stViewInfo;
	stViewInfo.pHwnd = &this->m_hWnd;
	stViewInfo.ptSizeXY = CPoint(rect.Width(), rect.Height());
	stViewInfo.rcRect = rect;

	Initialize_Minimap(stViewInfo);

	m_bInit = TRUE;

	return 0;
}

// CMiniMapFormView_V2 *CMiniMapFormView_V2::CreateOne(CWnd *pParent)
// {
// 	CMiniMapFormView_V2 *pFormView = new CMiniMapFormView_V2;
// 	CCreateContext *pContext = NULL;
// 
// 	if (!pFormView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 100, 100), pParent, 0, pContext))
// 	{
// 		TRACE0("Failed in creating CMiniMapFormView");
// 	}
// 
// 	return pFormView;
// }

void CMiniMapFormView_V2::OnInitialUpdate()
{
	
}


void CMiniMapFormView_V2::OnMouseMove(UINT nFlags, CPoint point)
{
	MinimapMouseMove(nFlags, point);

	//CDockablePane::OnMouseMove(nFlags, point);
}


void CMiniMapFormView_V2::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if (GetSafeHwnd() == nullptr) return;

	CRect rect;
	GetClientRect(rect);

	ViewInfo_t stViewInfo;
	stViewInfo.pHwnd = &this->m_hWnd;
	stViewInfo.ptSizeXY = CPoint(rect.Width(), rect.Height());
	stViewInfo.rcRect = rect;

	SetMinimapSizeChange(&stViewInfo);

	CString str;
	str.Format(_T("x = %d, y = %d\n"), cx, cy);
	TRACE(str);
}


void CMiniMapFormView_V2::OnLButtonDown(UINT nFlags, CPoint point)
{
	MinimapMouseLButtonDown(nFlags, point);	

	CFormView::OnLButtonDown(nFlags, point);
}


void CMiniMapFormView_V2::OnLButtonUp(UINT nFlags, CPoint point)
{	
	MinimapMouseLButtonUp(nFlags, point);

	CFormView::OnLButtonUp(nFlags, point);
}


void CMiniMapFormView_V2::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CFormView::OnRButtonDown(nFlags, point);
}


void CMiniMapFormView_V2::OnRButtonUp(UINT nFlags, CPoint point)
{
	POINT stCurPos;
	GetCursorPos(&stCurPos);

	HMENU hMenu = ::CreatePopupMenu();
	if (hMenu == nullptr)
		return ;

	/*::AppendMenu(hMenu, MF_STRING, MinimapMenu::enumID_Copy, MinimapMenu::strName[MinimapMenu::enumID_Copy - MinimapMenu::enumID_Copy]);
	::AppendMenu(hMenu, MF_STRING, MinimapMenu::enumID_Paste, MinimapMenu::strName[MinimapMenu::enumID_Paste - MinimapMenu::enumID_Copy]);
	::AppendMenu(hMenu, MF_STRING, MinimapMenu::enumID_Delete, MinimapMenu::strName[MinimapMenu::enumID_Delete - MinimapMenu::enumID_Copy]);*/
	// 2022.06.10
	// 주석추가 김준호
	// 미니맵 inspect 선택 위한 메뉴 추가
	::AppendMenu(hMenu, MF_STRING, MinimapMenu::enumID_Inspect, MinimapMenu::strName[MinimapMenu::enumID_Inspect - MinimapMenu::enumID_Inspect]);
	::AppendMenu(hMenu, MF_STRING, MinimapMenu::enumID_Copy, MinimapMenu::strName[MinimapMenu::enumID_Copy - MinimapMenu::enumID_Inspect]);
	::AppendMenu(hMenu, MF_STRING, MinimapMenu::enumID_Paste, MinimapMenu::strName[MinimapMenu::enumID_Paste - MinimapMenu::enumID_Inspect]);
	::AppendMenu(hMenu, MF_STRING, MinimapMenu::enumID_Delete, MinimapMenu::strName[MinimapMenu::enumID_Delete - MinimapMenu::enumID_Inspect]);
	::AppendMenu(hMenu, MF_STRING, MinimapMenu::enumID_ResetZoom, MinimapMenu::strName[MinimapMenu::enumID_ResetZoom - MinimapMenu::enumID_Inspect]);


	RECTD drtSelect;
	BOOL bRet = FALSE;
	bRet = Minimap_GetSelectRect(drtSelect);	

	/*if (drtSelect.CenterX() == 0 || drtSelect.CenterY() == 0)
		EnableMenuItem(hMenu, MinimapMenu::enumID_Paste, 1);

	if (bCopy == TRUE)
	{
		EnableMenuItem(hMenu, MinimapMenu::enumID_Paste, 0);		
	}
	else
	{
		EnableMenuItem(hMenu, MinimapMenu::enumID_Paste, 1);
	}*/	

	INT uiMenuCmd = ::TrackPopupMenuEx(hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, stCurPos.x, stCurPos.y, m_hWnd, nullptr);
		

	switch (uiMenuCmd)
	{
	case MinimapMenu::enumID_Inspect:
		Minimap_SetInspect();
		break;
	case MinimapMenu::enumID_Copy:		
		Minimap_SetCopy(bCopy);
		break;
	case MinimapMenu::enumID_Paste:
		Minimap_SetPaste(bCopy);
		break;
	case MinimapMenu::enumID_Delete:
		Minimap_SetDelete();
		break;
	case MinimapMenu::enumID_ResetZoom:
		MinimapResetZoom();
		break;
	}

	//CDockablePane::OnRButtonUp(nFlags, point);
}



BOOL CMiniMapFormView_V2::PreTranslateMessage(MSG* pMsg)
{

	RECTD drtSelect;
	BOOL bRet = RESULT_GOOD;
	bRet = Minimap_GetSelectRect(drtSelect);

	if (bRet == RESULT_GOOD)
	{
		if (pMsg->message == WM_KEYDOWN)
		{
			switch (pMsg->wParam)
			{
			case 'I':
			case VK_RETURN:
			{
				Minimap_SetInspect();
				break;
			}
			case 'C':
			{
				if (GetKeyState(VK_CONTROL) < 0)
				{
					Minimap_SetCopy(bCopy);
				}
				break;
			}
			case 'V':
			{
				if (GetKeyState(VK_CONTROL) < 0)
				{
					Minimap_SetPaste(bCopy);
				}
				break;
			}
			case 'D':								
			case VK_DELETE:
				Minimap_SetDelete();
				break;
			default:
				break;
			}		
		}
	}

	return CFormView::PreTranslateMessage(pMsg);
}


BOOL CMiniMapFormView_V2::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	MinimapMouseWheel(nFlags, zDelta, pt);

	return CFormView::OnMouseWheel(nFlags, zDelta, pt);
}


void CMiniMapFormView_V2::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	//더블클릭 방지를 하여 주석처리
	CFormView::OnLButtonDblClk(nFlags, point);
}
