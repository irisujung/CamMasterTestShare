// CHistogram.cpp: 구현 파일
//

#include "pch.h"
#include "iMaster.h"
#include "MainFrm.h"
#include "CHistogram.h"
#include "afxdialogex.h"

// CHistogram 대화 상자

IMPLEMENT_DYNAMIC(CHistogram, CDialogEx)

CHistogram::CHistogram(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HISTOGRAM, pParent)
{
}

CHistogram::~CHistogram()
{
}

void CHistogram::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_HISTOGRAM, m_columnTree);
}

BEGIN_MESSAGE_MAP(CHistogram, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_RESET_SELECT, &CHistogram::OnBnClickedButtonResetSelect)
	ON_BN_CLICKED(IDC_BUTTON_ALL_SHOW, &CHistogram::OnBnClickedButtonAllShow)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CHistogram::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_SELECT, &CHistogram::OnBnClickedButtonSelect)
	ON_BN_CLICKED(IDC_BUTTON_UNSELECT, &CHistogram::OnBnClickedButtonUnselect)
	ON_BN_CLICKED(IDC_BUTTON_SHOW, &CHistogram::OnBnClickedButtonShow)
	ON_BN_CLICKED(IDC_BUTTON_HIDE, &CHistogram::OnBnClickedButtonHide)
END_MESSAGE_MAP()


// CHistogram 메시지 처리기

void CHistogram::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow == TRUE)
	{

	}
}


BOOL CHistogram::OnInitDialog()
{
	CDialogEx::OnInitDialog();
		
	m_bUpdateUI = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CHistogram::SetLayerPtr(CLayer * pLayer, eDrawOption drawOp)
{
	m_pLayer = pLayer;

	
	switch (m_nHistoType)
	{
	default:
	case DEF_HISTOGRAM::Type_Feature :
		_SetHistoData_Feature(drawOp);
		_SortHistoData_Feature(m_vecHistoData);
		if (m_bUpdateUI) _UpDateHistoTree_Feature();
		break;
	case DEF_HISTOGRAM::Type_Attribute:
		_SetHistoData_Attribute();
		if(m_bUpdateUI) _UpDateHistoTree_Att();
		break;
	case DEF_HISTOGRAM::Type_Symbol:
		_SetHistoData_Symbol();
		_SortHistoData_Symbol(m_vecSymbolData);
		if (m_bUpdateUI) _UpDateHistoTree_Symbol();
		break;
	}
	
}



void CHistogram::SetHistoType(int nType)
{
	if (nType < 0) nType = 0;
	if (nType >= DEF_HISTOGRAM::TypeMax) nType = DEF_HISTOGRAM::TypeMax - 1;

	m_nHistoType = nType;

	if (m_bUpdateUI)	
		SetWindowText(DEF_HISTOGRAM::strName[m_nHistoType]);

	_InitHistoTree();	
}

void CHistogram::SetStepName(IN const CString &strName)
{
	m_strStepName = strName;
}

void CHistogram::SetLayerName(IN const CString &strName)
{
	m_strLayerName = strName;
}

CString CHistogram::GetLayerName()
{
	return m_strLayerName;
}

void CHistogram::_ClearHistoData_Feature()
{
	int nCount = static_cast<int>(m_vecHistoData.size());
	for (int i = 0; i < nCount; i++)
	{
		m_vecHistoData[i].strTypeName = _T("");
		int nDataCount = static_cast<int>(m_vecHistoData[i].vecHistoData.size());
		for (int j = 0; j < nDataCount; j++)
		{
			m_vecHistoData[i].vecHistoData[j]._Reset();
		}
		m_vecHistoData[i].vecHistoData.clear();
	}

	m_vecHistoData.clear();

	CString strName;
	m_vecHistoData.resize(static_cast<int>(FeatureType::FeatureTypeNone));
	for (int i = 0; i < static_cast<int>(FeatureType::FeatureTypeNone) ; i++)
	{
		strName = ConvFeatureType(static_cast<FeatureType>(i));

		m_vecHistoData[i].strTypeName = strName;
	}

	//////////////////////////////////////////////////////////////////////////
	
}

void CHistogram::_ClearHistoData_Att()
{
	int nCount = static_cast<int>(m_vecAttData.size());
	for (int i = 0; i < nCount; i++)
	{
		m_vecAttData[i].strTypeName = _T("");
		int nDataCount = static_cast<int>(m_vecAttData[i].vecHistoData.size());
		for (int j = 0; j < nDataCount; j++)
		{
			m_vecAttData[i].vecHistoData[j]._Reset();
		}
		m_vecAttData[i].vecHistoData.clear();
	}

	m_vecAttData.clear();

	CString strName;
	m_vecAttData.resize(static_cast<int>(FeatureType::FeatureTypeNone));
	for (int i = 0; i < static_cast<int>(FeatureType::FeatureTypeNone); i++)
	{
		strName = ConvFeatureType(static_cast<FeatureType>(i));

		m_vecAttData[i].strTypeName = strName;
	}

}

void CHistogram::_ClearHistoData_Symbol()
{
	int nCount = static_cast<int>(m_vecSymbolData.size());
	for (int i = 0; i < nCount; i++)
	{
		m_vecSymbolData[i].strTypeName = _T("");
		int nDataCount = static_cast<int>(m_vecSymbolData[i].vecHistoData.size());
		for (int j = 0; j < nDataCount; j++)
		{
			m_vecSymbolData[i].vecHistoData[j]._Reset();
		}
		m_vecSymbolData[i].vecHistoData.clear();
	}

	m_vecSymbolData.clear();

	CString strName;
	m_vecSymbolData.resize(static_cast<int>(FeatureType::FeatureTypeNone));
	for (int i = 0; i < static_cast<int>(FeatureType::FeatureTypeNone); i++)
	{
		strName = ConvFeatureType(static_cast<FeatureType>(i));

		m_vecSymbolData[i].strTypeName = strName;
	}
}

void CHistogram::_SetHistoData_Feature(eDrawOption drawOp)
{
	if (m_pLayer == nullptr) return;
	
	_ClearHistoData_Feature();

	if (m_bUpdateUI)
	{
		GetDlgItem(IDC_STATIC_STEPNAME)->SetWindowText(_T("Step : ") + m_strStepName);
		GetDlgItem(IDC_STATIC_LAYERNAME)->SetWindowText(_T("Layer : ") + m_pLayer->m_strLayerName);
	}
	
	INT_PTR nFeatureCount = m_pLayer->m_FeatureFile.m_arrFeature.GetCount();
	for (int f = 0; f < nFeatureCount; f++)
	{
		CFeature* pFeature = m_pLayer->m_FeatureFile.m_arrFeature.GetAt(f);
		if (pFeature == nullptr) continue;

		if (drawOp != eDrawOption::eAll)
		{
			switch (drawOp)
			{
			case eDrawOption::eLineOnly:
			{
				if (pFeature->m_eType != FeatureType::L)
					continue;
			}
			break;

			case eDrawOption::eArcOnly:
			{
				if (pFeature->m_eType != FeatureType::A)
					continue;
			}
			break;

			case eDrawOption::eLineArcOnly:
			{
				if (pFeature->m_eType != FeatureType::L &&
					pFeature->m_eType != FeatureType::A)
					continue;
			}
			break;

			case eDrawOption::ePadOnly:
			{
				if (pFeature->m_eType != FeatureType::P)
					continue;
			}
			break;

			case eDrawOption::eTextOnly:
			{
				if (pFeature->m_eType != FeatureType::T)
					continue;
			}
			break;

			case eDrawOption::eSurfaceOnly:
			{
				if (pFeature->m_eType != FeatureType::S)
					continue;
			}
			break;
			}
		}

		switch (pFeature->m_eType)
		{
		case FeatureType::Z:
		{
			
			CFeatureZ *pFeatureTp = static_cast<CFeatureZ*>(pFeature);

			InsertHistoData_Feature(m_vecHistoData[static_cast<int>(pFeature->m_eType)].vecHistoData, pFeatureTp);

		}
		break;
		case FeatureType::L://Line
		{
			CFeatureL *pFeatureLine = static_cast<CFeatureL*>(pFeature);

			InsertHistoData_Feature(m_vecHistoData[static_cast<int>(pFeature->m_eType)].vecHistoData, pFeatureLine);
		}
		break;
		case FeatureType::P://Pad
		{
			CFeatureP *pFeaturePad = static_cast<CFeatureP*>(pFeature);
			
			InsertHistoData_Feature(m_vecHistoData[static_cast<int>(pFeature->m_eType)].vecHistoData, pFeaturePad);
		}
		break;
		case FeatureType::A://Arc
		{
			CFeatureA *pFeatureArc = static_cast<CFeatureA*>(pFeature);

			InsertHistoData_Feature(m_vecHistoData[static_cast<int>(pFeature->m_eType)].vecHistoData, pFeatureArc);
		}
		break;
		case FeatureType::T://Text
		{
			CFeatureT *pFeatureText = static_cast<CFeatureT*>(pFeature);

			InsertHistoData_Feature(m_vecHistoData[static_cast<int>(pFeature->m_eType)].vecHistoData, pFeatureText);
		}
		break;
		case FeatureType::S:
		{
			CFeatureS *pFeatureSurface = static_cast<CFeatureS*>(pFeature);

			InsertHistoData_Feature(m_vecHistoData[static_cast<int>(pFeature->m_eType)].vecHistoData, pFeatureSurface);

		}
		break;
		default:
			break;
		}

	}
}

void CHistogram::_SetHistoData_Attribute()
{
	if (m_pLayer == nullptr) return;

	_ClearHistoData_Att();

	if (m_bUpdateUI)
	{
		GetDlgItem(IDC_STATIC_STEPNAME)->SetWindowText(_T("Step : ") + m_strStepName);
		GetDlgItem(IDC_STATIC_LAYERNAME)->SetWindowText(_T("Layer : ") + m_pLayer->m_strLayerName);
	}

	INT_PTR nAttributeCount = m_pLayer->m_FeatureFile.m_arrAttribute.GetCount();
	INT_PTR nFeatureCount = m_pLayer->m_FeatureFile.m_arrFeature.GetCount();
	for (int i = 0; i < nFeatureCount; i++)
	{
		CFeature* pFeature = m_pLayer->m_FeatureFile.m_arrFeature.GetAt(i);

		switch (pFeature->m_eType)
		{
		case FeatureType::Z:
		{
			CFeatureZ *pFeatureTp = static_cast<CFeatureZ*>(pFeature);
			InsertHistoData_Att(m_vecAttData[static_cast<int>(pFeature->m_eType)].vecHistoData, pFeatureTp);
		}
		break;
		case FeatureType::L://Line
		{
			CFeatureL *pFeatureLine = static_cast<CFeatureL*>(pFeature);

			InsertHistoData_Att(m_vecAttData[static_cast<int>(pFeature->m_eType)].vecHistoData, pFeatureLine);
		}
		break;
		case FeatureType::P://Pad
		{
			CFeatureP *pFeaturePad = static_cast<CFeatureP*>(pFeature);

			InsertHistoData_Att(m_vecAttData[static_cast<int>(pFeature->m_eType)].vecHistoData, pFeaturePad);
		}
		break;
		case FeatureType::A://Arc
		{
			CFeatureA *pFeatureArc = static_cast<CFeatureA*>(pFeature);

			InsertHistoData_Att(m_vecAttData[static_cast<int>(pFeature->m_eType)].vecHistoData, pFeatureArc);
		}
		break;
		case FeatureType::T://Text
		{
			CFeatureT *pFeatureText = static_cast<CFeatureT*>(pFeature);

			InsertHistoData_Att(m_vecAttData[static_cast<int>(pFeature->m_eType)].vecHistoData, pFeatureText);
		}
		break;
		case FeatureType::S:
		{
			CFeatureS *pFeatureSurface = static_cast<CFeatureS*>(pFeature);

			InsertHistoData_Att(m_vecAttData[static_cast<int>(pFeature->m_eType)].vecHistoData, pFeatureSurface);

		}
		break;
		default:
			break;
		}

	}
}

void CHistogram::_SetHistoData_Symbol()
{
	if (m_pLayer == nullptr) return;

	_ClearHistoData_Symbol();

	if (m_bUpdateUI)
	{
		GetDlgItem(IDC_STATIC_STEPNAME)->SetWindowText(_T("Step : ") + m_strStepName);
		GetDlgItem(IDC_STATIC_LAYERNAME)->SetWindowText(_T("Layer : ") + m_pLayer->m_strLayerName);
	}

	INT_PTR nFeatureCount = m_pLayer->m_FeatureFile.m_arrFeature.GetCount();
	for (int i = 0; i < nFeatureCount; i++)
	{
		CFeature* pFeature = m_pLayer->m_FeatureFile.m_arrFeature.GetAt(i);

		switch (pFeature->m_eType)
		{
		case FeatureType::L://Line
		{
			CFeatureL *pFeatureLine = static_cast<CFeatureL*>(pFeature);

			InsertHistoData_Symbol(m_vecSymbolData[static_cast<int>(pFeature->m_eType)].vecHistoData, pFeatureLine);
		}
		break;
		case FeatureType::P://Pad
		{
			CFeatureP *pFeaturePad = static_cast<CFeatureP*>(pFeature);

			InsertHistoData_Symbol(m_vecSymbolData[static_cast<int>(pFeature->m_eType)].vecHistoData, pFeaturePad);
		}
		break;
		case FeatureType::A://Arc
		{
			CFeatureA *pFeatureArc = static_cast<CFeatureA*>(pFeature);

			InsertHistoData_Symbol(m_vecSymbolData[static_cast<int>(pFeature->m_eType)].vecHistoData, pFeatureArc);
		}
		break;
		case FeatureType::T://Text
		{
			CFeatureT *pFeatureText = static_cast<CFeatureT*>(pFeature);

			InsertHistoData_Symbol(m_vecSymbolData[static_cast<int>(pFeature->m_eType)].vecHistoData, pFeatureText);
		}
		break;
		case FeatureType::S:
		{
			CFeatureS *pFeatureSurface = static_cast<CFeatureS*>(pFeature);

			InsertHistoData_Symbol(m_vecSymbolData[static_cast<int>(pFeature->m_eType)].vecHistoData, pFeatureSurface);

		}
		break;
		default:
			break;
		}
	}
}

void CHistogram::_InitHistoTree()
{
	if (!m_bUpdateUI) return;

	// set style for tree view
	UINT uTreeStyle = TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS;
	m_columnTree.GetTreeCtrl().ModifyStyle(0, uTreeStyle);

	switch (m_nHistoType)
	{
	default:
	case DEF_HISTOGRAM::Type_Feature:
		m_columnTree.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 120);
		m_columnTree.InsertColumn(1, _T("Neg/Pos"), LVCFMT_CENTER, 70);
		m_columnTree.InsertColumn(2, _T("Count"), LVCFMT_CENTER, 50);
		m_columnTree.InsertColumn(3, _T("Attribute"), LVCFMT_CENTER, 100);
		m_columnTree.InsertColumn(4, _T("Symbol"), LVCFMT_CENTER, 100);
		break;

	case DEF_HISTOGRAM::Type_Attribute:
		m_columnTree.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 220);
		m_columnTree.InsertColumn(1, _T("Count"), LVCFMT_CENTER, 220);
		break;
	case DEF_HISTOGRAM::Type_Symbol:
		m_columnTree.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 220);
		m_columnTree.InsertColumn(1, _T("Count"), LVCFMT_CENTER, 220);
		break;
	}
}

void CHistogram::_ClearHistoTree()
{
	m_columnTree.GetTreeCtrl().DeleteAllItems();
}

void CHistogram::_UpDateHistoTree_Feature()
{
	if (!m_bUpdateUI) return;

	_ClearHistoTree();

	int nCount = static_cast<int>(m_vecHistoData.size());

	vector<CString> vecPolarity;
	vecPolarity.push_back(_T("Neg"));
	vecPolarity.push_back(_T("Pos"));


	HTREEITEM hRootTotal, hRoot, hItem;
	HistoData_Feature *pHistoData;
	CString strTemp;
	
	int nTotalCount = 0;
	int nTypeCount = 0;
	hRootTotal = m_columnTree.GetTreeCtrl().InsertItem(_T("Total"));

	for ( int i = 0 ; i < nCount ; i++)
	{
		int nHistoCount = static_cast<int>(m_vecHistoData[i].vecHistoData.size());
		if (nHistoCount == 0) continue;

		hRoot = m_columnTree.GetTreeCtrl().InsertItem(m_vecHistoData[i].strTypeName);
		nTypeCount = 0;
		for (int j = 0; j < nHistoCount; j++)
		{
			pHistoData = &m_vecHistoData[i].vecHistoData[j];

			//Name
			//hItem = m_columnTree.GetTreeCtrl().InsertItem(GetSymbolName(pHistoData->emSymType) + pHistoData->strName, hRoot);
			hItem = m_columnTree.GetTreeCtrl().InsertItem(pHistoData->strName, hRoot);
			
			//Neg/Pos
			m_columnTree.SetItemText(hItem, 1, vecPolarity[pHistoData->bPositive]);

			//Count
			strTemp.Format(_T("%d"), pHistoData->nCount);
			m_columnTree.SetItemText(hItem, 2, strTemp);

			//Attribute
			int nAttCount = static_cast<int>(pHistoData->vecAttribute.size());
			strTemp = _T("");
			for (int nAtt = 0; nAtt < nAttCount; nAtt++)
			{
				if (nAtt != 0)
				{
					strTemp += _T(",");
				}

				strTemp += pHistoData->vecAttribute[nAtt];
			}
			m_columnTree.SetItemText(hItem, 3, strTemp);

			m_columnTree.SetItemText(hItem, 4, pHistoData->strSymbol);//Symbol

			nTypeCount += pHistoData->nCount;
		}
		strTemp.Format(_T("%d"), nTypeCount);
		m_columnTree.SetItemText(hRoot, 2, strTemp);

		nTotalCount += nTypeCount;
	}

	strTemp.Format(_T("%d"), nTotalCount);
	m_columnTree.SetItemText(hRootTotal, 2, strTemp);


}

void CHistogram::_UpDateHistoTree_Att()
{
	if (!m_bUpdateUI) return;

	_ClearHistoTree();

	int nCount = static_cast<int>(m_vecAttData.size());

	HTREEITEM hRootTotal, hRoot, hItem;
	HistoData_Att *pHistoData;
	CString strTemp;

	int nTotalCount = 0;
	int nTypeCount = 0;
	hRootTotal = m_columnTree.GetTreeCtrl().InsertItem(_T("Total"));

	for (int i = 0; i < nCount; i++)
	{
		int nHistoCount = static_cast<int>(m_vecAttData[i].vecHistoData.size());
		if (nHistoCount == 0) continue;

		hRoot = m_columnTree.GetTreeCtrl().InsertItem(m_vecAttData[i].strTypeName);
		nTypeCount = 0;
		for (int j = 0; j < nHistoCount; j++)
		{
			pHistoData = &m_vecAttData[i].vecHistoData[j];
			//Name
			hItem = m_columnTree.GetTreeCtrl().InsertItem(pHistoData->GetAttNames(), hRoot);


			//Count
			strTemp.Format(_T("%d"), pHistoData->nCount);
			m_columnTree.SetItemText(hItem, 1, strTemp);

			nTypeCount += pHistoData->nCount;
		}
		strTemp.Format(_T("%d"), nTypeCount);
		m_columnTree.SetItemText(hRoot, 1, strTemp);

		nTotalCount += nTypeCount;
	}

	strTemp.Format(_T("%d"), nTotalCount);
	m_columnTree.SetItemText(hRootTotal, 1, strTemp);
}

void CHistogram::_UpDateHistoTree_Symbol()
{
	if (!m_bUpdateUI) return;

	_ClearHistoTree();

	int nCount = static_cast<int>(m_vecSymbolData.size());

	HTREEITEM hRootTotal, hRoot, hItem;
	HistoData_Sym *pHistoData;
	CString strTemp;

	int nTotalCount = 0;
	int nTypeCount = 0;
	hRootTotal = m_columnTree.GetTreeCtrl().InsertItem(_T("Total"));
	
	for (int i = 0; i < nCount; i++)
	{
		int nHistoCount = static_cast<int>(m_vecSymbolData[i].vecHistoData.size());
		if (nHistoCount == 0) continue;

		hRoot = m_columnTree.GetTreeCtrl().InsertItem(m_vecSymbolData[i].strTypeName);
		nTypeCount = 0;
		for (int j = 0; j < nHistoCount; j++)
		{
			pHistoData = &m_vecSymbolData[i].vecHistoData[j];


			//Name
			hItem = m_columnTree.GetTreeCtrl().InsertItem(pHistoData->strName, hRoot);


			//Count
			strTemp.Format(_T("%d"), pHistoData->nCount);
			m_columnTree.SetItemText(hItem, 1, strTemp);

			nTypeCount += pHistoData->nCount;
		}
		strTemp.Format(_T("%d"), nTypeCount);
		m_columnTree.SetItemText(hRoot, 1, strTemp);

		nTotalCount += nTypeCount;
	}

	strTemp.Format(_T("%d"), nTotalCount);
	m_columnTree.SetItemText(hRootTotal, 1, strTemp);
}

CString CHistogram::ConvFeatureType(enum FeatureType eFeatureType)
{
	CString strName;
	switch (eFeatureType)
	{
	case FeatureType::L://Line
		strName = _T("Line");
		break;
	case FeatureType::P://Pad
		strName = _T("Pad");
		break;
	case FeatureType::A://Arc
		strName = _T("Arc");
		break;
	case FeatureType::T://Text
		strName = _T("Text");
		break;
	case FeatureType::S://Surface
		strName = _T("Surface");
		break;
	case FeatureType::B://Barcode
		strName = _T("Barcode");
		break;
	default:
		strName = _T("");
		break;
	}

	return strName;
}

enum FeatureType CHistogram::ConvFeatureString(CString &strFeature)
{
	FeatureType eFeatureType;

	if (strFeature == _T("Line")) eFeatureType = FeatureType::L;
	else if (strFeature == _T("Pad")) eFeatureType = FeatureType::P;
	else if (strFeature == _T("Arc")) eFeatureType = FeatureType::A;
	else if (strFeature == _T("Text")) eFeatureType = FeatureType::T;
	else if (strFeature == _T("Surface")) eFeatureType = FeatureType::S;
	else if (strFeature == _T("Barcode")) eFeatureType = FeatureType::B;
	else eFeatureType = FeatureType::FeatureTypeNone;
	
	return eFeatureType;
}

void CHistogram::_GetSelectInfo(vector<FEATURE_INFO> &vecFeatureInfo)
{
	vecFeatureInfo.clear();

	vector<CString> vecParent;
	vector<CString> vecInfo;
	m_columnTree.GetSelectInfo(vecParent, vecInfo);//트리에서 정보 가져오기

	//정보 파싱
	SetFeatureInfo(vecParent, vecInfo, vecFeatureInfo);
}

void CHistogram::SetFeatureInfo(vector<CString> &vecParent, vector<CString> &vecInfo, vector<FEATURE_INFO> &vecFeatureInfo)
{
	vecFeatureInfo.clear();

	int nParentCount = static_cast<int>(vecParent.size());
	int nInfoCount = static_cast<int>(vecInfo.size());

	if (nParentCount != nInfoCount)
	{
		TRACE("abnormal case\n");
		return;
	}

	FEATURE_INFO stInfo;
	if (m_nHistoType == DEF_HISTOGRAM::Type_Feature)
	{
		for (int i = 0; i < nInfoCount; i++)
		{
			stInfo.nHistoSelectType = DEF_HISTOGRAM::Type_Feature;

			vector<CString> vecParentSplit;
			vector<CString> vecInfoSplit;
			//type
			StringUtil::Split(vecParent[i], _T("\t"), vecParentSplit);
			StringUtil::Split(vecInfo[i], _T("\t"), vecInfoSplit);

			if (vecParentSplit.size() <= 0) continue;
			if (vecInfoSplit.size() <= 0) continue;

			stInfo.eType = ConvFeatureString(vecParentSplit[0]);
			stInfo.strLayerName = m_strLayerName;
			stInfo.strFeatureName = vecInfoSplit[0];
			stInfo.vecAttString.resize(1);
			if (vecInfoSplit[3] == _T(""))
			{
				stInfo.vecAttString[0] = DEF_HISTOGRAM::strAttNull;
			}
			else
			{
				stInfo.vecAttString[0] = vecInfoSplit[3];
			}
			stInfo.strSymbolName = vecInfoSplit[4];

			vecFeatureInfo.push_back(stInfo);
			stInfo._Reset();
		}
	}
	else if (m_nHistoType == DEF_HISTOGRAM::Type_Attribute)
	{
		for (int i = 0; i < nInfoCount; i++)
		{
			stInfo.nHistoSelectType = DEF_HISTOGRAM::Type_Attribute;

			vector<CString> vecParentSplit;
			vector<CString> vecInfoSplit;
			//type
			StringUtil::Split(vecParent[i], _T("\t"), vecParentSplit);
			StringUtil::Split(vecInfo[i], _T("\t"), vecInfoSplit);

			if (vecParentSplit.size() <= 0) continue;
			if (vecInfoSplit.size() <= 0) continue;

			stInfo.eType = ConvFeatureString(vecParentSplit[0]);
			stInfo.strLayerName = m_strLayerName;

// 			vector<CString> vecAttSplit;
// 			StringUtil::Split(vecInfoSplit[0], _T(","), vecAttSplit);
			stInfo.vecAttString.resize(1);
			stInfo.vecAttString[0] = vecInfoSplit[0];
			
			

			vecFeatureInfo.push_back(stInfo);
			stInfo._Reset();
		}
	}
	else if (m_nHistoType == DEF_HISTOGRAM::Type_Symbol)
	{
		for (int i = 0; i < nInfoCount; i++)
		{
			stInfo.nHistoSelectType = DEF_HISTOGRAM::Type_Symbol;

			vector<CString> vecParentSplit;
			vector<CString> vecInfoSplit;
			//type
			StringUtil::Split(vecParent[i], _T("\t"), vecParentSplit);
			StringUtil::Split(vecInfo[i], _T("\t"), vecInfoSplit);

			if (vecParentSplit.size() <= 0) continue;
			if (vecInfoSplit.size() <= 0) continue;

			stInfo.eType = ConvFeatureString(vecParentSplit[0]);
			stInfo.strLayerName = m_strLayerName;
			stInfo.strSymbolName = vecInfoSplit[0];

			vecFeatureInfo.push_back(stInfo);
			stInfo._Reset();
		}
	}
}

void CHistogram::OnBnClickedButtonResetSelect()
{
	ResetSelect();
}

void CHistogram::OnBnClickedButtonSelect()
{
	vector<FEATURE_INFO> vecFeatureInfo;
	_GetSelectInfo(vecFeatureInfo);

	SetSelect(vecFeatureInfo);
}

void CHistogram::OnBnClickedButtonUnselect()
{
	vector<FEATURE_INFO> vecFeatureInfo;
	_GetSelectInfo(vecFeatureInfo);

	SetUnSelect(vecFeatureInfo);
}

void CHistogram::OnBnClickedButtonAllShow()
{
	ResetHide();
}

void CHistogram::OnBnClickedButtonShow()
{
	vector<FEATURE_INFO> vecFeatureInfo;
	_GetSelectInfo(vecFeatureInfo);

	SetShow(vecFeatureInfo);
}

void CHistogram::OnBnClickedButtonHide()
{
	vector<FEATURE_INFO> vecFeatureInfo;
	_GetSelectInfo(vecFeatureInfo);

	SetHide(vecFeatureInfo);
}



void CHistogram::OnBnClickedButtonClose()
{
	this->ShowWindow(SW_HIDE);
}


void CHistogram::_SortHistoData_Feature(vector<FeatureHistoData>	&vecHistoData)
{
	HistoData_Feature stTemp;

	int nTypeCount = static_cast<int>(vecHistoData.size());
	for (int nType = 0; nType < nTypeCount; nType++)
	{
		if(vecHistoData[nType].strTypeName == _T("Surface") ) continue;

		int nFeatureCount = static_cast<int>(vecHistoData[nType].vecHistoData.size());

		//종류별 정렬
		for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
		{
			for (int nfeno1 = nfeno+1; nfeno1 < nFeatureCount; nfeno1++)
			{
				if (vecHistoData[nType].vecHistoData[nfeno].emSymType >
					vecHistoData[nType].vecHistoData[nfeno1].emSymType)

				{
					stTemp = vecHistoData[nType].vecHistoData[nfeno];
					vecHistoData[nType].vecHistoData[nfeno] = vecHistoData[nType].vecHistoData[nfeno1];
					vecHistoData[nType].vecHistoData[nfeno1] = stTemp;
				}
			}
		}

		//크기별 정렬
		for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
		{
			for (int nfeno1 = nfeno + 1; nfeno1 < nFeatureCount; nfeno1++)
			{
				if (vecHistoData[nType].vecHistoData[nfeno].emSymType ==
					vecHistoData[nType].vecHistoData[nfeno1].emSymType)
				{
					SymbolName eSymbolName = vecHistoData[nType].vecHistoData[nfeno].emSymType;
					CString strSymbolName = DefSymbol::pName[static_cast<int>(eSymbolName)];
					
					double dSize = CDrawFunction::GetSymbolSize(vecHistoData[nType].vecHistoData[nfeno].strName);//_ttof(vecHistoData[nType].vecHistoData[nfeno].strName);
					double dSize1 = CDrawFunction::GetSymbolSize(vecHistoData[nType].vecHistoData[nfeno1].strName);
					
					if (dSize > dSize1)
					{
						stTemp = vecHistoData[nType].vecHistoData[nfeno];
						vecHistoData[nType].vecHistoData[nfeno] = vecHistoData[nType].vecHistoData[nfeno1];
						vecHistoData[nType].vecHistoData[nfeno1] = stTemp;
					}
				}
			}
		}
	}
}

void CHistogram::_SortHistoData_Symbol(vector<SymHistoData>		&vecSymbolData)
{
	HistoData_Sym stTemp;

	int nTypeCount = static_cast<int>(vecSymbolData.size());
	for (int nType = 0; nType < nTypeCount; nType++)
	{
		if (vecSymbolData[nType].strTypeName == _T("Surface")) continue;

		int nFeatureCount = static_cast<int>(vecSymbolData[nType].vecHistoData.size());

		//종류별 정렬
		for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
		{
			for (int nfeno1 = nfeno + 1; nfeno1 < nFeatureCount; nfeno1++)
			{
				vecSymbolData[nType].vecHistoData[nfeno].strName;

				LONGLONG lConvNum = _GetStringNum(vecSymbolData[nType].vecHistoData[nfeno].strName);
				LONGLONG lConvNum1 = _GetStringNum(vecSymbolData[nType].vecHistoData[nfeno1].strName);
				if (lConvNum > lConvNum1)
				{
					stTemp = vecSymbolData[nType].vecHistoData[nfeno];
					vecSymbolData[nType].vecHistoData[nfeno] = vecSymbolData[nType].vecHistoData[nfeno1];
					vecSymbolData[nType].vecHistoData[nfeno1] = stTemp;
				}
			}
		}

	}
}

LONGLONG CHistogram::_GetStringNum(IN const CString &strName)
{
	int nDigit = 5;

	int nLength = strName.GetLength();
	LONGLONG lConv = 0;
	CString str;
	char chChar;
	
	int nLoop;
	if (nLength > nDigit)
	{
		nLoop = nDigit;
	}
	else
	{
		nLoop = nLength;
	}
	for (int i = 0; i < nLoop; i++)
	{
		str = strName.Mid(i, 1);
		memcpy(&chChar, str.GetBuffer(), sizeof(char));
		lConv += static_cast<LONGLONG>(chChar * pow(100, nDigit));
		
		nDigit--;
	}

	return lConv;
}

BOOL CHistogram::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}
