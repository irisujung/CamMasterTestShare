#pragma once

#include <vector>
using namespace std;

#include "../iODB/iODB.h"
#include "CDrawFunction.h"
#include "DxDraw/CBackBuffer.h"

// typedef struct _SubStep_BackBuffer_Unit
// {	
// 	CString			strStepName;
// 	CBackBuffer*	pBackBuffer;
// 
// 	_SubStep_BackBuffer_Unit()
// 	{
// 		pBackBuffer = nullptr;
// 		_Reset();
// 	}
// 
// 	void _Reset()
// 	{
// 		strStepName = _T("");
// 		if (pBackBuffer != nullptr)
// 		{
// 			delete pBackBuffer;
// 			pBackBuffer = nullptr;
// 		}
// 	}
// 
// 	BOOL SetBackBuffer(IN const CString &in_strStepName)
// 	{
// 		CString strStepName = in_strStepName;
// 		if (pBackBuffer != nullptr)
// 		{
// 			delete pBackBuffer;
// 			pBackBuffer = nullptr;
// 		}
// 		pBackBuffer = new CBackBuffer();
// 		if (pBackBuffer == nullptr)
// 		{
// 			return FALSE;
// 		}
// 
// 		return TRUE;
// 	}
// 
// }SUB_BACKFUFFER_UNIT;
// 
// typedef struct _SubStep_BackBuffer_Strip
// {
// 	CString				strStepName;
// 	CBackBuffer			*pBackbuffer_Strip;
// 
// 
// 	vector<SUB_BACKFUFFER_UNIT> vecBackbuffer_Unit;
// 
// 	_SubStep_BackBuffer_Strip()
// 	{
// 		pBackbuffer_Strip = nullptr;
// 		_Reset();
// 	}
// 
// 	void _Reset()
// 	{
// 		strStepName = _T("");
// 
// 		int nSize = static_cast<int>(vecBackbuffer_Unit.size());
// 		for (int i = 0; i < nSize; i++)
// 		{
// 			vecBackbuffer_Unit[i]._Reset();
// 		}
// 		vecBackbuffer_Unit.clear();
// 
// 		if (pBackbuffer_Strip != nullptr)
// 		{
// 			delete pBackbuffer_Strip;
// 			pBackbuffer_Strip = nullptr;
// 		}
// 	}
// 
// 	BOOL SetBackBuffer(IN const CString &in_strStepName)
// 	{
// 		strStepName = in_strStepName;
// 		if (pBackbuffer_Strip != nullptr)
// 		{
// 			delete pBackbuffer_Strip;
// 			pBackbuffer_Strip = nullptr;
// 		}
// 		pBackbuffer_Strip = new CBackBuffer();
// 		if (pBackbuffer_Strip == nullptr)
// 		{
// 			return FALSE;
// 		}
// 
// 		return TRUE;
// 	}
// 
// 	BOOL AddSubStep(IN const int &in_nSubIndex, IN const CString &in_strName)
// 	{
// 		int nSubSize = static_cast<int>(vecBackbuffer_Unit.size());
// 
// 		BOOL bMake = FALSE;
// 		if (nSubSize <= in_nSubIndex) bMake = TRUE;
// 
// 		SUB_BACKFUFFER_UNIT stTempUnit;
// 		if (bMake == TRUE)
// 		{
// 			vecBackbuffer_Unit.push_back(stTempUnit);
// 		}
// 		
// 		if (vecBackbuffer_Unit[nSubSize].SetBackBuffer(in_strName) == FALSE)
// 		{
// 			return FALSE;
// 		}
// 
// 		return TRUE;
// 	}
// 
// }SUB_BACKFUFFER_STRIP;

typedef struct _DrawIndexInfo
{
	int nSubIndex;
	int nRepeatX;
	int nRepeatY;

	_DrawIndexInfo()
	{
		Reset();
	}
	void  Reset()
	{
		nSubIndex = -1;
		nRepeatX = -1;
		nRepeatY = -1;
	}

}DrawIndexInfo;

typedef struct _SubStep_BackBuffer
{

	CString				strStepName = _T("");;
	CBackBuffer			*pBackbuffer = nullptr;

	vector<_SubStep_BackBuffer> vecSubStep;

	_SubStep_BackBuffer()
	{
		pBackbuffer = nullptr;
		_Reset();
	}


	void _Reset()
	{
		strStepName = _T("");

		int nSize = static_cast<int>(vecSubStep.size());
		for (int i = 0; i < nSize; i++)
		{
			vecSubStep[i]._Reset();
		}
		vecSubStep.clear();

		if (pBackbuffer != nullptr)
		{
			delete pBackbuffer;
			pBackbuffer = nullptr;
		}
	}

	BOOL SetBackBuffer(IN const CString &in_strStepName)
	{
		strStepName = in_strStepName;
		if (pBackbuffer != nullptr)
		{
			delete pBackbuffer;
			pBackbuffer = nullptr;
		}
		pBackbuffer = new CBackBuffer();
		if (pBackbuffer == nullptr)
		{
			return FALSE;
		}

		return TRUE;
	}

	BOOL AddSubStep(IN const int &in_nSubIndex, IN const CString &in_strName)
	{
		int nSubSize = static_cast<int>(vecSubStep.size());

		BOOL bMake = FALSE;
		if (nSubSize <= in_nSubIndex) bMake = TRUE;

		_SubStep_BackBuffer stTempUnit;
		if (bMake == TRUE)
		{
			vecSubStep.push_back(stTempUnit);
		}

		if (vecSubStep[nSubSize].SetBackBuffer(in_strName) == FALSE)
		{
			return FALSE;
		}

		return TRUE;
	}

}SUBSTEP_BACKBUFFER;

typedef struct _SubStepInfo
{
	CString				strStepName;
	double				dAngle;
	bool				bMirror;

	_SubStepInfo()
	{
		Reset();
	}
	

	_SubStepInfo(CString in_strStepName, double in_dAngle, bool in_bMirror)
		: strStepName(in_strStepName), dAngle(in_dAngle), bMirror(in_bMirror) {};

	void Reset()
	{
		strStepName = _T("");
		dAngle = 0;
		bMirror = FALSE;
	}

	BOOL operator==(const _SubStepInfo &stOther) const
	{
		if (strStepName == _T("") ) return FALSE;
		if (strStepName != stOther.strStepName) return FALSE;
		if (dAngle != stOther.dAngle) return FALSE;
		if (bMirror != stOther.bMirror) return FALSE;

		return TRUE;
	}

}SUBSTEP_INFO;

typedef struct _DrawBackBuffer
{
	SUBSTEP_INFO		stSubStepInfo;
	CBackBuffer			*pBackbuffer;

	_DrawBackBuffer()
	{
		pBackbuffer = nullptr;
		_Reset();
	}
	~_DrawBackBuffer()
	{
		_Reset();
	}

	void _Reset()
	{
		stSubStepInfo.Reset();

		if (pBackbuffer != nullptr)
		{
			delete pBackbuffer;
			pBackbuffer = nullptr;
		}
	}

	BOOL SetBackBuffer(IN const SUBSTEP_INFO &in_StepInfo)
	{
		stSubStepInfo = in_StepInfo;
		if (pBackbuffer != nullptr)
		{
			delete pBackbuffer;
			pBackbuffer = nullptr;
		}
		pBackbuffer = new CBackBuffer();
		if (pBackbuffer == nullptr)
		{
			return FALSE;
		}

		return TRUE;
	}
}DRAW_BACKBUFFER;

class __declspec(dllexport) CDrawLayer
{
	

public:
	CDrawLayer();
	~CDrawLayer();

	UINT32 SetLink(IN ID2D1Factory1 *pFactory, IN ID2D1DeviceContext* pD2DContext, IN SystemSpec_t *pSystemSpec);
	
	UINT32 OnRender(IN ID2D1BitmapRenderTarget* pRender, IN CJobFile *pJobFile, const int &nStepIndex, IN const LayerSet_t* pLayerSet,
		IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo, IN const BOOL &bEditMode);

	UINT32 OnRender_SubStep(IN ID2D1BitmapRenderTarget* pRender, IN CJobFile *pJobFile, const int &nStepIndex, IN const LayerSet_t* pLayerSet,
		IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo, IN const BOOL &bEditMode);

	UINT32 SetDrawOnly_MeasureUnit(IN const BOOL &bMeaureUnit, vector<MeasureUnit>& vecMeasureUnitInfo);

	UINT32 ResetView();
private:
	UINT32 _Render_SubStep(IN ID2D1RenderTarget* pRender, CSubStep* pSubStep, IN const int &nSubIndex, 
		IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
		CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo, IN const BOOL &bEditMode, IN int &nSubLevel, SUBSTEP_BACKBUFFER *pSubStep_BackBuffer);

	

	BOOL _CheckBackBufferUpdate(IN const CStep *pStep);
	BOOL _CheckBackBufferCount(CSubStep *pSubStep, IN const int &nSubIndex, SUBSTEP_BACKBUFFER *pSubStep_BackBuffer);
	BOOL _CheckBackBufferValid(SUBSTEP_BACKBUFFER *pSubStep_BackBuffer);
	UINT32 _ClearBackBuffer();
	UINT32 _MakeBackBuffer(IN const CStep *pStep);
	UINT32 _MakeBackBuffer_SubStep(IN const CSubStep *pSubStep, SUBSTEP_BACKBUFFER *pSubStep_BackBuffer);
	UINT32 _DrawBackBuffer_SubStep(IN CSubStep *pSubStep, IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
		CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo, SUBSTEP_BACKBUFFER *pSubStep_BackBuffer);
	
	//Back Buffer V2 준비중
	BOOL					_IsNeedMaskBackBuffer(IN const vector<SUBSTEP_INFO> &vecSubStepInfo);
	vector<SUBSTEP_INFO>	_GetSubStepInfo(IN CStep *pStep);
	UINT32					_MakeBackBuffer_V2(IN const vector<SUBSTEP_INFO> &vecSubStepInfo);
	UINT32					_DrawBackBuffer_SubStep_V2(IN CJobFile *pJobFile, IN const LayerSet_t* pLayerSet,
		CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,IN const UserSetInfo_t *pUserSetInfo, DRAW_BACKBUFFER *pBackBuffer);
	int						_GetBackBufferIndex(IN const SUBSTEP_INFO &stSubStepInfo, IN const vector<DRAW_BACKBUFFER> &vecBackBuffer);

	vector<SUBSTEP_INFO>	_GetSubStepInfo_Step(IN CStep *pStep);
	vector<SUBSTEP_INFO>	_GetSubStepInfo_SubStep(IN CSubStep *pSubStep, IN SUBSTEP_INFO &in_stSubStepInfo);

	UINT32 _Render_SubStep_V2(IN ID2D1RenderTarget* pRender, CSubStep* pSubStep, IN const int &nSubIndex,
		IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
		CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo, IN const BOOL &bEditMode, 
		IN int nSubLevel = -1, IN int nParent_StepX = -1, IN int nParent_StepY = -1, IN int nParentSubStepIndex = -1);
	//

private:
	
	CDrawFunction				*m_pDrawFunction = nullptr;
	
	//vector<SUB_BACKFUFFER_STRIP>		m_vecBackBufferStrip;
	vector<SUBSTEP_BACKBUFFER>		m_vecSubStep_BackBuffer;

	vector<DRAW_BACKBUFFER>		m_vecDrawBackBuffer;

	CString						m_strLayerName = _T("");
	bool						m_bDrawSurface = false;
	int							m_nCurStepIndex = -1;
	eDrawLevel					m_eDrawLv = eDrawLevel::DrawLevelNone;

	BOOL					m_bMeasureUnitOnly = FALSE;//FALSE = 전체 그리기, TRUE = 측정유닛만 그린다.
	vector<MeasureUnit>		m_vecMeasureUnitInfo;

	//외부 ptr
	//지우지 말것
	ID2D1Factory1 *m_pFactory = nullptr;
	ID2D1DeviceContext* m_pD2DContext = nullptr;
	ViewInfo_t* m_pViewInfo = nullptr;
	SystemSpec_t			*m_pSystemSpec = nullptr;


	
};

