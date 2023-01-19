#include "pch.h"
#include "CFeatureData.h"

#include "CDrawMakePoint.h"

CFeatureData::CFeatureData()
{
	Reset();
}

CFeatureData::~CFeatureData()
{
	Reset();
}

void CFeatureData::SetTp(int nLayer, int nFeatureIndex, CFeatureZ* in_pFeature, int in_nMaskIndex)
{
	Reset();
	if (in_pFeature == nullptr) return;

	m_nLayer = nLayer;
	m_nFeatureIndex = nFeatureIndex;
	m_nMaskIndex = in_nMaskIndex;
	m_pFeature = new CFeatureZ();

	*m_pFeature = *(CFeature*)in_pFeature;

	//심볼의 포인터는 복사하지 않는다.
	//심볼의 Params 복사
	if (in_pFeature->m_pSymbol != nullptr)
	{
		m_pFeature->m_pSymbol = nullptr;
		m_pFeature->m_pSymbol = new CSymbol();

		*m_pFeature->m_pSymbol = *in_pFeature->m_pSymbol;
		m_pFeature->m_pSymbol->m_pUserSymbol = nullptr;
		m_pFeature->m_pSymbol->m_vcParams = in_pFeature->m_pSymbol->m_vcParams;
		m_pFeature->m_pSymbol->SetMinMaxRect();
	}

	CFeatureZ* pFeature = (CFeatureZ*)m_pFeature;
	*pFeature = *in_pFeature;

	//Attribute Name
	int nAttCount = static_cast<int>(in_pFeature->m_arrAttribute.GetCount());
	for (int i = 0; i < nAttCount; i++)
	{
		if (in_pFeature->m_arrAttribute[i] == nullptr) continue;

		CAttribute *pAttName = new CAttribute();
		*pAttName = *in_pFeature->m_arrAttribute[i];

		m_pFeature->m_arrAttribute.Add(pAttName);
	}

	//Attribute String
	int nAttStringCount = static_cast<int>(in_pFeature->m_arrAttributeTextString.GetCount());
	for (int i = 0; i < nAttStringCount; i++)
	{
		if (in_pFeature->m_arrAttributeTextString[i] == nullptr) continue;

		CAttributeTextString *pAttString = new CAttributeTextString();
		*pAttString = *in_pFeature->m_arrAttributeTextString[i];

		m_pFeature->m_arrAttributeTextString.Add(pAttString);
	}

	UpdateEditPoint();
}


void CFeatureData::SetLine(int nLayer, int nFeatureIndex, CFeatureL *in_pFeature, int in_nMaskIndex)
{
	Reset();
	if (in_pFeature == nullptr) return;

	m_nLayer = nLayer;
	m_nFeatureIndex = nFeatureIndex;
	m_nMaskIndex = in_nMaskIndex;
	m_pFeature = new CFeatureL();

	*m_pFeature = *(CFeature*)in_pFeature;

	//심볼의 포인터는 복사하지 않는다.
	//심볼의 Params 복사
	if (in_pFeature->m_pSymbol != nullptr)
	{
		m_pFeature->m_pSymbol = nullptr;
		m_pFeature->m_pSymbol = new CSymbol();

		*m_pFeature->m_pSymbol = *in_pFeature->m_pSymbol;
		m_pFeature->m_pSymbol->m_pUserSymbol = nullptr;
		m_pFeature->m_pSymbol->m_vcParams = in_pFeature->m_pSymbol->m_vcParams;
		m_pFeature->m_pSymbol->SetMinMaxRect();
	}

	CFeatureL* pFeature = (CFeatureL*)m_pFeature;
	*pFeature = *in_pFeature;

	//Attribute Name
	int nAttCount = static_cast<int>(in_pFeature->m_arrAttribute.GetCount());
	for (int i = 0; i < nAttCount; i++)
	{
		if (in_pFeature->m_arrAttribute[i] == nullptr) continue;

		CAttribute *pAttName = new CAttribute();
		*pAttName = *in_pFeature->m_arrAttribute[i];

		m_pFeature->m_arrAttribute.Add(pAttName);
	}

	//Attribute String
	int nAttStringCount = static_cast<int>(in_pFeature->m_arrAttributeTextString.GetCount());
	for (int i = 0; i < nAttStringCount; i++)
	{
		if ( in_pFeature->m_arrAttributeTextString[i] == nullptr) continue;

		CAttributeTextString *pAttString = new CAttributeTextString();
		*pAttString = *in_pFeature->m_arrAttributeTextString[i];

		m_pFeature->m_arrAttributeTextString.Add(pAttString);
	}

	UpdateEditPoint();
}

void CFeatureData::SetPad(int nLayer, int nFeatureIndex, CFeatureP *in_pFeature, int in_nMaskIndex)
{
	Reset();

	m_nLayer = nLayer;
	m_nFeatureIndex = nFeatureIndex;
	m_nMaskIndex = in_nMaskIndex;
	m_pFeature = new CFeatureP();

	*m_pFeature = *(CFeature*)in_pFeature;

	//심볼의 포인터는 복사하지 않는다.
	//심볼의 Params 복사
	if (in_pFeature->m_pSymbol != nullptr)
	{
		m_pFeature->m_pSymbol = nullptr;
		m_pFeature->m_pSymbol = new CSymbol();

		*m_pFeature->m_pSymbol = *in_pFeature->m_pSymbol;
		m_pFeature->m_pSymbol->m_pUserSymbol = nullptr;
		m_pFeature->m_pSymbol->m_vcParams = in_pFeature->m_pSymbol->m_vcParams;
		m_pFeature->m_pSymbol->SetMinMaxRect();
	}

	CFeatureP* pFeature = (CFeatureP*)m_pFeature;
	*pFeature = *in_pFeature;

	//Attribute Name
	int nAttCount = static_cast<int>(in_pFeature->m_arrAttribute.GetCount());
	for (int i = 0; i < nAttCount; i++)
	{
		if (in_pFeature->m_arrAttribute[i] == nullptr) continue;

		CAttribute *pAttName = new CAttribute();
		*pAttName = *in_pFeature->m_arrAttribute[i];

		m_pFeature->m_arrAttribute.Add(pAttName);
	}

	//Attribute String
	int nAttStringCount = static_cast<int>(in_pFeature->m_arrAttributeTextString.GetCount());
	for (int i = 0; i < nAttStringCount; i++)
	{
		if (in_pFeature->m_arrAttributeTextString[i] == nullptr) continue;

		CAttributeTextString *pAttString = new CAttributeTextString();
		*pAttString = *in_pFeature->m_arrAttributeTextString[i];

		m_pFeature->m_arrAttributeTextString.Add(pAttString);
	}

	UpdateEditPoint();
}

void CFeatureData::SetArc(int nLayer, int nFeatureIndex, CFeatureA *in_pFeature, int in_nMaskIndex)
{
	Reset();

	m_nLayer = nLayer;
	m_nFeatureIndex = nFeatureIndex;
	m_nMaskIndex = in_nMaskIndex;
	m_pFeature = new CFeatureA();

	*m_pFeature = *(CFeature*)in_pFeature;

	//심볼의 포인터는 복사하지 않는다.
	//심볼의 Params 복사
	if (in_pFeature->m_pSymbol != nullptr)
	{
		m_pFeature->m_pSymbol = nullptr;
		m_pFeature->m_pSymbol = new CSymbol();

		*m_pFeature->m_pSymbol = *in_pFeature->m_pSymbol;
		m_pFeature->m_pSymbol->m_pUserSymbol = nullptr;
		m_pFeature->m_pSymbol->m_vcParams = in_pFeature->m_pSymbol->m_vcParams;
		m_pFeature->m_pSymbol->SetMinMaxRect();
	}

	CFeatureA* pFeature = (CFeatureA*)m_pFeature;
	*pFeature = *in_pFeature;

	//Attribute Name
	int nAttCount = static_cast<int>(in_pFeature->m_arrAttribute.GetCount());
	for (int i = 0; i < nAttCount; i++)
	{
		if (in_pFeature->m_arrAttribute[i] == nullptr) continue;

		CAttribute *pAttName = new CAttribute();
		*pAttName = *in_pFeature->m_arrAttribute[i];

		m_pFeature->m_arrAttribute.Add(pAttName);
	}

	//Attribute String
	int nAttStringCount = static_cast<int>(in_pFeature->m_arrAttributeTextString.GetCount());
	for (int i = 0; i < nAttStringCount; i++)
	{
		if (in_pFeature->m_arrAttributeTextString[i] == nullptr) continue;

		CAttributeTextString *pAttString = new CAttributeTextString();
		*pAttString = *in_pFeature->m_arrAttributeTextString[i];

		m_pFeature->m_arrAttributeTextString.Add(pAttString);
	}

	UpdateEditPoint();
}

void CFeatureData::SetText(int nLayer, int nFeatureIndex, CFeatureT *in_pFeature, int in_nMaskIndex)
{
	Reset();

	m_nLayer = nLayer;
	m_nFeatureIndex = nFeatureIndex;
	m_nMaskIndex = in_nMaskIndex;
	m_pFeature = new CFeatureT();

	*m_pFeature = *(CFeature*)in_pFeature;

	//심볼의 포인터는 복사하지 않는다.
	//심볼의 Params 복사
	if (in_pFeature->m_pSymbol != nullptr)
	{
		m_pFeature->m_pSymbol = nullptr;
		m_pFeature->m_pSymbol = new CSymbol();

		*m_pFeature->m_pSymbol = *in_pFeature->m_pSymbol;
		m_pFeature->m_pSymbol->m_pUserSymbol = nullptr;
		m_pFeature->m_pSymbol->m_vcParams = in_pFeature->m_pSymbol->m_vcParams;
		m_pFeature->m_pSymbol->SetMinMaxRect();
	}

	CFeatureT* pFeature = (CFeatureT*)m_pFeature;
	*pFeature = *in_pFeature;

	//Attribute Name
	int nAttCount = static_cast<int>(in_pFeature->m_arrAttribute.GetCount());
	for (int i = 0; i < nAttCount; i++)
	{
		if (in_pFeature->m_arrAttribute[i] == nullptr) continue;

		CAttribute *pAttName = new CAttribute();
		*pAttName = *in_pFeature->m_arrAttribute[i];

		m_pFeature->m_arrAttribute.Add(pAttName);
	}

	//Attribute String
	int nAttStringCount = static_cast<int>(in_pFeature->m_arrAttributeTextString.GetCount());
	for (int i = 0; i < nAttStringCount; i++)
	{
		if (in_pFeature->m_arrAttributeTextString[i] == nullptr) continue;

		CAttributeTextString *pAttString = new CAttributeTextString();
		*pAttString = *in_pFeature->m_arrAttributeTextString[i];

		m_pFeature->m_arrAttributeTextString.Add(pAttString);
	}

	UpdateEditPoint();
}

void CFeatureData::SetSurface(int nLayer, int nFeatureIndex, CFeatureS *in_pFeature, int in_nMaskIndex)
{//주의 : Highlight되어 있는 ObOe 정보만 입력 받음
	Reset();

	m_nLayer = nLayer;
	m_nFeatureIndex = nFeatureIndex;
	m_nMaskIndex = in_nMaskIndex;

	m_pFeature = new CFeatureS();
	*m_pFeature = *(CFeature*)in_pFeature;

	CFeatureS* pFeatureS = (CFeatureS*)m_pFeature;

	int nObOeCount = static_cast<int>(in_pFeature->m_arrObOe.GetCount());
	for (int i = 0; i < nObOeCount; i++)
	{
		CObOe *in_pObOe = in_pFeature->m_arrObOe.GetAt(i);
		if (in_pObOe == nullptr) continue;

		if (in_pObOe->m_bHighlight == true) //true
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

	//Attribute Name
	int nAttCount = static_cast<int>(in_pFeature->m_arrAttribute.GetCount());
	for (int i = 0; i < nAttCount; i++)
	{
		if (in_pFeature->m_arrAttribute[i] == nullptr) continue;

		CAttribute *pAttName = new CAttribute();
		*pAttName = *in_pFeature->m_arrAttribute[i];

		m_pFeature->m_arrAttribute.Add(pAttName);
	}

	//Attribute String
	int nAttStringCount = static_cast<int>(in_pFeature->m_arrAttributeTextString.GetCount());
	for (int i = 0; i < nAttStringCount; i++)
	{
		if (in_pFeature->m_arrAttributeTextString[i] == nullptr) continue;

		CAttributeTextString *pAttString = new CAttributeTextString();
		*pAttString = *in_pFeature->m_arrAttributeTextString[i];

		m_pFeature->m_arrAttributeTextString.Add(pAttString);
	}

	UpdateEditPoint();
}

void CFeatureData::SetMove(IN const double &dMoveX_mm, IN const double &dMoveY_mm)
{
	if (m_pFeature == nullptr) return;

	switch (m_pFeature->m_eType)
	{
	case FeatureType::Z:
	{
		CFeatureZ *pFeature = (CFeatureZ*)m_pFeature;
		pFeature->m_dX += dMoveX_mm;
		pFeature->m_dY += dMoveY_mm;
	}
	break;
	case FeatureType::L:
	{
		CFeatureL *pFeature = (CFeatureL*)m_pFeature;
		pFeature->m_dXs += dMoveX_mm;
		pFeature->m_dYs += dMoveY_mm;

		pFeature->m_dXe += dMoveX_mm;
		pFeature->m_dYe += dMoveY_mm;
	}
	break;
	case FeatureType::P:
	{
		CFeatureP *pFeature = (CFeatureP*)m_pFeature;
		pFeature->m_dX += dMoveX_mm;
		pFeature->m_dY += dMoveY_mm;
	}
	break;
	case FeatureType::A:
	{
		CFeatureA *pFeature = (CFeatureA*)m_pFeature;
		pFeature->m_dXs += dMoveX_mm;
		pFeature->m_dYe += dMoveY_mm;

		pFeature->m_dXe += dMoveX_mm;
		pFeature->m_dYs += dMoveY_mm;

		pFeature->m_dXc += dMoveX_mm;
		pFeature->m_dYc += dMoveY_mm;
	}
	break;
	case FeatureType::T:
	{
		CFeatureT *pFeature = (CFeatureT*)m_pFeature;
		pFeature->m_dX += dMoveX_mm;
		pFeature->m_dY += dMoveY_mm;
	}
	break;
	case FeatureType::S:
	{
		CFeatureS *pFeature = (CFeatureS*)m_pFeature;
		int nObOeCount = static_cast<int>(pFeature->m_arrObOe.GetCount());
		for (int nObOe = 0; nObOe < nObOeCount; nObOe++)
		{
			CObOe *pObOe = pFeature->m_arrObOe.GetAt(nObOe);
			if (pObOe == nullptr) continue;

			pObOe->m_dXbs += dMoveX_mm;
			pObOe->m_dYbs += dMoveY_mm;

			int nOsOcCount = static_cast<int>(pObOe->m_arrOsOc.GetCount());
			for (int j = 0; j < nOsOcCount; j++)
			{
				COsOc *pOsOc = pObOe->m_arrOsOc.GetAt(j);

				pOsOc->m_dX += dMoveX_mm;
				pOsOc->m_dY += dMoveY_mm;

				if (pOsOc->m_bOsOc == false)
				{
					((COc*)pOsOc)->m_dXc += dMoveX_mm;
					((COc*)pOsOc)->m_dYc += dMoveY_mm;
				}
			}
		}
	}
	break;
	}
}

void CFeatureData::Reset()
{
	//
	m_vecEditPoint.clear();
	//

	m_nMaskIndex = -1;
	m_nLayer = -1;
	m_nFeatureIndex = -1;

	if (m_pFeature == nullptr) return;

	/*if (m_pFeature->m_pSymbol != nullptr)
	{
		delete m_pFeature->m_pSymbol;
		m_pFeature->m_pSymbol = nullptr;
	}*/

	/*
	int nAttNameCount = static_cast<int>(m_pFeature->m_arrAttribute.GetCount());
	for (int i = 0; i < nAttNameCount; i++)
	{
		if (m_pFeature->m_arrAttribute[i] != nullptr)
		{
			delete m_pFeature->m_arrAttribute[i];
			m_pFeature->m_arrAttribute[i] = nullptr;
		}
	}
	*/

	/*
	int nAttStringCount = static_cast<int>(m_pFeature->m_arrAttributeTextString.GetCount());
	for (int i = 0; i < nAttStringCount; i++)
	{
		if (m_pFeature->m_arrAttributeTextString[i] != nullptr)
		{
			delete m_pFeature->m_arrAttributeTextString[i];
			m_pFeature->m_arrAttributeTextString[i] = nullptr;
		}
	}
	*/

	if (m_pFeature != nullptr)
	{
		delete m_pFeature;
		m_pFeature = nullptr;
	}
}


RECTD CFeatureData::_GetRect(IN const CPointD &dptCenter, IN const double &dHalfWidth, IN const double &dHalfHeight)
{
	RECTD drtRect;

	drtRect.left = dptCenter.x - dHalfWidth;
	drtRect.top = dptCenter.y - dHalfHeight;
	drtRect.right = dptCenter.x + dHalfWidth;
	drtRect.bottom = dptCenter.y + dHalfHeight;

	return drtRect;
}

void CFeatureData::UpdateEditPoint()
{
	_SetEditPoint(m_pFeature, m_vecEditPoint);
}

UINT32 CFeatureData::_SetEditPoint(CFeature *pFeature, vector<RECTD> &vecEditPoint)
{
	switch (pFeature->m_eType)
	{
	case FeatureType::L :
	{
		CFeatureL *pFeatureL = (CFeatureL*)pFeature;
		_SetEditPoint_Line(pFeatureL, vecEditPoint);
	}
		break;
	case FeatureType::P:
	{
		CFeatureP *pFeatureP = (CFeatureP*)pFeature;
		_SetEditPoint_Pad(pFeatureP, vecEditPoint);
	}
		break;
	case FeatureType::A:
	{
		CFeatureA *pFeatureA = (CFeatureA*)pFeature;
		_SetEditPoint_Arc(pFeatureA, vecEditPoint);
	}
		break;
	case FeatureType::S:
	{
		CFeatureS *pFeatureS = (CFeatureS*)pFeature;
		_SetEditPoint_Surface(pFeatureS, vecEditPoint);
	}
		break;
	default:
		return RESULT_BAD;
		break;
	}

	return RESULT_GOOD;
}

UINT32 CFeatureData::_SetEditPoint_Line(CFeatureL *pFeatureL, vector<RECTD> &vecEditPoint)
{
	vecEditPoint.clear();

	if (pFeatureL == nullptr) return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CFeatureData::_SetEditPoint_Pad(CFeatureP *pFeatureP, vector<RECTD> &vecEditPoint)
{
	vecEditPoint.clear();

	if (pFeatureP == nullptr) return RESULT_BAD;
	CSymbol *pSymbol = pFeatureP->m_pSymbol;
	if (pSymbol == nullptr) return RESULT_BAD;

	switch (pSymbol->m_eSymbolName)
	{
	case SymbolName::r:		//circle
		break;
	case SymbolName::s:		//square
		_SetEditPoint_Pad_Square(pFeatureP, vecEditPoint);
		break;
	case SymbolName::rect:		//rect
		_SetEditPoint_Pad_Rect(pFeatureP, vecEditPoint);
		break;
	default:
		break;
	}

	return RESULT_GOOD;
}

UINT32 CFeatureData::_SetEditPoint_Arc(CFeatureA *pFeatureA, vector<RECTD> &vecEditPoint)
{
	vecEditPoint.clear();

	if (pFeatureA == nullptr) return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CFeatureData::_SetEditPoint_Surface(CFeatureS *pFeatureS, vector<RECTD> &vecEditPoint)
{
	vecEditPoint.clear();

	if (pFeatureS == nullptr) return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CFeatureData::_SetEditPoint_Pad_Square(CFeatureP *pFeatureP, vector<RECTD> &vecEditPoint)
{
	vecEditPoint.clear();

	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	CPointD dptFeatureCenter = CPointD(pFeatureP->m_dX, pFeatureP->m_dY);

	//
	int nParaCount = static_cast<int>(pFeatureP->m_pSymbol->m_vcParams.size());
	double dFeatureWidth = pFeatureP->m_pSymbol->m_vcParams[0];

	double dRectSize_W = dFeatureWidth * 0.01 * m_nPointSize;//Feature 크기의 5%

	CPointD dptWidthCenter = CPointD(dptFeatureCenter.x - dFeatureWidth / 2., dptFeatureCenter.y);

	vecEditPoint.resize(nParaCount + enumEditPadCount);

	vecEditPoint[0] = _GetRect(dptWidthCenter, dRectSize_W / 2., dRectSize_W / 2.);//Width

	return RESULT_GOOD;
}

UINT32 CFeatureData::_SetEditPoint_Pad_Rect(CFeatureP *pFeatureP, vector<RECTD> &vecEditPoint)
{
	vecEditPoint.clear();

	if (pFeatureP == nullptr) return RESULT_BAD;
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	CPointD dptFeatureCenter = CPointD(pFeatureP->m_dX, pFeatureP->m_dY);

	//
	int nParaCount = static_cast<int>(pFeatureP->m_pSymbol->m_vcParams.size());
	double dFeaureWidth = pFeatureP->m_pSymbol->m_vcParams[0];
	double dFeaureHeight = pFeatureP->m_pSymbol->m_vcParams[1];

	double dRectSize_W = dFeaureWidth * 0.01 * m_nPointSize;//Feature 크기의 5%
	double dRectSize_H = dFeaureHeight * 0.01 * m_nPointSize;//Feature 크기의 5%
	
	CPointD dptWidthCenter = CPointD(dptFeatureCenter.x - dFeaureWidth / 2., dptFeatureCenter.y);
	CPointD dptHeightCenter = CPointD(dptFeatureCenter.x, dptFeatureCenter.y - dFeaureHeight / 2.);

	vecEditPoint.resize(nParaCount + enumEditPadCount);

	vecEditPoint[0] = _GetRect(dptWidthCenter, dRectSize_W /2., dRectSize_W / 2.);//Width
	vecEditPoint[1] = _GetRect(dptHeightCenter, dRectSize_H / 2., dRectSize_H / 2.);//Height


	return RESULT_GOOD;
}

UINT32 CFeatureData::MoveEditPoint(IN const int nPointIndex, IN const double &dMoveX, IN const double &dMoveY)
{
	if (nPointIndex < 0) return RESULT_BAD;
	if (nPointIndex >= static_cast<int>(m_vecEditPoint.size())) return RESULT_BAD;

	if (m_pFeature == nullptr) return RESULT_BAD;

	CFeature *pFeature = m_pFeature;
	switch (m_pFeature->m_eType)
	{
	case FeatureType::L:
	{
		CFeatureL *pFeatureL = (CFeatureL*)pFeature;
		_MoveEditPoint_Line(pFeatureL, nPointIndex, dMoveX, dMoveY);
	}
	break;
	case FeatureType::P:
	{
		CFeatureP *pFeatureP = (CFeatureP*)pFeature;
		_MoveEditPoint_Pad(pFeatureP, nPointIndex, dMoveX, dMoveY);
	}
	break;
	case FeatureType::A:
	{
		CFeatureA *pFeatureA = (CFeatureA*)pFeature;
		_MoveEditPoint_Arc(pFeatureA, nPointIndex, dMoveX, dMoveY);
	}
	break;
	case FeatureType::S:
	{
		CFeatureS *pFeatureS = (CFeatureS*)pFeature;
		_MoveEditPoint_Surface(pFeatureS, nPointIndex, dMoveX, dMoveY);
	}
	break;
	default:
		return RESULT_BAD;
		break;
	}

	UpdateEditPoint();

	return RESULT_GOOD;
}


UINT32 CFeatureData::_MoveEditPoint_Line(CFeatureL *pFeatureL, IN const int nPointIndex, IN const double &dMoveX, IN const double &dMoveY)
{
	return RESULT_GOOD;
}

UINT32 CFeatureData::_MoveEditPoint_Pad(CFeatureP *pFeatureP, IN const int nPointIndex, IN const double &dMoveX, IN const double &dMoveY)
{
	if (pFeatureP == nullptr) return RESULT_BAD;
	CSymbol *pSymbol = pFeatureP->m_pSymbol;
	if (pSymbol == nullptr) return RESULT_BAD;

	switch (pSymbol->m_eSymbolName)
	{
	case SymbolName::r:		//circle
		break;
	case SymbolName::s:		//square
		_MoveEditPoint_Pad_Square(pFeatureP, nPointIndex, dMoveX, dMoveY);
		break;
	case SymbolName::rect:		//rect
		_MoveEditPoint_Pad_Rect(pFeatureP, nPointIndex, dMoveX, dMoveY);
		break;
	default:
		break;
	}

	return RESULT_GOOD;
}

UINT32 CFeatureData::_MoveEditPoint_Arc(CFeatureA *pFeatureA, IN const int nPointIndex, IN const double &dMoveX, IN const double &dMoveY)
{
	return RESULT_GOOD;
}

UINT32 CFeatureData::_MoveEditPoint_Surface(CFeatureS *pFeatureS, IN const int nPointIndex, IN const double &dMoveX, IN const double &dMoveY)
{
	return RESULT_GOOD;
}


//PAD
UINT32 CFeatureData::_MoveEditPoint_Pad_Square(CFeatureP *pFeatureP, IN const int nPointIndex, IN const double &dMoveX, IN const double &dMoveY)
{
	if (pFeatureP == nullptr) return RESULT_BAD;

	const int nWidthIndex = 0;

	if (nPointIndex == nWidthIndex)
	{//Width
		m_vecEditPoint[nPointIndex].left += dMoveX;
		m_vecEditPoint[nPointIndex].right += dMoveX;


	}
	else if (nPointIndex == -1)
	{//All Move
		m_vecEditPoint[nPointIndex].left += dMoveX;
		m_vecEditPoint[nPointIndex].right += dMoveX;
		m_vecEditPoint[nPointIndex].top += dMoveY;
		m_vecEditPoint[nPointIndex].bottom += dMoveY;
	}

	//Center간의 거리 계산
	CPointD dptFeature = CPointD(pFeatureP->m_dX, pFeatureP->m_dY);
	CPointD dptWidth_hs = CPointD(m_vecEditPoint[nWidthIndex].CenterX(), m_vecEditPoint[nWidthIndex].CenterY());

	double dWidth = CDrawMakePoint::GetDistance(dptFeature, dptWidth_hs) * 2.;

	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	int nParamSize = static_cast<int>(pFeatureP->m_pSymbol->m_vcParams.size());
	if (nParamSize <= 0) return RESULT_BAD;

	//Param 변경
	if (nParamSize > nWidthIndex)
	{
		pFeatureP->m_pSymbol->m_vcParams[nWidthIndex] = dWidth;
	}

	return RESULT_GOOD;
}

UINT32 CFeatureData::_MoveEditPoint_Pad_Rect(CFeatureP *pFeatureP, IN const int nPointIndex, IN const double &dMoveX, IN const double &dMoveY)
{
	if (pFeatureP == nullptr) return RESULT_BAD;

	const int nWidthIndex = 0;
	const int nHeightIndex = 1;

	if (nPointIndex == nWidthIndex)
	{//Width
		m_vecEditPoint[nPointIndex].left += dMoveX;
		m_vecEditPoint[nPointIndex].right += dMoveX;

		
	}
	else if (nPointIndex == nHeightIndex)
	{//Height
		m_vecEditPoint[nPointIndex].top += dMoveY;
		m_vecEditPoint[nPointIndex].bottom += dMoveY;
	}
	else if (nPointIndex == -1)
	{//All Move
		m_vecEditPoint[nPointIndex].left += dMoveX;
		m_vecEditPoint[nPointIndex].right += dMoveX;
		m_vecEditPoint[nPointIndex].top += dMoveY;
		m_vecEditPoint[nPointIndex].bottom += dMoveY;
	}
	
	//Center간의 거리 계산
	CPointD dptFeature = CPointD(pFeatureP->m_dX, pFeatureP->m_dY);
	CPointD dptWidth_hs = CPointD(m_vecEditPoint[nWidthIndex].CenterX(), m_vecEditPoint[nWidthIndex].CenterY());
	CPointD dptHeight_hs = CPointD(m_vecEditPoint[nHeightIndex].CenterX(), m_vecEditPoint[nHeightIndex].CenterY());

	double dWidth = CDrawMakePoint::GetDistance(dptFeature, dptWidth_hs) * 2.;
	double dHeight = CDrawMakePoint::GetDistance(dptFeature, dptHeight_hs) * 2.;
	
	if (pFeatureP->m_pSymbol == nullptr) return RESULT_BAD;

	int nParamSize = static_cast<int>(pFeatureP->m_pSymbol->m_vcParams.size());
	if (nParamSize <= 0) return RESULT_BAD;

	//Param 변경
	if (nParamSize > nWidthIndex)
	{
		pFeatureP->m_pSymbol->m_vcParams[nWidthIndex] = dWidth;
	}

	if (nParamSize > nHeightIndex)
	{
		pFeatureP->m_pSymbol->m_vcParams[nHeightIndex] = dHeight;
	}

	return RESULT_GOOD;
}


UINT32 CFeatureData::Resize_Apply(IN const float &fSizePersent)
{
	if (fSizePersent == 0.f) return RESULT_GOOD;
	// 
	// 	int nFeatureCount = static_cast<int>(m_vecFeature.size());
	// 
	// 	for (int i = 0; i < nFeatureCount; i++)
	// 	{
	// 		CFeature *pFeature = m_vecFeature[i]->m_pFeature;
	// 		if (pFeature == nullptr) continue;
	// 
	// 		switch (pFeature->m_eType)
	// 		{
	// 		case FeatureType::L:		//Line	
	// 		{
	// 			CFeatureL *pFeatureL = (CFeatureL *)pFeature;
	// 			if (pFeatureL == nullptr) continue;
	// 			if (pFeatureL->m_pSymbol == nullptr) continue;
	// 			if (pFeatureL->m_pSymbol->m_vcParams.size() <= 0) continue;
	// 			pFeatureL->m_pSymbol->m_vcParams[0] = (pFeatureL->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
	// 		}
	// 		break;
	// 		case FeatureType::A:		//Arc	
	// 		{
	// 			CFeatureA *pFeatureA = (CFeatureA *)pFeature;
	// 
	// 			if (pFeatureA == nullptr) continue;
	// 			if (pFeatureA->m_pSymbol == nullptr) continue;
	// 			if (pFeatureA->m_pSymbol->m_vcParams.size() <= 0) continue;
	// 
	// 			pFeatureA->m_pSymbol->m_vcParams[0] = (pFeatureA->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
	// 		}
	// 		break;
	// 		}
	// 	}
	// 
	return RESULT_GOOD;

	return RESULT_GOOD;
}
