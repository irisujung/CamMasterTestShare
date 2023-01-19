
// iMasterView.cpp: CiMasterView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "iMaster.h"
#endif

#include "iMasterDoc.h"
#include "iMasterView.h"
#include "MainFrm.h" 
#include <gdiplus.h>

#include "CFeatureEdit_AddDlg.h"
#include "CAlignMaskEditDlg.h"

#include "FeatureEditFormView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CiMasterView

IMPLEMENT_DYNCREATE(CiMasterView, CFormView)

BEGIN_MESSAGE_MAP(CiMasterView, CFormView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CiMasterView 생성/소멸

CiMasterView::CiMasterView()
	: CFormView(CiMasterView::IDD)
{
	
	// TODO: 여기에 생성 코드를 추가합니다.
	
}

CiMasterView::~CiMasterView()
{
	
}

void CiMasterView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BOOL CiMasterView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CiMasterView 그리기

void CiMasterView::OnDraw(CDC* /*pDC*/)
{
	BeginWaitCursor();
	DrawCamData(FALSE);
	EndWaitCursor();
}

// CiMasterView 진단

#ifdef _DEBUG
void CiMasterView::AssertValid() const
{
	CView::AssertValid();
}

void CiMasterView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CiMasterDoc* CiMasterView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CiMasterDoc)));
	return (CiMasterDoc*)m_pDocument;
}
#endif //_DEBUG


// CiMasterView 메시지 처리기
void CiMasterView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	
	CRect rect;
	GetClientRect(rect);
	
	m_stViewInfo.pHwnd = &this->m_hWnd;
	m_stViewInfo.hDC = ::GetDC(this->m_hWnd);
	m_stViewInfo.rcRect = rect;
	m_stViewInfo.ptSizeXY.x = rect.Width();
	m_stViewInfo.ptSizeXY.y = rect.Height();	

	Initialize_SetMainView(this);

	//DirectX 연동 : iDXPicCtrl Class 구현
	
	//m_pDXView.InitDXControlHwnd(this->GetSafeHwnd(), GetMaxLayerNum());
	Initialize_iProcess();
	Initialize_DxDraw(&m_stViewInfo);

	AddApplicationLog(L"==== Display View Initialize success ====");
}

void CiMasterView::InitCursorPt()
{
//	m_cursorPt.x = m_cursorPt.y = -1;
}

void CiMasterView::OnLButtonDown(UINT nFlags, CPoint point)
{
// 	m_prevPt = point;
// 	m_bMoveRMouse = FALSE;

	DrawLButtonDown(nFlags, point);
	UpdateStatusBar(point);



	CFormView::OnLButtonDown(nFlags, point);
}

void CiMasterView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
// 	BOOL bLoaded = ((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetFlag_LoadLayers();
// 	if (!bLoaded)
// 		return;

	DrawLBtuttonDblClk(nFlags, point);

	CFormView::OnLButtonDblClk(nFlags, point);
}



void CiMasterView::OnLButtonUp(UINT nFlags, CPoint point)
{
	BOOL bLoaded = ((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetFlag_LoadLayers();

	DrawLButtonUp(nFlags, point);
	UpdateStatusBar(point);
	
	
	CFormView::OnLButtonUp(nFlags, point);
}

void CiMasterView::OnRButtonDown(UINT nFlags, CPoint point)
{
	DrawRButtonDown(nFlags, point);



	CFormView::OnRButtonDown(nFlags, point);
}

void CiMasterView::OnRButtonUp(UINT nFlags, CPoint point)
{

	if (GetWorkLayer() == "") return ;

	if (IsZoomRectMode() || IsMeasureMode())
	{
		DrawRButtonUp(nFlags, point);
	}

	if (GetViewerEditMode() == EditMode::enumMode_EditMask || GetViewerEditMode() == EditMode::enumMode_EditMask_Add_FeatureBase) return;

	else
	{
		
		BOOL bSelect = IsSelect();
		BOOL bMove = IsMove();


		//Feature 선택이 안되어 있다면,
		//R Button Double Click을 실행하기 위하여
		//Feature 선택이 되었을 때만 Menu를 Popup한다.
		if (bSelect == TRUE || bMove ==TRUE)
		{

			POINT stCurPos;
			GetCursorPos(&stCurPos);

			HMENU hMenu = ::CreatePopupMenu();
			if (hMenu == nullptr)
			{
				return;
			}

			::AppendMenu(hMenu, MF_STRING, ID_EDIT_FEATRURE, _T("Edit Feature"));
			::AppendMenu(hMenu, MF_STRING, ID_EDIT_APPLY, _T("Edit Apply"));


			if (bSelect == TRUE || bMove ==TRUE)
			{
				//Log_FeatureSelect();
				EnableMenuItem(hMenu, ID_EDIT_FEATRURE, MF_BYCOMMAND | MF_ENABLED);
			}
// 			if (bMove == FALSE)
// 			{
// 				EnableMenuItem(hMenu, ID_EDIT_FEATRURE, MF_BYCOMMAND | MF_GRAYED);
// 			}


			INT uiMenuCmd = ::TrackPopupMenuEx(hMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, stCurPos.x, stCurPos.y, m_hWnd, nullptr);

			switch (uiMenuCmd)
			{
			case ID_EDIT_FEATRURE:
			{
				SetEdit_Feature(); 
			}
			break;
			case ID_EDIT_APPLY:
			{
				SetEdit_Apply();
			}
			}
		}

	}

	CFormView::OnRButtonUp(nFlags, point);
}

void CiMasterView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!GetFlag_LoadLayers()) return;

	DrawMouseMove(nFlags, point);

	UpdateStatusBar(point);

	CFormView::OnMouseMove(nFlags, point);
}

BOOL CiMasterView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	//if (nFlags & (MK_SHIFT | MK_CONTROL)) 
	//	return FALSE;

	//BOOL bLoaded = ((CMainFrame*)AfxGetApp()->m_pMainWnd)->GetFlag_LoadLayers();
	//if (!bLoaded)
	//	return FALSE;

	BeginWaitCursor();
	/*
	if (zDelta <= 0) // wheel down	
	{
		DrawZoomInOut(eZoomType::eZoomOut);
	}
	else       // wheel up
	{

		DrawZoomInOut(eZoomType::eZoomIn);
	}
	*/

	DrawMouseWheel(nFlags, zDelta, pt);

	EndWaitCursor();

	//m_cursorPt.x = m_cursorPt.y = -1;

	return CFormView::OnMouseWheel(nFlags, zDelta, pt);
}
// 
// void CiMasterView::DrawZoomBox(CDC *pDC, UINT nFlags, CPoint point)
// {
// 	if (nFlags != MK_LBUTTON) 
// 	{ 
// 		m_bZoomRect = FALSE;
// 		return;
// 	}
// 
// 	CPen	Pen, *pOldPen;
// 	CBrush	Brush, *pOldBrush;
// 
// 	Brush.CreateSolidBrush(WHITE_COLOR);
// 	pOldBrush = (CBrush*)pDC->SelectObject(&Brush);
// 
// 	Pen.CreatePen(PS_DOT, 1, WHITE_COLOR);
// 	pOldPen = (CPen*)pDC->SelectObject(&Pen);
// 
// 	if (!m_bZoomRect)
// 	{
// 		m_rcZoomRectTmp.top = m_rcZoomRectTmp.bottom = point.y;
// 		m_rcZoomRectTmp.left = m_rcZoomRectTmp.right = point.x;
// 		m_rcZoomRect.top = m_rcZoomRect.bottom = point.y;
// 		m_rcZoomRect.left = m_rcZoomRect.right = point.x;
// 		m_bZoomRect = TRUE;
// 	}
// 	pDC->SetROP2(R2_NOT);
// 	pDC->DrawFocusRect(&m_rcZoomRect);
// 
// 	m_rcZoomRectTmp.bottom = point.y;
// 	m_rcZoomRectTmp.right = point.x;
// 
// 	if (m_rcZoomRectTmp.top >= m_rcZoomRectTmp.bottom)
// 	{
// 		m_rcZoomRect.top = m_rcZoomRectTmp.bottom;
// 		m_rcZoomRect.bottom = m_rcZoomRectTmp.top;
// 	}
// 	else
// 	{
// 		m_rcZoomRect.top = m_rcZoomRectTmp.top;
// 		m_rcZoomRect.bottom = m_rcZoomRectTmp.bottom;
// 	}
// 
// 	if (m_rcZoomRectTmp.left >= m_rcZoomRectTmp.right)
// 	{
// 		m_rcZoomRect.left = m_rcZoomRectTmp.right;
// 		m_rcZoomRect.right = m_rcZoomRectTmp.left;
// 	}
// 	else
// 	{
// 		m_rcZoomRect.left = m_rcZoomRectTmp.left;
// 		m_rcZoomRect.right = m_rcZoomRectTmp.right;
// 	}
// 
// 	pDC->DrawFocusRect(&m_rcZoomRect);
// 
// 	pDC->SelectObject(pOldPen);
// 	pDC->SelectObject(pOldBrush);
// 	Brush.DeleteObject();
// 	Pen.DeleteObject();
// }

void CiMasterView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	DrawRButtonDblClk(nFlags, point);

	CFormView::OnRButtonDblClk(nFlags, point);
}


BOOL CiMasterView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_ESCAPE)
	{
		ResetSelect();

		ResetEdit();


		return TRUE;
	}
	
	if (pMsg->message == WM_LBUTTONDOWN) 
	{
		BOOL bSend = FALSE;
		CFeatureEdit_AddDlg *pDlg = GetFeature_AddDlg();
		if (pDlg != nullptr)
		{
			if (pDlg->IsWindowVisible())
			{
				pDlg->PreTranslateMessage(pMsg);
				bSend = TRUE;
			}
		}
	}

	if (pMsg->wParam == VK_SHIFT )
	{
		BOOL bSend = FALSE;
		CFeatureEdit_AddDlg *pDlg = GetFeature_AddDlg();
		if (pDlg != nullptr)
		{
			if (pDlg->IsWindowVisible())
			{
				pDlg->PreTranslateMessage(pMsg);
				bSend = TRUE;
			}
		}
	}

	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == 'S' || pMsg->wParam == 'E' || pMsg->wParam =='C' || pMsg->wParam =='D'||pMsg->wParam ==VK_DELETE)
		{//Add Dlg에 단축키 전송
			BOOL bSend = FALSE;
			CFeatureEdit_AddDlg *pDlg = GetFeature_AddDlg();
			
			if (pDlg != nullptr)
			{
				if (pDlg->IsWindowVisible())
				{
					pDlg->PreTranslateMessage(pMsg);
					bSend = TRUE;
				}
			}

			if (bSend == FALSE)
			{
				CAlignMaskEditDlg *pDlg = GetAlignMaskEditDlg();
				if (pDlg != nullptr)
				{
					if (pDlg->IsWindowVisible())
					{
						pDlg->PreTranslateMessage(pMsg);
						bSend = TRUE;
					}
				}
			}
		}
	}

	return CFormView::PreTranslateMessage(pMsg);
}

void CiMasterView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if (cx == 0 || cy == 0) return;

	m_stViewInfo.ptSizeXY.x = cx;
	m_stViewInfo.ptSizeXY.y = cy;

	GetClientRect(&m_stViewInfo.rcRect);

	SetViewInfo(&m_stViewInfo);

	Invalidate(FALSE);
}


void CiMasterView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	UserSetInfo_t* pstUserSetInfo = GetUserSetInfo();
	PointDXY point = CPointD(0, 0);
	BOOL bSelect = FALSE;
	bSelect = IsSelect();	

	switch (nChar)
	{
	case 0x43:
	{
		Set_CW();
	}
	break;
	case VK_DELETE:
	case 'd':
	case 'D':
	{
		//BOOL bSelect = FALSE;

		//bSelect = IsSelect();

		if (bSelect == FALSE)
		{
			return;
		}
		else 
		{
			if (AfxMessageBox(_T("삭제하시겠습니까?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				SetDelete_Apply();

				AddApplicationLog(APPLY_EDIT_DELETE_TXT);
			}
			else
			{
				SetViewerEditMode(EditMode::enumMode_EditMode);
			}
		}
	}
	break;
	case VK_UP:
	{		
		if (pstUserSetInfo->dAngle == 0 && pstUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본0도
		{
			point.y += m_Move.y;
		}
		else if (pstUserSetInfo->dAngle == 90 && pstUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본90도 
		{
			point.x -= m_Move.x;
		}
		else if (pstUserSetInfo->dAngle == 180 && pstUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본180도
		{
			point.y -= m_Move.y;
		}
		else if (pstUserSetInfo->dAngle == 270 && pstUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본270도
		{
			point.x += m_Move.x;
		}

		else if (pstUserSetInfo->dAngle == 0 && pstUserSetInfo->mirrorDir == eDirectionType::eHorizontal) //  좌우반전 0도
		{
			point.y += m_Move.y;
		}
		else if (pstUserSetInfo->dAngle == 90 && pstUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 90도
		{
			point.x -= m_Move.x;

		}
		else if (pstUserSetInfo->dAngle == 180 && pstUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 180도
		{
			point.y -= m_Move.y;
		}
		else if (pstUserSetInfo->dAngle == 270 && pstUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 270도
		{
			point.x += m_Move.x;
		}
		if (point.y == 0 && point.x == 0)
		{
			MessageBox(L"이동거리가 입력되지 않았습니다.", L"", MB_OK | MB_ICONQUESTION);
			SetEdit_Feature();
		}
		

 		Feature_Move_KeyBoard(point, eKeyDir::eUP);

	}
	break;
	case VK_DOWN:
	{
		//BOOL bSelect = FALSE;

		//bSelect = IsSelect();
		
		//PointDXY point = CPointD(0, 0);

		
		if (pstUserSetInfo->dAngle == 0 && pstUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본0도
		{
			point.y -= m_Move.y;
		}
		else if (pstUserSetInfo->dAngle == 90 && pstUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본90도 
		{
			point.x += m_Move.x;
		}
		else if (pstUserSetInfo->dAngle == 180 && pstUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본180도
		{
			point.y += m_Move.y;
		}
		else if (pstUserSetInfo->dAngle == 270 && pstUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본270도
		{
			point.x -= m_Move.x;
		}

		else if (pstUserSetInfo->dAngle == 0 && pstUserSetInfo->mirrorDir == eDirectionType::eHorizontal) //  좌우반전 0도
		{
			point.y -= m_Move.y;
		}
		else if (pstUserSetInfo->dAngle == 90 && pstUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 90도
		{
			point.x += m_Move.x;

		}
		else if (pstUserSetInfo->dAngle == 180 && pstUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 180도
		{
			point.y += m_Move.y;
		}
		else if (pstUserSetInfo->dAngle == 270 && pstUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 270도
		{
			point.x -= m_Move.x;
		}

		if (point.y == 0 && point.x == 0)
		{
			MessageBox(L"이동거리가 입력되지 않았습니다.", L"", MB_OK | MB_ICONQUESTION);
			SetEdit_Feature();
		}		

		Feature_Move_KeyBoard(point, eKeyDir::eDown);

	}
	break;
	case VK_RIGHT:
	{
		//BOOL bSelect = FALSE;

		//bSelect = IsSelect();

		//PointDXY point = CPointD(0, 0);

		if (pstUserSetInfo->dAngle == 0 && pstUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본0도
		{
			point.x += m_Move.x;
		}
		else if (pstUserSetInfo->dAngle == 90 && pstUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본90도 
		{
			point.y += m_Move.y;
		}
		else if (pstUserSetInfo->dAngle == 180 && pstUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본180도
		{
			point.x -= m_Move.x;
		}
		else if (pstUserSetInfo->dAngle == 270 && pstUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본270도
		{
			point.y -= m_Move.y;
		}

		else if (pstUserSetInfo->dAngle == 0 && pstUserSetInfo->mirrorDir == eDirectionType::eHorizontal) //  좌우반전 0도
		{
			point.x -= m_Move.x;
		}
		else if (pstUserSetInfo->dAngle == 90 && pstUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 90도
		{
			point.y -= m_Move.y;


		}
		else if (pstUserSetInfo->dAngle == 180 && pstUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 180도
		{
			point.x += m_Move.x;

		}
		else if (pstUserSetInfo->dAngle == 270 && pstUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 270도
		{
			point.y += m_Move.y;

		}

		if (point.y == 0 && point.x == 0)
		{
			MessageBox(L"이동거리가 입력되지 않았습니다.", L"", MB_OK | MB_ICONQUESTION);
			SetEdit_Feature();
		}
			
		 Feature_Move_KeyBoard(point, eKeyDir::eRight);

	}
	break;
	case VK_LEFT:
	{
		
		
		if (pstUserSetInfo->dAngle == 0 && pstUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본0도
		{
			point.x -= m_Move.x;
		}
		else if (pstUserSetInfo->dAngle == 90 && pstUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본90도 
		{
			point.y -= m_Move.y;
		}
		else if (pstUserSetInfo->dAngle == 180 && pstUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본180도
		{
			point.x += m_Move.x;
		}
		else if (pstUserSetInfo->dAngle == 270 && pstUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본270도
		{
			point.y += m_Move.y;
		}

		else if (pstUserSetInfo->dAngle == 0 && pstUserSetInfo->mirrorDir == eDirectionType::eHorizontal) //  좌우반전 0도
		{
			point.x += m_Move.x;
		}
		else if (pstUserSetInfo->dAngle == 90 && pstUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 90도
		{
			point.y += m_Move.y;


		}
		else if (pstUserSetInfo->dAngle == 180 && pstUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 180도
		{
			point.x -= m_Move.x;

		}
		else if (pstUserSetInfo->dAngle == 270 && pstUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 270도
		{
			point.y -= m_Move.y;

		}

		if (point.y == 0 && point.x == 0)
		{
			MessageBox(L"이동거리가 입력되지 않았습니다.", L"", MB_OK | MB_ICONQUESTION);
			SetEdit_Feature();
		}		
		
		Feature_Move_KeyBoard(point, eKeyDir::eLeft);

	}
	break;
	
	// 2022-06-30
	// 장동진 주석처리
	// '+' , '-' 입력으로 키보드 Move 시 Move 값 설정 가능 

// 	case VK_OEM_PLUS:
// 	{
// 		m_Move.x += 0.05;
// 		m_Move.y += 0.05;
// 		
// 	}
// 	break;
// 	case VK_OEM_MINUS:
// 	{
// 		m_Move.x -= 0.05;
// 		m_Move.y -= 0.05;
// 	}
// 	break;
	}



	CFormView::OnKeyDown(nChar, nRepCnt, nFlags);
}
