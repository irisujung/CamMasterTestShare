// FeatureEditFormView.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "FeatureEditFormView.h"
#include "MainFrm.h"


// FeatureEditFormView

IMPLEMENT_DYNCREATE(CFeatureEditFormView, CFormView)

CFeatureEditFormView::CFeatureEditFormView()
	: CFormView(IDD_EDIT_DLG)
{
	m_bInit = FALSE;
}

CFeatureEditFormView::~CFeatureEditFormView()
{
}

void CFeatureEditFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFeatureEditFormView, CFormView)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_COMMAND(IDC_SAVENAPPLY_BTN,&OnApplyBtnClick)
	ON_COMMAND(IDC_PREVIEW_BUTTON,&OnPreviewBtnClick)	
	ON_COMMAND(IDC_DELETE_BTN,&OnDeleteBtnClick)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, &CFeatureEditFormView::OnPropertyChanged)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// FeatureEditFormView 진단

#ifdef _DEBUG
void CFeatureEditFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CFeatureEditFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

// FeatureEditFormView 메시지 처리기

CFeatureEditFormView* CFeatureEditFormView::CreateOne(CWnd* pParent)
{
	CFeatureEditFormView* pFormView = new CFeatureEditFormView;
	CCreateContext* pContext = NULL;
	if (!pFormView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 300, 300), pParent, 0, pContext))
	{
		TRACE0("Failed in creating CFeatureEditFormView");
	}
	return pFormView;

}

void CFeatureEditFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	if (!m_bInit)
	{
		CRect rectDummy(0, 0, 0, 0);
		const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER;
		if (!m_PropGridCtrl.Create(dwViewStyle, rectDummy, this, IDC_EDIT_GRID))
		{
			TRACE0("Failed to create List Control \n");
			return;
		}

		m_PropGridCtrl.EnableHeaderCtrl(FALSE);
		m_PropGridCtrl.EnableDescriptionArea();
		m_PropGridCtrl.SetVSDotNetLook();
		m_PropGridCtrl.MarkModifiedProperties();
		m_PropGridCtrl.SetCustomColors(COLOR_GRAY1, COLOR_GRAY1, COLOR_GRAY1, COLOR_GREEN2, COLOR_GRAY1, COLOR_GRAY1, COLOR_GREEN1);

		m_btnApply.Create(L"Apply", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, IDC_SAVENAPPLY_BTN);
		m_btnPreview.Create(L"Preview", WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, IDC_PREVIEW_BUTTON);
		m_btnDelete.Create(L"Delete", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, IDC_DELETE_BTN);




		InitalizeFeatureWindow();
		//LoadFeatureData();


		
		m_bInit = TRUE;
		SetTimer(FEAUTRE_EDIT_LOAD_TIMER, FEATURE_EDIT_LOAD_TIMER_DELAY, NULL);
	}

}

UINT32 CFeatureEditFormView::InitalizeFeatureWindow()
{
	if (RESULT_GOOD != InitailzeResize())
		return RESULT_BAD;

	if (RESULT_GOOD != InitializeRotate())
		return RESULT_BAD;

	if (RESULT_GOOD != InitailizeMove())
		return RESULT_BAD;

// 	if (RESULT_GOOD != InitailizeShape())
// 		return RESULT_BAD;

	if (RESULT_GOOD != InitailizeCopy())
		return RESULT_BAD;

	if (RESULT_GOOD != InitailizeAttr())
		return RESULT_BAD;

	return RESULT_GOOD;
}






UINT32 CFeatureEditFormView::SetResizeRatio()
{
	if (m_pResize[0])
	{
		//CString strTemp = m_pResize[0]->GetValue().bstrVal;
		m_ResizeRatio = m_pResize[0]->GetValue().dblVal;
		m_ResizeRatio = m_ResizeRatio/100.f;
	}
	else
		return RESULT_BAD;

	return RESULT_GOOD;
}

void CFeatureEditFormView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	SetTimer(FEAUTRE_EDIT_LOAD_TIMER, FEATURE_EDIT_LOAD_TIMER_DELAY, NULL);
}

void CFeatureEditFormView::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case FEAUTRE_EDIT_LOAD_TIMER:
		KillTimer(FEAUTRE_EDIT_LOAD_TIMER);

		if (!m_bInit) return;

		CRect rectClient;
		GetClientRect(&rectClient);

		m_PropGridCtrl.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height() - 70, NULL);

		m_btnPreview.MoveWindow(rectClient.left, rectClient.Height() - 60 + 45 / 4, 120, 30);
		m_btnPreview.RedrawWindow();
	
		m_btnDelete.MoveWindow(rectClient.left+150 , rectClient.Height() - 60 + 45 / 4, 120, 30);
		m_btnDelete.RedrawWindow();

		m_btnApply.MoveWindow(rectClient.Width() - 120, rectClient.Height() - 60 + 45 / 4, 120, 30);
		m_btnApply.RedrawWindow();

		
	}
	
		CFormView::OnTimer(nIDEvent);
}



void CFeatureEditFormView::OnApplyBtnClick()
{
	bool bEditBox = IsResize();
	bool bMove = IsMove();

	if (AfxMessageBox(_T("적용하시겠습니까?"), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		if (m_bPreview == TRUE || bEditBox == TRUE || bMove == TRUE)
		{			
			SetTempData_Apply();
		}		
		else
		{
			SetResizeRatio();
			if (m_ResizeRatio != 1.f)
			{
				SetResize_Apply_SelectOnly(m_ResizeRatio);
			}
		
 			SetMirror();
 			SetRotate();
			if (m_nAngleMode != -1)
			{
				SetRotate_Apply(m_nAngleMode, m_bMirror);
			}
			
			SetMove();
			_SetCopy();			
		}	

		Reset();

		// 2022.06.07 
		// 김준호 주석추가
		// edit 후 measureinfo update 위해 추가 
		if (GetMachineType() == eMachineType::eNSIS)
		{
			Update_NSIS_MeasureData();			
		}
	}
	else
	{
		SetViewerEditMode(EditMode::enumMode_EditMode);
		Reset();		
		ResetSelect();
	}

	m_bPreview = FALSE;	
	
}

void CFeatureEditFormView::OnPreviewBtnClick()
{	
	PointDXY	fMoveXY = CPointD(0, 0);
	CPointD		dptCopyXY = CPointD(0, 0);
	CPointD		dptRepeatPitchXY = CPointD(0, 0);
	CPoint		ptRepeatCount = CPoint(0, 0);

	CString strTemp;
	if (m_pMove[0])
	{
		fMoveXY.x = m_pMove[0]->GetValue().dblVal;
	}
	if (m_pMove[1])
	{
		fMoveXY.y = m_pMove[1]->GetValue().dblVal;
	}

	SetResizeRatio();

	SetMirror();
	SetRotate();
	//SetParam();

	SetPreviewCopy(dptCopyXY, dptRepeatPitchXY, ptRepeatCount);

	/*CString strRepeat = m_pCopy[0]->GetValue().bstrVal;
	CString strTemp2;
	CString strTemp3;
	
	if (strRepeat == STRING_COPY_ONCE)
	{
		strTemp2 = m_pCopy[1]->GetValue().bstrVal;
		dptCopyXY.x = static_cast<double>(_ttof(strTemp2));
		strTemp2 = m_pCopy[2]->GetValue().bstrVal;
		dptCopyXY.y = static_cast<double>(_ttof(strTemp2));
		m_bRepeat = FALSE;
	}

	if (strRepeat == STRING_COPY_REPEAT)
	{
		strTemp3 = m_pCopy[1]->GetValue().bstrVal;
		dptRepeatPitchXY.x = static_cast<double>(_ttof(strTemp3));
		strTemp3 = m_pCopy[2]->GetValue().bstrVal;
		dptRepeatPitchXY.y = static_cast<double>(_ttof(strTemp3));
		strTemp3 = m_pCopy[3]->GetValue().bstrVal;
		ptRepeatCount.x = static_cast<LONG>(_ttof(strTemp3));
		strTemp3 = m_pCopy[4]->GetValue().bstrVal;
		ptRepeatCount.y = static_cast<LONG>(_ttof(strTemp3));
		m_bRepeat = TRUE;
	}*/
	
	Set_Preview(fMoveXY, m_ResizeRatio, m_nAngleMode, m_bMirror, dptCopyXY,dptRepeatPitchXY,ptRepeatCount,m_bRepeat, m_bCopy, m_dRadius,m_dWidth,m_dHeight ,m_strShape, m_dLineWidth);

	m_bPreview = TRUE;
	m_bCopy = FALSE;
	
}

void CFeatureEditFormView::OnDeleteBtnClick()
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

	
	

	//m_PropGridCtrl.ResetOriginalValues(TRUE);
 }

UINT32 CFeatureEditFormView::_InitailzeResize()
{
	VARIANT var;
	var.dblVal = 0.0;
	var.intVal = 0;
	var.fltVal = 0.f;
	m_pResize[0] = new CMFCPropertyGridProperty(STRING_RESIZE_PERCENT, var.dblVal, STRING_RESIZE_DESC);
	if (m_pResize)
	{

		m_pResize[0]->SetOriginalValue(100.0);
		m_pResize[0]->AllowEdit(TRUE);
		m_pResize[0]->Enable(TRUE);
		return RESULT_GOOD;
	}
	else
	{
		return RESULT_BAD;
	}
}

UINT32 CFeatureEditFormView::InitailzeResize()
{
	CMFCPropertyGridProperty* pResizeGroup = new CMFCPropertyGridProperty(STRING_RESIZE);
	if (pResizeGroup == nullptr) return RESULT_BAD;

	if (RESULT_GOOD == _InitailzeResize())
	{

		pResizeGroup->AddSubItem(m_pResize[0]);
	}
	else
	{
		return RESULT_BAD;
	}
	m_PropGridCtrl.AddProperty(pResizeGroup);
	pResizeGroup->Expand(TRUE);
	return RESULT_GOOD;
}

UINT32 CFeatureEditFormView::_InitailzeRotate()
{
	return RESULT_GOOD;
}
UINT32 CFeatureEditFormView::_InitailizeMirror()
{
	return RESULT_GOOD;
}

UINT32 CFeatureEditFormView::InitializeRotate()
{
	CMFCPropertyGridProperty* pRotateGroup = new CMFCPropertyGridProperty(STRING_ROTATE);
	if (pRotateGroup == nullptr) return RESULT_BAD;

	m_pRotate[0] = new CMFCPropertyGridProperty(STRING_ROTATE, L"", STRING_ROTATE_DESC);

	if (m_pRotate[0])
	{
		m_pRotate[0]->SetOriginalValue(_T("0"));
		m_pRotate[0]->AddOption(STRING_ROTATE_0);
		m_pRotate[0]->AddOption(STRING_ROTATE_90);
		m_pRotate[0]->AddOption(STRING_ROTATE_180);
		m_pRotate[0]->AddOption(STRING_ROTATE_270);
		m_pRotate[0]->AllowEdit(TRUE);
		m_pRotate[0]->Enable(TRUE);
		pRotateGroup->AddSubItem(m_pRotate[0]);
	}

	else
	{
		return RESULT_BAD;
	}

	m_pMirror[0] = new CMFCPropertyGridProperty(STRING_MIRROR, L"", STRING_MIRROR_DESC);

	if (m_pMirror[0])
	{
		m_pMirror[0]->SetOriginalValue(_T("No"));
		m_pMirror[0]->AddOption(STRING_MIRROR_NO);
		m_pMirror[0]->AddOption(STRING_MIRROR_YES);
		m_pMirror[0]->AllowEdit(TRUE);
		m_pMirror[0]->Enable(TRUE);
		pRotateGroup->AddSubItem(m_pMirror[0]);
	}
	else
	{
		return RESULT_BAD;
	}
	
	m_PropGridCtrl.AddProperty(pRotateGroup);
	pRotateGroup->Expand(TRUE);

	return RESULT_GOOD;
}

UINT32 CFeatureEditFormView::SetRotate()
{

	if (m_pRotate[0])
	{
		CString strTemp = m_pRotate[0]->GetValue().bstrVal;
		if(strTemp == STRING_ROTATE_0)
		{
			m_nAngleMode = 0;
		}
		if (strTemp == STRING_ROTATE_90)
		{
			m_nAngleMode = 1;
		}
		if (strTemp == STRING_ROTATE_180)
		{
			m_nAngleMode = 2;
		}
		if (strTemp == STRING_ROTATE_270)
		{
			m_nAngleMode = 3;
		}
	}
	else
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CFeatureEditFormView::SetMirror()
{
	if (m_pMirror[0])
	{
		CString strTemp = m_pMirror[0]->GetValue().bstrVal;
		if (strTemp == STRING_MIRROR_NO)
		{
			m_bMirror = FALSE;
		}
		if (strTemp == STRING_MIRROR_YES)
		{
			m_bMirror = TRUE;
		}
	}
	else 
		return RESULT_BAD;

	return RESULT_GOOD;
}


UINT32 CFeatureEditFormView::SetPreviewCopy(CPointD &dptCopyXY, CPointD &dptRepeatPitchXY, CPoint &ptRepeatCount)
{
	CString strRepeat = m_pCopy[0]->GetValue().bstrVal;
	CString strTemp2;
	CString strTemp3;

	if (strRepeat == STRING_COPY_ONCE)
	{
		strTemp2 = m_pCopy[1]->GetValue().bstrVal;
		dptCopyXY.x = static_cast<double>(_ttof(strTemp2));
		strTemp2 = m_pCopy[2]->GetValue().bstrVal;
		dptCopyXY.y = static_cast<double>(_ttof(strTemp2));
		m_bRepeat = FALSE;
		m_bCopy = TRUE;

	}
	else if (strRepeat == STRING_COPY_REPEAT)
	{
		strTemp3 = m_pCopy[1]->GetValue().bstrVal;
		dptRepeatPitchXY.x = static_cast<double>(_ttof(strTemp3));
		strTemp3 = m_pCopy[2]->GetValue().bstrVal;
		dptRepeatPitchXY.y = static_cast<double>(_ttof(strTemp3));
		strTemp3 = m_pCopy[3]->GetValue().bstrVal;
		ptRepeatCount.x = static_cast<LONG>(_ttof(strTemp3));
		strTemp3 = m_pCopy[4]->GetValue().bstrVal;
		ptRepeatCount.y = static_cast<LONG>(_ttof(strTemp3));
		m_bRepeat = TRUE;
		m_bCopy = TRUE;
	}
	else
		return RESULT_BAD;

	return RESULT_GOOD;
}


UINT32 CFeatureEditFormView::InitailizeMove()
{
	VARIANT var;
	var.dblVal = 0.0;
	var.intVal = 0;
	var.fltVal = 0.f;
	
	VARIANT varMove;
	varMove.dblVal = 0.00;

	CMFCPropertyGridProperty *pMoveGroup = new CMFCPropertyGridProperty(STRING_MOVE);
	if (pMoveGroup == nullptr) { return RESULT_BAD; }


	m_pMove[0] = new CMFCPropertyGridProperty(STRING_MOVE_X, var.dblVal, STRING_MOVE_DESC);
	if (m_pMove[0])
	{
		m_pMove[0]->SetOriginalValue(0.0);
		m_pMove[0]->AllowEdit(TRUE);
		m_pMove[0]->Enable(TRUE);
		pMoveGroup->AddSubItem(m_pMove[0]);
	}
	else
	{
		return RESULT_BAD;
	}
	m_pMove[1] = new CMFCPropertyGridProperty(STRING_MOVE_Y, var.dblVal, STRING_MOVE_DESC);
	if (m_pMove[1])
	{
		m_pMove[1]->SetOriginalValue(0.0);
		m_pMove[1]->AllowEdit(TRUE);
		m_pMove[1]->Enable(TRUE);
		pMoveGroup->AddSubItem(m_pMove[1]);
	}
	
	m_pMove[2] = new CMFCPropertyGridProperty(STRING_MOVE_VALUE, var.dblVal, STRING_MOVE_VALUE);
	if (m_pMove[2])
	{
		m_pMove[2]->SetValue(varMove.dblVal);
		m_pMove[2]->AllowEdit(TRUE);
		m_pMove[2]->Enable(TRUE);
		pMoveGroup->AddSubItem(m_pMove[2]);

	}
	else
	{
		return RESULT_BAD;
	}

	


	m_PropGridCtrl.AddProperty(pMoveGroup);
	pMoveGroup->Expand(TRUE);

	return RESULT_GOOD;
}



UINT32 CFeatureEditFormView::MovePreview()
{
	PointDXY fMoveXY = CPointD(0, 0);
	CString strTemp;
	if (m_pMove[0])
	{
		fMoveXY.x = m_pMove[0]->GetValue().dblVal;
		
	}
	if (m_pMove[1])
	{
		fMoveXY.y = m_pMove[0]->GetValue().dblVal;

	}

	ManualMove(fMoveXY);

	return RESULT_GOOD;
}


UINT32 CFeatureEditFormView::SetMove()
{

	PointDXY fMoveXY = CPointD(0, 0);
	CString strTemp;
	if (m_pMove[0])
	{
		
		fMoveXY.x = m_pMove[0]->GetValue().dblVal;
	}
	if (m_pMove[1])
	{
		fMoveXY.y = m_pMove[1]->GetValue().dblVal;
	}
	
	if (fMoveXY.x != 0.f || fMoveXY.y != 0.f)
	{
		AddApplicationLog(APPLY_EDIT_MOVE_TXT);
		if (m_bDragMode == TRUE)
		{
			SetMove_Apply();	
		}
		else
		{
			ManualMove(fMoveXY);
			SetMove_Apply();
		}
	}

	
	
	return RESULT_GOOD;
}

UINT32 CFeatureEditFormView::InitailizeCopy()
{
	CMFCPropertyGridProperty *pCopyGroup = new CMFCPropertyGridProperty(STRING_COPY);

	if (pCopyGroup == nullptr) { return RESULT_BAD; }

	m_pCopy[0] = new CMFCPropertyGridProperty(STRING_COPY_TIME, L"", STRING_COPY_DESC);
	if (m_pCopy[0])
	{
		m_pCopy[0]->AddOption(STRING_COPY_ONCE);
		m_pCopy[0]->AddOption(STRING_COPY_REPEAT);


		m_pCopy[0]->AllowEdit(TRUE);
		m_pCopy[0]->Enable(TRUE);
		pCopyGroup->AddSubItem(m_pCopy[0]);
	}
	m_pCopy[1] = new CMFCPropertyGridProperty(STRING_COPY_PITCH_X,L"",L"");
	if (m_pCopy[1])
	{
		m_pCopy[1]->AllowEdit(TRUE);
		m_pCopy[1]->Enable(FALSE);
		pCopyGroup->AddSubItem(m_pCopy[1]);
	}
	m_pCopy[2] = new CMFCPropertyGridProperty(STRING_COPY_PITCH_Y,L"", L"");
	if (m_pCopy[2])
	{
		m_pCopy[2]->AllowEdit(TRUE);
		m_pCopy[2]->Enable(FALSE);
		pCopyGroup->AddSubItem(m_pCopy[2]);
	}
	m_pCopy[3] = new CMFCPropertyGridProperty(STRING_COPY_REPEAT_COUNT_X, L"", L"");
	if (m_pCopy[3])
	{
		m_pCopy[3]->AllowEdit(TRUE);
		m_pCopy[3]->Enable(FALSE);
		pCopyGroup->AddSubItem(m_pCopy[3]);
	}
	m_pCopy[4] = new CMFCPropertyGridProperty(STRING_COPY_REPEAT_COUNT_Y, L"", L"");
	if (m_pCopy[4])
	{
		m_pCopy[4]->AllowEdit(TRUE);
		m_pCopy[4]->Enable(FALSE);
		pCopyGroup->AddSubItem(m_pCopy[4]);
	}

	else
	{
		return RESULT_BAD;
	}

	m_PropGridCtrl.AddProperty(pCopyGroup);
	pCopyGroup->Expand(TRUE);
	

	return RESULT_GOOD;
}



UINT32 CFeatureEditFormView::_SetCopy()
{

	CPointD dptCopyXY = CPointD(0, 0);
	CPointD dptRepeatPitchXY = CPointD(0, 0);
	CPoint ptRepeatCount = CPoint(0, 0);

	CString strRepeat = m_pCopy[0]->GetValue().bstrVal;
	CString strTemp;
	if (m_pCopy[0])
	{
		if (strRepeat == STRING_COPY_ONCE)
		{
			strTemp = m_pCopy[1]->GetValue().bstrVal;
			dptCopyXY.x = static_cast<double>(_ttof(strTemp));
			strTemp = m_pCopy[2]->GetValue().bstrVal;
			dptCopyXY.y = static_cast<double>(_ttof(strTemp));
			
			if (strTemp != _T(""))
			{
				AddApplicationLog(APPLY_EDIT_COPY_TXT);
				SetCopy_Apply(dptCopyXY);
			}
		}
		if (strRepeat == STRING_COPY_REPEAT)
		{
			strTemp = m_pCopy[1]->GetValue().bstrVal;
			dptRepeatPitchXY.x = static_cast<double>(_ttof(strTemp));
			strTemp = m_pCopy[2]->GetValue().bstrVal;
			dptRepeatPitchXY.y = static_cast<double>(_ttof(strTemp));
			strTemp = m_pCopy[3]->GetValue().bstrVal;
			ptRepeatCount.x = static_cast<LONG>(_ttof(strTemp));
			strTemp = m_pCopy[4]->GetValue().bstrVal;
			ptRepeatCount.y = static_cast<LONG>(_ttof(strTemp));
			if (strTemp != _T(""))
			{
				AddApplicationLog(APPLY_EDIT_COPY_TXT);
				SetCopy_Repeat_Apply(dptRepeatPitchXY, ptRepeatCount);
			}

		}
	}

	else
	{
		return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 CFeatureEditFormView::CopyPreview()
{
	CPointD									dptCopyXY = CPointD(0, 0);
	CPointD									dptRepeatPitchXY = CPointD(0, 0);
	CPoint									ptRepeatCount = CPoint(0, 0);

	CString strRepeat = m_pCopy[0]->GetValue().bstrVal;
	CString strTemp;
	if (m_pCopy[0])
	{
		if (strRepeat == STRING_COPY_ONCE)
		{
			strTemp = m_pCopy[1]->GetValue().bstrVal;
			dptCopyXY.x = static_cast<double>(_ttof(strTemp));
			strTemp = m_pCopy[2]->GetValue().bstrVal;
			dptCopyXY.y = static_cast<double>(_ttof(strTemp));

			SetCopy(dptCopyXY);
		}
		if (strRepeat == STRING_COPY_REPEAT)
		{
			strTemp = m_pCopy[1]->GetValue().bstrVal;
			dptRepeatPitchXY.x = static_cast<double>(_ttof(strTemp));
			strTemp = m_pCopy[2]->GetValue().bstrVal;
			dptRepeatPitchXY.y = static_cast<double>(_ttof(strTemp));
			strTemp = m_pCopy[3]->GetValue().bstrVal;
			ptRepeatCount.x = static_cast<LONG>(_ttof(strTemp));
			strTemp = m_pCopy[4]->GetValue().bstrVal;
			ptRepeatCount.y = static_cast<LONG>(_ttof(strTemp));
			SetCopy_Repeat(dptRepeatPitchXY, ptRepeatCount);
		}
	}

	else
	{
		return RESULT_BAD;
	}

	return RESULT_GOOD;
}
;
UINT32 CFeatureEditFormView::InitailizeShape()
{

// 	VARIANT var;
// 	var.dblVal = 0.0;
// 	
// 
// 	CMFCPropertyGridProperty *pShapeGroup = new CMFCPropertyGridProperty(STRING_EIDT_SHPAE);
// 
// 	m_pShape[0] = new CMFCPropertyGridProperty(STRING_RADIUS, var.dblVal, L"");
// 	if (m_pShape[0])
// 	{
// 		m_pShape[0]->SetOriginalValue(0.0);
// 		m_pShape[0]->Enable(TRUE);
// 		m_pShape[0]->AllowEdit(TRUE);
// 		pShapeGroup->AddSubItem(m_pShape[0]);
// 	}
// 	
// 	m_pShape[1] = new CMFCPropertyGridProperty(STRING_WIDTH, var.dblVal, L"");
// 	if (m_pShape[1])
// 	{
// 		m_pShape[1]->SetOriginalValue(0.0);
// 		m_pShape[1]->Enable(TRUE);
// 		m_pShape[1]->AllowEdit(TRUE);
// 		pShapeGroup->AddSubItem(m_pShape[1]);
// 	}
// 
// 	m_pShape[2] = new CMFCPropertyGridProperty(STRING_HEIGHT, var.dblVal, L"");
// 	if (m_pShape[2])
// 	{
// 		m_pShape[2]->SetOriginalValue(0.0);
// 		m_pShape[2]->Enable(TRUE);
// 		m_pShape[2]->AllowEdit(TRUE);
// 		pShapeGroup->AddSubItem(m_pShape[2]);
// 	}
// 	m_pShape[3] = new CMFCPropertyGridProperty(STRING_SHAPE, L"", L"");
// 	{
// 		m_pShape[3]->SetOriginalValue(_T(""));
// 		m_pShape[3]->Enable(TRUE);
// 		m_pShape[3]->AllowEdit(TRUE);
// 		m_pShape[3]->AddOption(STRING_SHAPE_ROUND);
// 		m_pShape[3]->AddOption(STRING_SHAPE_SQUARE);
// 		pShapeGroup->AddSubItem(m_pShape[3]);
// 	}
// 
// 	m_pShape[4] = new CMFCPropertyGridProperty(STRING_WIDTH, var.dblVal, L"");
// 	{
// 		m_pShape[4]->SetOriginalValue(0.0);
// 		m_pShape[4]->Enable(TRUE);
// 		m_pShape[4]->AllowEdit(TRUE);
// 		pShapeGroup->AddSubItem(m_pShape[4]);
// 	}
// 
// 
// 	m_PropGridCtrl.AddProperty(pShapeGroup);
// 	pShapeGroup->Expand(TRUE);
// 
 	return RESULT_GOOD;

}

UINT32 CFeatureEditFormView::SetParam()
{
// 	if (m_pShape[0])
// 	{
// 		m_dRadius = m_pShape[0]->GetValue().dblVal;
// 	}
// 	if (m_pShape[1])
// 	{
// 		m_dWidth = m_pShape[1]->GetValue().dblVal;
// 	}
// 	if (m_pShape[2])
// 	{
// 		m_dHeight = m_pShape[2]->GetValue().dblVal;
// 	}
// 
// 	if (m_pShape[3])
// 	{
// 		m_strShape = m_pShape[3]->GetValue().bstrVal;
// 	}
// 
// 	if (m_pShape[4])
// 	{
// 		m_dLineWidth = m_pShape[4]->GetValue().dblVal;
// 	}

	//_SetShape(m_dRadius, m_dWidth, m_dHeight);

	return RESULT_GOOD;
}

UINT32 CFeatureEditFormView::InitailizeAttr()
{
// 	CMFCPropertyGridProperty *pAttrGroup = new CMFCPropertyGridProperty(STRING_EDIT_ATTRIBUTE);
// 
// 	m_pAttr[0] = new CMFCPropertyGridProperty(STRING_EDIT_ATTRIBUTE, L"", L"");
// 	if (m_pAttr[0])
// 	{
// 		m_pAttr[0]->SetOriginalValue(_T(""));
// 		m_pAttr[0]->AddOption(STRING_ATTRIBUTE_NAME);
// 		m_pAttr[0]->Enable(TRUE);
// 		m_pAttr[0]->AllowEdit(TRUE);
// 		pAttrGroup->AddSubItem(m_pAttr[0]);
// 	}
// 
// 	m_pAttr[1] = new CMFCPropertyGridProperty(STRING_EDIT_ATTRIBUTE_STRING, L"", L"");
// 	if (m_pAttr[1])
// 	{
// 		m_pAttr[1]->SetOriginalValue(_T(""));
// 		m_pAttr[1]->AddOption(STRING_ATTRIBUTE_STRING_FM_P);
// 		m_pAttr[1]->AddOption(STRING_ATTRIBUTE_STRING_TOP);
// 		m_pAttr[1]->AddOption(STRING_ATTRIBUTE_STRING_BOL);
// 		m_pAttr[1]->AddOption(STRING_ATTRIBUTE_STRING_BALL);
// 		m_pAttr[1]->AddOption(STRING_ATTRIBUTE_STRING_ALIGN);
// 		m_pAttr[1]->Enable(TRUE);
// 		m_pAttr[1]->AllowEdit(TRUE);
// 		pAttrGroup->AddSubItem(m_pAttr[1]);
// 	}
// 
// 	m_PropGridCtrl.AddProperty(pAttrGroup);
// 	pAttrGroup->Expand(TRUE);
// 
	return RESULT_GOOD;

}

UINT32 CFeatureEditFormView::SetAttribute()
{
// 	if (m_pAttr[0])
// 
// 	{
// 		m_strAttributeName = m_pAttr[0]->GetValue().bstrVal;
// 	}
// 	
// 	if (m_pAttr[1])
// 
// 	{
// 		m_strAttributeString = m_pAttr[1]->GetValue().bstrVal;
// 	}
// 
// 	if (m_strAttributeName != _T("") && m_strAttributeString != _T(""))
// 	{
// 		CString strLog;
// 		strLog.Format(_T("%s [Attribute : %s, Attribute String : %s]"), APPLY_EDIT_ATTRIBUTE_TXT, m_strAttributeName, m_strAttributeString);
// 		AddApplicationLog(strLog);
// 		SetAttribute_Apply(m_strAttributeName, m_strAttributeString);
// 	}
	

	return RESULT_GOOD;
}

UINT32 CFeatureEditFormView::SetMoveValue_Key(FeatureSpec_t* pFeatureSpec)
{
	m_dMouseMove_X = pFeatureSpec->dMoveX;
	m_dMouseMove_Y = pFeatureSpec->dMoveY;
	m_dKeyMove_X = pFeatureSpec->dMoveX_Key;
	m_dKeyMove_Y = pFeatureSpec->dMoveY_Key;

	m_dTotal_X = m_dMouseMove_X + m_dKeyMove_X;
	m_dTotal_Y = m_dMouseMove_Y + m_dKeyMove_Y;

	m_pMove[0]->SetValue(m_dTotal_X);
	m_pMove[1]->SetValue(m_dTotal_Y);

	m_bWheelMode = pFeatureSpec->bWheelMode;
	m_bDragMode = pFeatureSpec->bDragMode;
	m_bKeyMode = pFeatureSpec->bKeyMode;

	m_btnPreview.EnableWindow(TRUE);

	if (m_bWheelMode || m_bDragMode || m_bKeyMode)
		m_btnPreview.EnableWindow(FALSE);
	else
		m_btnPreview.EnableWindow(TRUE);

	return RESULT_GOOD;
}

UINT32 CFeatureEditFormView::SetMoveValue(FeatureSpec_t* pFeatureSpec)
{

	m_dMouseMove_X = pFeatureSpec->dMoveX;
	m_dMouseMove_Y = pFeatureSpec->dMoveY;
	m_dKeyMove_X = pFeatureSpec->dMoveX_Key;
	m_dKeyMove_Y = pFeatureSpec->dMoveY_Key;

	m_dTotal_X = m_dMouseMove_X + m_dKeyMove_X;
	m_dTotal_Y = m_dMouseMove_Y + m_dKeyMove_Y;

	m_pMove[0]->SetValue(m_dTotal_X);
	m_pMove[1]->SetValue(m_dTotal_Y);

	m_bWheelMode = pFeatureSpec->bWheelMode;
	m_bDragMode = pFeatureSpec->bDragMode;
	m_bKeyMode = pFeatureSpec->bKeyMode;

	m_btnPreview.EnableWindow(TRUE);

	if (m_bWheelMode || m_bDragMode || m_bKeyMode)
		m_btnPreview.EnableWindow(FALSE);
	else
		m_btnPreview.EnableWindow(TRUE);

	return RESULT_GOOD;
}



UINT32 CFeatureEditFormView::SetFeatureSpec(FeatureSpec_t* pFeatureSpec)
{
	//m_bPreview = pFeatureSpec->bPreview;

	

	//m_PropGridCtrl.ResetOriginalValues(TRUE);

	//m_btnPreview.Create(L"Preview", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, IDC_PREVIEW_BUTTON);

	m_btnPreview.EnableWindow(TRUE);

	m_bWheelMode = pFeatureSpec->bWheelMode;
	m_bDragMode = pFeatureSpec->bDragMode;

	m_btnPreview.EnableWindow(TRUE);
	if (m_bWheelMode || m_bDragMode)
		m_btnPreview.EnableWindow(FALSE);
	else
		m_btnPreview.EnableWindow(TRUE);

	m_pCopy[0]->SetValue(_T(""));
	m_pCopy[1]->SetValue(_T(""));
	m_pCopy[2]->SetValue(_T(""));
	m_pCopy[3]->SetValue(_T(""));
	m_pCopy[4]->SetValue(_T(""));

	

	m_pResize[0]->SetValue(floor(static_cast<double>(pFeatureSpec->dRatio * 100)));

	if (pFeatureSpec->strFeature == _T("Pad"))
	{
// 		if (pFeatureSpec->nShape == 0)
// 		{
// 			m_pShape[0]->Show(TRUE);
// 			m_pShape[0]->SetValue((_variant_t)static_cast<double>(pFeatureSpec->dRadius));
// 			m_pShape[1]->Show(FALSE);
// 			m_pShape[2]->Show(FALSE);
// 			m_pShape[3]->Show(FALSE);
// 			m_pShape[4]->Show(FALSE);
// 
// 		}
// 
// 		if (pFeatureSpec->nShape == 2)
// 		{
// 			m_pShape[0]->Show(FALSE);
// 			m_pShape[1]->Show(TRUE);
// 			m_pShape[2]->Show(TRUE);
// 			m_pShape[3]->Show(FALSE);
// 			m_pShape[4]->Show(FALSE);
// 			m_pShape[1]->SetValue((_variant_t)static_cast<double>(pFeatureSpec->dWidth));
// 			m_pShape[2]->SetValue((_variant_t)static_cast<double>(pFeatureSpec->dHeight));
// 		}

		m_pResize[0]->SetValue(floor(static_cast<double>(pFeatureSpec->dRatio * 100)));
		m_pRotate[0]->Show(TRUE);
		m_pMirror[0]->Show(TRUE);
		m_pRotate[0]->SetValue(pFeatureSpec->strRotate);
		m_pMirror[0]->SetValue(pFeatureSpec->strMirror);
	}

	
	else if (pFeatureSpec->strFeature == _T("Line"))

	{
		m_pResize[0]->SetValue(floor(static_cast<double>(pFeatureSpec->dRatio * 100)));

		m_pRotate[0]->Show(FALSE);
		m_pMirror[0]->Show(FALSE);
// 		m_pShape[0]->Show(FALSE);
// 		m_pShape[1]->Show(FALSE);
// 		m_pShape[2]->Show(FALSE);
// 		m_pShape[3]->Show(TRUE);
// 		m_pShape[4]->Show(TRUE);
// 
// 		m_pShape[4]->SetValue((_variant_t)static_cast<double>(pFeatureSpec->dWidth));
// 
// 		if (pFeatureSpec->nShape == 0)
// 		{
// 			m_pShape[3]->SetValue(_T("Round"));
// 			
// 		}
// 		if (pFeatureSpec->nShape == 1)
// 		{
// 			m_pShape[3]->SetValue(_T("Sqaure"));
// 
// 		}
	}

	else if (pFeatureSpec->strFeature == _T("Surface"))
	{
		m_pResize[0]->SetValue(floor(static_cast<double>(pFeatureSpec->dRatio * 100)));

		m_pRotate[0]->Show(FALSE);
		m_pMirror[0]->Show(FALSE);
// 		m_pShape[0]->Show(FALSE);
// 		m_pShape[1]->Show(FALSE);
// 		m_pShape[2]->Show(FALSE);
// 		m_pShape[3]->Show(FALSE);

	}

	else if (pFeatureSpec->strFeature == _T("Arc"))
	{
		
		

		m_pRotate[0]->Show(FALSE);
		m_pMirror[0]->Show(FALSE);
// 		m_pRotate[0]->Show(FALSE);
// 		m_pMirror[0]->Show(FALSE);
// 		m_pShape[0]->Show(FALSE);
// 		m_pShape[1]->Show(FALSE);
// 		m_pShape[2]->Show(FALSE);
// 		m_pShape[3]->Show(FALSE);
// 		m_pShape[4]->Show(FALSE);
	}

// 	if (m_pAttr[0])
// 	{
// 		m_pAttr[0]->SetValue(pFeatureSpec->strAttr);
// 	}
// 
// 	if (m_pAttr[1])
// 	{
// 		m_pAttr[1]->SetValue(pFeatureSpec->strAttrString);
// 	}
	else
		return RESULT_BAD;

	//pFeatureSpec->Clear();

	return RESULT_GOOD;
}

UINT32 CFeatureEditFormView::LoadFeatureData()
{
	FeatureSpec_t*pFeatureSpec = GetFeatureSpec();
	
	SetFeatureSpec(pFeatureSpec);	

	return RESULT_GOOD;
}

UINT32 CFeatureEditFormView::LoadMoveData()
{
	FeatureSpec_t*pFeatureSpec = GetFeatureSpec();

	SetMoveValue(pFeatureSpec);

	return RESULT_GOOD;
}

UINT32 CFeatureEditFormView::LoadMove_KeyData()
{
	FeatureSpec_t*pFeatureSpec = GetFeatureSpec();

	SetMoveValue_Key(pFeatureSpec);

	return RESULT_GOOD;
}

LRESULT CFeatureEditFormView::OnPropertyChanged(WPARAM wp, LPARAM lp)
{

	PointDXY Move_Key = CPointD(0, 0);
	double MoveValue = m_pMove[2]->GetValue().dblVal;
	Move_Key.x += MoveValue;
	Move_Key.y += MoveValue;

	GetMoveValue(Move_Key);

	
	double MoveValueX = m_pMove[0]->GetValue().dblVal;
	double MoveValueY = m_pMove[1]->GetValue().dblVal;

	if (MoveValueX != 0.0 || MoveValueY != 0.0)
	{
		m_pCopy[0]->Enable(FALSE);
		m_pCopy[1]->Enable(FALSE);
		m_pCopy[2]->Enable(FALSE);
		m_pCopy[3]->Enable(FALSE);
		m_pCopy[4]->Enable(FALSE);
	}

	if (MoveValueX == 0.0 && MoveValueY == 0.0)
	{
		m_pCopy[0]->Enable(TRUE);
		/*m_pCopy[1]->Enable(TRUE);
		m_pCopy[2]->Enable(TRUE);
		m_pCopy[3]->Enable(TRUE);
		m_pCopy[4]->Enable(TRUE);*/
	}

	CString strRepeat = m_pCopy[0]->GetValue().bstrVal;
	if (m_pCopy[0])
	{
		if (strRepeat == STRING_COPY_ONCE)
		{
			m_pCopy[1]->Enable(TRUE);
			m_pCopy[2]->Enable(TRUE);
			m_pCopy[3]->Enable(FALSE);
			m_pCopy[4]->Enable(FALSE);
		}
		if (strRepeat == STRING_COPY_REPEAT)
		{
			m_pCopy[1]->Enable(TRUE);
			m_pCopy[2]->Enable(TRUE);
			m_pCopy[3]->Enable(TRUE);
			m_pCopy[4]->Enable(TRUE);
		}
	}

	//SetFeatureSpec(pFeatureSpec);

	return 0;

}


