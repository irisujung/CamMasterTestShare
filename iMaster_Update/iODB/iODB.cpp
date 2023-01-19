// iODB.cpp : 정적 라이브러리를 위한 함수를 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "iODB.h"
#include <map>

#define _USE_MATH_DEFINES
#include <math.h>

IMPLEMENT_SERIAL(CAttribute, CObject, 1)
IMPLEMENT_SERIAL(CAttributeTextString, CObject, 1)
IMPLEMENT_SERIAL(CJobFile, CObject, 1)
IMPLEMENT_SERIAL(CMatrix, CObject, 1)
IMPLEMENT_SERIAL(CMatrixStep, CObject, 1)
IMPLEMENT_SERIAL(CMatrixLayer, CObject, 1)
IMPLEMENT_SERIAL(CFontLine, CObject, 1)
IMPLEMENT_SERIAL(CFontChar, CObject, 1)
IMPLEMENT_SERIAL(CStandardFont, CObject, 1)
IMPLEMENT_SERIAL(CUserSymbolAttrlist, CObject, 1)
IMPLEMENT_SERIAL(CUserSymbol, CObject, 1)
IMPLEMENT_SERIAL(CSymbol, CObject, 1)
IMPLEMENT_SERIAL(CFeature, CObject, 1)
IMPLEMENT_SERIAL(CFeatureL, CObject, 1)
IMPLEMENT_SERIAL(CFeatureP, CObject, 1)
IMPLEMENT_SERIAL(CFeatureA, CObject, 1)
IMPLEMENT_SERIAL(CFeatureT, CObject, 1)
IMPLEMENT_SERIAL(CFeatureB, CObject, 1)
IMPLEMENT_SERIAL(CFeatureS, CObject, 1)
IMPLEMENT_SERIAL(CFeatureZ,CObject,1)
IMPLEMENT_SERIAL(COsOc, CObject, 1)
IMPLEMENT_SERIAL(COc, CObject, 1)
IMPLEMENT_SERIAL(COs, CObject, 1)
IMPLEMENT_SERIAL(CObOe, CObject, 1)
IMPLEMENT_SERIAL(CFeatureFile, CObject, 1)
IMPLEMENT_SERIAL(CRecord, CObject, 1)
IMPLEMENT_SERIAL(CRecordRC, CObject, 1)
IMPLEMENT_SERIAL(CRecordCR, CObject, 1)
IMPLEMENT_SERIAL(CRecordSQ, CObject, 1)
IMPLEMENT_SERIAL(CRecordCT, CObject, 1)
IMPLEMENT_SERIAL(CPackage, CObject, 1)
IMPLEMENT_SERIAL(CPin, CObject, 1)
IMPLEMENT_SERIAL(CToeprint, CObject, 1)
IMPLEMENT_SERIAL(CComponentFile, CObject, 1)
IMPLEMENT_SERIAL(CComponent, CObject, 1)
IMPLEMENT_SERIAL(CLayer, CObject, 1)
IMPLEMENT_SERIAL(CStepRepeat, CObject, 1)
IMPLEMENT_SERIAL(CStephdr, CObject, 1)
IMPLEMENT_SERIAL(CProfile, CObject, 1)
IMPLEMENT_SERIAL(CStep, CObject, 1)
IMPLEMENT_SERIAL(CSubStep, CObject, 1)

CJobFile* g_pJobFile = nullptr;
CTypedPtrArray <CObArray, CStep*> g_arrStep;

void CMatrixStep::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);   
								  
	if (ar.IsStoring())
	{ 
		ar << m_iCol;
		ar << m_strDefine;
		ar << m_strName;
	}
	else
	{  
		ar >> m_iCol;
		ar >> m_strDefine;
		ar >> m_strName;
	}
}

void CMatrixLayer::SetType(CString strType)
{
	m_enSideType = SideType::SideTypeNone;

	if (strType.Find(L"SIGNAL") != -1)
		m_enSideType = SideType::TSignal;
	else if (strType.Find(L"POWER_GROUND") != -1)
		m_enSideType = SideType::TPowerGround;
	else if (strType.Find(L"DIELECTRIC") != -1)
		m_enSideType = SideType::TDielectric;
	else if (strType.Find(L"MIXED") != -1)
		m_enSideType = SideType::TMixed;
	else if (strType.Find(L"SOLDER_MASK") != -1)
		m_enSideType = SideType::TSolderMask;
	else if (strType.Find(L"SOLDER_PASTE") != -1)
		m_enSideType = SideType::TSolderPaste;
	else if (strType.Find(L"SILK_SCREEN") != -1)
		m_enSideType = SideType::TSilkScreen;
	else if (strType.Find(L"DRILL") != -1)
		m_enSideType = SideType::TDrill;
	else if (strType.Find(L"ROUT") != -1)
		m_enSideType = SideType::TRout;
	else if (strType.Find(L"DOCUMENT") != -1)
		m_enSideType = SideType::TDocument;
	else if (strType.Find(L"COMPONENT") != -1)
		m_enSideType = SideType::TComponent;
	else if (strType.Find(L"MASK") != -1)
		m_enSideType = SideType::TMask;
	else if (strType.Find(L"CONDUCTIVE_PASTE") != -1)
		m_enSideType = SideType::TConductivePaste;
	else if (strType.Find(L"MISC") != -1)
		m_enSideType = SideType::TMisc;
}

void CMatrixLayer::SetPolarity(CString strPolarity)
{
	m_enPolarity = Polarity::PolarityNone;

	if (strPolarity.Find(L"POSITIVE") != -1)
		m_enPolarity = Polarity::PPositive;
	else if (strPolarity.Find(L"NEGATIVE") != -1)
		m_enPolarity = Polarity::PNegative;		
}

void CMatrixLayer::SetColor(CString strColor)
{
	//percentage
	BYTE r, g, b;
	r = (BYTE)(2.55 * _ttof(strColor.Left(2)));
	g = (BYTE)(2.55 * _ttof(strColor.Mid(2, 2)));
	b = (BYTE)(2.55 * _ttof(strColor.Right(2)));
	m_color = RGB(r, g, b);
}

void CMatrixLayer::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);   

	if (ar.IsStoring())
	{  
		ar.Write(&m_enPolarity, sizeof(m_enPolarity));
		ar.Write(&m_enSideType, sizeof(m_enSideType));
		ar << m_iRow;
		ar.Write(&m_color, sizeof(m_color));
		ar << m_strContext;
		ar << m_strType;
		ar << m_strName;		
	}
	else
	{  
		ar.Read(&m_enPolarity, sizeof(m_enPolarity));
		ar.Read(&m_enSideType, sizeof(m_enSideType));
		ar >> m_iRow;
		ar.Read(&m_color, sizeof(m_color));
		ar >> m_strContext;		
		ar >> m_strType;
		ar >> m_strName;
	}
}

void CMatrix::ClearMatrixLayers()
{
	for (UINT32 i = 0; i < m_arrMatrixLayer.GetSize(); i++)
		delete m_arrMatrixLayer[i];

	m_arrMatrixLayer.RemoveAll();
}

void CMatrix::ClearMatrixSteps()
{
	for (UINT32 i = 0; i < m_arrMatrixStep.GetSize(); i++)
		delete m_arrMatrixStep[i];

	m_arrMatrixStep.RemoveAll();
}

void CMatrix::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);   
	
	if (ar.IsStoring())
	{
		ar.Write(&m_bIsStepSet, sizeof(m_bIsStepSet));
		ar << m_iStartLayerIdx;
		ar << m_iEndLayerIdx;

		ar << m_arrMatrixStep.GetSize();
		for (UINT32 i = 0; i < m_arrMatrixStep.GetSize(); i++)
		{
			CMatrixStep* pMatrixStepTemp = m_arrMatrixStep.GetAt(i);
			pMatrixStepTemp->Serialize(ar);
		}

		ar << m_arrMatrixLayer.GetSize();
		for (UINT32 i = 0; i < m_arrMatrixLayer.GetSize(); i++)
		{
			CMatrixLayer* pMatrixLayerTemp = m_arrMatrixLayer.GetAt(i);
			pMatrixLayerTemp->Serialize(ar);
		}
	}
	else
	{ 
		ar.Read(&m_bIsStepSet, sizeof(m_bIsStepSet));
		ar >> m_iStartLayerIdx;
		ar >> m_iEndLayerIdx;

		UINT32 nCount = 0;
		ar >> nCount;
		for (UINT32 i = 0; i < nCount; i++)
		{
			CMatrixStep*  pMatrixStepTemp = new CMatrixStep;
			pMatrixStepTemp->Serialize(ar);
			m_arrMatrixStep.Add(pMatrixStepTemp);
		}

		ar >> nCount;
		for (UINT32 i = 0; i < nCount; i++)
		{
			CMatrixLayer*  pMatrixLayerTemp = new CMatrixLayer;
			pMatrixLayerTemp->Serialize(ar);
			m_arrMatrixLayer.Add(pMatrixLayerTemp);
		}
	}
}

UINT32 CMatrix::LoadMatrixFile(CString strFileName)
{
	ClearMatrixSteps();
	ClearMatrixLayers();

	CStdioFile cStdFile;
	CString strTmp, strTrans = L"", strPattern;
	int nTmp;
	_TCHAR bom = (_TCHAR)0xFEFF;
	CMatrixStep* pMatrixStepTmp;
	CMatrixLayer* pMatrixLayerTmp;
	CFileFind filefinder;

	if (!cStdFile.Open(strFileName.GetBuffer(0), CFile::modeRead | CFile::typeText)) 
		return RESULT_BAD;
	//	CArchive ar(&cStdFile, CArchive::load);

	//cStdFile.ReadString(strTmp);
	while (cStdFile.ReadString(strTmp))
	{
		strTmp.TrimLeft(L"\t ");
		strTmp.TrimRight(L"\t ");
		strTmp.MakeUpper();
		if (strTmp.GetLength() == 0) continue;	// 빈줄 Skip

		if (strTmp.Find(L"STEP") != -1)
		{
			pMatrixStepTmp = new CMatrixStep;

			while (cStdFile.ReadString(strTmp))
			{
				strTmp.TrimLeft(L"\t ");
				strTmp.TrimRight(L"\t ");
				strTmp.MakeUpper();
				if (strTmp.GetLength() == 0)  continue;	// 빈줄 Skip
				if (strTmp.Find('}') != -1) break;

				nTmp = strTmp.Find('=');
				if (nTmp != -1)
				{
					if (strTmp.Find(L"COL") != -1)
						pMatrixStepTmp->m_iCol = atoi((CStringA)strTmp.Right(strTmp.GetLength() - nTmp - 1));
					else if (strTmp.Find(L"NAME") != -1)
						pMatrixStepTmp->m_strName = strTmp.Right(strTmp.GetLength() - nTmp - 1);
					else OutputDebugString(strTmp + L" : It is a new member of STEP or a error in Matrix File");
				}
			}
			m_arrMatrixStep.Add(pMatrixStepTmp);
		}
		else if (strTmp.Find(L"LAYER") != -1)
		{
			pMatrixLayerTmp = new CMatrixLayer;

			while (cStdFile.ReadString(strTmp))
			{
				strTmp.TrimLeft(L"\t ");
				strTmp.TrimRight(L"\t ");
				strTmp.MakeUpper();
				if (strTmp.Find('}') != -1)	break;

				nTmp = strTmp.Find('=');
				if (nTmp != -1)
				{
					if (strTmp.Find(L"ROW") != -1)
						pMatrixLayerTmp->m_iRow = atoi((CStringA)strTmp.Right(strTmp.GetLength() - nTmp - 1));
					else if (strTmp.Find(L"CONTEXT") != -1)
						pMatrixLayerTmp->m_strContext = strTmp.Right(strTmp.GetLength() - nTmp - 1);
					else if (strTmp.Find(L"TYPE") != -1)
					{
						if (strTmp.Find(L"_TYPE") != -1)
							continue;

						pMatrixLayerTmp->m_strType = strTmp.Right(strTmp.GetLength() - nTmp - 1);
						pMatrixLayerTmp->SetType(strTmp.Right(strTmp.GetLength() - nTmp - 1));
					}
					else if (strTmp.Find(L"POLARITY") != -1)
						pMatrixLayerTmp->SetPolarity(strTmp.Right(strTmp.GetLength() - nTmp - 1));
					else if (strTmp.Find(L"ORIENTATION") != -1) {}			
					else if (strTmp.Find(_T("START_NAME")) != -1) {}
					else if (strTmp.Find(_T("END_NAME")) != -1) {}
					else if (strTmp.Find(_T("OLD_NAME")) != -1) {}
					else if (strTmp.Find(L"NAME") != -1)
						pMatrixLayerTmp->m_strName = strTmp.Right(strTmp.GetLength() - nTmp - 1);					
					else if (strTmp.Find(L"COLOR") != -1)
						pMatrixLayerTmp->SetColor(strTmp.Right(strTmp.GetLength() - nTmp - 1));
					else
						OutputDebugString(strTmp + L" : It is a new member of LAYER or a error in Matrix File ");
				}
			}
			m_arrMatrixLayer.Add(pMatrixLayerTmp);
		}
		else
			OutputDebugString(strTmp + L" : It is a new member or a error in Matrix File ");
	}
	//	ar.Close();
	cStdFile.Close();
	return RESULT_GOOD;
}

UINT32 CMatrix::AddLayer(CString strFilePath, std::map<CString, bool>& vcLayer)
{
	if (strFilePath.GetLength() < 1 ||
		m_arrMatrixLayer.GetSize() <= 0)
		return RESULT_BAD;

	CString strFileName = strFilePath + MATRIX_PATH;

	CStdioFile cStdFile;
	if (!cStdFile.Open(strFileName.GetBuffer(0), CFile::modeWrite | CFile::typeText)) return RESULT_BAD;

	cStdFile.SeekToEnd();

	UINT32 nMaxLayerIdx = static_cast<UINT32>(m_arrMatrixLayer.GetSize());

	CString strTmp;
	for (auto it : vcLayer)
	{
		if (it.second)
			continue;

		CMatrixLayer* pMatrixLayerTmp = new CMatrixLayer;
		pMatrixLayerTmp->m_iRow = ++nMaxLayerIdx;
		pMatrixLayerTmp->m_strContext = _T("MISC");
		pMatrixLayerTmp->m_strType = _T("SIGNAL");
		pMatrixLayerTmp->SetType(pMatrixLayerTmp->m_strType);
		pMatrixLayerTmp->m_strName = it.first;
		pMatrixLayerTmp->SetPolarity(_T("POSITIVE"));

		strTmp.Format(_T("\nLAYER {\n"));
		cStdFile.WriteString(strTmp);
		strTmp.Format(_T("    ROW=%d\n"), pMatrixLayerTmp->m_iRow);
		cStdFile.WriteString(strTmp);
		strTmp.Format(_T("    CONTEXT=%s\n"), pMatrixLayerTmp->m_strContext);
		cStdFile.WriteString(strTmp);
		strTmp.Format(_T("    TYPE=%s\n"), pMatrixLayerTmp->m_strType);
		cStdFile.WriteString(strTmp);
		strTmp.Format(_T("    NAME=%s\n"), pMatrixLayerTmp->m_strName);
		cStdFile.WriteString(strTmp);
		strTmp.Format(_T("    POLARITY=POSITIVE\n"));
		cStdFile.WriteString(strTmp);
		strTmp.Format(_T("    ORIENTATION=NOT_DEFINED\n"));
		cStdFile.WriteString(strTmp);
		strTmp.Format(_T("    START_NAME=\n"));
		cStdFile.WriteString(strTmp);
		strTmp.Format(_T("    END_NAME=\n"));
		cStdFile.WriteString(strTmp);
		strTmp.Format(_T("    OLD_NAME=\n"));
		cStdFile.WriteString(strTmp);
		strTmp.Format(_T("}\n"));
		cStdFile.WriteString(strTmp);

		m_arrMatrixLayer.Add(pMatrixLayerTmp);
	}

	cStdFile.Close();

	CFileFind finder;
	for (int j = 0; j < m_arrMatrixStep.GetSize(); j++)
	{
		CString strStep = m_arrMatrixStep[j]->m_strName;

		for (auto it : vcLayer)
		{
			strTmp.Format(_T("%s\\steps\\%s\\layers\\%s"), strFilePath, strStep, it.first);
			strFileName.Format(_T("%s\\features"), strTmp);

			if (finder.FindFile(strFileName))			
				continue;			

			CreateDirectory(strTmp, NULL);

			if (!cStdFile.Open(strFileName.GetBuffer(0), CFile::modeCreate | CFile::modeWrite | CFile::typeText))
				return RESULT_BAD;

			cStdFile.Close();
		}
	}
	
	return RESULT_GOOD;
}

void CFontLine::Serialize(CArchive& ar)
{
	CObject::Serialize(ar); 

	if (ar.IsStoring())
	{
		ar.Write(&m_bShape, sizeof(m_bShape));
		ar.Write(&m_bPol, sizeof(m_bPol));
		ar << m_dXs;
		ar << m_dYs;
		ar << m_dXe;
		ar << m_dYe;		
		ar << m_dWidth;
	}
	else
	{
		ar.Read(&m_bShape, sizeof(m_bShape));
		ar.Read(&m_bPol, sizeof(m_bPol));
		ar >> m_dXs;
		ar >> m_dYs;
		ar >> m_dXe;
		ar >> m_dYe;		
		ar >> m_dWidth;
	}
}

void CFontChar::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_Char;
		ar << m_MinMaxRect.left;
		ar << m_MinMaxRect.top;
		ar << m_MinMaxRect.right;
		ar << m_MinMaxRect.bottom;

		ar << m_arrFontLine.GetSize();
		for (UINT32 i = 0; i < m_arrFontLine.GetSize(); i++)
		{
			CFontLine* pCFontLineTemp = m_arrFontLine.GetAt(i);
			pCFontLineTemp->Serialize(ar);
		}		
	}
	else
	{
		ar >> m_Char;
		ar >> m_MinMaxRect.left;
		ar >> m_MinMaxRect.top;
		ar >> m_MinMaxRect.right;
		ar >> m_MinMaxRect.bottom;

		UINT32 nCount = 0;
		ar >> nCount;
		for (UINT32 i = 0; i < nCount; i++)
		{
			CFontLine* pCFontLineTemp = new CFontLine;
			pCFontLineTemp->Serialize(ar);
			m_arrFontLine.Add(pCFontLineTemp);
		}
	}
}

void CFontChar::ClearFontLines()
{
	for (UINT32 i = 0; i < m_arrFontLine.GetSize(); i++)
	{
		delete m_arrFontLine[i];
	}

	m_arrFontLine.RemoveAll();
}

UINT32 CFontChar::LoadFontLine(CString strFontLine, bool bIsMMUnit)
{
	if (strFontLine.GetLength() < 1)
		return RESULT_BAD;

	CFontLine* pFontLineTmp = new CFontLine();
	if (pFontLineTmp == nullptr)
		return RESULT_BAD;

	int nTmp = 0;
	CString strTmp = L"", strTmp2 = L"";

	//Parameter를 축출함.
	if ((nTmp = strFontLine.Find(' ')) != -1) // LINE 문자 제거
		strFontLine.Delete(0, nTmp + 1);

	if ((nTmp = strFontLine.Find(' ')) != -1)  // <xs> 값 축출
	{
		strTmp2 = strFontLine.Left(nTmp);
	
		pFontLineTmp->m_dXs = bIsMMUnit ? atof((CStringA)strTmp2.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp2.GetBuffer(0)));
		
		strFontLine.Delete(0, nTmp + 1);
	}

	if ((nTmp = strFontLine.Find(' ')) != -1)  // <ys> 값 축출
	{
		strTmp2 = strFontLine.Left(nTmp);
	
		pFontLineTmp->m_dYs = bIsMMUnit ? atof((CStringA)strTmp2.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp2.GetBuffer(0)));

		strFontLine.Delete(0, nTmp + 1);
	}

	if ((nTmp = strFontLine.Find(' ')) != -1)  // <xe> 값 축출
	{
		strTmp2 = strFontLine.Left(nTmp);
	
		pFontLineTmp->m_dXe = bIsMMUnit ? atof((CStringA)strTmp2.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp2.GetBuffer(0)));

		strFontLine.Delete(0, nTmp + 1);
	}

	if ((nTmp = strFontLine.Find(' ')) != -1)  // <ye> 값 축출
	{
		strTmp2 = strFontLine.Left(nTmp);
		
		pFontLineTmp->m_dYe = bIsMMUnit ? atof((CStringA)strTmp2.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp2.GetBuffer(0)));

		strFontLine.Delete(0, nTmp + 1);
	}

	if ((nTmp = strFontLine.Find(' ')) != -1)  // <pol> 값 축출
	{
		strTmp2 = strFontLine.Left(nTmp);
		if (strTmp2.GetAt(0) == 'P')
			pFontLineTmp->m_bPol = true;

		strFontLine.Delete(0, nTmp + 1);
	}

	if ((nTmp = strFontLine.Find(' ')) != -1)  // <shape> 값 축출
	{
		strTmp2 = strFontLine.Left(nTmp);
		if (strTmp2.GetAt(0) == 'R')
			pFontLineTmp->m_bShape = true;

		strFontLine.Delete(0, nTmp + 1);
	}

	if (strFontLine.GetLength())  // <width> 값 축출
	{
		pFontLineTmp->m_dWidth = bIsMMUnit ? atof((CStringA)strFontLine.GetBuffer(0)) :
				CJobFile::InchToMM(atof((CStringA)strFontLine.GetBuffer(0)));
	}

	if (m_MinMaxRect.left > pFontLineTmp->m_dXs)	m_MinMaxRect.left = pFontLineTmp->m_dXs;
	if (m_MinMaxRect.left > pFontLineTmp->m_dXe)	m_MinMaxRect.left = pFontLineTmp->m_dXe;
	if (m_MinMaxRect.right < pFontLineTmp->m_dXs)	m_MinMaxRect.right = pFontLineTmp->m_dXs;
	if (m_MinMaxRect.right < pFontLineTmp->m_dXe)	m_MinMaxRect.right = pFontLineTmp->m_dXe;
	if (m_MinMaxRect.top < pFontLineTmp->m_dYs)		m_MinMaxRect.top = pFontLineTmp->m_dYs;
	if (m_MinMaxRect.top < pFontLineTmp->m_dYe)		m_MinMaxRect.top = pFontLineTmp->m_dYe;
	if (m_MinMaxRect.bottom > pFontLineTmp->m_dYs)	m_MinMaxRect.bottom = pFontLineTmp->m_dYs;
	if (m_MinMaxRect.bottom > pFontLineTmp->m_dYe)	m_MinMaxRect.bottom = pFontLineTmp->m_dYe;
	
	m_arrFontLine.Add(pFontLineTmp);
	return RESULT_GOOD;
}

void CStandardFont::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_dXSize;
		ar << m_dYSize;
		ar << m_dOffset;
		ar << m_strFontName;		

		ar << m_arrFontChar.GetSize();
		for (UINT32 i = 0; i < m_arrFontChar.GetSize(); i++)
		{
			CFontChar* pCFontCharTemp = m_arrFontChar.GetAt(i);
			pCFontCharTemp->Serialize(ar);
		}
	}
	else
	{ 
		ar >> m_dXSize;
		ar >> m_dYSize;
		ar >> m_dOffset;
		ar >> m_strFontName;		

		UINT32 nCount = 0;
		ar >> nCount;
		for (UINT32 i = 0; i < nCount; i++)
		{
			CFontChar* pCFontCharTemp = new CFontChar;
			pCFontCharTemp->Serialize(ar);
			m_arrFontChar.Add(pCFontCharTemp);
		}
	}
}

void CStandardFont::ClearFontChars()
{
	for (UINT32 i = 0; i < m_arrFontChar.GetSize(); i++)
	{
		m_arrFontChar[i]->ClearFontLines();
		delete m_arrFontChar[i];
	}

	m_arrFontChar.RemoveAll();
}

UINT32 CStandardFont::LoadStandardFile(CString strFileName)
{
	CStdioFile cStdFile;
	CString strTmp, strTmp2;
	int nTmp; CFileFind filefinder;
	CFontChar* pFontCharTmp = nullptr;
	_TCHAR bom = (_TCHAR)0xFEFF;
	bool bIsMMUnit = false;

	if (m_arrFontChar.GetSize()) ClearFontChars();

	if (!cStdFile.Open(strFileName.GetBuffer(0), CFile::modeRead | CFile::typeText)) 
		return RESULT_BAD;
	//	CArchive ar(&cStdFile, CArchive::load);

	while (cStdFile.ReadString(strTmp))
	{
		strTmp.TrimLeft(L"\t ");
		strTmp.TrimRight(L"\t ");
		if (strTmp.GetLength() == 0)  // 빈줄 Skip
			continue;

		if (strTmp.Find(L"U MM") != -1)
		{
			bIsMMUnit = true;
			continue;
		}

		if (strTmp.Find(L"XSIZE") != -1)
		{
			strTmp2 = strTmp.Right(strTmp.GetLength() - sizeof("XSIZE") - 1);
			
			m_dXSize = bIsMMUnit ? atof((CStringA)strTmp2.GetBuffer(0)) :
				 CJobFile::InchToMM(atof((CStringA)strTmp2.GetBuffer(0)));
		}
		else if (strTmp.Find(L"YSIZE") != -1)
		{
			strTmp2 = strTmp.Right(strTmp.GetLength() - sizeof("YSIZE") - 1);
			
			m_dYSize = bIsMMUnit ? atof((CStringA)strTmp2.GetBuffer(0)) :
				CJobFile::InchToMM(atof((CStringA)strTmp2.GetBuffer(0)));
		}
		else if (strTmp.Find(L"OFFSET") != -1)
		{
			strTmp2 = strTmp.Right(strTmp.GetLength() - sizeof("OFFSET") - 1);
			
			m_dOffset = bIsMMUnit ? atof((CStringA)strTmp2.GetBuffer(0)) :
				CJobFile::InchToMM(atof((CStringA)strTmp2.GetBuffer(0)));
		}
		else if (strTmp.Find(L"CHAR") != -1)
		{
			pFontCharTmp = new CFontChar;

			nTmp = strTmp.Find(' ');

			if (nTmp != -1)
			{
				strTmp.Delete(0, nTmp + 1);
				strTmp.TrimLeft(L"\t ");
				strTmp.TrimRight(L"\t ");
				pFontCharTmp->m_Char = strTmp.GetAt(0);
			}

			while (cStdFile.ReadString(strTmp))
			{
				strTmp.TrimLeft(L"\t ");
				strTmp.TrimRight(L"\t ");
				strTmp.MakeUpper();

				if (strTmp.GetLength() == 0)  // 빈줄 Skip
					continue;

				if (strTmp.Find(L"ECHAR") != -1)
					break;

				if (strTmp.Find(L"LINE") != -1)
					pFontCharTmp->LoadFontLine(strTmp, bIsMMUnit);
			}
			m_arrFontChar.Add(pFontCharTmp);
		}
		else
			OutputDebugString(strTmp + L" : It is a new member or a error in standard File\n");
	}
	//	ar.Close();
	cStdFile.Close();
	return RESULT_GOOD;
}

void CUserSymbolAttrlist::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		ar.Write(&m_bOutBreak, sizeof(m_bOutBreak));
		ar.Write(&m_bOutScale, sizeof(m_bOutScale));
		ar.Write(&m_bBreakAway, sizeof(m_bBreakAway));		
		ar << m_nEntityVersion;
		ar << m_dFillDx;
		ar << m_dFillDy;
		ar << m_dImageDx;
		ar << m_dImageDy;
		ar << m_strComment;
		ar << m_strSourceName;
	}
	else
	{
		ar.Read(&m_bOutBreak, sizeof(m_bOutBreak));
		ar.Read(&m_bOutScale, sizeof(m_bOutScale));
		ar.Read(&m_bBreakAway, sizeof(m_bBreakAway));		
		ar >> m_nEntityVersion;
		ar >> m_dFillDx;
		ar >> m_dFillDy;
		ar >> m_dImageDx;
		ar >> m_dImageDy;
		ar >> m_strComment;
		ar >> m_strSourceName;
	}
}

void CSymbol::SetSymbolName(CString strSymbolName, CString strParams)
{
	if (!strSymbolName.CompareNoCase(L"R"))
		m_eSymbolName = SymbolName::r;
	else if (!strSymbolName.CompareNoCase(L"S"))
		m_eSymbolName = SymbolName::s;
	else if (!strSymbolName.CompareNoCase(L"RECT"))
	{
		if (strParams.Find(L"XR") != -1)
			m_eSymbolName = SymbolName::rectxr;
		else if (strParams.Find(L"XC") != -1)
			m_eSymbolName = SymbolName::rectxc;
		else
			m_eSymbolName = SymbolName::rect;
	}
	else if (!strSymbolName.CompareNoCase(L"OVAL"))
		m_eSymbolName = SymbolName::oval;
	else if (!strSymbolName.CompareNoCase(L"DI"))
		m_eSymbolName = SymbolName::di;
	else if (!strSymbolName.CompareNoCase(L"OCT"))
		m_eSymbolName = SymbolName::octagon;
	else if (!strSymbolName.CompareNoCase(L"DONUT_R"))
		m_eSymbolName = SymbolName::donut_r;
	else if (!strSymbolName.CompareNoCase(L"DONUT_S"))
		m_eSymbolName = SymbolName::donut_s;
	else if (!strSymbolName.CompareNoCase(L"HEX_L"))
		m_eSymbolName = SymbolName::hex_l;
	else if (!strSymbolName.CompareNoCase(L"HEX_S"))
		m_eSymbolName = SymbolName::hex_s;
	else if (!strSymbolName.CompareNoCase(L"BFR"))
		m_eSymbolName = SymbolName::bfr;
	else if (!strSymbolName.CompareNoCase(L"BFS"))
		m_eSymbolName = SymbolName::bfs;
	else if (!strSymbolName.CompareNoCase(L"TRI"))
		m_eSymbolName = SymbolName::tri;
	else if (!strSymbolName.CompareNoCase(L"OVAL_H"))
		m_eSymbolName = SymbolName::oval_h;
	else if (!strSymbolName.CompareNoCase(L"THR"))
		m_eSymbolName = SymbolName::thr;
	else if (!strSymbolName.CompareNoCase(L"THS"))
		m_eSymbolName = SymbolName::ths;
	else if (!strSymbolName.CompareNoCase(L"S_THS"))
		m_eSymbolName = SymbolName::s_ths;
	else if (!strSymbolName.CompareNoCase(L"S_THO"))
		m_eSymbolName = SymbolName::s_tho;
	else if (!strSymbolName.CompareNoCase(L"SR_THS"))
		m_eSymbolName = SymbolName::sr_ths;
	else if (!strSymbolName.CompareNoCase(L"RC_THS"))
		m_eSymbolName = SymbolName::rc_ths;
	else if (!strSymbolName.CompareNoCase(L"RC_THO"))
		m_eSymbolName = SymbolName::rc_tho;
	else if (!strSymbolName.CompareNoCase(L"EL"))
		m_eSymbolName = SymbolName::el;
	else if (!strSymbolName.CompareNoCase(L"MOIRE"))
		m_eSymbolName = SymbolName::moire;
	else if (!strSymbolName.CompareNoCase(L"HOLE"))
		m_eSymbolName = SymbolName::hole;
	else if (!strSymbolName.CompareNoCase(L"NULL"))
		m_eSymbolName = SymbolName::null;
	else	
		m_eSymbolName = SymbolName::userdefined;		
	
	m_strUserSymbol = strParams;
}

void CSymbol::SetParamVal(int nIndex, CString strValue)
{
	double dValue = 0.0;
	if (SymbolName::hole == m_eSymbolName && 1 == nIndex)
	{
		switch (strValue.GetAt(0))
		{
		case 'P': dValue = 1.0; break;
		case 'N': dValue = 0.0; break;
		case 'V': dValue = -1.0; break;
		}
	}
	else
		dValue = atof((CStringA)strValue.GetBuffer(0));

	m_vcParams.emplace_back(dValue);
}

void CSymbol::ConvertValue(class CJobFile* pJobFile)
{
	switch (m_eSymbolName)
	{
	case SymbolName::r:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		break;
	case SymbolName::s:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		break;
	case SymbolName::rect:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		m_vcParams[1] = CJobFile::MilToMM(m_vcParams[1]);
		break;
	case SymbolName::rectxr:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		m_vcParams[1] = CJobFile::MilToMM(m_vcParams[1]);
		m_vcParams[2] = CJobFile::MilToMM(m_vcParams[2]);
		break;
	case SymbolName::rectxc:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		m_vcParams[1] = CJobFile::MilToMM(m_vcParams[1]);
		m_vcParams[2] = CJobFile::MilToMM(m_vcParams[2]);
		break;
	case SymbolName::oval:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		m_vcParams[1] = CJobFile::MilToMM(m_vcParams[1]);
		break;
	case SymbolName::di:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		m_vcParams[1] = CJobFile::MilToMM(m_vcParams[1]);
		break;
	case SymbolName::octagon:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		m_vcParams[1] = CJobFile::MilToMM(m_vcParams[1]);
		m_vcParams[2] = CJobFile::MilToMM(m_vcParams[2]);
		break;
	case SymbolName::donut_r:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		m_vcParams[1] = CJobFile::MilToMM(m_vcParams[1]);
		break;
	case SymbolName::donut_s:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		m_vcParams[1] = CJobFile::MilToMM(m_vcParams[1]);
		break;
	case SymbolName::hex_l:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		m_vcParams[1] = CJobFile::MilToMM(m_vcParams[1]);
		m_vcParams[2] = CJobFile::MilToMM(m_vcParams[2]);
		break;
	case SymbolName::hex_s:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		m_vcParams[1] = CJobFile::MilToMM(m_vcParams[1]);
		m_vcParams[2] = CJobFile::MilToMM(m_vcParams[2]);
		break;
	case SymbolName::bfr:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		break;
	case SymbolName::bfs:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		break;
	case SymbolName::tri:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		m_vcParams[1] = CJobFile::MilToMM(m_vcParams[1]);
		break;
	case SymbolName::oval_h:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		m_vcParams[1] = CJobFile::MilToMM(m_vcParams[1]);
		break;
	case SymbolName::thr:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		m_vcParams[1] = CJobFile::MilToMM(m_vcParams[1]);
		m_vcParams[4] = CJobFile::MilToMM(m_vcParams[4]);
		break;
	case SymbolName::ths:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		m_vcParams[1] = CJobFile::MilToMM(m_vcParams[1]);
		m_vcParams[4] = CJobFile::MilToMM(m_vcParams[4]);
		break;
	case SymbolName::s_ths:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		m_vcParams[1] = CJobFile::MilToMM(m_vcParams[1]);
		m_vcParams[4] = CJobFile::MilToMM(m_vcParams[4]);
		break;
	case SymbolName::s_tho:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		m_vcParams[1] = CJobFile::MilToMM(m_vcParams[1]);
		m_vcParams[4] = CJobFile::MilToMM(m_vcParams[4]);
		break;
	case SymbolName::sr_ths:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		m_vcParams[1] = CJobFile::MilToMM(m_vcParams[1]);
		m_vcParams[4] = CJobFile::MilToMM(m_vcParams[4]);
		break;
	case SymbolName::rc_ths:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		m_vcParams[1] = CJobFile::MilToMM(m_vcParams[1]);
		m_vcParams[4] = CJobFile::MilToMM(m_vcParams[4]);
		m_vcParams[5] = CJobFile::MilToMM(m_vcParams[5]);
		break;
	case SymbolName::rc_tho:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		m_vcParams[1] = CJobFile::MilToMM(m_vcParams[1]);
		m_vcParams[4] = CJobFile::MilToMM(m_vcParams[4]);
		m_vcParams[5] = CJobFile::MilToMM(m_vcParams[5]);
		break;
	case SymbolName::el:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		m_vcParams[1] = CJobFile::MilToMM(m_vcParams[1]);
		break;
	case SymbolName::moire:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		m_vcParams[1] = CJobFile::MilToMM(m_vcParams[1]);
		m_vcParams[2] = m_vcParams[2];
		m_vcParams[3] = CJobFile::MilToMM(m_vcParams[3]);
		m_vcParams[4] = CJobFile::MilToMM(m_vcParams[4]);
		break;
	case SymbolName::hole:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		break;
	case SymbolName::null:
		m_vcParams[0] = CJobFile::MilToMM(m_vcParams[0]);
		break;
	case SymbolName::userdefined:
	{
		for (int i = 0, nRet = -1; i < pJobFile->m_arrSymbol.GetSize(); i++)
		{
			nRet = m_strUserSymbol.CompareNoCase(pJobFile->m_arrSymbol[i]->m_strSymbolName.GetBuffer(0));
			if (!nRet)
			{
				m_pUserSymbol = pJobFile->m_arrSymbol[i];
				break;
			}
		}

		
	}
	break;
	case SymbolName::SymbolNameNone:
		break;
	};
}

void CSymbol::SetMinMaxRect()
{
	switch (m_eSymbolName)
	{
	case SymbolName::r:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[0] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[0] / 2;
		break;
	case SymbolName::s:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[0] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[0] / 2;
		break;
	case SymbolName::rect:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[1] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[1] / 2;
		break;
	case SymbolName::rectxr:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[1] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[1] / 2;
		break;
	case SymbolName::rectxc:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[1] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[1] / 2;
		break;
	case SymbolName::oval:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[1] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[1] / 2;
		break;
	case SymbolName::di:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[1] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[1] / 2;
		break;
	case SymbolName::octagon:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[1] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[1] / 2;
		break;
	case SymbolName::donut_r:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[0] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[0] / 2;
		break;
	case SymbolName::donut_s:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[0] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[0] / 2;
		break;
	case SymbolName::hex_l:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[1] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[1] / 2;
		break;
	case SymbolName::hex_s:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[1] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[1] / 2;
		break;
	case SymbolName::bfr:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[0] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[0] / 2;
		break;
	case SymbolName::bfs:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[0] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[0] / 2;
		break;
	case SymbolName::tri:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[1] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[1] / 2;
		break;
	case SymbolName::oval_h:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[1] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[1] / 2;
		break;
	case SymbolName::thr:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[0] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[0] / 2;
		break;
	case SymbolName::ths:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[0] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[0] / 2;
		break;
	case SymbolName::s_ths:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[0] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[0] / 2;
		break;
	case SymbolName::s_tho:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[0] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[0] / 2;
		break;
	case SymbolName::sr_ths:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[0] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[0] / 2;
		break;
	case SymbolName::rc_ths:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[1] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[1] / 2;
		break;
	case SymbolName::rc_tho:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[1] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[1] / 2;
		break;
	case SymbolName::el:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[1] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[1] / 2;
		break;
	case SymbolName::moire:
		m_MinMaxRect.left = -1 * m_vcParams[4] / 2;
		m_MinMaxRect.right = m_vcParams[4] / 2;
		m_MinMaxRect.top = m_vcParams[4] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[4] / 2;
		break;
	case SymbolName::hole:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[0] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[0] / 2;
		break;
	case SymbolName::null:
		m_MinMaxRect.left = -1 * m_vcParams[0] / 2;
		m_MinMaxRect.right = m_vcParams[0] / 2;
		m_MinMaxRect.top = m_vcParams[0] / 2;
		m_MinMaxRect.bottom = -1 * m_vcParams[0] / 2;
		break;
	case SymbolName::userdefined:

		if (m_pUserSymbol != NULL)
		{
			//User Symbol영역 처리시 기준점을 찾았다. 혹시 사용할지 모르니깐.. 내비두자!
			/*
			m_MinMaxRect.top = (m_pUserSymbol->m_FeatureFile.m_MinMaxRect.top-m_pUserSymbol->m_FeatureFile.m_MinMaxRect.bottom)/2;
			m_MinMaxRect.bottom = -1*(m_pUserSymbol->m_FeatureFile.m_MinMaxRect.top-m_pUserSymbol->m_FeatureFile.m_MinMaxRect.bottom)/2;
			m_MinMaxRect.left = -1*(m_pUserSymbol->m_FeatureFile.m_MinMaxRect.right-m_pUserSymbol->m_FeatureFile.m_MinMaxRect.left)/2;
			m_MinMaxRect.right = (m_pUserSymbol->m_FeatureFile.m_MinMaxRect.right-m_pUserSymbol->m_FeatureFile.m_MinMaxRect.left)/2;
			*/

			m_MinMaxRect.top = m_pUserSymbol->m_FeatureFile.m_MinMaxRect.top;
			m_MinMaxRect.bottom = m_pUserSymbol->m_FeatureFile.m_MinMaxRect.bottom;
			m_MinMaxRect.left = m_pUserSymbol->m_FeatureFile.m_MinMaxRect.left;
			m_MinMaxRect.right = m_pUserSymbol->m_FeatureFile.m_MinMaxRect.right;

			

		}
		break;
	case SymbolName::SymbolNameNone:
		break;
	}
}

UINT32 CSymbol::SetFinalParamVal(int nFinalIndex, CString strValue, CJobFile* pJobFile, bool bRotated)
{
	UINT32 ivcSize = static_cast<UINT32>(m_vcParams.size());
	UINT32 iIndex = static_cast<UINT32>(nFinalIndex + 1);
	if (ivcSize < iIndex)
	{
		for (UINT32 i = ivcSize; i <= iIndex; i++)
			m_vcParams.emplace_back(0.f);
	}

	switch (m_eSymbolName)
	{
	case SymbolName::rectxr:
	case SymbolName::rectxc:
	{
		if (bRotated)
		{
			if (3 == nFinalIndex)	// Rotated, Corner가 생략된 경우
			{
				m_vcParams[nFinalIndex] = 0.0;
				m_vcParams[nFinalIndex + 1] = atof((CStringA)strValue.GetBuffer(0));
			}
			else	// Rotated, Corner가 있는 경우
				m_vcParams[nFinalIndex] = atof((CStringA)strValue.GetBuffer(0));
		}
		else
		{
			if (2 == nFinalIndex)
			{
				m_vcParams[nFinalIndex] = atof((CStringA)strValue.GetBuffer(0));
				m_vcParams[nFinalIndex + 1] = 0.0;
			}
			else	//Corner가 있는 경우
				m_vcParams[nFinalIndex] = atof((CStringA)strValue.GetBuffer(0));
		}
		break;
	}
	case SymbolName::userdefined:
	{
		int i, nTmp;
		for (i = 0; i < pJobFile->m_arrSymbol.GetSize(); i++)
		{
			nTmp = pJobFile->m_arrSymbol[i]->m_strSymbolName.CompareNoCase(m_strUserSymbol.GetBuffer(0));
			if (!nTmp)
			{
				m_pUserSymbol = pJobFile->m_arrSymbol[i];
				break;
			}
		}
		break;
	}
	default:
		m_vcParams[nFinalIndex] = atof((CStringA)strValue.GetBuffer(0));
		break;
	}
	return RESULT_GOOD;
}

void CFeature::Serialize(CArchive& ar, CFeatureFile* pFeatureFile)
{
	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		//ar.Write(&m_bHighlight, sizeof(m_bHighlight));
		ar.Write(&m_bPolarity, sizeof(m_bPolarity));
		ar.Write(&m_eType, sizeof(m_eType));
		ar << m_nDecode;
		ar.Write(&m_MinMaxRect, sizeof(RECTD));
	}
	else
	{
		//ar.Read(&m_bHighlight, sizeof(m_bHighlight));
		ar.Read(&m_bPolarity, sizeof(m_bPolarity));
		ar.Read(&m_eType, sizeof(m_eType));
		ar >> m_nDecode;
		ar.Read(&m_MinMaxRect, sizeof(RECTD));		
	}
}

void CFeature::LoadFeatureAttribute(CString strAttribute, CFeatureFile* pFeatureFile, CJobFile* pJobFile)
{
	if (pFeatureFile == NULL || pJobFile == NULL) return;

	int nAttrNum = static_cast<int>(pFeatureFile->m_arrAttribute.GetSize());
	if (nAttrNum <= 0) return;

	int nStringNum = static_cast<int>(pFeatureFile->m_arrAttributeTextString.GetSize());	
	if (nStringNum <= 0) return;

	int nPadUsageNum = static_cast<int>(pJobFile->m_arrPadUsage.GetSize());
	if (nPadUsageNum <= 0) 
		return;

	int nTmp = 0, iAttr_Serial = 0, iString_Serial = 0;
	CString str = L"", strTmp = strAttribute;
	CString strAttr = L"", strValue = L"";
	
	while (strTmp.GetLength() > 0)		// atr=value,atr=value,.....
	{
		AfxExtractSubString(str, strTmp, 0, ',');
		//atr = value
		if ((nTmp = str.Find('=')) != -1)
		{
			strAttr = str.Left(nTmp);
			strAttr.TrimLeft(L"\t ");
			strAttr.TrimRight(L"\t ");
			iAttr_Serial = _ttoi(strAttr);

			strValue = str.Right(str.GetLength() - nTmp - 1);
			strValue.TrimLeft(L"\t ");
			strValue.TrimRight(L"\t ");
			iString_Serial = _ttoi(strValue);

			if (iAttr_Serial < nAttrNum && iString_Serial < nStringNum)
			{
				if (pFeatureFile->m_arrAttribute[iAttr_Serial]->m_strName.CompareNoCase(L".pad_usage") == 0)
				{
					if (iString_Serial < nPadUsageNum)
					{
						m_arrAttribute.Add(pFeatureFile->m_arrAttribute[iAttr_Serial]);
						m_arrAttributeTextString.Add(pJobFile->m_arrPadUsage[iString_Serial]);
					}
				}
				else
				{
					m_arrAttribute.Add(pFeatureFile->m_arrAttribute[iAttr_Serial]);
					m_arrAttributeTextString.Add(pFeatureFile->m_arrAttributeTextString[iString_Serial]);
				}
			}
		}
		else
		{
			iAttr_Serial = _ttoi(str);

			m_arrAttribute.Add(pFeatureFile->m_arrAttribute[iAttr_Serial]);
			m_arrAttributeTextString.Add(nullptr);
		}

		strTmp.Delete(0, str.GetLength() + 1);
	}
}

void CFeatureL::SetMinMaxRect()
{
	if (m_pSymbol)
	{
		m_MinMaxRect.top = max(
			max(m_dYs + m_pSymbol->m_MinMaxRect.top, m_dYs + m_pSymbol->m_MinMaxRect.bottom),
			max(m_dYe + m_pSymbol->m_MinMaxRect.top, m_dYe + m_pSymbol->m_MinMaxRect.bottom)
		);

		m_MinMaxRect.bottom = min(
			min(m_dYs + m_pSymbol->m_MinMaxRect.top, m_dYs + m_pSymbol->m_MinMaxRect.bottom),
			min(m_dYe + m_pSymbol->m_MinMaxRect.top, m_dYe + m_pSymbol->m_MinMaxRect.bottom)
		);

		m_MinMaxRect.left = min(
			min(m_dXs + m_pSymbol->m_MinMaxRect.left, m_dXs + m_pSymbol->m_MinMaxRect.right),
			min(m_dXe + m_pSymbol->m_MinMaxRect.left, m_dXe + m_pSymbol->m_MinMaxRect.right)
		);

		m_MinMaxRect.right = max(
			max(m_dXs + m_pSymbol->m_MinMaxRect.left, m_dXs + m_pSymbol->m_MinMaxRect.right),
			max(m_dXe + m_pSymbol->m_MinMaxRect.left, m_dXe + m_pSymbol->m_MinMaxRect.right)
		);
	}
}

void CFeatureL::LoadFeature(CString strOneLine, CStdioFile& cStdFile, CFeatureFile* pFeatureFile, CJobFile* pJobFile, bool bIsMMUnit)
{
	CString strTmpParams, strTmpAttributes;
	CString strTmp;
	int nTmp;

	// Param과 attribute 내용을 분리함.
	if ((nTmp = strOneLine.Find(';')) != -1)
	{
		strTmpParams = strOneLine.Left(nTmp);
		strTmpParams.TrimLeft(L"\t ");
		strTmpParams.TrimRight(L"\t ");
		strTmpAttributes = strOneLine.Right(strOneLine.GetLength() - nTmp - 1);
		strTmpAttributes.TrimLeft(L"\t ");
		strTmpAttributes.TrimRight(L"\t ");
	}
	else
	{
		strTmpParams = strOneLine;
		strTmpAttributes = "";
	}

	//Parameter를 축출함.
	if ((nTmp = strTmpParams.Find(' ')) != -1) // Feature 문자 제거
		strTmpParams.Delete(0, nTmp + 1);

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // xs 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		
		m_dXs = bIsMMUnit ? atof((CStringA)strTmp.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // ys 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		
		m_dYs = bIsMMUnit ? atof((CStringA)strTmp.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // xe 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		
		m_dXe = bIsMMUnit? atof((CStringA)strTmp.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // ye 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		
		m_dYe = bIsMMUnit ? atof((CStringA)strTmp.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // sym_num 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		m_nSymNum = atoi((CStringA)strTmp.GetBuffer(0));
		strTmpParams.Delete(0, nTmp + 1);
		for (int i = 0; i < pFeatureFile->m_arrSymbol.GetSize(); i++)
		{
			if (m_nSymNum == pFeatureFile->m_arrSymbol[i]->m_iSerialNum)
			{
				m_pSymbol = pFeatureFile->m_arrSymbol[i];
				break;
			}
		}
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // polarity 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		if (strTmp.GetAt(0) == 'P')
			m_bPolarity = true;
		else
			m_bPolarity = false;

		strTmpParams.Delete(0, nTmp + 1);
	}

	if (strTmpParams.GetLength())  // dcode 값 축출
	{
		m_nDecode = atoi((CStringA)strTmpParams.GetBuffer(0));
	}

	//Attribute 부분을 축출함.
	LoadFeatureAttribute(strTmpAttributes, pFeatureFile, pJobFile);  // Attribute parsing and loading
}

void CFeatureL::Serialize(CArchive& ar, CFeatureFile* pFeatureFile)
{
	CFeature::Serialize(ar, pFeatureFile);

	if (ar.IsStoring())
	{
		ar << m_nSymNum;
		ar << m_dXs;
		ar << m_dYs;
		ar << m_dXe;
		ar << m_dYe;

		ar << m_bHighlight;
		ar << m_bShow;
	}
	else
	{
		ar >> m_nSymNum;
		ar >> m_dXs;
		ar >> m_dYs;
		ar >> m_dXe;
		ar >> m_dYe;

		ar >> m_bHighlight;
		ar >> m_bShow;

		//해당 Feature에 연관된 Symbol 포인트를 지정함.
		for (UINT32 i = 0; i < pFeatureFile->m_arrSymbol.GetSize(); i++)
		{
			if (m_nSymNum == pFeatureFile->m_arrSymbol[i]->m_iSerialNum)
			{
				m_pSymbol = pFeatureFile->m_arrSymbol[i];
				break;
			}
		}
	}
}


void CFeatureZ::SetMinMaxRect()
{
	if (m_pSymbol)
	{

		

// 		m_MinMaxRect.top = max(m_dY + m_pSymbol->m_MinMaxRect.top, m_dY + m_pSymbol->m_MinMaxRect.bottom);
// 		m_MinMaxRect.bottom = min(m_dY + m_pSymbol->m_MinMaxRect.top, m_dY + m_pSymbol->m_MinMaxRect.bottom);
// 		m_MinMaxRect.left = min(m_dX + m_pSymbol->m_MinMaxRect.left, m_dX + m_pSymbol->m_MinMaxRect.right);
// 		m_MinMaxRect.right = max(m_dX + m_pSymbol->m_MinMaxRect.left, m_dX + m_pSymbol->m_MinMaxRect.right);

		m_MinMaxRect.top = m_dY + 0.3;
		m_MinMaxRect.bottom = m_dY - 0.3;
		m_MinMaxRect.left = m_dX - 0.3;
		m_MinMaxRect.right = m_dX + 0.3;


	}
}

void CFeatureZ::Serialize(CArchive& ar, CFeatureFile* pFeatureFile)
{
	CFeature::Serialize(ar, pFeatureFile);

	if (ar.IsStoring())
	{
		ar << m_nSymNum;
		ar << m_dX;
		ar << m_dY;
		

		ar << m_bHighlight;
		ar << m_bShow;
	}
	else
	{
		ar >> m_nSymNum;
		ar >> m_dX;
		ar >> m_dY;
		

		ar >> m_bHighlight;
		ar >> m_bShow;

		//해당 Feature에 연관된 Symbol 포인트를 지정함.
		for (UINT32 i = 0; i < pFeatureFile->m_arrSymbol.GetSize(); i++)
		{
			if (m_nSymNum == pFeatureFile->m_arrSymbol[i]->m_iSerialNum)
			{
				m_pSymbol = pFeatureFile->m_arrSymbol[i];
				break;
			}
		}
	}
}
void CFeatureZ::LoadFeature(CString strOneLine, CStdioFile& cStdFile, class CFeatureFile* pFeatureFile /* = NULL */, class CJobFile* pJobFile /* = NULL */, bool bIsMMUnit /* = false */)
{
	CString strTmpParams, strTmpAttributes;
	CString strTmp;
	int nTmp;

	// Param과 attribute 내용을 분리함.
	if ((nTmp = strOneLine.Find(';')) != -1)
	{
		strTmpParams = strOneLine.Left(nTmp);
		strTmpParams.TrimLeft(L"\t ");
		strTmpParams.TrimRight(L"\t ");
		strTmpAttributes = strOneLine.Right(strOneLine.GetLength() - nTmp - 1);
		strTmpAttributes.TrimLeft(L"\t ");
		strTmpAttributes.TrimRight(L"\t ");
	}
	else
	{
		strTmpParams = strOneLine;
		strTmpAttributes = "";
	}

	//Parameter를 축출함.
	if ((nTmp = strTmpParams.Find(' ')) != -1) // Feature 문자 제거
		strTmpParams.Delete(0, nTmp + 1);

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // x 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);

		m_dX = bIsMMUnit ? atof((CStringA)strTmp.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // y 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);

		m_dY = bIsMMUnit ? atof((CStringA)strTmp.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // sym_num 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		m_nSymNum = atoi((CStringA)strTmp.GetBuffer(0));
		ASSERT(m_nSymNum >= 0);			// -1 <sym_num> <resize_factor> for resized symbol : currently not supported
										// resize_factor : thousandths of the units being used (mils or microns)

		strTmpParams.Delete(0, nTmp + 1);
		for (int i = 0; i < pFeatureFile->m_arrSymbol.GetSize(); i++)
		{
			if (m_nSymNum == pFeatureFile->m_arrSymbol[i]->m_iSerialNum)
			{
				m_pSymbol = pFeatureFile->m_arrSymbol[i];
				break;
			}
		}
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // polarity 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		if (strTmp.GetAt(0) == 'P')
			m_bPolarity = true;
		else
			m_bPolarity = false;

		strTmpParams.Delete(0, nTmp + 1);
	}

	if (strTmpParams.GetLength())  // dcode 값 축출
	{
		m_nDecode = atoi((CStringA)strTmpParams.GetBuffer(0));
	}

	//Attribute 부분을 축출함.
	LoadFeatureAttribute(strTmpAttributes, pFeatureFile, pJobFile);  // Attribute parsing and loading

}


void CFeatureP::SetMinMaxRect()
{
	if (m_pSymbol)
	{
		m_MinMaxRect.top = max(m_dY + m_pSymbol->m_MinMaxRect.top, m_dY + m_pSymbol->m_MinMaxRect.bottom);
		m_MinMaxRect.bottom = min(m_dY + m_pSymbol->m_MinMaxRect.top, m_dY + m_pSymbol->m_MinMaxRect.bottom);
		m_MinMaxRect.left = min(m_dX + m_pSymbol->m_MinMaxRect.left, m_dX + m_pSymbol->m_MinMaxRect.right);
		m_MinMaxRect.right = max(m_dX + m_pSymbol->m_MinMaxRect.left, m_dX + m_pSymbol->m_MinMaxRect.right);
	}
}

void CFeatureP::LoadFeature(CString strOneLine, CStdioFile& cStdFile, CFeatureFile* pFeatureFile, CJobFile* pJobFile, bool bIsMMUnit)
{
	CString strTmpParams, strTmpAttributes;
	CString strTmp;
	int nTmp;

	// Param과 attribute 내용을 분리함.
	if ((nTmp = strOneLine.Find(';')) != -1)
	{
		strTmpParams = strOneLine.Left(nTmp);
		strTmpParams.TrimLeft(L"\t ");
		strTmpParams.TrimRight(L"\t ");
		strTmpAttributes = strOneLine.Right(strOneLine.GetLength() - nTmp - 1);
		strTmpAttributes.TrimLeft(L"\t ");
		strTmpAttributes.TrimRight(L"\t ");
	}
	else
	{
		strTmpParams = strOneLine;
		strTmpAttributes = "";
	}

	//Parameter를 축출함.
	if ((nTmp = strTmpParams.Find(' ')) != -1) // Feature 문자 제거
		strTmpParams.Delete(0, nTmp + 1);

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // x 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		
		m_dX = bIsMMUnit ? atof((CStringA)strTmp.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // y 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		
		m_dY = bIsMMUnit ? atof((CStringA)strTmp.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // sym_num 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		m_nSymNum = atoi((CStringA)strTmp.GetBuffer(0));
		ASSERT(m_nSymNum >= 0);			// -1 <sym_num> <resize_factor> for resized symbol : currently not supported
										// resize_factor : thousandths of the units being used (mils or microns)

		strTmpParams.Delete(0, nTmp + 1);
		for (int i = 0; i < pFeatureFile->m_arrSymbol.GetSize(); i++)
		{
			if (m_nSymNum == pFeatureFile->m_arrSymbol[i]->m_iSerialNum)
			{
				m_pSymbol = pFeatureFile->m_arrSymbol[i];
				break;
			}
		}
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // polarity 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		if (strTmp.GetAt(0) == 'P')
			m_bPolarity = true;
		else
			m_bPolarity = false;

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // dcode 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		m_nDecode = atoi((CStringA)strTmp.GetBuffer(0));
		strTmpParams.Delete(0, nTmp + 1);
	}

	if (strTmpParams.GetLength())   //orient값 축출 
	{
		switch (atoi((CStringA)strTmpParams.GetBuffer(0)))
		{
		case static_cast<UINT8>(Orient::NoMir0Deg) :
			m_eOrient = Orient::NoMir0Deg;
			break;
		case static_cast<UINT8>(Orient::NoMir90Deg) :
			m_eOrient = Orient::NoMir90Deg;
			break;
		case static_cast<UINT8>(Orient::NoMir180Deg) :
			m_eOrient = Orient::NoMir180Deg;
			break;
		case static_cast<UINT8>(Orient::NoMir270Deg) :
			m_eOrient = Orient::NoMir270Deg;
			break;
		case static_cast<UINT8>(Orient::Mir0Deg) :
			m_eOrient = Orient::Mir0Deg;
			break;
		case static_cast<UINT8>(Orient::Mir90Deg) :
			m_eOrient = Orient::Mir90Deg;
			break;
		case static_cast<UINT8>(Orient::Mir180Deg) :
			m_eOrient = Orient::Mir180Deg;
			break;
		case static_cast<UINT8>(Orient::Mir270Deg) :
			m_eOrient = Orient::Mir270Deg;
			break;
		case static_cast<UINT8>(Orient::NoMirAnyDeg) :
			m_eOrient = Orient::NoMirAnyDeg;
			break;
		case static_cast<UINT8>(Orient::MirAnyDeg) :
			m_eOrient = Orient::MirAnyDeg;
			break;
		default:
			m_eOrient = Orient::OrientNone;
		}
	}

	//Attribute 부분을 축출함.
	LoadFeatureAttribute(strTmpAttributes, pFeatureFile, pJobFile);  // Attribute parsing and loading
}

void CFeatureP::Serialize(CArchive& ar, CFeatureFile* pFeatureFile)
{
	CFeature::Serialize(ar, pFeatureFile);

	if (ar.IsStoring())
	{
		ar << m_nSymNum;
		ar << m_dX;
		ar << m_dY;		
		ar.Write(&m_eOrient, sizeof(m_eOrient));

		ar << m_bHighlight;
		ar << m_bShow;
	}
	else
	{
		ar >> m_nSymNum;
		ar >> m_dX;
		ar >> m_dY;		
		ar.Read(&m_eOrient, sizeof(m_eOrient));

		ar >> m_bHighlight;
		ar >> m_bShow;

		//해당 Feature에 연관된 Symbol 포인트를 지정함.
		for (UINT32 i = 0; i < pFeatureFile->m_arrSymbol.GetSize(); i++)
		{
			if (m_nSymNum == pFeatureFile->m_arrSymbol[i]->m_iSerialNum)
			{
				m_pSymbol = pFeatureFile->m_arrSymbol[i];
				break;
			}
		}
	}
}

void CFeatureA::SetMinMaxRect()
{
	double X1, X2, Y1, Y2, XC, YC;
	double X1s, X2s, Y1s, Y2s, xl[6] = { 0 }, yl[6] = { 0 };
	double dRadius = 0, xMIN, yMIN, xMAX, yMAX;
	RECTD tmpMinMax;
	int	   nNumP = 0;				// 극점 후보 수 

	X1 = m_dXs;	X2 = m_dXe;	XC = m_dXc;	X1s = X1 - XC; X2s = X2 - XC;
	Y1 = m_dYs;	Y2 = m_dYe;	YC = m_dYc; Y1s = Y1 - YC; Y2s = Y2 - YC;
	dRadius = sqrt((XC - X1) * (XC - X1) + (YC - Y1) * (YC - Y1));

	xl[0] = X1s, yl[0] = Y1s;	// start
	xl[1] = X2s, yl[1] = Y2s;	// end
	nNumP = 2;

	if (X1 < X2) { tmpMinMax.left = X1;	tmpMinMax.right = X2; }
	else { tmpMinMax.left = X2; tmpMinMax.right = X1; }

	if (Y1 < Y2) { tmpMinMax.bottom = Y1; tmpMinMax.top = Y2; }
	else { tmpMinMax.bottom = Y2; tmpMinMax.top = Y1; }

	if (m_bCw)
	{
		if (X1 == X2 && Y1 == Y2) //같으면 모든 극점포함
		{
			nNumP = 6;
			xl[2] = 0.0;		yl[2] = dRadius;
			xl[3] = -dRadius;	yl[3] = 0.0;
			xl[4] = 0.0;		yl[4] = -dRadius;
			xl[5] = dRadius;	yl[5] = 0.0;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s >= 0 && Y2s >= 0)	// CW 1사분면
		{
			if ((X2s - X1s) >= 0 && (Y2s - Y1s) <= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 2 + 1;
			xl[2] = dRadius;	yl[2] = 0.0;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 5;
			xl[2] = dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = -dRadius;
			xl[4] = -dRadius;	yl[4] = 0.0;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 4;
			xl[2] = dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = -dRadius;
		}
		else if (X1s >= 0 && Y1s < 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 5;
			xl[2] = 0.0;		yl[2] = -dRadius;
			xl[3] = -dRadius;	yl[3] = 0.0;
			xl[4] = 0.0;		yl[4] = dRadius;
		}
		else if (X1s >= 0 && Y1s < 0 && X2s >= 0 && Y2s < 0) // CW 4사분면
		{
			if ((X2s - X1s) <= 0 && (Y2s - Y1s) <= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else if (X1s >= 0 && Y1s < 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 4;
			xl[2] = 0.0;		yl[2] = -dRadius;
			xl[3] = -dRadius;	yl[3] = 0.0;
		}
		else if (X1s >= 0 && Y1s < 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 3;
			xl[2] = 0.0;	yl[2] = -dRadius;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 3;
			xl[2] = 0.0;	yl[2] = dRadius;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 4;
			xl[2] = 0.0;		yl[2] = dRadius;
			xl[3] = dRadius;	yl[3] = 0.0;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s < 0 && Y2s >= 0)	// CW 2사분면
		{
			if ((X2s - X1s) >= 0 && (Y2s - Y1s) >= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else if (X1s < 0 && Y1s >= 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 5;
			xl[2] = 0.0;		yl[2] = dRadius;
			xl[3] = dRadius;	yl[3] = 0.0;
			xl[4] = 0.0;		yl[4] = -dRadius;
		}
		else if (X1s < 0 && Y1s < 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 4;
			xl[2] = -dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = dRadius;
		}
		else if (X1s < 0 && Y1s < 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 5;
			xl[2] = -dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = dRadius;
			xl[4] = dRadius;	yl[4] = 0.0;
		}
		else if (X1s < 0 && Y1s < 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 3;
			xl[2] = -dRadius;	yl[2] = 0.0;
		}
		else if (X1s < 0 && Y1s < 0 && X2s < 0 && Y2s < 0) // CW 3사분면
		{
			if ((X2s - X1s) <= 0 && (Y2s - Y1s) >= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else
		{
			OutputDebugString(L"CW에서 있을수 없는 경우 발생...");
		}
	}
	else
	{
		if (X1 == X2 && Y1 == Y2)	// 모든 극점 포함
		{
			nNumP = 6;
			xl[2] = 0.0, yl[2] = dRadius;
			xl[3] = -dRadius, yl[3] = 0.0;
			xl[4] = 0.0, yl[4] = -dRadius;
			xl[5] = dRadius, yl[5] = 0.0;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s >= 0 && Y2s >= 0)	// CCW 1사분면
		{
			if ((X2s - X1s) <= 0 && (Y2s - Y1s) >= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0, yl[2] = dRadius;
				xl[3] = -dRadius, yl[3] = 0.0;
				xl[4] = 0.0, yl[4] = -dRadius;
				xl[5] = dRadius, yl[5] = 0.0;
			}
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 5;
			xl[2] = 0.0;		yl[2] = dRadius;
			xl[3] = -dRadius;	yl[3] = 0.0;
			xl[4] = 0.0;		yl[4] = -dRadius;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 3;
			xl[2] = 0.0;		yl[2] = dRadius;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 4;
			xl[2] = 0.0;		yl[2] = dRadius;
			xl[3] = -dRadius;	yl[3] = 0.0;
		}
		else if (X1s >= 0 && Y1s < 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 3;
			xl[2] = dRadius;	yl[2] = 0.0;
		}
		else if (X1s >= 0 && Y1s < 0 && X2s >= 0 && Y2s < 0)	// CCW 4사분면
		{
			if ((X2s - X1s) >= 0 && (Y2s - Y1s) >= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else if (X1s >= 0 && Y1s < 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 4;
			xl[2] = dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = dRadius;

		}
		else if (X1s >= 0 && Y1s < 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 5;
			xl[2] = dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = dRadius;
			xl[4] = -dRadius;	yl[4] = 0.0;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 5;
			xl[2] = -dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = -dRadius;
			xl[4] = dRadius;	yl[4] = 0.0;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 4;
			xl[2] = -dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = -dRadius;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s < 0 && Y2s >= 0)	// CCW 2사분면
		{
			if ((X2s - X1s) <= 0 && (Y2s - Y1s) <= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else if (X1s < 0 && Y1s >= 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 3;
			xl[2] = -dRadius;	yl[2] = 0.0;
		}
		else if (X1s < 0 && Y1s < 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 4;
			xl[2] = 0.0;		yl[2] = -dRadius;
			xl[3] = dRadius;	yl[3] = 0.0;

		}
		else if (X1s < 0 && Y1s < 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 3;
			xl[2] = 0.0;	yl[2] = -dRadius;
		}
		else if (X1s < 0 && Y1s < 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 5;
			xl[2] = 0.0;		yl[2] = -dRadius;
			xl[3] = dRadius;	yl[3] = 0.0;
			xl[4] = 0.0;		yl[4] = dRadius;
		}
		else if (X1s < 0 && Y1s < 0 && X2s < 0 && Y2s < 0)	// CCW 3사분면
		{
			if ((X2s - X1s) >= 0 && (Y2s - Y1s) <= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else
		{
			OutputDebugString(L"CCW 있을수 없는 경우 발생...");
		}
	}//end else

	xMIN = xl[0], xMAX = xl[0];
	yMIN = yl[0], yMAX = yl[0];

	for (int i = 0; i < nNumP; i++)
	{
		yMAX = max(yMAX, yl[i]);
		yMIN = min(yMIN, yl[i]);
		xMAX = max(xMAX, xl[i]);
		xMIN = min(xMIN, xl[i]);
	}

	tmpMinMax.left = xMIN + XC;
	tmpMinMax.right = xMAX + XC;
	tmpMinMax.top = yMAX + YC;
	tmpMinMax.bottom = yMIN + YC;

	if (m_pSymbol)
	{
		m_MinMaxRect.top = max(tmpMinMax.top + m_pSymbol->m_MinMaxRect.top, tmpMinMax.top + m_pSymbol->m_MinMaxRect.bottom);
		m_MinMaxRect.bottom = min(tmpMinMax.bottom + m_pSymbol->m_MinMaxRect.top, tmpMinMax.bottom + m_pSymbol->m_MinMaxRect.bottom);
		m_MinMaxRect.left = min(tmpMinMax.left + m_pSymbol->m_MinMaxRect.left, tmpMinMax.left + m_pSymbol->m_MinMaxRect.right);
		m_MinMaxRect.right = max(tmpMinMax.right + m_pSymbol->m_MinMaxRect.left, tmpMinMax.right + m_pSymbol->m_MinMaxRect.right);
	}
	else
	{
		m_MinMaxRect.top = max(tmpMinMax.top, m_MinMaxRect.top);
		m_MinMaxRect.bottom = min(tmpMinMax.bottom, m_MinMaxRect.bottom);
		m_MinMaxRect.left = min(tmpMinMax.left, m_MinMaxRect.left);
		m_MinMaxRect.right = max(tmpMinMax.right, m_MinMaxRect.right);
	}
}

void CFeatureA::LoadFeature(CString strOneLine, CStdioFile& cStdFile, CFeatureFile* pFeatureFile, CJobFile* pJobFile, bool bIsMMUnit)
{
	CString strTmpParams, strTmpAttributes;
	CString strTmp;
	int nTmp;

	// Param과 attribute 내용을 분리함.
	if ((nTmp = strOneLine.Find(';')) != -1)
	{
		strTmpParams = strOneLine.Left(nTmp);
		strTmpParams.TrimLeft(L"\t ");
		strTmpParams.TrimRight(L"\t ");
		strTmpAttributes = strOneLine.Right(strOneLine.GetLength() - nTmp - 1);
		strTmpAttributes.TrimLeft(L"\t ");
		strTmpAttributes.TrimRight(L"\t ");
	}
	else
	{
		strTmpParams = strOneLine;
		strTmpAttributes = "";
	}

	//Parameter를 축출함.
	if ((nTmp = strTmpParams.Find(' ')) != -1) // Feature 문자 제거
		strTmpParams.Delete(0, nTmp + 1);

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // xs 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		
		m_dXs = bIsMMUnit ? atof((CStringA)strTmp.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // ys 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		
		m_dYs = bIsMMUnit ? atof((CStringA)strTmp.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // xe 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
	
		m_dXe = bIsMMUnit ? atof((CStringA)strTmp.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // ye 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		
		m_dYe = bIsMMUnit ? atof((CStringA)strTmp.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // xc 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		
		m_dXc = bIsMMUnit ? atof((CStringA)strTmp.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // yc 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		
		m_dYc = bIsMMUnit ? atof((CStringA)strTmp.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // sym_num 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		m_nSymNum = atoi((CStringA)strTmp.GetBuffer(0));
		strTmpParams.Delete(0, nTmp + 1);
		for (int i = 0; i < pFeatureFile->m_arrSymbol.GetSize(); i++)
		{
			if (m_nSymNum == pFeatureFile->m_arrSymbol[i]->m_iSerialNum)
			{
				m_pSymbol = pFeatureFile->m_arrSymbol[i];
				break;
			}
		}
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // polarity 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		if (strTmp.GetAt(0) == 'P')
			m_bPolarity = true;
		else
			m_bPolarity = false;

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // dcode 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		m_nDecode = atoi((CStringA)strTmp.GetBuffer(0));
		strTmpParams.Delete(0, nTmp + 1);
	}

	if (strTmpParams.GetLength())
	{
		if (strTmpParams.GetAt(0) == 'Y')
			m_bCw = true;
		else
			m_bCw = false;
	}

	//Attribute 부분을 축출함.
	LoadFeatureAttribute(strTmpAttributes, pFeatureFile, pJobFile);  // Attribute parsing and loading
}

void CFeatureA::Serialize(CArchive& ar, CFeatureFile* pFeatureFile)
{
	CFeature::Serialize(ar, pFeatureFile);

	if (ar.IsStoring())
	{
		ar.Write(&m_bCw, sizeof(m_bCw));
		ar << m_nSymNum;
		ar << m_dXs;
		ar << m_dYs;
		ar << m_dXe;
		ar << m_dYe;
		ar << m_dXc;
		ar << m_dYc;

		ar << m_bHighlight;
		ar << m_bShow;
	}
	else
	{
		ar.Read(&m_bCw, sizeof(m_bCw));
		ar >> m_nSymNum;
		ar >> m_dXs;
		ar >> m_dYs;
		ar >> m_dXe;
		ar >> m_dYe;
		ar >> m_dXc;
		ar >> m_dYc;

		ar >> m_bHighlight;
		ar >> m_bShow;

		//해당 Feature에 연관된 Symbol 포인트를 지정함.
		for (UINT32 i = 0; i < pFeatureFile->m_arrSymbol.GetSize(); i++)
		{
			if (m_nSymNum == pFeatureFile->m_arrSymbol[i]->m_iSerialNum)
			{
				m_pSymbol = pFeatureFile->m_arrSymbol[i];
				break;
			}
		}
	}
}

void CFeatureT::SetMinMaxRect()
{
	double dTextLength = m_strText.GetLength() * m_dXSize;

	m_MinMaxRect.top = m_dY + m_dYSize;
	m_MinMaxRect.bottom = m_dY;
	m_MinMaxRect.left = m_dX;
	m_MinMaxRect.right = m_dX + dTextLength;
}

void CFeatureT::LoadFeature(CString strOneLine, CStdioFile& cStdFile, CFeatureFile* pFeatureFile, CJobFile* pJobFile, bool bIsMMUnit)
{
	CString strTmpParams, strTmpAttributes;
	CString strTmp;
	int nTmp;

	// Param과 attribute 내용을 분리함.
	if ((nTmp = strOneLine.Find(';')) != -1)
	{
		strTmpParams = strOneLine.Left(nTmp);
		strTmpParams.TrimLeft(L"\t ");
		strTmpParams.TrimRight(L"\t ");
		strTmpAttributes = strOneLine.Right(strOneLine.GetLength() - nTmp - 1);
		strTmpAttributes.TrimLeft(L"\t ");
		strTmpAttributes.TrimRight(L"\t ");
	}
	else
	{
		strTmpParams = strOneLine;
		strTmpAttributes = "";
	}

	//Parameter를 축출함.
	if ((nTmp = strTmpParams.Find(' ')) != -1) // Feature 문자 제거
		strTmpParams.Delete(0, nTmp + 1);

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // x 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		
		m_dX = bIsMMUnit ? atof((CStringA)strTmp.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // y 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		
		m_dY = bIsMMUnit ? atof((CStringA)strTmp.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // font 값 축출
	{
		m_strFont = strTmpParams.Left(nTmp);
		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // polarity 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		if (strTmp.GetAt(0) == 'P')
			m_bPolarity = true;
		else
			m_bPolarity = false;

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)    //orient값 축출 
	{
		strTmp = strTmpParams.Left(nTmp);
		switch (atoi((CStringA)strTmp.GetBuffer(0)))
		{
		case static_cast<UINT8>(Orient::NoMir0Deg) :
			m_eOrient = Orient::NoMir0Deg;
			break;
		case static_cast<UINT8>(Orient::NoMir90Deg) :
			m_eOrient = Orient::NoMir90Deg;
			break;
		case static_cast<UINT8>(Orient::NoMir180Deg) :
			m_eOrient = Orient::NoMir180Deg;
			break;
		case static_cast<UINT8>(Orient::NoMir270Deg) :
			m_eOrient = Orient::NoMir270Deg;
			break;
		case static_cast<UINT8>(Orient::Mir0Deg) :
			m_eOrient = Orient::Mir0Deg;
			break;
		case static_cast<UINT8>(Orient::Mir90Deg) :
			m_eOrient = Orient::Mir90Deg;
			break;
		case static_cast<UINT8>(Orient::Mir180Deg) :
			m_eOrient = Orient::Mir180Deg;
			break;
		case static_cast<UINT8>(Orient::Mir270Deg) :
			m_eOrient = Orient::Mir270Deg;
			break;
		case static_cast<UINT8>(Orient::NoMirAnyDeg) :
			m_eOrient = Orient::NoMirAnyDeg;
			break;
		case static_cast<UINT8>(Orient::MirAnyDeg) :
			m_eOrient = Orient::MirAnyDeg;
			break;
		default:
			m_eOrient = Orient::OrientNone;
		}

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // xsize 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		
		m_dXSize = bIsMMUnit ? atof((CStringA)strTmp.GetBuffer(0)) : 
			CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // ysize 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		
		m_dYSize = bIsMMUnit ? atof((CStringA)strTmp.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // width factor 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		m_dWidthFactor = atof((CStringA)strTmp.GetBuffer(0));
		strTmpParams.Delete(0, nTmp + 2);
	}

	if ((nTmp = strTmpParams.Find(L"'")) != -1)  //text값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		m_strText = strTmp.Mid(0, strTmp.GetLength());
		strTmpParams.Delete(0, nTmp + 2);
	}

	if (strTmpParams.GetLength())  // version 값 축출
	{
		m_nVersion = atoi((CStringA)strTmpParams.GetBuffer(0));
	}

	//Attribute 부분을 축출함.
	LoadFeatureAttribute(strTmpAttributes, pFeatureFile, pJobFile);  // Attribute parsing and loading
}

void CFeatureT::Serialize(CArchive& ar, CFeatureFile* pFeatureFile)
{
	CFeature::Serialize(ar, pFeatureFile);

	if (ar.IsStoring())
	{
		ar << m_nVersion;
		ar.Write(&m_eOrient, sizeof(m_eOrient));
		ar << m_dX;
		ar << m_dY;
		ar << m_dXSize;
		ar << m_dYSize;
		ar << m_dWidthFactor;
		ar << m_strFont;		
		ar << m_strText;

		ar << m_bHighlight;
		ar << m_bShow;
	}
	else
	{
		ar >> m_nVersion;
		ar.Read(&m_eOrient, sizeof(m_eOrient));
		ar >> m_dX;
		ar >> m_dY;
		ar >> m_dXSize;
		ar >> m_dYSize;
		ar >> m_dWidthFactor;
		ar >> m_strFont;		
		ar >> m_strText;	

		ar >> m_bHighlight;
		ar >> m_bShow;
	}
}

void CFeatureB::SetMinMaxRect()
{
	m_MinMaxRect.top = m_dY + m_dH;
	m_MinMaxRect.bottom = m_dY;
	m_MinMaxRect.left = m_dX;
	m_MinMaxRect.right = m_dX + m_dW;

	switch (m_eOrient)
	{
	case Orient::NoMir90Deg:
	case Orient::Mir90Deg:
		m_MinMaxRect.top = m_dY;
		m_MinMaxRect.bottom = m_dY - m_dW;
		m_MinMaxRect.left = m_dX;
		m_MinMaxRect.right = m_dX + m_dH;
		break;
	case Orient::NoMir180Deg:
	case Orient::Mir180Deg:
		m_MinMaxRect.top = m_dY;
		m_MinMaxRect.bottom = m_dY - m_dH;
		m_MinMaxRect.left = m_dX - m_dW;
		m_MinMaxRect.right = m_dX;
		break;
	case Orient::NoMir270Deg:
	case Orient::Mir270Deg:
		m_MinMaxRect.top = m_dY + m_dW;
		m_MinMaxRect.bottom = m_dY;
		m_MinMaxRect.left = m_dX - m_dH;
		m_MinMaxRect.right = m_dX;
		break;
	case Orient::NoMirAnyDeg:
	case Orient::MirAnyDeg:
		// m_dX, m_dY 기준 any angle 회전 width=m_dW, height=m_dH

		break;
	}
}

void CFeatureB::LoadFeature(CString strOneLine, CStdioFile& cStdFile, CFeatureFile* pFeatureFile, CJobFile* pJobFile, bool bIsMMUnit)
{
	CString strTmpParams, strTmpAttributes;
	CString strTmp;
	int nTmp;

	// Param과 attribute 내용을 분리함.
	if ((nTmp = strOneLine.Find(';')) != -1)
	{
		strTmpParams = strOneLine.Left(nTmp);
		strTmpParams.TrimLeft(L"\t ");
		strTmpParams.TrimRight(L"\t ");
		strTmpAttributes = strOneLine.Right(strOneLine.GetLength() - nTmp - 1);
		strTmpAttributes.TrimLeft(L"\t ");
		strTmpAttributes.TrimRight(L"\t ");
	}
	else
	{
		strTmpParams = strOneLine;
		strTmpAttributes = "";
	}

	//Parameter를 축출함.
	if ((nTmp = strTmpParams.Find(' ')) != -1) // Feature 문자 제거
		strTmpParams.Delete(0, nTmp + 1);

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // x 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		
		m_dX = bIsMMUnit ? atof((CStringA)strTmp.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // y 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		
		m_dY = bIsMMUnit ? atof((CStringA)strTmp.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // barcode 값 축출
	{
		m_strBarcode = strTmpParams.Left(nTmp);
		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // font 값 축출
	{
		m_strFont = strTmpParams.Left(nTmp);
		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // polarity 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		if (strTmp.GetAt(0) == 'P')
			m_bPolarity = true;
		else
			m_bPolarity = false;

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)    //orient값 축출 
	{
		strTmp = strTmpParams.Left(nTmp);
		switch (atoi((CStringA)strTmp.GetBuffer(0)))
		{
		case static_cast<UINT8>(Orient::NoMir0Deg) :
			m_eOrient = Orient::NoMir0Deg;
			break;
		case static_cast<UINT8>(Orient::NoMir90Deg) :
			m_eOrient = Orient::NoMir90Deg;
			break;
		case static_cast<UINT8>(Orient::NoMir180Deg) :
			m_eOrient = Orient::NoMir180Deg;
			break;
		case static_cast<UINT8>(Orient::NoMir270Deg) :
			m_eOrient = Orient::NoMir270Deg;
			break;
		case static_cast<UINT8>(Orient::Mir0Deg) :
			m_eOrient = Orient::Mir0Deg;
			break;
		case static_cast<UINT8>(Orient::Mir90Deg) :
			m_eOrient = Orient::Mir90Deg;
			break;
		case static_cast<UINT8>(Orient::Mir180Deg) :
			m_eOrient = Orient::Mir180Deg;
			break;
		case static_cast<UINT8>(Orient::Mir270Deg) :
			m_eOrient = Orient::Mir270Deg;
			break;
		case static_cast<UINT8>(Orient::NoMirAnyDeg) :
			m_eOrient = Orient::NoMirAnyDeg;
			break;
		case static_cast<UINT8>(Orient::MirAnyDeg) :
			m_eOrient = Orient::MirAnyDeg;
			break;
		default:
			m_eOrient = Orient::OrientNone;
		}

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // E값 축출
		strTmpParams.Delete(0, nTmp + 1);

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // w 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		
		m_dW = bIsMMUnit ? atof((CStringA)strTmp.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // h 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		
		m_dH = bIsMMUnit ? atof((CStringA)strTmp.GetBuffer(0)) :
			CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // fasc 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		if (strTmp.GetAt(0) == 'Y')
			m_bFasc = true;
		else
			m_bFasc = false;

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // cs 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		if (strTmp.GetAt(0) == 'Y')
			m_bCs = true;
		else
			m_bCs = false;

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // bg 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		if (strTmp.GetAt(0) == 'Y')
			m_bBg = true;
		else
			m_bBg = false;

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // astr 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		if (strTmp.GetAt(0) == 'Y')
			m_bAstr = true;
		else
			m_bAstr = false;

		strTmpParams.Delete(0, nTmp + 1);
	}

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // astr_pos 값 축출
	{
		strTmp = strTmpParams.Left(nTmp);
		if (strTmp.GetAt(0) == 'T')
			m_bAstrPos = true;
		else
			m_bAstrPos = false;

		strTmpParams.Delete(0, nTmp + 1);
	}

	if (strTmpParams.GetLength())  // Text 값 축출
	{
		m_strText = strTmpParams;
	}

	//Attribute 부분을 축출함.
	LoadFeatureAttribute(strTmpAttributes, pFeatureFile, pJobFile);  // Attribute parsing and loading
}

void CFeatureB::Serialize(CArchive& ar, CFeatureFile* pFeatureFile)
{
	CFeature::Serialize(ar, pFeatureFile);

	if (ar.IsStoring())
	{
		ar.Write(&m_bFasc, sizeof(m_bFasc));
		ar.Write(&m_bCs, sizeof(m_bCs));
		ar.Write(&m_bBg, sizeof(m_bBg));
		ar.Write(&m_bAstr, sizeof(m_bAstr));
		ar.Write(&m_bAstrPos, sizeof(m_bAstrPos));
		ar.Write(&m_eOrient, sizeof(m_eOrient));
		ar << m_dX;
		ar << m_dY;
		ar << m_dW;
		ar << m_dH;
		ar << m_strBarcode; // 항시 "UPC39" 문자를 가짐.		
		ar << m_strFont;
		ar << m_strText;		
	}
	else
	{
		ar.Read(&m_bFasc, sizeof(m_bFasc));
		ar.Read(&m_bCs, sizeof(m_bCs));
		ar.Read(&m_bBg, sizeof(m_bBg));
		ar.Read(&m_bAstr, sizeof(m_bAstr));
		ar.Read(&m_bAstrPos, sizeof(m_bAstrPos));
		ar.Read(&m_eOrient, sizeof(m_eOrient));
		ar >> m_dX;
		ar >> m_dY;
		ar >> m_dW;
		ar >> m_dH;
		ar >> m_strBarcode; // 항시 "UPC39" 문자를 가짐.
		ar >> m_strFont;		
		ar >> m_strText;		
	}
}

void COsOc::Serialize(CArchive& ar)
{
	CObject::Serialize(ar); 

	if (ar.IsStoring())
	{
		ar.Write(&m_bOsOc, sizeof(m_bOsOc));
		ar << m_dX;
		ar << m_dY;
	}
	else
	{
		ar.Read(&m_bOsOc, sizeof(m_bOsOc));
		ar >> m_dX;
		ar >> m_dY;
	}
}

void COs::Serialize(CArchive& ar)
{
	COsOc::Serialize(ar); 

	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

void COc::Serialize(CArchive& ar)
{
	COsOc::Serialize(ar);

	if (ar.IsStoring())
	{
		ar.Write(&m_bCw, sizeof(m_bCw));
		ar << m_dXc;
		ar << m_dYc;
	}
	else
	{
		ar.Read(&m_bCw, sizeof(m_bCw));
		ar >> m_dXc;
		ar >> m_dYc;		
	}
}

void CObOe::ClearOsOc()
{
	for (UINT32 i = 0; i < m_arrOsOc.GetSize(); i++)
	{
		if (m_arrOsOc[i] != nullptr)
		{
			delete m_arrOsOc[i];
		}
	}

	m_arrOsOc.RemoveAll();

	//hj.kim
	if (m_pGeoObject != nullptr)
	{
		m_pGeoObject->Release();
		m_pGeoObject = nullptr;
	}
}

void CObOe::LoadOsOc(CString strOsOcLine, bool bIsMMUnit)
{
	CString strTmp2 = L"";
	int nTmp = 0;

	COsOc* pTmpOsOc = nullptr;
	m_bAllOs = true;

	if (strOsOcLine.Find(L"OS") != -1)
	{
		pTmpOsOc = new COs;
		pTmpOsOc->m_bOsOc = true;

		if ((nTmp = strOsOcLine.Find(' ')) != -1) // OS 문자 제거
			strOsOcLine.Delete(0, nTmp + 1);

		if ((nTmp = strOsOcLine.Find(' ')) != -1)  // x 값 축출
		{
			strTmp2 = strOsOcLine.Left(nTmp);
			
			pTmpOsOc->m_dX = bIsMMUnit ? atof((CStringA)strTmp2.GetBuffer(0)) : 
				CJobFile::InchToMM(atof((CStringA)strTmp2.GetBuffer(0)));

			strOsOcLine.Delete(0, nTmp + 1);
		}
		if (strOsOcLine.GetLength())  // y 값 축출
		{
			pTmpOsOc->m_dY = bIsMMUnit ? atof((CStringA)strOsOcLine.GetBuffer(0)) :
				CJobFile::InchToMM(atof((CStringA)strOsOcLine.GetBuffer(0)));
		}

		m_arrOsOc.Add(pTmpOsOc);

	}
	else if (strOsOcLine.Find(L"OC") != -1)
	{
		pTmpOsOc = new COc;
		pTmpOsOc->m_bOsOc = false;

		if ((nTmp = strOsOcLine.Find(' ')) != -1) // OC 문자 제거
			strOsOcLine.Delete(0, nTmp + 1);

		if ((nTmp = strOsOcLine.Find(' ')) != -1)  // XE 값 축출
		{
			strTmp2 = strOsOcLine.Left(nTmp);
			
			pTmpOsOc->m_dX = bIsMMUnit ? atof((CStringA)strTmp2.GetBuffer(0)) :
				CJobFile::InchToMM(atof((CStringA)strTmp2.GetBuffer(0)));

			strOsOcLine.Delete(0, nTmp + 1);
		}

		if ((nTmp = strOsOcLine.Find(' ')) != -1)  // YE 값 축출
		{
			strTmp2 = strOsOcLine.Left(nTmp);
			
			pTmpOsOc->m_dY = bIsMMUnit ? atof((CStringA)strTmp2.GetBuffer(0)) :
				CJobFile::InchToMM(atof((CStringA)strTmp2.GetBuffer(0)));

			strOsOcLine.Delete(0, nTmp + 1);
		}

		if ((nTmp = strOsOcLine.Find(' ')) != -1)  // XC 값 축출
		{
			strTmp2 = strOsOcLine.Left(nTmp);
			
			((COc*)pTmpOsOc)->m_dXc = bIsMMUnit ? atof((CStringA)strTmp2.GetBuffer(0)) :
				CJobFile::InchToMM(atof((CStringA)strTmp2.GetBuffer(0)));

			strOsOcLine.Delete(0, nTmp + 1);
		}

		if ((nTmp = strOsOcLine.Find(' ')) != -1)  // YC 값 축출
		{
			strTmp2 = strOsOcLine.Left(nTmp);
			
			((COc*)pTmpOsOc)->m_dYc = bIsMMUnit ? atof((CStringA)strTmp2.GetBuffer(0)) :
				CJobFile::InchToMM(atof((CStringA)strTmp2.GetBuffer(0)));

			strOsOcLine.Delete(0, nTmp + 1);
		}

		if (strOsOcLine.GetLength())  // CW 값 축출
		{
			if (strOsOcLine.GetAt(0) == 'Y')
				((COc*)pTmpOsOc)->m_bCw = true;
			else
				((COc*)pTmpOsOc)->m_bCw = false;
		}
		m_arrOsOc.Add(pTmpOsOc);

		m_bAllOs = false; // Oc exist
	}

	SetMinMaxRect();
}

void	CObOe::SetMinMaxRect()
{
	RECTD tmpMinMax;
	double dXMin, dXMax, dYMin, dYMax, dRadius;
	double nXbs, nYbs, nX, nY, nXe, nYe, nXc, nYc;

	m_MinMaxRect.top = m_MinMaxRect.right = -1000000;
	m_MinMaxRect.bottom = m_MinMaxRect.left = 1000000;

	nXbs = m_dXbs;
	nYbs = m_dYbs;

	dYMax = dYMin = nYbs;
	dXMax = dXMin = nXbs;

	for (int j = 0; j < m_arrOsOc.GetSize(); j++)
	{
		if (m_arrOsOc[j]->m_bOsOc)		// OS 처리
		{
			nX = m_arrOsOc[j]->m_dX;
			nY = m_arrOsOc[j]->m_dY;

			dYMax = max(dYMax, nY);
			dYMin = min(dYMin, nY);
			dXMax = max(dXMax, nX);
			dXMin = min(dXMin, nX);
		}
		else                                       // OC 처리
		{
			dRadius = sqrt((((COc*)m_arrOsOc[j])->m_dX - ((COc*)m_arrOsOc[j])->m_dXc) * (((COc*)m_arrOsOc[j])->m_dX - ((COc*)m_arrOsOc[j])->m_dXc) +
				(((COc*)m_arrOsOc[j])->m_dY - ((COc*)m_arrOsOc[j])->m_dYc) * (((COc*)m_arrOsOc[j])->m_dY - ((COc*)m_arrOsOc[j])->m_dYc));

			// 시작점
			if (0 == j)
			{
				nX = nXbs;
				nY = nYbs;
			}
			else
			{
				nX = m_arrOsOc[j - 1]->m_dX;
				nY = m_arrOsOc[j - 1]->m_dY;
			}

			// 끝점
			nXe = m_arrOsOc[j]->m_dX;
			nYe = m_arrOsOc[j]->m_dY;

			// 중심점
			nXc = ((COc*)m_arrOsOc[j])->m_dXc;
			nYc = ((COc*)m_arrOsOc[j])->m_dYc;

			bool bTmpCw = ((COc*)m_arrOsOc[j])->m_bCw;
			SetMinMaxOsOc(nX, nY, nXe, nYe, nXc, nYc, bTmpCw, tmpMinMax);

			dYMax = max(dYMax, tmpMinMax.top);
			dYMin = min(dYMin, tmpMinMax.bottom);
			dXMax = max(dXMax, tmpMinMax.right);
			dXMin = min(dXMin, tmpMinMax.left);
		}
	} //for m_arrOsOc

	m_MinMaxRect.top = max(m_MinMaxRect.top, dYMax);
	m_MinMaxRect.bottom = min(m_MinMaxRect.bottom, dYMin);
	m_MinMaxRect.left = min(m_MinMaxRect.left, dXMin);
	m_MinMaxRect.right = max(m_MinMaxRect.right, dXMax);

}


void CObOe::SetMinMaxOsOc(double X1, double Y1, double X2, double Y2, double XC, double YC, bool cw, RECTD& tmpMinMax)
{
	double X1s, X2s, Y1s, Y2s, xl[6] = { 0 }, yl[6] = { 0 };
	double dRadius = 0, xMIN, yMIN, xMAX, yMAX;
	int	   nNumP = 0;				// 극점 후보 수 

	X1s = X1 - XC; X2s = X2 - XC;
	Y1s = Y1 - YC; Y2s = Y2 - YC;
	dRadius = sqrt((XC - X1) * (XC - X1) + (YC - Y1) * (YC - Y1));

	xl[0] = X1s, yl[0] = Y1s;	// start
	xl[1] = X2s, yl[1] = Y2s;	// end
	nNumP = 2;

	if (X1 < X2) { tmpMinMax.left = X1;	tmpMinMax.right = X2; }
	else { tmpMinMax.left = X2; tmpMinMax.right = X1; }

	if (Y1 < Y2) { tmpMinMax.bottom = Y1; tmpMinMax.top = Y2; }
	else { tmpMinMax.bottom = Y2; tmpMinMax.top = Y1; }

	if (cw)
	{
		if (X1 == X2 && Y1 == Y2) //같으면 모든 극점포함
		{
			nNumP = 6;
			xl[2] = 0.0;		yl[2] = dRadius;
			xl[3] = -dRadius;	yl[3] = 0.0;
			xl[4] = 0.0;		yl[4] = -dRadius;
			xl[5] = dRadius;	yl[5] = 0.0;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s >= 0 && Y2s >= 0)	// CW 1사분면
		{
			if ((X2s - X1s) >= 0 && (Y2s - Y1s) <= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 2 + 1;
			xl[2] = dRadius;	yl[2] = 0.0;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 5;
			xl[2] = dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = -dRadius;
			xl[4] = -dRadius;	yl[4] = 0.0;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 4;
			xl[2] = dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = -dRadius;
		}
		else if (X1s >= 0 && Y1s < 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 5;
			xl[2] = 0.0;		yl[2] = -dRadius;
			xl[3] = -dRadius;	yl[3] = 0.0;
			xl[4] = 0.0;		yl[4] = dRadius;
		}
		else if (X1s >= 0 && Y1s < 0 && X2s >= 0 && Y2s < 0) // CW 4사분면
		{
			if ((X2s - X1s) <= 0 && (Y2s - Y1s) <= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else if (X1s >= 0 && Y1s < 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 4;
			xl[2] = 0.0;		yl[2] = -dRadius;
			xl[3] = -dRadius;	yl[3] = 0.0;
		}
		else if (X1s >= 0 && Y1s < 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 3;
			xl[2] = 0.0;	yl[2] = -dRadius;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 3;
			xl[2] = 0.0;	yl[2] = dRadius;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 4;
			xl[2] = 0.0;		yl[2] = dRadius;
			xl[3] = dRadius;	yl[3] = 0.0;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s < 0 && Y2s >= 0)	// CW 2사분면
		{
			if ((X2s - X1s) >= 0 && (Y2s - Y1s) >= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else if (X1s < 0 && Y1s >= 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 5;
			xl[2] = 0.0;		yl[2] = dRadius;
			xl[3] = dRadius;	yl[3] = 0.0;
			xl[4] = 0.0;		yl[4] = -dRadius;
		}
		else if (X1s < 0 && Y1s < 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 4;
			xl[2] = -dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = dRadius;
		}
		else if (X1s < 0 && Y1s < 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 5;
			xl[2] = -dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = dRadius;
			xl[4] = dRadius;	yl[4] = 0.0;
		}
		else if (X1s < 0 && Y1s < 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 3;
			xl[2] = -dRadius;	yl[2] = 0.0;
		}
		else if (X1s < 0 && Y1s < 0 && X2s < 0 && Y2s < 0) // CW 3사분면
		{
			if ((X2s - X1s) <= 0 && (Y2s - Y1s) >= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else
		{
			OutputDebugString(L"CW에서 있을수 없는 경우 발생...");
		}
	}
	else
	{
		if (X1 == X2 && Y1 == Y2)	// 모든 극점 포함
		{
			nNumP = 6;
			xl[2] = 0.0, yl[2] = dRadius;
			xl[3] = -dRadius, yl[3] = 0.0;
			xl[4] = 0.0, yl[4] = -dRadius;
			xl[5] = dRadius, yl[5] = 0.0;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s >= 0 && Y2s >= 0)	// CCW 1사분면
		{
			if ((X2s - X1s) <= 0 && (Y2s - Y1s) >= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0, yl[2] = dRadius;
				xl[3] = -dRadius, yl[3] = 0.0;
				xl[4] = 0.0, yl[4] = -dRadius;
				xl[5] = dRadius, yl[5] = 0.0;
			}
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 5;
			xl[2] = 0.0;		yl[2] = dRadius;
			xl[3] = -dRadius;	yl[3] = 0.0;
			xl[4] = 0.0;		yl[4] = -dRadius;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 3;
			xl[2] = 0.0;		yl[2] = dRadius;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 4;
			xl[2] = 0.0;		yl[2] = dRadius;
			xl[3] = -dRadius;	yl[3] = 0.0;
		}
		else if (X1s >= 0 && Y1s < 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 3;
			xl[2] = dRadius;	yl[2] = 0.0;
		}
		else if (X1s >= 0 && Y1s < 0 && X2s >= 0 && Y2s < 0)	// CCW 4사분면
		{
			if ((X2s - X1s) >= 0 && (Y2s - Y1s) >= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else if (X1s >= 0 && Y1s < 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 4;
			xl[2] = dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = dRadius;

		}
		else if (X1s >= 0 && Y1s < 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 5;
			xl[2] = dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = dRadius;
			xl[4] = -dRadius;	yl[4] = 0.0;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 5;
			xl[2] = -dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = -dRadius;
			xl[4] = dRadius;	yl[4] = 0.0;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 4;
			xl[2] = -dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = -dRadius;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s < 0 && Y2s >= 0)	// CCW 2사분면
		{
			if ((X2s - X1s) <= 0 && (Y2s - Y1s) <= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else if (X1s < 0 && Y1s >= 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 3;
			xl[2] = -dRadius;	yl[2] = 0.0;
		}
		else if (X1s < 0 && Y1s < 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 4;
			xl[2] = 0.0;		yl[2] = -dRadius;
			xl[3] = dRadius;	yl[3] = 0.0;

		}
		else if (X1s < 0 && Y1s < 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 3;
			xl[2] = 0.0;	yl[2] = -dRadius;
		}
		else if (X1s < 0 && Y1s < 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 5;
			xl[2] = 0.0;		yl[2] = -dRadius;
			xl[3] = dRadius;	yl[3] = 0.0;
			xl[4] = 0.0;		yl[4] = dRadius;
		}
		else if (X1s < 0 && Y1s < 0 && X2s < 0 && Y2s < 0)	// CCW 3사분면
		{
			if ((X2s - X1s) >= 0 && (Y2s - Y1s) <= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else
		{
			OutputDebugString(L"CCW 있을수 없는 경우 발생...");
		}
	}//end else

	xMIN = xl[0], xMAX = xl[0];
	yMIN = yl[0], yMAX = yl[0];

	for (int i = 0; i < nNumP; i++)
	{
		yMAX = max(yMAX, yl[i]);
		yMIN = min(yMIN, yl[i]);
		xMAX = max(xMAX, xl[i]);
		xMIN = min(xMIN, xl[i]);
	}

	tmpMinMax.left = xMIN + XC;
	tmpMinMax.right = xMAX + XC;
	tmpMinMax.top = yMAX + YC;
	tmpMinMax.bottom = yMIN + YC;
}

void CObOe::Serialize(CArchive& ar)
{
	CObject::Serialize(ar); 

	if (ar.IsStoring())
	{
		ar.Write(&m_bAllOs, sizeof(m_bAllOs));
		ar.Write(&m_bPolyType, sizeof(m_bPolyType));
		ar << m_dXbs;
		ar << m_dYbs;
		ar.Write(&m_MinMaxRect, sizeof(m_MinMaxRect));		

		ar << m_bHighlight;
		ar << m_bShow;

		ar << m_arrOsOc.GetSize();
		for (UINT32 i = 0; i < m_arrOsOc.GetSize(); i++)
		{
			COsOc*  pCOsOcTemp = m_arrOsOc.GetAt(i);
			ar.Write(&(pCOsOcTemp->m_bOsOc), sizeof(pCOsOcTemp->m_bOsOc));
			pCOsOcTemp->Serialize(ar);
		}
	}
	else
	{
		ar.Read(&m_bAllOs, sizeof(m_bAllOs));
		ar.Read(&m_bPolyType, sizeof(m_bPolyType));		
		ar >> m_dXbs;
		ar >> m_dYbs;
		ar.Read(&m_MinMaxRect, sizeof(m_MinMaxRect));

		ar >> m_bHighlight;
		ar >> m_bShow;

		bool bTemp = false;
		UINT32 nCount = 0;

		ar >> nCount;
		for (UINT32 i = 0; i < nCount; i++)
		{
			ar.Read(&bTemp, sizeof(bTemp));
			COsOc* pCOsOcTemp = nullptr;

			if (bTemp)
				pCOsOcTemp = new COs;
			else
				pCOsOcTemp = new COc;
			pCOsOcTemp->Serialize(ar);
			m_arrOsOc.Add(pCOsOcTemp);
		}
	}
}

void CFeatureS::ClearFeature()
{
	for (UINT32 i = 0; i < m_arrObOe.GetSize(); i++)
	{ 
		if (m_arrObOe[i] != nullptr)
		{
			m_arrObOe[i]->ClearOsOc();
			delete m_arrObOe[i];
		}
	}

	m_arrObOe.RemoveAll();

	//hj.kim
	if (m_pGeoObject != nullptr)
	{
		m_pGeoObject->Release();
		m_pGeoObject = nullptr;
	}
}

void CFeatureS::SetMinMaxRect()
{
	RECTD tmpMinMax;
	double dXMin, dXMax, dYMin, dYMax, dRadius;
	double nXbs, nYbs, nX, nY, nXe, nYe, nXc, nYc;

	m_MinMaxRect.top = m_MinMaxRect.right = -1000000;
	m_MinMaxRect.bottom = m_MinMaxRect.left = 1000000;

	for (int i = 0; i < m_arrObOe.GetSize(); i++)
	{
		CObOe*  pObOeTmp = m_arrObOe.GetAt(i);
		if (pObOeTmp == nullptr)
			continue;

		nXbs = pObOeTmp->m_dXbs;
		nYbs = pObOeTmp->m_dYbs;

		dYMax = dYMin = nYbs;
		dXMax = dXMin = nXbs;

		for (int j = 0; j < pObOeTmp->m_arrOsOc.GetSize(); j++)
		{
			if (pObOeTmp->m_arrOsOc[j]->m_bOsOc)		// OS 처리
			{
				nX = pObOeTmp->m_arrOsOc[j]->m_dX;
				nY = pObOeTmp->m_arrOsOc[j]->m_dY;

				dYMax = max(dYMax, nY);
				dYMin = min(dYMin, nY);
				dXMax = max(dXMax, nX);
				dXMin = min(dXMin, nX);
			}
			else                                       // OC 처리
			{
				dRadius = sqrt((((COc*)pObOeTmp->m_arrOsOc[j])->m_dX - ((COc*)pObOeTmp->m_arrOsOc[j])->m_dXc) * (((COc*)pObOeTmp->m_arrOsOc[j])->m_dX - ((COc*)pObOeTmp->m_arrOsOc[j])->m_dXc) +
					(((COc*)pObOeTmp->m_arrOsOc[j])->m_dY - ((COc*)pObOeTmp->m_arrOsOc[j])->m_dYc) * (((COc*)pObOeTmp->m_arrOsOc[j])->m_dY - ((COc*)pObOeTmp->m_arrOsOc[j])->m_dYc));

				// 시작점
				if (0 == j)
				{
					nX = nXbs;
					nY = nYbs;
				}
				else
				{
					nX = pObOeTmp->m_arrOsOc[j - 1]->m_dX;
					nY = pObOeTmp->m_arrOsOc[j - 1]->m_dY;
				}

				// 끝점
				nXe = pObOeTmp->m_arrOsOc[j]->m_dX;
				nYe = pObOeTmp->m_arrOsOc[j]->m_dY;

				// 중심점
				nXc = ((COc*)pObOeTmp->m_arrOsOc[j])->m_dXc;
				nYc = ((COc*)pObOeTmp->m_arrOsOc[j])->m_dYc;

				bool bTmpCw = ((COc*)pObOeTmp->m_arrOsOc[j])->m_bCw;
				SetMinMaxOsOc(nX, nY, nXe, nYe, nXc, nYc, bTmpCw, tmpMinMax);

				dYMax = max(dYMax, tmpMinMax.top);
				dYMin = min(dYMin, tmpMinMax.bottom);
				dXMax = max(dXMax, tmpMinMax.right);
				dXMin = min(dXMin, tmpMinMax.left);
			}
		} //for m_arrOsOc
		m_arrObOe[i]->m_MinMaxRect.top = dYMax;
		m_arrObOe[i]->m_MinMaxRect.bottom = dYMin;
		m_arrObOe[i]->m_MinMaxRect.left = dXMin;
		m_arrObOe[i]->m_MinMaxRect.right = dXMax;

		m_MinMaxRect.top = max(m_MinMaxRect.top, dYMax);
		m_MinMaxRect.bottom = min(m_MinMaxRect.bottom, dYMin);
		m_MinMaxRect.left = min(m_MinMaxRect.left, dXMin);
		m_MinMaxRect.right = max(m_MinMaxRect.right, dXMax);
	}//for m_arrObOe
}

void CFeatureS::SetMinMaxOsOc(double X1, double Y1, double X2, double Y2, double XC, double YC, bool cw, RECTD& tmpMinMax)
{
	double X1s, X2s, Y1s, Y2s, xl[6] = { 0 }, yl[6] = { 0 };
	double dRadius = 0, xMIN, yMIN, xMAX, yMAX;
	int	   nNumP = 0;				// 극점 후보 수 

	X1s = X1 - XC; X2s = X2 - XC;
	Y1s = Y1 - YC; Y2s = Y2 - YC;
	dRadius = sqrt((XC - X1) * (XC - X1) + (YC - Y1) * (YC - Y1));

	xl[0] = X1s, yl[0] = Y1s;	// start
	xl[1] = X2s, yl[1] = Y2s;	// end
	nNumP = 2;

	if (X1 < X2) { tmpMinMax.left = X1;	tmpMinMax.right = X2; }
	else { tmpMinMax.left = X2; tmpMinMax.right = X1; }

	if (Y1 < Y2) { tmpMinMax.bottom = Y1; tmpMinMax.top = Y2; }
	else { tmpMinMax.bottom = Y2; tmpMinMax.top = Y1; }

	if (cw)
	{
		if (X1 == X2 && Y1 == Y2) //같으면 모든 극점포함
		{
			nNumP = 6;
			xl[2] = 0.0;		yl[2] = dRadius;
			xl[3] = -dRadius;	yl[3] = 0.0;
			xl[4] = 0.0;		yl[4] = -dRadius;
			xl[5] = dRadius;	yl[5] = 0.0;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s >= 0 && Y2s >= 0)	// CW 1사분면
		{
			if ((X2s - X1s) >= 0 && (Y2s - Y1s) <= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 2 + 1;
			xl[2] = dRadius;	yl[2] = 0.0;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 5;
			xl[2] = dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = -dRadius;
			xl[4] = -dRadius;	yl[4] = 0.0;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 4;
			xl[2] = dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = -dRadius;
		}
		else if (X1s >= 0 && Y1s < 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 5;
			xl[2] = 0.0;		yl[2] = -dRadius;
			xl[3] = -dRadius;	yl[3] = 0.0;
			xl[4] = 0.0;		yl[4] = dRadius;
		}
		else if (X1s >= 0 && Y1s < 0 && X2s >= 0 && Y2s < 0) // CW 4사분면
		{
			if ((X2s - X1s) <= 0 && (Y2s - Y1s) <= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else if (X1s >= 0 && Y1s < 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 4;
			xl[2] = 0.0;		yl[2] = -dRadius;
			xl[3] = -dRadius;	yl[3] = 0.0;
		}
		else if (X1s >= 0 && Y1s < 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 3;
			xl[2] = 0.0;	yl[2] = -dRadius;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 3;
			xl[2] = 0.0;	yl[2] = dRadius;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 4;
			xl[2] = 0.0;		yl[2] = dRadius;
			xl[3] = dRadius;	yl[3] = 0.0;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s < 0 && Y2s >= 0)	// CW 2사분면
		{
			if ((X2s - X1s) >= 0 && (Y2s - Y1s) >= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else if (X1s < 0 && Y1s >= 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 5;
			xl[2] = 0.0;		yl[2] = dRadius;
			xl[3] = dRadius;	yl[3] = 0.0;
			xl[4] = 0.0;		yl[4] = -dRadius;
		}
		else if (X1s < 0 && Y1s < 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 4;
			xl[2] = -dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = dRadius;
		}
		else if (X1s < 0 && Y1s < 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 5;
			xl[2] = -dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = dRadius;
			xl[4] = dRadius;	yl[4] = 0.0;
		}
		else if (X1s < 0 && Y1s < 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 3;
			xl[2] = -dRadius;	yl[2] = 0.0;
		}
		else if (X1s < 0 && Y1s < 0 && X2s < 0 && Y2s < 0) // CW 3사분면
		{
			if ((X2s - X1s) <= 0 && (Y2s - Y1s) >= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else
		{
			OutputDebugString(L"CW에서 있을수 없는 경우 발생...");
		}
	}
	else
	{
		if (X1 == X2 && Y1 == Y2)	// 모든 극점 포함
		{
			nNumP = 6;
			xl[2] = 0.0, yl[2] = dRadius;
			xl[3] = -dRadius, yl[3] = 0.0;
			xl[4] = 0.0, yl[4] = -dRadius;
			xl[5] = dRadius, yl[5] = 0.0;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s >= 0 && Y2s >= 0)	// CCW 1사분면
		{
			if ((X2s - X1s) <= 0 && (Y2s - Y1s) >= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0, yl[2] = dRadius;
				xl[3] = -dRadius, yl[3] = 0.0;
				xl[4] = 0.0, yl[4] = -dRadius;
				xl[5] = dRadius, yl[5] = 0.0;
			}
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 5;
			xl[2] = 0.0;		yl[2] = dRadius;
			xl[3] = -dRadius;	yl[3] = 0.0;
			xl[4] = 0.0;		yl[4] = -dRadius;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 3;
			xl[2] = 0.0;		yl[2] = dRadius;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 4;
			xl[2] = 0.0;		yl[2] = dRadius;
			xl[3] = -dRadius;	yl[3] = 0.0;
		}
		else if (X1s >= 0 && Y1s < 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 3;
			xl[2] = dRadius;	yl[2] = 0.0;
		}
		else if (X1s >= 0 && Y1s < 0 && X2s >= 0 && Y2s < 0)	// CCW 4사분면
		{
			if ((X2s - X1s) >= 0 && (Y2s - Y1s) >= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else if (X1s >= 0 && Y1s < 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 4;
			xl[2] = dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = dRadius;

		}
		else if (X1s >= 0 && Y1s < 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 5;
			xl[2] = dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = dRadius;
			xl[4] = -dRadius;	yl[4] = 0.0;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 5;
			xl[2] = -dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = -dRadius;
			xl[4] = dRadius;	yl[4] = 0.0;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 4;
			xl[2] = -dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = -dRadius;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s < 0 && Y2s >= 0)	// CCW 2사분면
		{
			if ((X2s - X1s) <= 0 && (Y2s - Y1s) <= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else if (X1s < 0 && Y1s >= 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 3;
			xl[2] = -dRadius;	yl[2] = 0.0;
		}
		else if (X1s < 0 && Y1s < 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 4;
			xl[2] = 0.0;		yl[2] = -dRadius;
			xl[3] = dRadius;	yl[3] = 0.0;

		}
		else if (X1s < 0 && Y1s < 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 3;
			xl[2] = 0.0;	yl[2] = -dRadius;
		}
		else if (X1s < 0 && Y1s < 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 5;
			xl[2] = 0.0;		yl[2] = -dRadius;
			xl[3] = dRadius;	yl[3] = 0.0;
			xl[4] = 0.0;		yl[4] = dRadius;
		}
		else if (X1s < 0 && Y1s < 0 && X2s < 0 && Y2s < 0)	// CCW 3사분면
		{
			if ((X2s - X1s) >= 0 && (Y2s - Y1s) <= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else
		{
			OutputDebugString(L"CCW 있을수 없는 경우 발생...");
		}
	}//end else

	xMIN = xl[0], xMAX = xl[0];
	yMIN = yl[0], yMAX = yl[0];

	for (int i = 0; i < nNumP; i++)
	{
		yMAX = max(yMAX, yl[i]);
		yMIN = min(yMIN, yl[i]);
		xMAX = max(xMAX, xl[i]);
		xMIN = min(xMIN, xl[i]);
	}

	tmpMinMax.left = xMIN + XC;
	tmpMinMax.right = xMAX + XC;
	tmpMinMax.top = yMAX + YC;
	tmpMinMax.bottom = yMIN + YC;
}

void CFeatureS::LoadFeature(CString strOneLine, CStdioFile& cStdFile, CFeatureFile* pFeatureFile, CJobFile* pJobFile, bool bIsMMUnit)
{
	int nTmp = 0;
	CString strTmpParams = L"", strTmpAttributes = L"";
	CString strTmp = L"", strTmp2 = L"";

	// Param과 attribute 내용을 분리함.
	if ((nTmp = strOneLine.Find(';')) != -1)
	{
		strTmpParams = strOneLine.Left(nTmp);
		strTmpParams.TrimLeft(L"\t ");
		strTmpParams.TrimRight(L"\t ");
		strTmpAttributes = strOneLine.Right(strOneLine.GetLength() - nTmp - 1);
		strTmpAttributes.TrimLeft(L"\t ");
		strTmpAttributes.TrimRight(L"\t ");
	}
	else
	{
		strTmpParams = strOneLine;
		strTmpAttributes = "";
	}

	//Parameter를 축출함.
	if ((nTmp = strTmpParams.Find(' ')) != -1) // Feature 문자 제거
		strTmpParams.Delete(0, nTmp + 1);

	if ((nTmp = strTmpParams.Find(' ')) != -1)  // polarity 값 축출
	{
		strTmp2 = strTmpParams.Left(nTmp);
		if (strTmp2.GetAt(0) == 'P')
			m_bPolarity = true;
		else
			m_bPolarity = false;

		strTmpParams.Delete(0, nTmp + 1);
	}

	if (strTmpParams.GetLength())  // dcode 값 축출
	{
		m_nDecode = atoi((CStringA)strTmpParams.GetBuffer(0));
	}

	//Attribute 부분을 축출함.
	LoadFeatureAttribute(strTmpAttributes, pFeatureFile, pJobFile);  // Attribute parsing and loading

	//S~SE 구간 처리부 
	while (cStdFile.ReadString(strTmp))
	{
		strTmp.TrimLeft(L"\t ");
		strTmp.TrimRight(L"\t ");
		strTmp.MakeUpper();
		if (strTmp.GetLength() == 0)  // 빈줄 Skip
			continue;

		if (!strTmp.CompareNoCase(L"SE"))
			break;

		if (strTmp.Find(L"OB") != -1)
		{
			CObOe* pTmpObOe = new CObOe;

			if ((nTmp = strTmp.Find(' ')) != -1) // OB 문자 제거
				strTmp.Delete(0, nTmp + 1);

			if ((nTmp = strTmp.Find(' ')) != -1)  // xbs 값 축출
			{
				strTmp2 = strTmp.Left(nTmp);
				
				pTmpObOe->m_dXbs = bIsMMUnit ? atof((CStringA)strTmp2.GetBuffer(0)) :
					CJobFile::InchToMM(atof((CStringA)strTmp2.GetBuffer(0)));

				strTmp.Delete(0, nTmp + 1);
			}

			if ((nTmp = strTmp.Find(' ')) != -1)  // ybs 값 축출
			{
				strTmp2 = strTmp.Left(nTmp);
				
				pTmpObOe->m_dYbs = bIsMMUnit ? atof((CStringA)strTmp2.GetBuffer(0)) :
					CJobFile::InchToMM(atof((CStringA)strTmp2.GetBuffer(0)));

				strTmp.Delete(0, nTmp + 1);
			}

			if (strTmp.GetLength())  // poly_type 값 축출
			{
				if (strTmp.GetAt(0) == 'I')
					pTmpObOe->m_bPolyType = true;
				else
					pTmpObOe->m_bPolyType = false;
			}

			while (cStdFile.ReadString(strTmp))
			{
				strTmp.TrimLeft(L"\t ");
				strTmp.TrimRight(L"\t ");
				strTmp.MakeUpper();
				if (strTmp.GetLength() == 0)  // 빈줄 Skip
					continue;
				if (!strTmp.CompareNoCase(L"OE"))
					break;

				pTmpObOe->LoadOsOc(strTmp, bIsMMUnit);  // Os, Oc 생성 (Load)
			}
			m_arrObOe.Add(pTmpObOe);
		}
	}
}

void CFeatureFile::ClearSymbols()
{
	for (UINT32 i = 0; i < m_arrSymbol.GetSize(); i++)
	{
		if (m_arrSymbol[i] != nullptr)
		{
			delete m_arrSymbol[i];
			m_arrSymbol[i] = nullptr;
		}
	}

	m_arrSymbol.RemoveAll();
}

void CFeatureFile::ClearFeatures()
{
	for (UINT32 i = 0; i < m_arrFeature.GetSize(); i++)
	{
//		m_arrFeature[i]->ClearFeature();
		delete m_arrFeature[i];
	}

	m_arrFeature.RemoveAll();
}

void CFeatureFile::ClearAttributes()
{
	for (UINT32 i = 0; i < m_arrAttribute.GetSize(); i++)
		delete m_arrAttribute[i];

	m_arrAttribute.RemoveAll();
}

void CFeatureFile::ClearAttributeTextStrings()
{
	for (UINT32 i = 0; i < m_arrAttributeTextString.GetSize(); i++)
		delete m_arrAttributeTextString[i];

	m_arrAttributeTextString.RemoveAll();
}

void CFeatureFile::ClearAll()
{
	ClearSymbols();
	ClearFeatures();
	ClearAttributes();
	ClearAttributeTextStrings();
};

void CFeatureFile::SetMinMaxRect()
{
	RECTD tmpMinMax;
	double dCenterX, dCenterY;

	if (m_arrFeature.GetSize())
		m_MinMaxRect = m_arrFeature[0]->m_MinMaxRect;

	for (UINT32 i = 0; i < m_arrFeature.GetSize(); i++)
	{
		tmpMinMax.top = m_arrFeature[i]->m_MinMaxRect.top;
		tmpMinMax.bottom = m_arrFeature[i]->m_MinMaxRect.bottom;
		tmpMinMax.left = m_arrFeature[i]->m_MinMaxRect.left;
		tmpMinMax.right = m_arrFeature[i]->m_MinMaxRect.right;

		switch (m_arrFeature[i]->m_eType)
		{
		case FeatureType::P:
			dCenterX = ((CFeatureP*)m_arrFeature[i])->m_dX;
			dCenterY = ((CFeatureP*)m_arrFeature[i])->m_dY;
			switch (((CFeatureP*)m_arrFeature[i])->m_eOrient)
			{
			case Orient::NoMir0Deg:
				break;
			case Orient::Mir0Deg:
				tmpMinMax.top = tmpMinMax.top;
				tmpMinMax.bottom = tmpMinMax.bottom;
				//2006.05.03 
				tmpMinMax.left = -(m_arrFeature[i]->m_MinMaxRect.right - dCenterX) + dCenterX;
				tmpMinMax.right = -(m_arrFeature[i]->m_MinMaxRect.left - dCenterX) + dCenterX;
				break;
			case Orient::NoMir90Deg:
				//2006.05.03 
				tmpMinMax.top = -(m_arrFeature[i]->m_MinMaxRect.left - dCenterX) + dCenterY;
				tmpMinMax.bottom = -(m_arrFeature[i]->m_MinMaxRect.right - dCenterX) + dCenterY;
				tmpMinMax.left = -(m_arrFeature[i]->m_MinMaxRect.top - dCenterY) + dCenterX;
				tmpMinMax.right = -(m_arrFeature[i]->m_MinMaxRect.bottom - dCenterY) + dCenterX;
				break;
			case Orient::Mir90Deg:
				tmpMinMax.top = dCenterY + (dCenterX - m_arrFeature[i]->m_MinMaxRect.left);
				tmpMinMax.bottom = dCenterY - (m_arrFeature[i]->m_MinMaxRect.right - dCenterX);
				tmpMinMax.left = dCenterX - (m_arrFeature[i]->m_MinMaxRect.top - dCenterY);
				tmpMinMax.right = dCenterX + (dCenterY - m_arrFeature[i]->m_MinMaxRect.bottom);
				break;
			case Orient::NoMir180Deg:
				tmpMinMax.top = dCenterY + (dCenterY - m_arrFeature[i]->m_MinMaxRect.bottom);
				tmpMinMax.bottom = dCenterY - (m_arrFeature[i]->m_MinMaxRect.top - dCenterY);
				tmpMinMax.left = dCenterX - (m_arrFeature[i]->m_MinMaxRect.right - dCenterX);
				tmpMinMax.right = dCenterX + (dCenterX - m_arrFeature[i]->m_MinMaxRect.left);
				break;
			case Orient::Mir180Deg:
				tmpMinMax.top = dCenterY + (dCenterY - m_arrFeature[i]->m_MinMaxRect.bottom);
				tmpMinMax.bottom = dCenterY - (m_arrFeature[i]->m_MinMaxRect.top - dCenterY);
				tmpMinMax.left = dCenterX - (dCenterX - m_arrFeature[i]->m_MinMaxRect.left);
				tmpMinMax.right = dCenterX + (m_arrFeature[i]->m_MinMaxRect.right - dCenterX);
				break;
			case Orient::NoMir270Deg:
				tmpMinMax.top = dCenterY + (m_arrFeature[i]->m_MinMaxRect.right - dCenterX);
				tmpMinMax.bottom = dCenterY - (dCenterX - m_arrFeature[i]->m_MinMaxRect.left);
				tmpMinMax.left = dCenterX - (m_arrFeature[i]->m_MinMaxRect.top - dCenterY);
				tmpMinMax.right = dCenterX + (dCenterY - m_arrFeature[i]->m_MinMaxRect.bottom);
				break;
			case Orient::Mir270Deg:
				tmpMinMax.top = dCenterY + (m_arrFeature[i]->m_MinMaxRect.right - dCenterX);
				tmpMinMax.bottom = dCenterY - (dCenterX - m_arrFeature[i]->m_MinMaxRect.left);
				tmpMinMax.left = dCenterX - (dCenterY - m_arrFeature[i]->m_MinMaxRect.bottom);
				tmpMinMax.right = dCenterX + (m_arrFeature[i]->m_MinMaxRect.top - dCenterY);
				break;
			}
			break;
		case FeatureType::T:
			dCenterX = ((CFeatureT*)m_arrFeature[i])->m_dX;
			dCenterY = ((CFeatureT*)m_arrFeature[i])->m_dY;
			switch (((CFeatureT*)m_arrFeature[i])->m_eOrient)
			{
			case Orient::NoMir0Deg:
				break;
			case Orient::Mir0Deg:
				tmpMinMax.top = tmpMinMax.top;
				tmpMinMax.bottom = tmpMinMax.bottom;
				tmpMinMax.left = dCenterX - (m_arrFeature[i]->m_MinMaxRect.right - dCenterX);
				tmpMinMax.right = dCenterX + (dCenterX - m_arrFeature[i]->m_MinMaxRect.left);
				break;
			case Orient::NoMir90Deg:
				tmpMinMax.top = dCenterY + (dCenterX - m_arrFeature[i]->m_MinMaxRect.left);
				tmpMinMax.bottom = dCenterY - (m_arrFeature[i]->m_MinMaxRect.right - dCenterX);
				tmpMinMax.left = dCenterX - (dCenterY - m_arrFeature[i]->m_MinMaxRect.bottom);
				tmpMinMax.right = dCenterX + (m_arrFeature[i]->m_MinMaxRect.top - dCenterY);
				break;
			case Orient::Mir90Deg:
				tmpMinMax.top = dCenterY + (dCenterX - m_arrFeature[i]->m_MinMaxRect.left);
				tmpMinMax.bottom = dCenterY - (m_arrFeature[i]->m_MinMaxRect.right - dCenterX);
				tmpMinMax.left = dCenterX - (m_arrFeature[i]->m_MinMaxRect.top - dCenterY);
				tmpMinMax.right = dCenterX + (dCenterY - m_arrFeature[i]->m_MinMaxRect.bottom);
				break;
			case Orient::NoMir180Deg:
				tmpMinMax.top = dCenterY + (dCenterY - m_arrFeature[i]->m_MinMaxRect.bottom);
				tmpMinMax.bottom = dCenterY - (m_arrFeature[i]->m_MinMaxRect.top - dCenterY);
				tmpMinMax.left = dCenterX - (m_arrFeature[i]->m_MinMaxRect.right - dCenterX);
				tmpMinMax.right = dCenterX + (dCenterX - m_arrFeature[i]->m_MinMaxRect.left);
				break;
			case Orient::Mir180Deg:
				tmpMinMax.top = dCenterY + (dCenterY - m_arrFeature[i]->m_MinMaxRect.bottom);
				tmpMinMax.bottom = dCenterY - (m_arrFeature[i]->m_MinMaxRect.top - dCenterY);
				tmpMinMax.left = dCenterX - (dCenterX - m_arrFeature[i]->m_MinMaxRect.left);
				tmpMinMax.right = dCenterX + (m_arrFeature[i]->m_MinMaxRect.right - dCenterX);
				break;
			case Orient::NoMir270Deg:
				tmpMinMax.top = dCenterY + (m_arrFeature[i]->m_MinMaxRect.right - dCenterX);
				tmpMinMax.bottom = dCenterY - (dCenterX - m_arrFeature[i]->m_MinMaxRect.left);
				tmpMinMax.left = dCenterX - (m_arrFeature[i]->m_MinMaxRect.top - dCenterY);
				tmpMinMax.right = dCenterX + (dCenterY - m_arrFeature[i]->m_MinMaxRect.bottom);
				break;
			case Orient::Mir270Deg:
				tmpMinMax.top = dCenterY + (m_arrFeature[i]->m_MinMaxRect.right - dCenterX);
				tmpMinMax.bottom = dCenterY - (dCenterX - m_arrFeature[i]->m_MinMaxRect.left);
				tmpMinMax.left = dCenterX - (dCenterY - m_arrFeature[i]->m_MinMaxRect.bottom);
				tmpMinMax.right = dCenterX + (m_arrFeature[i]->m_MinMaxRect.top - dCenterY);
				break;
			}
			break;
		default:
			break;
		}
		m_MinMaxRect.top = max(m_MinMaxRect.top, tmpMinMax.top);
		m_MinMaxRect.bottom = min(m_MinMaxRect.bottom, tmpMinMax.bottom);
		m_MinMaxRect.left = min(m_MinMaxRect.left, tmpMinMax.left);
		m_MinMaxRect.right = max(m_MinMaxRect.right, tmpMinMax.right);
	}
}

UINT32 CFeatureFile::LoadFeatureFile(CString strFeatureFile, CJobFile* pJobFile)
{
	CStdioFile cStdFile;
	CString strTmp, strTmp2, strTrans = L"";
	int nTmp, nParamIndex;
	wchar_t chTmp = 0;
	CFileFind filefinder;
	_TCHAR bom = (_TCHAR)0xFEFF;
	CSymbol* pSymbolTmp = NULL;
	CFeature* pFeatureTmp = NULL;
	CAttribute* pAttribute = NULL;
	CAttributeTextString* pAttributeTextString = NULL;
	bool bIsMMUnit = false;

	int nFNum = 0;

	if (!cStdFile.Open(strFeatureFile.GetBuffer(0), CFile::modeRead | CFile::typeText))
		return RESULT_GOOD;
	//	CArchive ar(&cStdFile, CArchive::load);

	while (cStdFile.ReadString(strTmp))
	{
		strTmp.TrimLeft(L"\t ");
		strTmp.TrimRight(L"\t ");
		if (strTmp.GetLength() == 0) continue;

		if (strTmp.Find(L"U MM") != -1)
		{
			bIsMMUnit = true;
			continue;
		}

		chTmp = strTmp.GetAt(0);
		if (chTmp == '#')	// 주석인 경우
		{
			continue;
		}
		else if (chTmp == '$')	//Symbol인 경우
		{
			strTmp.MakeUpper();

			nTmp = strTmp.Find(L" ");
			strTmp2 = strTmp.Mid(1, nTmp - 1);
			strTmp.Delete(0, nTmp + 1);

			pSymbolTmp = new CSymbol;
			pSymbolTmp->m_iSerialNum = atoi((CStringA)strTmp2.GetBuffer(0));  // 번호 입력 

			//			bExcept[pSymbolTmp->m_nSerialNum] = true;

			for (int i = 0; i < pJobFile->m_arrSymbol.GetSize(); i++)
			{
				nTmp = strTmp.CompareNoCase(pJobFile->m_arrSymbol[i]->m_strSymbolName.GetBuffer(0));
				if (!nTmp) break;
			}

			nTmp = strTmp.Find(L" M");
			if (nTmp == -1) strTmp2 = strTmp;
			else strTmp2 = strTmp.Left(nTmp);
			strTmp = strTmp2;

			int posi = strTmp.Find('_');
			if (posi != -1)
			{
				CString str = strTmp.Left(posi);
				if (str.FindOneOf(L"-+.1234567890") != -1)
				{
					nTmp = 0;
					strTmp2 = L"userdefinedSymbol";
				}
				else
				{
					nTmp = strTmp.FindOneOf(L"-+.1234567890");
					if (nTmp == -1) strTmp2 = strTmp;
					else strTmp2 = strTmp.Left(nTmp);
				}
			}
			else
			{
				nTmp = strTmp.FindOneOf(L"-+.1234567890");
				if (nTmp == -1) strTmp2 = strTmp;
				else strTmp2 = strTmp.Left(nTmp);
			}

			if (nTmp < 0)
				nTmp = 0;

			//기존 코드
			//pSymbolTmp->SetSymbolName(strTmp2, strTmp.GetBuffer(nTmp));	// Symbol Name 설정 
			//변경 : Feature 파일에 소문자로 되어 있기 때문에 소문자로 변수를 채우기 위함 hj.kim
			CString strSymbolName = strTmp2;
			CString strSymbolFullName = strTmp;
			strSymbolName.MakeLower();
			strSymbolFullName.MakeLower();
			pSymbolTmp->SetSymbolName(strSymbolName, strSymbolFullName);	// Symbol Name 설정 

			//Parameter 축출
			strTmp.Delete(0, nTmp);
			nParamIndex = 0;

			if (pSymbolTmp->m_eSymbolName != SymbolName::userdefined)
			{
				while ((nTmp = strTmp.FindOneOf(L"X")) != -1)// X가 있으면
				{
					strTmp2 = strTmp.Left(nTmp);
					pSymbolTmp->SetParamVal(nParamIndex++, strTmp2);

					if (strTmp.GetAt(nTmp + 1) == 'R' || strTmp.GetAt(nTmp + 1) == 'C')
						strTmp.Delete(0, nTmp + 2);
					else strTmp.Delete(0, nTmp + 1);
				}

				if ((nTmp = strTmp.FindOneOf(L"_")) != -1)
				{
					strTmp2 = strTmp.Left(nTmp);					
					pSymbolTmp->SetFinalParamVal(nParamIndex, strTmp2, pJobFile, true);
				
					strTmp.Delete(0, nTmp + 1);
					pSymbolTmp->m_dRotatedDeg = atof((CStringA)strTmp.GetBuffer(0));
				}
				else
				{					
					pSymbolTmp->SetFinalParamVal(nParamIndex, strTmp, pJobFile, false);					
					pSymbolTmp->m_dRotatedDeg = 0.0;
				}
			}

			pSymbolTmp->ConvertValue(pJobFile);
			pSymbolTmp->SetMinMaxRect();	// insert  Symbol Minmax 설정
			m_arrSymbol.Add(pSymbolTmp);	// Symbol 등록 
		}
		else if (chTmp == '@')	// attribute인 경우
		{
			//strTmp.MakeUpper();
			
			nTmp = strTmp.Find(L" ");
			strTmp2 = strTmp.Mid(1, nTmp - 1);
			strTmp.Delete(0, nTmp + 1);

			pAttribute = new CAttribute();
			pAttribute->m_iSerialNum = atoi((CStringA)strTmp2.GetBuffer(0));  // 번호 입력 
			pAttribute->m_strName = strTmp;		//attribute 등록

			m_arrAttribute.Add(pAttribute);
		}
		else if (chTmp == '&')	// attribute text string인 경우
		{
			//strTmp.MakeUpper();

			nTmp = strTmp.Find(L" ");
			strTmp2 = strTmp.Mid(1, nTmp - 1);
			strTmp.Delete(0, nTmp + 1);

			pAttributeTextString = new CAttributeTextString();
			pAttributeTextString->m_iSerialNum = atoi((CStringA)strTmp2.GetBuffer(0));  // 번호 입력 
			pAttributeTextString->m_strText = strTmp;		//attribute text string 등록

			m_arrAttributeTextString.Add(pAttributeTextString);
		}		
		else					// Feature인 경우
		{		
			CString strOriginal = strTmp; // to keep text record changing into uppercase
			strTmp.MakeUpper();

			if ('L' == chTmp)	pFeatureTmp = new CFeatureL;
			else if ('P' == chTmp)	pFeatureTmp = new CFeatureP;
			else if ('A' == chTmp)	pFeatureTmp = new CFeatureA;
			else if ('T' == chTmp)	pFeatureTmp = new CFeatureT;
			else if ('B' == chTmp)	pFeatureTmp = new CFeatureB;
			else if ('S' == chTmp)	pFeatureTmp = new CFeatureS;
			else if ('Z' == chTmp)  pFeatureTmp = new CFeatureZ;
			else continue;

			if (chTmp == 'T') strTmp = strOriginal; // to keep text record changing into uppercase

			pFeatureTmp->LoadFeature(strTmp, cStdFile, this, pJobFile, bIsMMUnit);
			pFeatureTmp->SetMinMaxRect();
			m_arrFeature.Add(pFeatureTmp);
			nFNum++;
		}
	}
	
	//	ar.Close();	
	strTmp.Format(L"\n%s = %d Feature Num\n", strFeatureFile, nFNum);
	OutputDebugString(strTmp);

	cStdFile.Close();
	SetMinMaxRect();
	return RESULT_GOOD;
}

UINT32 CFeatureFile::SaveFeatureFile(CString strFeatureFile)
{
	vector<CString> vecSaveData;

	//Symbol
	
	int nSymbolCount = static_cast<int>(m_arrSymbol.GetCount());
	if (nSymbolCount > 0)
	{
		vecSaveData.push_back(_T("#"));
		vecSaveData.push_back(_T("#Feature symbol names"));
		vecSaveData.push_back(_T("#"));

		for (int i = 0; i < nSymbolCount; i++)
		{
			CSymbol *pSymbol = m_arrSymbol.GetAt(i);
			CString strName;
			strName.Format(_T("$%d %s"), i, pSymbol->m_strUserSymbol);

			vecSaveData.push_back(strName);
		}

		vecSaveData.push_back(_T(""));
	}
	
	

	//attribute names
	int nAttCount = static_cast<int>(m_arrAttribute.GetCount());
	if (nAttCount > 0)
	{
		vecSaveData.push_back(_T("#"));
		vecSaveData.push_back(_T("#Feature attribute names"));
		vecSaveData.push_back(_T("#"));

		for (int i = 0; i < nAttCount; i++)
		{
			CAttribute *pAtt = m_arrAttribute.GetAt(i);
			CString strName;
			strName.Format(_T("@%d %s"), i, pAtt->m_strName);

			vecSaveData.push_back(strName);
		}
		vecSaveData.push_back(_T(""));
	}
	

	//attribute text strings
	int nAttStringCount = static_cast<int>(m_arrAttributeTextString.GetCount());
	if (nAttStringCount > 0)
	{
		vecSaveData.push_back(_T("#"));
		vecSaveData.push_back(_T("#Feature attribute text strings"));
		vecSaveData.push_back(_T("#"));

		for (int i = 0; i < nAttStringCount; i++)
		{
			CAttributeTextString *pAttString = m_arrAttributeTextString.GetAt(i);
			CString strName;
			strName.Format(_T("&%d %s"), i, pAttString->m_strText);

			vecSaveData.push_back(strName);
		}
		vecSaveData.push_back(_T(""));
	}
	

	//Layer features
	
	int nFeatureCount = static_cast<int>(m_arrFeature.GetCount());
	if (nFeatureCount > 0)
	{
		vecSaveData.push_back(_T("#"));
		vecSaveData.push_back(_T("#Layer features"));
		vecSaveData.push_back(_T("#"));

		
		CString strAttTemp, strAttStringTemp;
		CString strAttUnit;
		CString strFeatureData;//Attribute을 제외한 파일 한줄에 대한 String
		CString strFeatureAttData;//Attribute 한줄에 대한 String
		vector<CString> vecPolarity; vecPolarity.push_back(_T("N")); vecPolarity.push_back(_T("P"));
		vector<CString> vecYes; vecYes.push_back(_T("N")); vecYes.push_back(_T("Y"));
		vector<CString> vecTop; vecTop.push_back(_T("B")); vecTop.push_back(_T("T"));
		vector<CString> vecIsland; vecIsland.push_back(_T("H")); vecIsland.push_back(_T("I"));
		for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
		{
			CFeature *pFeature = m_arrFeature.GetAt(nfeno);
			if (pFeature == nullptr) continue;

			strFeatureData = _T("");
			strFeatureAttData = _T("");
			strAttUnit = _T("");

			//SetF
			switch (pFeature->m_eType)
			{
			case FeatureType::Z:
			{
				CFeatureZ *pFeatureZ= (CFeatureZ*)pFeature;

				strFeatureData.Format(_T("Z %.8lf %.8lf %d %s %d "), CJobFile::MMToInch(pFeatureZ->m_dX), CJobFile::MMToInch(pFeatureZ->m_dY),
					pFeatureZ->m_nSymNum, vecPolarity[pFeatureZ->m_bPolarity == false ? 0 : 1], pFeatureZ->m_nDecode);
			}
			break;
			case FeatureType::L:
			{
				//<xs> <ys>
				//<xe> <ye>
				//<sym_num> <polarity> <dcode>
				CFeatureL *pFeatureL = (CFeatureL *)pFeature;
				strFeatureData.Format(_T("L %.8lf %.8lf %.8lf %.8lf %d %s %d "), CJobFile::MMToInch(pFeatureL->m_dXs), CJobFile::MMToInch(pFeatureL->m_dYs),
					CJobFile::MMToInch(pFeatureL->m_dXe), CJobFile::MMToInch(pFeatureL->m_dYe),
					pFeatureL->m_nSymNum, vecPolarity[pFeatureL->m_bPolarity == false ? 0 : 1], pFeatureL->m_nDecode);
			}
			break;
			case FeatureType::P:
			{
				//<x> <y>
				//<apt_def> <polarity> <dcode> <orient_def>
				CFeatureP *pFeatureP = (CFeatureP *)pFeature;
				if (static_cast<UINT8>(pFeatureP->m_eOrient) == 8 || static_cast<UINT8>(pFeatureP->m_eOrient) == 9)
				{//현재 버전에서는 Load를 먼저 수정해야함,
	// 				strTemp.Format(_T("P %.8lf %.8lf %d %s %d %d %d %.1lf;"), pFeatureP->m_dX, pFeatureP->m_dY,
	// 					pFeatureP->m_nSymNum, vecPolarity[pFeatureP->m_bPolarity == false ? 0 : 1], pFeatureP->m_nDecode, static_cast<UINT8>(pFeatureP->m_eOrient), /*(AnyDegree)*/);
				}
				else
				{
					strFeatureData.Format(_T("P %.8lf %.8lf %d %s %d %d"), CJobFile::MMToInch(pFeatureP->m_dX), CJobFile::MMToInch(pFeatureP->m_dY),
						pFeatureP->m_nSymNum, vecPolarity[pFeatureP->m_bPolarity == false ? 0 : 1], pFeatureP->m_nDecode, static_cast<UINT8>(pFeatureP->m_eOrient));
				}

			}
			break;
			case FeatureType::A:
			{
				//<xs> <ys> <xe> <ye>
				//<xc> <yc>
				//<sym_num> <polarity> <dcode> <cw>
				CFeatureA *pFeatureA = (CFeatureA *)pFeature;
				strFeatureData.Format(_T("A %.8lf %.8lf %.8lf %.8lf %.8lf %.8lf %d %s %d %s"),
					CJobFile::MMToInch(pFeatureA->m_dXs), CJobFile::MMToInch(pFeatureA->m_dYs),
					CJobFile::MMToInch(pFeatureA->m_dXe), CJobFile::MMToInch(pFeatureA->m_dYe),
					CJobFile::MMToInch(pFeatureA->m_dXc), CJobFile::MMToInch(pFeatureA->m_dYc),
					pFeatureA->m_nSymNum, vecPolarity[pFeatureA->m_bPolarity == false ? 0 : 1], pFeatureA->m_nDecode, vecYes[pFeatureA->m_bCw == false ? 0 : 1]);
			}
			break;
			case FeatureType::T:
			{
				//<x> <y>
				//<font> <polarity> <orient_def>
				//<xsize> <ysize>
				//<width factor> <text> <version>
				CFeatureT *pFeatureT = (CFeatureT *)pFeature;
				strFeatureData.Format(_T("T %.8lf %.8lf %s %s %d %.8lf %.8lf %.8lf '%s' %d"), CJobFile::MMToInch(pFeatureT->m_dX), CJobFile::MMToInch(pFeatureT->m_dY),
					pFeatureT->m_strFont, vecPolarity[pFeatureT->m_bPolarity == false ? 0 : 1], static_cast<UINT8>(pFeatureT->m_eOrient),
					CJobFile::MMToInch(pFeatureT->m_dXSize), CJobFile::MMToInch(pFeatureT->m_dYSize),
					(pFeatureT->m_dWidthFactor), pFeatureT->m_strText, pFeatureT->m_nVersion);
			}
			break;
			case FeatureType::S:
			{
				CFeatureS *pFeatureS = (CFeatureS *)pFeature;
				strFeatureData.Format(_T("S %s %d"), vecPolarity[pFeatureS->m_bPolarity == false ? 0 : 1], pFeatureS->m_nDecode);
			}
			break;
			case FeatureType::B:
			{
				//<x> <y> <barcode> <font>
				//<polarity> <orient_def>
				//E <w> <h> 
				//<fasc> <cs> <bg>
				//<astr> <astr_pos>
				//<text>

				CFeatureB *pFeatureB = (CFeatureB *)pFeature;
				strFeatureData.Format(_T("B %.8lf %.8lf %s %s %s %d E %.8lf %.8lf %s %s %s %s %s %s"),
					CJobFile::MMToInch(pFeatureB->m_dX), CJobFile::MMToInch(pFeatureB->m_dY), pFeatureB->m_strBarcode, pFeatureB->m_strFont,
					vecPolarity[pFeatureB->m_bPolarity == false ? 0 : 1], static_cast<UINT8>(pFeatureB->m_eOrient),
					CJobFile::MMToInch(pFeatureB->m_dW), CJobFile::MMToInch(pFeatureB->m_dH),
					vecYes[pFeatureB->m_bFasc == false ? 0 : 1], vecYes[pFeatureB->m_bCs == false ? 0 : 1], vecYes[pFeatureB->m_bBg == false ? 0 : 1],
					vecYes[pFeatureB->m_bAstr == false ? 0 : 1], vecTop[pFeatureB->m_bAstrPos == false ? 0 : 1],
					pFeatureB->m_strText);
			}
			break;
			default:
				continue;
				break;
			}//End Switch


			//0,1=20,2=12 
			//
			int nFeaureAttCount = static_cast<int>(pFeature->m_arrAttribute.GetCount());
			int nFeaureAttStringCount = static_cast<int>(pFeature->m_arrAttributeTextString.GetCount());
			if (nFeaureAttCount == nFeaureAttStringCount)
			{
				for (int i = 0; i < nFeaureAttCount ; i++)
				{
					strAttTemp = _T("");
					strAttStringTemp = _T("");

					CAttribute *pAtt = pFeature->m_arrAttribute.GetAt(i);
					if (pAtt == nullptr) continue;

					strAttTemp.Format(_T("%d"), pAtt->m_iSerialNum);

					CAttributeTextString *pAttString = pFeature->m_arrAttributeTextString.GetAt(i);
					if (pAttString != nullptr)
					{
						strAttStringTemp.Format(_T("=%d"), pAttString->m_iSerialNum);
					}

					if (i == 0)
					{
						strAttUnit += (strAttTemp + strAttStringTemp);
					}
					else
					{
						strAttUnit += (_T(",") + strAttTemp + strAttStringTemp);
					}
				}
			}
			else
			{//갯수가 같지 않는 케이스//Error Case
				OutputDebugString(L"Attribute와 Attribute String의 갯수 차이 발생...");
			}


			//Make Attribute String (1Line)
			if (nFeaureAttCount > 0)
			{
				strFeatureAttData = _T(";") + strAttUnit;
			}
			else
			{
				strFeatureAttData = _T("");
			}

			vecSaveData.push_back(strFeatureData + strFeatureAttData);

			//Surface일 경우 OB, OS, OE를 추가해야한다.
			if (pFeature->m_eType == FeatureType::S)
			{
				CFeatureS *pFeatureS = (CFeatureS *)pFeature;
				if (pFeatureS == nullptr) continue;

				int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
				for (int nObOe = 0; nObOe < nObOeCount; nObOe++)
				{
					CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nObOe);
					if (pObOe == nullptr) continue;

					strFeatureData.Format(_T("OB %.12lf %.12lf %s"), CJobFile::MMToInch(pObOe->m_dXbs), CJobFile::MMToInch(pObOe->m_dYbs),
						vecIsland[pObOe->m_bPolyType == false ? 0 : 1]);
					vecSaveData.push_back(strFeatureData);

					int nOsOcCount = static_cast<int>(pObOe->m_arrOsOc.GetCount());
					for (int nOsOc = 0; nOsOc < nOsOcCount; nOsOc++)
					{
						COsOc *pOsOc = pObOe->m_arrOsOc.GetAt(nOsOc);
						if (pOsOc == nullptr) continue;

						if (pOsOc->m_bOsOc == true)
						{//Os
							strFeatureData.Format(_T("OS %.12lf %.12lf"), CJobFile::MMToInch(pOsOc->m_dX), CJobFile::MMToInch(pOsOc->m_dY));
						}
						else
						{//Oc
							COc *pOc = (COc *)pOsOc;
							strFeatureData.Format(_T("OC %.12lf %.12lf %.12lf %.12lf %s"), CJobFile::MMToInch(pOc->m_dX), CJobFile::MMToInch(pOc->m_dY),
								CJobFile::MMToInch(pOc->m_dXc), CJobFile::MMToInch(pOc->m_dYc), vecYes[pOc->m_bCw == false ? 0 : 1]);
						}

						vecSaveData.push_back(strFeatureData);
					}


					vecSaveData.push_back(_T("OE"));
				}

				vecSaveData.push_back(_T("SE"));

			}

		}//End Feature
	}

	//Write File
	CStdioFile cStdFile;
	if (!cStdFile.Open(strFeatureFile.GetBuffer(0), CFile::modeWrite | CFile::modeCreate | CFile::typeText))
		return RESULT_BAD;

	int nDataCount = static_cast<int>(vecSaveData.size());
	for (int i = 0; i < nDataCount; i++)
	{
		cStdFile.WriteString(vecSaveData[i] + _T("\n"));
	}

	cStdFile.Close();

	return RESULT_GOOD;
}

void CRecordRC::SetMinMaxRect()
{
	m_MinMaxRect.top = m_dLBy + m_dHeight;
	m_MinMaxRect.bottom = m_dLBy;
	m_MinMaxRect.left = m_dLBx;
	m_MinMaxRect.right = m_dLBx + m_dWidth;
}

void CRecordRC::LoadRecord(CString strOneLine, CStdioFile& cStdFile, bool bIsMMUnit)
{
	int nTmp = -1, iPos = 1;
	CString strTmp, strToken;

	if ((nTmp = strOneLine.Find(';')) != -1)
		strTmp = strOneLine.Left(nTmp);
	else
		strTmp = strOneLine;

	AfxExtractSubString(strToken, strTmp, iPos++, ' ');
	m_dLBx = bIsMMUnit ? atof((CStringA)strToken.GetBuffer(0)) :
		CJobFile::InchToMM(atof((CStringA)strToken.GetBuffer(0)));

	AfxExtractSubString(strToken, strTmp, iPos++, ' ');
	m_dLBy = bIsMMUnit ? atof((CStringA)strToken.GetBuffer(0)) :
		CJobFile::InchToMM(atof((CStringA)strToken.GetBuffer(0)));

	AfxExtractSubString(strToken, strTmp, iPos++, ' ');
	m_dWidth = bIsMMUnit ? atof((CStringA)strToken.GetBuffer(0)) :
		CJobFile::InchToMM(atof((CStringA)strToken.GetBuffer(0)));

	AfxExtractSubString(strToken, strTmp, iPos++, ' ');
	m_dHeight = bIsMMUnit ? atof((CStringA)strToken.GetBuffer(0)) :
		CJobFile::InchToMM(atof((CStringA)strToken.GetBuffer(0)));
}

void CRecordCR::SetMinMaxRect()
{
	m_MinMaxRect.top = m_dCy + m_dRadius;
	m_MinMaxRect.bottom = m_dCy - m_dRadius;
	m_MinMaxRect.left = m_dCx - m_dRadius;
	m_MinMaxRect.right = m_dCx + m_dRadius;
}

void CRecordCR::LoadRecord(CString strOneLine, CStdioFile& cStdFile, bool bIsMMUnit)
{
	int nTmp = -1, iPos = 1;
	CString strTmp, strToken;

	if ((nTmp = strOneLine.Find(';')) != -1)
		strTmp = strOneLine.Left(nTmp);
	else
		strTmp = strOneLine;

	AfxExtractSubString(strToken, strTmp, iPos++, ' ');
	m_dCx = bIsMMUnit ? atof((CStringA)strToken.GetBuffer(0)) :
		CJobFile::InchToMM(atof((CStringA)strToken.GetBuffer(0)));

	AfxExtractSubString(strToken, strTmp, iPos++, ' ');
	m_dCy = bIsMMUnit ? atof((CStringA)strToken.GetBuffer(0)) :
		CJobFile::InchToMM(atof((CStringA)strToken.GetBuffer(0)));

	AfxExtractSubString(strToken, strTmp, iPos++, ' ');
	m_dRadius = bIsMMUnit ? atof((CStringA)strToken.GetBuffer(0)) :
		CJobFile::InchToMM(atof((CStringA)strToken.GetBuffer(0)));
}

void CRecordSQ::SetMinMaxRect()
{
	m_MinMaxRect.top = m_dCy + m_dHalf;
	m_MinMaxRect.bottom = m_dCy - m_dHalf;
	m_MinMaxRect.left = m_dCx - m_dHalf;
	m_MinMaxRect.right = m_dCx + m_dHalf;
}

void CRecordSQ::LoadRecord(CString strOneLine, CStdioFile& cStdFile, bool bIsMMUnit)
{
	int nTmp = -1, iPos = 1;
	CString strTmp, strToken;

	if ((nTmp = strOneLine.Find(';')) != -1)
		strTmp = strOneLine.Left(nTmp);
	else
		strTmp = strOneLine;

	AfxExtractSubString(strToken, strTmp, iPos++, ' ');
	m_dCx = bIsMMUnit ? atof((CStringA)strToken.GetBuffer(0)) :
		CJobFile::InchToMM(atof((CStringA)strToken.GetBuffer(0)));

	AfxExtractSubString(strToken, strTmp, iPos++, ' ');
	m_dCy = bIsMMUnit ? atof((CStringA)strToken.GetBuffer(0)) :
		CJobFile::InchToMM(atof((CStringA)strToken.GetBuffer(0)));

	AfxExtractSubString(strToken, strTmp, iPos++, ' ');
	m_dHalf = bIsMMUnit ? atof((CStringA)strToken.GetBuffer(0)) :
		CJobFile::InchToMM(atof((CStringA)strToken.GetBuffer(0)));
}

void CRecordCT::SetMinMaxRect()
{
	RECTD tmpMinMax;
	double dXMin, dXMax, dYMin, dYMax, dRadius;
	double nXbs, nYbs, nX, nY, nXe, nYe, nXc, nYc;

	m_MinMaxRect.top = m_MinMaxRect.right = -1000000;
	m_MinMaxRect.bottom = m_MinMaxRect.left = 1000000;

	for (UINT32 i = 0; i < m_arrObOe.GetSize(); i++)
	{
		CObOe* pObOeTmp = m_arrObOe.GetAt(i);
		nXbs = pObOeTmp->m_dXbs;
		nYbs = pObOeTmp->m_dYbs;

		dYMax = dYMin = nYbs;
		dXMax = dXMin = nXbs;

		for (UINT32 j = 0; j < pObOeTmp->m_arrOsOc.GetSize(); j++)
		{
			if (pObOeTmp->m_arrOsOc[j]->m_bOsOc)		// OS 처리
			{
				nX = pObOeTmp->m_arrOsOc[j]->m_dX;
				nY = pObOeTmp->m_arrOsOc[j]->m_dY;

				dYMax = max(dYMax, nY);
				dYMin = min(dYMin, nY);
				dXMax = max(dXMax, nX);
				dXMin = min(dXMin, nX);
			}
			else                                       // OC 처리
			{
				dRadius = sqrt((((COc*)pObOeTmp->m_arrOsOc[j])->m_dX - ((COc*)pObOeTmp->m_arrOsOc[j])->m_dXc) * (((COc*)pObOeTmp->m_arrOsOc[j])->m_dX - ((COc*)pObOeTmp->m_arrOsOc[j])->m_dXc) +
					(((COc*)pObOeTmp->m_arrOsOc[j])->m_dY - ((COc*)pObOeTmp->m_arrOsOc[j])->m_dYc) * (((COc*)pObOeTmp->m_arrOsOc[j])->m_dY - ((COc*)pObOeTmp->m_arrOsOc[j])->m_dYc));

				// 시작점
				if (0 == j)
				{
					nX = nXbs;
					nY = nYbs;
				}
				else
				{
					nX = pObOeTmp->m_arrOsOc[j - 1]->m_dX;
					nY = pObOeTmp->m_arrOsOc[j - 1]->m_dY;
				}

				// 끝점
				nXe = pObOeTmp->m_arrOsOc[j]->m_dX;
				nYe = pObOeTmp->m_arrOsOc[j]->m_dY;

				// 중심점
				nXc = ((COc*)pObOeTmp->m_arrOsOc[j])->m_dXc;
				nYc = ((COc*)pObOeTmp->m_arrOsOc[j])->m_dYc;

				bool bTmpCw = ((COc*)pObOeTmp->m_arrOsOc[j])->m_bCw;
				SetMinMaxOsOc(nX, nY, nXe, nYe, nXc, nYc, bTmpCw, tmpMinMax);

				dYMax = max(dYMax, tmpMinMax.top);
				dYMin = min(dYMin, tmpMinMax.bottom);
				dXMax = max(dXMax, tmpMinMax.right);
				dXMin = min(dXMin, tmpMinMax.left);
			}
		} //for m_arrOsOc
		m_arrObOe[i]->m_MinMaxRect.top = dYMax;
		m_arrObOe[i]->m_MinMaxRect.bottom = dYMin;
		m_arrObOe[i]->m_MinMaxRect.left = dXMin;
		m_arrObOe[i]->m_MinMaxRect.right = dXMax;

		m_MinMaxRect.top = max(m_MinMaxRect.top, dYMax);
		m_MinMaxRect.bottom = min(m_MinMaxRect.bottom, dYMin);
		m_MinMaxRect.left = min(m_MinMaxRect.left, dXMin);
		m_MinMaxRect.right = max(m_MinMaxRect.right, dXMax);
	}//for m_arrObOe
}

void CRecordCT::ClearRecord()
{
	for (UINT32 i = 0; i < m_arrObOe.GetSize(); i++) 
	{ 
		m_arrObOe[i]->ClearOsOc(); 
		delete m_arrObOe[i];
	}

	m_arrObOe.RemoveAll();
}

void CRecordCT::SetMinMaxOsOc(double X1, double Y1, double X2, double Y2, double XC, double YC, bool cw, RECTD& tmpMinMax)
{
	double X1s, X2s, Y1s, Y2s, xl[6] = { 0 }, yl[6] = { 0 };
	double dRadius = 0, xMIN, yMIN, xMAX, yMAX;
	int	   nNumP = 0;				// 극점 후보 수 

	X1s = X1 - XC; X2s = X2 - XC;
	Y1s = Y1 - YC; Y2s = Y2 - YC;
	dRadius = sqrt((XC - X1) * (XC - X1) + (YC - Y1) * (YC - Y1));

	xl[0] = X1s, yl[0] = Y1s;	// start
	xl[1] = X2s, yl[1] = Y2s;	// end
	nNumP = 2;

	if (X1 < X2) { tmpMinMax.left = X1;	tmpMinMax.right = X2; }
	else { tmpMinMax.left = X2; tmpMinMax.right = X1; }

	if (Y1 < Y2) { tmpMinMax.bottom = Y1; tmpMinMax.top = Y2; }
	else { tmpMinMax.bottom = Y2; tmpMinMax.top = Y1; }

	if (cw)
	{
		if (X1 == X2 && Y1 == Y2) //같으면 모든 극점포함
		{
			nNumP = 6;
			xl[2] = 0.0;		yl[2] = dRadius;
			xl[3] = -dRadius;	yl[3] = 0.0;
			xl[4] = 0.0;		yl[4] = -dRadius;
			xl[5] = dRadius;	yl[5] = 0.0;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s >= 0 && Y2s >= 0)	// CW 1사분면
		{
			if ((X2s - X1s) >= 0 && (Y2s - Y1s) <= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 2 + 1;
			xl[2] = dRadius;	yl[2] = 0.0;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 5;
			xl[2] = dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = -dRadius;
			xl[4] = -dRadius;	yl[4] = 0.0;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 4;
			xl[2] = dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = -dRadius;
		}
		else if (X1s >= 0 && Y1s < 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 5;
			xl[2] = 0.0;		yl[2] = -dRadius;
			xl[3] = -dRadius;	yl[3] = 0.0;
			xl[4] = 0.0;		yl[4] = dRadius;
		}
		else if (X1s >= 0 && Y1s < 0 && X2s >= 0 && Y2s < 0) // CW 4사분면
		{
			if ((X2s - X1s) <= 0 && (Y2s - Y1s) <= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else if (X1s >= 0 && Y1s < 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 4;
			xl[2] = 0.0;		yl[2] = -dRadius;
			xl[3] = -dRadius;	yl[3] = 0.0;
		}
		else if (X1s >= 0 && Y1s < 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 3;
			xl[2] = 0.0;	yl[2] = -dRadius;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 3;
			xl[2] = 0.0;	yl[2] = dRadius;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 4;
			xl[2] = 0.0;		yl[2] = dRadius;
			xl[3] = dRadius;	yl[3] = 0.0;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s < 0 && Y2s >= 0)	// CW 2사분면
		{
			if ((X2s - X1s) >= 0 && (Y2s - Y1s) >= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else if (X1s < 0 && Y1s >= 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 5;
			xl[2] = 0.0;		yl[2] = dRadius;
			xl[3] = dRadius;	yl[3] = 0.0;
			xl[4] = 0.0;		yl[4] = -dRadius;
		}
		else if (X1s < 0 && Y1s < 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 4;
			xl[2] = -dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = dRadius;
		}
		else if (X1s < 0 && Y1s < 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 5;
			xl[2] = -dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = dRadius;
			xl[4] = dRadius;	yl[4] = 0.0;
		}
		else if (X1s < 0 && Y1s < 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 3;
			xl[2] = -dRadius;	yl[2] = 0.0;
		}
		else if (X1s < 0 && Y1s < 0 && X2s < 0 && Y2s < 0) // CW 3사분면
		{
			if ((X2s - X1s) <= 0 && (Y2s - Y1s) >= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else
		{
			OutputDebugString(L"CW에서 있을수 없는 경우 발생...");
		}
	}
	else
	{
		if (X1 == X2 && Y1 == Y2)	// 모든 극점 포함
		{
			nNumP = 6;
			xl[2] = 0.0, yl[2] = dRadius;
			xl[3] = -dRadius, yl[3] = 0.0;
			xl[4] = 0.0, yl[4] = -dRadius;
			xl[5] = dRadius, yl[5] = 0.0;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s >= 0 && Y2s >= 0)	// CCW 1사분면
		{
			if ((X2s - X1s) <= 0 && (Y2s - Y1s) >= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0, yl[2] = dRadius;
				xl[3] = -dRadius, yl[3] = 0.0;
				xl[4] = 0.0, yl[4] = -dRadius;
				xl[5] = dRadius, yl[5] = 0.0;
			}
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 5;
			xl[2] = 0.0;		yl[2] = dRadius;
			xl[3] = -dRadius;	yl[3] = 0.0;
			xl[4] = 0.0;		yl[4] = -dRadius;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 3;
			xl[2] = 0.0;		yl[2] = dRadius;
		}
		else if (X1s >= 0 && Y1s >= 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 4;
			xl[2] = 0.0;		yl[2] = dRadius;
			xl[3] = -dRadius;	yl[3] = 0.0;
		}
		else if (X1s >= 0 && Y1s < 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 3;
			xl[2] = dRadius;	yl[2] = 0.0;
		}
		else if (X1s >= 0 && Y1s < 0 && X2s >= 0 && Y2s < 0)	// CCW 4사분면
		{
			if ((X2s - X1s) >= 0 && (Y2s - Y1s) >= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else if (X1s >= 0 && Y1s < 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 4;
			xl[2] = dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = dRadius;

		}
		else if (X1s >= 0 && Y1s < 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 5;
			xl[2] = dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = dRadius;
			xl[4] = -dRadius;	yl[4] = 0.0;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 5;
			xl[2] = -dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = -dRadius;
			xl[4] = dRadius;	yl[4] = 0.0;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 4;
			xl[2] = -dRadius;	yl[2] = 0.0;
			xl[3] = 0.0;		yl[3] = -dRadius;
		}
		else if (X1s < 0 && Y1s >= 0 && X2s < 0 && Y2s >= 0)	// CCW 2사분면
		{
			if ((X2s - X1s) <= 0 && (Y2s - Y1s) <= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else if (X1s < 0 && Y1s >= 0 && X2s < 0 && Y2s < 0)
		{
			nNumP = 3;
			xl[2] = -dRadius;	yl[2] = 0.0;
		}
		else if (X1s < 0 && Y1s < 0 && X2s >= 0 && Y2s >= 0)
		{
			nNumP = 4;
			xl[2] = 0.0;		yl[2] = -dRadius;
			xl[3] = dRadius;	yl[3] = 0.0;

		}
		else if (X1s < 0 && Y1s < 0 && X2s >= 0 && Y2s < 0)
		{
			nNumP = 3;
			xl[2] = 0.0;	yl[2] = -dRadius;
		}
		else if (X1s < 0 && Y1s < 0 && X2s < 0 && Y2s >= 0)
		{
			nNumP = 5;
			xl[2] = 0.0;		yl[2] = -dRadius;
			xl[3] = dRadius;	yl[3] = 0.0;
			xl[4] = 0.0;		yl[4] = dRadius;
		}
		else if (X1s < 0 && Y1s < 0 && X2s < 0 && Y2s < 0)	// CCW 3사분면
		{
			if ((X2s - X1s) >= 0 && (Y2s - Y1s) <= 0) nNumP = 2;
			else
			{
				nNumP = 6;
				xl[2] = 0.0;		yl[2] = dRadius;
				xl[3] = -dRadius;	yl[3] = 0.0;
				xl[4] = 0.0;		yl[4] = -dRadius;
				xl[5] = dRadius;	yl[5] = 0.0;
			}
		}
		else
		{
			OutputDebugString(L"CCW 있을수 없는 경우 발생...");
		}
	}//end else

	xMIN = xl[0], xMAX = xl[0];
	yMIN = yl[0], yMAX = yl[0];

	for (int i = 0; i < nNumP; i++)
	{
		yMAX = max(yMAX, yl[i]);
		yMIN = min(yMIN, yl[i]);
		xMAX = max(xMAX, xl[i]);
		xMIN = min(xMIN, xl[i]);
	}

	tmpMinMax.left = xMIN + XC;
	tmpMinMax.right = xMAX + XC;
	tmpMinMax.top = yMAX + YC;
	tmpMinMax.bottom = yMIN + YC;
}

void CRecordCT::LoadRecord(CString strOneLine, CStdioFile& cStdFile, bool bIsMMUnit)
{
	if (strOneLine.Compare(L"CT") != 0)
		return;

	CString strTmp, strTmp2;
	CObOe* pTmpObOe = nullptr;
	int nTmp;

	//S~SE 구간 처리부 
	while (cStdFile.ReadString(strTmp))
	{
		strTmp.TrimLeft(L"\t ");
		strTmp.TrimRight(L"\t ");
		strTmp.MakeUpper();
		if (strTmp.GetLength() == 0)  // 빈줄 Skip
			continue;

		if (!strTmp.CompareNoCase(L"CE"))
			break;

		if (strTmp.Find(L"OB") != -1)
		{
			pTmpObOe = new CObOe;

			if ((nTmp = strTmp.Find(' ')) != -1) // OB 문자 제거
				strTmp.Delete(0, nTmp + 1);

			if ((nTmp = strTmp.Find(' ')) != -1)  // xbs 값 축출
			{
				strTmp2 = strTmp.Left(nTmp);
				
				pTmpObOe->m_dXbs = bIsMMUnit ? atof((CStringA)strTmp2.GetBuffer(0)) :
					CJobFile::InchToMM(atof((CStringA)strTmp2.GetBuffer(0)));

				strTmp.Delete(0, nTmp + 1);
			}

			if ((nTmp = strTmp.Find(' ')) != -1)  // ybs 값 축출
			{
				strTmp2 = strTmp.Left(nTmp);
				
				pTmpObOe->m_dYbs = bIsMMUnit ? atof((CStringA)strTmp2.GetBuffer(0)) :
					CJobFile::InchToMM(atof((CStringA)strTmp2.GetBuffer(0)));

				strTmp.Delete(0, nTmp + 1);
			}

			if (strTmp.GetLength())  // poly_type 값 축출
			{
				if (strTmp.GetAt(0) == 'I')
					pTmpObOe->m_bPolyType = true;
				else
					pTmpObOe->m_bPolyType = false;
			}

			while (cStdFile.ReadString(strTmp))
			{
				strTmp.TrimLeft(L"\t ");
				strTmp.TrimRight(L"\t ");
				strTmp.MakeUpper();
				if (strTmp.GetLength() == 0)  // 빈줄 Skip
					continue;

				if (!strTmp.CompareNoCase(L"OE"))
					break;

				pTmpObOe->LoadOsOc(strTmp, bIsMMUnit);  // Os, Oc 생성 (Load)
			}
			m_arrObOe.Add(pTmpObOe);
		}
	}
}

void CPackage::ClearPackage()
{
	if (m_pRecord)
		delete m_pRecord;
};

void CPackage::ClearPins()
{
	for (UINT32 i = 0; i < m_arrPin.GetSize(); i++)
	{
		m_arrPin[i]->ClearRecord();
		delete m_arrPin[i];
	}

	m_arrPin.RemoveAll();
}

void CComponent::ClearToeprints()
{
	for (UINT32 i = 0; i < m_arrToeprint.GetSize(); i++)
		delete m_arrToeprint[i];

	m_arrToeprint.RemoveAll();
}

void CComponentFile::ClearComponents()
{
	for (UINT32 i = 0; i < m_arrComponent.GetSize(); i++)
	{
		m_arrComponent[i]->ClearToeprints();
		delete m_arrComponent[i];
	}

	m_arrComponent.RemoveAll();
}

void CComponentFile::SetMinMaxRect()
{
	RECTD tmpMinMax;

	if (m_arrComponent.GetSize())
		m_MinMaxRect = m_arrComponent[0]->m_MinMaxRect;

	for (int i = 0; i < m_arrComponent.GetSize(); i++)
	{
		tmpMinMax.top = m_arrComponent[i]->m_MinMaxRect.top;
		tmpMinMax.bottom = m_arrComponent[i]->m_MinMaxRect.bottom;
		tmpMinMax.left = m_arrComponent[i]->m_MinMaxRect.left;
		tmpMinMax.right = m_arrComponent[i]->m_MinMaxRect.right;

		m_MinMaxRect.top = max(m_MinMaxRect.top, tmpMinMax.top);
		m_MinMaxRect.bottom = min(m_MinMaxRect.bottom, tmpMinMax.bottom);
		m_MinMaxRect.left = min(m_MinMaxRect.left, tmpMinMax.left);
		m_MinMaxRect.right = max(m_MinMaxRect.right, tmpMinMax.right);
	}
}

UINT32 CComponentFile::LoadComponentFile(CString strComponentFile, class CStep* pStep)
{
	CFileFind finder;
	if (!finder.FindFile(strComponentFile))
		return RESULT_GOOD;

	if (pStep == nullptr)
		return RESULT_BAD;

	CStdioFile cStdFile;
	CString strTmp, strToken;

	if (!cStdFile.Open(strComponentFile.GetBuffer(0), CFile::modeRead | CFile::typeText))
		return RESULT_BAD;

	ULONGLONG lFilePos = -1;
	int nTmp = -1;
	bool bIsMMUnit = false;

	while (cStdFile.ReadString(strTmp))
	{
		strTmp.TrimLeft(L"\t ");
		strTmp.TrimRight(L"\t ");
		if (strTmp.GetLength() == 0 || strTmp.Find(L"CMP") < 0)
		{
			lFilePos = cStdFile.GetPosition();
			continue;
		}

		break;
	}

	if (lFilePos < 0) return RESULT_GOOD;

	cStdFile.Seek(lFilePos, CStdioFile::begin);

	CComponent* pCompTmp = NULL;
	CToeprint* pToeTmp = NULL;
	UINT32 nCmpNum = 0;
	UINT32 nPkgNum = static_cast<UINT32>(pStep->m_arrPackage.GetSize());

	while (cStdFile.ReadString(strTmp))
	{
		if ((nTmp = strTmp.Find(';')) != -1)
			strTmp = strTmp.Left(nTmp);

		strTmp.TrimLeft(L"\t ");
		strTmp.TrimRight(L"\t ");

		int iPos = 0;
		AfxExtractSubString(strToken, strTmp, iPos++, ' ');

		if (strToken.Compare(L"CMP") == 0)
		{
			pCompTmp = new CComponent;

			AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//pkg index
			pCompTmp->m_nPkgIdx = atoi((CStringA)strToken.GetBuffer(0));

			if (pCompTmp->m_nPkgIdx >= nPkgNum)
			{
				delete pCompTmp;
				return RESULT_BAD;
			}

			pCompTmp->m_pPackage = pStep->m_arrPackage.GetAt(pCompTmp->m_nPkgIdx);
			if (pCompTmp->m_pPackage == nullptr)
				return RESULT_BAD;

			AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//x
			pCompTmp->m_dSx = bIsMMUnit ? atof((CStringA)strToken.GetBuffer(0)) :
				CJobFile::InchToMM(atof((CStringA)strToken.GetBuffer(0)));

			AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//y
			pCompTmp->m_dSy = bIsMMUnit ? atof((CStringA)strToken.GetBuffer(0)) :
				CJobFile::InchToMM(atof((CStringA)strToken.GetBuffer(0)));

			AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//rotation
			double dAngle = atof((CStringA)strToken.GetBuffer(0));

			AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//mirror
			if (strToken.CompareNoCase(L"M") == 0)
				pCompTmp->m_bMirror = true;

			if ((dAngle == 0) && (pCompTmp->m_bMirror == false))			pCompTmp->m_eOrient = Orient::NoMir0Deg;
			else if ((dAngle == 45) && (pCompTmp->m_bMirror == false))		pCompTmp->m_eOrient = Orient::NoMir45Deg;
			else if ((dAngle == 90) && (pCompTmp->m_bMirror == false))		pCompTmp->m_eOrient = Orient::NoMir90Deg;
			else if ((dAngle == 135) && (pCompTmp->m_bMirror == false))		pCompTmp->m_eOrient = Orient::NoMir135Deg;
			else if ((dAngle == 180) && (pCompTmp->m_bMirror == false))		pCompTmp->m_eOrient = Orient::NoMir180Deg;
			else if ((dAngle == 225) && (pCompTmp->m_bMirror == false))		pCompTmp->m_eOrient = Orient::NoMir225Deg;
			else if ((dAngle == 270) && (pCompTmp->m_bMirror == false))		pCompTmp->m_eOrient = Orient::NoMir270Deg;
			else if ((dAngle == 315) && (pCompTmp->m_bMirror == false))		pCompTmp->m_eOrient = Orient::NoMir315Deg;
			else if ((dAngle == 0) && (pCompTmp->m_bMirror == true))		pCompTmp->m_eOrient = Orient::Mir0Deg;
			else if ((dAngle == 45) && (pCompTmp->m_bMirror == true))		pCompTmp->m_eOrient = Orient::Mir45Deg;
			else if ((dAngle == 90) && (pCompTmp->m_bMirror == true))		pCompTmp->m_eOrient = Orient::Mir90Deg;
			else if ((dAngle == 135) && (pCompTmp->m_bMirror == true))		pCompTmp->m_eOrient = Orient::Mir135Deg;
			else if ((dAngle == 180) && (pCompTmp->m_bMirror == true))		pCompTmp->m_eOrient = Orient::Mir180Deg;
			else if ((dAngle == 225) && (pCompTmp->m_bMirror == true))		pCompTmp->m_eOrient = Orient::Mir225Deg;
			else if ((dAngle == 270) && (pCompTmp->m_bMirror == true))		pCompTmp->m_eOrient = Orient::Mir270Deg;
			else if ((dAngle == 315) && (pCompTmp->m_bMirror == true))		pCompTmp->m_eOrient = Orient::Mir315Deg;
			else pCompTmp->m_eOrient = Orient::NoMir0Deg;

			AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//comp name
			pCompTmp->m_strCompName = strToken;

			AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//part name
			pCompTmp->m_strPartName = strToken;

			if (pCompTmp->m_pPackage->m_pRecord)
			{
				m_MinMaxRect.top = max(pCompTmp->m_dSy + pCompTmp->m_pPackage->m_pRecord->m_MinMaxRect.top, pCompTmp->m_dSy + pCompTmp->m_pPackage->m_pRecord->m_MinMaxRect.bottom);
				m_MinMaxRect.bottom = min(pCompTmp->m_dSy + pCompTmp->m_pPackage->m_pRecord->m_MinMaxRect.top, pCompTmp->m_dSy + pCompTmp->m_pPackage->m_pRecord->m_MinMaxRect.bottom);
				m_MinMaxRect.left = min(pCompTmp->m_dSx + pCompTmp->m_pPackage->m_pRecord->m_MinMaxRect.left, pCompTmp->m_dSx + pCompTmp->m_pPackage->m_pRecord->m_MinMaxRect.right);
				m_MinMaxRect.right = max(pCompTmp->m_dSx + pCompTmp->m_pPackage->m_pRecord->m_MinMaxRect.left, pCompTmp->m_dSx + pCompTmp->m_pPackage->m_pRecord->m_MinMaxRect.right);

				pCompTmp->m_MinMaxRect = m_MinMaxRect;
			}

			lFilePos = cStdFile.GetPosition();

			UINT32 nToeNum = 0;
			UINT32 nPinNum = static_cast<UINT32>(pCompTmp->m_pPackage->m_arrPin.GetSize());

			while (cStdFile.ReadString(strTmp))
			{
				iPos = 0;
				AfxExtractSubString(strToken, strTmp, iPos++, ' ');
				if (strToken.Compare(L"TOP") != 0)
				{
					cStdFile.Seek(lFilePos, CStdioFile::begin);
					break;
				}

				pToeTmp = new CToeprint;

				AfxExtractSubString(strToken, strTmp, iPos++, ' ');	// pin_num
				pToeTmp->m_nPinIdx = atoi((CStringA)strToken.GetBuffer(0));

				if (pToeTmp->m_nPinIdx >= nPinNum)
				{
					delete pCompTmp;
					delete pToeTmp;
					return RESULT_BAD;
				}

				pToeTmp->m_pPin = pCompTmp->m_pPackage->m_arrPin.GetAt(pToeTmp->m_nPinIdx);
				if (pToeTmp->m_pPin == nullptr)
					return RESULT_BAD;

				AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//x
				pToeTmp->m_dSx = bIsMMUnit ? atof((CStringA)strToken.GetBuffer(0)) :
					CJobFile::InchToMM(atof((CStringA)strToken.GetBuffer(0)));

				AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//y
				pToeTmp->m_dSy = bIsMMUnit ? atof((CStringA)strToken.GetBuffer(0)) :
					CJobFile::InchToMM(atof((CStringA)strToken.GetBuffer(0)));

				AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//rotation
				double dAngle = atof((CStringA)strToken.GetBuffer(0));

				AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//mirror
				if (strToken.CompareNoCase(L"M") == 0)
					pToeTmp->m_bMirror = true;

				if ((dAngle == 0) && (pToeTmp->m_bMirror == false))				pToeTmp->m_eOrient = Orient::NoMir0Deg;
				else if ((dAngle == 45) && (pToeTmp->m_bMirror == false))		pToeTmp->m_eOrient = Orient::NoMir45Deg;
				else if ((dAngle == 90) && (pToeTmp->m_bMirror == false))		pToeTmp->m_eOrient = Orient::NoMir90Deg;
				else if ((dAngle == 135) && (pToeTmp->m_bMirror == false))		pToeTmp->m_eOrient = Orient::NoMir135Deg;
				else if ((dAngle == 180) && (pToeTmp->m_bMirror == false))		pToeTmp->m_eOrient = Orient::NoMir180Deg;
				else if ((dAngle == 225) && (pToeTmp->m_bMirror == false))		pToeTmp->m_eOrient = Orient::NoMir225Deg;
				else if ((dAngle == 270) && (pToeTmp->m_bMirror == false))		pToeTmp->m_eOrient = Orient::NoMir270Deg;
				else if ((dAngle == 315) && (pToeTmp->m_bMirror == false))		pToeTmp->m_eOrient = Orient::NoMir315Deg;
				else if ((dAngle == 0) && (pToeTmp->m_bMirror == true))			pToeTmp->m_eOrient = Orient::Mir0Deg;
				else if ((dAngle == 45) && (pToeTmp->m_bMirror == true))		pToeTmp->m_eOrient = Orient::Mir45Deg;
				else if ((dAngle == 90) && (pToeTmp->m_bMirror == true))		pToeTmp->m_eOrient = Orient::Mir90Deg;
				else if ((dAngle == 135) && (pToeTmp->m_bMirror == true))		pToeTmp->m_eOrient = Orient::Mir135Deg;
				else if ((dAngle == 180) && (pToeTmp->m_bMirror == true))		pToeTmp->m_eOrient = Orient::Mir180Deg;
				else if ((dAngle == 225) && (pToeTmp->m_bMirror == true))		pToeTmp->m_eOrient = Orient::Mir225Deg;
				else if ((dAngle == 270) && (pToeTmp->m_bMirror == true))		pToeTmp->m_eOrient = Orient::Mir270Deg;
				else if ((dAngle == 315) && (pToeTmp->m_bMirror == true))		pToeTmp->m_eOrient = Orient::Mir315Deg;
				else pToeTmp->m_eOrient = Orient::NoMir0Deg;

				AfxExtractSubString(strToken, strTmp, iPos++, ' ');	// net_num
				pToeTmp->m_nNetNum = atoi((CStringA)strToken.GetBuffer(0));
				AfxExtractSubString(strToken, strTmp, iPos++, ' ');	// subnet_num
				pToeTmp->m_nSubNetNum = atoi((CStringA)strToken.GetBuffer(0));
				AfxExtractSubString(strToken, strTmp, iPos++, ' ');	// toeprint name
				pToeTmp->m_strName = strToken;

				if (pToeTmp->m_pPin->m_pRecord)
				{
					m_MinMaxRect.top = max(m_MinMaxRect.top, max(pToeTmp->m_dSy + pToeTmp->m_pPin->m_pRecord->m_MinMaxRect.top, pToeTmp->m_dSy + pToeTmp->m_pPin->m_pRecord->m_MinMaxRect.bottom));
					m_MinMaxRect.bottom = min(m_MinMaxRect.bottom, min(pToeTmp->m_dSy + pToeTmp->m_pPin->m_pRecord->m_MinMaxRect.top, pToeTmp->m_dSy + pToeTmp->m_pPin->m_pRecord->m_MinMaxRect.bottom));
					m_MinMaxRect.left = min(m_MinMaxRect.left, min(pToeTmp->m_dSx + pToeTmp->m_pPin->m_pRecord->m_MinMaxRect.left, pToeTmp->m_dSx + pToeTmp->m_pPin->m_pRecord->m_MinMaxRect.right));
					m_MinMaxRect.right = max(m_MinMaxRect.right, max(pToeTmp->m_dSx + pToeTmp->m_pPin->m_pRecord->m_MinMaxRect.left, pToeTmp->m_dSx + pToeTmp->m_pPin->m_pRecord->m_MinMaxRect.right));
				}

				lFilePos = cStdFile.GetPosition();

				pCompTmp->m_arrToeprint.Add(pToeTmp);
				nToeNum++;
			}

			m_arrComponent.Add(pCompTmp);
			nCmpNum++;

			strTmp.Format(L"\n[%d] Component = [%d] Pin Num\n", nCmpNum, nToeNum);
			OutputDebugString(strTmp);
		}
	}

	cStdFile.Close();
	strTmp.Format(L"%s\n : %d Component Num\n", strComponentFile, nCmpNum);
	OutputDebugString(strTmp);

	SetMinMaxRect();
	return RESULT_GOOD;
}


void CLayer::ClearLayer()
{ 
	m_FeatureFile.ClearAll(); 
	m_ComponentFile.ClearComponents();

	ClearAlignMask(MaskType::enumType_Align);
	ClearAlignMask(MaskType::enumType_Mask);
}

void CLayer::Serialize(CArchive& ar)
{
	CObject::Serialize(ar); 

	if (ar.IsStoring())
	{
		ar.Write(&m_bVisible, sizeof(m_bVisible));
		ar.Write(&m_nID, sizeof(m_nID));
		ar.Write(&m_fXScale, sizeof(m_fXScale));
		ar.Write(&m_fYScale, sizeof(m_fYScale));
		ar << m_strLayerName;

		m_FeatureFile.Serialize(ar);	
		m_ComponentFile.Serialize(ar);
	}
	else
	{	
		ar.Read(&m_bVisible, sizeof(m_bVisible));
		ar.Read(&m_nID, sizeof(m_nID));
		ar.Read(&m_fXScale, sizeof(m_fXScale));
		ar.Read(&m_fYScale, sizeof(m_fYScale));
		ar >> m_strLayerName;

		m_FeatureFile.Serialize(ar);
		m_ComponentFile.Serialize(ar);
	}
}

UINT32 CLayer::AssignDynamicStrings(CString strStepName, CJobFile* pJobFile)
{
	CFeatureFile* pFeatureFile = &(this->m_FeatureFile);
	if (pFeatureFile == nullptr)
		return RESULT_BAD;

	UINT DayOfWeek[] = {
		LOCALE_SDAYNAME7,   // Sunday
		LOCALE_SDAYNAME1,
		LOCALE_SDAYNAME2,
		LOCALE_SDAYNAME3,
		LOCALE_SDAYNAME4,
		LOCALE_SDAYNAME5,
		LOCALE_SDAYNAME6   // Saturday
	};

	std::map<CString, UINT32> vcDynamicStr;
	vcDynamicStr.insert(std::make_pair(L"$$MM$$DD", 0));
	vcDynamicStr.insert(std::make_pair(L"$$YY$$MM$$DD", 1));
	vcDynamicStr.insert(std::make_pair(L"$$DATE-MMDDYY", 2));	//"$$date-mmddyy" 문자 처리		mm/dd/yy
	vcDynamicStr.insert(std::make_pair(L"$$DATE-DDMMYY", 3));	//"$$date-ddmmyy" 문자 처리		dd/mm/yy
	vcDynamicStr.insert(std::make_pair(L"$$DATE-MMDDYYYY", 4));	//"$$date-mmddyyyy" 문자 처리	mm/dd/yyyy
	vcDynamicStr.insert(std::make_pair(L"$$DATE-DDMMYYYY", 5));	//"$$date-ddmmyyyy" 문자 처리	dd/mm/yyyy
	vcDynamicStr.insert(std::make_pair(L"$$DD", 6));			//"$$dd" 문자 처리				dd
	vcDynamicStr.insert(std::make_pair(L"$$MM", 7));			//"$$mm" 문자 처리				mm
	vcDynamicStr.insert(std::make_pair(L"$$YY", 8));			//"$$yy" 문자 처리				yy
	vcDynamicStr.insert(std::make_pair(L"$$YYYY", 9));			//"$$yyyy" 문자 처리			yyyy
	vcDynamicStr.insert(std::make_pair(L"$$WEEK-DAY", 10));		//"$$week-day" 문자 처리		Sunday-Saturday
	vcDynamicStr.insert(std::make_pair(L"$${YY}$${WW}", 11));
	vcDynamicStr.insert(std::make_pair(L"$$WW", 12));			//"$$ww" 문자 처리				week of year(01-52)
	vcDynamicStr.insert(std::make_pair(L"$$YY $$WW", 13));
	vcDynamicStr.insert(std::make_pair(L"$$WW$$YY", 14));
	vcDynamicStr.insert(std::make_pair(L"$$TIME", 15));			//"$$time" 문자 처리			hh:mm
	vcDynamicStr.insert(std::make_pair(L"$${JOB}", 16));
	vcDynamicStr.insert(std::make_pair(L"$$JOB", 17));
	vcDynamicStr.insert(std::make_pair(L"$$STEP", 18));
	vcDynamicStr.insert(std::make_pair(L"$$LAYER", 19));
	vcDynamicStr.insert(std::make_pair(L"( $$LAYER )", 20));
	vcDynamicStr.insert(std::make_pair(L"$$PART_NUMBER", 21));
	vcDynamicStr.insert(std::make_pair(L"$$LOT_WEEK$$LOT_YEAR$$LOT_REV", 22));
	vcDynamicStr.insert(std::make_pair(L"$$JOB $$LAYER", 23));
	vcDynamicStr.insert(std::make_pair(L"$$JOB $${LAYER}", 24));
	vcDynamicStr.insert(std::make_pair(L"$$JOB      $$LAYER", 25));
	vcDynamicStr.insert(std::make_pair(L"$$YY$$WW", 26));
	vcDynamicStr.insert(std::make_pair(L"$$X", 27));			//"$$x" 문자 처리				x coordinate(inches)
	vcDynamicStr.insert(std::make_pair(L"$$Y", 28));			//"$$y" 문자 처리				y coordinate(inches)
	vcDynamicStr.insert(std::make_pair(L"$$X_MM", 29));			//"$$x_mm" 문자 처리			x coordinate(mm)
	vcDynamicStr.insert(std::make_pair(L"$$Y_MM", 30));			//"$$y_mm" 문자 처리			y coordinate(mm)
	vcDynamicStr.insert(std::make_pair(L"$$DATE", 31));			//"$$y_mm" 문자 처리			y coordinate(mm)
	
	CTime currentTime = CTime::GetCurrentTime();

	for (UINT32 i = 0; i < pFeatureFile->m_arrFeature.GetSize(); i++)
	{
		CFeature* pFeature = pFeatureFile->m_arrFeature.GetAt(i);
		if (pFeature == nullptr || pFeature->m_eType != FeatureType::T)
			continue;

		auto it = vcDynamicStr.find(((CFeatureT*)pFeature)->m_strText);
		if (it != vcDynamicStr.end())
		{
			CString tmpStr2 = L"", tmpStr3 = L"";

			switch (it->second)
			{
			case 0:
				tmpStr2.Format(L"%02d/%02d", currentTime.GetMonth(), currentTime.GetDay());
				break;
			case 1:
				tmpStr3.Format(L"%d", currentTime.GetYear());
				tmpStr2.Format(L"%s/%02d/%02d", tmpStr3.Right(2).GetBuffer(0), currentTime.GetMonth(), currentTime.GetDay());
				break;
			case 2:
			case 31:
				tmpStr3.Format(L"%d", currentTime.GetYear());
				tmpStr2.Format(L"%02d/%02d/%s", currentTime.GetMonth(), currentTime.GetDay(), tmpStr3.Right(2).GetBuffer(0));
				break;
			case 3:
				tmpStr3.Format(L"%d", currentTime.GetYear());
				tmpStr2.Format(L"%02d/%02d/%s", currentTime.GetDay(), currentTime.GetMonth(), tmpStr3.Right(2).GetBuffer(0));
				break;
			case 4:
				tmpStr2.Format(L"%02d/%02d/%04d", currentTime.GetMonth(), currentTime.GetDay(), currentTime.GetYear());
				break;
			case 5:
				tmpStr2.Format(L"%02d/%02d/%04d", currentTime.GetDay(), currentTime.GetMonth(), currentTime.GetYear());
				break;
			case 6:
				tmpStr2.Format(L"%02d", currentTime.GetDay());
				break;
			case 7:
				tmpStr2.Format(L"%02d", currentTime.GetMonth());
				break;
			case 8:
				tmpStr3.Format(L"%d", currentTime.GetYear());
				tmpStr2.Format(L"%s", tmpStr3.Right(2).GetBuffer(0));
				break;
			case 9:
				tmpStr2.Format(L"%04d", currentTime.GetYear());
				break;
			case 10:
				tmpStr2.Format(L"%s", DayOfWeek[currentTime.GetDayOfWeek() - 1]);
				break;
			case 11:
				tmpStr3.Format(L"%d", currentTime.GetYear());
				tmpStr2.Format(L"%s%s", tmpStr3.Right(2).GetBuffer(0), currentTime.Format(L"%U"));
				break;
			case 12:
				tmpStr2.Format(L"%s", currentTime.Format(L"%U"));
				break;
			case 13:
				tmpStr3.Format(L"%d", currentTime.GetYear());
				tmpStr2.Format(L"%s %s", tmpStr3.Right(2).GetBuffer(0), currentTime.Format(L"%U"));
				break;
			case 14:
				tmpStr3.Format(L"%d", currentTime.GetYear());
				tmpStr2.Format(L"%s%s", currentTime.Format(L"%U"), tmpStr3.Right(2).GetBuffer(0));
				break;
			case 15:
				tmpStr2.Format(L"%02d:%02d", currentTime.GetHour(), currentTime.GetMinute());
				break;
			case 16:
			case 17:
				tmpStr2 = pJobFile->m_Misc.GetModelName();
				break;
			case 18:
				tmpStr2 = strStepName.MakeUpper();
				break;
			case 19:
				tmpStr2 = this->m_strLayerName;
				break;
			case 20:
				tmpStr3.Format(L"( ");
				tmpStr2.Format(L"%s%s )", tmpStr3, this->m_strLayerName);
				break;
			case 21:
				tmpStr2 = pJobFile->m_Misc.m_strPartNum;
				break;
			case 22:
				tmpStr2 = pJobFile->m_Misc.m_strLotWeek + pJobFile->m_Misc.m_strLotYear + pJobFile->m_Misc.m_strLotRev;
				break;
			case 23:
				tmpStr2.Format(L"%s %s", pJobFile->m_Misc.GetModelName(), this->m_strLayerName);
				break;
			case 24:
				tmpStr2.Format(L"%s {%s}", pJobFile->m_Misc.GetModelName(), this->m_strLayerName);
				break;
			case 25:
				tmpStr2.Format(L"%s      %s", pJobFile->m_Misc.GetModelName(), this->m_strLayerName);
				tmpStr2.MakeUpper();
				break;
			case 26:
				tmpStr3.Format(L"%d", currentTime.GetYear());
				tmpStr2.Format(L"%s%s", tmpStr3.Right(2).GetBuffer(0), currentTime.Format(L"%U"));
				break;
			case 27:
				tmpStr2.Format(L"%f", CJobFile::MMToInch(((CFeatureT*)pFeature)->m_dX));
				break;
			case 28:
				tmpStr2.Format(L"%f", CJobFile::MMToInch(((CFeatureT*)pFeature)->m_dY));
				break;
			case 29:
				tmpStr2.Format(L"%f", ((CFeatureT*)pFeature)->m_dX);
				break;
			case 30:
				tmpStr2.Format(L"%f", ((CFeatureT*)pFeature)->m_dY);
				break;
			default:
				//Attribute문자를 처리할것 나중에 ??????????????? 
				OutputDebugString(pJobFile->m_Misc.GetModelName() + L" " + strStepName + L" " +
					this->m_strLayerName + L" " + ((CFeatureT*)pFeature)->m_strText +
					L" : You must add new codes for the dynamic strings");
				break;
			}

			if (tmpStr2.GetLength() < 1)
				continue;

			((CFeatureT*)pFeature)->m_strText = tmpStr2;
			OutputDebugString(tmpStr2 + L"\n");
			((CFeatureT*)pFeature)->SetMinMaxRect();
		}
	}
	
	return RESULT_GOOD;
}

UINT32 CLayer::ClearAlignMask(IN MaskType eMaskType)
{
	CTypedPtrArray<CObArray, CAlignMask*> *pArrAlignMask = nullptr;
	if (eMaskType == MaskType::enumType_Align)
	{
		pArrAlignMask = &m_arrAlign;
	}
	else if (eMaskType == MaskType::enumType_Mask)
	{
		pArrAlignMask = &m_arrMask;
	}
	else
	{
		return RESULT_BAD;
	}
	if (pArrAlignMask == nullptr) return RESULT_BAD;

	//Feature 파일에 없는 Symbol이기 때문에 삭제해준다.
	for (UINT32 i = 0; i < pArrAlignMask->GetCount(); i++)
	{
		CAlignMask *pAlignMask = pArrAlignMask->GetAt(i);

		pAlignMask->Clear();
	}

	return RESULT_GOOD;
}

UINT32 CLayer::LoadAlignMask(IN CString &strPathName, IN MaskType eMaskType)
{
	ClearAlignMask(eMaskType);

	CTypedPtrArray<CObArray, CAlignMask*> *pArrAlignMask = nullptr;
	if (eMaskType == MaskType::enumType_Align)
	{
		pArrAlignMask = &m_arrAlign;
	}
	else if (eMaskType == MaskType::enumType_Mask)
	{
		pArrAlignMask = &m_arrMask;
	}
	else
	{
		return RESULT_BAD;
	}
	if (pArrAlignMask == nullptr) return RESULT_BAD;

	CStdioFile cStdFile;
	CString strTmp = L"";
	TCHAR chTmp = 0;

	if (!cStdFile.Open(strPathName.GetBuffer(0), CFile::modeRead | CFile::typeText))
		return RESULT_GOOD;

	CAlignMask *pAlignMask = nullptr;

	while (cStdFile.ReadString(strTmp))
	{
		strTmp.TrimLeft(L"\t ");
		strTmp.TrimRight(L"\t ");
		if (strTmp.GetLength() == 0) continue;
		
		chTmp = strTmp.GetAt(0);
		if (chTmp == '#')	// 주석인 경우
		{
			continue;
		}
		else
		{
			//Name, X, Y, Width, Height
			vector<CString> vecSplit;
			CUtils::Split(strTmp, _T(" "), vecSplit);
			
			/*
			_T("PS"),
			_T("SS"),
			_T("SE"),
			_T("PE"),
			*/
			

			
			CFeature *pFeature = nullptr;
			if (vecSplit[DefAlignMaskPara::enumHead] == DefAlignMaskHead::pStrName[DefAlignMaskHead::enumPatternStart])
			{
				int nSepCount = static_cast<int>(vecSplit.size());
				if (nSepCount < DefAlignMaskPara::enumMax) continue;

				pAlignMask = new CAlignMask();
				pAlignMask->Add_Pad(CJobFile::InchToMM(_ttof(vecSplit[DefAlignMaskPara::enumCenterX])), CJobFile::InchToMM(_ttof(vecSplit[DefAlignMaskPara::enumCenterY])),
					CJobFile::InchToMM(_ttof(vecSplit[DefAlignMaskPara::enumWidth])), CJobFile::InchToMM(_ttof(vecSplit[DefAlignMaskPara::enumHeight]))
					);
			}
			else if (vecSplit[DefAlignMaskPara::enumHead] == DefAlignMaskHead::pStrName[DefAlignMaskHead::enumSubStart])
			{
				int nSepCount = static_cast<int>(vecSplit.size());
				if (nSepCount < DefAlignMaskPara::enumMax) continue;

				pAlignMask->Add_Pad(CJobFile::InchToMM(_ttof(vecSplit[DefAlignMaskPara::enumCenterX])), CJobFile::InchToMM(_ttof(vecSplit[DefAlignMaskPara::enumCenterY])),
					CJobFile::InchToMM(_ttof(vecSplit[DefAlignMaskPara::enumWidth])), CJobFile::InchToMM(_ttof(vecSplit[DefAlignMaskPara::enumHeight]))
				);
			}
			else if (vecSplit[DefAlignMaskPara::enumHead] == DefAlignMaskHead::pStrName[DefAlignMaskHead::enumSubEnd])
			{

			}
			else if (vecSplit[DefAlignMaskPara::enumHead] == DefAlignMaskHead::pStrName[DefAlignMaskHead::enumPatternEnd])
			{
				pArrAlignMask->Add(pAlignMask);
			}

		}
	}

	return RESULT_GOOD;
}

UINT32 CLayer::SaveAlignMask(IN CString &strPathName, IN MaskType eMaskType)
{
	CTypedPtrArray<CObArray, CAlignMask*> *pArrAlignMask = nullptr;
	if (eMaskType == MaskType::enumType_Align)
	{
		pArrAlignMask = &m_arrAlign;
	}
	else if (eMaskType == MaskType::enumType_Mask)
	{
		pArrAlignMask = &m_arrMask;
	}
	else
	{
		return RESULT_BAD;
	}
	if (pArrAlignMask == nullptr) return RESULT_BAD;

	int nCount = static_cast<int>(pArrAlignMask->GetCount());
	
	vector<CString> vecSaveData;

	vecSaveData.clear();
	if (nCount > 0)
	{
		CString strTemp;
		for (int nAlign = 0; nAlign < nCount; nAlign++)
		{
			CAlignMask *pAlignMask = pArrAlignMask->GetAt(nAlign);
			if (pAlignMask == nullptr) continue;

			int nSubCount = static_cast<int>(pAlignMask->m_arrFeature.GetCount());
			if (nSubCount > 0)
			{
				for (int nSub = 0; nSub < nSubCount; nSub++)
				{
					CFeature *pFeature = pAlignMask->m_arrFeature.GetAt(nSub);
					if (pFeature == nullptr) continue;

					if (pFeature->m_eType != FeatureType::P) continue;

					CFeatureP *pFeatureP = (CFeatureP *)pFeature;
					CSymbol *pSymbol = pFeatureP->m_pSymbol;
					if (pSymbol == nullptr) continue;

					if (pSymbol->m_vcParams.size() < 2) continue;

					//Name, Center X, Y, With, Height
					if (nSub == 0)
					{
						strTemp.Format(_T("%s %.8lf %.8lf %.8lf %.8lf"), DefAlignMaskHead::pStrName[DefAlignMaskHead::enumPatternStart], CJobFile::MMToInch(pFeatureP->m_dX),
							CJobFile::MMToInch(pFeatureP->m_dY), CJobFile::MMToInch(pSymbol->m_vcParams[0]), CJobFile::MMToInch(pSymbol->m_vcParams[1]));
					}
					else
					{
						//Align Sub
						strTemp.Format(_T("%s %.8lf %.8lf %.8lf %.8lf"), DefAlignMaskHead::pStrName[DefAlignMaskHead::enumSubStart], CJobFile::MMToInch(pFeatureP->m_dX),
							CJobFile::MMToInch(pFeatureP->m_dY), CJobFile::MMToInch(pSymbol->m_vcParams[0]), CJobFile::MMToInch(pSymbol->m_vcParams[1]));
					}

					vecSaveData.push_back(strTemp);
				}//End for Sub

				if (nSubCount > 1)
				{
					strTemp.Format(_T("%s"), DefAlignMaskHead::pStrName[DefAlignMaskHead::enumSubEnd]);
					vecSaveData.push_back(strTemp);
				}

			}//End Sub

			strTemp.Format(_T("%s"), DefAlignMaskHead::pStrName[DefAlignMaskHead::enumPatternEnd]);
			vecSaveData.push_back(strTemp);

			strTemp.Format(_T("\n"));
			vecSaveData.push_back(strTemp);

		}//End for Align

		//Write File
		CStdioFile cStdFile;
		if (!cStdFile.Open(strPathName.GetBuffer(0), CFile::modeWrite | CFile::modeCreate | CFile::typeText))
			return RESULT_BAD;

		int nDataCount = static_cast<int>(vecSaveData.size());
		for (int i = 0; i < nDataCount; i++)
		{
			cStdFile.WriteString(vecSaveData[i] + _T("\n"));
		}

		cStdFile.Close();

	}//End Align


	return RESULT_GOOD;
}

void CStepRepeat::Serialize(CArchive& ar)
{
	CObject::Serialize(ar); 

	if (ar.IsStoring())
	{
		ar.Write(&m_bFlip, sizeof(m_bFlip));
		ar.Write(&m_bMirror, sizeof(m_bMirror));
		ar << m_nNX;
		ar << m_nNY;
		ar << m_dX;
		ar << m_dY;
		ar << m_dDX;
		ar << m_dDY;
		ar << m_dAngle;
		ar.Write(&m_FeatureMinMax, sizeof(m_FeatureMinMax));
		ar.Write(&m_ProfileMinMax, sizeof(m_ProfileMinMax));
		ar << m_strName;	
		ar.Write(&m_mirDir, sizeof(m_mirDir));
	}
	else
	{
		ar.Read(&m_bFlip, sizeof(m_bFlip));
		ar.Read(&m_bMirror, sizeof(m_bMirror));
		ar >> m_nNX;
		ar >> m_nNY;
		ar >> m_dX;
		ar >> m_dY;
		ar >> m_dDX;
		ar >> m_dDY;
		ar >> m_dAngle;
		ar.Read(&m_FeatureMinMax, sizeof(m_FeatureMinMax));
		ar.Read(&m_ProfileMinMax, sizeof(m_ProfileMinMax));
		ar >> m_strName;		
		ar.Read(&m_mirDir, sizeof(m_mirDir));
	}
}

void CProfile::Clear() 
{
	if (m_pProfileLayer)
	{
		m_pProfileLayer->m_FeatureFile.ClearAll();
		delete m_pProfileLayer;
		m_pProfileLayer = NULL;
	}
}

void CProfile::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);

	if (ar.IsStoring())
	{
		ar.Write(&m_MinMax, sizeof(m_MinMax));
	}
	else
	{
		ar.Read(&m_MinMax, sizeof(m_MinMax));
	}
}

UINT32 CProfile::LoadProfile(CString strFullPathName)
{
	CFile profileFile; CStdioFile cStdFile;
	CFileFind filefinder; int nTmp;
	CString strTmp, strTmp2, strReFileName;
	double dX, dY;
	bool bIsMMUnit = false;
	_TCHAR bom = (_TCHAR)0xFEFF;

	m_MinMax.top = -100000; m_MinMax.bottom = 100000;
	m_MinMax.left = 100000; m_MinMax.right = -100000;

	if (!cStdFile.Open(strFullPathName.GetBuffer(0), CFile::modeRead | CFile::typeText)) 
		return RESULT_BAD;
	//	CArchive ar(&cStdFile, CArchive::load);	

	while (cStdFile.ReadString(strTmp))
	{
		strTmp.TrimLeft(L"\t ");
		strTmp.TrimRight(L"\t ");
		strTmp.MakeUpper();
		if (strTmp.GetLength() == 0) continue;

		if (strTmp.Find(L"U MM") != -1)
		{
			bIsMMUnit = true;
			continue;
		}

		if (strTmp.Find(L"OB") != -1)  // OB 인 경우 
		{
			if ((nTmp = strTmp.Find(' ')) != -1) // OB 문자 제거
				strTmp.Delete(0, nTmp + 1);

			if ((nTmp = strTmp.Find(' ')) != -1)  // x 값 축출
			{
				strTmp2 = strTmp.Left(nTmp);
				if (bIsMMUnit)
					dX = atof((CStringA)strTmp2.GetBuffer(0));
				else
					dX = CJobFile::InchToMM(atof((CStringA)strTmp2.GetBuffer(0)));
				m_MinMax.left = min(m_MinMax.left, dX);
				m_MinMax.right = max(m_MinMax.right, dX);
				strTmp.Delete(0, nTmp + 1);
			}

			if ((nTmp = strTmp.Find(' ')) != -1)  // Y 값 축출
			{
				strTmp2 = strTmp.Left(nTmp);
				
				dY = bIsMMUnit ? atof((CStringA)strTmp2.GetBuffer(0)) :
					CJobFile::InchToMM(atof((CStringA)strTmp2.GetBuffer(0)));

				m_MinMax.top = max(m_MinMax.top, dY);
				m_MinMax.bottom = min(m_MinMax.bottom, dY);
				strTmp.Delete(0, nTmp + 1);
			}
		}
		else if (strTmp.Find(L"OS") != -1)  // OS 인 경우 
		{
			if ((nTmp = strTmp.Find(' ')) != -1) // OS 문자 제거
				strTmp.Delete(0, nTmp + 1);

			if ((nTmp = strTmp.Find(' ')) != -1)  // x 값 축출
			{
				strTmp2 = strTmp.Left(nTmp);
				
				dX = bIsMMUnit ? atof((CStringA)strTmp2.GetBuffer(0)) :
					CJobFile::InchToMM(atof((CStringA)strTmp2.GetBuffer(0)));

				m_MinMax.left = min(m_MinMax.left, dX);
				m_MinMax.right = max(m_MinMax.right, dX);
				strTmp.Delete(0, nTmp + 1);
			}

			if (strTmp.GetLength())  // y 값 축출
			{
				dY = bIsMMUnit ? atof((CStringA)strTmp.GetBuffer(0)) :
					CJobFile::InchToMM(atof((CStringA)strTmp.GetBuffer(0)));

				m_MinMax.top = max(m_MinMax.top, dY);
				m_MinMax.bottom = min(m_MinMax.bottom, dY);
			}
		}
	}
	//	ar.Close();
	cStdFile.Close();
	return RESULT_GOOD;
}

UINT32 CProfile::LoadProfileLayer(CString strFullPathName)
{
	Clear();

	m_pProfileLayer = new CLayer;

	if (m_pProfileLayer)
	{
		m_pProfileLayer->m_strLayerName = L"profile";

		m_pProfileLayer->m_FeatureFile.LoadFeatureFile(strFullPathName, g_pJobFile);
		m_pProfileLayer->m_bVisible = TRUE;

		m_pProfileLayer->m_fXScale = 1.0;
		m_pProfileLayer->m_fYScale = 1.0;
		m_pProfileLayer->m_nID = -1;
		return RESULT_GOOD;
	}
	else 
		return RESULT_BAD;
}

void CStephdr::ClearStepRepeats()
{
	for (UINT32 i = 0; i < m_arrStepRepeat.GetSize(); i++)
		delete m_arrStepRepeat[i];

	m_arrStepRepeat.RemoveAll();
}

void CStephdr::Serialize(CArchive& ar)
{
	CObject::Serialize(ar); 

	if (ar.IsStoring())
	{
		ar.Write(&m_bStepRepeat, sizeof(m_bStepRepeat));
		ar << m_dXDatum;
		ar << m_dYDatum;
		ar << m_dXOrigin;
		ar << m_dYOrigin;
		ar << m_dTopActive;
		ar << m_dBottomActive;
		ar << m_dRightActive;
		ar << m_dLeftActive;

		ar << m_arrStepRepeat.GetSize();
		for (UINT32 i = 0; i < m_arrStepRepeat.GetSize(); i++)
		{
			CStepRepeat* pStepRepeatTemp = (CStepRepeat*)m_arrStepRepeat.GetAt(i);
			pStepRepeatTemp->Serialize(ar);
		}
	}
	else
	{
		ar.Read(&m_bStepRepeat, sizeof(m_bStepRepeat));
		ar >> m_dXDatum;
		ar >> m_dYDatum;
		ar >> m_dXOrigin;
		ar >> m_dYOrigin;
		ar >> m_dTopActive;
		ar >> m_dBottomActive;
		ar >> m_dRightActive;
		ar >> m_dLeftActive;

		UINT32 nCount = 0;
		ar >> nCount;
		for (UINT32 i = 0; i < nCount; i++)
		{
			CStepRepeat* pStepRepeatTemp = new CStepRepeat();
			pStepRepeatTemp->Serialize(ar);
			m_arrStepRepeat.Add(pStepRepeatTemp);
		}			
	}
}

UINT32 CStephdr::LoadStephdr(CString strFullPathName)
{
	if (m_arrStepRepeat.GetSize()) ClearStepRepeats();
	CFile StephdrFile; CStdioFile cStdFile;
	CFileFind filefinder; int nTmp;
	CString strTmp, strTmp2, strReFileName;
	CStepRepeat* pStepRepeatTmp;
	_TCHAR bom = (_TCHAR)0xFEFF;

	if (!cStdFile.Open(strFullPathName.GetBuffer(0), CFile::modeRead | CFile::typeText)) 
		return RESULT_BAD;
	//	CArchive ar(&cStdFile, CArchive::load);	

	while (cStdFile.ReadString(strTmp))
	{
		strTmp.TrimLeft(L"\t ");  // 빈줄 ,tab trim
		strTmp.TrimRight(L"\t ");
		strTmp.MakeUpper();
		if (strTmp.GetLength() == 0) continue;

		if (strTmp.Find(L"STEP-REPEAT") != -1)
		{
			pStepRepeatTmp = new CStepRepeat;
			m_bStepRepeat = true;

			while (cStdFile.ReadString(strTmp))
			{
				strTmp.TrimLeft(L"\t ");
				strTmp.TrimRight(L"\t ");
				strTmp.MakeUpper();

				if (strTmp.GetLength() == 0) continue;
				if (strTmp.Find('}') != -1) break;

				nTmp = strTmp.Find('=');
				if (nTmp != -1)
				{
					if (strTmp.Find(L"NAME") != -1)
						pStepRepeatTmp->m_strName = strTmp.Right(strTmp.GetLength() - nTmp - 1);
					else if (strTmp.Find(L"DX") != -1)
						pStepRepeatTmp->m_dDX = CJobFile::InchToMM(atof((CStringA)strTmp.Right(strTmp.GetLength() - nTmp - 1)));
					else if (strTmp.Find(L"DY") != -1)
						pStepRepeatTmp->m_dDY = CJobFile::InchToMM(atof((CStringA)strTmp.Right(strTmp.GetLength() - nTmp - 1)));
					else if (strTmp.Find(L"NX") != -1)
						pStepRepeatTmp->m_nNX = atoi((CStringA)strTmp.Right(strTmp.GetLength() - nTmp - 1));
					else if (strTmp.Find(L"NY") != -1)
						pStepRepeatTmp->m_nNY = atoi((CStringA)strTmp.Right(strTmp.GetLength() - nTmp - 1));
					//else if( strTmp.Find("X") != -1 )				
					else if (strTmp.GetAt(0) == 'X')
						pStepRepeatTmp->m_dX = CJobFile::InchToMM(atof((CStringA)strTmp.Right(strTmp.GetLength() - nTmp - 1)));
					//else if( strTmp.Find("Y") != -1 )				
					else if (strTmp.GetAt(0) == 'Y')
						pStepRepeatTmp->m_dY = CJobFile::InchToMM(atof((CStringA)strTmp.Right(strTmp.GetLength() - nTmp - 1)));
					else if (strTmp.Find(L"ANGLE") != -1)
						pStepRepeatTmp->m_dAngle = atof((CStringA)strTmp.Right(strTmp.GetLength() - nTmp - 1));
					else if (strTmp.Find(L"FLIP") != -1)
					{
						if (strTmp.Find(L"YES") != -1)
						{
							pStepRepeatTmp->m_bFlip = true;
							pStepRepeatTmp->m_mirDir = eDirectionType::eVertical;
						}
						else
							pStepRepeatTmp->m_bFlip = false;
					}
					else if (strTmp.Find(L"MIRROR") != -1)
					{
						if (strTmp.Find(L"YES") != -1)
						{
							pStepRepeatTmp->m_bMirror = true;
							pStepRepeatTmp->m_mirDir = eDirectionType::eHorizontal;
						}
						else
							pStepRepeatTmp->m_bMirror = false;
					}
					else
						OutputDebugString(strTmp + L" : It is a new member of STEP-REPEAT or a error in Stephdr File\n");
				}
			}
			m_arrStepRepeat.Add(pStepRepeatTmp);
		}
		else if (strTmp.Find(L"X_DATUM") != -1)
		{
			nTmp = strTmp.Find('=');
			if (nTmp != -1)
				m_dXDatum = CJobFile::InchToMM(atof((CStringA)strTmp.Right(strTmp.GetLength() - nTmp - 1)));
		}
		else if (strTmp.Find(L"Y_DATUM") != -1)
		{
			nTmp = strTmp.Find('=');
			if (nTmp != -1)
				m_dYDatum = CJobFile::InchToMM(atof((CStringA)strTmp.Right(strTmp.GetLength() - nTmp - 1)));
		}
		else if (strTmp.Find(L"X_ORIGIN") != -1)
		{
			nTmp = strTmp.Find('=');
			if (nTmp != -1)
				m_dXOrigin = CJobFile::InchToMM(atof((CStringA)strTmp.Right(strTmp.GetLength() - nTmp - 1)));
		}
		else if (strTmp.Find(L"Y_ORIGIN") != -1)
		{
			nTmp = strTmp.Find('=');
			if (nTmp != -1)
				m_dYOrigin = CJobFile::InchToMM(atof((CStringA)strTmp.Right(strTmp.GetLength() - nTmp - 1)));
		}
		else if (strTmp.Find(L"TOP_ACTIVE") != -1)
		{
			nTmp = strTmp.Find('=');
			if (nTmp != -1)
				m_dTopActive = CJobFile::InchToMM(atof((CStringA)strTmp.Right(strTmp.GetLength() - nTmp - 1)));
		}
		else if (strTmp.Find(L"BOTTOM_ACTIVE") != -1)
		{
			nTmp = strTmp.Find('=');
			if (nTmp != -1)
				m_dBottomActive = CJobFile::InchToMM(atof((CStringA)strTmp.Right(strTmp.GetLength() - nTmp - 1)));
		}
		else if (strTmp.Find(L"RIGHT_ACTIVE") != -1)
		{
			nTmp = strTmp.Find('=');
			if (nTmp != -1)
				m_dRightActive = CJobFile::InchToMM(atof((CStringA)strTmp.Right(strTmp.GetLength() - nTmp - 1)));
		}
		else if (strTmp.Find(L"LEFT_ACTIVE") != -1)
		{
			nTmp = strTmp.Find('=');
			if (nTmp != -1)
				m_dLeftActive = CJobFile::InchToMM(atof((CStringA)strTmp.Right(strTmp.GetLength() - nTmp - 1)));
		}		
		else
			OutputDebugString(strTmp + L" : It is a new member or a error in Stephdr File\n");
	}

	//	ar.Close();
	cStdFile.Close();
	return RESULT_GOOD;
}

void CSubStep::ClearSubSteps()
{
	m_arrNextStep.RemoveAll();
	return;

	for (UINT32 i = 0; i < m_arrNextStep.GetSize(); i++)
	{
		if (m_arrNextStep[i])
		{
			m_arrNextStep[i]->ClearSubSteps();
			delete m_arrNextStep[i];
		}
		m_arrNextStep[i] = nullptr;
	}

	m_arrNextStep.RemoveAll();
}

void CStep::ClearLayers()
{
	for (int i = 0; i < m_arrLayer.GetSize(); i++)
	{
		m_arrLayer[i]->ClearLayer();
		delete m_arrLayer[i];
		m_arrLayer[i] = nullptr;
	}

	m_arrLayer.RemoveAll();
}

void CStep::ClearPackages()
{
	for (int i = 0; i < m_arrPackage.GetSize(); i++)
	{
		m_arrPackage[i]->ClearPackage();
		m_arrPackage[i]->ClearPins();
		delete m_arrPackage[i];
		m_arrPackage[i] = nullptr;
	}

	m_arrPackage.RemoveAll();
}

void CStep::ClearSubSteps()
{
	for (UINT32 i = 0; i < m_arrSubStep.GetSize(); i++)
	{
		if (m_arrSubStep[i])
		{
			m_arrSubStep[i]->ClearSubSteps();
			delete m_arrSubStep[i];
		}
		m_arrSubStep[i] = nullptr;
	}

	m_arrSubStep.RemoveAll();	
}

void CStep::Serialize(CArchive& ar)
{
	CObject::Serialize(ar); 

	if (ar.IsStoring())
	{ 
		ar << m_nStepRepeatType;
		ar << m_nStepID;
		ar << m_nSubStepCnt;
		ar.Write(&m_FeatureMinMax, sizeof(m_FeatureMinMax));
		ar.Write(&m_ActiveRect, sizeof(m_ActiveRect));
		ar << m_strStepName;

		m_Stephdr.Serialize(ar);
		m_Profile.Serialize(ar);

		// Layer Array 저장 
		for (UINT32 i = 0; i < m_arrLayer.GetSize(); i++)
		{
			CLayer* pLayerTemp = m_arrLayer.GetAt(i);
			pLayerTemp->Serialize(ar);
		}
	}
	else
	{
		ar >> m_nStepRepeatType;
		ar >> m_nStepID;
		ar >> m_nSubStepCnt;
		ar.Read(&m_FeatureMinMax, sizeof(m_FeatureMinMax));
		ar.Read(&m_ActiveRect, sizeof(m_ActiveRect));
		ar >> m_strStepName;

		m_Stephdr.Serialize(ar);
		m_Profile.Serialize(ar);

		for (UINT32 i = 0; i < m_arrLayer.GetSize(); i++)
		{
			CLayer* pLayerTemp = new CLayer();
			pLayerTemp->Serialize(ar);
			m_arrLayer.Add(pLayerTemp);
		}
	}
}

UINT32 CStep::LoadStep(CString strPathName)
{
	UINT32 bRst = RESULT_BAD;

	CString strFileName; 
	strFileName.Format(L"%s\\steps\\%s\\stephdr", strPathName, m_strStepName);
	bRst = m_Stephdr.LoadStephdr(strFileName);

	strFileName.Format(L"%s\\steps\\%s\\profile", strPathName, m_strStepName);
	bRst |= m_Profile.LoadProfile(strFileName);
	bRst |= m_Profile.LoadProfileLayer(strFileName);

	return bRst;
}

UINT32 CStep::LoadPackage(CString strPathName)
{
	if (m_arrPackage.GetSize())
		ClearPackages();

	CFileFind finder;
	CString strFileName;
	strFileName.Format(L"%s\\steps\\%s\\eda\\data", strPathName, m_strStepName);

	if (!finder.FindFile(strFileName))
		return RESULT_GOOD;

	CStdioFile cStdFile;
	CString strTmp, strToken;

	if (!cStdFile.Open(strFileName.GetBuffer(0), CFile::modeRead | CFile::typeText))
		return RESULT_BAD;

	ULONGLONG lFilePos = -1;
	int nTmp = -1;

	while (cStdFile.ReadString(strTmp))
	{
		strTmp.TrimLeft(L"\t ");
		strTmp.TrimRight(L"\t ");
		if (strTmp.GetLength() == 0 || strTmp.Find(L"PKG") < 0)
		{
			lFilePos = cStdFile.GetPosition();
			continue;
		}

		break;
	}

	if (lFilePos < 0) return RESULT_GOOD;

	cStdFile.Seek(lFilePos, CStdioFile::begin);

	CPackage* pPkgTmp = NULL;
	CPin* pPinTmp = NULL;
	int nCmpNum = 0;

	while (cStdFile.ReadString(strTmp))
	{
		if ((nTmp = strTmp.Find(';')) != -1)
			strTmp = strTmp.Left(nTmp);

		int iPos = 0;
		AfxExtractSubString(strToken, strTmp, iPos++, ' ');

		if (strToken.Compare(L"PKG") == 0)
		{
			pPkgTmp = new CPackage;

			AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//name
			pPkgTmp->m_strPkgName = strToken;
			AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//pitch
			pPkgTmp->m_fPitch = atof((CStringA)strToken.GetBuffer(0));
			AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//xmin
			pPkgTmp->m_MinMaxRect.left = atof((CStringA)strToken.GetBuffer(0));
			AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//ymin
			pPkgTmp->m_MinMaxRect.bottom = atof((CStringA)strToken.GetBuffer(0));
			AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//xmax
			pPkgTmp->m_MinMaxRect.right = atof((CStringA)strToken.GetBuffer(0));
			AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//ymax		
			pPkgTmp->m_MinMaxRect.top = atof((CStringA)strToken.GetBuffer(0));

			cStdFile.ReadString(strTmp);
			AfxExtractSubString(strToken, strTmp, 0, ' ');

			if (strToken.Compare(L"RC") == 0)
				pPkgTmp->m_pRecord = new CRecordRC;
			else if (strToken.CompareNoCase(L"CR") == 0)
				pPkgTmp->m_pRecord = new CRecordCR;
			else if (strToken.CompareNoCase(L"SQ") == 0)
				pPkgTmp->m_pRecord = new CRecordSQ;
			else if (strToken.CompareNoCase(L"CT") == 0)
				pPkgTmp->m_pRecord = new CRecordCT;

			if (pPkgTmp->m_pRecord)
			{
				pPkgTmp->m_pRecord->LoadRecord(strTmp, cStdFile);
				pPkgTmp->m_pRecord->SetMinMaxRect();
			}

			lFilePos = cStdFile.GetPosition();

			int nPinNum = 0;
			while (cStdFile.ReadString(strTmp))
			{
				iPos = 0;
				AfxExtractSubString(strToken, strTmp, iPos++, ' ');
				if (strToken.Compare(L"PIN") != 0)
				{
					cStdFile.Seek(lFilePos, CStdioFile::begin);
					break;
				}

				pPinTmp = new CPin;

				AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//name
				pPinTmp->m_strPinName = strToken;
				AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//type
				if (strToken.CompareNoCase(L"T") == 0)		pPinTmp->m_ePinType = PinType::PT;
				else if (strToken.CompareNoCase(L"B") == 0)	pPinTmp->m_ePinType = PinType::PB;
				else if (strToken.CompareNoCase(L"S") == 0)	pPinTmp->m_ePinType = PinType::PS;

				AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//cx
				pPinTmp->m_cx = atof((CStringA)strToken.GetBuffer(0));
				AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//cy
				pPinTmp->m_cy = atof((CStringA)strToken.GetBuffer(0));
				AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//fhs
				pPinTmp->m_fhs = atof((CStringA)strToken.GetBuffer(0));

				AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//etype		
				if (strToken.CompareNoCase(L"U") == 0)		pPinTmp->m_eElecType = ElecType::PEU;
				else if (strToken.CompareNoCase(L"E") == 0)	pPinTmp->m_eElecType = ElecType::PEE;
				else if (strToken.CompareNoCase(L"M") == 0)	pPinTmp->m_eElecType = ElecType::PEM;

				AfxExtractSubString(strToken, strTmp, iPos++, ' ');	//mtype		
				if (strToken.CompareNoCase(L"U") == 0)		pPinTmp->m_eMountType = MountType::PMU;
				else if (strToken.CompareNoCase(L"S") == 0)	pPinTmp->m_eMountType = MountType::SMT;
				else if (strToken.CompareNoCase(L"D") == 0)	pPinTmp->m_eMountType = MountType::RSMT;
				else if (strToken.CompareNoCase(L"T") == 0)	pPinTmp->m_eMountType = MountType::MThrough;
				else if (strToken.CompareNoCase(L"R") == 0)	pPinTmp->m_eMountType = MountType::RMThrough;
				else if (strToken.CompareNoCase(L"P") == 0)  pPinTmp->m_eMountType = MountType::Pressfit;
				else if (strToken.CompareNoCase(L"N") == 0)	pPinTmp->m_eMountType = MountType::Nonboard;
				else if (strToken.CompareNoCase(L"H") == 0)	pPinTmp->m_eMountType = MountType::MHole;

				cStdFile.ReadString(strTmp);
				AfxExtractSubString(strToken, strTmp, 0, ' ');

				if (strToken.Compare(L"RC") == 0)
					pPinTmp->m_pRecord = new CRecordRC;
				else if (strToken.CompareNoCase(L"CR") == 0)
					pPinTmp->m_pRecord = new CRecordCR;
				else if (strToken.CompareNoCase(L"SQ") == 0)
					pPinTmp->m_pRecord = new CRecordSQ;
				else if (strToken.CompareNoCase(L"CT") == 0)
					pPinTmp->m_pRecord = new CRecordCT;

				if (pPinTmp->m_pRecord)
				{
					pPinTmp->m_pRecord->LoadRecord(strTmp, cStdFile);
					pPinTmp->m_pRecord->SetMinMaxRect();
				}

				lFilePos = cStdFile.GetPosition();

				pPkgTmp->m_arrPin.Add(pPinTmp);
				nPinNum++;
			}

			m_arrPackage.Add(pPkgTmp);
			nCmpNum++;

			strTmp.Format(L"\n[%d] Component = [%d] Pin Num\n", nCmpNum, nPinNum);
			OutputDebugString(strTmp);
		}
	}

	cStdFile.Close();
	strTmp.Format(L"%s\n : %d Component Num\n", strFileName, nCmpNum);
	OutputDebugString(strTmp);
	return RESULT_GOOD;
}

CJobFile::CJobFile()
	: m_bLoaded(false), m_bMatrixNStepLoaded(false), m_eUserDefinedOrient(Orient::OrientNone), 
	m_strFullPathName(L"")
{
	g_pJobFile = this;
	InitializeCriticalSection(&m_CS);
}

UINT32 CJobFile::LoadMatrix()
{
	if (RESULT_GOOD == m_Matrix.LoadMatrixFile(m_strFullPathName + MATRIX_PATH))
	{
		bool bFind = false;
		CString strLayer = L"", strLayerIdx = L"";
		int nStartLayerID = 99999, nEndLayerID = -99999;
		for (UINT32 iLoop = 0; iLoop < m_Matrix.m_arrMatrixLayer.GetSize(); iLoop++)
		{
			CMatrixLayer* pMatrixLayerTmp = m_Matrix.m_arrMatrixLayer[iLoop];

			if (pMatrixLayerTmp->m_strContext.CompareNoCase(L"BOARD") != 0 ||
				(pMatrixLayerTmp->m_strType.CompareNoCase(L"SIGNAL") != 0))
				continue;

			strLayer = pMatrixLayerTmp->m_strName;

			//하이픈 제거
			int iPos = strLayer.Find('-');
			int iLength = strLayer.GetLength();
			if (iPos >= 0)
				strLayer.Delete(iPos, iLength - iPos);

			for (int ichar = 0; ichar < strLayer.GetLength(); ichar++)
			{
				if (isdigit(strLayer.GetAt(ichar)))
					strLayerIdx.Insert(strLayerIdx.GetLength(), strLayer.GetAt(ichar));
			}

			int nLayerID = _ttoi(strLayerIdx);
			if (nLayerID > nEndLayerID) nEndLayerID = nLayerID;
			if (nLayerID < nStartLayerID) nStartLayerID = nLayerID;

			bFind = true;
			strLayerIdx = L"";
		}

		if (bFind)
		{
			m_Matrix.m_iStartLayerIdx = nStartLayerID;
			m_Matrix.m_iEndLayerIdx = nEndLayerID;
			return RESULT_GOOD;
		}
		else
		{
			m_Matrix.m_iStartLayerIdx = -1;
			m_Matrix.m_iEndLayerIdx = -1;
			return RESULT_BAD;
		}
	}
	else
	{
		m_Matrix.m_iStartLayerIdx = -1;
		m_Matrix.m_iEndLayerIdx = -1;
		return RESULT_BAD;
	}
}

UINT32 CJobFile::LoadMatrix_Sem(SystemSpec_t* pSystemSpec)
{
	if (RESULT_GOOD == m_Matrix.LoadMatrixFile(m_strFullPathName + MATRIX_PATH))
	{
		bool bFind = false;
		CString strLayer = L"", strLayerIdx = L"", strLayerIdx_old = L"";
		CString strLayerName = L"";
		int iPos = 0, iTempCount = 30;
		int nStartLayerID = 99999, nEndLayerID = -99999;
		vector<BOOL> vecPatternCheck;

		vector<CString> vecSliceFilter;
		int iLength = -1;
		bool bTempCheck = false;

		for (UINT32 iLoop = 0; iLoop < m_Matrix.m_arrMatrixLayer.GetSize(); iLoop++)
		{
			CMatrixLayer* pMatrixLayerTmp = m_Matrix.m_arrMatrixLayer[iLoop];

			if (pMatrixLayerTmp->m_strContext.CompareNoCase(L"BOARD") != 0 ||
				(pMatrixLayerTmp->m_strType.CompareNoCase(L"SIGNAL") != 0))
				continue;

			strLayer = pMatrixLayerTmp->m_strName;						

			bFind = false;

			for (auto it : pSystemSpec->sysLayer.vcLayer)
			{
				for (auto itNames : it->vcFrontLayerNames)
				{
					strLayerName = itNames;					
					strLayerName.MakeUpper();					

					CUtils::SliceString(strLayerName, _T("[S]"), vecSliceFilter);
					
					int nFilterSize = static_cast<int>(vecSliceFilter.size());

					for (int i = 0; i < nFilterSize; i++)
					{
						
						if (nFilterSize == 1)
						{
							CString strTemp = vecSliceFilter[i];						
							int iFind = strLayer.Find(strTemp);
							
							if (iFind != -1 )
							{
								strLayer.Delete(0, iFind + 1);
								bTempCheck = true;
							}						
						}
						else if (nFilterSize > 1)
						{
							CString strTemp = vecSliceFilter[i];
							int iFind = strLayer.Find(strTemp);


							if (iFind != -1)
							{
								strLayer.Delete(0, iFind + 1);

								CString strTemp2 = vecSliceFilter[i+1];
								int iFind2 = strLayer.Find(strTemp2);

								strLayer.Delete(iFind2, strLayer.GetLength());
								bTempCheck = true;
							}

						}

						if (bTempCheck) break;
					}

					if (bTempCheck)
					{
						iLength = strLayer.GetLength();
						for (int i = 0; i < iLength; i++)
						{
							if (isdigit(strLayer.GetAt(i)))
							{
								strLayerIdx.Insert(strLayerIdx.GetLength(), strLayer.GetAt(i));
							}					
						}

						int nLayerID = _ttoi(strLayerIdx);
						if (nLayerID > nEndLayerID) nEndLayerID = nLayerID;
						if (nLayerID < nStartLayerID) nStartLayerID = nLayerID;

						bFind = true;
						strLayerIdx = L"";		
						bTempCheck = false;
					}
				}					

			}


			/*if (strLayer.Find(_T("-uvdi")) >= 0 ||
				strLayer.Find(_T("-UVDI")) >= 0)
			{//임시조치 //Sem일 경우 -uvdi만 파싱한다.,
				int iPos = strLayer.Find('-');
				int iLength = strLayer.GetLength();
				if (iPos >= 0)
					strLayer.Delete(iPos, iLength - iPos);

				for (int ichar = 0; ichar < strLayer.GetLength(); ichar++)
				{
					if (isdigit(strLayer.GetAt(ichar)))
						strLayerIdx.Insert(strLayerIdx.GetLength(), strLayer.GetAt(ichar));
				}

				int nLayerID = _ttoi(strLayerIdx);
				if (nLayerID > nEndLayerID) nEndLayerID = nLayerID;
				if (nLayerID < nStartLayerID) nStartLayerID = nLayerID;
			}

			bFind = true;
			strLayerIdx = L"";*/
		}

		//MP Layer 존재 유뮤 확인
		CheckLayer_MpTp();		

		if (bFind)
		{
			m_Matrix.m_iStartLayerIdx = nStartLayerID;
			m_Matrix.m_iEndLayerIdx = nEndLayerID;
			return RESULT_GOOD;
		}
		else
		{
			m_Matrix.m_iStartLayerIdx = -1;
			m_Matrix.m_iEndLayerIdx = -1;
			return RESULT_BAD;
		}
	}
	else
	{
		m_Matrix.m_iStartLayerIdx = -1;
		m_Matrix.m_iEndLayerIdx = -1;
		return RESULT_BAD;
	}
}

UINT32 CJobFile::LoadFont()
{
	ClearFonts();

	CFileFind	finder;
	CString		strFontPath, strFontFile;

	CString strFontRootPath = m_strFullPathName + FONT_PATH;
	BOOL bContinue = finder.FindFile(strFontRootPath + L"*.*");

	while (bContinue)
	{
		bContinue = finder.FindNextFile();
		if (finder.IsDots() || finder.IsDirectory())
			continue;

		strFontPath = finder.GetFilePath();
		strFontFile = finder.GetFileName();

		CStandardFont* pFont = new CStandardFont;
		if (pFont == nullptr)
			continue;

		if (pFont->LoadStandardFile(strFontPath) == RESULT_GOOD)
		{
			pFont->m_strFontName = strFontFile;
			m_arrFont.Add(pFont);
		}
		else
		{
			delete pFont;
			pFont = nullptr;
		}		
	}
	finder.Close();

	if (m_arrFont.GetSize() <= 0) return RESULT_BAD;
	return RESULT_GOOD;
}

UINT32 CJobFile::LoadUserSymbols()
{
	ClearUserSymbols();

	CFileFind	finder;
	CString		strDirName = L"", strTmp = L"";

	CString strPath = m_strFullPathName + SYMBOL_PATH;

	if (strPath.Right(1) != L"\\")
		strPath += L"\\";
	strPath += L"*";

	BOOL bContinue = finder.FindFile(strPath);
	while (bContinue)
	{
		bContinue = finder.FindNextFile();

		if (finder.IsDots())
			continue;

		if (finder.IsDirectory())
		{
			strDirName = finder.GetFileName();

			//if (strDirName.Find(_T("2dcode")) >= 0 || strDirName.Find(_T("barcode")) >= 0)
			if (strDirName.Find(_T("pkg_dummy")) >= 0)					
				continue;

			{
				strTmp = strDirName;

				strTmp.TrimLeft(_T("\t "));
				strTmp.TrimRight(_T("\t "));
				strTmp.MakeUpper();

				CUserSymbol* pTmpUserSymbol = new CUserSymbol;
				pTmpUserSymbol->m_strSymbolName = strDirName;

				//load attrlist

				strTmp = m_strFullPathName + SYMBOL_PATH + strDirName + _T("\\features");
				pTmpUserSymbol->m_FeatureFile.LoadFeatureFile(strTmp, this);

				m_arrSymbol.Add(pTmpUserSymbol);
			}
		}
	}
	finder.Close();

	for (UINT32 i = 0; i < m_arrSymbol.GetSize(); i++)
	{
		CUserSymbol* pUserSymbol = m_arrSymbol[i];

		if (!pUserSymbol)
			continue;

		for (UINT32 j = 0; j < pUserSymbol->m_FeatureFile.m_arrSymbol.GetSize(); j++)
		{
			bool bFind = false;
			CSymbol* pSymbol = pUserSymbol->m_FeatureFile.m_arrSymbol[j];

			if (pSymbol->m_pUserSymbol || pSymbol->m_strUserSymbol.GetLength() < 1)
				continue;

			for (int k = 0; k < m_arrSymbol.GetSize(); k++)
			{
				CUserSymbol* pRefSymbol = m_arrSymbol[k];

				if (pRefSymbol->m_strSymbolName.CompareNoCase(pSymbol->m_strUserSymbol.GetBuffer(0)) == 0)
				{
					pSymbol->m_pUserSymbol = pRefSymbol;
					bFind = true;
					break;
				}
			}

			if (bFind)
				break;
		}
	}

	return RESULT_GOOD;
}

UINT32 CJobFile::LoadMisc()
{
	//1. info
	if (RESULT_GOOD == m_Misc.LoadInfo(m_strFullPathName))
	{
		//2. attrlist
		return m_Misc.LoadAttrlist(m_strFullPathName);
	}
	else
		return RESULT_BAD;
}

UINT32 CJobFile::SaveMisc(IN CString &strPath)
{
	if (RESULT_GOOD == m_Misc.SaveInfo(strPath))
	{
		//2. attrlist
		return m_Misc.SaveAttrlist(strPath);
	}
	else
		return RESULT_BAD;
}

UINT32 CMisc::LoadInfo(IN const CString &strPath)
{
	m_strVersion = L"";

	CFileFind filefinder;
	CString strFileName = strPath + INFO_PATH;

	if (!filefinder.FindFile(strFileName))
		return RESULT_BAD;

	CStdioFile cStdFile;
	if (!cStdFile.Open(strFileName.GetBuffer(0), CFile::modeRead | CFile::typeText))
		return RESULT_BAD;

	UINT32 iMajor = 0, iMinor = 0;
	CString strTmp = L"", strToken = L"", strJobName = L"", strName = L"JOB_NAME";
	CString strMajor = L"ODB_VERSION_MAJOR", strMinor = L"ODB_VERSION_MINOR";
	while (cStdFile.ReadString(strTmp))
	{
		strTmp.TrimLeft(_T("\t "));
		strTmp.TrimRight(_T("\t "));
		strTmp.MakeUpper();
		if (strTmp.GetLength() == 0) continue;	// 빈줄 Skip

		if (strTmp.Find(strName) >= 0)
		{
			AfxExtractSubString(strToken, strTmp, 1, '=');
			strJobName = strToken;
			continue;
		}

		if (strTmp.Find(strMajor) >= 0)
		{
			AfxExtractSubString(strToken, strTmp, 1, '=');
			iMajor = _ttoi(strToken);
			continue;
		}

		if (strTmp.Find(strMinor) >= 0)
		{
			AfxExtractSubString(strToken, strTmp, 1, '=');
			iMinor = _ttoi(strToken);
			continue;
		}
	}

	if (strJobName.CompareNoCase(m_strJobName) != 0)
	{
		strTmp.Format(L"ODB File Name and Job Name are Different : (%s, %s)\n", m_strJobName, strJobName);
		OutputDebugString(strTmp);
	}

	m_strVersion.Format(L"%d.%d", iMajor, iMinor);
	strTmp.Format(L"%s Version : %s\n", m_strJobName, m_strVersion);
	OutputDebugString(strTmp);

	cStdFile.Close();
	return RESULT_GOOD;
}


UINT32 CMisc::SaveInfo(IN const CString &strPath)
{
	CString strFileName = strPath + INFO_PATH;
	CStdioFile cStdFile;
	if (!cStdFile.Open(strFileName.GetBuffer(0), CFile::modeReadWrite | CFile::typeText))
		return RESULT_BAD;
	
	CString strTmp = L"", strToken = L"", strJobName = L"", strName = L"JOB_NAME";
	CString strMajor = L"ODB_VERSION_MAJOR", strMinor = L"ODB_VERSION_MINOR";

	CString strRead = _T("");
	while (cStdFile.ReadString(strTmp))
	{
		strRead += (strTmp + _T("\n"));
	}

	CString strText;

	//Model Name
	strText.Format(_T("%s=%s"), strName, m_strJobName);
	CUtils::ChangeString(strRead, strName, _T("\n"), strText);

	//Version
	vector<CString> vecVersion;
	CUtils::Split(m_strVersion, _T("."), vecVersion);
	
	
	if (CUtils::OutOfRange(vecVersion, 0) == FALSE)
	{
		strText.Format(_T("%s=%s"), strMajor, vecVersion[0]);
		CUtils::ChangeString(strRead, strMajor, _T("\n"), strText);
	}
	
	if (CUtils::OutOfRange(vecVersion, 1) == FALSE)
	{
		strText.Format(_T("%s=%s"), strMinor, vecVersion[1]);
		CUtils::ChangeString(strRead, strMinor, _T("\n"), strText);
	}

	cStdFile.Seek(0, CFile::begin);
	cStdFile.WriteString(strRead);

	cStdFile.Close();

	return RESULT_GOOD;
}

UINT32 CMisc::LoadAttrlist(IN const CString &strPath)
{
	//misc\\attrlist
	m_strPartNum = _T("");
	m_strLotWeek = _T("");
	m_strLotYear = _T("");
	m_strLotRev = _T("");
	m_strModel = _T("");
	m_dThickness = 0;

	CFileFind filefinder;
	CString strFileName = strPath + ATTRLIST_PATH;

	if (!filefinder.FindFile(strFileName))
		return RESULT_BAD;

	CStdioFile cStdFile;

	CString strTmp = L"", strThickness = L".BOARD_THICKNESS";
	CString	strToken = L"", strPart_number = L"PART_NUMBER";

	CString strLot_week = L"LOT_WEEK";
	CString strLot_year = L"LOT_YEAR";
	CString strLot_rev = L"LOT_REV";
	CString strModel = L"MODEL";

	if (!cStdFile.Open(strFileName.GetBuffer(0), CFile::modeRead | CFile::typeText))
		return RESULT_BAD;

	while (cStdFile.ReadString(strTmp))
	{
		strTmp.TrimLeft(_T("\t "));
		strTmp.TrimRight(_T("\t "));
		strTmp.MakeUpper();
		if (strTmp.GetLength() == 0) continue;	// 빈줄 Skip

		if (strTmp.Find(strThickness) >= 0)
		{
			AfxExtractSubString(strToken, strTmp, 1, '=');
			m_dThickness = _ttof(strToken) * 25.4;

			strTmp.Format(L"%s Thickness : %f\n", m_strJobName, m_dThickness);
			OutputDebugString(strTmp);
			continue;
		}

		if (strTmp.Find(strPart_number) >= 0)
		{
			AfxExtractSubString(strToken, strTmp, 1, '=');
			if (strTmp.GetLength() > 0)
			{
				strToken.TrimLeft(L"\t ");
				strToken.TrimRight(L"\t ");

				m_strPartNum.Format(L"%s", strToken);
				strTmp.Format(L"%s Part Number : %s\n", m_strJobName, strToken);
				OutputDebugString(strTmp);
			}
		}

		if (strTmp.Find(strLot_week) >= 0)
		{
			AfxExtractSubString(strToken, strTmp, 1, '=');
			if (strTmp.GetLength() > 0)
			{
				strToken.TrimLeft(L"\t ");
				strToken.TrimRight(L"\t ");

				m_strLotWeek.Format(L"%s", strToken);
				strTmp.Format(L"%s Lot Week : %s\n", m_strJobName, strToken);
				OutputDebugString(strTmp);
			}
		}

		if (strTmp.Find(strLot_year) >= 0)
		{
			AfxExtractSubString(strToken, strTmp, 1, '=');
			if (strTmp.GetLength() > 0)
			{
				strToken.TrimLeft(L"\t ");
				strToken.TrimRight(L"\t ");

				m_strLotYear.Format(L"%s", strToken);
				strTmp.Format(L"%s Lot Year : %s\n", m_strJobName, strToken);
				OutputDebugString(strTmp);
			}
		}

		if (strTmp.Find(strLot_rev) >= 0)
		{
			AfxExtractSubString(strToken, strTmp, 1, '=');
			if (strTmp.GetLength() > 0)
			{
				strToken.TrimLeft(_T("\t "));
				strToken.TrimRight(_T("\t "));

				m_strLotRev.Format(L"%s", strToken);
				strTmp.Format(L"%s Lot Rev : %s\n", m_strJobName, strToken);
				OutputDebugString(strTmp);
			}
		}

		if (strTmp.Find(strModel) >= 0)
		{
			AfxExtractSubString(strToken, strTmp, 1, '=');
			if (strTmp.GetLength() > 0)
			{
				strToken.TrimLeft(_T("\t "));
				strToken.TrimRight(_T("\t "));

				m_strModel.Format(L"%s", strToken);
				strTmp.Format(L"%s Model : %s\n", m_strJobName, strToken);
				OutputDebugString(strTmp);
			}
		}
	}

	cStdFile.Close();
	return RESULT_GOOD;
}

UINT32 CMisc::SaveAttrlist(IN const CString &strPath)
{
	return RESULT_GOOD;
}

UINT32 CJobFile::LoadPadUsage()
{
	//attribute = .pad_usage 인 경우, 별도 처리
	// 0 - toeprint, 1 - via, 2 - g_fiducial, 3 - l_fiducial, 4 - tooling_hole
	CAttributeTextString *pPadUsage = NULL;

	pPadUsage = new CAttributeTextString();
	pPadUsage->m_iSerialNum = static_cast<UINT32>(m_arrPadUsage.GetSize());
	pPadUsage->m_strText = L"toeprint";
	m_arrPadUsage.Add(pPadUsage);

	pPadUsage = new CAttributeTextString();
	pPadUsage->m_iSerialNum = static_cast<UINT32>(m_arrPadUsage.GetSize());
	pPadUsage->m_strText = L"via";
	m_arrPadUsage.Add(pPadUsage);

	pPadUsage = new CAttributeTextString();
	pPadUsage->m_iSerialNum = static_cast<UINT32>(m_arrPadUsage.GetSize());
	pPadUsage->m_strText = L"g_fiducial";
	m_arrPadUsage.Add(pPadUsage);

	pPadUsage = new CAttributeTextString();
	pPadUsage->m_iSerialNum = static_cast<UINT32>(m_arrPadUsage.GetSize());
	pPadUsage->m_strText = L"l_fiducial";
	m_arrPadUsage.Add(pPadUsage);

	pPadUsage = new CAttributeTextString();
	pPadUsage->m_iSerialNum = static_cast<UINT32>(m_arrPadUsage.GetSize());
	pPadUsage->m_strText = L"tooling_hole";
	m_arrPadUsage.Add(pPadUsage);

	return RESULT_GOOD;
}

UINT32 CJobFile::LoadLayer(UINT32 iStepIndex, CString strStepName, LayerSet_t* pLayerSet)
{
	if (pLayerSet == nullptr)
		return RESULT_BAD;

	CString strLayerPath = L"";
	strLayerPath.Format(L"%s\\steps\\%s\\layers\\%s\\features", m_strFullPathName, strStepName, pLayerSet->strLayer);

	CFileFind finder;
	if (!finder.FindFile(strLayerPath))
		return RESULT_GOOD;

	CStdioFile cStdFile;
	if (!cStdFile.Open(strLayerPath.GetBuffer(0), CFile::modeRead | CFile::typeText))
	{
		cStdFile.Close();
		return RESULT_BAD;
	}
	cStdFile.Close();

	CLayer* pLayer = new CLayer;
	pLayer->m_strLayerName = pLayerSet->strLayer;

	UINT bRst = pLayer->m_FeatureFile.LoadFeatureFile(strLayerPath, this);
	bRst |= pLayer->AssignDynamicStrings(strStepName, this);
	pLayer->m_nID = pLayerSet->index;

	//
	CString strPathName = L"";
	strPathName.Format(L"%s\\steps\\%s\\layers\\%s\\Align", m_strFullPathName, strStepName, pLayerSet->strLayer);
	pLayer->LoadAlignMask(strPathName, MaskType::enumType_Align);
	strPathName.Format(L"%s\\steps\\%s\\layers\\%s\\Mask", m_strFullPathName, strStepName, pLayerSet->strLayer);
	pLayer->LoadAlignMask(strPathName, MaskType::enumType_Mask);
	//

	EnterCriticalSection(&m_CS);
	m_arrStep[iStepIndex]->m_arrLayer.Add(pLayer);
	LeaveCriticalSection(&m_CS);

	CString strTemp = L"";
	strTemp.Format(_T("\nLoad Layer Completed Thread %s Step, %s Layer\n"), strStepName, pLayerSet->strLayer);
	OutputDebugString(strTemp);

	return bRst;
}




UINT32 CJobFile::LoadStep()
{
	UINT32 bRst = RESULT_GOOD;

	if (m_Matrix.m_arrMatrixStep.GetSize() < 1) 
		return bRst;	

	CFileFind finder;
	CString strStepPath = L"", strStepName = L"", strLayerName = L"";

	for (UINT32 i = 0; i < m_Matrix.m_arrMatrixStep.GetSize(); i++)
	{
		CMatrixStep* pMatrixStepTmp = m_Matrix.m_arrMatrixStep[i];
		strStepPath.Format(L"%s\\steps\\%s", m_strFullPathName, pMatrixStepTmp->m_strName);

		if (finder.FindFile(strStepPath))
		{
			CStep* pStep = new CStep;

			pStep->m_nStepID = i;
			pStep->m_strStepName.Format(L"%s", pMatrixStepTmp->m_strName);

			bRst |= pStep->LoadStep(m_strFullPathName);
			bRst |= pStep->LoadPackage(m_strFullPathName);

			if (pStep->m_Stephdr.m_bStepRepeat)
				pStep->m_nStepRepeatType = static_cast<UINT32>(eStepRepeatType::eTwoStep);
			else 
				pStep->m_nStepRepeatType = static_cast<UINT32>(eStepRepeatType::eOneStep);

			m_arrStep.Add(pStep);
		}
	}

	// Sub-Step
	UINT32 iStepCnt = static_cast<UINT32>(m_arrStep.GetSize());
	for (UINT32 i = 0; i < iStepCnt; i++)
	{
		CStep *pStep = m_arrStep.GetAt(i);

		if (pStep == nullptr ||
			pStep->m_nStepRepeatType == static_cast<UINT32>(eStepRepeatType::eOneStep))
			continue;

		pStep->m_nSubStepCnt = 0;
		for (UINT32 j = 0; j < pStep->m_Stephdr.m_arrStepRepeat.GetSize(); j++)
		{
			CStepRepeat* pStepRepeatTmp = pStep->m_Stephdr.m_arrStepRepeat.GetAt(j);
			if (pStepRepeatTmp == nullptr)
				continue;

			for (UINT32 k = 0; k < iStepCnt; k++)
			{
				if (i == k) continue;

				CStep* pSubStep = m_arrStep.GetAt(k);
				if (pSubStep == nullptr)
					continue;

				strStepName = pSubStep->m_strStepName;

				if (pStepRepeatTmp->m_strName.CompareNoCase(strStepName) != 0)
					continue;

				if (pStep->m_nStepRepeatType <= pSubStep->m_nStepRepeatType)
					pStep->m_nStepRepeatType = pSubStep->m_nStepRepeatType + 1;

				CSubStep* pNextStep = new CSubStep;

				pNextStep->m_nStepRepeatType = pSubStep->m_nStepRepeatType;
				pNextStep->m_nStepID = pSubStep->m_nStepID;
				pNextStep->m_pSubStep = pSubStep;
				pNextStep->m_arrStepRepeat = *pStepRepeatTmp;

				pNextStep->m_dXDatum = pSubStep->m_Stephdr.m_dXDatum;
				pNextStep->m_dYDatum = pSubStep->m_Stephdr.m_dYDatum;
				pNextStep->m_dXOrigin = pSubStep->m_Stephdr.m_dXOrigin;
				pNextStep->m_dYOrigin = pSubStep->m_Stephdr.m_dYOrigin;

				pStep->m_arrSubStep.Add(pNextStep);
				pStep->m_nSubStepCnt++;
				break;
			}
		}
	}

	// Copy Sub-Step of Sub-Step
	for (UINT32 i = 0; i < iStepCnt; i++)
	{
		CStep* pStep = m_arrStep.GetAt(i);

		if (pStep == nullptr ||
			pStep->m_nStepRepeatType == static_cast<UINT32>(eStepRepeatType::eOneStep))
			continue;

		for (UINT32 j = 0; j < pStep->m_nSubStepCnt; j++)
		{
			strStepName = pStep->m_arrSubStep[j]->m_pSubStep->m_strStepName;
				
			pStep->m_arrSubStep[j]->m_nNextStepCnt = 0;

			for (UINT32 k = 0; k < iStepCnt; k++)
			{
				if (i == k) continue;

				CStep* pSubStep = m_arrStep.GetAt(k);
				if (pSubStep == nullptr)
					continue;

				if (pSubStep->m_strStepName.CompareNoCase(strStepName) != 0)
					continue;

				if (pSubStep->m_nStepRepeatType == static_cast<UINT32>(eStepRepeatType::eOneStep)) 
					break;

				for (UINT32 l = 0; l < pSubStep->m_nSubStepCnt; l++)
				{
					//CSubStep* pNextStep = new CSubStep;
					CSubStep* pNextStep = pSubStep->m_arrSubStep[l];

					pStep->m_arrSubStep[j]->m_arrNextStep.Add(pNextStep);
					pStep->m_arrSubStep[j]->m_nNextStepCnt++;
				}
				break;
			}
		}
	}
	
	return bRst;
}

void CJobFile::ClearAll()
{
	m_Matrix.ClearMatrixLayers();
	m_Matrix.ClearMatrixSteps();		
	
	ClearFonts();
	ClearUserSymbols();
	ClearSteps();	
	ClearPadUsage();
}

void CJobFile::ClearSteps()
{
	for (UINT32 i = 0; i < m_arrStep.GetSize(); i++)
	{
		m_arrStep[i]->m_Profile.Clear();
		m_arrStep[i]->m_Stephdr.ClearStepRepeats();
		m_arrStep[i]->ClearLayers();
		m_arrStep[i]->ClearPackages();
		m_arrStep[i]->ClearSubSteps();
		delete m_arrStep[i];
		m_arrStep[i] = nullptr;
	}

	m_arrStep.RemoveAll();	
}

void CJobFile::ClearUserSymbols()
{
	for (UINT32 i = 0; i < m_arrSymbol.GetSize(); i++)
	{
		m_arrSymbol[i]->ClearUserSymbol();
		delete m_arrSymbol[i];
		m_arrSymbol[i] = nullptr;
	}

	m_arrSymbol.RemoveAll();
}

void CJobFile::ClearFonts()
{
	for (UINT32 i = 0; i < m_arrFont.GetSize(); i++)
	{
		m_arrFont[i]->ClearFontChars();
		delete m_arrFont[i];
		m_arrFont[i] = nullptr;
	}

	m_arrFont.RemoveAll();
}

void CJobFile::ClearPadUsage()
{
	for (UINT32 i = 0; i < m_arrPadUsage.GetSize(); i++)
	{
		delete m_arrPadUsage[i];
		m_arrPadUsage[i] = nullptr;
	}

	m_arrPadUsage.RemoveAll();
}

void CJobFile::ClearLayers()
{
	for (UINT32 i = 0; i < m_arrStep.GetSize(); i++)
	{
		m_arrStep[i]->ClearLayers();
	}
}



void CJobFile::SetFeatureMinMaxOfStep()
{
	RECTD tmpMinMax;

	UINT32 iStepCnt = static_cast<UINT32>(m_arrStep.GetSize());
	for (UINT32 stepid = 0; stepid < iStepCnt; stepid++)
	{
		tmpMinMax.top = tmpMinMax.right = -1000000;
		tmpMinMax.bottom = tmpMinMax.left = 1000000;

		memset(&m_arrStep[stepid]->m_FeatureMinMax, 0x00, sizeof(RECTD));
		for (int layerno = 0; layerno < m_arrStep[stepid]->m_arrLayer.GetSize(); layerno++)
		{
			CLayer* pLayer = m_arrStep[stepid]->m_arrLayer.GetAt(layerno);
			if (pLayer == nullptr || pLayer->m_FeatureFile.m_arrFeature.GetSize() <= 0) continue;

			tmpMinMax.top	 = MAX(tmpMinMax.top, pLayer->m_FeatureFile.m_MinMaxRect.top);
			tmpMinMax.bottom = MIN(tmpMinMax.bottom, pLayer->m_FeatureFile.m_MinMaxRect.bottom);
			tmpMinMax.left	 = MIN(tmpMinMax.left, pLayer->m_FeatureFile.m_MinMaxRect.left);
			tmpMinMax.right  = MAX(tmpMinMax.right, pLayer->m_FeatureFile.m_MinMaxRect.right);
		}

		tmpMinMax.top	 = MAX(tmpMinMax.top, m_arrStep[stepid]->m_Profile.m_MinMax.top);
		tmpMinMax.bottom = MIN(tmpMinMax.bottom, m_arrStep[stepid]->m_Profile.m_MinMax.bottom);
		tmpMinMax.left	 = MIN(tmpMinMax.left, m_arrStep[stepid]->m_Profile.m_MinMax.left);
		tmpMinMax.right  = MAX(tmpMinMax.right, m_arrStep[stepid]->m_Profile.m_MinMax.right);

		memcpy(&m_arrStep[stepid]->m_FeatureMinMax, &tmpMinMax, sizeof(RECTD));
	}
}

void CJobFile::SetFeatureMinMaxOfStepNRepeat()
{	
	enum Orient eOrient;
	double refX, refY, dTmpX, dTmpY;
	double parentX, parentY, dTmp;
	RECTD tmpMinMax, tmpProfile;
	RECTD tmpFeatureBox, tmpProfileBox;

	tmpFeatureBox.top = tmpFeatureBox.right = -1000000;
	tmpFeatureBox.bottom = tmpFeatureBox.left = 1000000;
	tmpProfileBox.top = tmpProfileBox.right = -1000000;
	tmpProfileBox.bottom = tmpProfileBox.left = 1000000;

	CRotMirrArr rotMirrArr;
	rotMirrArr.ClearAll();

	CStepRepeat* pStepRepeatTmp = nullptr;
	UINT32 iStepCnt = static_cast<UINT32>(m_arrStep.GetSize());
	for (UINT32 stepid = 0; stepid < iStepCnt; stepid++)
	{
		if (!m_arrStep[stepid]->m_Stephdr.m_bStepRepeat) continue;
		for (int srno = 0; srno < m_arrStep[stepid]->m_Stephdr.m_arrStepRepeat.GetSize(); srno++)
		{
			pStepRepeatTmp = m_arrStep[stepid]->m_Stephdr.m_arrStepRepeat.GetAt(srno);

			for (UINT32 fno = 0; fno < iStepCnt; fno++)
			{
				if (pStepRepeatTmp->m_strName.CompareNoCase(m_arrStep[fno]->m_strStepName) != 0) continue;

					 if ((pStepRepeatTmp->m_dAngle == 0) && (pStepRepeatTmp->m_bMirror == false))	eOrient = Orient::NoMir0Deg;
				else if ((pStepRepeatTmp->m_dAngle == 90) && (pStepRepeatTmp->m_bMirror == false))	eOrient = Orient::NoMir90Deg;
				else if ((pStepRepeatTmp->m_dAngle == 180) && (pStepRepeatTmp->m_bMirror == false))	eOrient = Orient::NoMir180Deg;
				else if ((pStepRepeatTmp->m_dAngle == 270) && (pStepRepeatTmp->m_bMirror == false))	eOrient = Orient::NoMir270Deg;
				else if ((pStepRepeatTmp->m_dAngle == 0) && (pStepRepeatTmp->m_bMirror == true))	eOrient = Orient::Mir0Deg;
				else if ((pStepRepeatTmp->m_dAngle == 90) && (pStepRepeatTmp->m_bMirror == true))	eOrient = Orient::Mir90Deg;
				else if ((pStepRepeatTmp->m_dAngle == 180) && (pStepRepeatTmp->m_bMirror == true))	eOrient = Orient::Mir180Deg;
				else if ((pStepRepeatTmp->m_dAngle == 270) && (pStepRepeatTmp->m_bMirror == true))	eOrient = Orient::Mir270Deg;
				else eOrient = Orient::NoMir0Deg;

				for (UINT32 ystep = 0; ystep < pStepRepeatTmp->m_nNY; ystep++)
				{
					for (UINT32 xstep = 0; xstep < pStepRepeatTmp->m_nNX; xstep++)
					{
						refX = m_arrStep[fno]->m_Stephdr.m_dXDatum;
						refY = m_arrStep[fno]->m_Stephdr.m_dYDatum;
						rotMirrArr.ConvertPoint(&dTmpX, &dTmpY, 0, 0, refX, refY, eOrient);

						parentX = (pStepRepeatTmp->m_dX - dTmpX) + pStepRepeatTmp->m_dDX * xstep;
						parentY = (pStepRepeatTmp->m_dY - dTmpY) + pStepRepeatTmp->m_dDY * ystep;
						rotMirrArr.InputData(parentX, parentY, eOrient);

						rotMirrArr.FinalPoint(&tmpMinMax.left, &tmpMinMax.top, m_arrStep[fno]->m_FeatureMinMax.left, m_arrStep[fno]->m_FeatureMinMax.top);
						rotMirrArr.FinalPoint(&tmpMinMax.right, &tmpMinMax.bottom, m_arrStep[fno]->m_FeatureMinMax.right, m_arrStep[fno]->m_FeatureMinMax.bottom);

						if (tmpMinMax.top > tmpMinMax.bottom) SWAP(tmpMinMax.top, tmpMinMax.bottom, dTmp);
						if (tmpMinMax.left > tmpMinMax.right) SWAP(tmpMinMax.left, tmpMinMax.right, dTmp);

						rotMirrArr.FinalPoint(&tmpProfile.left, &tmpProfile.top, m_arrStep[fno]->m_Profile.m_MinMax.left, m_arrStep[fno]->m_Profile.m_MinMax.top);
						rotMirrArr.FinalPoint(&tmpProfile.right, &tmpProfile.bottom, m_arrStep[fno]->m_Profile.m_MinMax.right, m_arrStep[fno]->m_Profile.m_MinMax.bottom);

						if (tmpProfile.top > tmpProfile.bottom) SWAP(tmpProfile.top, tmpProfile.bottom, dTmp);
						if (tmpProfile.left > tmpProfile.right) SWAP(tmpProfile.left, tmpProfile.right, dTmp);

						rotMirrArr.RemoveTail();

						tmpFeatureBox.left	 = MIN(tmpFeatureBox.left, tmpMinMax.left);
						tmpFeatureBox.right  = MAX(tmpFeatureBox.right, tmpMinMax.right);
						tmpFeatureBox.top	 = MAX(tmpFeatureBox.top, tmpMinMax.top);
						tmpFeatureBox.bottom = MIN(tmpFeatureBox.bottom, tmpMinMax.bottom);

						tmpProfileBox.left	 = MIN(tmpProfileBox.left, tmpProfile.left);
						tmpProfileBox.right  = MAX(tmpProfileBox.right, tmpProfile.right);
						tmpProfileBox.top	 = MAX(tmpProfileBox.top, tmpProfile.top);
						tmpProfileBox.bottom = MIN(tmpProfileBox.bottom, tmpProfile.bottom);
					} //for xstep
				} // for ystep
			} //for fno
		} //for srno
		if (m_arrStep[stepid]->m_Stephdr.m_arrStepRepeat.GetSize() > 0)
		{
			memcpy(&pStepRepeatTmp->m_FeatureMinMax, &tmpFeatureBox, sizeof(RECTD));
			memcpy(&pStepRepeatTmp->m_ProfileMinMax, &tmpProfileBox, sizeof(RECTD));
		}
	} //for stepid
	rotMirrArr.ClearAll();
}

UINT32 CJobFile::SaveLayer(UINT32 iStepIndex, CString strStepName, LayerSet_t* pLayerSet)
{
	if (pLayerSet == nullptr)
		return RESULT_BAD;

	CStep *pStep = m_arrStep.GetAt(iStepIndex);
	if (pStep == nullptr) return RESULT_BAD;
	
	CString strLayerPath = L"";
	strLayerPath.Format(L"%s\\steps\\%s\\layers\\%s\\features", m_strFullPathName, strStepName, pLayerSet->strLayer);

	CLayer *pLayer = nullptr;
	//Layer set 이름과 같은 Layer 가져오기
	int nLayerCount = static_cast<int>(pStep->m_arrLayer.GetCount());
	for (int i = 0; i < nLayerCount; i++)
	{
		CLayer *pLayerComp = pStep->m_arrLayer.GetAt(i);
		if (pLayerComp == nullptr) continue;

		CString strLayerName = pLayerComp->m_strLayerName;
		CString strLayerSetName = pLayerSet->strLayer;

		if (strLayerName.MakeLower() == strLayerSetName.MakeLower())
		{
			pLayer = pLayerComp;
			break;
		}
	}

	if (pLayer == nullptr) return RESULT_BAD;

	UINT32 iRet;
	iRet = pLayer->m_FeatureFile.SaveFeatureFile(strLayerPath);

	//
	CString strPathName = L"";
	strPathName.Format(L"%s\\steps\\%s\\layers\\%s\\Align", m_strFullPathName, strStepName, pLayerSet->strLayer);
	pLayer->SaveAlignMask(strPathName, MaskType::enumType_Align);
	strPathName.Format(L"%s\\steps\\%s\\layers\\%s\\Mask", m_strFullPathName, strStepName, pLayerSet->strLayer);
	pLayer->SaveAlignMask(strPathName, MaskType::enumType_Mask);

	return iRet;
}

double CJobFile::InchToMM(double dValue)
{
	return dValue * 25.4;
}
double CJobFile::MMToInch(double dValue)
{
	// 1 mil = 1/1000 inch
	// 1 inch = 2.54 cm = 25.4mm
	// 1 mil = 1/1000*25.4 mm
	return dValue / 25.4;
}
double CJobFile::MilToMM(double dValue)
{
	// 1 mil = 1/1000 inch
	// 1 inch = 2.54 cm = 25.4mm
	// 1 mil = 1/1000*25.4 mm
	return dValue * 25.4 / 1000;
}

double CJobFile::MMToMil(double dValue)
{
	// 1 mil = 1/1000 inch
	// 1 inch = 2.54 cm = 25.4mm
	// 1 mil = 1/1000*25.4 mm
	// 1 mm = 1 / 25.4 * 1000 mil
	return dValue / 25.4 * 1000;
}

CPointD CJobFile::RotatePt(double dx, double dy, double angle, double cenx, double ceny)
{
	CPointD pt;
	double rad, cosrad, sinrad;

	rad = angle * M_PI / 180.0;
	cosrad = cos(rad);
	sinrad = sin(rad);

	pt.x = (cosrad * (dx - cenx) + sinrad * (dy - ceny) + cenx);
	pt.y = (-sinrad * (dx - cenx) + cosrad * (dy - ceny) + ceny);
	return pt;
}

double CJobFile::RotatePtX(double dx, double dy, double angle, double cenx, double ceny)
{
	double ptX;
	double rad, cosrad, sinrad;

	rad = angle * M_PI / 180.0;
	cosrad = cos(rad);
	sinrad = sin(rad);

	ptX = (cosrad * (dx - cenx) + sinrad * (dy - ceny) + cenx);
	return ptX;
}

double CJobFile::RotatePtY(double dx, double dy, double angle, double cenx, double ceny)
{
	double ptY;
	double rad, cosrad, sinrad;

	rad = angle * M_PI / 180.0;
	cosrad = cos(rad);
	sinrad = sin(rad);

	ptY = (-sinrad * (dx - cenx) + cosrad * (dy - ceny) + ceny);
	return ptY;
}

CStandardFont* CJobFile::GetStandardFont(CString strFontName)
{
	CStandardFont* pFont = nullptr;

	if (strFontName.GetLength() < 1)
		return pFont;

	for (UINT32 i = 0; i < m_arrFont.GetSize(); i++)
	{
		CString sFont = m_arrFont[i]->m_strFontName;
		if (strFontName.CompareNoCase(sFont) == 0)		
			pFont = m_arrFont[i];
	}

	return pFont;
}

UINT32 CJobFile::CheckLayer_MpTp()
{
	UINT32 bRst = RESULT_GOOD;

	std::vector<CString> vcPatternLayer;
	for (UINT32 iLoop = 0; iLoop < m_Matrix.m_arrMatrixLayer.GetSize(); iLoop++)
	{
		CMatrixLayer* pMatrixLayerTmp = m_Matrix.m_arrMatrixLayer[iLoop];

		//모든 Layer에서 MP, TP 만들도록 주석 처리
// 		if (pMatrixLayerTmp->m_strContext.CompareNoCase(L"BOARD") != 0 ||
// 			(pMatrixLayerTmp->m_strType.CompareNoCase(L"SIGNAL") != 0))
// 			continue;

		//MP, TP를 제외한 모든 레이어에 MP, TP 생성
		if (pMatrixLayerTmp->m_strName.Find(_T("_mp")) >= 0 ||
			pMatrixLayerTmp->m_strName.Find(_T("_MP")) >= 0 ||
			pMatrixLayerTmp->m_strName.Find(_T("_tp")) >= 0 ||
			pMatrixLayerTmp->m_strName.Find(_T("_TP")) >= 0)
			continue;

		vcPatternLayer.emplace_back(pMatrixLayerTmp->m_strName);
	}

	std::map<CString, bool> vcMeasureLayer;
	CString strLayer = L"";

	//Check Mp Layer
	for (auto it : vcPatternLayer)
	{
		strLayer.Format(L"%s_MP", it);

		bool bExist = false;
		for (UINT32 iLoop = 0; iLoop < m_Matrix.m_arrMatrixLayer.GetSize(); iLoop++)
		{
			CMatrixLayer* pMatrixLayerTmp = m_Matrix.m_arrMatrixLayer[iLoop];

			if (strLayer.CompareNoCase(pMatrixLayerTmp->m_strName) == 0)
			{
				bExist = true;
				break;
			}
		}

		vcMeasureLayer.insert(std::make_pair(strLayer.MakeUpper(), bExist));
	}

	if (vcMeasureLayer.size() > 0)
		bRst = m_Matrix.AddLayer(m_strFullPathName, vcMeasureLayer);

	vcMeasureLayer.clear();

	//Check Tp Layer
	for (auto it : vcPatternLayer)
	{
		strLayer.Format(L"%s_TP", it);

		bool bExist = false;
		for (UINT32 iLoop = 0; iLoop < m_Matrix.m_arrMatrixLayer.GetSize(); iLoop++)
		{
			CMatrixLayer* pMatrixLayerTmp = m_Matrix.m_arrMatrixLayer[iLoop];

			if (strLayer.CompareNoCase(pMatrixLayerTmp->m_strName) == 0)
			{
				bExist = true;
				break;
			}
		}

		vcMeasureLayer.insert(std::make_pair(strLayer.MakeUpper(), bExist));
	}

	if (vcMeasureLayer.size() > 0)
		bRst = m_Matrix.AddLayer(m_strFullPathName, vcMeasureLayer);

	vcPatternLayer.clear();
	vcMeasureLayer.clear();

	return bRst;
}

void CRotMirrArr::ClearAll()
{
	for (UINT32 i = 0; i < m_arrRotMirr.GetSize(); i++)
		delete m_arrRotMirr[i];

	m_arrRotMirr.RemoveAll();
}

enum Orient CRotMirrArr::GetOrient(UINT32 idx)
{
	if (idx >= m_arrRotMirr.GetSize())
		return Orient::NoMir0Deg;

	return m_arrRotMirr[idx]->GetOrient();
}

double CRotMirrArr::GetX(UINT32 idx)
{
	if (idx >= m_arrRotMirr.GetSize())
		return 0.;

	return m_arrRotMirr[idx]->GetX();
}

double CRotMirrArr::GetY(UINT32 idx)
{
	if (idx >= m_arrRotMirr.GetSize())
		return 0.;

	return m_arrRotMirr[idx]->GetY();
}

UINT32 CRotMirrArr::GetSize()
{
	return static_cast<UINT32>(m_arrRotMirr.GetSize());
}

UINT32 CRotMirrArr::RemoveTail()
{
	UINT32 iSize = GetSize();
	if (iSize > 0)
	{
		delete m_arrRotMirr[iSize - 1];
		m_arrRotMirr.RemoveAt(iSize - 1);
	}
	return RESULT_GOOD;
}

enum Orient CRotMirrArr::FinalOrient(enum Orient InOrient)
{
	enum Orient eTmpOrient = InOrient;
	UINT32 iSize = GetSize();

	for (UINT32 i = 0; i < iSize; i++)
		ConvertOrient(&eTmpOrient, m_arrRotMirr[iSize - i - 1]->GetOrient(), eTmpOrient);

	return eTmpOrient;
}

UINT32 CRotMirrArr::InputData(double dX, double dY, enum Orient eOrient, enum eDirectionType mirDir)
{
	CRotMirr* pRotMirr = new CRotMirr(eOrient, dX, dY, mirDir);

	return static_cast<UINT32>(m_arrRotMirr.Add(pRotMirr));
}

UINT32 CRotMirrArr::ConvertOrient(enum Orient *pOrient, enum Orient eParent, enum Orient eChild)
{					
	ASSERT(eParent <= Orient::Mir270Deg);	//not supported for now

	int iRotation = 0, iTmpOrient = 0;
	bool bMirror = false;

	switch (eParent)
	{
		case Orient::NoMir90Deg:	iRotation = 1;	break;
		case Orient::NoMir180Deg:	iRotation = 2;	break;
		case Orient::NoMir270Deg:	iRotation = 3;	break;
		case Orient::Mir0Deg:		bMirror = true;	break;
		case Orient::Mir90Deg:		iRotation = 1;	bMirror = true;		break;
		case Orient::Mir180Deg:		iRotation = 2;	bMirror = true;		break;
		case Orient::Mir270Deg:		iRotation = 3;	bMirror = true;		break;
	}
	int iMir0Deg = static_cast<int>(Orient::Mir0Deg);

	//rotation 처리
	if (eChild < Orient::Mir0Deg)
	{
		iTmpOrient = static_cast<int>(eChild) + iRotation;
		if (iTmpOrient >= iMir0Deg)
			iTmpOrient -= iMir0Deg;
	}
	else
	{
		iTmpOrient = static_cast<int>(eChild) - iRotation;
		if (iTmpOrient < iMir0Deg)
			iTmpOrient += iMir0Deg;
	}

	//Mirror처리
	if (bMirror)
	{
		if (iTmpOrient < iMir0Deg)
			iTmpOrient = iTmpOrient + iMir0Deg;
		else
			iTmpOrient = iTmpOrient - iMir0Deg;
	}

	*pOrient = static_cast<enum Orient>(iTmpOrient);
	return RESULT_GOOD;
}

UINT32 CRotMirrArr::ConvertPoint(double *pdFinalX, double *pdFinalY, double dParentX, double dParentY, double dChildX, double dChildY, enum Orient eParentOrient, enum eDirectionType mirDir)
{
	if (pdFinalX == nullptr || pdFinalY == nullptr)
		return RESULT_BAD;

	double dAngle = 0.;
	bool bMirror = false;

	switch (eParentOrient)
	{
		case Orient::NoMir45Deg:	dAngle = 45.0;		break;
		case Orient::NoMir90Deg:	dAngle = 90.0;		break;
		case Orient::NoMir135Deg:	dAngle = 135.0;		break;
		case Orient::NoMir180Deg:	dAngle = 180.0;		break;
		case Orient::NoMir225Deg:	dAngle = 225.0;		break;
		case Orient::NoMir270Deg:	dAngle = 270.0;		break;
		case Orient::NoMir315Deg:	dAngle = 315.0;		break;
		case Orient::Mir0Deg:		bMirror = true;		break;
		case Orient::Mir45Deg:		dAngle = 45.0;		bMirror = true;		break;
		case Orient::Mir90Deg:		dAngle = 90.0;		bMirror = true;		break;
		case Orient::Mir135Deg:		dAngle = 135.0;		bMirror = true;		break;
		case Orient::Mir180Deg:		dAngle = 180.0;		bMirror = true;		break;
		case Orient::Mir225Deg:		dAngle = 225.0;		bMirror = true;		break;
		case Orient::Mir270Deg:		dAngle = 270.0;		bMirror = true;		break;
		case Orient::Mir315Deg:		dAngle = 315.0;		bMirror = true;		break;
	}

	double dTmpX = dChildX;
	double dTmpY = dChildY;

	//각도 회전 처리
	if (dAngle != 0.0)
	{
		dTmpX = dChildX * cos(-1 * PI*dAngle / 180.0) - dChildY * sin(-1 * PI*dAngle / 180.0);
		dTmpY = dChildX * sin(-1 * PI*dAngle / 180.0) + dChildY * cos(-1 * PI*dAngle / 180.0);
	}	

	//Mirror 처리
	if (bMirror)
	{
		if (mirDir == eDirectionType::eBoth)
		{
			dTmpX = -1 * dTmpX;
			dTmpY = -1 * dTmpY;
		}
		else if (mirDir == eDirectionType::eHorizontal)
		{
			dTmpX = -1 * dTmpX;
			dTmpY = dTmpY;
		}
		else if (mirDir == eDirectionType::eVertical)
		{
			dTmpX = dTmpX;
			dTmpY = -1 * dTmpY;
		}
	}

	*pdFinalX = dParentX + dTmpX;
	*pdFinalY = dParentY + dTmpY;

	return RESULT_GOOD;
}

enum eDirectionType CRotMirrArr::FinalDirection()
{
	

	bool bMirror_h = false;
	bool bMirror_v = false;
	
	UINT32 iSize = GetSize();

	for (UINT32 i = 0; i < iSize; i++)
	{
		enum eDirectionType eTmpDirection = m_arrRotMirr[iSize - i - 1]->GetMirDir();
		enum Orient eOrient = m_arrRotMirr[iSize - i - 1]->GetOrient();

		if (eOrient >= Orient::Mir0Deg &&
			eOrient <= Orient::Mir270Deg)
		{//Mirror가 있을때만 체크
			if (eTmpDirection == eDirectionType::eHorizontal)
			{
				bMirror_h = !bMirror_h;
			}
			else if (eTmpDirection == eDirectionType::eVertical)
			{
				bMirror_v = !bMirror_v;
			}
			else if (eTmpDirection == eDirectionType::eBoth)
			{
				bMirror_h = !bMirror_h;
				bMirror_v = !bMirror_v;
			}
		}
	}

	enum eDirectionType eDirection = eDirectionType::DirectionTypeNone;

	if (bMirror_h == true && bMirror_v == true)
	{
		eDirection = eDirectionType::eBoth;
	}
	else if (bMirror_h == true && bMirror_v == false)
	{
		eDirection = eDirectionType::eHorizontal;
	}
	else if (bMirror_h == false && bMirror_v == true)
	{//false, false
		eDirection = eDirectionType::eHorizontal;
	}

	return eDirection;
}

// Parent 좌표를  Mirror및 rotation   후 Child좌표로 변환된 좌표를 구해주는 수 
UINT32 CRotMirrArr::ReversePoint(double *pdFinalX, double *pdFinalY, double dParentX, double dParentY, enum Orient eParentOrient, enum eDirectionType mirDir)
{
	if (pdFinalX == nullptr || pdFinalY == nullptr)
		return RESULT_BAD;

	double dAngle = 0.;
	bool bMirror = false;

	switch (eParentOrient)
	{
		case Orient::NoMir45Deg:	dAngle = 45.0;		break;
		case Orient::NoMir90Deg:	dAngle = 90.0;		break;
		case Orient::NoMir135Deg:	dAngle = 135.0;		break;
		case Orient::NoMir180Deg:	dAngle = 180.0;		break;
		case Orient::NoMir225Deg:	dAngle = 225.0;		break;
		case Orient::NoMir270Deg:	dAngle = 270.0;		break;
		case Orient::NoMir315Deg:	dAngle = 315.0;		break;
		case Orient::Mir0Deg:		bMirror = true;		break;
		case Orient::Mir45Deg:		dAngle = 45.0;		bMirror = true;		break;
		case Orient::Mir90Deg:		dAngle = 90.0;		bMirror = true;		break;
		case Orient::Mir135Deg:		dAngle = 135.0;		bMirror = true;		break;
		case Orient::Mir180Deg:		dAngle = 180.0;		bMirror = true;		break;
		case Orient::Mir225Deg:		dAngle = 225.0;		bMirror = true;		break;
		case Orient::Mir270Deg:		dAngle = 270.0;		bMirror = true;		break;
		case Orient::Mir315Deg:		dAngle = 315.0;		bMirror = true;		break;
	}

	//Mirror 처리
	if (bMirror)
	{
		if (mirDir == eDirectionType::eBoth)
		{
			dParentX = -1 * dParentX;
			dParentY = -1 * dParentY;
		}
		else if(mirDir == eDirectionType::eHorizontal)
		{
			dParentX = -1 * dParentX;
			dParentY = dParentY;
		}
		else if (mirDir == eDirectionType::eVertical)
		{
			dParentX = dParentX;
			dParentY = -1 * dParentY;
		}
	}

	double dTmpX = dParentX;
	double dTmpY = dParentY;

	//각도 회전 처리
	if (dAngle != 0.0)
	{
		dTmpX = dParentX * cos(PI*dAngle / 180.0) - dParentY * sin(PI*dAngle / 180.0);
		dTmpY = dParentX * sin(PI*dAngle / 180.0) + dParentY * cos(PI*dAngle / 180.0);
	}

	*pdFinalX = dTmpX;
	*pdFinalY = dTmpY;

	return RESULT_GOOD;
}

UINT32 CRotMirrArr::FinalPoint(double *pdFinalX, double *pdFinalY, double dInputX, double dInputY)
{
	if (pdFinalX == nullptr || pdFinalY == nullptr)
		return RESULT_BAD;

	*pdFinalX = dInputX;
	*pdFinalY = dInputY;

	UINT32 iSize = GetSize();
	for (UINT32 i = 0; i < iSize; i++)
	{
		ConvertPoint(pdFinalX, pdFinalY, m_arrRotMirr[iSize - i - 1]->GetX(), m_arrRotMirr[iSize - i - 1]->GetY(),
			*pdFinalX, *pdFinalY, m_arrRotMirr[iSize - i - 1]->GetOrient(), m_arrRotMirr[iSize - i - 1]->GetMirDir());
	}

	return RESULT_GOOD;
}

UINT32 CAlignMask::Add_Pad(IN const double &dCenterX, IN const double &dCenterY, IN const double &dWidth, IN const double &dHeight)
{
	CFeature *pFeature = nullptr;

	pFeature = new CFeatureP();
	if (pFeature == nullptr )return RESULT_BAD;

	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_dX = dCenterX;
	pFeatureP->m_dY = dCenterY;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = true;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_eType = FeatureType::P;

	pFeature->m_pSymbol = new CSymbol();
	pFeature->m_pSymbol->m_eSymbolName = SymbolName::rect;
	pFeature->m_pSymbol->m_dRotatedDeg = 0;
	pFeature->m_pSymbol->m_pUserSymbol = nullptr;

	pFeature->m_pSymbol->m_vcParams.resize(DefSymbol::pParaCount[static_cast<int>(SymbolName::rect)]);
	
	pFeature->m_pSymbol->m_vcParams[0] = dWidth;
	pFeature->m_pSymbol->m_vcParams[1] = dHeight;
	pFeature->m_pSymbol->SetMinMaxRect();

	pFeatureP->SetMinMaxRect();

	m_arrFeature.Add(pFeature);

	return RESULT_GOOD;
}

CAlignMask& CAlignMask::operator=(const CAlignMask& m)
{
	if (this != &m)
	{
		Clear();

		int nCount = static_cast<int>(m.m_arrFeature.GetCount());
		for (int i = 0; i < nCount; i++)
		{
			CFeature *pFeature_Ori = m.m_arrFeature.GetAt(i);
			CFeature *pFeature = nullptr;

			switch (pFeature_Ori->m_eType)
			{
			case FeatureType::P:
			{
				pFeature = new CFeatureP();
				if (pFeature_Ori->m_pSymbol != nullptr)
				{
					pFeature->m_pSymbol = nullptr;
					pFeature->m_pSymbol = new CSymbol();

					*pFeature->m_pSymbol = *pFeature_Ori->m_pSymbol;
					pFeature->m_pSymbol->m_pUserSymbol = nullptr;
					pFeature->m_pSymbol->m_vcParams = pFeature_Ori->m_pSymbol->m_vcParams;
				}
				*pFeature = *pFeature_Ori;
				*(CFeatureP*)pFeature = *(CFeatureP*)pFeature_Ori;
			}
			break;
			case FeatureType::S:
			{
				pFeature = new CFeatureS();

				*pFeature = *pFeature_Ori;

				CFeatureS *pFeatureS_Ori = (CFeatureS *)pFeature_Ori;
				CFeatureS *pFeatureS = (CFeatureS *)pFeature;

				int nObOeCount = static_cast<int>(pFeatureS_Ori->m_arrObOe.GetCount());
				for (int i = 0; i < nObOeCount; i++)
				{
					CObOe *in_pObOe = pFeatureS_Ori->m_arrObOe.GetAt(i);
					if (in_pObOe == nullptr) continue;

					if (in_pObOe->m_bHighlight == true)
					{
						CObOe *pObOe = new CObOe();
						if (pObOe == nullptr) continue;
						*pObOe = *in_pObOe;

						int nOsOcCount = static_cast<int>(in_pObOe->m_arrOsOc.GetCount());
						for (int j = 0; j < nOsOcCount; j++)
						{
							COsOc *in_pOsOc = in_pObOe->m_arrOsOc.GetAt(j);
							if (in_pOsOc == nullptr) continue;

							COsOc *pOsOc = nullptr;
							if (in_pOsOc->m_bOsOc == true)
							{
								pOsOc = new COs();
							}
							else
							{
								pOsOc = new COc();
							}
							*pOsOc = *in_pOsOc;

							//Arc 인경우 
							if (in_pOsOc->m_bOsOc == false)
							{
								*((COc*)pOsOc) = *((COc*)in_pOsOc);
							}
							pObOe->m_arrOsOc.Add(pOsOc);
						}
						pFeatureS->m_arrObOe.Add(pObOe);
					}
					else
					{
						pFeatureS->m_arrObOe.Add(nullptr);
					}
				}
			}
			break;
			default:
				break;
			}

			if (pFeature != nullptr)
			{
				m_arrFeature.Add(pFeature);
			}

		}
	}
	return *this;
}