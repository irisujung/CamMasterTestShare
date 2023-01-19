#pragma once

#include "Lib/ButtonEx.h"
#include "Lib/HeaderCtrlEx.h"
#include "../iCommonIF/CommonIF-Struct.h"
#include "../iODB/iODB.h"

// CCamInfoFormView 폼 보기

class CCamInfoFormView : public CFormView
{
	DECLARE_DYNCREATE(CCamInfoFormView)

protected:
	CCamInfoFormView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CCamInfoFormView();

private:
	CListCtrl		m_StepList;
	CListCtrl		m_LayerList;
	CListCtrl		m_CompList;

	CHeaderCtrlEx   m_StepHeaderCtrl;
	CHeaderCtrlEx   m_LayerHeaderCtrl;
	CHeaderCtrlEx	m_CompHeaderCtrl;


	std::vector<CButtonEx*> m_vcSteps;


	std::map<INT32, std::vector<Orient>> m_mapStepOrient;
	
protected:
	BOOL			m_bInit = FALSE;
	BOOL			m_bStepBtnCreated;
	UINT32			m_nStepNum;
	UINT32			m_nLayerNum;
	INT32			m_nSelectedStep;

	CStringArray	m_strArrLayerList[2];
	vector<vector<CString>> m_vecLayerList[2];

	std::vector<stStepInfo*>		m_vcStepInfo;
	std::vector<stLayerInfo*>		m_vcLayerInfo;
	std::vector<stComponentInfo*>	m_vcCompInfo;
//	CStep*			m_pStep;
//	CMatrix*		m_pMatrix;

	

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CAMINFO_FORM };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	static CCamInfoFormView* CreateOne(CWnd* pParent);
	UINT32 SetStepCount(UINT32& iStepCnt, std::vector<UINT32>& vcStepRepeatCnt);
	std::vector<stStepInfo*>* GetStepInfo() { return &m_vcStepInfo; }
	UINT32 SetLayerCount(UINT32& iLayerCnt);
	std::vector<stLayerInfo*>* GetLayerInfo() { return &m_vcLayerInfo; }
	UINT SetComponentCount(UINT32& iStepCnt, std::vector<UINT32>& vcCompCnt);
	std::vector<stComponentInfo*>* GetComponentInfo() { return &m_vcCompInfo; }
	UINT32 CheckLayerInfo(std::vector<stLayerInfo*>* v);
	UINT32 CheckLayerInfo_V2(std::vector<stLayerInfo*>* v,
		vector<vector<stLayerInfo*>> &vecPatternLayer,
		vector<vector<stLayerInfo*>> &vecSRLayer,
		vector < vector<stLayerInfo*>> &vecMPLayer,
		vector < vector<stLayerInfo*>> &vecTPLayer);
	CStringArray* GetLayerList(int idx) { return &m_strArrLayerList[idx]; }

	UINT32 DisplayCamInfo(bool bLayout = true);
	CString GetWorkStepName();

	UINT32 SetCurStepIndex();
	INT32  GetCurStepIndex() { return m_nSelectedStep; }

	UINT32 ChangeStep(int uiMenuCmd);

	UINT32		EstimateStepOrient();

private:
	void ClearStepButtons();
	UINT32 CreateStepButtons();
	void ClearLayerInfo();
	UINT32 DisplayStepList(UINT32 iStepIdx = 0);
	void SetItem(CListCtrl& ctrl, int nItem, int nSubItem, LPCTSTR lpszItem);
	UINT32 DisplayLayerList();
	void ClearComponentInfo();
	UINT32 DisplayComponentList(UINT32 iStepIdx = 0);

	//Panel에서 보여지는 대로 현재의 Step의 보이는 방향을 설정
	UINT32		GetStepOrient(double &dAngle, bool &bMirror);
	//
	
	UINT32		EstimateSubStepOrient(CSubStep* pStep, Orient &eOri);
	
	//
	UINT32		_UpdateUI_Pos();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnStepButtonClicked(UINT32 nID);
	
};


