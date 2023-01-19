#pragma once

#define STRING_AUTOALIGN				_T("AUTO ALIGN")
#define STRING_AUTO_MANUAL_ALIGN		_T("Spec 기준")
#define STRING_AUTO_MANUAL_TRUE			_T("TRUE")
#define STRING_AUTO_MANUAL_FALSE		_T("FALSE")
#define STRING_AUTOALIGN_VALUE			_T("AUTO ALIGN VALUE")
#define STRING_AUTOALIGN_X				_T("AUTO ALIGN X")
#define STRING_AUTOALIGN_Y				_T("AUTO ALIGN Y")


// CAlignMaskEditDlg 대화 상자

class CAlignMaskEditDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAlignMaskEditDlg)

	enum {
		enumSelectPoint_Start = 0,
		enumSelectPoint_End,

		enumSelectPointMax,
	};

public:
	CAlignMaskEditDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CAlignMaskEditDlg();


	UINT32 SetSetlectPoint(IN const int &nIndex, IN const float &fX, IN const float &fY);

	UINT32 SetSetlectFeatureInfo(IN SELECT_INFO *pSelectInfo);

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ALIGN_MASK3 };
#endif

private:	
	CMFCPropertyGridProperty *m_pAlignFlag, *m_pAlignPos[8];	

	void			_InitUI();
	

	void			_InitData();
	UINT32			_InitAlignData();
	void			_UpdateDataToUi();
	void			_UpdateUIToData();

	void			_UpdataPosSelect_Mode(IN const BOOL &bOn);

	void			_SetLayerName();//Combo Box에 선택된 Layer 이름 입력
	CString			_GetLayerName();//Combo Box에서 선택된 Layer 이름 가져오기

	void			_ClearFeatureData();

	UINT32			_MakeRectFeature_ViewCenter(vector<CFeature*> &vecFeatureData);
	UINT32			_MakeSquareFeature_ViewCenter(vector<CFeature*> &vecFeatureData);

	UINT32			_MakeSquareFeature_SelectInfo(IN SELECT_INFO_UNIT &stSelectInfo, vector<CFeature*> &vecFeatureData);

private:
	MaskType		m_eMaskType = MaskType::enumType_Align;
	//MaskEditMode	m_eMaskEditMode = MaskEditMode::enumMode_Add;

// 	float			m_fAddStartX = 0.0f;
// 	float			m_fAddStartY = 0.0f;
// 	float			m_fAddEndX = 0.0f;
// 	float			m_fAddEndY = 0.0f;

	float			m_fResizeWidth = 100.;
	float			m_fResizeHeight = 100.;

	vector<CFeature*>	m_vecFeatureData;

	CComboBox		m_cbLayer;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	LRESULT OnPropertyChanged(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnMaskTypeClicked(UINT32 nID);
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonAutoSearch();
	afx_msg void OnBnClickedButtonTestClear();
	afx_msg void OnBnClickedButtonAddAlignMask();
	afx_msg void OnBnClickedRadioMoveXy();
	afx_msg void OnBnClickedRadioMoveX();
	afx_msg void OnBnClickedRadioMoveY();
	afx_msg void OnBnClickedButtonResize();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedCheckAddAlignMaskFeatureBase();
	afx_msg void OnBnClickedButtonAutoFindAlign();	
	afx_msg void OnBnClickedButtonSaveAlignSpec();	
	afx_msg void OnBnClickedButtonLocationLeftTop();
	afx_msg void OnBnClickedButtonLocationLeftBottom();
	afx_msg void OnBnClickedButtonLocationRightTop();
	afx_msg void OnBnClickedButtonLocationRightBottom();
	CMFCPropertyGridCtrl m_pPropGridCtrlAlign;
	CMFCPropertyGridProperty* _GetAutoAlignFlagData();
	vector<double> _GetAutoAlignPosData();
	BOOL	GetAutoAlignFlagDlg();
	void	SetAutoAlignFlagDlg(INT32 iData);
	double  GetAutoAlignDataDlg(UINT32 index);
	void	SetAutoAlignDataXDlg(UINT32 index, double strData);
	void	SetAutoAlignDataYDlg(UINT32 index, double strData);
	UINT32	ChangeButtonUI();

	INT32	m_iKind = 0;
};
