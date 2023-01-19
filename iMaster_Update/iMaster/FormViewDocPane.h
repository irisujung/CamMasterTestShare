#pragma once

#include <afxdockablepane.h>
#include "JobInfoFormView.h"
#include "CamInfoFormView.h"
#include "SettingsFormView.h"
#include "MiniMapFormView_V2.h"
#include "FeatureEditFormView.h"

#include "MiniMapFormView_V2.h"

class CFormViewDocPane : public CDockablePane
{
private:
	UINT32					m_iFormViewIdx;
	CJobInfoFormView*		m_pWndJobInfo = nullptr;
	CCamInfoFormView*		m_pWndCamInfo = nullptr;
	CSettingsFormView*		m_pWndSettings = nullptr;
	CFeatureEditFormView*   m_pWndFeatureEdit = nullptr;

	CMiniMapFormView_V2*    m_pWndMinimap_V2 = nullptr;

private:
	void	AdjustLayout();

public:
	void    LockJobTree(BOOL bEnable);
	void	InitialUpdate();

	UINT32  SetJobPath(CString strJobPath);
	UINT32	SetStepCount(UINT32& iStepCnt, std::vector<UINT32>& vcStepRepeatCnt);
	UINT32	GetStepInfo(std::vector<stStepInfo*>** v);
	UINT32	DisplayCamInfo();
	UINT32	EstimateStepOrient();
	CString GetWorkStepName();

	UINT32	SetLayerCount(UINT32& iLayerCnt);
	UINT32  GetLayerInfo(std::vector<stLayerInfo*>** v);
	UINT32  CheckLayerInfo(std::vector<stLayerInfo*>** v,
		vector<vector<stLayerInfo*>> &vecPatternList,
		vector<vector<stLayerInfo*>> &vecSRList,
		vector<vector<stLayerInfo*>> &vecMPList,
		vector<vector<stLayerInfo*>> &vecTPList);

	UINT32  SetComponentCount(UINT32& iStepCnt, std::vector<UINT32>& vcCompCnt);
	UINT32  GetComponentInfo(std::vector<stComponentInfo*>** v);

	INT32	GetCurStepIndex();
	BOOL	SetODBFile(CString csODBFile);
	CStringArray* GetLayerList(int idx);

	UINT32  LoadSpecData();

	UINT32  LoadFeatureData();
	UINT32  LoadMoveData();
	UINT32	LoadMove_KeyData();

	UINT32	Refresh_JobList();
	UINT32	ChangeStep(int uiMenuCmd);

public:
	void	SetFormId(UINT32 idx) { m_iFormViewIdx = idx; }

	CFormViewDocPane() : m_iFormViewIdx(0), m_pWndJobInfo(nullptr), m_pWndCamInfo(nullptr), m_pWndSettings(nullptr),m_pWndFeatureEdit(nullptr) {};
	~CFormViewDocPane() {};

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
};