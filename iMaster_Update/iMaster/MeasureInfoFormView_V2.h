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
// measurelist define for sem
// #define LIST_ITEM_COLUMN_SEM				0
// #define LIST_TYPE_COLUMN_SEM				1
// #define LIST_OPTIC_COLUMN_SEM				2
// #define LIST_MAG_COLUMN_SEM					3
// #define LIST_ZOOM_COLUMN_SEM				4	
// #define LIST_OOS_MIN_COLUMN_SEM				5
// #define LIST_OOS_MAX_COLUMN_SEM				6
// #define LIST_OOR_MIN_COLUMN_SEM				7
// #define LIST_OOR_MAX_COLUMN_SEM				8
// #define LIST_OOC_MIN_COLUMN_SEM				9
// #define LIST_OOC_MAX_COLUMN_SEM				10
// #define LIST_WIDTHRANGE_COLUMN_SEM			11
// #define LIST_SIDE_COLUMN_SEM				12
// #define LIST_LEFT_COLUMN_SEM				13
// #define LIST_TOP_COLUMN_SEM					14
// #define LIST_RIGHT_COLUMN_SEM				15
// #define LIST_BOT_COLUMN_SEM					16
// #define LIST_FEATURE_TYPE_SEM				17

namespace MeasureList_Sem
{
	enum {
		enumItem = 0,
		enumType,
		enumOptic,
		enumInspSide,
		enumMag,
		enumZoom,
		enumOOS_Min,
		enumOOS_Max,
		enumOOR_Min,
		enumOOR_Max,
		enumOOC_Min,
		enumOOC_Max,
		enumWidthRange,
// 		enumLeft,
// 		enumTop,
// 		enumRight,
// 		enumBottom,
//		enumFeatureType,

		enumItemMax,
	};

	const CString pName[enumItemMax] = {
		_T("ITEM"),
		_T("TYPE"),
		_T("OPTIC"),
		_T("SIDE"),
		_T("MAG"),
		_T("ZOOM"),
		_T("OOS_MIN"),
		_T("OOS_MAX"),
		_T("OOR_MIN"),
		_T("OOR_MAX"),
		_T("OOC_MIN"),
		_T("OOC_MAX"),
		_T("WR"),
// 		_T("LEFT"),
// 		_T("TOP"),
// 		_T("RIGHT"),
// 		_T("BOT"),
//		_T("FEATURE"),

	};

	enum {
		enumListWidth_Type = 130,
		enumListWidth_Other = 60,
	};
}

namespace SpecSet_Sem
{
	enum {
		enumOOS_Min = 0,
		enumOOS_Max,
		enumOOR_Min,
		enumOOR_Max,
		enumOOC_Min,
		enumOOC_Max,

		enumMax,
	};

	

	enum {
		enumItemOOS = 0,
		enumItemOOR,
		enumItemOOC,

		enumItemMax,
	};

	const CString pSubjectName[enumItemMax] = {
		_T("OOS Spec Min/Max"),
		_T("OOR Spec Min/Max"),
		_T("OOC Spec Min/Max"),
	};

	const CString pBtntName[enumItemMax] = {
		_T("Set OOS"),
		_T("Set OOR"),
		_T("Set OOC"),
	};

}


class CDataManager;
//
class CStaticPanel;
class CButtonEx;

class CMeasureInfoFormView_V2 : public CDockablePane
{
	

public:
	CMeasureInfoFormView_V2();

	void ClearMeasureList();

	virtual ~CMeasureInfoFormView_V2();

	UINT32 SetMeasureData();
	

	UINT32 ResetFeatureSelect_UI();
	UINT32 UpdateFeatureSelect_UI();


private:
	void ClearItemList();
	void InitializeList();
	void AdjustLayout();

	UINT32 UpdateComboChange(BOOL bIsItemList, int iSelRow, int iSelCol);
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
	
// 	UINT32 InsertMeasureRow(UINT32 &iRow, CString &strTitle, MeasureItem_t *itPoint);
// 	UINT32 InsertAlignRow(UINT32 &iRow, CString &strTitle, MeasureItem_t *itAlign);
	
	UINT32 _SetMeasureData_Sem();
	
	UINT32 UpdateFeatureSelect_UI_Sem();

	UINT32 _SetMeasureItem(IN PanelInfo_t* pPanelInfo, IN RECTD &drtSelect, IN const eRecipeMode &emRecipeMode, vector<MeasureItem_t*> &vecMeasureItem);
	UINT32 _UpdateMeasureList(vector<MeasureItem_t*> m_vecMeasureItem);//List 내용 업데이트
	UINT32 _UpdateMeasureList_Spec(vector<MeasureItem_t*> m_vecMeasureItem);//List 내용 업데이트
	
	BOOL _MeasureItemInRect(IN RECTD &drtSelect, IN MeasureItem_t *pMeasureItem);

	CLayer *GetCurEditLayer();//현재 편집하는 Layer Pointer//MP, TP
	UINT32 _SetMeasureItem_Attribute(CCustomListCtrl *pMeasureList, IN vector<MeasureItem_t*> &vecMeasureItem);
	
	void _InitialBtn();

	void _InitialSetSpec();

private:
	CCustomListCtrl m_ItemList;
	CHeaderCtrlEx   m_ItemHeaderCtrl;

	CCustomListCtrl m_MeasureList;
	CHeaderCtrlEx   m_MeasureHeaderCtrl;

	CButtonEx		*m_pBtnAccept = nullptr;

	//SpecSet
	//
	CStaticPanel	*m_staticSpecSet[SpecSet_Sem::enumItemMax] = {nullptr, };
	CEdit			*m_editSpecSet[SpecSet_Sem::enumMax] = { nullptr, };
	CButtonEx		*m_BtnSpecSet[SpecSet_Sem::enumItemMax] = { nullptr, };
	//

	UINT32			m_nItemListCount = 0;
	
	CStringArray	m_strOpticArr;
	CStringArray	m_strMagArr;
	CStringArray	m_strZoomArr;
	
	//hj.kim 추가
	//MP 또는 TP Layer에 미리 정의되어 잇는 att가 없다면 Measure Info에서 선택할수 있어야함.
	CStringArray	m_strTypeArr;//MeasureType;
	CStringArray	m_strTypeArr_SR;//MeasureType_SR;
	CStringArray	m_strSideArr;
	//

	
	PanelInfo_t*	m_pPanelInfo = nullptr;
	
	//vector<CFeature *> m_vecFeature;
	vector<MeasureItem_t*> m_vecMeasureItem;//현재 보여지고 있는 MeasureItem;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnItemClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnComboBoxChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEditChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonAccept();
	afx_msg void OnBnClickedButtonSetOOS();
	afx_msg void OnBnClickedButtonSetOOR();
	afx_msg void OnBnClickedButtonSetOOC();
	afx_msg void OnUpdateButton(CCmdUI *pCmdUI);
	DECLARE_MESSAGE_MAP()
};