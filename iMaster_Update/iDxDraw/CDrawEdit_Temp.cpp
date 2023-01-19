#include "pch.h"
#include "CDrawEdit_Temp.h"

#include "CDrawManager.h"
#include "CDrawFunction.h"


CDrawEdit_Temp::CDrawEdit_Temp()
{
	m_pDrawFunction = new CDrawFunction();

	ClearTempPoint();
}

CDrawEdit_Temp::~CDrawEdit_Temp()
{
	ClearFeatureData();
	ClearTempPoint();

	delete m_pDrawFunction;
}




UINT32	CDrawEdit_Temp::Set_CW(IN const int&nIndex)
{
	int nSize = static_cast<int>(m_vecTempPoint.size());
	if (nIndex < 0) return RESULT_BAD;
	if (nIndex >= nSize) return RESULT_BAD;

	if (m_vecTempPoint[nIndex].bCw == true)
	{
		m_vecTempPoint[nIndex].bCw = false;
	}
	else
	{
		m_vecTempPoint[nIndex].bCw = true;
	}

	return RESULT_GOOD;
}

void CDrawEdit_Temp::SetLink(IN ID2D1Factory1 *pFactory, IN ID2D1DeviceContext* p2DContext, IN CDrawManager *pDrawManager)
{
	m_pFactory = pFactory;
	m_pD2DContext = p2DContext;
	m_pDrawManager = pDrawManager;

	if (m_pDrawFunction == nullptr) return;

	m_pDrawFunction->_SetLink(pFactory, p2DContext);
}

UINT32 CDrawEdit_Temp::SetUserSetInfo(UserSetInfo_t *pUserSetInfo, UserLayerSet_t *pUserLayerSetInfo)
{
	m_pUserSetInfo = pUserSetInfo;
	m_pUserLayerSetInfo = pUserLayerSetInfo;

	return RESULT_GOOD;
}

UINT32 CDrawEdit_Temp::SetViewInfo(IN ViewInfo_t* pViewInfo)
{
	if (pViewInfo == nullptr) return RESULT_BAD;

	m_pViewInfo = pViewInfo;

	if (m_pD2DContext != nullptr)
	{
		m_sizeTarget = D2D1::SizeF(static_cast<float>(m_pViewInfo->ptSizeXY.x),
			static_cast<float>(m_pViewInfo->ptSizeXY.y));

		SafeRelease(&m_pBmpRenderTarget);

		m_pD2DContext->CreateCompatibleRenderTarget(m_sizeTarget, &m_pBmpRenderTarget);
	}

	return RESULT_GOOD;
}

UINT32 CDrawEdit_Temp::SetTransform(IN const D2D1::Matrix3x2F& matTrans)
{
	m_ViewScale = matTrans;

	return RESULT_GOOD;
}

ID2D1BitmapRenderTarget * CDrawEdit_Temp::GetRender()
{
	return m_pBmpRenderTarget;
}

UINT32 CDrawEdit_Temp::ClearFeatureData()
{
	int nCount = static_cast<int>(m_vecFeature.size());
	for (int i = 0; i < nCount; i++)
	{
		delete m_vecFeature[i];
		m_vecFeature[i] = nullptr;
	}

	m_vecFeature.clear();

	m_bMoveMode = FALSE;
	m_bEditPoint = FALSE;

	return RESULT_GOOD;
}

//Step에서 선택된 항목을 셋한다.
UINT32 CDrawEdit_Temp::SetFeatureData(CStep* pStep)
{
	if (pStep == nullptr) return RESULT_BAD;

	//ClearFeatureData();

	int nLayerSetCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());
	int nLayerIndex;

	//Set Feature Data
	for (int i = 0; i < nLayerSetCount; i++)
	{

		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];

		if (pLayerSet->bCheck == true)
		{
			CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, pLayerSet->strLayer, nLayerIndex);
			if (pLayer == nullptr) continue;

			int nFeatureCount = static_cast<int>(pLayer->m_FeatureFile.m_arrFeature.GetCount());
			for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
			{

				CFeature *pFeature = pLayer->m_FeatureFile.m_arrFeature.GetAt(nfeno);

				AddFeatureData(nLayerIndex, nfeno, pFeature);
//				
// 				if (m_pDrawManager->m_bMoveMode == TRUE)
// 				{
// 					m_bMoveMode = TRUE;
// 				}
				
			}
		}
	}


	 
	return RESULT_GOOD;
}

UINT32 CDrawEdit_Temp::SetMaskData(IN const MaskType &eMaskType, CStep* pStep)
{//Set Mask Data
	if (pStep == nullptr) return RESULT_BAD;

	ClearFeatureData();

	int nLayerSetCount = static_cast<int>(m_pUserLayerSetInfo->vcLayerSet.size());
	int nLayerIndex;

	//Set Feature Data
	for (int i = 0; i < nLayerSetCount; i++)
	{
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[i];

		if (pLayerSet->bCheck == true)
		{
			CLayer *pLayer = CDrawFunction::_GetLayerPrt(pStep->m_arrLayer, pLayerSet->strLayer, nLayerIndex);
			if (pLayer == nullptr) continue;

			if (eMaskType == MaskType::enumType_Align)
			{
				int nAlignMaskCount = static_cast<int>(pLayer->m_arrAlign.GetCount());

				for (int nAlign = 0; nAlign < nAlignMaskCount; nAlign++)
				{
					CAlignMask *pAlignMask = pLayer->m_arrAlign.GetAt(nAlign);
					int nFeatureCount = static_cast<int>(pAlignMask->m_arrFeature.GetCount());
					for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
					{
						CFeature *pFeature = pAlignMask->m_arrFeature.GetAt(nfeno);

						AddFeatureData(nLayerIndex, nfeno, pFeature, nAlign);
					}
				}
			}
			else if (eMaskType == MaskType::enumType_Mask)
			{
				int nAlignMaskCount = static_cast<int>(pLayer->m_arrAlign.GetCount());
				for (int nAlign = 0; nAlign < nAlignMaskCount; nAlign++)
				{
					CAlignMask *pAlignMask = pLayer->m_arrMask.GetAt(nAlign);
					int nFeatureCount = static_cast<int>(pAlignMask->m_arrFeature.GetCount());
					for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
					{
						CFeature *pFeature = pAlignMask->m_arrFeature.GetAt(nfeno);

						AddFeatureData(nLayerIndex, nfeno, pFeature, nAlign);
					}
				}
			}

		}
	}

	return RESULT_GOOD;
}

UINT32 CDrawEdit_Temp::AddFeatureData(IN const int &nLayer, IN const int &nFeatureIndex, CFeature *pFeature, IN const int &nMaskIndex,
	IN const double &dMoveX_mm, IN const double &dMoveY_mm)
{
	if (pFeature == nullptr) return RESULT_BAD;

	switch (pFeature->m_eType)
	{
	case::FeatureType::Z:
	{
		CFeatureZ *pFeatureZ = (CFeatureZ*)pFeature;
		if (pFeatureZ->m_bHighlight == true)
		{
			CFeatureData *pTempFeature = new CFeatureData();
			pTempFeature->SetTp(nLayer, nFeatureIndex, pFeatureZ, nMaskIndex);
			pTempFeature->SetMove(dMoveX_mm, dMoveY_mm);

			m_vecFeature.push_back(pTempFeature);
		}
	}
	break;
	case FeatureType::L:
	{
		CFeatureL *pFeatureL = (CFeatureL*)pFeature;
		if (pFeatureL->m_bHighlight == true)
		{
			CFeatureData *pTempFeature = new CFeatureData();

			pTempFeature->SetLine(nLayer, nFeatureIndex, pFeatureL, nMaskIndex);
			pTempFeature->SetMove(dMoveX_mm, dMoveY_mm);

			m_vecFeature.push_back(pTempFeature);
		}
	}
	break;
	case FeatureType::P:
	{
		CFeatureP *pFeatureP = (CFeatureP*)pFeature;
		if (pFeatureP->m_bHighlight == true)
		{
			CFeatureData *pTempFeature = new CFeatureData();

			pTempFeature->SetPad(nLayer, nFeatureIndex, pFeatureP, nMaskIndex);
			pTempFeature->SetMove(dMoveX_mm, dMoveY_mm);

			m_vecFeature.push_back(pTempFeature);
		}
	}
	break;
	case FeatureType::A:
	{
		CFeatureA *pFeatureA = (CFeatureA*)pFeature;
		if (pFeatureA->m_bHighlight == true)
		{
			CFeatureData *pTempFeature = new CFeatureData();

			pTempFeature->SetArc(nLayer, nFeatureIndex, pFeatureA, nMaskIndex);
			pTempFeature->SetMove(dMoveX_mm, dMoveY_mm);

			m_vecFeature.push_back(pTempFeature);
		}
	}
	break;
	case FeatureType::T:
	{
		CFeatureT *pFeatureT = (CFeatureT*)pFeature;
		if (pFeatureT->m_bHighlight == true)
		{
			CFeatureData *pTempFeature = new CFeatureData();

			pTempFeature->SetText(nLayer, nFeatureIndex, pFeatureT, nMaskIndex);
			pTempFeature->SetMove(dMoveX_mm, dMoveY_mm);

			m_vecFeature.push_back(pTempFeature);
		}
	}
	break;
	case FeatureType::S:
	{
		CFeatureS *pFeatureS = (CFeatureS*)pFeature;

		//ObOe중 하나라도 선택이 되어 있는지 확인
		bool bSelect = FALSE;
		int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
		for (int nObOe = 0; nObOe < nObOeCount; nObOe++)
		{
			CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(nObOe);
			if (pObOe == nullptr) continue;

			if (pObOe->m_bHighlight == true)
			{
				bSelect = TRUE;
				break;
			}
		}
		//

		if (bSelect == TRUE)
		{
			CFeatureData *pTempFeature = new CFeatureData();
			pTempFeature->SetSurface(nLayer, nFeatureIndex, pFeatureS, nMaskIndex);
			pTempFeature->SetMove(dMoveX_mm, dMoveY_mm);

			m_vecFeature.push_back(pTempFeature);
		}

	}
	break;
	}


	return RESULT_GOOD;
}

BOOL   CDrawEdit_Temp::_IsOverlapFeature(vector<CFeatureData*> &vecFeature, IN const int &nLayer, IN const int &nFeatureIndex, CFeature *pFeature, IN const int &nMaskIndex )
{
	if (pFeature == nullptr) return FALSE;

	BOOL bOverlap = FALSE;//중복
	int nFeatureCount = static_cast<int>(vecFeature.size());
	for (int i = 0; i < nFeatureCount; i++)
	{
		CFeatureData* pFeatureData = vecFeature[i];

		if (pFeatureData->m_nLayer == nLayer &&
			pFeatureData->m_nFeatureIndex == nFeatureIndex &&
			pFeatureData->m_nMaskIndex == nMaskIndex)
		{//중복되는 Feature 있음
			bOverlap = TRUE;
			break;
		}
	}

	return bOverlap;

}

vector<CFeatureData*>& CDrawEdit_Temp::GetFeatureData()
{
	return m_vecFeature;
}

UINT32 CDrawEdit_Temp::SetMoveAxis(IN const int &nMoveAxis)
{
	m_nMoveAxis = nMoveAxis;

	return RESULT_GOOD;
}


UINT32 CDrawEdit_Temp::OnRender(CTypedPtrArray <CObArray, CStandardFont*>* pFontArr)
{
	if (m_pFactory == nullptr) return RESULT_BAD;
	if (m_pD2DContext == nullptr) return RESULT_BAD;

	if (m_pViewInfo == nullptr) return RESULT_BAD;

	if (m_pBmpRenderTarget == nullptr) return RESULT_BAD;
	if (m_pDrawFunction == nullptr) return RESULT_BAD;
	if (m_pUserSetInfo == nullptr) return RESULT_BAD;
	if (m_pUserLayerSetInfo == nullptr) return RESULT_BAD;

	m_pBmpRenderTarget->BeginDraw();
	m_pBmpRenderTarget->Clear(0);

	m_pBmpRenderTarget->SetTransform(m_ViewScale);

	int nFeatureCount = static_cast<int>(m_vecFeature.size());

	CSymbol*	pSymbol = nullptr;
	for (int i = 0; i < nFeatureCount; i++)
	{
		int nLayer = m_vecFeature[i]->m_nLayer;
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[nLayer];
		if (pLayerSet == nullptr) continue;

		switch (m_vecFeature[i]->m_pFeature->m_eType)
		{
		case FeatureType::S:
			CFeatureS* pFeatureS = (CFeatureS*)(m_vecFeature[i]->m_pFeature);
			pSymbol = pFeatureS->m_pSymbol;
			m_pDrawFunction->_DrawSurfaceFeature(m_pBmpRenderTarget, pFeatureS, pSymbol, pLayerSet, nullptr, TRUE, FALSE);
			break;
		}
	}

	for (int i = 0; i < nFeatureCount; i++)
	{
		int nLayer = m_vecFeature[i]->m_nLayer;
		LayerSet_t* pLayerSet = m_pUserLayerSetInfo->vcLayerSet[nLayer];
		if (pLayerSet == nullptr) continue;

		switch (m_vecFeature[i]->m_pFeature->m_eType)
		{
		case FeatureType::Z:
		{
			CFeatureZ* pFeatureZ = (CFeatureZ*)(m_vecFeature[i]->m_pFeature);
			pSymbol = pFeatureZ->m_pSymbol;
			m_pDrawFunction->_DrawTpFeature(m_pBmpRenderTarget, pFeatureZ, pSymbol, pLayerSet, nullptr, TRUE);
		}
		break;
		case FeatureType::L:
		{
			CFeatureL* pFeatureL = (CFeatureL*)(m_vecFeature[i]->m_pFeature);
			pSymbol = pFeatureL->m_pSymbol;
			m_pDrawFunction->_DrawLineFeature(m_pBmpRenderTarget, pFeatureL, pSymbol, pLayerSet, nullptr, TRUE);
		}
		break;

		case FeatureType::P:
		{
			CFeatureP* pFeatureP = (CFeatureP*)(m_vecFeature[i]->m_pFeature);
			pSymbol = pFeatureP->m_pSymbol;
			m_pDrawFunction->_DrawPadFeature(m_pBmpRenderTarget, pFeatureP, pSymbol, pLayerSet, pFontArr, nullptr, TRUE, m_pUserSetInfo, FALSE);
		}
		break;

		case FeatureType::A:
		{
			CFeatureA* pFeatureA = (CFeatureA*)(m_vecFeature[i]->m_pFeature);
			pSymbol = pFeatureA->m_pSymbol;
			m_pDrawFunction->_DrawArcFeature(m_pBmpRenderTarget, pFeatureA, pSymbol, pLayerSet, nullptr, TRUE);
		}

		break;
		case FeatureType::T:
		{
			CFeatureT* pFeatureT = (CFeatureT*)(m_vecFeature[i]->m_pFeature);
			pSymbol = pFeatureT->m_pSymbol;
			m_pDrawFunction->_DrawTextFeature(m_pBmpRenderTarget, pFeatureT, pSymbol, pLayerSet, pFontArr, nullptr, TRUE);
		}
		break;
		}
	}


	//
	_DrawTempPoint();

	_Draw_EditPoint();

	m_pBmpRenderTarget->EndDraw();

	return RESULT_GOOD;
}

UINT32 CDrawEdit_Temp::_Draw_EditPoint()
{
	if (m_pBmpRenderTarget == nullptr) return RESULT_BAD;

	

	int nFeatureCount = static_cast<int>(m_vecFeature.size());

	for (int i = 0; i < nFeatureCount; i++)
	{
		if (m_vecFeature[i] == nullptr) continue;

		int nPointCount = static_cast<int>(m_vecFeature[i]->m_vecEditPoint.size());
		for (int j = 0; j < nPointCount; j++)
		{
			m_pDrawFunction->DrawEditPoint(m_pBmpRenderTarget, m_vecFeature[i]->m_vecEditPoint[j]);
		}
	}

	return RESULT_GOOD;
}

UINT32 CDrawEdit_Temp::_DrawTempPoint()
{
	if (m_vecTempPoint[0].fptPoint.x != 0 &&
		m_vecTempPoint[0].fptPoint.y != 0)
	{
		m_pDrawFunction->_DrawTempPoint(m_pBmpRenderTarget, m_vecTempPoint[0].fptPoint);
	}


	//Draw Temp Point
	int nLineWidth = 1;
	for (int i = 1; i < m_nMaxTempPoint; i++)
	{
		if (m_vecTempPoint[i].strName == _T("")) continue;
		m_pDrawFunction->_DrawTempPoint(m_pBmpRenderTarget, m_vecTempPoint[i].fptPoint);
		//_DrawTempLine();
		//cemter
		if (m_vecTempPoint[i].bosoc == false)
		{
			m_pDrawFunction->_DrawTempArcPoint(m_pBmpRenderTarget, m_vecTempPoint[i].fptPoint2);
			m_pDrawFunction->_DrawTempArc(m_pBmpRenderTarget, m_vecTempPoint[i - 1].fptPoint, m_vecTempPoint[i].fptPoint, m_vecTempPoint[i].fptPoint2, m_vecTempPoint[i].bCw);
		}

		if (m_vecTempPoint[i].bosoc == true && m_vecTempPoint[i].strName != _T("Arc End") && m_vecTempPoint[i].strName != _T("Arc Center"))
		{
			m_pDrawFunction->_DrawTempLine(m_pBmpRenderTarget, m_vecTempPoint[i - 1].fptPoint, m_vecTempPoint[i].fptPoint);
		}

		
	}
	return RESULT_GOOD;
}







UINT32 CDrawEdit_Temp::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_pDrawManager == nullptr) return RESULT_BAD;
	if (m_pBmpRenderTarget == nullptr) return RESULT_BAD;


	D2D1_POINT_2F fptMouse = D2D1::Point2F(static_cast<float>(point.x), static_cast<float>(point.y));
	D2D1_MATRIX_3X2_F matTrans;
	m_pBmpRenderTarget->GetTransform(&matTrans);
	D2D1_POINT_2F fptMouse_Real = CDrawFunction::DetansformPoint(fptMouse, matTrans, m_pUserSetInfo);

	if (_HitTest_EditPoint(m_vecFeature, fptMouse_Real) == TRUE)
	{
		m_bEditPoint = TRUE;
		m_bMoveMode = FALSE;
		m_bResize = TRUE;
		m_stEditSelectInfo.dptMove.x = fptMouse_Real.x;
		m_stEditSelectInfo.dptMove.y = fptMouse_Real.y;

		m_dptPreMoveTemp.x = fptMouse_Real.x;
		m_dptPreMoveTemp.y = fptMouse_Real.y;

		HCURSOR hCursor = ::LoadCursor(NULL, IDC_CROSS);
		SetCursor(hCursor);
	}
	else if (BOOL bTouch = _HitTest_Feature(m_vecFeature, fptMouse, m_ViewScale) == TRUE )
	{
		if (bTouch == TRUE)
		{
			m_bMoveMode = TRUE;
			m_bEditPoint = FALSE;
			m_ptStartPoint = point;//D2D1::Point2F(static_cast<float>(point.x), static_cast<float>(point.y));
		}
		else
		{
			m_bMoveMode = FALSE;
			m_bEditPoint = FALSE;
		}
	}
	else
	{
		m_bMoveMode = FALSE;
		m_bEditPoint = FALSE;

		HCURSOR hCursor = ::LoadCursor(NULL, IDC_ARROW);
		SetCursor(hCursor);

		return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 CDrawEdit_Temp::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bMoveMode = FALSE;
	m_bEditPoint = FALSE;
	
	
	m_stEditSelectInfo.Reset();

	return RESULT_GOOD;
}

UINT32 CDrawEdit_Temp::OnMouseMove(UINT nFlags, CPoint point)
{

	if (m_pDrawManager == nullptr) return RESULT_BAD;
	
	D2D1_POINT_2F fptMouse = D2D1::Point2F(static_cast<float>(point.x), static_cast<float>(point.y));
	D2D1_MATRIX_3X2_F matTrans;
	m_pBmpRenderTarget->GetTransform(&matTrans);
	D2D1_POINT_2F fptMouse_Real = CDrawFunction::DetansformPoint(fptMouse, matTrans, m_pUserSetInfo);

	if (m_bEditPoint == FALSE && _HitTest_EditPoint(m_vecFeature, fptMouse_Real) == TRUE )
	{
		HCURSOR hCursor = ::LoadCursor(NULL, IDC_CROSS);
		SetCursor(hCursor);
	}
	else if (m_bEditPoint == TRUE)
	{//Point Edit Mode
		CPointD dptMove;
		dptMove.x = fptMouse_Real.x - m_dptPreMoveTemp.x;
		dptMove.y = fptMouse_Real.y - m_dptPreMoveTemp.y;

		if (m_stEditSelectInfo.nFeatureIndex >= 0 &&
			m_stEditSelectInfo.nFeatureIndex < static_cast<int>(m_vecFeature.size()))
		{
			m_vecFeature[m_stEditSelectInfo.nFeatureIndex]->MoveEditPoint(m_stEditSelectInfo.nPointIndex, dptMove.x, dptMove.y);
		}

		m_dptPreMoveTemp.x = fptMouse_Real.x;
		m_dptPreMoveTemp.y = fptMouse_Real.y;

		m_bResize = TRUE;
	}
	else if (m_bMoveMode == FALSE )
	{
		BOOL bTouch = _HitTest_Feature(m_vecFeature, fptMouse, m_ViewScale);
		if (bTouch == TRUE)
		{
			HCURSOR hHandCursor = ::LoadCursor(NULL, IDC_SIZEALL);
			SetCursor(hHandCursor);
		}
		else
		{
			HCURSOR hHandCursor = ::LoadCursor(NULL, IDC_ARROW);
			SetCursor(hHandCursor);

		}
	}
	else if (m_bMoveMode == TRUE)
	{
		
		CPoint ptMove = CPoint(point.x - m_ptStartPoint.x, point.y - m_ptStartPoint.y);
		D2D1_POINT_2F fptMove = D2D1::Point2F(static_cast<float>(ptMove.x), static_cast<float>(ptMove.y));


		D2D1::Matrix3x2F matScale = m_ViewScale;
		matScale.dx = 0;
		matScale.dy = 0;

		D2D1_POINT_2F fptMove_mm = m_pDrawFunction->DetansformPoint(fptMove, matScale, m_pUserSetInfo);

		m_totalMove.x += fptMove_mm.x;
		m_totalMove.y += fptMove_mm.y;

		m_pDrawManager->FeatureSpec.dMoveX = static_cast<float>(m_totalMove.x);
		m_pDrawManager->FeatureSpec.dMoveY = static_cast<float>(m_totalMove.y);

		
		_Move_Feature(m_vecFeature, fptMove_mm);
		

		m_ptStartPoint = point;
		
		
		
	}

	return RESULT_GOOD;
}

BOOL CDrawEdit_Temp::_HitTest_Feature(vector<CFeatureData*> &vecFeature, D2D1_POINT_2F fptPoint, D2D1::Matrix3x2F &ViewScale)
{
	BOOL bTouch = FALSE;
	CSymbol *pSymbol;
	int nFeatureCount = static_cast<int>(vecFeature.size());

	for (int i = nFeatureCount - 1; i >= 0; i--)
	{
		CFeature *pFeature = vecFeature[i]->m_pFeature;
		if (pFeature == nullptr) continue;

		switch (pFeature->m_eType)
		{
		case FeatureType::Z:
		{
			CFeatureZ *pFeatureZ = (CFeatureZ*)pFeature;
			pSymbol = pFeatureZ->m_pSymbol;
			if (m_pDrawFunction->CheckHighlight_TpFeature(fptPoint, pFeatureZ, pSymbol, ViewScale) == TRUE)

			{
				bTouch = TRUE;
			}	
		}
		break;
		case FeatureType::L:		//Line
		{
			CFeatureL *pFeatureL = (CFeatureL*)pFeature;
			pSymbol = pFeatureL->m_pSymbol;
			if (m_pDrawFunction->CheckHighlight_LineFeature(fptPoint, pFeatureL, pSymbol, ViewScale) == TRUE)
			{
				bTouch = TRUE;
			}
		}
		break;
		case FeatureType::P:		//Pad
		{
			CFeatureP *pFeatureP = (CFeatureP*)pFeature;
			pSymbol = pFeatureP->m_pSymbol;
			if (m_pDrawFunction->CheckHighlight_PadFeature(fptPoint, pFeatureP, pSymbol, ViewScale) == TRUE)
			{
				bTouch = TRUE;
			}
		}
		break;
		case FeatureType::A:		//Arc
		{
			CFeatureA *pFeatureA = (CFeatureA*)pFeature;
			pSymbol = pFeatureA->m_pSymbol;
			if (m_pDrawFunction->CheckHighlight_ArcFeature(fptPoint, pFeatureA, pSymbol, ViewScale) == TRUE)
			{
				bTouch = TRUE;
			}
		}
		break;
		case FeatureType::T:		//Text
		{
		}
		break;
		}

		if (bTouch == TRUE)
		{
			break;
		}
	}

	if (bTouch == FALSE)
	{
		int nFeatureCount = static_cast<int>(vecFeature.size());
		for (int i = nFeatureCount - 1; i >= 0; i--)
		{
			CFeature *pFeature = vecFeature[i]->m_pFeature;
			if (pFeature == nullptr) continue;

			switch (pFeature->m_eType)
			{
			case FeatureType::S:		//Surface	
			{
				CFeatureS *pFeatureS = (CFeatureS*)pFeature;
				pSymbol = pFeatureS->m_pSymbol;
				int nSelectIndex = -1;//사용안함
				float  fSelectArea = 0.f;//사용안함
				if (m_pDrawFunction->CheckHighlight_SurfaceFeature(fptPoint, pFeatureS, pSymbol, ViewScale, nSelectIndex, fSelectArea) == TRUE)
				{
					bTouch = TRUE;
				}
			}
			}

			if (bTouch == TRUE)
			{
				break;
			}
		}
	}

	return bTouch;
}

BOOL CDrawEdit_Temp::_HitTest_EditPoint(vector<CFeatureData*> &vecFeature, D2D1_POINT_2F fptPoint)
{
	BOOL bHit = FALSE;

	int nFeatureCount = static_cast<int>(vecFeature.size());
	for (int nfeno = 0; nfeno < nFeatureCount; nfeno++)
	{
		if ( vecFeature[nfeno] == nullptr ) continue;

		int nPointCount = static_cast<int>(vecFeature[nfeno]->m_vecEditPoint.size());
		for (int nPoint = 0; nPoint < nPointCount; nPoint++)
		{
			if (vecFeature[nfeno]->m_vecEditPoint[nPoint].IsPtInRectD(fptPoint.x, fptPoint.y) == TRUE)
			{
				m_stEditSelectInfo.nFeatureIndex = nfeno;
				m_stEditSelectInfo.nPointIndex = nPoint;
				bHit = TRUE;
				break;
			}
		}

		if ( bHit == TRUE ) break;

	}

	if (bHit == FALSE)
	{
		m_bEditPoint = FALSE;
		m_stEditSelectInfo.Reset();
	}

	return bHit;

}




UINT32 CDrawEdit_Temp::_Move_Feature(vector<CFeatureData*> &vecFeature, D2D1_POINT_2F fptPoint)
{
	
	int nFeatureCount = static_cast<int>(vecFeature.size());

// 	if (m_nMoveAxis == Edit_MoveAxis::enumMoveAxis_None)
// 	{
// 		return RESULT_BAD;
// 	}
	if (m_nMoveAxis == Edit_MoveAxis::enumMoveAxis_X)
	{
		//return RESULT_BAD;
	}
	else if (m_nMoveAxis == Edit_MoveAxis::enumMoveAxis_X)
	{
		fptPoint.y = 0.f;
	}
	else if (m_nMoveAxis == Edit_MoveAxis::enumMoveAxis_Y)
	{
		fptPoint.x = 0.f;
	}

	for (int i = 0; i < nFeatureCount; i++)
	{
		CFeature *pFeature = vecFeature[i]->m_pFeature;
		if (pFeature == nullptr) continue;

		switch (pFeature->m_eType)
		{
		case FeatureType::Z:
		{
			CFeatureZ *pFeatureZ = (CFeatureZ*)pFeature;
			pFeatureZ->m_dX += fptPoint.x;
			pFeatureZ->m_dY += fptPoint.y;
		}
		break;
		case FeatureType::L:		//Line	
		{
			CFeatureL *pFeatureL = (CFeatureL *)pFeature;
			pFeatureL->m_dXs += fptPoint.x;
			pFeatureL->m_dXe += fptPoint.x;
			pFeatureL->m_dYs += fptPoint.y;
			pFeatureL->m_dYe += fptPoint.y;
		}
		break;
		case FeatureType::P:		//Pad	
		{
			CFeatureP *pFeatureP = (CFeatureP *)pFeature;
			pFeatureP->m_dX += fptPoint.x;
			pFeatureP->m_dY += fptPoint.y;

		}
		break;
		case FeatureType::A:		//Arc	
		{
			CFeatureA *pFeatureA = (CFeatureA *)pFeature;
			pFeatureA->m_dXs += fptPoint.x;
			pFeatureA->m_dYs += fptPoint.y;
			pFeatureA->m_dXe += fptPoint.x;
			pFeatureA->m_dYe += fptPoint.y;
			pFeatureA->m_dXc += fptPoint.x;
			pFeatureA->m_dYc += fptPoint.y;
		}
		break;
		case FeatureType::T:		//Text	
		{
			CFeatureT *pFeatureT = (CFeatureT *)pFeature;
		}
		break;
		case FeatureType::S:		//Surface	
		{
			CFeatureS *pFeatureS = (CFeatureS *)pFeature;

			int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
			for (int i = 0; i < nObOeCount; i++)
			{
				CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(i);
				if (pObOe == nullptr) continue;

				if (pObOe->m_bHighlight == true)
				{
					int nOsOcCount = static_cast<int>(pObOe->m_arrOsOc.GetCount());
					for (int j = 0; j < nOsOcCount; j++)
					{
						COsOc *pOsOc = pObOe->m_arrOsOc.GetAt(j);
						if (pOsOc == nullptr) continue;

						pOsOc->m_dX += fptPoint.x;
						pOsOc->m_dY += fptPoint.y;

						if (pOsOc->m_bOsOc == false)
						{
							((COc*)pOsOc)->m_dXc += fptPoint.x;
							((COc*)pOsOc)->m_dYc += fptPoint.y;
						}

					}

					pObOe->m_dXbs += fptPoint.x;
					pObOe->m_dYbs += fptPoint.y;
				}
			}
		}
		break;
		}

		//Move 되었으니 EditPoint를 재 세팅한다.
		vecFeature[i]->UpdateEditPoint();
	}

// 	m_KeyMove.x += fptPoint.x;
// 	m_KeyMove.y += fptPoint.y;

	m_bMoved = TRUE;

	

	return RESULT_GOOD;
}

UINT32 CDrawEdit_Temp::SetPadRotate(IN const int&nIndex, IN const BOOL&bMirror)
{
	if (nIndex < 0) return RESULT_BAD;

	int nFeatureCount = static_cast<int>(m_vecFeature.size());

	for (int i = 0; i < nFeatureCount; i++)
	{
		CFeature *pFeature = m_vecFeature[i]->m_pFeature;
		if (pFeature == nullptr) continue;

		switch (pFeature->m_eType)
		{
		case FeatureType::P:
		{CFeatureP* pFeatureP = (CFeatureP *)pFeature;
		if (pFeatureP == nullptr) continue;

		if (bMirror == FALSE)
		{
			pFeatureP->m_eOrient = static_cast<Orient>(nIndex);
		}

		else
		{
			pFeatureP->m_eOrient = static_cast<Orient>(nIndex + 4);
		}

		}
		break;
		}


	}
	return RESULT_GOOD;
}



UINT32 CDrawEdit_Temp::SetLineArcResize(IN const double &dSizePersent)
{


	if (dSizePersent == 0.0) return RESULT_GOOD;

	int nFeatureCount = static_cast<int>(m_vecFeature.size());

	for (int i = 0; i < nFeatureCount; i++)
	{
		CFeature *pFeature = m_vecFeature[i]->m_pFeature;
		if (pFeature == nullptr) continue;

		switch (pFeature->m_eType)
		{
		case FeatureType::L:		//Line	
		{
			

			CFeatureL *pFeatureL = (CFeatureL *)pFeature;
			
			if (pFeatureL == nullptr) continue;
			if (pFeatureL->m_pSymbol == nullptr) continue;
			if (pFeatureL->m_pSymbol->m_vcParams.size() <= 0) continue;
			pFeatureL->m_pSymbol->m_vcParams[0] = (pFeatureL->m_pSymbol->m_vcParams[0] * (dSizePersent));

			pFeature->m_pSymbol->m_strUserSymbol = CDrawFunction::GetSymbolName(pFeature->m_pSymbol->m_eSymbolName, pFeature->m_pSymbol->m_vcParams);
		}
		break;
		case FeatureType::A:		//Arc	
		{
			CFeatureA *pFeatureA = (CFeatureA *)pFeature;

			if (pFeatureA == nullptr) continue;
			if (pFeatureA->m_pSymbol == nullptr) continue;
			if (pFeatureA->m_pSymbol->m_vcParams.size() <= 0) continue;

			pFeatureA->m_pSymbol->m_vcParams[0] = (pFeatureA->m_pSymbol->m_vcParams[0] * (dSizePersent));

			pFeature->m_pSymbol->m_strUserSymbol = CDrawFunction::GetSymbolName(pFeature->m_pSymbol->m_eSymbolName, pFeature->m_pSymbol->m_vcParams);
		}
		break;
		}
	}

	return RESULT_GOOD;
}

UINT32 CDrawEdit_Temp::SetAttribute(IN const CString &strAttrName, IN const CString &strAttString)
{
	int nFeatureCount = static_cast<int>(m_vecFeature.size());

	for (int i = 0; i < nFeatureCount; i++)
	{
		CFeature *pFeature = m_vecFeature[i]->m_pFeature;
		if (pFeature == nullptr) continue;
		switch (pFeature->m_eType)
		{
		case FeatureType::L:
		{
			CFeatureL *pFeatureL = (CFeatureL *)pFeature;
			if (pFeatureL == nullptr) continue;
			if (pFeatureL->m_pSymbol == nullptr) continue;
			if (pFeatureL->m_pSymbol->m_vcParams.size() <= 0) continue;

			CAttribute* pAttribute = new CAttribute();

			CAttributeTextString *pAttributeString = new CAttributeTextString();

			pAttributeString->m_strText = strAttString;

			pAttribute->m_strName = strAttrName;

			pFeatureL->m_arrAttribute.Add(pAttribute);
			pFeatureL->m_arrAttributeTextString.Add(pAttributeString);

		}
		break;
		case FeatureType::S:
		{
			CFeatureS *pFeatureS = (CFeatureS *)pFeature;
			if (pFeatureS == nullptr) continue;
			if (pFeatureS->m_pSymbol == nullptr) continue;

			CAttribute* pAttribute = new CAttribute();

			CAttributeTextString *pAttributeString = new CAttributeTextString();

			pAttributeString->m_strText = strAttString;

			pAttribute->m_strName = strAttrName;

			pFeatureS->m_arrAttribute.Add(pAttribute);
			pFeatureS->m_arrAttributeTextString.Add(pAttributeString);
		}
		break;
		case FeatureType::P:
		{
			CFeatureP *pFeatureP = (CFeatureP *)pFeature;
			if (pFeatureP == nullptr) continue;
			if (pFeatureP->m_pSymbol == nullptr) continue;
			if (pFeatureP->m_pSymbol->m_vcParams.size() <= 0) continue;


			switch (pFeatureP->m_pSymbol->m_eSymbolName)
			{


			case SymbolName::r:
			{
				
				CAttribute* pAttribute = new CAttribute();

				CAttributeTextString *pAttributeString = new CAttributeTextString();

				pAttributeString->m_strText = strAttString;

				pAttribute->m_strName = strAttrName;

				pFeatureP->m_arrAttribute.Add(pAttribute);
				pFeatureP->m_arrAttributeTextString.Add(pAttributeString);
			}
			break;
			case SymbolName::rect:
			{
				CAttribute* pAttribute = new CAttribute();
				CAttributeTextString *pAttributeString = new CAttributeTextString();

				pAttribute->m_strName = strAttrName;
				pAttributeString->m_strText = strAttString;

				pFeatureP->m_arrAttribute.Add(pAttribute);
				pFeatureP->m_arrAttributeTextString.Add(pAttributeString);

			}

			break;


			}

			
		}
		}
		
		
	}
	return RESULT_GOOD;
}

UINT32 CDrawEdit_Temp::SetLineShape(IN const CString &strShape , IN const double &dLineWidth)
{
	int nFeatureCount = static_cast<int>(m_vecFeature.size());

	for (int i = 0; i < nFeatureCount; i++)
	{
		CFeature *pFeature = m_vecFeature[i]->m_pFeature;
		if (pFeature == nullptr) continue;

		switch (pFeature->m_eType)
		{
		case FeatureType::L:
		{
			CFeatureP *pFeatureL = (CFeatureP *)pFeature;
			if (pFeatureL == nullptr) continue;
			if (pFeatureL->m_pSymbol == nullptr) continue;

			pFeatureL->m_pSymbol->m_vcParams[0] = dLineWidth;

			if (strShape == "Round")
			{
				pFeatureL->m_pSymbol->m_eSymbolName = SymbolName::r;
			}

			if (strShape == "Square")
			{
				pFeatureL->m_pSymbol->m_eSymbolName = SymbolName::s;
			}
		}
		break;
		}
	}
	return RESULT_GOOD;
}


UINT32 CDrawEdit_Temp::SetPadShape(IN const double &dRadius, IN const double &dWidth, IN const double &dHeight)
{
	int nFeatureCount = static_cast<int>(m_vecFeature.size());

	for (int i = 0; i < nFeatureCount; i++)
	{
		CFeature *pFeature = m_vecFeature[i]->m_pFeature;
		if (pFeature == nullptr) continue;

		switch (pFeature->m_eType)
		{
		case FeatureType::P:		//Pad
		{
			CFeatureP *pFeatureP = (CFeatureP *)pFeature;
			if (pFeatureP == nullptr) continue;
			if (pFeatureP->m_pSymbol == nullptr) continue;
			if (pFeatureP->m_pSymbol->m_vcParams.size() <= 0) continue;


			switch (pFeatureP->m_pSymbol->m_eSymbolName)
			{
			case SymbolName::r:
			{
				pFeatureP->m_pSymbol->m_vcParams[0] = dRadius;
			}
			break;
			case SymbolName::rect:
			{
				pFeatureP->m_pSymbol->m_vcParams[0] = dWidth;
				pFeatureP->m_pSymbol->m_vcParams[1] = dHeight;
			}
			break;
			}

		}
		break;
		}
	
	}
	return RESULT_GOOD;
}

UINT32 CDrawEdit_Temp::SetPadTextResize(IN const double &dSizePersent)
{
	if (dSizePersent == 0.f) return RESULT_GOOD;

	int nFeatureCount = static_cast<int>(m_vecFeature.size());

	
	

	for (int i = 0; i < nFeatureCount; i++)
	{
		CFeature *pFeature = m_vecFeature[i]->m_pFeature;
		if (pFeature == nullptr) continue;


		switch (pFeature->m_eType)
		{
		case FeatureType::P:		//Pad
		{
			CFeatureP *pFeatureP = (CFeatureP *)pFeature;
			if (pFeatureP == nullptr) continue;
			if (pFeatureP->m_pSymbol == nullptr) continue;
			if (pFeatureP->m_pSymbol->m_vcParams.size() <= 0) continue;


			switch (pFeatureP->m_pSymbol->m_eSymbolName)
			{
			case SymbolName::r:

				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (dSizePersent));
				break;
			case SymbolName::rect:
 
// 				pFeatureP->m_pSymbol->m_vcParams[0] = 0.2;
// 				pFeatureP->m_pSymbol->m_vcParams[1] = 0.3;
				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * dSizePersent);  /** (fSizePersent * 0.01))*/
				pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * dSizePersent);
				
				break;
			case SymbolName::s:
				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (dSizePersent));
				break;
// 			case SymbolName::rectxr:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * (fSizePersent * 0.01));
// 				break;
// 			case SymbolName::rectxc:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * (fSizePersent * 0.01));
// 				break;
// 			case SymbolName::oval:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * (fSizePersent * 0.01));
// 				break;
// 			case SymbolName::di:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * (fSizePersent * 0.01));
// 				break;
// 			case SymbolName::octagon:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[2] = (pFeatureP->m_pSymbol->m_vcParams[2] * (fSizePersent * 0.01));
// 				break;
// 			case SymbolName::donut_r:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * (fSizePersent * 0.01));
// 				break;
// 			case SymbolName::donut_s:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * (fSizePersent * 0.01));
// 				break;
// 			case SymbolName::hex_l:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[2] = (pFeatureP->m_pSymbol->m_vcParams[2] * (fSizePersent * 0.01));
// 				break;
// 			case SymbolName::hex_s:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[2] = (pFeatureP->m_pSymbol->m_vcParams[2] * (fSizePersent * 0.01));
// 				break;
// 			case SymbolName::bfr:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				break;
// 			case SymbolName::bfs:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				break;
// 			case SymbolName::tri:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * (fSizePersent * 0.01));
// 				break;
// 			case SymbolName::oval_h:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * (fSizePersent * 0.01));
// 				break;
// 			case SymbolName::thr:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[4] = (pFeatureP->m_pSymbol->m_vcParams[4] * (fSizePersent * 0.01));
// 				break;
// 			case SymbolName::ths:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[4] = (pFeatureP->m_pSymbol->m_vcParams[4] * (fSizePersent * 0.01));
// 				break;
// 			case SymbolName::s_ths:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[4] = (pFeatureP->m_pSymbol->m_vcParams[4] * (fSizePersent * 0.01));
// 				break;
// 			case SymbolName::s_tho:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[4] = (pFeatureP->m_pSymbol->m_vcParams[4] * (fSizePersent * 0.01));
// 				break;
// 			case SymbolName::sr_ths:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[4] = (pFeatureP->m_pSymbol->m_vcParams[4] * (fSizePersent * 0.01));
// 				break;
// 			case SymbolName::rc_ths:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[4] = (pFeatureP->m_pSymbol->m_vcParams[4] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[5] = (pFeatureP->m_pSymbol->m_vcParams[5] * (fSizePersent * 0.01));
// 				break;
// 			case SymbolName::rc_tho:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[4] = (pFeatureP->m_pSymbol->m_vcParams[4] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[5] = (pFeatureP->m_pSymbol->m_vcParams[5] * (fSizePersent * 0.01));
// 				break;
// 			case SymbolName::el:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * (fSizePersent * 0.01));
// 				break;
// 			case SymbolName::moire:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[3] = (pFeatureP->m_pSymbol->m_vcParams[3] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[4] = (pFeatureP->m_pSymbol->m_vcParams[4] * (fSizePersent * 0.01));
// 				break;
// 			case SymbolName::hole:
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fSizePersent * 0.01));
// 				/*
// 				pFeatureP->m_pSymbol->m_vcParams[2] = (pFeatureP->m_pSymbol->m_vcParams[2] * (fSizePersent * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[3] = (pFeatureP->m_pSymbol->m_vcParams[3] * (fSizePersent * 0.01));
// 				*/

			default:
				break;
			}


			pFeature->m_pSymbol->m_strUserSymbol = CDrawFunction::GetSymbolName(pFeature->m_pSymbol->m_eSymbolName, pFeature->m_pSymbol->m_vcParams);

			{

			}
		}
		break;
		case FeatureType::T:		//Text
		{
		}
		break;
		}
	}

	return RESULT_GOOD;
}

UINT32 CDrawEdit_Temp::SetSurfaceResize(IN const double &dSizePersent)
{
	if (dSizePersent == 0.f) return RESULT_GOOD;

	int nFeatureCount = static_cast<int>(m_vecFeature.size());

	for (int i = 0; i < nFeatureCount; i++)
	{
		CFeature *pFeature = m_vecFeature[i]->m_pFeature;
		if (pFeature == nullptr) continue;

		switch (pFeature->m_eType)
		{

		case FeatureType::S:		//Surface
		{
			CFeatureS *pFeatureS = (CFeatureS *)pFeature;
			if (pFeatureS == nullptr) continue;

			int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
			for (int nObOe = 0; nObOe < nObOeCount; nObOe++)
			{
				CObOe *pObOe = (CObOe *)pFeatureS->m_arrObOe.GetAt(nObOe);
				if (pObOe == nullptr) continue;

				double dCenterX = (pObOe->m_MinMaxRect.left + pObOe->m_MinMaxRect.right) / 2.;
				double dCenterY = (pObOe->m_MinMaxRect.top + pObOe->m_MinMaxRect.bottom) / 2.;

				pObOe->m_dXbs = pObOe->m_dXbs + (pObOe->m_dXbs - dCenterX) * (dSizePersent) - (pObOe->m_dXbs - dCenterX);
				pObOe->m_dYbs = pObOe->m_dYbs + (pObOe->m_dYbs - dCenterY) * (dSizePersent) - (pObOe->m_dYbs - dCenterY);

				int nOsOcCount = static_cast<int>(pObOe->m_arrOsOc.GetCount());
				for (int nOsOc = 0; nOsOc < nOsOcCount; nOsOc++)
				{
					COsOc *pOsOc = (COsOc *)pObOe->m_arrOsOc.GetAt(nOsOc);
					if (pOsOc == nullptr) continue;

					pOsOc->m_dX = pOsOc->m_dX + (pOsOc->m_dX - dCenterX) * (dSizePersent) - (pOsOc->m_dX - dCenterX);
					pOsOc->m_dY = pOsOc->m_dY + (pOsOc->m_dY - dCenterY) * (dSizePersent) - (pOsOc->m_dY - dCenterY);

					if (pOsOc->m_bOsOc == false)
					{//OC
						COc *pOc = (COc *)pOsOc;

						pOc->m_dXc = pOc->m_dXc + (pOc->m_dXc - dCenterX) * (dSizePersent) - (pOc->m_dXc - dCenterX);
						pOc->m_dYc = pOc->m_dYc + (pOc->m_dYc - dCenterY) * (dSizePersent) - (pOc->m_dYc - dCenterY);

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

UINT32 CDrawEdit_Temp::SetResize(IN const float &fSizePersent)
{
	if (fSizePersent == 0.f) return RESULT_GOOD;

	int nFeatureCount = static_cast<int>(m_vecFeature.size());
	
	for (int i = 0; i < nFeatureCount; i++)
	{
		CFeature *pFeature = m_vecFeature[i]->m_pFeature;
		if (pFeature == nullptr) continue;


		switch (pFeature->m_eType)
		{
		case FeatureType::P:		//Pad
		{
			CFeatureP *pFeatureP = (CFeatureP *)pFeature;
			if (pFeatureP == nullptr) continue;
			if (pFeatureP->m_pSymbol == nullptr) continue;
			if (pFeatureP->m_pSymbol->m_vcParams.size() <= 0) continue;


			switch (pFeatureP->m_pSymbol->m_eSymbolName)
			{
			case SymbolName::rect:

				pFeatureP->m_pSymbol->m_vcParams[0] = pFeatureP->m_pSymbol->m_vcParams[0] * fSizePersent;
				pFeatureP->m_pSymbol->m_vcParams[1] = pFeatureP->m_pSymbol->m_vcParams[1] * fSizePersent;

				break;
			}
		}
		}
	}

	

	return RESULT_GOOD;
}

UINT32 CDrawEdit_Temp::SetMaskResize(IN const float &fPersentWidth, IN const float &fPersentHeight)
{
	if (fPersentWidth == 0.f) return RESULT_GOOD;
	if (fPersentHeight == 0.f) return RESULT_GOOD;

	int nFeatureCount = static_cast<int>(m_vecFeature.size());

	for (int i = 0; i < nFeatureCount; i++)
	{
		CFeature *pFeature = m_vecFeature[i]->m_pFeature;
		if (pFeature == nullptr) continue;

		switch (pFeature->m_eType)
		{
		case FeatureType::P:		//Pad
		{
			CFeatureP *pFeatureP = (CFeatureP *)pFeature;
			if (pFeatureP == nullptr) continue;
			if (pFeatureP->m_pSymbol == nullptr) continue;
			if (pFeatureP->m_pSymbol->m_vcParams.size() <= 0) continue;

			int nParamCount = static_cast<int>(pFeatureP->m_pSymbol->m_vcParams.size());
			if (nParamCount <= 0 || nParamCount > 2) continue;


			pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fPersentWidth * 0.01));
			pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * (fPersentHeight * 0.01));

// 			if (pFeatureP->m_pSymbol->m_eSymbolName == SymbolName::s)
// 			{
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fPersentWidth * 0.01));
// 			}
// 			
// 			else if (pFeatureP->m_pSymbol->m_eSymbolName == SymbolName::r)
// 
// 			{
// 				pFeatureP->m_pSymbol->m_vcParams[0] = (pFeatureP->m_pSymbol->m_vcParams[0] * (fPersentWidth * 0.01));
// 				pFeatureP->m_pSymbol->m_vcParams[1] = (pFeatureP->m_pSymbol->m_vcParams[1] * (fPersentHeight * 0.01));
// 			}
		}
		break;
		}
	}

	return RESULT_GOOD;
}

void CDrawEdit_Temp::ClearTempPoint()
{
	m_vecTempPoint.clear();
	m_vecTempPoint.resize(m_nMaxTempPoint);
}

UINT32 CDrawEdit_Temp::SetTempPoint(vector<TempPoint> &vecTempPoint)
{
	ClearTempPoint();

	int nSetCount;
	int nSrcSize = static_cast<int>(vecTempPoint.size());
	if (nSrcSize < m_nMaxTempPoint)
	{
		nSetCount = nSrcSize;
	}
	else
	{
		nSetCount = m_nMaxTempPoint;
	}

	for (int i = 0; i < nSetCount; i++)
	{
		m_vecTempPoint[i] = vecTempPoint[i];
	}

	return RESULT_GOOD;
}

UINT32 CDrawEdit_Temp::SetTempPoint(IN const int &nIndex, IN const CString &strName, IN const float &fX, IN const float &fY)
{
	int nSize = static_cast<int>(m_vecTempPoint.size());
	if (nIndex < 0) return RESULT_BAD;
	if (nIndex >= nSize) return RESULT_BAD;



	m_vecTempPoint[nIndex].strName = strName;
	m_vecTempPoint[nIndex].fptPoint.x = fX;
	m_vecTempPoint[nIndex].fptPoint.y = fY;

	m_vecTempPoint[nIndex].strName = strName;
	m_vecTempPoint[nIndex].fptPoint2.x = 0;
	m_vecTempPoint[nIndex].fptPoint2.y = 0;


	return RESULT_GOOD;
}

UINT32 CDrawEdit_Temp::SetTempPoints(IN const int &nIndex, IN const CString &strName, IN const float &fX, IN const float &fY)
{
	int nSize = static_cast<int>(m_vecTempPoint.size());
	if (nIndex < 0) return RESULT_BAD;
	if (nIndex >= nSize) return RESULT_BAD;

	m_vecTempPoint[nIndex].strName = strName;
	m_vecTempPoint[nIndex].fptPoint2.x = fX;
	m_vecTempPoint[nIndex].fptPoint2.y = fY;
	m_vecTempPoint[nIndex].bosoc = false;
	m_vecTempPoint[nIndex].bCw = false;


	return RESULT_GOOD;
}



vector<TempPoint> CDrawEdit_Temp::GetTempPoint()
{
	return m_vecTempPoint;
}

TempPoint CDrawEdit_Temp::GetTempPoint(IN const int &nIndex)
{
	TempPoint stTempPoint;

	int nSize = static_cast<int>(m_vecTempPoint.size());
	if (nIndex < 0) return stTempPoint;
	if (nIndex >= nSize) return stTempPoint;

	return m_vecTempPoint[nIndex];
}

UINT32 CDrawEdit_Temp::_ManualMove_Feature(PointDXY point)
{
	if (m_pDrawManager == nullptr) return RESULT_BAD;
// 	if (m_pDrawManager->GetEditMode() != EditMode::enumMode_EditMove &&
// 		m_pDrawManager->GetEditMode() != EditMode::enumMode_EditCopy)
// 		return RESULT_BAD;

	D2D1_POINT_2F fptMove = D2D1::Point2F(static_cast<float>(point.x), static_cast<float>(point.y));

	D2D1::Matrix3x2F matScale = m_ViewScale;
	matScale.dx = 0;
	matScale.dy = 0;

	D2D1_POINT_2F fptMove_mm = m_pDrawFunction->DetansformPoint(fptMove, matScale, m_pUserSetInfo);

	

	_Move_Feature(m_vecFeature, fptMove);




	

	return RESULT_GOOD;
}

UINT32 CDrawEdit_Temp::_SetPitch_Feateure(PointDXY point, int nRepeatX, int nRepeatY)
{
	if (m_pDrawManager == nullptr) return RESULT_BAD;
	if (m_pDrawManager->GetEditMode() != EditMode::enumMode_EditCopy)	return RESULT_BAD;

	D2D1_POINT_2F fptPitch = D2D1::Point2F(static_cast<float>(point.x), static_cast<float>(point.y));

	/*D2D1::Matrix3x2F matScale = m_ViewScale;
	matScale.dx = 0;
	matScale.dy = 0;
	D2D1_POINT_2F fptPitch_mm = m_pDrawFunction->DetansformPoint(fptPitch, matScale);
	*/

	_Pitch_Feature(m_vecFeature, fptPitch);

	return RESULT_GOOD;

}

UINT32 CDrawEdit_Temp::_Pitch_Feature(vector<CFeatureData *> &vecFeature, D2D1_POINT_2F fptPoint)
{
	int nFeatureCount = static_cast<int>(vecFeature.size());

	for (int i = 0; i < nFeatureCount; i++)
	{
		CFeature *pFeature = vecFeature[i]->m_pFeature;
		if (pFeature == nullptr) continue;

		{
			switch (pFeature->m_eType)
			{
			case FeatureType::L:		//Line	
			{
				{
					CFeatureL *pFeatureL = (CFeatureL *)pFeature;
					pFeatureL->m_dXs += fptPoint.x;
					pFeatureL->m_dXe += fptPoint.x;
					pFeatureL->m_dYs += fptPoint.y;
					pFeatureL->m_dYe += fptPoint.y;

				}

			}
			break;
			case FeatureType::P:		//Pad	
			{
				CFeatureP *pFeatureP = (CFeatureP *)pFeature;
				pFeatureP->m_dX += fptPoint.x;
				pFeatureP->m_dY += fptPoint.y;
			}
			break;
			case FeatureType::A:		//Arc	
			{
				CFeatureA *pFeatureA = (CFeatureA *)pFeature;
				pFeatureA->m_dXs += fptPoint.x;
				pFeatureA->m_dYs += fptPoint.y;
				pFeatureA->m_dXe += fptPoint.x;
				pFeatureA->m_dYe += fptPoint.y;
				pFeatureA->m_dXc += fptPoint.x;
				pFeatureA->m_dYc += fptPoint.y;
			}
			break;
			case FeatureType::T:		//Text	
			{
				CFeatureT *pFeatureT = (CFeatureT *)pFeature;
			}
			break;
			case FeatureType::S:		//Surface	
			{
				CFeatureS *pFeatureS = (CFeatureS *)pFeature;

				int nObOeCount = static_cast<int>(pFeatureS->m_arrObOe.GetCount());
				for (int i = 0; i < nObOeCount; i++)
				{
					CObOe *pObOe = pFeatureS->m_arrObOe.GetAt(i);
					if (pObOe == nullptr) continue;

					if (pObOe->m_bHighlight == true)
					{
						int nOsOcCount = static_cast<int>(pObOe->m_arrOsOc.GetCount());
						for (int j = 0; j < nOsOcCount; j++)
						{
							COsOc *pOsOc = pObOe->m_arrOsOc.GetAt(j);
							if (pOsOc == nullptr) continue;

							pOsOc->m_dX += fptPoint.x;
							pOsOc->m_dY += fptPoint.y;

							if (pOsOc->m_bOsOc == false)
							{
								((COc*)pOsOc)->m_dXc += fptPoint.x;
								((COc*)pOsOc)->m_dYc += fptPoint.y;
							}

						}

						pObOe->m_dXbs += fptPoint.x;
						pObOe->m_dYbs += fptPoint.y;
					}
				}
			}
			break;
			}
		}

	}
	return RESULT_GOOD;
}











