// CamInfoFormView.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "CamInfoFormView.h"
#include "MainFrm.h"
#include <algorithm>

#define GRID_DEFAULT_COLUMN_WIDTH			50
#define GRID_STEP_COLUMN_WIDTH				120
#define GRID_LAYER_COLUMN_WIDTH				150
#define GRID_CMP_COLUMN_WIDTH				300

#define GRID_STEPNAME_COLUMN				0
#define GRID_STEPDX_COLUMN					1
#define GRID_STEPDY_COLUMN					2
#define GRID_STEPANGLE_COLUMN				3
#define GRID_STEPMIRROR_COLUMN				4

#define GRID_LAYERNO_COLUMN					0 
#define GRID_LAYERNAME_COLUMN				1
#define GRID_LAYERCONTEXT_COLUMN			2
#define GRID_LAYERTYPE_COLUMN				3
#define GRID_LAYERPOLARITY_COLUMN			4

#define GRID_CMPNO_COLUMN					0
#define GRID_CMPNAME_COLUMN					1
#define GRID_CMPTYPE_COLUMN					2
#define GRID_CMPPINNUM_COLUMN				3

#define IDC_STEP_BUTTON_FIRST				60011
#define IDC_STEP_BUTTON_LAST				(IDC_STEP_BUTTON_FIRST + MAX_BTN_STEP_NUM - 1)

// CCamInfoFormView

IMPLEMENT_DYNCREATE(CCamInfoFormView, CFormView)

CCamInfoFormView::CCamInfoFormView()
	: CFormView(IDD_CAMINFO_FORM)
{
	m_bInit = FALSE;
	m_bStepBtnCreated = FALSE;
	m_nStepNum = MIN_BTN_STEP_NUM;
	m_nLayerNum = 0;
	m_nSelectedStep = -1;
}

bool SortAscending(stLayerInfo* a, stLayerInfo* b)
{
	return a->strName < b->strName;
}

void CCamInfoFormView::ClearStepButtons()
{
	for (auto it : m_vcSteps)
	{
		delete it;
		it = nullptr;
	}
	m_vcSteps.clear();

	for (auto it : m_vcStepInfo)
	{
		if (it)
		{
			it->Clear();
			delete it;
			it = nullptr;
		}
	}
	m_vcStepInfo.clear();

	m_nStepNum = 0;
	m_nSelectedStep = -1;
	m_bStepBtnCreated = FALSE;
}

void CCamInfoFormView::ClearLayerInfo()
{
	for (auto it : m_vcLayerInfo)
	{
		delete it;
		it = nullptr;
	}
	m_vcLayerInfo.clear();	

	m_nLayerNum = 0;
}

void CCamInfoFormView::ClearComponentInfo()
{
	for (auto it : m_vcCompInfo)
	{
		if (it)
		{
			delete it;
			it = nullptr;
		}
	}
	m_vcCompInfo.clear();	
}

CCamInfoFormView::~CCamInfoFormView()
{
	ClearStepButtons();	
	ClearLayerInfo();
	ClearComponentInfo();

	if (m_StepList.GetSafeHwnd())
		m_StepList.DeleteAllItems();

	if (m_LayerList.GetSafeHwnd())
		m_LayerList.DeleteAllItems();

	if (m_CompList.GetSafeHwnd())
		m_CompList.DeleteAllItems();	
}

void CCamInfoFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_STEP, m_StepList);
	DDX_Control(pDX, IDC_LIST_LAYER, m_LayerList);
	DDX_Control(pDX, IDC_LIST_COMP, m_CompList);
}

BEGIN_MESSAGE_MAP(CCamInfoFormView, CFormView)	
	ON_WM_SIZE()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_STEP_BUTTON_FIRST, IDC_STEP_BUTTON_LAST, OnStepButtonClicked)
END_MESSAGE_MAP()


// CCamInfoFormView 진단

#ifdef _DEBUG
void CCamInfoFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CCamInfoFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CCamInfoFormView 메시지 처리기
CCamInfoFormView* CCamInfoFormView::CreateOne(CWnd* pParent)
{
	CCamInfoFormView* pFormView = new CCamInfoFormView;
	CCreateContext* pContext = NULL;

	if (!pFormView->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 100, 100), pParent, 0, pContext))
	{
		TRACE0("Failed in creating CLayerInfoFormView");
	}
	return pFormView;
}

UINT32 CCamInfoFormView::CreateStepButtons()
{
	if (m_nStepNum <= 0)
		return RESULT_BAD;

	TCHAR szPath[_MAX_PATH + 1];
	GetModuleFileName(AfxGetApp()->m_hInstance, szPath, _MAX_PATH);

	CString strCurAppDir = szPath;
	int nLen = strCurAppDir.ReverseFind('\\');
	if (nLen > 0)
		strCurAppDir = strCurAppDir.Left(nLen);

	CString strImg = L"";
	SIZE szIcon;
	szIcon.cx = szIcon.cy = 16;

	for (auto it : m_vcSteps)
	{
		delete it;
		it = nullptr;
	}
	m_vcSteps.clear();

	for (UINT32 i = 0; i < m_nStepNum; i++)
	{
		CButtonEx* pBtn = new CButtonEx();

		// 2022.06.22
		// 김준호 주석추가
		// ICon 변경위해 주석처리
		/*switch (i)
		{
		case 0:	strImg = UNIT_STEP_ICON;	break;
		case 1:	strImg = OE_STEP_ICON;		break;
		case 2:	strImg = SHEET_STEP_ICON;	break;
		case 3:	strImg = PANEL_STEP_ICON;	break;
		default:
			strImg = PANEL_STEP_ICON;
		}*/


		pBtn->SetImages(strCurAppDir + BUTTON_PRESSED_IMAGE, 
						strCurAppDir + BUTTON_NORMAL_IMAGE,
						strCurAppDir + BUTTON_PRESSED_IMAGE, 
						strCurAppDir + strImg, szIcon);
		pBtn->SetFontSize(13);
		pBtn->SetFontColor(BLACK_COLOR, BLACK_COLOR);

		m_vcSteps.emplace_back(pBtn);		
	}
	
	_UpdateUI_Pos();

	return RESULT_GOOD;
}

void CCamInfoFormView::OnInitialUpdate()
{
	if (m_bInit)
		return;

	CFormView::OnInitialUpdate();

	CreateStepButtons();
	
	m_StepHeaderCtrl.SubclassWindow(m_StepList.GetHeaderCtrl()->m_hWnd);
	m_LayerHeaderCtrl.SubclassWindow(m_LayerList.GetHeaderCtrl()->m_hWnd);
	m_CompHeaderCtrl.SubclassWindow(m_CompList.GetHeaderCtrl()->m_hWnd);

	m_StepList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_StepList.SetBkColor(WHITE_COLOR);	

	m_StepList.InsertColumn(GRID_STEPNAME_COLUMN, _T("Step Name"), LVCFMT_LEFT, GRID_STEP_COLUMN_WIDTH);
	m_StepList.InsertColumn(GRID_STEPDX_COLUMN, _T("NX"), LVCFMT_CENTER, GRID_STEP_COLUMN_WIDTH);
	m_StepList.InsertColumn(GRID_STEPDY_COLUMN, _T("NY"), LVCFMT_CENTER, GRID_STEP_COLUMN_WIDTH);
	m_StepList.InsertColumn(GRID_STEPANGLE_COLUMN, _T("ANGLE"), LVCFMT_CENTER, GRID_STEP_COLUMN_WIDTH);
	m_StepList.InsertColumn(GRID_STEPMIRROR_COLUMN, _T("MIRROR"), LVCFMT_CENTER, GRID_STEP_COLUMN_WIDTH);

	m_LayerList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_LayerList.SetBkColor(WHITE_COLOR);	

	m_LayerList.InsertColumn(GRID_LAYERNO_COLUMN, _T("No"), LVCFMT_CENTER, GRID_DEFAULT_COLUMN_WIDTH);
	m_LayerList.InsertColumn(GRID_LAYERNAME_COLUMN, _T("Layer Name"), LVCFMT_CENTER, GRID_LAYER_COLUMN_WIDTH);
	m_LayerList.InsertColumn(GRID_LAYERCONTEXT_COLUMN, _T("Context"), LVCFMT_CENTER, GRID_LAYER_COLUMN_WIDTH);
	m_LayerList.InsertColumn(GRID_LAYERTYPE_COLUMN, _T("Type"), LVCFMT_CENTER, GRID_LAYER_COLUMN_WIDTH);
	m_LayerList.InsertColumn(GRID_LAYERPOLARITY_COLUMN, _T("Polarity"), LVCFMT_CENTER, GRID_LAYER_COLUMN_WIDTH);

	/*m_CompList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_CompList.SetBkColor(WHITE_COLOR);

	m_CompList.InsertColumn(GRID_CMPNO_COLUMN, _T("No"), LVCFMT_CENTER, GRID_DEFAULT_COLUMN_WIDTH);
	m_CompList.InsertColumn(GRID_CMPNAME_COLUMN, _T("Component Name"), LVCFMT_CENTER, GRID_CMP_COLUMN_WIDTH);
	m_CompList.InsertColumn(GRID_CMPTYPE_COLUMN, _T("Type"), LVCFMT_CENTER, GRID_STEP_COLUMN_WIDTH);
	m_CompList.InsertColumn(GRID_CMPPINNUM_COLUMN, _T("Pin Num"), LVCFMT_CENTER, GRID_STEP_COLUMN_WIDTH);*/

	m_bInit = TRUE;

	_UpdateUI_Pos();
}

void CCamInfoFormView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	_UpdateUI_Pos();
}

UINT32		CCamInfoFormView::_UpdateUI_Pos()
{
	CRect clRect, clBtnRect;
	CString strBtnName;
	UINT32 iBtnWidth, iStepListHeight;
	UINT32 iBtnXNum = 5, iBtnYNum = 1;

	if (m_nStepNum > 0)
	{
		iBtnYNum = m_nStepNum / iBtnXNum;
		if (m_nStepNum % iBtnXNum > 0) iBtnYNum++;
	}

	if (!m_bInit) return RESULT_BAD;

	GetWindowRect(&clRect);
	iBtnWidth = clRect.Width() / iBtnXNum;

	clBtnRect.left = clBtnRect.top = 0;
	clBtnRect.right = clBtnRect.left + iBtnWidth;
	clBtnRect.bottom = clBtnRect.top + 45;

	UINT32 iStepBtnCnt = static_cast<UINT32>(m_vcSteps.size());

	bool bCreated = false;
	for (UINT32 iLoopY = 0; iLoopY < iBtnYNum; iLoopY++)
	{
		for (UINT32 iLoopX = 0; iLoopX < iBtnXNum; iLoopX++)
		{
			UINT32 iStepIdx = iLoopY * iBtnXNum + iLoopX;
			if (iStepIdx >= m_vcSteps.size())
				continue;

			strBtnName.Format(_T("%d Step"), iStepIdx + 1);

			if (m_bStepBtnCreated)
				m_vcSteps[iStepIdx]->MoveWindow(&clBtnRect);
			else
			{
				if (m_vcStepInfo.size() > iStepIdx)
					strBtnName = m_vcStepInfo[iStepIdx]->strName;
					

				m_vcSteps[iStepIdx]->Create(strBtnName, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, clBtnRect,
					this, IDC_STEP_BUTTON_FIRST + iStepIdx);

				bCreated = true;
			}

			clBtnRect.left = clBtnRect.left + iBtnWidth;
			clBtnRect.right = clBtnRect.right + iBtnWidth;
		}
		clBtnRect.top = clBtnRect.bottom;
		clBtnRect.bottom = clBtnRect.top + 45;
		clBtnRect.left = 0;
		clBtnRect.right = iBtnWidth;
	}

	if (m_nSelectedStep >= 0)
		m_vcSteps[m_nSelectedStep]->SetStickyState(TRUE);

	iStepListHeight = (clRect.Height() - 45 * iBtnYNum) / 4;
	m_StepList.MoveWindow(0, clBtnRect.bottom - 45, clRect.Width(), iStepListHeight);
	m_LayerList.MoveWindow(0, clBtnRect.bottom - 45 + iStepListHeight, clRect.Width(), iStepListHeight * 3);
	//		m_LayerList.MoveWindow(0, clBtnRect.bottom - 45 + iStepListHeight, clRect.Width(), iStepListHeight * 2);
	//		m_CompList.MoveWindow(0, clBtnRect.bottom - 45 + iStepListHeight * 3, clRect.Width(), iStepListHeight);

	//m_StepList.Invalidate();
	//m_LayerList.Invalidate();
	//		m_CompList.Invalidate();	

	if (bCreated)
		m_bStepBtnCreated = TRUE;

	return RESULT_GOOD;
}

UINT32 CCamInfoFormView::SetStepCount(UINT32& iStepCnt, std::vector<UINT32>& vcStepRepeatCnt)
{
	ClearStepButtons();
	
 	CJobFile *pJobFile = GetJobPtr();
	if (pJobFile == nullptr) return RESULT_BAD;
	
	m_nStepNum = iStepCnt;
	for (UINT32 i = 0; i < m_nStepNum; i++)
	{
		stStepInfo* pStepInfo = new stStepInfo();
		for (UINT32 j = 0; j < vcStepRepeatCnt[i]; j++)
		{
			stStepRepeat* pStepRepeat = new stStepRepeat();
			pStepInfo->vcStepRepeat.emplace_back(pStepRepeat);			
		}
		/*
		// 2022.06.22
		// 김준호 주석처리
		// CamInfo TEXT NAME 위해 추가
		if (i == 0)
			pStepInfo->strName = L"UNIT";
		else if(i==1)
			pStepInfo->strName = L"STRIP";
		else
			pStepInfo->strName = L"PANEL";
		*/
		//Step에 실제 이름을 넣음
		if (pJobFile->m_arrStep.GetCount() <= i)
		{//NG Case
			if (i == 0)
				pStepInfo->strName = L"UNIT";
			else if (i == 1)
				pStepInfo->strName = L"STRIP";
			else
				pStepInfo->strName = L"PANEL";
		}
		else
		{//OK Case
			pStepInfo->strName = pJobFile->m_arrStep.GetAt(i)->m_strStepName;
		}

		m_vcStepInfo.emplace_back(pStepInfo);
	}

	return CreateStepButtons();
}

UINT32 CCamInfoFormView::SetLayerCount(UINT32& iLayerCnt)
{
	ClearLayerInfo();

	m_nLayerNum = iLayerCnt;
	for (UINT32 i = 0; i < m_nLayerNum; i++)
	{
		stLayerInfo* pLayerInfo = new stLayerInfo();
		m_vcLayerInfo.emplace_back(pLayerInfo);
	}

	if (m_nLayerNum == static_cast<UINT32>(m_vcLayerInfo.size()))
		return RESULT_GOOD;
	else
		return RESULT_BAD;
}

UINT32 CCamInfoFormView::SetComponentCount(UINT32& iStepCnt, std::vector<UINT32>& vcCompCnt)
{
	if (iStepCnt != m_nStepNum)
		return RESULT_BAD;

	ClearComponentInfo();

	for (UINT32 i = 0; i < m_nStepNum; i++)
	{
		stComponentInfo* pCompInfo = new stComponentInfo();
		for (UINT32 j = 0; j < vcCompCnt[i]; j++)
		{
			stPackage* pPkg = new stPackage();
			pCompInfo->vcPackages.emplace_back(pPkg);
		}
		m_vcCompInfo.emplace_back(pCompInfo);
	}

	if (m_nStepNum == static_cast<UINT32>(m_vcCompInfo.size()))
		return RESULT_GOOD;
	else
		return RESULT_BAD;
}

UINT32 CCamInfoFormView::DisplayCamInfo(bool bLayout)
{		
	if (m_nSelectedStep < 0)
		SetCurStepIndex();

	DisplayStepList(m_nSelectedStep);
	DisplayLayerList();
//	DisplayComponentList();

	if (bLayout)
	{
		_UpdateUI_Pos();
	}

	return RESULT_GOOD;
}

void CCamInfoFormView::SetItem(CListCtrl& ctrl, int nItem, int nSubItem, LPCTSTR lpszItem)
{
	LV_ITEM lvi;
	CString str;
	str.Format(_T("  %s"), lpszItem);
	lvi.mask = LVIF_TEXT | LVIS_SELECTED;
	lvi.iItem = nItem;
	lvi.iSubItem = nSubItem;
	lvi.pszText = str.GetBuffer(str.GetLength());
	ctrl.SetItem(&lvi);
}

UINT32 CCamInfoFormView::DisplayStepList(UINT32 iStepIdx)
{
	m_StepList.DeleteAllItems();

	if (iStepIdx >= m_nStepNum)
		return RESULT_BAD;


	if (m_vcStepInfo.size() <= 0) return RESULT_BAD;
	if (iStepIdx < 0) return RESULT_BAD;
	if (m_vcStepInfo.size() <= iStepIdx) return RESULT_BAD;

	UINT32 iSubCnt = static_cast<UINT32>(m_vcStepInfo[iStepIdx]->vcStepRepeat.size());

	if (iSubCnt <= 0)
		return RESULT_GOOD;

	CString strTmp = L"";
	UINT32 nStepListRowNum = 0;
	for (UINT32 iSub = 0; iSub < iSubCnt; iSub++)
	{
		m_StepList.InsertItem(nStepListRowNum, _T(""));

		SetItem(m_StepList, nStepListRowNum, GRID_STEPNAME_COLUMN, 
			m_vcStepInfo[iStepIdx]->vcStepRepeat[iSub]->strName);
		strTmp.Format(_T("%d"), m_vcStepInfo[iStepIdx]->vcStepRepeat[iSub]->iNx);

		SetItem(m_StepList, nStepListRowNum, GRID_STEPDX_COLUMN, strTmp);
		strTmp.Format(_T("%d"), m_vcStepInfo[iStepIdx]->vcStepRepeat[iSub]->iNy);

		SetItem(m_StepList, nStepListRowNum, GRID_STEPDY_COLUMN, strTmp);
		strTmp.Format(_T("%.1f"), m_vcStepInfo[iStepIdx]->vcStepRepeat[iSub]->dAngle);

		SetItem(m_StepList, nStepListRowNum, GRID_STEPANGLE_COLUMN, strTmp);
		if (m_vcStepInfo[iStepIdx]->vcStepRepeat[iSub]->bMirror) 
			strTmp.Format(_T("Yes"));
		else 
			strTmp.Format(_T("No"));
		SetItem(m_StepList, nStepListRowNum, GRID_STEPMIRROR_COLUMN, strTmp);
		nStepListRowNum++;
	}

	m_StepList.Invalidate();
	return RESULT_GOOD;
}

CString CCamInfoFormView::GetWorkStepName()
{
	if (m_nSelectedStep < 0)
		return L"";

	if (m_nSelectedStep >= m_vcStepInfo.size())
		return L"";

	return m_vcStepInfo[m_nSelectedStep]->strName;
}

void CCamInfoFormView::OnStepButtonClicked(UINT32 nID)
{
	if (nID < 0)
		return;

	UINT32 iStepNo = nID - IDC_STEP_BUTTON_FIRST;

	if (m_nSelectedStep == iStepNo)
		return;

	if (iStepNo >= m_vcSteps.size())
		return;

	for (UINT32 iLoop = 0; iLoop < m_nStepNum; iLoop++)	
		m_vcSteps[iLoop]->SetStickyState(FALSE);		

	m_vcSteps[iStepNo]->ToggleStickyState();
	m_nSelectedStep = iStepNo;

	if (RESULT_GOOD == EstimateStepOrient())
	{
		//
		double dAngle = 0.0;
		bool bMirror = false;
		GetStepOrient(dAngle, bMirror);

		UserSetInfo_t * pUserSetInfo = GetUserSetInfo();
		if (pUserSetInfo != nullptr)
		{
			pUserSetInfo->dAngle = dAngle;
			pUserSetInfo->bMirror = bMirror;
			if (bMirror == true)
			{
				pUserSetInfo->mirrorDir = eDirectionType::eHorizontal;
			}
		}
	}

	DisplayCamInfo(false);
	DrawCamData();
}

UINT32 CCamInfoFormView::SetCurStepIndex()
{
	if (m_vcStepInfo.size() <= 0)
		return RESULT_BAD;

	INT32 iUserStep = -1;
	SystemSpec_t* pSysSpec = GetSystemSpec();

	for (auto it : pSysSpec->sysStep.vcPanelNames)
	{
		INT32 index = 0;
		for (auto itStep : m_vcStepInfo)
		{
			// 2023.01.16 KJH ADD 
			// 기존 INI Panel 이름과 ODB Panel 이름이 달라서 비교 안됨 Find 문으로 변경
			//if (itStep->strName.CompareNoCase(it) == 0)
			if (itStep->strName.Find(it) == 0)
			{
				iUserStep = index;
				break;
			}
			index++;
		}
	}	

	if (iUserStep >= m_vcStepInfo.size())
		return RESULT_BAD;	
	
	m_nSelectedStep = iUserStep;

	return RESULT_GOOD;
}

UINT32 CCamInfoFormView::DisplayLayerList()
{
	m_LayerList.DeleteAllItems();
	
	UINT32 iLayerCnt = static_cast<UINT32>(m_vcLayerInfo.size());
	if (iLayerCnt <= 0)
		return RESULT_GOOD;

	CString strTmp = L"";
	UINT32 nLayerListRowNum = 0;
	for (UINT32 iLayer = 0; iLayer < iLayerCnt; iLayer++)
	{
		strTmp.Format(_T("%d"), nLayerListRowNum + 1);
		m_LayerList.InsertItem(nLayerListRowNum, _T(""));

		SetItem(m_LayerList, nLayerListRowNum, GRID_LAYERNO_COLUMN, strTmp);
		SetItem(m_LayerList, nLayerListRowNum, GRID_LAYERNAME_COLUMN, m_vcLayerInfo[iLayer]->strName);
		SetItem(m_LayerList, nLayerListRowNum, GRID_LAYERCONTEXT_COLUMN, m_vcLayerInfo[iLayer]->strContext);
		SetItem(m_LayerList, nLayerListRowNum, GRID_LAYERTYPE_COLUMN, m_vcLayerInfo[iLayer]->strType);

		if (m_vcLayerInfo[iLayer]->bIsPositive)
			SetItem(m_LayerList, nLayerListRowNum, GRID_LAYERPOLARITY_COLUMN, _T("POSITIVE"));
		else
			SetItem(m_LayerList, nLayerListRowNum, GRID_LAYERPOLARITY_COLUMN, _T("NEGATIVE"));

		nLayerListRowNum++;
	}

	m_LayerList.Invalidate();
	return RESULT_GOOD;
}

UINT32 CCamInfoFormView::DisplayComponentList(UINT32 iStepIdx)
{
	m_CompList.DeleteAllItems();

	if (iStepIdx >= m_nStepNum)
		return RESULT_BAD;

	UINT32 iCompCnt = static_cast<UINT32>(m_vcCompInfo[iStepIdx]->vcPackages.size());
	if (iCompCnt <= 0)
		return RESULT_GOOD;

	CString strTmp = L"";
	UINT32 nCompListRowNum = 0;
	for (UINT32 iComp = 0; iComp < iCompCnt; iComp++)
	{
		strTmp.Format(_T("%d"), nCompListRowNum + 1);
		m_CompList.InsertItem(nCompListRowNum, _T(""));

		SetItem(m_CompList, nCompListRowNum, GRID_CMPNO_COLUMN, strTmp);
		SetItem(m_CompList, nCompListRowNum, GRID_CMPNAME_COLUMN, m_vcCompInfo[iStepIdx]->vcPackages[iComp]->strName);
		SetItem(m_CompList, nCompListRowNum, GRID_CMPTYPE_COLUMN, m_vcCompInfo[iStepIdx]->vcPackages[iComp]->strType);
		
		strTmp.Format(_T("%d"), m_vcCompInfo[iStepIdx]->vcPackages[iComp]->iPinCnt);
		SetItem(m_CompList, nCompListRowNum, GRID_CMPPINNUM_COLUMN, strTmp);

		nCompListRowNum++;
	}

	m_CompList.Invalidate();
	return RESULT_GOOD;
}

UINT32 CCamInfoFormView::CheckLayerInfo(std::vector<stLayerInfo*>* v)
{
	for (int i = 0; i < 2; i++)
		m_strArrLayerList[i].RemoveAll();

	if (v == nullptr)
		return RESULT_BAD;

	int iLayerNum = static_cast<int>(v->size());
	if (iLayerNum < 1)
		return RESULT_GOOD;
	
	CString LayerName = L"", strTemp = L"";
	for (auto Layer : *v)
	{
		LayerName = Layer->strName;
		LayerName.Delete(0, LayerName.GetLength() - 3);

		if (LayerName.CompareNoCase(_T("_MP")) == 0)
		{
			LayerName = Layer->strName;
			m_strArrLayerList[0].Add(LayerName);

			LayerName.Replace(_T("_MP"), _T("_TP"));
			LayerName.Replace(_T("_mp"), _T("_TP"));

			m_strArrLayerList[0].Add(LayerName);

			LayerName = Layer->strName;

			LayerName.Replace(_T("_MP"), _T(""));
			LayerName.Replace(_T("_mp"), _T(""));

			m_strArrLayerList[0].Add(LayerName);
		}
	}
	

	for (int i = 0; i < m_strArrLayerList[0].GetSize(); i++)
	{
		LayerName = m_strArrLayerList[0].GetAt(i);

		if (LayerName.Find(_T("MP")) >= 0 || LayerName.Find(_T("mp")) >= 0)
		{
			LayerName.Delete(0, LayerName.GetLength() - 4);
			strTemp.Format(_T("B_MP"));
		}
		else
		{
			LayerName.Delete(0, LayerName.GetLength() - 1);
			strTemp.Format(_T("B"));
		}

		if (LayerName.CompareNoCase(strTemp) == 0)
		{
			LayerName = m_strArrLayerList[0].GetAt(i);
			m_strArrLayerList[1].Add(LayerName);

			m_strArrLayerList[0].RemoveAt(i);

			LayerName.Replace(_T("B"), _T(""));
			LayerName.Replace(_T("b"), _T(""));

			for (int j = 0; j < m_strArrLayerList[0].GetSize(); j++)
			{
				strTemp = m_strArrLayerList[0].GetAt(j);

				if (strTemp.CompareNoCase(LayerName) == 0)
				{
					m_strArrLayerList[1].Add(strTemp);
					m_strArrLayerList[0].RemoveAt(j);
					i = -1;	// m_strArrLayerList[0] Size 변동되어 0 번부터 다시 검색
					break;
				}
			}
		}
	}

	for (int i = 0; i < iLayerNum; i++)
	{
		stLayerInfo* Layer = v->at(i);

		bool bFind = false;
		for (int j = 0; j < m_strArrLayerList[1].GetSize(); j++)
		{
			LayerName = m_strArrLayerList[1].GetAt(j);

			if (LayerName.CompareNoCase(Layer->strName) == 0)
			{
				bFind = true;
				break;
			}
		}

		if (!bFind)
			continue;

		if (LayerName.Find(_T("B")) >= 0)
		{
			Layer->bIsTop = false;
			LayerName.Replace(_T("B"), _T(""));
		}
		else if (Layer->strName.Find(_T("b")) >= 0)
		{
			Layer->bIsTop = false;
			LayerName.Replace(_T("b"), _T(""));
		}
		else
		{
			Layer->bIsTop = true;
			if (LayerName.Find(_T("_mp")) >= 0)			
				LayerName.Replace(_T("_mp"), _T("B_mp"));			
			else if (LayerName.Find(_T("_MP")) >= 0)			
				LayerName.Replace(_T("_MP"), _T("B_MP"));			
			else
				LayerName += _T("B");
		}

		for (int j = 0; j < iLayerNum; j++)
		{
			stLayerInfo* Layer2 = v->at(j);
			if (Layer2->strName.CompareNoCase(LayerName) == 0)
			{
				Layer->iPairIndex = j;
				Layer->strPairLayerName = Layer2->strName;
				break;
			}
		}
	}

	stLayerInfoTemp stTempInfo;
	if (!stTempInfo.Alloc(iLayerNum))
		return RESULT_BAD;
	
	INT32 iCount = 0, iMPCount = 0, iLayerIdx = 0;
	INT32 iTPCount = 0;
	for (int i = 0; i < iLayerNum; i++)
	{
		stLayerInfo* Layer = v->at(i);
		if (Layer == nullptr)
			continue;

		bool bFind = false;
		for (int j = 0; j < m_strArrLayerList[0].GetSize(); j++)
		{
			LayerName = m_strArrLayerList[0].GetAt(j);

			if (LayerName.CompareNoCase(Layer->strName) == 0)
			{
				bFind = true;
				break;
			}
		}

		if (!bFind)
			continue;

		LayerName.Replace(_T("L"), _T(""));
		LayerName.Replace(_T("l"), _T(""));

		if (LayerName.Find(_T("_MP")) >= 0 || LayerName.Find(_T("_mp")) >= 0)
		{
			LayerName.Replace(_T("_MP"), _T(""));
			LayerName.Replace(_T("_mp"), _T(""));

			iLayerIdx = _ttoi(LayerName);

			stTempInfo.vecMPLayerNum[iMPCount++] = iLayerIdx;
			stTempInfo.vecMPLayerNo[i] = iLayerIdx;
		}
		else if (LayerName.Find(_T("_TP")) >= 0 || LayerName.Find(_T("_tp")) >= 0)
		{
			LayerName.Replace(_T("_TP"), _T(""));
			LayerName.Replace(_T("_tp"), _T(""));

			iLayerIdx = _ttoi(LayerName);

			stTempInfo.vecTPLayerNum[iTPCount++] = iLayerIdx;
			stTempInfo.vecTPLayerNo[i] = iLayerIdx;
		}
		else
		{
			iLayerIdx = _ttoi(LayerName);

			stTempInfo.vecLayerNum[iCount++] = iLayerIdx;
			stTempInfo.vecLayerNo[i] = iLayerIdx;
		}
	}

	int size = static_cast<int>(m_strArrLayerList[0].GetSize());	
 	std::sort(stTempInfo.vecLayerNum.begin(), stTempInfo.vecLayerNum.begin() + stTempInfo.vecLayerNum.size());
	std::sort(stTempInfo.vecMPLayerNum.begin(), stTempInfo.vecMPLayerNum.begin() + stTempInfo.vecMPLayerNum.size());
	std::sort(stTempInfo.vecTPLayerNum.begin(), stTempInfo.vecTPLayerNum.begin() + stTempInfo.vecTPLayerNum.size());

	for (int i = 0; i < iLayerNum; i++)
	{
		stLayerInfo* Layer = v->at(i);
		if (Layer == nullptr)
			continue;

		bool bFind = false;
		for (int j = 0; j < m_strArrLayerList[0].GetSize(); j++)
		{
			LayerName = m_strArrLayerList[0].GetAt(j);

			if (LayerName.CompareNoCase(Layer->strName) == 0)
			{
				bFind = true;
				break;
			}
		}

		if (!bFind)
			continue;

		stLayerInfo* Layer2 = nullptr;
		if (Layer->strName.Find(_T("_MP")) >= 0 || Layer->strName.Find(_T("_mp")) >= 0)
		{
			int j, iTempIndex = -1;
			for (j = 0; j < iMPCount; j++)
			{
				if (stTempInfo.vecMPLayerNo[i] == stTempInfo.vecMPLayerNum[j])					
					break;				
			}

			iTempIndex = j;
			for (j = 0; j < iLayerNum; j++)
			{
				if (iMPCount - iTempIndex - 1 >= 0)
				{
					if (stTempInfo.vecMPLayerNum[iMPCount - iTempIndex - 1] == stTempInfo.vecMPLayerNo[j])
					{
						Layer2 = v->at(j);
						break;
					}
				}
			}

			if (Layer2 == nullptr)
				continue;

			if (iMPCount - iTempIndex - 1 < 0) continue;

			if (stTempInfo.vecMPLayerNo[i] < stTempInfo.vecMPLayerNum[iMPCount - iTempIndex - 1])
				Layer->bIsTop = true;
			else
				Layer->bIsTop = false;

			Layer->iPairIndex = j;
			Layer->strPairLayerName = Layer2->strName;
		}
		else if (Layer->strName.Find(_T("_TP")) >= 0 || Layer->strName.Find(_T("_tp")) >= 0)
		{
			int j, iTempIndex = -1;
			for (j = 0; j < iTPCount; j++)
			{
				if (stTempInfo.vecTPLayerNo[i] == stTempInfo.vecTPLayerNum[j])
					break;
			}

			iTempIndex = j;
			for (j = 0; j < iLayerNum; j++)
			{
				if (iTPCount - iTempIndex - 1 >= 0)
				{
					if (stTempInfo.vecTPLayerNum[iTPCount - iTempIndex - 1] == stTempInfo.vecTPLayerNo[j])
					{
						Layer2 = v->at(j);
						break;
					}
				}
			}

			if (Layer2 == nullptr)
				continue;

			if (iTPCount - iTempIndex - 1 < 0) continue;

			if (stTempInfo.vecTPLayerNo[i] < stTempInfo.vecTPLayerNum[iTPCount - iTempIndex - 1])
				Layer->bIsTop = true;
			else
				Layer->bIsTop = false;

			Layer->iPairIndex = j;
			Layer->strPairLayerName = Layer2->strName;
		}
		else
		{
			int j, iTempIndex = -1;
			for (j = 0; j < iMPCount; j++)
			{
				if (stTempInfo.vecLayerNo[i] == stTempInfo.vecLayerNum[j])
					break;
			}


			iTempIndex = j;
			for (j = 0; j < iLayerNum; j++)
			{
				if (iCount - iTempIndex - 1 >= 0)
				{

					if (stTempInfo.vecLayerNum[iCount - iTempIndex - 1] == stTempInfo.vecLayerNo[j])
					{
						Layer2 = v->at(j);
						break;
					}
				}
			}

			if (Layer2 == nullptr)
				continue;

			if (iCount - iTempIndex - 1 < 0) continue;

			if (stTempInfo.vecLayerNo[i] < stTempInfo.vecLayerNum[iCount - iTempIndex - 1])
				Layer->bIsTop = true;
			else
				Layer->bIsTop = false;

			Layer->iPairIndex = j;
			Layer->strPairLayerName = Layer2->strName;
		}
	}

	stTempInfo.Clear();
	return RESULT_GOOD;
}

// Layer Top/Bot Pair 만들기
UINT32 CCamInfoFormView::CheckLayerInfo_V2(std::vector<stLayerInfo*>* v, 
	vector<vector<stLayerInfo*>> &vecPatternLayer,
	vector<vector<stLayerInfo*>> &vecSRLayer,
	vector<vector<stLayerInfo*>> &vecMPLayer,
	vector<vector<stLayerInfo*>> &vecTPLayer)
{	
	m_vecLayerList->clear();

	if (v == nullptr)
		return RESULT_BAD;

	SystemSpec_t *pSystemSpec = GetSystemSpec();

	if (pSystemSpec == nullptr)
		return RESULT_BAD;

	int iLayerNum = static_cast<int>(v->size());
	if (iLayerNum < 1)
		return RESULT_GOOD;


	bool bPattern = false, bMP = false, bTP = false;	// , bFindPair = false;
	BOOL bCheck = FALSE;
	INT32 iFrontIdx = -1, iBackIdx = -1;
	INT32 iKind = -1;
	vector<CString> vecSliceFilter;
	CString strLayer = L"", strLayer2 = L"", strLayerName = L"", strLayerIdx = L"";
	CString strFilterName;

	//전체 Layer 순서대로 Pattern, Measure, Measure_SR 로 나눠서 각각 vector에 담기
	for (int i = 0; i < iLayerNum; i++)
	{
		stLayerInfo* Layer = v->at(i);
		strLayer = Layer->strName;
		strLayer.MakeUpper();
		if (strLayer.Find(_T("_BACKUP")) >= 0)
		{
			continue;
		}
		if (strLayer.Find(_T("_MP")) >= 0)
		{// MP Layer
			iKind = 1;
		}
		else if (strLayer.Find(_T("_TP")) >= 0)
		{// TP Layer
			iKind = 2;
		}
		else
		{// onther Layer
			iKind = 0;
		}
		
		// Layer Filtering		
		for (auto it : pSystemSpec->sysLayer.vcLayer)
		{
			if (iKind == 0 && it->strDefine == PATTERN_LAYER_TXT)
			{
				bCheck = FALSE;
				//for (auto itFrontName : it->vcFrontLayerNames)
				vecPatternLayer.resize(it->vcFrontLayerNames.size());
				for(int i = 0; i < it->vcFrontLayerNames.size(); i++)
				{
					//strFilterName = itFrontName;
					strFilterName = it->vcFrontLayerNames[i];
					strFilterName.MakeUpper();

					CUtils::SliceString(strFilterName, _T("[S]"), vecSliceFilter);

					CUtils::StringOutputIndex(strLayer, vecSliceFilter, iFrontIdx, bCheck);

					if (iFrontIdx != -1 && bCheck == TRUE)
					{
						vecPatternLayer[i].push_back(Layer);						
						bCheck = FALSE;
					}
				}
				//for (auto itBackName : it->vcBackLayerNames)
				for (int i = 0; i < it->vcBackLayerNames.size(); i++)
				{
					//strFilterName = itBackName;
					strFilterName = it->vcBackLayerNames[i];
					strFilterName.MakeUpper();

					CUtils::SliceString(strFilterName, _T("[E]"), vecSliceFilter);

					CUtils::StringOutputIndex(strLayer, vecSliceFilter, iBackIdx, bCheck);

					if (iBackIdx != -1 && bCheck == TRUE)
					{						
						auto bFind = find(vecPatternLayer[i].begin(), vecPatternLayer[i].end(), Layer);
						if (bFind == vecPatternLayer[i].end())
						{
							vecPatternLayer[i].push_back(Layer);
							bCheck = FALSE;
						}
					}
				}
			}
			else if (iKind == 0 && it->strDefine == SR_LAYER_TXT)
			{
				bCheck = FALSE;
				//for (auto itFrontName : it->vcFrontLayerNames)
				vecSRLayer.resize(it->vcFrontLayerNames.size());
				for (int i = 0; i < it->vcFrontLayerNames.size(); i++)
				{
					//strFilterName = itFrontName;
					strFilterName = it->vcFrontLayerNames[i];
					strFilterName.MakeUpper();

					CUtils::SliceString(strFilterName, _T("[S]"), vecSliceFilter);

					CUtils::StringOutputIndex(strLayer, vecSliceFilter, iFrontIdx, bCheck);

					if (iFrontIdx != -1 && bCheck == TRUE)
					{
						vecSRLayer[i].push_back(Layer);
						bCheck = FALSE;
					}
				}
				//for (auto itBackName : it->vcBackLayerNames)
				for (int i = 0; i < it->vcBackLayerNames.size(); i++)
				{
					//strFilterName = itBackName;
					strFilterName = it->vcBackLayerNames[i];
					strFilterName.MakeUpper();

					CUtils::SliceString(strFilterName, _T("[E]"), vecSliceFilter);

					CUtils::StringOutputIndex(strLayer, vecSliceFilter, iBackIdx, bCheck);

					if (iBackIdx != -1 && bCheck == TRUE)
					{
						auto bFind = find(vecSRLayer[i].begin(), vecSRLayer[i].end(), Layer);
						if (bFind == vecSRLayer[i].end())
						{
							vecSRLayer[i].push_back(Layer);
							bCheck = FALSE;
						}
					}
				}
			}
			else if (iKind == 1 && it->strDefine == MEASURE_LAYER_TXT)
			{
				bCheck = FALSE;
				//for (auto itFrontName : it->vcFrontLayerNames)
				vecMPLayer.resize(it->vcFrontLayerNames.size());
				for (int i = 0; i < it->vcFrontLayerNames.size(); i++)
				{
					//strFilterName = itFrontName;
					strFilterName = it->vcFrontLayerNames[i];				
					strFilterName.MakeUpper();

					CUtils::SliceString(strFilterName, _T("[S]"), vecSliceFilter);

					CUtils::StringOutputIndex(strLayer, vecSliceFilter, iFrontIdx, bCheck);

					if (iFrontIdx != -1 && bCheck == TRUE)
					{
						vecMPLayer[i].push_back(Layer);
						bCheck = FALSE;
					}
				}
				//for (auto itBackName : it->vcBackLayerNames)
				for (int i = 0; i < it->vcBackLayerNames.size(); i++)
				{
					//strFilterName = itBackName;
					strFilterName = it->vcBackLayerNames[i];
					strFilterName.MakeUpper();

					CUtils::SliceString(strFilterName, _T("[E]"), vecSliceFilter);

					CUtils::StringOutputIndex(strLayer, vecSliceFilter, iFrontIdx, bCheck);

					if (iFrontIdx != -1 && bCheck == TRUE)
					{
						auto bMPFind = find(vecMPLayer[i].begin(), vecMPLayer[i].end(), Layer);
						if (bMPFind == vecMPLayer[i].end())
						{
							vecMPLayer[i].push_back(Layer);
							bCheck = FALSE;
						}
					}
				}
			}
			else if (iKind == 2 && it->strDefine == THICKNESS_LAYER_TXT)
			{
				bCheck = FALSE;
				//for (auto itFrontName : it->vcFrontLayerNames)
				vecTPLayer.resize(it->vcFrontLayerNames.size());
				for (int i = 0; i < it->vcFrontLayerNames.size(); i++)
				{
					//strFilterName = itFrontName;
					strFilterName = it->vcFrontLayerNames[i];
					strFilterName.MakeUpper();

					CUtils::SliceString(strFilterName, _T("[S]"), vecSliceFilter);

					CUtils::StringOutputIndex(strLayer, vecSliceFilter, iFrontIdx, bCheck);

					if (iFrontIdx != -1 && bCheck == TRUE)
					{
						vecTPLayer[i].push_back(Layer);
						bCheck = FALSE;
					}
				}
				//for (auto itBackName : it->vcBackLayerNames)
				for (int i = 0; i < it->vcBackLayerNames.size(); i++)
				{
					//strFilterName = itBackName;
					strFilterName = it->vcBackLayerNames[i];
					strFilterName.MakeUpper();

					CUtils::SliceString(strFilterName, _T("[E]"), vecSliceFilter);

					CUtils::StringOutputIndex(strLayer, vecSliceFilter, iFrontIdx, bCheck);

					if (iFrontIdx != -1 && bCheck == TRUE)
					{
						auto bTPFind = find(vecTPLayer[i].begin(), vecTPLayer[i].end(), Layer);
						if (bTPFind == vecTPLayer[i].end())
						{
							vecTPLayer[i].push_back(Layer);
							bCheck = FALSE;
						}
					}
				}
			}
		}
	}
	
	// Pattern Layer Top/Bot Pair 	
	int nPatternSize = static_cast<int>(vecPatternLayer.size());
	for (int i = 0; i < nPatternSize; i++)
	{	
		//Sort
		sort(vecPatternLayer[i].begin(), vecPatternLayer[i].begin() + vecPatternLayer[i].size(), SortAscending);

		int nPatternSize2 = static_cast<int>(vecPatternLayer[i].size());
		for (int j = 0; j < nPatternSize2; j++)
		{

			if (vecPatternLayer[i][j]->strPairLayerName != _T("")) continue;
			// Top
			vecPatternLayer[i][j]->bIsTop = true;
			vecPatternLayer[i][j]->strPairLayerName = vecPatternLayer[i][nPatternSize2 - j - 1]->strName;
			
			if (nPatternSize2 / 2 >= j)
			{// Bot
				vecPatternLayer[i][nPatternSize2 - j - 1]->bIsTop = false;
				vecPatternLayer[i][nPatternSize2 - j - 1]->strPairLayerName = vecPatternLayer[i][j]->strName;
			}
		}
	}

	// SR Layer Top/Bot Pair 	
	int nSRSize = static_cast<int>(vecSRLayer.size());
	for (int i = 0; i < nSRSize; i++)
	{
		//Sort
		sort(vecSRLayer[i].begin(), vecSRLayer[i].begin() + vecSRLayer[i].size(), SortAscending);

		int nSRSize2 = static_cast<int>(vecSRLayer[i].size());
		for (int j = 0; j < nSRSize2; j++)
		{

			if (vecSRLayer[i][j]->strPairLayerName != _T("")) continue;
			// Top
			vecSRLayer[i][j]->bIsTop = true;
			vecSRLayer[i][j]->strPairLayerName = vecSRLayer[i][nSRSize2 - j - 1]->strName;

			if (nSRSize2 / 2 >= j)
			{// Bot
				vecSRLayer[i][nSRSize2 - j - 1]->bIsTop = false;
				vecSRLayer[i][nSRSize2 - j - 1]->strPairLayerName = vecSRLayer[i][j]->strName;
			}
		}
	}

	// MP Layer Top/Bot Pair 	
	int nMPSize = static_cast<int>(vecMPLayer.size());
	for (int i = 0; i < nMPSize; i++)
	{
		//Sort
		sort(vecMPLayer[i].begin(), vecMPLayer[i].begin() + vecMPLayer[i].size(), SortAscending);

		int nMPSize2 = static_cast<int>(vecMPLayer[i].size());
		for (int j = 0; j < nMPSize2; j++)
		{

			if (vecMPLayer[i][j]->strPairLayerName != _T("")) continue;
			// Top
			vecMPLayer[i][j]->bIsTop = true;
			vecMPLayer[i][j]->strPairLayerName = vecMPLayer[i][nMPSize2 - j - 1]->strName;

			if (nMPSize2 / 2 >= j)
			{// Bot
				vecMPLayer[i][nMPSize2 - j - 1]->bIsTop = false;
				vecMPLayer[i][nMPSize2 - j - 1]->strPairLayerName = vecMPLayer[i][j]->strName;
			}
		}
	}
	
	// TP Layer Top/Bot Pair 	
	int nTPSize = static_cast<int>(vecTPLayer.size());
	for (int i = 0; i < nTPSize; i++)
	{
		//Sort
		sort(vecTPLayer[i].begin(), vecTPLayer[i].begin() + vecTPLayer[i].size(), SortAscending);

		int nTPSize2 = static_cast<int>(vecTPLayer[i].size());
		for (int j = 0; j < nTPSize2; j++)
		{

			if (vecTPLayer[i][j]->strPairLayerName != _T("")) continue;
			// Top
			vecTPLayer[i][j]->bIsTop = true;
			vecTPLayer[i][j]->strPairLayerName = vecTPLayer[i][nTPSize2 - j - 1]->strName;

			if (nTPSize2 / 2 >= j)
			{// Bot
				vecTPLayer[i][nTPSize2 - j - 1]->bIsTop = false;
				vecTPLayer[i][nTPSize2 - j - 1]->strPairLayerName = vecTPLayer[i][j]->strName;
			}
		}
	}

	/*for (int i = 0; i < iLayerNum; i++)	
	{		
		stLayerInfo* Layer = v->at(i);

		// 현재 Layer Name
		strLayer = Layer->strName;

		if (strLayer.Find(_T("_BACKUP")) >= 0 || strLayer.Find(_T("_backup")) >= 0)
			continue;

		if (strLayer.Find(_T("_MP")) >= 0 || strLayer.Find(_T("_mp")) >= 0)
		{// MP Layer
			iKind = 1;
		}
		else if (strLayer.Find(_T("_TP")) >= 0 || strLayer.Find(_T("_tp")) >= 0)
		{// TP Layer
			iKind = 2;
		}
		else
		{// Pattern Layer
			iKind = 0;
		}
		
		// Layer Filtering		
		for (auto it : pSystemSpec->sysLayer.vcLayer)
		{	
			// PATTERN Layer
			if (iKind == 0 &&it->strDefine == L"PATTERN")
			{
				if (bPattern == true || Layer->strPairLayerName != L"") 
				{
					bPattern = false;
					continue;
				}

				for (auto itFrontNames : it->vcFrontLayerNames)
				{
					strLayerName = itFrontNames;
					strLayerName.MakeUpper();

					CUtils::SliceString(strLayerName, _T("[S]"), vecSliceFilter);

					CUtils::StringOutputIndex(strLayer, vecSliceFilter, iFrontIdx, bCheck);

					if (iFrontIdx != -1 && bCheck == TRUE)
					{
						Layer->bIsTop = true;
						bPattern = true;
						break;
					}
				}

				if (bPattern)
				{
					bPattern = false;

					// PairLayer Index, Name 찾기
					for (int j = 0; j < iLayerNum; j++)
					{
						stLayerInfo* Layer2 = v->at(j);
						strLayer2 = Layer2->strName;

						if (Layer2->strName.CompareNoCase(Layer->strName) == 0) continue;

						if (strLayer2.Find(_T("_BACKUP")) >= 0 || strLayer2.Find(_T("_backup")) >= 0)
							continue;

						if (strLayer2.Find(_T("_MP")) >= 0 || strLayer2.Find(_T("_mp")) >= 0)
						{// MP Layer
							iKind = 1;
							continue;
						}
						else if (strLayer2.Find(_T("_TP")) >= 0 || strLayer2.Find(_T("_tp")) >= 0)
						{// TP Layer
							iKind = 2;
							continue;
						}
						else
						{// Pattern Layer
							iKind = 0;
						}

						if (iKind == 0 && it->strDefine == L"PATTERN")
						{
							if (bPattern == true || Layer2->strPairLayerName != L"")
							{
								bPattern = false;
								continue;
							}
							for (auto itBackNames : it->vcBackLayerNames)
							{
								strLayerName = itBackNames;
								strLayerName.MakeUpper();

								CUtils::SliceString(strLayerName, _T("[E]"), vecSliceFilter);

								CUtils::StringOutputIndex(strLayer2, vecSliceFilter, iBackIdx, bCheck);

								if (iFrontIdx == iBackIdx) continue;
								if (iBackIdx != -1 && bCheck == TRUE)
								{
									Layer2->bIsTop = false;
									Layer2->iPairIndex = i;
									Layer2->strPairLayerName = Layer->strName;

									Layer->iPairIndex = j;
									Layer->strPairLayerName = Layer2->strName;
									bPattern = true;
									//bFindPair = true;
									break;
								}
							}
							if (bPattern)
								break;
						}

					}
				}
			}// it->strDefine Pattern 일 경우
			// MP Layer 
			else if (iKind == 1 && it->strDefine == L"MEASURE")
			{
				if (bMP == true || Layer->strPairLayerName != L"")
				{
					bMP = false;
					continue;
				}

				for (auto itFrontNames : it->vcFrontLayerNames)
				{
					strLayerName = itFrontNames;
					strLayerName.MakeUpper();

					CUtils::SliceString(strLayerName, _T("[S]"), vecSliceFilter);

					CUtils::StringOutputIndex(strLayer, vecSliceFilter, iFrontIdx, bCheck);

					if (iFrontIdx != -1 && bCheck == TRUE)
					{
						Layer->bIsTop = true;
						bMP = true;
						break;
					}
				}

				if (bMP)
				{
					bMP = false;

					// PairLayer Index, Name 찾기
					for (int j = 0; j < iLayerNum; j++)
					{
						stLayerInfo* Layer2 = v->at(j);
						strLayer2 = Layer2->strName;

						if (Layer2->strName.CompareNoCase(Layer->strName) == 0) continue;
						
						if (strLayer2.Find(_T("_BACKUP")) >= 0 || strLayer2.Find(_T("_backup")) >= 0)
							continue;

						if (strLayer2.Find(_T("_MP")) >= 0 || strLayer2.Find(_T("_mp")) >= 0)
						{// MP Layer
							iKind = 1;
						}
						else if (strLayer2.Find(_T("_TP")) >= 0 || strLayer2.Find(_T("_tp")) >= 0)
						{// TP Layer
							iKind = 2;
						}
						else
						{// Pattern Layer
							iKind = 0;
						}

						if (iKind == 1 && it->strDefine == L"MEASURE")
						{
							if (bMP == true || Layer2->strPairLayerName != L"")
							{
								bMP = false;
								continue;
							}

							for (auto itBackNames : it->vcBackLayerNames)
							{
								strLayerName = itBackNames;
								strLayerName.MakeUpper();

								CUtils::SliceString(strLayerName, _T("[E]"), vecSliceFilter);

								CUtils::StringOutputIndex(strLayer, vecSliceFilter, iBackIdx, bCheck);

								if (iFrontIdx == iBackIdx) continue;
								if (iBackIdx != -1 && bCheck == TRUE)
								{
									Layer2->bIsTop = false;
									Layer2->iPairIndex = i;
									Layer2->strPairLayerName = Layer->strName;

									Layer->iPairIndex = j;
									Layer->strPairLayerName = Layer2->strName;
									bMP = true;
									//bFindPair = true;

									break;
								}
							}
							if (bMP)
								break;
						}
					}
				}
			}// it->strDefine Measure 일 경우
			// TP Layer
			else if (iKind == 2 && it->strDefine == L"MEASURE_SR")
			{
				if (bTP == true || Layer->strPairLayerName != L"")
				{
					bTP = false;
					continue;
				}

				for (auto itFrontNames : it->vcFrontLayerNames)
				{
					strLayerName = itFrontNames;
					strLayerName.MakeUpper();

					CUtils::SliceString(strLayerName, _T("[S]"), vecSliceFilter);

					CUtils::StringOutputIndex(strLayer, vecSliceFilter, iFrontIdx, bCheck);

					if (iFrontIdx != -1 && bCheck == TRUE)
					{
						Layer->bIsTop = true;
						bTP = true;
						break;
					}
				}

				if (bTP)
				{
					bTP = false;

					// PairLayer Index, Name 찾기
					for (int j = 0; j < iLayerNum; j++)
					{
						stLayerInfo* Layer2 = v->at(j);
						strLayer2 = Layer2->strName;

						if (Layer2->strName.CompareNoCase(Layer->strName) == 0) continue;

						if (strLayer2.Find(_T("_BACKUP")) >= 0 || strLayer2.Find(_T("_backup")) >= 0)
							continue;

						if (strLayer2.Find(_T("_MP")) >= 0 || strLayer2.Find(_T("_mp")) >= 0)
						{// MP Layer
							iKind = 1;
						}
						else if (strLayer2.Find(_T("_TP")) >= 0 || strLayer2.Find(_T("_tp")) >= 0)
						{// TP Layer
							iKind = 2;
						}
						else
						{// Pattern Layer
							iKind = 0;
						}

						if (iKind == 2 && it->strDefine == L"MEASURE_SR")
						{
							if (bTP == true || Layer2->strPairLayerName != L"")
							{
								bTP = false;
								continue;
							}
							for (auto itBackNames : it->vcBackLayerNames)
							{
								strLayerName = itBackNames;
								strLayerName.MakeUpper();

								CUtils::SliceString(strLayerName, _T("[E]"), vecSliceFilter);

								CUtils::StringOutputIndex(strLayer, vecSliceFilter, iBackIdx, bCheck);

								if (iFrontIdx == iBackIdx) continue;
								if (iBackIdx != -1 && bCheck == TRUE)
								{
									Layer2->bIsTop = false;
									Layer2->iPairIndex = i;
									Layer2->strPairLayerName = Layer->strName;

									Layer->iPairIndex = j;
									Layer->strPairLayerName = Layer2->strName;
									bTP = true;
									//bFindPair = true;
									break;
								}
							}
							if (bTP)
								break;

						}
					}
				}
			}// it->strDefine MEASURE_SR 일 경우
		}

	}*/
	
	return RESULT_GOOD;
}

UINT32 CCamInfoFormView::ChangeStep(int uiMenuCmd)
{
	if (uiMenuCmd < 0)
		return RESULT_BAD;

	OnStepButtonClicked(uiMenuCmd + IDC_STEP_BUTTON_FIRST);

	return RESULT_GOOD;
}


UINT32 CCamInfoFormView::GetStepOrient(double &dAngle, bool &bMirror)
{
	if (m_nSelectedStep == -1) return RESULT_BAD;

	auto itSubOrient = m_mapStepOrient.find(m_nSelectedStep);
	if (itSubOrient == m_mapStepOrient.end())
		return RESULT_BAD;

	Orient eOrient = Orient::Mir0Deg;
	CDrawFunction::GetOrient(dAngle, bMirror, eOrient);

	if (itSubOrient->second.size() > 1)
	{
		Orient eSub = itSubOrient->second.at(0);
		for (int i = 1; i < itSubOrient->second.size(); i++)
		{
			if (eSub != itSubOrient->second.at(i))
			{
#ifdef _DEBUG
				AfxMessageBox(L"Multiple Steps Exists in Panel.");
#endif
				dAngle = 0.;
				bMirror = false;
				return RESULT_GOOD;
			}
		}
	}
	else
	{
		eOrient = CDrawFunction::AddOrient(eOrient, itSubOrient->second.at(0));
		CDrawFunction::GetAngle(eOrient, dAngle, bMirror);
	}

	return RESULT_GOOD;
}


UINT32 CCamInfoFormView::EstimateStepOrient()
{
	m_mapStepOrient.clear();

	CJobFile* pJobData = GetJobPtr();
	if (pJobData == nullptr) return RESULT_BAD;

	UserLayerSet_t*	pUserLayerSetInfo =  GetUserLayerSet();
	if (pUserLayerSetInfo == nullptr) return RESULT_BAD;
	if (pUserLayerSetInfo->nPanelStepIdx == -1) return RESULT_BAD;
	if (pUserLayerSetInfo->nPanelStepIdx >= pJobData->m_arrStep.GetCount()) return RESULT_BAD;

	CStep* pStep = pJobData->m_arrStep.GetAt(pUserLayerSetInfo->nPanelStepIdx);
	if (pStep == nullptr) return RESULT_BAD;

	Orient Ori = Orient::NoMir0Deg;

	std::vector<Orient> vcOrient;
	vcOrient.emplace_back(Ori);
	m_mapStepOrient.insert(std::make_pair(pStep->m_nStepID, vcOrient));

	for (UINT i = 0; i < pStep->m_nSubStepCnt; i++)
	{
		CSubStep* pSubStep = pStep->m_arrSubStep[i];
		if (pSubStep == nullptr) continue;

		EstimateSubStepOrient(pSubStep, Ori);
	}

	return RESULT_GOOD;
}

UINT32 CCamInfoFormView::EstimateSubStepOrient(CSubStep* pStep, Orient &eOri)
{
	if (pStep == nullptr) return RESULT_BAD;

	CStepRepeat* pSubStepRepeat = &(pStep->m_arrStepRepeat);
	if (pSubStepRepeat == nullptr)
		return RESULT_BAD;

	enum Orient eSubOrient = Orient::NoMir0Deg;
	if ((pSubStepRepeat->m_dAngle == 0) && (pSubStepRepeat->m_bMirror == false))			eSubOrient = Orient::NoMir0Deg;
	else if ((pSubStepRepeat->m_dAngle == 90) && (pSubStepRepeat->m_bMirror == false))		eSubOrient = Orient::NoMir90Deg;
	else if ((pSubStepRepeat->m_dAngle == 180) && (pSubStepRepeat->m_bMirror == false))		eSubOrient = Orient::NoMir180Deg;
	else if ((pSubStepRepeat->m_dAngle == 270) && (pSubStepRepeat->m_bMirror == false))		eSubOrient = Orient::NoMir270Deg;
	else if ((pSubStepRepeat->m_dAngle == 0) && (pSubStepRepeat->m_bMirror == true))		eSubOrient = Orient::Mir0Deg;
	else if ((pSubStepRepeat->m_dAngle == 90) && (pSubStepRepeat->m_bMirror == true))		eSubOrient = Orient::Mir90Deg;
	else if ((pSubStepRepeat->m_dAngle == 180) && (pSubStepRepeat->m_bMirror == true))		eSubOrient = Orient::Mir180Deg;
	else if ((pSubStepRepeat->m_dAngle == 270) && (pSubStepRepeat->m_bMirror == true))		eSubOrient = Orient::Mir270Deg;
	else eSubOrient = Orient::NoMir0Deg;

	Orient eAddOri = CDrawFunction::AddOrient(eOri, eSubOrient);

	auto itOri = m_mapStepOrient.find(pStep->m_nStepID);
	if (itOri != m_mapStepOrient.end())
	{
		auto itSub = std::find(itOri->second.begin(), itOri->second.end(), eAddOri);
		if (itSub == itOri->second.end())
			itOri->second.emplace_back(eAddOri);
	}
	else
	{
		std::vector<Orient> vcOrient;
		vcOrient.emplace_back(eAddOri);
		m_mapStepOrient.insert(std::make_pair(pStep->m_nStepID, vcOrient));
	}

	for (UINT i = 0; i < pStep->m_nNextStepCnt; i++)
	{
		CSubStep* pSubSubStep = pStep->m_arrNextStep[i];
		if (pSubSubStep == nullptr) continue;

		EstimateSubStepOrient(pSubSubStep, eAddOri);
	}

	return RESULT_GOOD;
}
