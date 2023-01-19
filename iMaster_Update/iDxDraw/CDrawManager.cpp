#include "pch.h"
#include "CDrawManager.h"



#include "DxDraw/CDirectView.h"
#include "CDrawProfile.h"
#include "CDrawFunction.h"
#include "CDrawEdit_Temp.h"
#include "CDrawFov.h"

#include "CEditFunction.h"
#include "CFeatureFilterFunction.h"
#include "CFeatureData.h"


CDrawManager::CDrawManager()
{
	m_matScaleView = D2D1::Matrix3x2F::Scale(1500.f, 1500.f, D2D1::Point2F(0, 0));

	m_pDirectView = new CDirectView();
	m_pDirectView->SetLink(this);

	m_pDrawProfile = new CDrawProfile();

	m_pDrawEdit_Temp = new CDrawEdit_Temp();

	m_pDrawFov = new CDrawFov();

	m_pFeatureFilter = new CFeatureFilterFunction();

	m_pFeatureData = new CFeatureData();
}

CDrawManager::~CDrawManager()
{
	if (m_pDirectView != nullptr)
	{
		delete m_pDirectView;
		m_pDirectView = nullptr;
	}

	if (m_pDrawProfile != nullptr)
	{
		delete m_pDrawProfile;
		m_pDrawProfile = nullptr;
	}

	if (m_pDrawEdit_Temp != nullptr)
	{
		delete m_pDrawEdit_Temp;
		m_pDrawEdit_Temp = nullptr;
	}

	if (m_pDrawFov != nullptr)
	{
		delete m_pDrawFov;
		m_pDrawFov = nullptr;
	}

	if (m_pFeatureFilter != nullptr)
	{
		delete m_pFeatureFilter;
		m_pFeatureFilter = nullptr;
	}


	CDirect_Base::Release();
}

UINT32 CDrawManager::SetSurface(IN const CString LayerName)
{
	vector<CFeature*> vecFeature;

	vecFeature.resize(1);

	CObOe* pObOe = new CObOe;
	pObOe->m_dXbs = m_vecPoint.at(0).dAddX;
	pObOe->m_dYbs = m_vecPoint.at(0).dAddY;
	pObOe->m_bHighlight = true;

	pObOe->m_bPolyType = true;


	CFeature* pFeature = new CFeatureS;
	CFeatureS *pFeatureS = (CFeatureS *)pFeature;



	COsOc* pOsOc = new COsOc;

	for (int i = 1; i < m_nPointIndex; i++)
	{
		if (m_vecPoint.at(i).bOsOc == true)
		{
			COsOc* pOsOc = new COs;
			pOsOc->m_bOsOc = true;
			pOsOc->m_dX = m_vecPoint.at(i).dAddX;
			pOsOc->m_dY = m_vecPoint.at(i).dAddY;
			pObOe->m_arrOsOc.Add(pOsOc);
		}

		else
		{
			COsOc* pOsOc = new COc;
			pOsOc->m_bOsOc = false;
			pOsOc->m_dX = m_vecPoint.at(i).dAddX;
			pOsOc->m_dY = m_vecPoint.at(i).dAddY;
			((COc*)pOsOc)->m_bCw = m_vecPoint.at(i).bCCW;
			((COc*)pOsOc)->m_dXc = m_vecPoint.at(i).dCenterX;
			((COc*)pOsOc)->m_dYc = m_vecPoint.at(i).dCenterY;
			pObOe->m_arrOsOc.Add(pOsOc);
		}

	}

	pOsOc->m_bOsOc = true;
	pOsOc->m_dX = m_vecPoint[0].dAddX;
	pOsOc->m_dY = m_vecPoint[0].dAddY;
	pObOe->m_arrOsOc.Add(pOsOc);

	pFeatureS->m_arrObOe.Add(pObOe);

	pFeatureS->m_bPolarity = true;

	// 2022.06.07
	// attribute, attrstring hard coding
	CAttribute* pAttribute = new CAttribute;
	pAttribute->m_strName = _T(".string");
	pFeatureS->m_arrAttribute.Add(pAttribute);

	CAttributeTextString* pAttributeString = new CAttributeTextString;
	pAttributeString->m_strText = _T("NONE");
	pFeatureS->m_arrAttributeTextString.Add(pAttributeString);

	vecFeature[0] = pFeatureS;

	CString strLayer = LayerName;

	SetAdd(strLayer, vecFeature);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();

	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);


	CDirect_Base::Render();

	m_vecPoint.clear();

 	return RESULT_GOOD;
}


UINT32 CDrawManager::_SetTp(IN const float fX, IN const float fY)
{
	m_vecPoint.clear();

	vector<vector<DrawProfileData>>	vecvecRect;
	int iStepIdx = m_pUserLayerSetInfo->nPanelStepIdx;

	vecvecRect.clear();
	vecvecRect = CDrawFunction::GetProfileData(m_pJobData, iStepIdx);

	MeasureItem_t stMeasure;
	stMeasure.MinMaxRect = RECTD (fX - 0.3, fY + 0.3,fX + 0.3,  fY - 0.3);
	stMeasure.MinMaxRect_In_Panel = RECTD(fX - 0.3, fY + 0.3, fX + 0.3, fY - 0.3);

	RECTD UnitRect;
	RECTD DummyRect;


	bool bCheckPanel = FALSE;
	bool bCheckStrip = FALSE;
	bool bCheckUnit = FALSE;
	int stripidx, unitidx = -1;
	int nStripStep;
	int nUnitStep;
	
	//
	int nPanelStepIndex = m_pUserLayerSetInfo->nPanelStepIdx;
	vector<int> nStripStepIndex = m_pUserLayerSetInfo->vcStripStepIdx;
	vector<int> nUnitStepIndex = m_pUserLayerSetInfo->vcUnitStepIdx;

	// True 면 In , False 면 Out
	//Panel Step Check
	bCheckPanel = CDrawFunction::CheckPanelInOut(vecvecRect, &stMeasure, nPanelStepIndex);
	//Strip Step Check
	for (nStripStep = 0; nStripStep < static_cast<int>(nStripStepIndex.size()); nStripStep++)
	{
		bCheckStrip = CDrawFunction::CheckStripInOut(vecvecRect, &stMeasure, stripidx, nStripStepIndex[nStripStep]);
		if (bCheckStrip == TRUE)
		{
			break;
		}
	}
	//Unit Step Check
	for (nUnitStep = 0; nUnitStep < static_cast<int>(nUnitStepIndex.size()); nUnitStep++)
	{
		bCheckUnit = CDrawFunction::CheckUnitInOut(vecvecRect, &stMeasure, unitidx, nUnitStepIndex[nUnitStep]);
		if (bCheckUnit == TRUE)
		{
			break;
		}
	}

	if (bCheckPanel == TRUE && bCheckStrip == TRUE && bCheckUnit == TRUE)		// Unit 의 Center Point에 TP 출력 
	{
		UnitRect = vecvecRect[nUnitStep][unitidx].drtRect;
		m_stParam.dCenterX = (UnitRect.left + UnitRect.right) / 2.f;
		m_stParam.dCenterY = (UnitRect.top + UnitRect.bottom) / 2.f;
	}

	if (bCheckPanel == TRUE && bCheckStrip == TRUE && bCheckUnit == FALSE)		// Dummy 영역의 중간 지점에 TP 출력
	{
		m_stParam.dCenterX = fX;
		m_stParam.dCenterY = fY;
	}




// 	Surfacepoint _vecCross;
// 
// 	_vecCross.dAddX = m_stParam.dCenterX - 0.1;
// 	_vecCross.dAddY = m_stParam.dCenterY - 0.3;
// 	_vecCross.bOsOc = true;
// 	m_vecPoint.push_back(_vecCross);
// 
// 	_vecCross.dAddX = m_stParam.dCenterX - 0.1;
// 	_vecCross.dAddY = m_stParam.dCenterY - 0.1;
// 	_vecCross.bOsOc = true;
// 	m_vecPoint.push_back(_vecCross);
// 
// 	_vecCross.dAddX = m_stParam.dCenterX - 0.3;
// 	_vecCross.dAddY = m_stParam.dCenterY - 0.1;
// 	_vecCross.bOsOc = true;
// 	m_vecPoint.push_back(_vecCross);
// 
// 	_vecCross.dAddX = m_stParam.dCenterX - 0.3;
// 	_vecCross.dAddY = m_stParam.dCenterY + 0.1;
// 	_vecCross.bOsOc = true;
// 	m_vecPoint.push_back(_vecCross);
// 
// 	_vecCross.dAddX = m_stParam.dCenterX - 0.1;
// 	_vecCross.dAddY = m_stParam.dCenterY + 0.1;
// 	_vecCross.bOsOc = true;
// 	m_vecPoint.push_back(_vecCross);
// 
// 	_vecCross.dAddX = m_stParam.dCenterX - 0.1;
// 	_vecCross.dAddY = m_stParam.dCenterY + 0.3;
// 	_vecCross.bOsOc = true;
// 	m_vecPoint.push_back(_vecCross);
// 
// 	_vecCross.dAddX = m_stParam.dCenterX + 0.1;
// 	_vecCross.dAddY = m_stParam.dCenterY + 0.3;
// 	_vecCross.bOsOc = true;
// 	m_vecPoint.push_back(_vecCross);
// 
// 	_vecCross.dAddX = m_stParam.dCenterX + 0.1;
// 	_vecCross.dAddY = m_stParam.dCenterY + 0.1;
// 	_vecCross.bOsOc = true;
// 	m_vecPoint.push_back(_vecCross);
// 
// 	_vecCross.dAddX = m_stParam.dCenterX + 0.3;
// 	_vecCross.dAddY = m_stParam.dCenterY + 0.1;
// 	_vecCross.bOsOc = true;
// 	m_vecPoint.push_back(_vecCross);
// 
// 	_vecCross.dAddX = m_stParam.dCenterX + 0.3;
// 	_vecCross.dAddY = m_stParam.dCenterY - 0.1;
// 	_vecCross.bOsOc = true;
// 	m_vecPoint.push_back(_vecCross);
// 
// 	_vecCross.dAddX = m_stParam.dCenterX + 0.1;
// 	_vecCross.dAddY = m_stParam.dCenterY - 0.1;
// 	_vecCross.bOsOc = true;
// 	m_vecPoint.push_back(_vecCross);
// 
// 	_vecCross.dAddX = m_stParam.dCenterX + 0.1;
// 	_vecCross.dAddY = m_stParam.dCenterY - 0.3;
// 	_vecCross.bOsOc = true;
// 	m_vecPoint.push_back(_vecCross);
// 
// 	m_nPointIndex = static_cast<int>(m_vecPoint.size());

	

	return RESULT_GOOD;
}


UINT32 CDrawManager::SetTP(IN const CString Layername)
{
	//_SetTp();
	
	vector<CFeature *> vecFeature;

	vecFeature.resize(1);

	CObOe* pObOe = new CObOe;
	pObOe->m_dXbs = m_vecPoint.at(0).dAddX;
	pObOe->m_dYbs = m_vecPoint.at(0).dAddY;
	pObOe->m_bHighlight = true;
	CFeature* pFeature = new CFeatureS;
	CFeatureS *pFeatureS = (CFeatureS *)pFeature;
	pObOe->m_bPolyType = true;

	COsOc* pOsOc = new COsOc;

	for (int i = 1; i < m_nPointIndex; i++)
	{
		if (m_vecPoint.at(i).bOsOc == true)
		{
			COsOc* pOsOc = new COs;
			pOsOc->m_bOsOc = true;
			pOsOc->m_dX = m_vecPoint.at(i).dAddX;
			pOsOc->m_dY = m_vecPoint.at(i).dAddY;
			pObOe->m_arrOsOc.Add(pOsOc);
		}
	}

	pFeatureS->m_arrObOe.Add(pObOe);

	pFeatureS->m_bPolarity = true;

	// 2022.06.07
	// attribute, attrstring hard coding
	CAttribute* pAttribute = new CAttribute;
	pAttribute->m_strName = _T(".string");
	pFeatureS->m_arrAttribute.Add(pAttribute);

	CAttributeTextString* pAttributeString = new CAttributeTextString;
	pAttributeString->m_strText = _T("NONE");
	pFeatureS->m_arrAttributeTextString.Add(pAttributeString);


	vecFeature[0] = pFeatureS;

	CString strLayer = Layername;

	SetAdd(strLayer, vecFeature);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();

	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);



	CDirect_Base::Render();

	m_vecPoint.clear();

	return RESULT_GOOD;

}

UINT32 CDrawManager::SetArc(IN const CString Layername)
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureA();
	CFeatureA *pFeatureA = (CFeatureA *)pFeature;

	pFeatureA->m_bShow = true;

	pFeatureA->m_nDecode = 0;
	pFeatureA->m_bPolarity = true;

	pFeatureA->m_eType = FeatureType::A;
	pFeatureA->m_bHighlight = true;

	if (m_vecPoint.size() <= 0) return RESULT_BAD;

	pFeatureA->m_dXs = m_vecPoint.at(0).dAddX;
	pFeatureA->m_dYs = m_vecPoint.at(0).dAddY;
	pFeatureA->m_dXe = m_vecPoint.at(1).dAddX;
	pFeatureA->m_dYe = m_vecPoint.at(1).dAddY;
	pFeatureA->m_dXc = m_vecPoint.at(1).dCenterX;
	pFeatureA->m_dYc = m_vecPoint.at(1).dCenterY;

	pFeatureA->m_bCw = m_vecPoint.at(1).bCCW;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::r;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;
	pSymbol->m_vcParams.resize(1);
	pSymbol->m_vcParams[0] = 0.03;
	pSymbol->m_strUserSymbol = _T("r1.181");

	pFeatureA->m_pSymbol = pSymbol;

	// 2022.06.07
	// attribute, attrstring hard coding
	CAttribute* pAttribute = new CAttribute;
	pAttribute->m_strName = _T(".string");
	pFeatureA->m_arrAttribute.Add(pAttribute);

	CAttributeTextString* pAttributeString = new CAttributeTextString;
	pAttributeString->m_strText = _T("NONE");
	pFeatureA->m_arrAttributeTextString.Add(pAttributeString);

	vecFeatureData[0] = pFeatureA;

	CString strLayer = Layername;

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();

	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	_vecFeatureData.clear();

	CDirect_Base::Render();

	m_vecPoint.clear();

	return RESULT_GOOD;
}

UINT32 CDrawManager::SetTp(IN const CString Layername)
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureZ();
	CFeatureZ *pFeatureZ = (CFeatureZ*)pFeature;

	pFeatureZ->m_bShow = true;

	pFeatureZ->m_nDecode = 0;
	pFeatureZ->m_bPolarity = true;

	pFeatureZ->m_eType = FeatureType::Z;
	pFeatureZ->m_bHighlight = true;

	pFeatureZ->m_dX = m_stParam.dCenterX;
	pFeatureZ->m_dY = m_stParam.dCenterY;

	pFeatureZ->m_eOrient = Orient::NoMir0Deg;
	

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::null;
	// 	pSymbol->m_dRotatedDeg = 0;

	pSymbol->m_vcParams.resize(1);
	pSymbol->m_vcParams[0] = 0.6;
	pSymbol->m_strUserSymbol = _T("null");

	pSymbol->m_MinMaxRect.bottom = -0.3;
	pSymbol->m_MinMaxRect.top = 0.3;
	pSymbol->m_MinMaxRect.left = -0.3;
	pSymbol->m_MinMaxRect.right = 0.3;

	pFeatureZ->m_pSymbol = pSymbol;


	// 2022.06.07
	// attribute, attrstring hard coding
	CAttribute* pAttribute = new CAttribute;
	pAttribute->m_strName = _T(".string");
	pFeatureZ->m_arrAttribute.Add(pAttribute);

	CAttributeTextString* pAttributeString = new CAttributeTextString;
	pAttributeString->m_strText = _T("NONE");
	pFeatureZ->m_arrAttributeTextString.Add(pAttributeString);


	vecFeatureData[0] = pFeatureZ;

	CString strLayer = Layername;

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();

	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	_vecFeatureData.clear();

	CDirect_Base::Render();

	return RESULT_GOOD;

}


UINT32 CDrawManager::SetLine(IN const CString Layername, IN const int &nLinemode)
{
	if (m_pUserSetInfo == nullptr) return RESULT_BAD;

	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureL();
	CFeatureL *pFeatureL = (CFeatureL *)pFeature;

	pFeatureL->m_bShow = true;

	pFeatureL->m_nDecode = 0;
	pFeatureL->m_bPolarity = true;

	pFeatureL->m_eType = FeatureType::L;
	pFeatureL->m_bHighlight = true;

	pFeatureL->m_dXs = m_stParam.dStartX;
	pFeatureL->m_dYs = m_stParam.dStartY;
	pFeatureL->m_dXe = m_stParam.dEndX;
	pFeatureL->m_dYe = m_stParam.dEndY;
			
	if (nLinemode == AddLineMode::enumLineMode_X)
	{
		if (m_pUserSetInfo->dAngle == 90 || m_pUserSetInfo->dAngle == 270)
		{
			pFeatureL->m_dXs = m_stParam.dStartX;
			pFeatureL->m_dYs = m_stParam.dStartY;
			pFeatureL->m_dXe = m_stParam.dStartX;
			pFeatureL->m_dYe = m_stParam.dEndY;
		}
		else
		{
			pFeatureL->m_dXs = m_stParam.dStartX;
			pFeatureL->m_dYs = m_stParam.dStartY;
			pFeatureL->m_dXe = m_stParam.dEndX;
			pFeatureL->m_dYe = m_stParam.dStartY;
		}
		
	}

	else if (nLinemode == AddLineMode::enumLineMode_Y)
	{
		if (m_pUserSetInfo->dAngle == 90 || m_pUserSetInfo->dAngle == 270)
		{
			pFeatureL->m_dXs = m_stParam.dStartX;
			pFeatureL->m_dYs = m_stParam.dStartY;
			pFeatureL->m_dXe = m_stParam.dEndX;
			pFeatureL->m_dYe = m_stParam.dStartY;
		}
		else
		{
			pFeatureL->m_dXs = m_stParam.dStartX;
			pFeatureL->m_dYs = m_stParam.dStartY;
			pFeatureL->m_dXe = m_stParam.dStartX;
			pFeatureL->m_dYe = m_stParam.dEndY;
		}
		
	}

	else
	{
		pFeatureL->m_dXs = m_stParam.dStartX;
		pFeatureL->m_dYs = m_stParam.dStartY;
		pFeatureL->m_dXe = m_stParam.dEndX;
		pFeatureL->m_dYe = m_stParam.dEndY;
	}

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::r ;
	pSymbol->m_dRotatedDeg = 0;
	
	pSymbol->m_vcParams.resize(1);
	pSymbol->m_vcParams[0] = 0.03;
	pSymbol->m_strUserSymbol = _T("r1.181");


	pFeatureL->m_pSymbol = pSymbol;


	// 2022.06.07
	// attribute, attrstring hard coding
	CAttribute* pAttribute = new CAttribute;
	pAttribute->m_strName = _T(".string");
	pFeatureL->m_arrAttribute.Add(pAttribute);

	CAttributeTextString* pAttributeString = new CAttributeTextString;
	pAttributeString->m_strText = _T("NONE");
	pFeatureL->m_arrAttributeTextString.Add(pAttributeString);


	vecFeatureData[0] = pFeatureL;

	CString strLayer = Layername;

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();

	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	_vecFeatureData.clear();

	CDirect_Base::Render();

	

	return RESULT_GOOD;
}


UINT32 CDrawManager::SetCircle(IN const CString LayerName, IN const double &dDiameter)
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::NoMir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	// 2022.06.07
	// attribute, attrstring hard coding
 	CAttribute* pAttribute = new CAttribute;
 	pAttribute->m_strName = _T(".string");
 	pFeatureP->m_arrAttribute.Add(pAttribute);
 
 	CAttributeTextString* pAttributeString = new CAttributeTextString;
 	pAttributeString->m_strText = _T("NONE");
 	pFeatureP->m_arrAttributeTextString.Add(pAttributeString);

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::r;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;
	pSymbol->m_vcParams.resize(DefSymbol::pParaCount[static_cast<int>(pSymbol->m_eSymbolName)]);
	if (dDiameter <= 0.)
	{
		pSymbol->m_vcParams[0] = 1; // 크기에 대한 입력이 없을 시, 기존 값으로 처리
	}
	else
	{
		pSymbol->m_vcParams[0] = dDiameter;
	}	
	pSymbol->m_strUserSymbol = CDrawFunction::GetSymbolName(pSymbol->m_eSymbolName, pSymbol->m_vcParams);
	
	pSymbol->SetMinMaxRect();

	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;
	
	CString strLayer = LayerName;

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();

		
	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);
	
	
	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();

	return RESULT_GOOD;
}


UINT32 CDrawManager::SetSquare(IN const CString Layername, IN const double &dWidth)
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::NoMir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::s;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;

	pSymbol->m_vcParams.resize(DefSymbol::pParaCount[static_cast<int>(pSymbol->m_eSymbolName)]);
	if (dWidth <= 0.)
	{
		pSymbol->m_vcParams[0] = 1; // 크기에 대한 입력이 없을 시, 기존 값으로 처리
	}
	else
	{
		pSymbol->m_vcParams[0] = dWidth;
	}
	pSymbol->m_strUserSymbol = CDrawFunction::GetSymbolName(pSymbol->m_eSymbolName, pSymbol->m_vcParams);
	pSymbol->SetMinMaxRect();

	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = Layername;

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);
		

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}

UINT32 CDrawManager::SetRect(IN const CString LayerName, IN const double &dWidth, IN const double &dHeight)
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::NoMir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	// 2022.06.07
	// attribute, attrstring hard coding
	CAttribute* pAttribute = new CAttribute;
	pAttribute->m_strName = _T(".string");
	pFeatureP->m_arrAttribute.Add(pAttribute);

	CAttributeTextString* pAttributeString = new CAttributeTextString;
	pAttributeString->m_strText = _T("NONE");
	pFeatureP->m_arrAttributeTextString.Add(pAttributeString);


	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName:: rect;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;

	pSymbol->m_vcParams.resize(DefSymbol::pParaCount[static_cast<int>(pSymbol->m_eSymbolName)]);
	if (dWidth <= 0. || dHeight <= 0.)
	{
		// 크기에 대한 입력이 없을 시, 기존 값으로 처리
		pSymbol->m_vcParams[0] = 0.2;	//width
		pSymbol->m_vcParams[1] = 0.3;	//height
	}
	else
	{
		pSymbol->m_vcParams[0] = dWidth;	//width
		pSymbol->m_vcParams[1] = dHeight;	//height
	}
	pSymbol->m_strUserSymbol = CDrawFunction::GetSymbolName(pSymbol->m_eSymbolName, pSymbol->m_vcParams);	

	pSymbol->SetMinMaxRect();	

	pFeatureP->m_pSymbol = pSymbol;
	
	
	/*pFeatureP->SetMinMaxRect();
	
	pFeatureP->m_MinMaxRect.left = pFeature->m_MinMaxRect.left - pSymbol->m_MinMaxRect.left;
	pFeatureP->m_MinMaxRect.right = pFeature->m_MinMaxRect.right + pSymbol->m_MinMaxRect.right;
	pFeatureP->m_MinMaxRect.top = pFeature->m_MinMaxRect.top - pSymbol->m_MinMaxRect.top;
	pFeatureP->m_MinMaxRect.bottom = pFeature->m_MinMaxRect.bottom - pSymbol ->m_MinMaxRect.bottom;*/


	vecFeatureData[0] = pFeatureP;

	CString strLayer = LayerName;

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}

UINT32 CDrawManager::SetRectxr()
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::rectxr;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;


	pSymbol->m_vcParams.resize(4);

	pSymbol->m_vcParams[0] = 6;
	pSymbol->m_vcParams[1] = 3;
	pSymbol->m_vcParams[2] = 1;
	pSymbol->m_vcParams[3] = 13;
	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = _T("L01_MP");

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}

UINT32 CDrawManager::SetRectxc()
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::rectxc;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;


	pSymbol->m_vcParams.resize(4);

	pSymbol->m_vcParams[0] = 6;
	pSymbol->m_vcParams[1] = 3;
	pSymbol->m_vcParams[2] = 1;
	pSymbol->m_vcParams[3] = 13;
	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = _T("L01_MP");

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}

UINT32 CDrawManager::SetOval()
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::oval;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;


	pSymbol->m_vcParams.resize(2);

	pSymbol->m_vcParams[0] = 6;
	pSymbol->m_vcParams[1] = 3;
	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = _T("L01_MP");

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}

UINT32 CDrawManager::SetDi()
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::di;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;


	pSymbol->m_vcParams.resize(2);

	pSymbol->m_vcParams[0] = 6;
	pSymbol->m_vcParams[1] = 3;
	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = _T("L01_MP");

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}

UINT32 CDrawManager::SetOct()
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::octagon;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;


	pSymbol->m_vcParams.resize(3);

	pSymbol->m_vcParams[0] = 6;
	pSymbol->m_vcParams[1] = 6;
	pSymbol->m_vcParams[2] = 2;
	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = _T("L01_MP");

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}

UINT32 CDrawManager::SetDonut_r()
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::donut_r;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;


	pSymbol->m_vcParams.resize(2);

	pSymbol->m_vcParams[0] = 6;
	pSymbol->m_vcParams[1] = 3;
	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = _T("L01_MP");

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();

	return RESULT_GOOD;
}

UINT32 CDrawManager::SetDonut_s()
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::donut_s;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;


	pSymbol->m_vcParams.resize(2);

	pSymbol->m_vcParams[0] = 6;
	pSymbol->m_vcParams[1] = 3;
	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = _T("L01_MP");

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}
UINT32 CDrawManager::SetHex_l()
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::hex_l;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;


	pSymbol->m_vcParams.resize(3);

	pSymbol->m_vcParams[0] = 6;
	pSymbol->m_vcParams[1] = 6;
	pSymbol->m_vcParams[2] = 2;
	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = _T("L01_MP");

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}

UINT32 CDrawManager::SetHex_s()
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::hex_s;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;


	pSymbol->m_vcParams.resize(3);

	pSymbol->m_vcParams[0] = 6;
	pSymbol->m_vcParams[1] = 6;
	pSymbol->m_vcParams[2] = 2;
	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = _T("L01_MP");

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}

UINT32 CDrawManager::SetBfr()
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::bfr;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;


	pSymbol->m_vcParams.resize(1);

	pSymbol->m_vcParams[0] = 6;
	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = _T("L01_MP");

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}
UINT32 CDrawManager::SetBfs()
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::bfs;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;


	pSymbol->m_vcParams.resize(1);

	pSymbol->m_vcParams[0] = 6;
	
	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = _T("L01_MP");

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}


UINT32 CDrawManager::SetTri()
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::tri;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;


	pSymbol->m_vcParams.resize(2);

	pSymbol->m_vcParams[0] = 6;
	pSymbol->m_vcParams[1] = 3;
	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = _T("L01_MP");

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}

UINT32 CDrawManager::SetOval_h()
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::oval_h;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;


	pSymbol->m_vcParams.resize(2);

	pSymbol->m_vcParams[0] = 6;
	pSymbol->m_vcParams[1] = 3;
	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = _T("L01_MP");

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}

UINT32 CDrawManager::SetThr()
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::thr;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;


	pSymbol->m_vcParams.resize(5);

	pSymbol->m_vcParams[0] = 6;
	pSymbol->m_vcParams[1] = 4;
	pSymbol->m_vcParams[2] = 45;
	pSymbol->m_vcParams[3] = 4;
	pSymbol->m_vcParams[4] = 0.5;
	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = _T("L01_MP");

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}

UINT32 CDrawManager::SetThs()
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::ths;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;


	pSymbol->m_vcParams.resize(5);

	pSymbol->m_vcParams[0] = 6;
	pSymbol->m_vcParams[1] = 4;
	pSymbol->m_vcParams[2] = 45;
	pSymbol->m_vcParams[3] = 4;
	pSymbol->m_vcParams[4] = 1;
	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = _T("L01_MP");

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}

UINT32 CDrawManager::SetS_Ths()
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::s_ths;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;


	pSymbol->m_vcParams.resize(5);

	pSymbol->m_vcParams[0] = 6;
	pSymbol->m_vcParams[1] = 4;
	pSymbol->m_vcParams[2] = 45;
	pSymbol->m_vcParams[3] = 4;
	pSymbol->m_vcParams[4] = 1;
	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = _T("L01_MP");

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}

UINT32 CDrawManager::SetS_Tho()
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::s_tho;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;


	pSymbol->m_vcParams.resize(5);

	pSymbol->m_vcParams[0] = 6;
	pSymbol->m_vcParams[1] = 4;
	pSymbol->m_vcParams[2] = 45;
	pSymbol->m_vcParams[3] = 4;
	pSymbol->m_vcParams[4] = 1;
	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = _T("L01_MP");

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}

UINT32 CDrawManager::SetSr_Ths()
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::sr_ths;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;


	pSymbol->m_vcParams.resize(5);

	pSymbol->m_vcParams[0] = 6;
	pSymbol->m_vcParams[1] = 4;
	pSymbol->m_vcParams[2] = 45;
	pSymbol->m_vcParams[3] = 4;
	pSymbol->m_vcParams[4] = 1;
	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = _T("L01_MP");

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}

UINT32 CDrawManager::SetRc_Ths()
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::rc_ths;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;


	pSymbol->m_vcParams.resize(6);

	pSymbol->m_vcParams[0] = 6;
	pSymbol->m_vcParams[1] = 4;
	pSymbol->m_vcParams[2] = 45;
	pSymbol->m_vcParams[3] = 4;
	pSymbol->m_vcParams[4] = 1;
	pSymbol->m_vcParams[5] = 1;

	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = _T("L01_MP");

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}

UINT32 CDrawManager::SetRc_Tho()
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::rc_tho;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;


	pSymbol->m_vcParams.resize(6);

	pSymbol->m_vcParams[0] = 6;
	pSymbol->m_vcParams[1] = 4;
	pSymbol->m_vcParams[2] = 45;
	pSymbol->m_vcParams[3] = 4;
	pSymbol->m_vcParams[4] = 1;
	pSymbol->m_vcParams[5] = 1;

	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = _T("L01_MP");

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}

UINT32 CDrawManager::SetEl()
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::Mir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::el;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;


	pSymbol->m_vcParams.resize(2);

	pSymbol->m_vcParams[0] = 6;
	pSymbol->m_vcParams[1] = 4;

	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = _T("L01_MP");

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}


UINT32 CDrawManager::SetMorie(IN const CString LayerName)
{
	vector<CFeature*> vecFeatureData;

	vecFeatureData.resize(1);

	CFeature *pFeature = new CFeatureP();
	CFeatureP *pFeatureP = (CFeatureP *)pFeature;

	pFeatureP->m_bShow = true;

	pFeatureP->m_nDecode = 0;
	pFeatureP->m_bPolarity = TRUE;

	pFeatureP->m_eOrient = Orient::NoMir0Deg;

	pFeatureP->m_dX = m_stParam.dCenterX;
	pFeatureP->m_dY = m_stParam.dCenterY;

	pFeatureP->m_eType = FeatureType::P;
	pFeatureP->m_bHighlight = true;

	CSymbol* pSymbol = new CSymbol();
	pSymbol->m_eSymbolName = SymbolName::moire;
	pSymbol->m_dRotatedDeg = 0;
	pSymbol->m_pUserSymbol = nullptr;


	pSymbol->m_vcParams.resize(6);

	pSymbol->m_vcParams[0] = 0.01;
	pSymbol->m_vcParams[1] = 0;
	pSymbol->m_vcParams[2] = 0;
	pSymbol->m_vcParams[3] = 0.01;
	pSymbol->m_vcParams[4] = 0.03;
	pSymbol->m_vcParams[5] = 0;
	pFeatureP->m_pSymbol = pSymbol;

	vecFeatureData[0] = pFeatureP;

	CString strLayer = LayerName;

	SetAdd(strLayer, vecFeatureData);

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	vector<CFeatureData*> _vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();


	UINT32 nRet;
	nRet = _SetFeature_Add(_vecFeatureData, pStep);

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();


	return RESULT_GOOD;
}

UINT32 CDrawManager::SetHole()
{
	return RESULT_GOOD;
}

UINT32 CDrawManager::GetFeatureData(vector<CFeatureData *>& vecFeatureData, CStep *pStep)
{
	
	int nDataSize = static_cast<int>(vecFeatureData.size());

	if (nDataSize <= 0) return RESULT_BAD;
	if (pStep == nullptr) return RESULT_BAD;

	CString strTemp;

	for (int nfeno = 0; nfeno < nDataSize; nfeno++)
	{
		CFeatureData *pFeatureData = vecFeatureData[nfeno];

		CLayer *pLayer = pStep->m_arrLayer.GetAt(pFeatureData->m_nLayer);
		if (pLayer == nullptr) continue;

		CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(pFeatureData->m_nFeatureIndex);

		switch (pFeature->m_eType)
		{
		case::FeatureType::S:
		{
			CFeatureS *pFeatureS_Edit = (CFeatureS *)pFeatureData->m_pFeature;
			FeatureSpec.strFeature = _T("Surface");
			int last = static_cast<int>(pFeatureS_Edit->m_arrAttribute.GetCount() - 1);
			if (last >= 0)
			{
				FeatureSpec.strAttr = pFeatureS_Edit->m_arrAttribute.GetAt(last)->m_strName;
			}
			int attribute_last = static_cast<int>(pFeatureS_Edit->m_arrAttributeTextString.GetCount() - 1);
			if (attribute_last >= 0)
			{
				FeatureSpec.strAttrString = pFeatureS_Edit->m_arrAttributeTextString.GetAt(last)->m_strText;
			}
		}
		break;

		case::FeatureType::L:
		{
			CFeatureL *pFeatureL_Edit = (CFeatureL *)pFeatureData->m_pFeature;
			FeatureSpec.strFeature = _T("Line");
			FeatureSpec.nShape = static_cast<int>(pFeatureL_Edit->m_pSymbol->m_eSymbolName);
			FeatureSpec.dWidth = pFeatureL_Edit->m_pSymbol->m_vcParams[0];

			int last = static_cast<int>(pFeatureL_Edit->m_arrAttribute.GetCount() - 1);
			if (last >= 0)
			{
				FeatureSpec.strAttr = pFeatureL_Edit->m_arrAttribute.GetAt(last)->m_strName;
				
			}
			int attribute_last = static_cast<int>(pFeatureL_Edit->m_arrAttributeTextString.GetCount() - 1);
			if (attribute_last >= 0)
			{
				FeatureSpec.strAttrString = pFeatureL_Edit->m_arrAttributeTextString.GetAt(last)->m_strText;
			}
		}
		break;

		case::FeatureType::A:
		{
			CFeatureA *pFeatureA_Edit = (CFeatureA *)pFeatureData->m_pFeature;
			FeatureSpec.strFeature = _T("Arc");
			FeatureSpec.dWidth = pFeatureA_Edit->m_pSymbol->m_vcParams[0];

			int last = static_cast<int>(pFeatureA_Edit->m_arrAttribute.GetCount() - 1);
			if (last >= 0)
			{
				FeatureSpec.strAttr = pFeatureA_Edit->m_arrAttribute.GetAt(last)->m_strName;
			}
			int attribute_last = static_cast<int>(pFeatureA_Edit->m_arrAttributeTextString.GetCount() - 1);
			if (attribute_last >= 0)
			{
				FeatureSpec.strAttrString = pFeatureA_Edit->m_arrAttributeTextString.GetAt(last)->m_strText;
			}
		}

		break;
		case FeatureType::P:
		{
			CFeatureP *pFeatureP_Edit = (CFeatureP *)pFeatureData->m_pFeature;
			FeatureSpec.strFeature = _T("Pad");

			switch (pFeatureP_Edit->m_pSymbol->m_eSymbolName)
			{


			case SymbolName::r:
			{
				FeatureSpec.dRadius = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
				FeatureSpec.nShape = static_cast<int>(pFeatureP_Edit->m_pSymbol->m_eSymbolName);
				
				int last = static_cast<int>(pFeatureP_Edit->m_arrAttribute.GetCount()-1);
				if (last >= 0)
				{
					FeatureSpec.strAttr = pFeatureP_Edit->m_arrAttribute.GetAt(last)->m_strName;					
				}
				int attribute_last = static_cast<int>(pFeatureP_Edit->m_arrAttributeTextString.GetCount() - 1);
				if (attribute_last >= 0)
				{
					FeatureSpec.strAttrString = pFeatureP_Edit->m_arrAttributeTextString.GetAt(last)->m_strText;
				}
			}
			break;
			case SymbolName::rect:
			{
				FeatureSpec.dWidth = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
				FeatureSpec.dHeight = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
				FeatureSpec.nShape = static_cast<int>(pFeatureP_Edit->m_pSymbol->m_eSymbolName);

				int last = static_cast<int>(pFeatureP_Edit->m_arrAttribute.GetCount() - 1);
				if (last >= 0)
				{
					FeatureSpec.strAttr = pFeatureP_Edit->m_arrAttribute.GetAt(last)->m_strName;

				}
				int attribute_last = static_cast<int>(pFeatureP_Edit->m_arrAttributeTextString.GetCount() - 1);
				if (attribute_last >= 0)
				{
					FeatureSpec.strAttrString = pFeatureP_Edit->m_arrAttributeTextString.GetAt(last)->m_strText;
				}
				
			}

			break;


			}

			if (pFeatureP_Edit == nullptr) continue;
			switch (pFeatureP_Edit->m_eOrient)
			{
			case Orient::Mir0Deg:
			{
				FeatureSpec.strRotate = "0";
				FeatureSpec.strMirror = "YES";

			}
			break;
			case Orient::Mir90Deg:
			{
				FeatureSpec.strRotate = "90";
				FeatureSpec.strMirror = "Yes";
			}
			break;
			case Orient::Mir180Deg:
			{
				FeatureSpec.strRotate = "180";
				FeatureSpec.strMirror = "Yes";
			}
			break;
			case Orient::Mir270Deg:
			{
				FeatureSpec.strRotate = "270";
				FeatureSpec.strMirror = "Yes";
			}
			break;
			case Orient::NoMir0Deg:
			{
				FeatureSpec.strRotate = "0";
				FeatureSpec.strMirror = "No";
			}
			break;
			case Orient::NoMir90Deg:
			{
				FeatureSpec.strRotate = "90";
				FeatureSpec.strMirror = "No";
			}
			break;
			case Orient::NoMir180Deg:
			{
				FeatureSpec.strRotate = "180";
				FeatureSpec.strMirror = "No";
			}
			break;
			case Orient::NoMir270Deg:
			{
				FeatureSpec.strRotate = "270";
				FeatureSpec.strMirror = "No";
			}
			break;
			}
			
		}
		}
	}
	
	return RESULT_GOOD;

}

UINT32 CDrawManager::SetFeatureData()
{
	if (m_pDrawEdit_Temp == nullptr) return RESULT_BAD;
	if (m_nSelectStep == -1 ) return RESULT_BAD;

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr) return RESULT_BAD ;

	//m_pDrawEdit_Temp->SetFeatureData(pStep);

	vector<CFeatureData*> vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();

	GetFeatureData(vecFeatureData, pStep);


	return RESULT_GOOD;
}

UINT32 CDrawManager::Reset()
{
	if (m_pDrawEdit_Temp->m_bMoved == TRUE)
	{
		m_pDrawEdit_Temp->m_bMoved = FALSE;
	}

	if (m_pDrawEdit_Temp->m_bResize == TRUE)
	{
		m_pDrawEdit_Temp->m_bResize = FALSE;
	}
	
	m_bKeyMode = FALSE;
	m_KeyMove.x = 0;
	m_KeyMove.y = 0;


	m_pDrawEdit_Temp->m_totalMove.x = 0;
	m_pDrawEdit_Temp->m_totalMove.y = 0;

	m_pDrawEdit_Temp->ClearFeatureData();

	
	return RESULT_GOOD;
}

BOOL CDrawManager::IsMove()
{
	FeatureSpec.bDragMode = m_pDrawEdit_Temp->m_bMoved;
	return m_pDrawEdit_Temp->m_bMoved;
}

BOOL CDrawManager::IsResize()
{
	return m_pDrawEdit_Temp->m_bResize;
}

BOOL CDrawManager::IsEditBox()
{
	return m_pDrawEdit_Temp->m_bEditPoint;
}

BOOL CDrawManager::Hit_Test()
{
	if (m_pDrawEdit_Temp == nullptr) return RESULT_BAD;
	//if (m_pUserLayerSetInfo == nullptr) return RESULT_BAD;
	if (m_pUserLayerSetInfo == nullptr) return FALSE;


	bool bSelect = FALSE;
	BOOL bSelected = FALSE;

	//m_pDrawEdit_Temp->ClearFeatureData();

	//Get Layer Index;
	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr) return RESULT_BAD;

	//Get Select Layer
	vector<CString> vecLayerName;
	for (int i = 0; i < m_pUserLayerSetInfo->vcLayerSet.size(); i++)
	{
		if (m_pUserLayerSetInfo->vcLayerSet[i]->bCheck == true)
		{
			vecLayerName.push_back(m_pUserLayerSetInfo->vcLayerSet[i]->strLayer);
		}
	}

	int nLayerCount = static_cast<int>(vecLayerName.size());
	for (int i = 0; i < nLayerCount; i++)
	{
		//체크 되어 있는 Layer 이름
		CString strLayer = vecLayerName[i];

		//Get Layer Ptr
		int nLayer = -1;
		CLayer *pLayerTmp = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, strLayer, nLayer);
		if (pLayerTmp == nullptr) continue;

		int nSurface_ObObIndex = -1;
		int nAddIndex = 0;
		int nFeatureCount = static_cast<int>(pLayerTmp->m_FeatureFile.m_arrFeature.GetCount());
		for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
		{
			CFeature *pFeature = pLayerTmp->m_FeatureFile.m_arrFeature.GetAt(nfeno);

			if (pFeature == nullptr) continue;

			
			switch (pFeature->m_eType)
			{
			case FeatureType::Z:
			{
				CFeatureZ *pFeatureZ = (CFeatureZ *)pFeature;
				bSelect = pFeatureZ->m_bHighlight;
			}
			break;
			case FeatureType::L:
			{
				CFeatureL *pFeatureL = (CFeatureL *)pFeature;
				bSelect = pFeatureL->m_bHighlight;
			}
			break;
			case FeatureType::P:
			{
				CFeatureP *pFeatureP = (CFeatureP *)pFeature;
				bSelect = pFeatureP->m_bHighlight;
			}
			break;
			case FeatureType::A:
			{
				CFeatureA *pFeatureA = (CFeatureA *)pFeature;
				bSelect = pFeatureA->m_bHighlight;
			}
			break;
			case FeatureType::T:
			{
				CFeatureT *pFeatureT = (CFeatureT *)pFeature;
				bSelect = pFeatureT->m_bHighlight;
			}
			break;
			case FeatureType::S:
			{
				CFeatureS *pFeatureS = (CFeatureS *)pFeature;
				//ObOe중 하나라도 선택이 되어 있는지 확인
				int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
				for (int nObOe = 0; nObOe < nObOeCount; nObOe++)
				{
					CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nObOe);
					if (pObOe == nullptr) continue;

					if (pObOe->m_bHighlight == true)
					{
						nSurface_ObObIndex = nObOe;
						bSelect = TRUE;
						break;
					}
				}
				//
			}
			break;
			}

			if (bSelect == FALSE) continue;

			 bSelected = TRUE;
		}

	}

	return bSelected;
}


UINT32 CDrawManager::SetSystemSpec(SystemSpec_t* pSystemSpec)
{
	CDirect_Base::SetSystemSpec(pSystemSpec);

	if (m_pDirectView == nullptr) return RESULT_BAD;
	
	UINT32 nRet = m_pDirectView->SetSystemSpec(pSystemSpec);
	

	return nRet;
}

UINT32 CDrawManager::SetUserSetInfo(UserSetInfo_t *pUserSetInfo, UserLayerSet_t *pUserLayerSetInfo)
{
	CDirect_Base::SetUserSetInfo(pUserSetInfo, pUserLayerSetInfo);

	if (m_pDirectView == nullptr) return RESULT_BAD;
	
	UINT32 nRet;
	nRet  = m_pDirectView->SetUserSetInfo(pUserSetInfo, pUserLayerSetInfo);
	nRet |= m_pDrawEdit_Temp->SetUserSetInfo(pUserSetInfo, pUserLayerSetInfo);
	nRet |= m_pDrawProfile->SetUserSetInfo(pUserSetInfo, pUserLayerSetInfo);
	nRet |= m_pDrawFov->SetUserSetInfo(pUserSetInfo, pUserLayerSetInfo);

	SetStepIndex(m_pUserSetInfo->iStepIdx);

	return nRet;
}

UINT32 CDrawManager::SetDrawOnly_MeasureUnit(IN const BOOL &bMeaureUnit)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = m_pDirectView->SetDrawOnly_MeasureUnit(bMeaureUnit);

	CDirect_Base::Render();

	return nRet;
}

UINT32 CDrawManager::Initialize(IN CJobFile* pCAMData, IN ViewInfo_t* pViewInfo, IN PanelInfo_t* pstPanelInfo, IN FovInfo_t* pFovInfo, IN CellInfo_t* pCellInfo)
{
	if (pCAMData == nullptr)		return RESULT_BAD;
	if (pViewInfo == nullptr)		return RESULT_BAD;
	if (pstPanelInfo == nullptr)	return RESULT_BAD;
	if (pFovInfo == nullptr)		return RESULT_BAD;
	if (pCellInfo == nullptr)		return RESULT_BAD;

	m_pJobData = pCAMData;

	if (RESULT_GOOD == CDirect_Base::Initialize(pCAMData, pViewInfo))
	{
		if (m_pDirectView == nullptr) return RESULT_BAD;

		if (RESULT_GOOD != m_pDirectView->Initialize(m_pFactory, m_pD2DContext, m_vecRender, m_vecRender_Substep,
			pCAMData, pViewInfo, pstPanelInfo, pFovInfo, pCellInfo))
			return RESULT_BAD;

		m_pDrawProfile->SetLink(m_pFactory, m_pD2DContext);
		
		m_pDrawEdit_Temp->SetLink(m_pFactory, m_pD2DContext, this);
		m_pDrawEdit_Temp->SetViewInfo(pViewInfo);

		m_pDrawFov->SetLink(m_pFactory, m_pD2DContext, pFovInfo, pCellInfo);

		m_pFeatureFilter->SetLink(m_pFactory, m_pD2DContext);

		return RESULT_GOOD;
	}
	else
	{
		return RESULT_BAD;
	}
}

UINT32 CDrawManager::SetViewInfo(IN ViewInfo_t* pViewInfo)
{
	if (pViewInfo == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = CDirect_Base::SetViewInfo(pViewInfo);
	m_pDrawEdit_Temp->SetViewInfo(pViewInfo);

	if (m_pDirectView == nullptr) return RESULT_BAD;
	if (nRet == RESULT_GOOD)
	{
		m_pDirectView->SetRenderPtr(m_vecRender, m_vecRender_Substep);
		CDirect_Base::Render();

		ResetZoom();
	}

	return nRet;
}

UINT32 CDrawManager::ResetView()
{
	SetStepIndex(-1);
	
	if (m_pDirectView != nullptr)
	{
		m_pDirectView->ResetView();
	}

	if (m_pDrawProfile != nullptr)
	{
		m_pDrawProfile->ResetView();
	}

	if (m_pDrawFov != nullptr)
	{
		m_pDrawFov->ResetView();
	}

	return RESULT_GOOD;
}


UINT32 CDrawManager::SetStepIndex(int nStepIndex)
{
	m_nSelectStep = nStepIndex;
	if (nStepIndex == -1) return RESULT_BAD;

	if (m_pDirectView == nullptr) return RESULT_BAD;
	
	UINT32 nRet;
	nRet = m_pDirectView->SetStepIndex(nStepIndex);

	CStep* pStep = m_pJobData->m_arrStep.GetAt(nStepIndex);
	if (pStep == nullptr) return RESULT_BAD;

	D2D1_RECT_F fptPicture = D2D1::RectF(DoubleToFloat(pStep->m_FeatureMinMax.left), DoubleToFloat(pStep->m_FeatureMinMax.top),
		DoubleToFloat(pStep->m_FeatureMinMax.right), DoubleToFloat(pStep->m_FeatureMinMax.bottom));
	
	if (m_pSystemSpec && m_pSystemSpec->sysBasic.bProfileMode)
	{
		fptPicture = D2D1::RectF(DoubleToFloat(pStep->m_Profile.m_MinMax.left), DoubleToFloat(pStep->m_Profile.m_MinMax.top),
					DoubleToFloat(pStep->m_Profile.m_MinMax.right), DoubleToFloat(pStep->m_Profile.m_MinMax.bottom));
	}

	SetPictureBox(fptPicture);

	return nRet;
}


UINT32 CDrawManager::ZoomIn()
{
	CPoint ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(*(m_pViewInfo->pHwnd), &ptMouse);

	float newScale = static_cast<float>(m_matScaleView.m11* 1.1f);

	float foldPanX_mm = m_fptPan.x / m_matScaleView.m11;
	float foldPanY_mm = m_fptPan.y / m_matScaleView.m11;
	D2D_POINT_2F fptMouse_mm;
	fptMouse_mm.x = ptMouse.x / m_matScaleView.m11;
	fptMouse_mm.y = ptMouse.y / m_matScaleView.m11;

	m_fptPan.x = (foldPanX_mm - fptMouse_mm.x) * newScale + ptMouse.x;
	m_fptPan.y = (foldPanY_mm - fptMouse_mm.y) * newScale + ptMouse.y;

	m_matScaleView = D2D1::Matrix3x2F::Scale(newScale, newScale, D2D1::Point2F(0.f, 0.f));

	return RESULT_GOOD;
}

UINT32 CDrawManager::ZoomOut()
{
	CPoint ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(*(m_pViewInfo->pHwnd), &ptMouse);

	float newScale = static_cast<float>(m_matScaleView.m11* 0.9f);

	float foldPanX_mm = m_fptPan.x / m_matScaleView.m11;
	float foldPanY_mm = m_fptPan.y / m_matScaleView.m11;
	D2D_POINT_2F fptMouse_mm;
	fptMouse_mm.x = ptMouse.x / m_matScaleView.m11;
	fptMouse_mm.y = ptMouse.y / m_matScaleView.m11;

	m_fptPan.x = (foldPanX_mm - fptMouse_mm.x) * newScale + ptMouse.x;
	m_fptPan.y = (foldPanY_mm - fptMouse_mm.y) * newScale + ptMouse.y;

	m_matScaleView = D2D1::Matrix3x2F::Scale(newScale, newScale, D2D1::Point2F(0.f, 0.f));

	return RESULT_GOOD;
}

UINT32 CDrawManager::ResetZoom()
{
	D2D1_RECT_F frtPictureBox = m_frtPictureBox;
	
	frtPictureBox = CDrawFunction::NormalizeRect(frtPictureBox);

	//Draw Margin
	double dWidth = fabs(frtPictureBox.right - frtPictureBox.left);
	double dHeight = fabs(frtPictureBox.bottom - frtPictureBox.top);
	double dMargin = 0.05;//%
	frtPictureBox.left -= static_cast<float>(dWidth * dMargin);
	frtPictureBox.top -= static_cast<float>(dHeight * dMargin);
	frtPictureBox.right += static_cast<float>(dWidth * dMargin);
	frtPictureBox.bottom += static_cast<float>(dHeight * dMargin);

	frtPictureBox = CDrawFunction::NormalizeRect(frtPictureBox);
	//

	_SetZoomRect(frtPictureBox);

	return RESULT_GOOD;
}

UINT32 CDrawManager::DrawZoomRect(IN const RECTD &drtRect)
{
	D2D1_RECT_F frtRect = D2D1::RectF(static_cast<float>(drtRect.left),
		static_cast<float>(drtRect.top), static_cast<float>(drtRect.right), static_cast<float>(drtRect.bottom));
	
	_SetZoomRect(frtRect);

	return RESULT_GOOD;
}

void CDrawManager::_SetZoomRect(IN const CRect& rtRect)
{
	CRect rtClient = m_pViewInfo->rcRect;
	D2D1_RECT_F frtRect_Pixel = D2D1::RectF(static_cast<float>(rtRect.left), static_cast<float>(rtRect.top),
		static_cast<float>(rtRect.right), static_cast<float>(rtRect.bottom));
	D2D1_RECT_F frtRect_mm = CDrawFunction::DetansformRect(frtRect_Pixel, m_ViewScale, m_pUserSetInfo, TRUE);

	_SetZoomRect(frtRect_mm);
}

void CDrawManager::_SetZoomRect(IN const D2D1_RECT_F& frtRect_mm)
{
	if (m_pUserSetInfo == nullptr) return;

	CRect rtClient = m_pViewInfo->rcRect;

	float fRectWidth = fabs(frtRect_mm.right - frtRect_mm.left);
	float fRectHeight = fabs(frtRect_mm.bottom - frtRect_mm.top);

	//
	double dAngle = m_pUserSetInfo->dAngle;
	bool bMirror =  m_pUserSetInfo->bMirror;



	Orient ori = Orient::NoMir0Deg;
	CDrawFunction::GetOrient(dAngle, bMirror, ori);

	CRotMirrArr rotMirrArr;
	rotMirrArr.InputData(0.0, 0.0, ori);

	double dRectWidth, dRectHeight;
	rotMirrArr.FinalPoint(&dRectWidth, &dRectHeight, (double)fRectWidth, (double)fRectHeight);
	fRectWidth = static_cast<float>(fabs(dRectWidth));
	fRectHeight = static_cast<float>(fabs(dRectHeight));
	//
	
	float fScaleX = fabs(rtClient.Width() / fRectWidth);
	float fScaleY = fabs(rtClient.Height() / fRectHeight);
	float fScale = fScaleX <= fScaleY ? fScaleX : fScaleY;

	m_matScaleView = D2D1::Matrix3x2F(fScale, 0, 0, fScale, 0, 0);
	SetViewScale();

	D2D1_POINT_2F fptCenter = CDrawFunction::GetRectCenter(frtRect_mm);

	D2D1::Matrix3x2F matViewScale = D2D1::Matrix3x2F(m_ViewScale.m11, m_ViewScale.m12, m_ViewScale.m21, m_ViewScale.m22, 0, 0);
	D2D1_POINT_2F fptCenterT = matViewScale.TransformPoint(fptCenter);

	m_fptPan.x = (-1.0f*fptCenterT.x) + rtClient.Width() / 2.f;
	m_fptPan.y = (-1.0f*fptCenterT.y) + rtClient.Height() / 2.f;

	CDirect_Base::Render();
}

void CDrawManager::SetViewScale()
{
	if (m_pUserSetInfo == nullptr) return;

	double dAngle = m_pUserSetInfo->dAngle;
	bool bMirror = m_pUserSetInfo->bMirror;

	D2D1::Matrix3x2F matRotate = D2D1::Matrix3x2F::Rotation(static_cast<float>(dAngle),
		CDrawFunction::GetRectCenter(m_frtPictureBox));

	D2D1::Matrix3x2F matTotal = m_matScaleView * matRotate;

	if (bMirror)
	{
		if (m_pUserSetInfo->mirrorDir == eDirectionType::eBoth)
		{
			m_ViewScale = D2D1::Matrix3x2F(matTotal.m11 * -1.f, matTotal.m12* -1.f, matTotal.m21*-1.0f, matTotal.m22*-1.0f, m_fptPan.x, m_fptPan.y);
			m_ViewScale = D2D1::Matrix3x2F(m_ViewScale.m11, m_ViewScale.m12, m_ViewScale.m21*-1.0f, m_ViewScale.m22*-1.0f, m_fptPan.x, m_fptPan.y);
		}
		else if (m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal)
		{
			if (dAngle == 90.0 || dAngle == 270.0)
				m_ViewScale = D2D1::Matrix3x2F(matTotal.m11, matTotal.m12, matTotal.m21*1.0f, matTotal.m22*1.0f, m_fptPan.x, m_fptPan.y);
			else
				m_ViewScale = D2D1::Matrix3x2F(matTotal.m11 * -1.f, matTotal.m12* -1.f, matTotal.m21*-1.0f, matTotal.m22*-1.0f, m_fptPan.x, m_fptPan.y);
		}
		else if (m_pUserSetInfo->mirrorDir == eDirectionType::eVertical)
		{
			if (dAngle == 90.0 || dAngle == 270.0)
				m_ViewScale = D2D1::Matrix3x2F(matTotal.m11 * -1.f, matTotal.m12* -1.f, matTotal.m21*-1.0f, matTotal.m22*-1.0f, m_fptPan.x, m_fptPan.y);
			else
			{
// 				if (m_pUserSetInfo->mcType == eMachineType::eNSIS)
// 				{
// 					m_ViewScale = D2D1::Matrix3x2F(matTotal.m11, matTotal.m12, matTotal.m21*-1.0f, matTotal.m22*-1.0f, m_fptPan.x, m_fptPan.y);
// 				}
// 				else
				{
					m_ViewScale = D2D1::Matrix3x2F(matTotal.m11, matTotal.m12, matTotal.m21*1.0f, matTotal.m22*1.0f, m_fptPan.x, m_fptPan.y);
				}
				
			}
		}
		else
		{
			m_ViewScale = D2D1::Matrix3x2F(matTotal.m11, matTotal.m12, matTotal.m21*-1.0f, matTotal.m22*-1.0f, m_fptPan.x, m_fptPan.y);
		}
	}
	else
	{
		m_ViewScale = D2D1::Matrix3x2F(matTotal.m11, matTotal.m12, matTotal.m21*-1.0f, matTotal.m22*-1.0f, m_fptPan.x, m_fptPan.y);
	}
}

UINT32		CDrawManager::_SetFeature_Move(vector<CFeatureData*>& vecFeatureData, CStep *pStep)
{
	int nDataSize = static_cast<int>(vecFeatureData.size());

	if (nDataSize <= 0) return RESULT_BAD;
	if (pStep == nullptr) return RESULT_BAD;

	for (int nfeno = 0; nfeno < nDataSize; nfeno++)
	{
		CFeatureData *pFeatureData = vecFeatureData[nfeno];

		CLayer *pLayer = pStep->m_arrLayer.GetAt(pFeatureData->m_nLayer);
		if (pLayer == nullptr) continue;

		CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(pFeatureData->m_nFeatureIndex);

		if (pFeatureData->m_pFeature->m_eType != pFeature->m_eType) continue;

		switch (pFeature->m_eType)
		{
		case FeatureType::Z:
		{
			CFeatureZ *pFeatureZ = (CFeatureZ*)pFeature;
			CFeatureZ *pFeatureZ_Edit = (CFeatureZ *)pFeatureData->m_pFeature;

			FeatureSpec.dMoveX = pFeatureZ_Edit->m_dX - pFeatureZ->m_dX;
			FeatureSpec.dMoveY = pFeatureZ_Edit->m_dY - pFeatureZ->m_dY;

			pFeatureZ->m_dX = pFeatureZ_Edit->m_dX;
			pFeatureZ->m_dY = pFeatureZ_Edit->m_dY;

			pFeatureZ->SetMinMaxRect();
			//Redraw를 위하여 Geo 삭제
			if (pFeatureZ->m_pGeoObject != nullptr)
			{
				pFeatureZ->m_pGeoObject->Release();
				pFeatureZ->m_pGeoObject = nullptr;
			}

		}
		break;
		case FeatureType::L:
		{
			CFeatureL *pFeatureL = (CFeatureL *)pFeature;
			CFeatureL *pFeatureL_Edit = (CFeatureL *)pFeatureData->m_pFeature;


			FeatureSpec.dMoveX = pFeatureL_Edit->m_dXs - pFeatureL->m_dXs;
			FeatureSpec.dMoveY = pFeatureL_Edit->m_dYs - pFeatureL->m_dYs;

			pFeatureL->m_dXs = pFeatureL_Edit->m_dXs;
			pFeatureL->m_dXe = pFeatureL_Edit->m_dXe;
			pFeatureL->m_dYs = pFeatureL_Edit->m_dYs;
			pFeatureL->m_dYe = pFeatureL_Edit->m_dYe;		



			//Redraw를 위하여 Geo 삭제
			if (pFeatureL->m_pGeoObject != nullptr)
			{
				pFeatureL->m_pGeoObject->Release();
				pFeatureL->m_pGeoObject = nullptr;
			}			

		}
		break;
		case FeatureType::P:
		{
			CFeatureP *pFeatureP = (CFeatureP *)pFeature;
			CFeatureP *pFeatureP_Edit = (CFeatureP *)pFeatureData->m_pFeature;

			FeatureSpec.dMoveX = pFeatureP_Edit->m_dX - pFeatureP->m_dX  ;
			FeatureSpec.dMoveY = pFeatureP_Edit->m_dY - pFeatureP->m_dY ;

			pFeatureP->m_dX = pFeatureP_Edit->m_dX;
			pFeatureP->m_dY = pFeatureP_Edit->m_dY;

			

			pFeatureP->SetMinMaxRect();

			//Redraw를 위하여 Geo 삭제
			if (pFeatureP->m_pGeoObject != nullptr)
			{
				pFeatureP->m_pGeoObject->Release();
				pFeatureP->m_pGeoObject = nullptr;
			}

		}
		break;
		case FeatureType::A:
		{
			CFeatureA *pFeatureA = (CFeatureA *)pFeature;
			CFeatureA *pFeatureA_Edit = (CFeatureA *)pFeatureData->m_pFeature;


			FeatureSpec.dMoveX = pFeatureA_Edit->m_dXs - pFeatureA->m_dXs;
			FeatureSpec.dMoveY = pFeatureA_Edit->m_dYs - pFeatureA->m_dYs;

			pFeatureA->m_dXs = pFeatureA_Edit->m_dXs;
			pFeatureA->m_dYs = pFeatureA_Edit->m_dYs;
			pFeatureA->m_dXe = pFeatureA_Edit->m_dXe;
			pFeatureA->m_dYe = pFeatureA_Edit->m_dYe;
			pFeatureA->m_dXc = pFeatureA_Edit->m_dXc;
			pFeatureA->m_dYc = pFeatureA_Edit->m_dYc;

			if (pFeatureA->m_pGeoObject != nullptr)
			{
				pFeatureA->m_pGeoObject->Release();
				pFeatureA->m_pGeoObject = nullptr;
			}

			pFeatureA->SetMinMaxRect();
		}
		break;
		case FeatureType::T:
		{
			CFeatureT *pFeatureT = (CFeatureT *)pFeature;
			CFeatureT *pFeatureT_Edit = (CFeatureT *)pFeatureData->m_pFeature;

			pFeatureT->m_dX = pFeatureT_Edit->m_dX;
			pFeatureT->m_dY = pFeatureT_Edit->m_dY;

			pFeatureT->SetMinMaxRect();
		}
		break;
		case FeatureType::S:
		{
			CFeatureS *pFeatureS = (CFeatureS *)pFeature;
			CFeatureS *pFeatureS_Edit = (CFeatureS *)pFeatureData->m_pFeature;

			int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
			int nObOeCount_Edit = static_cast<int>(pFeatureS_Edit->m_arrObOe.GetCount());



			//
			if (nObOeCount == nObOeCount_Edit)
			{//갯수가 같은지 체크

				for (int i = 0; i < nObOeCount; i++)
				{
					CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(i);
					if (pObOe == nullptr) continue;

					CObOe *pObOe_Edit = pFeatureS_Edit->m_arrObOe.GetAt(i);
					if ( pObOe_Edit == nullptr ) continue;

					//if (pObOe->m_bHighlight == true)
					{
						pObOe->m_dXbs = pObOe_Edit->m_dXbs;
						pObOe->m_dYbs = pObOe_Edit->m_dYbs;

						int nObOcCount = static_cast<int>(pObOe->m_arrOsOc.GetCount());
						int nObOcCount_Edit = static_cast<int>(pObOe_Edit->m_arrOsOc.GetCount());

						if (nObOcCount == nObOcCount_Edit)
						{//갯수가 같은지 체크

							for (int j = 0; j < nObOcCount; j++)
							{
								COsOc *pOsOc = pObOe->m_arrOsOc.GetAt(j);
								if (pOsOc == nullptr) continue;

								COsOc *pOsOc_Edit = pObOe_Edit->m_arrOsOc.GetAt(j);
								if (pOsOc_Edit == nullptr) continue;

								FeatureSpec.dMoveX = pOsOc_Edit->m_dX - pOsOc->m_dX;
								FeatureSpec.dMoveY = pOsOc_Edit->m_dY - pOsOc->m_dY;

								pOsOc->m_dX = pOsOc_Edit->m_dX;
								pOsOc->m_dY = pOsOc_Edit->m_dY;

								if (pOsOc->m_bOsOc == false)
								{//Oc
									((COc*)pOsOc)->m_dXc = ((COc*)pOsOc_Edit)->m_dXc;
									((COc*)pOsOc)->m_dYc = ((COc*)pOsOc_Edit)->m_dYc;
								}

							}
						}


						if (pFeatureS->m_pGeoObject != nullptr)
						{
							pFeatureS->m_pGeoObject->Release();
							pFeatureS->m_pGeoObject = nullptr;
						}

						pObOe->SetMinMaxRect();
					}
				}
			}

		}
		break;
		}
		
	}

	return RESULT_GOOD;
}

UINT32		CDrawManager::_SetFeature_Resize_LineArc(vector<CFeatureData*>& vecFeatureData, CStep *pStep)
{
	int nDataSize = static_cast<int>(vecFeatureData.size());

	if (nDataSize <= 0) return RESULT_BAD;
	if (pStep == nullptr) return RESULT_BAD;

	for (int nfeno = 0; nfeno < nDataSize; nfeno++)
	{
		CFeatureData *pFeatureData = vecFeatureData[nfeno];

		CLayer *pLayer = pStep->m_arrLayer.GetAt(pFeatureData->m_nLayer);
		if (pLayer == nullptr) continue;

		CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(pFeatureData->m_nFeatureIndex);

		if (pFeatureData->m_pFeature->m_eType != pFeature->m_eType) continue;

		switch (pFeature->m_eType)
		{
		case FeatureType::L:
		{
			CFeatureL *pFeatureL = (CFeatureL *)pFeature;
			CFeatureL *pFeatureL_Edit = (CFeatureL *)pFeatureData->m_pFeature;

			if (pFeatureL == nullptr) continue;
			if (pFeatureL->m_pSymbol == nullptr) continue;
			if (pFeatureL->m_pSymbol->m_vcParams.size() <= 0) continue;

			if (pFeatureL_Edit == nullptr) continue;
			if (pFeatureL_Edit->m_pSymbol == nullptr) continue;
			if (pFeatureL_Edit->m_pSymbol->m_vcParams.size() <= 0) continue;

			pFeatureL->m_pSymbol->m_vcParams[0] = pFeatureL_Edit->m_pSymbol->m_vcParams[0];
			pFeatureL->m_pSymbol->SetMinMaxRect();
			pFeatureL->m_pSymbol->m_strUserSymbol = CDrawFunction::GetSymbolName(pFeatureL->m_pSymbol->m_eSymbolName, 
				pFeatureL->m_pSymbol->m_vcParams);

			pFeatureL->SetMinMaxRect();
		}
		break;
		case FeatureType::A:
		{
			CFeatureA *pFeatureA = (CFeatureA *)pFeature;
			CFeatureA *pFeatureA_Edit = (CFeatureA *)pFeatureData->m_pFeature;

			if (pFeatureA == nullptr) continue;
			if (pFeatureA->m_pSymbol == nullptr) continue;
			if (pFeatureA->m_pSymbol->m_vcParams.size() <= 0) continue;

			if (pFeatureA_Edit == nullptr) continue;
			if (pFeatureA_Edit->m_pSymbol == nullptr) continue;
			if (pFeatureA_Edit->m_pSymbol->m_vcParams.size() <= 0) continue;

			pFeatureA->m_pSymbol->m_vcParams[0] = pFeatureA_Edit->m_pSymbol->m_vcParams[0];
			pFeatureA->m_pSymbol->SetMinMaxRect();
			pFeatureA->m_pSymbol->m_strUserSymbol = CDrawFunction::GetSymbolName(pFeatureA->m_pSymbol->m_eSymbolName,
				pFeatureA->m_pSymbol->m_vcParams);

			pFeatureA->SetMinMaxRect();
		}
		break;
		}
	}

	return RESULT_GOOD;
}

UINT32		CDrawManager::_SetFeature_Resize_LineArc_SelectOnly(vector<CFeatureData*>& vecFeatureData, CStep *pStep)
{
	int nDataSize = static_cast<int>(vecFeatureData.size());

	if (nDataSize <= 0) return RESULT_BAD;
	if (pStep == nullptr) return RESULT_BAD;

	for (int nfeno = 0; nfeno < nDataSize; nfeno++)
	{
		CFeatureData *pFeatureData = vecFeatureData[nfeno];

		CLayer *pLayer = pStep->m_arrLayer.GetAt(pFeatureData->m_nLayer);
		if (pLayer == nullptr) continue;

		CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(pFeatureData->m_nFeatureIndex);

		if (pFeatureData->m_pFeature->m_eType != pFeature->m_eType) continue;

		switch (pFeature->m_eType)
		{
		case FeatureType::L:
		{
			CFeatureL *pFeatureL = (CFeatureL *)pFeature;
			CFeatureL *pFeatureL_Edit = (CFeatureL *)pFeatureData->m_pFeature;

			if (pFeatureL == nullptr) continue;
			if (pFeatureL->m_pSymbol == nullptr) continue;
			if (pFeatureL->m_pSymbol->m_vcParams.size() <= 0) continue;

			if (pFeatureL_Edit == nullptr) continue;
			if (pFeatureL_Edit->m_pSymbol == nullptr) continue;
			if (pFeatureL_Edit->m_pSymbol->m_vcParams.size() <= 0) continue;

			if ( pFeatureL->m_pSymbol->m_vcParams.size() != pFeatureL_Edit->m_pSymbol->m_vcParams.size() ) continue;

			if (pFeatureL->m_pSymbol->m_vcParams[0] != pFeatureL_Edit->m_pSymbol->m_vcParams[0])
			{//신규로 Symbol 생성

				CSymbol *pSymbol_New = new CSymbol();
				*pSymbol_New = *pFeatureL->m_pSymbol;

				pSymbol_New->m_vcParams.resize(pFeatureL_Edit->m_pSymbol->m_vcParams.size());

				//SetParam
				pSymbol_New->m_vcParams[0] = pFeatureL_Edit->m_pSymbol->m_vcParams[0];
				pSymbol_New->SetMinMaxRect();

				CString strSymbolName = CDrawFunction::GetSymbolName(pSymbol_New->m_eSymbolName, 
					pSymbol_New->m_vcParams);
				pSymbol_New->m_strUserSymbol = strSymbolName;

				//같은 변경하는 Symbol이 같은게 있는지 체크
				//
				BOOL bCompare = FALSE;
				int nSymbolCount = static_cast<int>(pLayer->m_FeatureFile.m_arrSymbol.GetCount());
				CSymbol *pSymbol = nullptr;
				for (int i = 0; i < nSymbolCount; i++)
				{
					CSymbol *pSymbolTemp = pLayer->m_FeatureFile.m_arrSymbol.GetAt(i);

					if (*pSymbolTemp == *pSymbol_New)
					{
						pSymbol = pSymbolTemp;
						bCompare = TRUE;
						break;
					}
				}

				//같은게 있으면, 신규로 생성한것을 지우고,
				//같은 심볼의 포인터를 연결해준다.
				if (bCompare == TRUE)
				{
					delete pSymbol_New;
					pSymbol_New = nullptr;

					pFeatureL->m_pSymbol = pSymbol;
					
				}
				else
				{

					int nMaxIndex = static_cast<int>(pLayer->m_FeatureFile.m_arrSymbol.GetCount());

					//Symbol num : 새로 Add 할것이기 때문에 MaxIndex를 넣어준다.
					pSymbol_New->m_iSerialNum = nMaxIndex;

					//신규 Symbol 포인터 연결
					pFeatureL->m_pSymbol = pSymbol_New;
					pFeatureL->m_nSymNum = nMaxIndex;
					pLayer->m_FeatureFile.m_arrSymbol.Add(pSymbol_New);
				}

			}

			pFeatureL->SetMinMaxRect();
		}
		break;
		case FeatureType::A:
		{
			CFeatureA *pFeatureA = (CFeatureA *)pFeature;
			CFeatureA *pFeatureA_Edit = (CFeatureA *)pFeatureData->m_pFeature;

			if (pFeatureA == nullptr) continue;
			if (pFeatureA->m_pSymbol == nullptr) continue;
			if (pFeatureA->m_pSymbol->m_vcParams.size() <= 0) continue;

			if (pFeatureA_Edit == nullptr) continue;
			if (pFeatureA_Edit->m_pSymbol == nullptr) continue;
			if (pFeatureA_Edit->m_pSymbol->m_vcParams.size() <= 0) continue;

			if (pFeatureA->m_pSymbol->m_vcParams.size() != pFeatureA_Edit->m_pSymbol->m_vcParams.size()) continue;

			if (pFeatureA->m_pSymbol->m_vcParams[0] != pFeatureA_Edit->m_pSymbol->m_vcParams[0])
			{//신규로 Symbol 생성

				CSymbol *pSymbol_New = new CSymbol();
				*pSymbol_New = *pFeatureA->m_pSymbol;

				pSymbol_New->m_vcParams.resize(pFeatureA_Edit->m_pSymbol->m_vcParams.size());

				//SetParam
				pSymbol_New->m_vcParams[0] = pFeatureA_Edit->m_pSymbol->m_vcParams[0];
				pSymbol_New->SetMinMaxRect();
				
				CString strSymbolName = CDrawFunction::GetSymbolName(pSymbol_New->m_eSymbolName,
					pSymbol_New->m_vcParams);
				pSymbol_New->m_strUserSymbol = strSymbolName;

				//같은 변경하는 Symbol이 같은게 있는지 체크
				//
				BOOL bCompare = FALSE;
				int nSymbolCount = static_cast<int>(pLayer->m_FeatureFile.m_arrSymbol.GetCount());
				CSymbol *pSymbol = nullptr;
				for (int i = 0; i < nSymbolCount; i++)
				{
					CSymbol *pSymbolTemp = pLayer->m_FeatureFile.m_arrSymbol.GetAt(i);

					if (*pSymbolTemp == *pSymbol_New)
					{
						pSymbol = pSymbolTemp;
						bCompare = TRUE;
						break;
					}
				}

				//같은게 있으면, 신규로 생성한것을 지우고,
				//같은 심볼의 포인터를 연결해준다.
				if (bCompare == TRUE)
				{
					delete pSymbol_New;
					pSymbol_New = nullptr;

					pFeatureA->m_pSymbol = pSymbol;

				}
				else
				{

					int nMaxIndex = static_cast<int>(pLayer->m_FeatureFile.m_arrSymbol.GetCount());

					//Symbol num : 새로 Add 할것이기 때문에 MaxIndex를 넣어준다.
					pSymbol_New->m_iSerialNum = nMaxIndex;

					//신규 Symbol 포인터 연결
					pFeatureA->m_pSymbol = pSymbol_New;

					pFeatureA->m_nSymNum = nMaxIndex;


					pLayer->m_FeatureFile.m_arrSymbol.Add(pSymbol_New);
				}
			}
			pFeatureA->SetMinMaxRect();

		}
		break;
		}
	}

	return RESULT_GOOD;
}

UINT32		CDrawManager::_SetFeature_Resize_PadText(vector<CFeatureData*>& vecFeatureData, CStep *pStep)
{
	int nDataSize = static_cast<int>(vecFeatureData.size());

	if (nDataSize <= 0) return RESULT_BAD;
	if (pStep == nullptr) return RESULT_BAD;

	for (int nfeno = 0; nfeno < nDataSize; nfeno++)
	{
		CFeatureData *pFeatureData = vecFeatureData[nfeno];

		CLayer *pLayer = pStep->m_arrLayer.GetAt(pFeatureData->m_nLayer);
		if (pLayer == nullptr) continue;

		CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(pFeatureData->m_nFeatureIndex);

		if (pFeatureData->m_pFeature->m_eType != pFeature->m_eType) continue;

		switch (pFeature->m_eType)
		{
		case FeatureType::P:
		{
			CFeatureP *pFeatureP = (CFeatureP *)pFeature;
			CFeatureP *pFeatureP_Edit = (CFeatureP *)pFeatureData->m_pFeature;

			if (pFeatureP == nullptr) continue;
			if (pFeatureP->m_pSymbol == nullptr) continue;
			if (pFeatureP->m_pSymbol->m_vcParams.size() <= 0) continue;

			if (pFeatureP_Edit == nullptr) continue;
			if (pFeatureP_Edit->m_pSymbol == nullptr) continue;
			if (pFeatureP_Edit->m_pSymbol->m_vcParams.size() <= 0) continue;

			int nParamCount = static_cast<int>(pFeatureP->m_pSymbol->m_vcParams.size());
			int nParamCount_Edit = static_cast<int>(pFeatureP_Edit->m_pSymbol->m_vcParams.size());

			if (nParamCount == nParamCount_Edit)
			{
				for (int i = 0; i < nParamCount; i++)
				{
					pFeatureP->m_pSymbol->m_vcParams[i] = pFeatureP_Edit->m_pSymbol->m_vcParams[i];
				}

				pFeatureP->m_pSymbol->SetMinMaxRect();
				pFeatureP->m_pSymbol->m_strUserSymbol = CDrawFunction::GetSymbolName(pFeatureP->m_pSymbol->m_eSymbolName,
					pFeatureP->m_pSymbol->m_vcParams);
			}






			pFeatureP->SetMinMaxRect();
		}
		break;
		case FeatureType::T:
		{
			CFeatureT *pFeatureT = (CFeatureT *)pFeature;
			CFeatureT *pFeatureT_Edit = (CFeatureT *)pFeatureData->m_pFeature;

			pFeatureT->SetMinMaxRect();
		}
		break;
		}
	}

	return RESULT_GOOD;
}

UINT32		CDrawManager::_SetFeature_Resize_PadText_SelectOnly(vector<CFeatureData*>& vecFeatureData, CStep *pStep)
{
	int nDataSize = static_cast<int>(vecFeatureData.size());

	if (nDataSize <= 0) return RESULT_BAD;
	if (pStep == nullptr) return RESULT_BAD;

	for (int nfeno = 0; nfeno < nDataSize; nfeno++)
	{
		CFeatureData *pFeatureData = vecFeatureData[nfeno];

		CLayer *pLayer = pStep->m_arrLayer.GetAt(pFeatureData->m_nLayer);
		if (pLayer == nullptr) continue;

		CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(pFeatureData->m_nFeatureIndex);

		if (pFeatureData->m_pFeature->m_eType != pFeature->m_eType) continue;

		switch (pFeature->m_eType)
		{
		case FeatureType::P:
		{
			CFeatureP *pFeatureP = (CFeatureP *)pFeature;
			CFeatureP *pFeatureP_Edit = (CFeatureP *)pFeatureData->m_pFeature;

			if (pFeatureP == nullptr) continue;
			if (pFeatureP->m_pSymbol == nullptr) continue;
			if (pFeatureP->m_pSymbol->m_vcParams.size() <= 0) continue;

			if (pFeatureP_Edit == nullptr) continue;
			if (pFeatureP_Edit->m_pSymbol == nullptr) continue;
			if (pFeatureP_Edit->m_pSymbol->m_vcParams.size() <= 0) continue;

			int nParamCount = static_cast<int>(pFeatureP->m_pSymbol->m_vcParams.size());
			int nParamCount_Edit = static_cast<int>(pFeatureP_Edit->m_pSymbol->m_vcParams.size());

			if (pFeatureP->m_pSymbol->m_vcParams.size() != pFeatureP_Edit->m_pSymbol->m_vcParams.size()) continue;

			if (pFeatureP->m_pSymbol->m_vcParams[0] != pFeatureP_Edit->m_pSymbol->m_vcParams[0])
			{//신규로 Symbol 생성

				CSymbol *pSymbol_New = new CSymbol();
				*pSymbol_New = *pFeatureP->m_pSymbol;

				pSymbol_New->m_vcParams.resize(pFeatureP_Edit->m_pSymbol->m_vcParams.size());

				//SetParam
				switch (pSymbol_New -> m_eSymbolName)
				{
				case SymbolName::r:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					break;
				case SymbolName::s:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					break;
				case SymbolName::rect:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					pSymbol_New->m_vcParams[1] = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
					break;
				case SymbolName::rectxr:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					pSymbol_New->m_vcParams[1] = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
					pSymbol_New->m_vcParams[2] = pFeatureP->m_pSymbol->m_vcParams[2];
					pSymbol_New->m_vcParams[3] = pFeatureP->m_pSymbol->m_vcParams[3];
					break;
				case SymbolName::rectxc:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					pSymbol_New->m_vcParams[1] = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
					pSymbol_New->m_vcParams[2] = pFeatureP->m_pSymbol->m_vcParams[2];
					pSymbol_New->m_vcParams[3] = pFeatureP->m_pSymbol->m_vcParams[3];
					break;
				case SymbolName::oval:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					pSymbol_New->m_vcParams[1] = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
					break;
				case SymbolName::di:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					pSymbol_New->m_vcParams[1] = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
					break;
				case SymbolName::octagon:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					pSymbol_New->m_vcParams[1] = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
					pSymbol_New->m_vcParams[2] = pFeatureP_Edit->m_pSymbol->m_vcParams[2];
					break;
				case SymbolName::donut_r:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					pSymbol_New->m_vcParams[1] = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
					break;
				case SymbolName::donut_s:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					pSymbol_New->m_vcParams[1] = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
					break;
				case SymbolName::hex_l:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					pSymbol_New->m_vcParams[1] = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
					pSymbol_New->m_vcParams[2] = pFeatureP_Edit->m_pSymbol->m_vcParams[2];
					break;
				case SymbolName::hex_s:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					pSymbol_New->m_vcParams[1] = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
					pSymbol_New->m_vcParams[2] = pFeatureP_Edit->m_pSymbol->m_vcParams[2];
					break;
				case SymbolName::bfr:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					break;
				case SymbolName::bfs:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					break;
				case SymbolName::tri:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					pSymbol_New->m_vcParams[1] = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
					break;
				case SymbolName::oval_h:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					pSymbol_New->m_vcParams[1] = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
					break;
				case SymbolName::thr:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					pSymbol_New->m_vcParams[1] = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
					pSymbol_New->m_vcParams[2] = pFeatureP->m_pSymbol->m_vcParams[2];
					pSymbol_New->m_vcParams[3] = pFeatureP->m_pSymbol->m_vcParams[3];
					pSymbol_New->m_vcParams[4] = pFeatureP_Edit->m_pSymbol->m_vcParams[4];
					break;
				case SymbolName::ths:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					pSymbol_New->m_vcParams[1] = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
					pSymbol_New->m_vcParams[2] = pFeatureP->m_pSymbol->m_vcParams[2];
					pSymbol_New->m_vcParams[3] = pFeatureP->m_pSymbol->m_vcParams[3];
					pSymbol_New->m_vcParams[4] = pFeatureP_Edit->m_pSymbol->m_vcParams[4];
					break;
				case SymbolName::s_ths:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					pSymbol_New->m_vcParams[1] = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
					pSymbol_New->m_vcParams[2] = pFeatureP->m_pSymbol->m_vcParams[2];
					pSymbol_New->m_vcParams[3] = pFeatureP->m_pSymbol->m_vcParams[3];
					pSymbol_New->m_vcParams[4] = pFeatureP_Edit->m_pSymbol->m_vcParams[4];
					break;
				case SymbolName::s_tho:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					pSymbol_New->m_vcParams[1] = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
					pSymbol_New->m_vcParams[2] = pFeatureP->m_pSymbol->m_vcParams[2];
					pSymbol_New->m_vcParams[3] = pFeatureP->m_pSymbol->m_vcParams[3];
					pSymbol_New->m_vcParams[4] = pFeatureP_Edit->m_pSymbol->m_vcParams[4];
					break;
				case SymbolName::sr_ths:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					pSymbol_New->m_vcParams[1] = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
					pSymbol_New->m_vcParams[2] = pFeatureP->m_pSymbol->m_vcParams[2];
					pSymbol_New->m_vcParams[3] = pFeatureP->m_pSymbol->m_vcParams[3];
					pSymbol_New->m_vcParams[4] = pFeatureP_Edit->m_pSymbol->m_vcParams[4];
					break;
				case SymbolName::rc_ths:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					pSymbol_New->m_vcParams[1] = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
					pSymbol_New->m_vcParams[2] = pFeatureP->m_pSymbol->m_vcParams[2];
					pSymbol_New->m_vcParams[3] = pFeatureP->m_pSymbol->m_vcParams[3];
					pSymbol_New->m_vcParams[4] = pFeatureP_Edit->m_pSymbol->m_vcParams[4];
					pSymbol_New->m_vcParams[5]= pFeatureP_Edit->m_pSymbol->m_vcParams[5];
				case SymbolName::rc_tho:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					pSymbol_New->m_vcParams[1] = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
					pSymbol_New->m_vcParams[2] = pFeatureP->m_pSymbol->m_vcParams[2];
					pSymbol_New->m_vcParams[3] = pFeatureP->m_pSymbol->m_vcParams[3];
					pSymbol_New->m_vcParams[4] = pFeatureP_Edit->m_pSymbol->m_vcParams[4];
					pSymbol_New->m_vcParams[5] = pFeatureP_Edit->m_pSymbol->m_vcParams[5];
					break;
				case SymbolName::el:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					pSymbol_New->m_vcParams[1] = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
					break;
				case SymbolName::moire:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					pSymbol_New->m_vcParams[1] = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
					pSymbol_New->m_vcParams[2] = pFeatureP->m_pSymbol->m_vcParams[2];
					pSymbol_New->m_vcParams[3] = pFeatureP_Edit->m_pSymbol->m_vcParams[3];
					pSymbol_New->m_vcParams[4] = pFeatureP_Edit->m_pSymbol->m_vcParams[4];
					pSymbol_New->m_vcParams[5] = pFeatureP->m_pSymbol->m_vcParams[5];
					break;
				case SymbolName::hole:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					/*
					pSymbol_New->m_vcParams[2] = pFeatureP_Edit->m_pSymbol->m_vcParams[2];
					pSymbol_New->m_vcParams[3] = pFeatureP_Edit->m_pSymbol->m_vcParams[3];
					*/


				default:
					break;
				}
				


				pSymbol_New->SetMinMaxRect();
				
				CString strSymbolName = CDrawFunction::GetSymbolName(pSymbol_New->m_eSymbolName,
					pSymbol_New->m_vcParams);
				pSymbol_New->m_strUserSymbol = strSymbolName;

				//같은 변경하는 Symbol이 같은게 있는지 체크
				//
				BOOL bCompare = FALSE;
				int nSymbolCount = static_cast<int>(pLayer->m_FeatureFile.m_arrSymbol.GetCount());
				CSymbol *pSymbol = nullptr;
				for (int i = 0; i < nSymbolCount; i++)
				{
					CSymbol *pSymbolTemp = pLayer->m_FeatureFile.m_arrSymbol.GetAt(i);

					if (*pSymbolTemp == *pSymbol_New)
					{
						pSymbol = pSymbolTemp;
						bCompare = TRUE;
						break;
					}
				}

				//같은게 있으면, 신규로 생성한것을 지우고,
				//같은 심볼의 포인터를 연결해준다.
				if (bCompare == TRUE)
				{
					delete pSymbol_New;
					pSymbol_New = nullptr;

					pFeatureP->m_pSymbol = pSymbol;

				}
				else
				{

					int nMaxIndex = static_cast<int>(pLayer->m_FeatureFile.m_arrSymbol.GetCount());

					//Symbol num : 새로 Add 할것이기 때문에 MaxIndex를 넣어준다.
					pSymbol_New->m_iSerialNum = nMaxIndex;
					

					//신규 Symbol 포인터 연결
					pFeatureP->m_pSymbol = pSymbol_New;
					pFeatureP->m_nSymNum = nMaxIndex;

					pLayer->m_FeatureFile.m_arrSymbol.Add(pSymbol_New);
				}

				

			}

			pFeatureP->SetMinMaxRect();

		}
		break;
		case FeatureType::T:
		{
			CFeatureT *pFeatureT = (CFeatureT *)pFeature;
			CFeatureT *pFeatureT_Edit = (CFeatureT *)pFeatureData->m_pFeature;

			pFeatureT->SetMinMaxRect();
		}
		break;
		}
	}


	
		


	return RESULT_GOOD;
}

UINT32		CDrawManager::_SetFeature_Resize_Surface(vector<CFeatureData*>& vecFeatureData, CStep *pStep)
{
	int nDataSize = static_cast<int>(vecFeatureData.size());

	if (nDataSize <= 0) return RESULT_BAD;
	if (pStep == nullptr) return RESULT_BAD;

	for (int nfeno = 0; nfeno < nDataSize; nfeno++)
	{
		CFeatureData *pFeatureData = vecFeatureData[nfeno];

		CLayer *pLayer = pStep->m_arrLayer.GetAt(pFeatureData->m_nLayer);
		if (pLayer == nullptr) continue;

		CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(pFeatureData->m_nFeatureIndex);

		if (pFeatureData->m_pFeature->m_eType != pFeature->m_eType) continue;

		switch (pFeature->m_eType)
		{
		case FeatureType::S:
		{
			CFeatureS *pFeatureS		= (CFeatureS *)pFeature;
			CFeatureS *pFeatureS_Edit	= (CFeatureS *)pFeatureData->m_pFeature;

			if (pFeatureS == nullptr) continue;
			if (pFeatureS_Edit == nullptr) continue;

			int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
			for (int i = 0; i < nObOeCount; i++)
			{
				CObOe *pObOe			= pFeatureS->m_arrObOe.GetAt(i);
				CObOe *pObOe_Edit		= pFeatureS_Edit->m_arrObOe.GetAt(i);

				if (pObOe == nullptr) continue;
				if (pObOe_Edit == nullptr) continue;

				if (pObOe->m_bHighlight == true)
				{
					*pObOe = *pObOe_Edit;

					int nOsOcCount = static_cast<int>(pObOe->m_arrOsOc.GetCount());
					for (int nOsOc = 0; nOsOc < nOsOcCount; nOsOc++)
					{
						COsOc *pOsOc = pObOe->m_arrOsOc.GetAt(nOsOc);
						COsOc *pOsOc_Edit = pObOe_Edit->m_arrOsOc.GetAt(nOsOc);

						*pOsOc = *pOsOc_Edit;

						if (pOsOc->m_bOsOc == false)
						{
							*((COc*)pOsOc) = *((COc*)pOsOc_Edit);
						}
					}
					
				}

				pObOe->SetMinMaxRect();
			}
		}
		break;
		}
	}


	return RESULT_GOOD;
}

UINT32		CDrawManager::_SetFeature_Copy(vector<CFeatureData *>& vecFeatureData, CStep *pStep)
{
	int nDataSize = static_cast<int>(vecFeatureData.size());

	if (nDataSize <= 0) return RESULT_BAD;
	if (pStep == nullptr) return RESULT_BAD;

	for (int nfeno = 0; nfeno < nDataSize; nfeno++)
	{
		CFeatureData *pFeatureData = vecFeatureData[nfeno];

		CLayer *pLayer = pStep->m_arrLayer.GetAt(pFeatureData->m_nLayer);
		if (pLayer == nullptr) continue;

		if (pFeatureData->m_pFeature == nullptr) continue;

		CFeature *pFeature = nullptr;
		switch (pFeatureData->m_pFeature->m_eType)
		{
		case FeatureType::P:
		{
			pFeature = new CFeatureP();
			*pFeature = *pFeatureData->m_pFeature;
			*(CFeatureP*)pFeature = *(CFeatureP*)pFeatureData->m_pFeature;
			pFeature->SetMinMaxRect();
			

			pLayer->m_FeatureFile.m_arrFeature.Add(pFeature);
		}
		break;
		case FeatureType::L:
		{
			pFeature = new CFeatureL();
			*pFeature = *pFeatureData->m_pFeature;
			*(CFeatureL*)pFeature = *(CFeatureL*)pFeatureData->m_pFeature;
			pFeature->SetMinMaxRect();

			pLayer->m_FeatureFile.m_arrFeature.Add(pFeature);
			
		}
		break;
		case FeatureType::A:
		{
			pFeature = new CFeatureA();
			*pFeature = *pFeatureData->m_pFeature;
			*(CFeatureA*)pFeature = *(CFeatureA*)pFeatureData->m_pFeature;

			pFeature->SetMinMaxRect();

			pLayer->m_FeatureFile.m_arrFeature.Add(pFeature);
		}
		break;

		}

		if (pFeature == nullptr) continue;

		CSymbol *pSymbol_New = new CSymbol();
		*pSymbol_New = *pFeatureData->m_pFeature->m_pSymbol;
		pSymbol_New->m_vcParams = pFeatureData->m_pFeature->m_pSymbol->m_vcParams;

		//추가하는 Symbol이 같은게 있는지 체크
		BOOL bCompare = FALSE;
		int nSymbolCount = static_cast<int>(pLayer->m_FeatureFile.m_arrSymbol.GetCount());
		CSymbol *pSymbol = nullptr;
		for (int i = 0; i < nSymbolCount; i++)
		{
			CSymbol *pSymbolTemp = pLayer->m_FeatureFile.m_arrSymbol.GetAt(i);

			if (*pSymbolTemp == *pSymbol_New)
			{
				pSymbol = pSymbolTemp;
				bCompare = TRUE;
				break;
			}
		}

		if (bCompare == TRUE)
		{//같은 Symbol이 있을 경우 
			delete pSymbol_New;
			pSymbol_New = nullptr;

			pFeature->m_pSymbol = pSymbol;
		}
		else
		{//같은 Symbol이 없을 경우 
			int nMaxIndex = static_cast<int>(pLayer->m_FeatureFile.m_arrSymbol.GetCount());

			pSymbol_New->SetMinMaxRect();

			pFeature->m_pSymbol = pSymbol_New;
			//Symbol num : 새로 Add 할것이기 때문에 MaxIndex를 넣어준다.
			pFeature->m_pSymbol->m_iSerialNum = nMaxIndex;

			pLayer->m_FeatureFile.m_arrSymbol.Add(pSymbol_New);
		}

		pFeature->m_arrAttribute.RemoveAll();
		//Att Name 추가
		int nAttNameCount = static_cast<int>(pFeatureData->m_pFeature->m_arrAttribute.GetCount());
		for (int nAttName = 0; nAttName < nAttNameCount; nAttName++)
		{
			CAttribute *pAttName_New = new CAttribute();
			CAttribute *pAttName = pFeatureData->m_pFeature->m_arrAttribute.GetAt(nAttName);

			*pAttName_New = *pAttName;

			//추가하는 Attribute이 같은게 있는지 체크
			BOOL bCompare = FALSE;
			int nSymbolCount = static_cast<int>(pLayer->m_FeatureFile.m_arrAttribute.GetCount());
			pAttName = nullptr;
			for (int i = 0; i < nSymbolCount; i++)
			{
				CAttribute *pAttNameTemp = pLayer->m_FeatureFile.m_arrAttribute.GetAt(i);

				if (pAttNameTemp->m_strName == pAttName_New->m_strName)
				{
					pAttName = pAttNameTemp;
					bCompare = TRUE;
					break;
				}
			}

			if (bCompare == TRUE)
			{//같은 Symbol이 있을 경우 
				delete pAttName_New;
				pAttName_New = nullptr;

				pFeature->m_arrAttribute.Add(pAttName);
			}
			else
			{//같은 Symbol이 없을 경우 
				int nMaxIndex = static_cast<int>(pLayer->m_FeatureFile.m_arrAttribute.GetCount());

				pFeature->m_arrAttribute.Add(pAttName_New);

				//Symbol num : 새로 Add 할것이기 때문에 MaxIndex를 넣어준다.
				int nIndex = static_cast<int>(pFeature->m_arrAttribute.GetCount()) - 1;
				pFeature->m_arrAttribute[nIndex]->m_iSerialNum = nMaxIndex;

				pLayer->m_FeatureFile.m_arrAttribute.Add(pAttName_New);
			}
		}

		pFeature->m_arrAttributeTextString.RemoveAll();
		//Att String 추가
		int nAttStringCount = static_cast<int>(pFeatureData->m_pFeature->m_arrAttributeTextString.GetCount());
		for (int nAttString = 0; nAttString < nAttStringCount; nAttString++)
		{
			CAttributeTextString *pAttString_New = new CAttributeTextString();
			CAttributeTextString *pAttString = pFeatureData->m_pFeature->m_arrAttributeTextString.GetAt(nAttString);

			*pAttString_New = *pAttString;

			//추가하는 Attribute이 같은게 있는지 체크
			BOOL bCompare = FALSE;
			int nSymbolCount = static_cast<int>(pLayer->m_FeatureFile.m_arrAttributeTextString.GetCount());
			pAttString = nullptr;
			for (int i = 0; i < nSymbolCount; i++)
			{
				CAttributeTextString *pAttStringTemp = pLayer->m_FeatureFile.m_arrAttributeTextString.GetAt(i);

				if (pAttStringTemp->m_strText == pAttString_New->m_strText)
				{
					pAttString = pAttStringTemp;
					bCompare = TRUE;
					break;
				}
			}

			if (bCompare == TRUE)
			{//같은 Symbol이 있을 경우 
				delete pAttString_New;
				pAttString_New = nullptr;

				pFeature->m_arrAttributeTextString.Add(pAttString);
			}
			else
			{//같은 Symbol이 없을 경우 
				int nMaxIndex = static_cast<int>(pLayer->m_FeatureFile.m_arrAttributeTextString.GetCount());

				pFeature->m_arrAttributeTextString.Add(pAttString_New);

				//Symbol num : 새로 Add 할것이기 때문에 MaxIndex를 넣어준다.
				int nIndex = static_cast<int>(pFeature->m_arrAttributeTextString.GetCount()) - 1;
				pFeature->m_arrAttributeTextString[nIndex]->m_iSerialNum = nMaxIndex;

				pLayer->m_FeatureFile.m_arrAttributeTextString.Add(pAttString_New);
			}
		}

	}

	return RESULT_GOOD;
}

UINT32		CDrawManager::_SetFeature_Add(vector<CFeatureData*>& vecFeatureData, CStep *pStep)
{
	int nDataSize = static_cast<int>(vecFeatureData.size());

	if (nDataSize <= 0) return RESULT_BAD;
	if (pStep == nullptr) return RESULT_BAD;

	for (int nfeno = 0; nfeno < nDataSize; nfeno++)
	{
		CFeatureData *pFeatureData = vecFeatureData[nfeno];

		CLayer *pLayer = pStep->m_arrLayer.GetAt(pFeatureData->m_nLayer);
		if (pLayer == nullptr) continue;

		if (pFeatureData->m_pFeature == nullptr) continue;
		
		CFeature *pFeature = nullptr;
		switch (pFeatureData->m_pFeature->m_eType)
		{

		case FeatureType::Z:
		{
			pFeature = new CFeatureZ();
			*pFeature = *pFeatureData->m_pFeature;
			*(CFeatureZ*)pFeature = *(CFeatureZ*)pFeatureData->m_pFeature;

			pFeature->SetMinMaxRect();

			pLayer->m_FeatureFile.m_arrFeature.Add(pFeature);
		}
		break;
		case FeatureType::P:
		{
			pFeature = new CFeatureP();
			*pFeature = *pFeatureData->m_pFeature;
			*(CFeatureP*)pFeature = *(CFeatureP*)pFeatureData->m_pFeature;

			pFeature->SetMinMaxRect();

			//Feature 추가
			pLayer->m_FeatureFile.m_arrFeature.Add(pFeature);
		}
		break;
		case FeatureType::L:
		{
			pFeature = new CFeatureL();
			*pFeature = *pFeatureData->m_pFeature;
			*(CFeatureL*)pFeature = *(CFeatureL*)pFeatureData->m_pFeature;

			pFeature->SetMinMaxRect();

			pLayer->m_FeatureFile.m_arrFeature.Add(pFeature);
		}
		break;
		case FeatureType::A:
		{
			pFeature = new CFeatureA();
			*pFeature = *pFeatureData->m_pFeature;
			*(CFeatureA*)pFeature = *(CFeatureA*)pFeatureData->m_pFeature;

			pFeature->SetMinMaxRect();

			pLayer->m_FeatureFile.m_arrFeature.Add(pFeature);
		}
		break;

		case FeatureType::S:
		{
			
			//O번째 ObOe가 nullptr 인지 확인,
			//nullptr이라면, 기존 Surface내에 신규 ObOe
			//nullptr이 아니라면, 신규 Surface 생성
			CFeatureS *pFeatureS_Src = (CFeatureS*)pFeatureData->m_pFeature;
			int nObOeCount = static_cast<int>(pFeatureS_Src->m_arrObOe.GetCount());
			if (nObOeCount == 0) continue;

			if (pFeatureS_Src->m_arrObOe.GetAt(0) == nullptr)
			{//기존 Surface내에 신규 ObOe
				CFeature *pFeature_Dst = pLayer->m_FeatureFile.m_arrFeature.GetAt(pFeatureData->m_nFeatureIndex);
				if (pFeature_Dst == nullptr) continue;
				CFeatureS *pFeatureS = (CFeatureS*)pFeature_Dst;

				for (int i = 0; i < nObOeCount; i++)
				{
					CObOe *pObOe_Src = pFeatureS_Src->m_arrObOe.GetAt(i);
					if (pObOe_Src == nullptr) continue;

					CObOe *pObOe = new CObOe();
					if (pObOe == nullptr) continue;
					*pObOe = *pObOe_Src;

					int nOsOcCount = static_cast<int>(pObOe_Src->m_arrOsOc.GetCount());
					for (int j = 0; j < nOsOcCount; j++)
					{
						COsOc *pOsOc_Src = pObOe_Src->m_arrOsOc.GetAt(j);
						if (pOsOc_Src == nullptr) continue;

						COsOc *pOsOc = nullptr;
						if (pOsOc_Src->m_bOsOc == true)
						{
							pOsOc = new COs();
						}
						else
						{
							pOsOc = new COc();
						}

						*pOsOc = *pOsOc_Src;

						//Arc 인경우 
						if (pOsOc_Src->m_bOsOc == false)
						{
							*((COc*)pOsOc) = *((COc*)pOsOc_Src);
						}

						pObOe->m_arrOsOc.Add(pOsOc);
					}

					pFeatureS->m_arrObOe.Add(pObOe);
				}

				pFeature = nullptr;
			}
			else
			{//신규 Surface 생성
				pFeature = new CFeatureS();

				*pFeature = *pFeatureData->m_pFeature;

				CFeatureS *pFeatureS = (CFeatureS*)pFeature;

				for (int i = 0; i < nObOeCount; i++)
				{
					CObOe *pObOe_Src = pFeatureS_Src->m_arrObOe.GetAt(i);
					if (pObOe_Src == nullptr) continue;

					CObOe *pObOe = new CObOe();
					if (pObOe == nullptr) continue;
					*pObOe = *pObOe_Src;

					int nOsOcCount = static_cast<int>(pObOe_Src->m_arrOsOc.GetCount());
					for (int j = 0; j < nOsOcCount; j++)
					{
						COsOc *pOsOc_Src = pObOe_Src->m_arrOsOc.GetAt(j);
						if (pOsOc_Src == nullptr) continue;

						COsOc *pOsOc = nullptr;
						if (pOsOc_Src->m_bOsOc == true)
						{
							pOsOc = new COs();
						}
						else
						{
							pOsOc = new COc();
						}

						*pOsOc = *pOsOc_Src;

						//Arc 인경우 
						if (pOsOc_Src->m_bOsOc == false)
						{
							*((COc*)pOsOc) = *((COc*)pOsOc_Src);
						}

						pObOe->m_arrOsOc.Add(pOsOc);
					}

					pFeatureS->m_arrObOe.Add(pObOe);
				}

				pFeature->SetMinMaxRect();
				pLayer->m_FeatureFile.m_arrFeature.Add(pFeature);
			}

			
		}
		break;

		}

		if (pFeature == nullptr) continue;
		
		if (pFeatureData->m_pFeature->m_pSymbol != nullptr)
		{
			CSymbol *pSymbol_New = new CSymbol();
			*pSymbol_New = *pFeatureData->m_pFeature->m_pSymbol;
			pSymbol_New->m_vcParams = pFeatureData->m_pFeature->m_pSymbol->m_vcParams;

			delete pFeatureData->m_pFeature->m_pSymbol;
			pFeatureData->m_pFeature->m_pSymbol = nullptr;

			pSymbol_New->m_strUserSymbol = CDrawFunction::GetSymbolName(pSymbol_New->m_eSymbolName, pSymbol_New->m_vcParams);
			pSymbol_New->SetMinMaxRect();

			int nSymbolIndex = 0;
			//추가하는 Symbol이 같은게 있는지 체크
			BOOL bCompare = FALSE;
			int nSymbolCount = static_cast<int>(pLayer->m_FeatureFile.m_arrSymbol.GetCount());
			CSymbol *pSymbol = nullptr;
			for (int i = 0; i < nSymbolCount; i++)
			{
				CSymbol *pSymbolTemp = pLayer->m_FeatureFile.m_arrSymbol.GetAt(i);

				if (*pSymbolTemp == *pSymbol_New)
				{
					pSymbol = pSymbolTemp;
					bCompare = TRUE;
					break;
				}
			}

			if (bCompare == TRUE)
			{//같은 Symbol이 있을 경우 
				delete pSymbol_New;
				pSymbol_New = nullptr;

				pFeature->m_pSymbol = pSymbol;

				nSymbolIndex = pSymbol->m_iSerialNum;
			}
			else
			{//같은 Symbol이 없을 경우 
				int nMaxIndex = static_cast<int>(pLayer->m_FeatureFile.m_arrSymbol.GetCount());

				pSymbol_New->SetMinMaxRect();

				pFeature->m_pSymbol = pSymbol_New;
				//Symbol num : 새로 Add 할것이기 때문에 MaxIndex를 넣어준다.
				pFeature->m_pSymbol->m_iSerialNum = nMaxIndex;
				nSymbolIndex = nMaxIndex;

				pLayer->m_FeatureFile.m_arrSymbol.Add(pSymbol_New);

			}
			//Symbol Index 업데이트
			switch (pFeature->m_eType)
			{
			case FeatureType::Z:
			{
				CFeatureZ* pFeatureZ = (CFeatureZ*)pFeature;
				pFeatureZ->m_nSymNum = nSymbolIndex;
			}
			break;
			case FeatureType::P:
			{
				CFeatureP* pFeatureP = (CFeatureP*)pFeature;
				pFeatureP->m_nSymNum = nSymbolIndex;
			}
			break;
			case FeatureType::L:
			{
				CFeatureL* pFeatureL = (CFeatureL*)pFeature;
				pFeatureL->m_nSymNum = nSymbolIndex;
			}
			break;
			case FeatureType::A:
				CFeatureA* pFeatureA = (CFeatureA*)pFeature;
				pFeatureA->m_nSymNum = nSymbolIndex;
			}
		}

		pFeature->m_arrAttribute.RemoveAll();
		//Att Name 추가
		int nAttNameCount = static_cast<int>(pFeatureData->m_pFeature->m_arrAttribute.GetCount());
		for (int nAttName = 0; nAttName < nAttNameCount; nAttName++)
		{
			CAttribute *pAttName_New = new CAttribute();
			CAttribute *pAttName = pFeatureData->m_pFeature->m_arrAttribute.GetAt(nAttName);

			*pAttName_New = *pAttName;

			//추가하는 Attribute이 같은게 있는지 체크
			BOOL bCompare = FALSE;
			int nSymbolCount = static_cast<int>(pLayer->m_FeatureFile.m_arrAttribute.GetCount());
			
			pAttName = nullptr;

			for (int i = 0; i < nSymbolCount; i++)
			{
				CAttribute *pAttNameTemp = pLayer->m_FeatureFile.m_arrAttribute.GetAt(i);

				if (pAttNameTemp->m_strName == pAttName_New->m_strName)
				{
					pAttName = pAttNameTemp;
					bCompare = TRUE;
					break;
				}
			}

			if (bCompare == TRUE)
			{//같은 Att이 있을 경우 
				delete pAttName_New;
				pAttName_New = nullptr;

				pFeature->m_arrAttribute.Add(pAttName);
			}
			else
			{//같은 Att가 없을 경우 
				int nMaxIndex = static_cast<int>(pLayer->m_FeatureFile.m_arrAttribute.GetCount());

				pFeature->m_arrAttribute.Add(pAttName_New);

				//Symbol num : 새로 Add 할것이기 때문에 MaxIndex를 넣어준다.
				int nIndex = static_cast<int>(pFeature->m_arrAttribute.GetCount()) - 1;
				pFeature->m_arrAttribute[nIndex]->m_iSerialNum = nMaxIndex;

				pLayer->m_FeatureFile.m_arrAttribute.Add(pAttName_New);
			}
		}

		pFeature->m_arrAttributeTextString.RemoveAll();
		//Att String 추가
		int nAttStringCount = static_cast<int>(pFeatureData->m_pFeature->m_arrAttributeTextString.GetCount());
		for (int nAttString = 0; nAttString < nAttStringCount; nAttString++)
		{
			CAttributeTextString *pAttString_New = new CAttributeTextString();
			CAttributeTextString *pAttString = pFeatureData->m_pFeature->m_arrAttributeTextString.GetAt(nAttString);

			*pAttString_New = *pAttString;

			//추가하는 Attribute이 같은게 있는지 체크
			BOOL bCompare = FALSE;
			int nSymbolCount = static_cast<int>(pLayer->m_FeatureFile.m_arrAttributeTextString.GetCount());
			
			pAttString = nullptr;

			for (int i = 0; i < nSymbolCount; i++)
			{
				CAttributeTextString *pAttStringTemp = pLayer->m_FeatureFile.m_arrAttributeTextString.GetAt(i);

				if (pAttStringTemp->m_strText == pAttString_New->m_strText)
				{
					pAttString = pAttStringTemp;
					bCompare = TRUE;
					break;
				}
			}

			if (bCompare == TRUE)
			{//같은 Symbol이 있을 경우 
				delete pAttString_New;
				pAttString_New = nullptr;

				pFeature->m_arrAttributeTextString.Add(pAttString);
			}
			else
			{//같은 Symbol이 없을 경우 
				int nMaxIndex = static_cast<int>(pLayer->m_FeatureFile.m_arrAttributeTextString.GetCount());

				pFeature->m_arrAttributeTextString.Add(pAttString_New);

				//Symbol num : 새로 Add 할것이기 때문에 MaxIndex를 넣어준다.
				int nIndex = static_cast<int>(pFeature->m_arrAttributeTextString.GetCount()) - 1;
				pFeature->m_arrAttributeTextString[nIndex]->m_iSerialNum = nMaxIndex;

				pLayer->m_FeatureFile.m_arrAttributeTextString.Add(pAttString_New);
			}
		}

	}

	return RESULT_GOOD;
}

UINT32		CDrawManager::_SetFeature_Add_Mask(IN const MaskType &eMaskType, vector<CFeatureData*>& vecFeatureData, CStep *pStep)
{
	if (eMaskType < MaskType::enumType_Align) return RESULT_BAD;
	if (eMaskType >= MaskType::enumType_Max) return RESULT_BAD;

	int nDataSize = static_cast<int>(vecFeatureData.size());

	if (nDataSize <= 0) return RESULT_BAD;
	if (pStep == nullptr) return RESULT_BAD;

	CAlignMask *pAlignMask = new CAlignMask;

	CLayer *pLayer = nullptr;
	for (int nfeno = 0; nfeno < nDataSize; nfeno++)
	{
		CFeatureData *pFeatureData = vecFeatureData[nfeno];
		if (pFeatureData->m_pFeature == nullptr) continue;
		
		CFeature *pFeature = nullptr;
		switch (pFeatureData->m_pFeature->m_eType)
		{
		case FeatureType::P:
		{
			pFeature = new CFeatureP();
			*pFeature = *pFeatureData->m_pFeature;
			*(CFeatureP*)pFeature = *(CFeatureP*)pFeatureData->m_pFeature;

			pFeature->SetMinMaxRect();

			pAlignMask->m_arrFeature.Add(pFeature);

			//Add 확인용
			pLayer = pStep->m_arrLayer.GetAt(pFeatureData->m_nLayer);
			if (pLayer == nullptr) continue;

		}
		break;
		}

		if (pFeature == nullptr) continue;

		CSymbol *pSymbol_New = new CSymbol();
		*pSymbol_New = *pFeatureData->m_pFeature->m_pSymbol;
		pSymbol_New->m_vcParams = pFeatureData->m_pFeature->m_pSymbol->m_vcParams;

		pSymbol_New->SetMinMaxRect();

		pFeature->m_pSymbol = pSymbol_New;
		pFeature->SetMinMaxRect();
		
		pFeature->m_arrAttribute.RemoveAll();
		pFeature->m_arrAttributeTextString.RemoveAll();
	}


	//Feature 추가
	if (pLayer != nullptr)
	{
		if (eMaskType == MaskType::enumType_Align)
		{
			pLayer->m_arrAlign.Add(pAlignMask);
		}
		else if (eMaskType == MaskType::enumType_Mask)
		{
			pLayer->m_arrMask.Add(pAlignMask);
		}
	}

	return RESULT_GOOD;
}

UINT32		CDrawManager::_SetFeature_Resize_Mask(IN const MaskType &eMaskType, vector<CFeatureData*>& vecFeatureData, CStep *pStep)
{
	int nDataSize = static_cast<int>(vecFeatureData.size());

	if (nDataSize <= 0) return RESULT_BAD;
	if (pStep == nullptr) return RESULT_BAD;

	for (int nfeno = 0; nfeno < nDataSize; nfeno++)
	{
		CFeatureData *pFeatureData = vecFeatureData[nfeno];

		CLayer *pLayer = pStep->m_arrLayer.GetAt(pFeatureData->m_nLayer);
		if (pLayer == nullptr) continue;

		int nMaskIndex = pFeatureData->m_nMaskIndex;
		int nFeatureIndex = pFeatureData->m_nFeatureIndex;

		CFeature *pFeature = nullptr;
		
		if (eMaskType == MaskType::enumType_Align)
		{
			CAlignMask *pAlignMask = pLayer->m_arrAlign.GetAt(nFeatureIndex);
			if ( pAlignMask == nullptr) continue;

			pFeature = pAlignMask->m_arrFeature.GetAt(nMaskIndex);
		}
		else if (eMaskType == MaskType::enumType_Mask)
		{
			CAlignMask *pAlignMask = pLayer->m_arrMask.GetAt(nFeatureIndex);
			if (pAlignMask == nullptr) continue;

			pFeature = pAlignMask->m_arrFeature.GetAt(nMaskIndex);
		}
		else
		{
			continue;
		}
		if (pFeature == nullptr) continue;

		if (pFeatureData->m_pFeature->m_eType != pFeature->m_eType) continue;

		switch (pFeature->m_eType)
		{
		case FeatureType::P:
		{
			CFeatureP *pFeatureP = (CFeatureP *)pFeature;
			CFeatureP *pFeatureP_Edit = (CFeatureP *)pFeatureData->m_pFeature;

			if (pFeatureP == nullptr) continue;
			if (pFeatureP->m_pSymbol == nullptr) continue;
			if (pFeatureP->m_pSymbol->m_vcParams.size() <= 0) continue;

			if (pFeatureP_Edit == nullptr) continue;
			if (pFeatureP_Edit->m_pSymbol == nullptr) continue;
			if (pFeatureP_Edit->m_pSymbol->m_vcParams.size() <= 0) continue;

			int nParamCount = static_cast<int>(pFeatureP->m_pSymbol->m_vcParams.size());
			int nParamCount_Edit = static_cast<int>(pFeatureP_Edit->m_pSymbol->m_vcParams.size());

			if (nParamCount == nParamCount_Edit)
			{
				for (int i = 0; i < nParamCount; i++)
				{
					pFeatureP->m_pSymbol->m_vcParams[i] = pFeatureP_Edit->m_pSymbol->m_vcParams[i];
				}

				pFeatureP->m_pSymbol->SetMinMaxRect();
				pFeatureP->m_pSymbol->m_strUserSymbol = CDrawFunction::GetSymbolName(pFeatureP->m_pSymbol->m_eSymbolName,
					pFeatureP->m_pSymbol->m_vcParams);
			}

			pFeatureP->SetMinMaxRect();

			//Redraw를 위하여 Geo 삭제
			if (pFeatureP->m_pGeoObject != nullptr)
			{
				pFeatureP->m_pGeoObject->Release();
				pFeatureP->m_pGeoObject = nullptr;
			}
		}
		break;
		}
	}

	return RESULT_GOOD;
}

UINT32		CDrawManager::_SetFeature_Move_Mask(IN const MaskType &eMaskType, vector<CFeatureData*>& vecFeatureData, CStep *pStep)
{
	int nDataSize = static_cast<int>(vecFeatureData.size());

	if (nDataSize <= 0) return RESULT_BAD;
	if (pStep == nullptr) return RESULT_BAD;

	for (int nfeno = 0; nfeno < nDataSize; nfeno++)
	{
		CFeatureData *pFeatureData = vecFeatureData[nfeno];

		CLayer *pLayer = pStep->m_arrLayer.GetAt(pFeatureData->m_nLayer);
		if (pLayer == nullptr) continue;

		int nMaskIndex = pFeatureData->m_nMaskIndex;
		int nFeatureIndex = pFeatureData->m_nFeatureIndex;

		CFeature *pFeature = nullptr;

		if (eMaskType == MaskType::enumType_Align)
		{
			CAlignMask *pAlignMask = pLayer->m_arrAlign.GetAt(nFeatureIndex);
			if (pAlignMask == nullptr) continue;

			pFeature = pAlignMask->m_arrFeature.GetAt(nMaskIndex);
		}
		else if (eMaskType == MaskType::enumType_Mask)
		{
			CAlignMask *pAlignMask = pLayer->m_arrMask.GetAt(nFeatureIndex);
			if (pAlignMask == nullptr) continue;

			pFeature = pAlignMask->m_arrFeature.GetAt(nMaskIndex);
		}
		else
		{
			continue;
		}

		if (pFeature == nullptr) continue;

		if (pFeatureData->m_pFeature->m_eType != pFeature->m_eType) continue;

		switch (pFeature->m_eType)
		{
		
		case FeatureType::P:
		{
			CFeatureP *pFeatureP = (CFeatureP *)pFeature;
			CFeatureP *pFeatureP_Edit = (CFeatureP *)pFeatureData->m_pFeature;

			pFeatureP->m_dX = pFeatureP_Edit->m_dX;
			pFeatureP->m_dY = pFeatureP_Edit->m_dY;

			pFeatureP->SetMinMaxRect();

			//Redraw를 위하여 Geo 삭제
			if (pFeatureP->m_pGeoObject != nullptr)
			{
				pFeatureP->m_pGeoObject->Release();
				pFeatureP->m_pGeoObject = nullptr;
			}
		}
		break;
		
		break;
		}

	}

	return RESULT_GOOD;
}

UINT32		CDrawManager::_SetFeature_Edit_Mask(IN const MaskType &eMaskType, vector<CFeatureData*>& vecFeatureData, CStep *pStep)
{
	int nDataSize = static_cast<int>(vecFeatureData.size());

	if (nDataSize <= 0) return RESULT_BAD;
	if (pStep == nullptr) return RESULT_BAD;

	for (int nfeno = 0; nfeno < nDataSize; nfeno++)
	{
		CFeatureData *pFeatureData = vecFeatureData[nfeno];

		CLayer *pLayer = pStep->m_arrLayer.GetAt(pFeatureData->m_nLayer);
		if (pLayer == nullptr) continue;

		int nMaskIndex = pFeatureData->m_nMaskIndex;
		int nFeatureIndex = pFeatureData->m_nFeatureIndex;

		CFeature *pFeature = nullptr;

		if (eMaskType == MaskType::enumType_Align)
		{
			CAlignMask *pAlignMask = pLayer->m_arrAlign.GetAt(nMaskIndex);
			if (pAlignMask == nullptr) continue;

			pFeature = pAlignMask->m_arrFeature.GetAt(nFeatureIndex);
		}
		else if (eMaskType == MaskType::enumType_Mask)
		{
			CAlignMask *pAlignMask = pLayer->m_arrMask.GetAt(nMaskIndex);
			if (pAlignMask == nullptr) continue;

			pFeature = pAlignMask->m_arrFeature.GetAt(nFeatureIndex);
		}
		else
		{
			continue;
		}

		if (pFeature == nullptr) continue;

		if (pFeatureData->m_pFeature->m_eType != pFeature->m_eType) continue;

		switch (pFeature->m_eType)
		{

		case FeatureType::P:
		{
			CFeatureP *pFeatureP = (CFeatureP *)pFeature;
			CFeatureP *pFeatureP_Edit = (CFeatureP *)pFeatureData->m_pFeature;

			if (pFeatureP == nullptr) continue;
			if (pFeatureP->m_pSymbol == nullptr) continue;
			if (pFeatureP->m_pSymbol->m_vcParams.size() <= 0) continue;

			if (pFeatureP_Edit == nullptr) continue;
			if (pFeatureP_Edit->m_pSymbol == nullptr) continue;
			if (pFeatureP_Edit->m_pSymbol->m_vcParams.size() <= 0) continue;

			int nParamCount = static_cast<int>(pFeatureP->m_pSymbol->m_vcParams.size());
			int nParamCount_Edit = static_cast<int>(pFeatureP_Edit->m_pSymbol->m_vcParams.size());

			if (nParamCount == nParamCount_Edit)
			{
				for (int i = 0; i < nParamCount; i++)
				{
					pFeatureP->m_pSymbol->m_vcParams[i] = pFeatureP_Edit->m_pSymbol->m_vcParams[i];
				}

				pFeatureP->m_pSymbol->SetMinMaxRect();
				pFeatureP->m_pSymbol->m_strUserSymbol = CDrawFunction::GetSymbolName(pFeatureP->m_pSymbol->m_eSymbolName,
					pFeatureP->m_pSymbol->m_vcParams);
			}

			pFeatureP->m_dX = pFeatureP_Edit->m_dX;
			pFeatureP->m_dY = pFeatureP_Edit->m_dY;

			pFeatureP->SetMinMaxRect();

			//Redraw를 위하여 Geo 삭제
			if (pFeatureP->m_pGeoObject != nullptr)
			{
				pFeatureP->m_pGeoObject->Release();
				pFeatureP->m_pGeoObject = nullptr;
			}
		}//end case
		break;
		}//end switch

	}//end for 

	m_pDrawEdit_Temp->m_bMoved = FALSE;

	return RESULT_GOOD;
}

UINT32 CDrawManager::OnLButtonDown(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_CONTROL) == MK_CONTROL)
	{//Panning
		HCURSOR hHandCursor = ::LoadCursor(NULL, IDC_HAND);
		SetCursor(hHandCursor);

		m_bPanMode = TRUE;
		m_bRectZoom = FALSE;
		m_ptPrevMove = point;
	}
	else
	{
		HCURSOR hHandCursor = ::LoadCursor(NULL, IDC_ARROW);
		SetCursor(hHandCursor);

		m_bPanMode = FALSE;
		m_bRectZoom = FALSE;

// 		if ((nFlags & MK_SHIFT) == MK_SHIFT)
// 		{
// 			D2D1_POINT_2F fptPoint = D2D1::Point2F(static_cast<float>(point.x), static_cast<float>(point.y));
// 			D2D1_POINT_2F fptReal = CDrawFunction::DetansformPoint(fptPoint, m_ViewScale);
// 
// 			m_nPointIndex -= 1;
// 			m_pDrawEdit_Temp->SetTempPoints(m_nPointIndex, m_strPointSet_Name, fptReal.x, fptReal.y);
// 		}
// 		else
// 		{
// 			D2D1_POINT_2F fptPoint = D2D1::Point2F(static_cast<float>(point.x), static_cast<float>(point.y));
// 			D2D1_POINT_2F fptReal = CDrawFunction::DetansformPoint(fptPoint, m_ViewScale);
// 			m_pDrawEdit_Temp->SetTempPoint(m_nPointIndex, m_strPointSet_Name, fptReal.x, fptReal.y);
// 		}

// 
// 		if (m_bPointSetMode == TRUE)
// 		{//Set Temp Point
// 			D2D1_POINT_2F fptPoint = D2D1::Point2F(static_cast<float>(point.x), static_cast<float>(point.y));
// 			D2D1_POINT_2F fptReal = CDrawFunction::DetansformPoint(fptPoint, m_ViewScale);
// 			
// 			m_pDrawEdit_Temp->SetTempPoint(m_nPointIndex, m_strPointSet_Name, fptReal.x, fptReal.y);
// 		}


		if (m_pDirectView == nullptr) return RESULT_BAD;

		D2D1_POINT_2F fptPixel = D2D1::Point2F(static_cast<float>(point.x), static_cast<float>(point.y));

		//D2D1_POINT_2F fptOrg_mm = CDrawFunction::DetansformPoint(fptPixel, m_ViewScale);
		
		CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
		if (m_nEditMode == EditMode::enumMode_EditMask ||
			m_nEditMode == EditMode::enumMode_EditMask_Add_FeatureBase)
		{//Align, Mask 모드

			m_bMoveMode = FALSE;
			UINT32 nRet = RESULT_GOOD;
			nRet = m_pDrawEdit_Temp->OnLButtonDown(nFlags, point);
			if (nRet != RESULT_GOOD)
			{//edit temp에서 선택된 것이 없다면
				m_pDirectView->OnLButtonDown(nFlags, fptPixel);

				//Align / Mask의 변경이 있을 수도 있기 때문에 Feature Data를 업데이트 한다.

				//CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
				if (pStep != nullptr)
				{
					m_pDrawEdit_Temp->ClearFeatureData();
					m_pDrawEdit_Temp->SetMaskData(m_eMaskSelectMode, pStep);

					CDirect_Base::Render();

					nRet = m_pDrawEdit_Temp->OnLButtonDown(nFlags, point);
				}

			}
		}		
		else if (m_nEditMode == EditMode::enumMode_EditAdd ||
			m_nEditMode == EditMode::enumMode_EditTP ||
			m_nEditMode == EditMode::enumMode_EditTP_Center  )
		{//Add Edits
			UINT32 nRet = RESULT_GOOD;
			nRet = m_pDrawEdit_Temp->OnLButtonDown(nFlags, point);
			if (nRet != RESULT_GOOD)
			{//edit temp에서 선택된 것이 없다면

				BOOL bSelect = Hit_Test();
				if (bSelect == FALSE)
				{
					m_pDirectView->OnLButtonDown(nFlags, fptPixel);
				}
				else
				{
					ResetSelect();
				}

				if (pStep != nullptr)
				{
					m_pDrawEdit_Temp->m_totalMove = D2D1::Point2F(0.f, 0.f);
					m_KeyMoveXY = CPointD(0, 0);
					m_pDrawEdit_Temp->ClearFeatureData();
					m_pDrawEdit_Temp->SetFeatureData(pStep);
					
					CDirect_Base::Render();
				}

			}
			//m_pDirectView->OnLButtonDown(nFlags, fptPixel);
			nRet = m_pDrawEdit_Temp->OnLButtonDown(nFlags, point);

			/*if (nRet != RESULT_GOOD)
			{
				ResetSelect();
			}*/
		}
		else
		{
			UINT32 nRet = RESULT_GOOD;
			nRet = m_pDrawEdit_Temp->OnLButtonDown(nFlags, point);
			if (nRet == RESULT_BAD)
			{
				m_pDirectView->OnLButtonDown(nFlags, fptPixel);
				if (pStep != nullptr)
				{
					m_pDrawEdit_Temp->m_totalMove = D2D1::Point2F(0.f, 0.f);
					m_KeyMoveXY = CPointD(0, 0);
					m_pDrawEdit_Temp->ClearFeatureData();
					m_pDrawEdit_Temp->SetFeatureData(pStep);


					FeatureSpec.dRatio = 1.f;
					FeatureSpec.dMoveX = 0.f;
					FeatureSpec.dMoveY = 0.f;
					FeatureSpec.dMoveX_Key = 0.f;
					FeatureSpec.dMoveY_Key = 0.f;
					m_KeyMove.x = 0.f;
					m_KeyMove.y = 0.f;

					CDirect_Base::Render();
				}
			}
			nRet = m_pDrawEdit_Temp->OnLButtonDown(nFlags, point);		
			
			//if (nRet != RESULT_GOOD)
			//{
			//	ResetSelect();
			//}
			
			m_bResizeMode = FALSE;
			m_bMoveMode = FALSE;

			
			
			FeatureSpec.bWheelMode = FALSE;			
			FeatureSpec.bDragMode = FALSE;
			FeatureSpec.bKeyMode = FALSE;
			m_pDrawEdit_Temp->m_bMoved = FALSE;					
		}		
		// 22.6.13 장동진 Feature Move 
		
	}

	CDirect_Base::Render();

	return RESULT_GOOD;
}


UINT32 CDrawManager::SetSelectPoint_Center(IN const int &nIndex, IN const float fX, IN const float fY)
{
	Surfacepoint SurfacePoint;

	m_vecPoint[nIndex].dCenterX = fX;
	m_vecPoint[nIndex].dCenterY = fY;
	m_vecPoint[nIndex].bCCW = true;
	m_vecPoint[nIndex].bOsOc = false;

	//m_vecPoint.push_back(SurfacePoint);

	m_nPointIndex = nIndex + 1;
	

	SetPointMode(TRUE, m_nPointIndex, _T("Add Center"));

	return RESULT_GOOD;

}

UINT32 CDrawManager::SetSelectPoint_Add(IN const int nIndex, IN const float fX, IN const float fY)
{
	Surfacepoint SurfacePoint;

	SurfacePoint.dAddX = fX;
	SurfacePoint.dAddY = fY;

	m_vecPoint.push_back(SurfacePoint);

	m_nPointIndex = nIndex;

	m_nPointIndex++;

	SetPointMode(TRUE, m_nPointIndex, _T("Add Point"));

	return RESULT_GOOD;
}


UINT32 CDrawManager::SetSelectPoint(IN const float fX, IN const float fY)
{

	m_stParam.dCenterX = fX;
	m_stParam.dCenterY = fY;

	return RESULT_GOOD;
}

UINT32 CDrawManager::SetSelect_EndPoint(IN const float fXe, IN const float fYe)
{
	m_stParam.dEndX = fXe;
	m_stParam.dEndY = fYe;

	return RESULT_GOOD;
}


UINT32 CDrawManager::SetSelect_StartPoint(IN const float fXs, IN const float fYs)
{

	m_stParam.dStartX = fXs;
	m_stParam.dStartY = fYs;
	


	return RESULT_GOOD;
}

UINT32 CDrawManager::OnLButtonUp(UINT nFlags, CPoint point)
{
	HCURSOR hHandCursor = ::LoadCursor(NULL, IDC_ARROW);
	SetCursor(hHandCursor);

	m_bPanMode = FALSE;

	m_pDirectView->OnLButtonUp(nFlags, point);
	m_pDrawEdit_Temp->OnLButtonUp(nFlags, point);
	
	
// 	if (m_bMoveMode == TRUE)
// 	{
// 		//CDirect_Base::Render()
// 		m_bMoveMode = FALSE;
// 	}
	
	// 22.6.13 장동진 Feature Move

	if (m_bPointSetMode == TRUE)
	{//Set Temp Point
		if ((nFlags & MK_SHIFT) == MK_SHIFT)
		{//Arc : Center
			D2D1_POINT_2F fptPoint = D2D1::Point2F(static_cast<float>(point.x), static_cast<float>(point.y));
			D2D1_POINT_2F fptReal = CDrawFunction::DetansformPoint(fptPoint, m_ViewScale, m_pUserSetInfo);


			m_nPointIndex -= 1;
			m_pDrawEdit_Temp->SetTempPoints(m_nPointIndex, m_strPointSet_Name, fptReal.x, fptReal.y);

		}
		else
		{//
			D2D1_POINT_2F fptPoint = D2D1::Point2F(static_cast<float>(point.x), static_cast<float>(point.y));
			D2D1_POINT_2F fptReal = CDrawFunction::DetansformPoint(fptPoint, m_ViewScale, m_pUserSetInfo);
			m_pDrawEdit_Temp->SetTempPoint(m_nPointIndex, m_strPointSet_Name, fptReal.x, fptReal.y);
		}

	}

	CDirect_Base::Render();

	return RESULT_GOOD;
}

UINT32 CDrawManager::OnMouseMove(UINT nFlags, CPoint point)
{
	

	if (m_bPanMode == TRUE)
	{
		HCURSOR hHandCursor = ::LoadCursor(NULL, IDC_HAND);
		SetCursor(hHandCursor);

		D2D_POINT_2F fptCurMove;
		//fptCurMove.x = (point.x - m_ptPrevMove.x) / m_matScaleView.m11;
		//fptCurMove.y = (point.y - m_ptPrevMove.y) / m_matScaleView.m11;

		fptCurMove.x = static_cast<float>(point.x - m_ptPrevMove.x);
		fptCurMove.y = static_cast<float>(point.y - m_ptPrevMove.y);

		//m_matTransView

		//m_matTransView = D2D1::Matrix3x2F::Translation(500, 300);
		m_fptPan.x += fptCurMove.x;
		m_fptPan.y += fptCurMove.y;

		m_ptPrevMove = point;

		CDirect_Base::Render();

	}
	else if (m_bRectZoom == TRUE)
	{
		m_rtRectZoom.right = point.x;
		m_rtRectZoom.bottom = point.y;

		m_rtRectZoom.NormalizeRect();

		CDirect_Base::Render();
	}
	else if (m_bMeasureMode == TRUE && m_bMeasureMode_Setting == TRUE )
	{
		D2D1_POINT_2F fptPoint = D2D1::Point2F(static_cast<float>(point.x), static_cast<float>(point.y));
		D2D1_POINT_2F fptReal = CDrawFunction::DetansformPoint(fptPoint, m_ViewScale, m_pUserSetInfo);
		m_frtMeasure.right = fptReal.x;
		m_frtMeasure.bottom = fptReal.y;

		

		CDirect_Base::Render();
	}
	else
	{

		if (m_bResizeMode == TRUE)
		{
			return RESULT_BAD;
		}

		UINT32 nResult;
		nResult = m_pDirectView->OnMouseMove(nFlags, point);

		/*if (nResult == RESULT_BAD)
		{
			CDirect_Base::Render();
				return RESULT_BAD;
		}
		else
		{
		
			m_pDrawEdit_Temp->OnMouseMove(nFlags, point);
		}*/
		m_pDrawEdit_Temp->OnMouseMove(nFlags, point);

		if (m_nEditMode != EditMode::enumMode_None)
		{
			CDirect_Base::Render();
		}
	}

	return RESULT_GOOD;
}

UINT32 CDrawManager::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	
	m_bSelect = Hit_Test();
	m_bResizeMode = false;

	BOOL bMoveResize = FALSE;
	
	if ((nFlags & MK_CONTROL) == MK_CONTROL)
	{
		if (zDelta > 0)
			ZoomIn();
		else if (zDelta < 0)
			ZoomOut();

		CDirect_Base::Render();
	}
	else if (m_bSelect == true )
	{
		if (zDelta > 0)
		{				
			m_dResizeRatio *= 1.1;			
			m_bResizeMode = true;
			m_bWheelMode = true;
			
		
		}
		else if (zDelta < 0)
		{	
			m_dResizeRatio *= 0.9;
			m_bResizeMode = true;
			m_bWheelMode = true;
			
		}

		
		FeatureSpec.dRatio = m_dResizeRatio;
		FeatureSpec.bWheelMode = m_bWheelMode;

		if (m_bResizeMode == true) 
		{
			if (m_dResizeRatio <= 0.01f)
			{
				return RESULT_BAD;
			}
			SetResize(m_dResizeRatio);
			CDirect_Base::Render();					
		}


		
	}

	else if  (GetEditMode()== EditMode::enumMode_EditMask || GetEditMode() == EditMode::enumMode_EditMask_Add_FeatureBase)
	{

		if (zDelta > 0)
		{
			m_dResize_Ratio *= 1.1f;
			
		}
		else if (zDelta < 0)
		{
			m_dResize_Ratio *= 0.9f;
		}
		float fResize_Ratio = static_cast<float>(m_dResize_Ratio);
			if (fResize_Ratio <= 0) return RESULT_BAD;

		SetResize_Mask(m_eMaskSelectMode, fResize_Ratio, fResize_Ratio);
		CDirect_Base::Render();
		
	}

	
	
	
	

	return RESULT_GOOD;
}

UINT32 CDrawManager::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	ResetZoom();

	CDirect_Base::Render();

	return RESULT_GOOD;
}


UINT32 CDrawManager::OnRButtonDown(UINT nFlags, CPoint point)
{
	if ((nFlags & MK_SHIFT) == MK_SHIFT)
	{
		HCURSOR hHandCursor = ::LoadCursor(NULL, IDC_ARROW);
		SetCursor(hHandCursor);

		m_bPanMode = FALSE;
		m_bRectZoom = TRUE;

		m_bMeasureMode = FALSE;
		m_bMeasureMode_Setting = FALSE;

		m_rtRectZoom.left = point.x;
		m_rtRectZoom.top = point.y;
		m_rtRectZoom.right = point.x;
		m_rtRectZoom.bottom = point.y;

		CDirect_Base::Render();
	}
	else if ((nFlags & MK_CONTROL) == MK_CONTROL)
	{
		HCURSOR hHandCursor = ::LoadCursor(NULL, IDC_ARROW);
		SetCursor(hHandCursor);

		m_bPanMode = FALSE;
		m_bRectZoom = FALSE;

		m_bMeasureMode = TRUE;
		m_bMeasureMode_Setting = TRUE;

		D2D1_POINT_2F fptPoint = D2D1::Point2F(static_cast<float>(point.x), static_cast<float>(point.y));
		D2D1_POINT_2F fptReal = CDrawFunction::DetansformPoint(fptPoint, m_ViewScale, m_pUserSetInfo);
		m_frtMeasure.left = fptReal.x;
		m_frtMeasure.top = fptReal.y;
		m_frtMeasure.right = fptReal.x;
		m_frtMeasure.bottom = fptReal.y;

		CDirect_Base::Render();

	}

	return RESULT_GOOD;
}

UINT32 CDrawManager::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (m_bRectZoom == TRUE)
	{
		m_bRectZoom = FALSE;
		m_rtRectZoom.right = point.x;
		m_rtRectZoom.bottom = point.y;

		m_rtRectZoom.NormalizeRect();

		// zoom size가 2보다 클때만 zoom 적용
		if (m_rtRectZoom.Width() > 2 || m_rtRectZoom.Height() > 2)
		{
			_SetZoomRect(m_rtRectZoom);
		}

		CDirect_Base::Render();
	}
	else if (m_bMeasureMode == TRUE && m_bMeasureMode_Setting == TRUE)
	{
		m_bMeasureMode_Setting = FALSE;

		D2D1_POINT_2F fptPoint = D2D1::Point2F(static_cast<float>(point.x), static_cast<float>(point.y));
		D2D1_POINT_2F fptReal = CDrawFunction::DetansformPoint(fptPoint, m_ViewScale, m_pUserSetInfo);

		m_frtMeasure.right = fptReal.x;
		m_frtMeasure.bottom = fptReal.y;
	}

	return RESULT_GOOD;
}


UINT32 CDrawManager::ConvertPos(CPoint &ptSrc, double &dDstX, double &dDstY)
{
	D2D1_POINT_2F fptPoint = D2D1::Point2F(static_cast<float>(ptSrc.x), static_cast<float>(ptSrc.y));

	D2D1_POINT_2F fptPointTrans = CDrawFunction::DetansformPoint(fptPoint, m_ViewScale, m_pUserSetInfo);

	dDstX = fptPointTrans.x / 25.4;
	dDstY = fptPointTrans.y / 25.4;

	if (m_pUserSetInfo == nullptr) return RESULT_BAD;

	if (m_pUserSetInfo->coordUnit == eCoordinateUnit::eInchToMM)
	{
		dDstX = dDstX * 25.4;
		dDstY = dDstY * 25.4;
	}

	return RESULT_GOOD;
}

UINT32 CDrawManager::GetPanPos_ForDebug(OUT float &fPanX, OUT float &fPanY)
{
	fPanX = m_fptPan.x;
	fPanY = m_fptPan.y;

	return RESULT_GOOD;
}


UINT32 CDrawManager::ClearDisplay()
{
	CDirect_Base::ClearDisplay();

	SetStepIndex(-1);

	//CDirect_Base::Render();

	return RESULT_GOOD;
}


UINT32  CDrawManager ::SetCW()
{
	if (m_nPointIndex < 0) return RESULT_BAD;

	int nPreIndex = m_nPointIndex - 1;
	if (nPreIndex < 0) return RESULT_BAD;

	m_pDrawEdit_Temp->Set_CW(nPreIndex);
	 
	if (m_vecPoint[nPreIndex].bOsOc == false && m_vecPoint[nPreIndex].bCCW == false)
	{
		m_vecPoint[nPreIndex].bCCW = true;
	}
	else
	{
		m_vecPoint[nPreIndex].bCCW = false;
	}

	CDirect_Base::Render();

	return RESULT_GOOD;

	
}
//EditMode
UINT32			CDrawManager::SetEditMode(IN const UINT32 &nEditMode)
{
	m_nEditMode = nEditMode;

	switch (m_nEditMode)
	{
	case EditMode::enumMode_None:
	{
		m_pDirectView->ClearPreSelect();
		m_pDirectView->ResetSelect();
		m_pDrawEdit_Temp->ClearFeatureData();
	}
	break;	
	case EditMode::enumMode_EditMode:
	{
		/*m_pDrawEdit_Temp->ClearTempPoint();
		m_pDrawEdit_Temp->ClearFeatureData();*/
	}
	break;
	case EditMode ::enumMode_EditMove:
	{
		if (m_nSelectStep == -1) break;
		if (m_pJobData == nullptr) break;

		CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
		if (pStep == nullptr) break;

		m_pDrawEdit_Temp->SetFeatureData(pStep);
	}
	break;
	case EditMode::enumMode_EditResize:
	{
		if (m_nSelectStep == -1) break;
		if (m_pJobData == nullptr) break;

		CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
		if (pStep == nullptr) break;

		m_pDrawEdit_Temp->SetFeatureData(pStep);

	}
	break;	
	case EditMode::enumMode_EditAdd:
	{
		m_pDrawEdit_Temp->ClearTempPoint();
		m_pDrawEdit_Temp->ClearFeatureData();
	}
	break;
	case EditMode::enumMode_EditCopy:
	{
		if (m_nSelectStep == -1) break;
		if (m_pJobData == nullptr) break;

		CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
		if (pStep == nullptr) break;

		m_pDrawEdit_Temp->SetFeatureData(pStep);
	}
	break;
	case EditMode::enumMode_EditRotate:
	{
		if (m_nSelectStep == -1) break;
		if (m_pJobData == nullptr) break;

		CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
		if (pStep == nullptr) break;

		m_pDrawEdit_Temp->SetFeatureData(pStep);
	}
	break;
	case EditMode::enumMode_EditTP:
	{
		m_pDrawEdit_Temp->ClearTempPoint();
		m_pDrawEdit_Temp->ClearFeatureData();
	}
	break;	
	case EditMode::enumMode_EditTP_Center:
	{
		m_pDrawEdit_Temp->ClearTempPoint();
		m_pDrawEdit_Temp->ClearFeatureData();
	}
	break;
	case EditMode::enumMode_EditMask:	
	case  EditMode::enumMode_EditMask_Add_FeatureBase:
	{
		m_pDirectView->ResetSelect();
		m_pDrawEdit_Temp->ClearTempPoint();
		m_pDrawEdit_Temp->ClearFeatureData();
	}
	break;
	}

	CDirect_Base::Render();

	return RESULT_GOOD;
}

UINT32				CDrawManager::GetEditMode()
{
	return m_nEditMode;
}

//Save
UINT32 CDrawManager::SaveLayer_Test(IN const double &dResolution, IN const CString &strPathName)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = m_pDirectView->SaveLayer_Test(dResolution, strPathName);

	return nRet;
}

//Feature Select
UINT32			CDrawManager::ResetSelect()
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	UINT32 nRet = RESULT_GOOD;
// 	if (m_nEditMode == EditMode::enumMode_EditMode || 
// 		m_eMaskSelectMode != MaskType::enumType_None ||
// 		m_nEditMode == EditMode::enumMode_EditTP)
	{
		nRet = m_pDirectView->ResetSelect();
		nRet |= m_pDrawEdit_Temp->ClearFeatureData();
	}

	m_bMeasureMode = FALSE;

	CDirect_Base::Render();

	return nRet;
}

UINT32			CDrawManager::SetSelect(vector<FEATURE_INFO> &vecFeatureInfo)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = m_pDirectView->SetSelect(vecFeatureInfo);

	CDirect_Base::Render();

	return nRet;
}

UINT32			CDrawManager::SetUnSelect(vector<FEATURE_INFO> &vecFeatureInfo)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = m_pDirectView->SetUnSelect(vecFeatureInfo);

	CDirect_Base::Render();

	return nRet;
}

UINT32			CDrawManager::UpdateSelectInfo()//선택된 Feature의 Status bar에 정보 출력을 위한 정보 취합
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = m_pDirectView->UpdateSelectInfo();

	return nRet;
}

//Feature Show/Hide
UINT32			CDrawManager::ResetHide()
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = m_pDirectView->ResetHide();

	CDirect_Base::Render();

	return nRet;
}

UINT32			CDrawManager::SetShow(vector<FEATURE_INFO> &vecFeatureInfo)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = m_pDirectView->SetShow(vecFeatureInfo);

	CDirect_Base::Render();

	return nRet;
}

UINT32			CDrawManager::SetHide(vector<FEATURE_INFO> &vecFeatureInfo)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = m_pDirectView->SetHide(vecFeatureInfo);

	CDirect_Base::Render();

	return nRet;
}

//Edit
//Delete
UINT32			CDrawManager::SetDelete_Apply()
{//현재 선택된 Feature Delet
	if (m_pDirectView == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = CEditFunction::SetDelete(m_pJobData, m_nSelectStep, m_pUserLayerSetInfo);

	m_pDrawEdit_Temp->ClearFeatureData();

	m_pDirectView->SetRedrawGeo();

	CDirect_Base::Render();

	return nRet;
}

UINT32			CDrawManager::SetMove_Apply()
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	vector<CFeatureData*> vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr )return RESULT_BAD;

	UINT32 nRet;
	nRet = _SetFeature_Move(vecFeatureData, pStep);

	m_pDirectView->SetRedrawGeo();
	
	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();

	FeatureSpec.dRatio = 1;
	
	m_pDrawEdit_Temp->m_totalMove.x = 0;
	m_pDrawEdit_Temp->m_totalMove.y = 0;
	
	m_bMoveMode = FALSE;

	return nRet;
}

UINT32			CDrawManager::ClearTempPoint()
{
	if (m_pDrawEdit_Temp == nullptr) return RESULT_BAD;
	
	m_pDrawEdit_Temp->ClearTempPoint();

	CDirect_Base::Render();

	return RESULT_GOOD;
}

UINT32			CDrawManager::SetPointMode(IN const BOOL &bMode, IN const int &nIndex, IN const CString &strName)
{
	m_bPointSetMode = bMode;
	if (m_bPointSetMode == TRUE)
	{
		m_nPointIndex = nIndex;
		m_strPointSet_Name = strName;
	}
	else
	{
		m_nPointIndex = -1;
		m_strPointSet_Name = _T("");
	}

	return RESULT_GOOD;
}

BOOL			CDrawManager::GetPointMode()
{
	return m_bPointSetMode;
}

UINT32			CDrawManager::GetTempPoint(IN const int &nIndex, float &fX, float &fY)
{
	fX = 0.f;
	fY = 0.f;

	if (m_pDrawEdit_Temp == nullptr) return RESULT_BAD;
	TempPoint stTempPoint = m_pDrawEdit_Temp->GetTempPoint(nIndex);

	fX = stTempPoint.fptPoint.x;
	fY = stTempPoint.fptPoint.y;
	

	return RESULT_GOOD;
}

UINT32			CDrawManager::GetTempPoint_Center(IN const int &nIndex, float &fX, float &fY)
{
	fX = 0.f;
	fY = 0.f;

	if (m_pDrawEdit_Temp == nullptr) return RESULT_BAD;
	TempPoint stTempPoint = m_pDrawEdit_Temp->GetTempPoint(nIndex);

	fX = stTempPoint.fptPoint2.x;
	fY = stTempPoint.fptPoint2.y;

	return RESULT_GOOD;
}

UINT32			CDrawManager::SetAdd(IN const CString &strLayer, vector<CFeature*> &vecFeature)
{
	if (m_pDrawEdit_Temp == nullptr) return RESULT_BAD;

	m_pDrawEdit_Temp->ClearFeatureData();

	//Get Layer Index;
	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr) return RESULT_BAD;

	int nLayer;
	CLayer *pLayerTmp = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, strLayer, nLayer);
	if (pLayerTmp == nullptr) return RESULT_BAD;

	int nFeatureIndex = static_cast<int>(pLayerTmp->m_FeatureFile.m_arrFeature.GetCount());

	int nCount = static_cast<int>(vecFeature.size());
	for (int i = 0; i < nCount; i++)
	{
		CFeature *pFeature = vecFeature[i];
		if (pFeature == nullptr ) continue;

		m_pDrawEdit_Temp->AddFeatureData(nLayer, nFeatureIndex + i, pFeature);
	}

	CDirect_Base::Render();

	return RESULT_GOOD;
}

UINT32			CDrawManager::SetAdd_Apply(IN const CString &strLayer, vector<CFeature*> &vecFeature)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	//PreView 를 호출하지 않고 Apply를 하는 경우도 있기 때문에 
	//SetAdd를 호출 후 Apply한다.
	SetAdd(strLayer, vecFeature);

	vector<CFeatureData*> vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	UINT32 nRet;
	nRet = _SetFeature_Add(vecFeatureData, pStep);

	CDirect_Base::Render();

	return nRet;
}

UINT32			CDrawManager::SetAdd_Mask(IN const MaskType eMaskType, IN const CString &strLayer, vector<CFeature*> vecFeature)
{
	if (m_pDrawEdit_Temp == nullptr) return RESULT_BAD;

	m_pDrawEdit_Temp->ClearFeatureData();

	//Get Layer Index;
	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr) return RESULT_BAD;

	int nLayer;
	CLayer *pLayerTmp = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, strLayer, nLayer);
	if (pLayerTmp == nullptr) return RESULT_BAD;

	int nFeatureIndex;
	if (eMaskType == MaskType::enumType_Align)
	{
		nFeatureIndex = static_cast<int>(pLayerTmp->m_arrAlign.GetCount());		
	}
	else if (eMaskType == MaskType::enumType_Mask)
	{
		nFeatureIndex = static_cast<int>(pLayerTmp->m_arrMask.GetCount());
	}
	else
	{
		return RESULT_BAD;
	}

	int nCount = static_cast<int>(vecFeature.size());
	for (int i = 0; i < nCount; i++)
	{
		CFeature *pFeature = vecFeature[i];
		if (pFeature == nullptr) continue;

		m_pDrawEdit_Temp->AddFeatureData(nLayer, 0, pFeature, nFeatureIndex);
	}

	CDirect_Base::Render();

	return RESULT_GOOD;
}

UINT32			CDrawManager::SetAdd_Apply_Mask(IN const MaskType eMaskType, IN const CString &strLayer, vector<CFeature*> vecFeature)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	UINT32 nRet;
	//PreView 를 호출하지 않고 Apply를 하는 경우도 있기 때문에 
	//SetAdd를 호출 후 Apply한다.
	nRet = SetAdd_Mask(eMaskType, strLayer, vecFeature);
	if (nRet != RESULT_GOOD) return RESULT_BAD;

	vector<CFeatureData*> vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	nRet = _SetFeature_Add_Mask(eMaskType, vecFeatureData, pStep);

	CDirect_Base::Render();

	return nRet;
}

UINT32			CDrawManager::SetResize_Mask(IN const MaskType eMaskType, IN const float &fPercentWidth, IN const float &fPercentHeight)
{
	if (m_pDrawEdit_Temp == nullptr) return RESULT_BAD;

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr) return RESULT_BAD;

	
	PointDXY KeyMoveXY = CPointD(0, 0);
	PointDXY MouseMoveXY = CPointD(0, 0);
	PointDXY TotalMoveXY = CPointD(0, 0);


	KeyMoveXY.x = m_KeyMove.x;
	KeyMoveXY.y = m_KeyMove.y;

	MouseMoveXY.x = m_pDrawEdit_Temp->m_totalMove.x;
	MouseMoveXY.y = m_pDrawEdit_Temp->m_totalMove.y;

	TotalMoveXY.x = KeyMoveXY.x + MouseMoveXY.x;
	TotalMoveXY.y = KeyMoveXY.y + MouseMoveXY.y;


	BOOL bMove = m_pDrawEdit_Temp->m_bMoved;
	
	

	if (bMove == TRUE)
	{
		m_pDrawEdit_Temp->ClearFeatureData();

		m_pDrawEdit_Temp->SetMaskData(eMaskType, pStep);

		m_pDrawEdit_Temp->_ManualMove_Feature(TotalMoveXY);	

		m_pDrawEdit_Temp->SetMaskResize(fPercentWidth, fPercentHeight);
	}

	else
	{
		m_pDrawEdit_Temp->SetMaskData(eMaskType, pStep);

		m_pDrawEdit_Temp->SetMaskResize(fPercentWidth, fPercentHeight);

	}

// 	m_pDrawEdit_Temp->SetMaskData(eMaskType, pStep);
// 	
// 	m_pDrawEdit_Temp->SetMaskResize(fPercentWidth, fPercentHeight);




	CDirect_Base::Render();

	


	return RESULT_GOOD;
}
UINT32			CDrawManager::SetResize_Apply_Mask(IN const MaskType eMaskType, IN const float &fPercentWidth, IN const float &fPercentHeight)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	SetResize_Mask(eMaskType, fPercentWidth, fPercentHeight);

	vector<CFeatureData*> vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	UINT32 nRet;
	nRet = _SetFeature_Resize_Mask(eMaskType, vecFeatureData, pStep);
	
	m_pDrawEdit_Temp->ClearFeatureData();

	m_dResize_Ratio = 100.f;


	CDirect_Base::Render();

	return RESULT_GOOD;
}

UINT32			CDrawManager::SetMove_Apply_Mask(IN const MaskType eMaskType)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	vector<CFeatureData*> vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	UINT32 nRet;
	nRet = _SetFeature_Move_Mask(eMaskType, vecFeatureData, pStep);
	
	CDirect_Base::Render();

	return nRet;
}

UINT32 CDrawManager::SetDelete_Apply_Mask(IN const MaskType eMaskType)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = CEditFunction::SetDelete_Mask(eMaskType, m_pJobData, m_nSelectStep, m_pUserLayerSetInfo);
	
	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep != nullptr) {
		m_pDrawEdit_Temp->SetMaskData(m_eMaskSelectMode, pStep);
	}

	CDirect_Base::Render();

	return nRet;
}


UINT32			CDrawManager::SetApply_Mask(IN const MaskType eMaskType)
{//EditTemp에 있는 내용을 Feature에 업데이트 한다.

	vector<CFeatureData*> vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	_SetFeature_Edit_Mask(eMaskType, vecFeatureData, pStep);


	CDirect_Base::Render();

	m_pDrawEdit_Temp->m_totalMove.x = 0;
	m_pDrawEdit_Temp->m_totalMove.y = 0;

	m_dResize_Ratio = 100.0;


	return RESULT_GOOD;
}

UINT32			CDrawManager::SetResize_Manual_Mask(IN const MaskType eMaskType, IN const float &fPercentWidth, IN const float &fPercentHeight)
{
	SetResize_Mask(eMaskType, fPercentWidth, fPercentHeight);

	return RESULT_GOOD;
}

UINT32 CDrawManager::_SetShape(IN const double &dRadius, IN const double &dWidth, IN const double &dHeight, IN const CString &strShape)
{
	if (m_pDrawEdit_Temp == nullptr) return RESULT_BAD;

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr) return RESULT_BAD;

	m_pDrawEdit_Temp->SetPadShape(dRadius,dWidth,dHeight);
	m_pDrawEdit_Temp->SetLineShape(strShape,dWidth);
	CDirect_Base::Render();

	return RESULT_GOOD;
}


UINT32 CDrawManager::Feature_Move_KeyBoard(PointDXY &point, eKeyDir eKeyDirection)
{
	m_pDrawEdit_Temp->_ManualMove_Feature(point);
	
	if (m_pUserSetInfo == nullptr) return RESULT_BAD;

	switch (eKeyDirection)
	{
		case eKeyDir::eUP:
		{
			if (m_pUserSetInfo->dAngle == 0 && m_pUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본0도
			{
				m_KeyMove.y += static_cast<float>(point.y);
			}
			else if (m_pUserSetInfo->dAngle == 90 && m_pUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본90도 
			{
				m_KeyMove.x -= static_cast<float>(point.x);
			}
			else if (m_pUserSetInfo->dAngle == 180 && m_pUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본180도
			{
				m_KeyMove.y -= static_cast<float>(point.y);
			}
			else if (m_pUserSetInfo->dAngle == 270 && m_pUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본270도
			{
				m_KeyMove.x += static_cast<float>(point.x);
			}

			else if (m_pUserSetInfo->dAngle == 0 && m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal) //  좌우반전 0도
			{
				m_KeyMove.y += static_cast<float>(point.y);
			}
			else if (m_pUserSetInfo->dAngle == 90 && m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 90도
			{
				m_KeyMove.x -= static_cast<float>(point.x);

			}
			else if (m_pUserSetInfo->dAngle == 180 && m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 180도
			{
				m_KeyMove.y -= static_cast<float>(point.y);
			}
			else if (m_pUserSetInfo->dAngle == 270 && m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 270도
			{
				m_KeyMove.x += static_cast<float>(point.x);
			}
			break;
		}
		case::eKeyDir::eDown:
		{
			if (m_pUserSetInfo->dAngle == 0 && m_pUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본0도
			{
				m_KeyMove.y += static_cast<float>(point.y);
			}
			else if (m_pUserSetInfo->dAngle == 90 && m_pUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본90도 
			{
				m_KeyMove.x += static_cast<float>(point.x);
			}
			else if (m_pUserSetInfo->dAngle == 180 && m_pUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본180도
			{
				m_KeyMove.y += static_cast<float>(point.y);
			}
			else if (m_pUserSetInfo->dAngle == 270 && m_pUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본270도
			{
				m_KeyMove.x -= static_cast<float>(point.x);
			}

			else if (m_pUserSetInfo->dAngle == 0 && m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal) //  좌우반전 0도
			{
				m_KeyMove.y -= static_cast<float>(point.y);
			}
			else if (m_pUserSetInfo->dAngle == 90 && m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 90도
			{
				m_KeyMove.x += static_cast<float>(point.x);
			}
			else if (m_pUserSetInfo->dAngle == 180 && m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 180도
			{
				m_KeyMove.y += static_cast<float>(point.y);
			}
			else if (m_pUserSetInfo->dAngle == 270 && m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 270도
			{
				m_KeyMove.x -= static_cast<float>(point.x);
			}
			break;
		}
		case::eKeyDir::eLeft:
		{
			if (m_pUserSetInfo->dAngle == 0 && m_pUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본0도
			{
				m_KeyMove.x += static_cast<float>(point.x);
			}
			else if (m_pUserSetInfo->dAngle == 90 && m_pUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본90도 
			{
				m_KeyMove.y -= static_cast<float>(point.y);
			}
			else if (m_pUserSetInfo->dAngle == 180 && m_pUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본180도
			{
				m_KeyMove.x += static_cast<float>(point.x);
			}
			else if (m_pUserSetInfo->dAngle == 270 && m_pUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본270도
			{
				m_KeyMove.y += static_cast<float>(point.y);
			}

			else if (m_pUserSetInfo->dAngle == 0 && m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal) //  좌우반전 0도
			{
				m_KeyMove.x += static_cast<float>(point.x);
			}
			else if (m_pUserSetInfo->dAngle == 90 && m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 90도
			{
				m_KeyMove.y += static_cast<float>(point.y);
			}
			else if (m_pUserSetInfo->dAngle == 180 && m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 180도
			{
				m_KeyMove.x -= static_cast<float>(point.x);
			}
			else if (m_pUserSetInfo->dAngle == 270 && m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 270도
			{
				m_KeyMove.y -= static_cast<float>(point.y);
			}
			break;
		}
		case::eKeyDir::eRight:
		{
			if (m_pUserSetInfo->dAngle == 0 && m_pUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본0도
			{
				m_KeyMove.x += static_cast<float>(point.x);
			}
			else if (m_pUserSetInfo->dAngle == 90 && m_pUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본90도 
			{
				m_KeyMove.y += static_cast<float>(point.y);
			}
			else if (m_pUserSetInfo->dAngle == 180 && m_pUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본180도
			{
				m_KeyMove.x -= static_cast<float>(point.x);
			}
			else if (m_pUserSetInfo->dAngle == 270 && m_pUserSetInfo->mirrorDir == eDirectionType::DirectionTypeNone) // 기본270도
			{
				m_KeyMove.y -= static_cast<float>(point.y);
			}

			else if (m_pUserSetInfo->dAngle == 0 && m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal) //  좌우반전 0도
			{
				m_KeyMove.x -= static_cast<float>(point.x);
			}
			else if (m_pUserSetInfo->dAngle == 90 && m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 90도
			{
				m_KeyMove.y -= static_cast<float>(point.y);
			}
			else if (m_pUserSetInfo->dAngle == 180 && m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 180도
			{
				m_KeyMove.x += static_cast<float>(point.x);
			}
			else if (m_pUserSetInfo->dAngle == 270 && m_pUserSetInfo->mirrorDir == eDirectionType::eHorizontal) // 좌우반전 270도
			{
				m_KeyMove.y += static_cast<float>(point.y);
			}
			break;
		}
	}

	/*m_KeyMove.x += static_cast<float>(point.x);
	m_KeyMove.y += static_cast<float>(point.y);*/

	FeatureSpec.dMoveX_Key= m_KeyMove.x;
	FeatureSpec.dMoveY_Key = m_KeyMove.y;
	FeatureSpec.bKeyMode = TRUE;

	m_bKeyMode = TRUE;

	CDirect_Base::Render();

	return RESULT_GOOD;
}

UINT32			CDrawManager::SetResize(IN const double &dResizePercent)
{
	if (m_pDrawEdit_Temp == nullptr) return RESULT_BAD;

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr) return RESULT_BAD;


	// 조건 추가 예정 
	//if (m_bMoveMode == TRUE) return RESULT_BAD;
	
	m_pDrawEdit_Temp->ClearFeatureData();
	m_pDrawEdit_Temp->SetFeatureData(pStep);
	m_pDrawEdit_Temp->SetLineArcResize(dResizePercent);
	m_pDrawEdit_Temp->SetPadTextResize(dResizePercent);
	m_pDrawEdit_Temp->SetSurfaceResize(dResizePercent);

	BOOL bMove = m_pDrawEdit_Temp->m_bMoved;

	if (bMove == TRUE && m_bWheelMode == TRUE)
	{
		
		/*PointDXY m_TotalMoveXY = CPointD(0, 0);*/

				
		m_KeyMoveXY.x = m_KeyMove.x;
		m_KeyMoveXY.y = m_KeyMove.y;

		m_MouseMoveXY.x = m_pDrawEdit_Temp->m_totalMove.x;
		m_MouseMoveXY.y = m_pDrawEdit_Temp->m_totalMove.y;
		
		m_TotalMoveXY.x = m_KeyMoveXY.x + m_MouseMoveXY.x;
		m_TotalMoveXY.y = m_KeyMoveXY.y + m_MouseMoveXY.y;

// 		m_pDrawEdit_Temp->ClearFeatureData();
// 		m_pDrawEdit_Temp->SetFeatureData(pStep);

		m_pDrawEdit_Temp->_ManualMove_Feature(m_TotalMoveXY);
// 		m_pDrawEdit_Temp->SetLineArcResize(dResizePercent);
// 		m_pDrawEdit_Temp->SetPadTextResize(dResizePercent);

		//m_pDrawEdit_Temp->SetSurfaceResize(dResizePercent);

		bMove = FALSE;
		m_bMoveMode = FALSE;

		
	}
	
	

	

	

// 	if (m_bResizeMode == TRUE)
// 	{
// 		m_pDrawEdit_Temp->SetFeatureData(pStep);
// 		m_pDrawEdit_Temp->SetResize(fResizePercent);
// 	}
	



	//CDirect_Base::Render();

	return RESULT_GOOD;
}

UINT32			CDrawManager::SetResize_Apply(IN const float &fResizePercent)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	SetResize(fResizePercent);

	vector<CFeatureData*> vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	UINT32 nRet;
	nRet = _SetFeature_Resize_LineArc(vecFeatureData, pStep);



	m_pDirectView->SetRedrawGeo();

	CDirect_Base::Render();

	return RESULT_GOOD;
}


UINT32			CDrawManager::SetRotate(IN const int&nIndex, IN const BOOL&bMirror)
{
	if (m_pDrawEdit_Temp == nullptr) return RESULT_BAD;
	

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr) return RESULT_BAD;

	m_pDrawEdit_Temp->SetFeatureData(pStep);

	m_pDrawEdit_Temp->SetPadRotate(nIndex, bMirror);

	CDirect_Base::Render();

	return RESULT_GOOD;
}

UINT32			CDrawManager::SetRotate_Apply(IN const int&nIndex, IN const BOOL&bMirror)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	SetRotate(nIndex,bMirror);

	vector<CFeatureData*> vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();

	

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	UINT32 nRet;
	nRet = _SetFeature_Rotate(vecFeatureData, pStep);



	m_pDirectView->SetRedrawGeo();

	CDirect_Base::Render();

	return nRet;
}

UINT32			CDrawManager::SetTempData_Apply()
{

	UINT32 nRet;
	if (bCheckRepeat != TRUE)
		nRet = CEditFunction::SetDelete(m_pJobData, m_nSelectStep, m_pUserLayerSetInfo);


	vector<CFeatureData*> vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();

	//vecFeatureData.
	   
	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	
	//nRet = _SetFeature_Resize_LineArc(vecFeatureData, pStep);
	nRet = _SetFeature_Add(vecFeatureData, pStep);

	//m_pDrawEdit_Temp->ClearFeatureData();

	m_pDirectView->SetRedrawGeo();

	//m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();

	bCheckRepeat = FALSE;

	return nRet;
}


UINT32			CDrawManager::_SetFeature_TempData(vector<CFeatureData *>&vecFeatureData, CStep *pStep)
{
	int nDataSize = static_cast<int>(vecFeatureData.size());

	if (nDataSize <= 0) return RESULT_BAD;
	if (pStep == nullptr) return RESULT_BAD;

	for (int nfeno = 0; nfeno < nDataSize; nfeno++)
	{
		CFeatureData *pFeatureData = vecFeatureData[nfeno];

		CLayer *pLayer = pStep->m_arrLayer.GetAt(pFeatureData->m_nLayer);
		if (pLayer == nullptr) continue;

		CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(pFeatureData->m_nFeatureIndex);

		



	}
	return RESULT_GOOD;
}


UINT32			CDrawManager :: _SetFeature_Rotate(vector<CFeatureData *>&vecFeatureData,CStep *pStep)
{
	int nDataSize = static_cast<int>(vecFeatureData.size());

	if (nDataSize <= 0) return RESULT_BAD;
	if (pStep == nullptr) return RESULT_BAD;

	for (int nfeno = 0; nfeno < nDataSize; nfeno++)
	{
		CFeatureData *pFeatureData = vecFeatureData[nfeno];

		CLayer *pLayer = pStep->m_arrLayer.GetAt(pFeatureData->m_nLayer);
		if (pLayer == nullptr) continue;

		CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(pFeatureData->m_nFeatureIndex);

		if (pFeatureData->m_pFeature->m_eType != pFeature->m_eType) continue;

		switch (pFeature->m_eType)
		{
		case FeatureType::P:
		{
			CFeatureP *pFeatureP = (CFeatureP *)pFeature;
			CFeatureP *pFeatureP_Edit = (CFeatureP *)pFeatureData->m_pFeature;

			if (pFeatureP == nullptr) continue;
			if (pFeatureP_Edit == nullptr) continue;

				
			{
				pFeatureP->m_eOrient = pFeatureP_Edit->m_eOrient;
			}

			pFeatureP->m_pSymbol->SetMinMaxRect();
			pFeatureP->m_pSymbol->m_strUserSymbol = CDrawFunction::GetSymbolName(pFeatureP->m_pSymbol->m_eSymbolName,pFeatureP->m_pSymbol->m_vcParams);
			pFeatureP->SetMinMaxRect();
		}
		break;
		}
	}
	return RESULT_GOOD;
}

UINT32			CDrawManager::SetCopy(IN const CPointD &dptPoint)
{
	if (m_pDrawEdit_Temp == nullptr) return RESULT_BAD;
	if (m_pUserLayerSetInfo == nullptr) return RESULT_BAD;

	//m_pDrawEdit_Temp->ClearFeatureData();

	//Get Layer Index;
	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr) return RESULT_BAD;

	//Get Select Layer
	vector<CString> vecLayerName;
	for (int i = 0; i < m_pUserLayerSetInfo->vcLayerSet.size(); i++)
	{
		if (m_pUserLayerSetInfo->vcLayerSet[i]->bCheck == true)
		{
			vecLayerName.push_back(m_pUserLayerSetInfo->vcLayerSet[i]->strLayer);
		}
	}

	int nLayerCount = static_cast<int>(vecLayerName.size());
	for (int i = 0; i < nLayerCount; i++)
	{
		//체크 되어 있는 Layer 이름
		CString strLayer = vecLayerName[i];
		
		//Get Layer Ptr
		int nLayer = -1;
		CLayer *pLayerTmp = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, strLayer, nLayer);
		if (pLayerTmp == nullptr) continue;

		int nSurface_ObObIndex = -1;
		int nAddIndex = 0;
		int nFeatureCount = static_cast<int>(pLayerTmp->m_FeatureFile.m_arrFeature.GetCount());
		for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
		{
			CFeature *pFeature = pLayerTmp->m_FeatureFile.m_arrFeature.GetAt(nfeno);
			if (pFeature == nullptr) continue;

			bool bSelect = FALSE;
			switch (pFeature->m_eType)
			{
			case FeatureType::L: 
			{
				CFeatureL *pFeatureL = (CFeatureL *)pFeature;
				bSelect = pFeatureL->m_bHighlight;
			}
				break;
			case FeatureType::P:
			{
				CFeatureP *pFeatureP = (CFeatureP *)pFeature;
				bSelect = pFeatureP->m_bHighlight;
			}
				break;
			case FeatureType::A:
			{
				CFeatureA *pFeatureA = (CFeatureA *)pFeature;
				bSelect = pFeatureA->m_bHighlight;
			}
				break;
			case FeatureType::T:
			{
				CFeatureT *pFeatureT = (CFeatureT *)pFeature;
				bSelect = pFeatureT->m_bHighlight;
			}
				break;
			case FeatureType::S:
			{
				CFeatureS *pFeatureS = (CFeatureS *)pFeature;
				//ObOe중 하나라도 선택이 되어 있는지 확인
				int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
				
				for (int nObOe = 0; nObOe < nObOeCount; nObOe++)
				{
					CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nObOe);
					if (pObOe == nullptr) continue;

					if (pObOe->m_bHighlight == true)
					{
						nSurface_ObObIndex = nObOe;
						bSelect = TRUE;
						break;
					}
				}
				//
			}
				break;
			}

			if (bSelect == FALSE) continue;

			if (pFeature->m_eType == FeatureType::S)
			{
				if (nSurface_ObObIndex == 0)
				{
					//선택된 Feature만 Data 추가
					m_pDrawEdit_Temp->AddFeatureData(nLayer, nFeatureCount + nAddIndex, pFeature);
					nAddIndex++;
				}
				else
				{
					//선택된 Feature만 Data 추가
					m_pDrawEdit_Temp->AddFeatureData(nLayer, i, pFeature);
				}
			}
			else
			{
				//선택된 Feature만 Data 추가
				m_pDrawEdit_Temp->AddFeatureData(nLayer, nFeatureCount + nAddIndex, pFeature);
				nAddIndex++;
			}

		}
	}

	//Move
	m_pDrawEdit_Temp->SetMoveAxis(Edit_MoveAxis::enumMoveAxis_XY);
	m_pDrawEdit_Temp->_ManualMove_Feature(dptPoint);

	CDirect_Base::Render();

	return RESULT_GOOD;
}

UINT32			CDrawManager::SetCopy_Repeat(IN const CPointD &dptPitch, IN const CPoint &ptCount)
{
	if (m_pDrawEdit_Temp == nullptr) return RESULT_BAD;
	if (m_pUserLayerSetInfo == nullptr) return RESULT_BAD;

	m_pDrawEdit_Temp->ClearFeatureData();

	//Get Layer Index;
	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr) return RESULT_BAD;

	//Get Select Layer
	vector<CString> vecLayerName;
	for (int i = 0; i < m_pUserLayerSetInfo->vcLayerSet.size(); i++)
	{
		if (m_pUserLayerSetInfo->vcLayerSet[i]->bCheck == true)
		{
			vecLayerName.push_back(m_pUserLayerSetInfo->vcLayerSet[i]->strLayer);
		}
	}

	int nLayerCount = static_cast<int>(vecLayerName.size());
	for (int i = 0; i < nLayerCount; i++)
	{
		//체크 되어 있는 Layer 이름
		CString strLayer = vecLayerName[i];

		//Get Layer Ptr
		int nLayer = -1;
		CLayer *pLayerTmp = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, strLayer, nLayer);
		if (pLayerTmp == nullptr) continue;

		int nSurface_ObObIndex = -1;
		int nAddIndex = 0;
		int nFeatureCount = static_cast<int>(pLayerTmp->m_FeatureFile.m_arrFeature.GetCount());
		for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
		{
			CFeature *pFeature = pLayerTmp->m_FeatureFile.m_arrFeature.GetAt(nfeno);
			if (pFeature == nullptr) continue;

			bool bSelect = FALSE;
			switch (pFeature->m_eType)
			{
			case FeatureType::L:
			{
				CFeatureL *pFeatureL = (CFeatureL *)pFeature;
				bSelect = pFeatureL->m_bHighlight;
				if (pFeatureL->m_pGeoObject != nullptr)
				{
					pFeatureL->m_pGeoObject->Release();
					pFeatureL->m_pGeoObject = nullptr;
				}
			}
			break;
			case FeatureType::P:
			{
				CFeatureP *pFeatureP = (CFeatureP *)pFeature;
				bSelect = pFeatureP->m_bHighlight;
				if (pFeatureP->m_pGeoObject != nullptr)
				{
					pFeatureP->m_pGeoObject->Release();
					pFeatureP->m_pGeoObject = nullptr;
				}
			}
			break;
			case FeatureType::A:
			{
				CFeatureA *pFeatureA = (CFeatureA *)pFeature;
				bSelect = pFeatureA->m_bHighlight;
				if (pFeatureA->m_pGeoObject != nullptr)
				{
					pFeatureA->m_pGeoObject->Release();
					pFeatureA->m_pGeoObject = nullptr;
				}
			}
			break;
			case FeatureType::T:
			{
				CFeatureT *pFeatureT = (CFeatureT *)pFeature;
				bSelect = pFeatureT->m_bHighlight;
			}
			break;
			case FeatureType::S:
			{
				CFeatureS *pFeatureS = (CFeatureS *)pFeature;
				//ObOe중 하나라도 선택이 되어 있는지 확인
				int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
				for (int nObOe = 0; nObOe < nObOeCount; nObOe++)
				{
					CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nObOe);
					if (pObOe == nullptr) continue;

					if (pObOe->m_bHighlight == true)
					{
						nSurface_ObObIndex = nObOe;
						bSelect = TRUE;
						if (pFeatureS->m_pGeoObject != nullptr)
						{
							pFeatureS->m_pGeoObject->Release();
							pFeatureS->m_pGeoObject = nullptr;
						}
						break;
					}
				}
				//
			}
			break;
			}

			if (bSelect == FALSE) continue;

			for (int nIndexY = 0; nIndexY < ptCount.y; nIndexY++)
			{
				for (int nIndexX = 0; nIndexX < ptCount.x; nIndexX++)
				{
					if ( nIndexY == 0 && nIndexX == 0 ) continue;

					if (pFeature->m_eType == FeatureType::S)
					{
						if (nSurface_ObObIndex == 0)
						{
							//선택된 Feature만 Data 추가
							m_pDrawEdit_Temp->AddFeatureData(nLayer, nFeatureCount + nAddIndex, pFeature, 0,
								dptPitch.x * nIndexX, dptPitch.y * nIndexY);
							nAddIndex++;
						}
						else
						{
							//선택된 Feature만 Data 추가
							m_pDrawEdit_Temp->AddFeatureData(nLayer, i, pFeature, 0,
								dptPitch.x * nIndexX, dptPitch.y * nIndexY);
						}
					}
					else
					{
						//선택된 Feature만 Data 추가
						m_pDrawEdit_Temp->AddFeatureData(nLayer, nFeatureCount + nAddIndex, pFeature, 0,
							dptPitch.x * nIndexX, dptPitch.y * nIndexY);
						nAddIndex++;
					}
				}
			}

			


		}
	}

	CDirect_Base::Render();

	return RESULT_GOOD;
}


UINT32			CDrawManager::SetCopy_Apply(IN const CPointD &dptPoint)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	SetCopy(dptPoint);

	vector<CFeatureData*> vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	UINT32 nRet;
	nRet = _SetFeature_Add(vecFeatureData, pStep);
	
	m_pDirectView->SetRedrawGeo();

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();

	return RESULT_GOOD;
}

UINT32			CDrawManager::SetCopy_Repeat_Apply(IN const CPointD &dptPitch, IN const CPoint &ptCount)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	SetCopy_Repeat(dptPitch, ptCount);

	vector<CFeatureData*> vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	UINT32 nRet;
	nRet = _SetFeature_Add(vecFeatureData, pStep);

	m_pDirectView->SetRedrawGeo();

	m_pDrawEdit_Temp->ClearFeatureData();

	CDirect_Base::Render();

	

	return RESULT_GOOD;
}

UINT32 CDrawManager::SetEdit_Shape(IN const double &dRadius, IN const double &dWidth, IN const double &dHeight, IN const CString &strShape)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	_SetShape(dRadius, dWidth, dHeight, strShape);

	vector<CFeatureData*> vecFeateureData = m_pDrawEdit_Temp->GetFeatureData();

	CStep* pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = _SetShape_Pad(vecFeateureData, pStep);
	//nRet |= _SetShape_Line(vecFeateureData, pStep);

	m_pDirectView->SetRedrawGeo();

	CDirect_Base::Render();

	return RESULT_GOOD;
}

UINT32 CDrawManager::_SetAttribute(IN const CString &strAttrName, IN const CString &strAttrString)
{
	if (m_pDrawEdit_Temp == nullptr) return RESULT_BAD;

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr) return RESULT_BAD;

	m_pDrawEdit_Temp->SetFeatureData(pStep);

	m_pDrawEdit_Temp->SetAttribute(strAttrName, strAttrString);



	return RESULT_GOOD;
}

UINT32 CDrawManager::SetAttribute(IN const CString &AttributeName, IN const CString &AttributeString)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	_SetAttribute(AttributeName, AttributeString);

	vector<CFeatureData*> vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();

	CStep* pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr) return RESULT_BAD;

	UINT32 nRet;
	nRet = _SetAttribute_Apply(vecFeatureData, pStep,AttributeName,AttributeString);

	m_pDirectView->SetRedrawGeo();

	CDirect_Base::Render();

	return RESULT_GOOD;

}

UINT32 CDrawManager::_SetAttribute_Apply(vector<CFeatureData*>&vecFeatureData, CStep *pStep, IN const CString&strAttName, IN const CString &strAttString)
{
	int nDataSize = static_cast<int>(vecFeatureData.size());

	if (nDataSize <= 0) return RESULT_BAD;
	if (pStep == nullptr) return RESULT_BAD;

	for (int nfeno = 0; nfeno < nDataSize; nfeno++)
	{
		CFeatureData *pFeatureData = vecFeatureData[nfeno];

		CLayer *pLayer = pStep->m_arrLayer.GetAt(pFeatureData->m_nLayer);
		if (pLayer == nullptr) continue;

		CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(pFeatureData->m_nFeatureIndex);

		if (pFeatureData->m_pFeature->m_eType != pFeature->m_eType) continue;

		switch (pFeature->m_eType)
		{
		case FeatureType::L:
		{	
			CFeatureL *pFeatureL = (CFeatureL *)pFeature;
			CFeatureL *pFeatureL_Edit = (CFeatureL *)pFeatureData->m_pFeature;

			CAttribute* pAttribute = new CAttribute();
			CAttributeTextString *pAttributeString = new CAttributeTextString;

			pAttribute->m_strName = strAttName;
			pAttributeString->m_strText = strAttString;

			pFeatureL->m_arrAttribute.Add(pAttribute);
			pFeatureL->m_arrAttributeTextString.Add(pAttributeString);
	
			/*int last = static_cast<int>(pFeatureL->m_arrAttribute.GetCount() - 1);*/
			
			
			//FeatureSpec.strAttr = pFeatureL_Edit->m_arrAttribute.GetAt(last)->m_strName;
			//FeatureSpec.strAttrString = pFeatureL_Edit->m_arrAttributeTextString.GetAt(last)->m_strText;
			
		}
		break;
		case FeatureType::P:
		{


			CFeatureP *pFeatureP = (CFeatureP *)pFeature;
			CFeatureP *pFeatureP_Edit = (CFeatureP *)pFeatureData->m_pFeature;

			CAttribute* pAttribute = new CAttribute();
			CAttributeTextString *pAttributeString = new CAttributeTextString;

			pAttribute->m_strName = strAttName;
			pAttributeString->m_strText = strAttString;

			pFeatureP->m_arrAttribute.Add(pAttribute);
			pFeatureP->m_arrAttributeTextString.Add(pAttributeString);

			/*int last = pFeatureP->m_arrAttribute.GetCount() - 1;*/

// 			FeatureSpec.strAttr = pFeatureP_Edit->m_arrAttribute.GetAt(last)->m_strName;
// 			FeatureSpec.strAttrString = pFeatureP_Edit->m_arrAttributeTextString.GetAt(last)->m_strText;

			
		}
		break;
		case FeatureType::S:
		{
			CFeatureS *pFeatureS = (CFeatureS*)pFeature;
			CFeatureS *pFeatureS_Edit = (CFeatureS*)pFeatureData->m_pFeature;

			CAttribute* pAttribute = new CAttribute();
			CAttributeTextString *pAttributeString = new CAttributeTextString;

			pAttribute->m_strName = strAttName;
			pAttributeString->m_strText = strAttString;

			pFeatureS->m_arrAttribute.Add(pAttribute);
			pFeatureS->m_arrAttributeTextString.Add(pAttributeString);

			/*int last = pFeatureS->m_arrAttribute.GetCount() - 1;*/

// 			FeatureSpec.strAttr = pFeatureS_Edit->m_arrAttribute.GetAt(last)->m_strName;
// 			FeatureSpec.strAttrString = pFeatureS_Edit->m_arrAttributeTextString.GetAt(last)->m_strText;

		}
		break;
		}
	}
	return RESULT_GOOD;
}





UINT32 CDrawManager::_SetShape_Pad(vector<CFeatureData*>& vecFeatureData, CStep *pStep)
{
	int nDataSize = static_cast<int>(vecFeatureData.size());

	if (nDataSize <= 0) return RESULT_BAD;
	if (pStep == nullptr) return RESULT_BAD;

	for (int nfeno = 0; nfeno < nDataSize; nfeno++)
	{
		CFeatureData *pFeatureData = vecFeatureData[nfeno];

		CLayer *pLayer = pStep->m_arrLayer.GetAt(pFeatureData->m_nLayer);
		if (pLayer == nullptr) continue;

		CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(pFeatureData->m_nFeatureIndex);

		if (pFeatureData->m_pFeature->m_eType != pFeature->m_eType) continue;

		switch (pFeature->m_eType)
		{
		case FeatureType::L:
		{
			CFeatureL *pFeatureL = (CFeatureL *)pFeature;
			CFeatureL *pFeatureL_Edit = (CFeatureL *)pFeatureData->m_pFeature;

			if (pFeatureL == nullptr) continue;
			if (pFeatureL->m_pSymbol == nullptr) continue;
			if (pFeatureL->m_pSymbol->m_vcParams.size() <= 0) continue;

			if (pFeatureL_Edit == nullptr) continue;
			if (pFeatureL_Edit->m_pSymbol == nullptr) continue;
			if (pFeatureL_Edit->m_pSymbol->m_vcParams.size() <= 0) continue;

			if (pFeatureL->m_pSymbol->m_vcParams.size() != pFeatureL_Edit->m_pSymbol->m_vcParams.size()) continue;

			pFeatureL->m_pSymbol->m_vcParams[0] = pFeatureL_Edit->m_pSymbol->m_vcParams[0];

			if (pFeatureL->m_pSymbol->m_eSymbolName != pFeatureL_Edit->m_pSymbol->m_eSymbolName)
			{//신규로 Symbol 생성

				CSymbol *pSymbol_New = new CSymbol();
				*pSymbol_New = *pFeatureL->m_pSymbol;

				pSymbol_New->m_vcParams.resize(pFeatureL_Edit->m_pSymbol->m_vcParams.size());

				//SetParam
				pSymbol_New->m_eSymbolName = pFeatureL_Edit->m_pSymbol->m_eSymbolName;
				
				pSymbol_New->SetMinMaxRect();

				CString strSymbolName = CDrawFunction::GetSymbolName(pSymbol_New->m_eSymbolName,
					pSymbol_New->m_vcParams);
				pSymbol_New->m_strUserSymbol = strSymbolName;

				//같은 변경하는 Symbol이 같은게 있는지 체크
				//
				BOOL bCompare = FALSE;
				int nSymbolCount = static_cast<int>(pLayer->m_FeatureFile.m_arrSymbol.GetCount());
				CSymbol *pSymbol = nullptr;
				for (int i = 0; i < nSymbolCount; i++)
				{
					CSymbol *pSymbolTemp = pLayer->m_FeatureFile.m_arrSymbol.GetAt(i);

					if (*pSymbolTemp == *pSymbol_New)
					{
						pSymbol = pSymbolTemp;
						bCompare = TRUE;
						break;
					}
				}

				//같은게 있으면, 신규로 생성한것을 지우고,
				//같은 심볼의 포인터를 연결해준다.
				if (bCompare == TRUE)
				{
					delete pSymbol_New;
					pSymbol_New = nullptr;

					pFeatureL->m_pSymbol = pSymbol;

				}
				else
				{

					int nMaxIndex = static_cast<int>(pLayer->m_FeatureFile.m_arrSymbol.GetCount());

					//Symbol num : 새로 Add 할것이기 때문에 MaxIndex를 넣어준다.
					pSymbol_New->m_iSerialNum = nMaxIndex;

					//신규 Symbol 포인터 연결
					pFeatureL->m_pSymbol = pSymbol_New;

					pLayer->m_FeatureFile.m_arrSymbol.Add(pSymbol_New);
				}

			}

			pFeatureL->SetMinMaxRect();
		}
	
		break;
		case FeatureType::P:
		{
			CFeatureP *pFeatureP = (CFeatureP *)pFeature;
			CFeatureP *pFeatureP_Edit = (CFeatureP *)pFeatureData->m_pFeature;

			if (pFeatureP == nullptr) continue;
			if (pFeatureP->m_pSymbol == nullptr) continue;
			if (pFeatureP->m_pSymbol->m_vcParams.size() <= 0) continue;

			if (pFeatureP_Edit == nullptr) continue;
			if (pFeatureP_Edit->m_pSymbol == nullptr) continue;
			if (pFeatureP_Edit->m_pSymbol->m_vcParams.size() <= 0) continue;

			int nParamCount = static_cast<int>(pFeatureP->m_pSymbol->m_vcParams.size());
			int nParamCount_Edit = static_cast<int>(pFeatureP_Edit->m_pSymbol->m_vcParams.size());

			if (pFeatureP->m_pSymbol->m_vcParams.size() != pFeatureP_Edit->m_pSymbol->m_vcParams.size()) continue;

			if (pFeatureP->m_pSymbol->m_vcParams[0] != pFeatureP_Edit->m_pSymbol->m_vcParams[0]) //|| pFeatureP->m_pSymbol->m_vcParams[1] != pFeatureP_Edit->m_pSymbol->m_vcParams[1])
			{//신규로 Symbol 생성

				CSymbol *pSymbol_New = new CSymbol();
				*pSymbol_New = *pFeatureP->m_pSymbol;

				pSymbol_New->m_vcParams.resize(pFeatureP_Edit->m_pSymbol->m_vcParams.size());

				//SetParam
				switch (pSymbol_New->m_eSymbolName)
				{
				case SymbolName::r:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					break;

				case SymbolName::rect:
					pSymbol_New->m_vcParams[0] = pFeatureP_Edit->m_pSymbol->m_vcParams[0];
					pSymbol_New->m_vcParams[1] = pFeatureP_Edit->m_pSymbol->m_vcParams[1];
					break;
				}

				pSymbol_New->SetMinMaxRect();

				CString strSymbolName = CDrawFunction::GetSymbolName(pSymbol_New->m_eSymbolName,
					pSymbol_New->m_vcParams);
				pSymbol_New->m_strUserSymbol = strSymbolName;

				//같은 변경하는 Symbol이 같은게 있는지 체크
				//
				BOOL bCompare = FALSE;
				int nSymbolCount = static_cast<int>(pLayer->m_FeatureFile.m_arrSymbol.GetCount());
				CSymbol *pSymbol = nullptr;
				for (int i = 0; i < nSymbolCount; i++)
				{
					CSymbol *pSymbolTemp = pLayer->m_FeatureFile.m_arrSymbol.GetAt(i);

					if (*pSymbolTemp == *pSymbol_New)
					{
						pSymbol = pSymbolTemp;
						bCompare = TRUE;
						break;
					}
				}

				//같은게 있으면, 신규로 생성한것을 지우고,
				//같은 심볼의 포인터를 연결해준다.
				if (bCompare == TRUE)
				{
					delete pSymbol_New;
					pSymbol_New = nullptr;

					pFeatureP->m_pSymbol = pSymbol;

				}
				else
				{

					int nMaxIndex = static_cast<int>(pLayer->m_FeatureFile.m_arrSymbol.GetCount());

					//Symbol num : 새로 Add 할것이기 때문에 MaxIndex를 넣어준다.
					pSymbol_New->m_iSerialNum = nMaxIndex;

					//신규 Symbol 포인터 연결
					pFeatureP->m_pSymbol = pSymbol_New;

					pLayer->m_FeatureFile.m_arrSymbol.Add(pSymbol_New);
				}
			}

			pFeatureP->SetMinMaxRect();

			}
			}
			



		}
		return RESULT_GOOD;
	}
	

UINT32 CDrawManager::EditApply()
{
	vector<CFeatureData*> vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	UINT32 nRet;
	nRet = _SetFeature_Resize_LineArc_SelectOnly(vecFeatureData, pStep);
	nRet |= _SetFeature_Resize_PadText_SelectOnly(vecFeatureData, pStep);
	nRet |= _SetFeature_Resize_Surface(vecFeatureData, pStep);

	m_pDirectView->SetRedrawGeo();

	CDirect_Base::Render();

	m_bResizeMode = FALSE;

	return RESULT_GOOD;
}


UINT32			CDrawManager::SetResize_Apply_SelectOnly(IN const double &dResizePercent)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	//if (m_bResizeMode == FALSE)
	{
		SetResize(dResizePercent);
	}
	
	vector<CFeatureData*> vecFeatureData = m_pDrawEdit_Temp->GetFeatureData();

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr)return RESULT_BAD;

	UINT32 nRet;
	nRet = _SetFeature_Resize_LineArc_SelectOnly(vecFeatureData, pStep);
	nRet |= _SetFeature_Resize_PadText_SelectOnly(vecFeatureData, pStep);
	nRet |= _SetFeature_Resize_Surface(vecFeatureData, pStep);

	m_pDirectView->SetRedrawGeo();

	CDirect_Base::Render();

	return RESULT_GOOD;
}

BOOL CDrawManager::CompareFeature(IN CLayer *pLayer, IN const int &nFeatureIndex_1, IN const int &nFeatureIndex_2)
{
	if (m_pFeatureFilter == nullptr) return FALSE;

	BOOL bCompare = FALSE;
	bCompare = m_pFeatureFilter->CompareFeature(pLayer, nFeatureIndex_1, nFeatureIndex_2);

	return bCompare;
}

RECTD			CDrawManager::GetViewRect()
{
	if (m_vecRender.size() <= 0) return RECTD();

	ID2D1RenderTarget* pRender = m_vecRender[0];

	D2D1::Matrix3x2F matTransform;
	pRender->GetTransform(&matTransform);
	
	RECTD drtDraw_mm;

	D2D1_POINT_2F fptPan = D2D1::Point2F(matTransform.dx, matTransform.dy);
	D2D1_POINT_2F fptSize = D2D1::Point2F(pRender->GetSize().width, pRender->GetSize().height);

	// 2022.11.01 KJH ADD
	// Back Layer Align View Center 일 경우 좌표 계산 위해 변경
	//D2D1_POINT_2F fptPan_mm = CDrawFunction::DetansformPoint(D2D1::Point2F(0.f, 0.f), matTransform, nullptr);
	D2D1_POINT_2F fptPan_mm = CDrawFunction::DetansformPoint(D2D1::Point2F(0.f, 0.f), matTransform, m_pUserSetInfo);

	D2D1::Matrix3x2F matScale = D2D1::Matrix3x2F(matTransform.m11, matTransform.m12, matTransform.m21, matTransform.m22, 0, 0);

	// 2022.11.01 KJH ADD
	// Back Layer Align View Center 일 경우 좌표 계산 위해 변경
	//D2D1_POINT_2F fptSize_mm = CDrawFunction::DetansformPoint(fptSize, matScale, nullptr);
	D2D1_POINT_2F fptSize_mm = CDrawFunction::DetansformPoint(fptSize, matScale, m_pUserSetInfo);

	drtDraw_mm = RECTD(fptPan_mm.x, fptPan_mm.y, fptPan_mm.x + fptSize_mm.x, fptPan_mm.y + fptSize_mm.y);
	drtDraw_mm.NormalizeRectD();

	return drtDraw_mm;
}

UINT32			CDrawManager::SetPictureBox(IN const D2D1_RECT_F & frtPictureBox)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;

	UINT32 nRet;
	m_frtPictureBox = frtPictureBox;
	nRet = m_pDirectView->SetPictureBox(frtPictureBox);

	CDirect_Base::Render();

	return nRet;
}

CString			CDrawManager::GetSelectInfoString()
{
	if (m_pDirectView == nullptr) return _T("");

	return m_pDirectView->GetSelectInfoString();
}

D2D1::Matrix3x2F	CDrawManager::GetViewScale()
{
	return m_ViewScale;
}

UINT32			CDrawManager::SetEditMoveAxis(IN const int &nMoveAxis)
{
	return m_pDrawEdit_Temp->SetMoveAxis(nMoveAxis);
}

UINT32			CDrawManager::SetMaskSelectMode(IN const MaskType &eSelectMode)
{
// 	if (m_nEditMode == EditMode::enumMode_EditMask_Move &&
// 		eSelectMode == MaskType::enumType_None)
// 	{//모브 모드이고, 선택이 끝났다면
// 		if (m_pDrawEdit_Temp == nullptr) return RESULT_BAD;
// 
// 		CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
// 		if (pStep == nullptr) return RESULT_BAD;
// 
// 		m_pDrawEdit_Temp->SetMaskData(m_eMaskSelectMode, pStep);
// 
// 		m_pDrawEdit_Temp->SetMoveAxis(Edit_MoveAxis::enumMoveAxis_XY);
// 	}
// 	else
// 	{
// 		m_pDrawEdit_Temp->ClearFeatureData();
// 		m_pDrawEdit_Temp->SetMoveAxis(Edit_MoveAxis::enumMoveAxis_None);
// 	}

	m_eMaskSelectMode = eSelectMode;

	CDirect_Base::Render();

	return RESULT_GOOD;
}

MaskType		CDrawManager::GetMaskSelectMode()
{
	return m_eMaskSelectMode;
}

void			CDrawManager::OnRender()
{
	if (m_pDirectView == nullptr) return ;
	if (m_pUserSetInfo == nullptr) return;
	if (m_nSelectStep == -1) return;

	BOOL bEditDraw = FALSE;
	int nDrawCount = m_nMaxLayer * 2;

	SetViewScale();

	for (int i = 0; i < m_nMaxLayer; i++)
	{
		m_vecRender[i]->SetTransform(m_ViewScale);
		m_vecRender_Substep[i]->SetTransform(m_ViewScale);
	}

	m_pDirectView->SetStepIndex(m_nSelectStep);
	m_pDirectView->OnRender();

	//
	if (m_pUserSetInfo->bShowProfile && m_pDrawProfile != nullptr)
	{//Draw Profile

		if (m_nSelectStep == -1) return;
		if (m_vecRender.size() <= 0) return;

		m_pDrawProfile->OnRender(m_pJobData, m_nSelectStep, m_vecRender[0]);
	}

	if (m_bRectZoom == TRUE)
	{
		m_pDrawProfile->DrawRect(m_vecRender[0], m_rtRectZoom);
	}

	if (m_bMeasureMode == TRUE)
	{
		BOOL bDispInch = FALSE;
		if (m_pUserSetInfo->coordUnit == eCoordinateUnit::eMMToInch) bDispInch = TRUE;
		
		m_pDrawProfile->DrawMeasure(m_vecRender[0], m_frtMeasure, bDispInch);
	}

	if (m_pDrawEdit_Temp != nullptr &&
		(m_nEditMode >= EditMode::enumMode_EditMode && m_nEditMode < EditMode::enumMode_Max))
		//m_nEditMode >= EditMode::enumMode_EditDelete && m_nEditMode < EditMode::enumMode_Max
	{
		m_pDrawEdit_Temp->SetTransform(m_ViewScale);
		m_pDrawEdit_Temp->OnRender(&m_pJobData->m_arrFont);

		bEditDraw = TRUE;
		nDrawCount++;
	}		 

	if (&m_pDrawFov != nullptr)
	{
		if (m_nSelectStep == -1) return;
		if (m_vecRender.size() <= 0) return;

		if (m_pUserSetInfo->bShowFOV)
			m_pDrawFov->OnRender(m_nSelectStep, m_vecRender[0], true);
		else if (m_pUserSetInfo->bShowCell)	
			m_pDrawFov->OnRender(m_nSelectStep, m_vecRender[0], false);
	}
	
	//
	int nCount = 0;
	m_pCompositeEffect->SetInputCount(nDrawCount);

	for (USHORT idx = 0; idx < m_nMaxLayer; idx++)
	{
		ID2D1Bitmap* pBitmap = nullptr;
		m_vecRender[idx]->GetBitmap(&pBitmap);

		if (pBitmap != nullptr)
		{
			m_pCompositeEffect->SetInput(nCount, pBitmap);
			nCount++;
		}
		pBitmap->Release();
		pBitmap = nullptr;
	}

	for (USHORT idx = 0; idx < m_nMaxLayer; idx++)
	{
		ID2D1Bitmap* pBitmap = nullptr;
		m_vecRender_Substep[idx]->GetBitmap(&pBitmap);

		if (pBitmap != nullptr)
		{
			m_pCompositeEffect->SetInput(nCount, pBitmap);
			nCount++;
		}
		pBitmap->Release();
		pBitmap = nullptr;
	}

	//
	if (bEditDraw == TRUE)
	{
		ID2D1Bitmap* pBitmap;
		m_pDrawEdit_Temp->GetRender()->GetBitmap(&pBitmap);

		if (pBitmap != nullptr)
		{
			m_pCompositeEffect->SetInput(nCount, pBitmap);
			nCount++;
		}
		pBitmap->Release();
		pBitmap = nullptr;
	}

	//m_pD2DContext->

	
	//DrawOut();
}

UINT32 CDrawManager::SaveUnitLayer(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dAngle, IN const bool bMirror, 
	IN const double &dResolution, IN const CString &strPathName)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;
	if (strPathName.GetLength() < 1) return RESULT_BAD;
	if (iStepIdx < 0 || dResolution <= 0.f) return RESULT_BAD;

	return m_pDirectView->SaveUnitLayer(iStepIdx, iIndex, dAngle, bMirror, dResolution, strPathName);
}

UINT32 CDrawManager::SaveStripLayer(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dAngle, IN const bool bMirror, IN const double &dResolution, IN const CString &strPathName)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;
	if (strPathName.GetLength() < 1) return RESULT_BAD;
	if (iStepIdx < 0 || dResolution <= 0.f) return RESULT_BAD;

	return m_pDirectView->SaveStripLayer(iStepIdx, iIndex, dAngle, bMirror, dResolution, strPathName);	
}

UINT32 CDrawManager::SaveUnitAlignPoint(IN const INT32 iStepIdx, IN const UINT32 iUnitIndex, IN const UINT32 iIndex, IN const double dAngle, IN const bool bMirror,
	IN const double dCamAngle, IN const double &dResolution, IN const RECTD &rcRect, IN const CString &strPathName)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;
	if (strPathName.GetLength() < 1) return RESULT_BAD;
	if (iStepIdx < 0 || dResolution <= 0.f) return RESULT_BAD;	

	return m_pDirectView->SaveUnitAlignPoint(iStepIdx, iUnitIndex, iIndex, dAngle, bMirror, dCamAngle, dResolution, rcRect, strPathName);
}

//YJD START 22.06.13 - SAVE MEASURE IMAGE
UINT32 CDrawManager::SaveMeasurePoint(IN const INT32 iStepIdx, IN const UINT32 iUnitIndex, IN const double dAngle, IN const bool bMirror,
	IN const double dCamAngle, IN const vector<double> &vecResolution, IN const vector<RECTD> &vecRect, IN const CString &strPathName)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;
	if (strPathName.GetLength() < 1) return RESULT_BAD;
	if (iStepIdx < 0) return RESULT_BAD;

	return m_pDirectView->SaveMeasurePoint(iStepIdx, iUnitIndex, dAngle, bMirror, dCamAngle, vecResolution, vecRect, strPathName);
}
//YJD END 22.06.13 - SAVE MEASURE IMAGE

UINT32 CDrawManager::SavePanelLayer(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle, IN const double &dResolution, IN const CString &strPathName)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;
	if (strPathName.GetLength() < 1) return RESULT_BAD;
	if (iStepIdx < 0 || dResolution <= 0.f) return RESULT_BAD;

	return m_pDirectView->SavePanelLayer(iStepIdx, iIndex, dCamAngle, dResolution, strPathName);
}

UINT32 CDrawManager::SavePanelLayer_FeatureSub(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle, IN const double &dResolution, IN const CString &strPathName, int eLineOption, vector <LINEWIDTH> &vecLineWidth)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;
	if (strPathName.GetLength() < 1) return RESULT_BAD;
	if (iStepIdx < 0 || dResolution <= 0.f) return RESULT_BAD;

	return m_pDirectView->SavePanelLayer_FeatureSub(iStepIdx, iIndex, dCamAngle, dResolution, strPathName, eLineOption, vecLineWidth);
}

UINT32 CDrawManager::SavePanelLayer_FeatureSub_V3(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double &dResolution, IN const CString &strPathName, vector<vector <LINEWIDTH>> &vecvecLineWidth)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;
	if (strPathName.GetLength() < 1) return RESULT_BAD;
	if (iStepIdx < 0 || dResolution <= 0.f) return RESULT_BAD;

	return m_pDirectView->SavePanelLayer_FeatureSub_V3(iStepIdx, iIndex, dResolution, strPathName, vecvecLineWidth);
}

UINT32 CDrawManager::SavePanelAlignPoint(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const double dCamAngle, IN const double &dResolution, 
	IN const RECTD &rcRect, IN const CString &strPathName)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;
	if (strPathName.GetLength() < 1) return RESULT_BAD;
	if (iStepIdx < 0 || dResolution <= 0.f) return RESULT_BAD;
	if (m_pUserSetInfo == nullptr) return RESULT_BAD;	

	return m_pDirectView->SavePanelAlignPoint(iStepIdx, iIndex, dCamAngle, dResolution, rcRect, m_pUserSetInfo->strWorkLayer, strPathName);
}

UINT32 CDrawManager::SaveUnitAlignPoint_In_Panel(IN const INT32 iStepIdx, IN const Orient &eOrient, IN const double &dResolution, IN const CString &strPathName)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;
	if (strPathName.GetLength() < 1) return RESULT_BAD;
	if (iStepIdx < 0 || dResolution <= 0.f) return RESULT_BAD;
	if (m_pUserSetInfo == nullptr) return RESULT_BAD;

	return m_pDirectView->SaveUnitAlignPoint_In_Panel(iStepIdx, eOrient, dResolution, m_pUserSetInfo->strWorkLayer, strPathName);

}

UINT32 CDrawManager::SaveProfile(IN const BOOL m_bIs4Step, IN const INT32 iStepIdx, IN const Orient &eOrient, IN const CString &strPathName)
{
	if (m_pDirectView == nullptr) return RESULT_BAD;
	if (strPathName.GetLength() < 1) return RESULT_BAD;

	return m_pDirectView->SaveProfile(m_bIs4Step, iStepIdx, eOrient, strPathName);
}

UINT32 CDrawManager::SaveUnitLayer_in_Panel(IN const INT32 iStepIdx, IN const UINT32 iIndex, IN const Orient &eOrient, IN const double &dResolution, IN const CString &strPathName)\
{
	if (m_pDirectView == nullptr) return RESULT_BAD;
	if (strPathName.GetLength() < 1) return RESULT_BAD;
	if (iStepIdx < 0 || dResolution <= 0.f) return RESULT_BAD;

	return m_pDirectView->SaveUnitLayer_in_Panel(iStepIdx, iIndex, eOrient, dResolution, strPathName);
}

UINT32 CDrawManager::ManualMove(IN const PointDXY &point)
{
	if (m_pDrawEdit_Temp == nullptr) return RESULT_BAD;

	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr) return RESULT_BAD;

	m_pDrawEdit_Temp->SetFeatureData(pStep);	

	UINT32 nRet = m_pDrawEdit_Temp->_ManualMove_Feature(point);

	if (m_nEditMode != EditMode::enumMode_None)
	{
		CDirect_Base::Render();
	}

	return nRet;
}

UINT32 CDrawManager::Draw_ManualMove(UINT nFlags, PointDXY &point)
{
	HCURSOR hHandCursor = ::LoadCursor(NULL, IDC_ARROW);
	SetCursor(hHandCursor);

	m_bPanMode = FALSE;
	CPoint pt;

	m_pDirectView->OnLButtonUp(nFlags, pt);
	m_pDrawEdit_Temp->OnLButtonUp(nFlags, pt);

	if (m_bPointSetMode == TRUE)
	{//Set Temp Point
	
		m_pDrawEdit_Temp->SetTempPoint(m_nPointIndex, m_strPointSet_Name, 
			static_cast<float>(point.x), static_cast<float>(point.y));
	}
	else if (GetEditMode() == EditMode::enumMode_EditMask)
	{
		if (m_pDrawEdit_Temp == nullptr) return RESULT_BAD;

		CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
		if (pStep == nullptr) return RESULT_BAD;

		m_pDrawEdit_Temp->SetMaskData(m_eMaskSelectMode, pStep);
	}

	CDirect_Base::Render();

	return RESULT_GOOD;
}

SELECT_INFO*			CDrawManager::GetSelectInfo()
{
	if (m_pDirectView == nullptr) return nullptr;

	return m_pDirectView->GetSelectInfo();
}

SELECT_INFO*			CDrawManager::SetSelectInfo(D2D_POINT_2F fptPoint_mm, D2D_POINT_2F fptPoint_Pixel)
{
	if (m_pDirectView == nullptr) return nullptr;

	return m_pDirectView->SetSelectInfo(fptPoint_mm, fptPoint_Pixel);
}


UINT32 CDrawManager::SetPreview(IN const PointDXY &point, double &fResizePercent, IN const int&nIndex, IN const BOOL&bMirror , IN const CPointD &point2, 
	IN const CPointD &dptPitch, IN const CPoint &ptCount, IN  BOOL &bRepeat, IN  BOOL &bCopy, IN const double &dRadius, IN const double &dWidth, IN const double &dHeight, IN const CString &strShape, IN const double &dLineWidth)
{

	if (m_pDrawEdit_Temp == nullptr) return RESULT_BAD;
	if (m_pUserLayerSetInfo == nullptr) return RESULT_BAD;

	m_pDrawEdit_Temp->ClearFeatureData();

	//Get Layer Index;
	CStep *pStep = m_pJobData->m_arrStep.GetAt(m_nSelectStep);
	if (pStep == nullptr) return RESULT_BAD;

	//Get Select Layer
	vector<CString> vecLayerName;
	for (int i = 0; i < m_pUserLayerSetInfo->vcLayerSet.size(); i++)
	{

		if (m_pUserSetInfo->mcType == eMachineType::eNSIS) 
		{
			if (m_pUserLayerSetInfo->vcLayerSet[i]->strDefine.CompareNoCase(MEASURE_LAYER_TXT) != 0)
				continue;
		}
		else 
		{
			if (m_pUserLayerSetInfo->vcLayerSet[i]->strDefine.CompareNoCase(PATTERN_LAYER_TXT) != 0)
			continue;
		}
		
		if (m_pUserLayerSetInfo->vcLayerSet[i]->bCheck == true)
		{
			vecLayerName.push_back(m_pUserLayerSetInfo->vcLayerSet[i]->strLayer);
		}
	}

	int nLayerCount = static_cast<int>(vecLayerName.size());
	
	for (int i = 0; i < nLayerCount; i++)
	{
		//체크 되어 있는 Layer 이름
		CString strLayer = vecLayerName[i];

		//Get Layer Ptr
		int nLayer = -1;
		CLayer *pLayerTmp = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, strLayer, nLayer);
		if (pLayerTmp == nullptr) continue;

		int nSurface_ObObIndex = -1;
		int nAddIndex = 0;
		int nFeatureCount = static_cast<int>(pLayerTmp->m_FeatureFile.m_arrFeature.GetCount());
		for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
		{
			CFeature *pFeature = pLayerTmp->m_FeatureFile.m_arrFeature.GetAt(nfeno);
			if (pFeature == nullptr) continue;

			bool bSelect = FALSE;
			switch (pFeature->m_eType)
			{
			case FeatureType::Z:
			{
				CFeatureZ *pFeatureZ = (CFeatureZ *)pFeature;				
				bSelect = pFeatureZ->m_bHighlight;
			}
			break;
			case FeatureType::L:
			{
				CFeatureL *pFeatureL = (CFeatureL *)pFeature;				
				bSelect = pFeatureL->m_bHighlight;
			}
			break;
			case FeatureType::P:
			{
				CFeatureP *pFeatureP = (CFeatureP *)pFeature;				
				bSelect = pFeatureP->m_bHighlight;
			}
			break;
			case FeatureType::A:
			{
				CFeatureA *pFeatureA = (CFeatureA *)pFeature;				
				bSelect = pFeatureA->m_bHighlight;
			}
			break;
			case FeatureType::T:
			{
				CFeatureT *pFeatureT = (CFeatureT *)pFeature;				
				bSelect = pFeatureT->m_bHighlight;
			}
			break;
			case FeatureType::S:
			{
				CFeatureS *pFeatureS = (CFeatureS *)pFeature;
				//ObOe중 하나라도 선택이 되어 있는지 확인
				int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());

				for (int nObOe = 0; nObOe < nObOeCount; nObOe++)
				{
					CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nObOe);
					if (pObOe == nullptr) continue;

					if (pObOe->m_bHighlight == true)
					{
						nSurface_ObObIndex = nObOe;		
						bSelect = TRUE;
						break;
					}
				}
				//
			}
			break;
			}

			if (bSelect != TRUE) 
				continue;

			if (bRepeat == TRUE && bCopy ==TRUE)
			{	
				for (int nIndexY = 0; nIndexY < ptCount.y; nIndexY++)
				{
					for (int nIndexX = 0; nIndexX < ptCount.x; nIndexX++)
					{
						//if (nIndexY == 0 && nIndexX == 0) continue;

						if (nIndexX ==0 &&nIndexY ==0) continue; 

						if (pFeature->m_eType == FeatureType::S)
						{
							if (nSurface_ObObIndex == 0)
							{
								//선택된 Feature만 Data 추가
								m_pDrawEdit_Temp->AddFeatureData(nLayer, nFeatureCount + nAddIndex, pFeature, 0,
									dptPitch.x * nIndexX, dptPitch.y * nIndexY);
								nAddIndex++;
							}
							else
							{
								//선택된 Feature만 Data 추가
								m_pDrawEdit_Temp->AddFeatureData(nLayer, i, pFeature, 0,
									dptPitch.x * nIndexX, dptPitch.y * nIndexY);
							}
						}
						else
						{
							//선택된 Feature만 Data 추가
							m_pDrawEdit_Temp->AddFeatureData(nLayer, nFeatureCount + nAddIndex, pFeature, 0,
								dptPitch.x * nIndexX, dptPitch.y * nIndexY);
							nAddIndex++;
						}
					}
				}
			}

			else if (bCopy == TRUE && bRepeat != TRUE)
			{	
				if (pFeature->m_eType == FeatureType::S)
				{
					if (nSurface_ObObIndex == 0)
					{
						//선택된 Feature만 Data 추가
						m_pDrawEdit_Temp->AddFeatureData(nLayer, nFeatureCount + nAddIndex, pFeature);
						nAddIndex++;
					}
					else
					{
						//선택된 Feature만 Data 추가
						m_pDrawEdit_Temp->AddFeatureData(nLayer, i, pFeature);
					}
				}
				else
				{
					//선택된 Feature만 Data 추가
					m_pDrawEdit_Temp->AddFeatureData(nLayer, nFeatureCount + nAddIndex, pFeature);
					nAddIndex++;
				}
			}
			
			else
			{
				m_pDrawEdit_Temp->SetFeatureData(pStep);
			}
		}
	}

	bCheckRepeat = bCopy;

	

	if (bRepeat == TRUE || bCopy == TRUE)
	{
		m_pDrawEdit_Temp->_ManualMove_Feature(point2);
	}
	else 
	{
		m_pDrawEdit_Temp->_ManualMove_Feature(point);
	}

	m_pDrawEdit_Temp->SetPadRotate(nIndex, bMirror);

	m_pDrawEdit_Temp->SetLineArcResize(fResizePercent);
	m_pDrawEdit_Temp->SetPadTextResize(fResizePercent);
	m_pDrawEdit_Temp->SetSurfaceResize(fResizePercent);

	

// 	m_pDrawEdit_Temp->SetLineShape(strShape,dLineWidth);
// 	m_pDrawEdit_Temp->SetPadShape(dRadius, dWidth, dHeight);	
	//m_pDrawEdit_Temp->SetMoveAxis(Edit_MoveAxis::enumMoveAxis_XY);
	//m_pDrawEdit_Temp->_ManualMove_Feature(point2);
	
	
	CDirect_Base::Render();
 	//Reset();
// 	ResetSelect();
	//FeatureSpec.bPreview = TRUE;
	bRepeat = FALSE;
	bCopy = FALSE;
	return RESULT_GOOD;
}

