#pragma once

#include <limits>

#include <vector>
using namespace std;

#include "../../iODB/iODB.h"
#include "../StringUtil.h"



#define CONV_FLOAT_POINT(X) (static_cast<float>(X))
#define ToRadian( degree )  ( (degree) * (PI / 180.0f) )
#define ToDegree( radian )  ( (radian) * (180.0f / PI) )

#define DoubleToFloat( dValue ) ( static_cast<float>(dValue) )
#define FloatToDouble( fValue ) ( static_cast<double>(fValue) )

template<class Interface>
inline void SafeRelease(Interface **ppInterfaceToRelease) {
	if (*ppInterfaceToRelease != nullptr) {
		(*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = nullptr;
	}
}

namespace OdbPointType
{
	enum {
		enumBase = 0,
		enumLine,
		enumArc,

		enumMax,
	};
}

typedef struct _GeoDraw
{
	int nType;//
	D2D1_POINT_2F ptPoint;

	//nType이 Arc일때만 사용
	BOOL bCw;//FALSE : CCW, TRUE : CW
	D2D1_POINT_2F ptCenter;//Arc의 Center

	_GeoDraw()
	{
		_Reset();
	}

	void _Reset()
	{
		nType = OdbPointType::enumMax;
		ptPoint = D2D1::Point2F(0.f, 0.f);
		bCw = TRUE;
		ptCenter = D2D1::Point2F(0.f, 0.f);
	}

	void SetBasePoint(IN const double &in_dPointX, IN const double &in_dPointY) {
		nType = OdbPointType::enumBase;
		ptPoint.x = CONV_FLOAT_POINT(in_dPointX);
		ptPoint.y = CONV_FLOAT_POINT(in_dPointY);
	}

	void SetLine(IN const double &in_dPointX, IN const double &in_dPointY) {
		nType = OdbPointType::enumLine;
		ptPoint.x = CONV_FLOAT_POINT(in_dPointX);
		ptPoint.y = CONV_FLOAT_POINT(in_dPointY);
	}

	void SetArc(IN const double &in_dPointX, IN const double &in_dPointY, IN const BOOL &in_bCW,
		IN const double &in_dCenterX, IN const double &in_dCenterY) {
		nType = OdbPointType::enumArc;
		ptPoint.x = CONV_FLOAT_POINT(in_dPointX);
		ptPoint.y = CONV_FLOAT_POINT(in_dPointY);

		bCw = in_bCW;
		ptCenter.x = CONV_FLOAT_POINT(in_dCenterX);
		ptCenter.y = CONV_FLOAT_POINT(in_dCenterY);
	}

	_GeoDraw& operator=(const _GeoDraw& m)
	{
		if (this != &m)
		{
			this->nType = m.nType;
			this->ptPoint = m.ptPoint;

			this->bCw = m.bCw;
			this->ptCenter = m.ptCenter;
		}
		return *this;
	}

}GEODRAW;

namespace DEF_HISTOGRAM
{
	enum
	{
		Type_Feature = 0,
		Type_Attribute,
		Type_Symbol,

		TypeMax,
	};

	const CString strName[TypeMax] = {
		_T("Feature Histogram"),
		_T("Attribute Histogram"),
		_T("Symbol Histogram"),
	};

	const CString strAttNull = _T("Null_Attribute");
	const CString strSymNull = _T("Null_Symbol");
}

typedef struct _FeatureInfo
{
	int nHistoSelectType;
	CString strLayerName;
	
	enum FeatureType eType;
	CString strFeatureName;

	vector<CString> vecAttString;
	CString strSymbolName;

	_FeatureInfo()
	{
		_Reset();
	}

	void _Reset()
	{
		nHistoSelectType = DEF_HISTOGRAM::TypeMax;

		strLayerName = _T("");

		eType = FeatureType::FeatureTypeNone;
		strFeatureName = _T("");

		vecAttString.clear();
		strSymbolName = _T("");
	}

	BOOL Compare_AttString(CTypedPtrArray <CObArray, CAttributeTextString*> &arrAttString)
	{
		BOOL bSame = FALSE;

		int nMySize = static_cast<int>(vecAttString.size());
		int nDstSize = static_cast<int>(arrAttString.GetCount());

		if (nDstSize == 0 && nMySize == 1 &&
			vecAttString[0] == DEF_HISTOGRAM::strAttNull)
		{
			return TRUE;
		}

		if (nMySize != nDstSize) return FALSE;

		BOOL bTempSame = FALSE;
		for (int i = 0; i < nMySize; i++)
		{
			bTempSame = FALSE;
			for (int j = 0; j < nDstSize; j++)
			{
				CAttributeTextString *pAttString = arrAttString.GetAt(j);
				if (pAttString == nullptr) continue;

				if (vecAttString[i] == pAttString->m_strText)
				{
					bTempSame = TRUE;
				}
			}

			if (bTempSame == FALSE)
			{//하나라도 다른게 있으면 일치 하지 않음
				return FALSE;
			}
		}

		return TRUE;
	}

}FEATURE_INFO;

typedef struct _SelectInfo_Unit
{
	CString strLayer;//선택된 Feature가 속해있는 Layer 이름
	int nFeatureIndex;//
	enum FeatureType emFeatureType;
	double dCx;//Feature 중심점
	double dCy;//Feature 중심점
	double dWidth;
	double dHeight;
	CString strSymbolName;
	BOOL bPolarity;
	vector<CString> vecAttTextString;
	
	CFeature *pFeature;//외부 Pointer//Pointer Delete 하지 말것
	CRotMirrArr arrRotMirr;

	//Surface Only
	double dDx;
	double dDy;
	int nIslandCount;
	int nHoleCount;

	//Surface & ObOe Only
	int nObOeIndex;

	_SelectInfo_Unit()
	{
		_Reset();
	}

	_SelectInfo_Unit(const _SelectInfo_Unit &other)
	{
		*this = other;
	}

	void _Reset()
	{
		strLayer = _T("");
		nFeatureIndex = 0;
		emFeatureType = FeatureType::FeatureTypeNone;

		dCx = 0.0;
		dCy = 0.0;

		strSymbolName = _T("");
		bPolarity = FALSE;

		vecAttTextString.clear();

		dDx = 0.0;
		dDy = 0.0;
		nIslandCount = 0;
		nHoleCount = 0;

		nObOeIndex = -1;

		pFeature = nullptr;
		arrRotMirr.ClearAll();
	}

	CString GetFeatureTypeName()
	{
		CString strTypeName = _T("");
		switch (emFeatureType)
		{
		case FeatureType::L:
			strTypeName = _T("Line");
			break;
		case FeatureType::P:
			strTypeName = _T("Pad");
			break;
		case FeatureType::A:
			strTypeName = _T("Arc");
			break;
		case FeatureType::T:
			strTypeName = _T("Text");
			break;
		case FeatureType::S:
			strTypeName = _T("Surface");
			break;
		case FeatureType::Z:
			strTypeName = _T("Tp");
			break;
		default:
			strTypeName = _T("");
			break;
		}

		return strTypeName;
	}

	CString GetPolarityString()
	{
		CString strPol = _T("POS");
		if (bPolarity == FALSE) strPol = _T("NEG");
		return strPol;
	}

	CString GetSurfaceTypeString()
	{
		CString strPol = _T("Island");
		if (bPolarity == FALSE) strPol = _T("Hole");
		return strPol;
	}

	CString GetAttTextString()
	{
		CString strTextString = _T("");
		int nCount = static_cast<int>(vecAttTextString.size());
		for (int i = 0; i < nCount; i++)
		{
			strTextString += vecAttTextString[i];
			if (i != (nCount - 1))
			{
				strTextString += _T(",");
			}
		}

		return strTextString;
	}

	CString GetDataString()
	{
		CString strData = _T("");
		
		if (emFeatureType == FeatureType::S)
		{
			if (nObOeIndex == -1)
			{
				strData.Format(_T("%s, #%d, %s, TextString=%s,CX=%.6lf, CY=%.6lf, DX=%.6lf, DY=%.6lf, Island=%d, Holes=%d, %s"),
					strLayer, nFeatureIndex + 1, GetFeatureTypeName(), GetAttTextString(), dCx, dCy, dDx, dDy, nIslandCount, nHoleCount, GetPolarityString());
			}
			else
			{//ObOe 선택
				strData.Format(_T("%s, #%d, %s, CX=%.6lf, CY=%.6lf, DX=%.6lf, DY=%.6lf, %s"),
					strLayer, nFeatureIndex + 1, GetFeatureTypeName(), dCx, dCy, dDx, dDy, GetSurfaceTypeString());
			}
		}
		else
		{
			strData.Format(_T("%s, #%d, %s, TextString=%s, CX=%.6lf, CY=%.6lf, %s, %s"),
				strLayer, nFeatureIndex + 1, GetFeatureTypeName(), GetAttTextString(), dCx, dCy, strSymbolName, GetPolarityString());
		}
		return strData;
	}

	BOOL operator==(const _SelectInfo_Unit &stOther) const
	{
		if( strLayer != stOther.strLayer) return FALSE;
		if( nFeatureIndex != stOther.nFeatureIndex) return FALSE;

		if (emFeatureType != stOther.emFeatureType) return FALSE;
		if (dCx != stOther.dCx) return FALSE;
		if (dCy != stOther.dCy) return FALSE;
		if (strSymbolName != stOther.strSymbolName) return FALSE;
		if (bPolarity != stOther.bPolarity) return FALSE;
		
		if (emFeatureType == FeatureType::S)
		{
			if (dDx != stOther.dDx) return FALSE;
			if (dDy != stOther.dDy) return FALSE;
			if (nIslandCount != stOther.nIslandCount) return FALSE;
			if (nHoleCount != stOther.nHoleCount) return FALSE;

			if (nObOeIndex != stOther.nObOeIndex) return FALSE;
			
		}

		return TRUE;
	}

	_SelectInfo_Unit& operator=(const _SelectInfo_Unit &stOther) 
	{
		if (this != &stOther)
		{
			this->strLayer = stOther.strLayer;
			this->nFeatureIndex = stOther.nFeatureIndex;
			this->emFeatureType = stOther.emFeatureType;
			this->dCx = stOther.dCx;
			this->dCy = stOther.dCy;
			this->dWidth = stOther.dWidth;
			this->dHeight = stOther.dHeight;
			this->strSymbolName = stOther.strSymbolName;
			this->bPolarity = stOther.bPolarity;
			this->vecAttTextString = stOther.vecAttTextString;

			this->pFeature = stOther.pFeature;
			this->arrRotMirr = stOther.arrRotMirr;

			//Surface Only
			this->dDx = stOther.dDx;
			this->dDy = stOther.dDy;
			this->nIslandCount = stOther.nIslandCount;
			this->nHoleCount = stOther.nHoleCount;

			//Surface & ObOe Only
			this->nObOeIndex = stOther.nObOeIndex;
		}
		return *this;
	}

}SELECT_INFO_UNIT;

typedef struct _SelectInfo
{
	vector<SELECT_INFO_UNIT> vecSelectInfoUnit;

	_SelectInfo()
	{
		_Reset();
	}

	void _Reset()
	{
		vecSelectInfoUnit.clear();
	}

	int GetCount()
	{
		return static_cast<int>(vecSelectInfoUnit.size());
	}

	CString GetDataString()
	{
		CString strData = _T("Select Count = 0");
		int nDataCount = static_cast<int>(vecSelectInfoUnit.size());

		if (nDataCount > 0)
		{
			strData.Format(_T("Select Count = %d, %s"), nDataCount, vecSelectInfoUnit[nDataCount - 1].GetDataString());
		}

		return strData;
	}

}SELECT_INFO;


typedef struct _MeasureUnit
{
	int nSubIndex;
	int nStripX;
	int nStripY;

	int nSubSubIndex;
	int nUnitX;
	int nUnitY;

	_MeasureUnit()
	{
		_Reset();
	}

	void _Reset()
	{
		nSubIndex = 0;

		nStripX = -1;
		nStripY = -1;

		nSubSubIndex = -1;
		nUnitX = -1;
		nUnitY = -1;
	}

	BOOL operator==(const _MeasureUnit &stOther) const
	{
		if (nSubIndex == stOther.nSubIndex &&
			nStripX == stOther.nStripX &&
			nStripY == stOther.nStripY &&
			nSubSubIndex == stOther.nSubSubIndex &&
			nUnitX == stOther.nUnitX &&
			nUnitY == stOther.nUnitY)
		{
			return TRUE;
		}

		return FALSE;
	}


}MeasureUnit;

typedef struct _MeasureStrip
{
	int nSubIndex;
	int nStripX;
	int nStripY;

	_MeasureStrip()
	{
		_Reset();
	}

	void _Reset()
	{
		nSubIndex = 0;
		nStripX = -1;
		nStripY = -1;
	}

	BOOL operator==(const _MeasureStrip &stOther) const
	{
		if (nSubIndex == stOther.nSubIndex &&
			nStripX == stOther.nStripX &&
			nStripY == stOther.nStripY)
		{
			return TRUE;
		}

		return FALSE;
	}


}MeasureStrip;

typedef struct _DrawFeatureGeo
{
	bool bIsUsed = true;
	bool bPolarity = true;
	ID2D1PathGeometry *pGeo = nullptr;

	_DrawFeatureGeo()
	{
		Clear();
	}
	~_DrawFeatureGeo()
	{
		Clear();
	}

	void Clear()
	{
		if (pGeo != nullptr)
		{
			pGeo->Release();
			pGeo = nullptr;
		}
	}

	_DrawFeatureGeo& operator=(const _DrawFeatureGeo& m)
	{
		if (this != &m)
		{
			this->bIsUsed = m.bIsUsed;
			this->bPolarity = m.bPolarity;
		}
		return *this;
	}

}DrawFeatureGeo;

typedef struct _MakeFeatureData
{
	bool bIsUsed = true;
	bool bPolarity = true;
	vector<GEODRAW> vecPointData;

	_MakeFeatureData()
	{
		Clear();
	}
	~_MakeFeatureData()
	{
		Clear();
	}

	void Clear()
	{
		vecPointData.clear();
	}

	_MakeFeatureData& operator=(const _MakeFeatureData& m)
	{
		if (this != &m)
		{
			this->bIsUsed = m.bIsUsed;
			this->bPolarity = m.bPolarity;
			this->vecPointData = m.vecPointData;
		}
		return *this;
	}

}MakeFeatureData;


namespace EditMode
{
	enum {
		enumMode_None = 0,
		enumMode_EditMode,
		enumMode_EditDelete,
		enumMode_EditMove,
		enumMode_EditResize,
		enumMode_EditCopy,
		enumMode_EditRotate,
				
		enumMode_EditAdd,

		enumMode_EditTP,
		enumMode_EditTP_Center,

		enumMode_EditMask,
		enumMode_EditMask_Add_FeatureBase,
// 		enumMode_EditMask_Add,
// 		enumMode_EditMask_Resize,
// 		enumMode_EditMask_Move,
// 		enumMode_EditMask_Delete,


		enumMode_Max,
	};
}

namespace Edit_MoveAxis
{
	enum
	{
		enumMoveAxis_None = 0,
		enumMoveAxis_X,
		enumMoveAxis_Y,
		enumMoveAxis_XY,

		enumMoveAxis_Max,
	};
	
}

namespace Edit_Add_Shape
{
	enum 
	{
		enumAddShape_Line = 0,
		enumAddShape_Circle,
		enumAddShape_Rect,

		enumAddShape_Surface,

		enumAddShape_Max,
	};
}

enum class SaveMode : UINT8
{
	SaveMode_All = 0,
	SaveMode_Line,
	SaveMode_Arc,
	SaveMode_LineArc,
	SaveMode_Pad,
	SaveMode_Tp,
	SaveMode_Surface_Pos,
	SaveMode_Surface_Neg,
	SaveMode_Mask,

	SaveMode_Line_Pos_Sub,//Line feature의 크기별로 저장
	SaveMode_Line_Neg_Sub,
	SaveMode_Arc_Pos_Sub,
	SaveMode_Arc_Neg_Sub,
	SaveMode_LineArc_Pos_Sub,
	SaveMode_LineArc_Neg_Sub,

	SaveMode_Max,
};

namespace SaveMode_Name
{
	const CString strName[static_cast<UINT8>(SaveMode::SaveMode_Max)] = {
		_T(""),
		_T("_Line"),
		_T("_Arc"),
		_T("_LineArc"),
		_T("_Pad"),
		_T("_Surface_Pos"),
		_T("_Surface_Neg"),
		_T("_Mask"),

		//FeatureSub
		_T("_Line_Pos_Sub"),
		_T("_Line_Neg_Sub"),
		_T("_Arc_Pos_Sub"),
		_T("_Arc_Neg_Sub"),
		_T("_LineArc_Pos_Sub"),
		_T("_LineArc_Neg_Sub"),


	};
}

namespace eLineWidthOption {
	enum {
		eLinePos = 0, eLineNeg, eArcPos, eArcNeg, eLineArcPos, eLineArcNeg, LineWidthOptionNone,
	};

	const CString pOptionName[LineWidthOptionNone] = {
		_T("positive_line_width"),
		_T("negative_line_width"),
		_T("positive_arc_width"),
		_T("negative_arc_width"),
		_T("positive_linearc_width"),
		_T("negative_linearc_width"),
	};
}

typedef struct _SplitPos
{
	CPoint	ptPos;//Image Position
	CString	strPathName;
	CString	strPathName_Region;

	int		nSplitWidth_Pixel;
	int		nSplitHeight_Pixel;
}SPLIT_POS;

enum class HStrokeStyle : UINT8
{
	enumStroke_r,
	enumStroke_s,

	enumStroke_Max,
};
// 
// 
// 
// enum class MaskEditMode
// {
// 	enumMode_Free = 0,
// 	enumMode_Add,
// 	enumMode_Resize,
// 	enumMode_Move,
// 	enumMode_Delete,
// 
// 	enumMode_Max,
// };

enum class JobMode : UINT8
{
	SaveLayer = 0,
	ConcatRegion,
	GetFilteredFeature,
	GetFilteredFeature_V3,
	SaveFilteredFeature,
	
	JobMode_Max,
};

enum class OpMode : UINT8
{
	MakeLayer = 0,
	MergeSwath,
	MakeSplitLayer_Merge,
	MakeRegion,
	ConvRegion,

	OpMode_Max,
};

typedef struct _LineWidth
{
	int nStep;
	CString strLineWidth;
}LINEWIDTH;

typedef struct _DrawProfileData
{
	BOOL bInspect;
	RECTD drtRect;

	_DrawProfileData()
	{
		_Reset();
	}

	void _Reset()
	{
		bInspect = FALSE;
		drtRect.SetRectEmpty();
	}

}DrawProfileData;

namespace AddLineMode
{
	enum 
	{
		enumLineMode_X = 0,
		enumLineMode_Y,
		enumLineMode_XY,
	};
}