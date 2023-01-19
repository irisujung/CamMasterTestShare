#pragma once


// CHistogram 대화 상자

#include "Lib/ColumnTreeCtrl.h"
#include "../iDxDraw/DxDraw/Def_Viewer.h"



typedef struct _HistoData_Sym
{
	CString strName;
	int nCount;

	_HistoData_Sym()
	{
		_Reset();
	}

	void _Reset()
	{
		strName = _T("");
		nCount = 0;
	}


	BOOL Compare(IN const _HistoData_Sym &in_HistoData)
	{
		if (strName == in_HistoData.strName )
		{
			return TRUE;
		}

		return FALSE;
	}

}HistoData_Sym;

typedef struct _SymHistoData
{
	CString strTypeName;
	vector<HistoData_Sym> vecHistoData;

}SymHistoData;

typedef struct _HistoData_Att
{
	vector<CString> vecName;
	int nCount;

	_HistoData_Att()
	{
		_Reset();
	}

	void _Reset()
	{
		vecName.clear();
		nCount = 0;
	}


	BOOL Compare(IN const _HistoData_Att &in_HistoData)
	{
		int nSize = static_cast<int>(vecName.size());
		int In_nSize = static_cast<int>(in_HistoData.vecName.size());
		if (nSize != In_nSize)
		{
			return FALSE;
		}

		for (int i = 0; i < nSize; i++)
		{
			if (vecName[i] != in_HistoData.vecName[i])
			{
				return FALSE;
			}
		}

		return TRUE;
	}

	CString GetAttNames()
	{
		CString strNames = _T("");
		int nSize = static_cast<int>(vecName.size());
		for (int i = 0; i < nSize; i++)
		{
			if (i != 0)
			{
				strNames += _T(",");
			}

			strNames += vecName[i];
		}

		return strNames;
	}

}HistoData_Att;

typedef struct _AttHistoData
{
	CString strTypeName;
	vector<HistoData_Att> vecHistoData;

}AttHistoData;

typedef struct _HistoData_Feature
{
	SymbolName emSymType;
	CString strName;
	BOOL bPositive;
	int nCount;
	vector<CString> vecAttribute;
	CString strSymbol;

	_HistoData_Feature()
	{
		_Reset();
	}

	void _Reset()
	{
		emSymType = SymbolName::SymbolNameNone;
		strName = _T("");
		nCount = 0;
		vecAttribute.clear();
		strSymbol = _T("");
	}

	BOOL CompareAttribute(IN const vector<CString> &in_vecAttribute)
	{
		int nMySize = static_cast<int>(vecAttribute.size());
		int nInSize = static_cast<int>(in_vecAttribute.size());
		if (nMySize != nInSize)
		{
			return FALSE;
		}

		for (int i = 0; i < nMySize; i++)
		{
			if (vecAttribute[i] != in_vecAttribute[i])
			{
				return FALSE;
			}
		}

		return TRUE;
	}

	BOOL Compare(IN const _HistoData_Feature &in_HistoData)
	{
		if (strName == in_HistoData.strName &&
			CompareAttribute(in_HistoData.vecAttribute) &&
			strSymbol == in_HistoData.strSymbol &&
			bPositive == in_HistoData.bPositive)
		{
			return TRUE;
		}

		return FALSE;
	}

	BOOL CompareName(IN const _HistoData_Feature &in_HistoData)
	{
		if (strName == in_HistoData.strName)
		{
			return TRUE;
		}

		return FALSE;
	}


}HistoData_Feature;

typedef struct _FeatureHistoData
{
	CString strTypeName;
	vector<HistoData_Feature> vecHistoData;

	_FeatureHistoData() { Clear(); }
	~_FeatureHistoData() { }
	void Clear()
	{
		strTypeName = L"";
		vecHistoData.clear();
	}
	_FeatureHistoData(const _FeatureHistoData& m)
	{
		Clear();

		strTypeName = m.strTypeName;
		for (auto it : m.vecHistoData)
			vecHistoData.emplace_back(it);
	}
	_FeatureHistoData& operator=(const _FeatureHistoData& m)
	{
		if (this != &m)
		{
			this->Clear();

			this->strTypeName = m.strTypeName;
			for (auto it : m.vecHistoData)
				this->vecHistoData.emplace_back(it);
		}
		return *this;
	};

}FeatureHistoData;


class CHistogram : public CDialogEx
{
	DECLARE_DYNAMIC(CHistogram)

public:
	CHistogram(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CHistogram();

	void SetUpdateUI(BOOL b) { m_bUpdateUI = b; }
	void SetLayerPtr(CLayer* pLayer, eDrawOption drawOp = eDrawOption::eAll);
	void SetHistoType(int nType);

	void SetStepName(IN const CString &strName);
	void SetLayerName(IN const CString &strName);
	CString GetLayerName();

	vector<FeatureHistoData>*	GetFeatureHisto() { return &m_vecHistoData; }
	vector<AttHistoData>*		GetAttributeHisto() { return &m_vecAttData; }
	vector<SymHistoData>*		GetSymbolHisto() { return &m_vecSymbolData; }

private:
	void _ClearHistoData_Feature();
	void _ClearHistoData_Att();
	void _ClearHistoData_Symbol();
	void _SetHistoData_Feature(eDrawOption drawOp);
	void _SetHistoData_Attribute();
	void _SetHistoData_Symbol();

	void _SortHistoData_Feature(vector<FeatureHistoData>	&vecHistoData);
	void _SortHistoData_Symbol(vector<SymHistoData>		&vecSymbolData);
	
	LONGLONG _GetStringNum(IN const CString &strName);
	

	void _InitHistoTree();
	void _ClearHistoTree();
	void _UpDateHistoTree_Feature();
	void _UpDateHistoTree_Att();
	void _UpDateHistoTree_Symbol();
	
	//Feature type <-> string name 변환
	CString ConvFeatureType(enum FeatureType eFeatureType);
	enum FeatureType ConvFeatureString(CString &strFeature);

	void _GetSelectInfo(vector<FEATURE_INFO> &vecFeatureInfo);

	void SetFeatureInfo(vector<CString> &vecParent, vector<CString> &vecInfo, vector<FEATURE_INFO> &vecFeatureInfo);

private:

	//HistoData
	vector<FeatureHistoData>	m_vecHistoData;//[FeatureType]
	vector<AttHistoData>		m_vecAttData;//[FeatureType]
	vector<SymHistoData>		m_vecSymbolData;//[FeatureType]
	

	BOOL						m_bUpdateUI = TRUE;

	//Control
	CColumnTreeCtrl m_columnTree;
	HTREEITEM m_hRoot;

	//
	int m_nHistoType = -1;

	CString m_strStepName = _T("");
	CString m_strLayerName = _T("");
	
	

	//넘겨 받은 포인터
	//지우지 말것
	CLayer* m_pLayer = nullptr;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HISTOGRAM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();


	template <class T>
	HistoData_Feature ConvFeatureDataToHistoData(T *pFeatureData, const int &nSize)
	{
		HistoData_Feature stHistoData;

		if (pFeatureData->m_pSymbol != nullptr)
		{
			stHistoData.emSymType = pFeatureData->m_pSymbol->m_eSymbolName;//GetSymbolName(static_cast<int>(pFeatureData->m_pSymbol->m_eSymbolName));
			//stHistoData.strName.Format(_T("%.3lf"), pFeatureData->m_pSymbol->m_MinMaxRect.width());
			stHistoData.strName.Format(_T("%s"), pFeatureData->m_pSymbol->m_strUserSymbol);
			stHistoData.strSymbol = pFeatureData->m_pSymbol->m_strUserSymbol;
		}
		else
		{//Symbol이 없으면 Surface
			//stHistoData.nSymType = -1;
			CString strNum;
			strNum.Format(_T("%03d"), nSize + 1);
			stHistoData.strName = strNum;//_T("Surface");
		}

		stHistoData.nCount = 1;
		stHistoData.bPositive = pFeatureData->m_bPolarity;

		stHistoData.vecAttribute.clear();
		CAttributeTextString *pTextString;
		int nAttCount = static_cast<int>(pFeatureData->m_arrAttributeTextString.GetCount());
		for (int i = 0; i < nAttCount; i++)
		{
			pTextString = pFeatureData->m_arrAttributeTextString.GetAt(i);
			if (pTextString == nullptr ) continue;
			stHistoData.vecAttribute.push_back(pTextString->m_strText);
		}

		

		return stHistoData;
	}

	template <class T>
	void InsertHistoData_Feature(vector<HistoData_Feature> &vecHistoData, T *pFeatureData)
	{
		HistoData_Feature stHistoData;
		if (vecHistoData.size() == 0)
		{//기존에 없으면 추가
			vecHistoData.push_back(ConvFeatureDataToHistoData(pFeatureData, static_cast<int>(vecHistoData.size())));
		}
		else
		{
			stHistoData = ConvFeatureDataToHistoData(pFeatureData, static_cast<int>(vecHistoData.size()));

			BOOL bFind = FALSE;
			int nCount = static_cast<int>(vecHistoData.size());
			for (int i = 0; i < nCount; i++)
			{
				
				if (vecHistoData[i].Compare(stHistoData) == TRUE)
				{
					vecHistoData[i].nCount++;
					bFind = TRUE;
					break;
				}
			}

			if (bFind == FALSE)
			{
				vecHistoData.push_back(stHistoData);
			}
		}
	}

	void InsertHistoData_Att(vector<HistoData_Att> &vecHistoData, CFeature *pData)
	{
		HistoData_Att stHistoData;
		stHistoData._Reset();
		//SetData
		int nSize = static_cast<int>(pData->m_arrAttributeTextString.GetSize());
		if (nSize == 0)
		{
			stHistoData.vecName.push_back(DEF_HISTOGRAM::strAttNull);
		}


		for (int i = 0; i < nSize; i++)
		{
			CAttributeTextString* pTextString = pData->m_arrAttributeTextString.GetAt(i);
			if(pTextString == nullptr ) continue;
			stHistoData.vecName.push_back(pTextString->m_strText);
		}

		//기존 히스토그램 데이터가 없다면 셋을 먼저 한다.
		if (vecHistoData.size() == 0)
		{
			stHistoData.nCount = 1;

			vecHistoData.push_back(stHistoData);
		}
		else
		{
			BOOL bFind = FALSE;

			int nCount = static_cast<int>(vecHistoData.size());
			for (int i = 0; i < nCount; i++)
			{
				if (vecHistoData[i].Compare(stHistoData) == TRUE)
				{
					vecHistoData[i].nCount++;
					bFind = TRUE;
					break;
				}
			}

			//중복된것이 없다면 새로 생성
			if (bFind == FALSE)
			{
				stHistoData.nCount = 1;
				vecHistoData.push_back(stHistoData);
			}
		}
	}

	void InsertHistoData_Symbol(vector<HistoData_Sym> &vecHistoData, CFeature *pData)
	{
		HistoData_Sym stHistoData;
		if (pData->m_pSymbol == nullptr)
		{
			stHistoData.strName = DEF_HISTOGRAM::strSymNull;
		}
		else
		{
			stHistoData.strName = pData->m_pSymbol->m_strUserSymbol;
		}


		if (vecHistoData.size() == 0)
		{
			
			stHistoData.nCount = 1;

			vecHistoData.push_back(stHistoData);
		}
		else
		{
			BOOL bFind = FALSE;

			int nCount = static_cast<int>(vecHistoData.size());
			for (int i = 0; i < nCount; i++)
			{
				if (vecHistoData[i].Compare(stHistoData) == TRUE)
				{
					vecHistoData[i].nCount++;
					bFind = TRUE;
					break;
				}
			}

			if (bFind == FALSE)
			{
				stHistoData.nCount = 1;

				vecHistoData.push_back(stHistoData);
			}
		}
	}
	afx_msg void OnBnClickedButtonResetSelect();
	afx_msg void OnBnClickedButtonAllShow();
	afx_msg void OnBnClickedButtonClose();
	afx_msg void OnBnClickedButtonSelect();
	afx_msg void OnBnClickedButtonUnselect();
	afx_msg void OnBnClickedButtonShow();
	afx_msg void OnBnClickedButtonHide();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

