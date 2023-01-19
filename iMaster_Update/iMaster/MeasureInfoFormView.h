#pragma once
#include <vector>
#include "Lib/ButtonEx.h"
#include "Lib/CustomListCtrl.h"
#include "Lib/HeaderCtrlEx.h"
#include "../iODB/iODB.h"
#include "../iProcess/CDataManager.h"

using namespace std;

//#define Foreground_Color				RGB(242, 139, 48)
#define Foreground_Color				RGB(51, 153, 255)
#define ITEM_LIST_HEIGHT				25
#define STATIC_TXT_HEIGHT				30
#define LIST_ITEM_NAME_COL				0
#define LIST_ITEM_OPTIC_COL				1
#define LIST_ITEM_MAG_COL				2
#define LIST_ITEM_ZOOM_COL				3	

// 2022.04.28 
// 주석추가 김준호
// Measurelist define for daeduck
#define LIST_ITEM_COLUMN				0
#define LIST_TYPE_COLUMN				1
#define LIST_OPTIC_COLUMN				2
#define LIST_MAG_COLUMN					3
#define LIST_ZOOM_COLUMN				4	
#define LIST_MIN_COLUMN					5
#define LIST_TARGET_COLUMN				6
#define LIST_MAX_COLUMN					7
#define LIST_WIDTHRANGE_COLUMN			8
#define LIST_SIDE_COLUMN				9
#define LIST_LEFT_COLUMN				10
#define LIST_TOP_COLUMN					11
#define LIST_RIGHT_COLUMN				12
#define LIST_BOT_COLUMN					13
#define LIST_FEATURE_TYPE				14

#define LIST_TYPE_WIDTH					120
#define LIST_COLUMN_WIDTH				50

class CDataManager;

class CMeasureInfoFormView : public CDockablePane
{

public:
	CMeasureInfoFormView();

	void ClearMeasureList();

	virtual ~CMeasureInfoFormView();

	// 2022.05.02
	// 주석추가 김준호
	// Sem 위해 eRecipeMode 추가 
	UINT32 SetMeasureData();
	

	UINT32 ResetFeatureSelect_UI();
	UINT32 UpdateFeatureSelect_UI();	

private:
	void ClearItemList();
	void InitializeList();
	void AdjustLayout();

	UINT32 UpdateMeasureData(BOOL bIsItemList, int iSelRow, int iSelCol);
	CString GetMeasureTypeString(MeasureType type);
	CString GetMeasureSideString(MeasureSide side);
	CString GetMagnificationString(BOOL bIs3D, INT8 iScanMode);
	CString GetMagnificationString(double dLens);
	CString GetZoomString(BOOL bIs3D, INT8 iScanMode);
	CString GetZoomString(double dZoom);
	INT8 SetScanMode(BOOL bIs3D, CString strMag, CString strZoom);

	void UpdateFeatureSelect(IN const int &nLineIndex, IN const BOOL &bChecked);//List에 Check 정보를 Viewer에 보내어 Select 하도록 한다.
	void ZoomRect(FeatureType &type, RECTD &rcRect);

	CString GetMeasureLayerName();
	UnitInfo_t* GetUnitIndex_In_Panel(INT32 iRow, INT32 iCol, INT32 &iStripIdx, INT32 &iBlockIdx, INT32 &iUnitIdx, RECTD &rcDrawRect);
	UnitInfo_t* GetUnitIndex_In_Strip(INT32 iRow, INT32 iCol, INT32 &iStripIdx, INT32 &iBlockIdx, INT32 &iUnitIdx, RECTD &rcDrawRect);

	UINT32 CheckLensZoom(bool bIsAlign, MeasureItem_t* pItem, double &dMag, double &dZoom);
		

// 	UINT32 Fill_AlignFeature(INT32 iSelRow, INT32 iSelCol, vector<CFeature *>& vecFeature);
// 	UINT32 Fill_NSISMeasureFeature(INT32 iSelRow, INT32 iSelCol, vector<CFeature *>& vecFeature);
// 	UINT32 Fill_SRMeasureFeature(INT32 iSelRow, INT32 iSelCol, vector<CFeature *>& vecFeature);

	UINT32 _SetMeasureData_Daeduck();
	
	UINT32 UpdateFeatureSelect_UI_Daeduck();

private:
	CCustomListCtrl m_ItemList;
	CHeaderCtrlEx   m_ItemHeaderCtrl;

	CCustomListCtrl m_MeasureList;
	CHeaderCtrlEx   m_MeasureHeaderCtrl;

	UINT32			m_nItemListCount = 0;
	UINT32			m_nMeasureListCount = 0;
	
	CStringArray	m_strOpticArr;
	CStringArray	m_strMagArr;
	CStringArray	m_strZoomArr;

	RECTD			m_rcFeature;

	INT32			m_iSelRow = -1;
	INT32			m_iSelCol = -1;

	PanelInfo_t*	m_pPanelInfo = nullptr;
	MeasurePanel_t* m_pMeasureData = nullptr;
	
	vector<CFeature *> m_vecFeature;
	
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnItemClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnComboBoxChange(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};