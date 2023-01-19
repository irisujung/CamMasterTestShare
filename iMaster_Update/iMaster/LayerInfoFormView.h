#pragma once
#include <vector>
#include "Lib/ButtonEx.h"
#include "Lib/CustomListCtrl.h"
#include "Lib/HeaderCtrlEx.h"
#include "../iODB/iODB.h"

#include "CHistogram.h"

using namespace std;

//#define Foreground_Color		RGB(242, 139, 48)
#define Foreground_Color		RGB(51, 153, 255)
#define STEP_LIST_HEIGHT		25
#define STATIC_TXT_HEIGHT		30
#define LIST_STEP_NAME_COL				0
#define LIST_STEP_DEFINE_COL			1

#define LIST_VIEW_COLUMN				0
#define LIST_LAYER_COLUMN				1
#define LIST_DEFINE_COLUMN				2
#define LIST_COLOR_COLUMN				3
// #define LIST_POLARITY_COLUMN			4
// #define LIST_SCALEX_COLUMN				5
// #define LIST_SCALEY_COLUMN				6
/*
#define LIST_SIDE_COLUMN				2
#define LIST_DEFINE_COLUMN				3
#define LIST_COLOR_COLUMN				4
#define LIST_POLARITY_COLUMN			5
#define LIST_SCALEX_COLUMN				6
#define LIST_SCALEY_COLUMN				7
*/
#define CMB_STEP_UNIT					0
#define CMB_STEP_STRIP					1
#define CMB_STEP_PANEL					2


class CHistogram;

class CLayerInfoFormView : public CDockablePane
{

public:
	CLayerInfoFormView();
	virtual ~CLayerInfoFormView();

	/*UINT32 DisplayLayerInfo(SystemSpec_t* pSysSpec, CString strJobPath, 
		std::vector<stStepInfo*>* vcStepInfo, std::vector<stLayerInfo*>* vcLayerInfo, UserLayerSet_t* pUserLayerSet,
		eProdSide prodSide, std::vector<stLayerPair*> *m_vcLayerList, INT32 iFrontlayerIdx = -1, INT32 iBacklayerIdx = -1);*/
	
	UINT32 DisplayLayerInfo(SystemSpec_t* pSysSpec, CString strJobPath,
		vector<stStepInfo*>* vcStepInfo,
		vector<stLayerInfo*>* vcLayerInfo,
		UserLayerSet_t* pUserLayerSet,
		UserSetInfo_t* pUserSetInfo,
		eProdSide prodSide,	
		vector<vector<stLayerInfo*>> pPatternLayerInfo,
		vector<vector<stLayerInfo*>> pSRLayerInfo,
		IN const CString& strFrontLayer,
		IN const CString& strBackLayer);

	UINT32 SetUserLayerInfo();
	UINT32 InitUserLayerInfo();
	CString GetPatternLayer() { return m_strPatternLayer; }
	CString GetSRLayer() { return m_strSRLayer; }
	CString GetDrillLayer() { return m_strDrillLayer; }
	CString GetViaLayer() { return m_strViaLayer; }
	CString GetMeasureLayer() { return m_strMeasureLayer; }
	CString GetThicknessLayer() { return m_strThicknessLayer; }

	UINT32 GetHisto_PatternWidth(IN const int &eLineOption, std::map<INT32, std::vector<CString>> &vcHisto_Pos, std::map<INT32, std::vector<CString>> &vcHisto_Neg);

private:
	void ClearList();
	void InitializeList();
	void AdjustLayout();

	void InitHistogram();
	void ClearHistogram();
	BOOL MakeHistogram(IN const int &nType, IN const int &nLayer);

	void SetHistogram(IN const int &nType, IN const int &nLayer, IN const CString &strLayerName);

	int _GetLayerIndex(IN CTypedPtrArray <CObArray, CLayer*> &arrLayer, IN const CString &strLayer);

	UINT32 DisplayStep_for_NSIS(CString strJobPath, std::vector<stStepInfo*>* vcStepInfo);
	UINT32 DisplayStep_for_AOI(CString strJobPath, std::vector<stStepInfo*>* vcStepInfo);
	UINT32 DisplayStep_for_AVI(CString strJobPath, std::vector<stStepInfo*>* vcStepInfo);

	UINT32 DisplayLayer_for_NSIS(CString strJobPath, std::vector<stStepInfo*>* vcStepInfo,
		std::vector<stLayerInfo*>* vcLayerInfo, eProdSide prodSide, std::vector<stLayerPair*> *m_vcLayerList,
		INT32 iFrontlayerIdx, INT32 iBacklayerIdx);
	/*UINT32 DisplayLayer_for_NSIS_SEM(CString strJobPath, std::vector<stStepInfo*>* vcStepInfo,
		std::vector<stLayerInfo*>* vcLayerInfo, eProdSide prodSide, std::vector<stLayerPair*> *m_vcLayerList,
		INT32 iFrontlayerIdx, INT32 iBacklayerIdx);*/
	UINT32 DisplayLayer_for_NSIS_SEM(CString strJobPath, std::vector<stStepInfo*>* vcStepInfo,
		std::vector<stLayerInfo*>* vcLayerInfo, eProdSide prodSide, CString strFrontlayer, CString strBacklayer);
	UINT32 DisplayLayer_for_NSIS_SEM_V2(CString strJobPath, std::vector<stStepInfo*>* vcStepInfo,
		std::vector<stLayerInfo*>* vcLayerInfo, 
		eProdSide prodSide,		
		UserSetInfo_t* pUserSetInfo,
		CString strFrontLayer,
		CString strBackLayer);
	UINT32 DisplayLayer_for_AOI(CString strJobPath, std::vector<stStepInfo*>* vcStepInfo, std::vector<stLayerInfo*>* vcLayerInfo);
	UINT32 DisplayLayer_for_AVI(CString strJobPath, std::vector<stStepInfo*>* vcStepInfo, std::vector<stLayerInfo*>* vcLayerInfo, INT32 iFrontlayerIdx, INT32 iBacklayerIdx);

	UINT32 SetLayerInfo_for_LGIT(CString strInspLayer);
	UINT32 SetLayerInfo_for_DAEDUCK(CString strInspLayer);
	UINT32 SetLayerInfo_for_DAEDUCK_AVI(CString strInspLayer);

	UINT32 Estimate_Pattern_Width(IN const int &eLineOption, std::map<INT32, std::vector<CString>> &vcHisto_Pos, std::map<INT32, std::vector<CString>> &vcHisto_Neg);
	UINT32 Check_If_SubStep_Exist(CSubStep* pStep, std::vector<INT32>& vcStepExist);
	
	void ClearPTWidthHisto();

private:
	CCustomListCtrl m_LayerList;
	CCustomListCtrl m_StepList;
	CHeaderCtrlEx   m_LayerHeaderCtrl;
	CHeaderCtrlEx   m_StepHeaderCtrl;
	CStatic			m_stInspLayer;
	CComboBox		m_cbInspLayer;

	BOOL			m_bIsDone;
	BOOL			m_bIsStepSet;
	UINT32			m_nStepListCount;
	UINT32          m_nLayerListCount;

	INT32			m_nStartLayerIdx;
	INT32			m_nEndLayerIdx;
	
	CString			m_strPatternLayer;
	CString			m_strSRLayer;
	CString			m_strDrillLayer;
	CString			m_strViaLayer;
	CString			m_strMeasureLayer;
	CString			m_strThicknessLayer;


	CStringArray	m_strStepArr;
	CStringArray	m_strLayerArr;
//	CStringArray	m_strSideArr;
	CStringArray    m_strPolarityArr;

	SystemSpec_t*	m_pSysSpec;
	UserLayerSet_t*	m_pstUserLayerSet;
	UserSetInfo_t* m_pstUserSetInfo;

	vector<vector<stLayerInfo*>> m_pPatternLayerInfo;
	vector<vector<stLayerInfo*>> m_pSRLayerInfo;

	//hj.kim
	int m_nSelectR_Item = -1;
	CString m_strSelectR_LayerName = _T("");
	vector<vector<CHistogram *>> m_vecvecHistogram;
	std::map<INT32, std::vector<FeatureHistoData*>> m_vcHistoPTWidth_Line;	// Pattern Layer Line Width (선폭)
	std::map<INT32, std::vector<FeatureHistoData*>> m_vcHistoPTWidth_Arc;	// Pattern Layer Arc Width (선폭)
		
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnItemClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnComboBoxChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLayerColorChanged(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnLayerItemRClick(WPARAM wparam, LPARAM lparam);
	afx_msg void OnPopMenu_FeatureHisto();//popup menu feature histogram 클릭시 실행
	afx_msg void OnPopMenu_AttributeHisto();//popup menu Attribute histogram 클릭시 실행
	afx_msg void OnPopMenu_SymbolHisto();//popup menu Symbol histogram 클릭시 실행
	afx_msg void OnCbnSelChangeInspLayerComboBox();
	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};

