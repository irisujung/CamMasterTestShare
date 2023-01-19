#pragma once
#include <afxtempl.h>
#include <afx.h> 
#include <vector>
#include "../iCommonIF/CommonIF-Struct.h"
#include "../iCommonIF/CommonIF-Error.h"
#include "../iCommonIF/CommonIF-Path.h"
#include "../iCommonIF/CommonIF-Define.h"
#include "../iUtilities/iUtilities.h"
#include <mutex>
//

#include <d2d1_1.h>

#pragma comment(lib, "D2D1.lib")
//

#define MAX_PAD_USAGE_NUM			5

enum class Polarity : UINT8 { PPositive = 0, PNegative, PolarityNone = UCHAR_MAX };
enum class Mode : UINT8 { MDISABLED = 0, MDEFERRED, MIMMEDIATE, ModeNone = UCHAR_MAX };
enum class Stat : UINT8 { SRED = 0, SYELLOW, SGREEN, StatNone = UCHAR_MAX };
enum class FeatureType : UINT8 { L = 0, P, A, T, S, B,Z, FeatureTypeNone = UCHAR_MAX };
enum class RecordType : UINT8 { RC = 0, CR, SQ, CT, RecordTypeNone = UCHAR_MAX };
enum class PinType : UINT8 { PT = 0, PB, PS, PinTypeNone = UCHAR_MAX };
enum class ElecType : UINT8 { PEU = 0, PEE, PEM, ElecTypeNone = UCHAR_MAX };
enum class MountType : UINT8
{
	PMU = 0, SMT = 0, RSMT, MThrough, RMThrough,
	Pressfit = 5, Nonboard, MHole, MountTypeNone = UCHAR_MAX 
};

enum class SymbolName : UINT8
{
	r = 0, s, rect, rectxr, rectxc,
	oval = 5, di, octagon, donut_r, donut_s,
	hex_l = 10, hex_s, bfr, bfs, tri,
	oval_h = 15, thr, ths, s_ths, s_tho,
	sr_ths = 20, rc_ths, rc_tho, el, moire,
	hole = 25, null, userdefined, SymbolNameNone = UCHAR_MAX

	
};

namespace DefFeatueType
{
	const CString pName[static_cast<UINT8>(FeatureType::FeatureTypeNone)] = {
		_T("Line"),	_T("Pad"),	_T("Arc"),	_T("Text"),	_T("Surface"),	_T("Barcode")
	};
}

namespace DefSymbol
{
	const CString pName[static_cast<UINT8>(SymbolName::SymbolNameNone)] = {
		_T("r"),		_T("s"),		_T("rect"),		_T("rect"),		_T("rect"),
		_T("Oval"),		_T("di"),		_T("oct"),		_T("donut_r"),	_T("donut_s"),
		_T("hex_l"),	_T("hex_s"),	_T("bfr"),		_T("bfs"),		_T("tri"),
		_T("oval_h"),	_T("thr"),		_T("ths"),		_T("s_ths"),	_T("s_tho"),
		_T("sr_ths"),	_T("rc_ths"),	_T("rc_tho"),	_T("el"),		_T("moire"),
		_T("hole"),		_T("ext"),		_T("User Defined")

	};	

	const CString pTypeName[static_cast<UINT8>(SymbolName::SymbolNameNone)] = {
		_T("Circle"),				_T("Square"),					_T("Rectangle"),							_T("Rounded Rectangle"),	_T("Chamfered Rectangle"),
		_T("Oval"),					_T("Diamond"),					_T("Octagon"),								_T("Round Donut"),			_T("Square Donut"),
		_T("Horizontal Hexagon"),	_T("Vertical Hexagon"),			_T("Butterfly"),							_T("Square Butterfly"),		_T("Triangle"),
		_T("Half Oval"),			_T("Round Thermal(Rounded)"),	_T("Round Thermal(Squared)"),				_T("Square Thermal"),		_T("Square Thermal(Open Corners)"),
		_T("Square Round Thermal"),	_T("Rectangular Thermal"),		_T("Rectangular Thermal(Open Corners)"),	_T("Ellipse"),				_T("Moire"),
		_T("Hole"),					_T("Ext"),						_T("UserDefined")

	};

	//추가 정의 필요
	const int pParaCount[static_cast<UINT8>(SymbolName::SymbolNameNone)] = {
		1,				1,				2,				4,				4,
		2,				2,				3,				2,				2,
		3,				3,				1,				1,				2,
		2,				5,				5,				5,				5,
		5,				6,				6,				2,				6,
		1,				1,				1
	};


}

enum class AttributeName : UINT8
{
	ANSmd = 0, ANNomenclature, ANPattern_fill, ANGold_plating, ANBit,
	ANDrill = 5, ANPatch, ANOrig_surf, AttributeNameNone = UCHAR_MAX
};

enum class Orient : UINT8
{
	NoMir0Deg = 0, NoMir90Deg, NoMir180Deg, NoMir270Deg,
	Mir0Deg = 4, Mir90Deg, Mir180Deg, Mir270Deg,
	NoMirAnyDeg = 8, MirAnyDeg,
	NoMir45Deg = 10, NoMir135Deg, NoMir225Deg, NoMir315Deg,
	Mir45Deg = 14,	Mir135Deg,	 Mir225Deg,	Mir315Deg,	
	OrientNone = UCHAR_MAX
};

enum class SideType : UINT8
{
	TSignal = 0,	  TPowerGround,	TDielectric,		TMixed,	TSolderMask,
	TSolderPaste = 5, TSilkScreen,	TDrill,				TRout,	TDocument,
	TComponent = 10,  TMask,		TConductivePaste,	TMisc,	SideTypeNone = UCHAR_MAX
};

//align/Mask
enum class MaskType
{
	enumType_None = 0,
	enumType_Align,	enumType_Mask,

	enumType_Max
};

namespace DefAlignMaskHead
{
	enum {
		enumPatternStart = 0,
		enumSubStart,
		enumSubEnd,
		enumPatternEnd,

		enumMax,
	};

	const CString pStrName[enumMax] = {
		_T("PS"),
		_T("SS"),
		_T("SE"),
		_T("PE"),
	};
}

namespace DefAlignMaskPara
{
	enum {
		enumHead = 0,
		enumCenterX,
		enumCenterY,
		enumWidth,
		enumHeight,

		enumMax,
	};
}


//////////////////////////////////////////////////////////////////////////////////////////
class CAttribute : public CObject
{
public:
	CAttribute() : m_iSerialNum(0), m_strName(L"") {};
	CAttribute(UINT iNum, CString strName)
		: m_iSerialNum(iNum), m_strName(strName) {};

	CAttribute& operator=(const CAttribute& m)
	{
		if (this != &m)
		{
			this->m_iSerialNum = m.m_iSerialNum;
			this->m_strName = m.m_strName;
		}
		return *this;
	};

public:
	UINT32			m_iSerialNum;
	CString			m_strName;

protected:
	DECLARE_SERIAL(CAttribute);
};

class CAttributeTextString : public CObject
{
public:
	CAttributeTextString() : m_iSerialNum(0), m_strText(L"") {};
	CAttributeTextString(UINT iNum, CString strText)
		: m_iSerialNum(iNum), m_strText(strText) {};

	CAttributeTextString& operator=(const CAttributeTextString& m)
	{
		if (this != &m)
		{
			this->m_iSerialNum = m.m_iSerialNum;
			this->m_strText = m.m_strText;
		}
		return *this;
	};

public:
	UINT32			m_iSerialNum;
	CString			m_strText;

protected:
	DECLARE_SERIAL(CAttributeTextString);
};

class CMatrixStep : public CObject
{
public:
	CMatrixStep() : m_iCol(0), m_strDefine(L""), m_strName(L"") {};
	CMatrixStep(UINT32 col, CString def, CString name) : m_iCol(col), m_strDefine(def), m_strName(name) {};
	CMatrixStep(const CMatrixStep& ms) : m_iCol(ms.m_iCol), m_strDefine(ms.m_strDefine), m_strName(ms.m_strName) {};
	
	CMatrixStep& operator=(const CMatrixStep& ms)
	{
		if (this != &ms)
		{
			this->m_iCol = ms.m_iCol;
			this->m_strDefine = ms.m_strDefine;
			this->m_strName = ms.m_strName;
		}
		return *this;
	};

	~CMatrixStep() {};
	virtual void Serialize(CArchive& ar);

public:
	UINT32	m_iCol;
	CString m_strDefine;
	CString m_strName;

protected:
	DECLARE_SERIAL(CMatrixStep);
};

class CMatrixLayer : public CObject
{
public:
	CMatrixLayer() 
		: m_enPolarity(Polarity::PolarityNone), m_enSideType(SideType::SideTypeNone), m_iRow(0), m_color(0), 
		m_strContext(L""), m_strType(L""), m_strName(L"") {};
	CMatrixLayer(Polarity p, SideType st, UINT32 row, COLORREF col, CString context, CString type, CString name)
		: m_enPolarity(p), m_enSideType(st), m_iRow(row), m_color(col), m_strContext(context), m_strType(type), m_strName(name) {};
	CMatrixLayer(const CMatrixLayer& ml) 
		: m_enPolarity(ml.m_enPolarity), m_enSideType(ml.m_enSideType), m_iRow(ml.m_iRow), m_color(ml.m_color),
		m_strContext(ml.m_strContext), m_strType(ml.m_strType), m_strName(ml.m_strName) {};

	CMatrixLayer& operator=(const CMatrixLayer& ml)
	{
		if (this != &ml)
		{
			this->m_enPolarity = ml.m_enPolarity;
			this->m_enSideType = ml.m_enSideType;
			this->m_iRow = ml.m_iRow;
			this->m_color = ml.m_color;
			this->m_strContext = ml.m_strContext;
			this->m_strType = ml.m_strType;
			this->m_strName = ml.m_strName;
		}
		return *this;
	};

	~CMatrixLayer() {};
	virtual void Serialize(CArchive& ar);

public:
	void SetType(CString strType);
	void SetPolarity(CString strPolarity);
	void SetColor(CString strColor);

public:
	Polarity	m_enPolarity;
	SideType	m_enSideType;
	UINT32		m_iRow;
	COLORREF	m_color;
	CString		m_strContext;
	CString		m_strType;
	CString		m_strName;

protected:
	DECLARE_SERIAL(CMatrixLayer);
};

class CMatrix : public CObject
{
public:
	CMatrix() : m_bIsStepSet(false), m_iStartLayerIdx(-1), m_iEndLayerIdx(-1) {};
	CMatrix(bool b, UINT32 s, UINT32 e) : m_bIsStepSet(b), m_iStartLayerIdx(s), m_iEndLayerIdx(e) {};
	CMatrix(const CMatrix& m) : m_bIsStepSet(m.m_bIsStepSet), m_iStartLayerIdx(m.m_iStartLayerIdx), m_iEndLayerIdx(m.m_iEndLayerIdx) {};

	CMatrix& operator=(const CMatrix& m)
	{
		if (this != &m)
		{
			this->m_bIsStepSet = m.m_bIsStepSet;
			this->m_iStartLayerIdx = m.m_iStartLayerIdx;
			this->m_iEndLayerIdx = m.m_iEndLayerIdx;
		}
		return *this;
	};

	~CMatrix() { ClearMatrixSteps();	ClearMatrixLayers(); };
	virtual void Serialize(CArchive& ar);
	
public:
	bool IsStepSet() { return m_bIsStepSet; };
	void ClearMatrixSteps();
	void ClearMatrixLayers();
	UINT32 LoadMatrixFile(CString strFileName);
	UINT32 AddLayer(CString strFilePath, std::map<CString, bool>& vcLayer);

public:
	bool		m_bIsStepSet;
	INT32		m_iStartLayerIdx;
	INT32		m_iEndLayerIdx;

	CTypedPtrArray <CObArray, CMatrixStep*> m_arrMatrixStep;
	CTypedPtrArray <CObArray, CMatrixLayer*> m_arrMatrixLayer;

protected:
	DECLARE_SERIAL(CMatrix);
};

class CFontLine : public CObject
{
public:
	CFontLine() : m_bShape(false), m_bPol(false), m_dXs(0.f), m_dYs(0.f), m_dXe(0.f), m_dYe(0.f), m_dWidth(0.f){};
	CFontLine(bool b, bool p, double xs, double ys, double xe, double ye, double w) 
		: m_bShape(b), m_bPol(p), m_dXs(xs), m_dYs(ys), m_dXe(xe), m_dYe(ye), m_dWidth(w) {};
	CFontLine(const CFontLine& m) 
		:m_bShape(m.m_bShape), m_bPol(m.m_bPol), m_dXs(m.m_dXs), m_dYs(m.m_dYs), m_dXe(m.m_dXe), m_dYe(m.m_dYe), m_dWidth(m.m_dWidth) {};

	CFontLine& operator=(const CFontLine& m)
	{
		if (this != &m)
		{
			this->m_bShape = m.m_bShape;		// shape of the ends of the line (R for rounded, S for square)
			this->m_bPol = m.m_bPol;			// polairty of the line (P for positive, N for negative)
			this->m_dXs = m.m_dXs;				// definition of a line between (Xs, Ys) and (Xe, Ye)
			this->m_dYs = m.m_dYs;	
			this->m_dXe = m.m_dXe;
			this->m_dYe = m.m_dYe;
			this->m_dWidth = m.m_dWidth;		// line width, in inches
		}
		return *this;
	};

	~CFontLine() {};
	virtual void Serialize(CArchive& ar);

public:
	bool	m_bShape;		// true : round, false : square
	bool	m_bPol;
	double	m_dXs;
	double	m_dYs;
	double	m_dXe;
	double	m_dYe;
	double	m_dWidth;

protected:
	DECLARE_SERIAL(CFontLine);
};

class CFontChar : public CObject
{
public:
	CFontChar() : m_Char(0), m_MinMaxRect(0.f, 0.f, 0.f, 0.f) {};
	CFontChar(wchar_t ch, RECTD rc) : m_Char(ch), m_MinMaxRect(rc) {};
	CFontChar(wchar_t ch, double l, double t, double r, double b)
		: m_Char(ch), m_MinMaxRect(l, t, r, b) {};

	CFontChar& operator=(const CFontChar& m)
	{
		if (this != &m)
		{
			this->m_Char = m.m_Char;					//defines the ASCII character which is defined by this block
			this->m_MinMaxRect = m.m_MinMaxRect;
		}
		return *this;
	};

	~CFontChar() { ClearFontLines(); };
	void ClearFontLines();
	UINT32 LoadFontLine(CString strFontLine, bool bIsMMUnit = false);
	virtual void Serialize(CArchive& ar);

public:
	wchar_t	m_Char;
	RECTD	m_MinMaxRect;
	CTypedPtrArray<CObArray, CFontLine*> m_arrFontLine;		//the origin of each character is at the lower left corner

protected:
	DECLARE_SERIAL(CFontChar);
};

class CStandardFont : public CObject
{
public:
	CStandardFont() : m_dXSize(0.f), m_dYSize(0.f), m_dOffset(0.f), m_strFontName(L"") {};
	CStandardFont(double x, double y, double off, CString str) 
		: m_dXSize(x), m_dYSize(y), m_dOffset(off), m_strFontName(str) {};

	CStandardFont& operator=(const CStandardFont& m)
	{
		if (this != &m)
		{
			this->m_dXSize = m.m_dXSize;					//horizontal size of a character, in inches
			this->m_dYSize = m.m_dYSize;					//vertical size of a character, in inches
			this->m_dOffset = m.m_dOffset;					//horizontal distance between the end of one character block and the beginning of the next one
			this->m_strFontName = m.m_strFontName;			
		}
		return *this;
	};

	~CStandardFont() { ClearFontChars(); };
	UINT32 LoadStandardFile(CString strFileName);
	virtual void Serialize(CArchive& ar);

public:
	void ClearFontChars();

public:
	CString m_strFontName;
	double	m_dXSize;
	double	m_dYSize;
	double	m_dOffset;	
	
	CTypedPtrArray<CObArray, CFontChar*> m_arrFontChar;

protected:
	DECLARE_SERIAL(CStandardFont);
};

class CFeature : public CObject
{
public:
	CFeature() : m_bPolarity(false), m_eType(FeatureType::FeatureTypeNone),
		m_nDecode(0), m_MinMaxRect(0.f, 0.f, 0.f, 0.f), m_pSymbol(nullptr) {};
	CFeature(bool hi, bool pol, FeatureType type, UINT32 dec, RECTD rc, class CSymbol* psym)
		: m_bPolarity(pol), m_eType(type),
		m_nDecode(dec), m_MinMaxRect(rc), m_pSymbol(psym) {};

	CFeature& operator=(const CFeature& m)
	{
		if (this != &m)
		{
			this->m_bPolarity = m.m_bPolarity;
			this->m_eType = m.m_eType;
			this->m_nDecode = m.m_nDecode;
			this->m_MinMaxRect = m.m_MinMaxRect;
			this->m_pSymbol = m.m_pSymbol;
		}
		return *this;
	};

	~CFeature() {
		m_arrAttribute.RemoveAll();
		m_arrAttributeTextString.RemoveAll();
	};

public:
	virtual void SetMinMaxRect() {};
	virtual void LoadFeature(CString strOneLine, CStdioFile& cStdFile, class CFeatureFile* pFeatureFile = NULL, class CJobFile* pJobFile = NULL, bool bIsMMUnit = false) {};
	virtual void Serialize(CArchive& ar, class CFeatureFile* pFeatureFile = NULL);
	virtual void LoadFeatureAttribute(CString strAttribute, CFeatureFile* pFeatureFile, CJobFile* pJobFile);

public:
	bool				m_bPolarity;
	enum FeatureType	m_eType;
	UINT32				m_nDecode;
	RECTD				m_MinMaxRect;
	class CSymbol*		m_pSymbol;

	CTypedPtrArray <CObArray, CAttribute*> m_arrAttribute;
	CTypedPtrArray <CObArray, CAttributeTextString*> m_arrAttributeTextString;


protected:
	DECLARE_SERIAL(CFeature)
};

class CFeatureL : public CFeature
{
	// L <xs> <ys> <xe> <ye> <sym_num> <polarity> <dcode> ; <atr>[=<value>], ~
public:
	CFeatureL() : m_nSymNum(0), m_dXs(0), m_dYs(0), m_dXe(0), m_dYe(0),
		m_bHighlight(false), m_bShow(true){
		m_eType = FeatureType::L;
	};
	CFeatureL(UINT32 sym, double xs, double ys, double xe, double ye)
		: m_nSymNum(sym), m_dXs(xs), m_dYs(ys), m_dXe(xe), m_dYe(ye),
		m_bHighlight(false), m_bShow(true)
	{
		m_eType = FeatureType::L;
	};

	CFeatureL& operator=(const CFeatureL& m)
	{
		if (this != &m)
		{
			this->m_nSymNum = m.m_nSymNum;
			this->m_dXs = m.m_dXs;
			this->m_dYs = m.m_dYs;
			this->m_dXe = m.m_dXe;
			this->m_dYe = m.m_dYe;

			this->m_bHighlight = m.m_bHighlight;
			this->m_bShow = m.m_bShow;
		}
		return *this;
	};

	~CFeatureL() {
		//hj.kim
		if (m_pGeoObject != nullptr)
		{
			m_pGeoObject->Release();
			m_pGeoObject = nullptr;
		}
	};

public:
	void SetMinMaxRect();
	void LoadFeature(CString strOneLine, CStdioFile& cStdFile, class CFeatureFile* pFeatureFile = NULL, class CJobFile* pJobFile = NULL, bool bIsMMUnit = false);
	virtual void Serialize(CArchive& ar, class CFeatureFile* pFeatureFile = NULL);

public:
	UINT32		m_nSymNum;
	double		m_dXs;
	double		m_dYs;
	double		m_dXe;
	double		m_dYe;

	//hj.kim
	ID2D1PathGeometry * m_pGeoObject = nullptr;
	bool				m_bHighlight;
	bool				m_bPreHighlight = false;
	bool				m_bShow;

protected:
	DECLARE_SERIAL(CFeatureL)
};

class CFeatureZ : public CFeature
{
public:
	CFeatureZ() : m_nSymNum(0), m_dX(0.f), m_dY(0.f), m_eOrient(Orient::OrientNone),
		m_bHighlight(false), m_bShow(true)
	{
		m_eType = FeatureType::Z;
	};
	CFeatureZ(UINT32 sym, double x, double y, Orient ori)
		: m_nSymNum(sym), m_dX(x), m_dY(y), m_eOrient(ori),
		m_bHighlight(false), m_bShow(true)
	{
		m_eType = FeatureType::Z;
	};

	CFeatureZ& operator=(const CFeatureZ& m) 
	{
		if (this != &m)
		{
			this->m_nSymNum = m.m_nSymNum;
			this->m_dX = m.m_dX;
			this->m_dY = m.m_dY;
			this->m_eOrient = m.m_eOrient;

			this->m_bHighlight = m.m_bHighlight;
			this->m_bShow = m.m_bShow;
		};
		return *this;
	};
	~CFeatureZ() {
		//hj.kim
		if (m_pGeoObject != nullptr)
		{
			m_pGeoObject->Release();
			m_pGeoObject = nullptr;
		}

		if (m_pGeoObject_Hole != nullptr)
		{
			m_pGeoObject_Hole->Release();
			m_pGeoObject_Hole = nullptr;
		}

	};

public:
 	void SetMinMaxRect();
 	void LoadFeature(CString strOneLine, CStdioFile& cStdFile, class CFeatureFile* pFeatureFile = NULL, class CJobFile* pJobFile = NULL, bool bIsMMUnit = false);
 	virtual void Serialize(CArchive& ar, class CFeatureFile* pFeatureFile = NULL);

public:
	UINT32		m_nSymNum;
	double		m_dX;
	double		m_dY;
	enum Orient m_eOrient;

	//hj.kim
	ID2D1PathGeometry * m_pGeoObject = nullptr;
	ID2D1PathGeometry * m_pGeoObject_Hole = nullptr;
	bool				m_bHighlight;
	bool				m_bPreHighlight = false;//Mouse Over
	bool				m_bShow;
protected:
	DECLARE_SERIAL(CFeatureZ)
};

class CFeatureP : public CFeature
{
	// P <x> <y> <sym_num> <polarity> <decode> <orient> ; <atr>[=<value>], ~ 
public:
	CFeatureP() : m_nSymNum(0), m_dX(0.f), m_dY(0.f), m_eOrient(Orient::OrientNone),
		m_bHighlight(false), m_bShow(true)
	{
		m_eType = FeatureType::P;
	};
	CFeatureP(UINT32 sym, double x, double y, Orient ori)
		: m_nSymNum(sym), m_dX(x), m_dY(y), m_eOrient(ori),
		m_bHighlight(false), m_bShow(true) 
	{
		m_eType = FeatureType::P;
	};

	CFeatureP& operator=(const CFeatureP& m)
	{
		if (this != &m)
		{
			this->m_nSymNum = m.m_nSymNum;
			this->m_dX = m.m_dX;
			this->m_dY = m.m_dY;
			this->m_eOrient = m.m_eOrient;

			this->m_bHighlight = m.m_bHighlight;
			this->m_bShow = m.m_bShow;
		}
		return *this;
	};

	~CFeatureP() {
		//hj.kim
		if (m_pGeoObject != nullptr)
		{
			m_pGeoObject->Release();
			m_pGeoObject = nullptr;
		}

		if (m_pGeoObject_Hole != nullptr)
		{
			m_pGeoObject_Hole->Release();
			m_pGeoObject_Hole = nullptr;
		}
		
	};

public:
	void SetMinMaxRect();
	void LoadFeature(CString strOneLine, CStdioFile& cStdFile, class CFeatureFile* pFeatureFile = NULL, class CJobFile* pJobFile = NULL, bool bIsMMUnit = false);
	virtual void Serialize(CArchive& ar, class CFeatureFile* pFeatureFile = NULL);

public:
	UINT32		m_nSymNum;
	double		m_dX;
	double		m_dY;
	enum Orient m_eOrient;

	//hj.kim
	ID2D1PathGeometry * m_pGeoObject = nullptr;
	ID2D1PathGeometry * m_pGeoObject_Hole = nullptr;
	bool				m_bHighlight;
	bool				m_bPreHighlight = false;//Mouse Over
	bool				m_bShow;

protected:
	DECLARE_SERIAL(CFeatureP)
};

class CFeatureA : public CFeature
{
	// A <xs> <ys> <xe> <ye> <xc> <yc> <sym_num> <polarity> <decode> <cw> ; <atr>[=<value>], ~
	// When the start and end-point of an arc coincide, it is considered a 360 degree arc.
	// There are no single-point arcs in ODB++ format.
public:
	CFeatureA() : m_bCw(false), m_nSymNum(0), m_dXs(0.f), m_dYs(0.f), m_dXe(0.f), m_dYe(0.f), m_dXc(0.f), m_dYc(0.f),
		m_bHighlight(false), m_bShow(true)
	{
		m_eType = FeatureType::A;
	};
	CFeatureA(bool cw, UINT32 sym, double xs, double ys, double xe, double ye, double xc, double yc)
		: m_bCw(cw), m_nSymNum(sym), m_dXs(xs), m_dYs(ys), m_dXe(xe), m_dYe(ye), m_dXc(xc), m_dYc(yc),
		m_bHighlight(false), m_bShow(true) 
	{
		m_eType = FeatureType::A;
	};

	CFeatureA& operator=(const CFeatureA& m)
	{
		if (this != &m)
		{
			this->m_bCw = m.m_bCw;
			this->m_nSymNum = m.m_nSymNum;
			this->m_dXs = m.m_dXs;
			this->m_dYs = m.m_dYs;
			this->m_dXe = m.m_dXe;
			this->m_dYe = m.m_dYe;
			this->m_dXc = m.m_dXc;
			this->m_dYc = m.m_dYc;

			this->m_bHighlight = m.m_bHighlight;
			this->m_bShow = m.m_bShow;
		}
		return *this;
	};

	~CFeatureA() {
		//hj.kim
		if (m_pGeoObject != nullptr)
		{
			m_pGeoObject->Release();
			m_pGeoObject = nullptr;
		}
	};

public:
	void SetMinMaxRect();
	void LoadFeature(CString strOneLine, CStdioFile& cStdFile, class CFeatureFile* pFeatureFile = NULL, class CJobFile* pJobFile = NULL, bool bIsMMUnit = false);
	virtual void Serialize(CArchive& ar, class CFeatureFile* pFeatureFile = NULL);

public:
	bool	m_bCw;		// true : clockwise, false : counter-clockwise
	UINT32	m_nSymNum;
	double	m_dXs;
	double	m_dYs;
	double	m_dXe;
	double	m_dYe;
	double	m_dXc;
	double	m_dYc;

	//hj.kim
	ID2D1PathGeometry * m_pGeoObject = nullptr;
	bool				m_bHighlight;
	bool				m_bPreHighlight = false;
	bool				m_bShow;

protected:
	DECLARE_SERIAL(CFeatureA)
};

class CFeatureT : public CFeature
{
	// T <x> <y> <font> <polarity> <orient> <xsize> <ysize> <width factor> <text> <version> ; <atr>[=<value>], ~
public:
	CFeatureT() : m_nVersion(0), m_eOrient(Orient::OrientNone), m_dX(0.f), m_dY(0.f), m_dXSize(0.f), m_dYSize(0.f),
				m_dWidthFactor(0.f), m_strFont(L""), m_strText(L""),
				m_bHighlight(false), m_bShow(true)
	{
		m_eType = FeatureType::T;
	};
	CFeatureT(UINT32 ver, Orient ori, double dx, double dy, double xsize, double ysize, double wf, CString font, CString txt) 
		: m_nVersion(ver), m_eOrient(ori), m_dX(dx), m_dY(dy), m_dXSize(xsize), m_dYSize(ysize),
		m_dWidthFactor(wf), m_strFont(font), m_strText(txt),
		m_bHighlight(false), m_bShow(true) 
	{
		m_eType = FeatureType::T;
	};

	CFeatureT& operator=(const CFeatureT& m)
	{
		if (this != &m)
		{
			this->m_nVersion = m.m_nVersion;
			this->m_eOrient = m.m_eOrient;
			this->m_dX = m.m_dX;
			this->m_dY = m.m_dX;
			this->m_dXSize = m.m_dXSize;
			this->m_dYSize = m.m_dYSize;
			this->m_dWidthFactor = m.m_dWidthFactor;
			this->m_strFont = m.m_strFont;
			this->m_strText = m.m_strText;

			this->m_bHighlight = m.m_bHighlight;
			this->m_bShow = m.m_bShow;
		}
		return *this;
	};

	~CFeatureT()
	{
		//hj.kim
		int nSize = static_cast<int>(m_vecGeoObject.size());
		for (int i = 0; i < nSize; i++)
		{
			if (m_vecGeoObject[i] != nullptr)
			{
				m_vecGeoObject[i]->Release();
				m_vecGeoObject[i] = nullptr;
			}
		}
	};

public:
	void SetMinMaxRect();
	void LoadFeature(CString strOneLine, CStdioFile& cStdFile, class CFeatureFile* pFeatureFile = NULL, class CJobFile* pJobFile = NULL, bool bIsMMUnit = false);
	virtual void Serialize(CArchive& ar, class CFeatureFile* pFeatureFile = NULL);

public:
	UINT32		m_nVersion;		// 0: previous version, 1: current version3
	enum Orient m_eOrient;
	double		m_dX;
	double		m_dY;
	double		m_dXSize;
	double		m_dYSize;
	double		m_dWidthFactor;
	CString		m_strFont;		// usually, "standard"
	CString		m_strText;

	//hj.kim
	std::vector<ID2D1PathGeometry*> m_vecGeoObject;//Font Line 마다 Style이 다를수 있으므로 vector로 생성한다.
	bool		m_bHighlight;
	bool		m_bPreHighlight = false;
	bool		m_bShow;

protected:
	DECLARE_SERIAL(CFeatureT)
};

class CFeatureB : public CFeature
{
	// B <x> <y> <barcode> <font> <polarity> <orient> E <w> <h> <fasc> <cs> <bg> <astr> <astr_pos> <text>
	//            UPC39							   element width, barcode height
	// <fasc> : Y=full ASCII, N=partial ASCII
	// <cs>   : Y=checksum, N=no checksum
	// <bg>   : Y=inverted background, N=no background
	// <astr> : Y=addition of a text string, N=only barcode
	// <astr_pos> : T=adding the string on top, B=bottom
public:
	CFeatureB() : m_bFasc(false), m_bCs(false), m_bBg(false), m_bAstr(false), m_bAstrPos(false), m_eOrient(Orient::OrientNone),
		m_dX(0.f), m_dY(0.f), m_dW(0.f), m_dH(0.f), m_strBarcode(L""), m_strFont(L""), m_strText(L"") {
		m_eType = FeatureType::B;
	};
	CFeatureB(bool fasc, bool cs, bool bg, bool astr, bool astrpos, Orient ori, double dx, double dy, double dw, double dh,
		CString bar, CString font, CString txt)
		: m_bFasc(fasc), m_bCs(cs), m_bBg(bg), m_bAstr(astr), m_bAstrPos(astrpos), m_eOrient(ori),
		m_dX(dx), m_dY(dy), m_dW(dw), m_dH(dh), m_strBarcode(bar), m_strFont(font), m_strText(txt) {
		m_eType = FeatureType::B;
	};

	CFeatureB& operator=(const CFeatureB& m)
	{
		if (this != &m)
		{
			this->m_bFasc = m.m_bFasc;
			this->m_bCs = m.m_bCs;
			this->m_bBg = m.m_bBg;
			this->m_bAstr = m.m_bAstr;
			this->m_bAstrPos = m.m_bAstrPos;
			this->m_eOrient = m.m_eOrient;
			this->m_dX = m.m_dX;
			this->m_dY = m.m_dY;
			this->m_dW = m.m_dW;
			this->m_dH = m.m_dH;
			this->m_strBarcode = m.m_strBarcode;
			this->m_strFont = m.m_strFont;
			this->m_strText = m.m_strText;
		}
		return *this;
	};

	~CFeatureB() {};

public:
	void SetMinMaxRect();
	void LoadFeature(CString strOneLine, CStdioFile& cStdFile, class CFeatureFile* pFeatureFile = NULL, class CJobFile* pJobFile = NULL, bool bIsMMUnit = false);
	virtual void Serialize(CArchive& ar, class CFeatureFile* pFeatureFile = NULL);

public:
	bool		m_bFasc;
	bool		m_bCs;
	bool		m_bBg;
	bool		m_bAstr;
	bool		m_bAstrPos;
	enum Orient m_eOrient;
	double		m_dX;
	double		m_dY;
	double		m_dW;
	double		m_dH;
	CString		m_strBarcode;
	CString		m_strFont;	// usually, "standard"
	CString		m_strText;

protected:
	DECLARE_SERIAL(CFeatureB)
};

class COsOc : public CObject
{
public:
	COsOc() : m_bOsOc(false), m_dX(0.f), m_dY(0.f) {};
	COsOc(bool b, double x, double y) : m_bOsOc(b), m_dX(x), m_dY(y) {};

	COsOc& operator=(const COsOc& m)
	{
		if (this != &m)
		{
			this->m_bOsOc = m.m_bOsOc;
			this->m_dX = m.m_dX;
			this->m_dY = m.m_dY;
		}
		return *this;
	};

	~COsOc() {};

public:
	virtual void Serialize(CArchive& ar);

public:
	bool	m_bOsOc;	// true=Os, false=Oc
	double	m_dX;		// Os의 x, y, Oc의 xe, ye
	double	m_dY;

protected:
	DECLARE_SERIAL(COsOc)
};

class COs : public COsOc
{
public:
	COs() {};
	~COs() {};

public:
	virtual void Serialize(CArchive& ar);
	
protected:
	DECLARE_SERIAL(COs)
};

class COc : public COsOc
{
public:
	COc() : m_bCw(false), m_dXc(0.f), m_dYc(0.f) {};
	COc(bool cw, double xc, double yc) : m_bCw(cw), m_dXc(xc), m_dYc(yc) {};

	COc& operator=(const COc& m)
	{
		if (this != &m)
		{
			this->m_bCw = m.m_bCw;
			this->m_dXc = m.m_dXc;
			this->m_dYc = m.m_dYc;
		}
		return *this;
	};

	~COc() {};

public:
	virtual void Serialize(CArchive& ar);

public:
	bool	m_bCw;	// true=cw, false=ccw
	double	m_dXc;
	double	m_dYc;

protected:
	DECLARE_SERIAL(COc)
};

class CObOe : public CObject
{
public:
	CObOe() : m_bAllOs(false), m_bPolyType(false), m_dXbs(0.f), m_dYbs(0.f), m_MinMaxRect(0.f, 0.f, 0.f, 0.f),
				m_bHighlight(false), m_bShow(true)
	{};
	CObOe(bool os, bool poly, double x, double y, RECTD rc)
		: m_bAllOs(os), m_bPolyType(poly), m_dXbs(x), m_dYbs(y), m_MinMaxRect(rc),
		m_bHighlight(false), m_bShow(true) 
	{};

	CObOe& operator=(const CObOe& m)
	{
		if (this != &m)
		{
			this->m_bAllOs = m.m_bAllOs;
			this->m_bPolyType = m.m_bPolyType;
			this->m_dXbs = m.m_dXbs;
			this->m_dYbs = m.m_dYbs;
			this->m_MinMaxRect = m.m_MinMaxRect;

			this->m_bHighlight = m.m_bHighlight;
			this->m_bShow = m.m_bShow;
		}
		return *this;
	};

	~CObOe() {
		ClearOsOc();
	};

public:
	void	ClearOsOc();
	void	LoadOsOc(CString strOsOcLine, bool bIsMMUnit);
	void	SetMinMaxRect();
	void	SetMinMaxOsOc(double X1, double Y1, double X2, double Y2, double XC, double YC, bool cw, RECTD& tmpMinMax);
	virtual void Serialize(CArchive& ar);

public:
	bool	m_bAllOs;		// true=모두 OS, false=OC가 적어도 1개 이상인 경우
	bool	m_bPolyType;	// true=Island(I), false=Hole(H)
	double	m_dXbs;
	double	m_dYbs;
	RECTD	m_MinMaxRect;

	CTypedPtrArray <CObArray, COsOc*> m_arrOsOc;
	
	//hj.kim
	ID2D1PathGeometry * m_pGeoObject = nullptr;
	bool		m_bHighlight;
	bool		m_bPreHighlight = false;
	bool		m_bShow;

protected:
	DECLARE_SERIAL(CObOe)
};

class CFeatureS : public CFeature
{
	// S <params> ; <atr>=<value>...
	// <polygon1>
	// <polygon2>
	// SE
	// params section contains <polarity> <decode>
	// polygon starts with an OB command, contains OS(segment) or OC(Arc) commands, and ends with an OE command
	// OB <xbs> <ybs> <poly_type> : start point, I for island, H for hole
	// OS <x> <y> : segment end point (previous polygon point is the start point)
	// OC <xe> <ye> <xc> <yc> <cw> : curve end point (previous polygon point is the start point), curve center point, Y for clockwise, N for counter-clockwise
	// OE
public:
	CFeatureS() { m_eType = FeatureType::S; };
	~CFeatureS() { ClearFeature(); };

public:
	void ClearFeature();
	void SetMinMaxRect();
	void LoadFeature(CString strOneLine, CStdioFile& cStdFile, class CFeatureFile* pFeatureFile = NULL, class CJobFile* pJobFile = NULL, bool bIsMMUnit = false);
	void SetMinMaxOsOc(double X1, double Y1, double X2, double Y2, double XC, double YC, bool cw, RECTD& tmpMinMax);

public:
	CTypedPtrArray <CObArray, CObOe*> m_arrObOe;
	
	//hj.kim
	ID2D1PathGeometry * m_pGeoObject = nullptr;
protected:
	DECLARE_SERIAL(CFeatureS)
};

class CFeatureFile : public CObject
{
public:
	CFeatureFile() : m_MinMaxRect(0.f, 0.f, 0.f, 0.f) {};
	CFeatureFile(RECTD rc) : m_MinMaxRect(rc) {};

	CFeatureFile& operator=(const CFeatureFile& m)
	{
		if (this != &m)
		{
			this->m_MinMaxRect = m.m_MinMaxRect;
		}
		return *this;
	};

	~CFeatureFile() { ClearAll(); };

public:
	void ClearSymbols();
	void ClearFeatures();
	void ClearAttributes();
	void ClearAttributeTextStrings();
	void ClearAll();
	void SetMinMaxRect();
	UINT32 LoadFeatureFile(CString strFeatureFile, class CJobFile* pJobFile);

	UINT32 SaveFeatureFile(CString strFeatureFile);

public:
	RECTD	m_MinMaxRect;
	CTypedPtrArray <CObArray, CSymbol*> m_arrSymbol;
	CTypedPtrArray <CObArray, CFeature*> m_arrFeature;
	CTypedPtrArray <CObArray, CAttribute*> m_arrAttribute;
	CTypedPtrArray <CObArray, CAttributeTextString*> m_arrAttributeTextString;

protected:
	DECLARE_SERIAL(CFeatureFile);
};

class CUserSymbolAttrlist : public CObject
{
public:
	CUserSymbolAttrlist() : m_bOutBreak(false), m_bOutScale(false), m_bBreakAway(false), m_nEntityVersion(0),
		m_dFillDx(0.f), m_dFillDy(0.f), m_dImageDx(0.f), m_dImageDy(0.f), m_strComment(L""), m_strSourceName(L"") {};
	CUserSymbolAttrlist(bool OutBreak, bool OutScale, bool BreakAway, int entity, double fdx, double fdy,
		double idx, double idy, CString com, CString name)
		: m_bOutBreak(OutBreak), m_bOutScale(OutScale), m_bBreakAway(BreakAway), m_nEntityVersion(entity),
		m_dFillDx(fdx), m_dFillDy(fdy), m_dImageDx(idx), m_dImageDy(idy), m_strComment(com), m_strSourceName(name) {};

	CUserSymbolAttrlist& operator=(const CUserSymbolAttrlist& m)
	{
		if (this != &m)
		{
			this->m_bOutBreak = m.m_bOutBreak;
			this->m_bOutScale = m.m_bOutScale;
			this->m_bBreakAway = m.m_bBreakAway;
			this->m_nEntityVersion = m.m_nEntityVersion;
			this->m_dFillDx = m.m_dFillDx;
			this->m_dFillDy = m.m_dFillDy;
			this->m_dImageDx = m.m_dImageDx;
			this->m_dImageDy = m.m_dImageDy;
			this->m_strComment = m.m_strComment;
			this->m_strSourceName = m.m_strSourceName;
		}
		return *this;
	};

	~CUserSymbolAttrlist() {};
	virtual void Serialize(CArchive& ar);

private:
	bool		m_bOutBreak;
	bool		m_bOutScale;
	bool		m_bBreakAway;
	int			m_nEntityVersion;
	double		m_dFillDx;
	double		m_dFillDy;
	double		m_dImageDx;
	double		m_dImageDy;
	CString		m_strComment;
	CString		m_strSourceName;

protected:
	DECLARE_SERIAL(CUserSymbolAttrlist)
};

class CUserSymbol : public CObject
{
public:
	CUserSymbol() : m_strSymbolName(L"") {};
	CUserSymbol(CString sym) : m_strSymbolName(sym) {};

	CUserSymbol& operator=(const CUserSymbol& m)
	{
		if (this != &m)
		{
			this->m_strSymbolName = m.m_strSymbolName;
			this->m_UserSymbolAttrlist = m.m_UserSymbolAttrlist;
			this->m_FeatureFile = m.m_FeatureFile;
		}
		return *this;
	};

	~CUserSymbol() {};

public:
	void ClearUserSymbol() { m_FeatureFile.ClearAll(); };

public:
	CString				m_strSymbolName;
	CFeatureFile		m_FeatureFile;

private:
	CUserSymbolAttrlist	m_UserSymbolAttrlist;

protected:
	DECLARE_SERIAL(CUserSymbol)
};

class CSymbol : public CObject
{
public:
	CSymbol() : m_eSymbolName(SymbolName::SymbolNameNone), m_iSerialNum(0), m_dRotatedDeg(0.f), m_strUserSymbol(L""), m_MinMaxRect(), m_pUserSymbol(nullptr) {};
	CSymbol(SymbolName en, UINT32 s, double deg, CString sym, RECTD rc, CUserSymbol* psym)
		: m_eSymbolName(en), m_iSerialNum(s), m_dRotatedDeg(deg), m_strUserSymbol(sym), m_MinMaxRect(rc), m_pUserSymbol(psym) { };

	CSymbol& operator=(const CSymbol& m)
	{
		if (this != &m)
		{
			this->m_eSymbolName = m.m_eSymbolName;
			this->m_iSerialNum = m.m_iSerialNum;
			this->m_dRotatedDeg = m.m_dRotatedDeg;
			this->m_strUserSymbol = m.m_strUserSymbol;
			this->m_pUserSymbol = m.m_pUserSymbol;
		}
		return *this;
	};

	BOOL operator==(const CSymbol& m) const
	{
		if ( this->m_eSymbolName != m.m_eSymbolName ) return FALSE;
		if (this->m_dRotatedDeg != m.m_dRotatedDeg) return FALSE;
		if (this->m_strUserSymbol != m.m_strUserSymbol) return FALSE;
		if (this->m_pUserSymbol != m.m_pUserSymbol) return FALSE;

		int nSrcSize = static_cast<int>(this->m_vcParams.size());
		int nDstSize = static_cast<int>(m.m_vcParams.size());

		if (nSrcSize != nDstSize)  return FALSE;

		for (int i = 0; i < nSrcSize; i++)
		{
			if (this->m_vcParams[i] != m.m_vcParams[i])
			{
				return FALSE;
			}
		}

		return TRUE;

	}

	~CSymbol() { m_vcParams.clear(); };

public:
	void SetSymbolName(CString strSymbolName, CString strParams);
	void SetParamVal(int nIndex, CString strValue);
	void ConvertValue(class CJobFile* pJobFile);
	void SetMinMaxRect();
	UINT32 SetFinalParamVal(int nFinalIndex, CString strValue, class CJobFile* pJobFile, bool bRotated);

public:
	enum SymbolName	m_eSymbolName;
	UINT32			m_iSerialNum;
	double			m_dRotatedDeg;
	RECTD			m_MinMaxRect;
	CString			m_strUserSymbol;

	class CUserSymbol* m_pUserSymbol;
	std::vector<double>	m_vcParams;

protected:
	DECLARE_SERIAL(CSymbol);
};

class CRecord : public CObject
{
public:
	CRecord() : m_bPolarity(true), m_eType(RecordType::RecordTypeNone), m_MinMaxRect(0.f, 0.f, 0.f,0.f) {};
	CRecord(bool pol, RecordType type, RECTD rc) : m_bPolarity(pol), m_eType(type), m_MinMaxRect(rc) {};

	CRecord& operator=(const CRecord& m)
	{
		if (this != &m)
		{
			this->m_bPolarity = m.m_bPolarity;
			this->m_eType = m.m_eType;
			this->m_MinMaxRect = m.m_MinMaxRect;
		}
		return *this;
	};

	~CRecord() {};

public:
	virtual void ClearRecord() {};
	virtual void SetMinMaxRect() {};
	virtual void LoadRecord(CString strOneLine, CStdioFile& cStdFile, bool bIsMMUnit = false) {};

public:
	bool			m_bPolarity;			// true=Positive, false=Negative
	enum RecordType m_eType;
	RECTD			m_MinMaxRect;

protected:
	DECLARE_SERIAL(CRecord)
};

class CRecordRC : public CRecord
{
	// RC <lower_left_x> <lower_left_y> <width> <height>
public:
	CRecordRC() : m_dLBx(0.f), m_dLBy(0.f), m_dWidth(0.f), m_dHeight(0.f) { m_eType = RecordType::RC; };
	CRecordRC(double lbx, double lby, double w, double h) : m_dLBx(lbx), m_dLBy(lby), m_dWidth(w), m_dHeight(h) {
		m_eType = RecordType::RC;
	};

	CRecordRC& operator=(const CRecordRC& m)
	{
		if (this != &m)
		{
			this->m_dLBx = m.m_dLBx;
			this->m_dLBy = m.m_dLBy;
			this->m_dWidth = m.m_dWidth;
			this->m_dHeight = m.m_dHeight;
		}
		return *this;
	};

	~CRecordRC() {};

public:
	void ClearRecord() {};
	void SetMinMaxRect();
	void LoadRecord(CString strOneLine, CStdioFile& cStdFile, bool bIsMMUnit = false);

public:
	double	m_dLBx;
	double	m_dLBy;
	double  m_dWidth;
	double  m_dHeight;

protected:
	DECLARE_SERIAL(CRecordRC)
};

class CRecordCR : public CRecord
{
	// CR <xc> <yc> <radius>
public:
	CRecordCR() : m_dCx(0.f), m_dCy(0.f), m_dRadius(0.f) { m_eType = RecordType::CR; };
	CRecordCR(double x, double y, double r) : m_dCx(x), m_dCy(y), m_dRadius(r) { m_eType = RecordType::CR; };

	CRecordCR& operator=(const CRecordCR& m)
	{
		if (this != &m)
		{
			this->m_dCx = m.m_dCx;
			this->m_dCy = m.m_dCy;
			this->m_dRadius = m.m_dRadius;
		}
		return *this;
	};

	~CRecordCR() {};

public:
	void ClearRecord() {};
	void SetMinMaxRect();
	void LoadRecord(CString strOneLine, CStdioFile& cStdFile, bool bIsMMUnit = false);

public:
	double	m_dCx;
	double	m_dCy;
	double  m_dRadius;

protected:
	DECLARE_SERIAL(CRecordCR)
};

class CRecordSQ : public CRecord
{
	// CR <xc> <yc> <radius>
public:
	CRecordSQ() : m_dCx(0.f), m_dCy(0.f), m_dHalf(0.f) { m_eType = RecordType::SQ; };
	CRecordSQ(double x, double y, double half) : m_dCx(x), m_dCy(y), m_dHalf(half) {
		m_eType = RecordType::SQ;
	};

	CRecordSQ& operator=(const CRecordSQ& m)
	{
		if (this != &m)
		{
			this->m_dCx = m.m_dCx;
			this->m_dCy = m.m_dCy;
			this->m_dHalf = m.m_dHalf;
		}
		return *this;
	};

	~CRecordSQ() {};

public:
	void ClearRecord() {};
	void SetMinMaxRect();
	void LoadRecord(CString strOneLine, CStdioFile& cStdFile, bool bIsMMUnit = false);

public:
	double	m_dCx;
	double	m_dCy;
	double  m_dHalf;

protected:
	DECLARE_SERIAL(CRecordSQ)
};

class CRecordCT : public CRecord
{
	// S <params> ; <atr>=<value>...
	// <polygon1>
	// <polygon2>
	// SE
	// params section contains <polarity> <decode>
	// polygon starts with an OB command, contains OS(segment) or OC(Arc) commands, and ends with an OE command
	// OB <xbs> <ybs> <poly_type> : start point, I for island, H for hole
	// OS <x> <y> : segment end point (previous polygon point is the start point)
	// OC <xe> <ye> <xc> <yc> <cw> : curve end point (previous polygon point is the start point), curve center point, Y for clockwise, N for counter-clockwise
	// OE
public:
	CRecordCT() { m_eType = RecordType::CT; };
	~CRecordCT() { ClearRecord(); };

public:
	void ClearRecord();
	void SetMinMaxRect();
	void LoadRecord(CString strOneLine, CStdioFile& cStdFile, bool bIsMMUnit = false);
	void SetMinMaxOsOc(double X1, double Y1, double X2, double Y2, double XC, double YC, bool cw, RECTD& tmpMinMax);

public:
	CTypedPtrArray <CObArray, CObOe*> m_arrObOe;

protected:
	DECLARE_SERIAL(CRecordCT)
};

class CPin : public CObject
{
public:
	CPin() : m_ePinType(PinType::PinTypeNone), m_eElecType(ElecType::ElecTypeNone), m_eMountType(MountType::MountTypeNone),
		m_cx(0.f), m_cy(0.f), m_fhs(0.f), m_strPinName(L""), m_pRecord(nullptr) {};
	CPin(PinType pin, ElecType ele, MountType mnt, double x, double y, double fhs, CString name, CRecord* p)
		: m_ePinType(pin), m_eElecType(ele), m_eMountType(mnt),	m_cx(x), m_cy(y), m_fhs(fhs), m_strPinName(name), m_pRecord(p) {};

	CPin& operator=(const CPin& m)
	{
		if (this != &m)
		{
			this->m_ePinType = m.m_ePinType;
			this->m_eElecType = m.m_eElecType;
			this->m_eMountType = m.m_eMountType;
			this->m_cx = m.m_cx;
			this->m_cy = m.m_cy;
			this->m_fhs = m.m_fhs;
			this->m_strPinName = m.m_strPinName;
			this->m_pRecord = m.m_pRecord;
		}
		return *this;
	};

	~CPin() { ClearRecord(); };

public:
	void ClearRecord() { if (m_pRecord)	delete m_pRecord; };

public:
	PinType m_ePinType;		// T: Through hole(top to bottom), B: Blind(top to inner or inner to bottom), S: Surface(top to top or bottom to bottom)
	ElecType m_eElecType;	// E: Electrical, M: Non-electrical(Mechanical), U: Undefined(default)
	MountType m_eMountType; // S: SMT, D: Recommended SMT pad, T: Through hole, R: Recommended Through hole, P: Pressfit, N: Non-board, H: Hole, U: Undefined(default)
	double	m_cx;			// center X
	double  m_cy;			// center Y
	double  m_fhs;			// finished hole size (currently not used - should be 0)
	CString m_strPinName;
	CRecord* m_pRecord;

protected:
	DECLARE_SERIAL(CPin);
};

class CPackage : public CObject
{
public:
	CPackage() : m_fPitch(0.f), m_MinMaxRect(0.f, 0.f, 0.f, 0.f), m_strPkgName(L""), m_pRecord(nullptr) {};
	CPackage(double pit, RECTD rc, CString name, CRecord* p)
		: m_fPitch(pit), m_MinMaxRect(rc), m_strPkgName(name), m_pRecord(p) {};

	CPackage& operator=(const CPackage& m)
	{
		if (this != &m)
		{
			this->m_fPitch = m.m_fPitch;
			this->m_MinMaxRect = m.m_MinMaxRect;
			this->m_strPkgName = m.m_strPkgName;
			this->m_pRecord = m.m_pRecord;
		}
		return *this;
	};

	~CPackage() { ClearPackage(); ClearPins(); };

public:
	void ClearPackage();
	void ClearPins();

public:	
	double	m_fPitch;
	RECTD	m_MinMaxRect;
	CString m_strPkgName;
	CRecord* m_pRecord;

	CTypedPtrArray <CObArray, CPin*> m_arrPin;

protected:
	DECLARE_SERIAL(CPackage);
};

class CToeprint : public CObject
{
	//TOP <pin_num> <x> <y> <rot> <mirror> <net_num> <subnet_num> <toeprint_name>
public:
	CToeprint() : m_bMirror(false), m_eOrient(Orient::OrientNone), m_nPinIdx(0), m_nNetNum(0), m_nSubNetNum(0),
		m_dSx(0.f), m_dSy(0.f), m_strName(L""), m_pPin(nullptr) {};
	CToeprint(bool mir, Orient ori, UINT32 pin, UINT32 net, UINT32 sub, double dx, double dy, CString name, CPin* p)
		: m_bMirror(mir), m_eOrient(ori), m_nPinIdx(pin), m_nNetNum(net), m_nSubNetNum(sub),
		m_dSx(dx), m_dSy(dy), m_strName(name), m_pPin(p) {};

	CToeprint& operator=(const CToeprint& m)
	{
		if (this != &m)
		{
			this->m_bMirror = m.m_bMirror;
			this->m_eOrient = m.m_eOrient;
			this->m_nPinIdx = m.m_nPinIdx;
			this->m_nNetNum = m.m_nNetNum;
			this->m_nSubNetNum = m.m_nSubNetNum;
			this->m_dSx = m.m_dSx;
			this->m_dSy = m.m_dSy;
			this->m_strName = m.m_strName;
			this->m_pPin = m.m_pPin;
		}
		return *this;
	};

	~CToeprint() {};

public:
	bool		m_bMirror;
	enum Orient m_eOrient;
	UINT32		m_nPinIdx;
	UINT32		m_nNetNum;
	UINT32		m_nSubNetNum;

	double		m_dSx;
	double		m_dSy;	
	CString		m_strName;
	CPin*		m_pPin;

protected:
	DECLARE_SERIAL(CToeprint);
};

class CComponent : public CObject
{
	//CMP <pkg_ref> <x> <y> <rot> <mirror> <comp_name> <part_name>;<attributes>;ID=<id>
public:
	CComponent() : m_bMirror(false), m_eOrient(Orient::OrientNone), m_nPkgIdx(0), m_dSx(0.f), m_dSy(0.f),
		m_MinMaxRect(0.f, 0.f, 0.f, 0.f), m_strCompName(L""), m_strPartName(L""), m_pPackage(nullptr) {};
	CComponent(bool mir, Orient ori, UINT32 idx, double sx, double sy, RECTD rc, CString comp, CString part, CPackage* p)
		: m_bMirror(mir), m_eOrient(ori), m_nPkgIdx(idx), m_dSx(sx), m_dSy(sy),
		m_MinMaxRect(rc), m_strCompName(comp), m_strPartName(part), m_pPackage(p) {};

	CComponent& operator=(const CComponent& m)
	{
		if (this != &m)
		{
			this->m_bMirror = m.m_bMirror;
			this->m_eOrient = m.m_eOrient;
			this->m_nPkgIdx = m.m_nPkgIdx;
			this->m_dSx = m.m_dSx;
			this->m_dSy = m.m_dSy;
			this->m_MinMaxRect = m.m_MinMaxRect;
			this->m_strCompName = m.m_strCompName;
			this->m_strPartName = m.m_strPartName;
			this->m_pPackage = m.m_pPackage;
		}
		return *this;
	};

	~CComponent() { ClearToeprints(); };

public:
	void ClearToeprints();

public:
	bool		m_bMirror;
	enum Orient m_eOrient;		//clockwise
	UINT32		m_nPkgIdx;
	double		m_dSx;
	double		m_dSy;
	RECTD		m_MinMaxRect;
	CString		m_strCompName;
	CString		m_strPartName;

	CPackage*	m_pPackage;
	CTypedPtrArray <CObArray, CToeprint*> m_arrToeprint;

protected:
	DECLARE_SERIAL(CComponent);
};

class CComponentFile : public CObject
{
public:
	CComponentFile() : m_MinMaxRect(0.f, 0.f, 0.f, 0.f) {};
	CComponentFile(RECTD rc) : m_MinMaxRect(rc) {};

	CComponentFile& operator=(const CComponentFile& m)
	{
		if (this != &m)
		{
			this->m_MinMaxRect = m.m_MinMaxRect;
		}
		return *this;
	};

	~CComponentFile() { ClearComponents(); };

public:
	void ClearComponents();
	void SetMinMaxRect();
	UINT32 LoadComponentFile(CString strComponentFile, class CStep* pStep);

public:
	RECTD m_MinMaxRect;
	CTypedPtrArray <CObArray, CComponent*> m_arrComponent;

protected:
	DECLARE_SERIAL(CComponentFile);
};

//////////////////////////////////////////////////////////////////////////
//Align/Mask
class CAlignMask : public CObject
{
public:
	CAlignMask() {};
	~CAlignMask()
	{
		Clear();
	}

	void Clear()
	{
		for (UINT32 i = 0; i < m_arrFeature.GetSize(); i++)
		{
			CFeature *pFeature = m_arrFeature.GetAt(i);
			if (pFeature == nullptr) continue;
			if (pFeature->m_pSymbol != nullptr)
			{
				delete pFeature->m_pSymbol;
				pFeature->m_pSymbol = nullptr;
			}
			if (pFeature != nullptr)
			{
				delete pFeature;
				pFeature = nullptr;
			}
		}

		m_arrFeature.RemoveAll();
	}

	UINT32 Add_Pad(IN const double &dCenterX, IN const double &dCenterY, IN const double &dWidth, IN const double &dHeight);

	CAlignMask& operator=(const CAlignMask& m);

public:
	CTypedPtrArray<CObArray, CFeature*> m_arrFeature;//0 : Pattern, other : Repeat Area
};

class CLayer : public CObject
{
public:
	CLayer() : m_bVisible(false), m_nID(0), m_fXScale(1.0f), m_fYScale(1.0f), m_strLayerName(L"") {};
	CLayer(bool v, UINT32 id, double fx, double fy, CString name) 
		: m_bVisible(v), m_nID(id), m_fXScale(fx), m_fYScale(fy), m_strLayerName(name) {};

	CLayer& operator=(const CLayer& m)
	{
		if (this != &m)
		{
			this->m_bVisible = m.m_bVisible;
			this->m_nID = m.m_nID;
			this->m_fXScale = m.m_fXScale;
			this->m_fYScale = m.m_fYScale;
			this->m_strLayerName = m.m_strLayerName;
		}
		return *this;
	};

	~CLayer() { ClearLayer(); };

public:
	void ClearLayer();
	virtual void Serialize(CArchive& ar);
	UINT32 AssignDynamicStrings(CString strStepName, CJobFile* pJobFile);

	UINT32 ClearAlignMask(IN MaskType eMaskType);
	UINT32 LoadAlignMask(IN CString &strPathName, IN MaskType eMaskType);
	UINT32 SaveAlignMask(IN CString &strPathName, IN MaskType eMaskType);

public:
	bool			m_bVisible;
	UINT32			m_nID;
	double			m_fXScale;        // X축 Scale 1.0
	double			m_fYScale;        // Y축 Scale 1.0

	CString			m_strLayerName;
	CFeatureFile	m_FeatureFile;
	CComponentFile	m_ComponentFile;

	//
	CTypedPtrArray<CObArray, CAlignMask*> m_arrAlign;
	CTypedPtrArray<CObArray, CAlignMask*> m_arrMask;

protected:
	DECLARE_SERIAL(CLayer);
};

class CStepRepeat : public CObject
{
public:
	CStepRepeat() : m_bFlip(false), m_bMirror(false), m_nNX(0), m_nNY(0), m_dX(0.f), m_dY(0.f), m_dDX(0.f), m_dDY(0.f),
		m_dAngle(0.f), m_FeatureMinMax(0.f, 0.f, 0.f, 0.f), m_ProfileMinMax(0.f, 0.f, 0.f, 0.f), m_strName(L""),
		m_mirDir(eDirectionType::DirectionTypeNone){};
	CStepRepeat(bool flip, bool mir, UINT32 nx, UINT32 ny, double x, double y, double dx, double dy, double angle,
		RECTD feature, RECTD profile, CString name, eDirectionType mirDir = eDirectionType::DirectionTypeNone)
		: m_bFlip(flip), m_bMirror(mir), m_nNX(nx), m_nNY(ny), m_dX(x), m_dY(y), m_dDX(dx), m_dDY(dy), m_dAngle(angle),
		m_FeatureMinMax(feature), m_ProfileMinMax(profile), m_strName(name), m_mirDir(mirDir) {};

	CStepRepeat& operator=(const CStepRepeat& m)
	{
		if (this != &m)
		{
			this->m_bFlip = m.m_bFlip;						//allows two identical steps to be placed on a panel in such a way that on the same side
															//it contains the top of one step and the bottom of the other.
			this->m_bMirror = m.m_bMirror;					//true for mirror (around X axis), false for no mirror
			this->m_nNX = m.m_nNX;
			this->m_nNY = m.m_nNY;
			this->m_dX = m.m_dX;
			this->m_dY = m.m_dY;
			this->m_dDX = m.m_dDX;
			this->m_dDY = m.m_dDY;
			this->m_dAngle = m.m_dAngle;					//rotation angle of the step (0 ~ 360 degree)	
			this->m_FeatureMinMax = m.m_FeatureMinMax;
			this->m_ProfileMinMax = m.m_ProfileMinMax;
			this->m_strName = m.m_strName;
			this->m_mirDir = m.m_mirDir;
		}
		return *this;
	};

	~CStepRepeat() {};

public:
	virtual void Serialize(CArchive& ar);

public:
	bool	m_bFlip;
	bool	m_bMirror;
	UINT32	m_nNX;
	UINT32	m_nNY;
	double  m_dX;
	double	m_dY;
	double  m_dDX;
	double  m_dDY;
	double  m_dAngle;
	RECTD	m_FeatureMinMax;
	RECTD	m_ProfileMinMax;
	CString m_strName;
	eDirectionType m_mirDir;

protected:
	DECLARE_SERIAL(CStepRepeat)
};

class CStephdr : public CObject
{
public:
	CStephdr() : m_bStepRepeat(false), m_dXDatum(0.f), m_dYDatum(0.f), m_dXOrigin(0.f), m_dYOrigin(0.f),
		m_dTopActive(0.f), m_dBottomActive(0.f), m_dRightActive(0.f), m_dLeftActive(0.f) {};
	CStephdr(bool br, double x, double y, double orix, double oriy, double t, double b, double r, double l)
		: m_bStepRepeat(br), m_dXDatum(x), m_dYDatum(y), m_dXOrigin(orix), m_dYOrigin(oriy),
		m_dTopActive(t), m_dBottomActive(b), m_dRightActive(r), m_dLeftActive(l) {};

	CStephdr& operator=(const CStephdr& m)
	{
		if (this != &m)
		{
			this->m_bStepRepeat = m.m_bStepRepeat;
			this->m_dXDatum = m.m_dXDatum;
			this->m_dYDatum = m.m_dYDatum;
			this->m_dXOrigin = m.m_dXOrigin;
			this->m_dYOrigin = m.m_dYOrigin;
			this->m_dTopActive = m.m_dTopActive;
			this->m_dBottomActive = m.m_dBottomActive;
			this->m_dRightActive = m.m_dRightActive;
			this->m_dLeftActive = m.m_dLeftActive;
		}
		return *this;
	};

	~CStephdr() { ClearStepRepeats(); };

public:
	UINT32 LoadStephdr(CString strFullPathName);
	void ClearStepRepeats();
	virtual void Serialize(CArchive& ar);

public:
	bool		m_bStepRepeat;
	double		m_dXDatum;
	double		m_dYDatum;
	double		m_dXOrigin;
	double		m_dYOrigin;
	double		m_dTopActive;				//active area for step & repeat (positive distance from the top edge)
	double		m_dBottomActive;			//active area for step & repeat (positive distance from the bottom edge)
	double		m_dRightActive;				//active area for step & repeat (positive distance from the right edge)
	double		m_dLeftActive;				//active area for step & repeat (positive distance from the left edge)

	CTypedPtrArray <CObArray, CStepRepeat*> m_arrStepRepeat;

protected:
	DECLARE_SERIAL(CStephdr)
};

class CProfile : public CObject
{
public:
	CProfile() : m_MinMax(0.f, 0.f, 0.f, 0.f), m_pProfileLayer(nullptr) {};
	CProfile(RECTD rc, CLayer* p) : m_MinMax(rc), m_pProfileLayer(p) {};

	CProfile& operator=(const CProfile& m)
	{
		if (this != &m)
		{
			this->m_MinMax = m.m_MinMax;
			this->m_pProfileLayer = m.m_pProfileLayer;
		}
		return *this;
	};

	~CProfile() { Clear(); };

public:
	void Clear();
	virtual void Serialize(CArchive& ar);
	UINT32 LoadProfile(CString strFullPathName);
	UINT32 LoadProfileLayer(CString strFullPathName);

public:
	RECTD m_MinMax;					
	CLayer* m_pProfileLayer;

protected:
	DECLARE_SERIAL(CProfile)
};

class CSubStep : public CObject
{
public:
	CSubStep() : m_nStepRepeatType(0), m_nNextStepCnt(0), m_nStepID(0), m_dXDatum(0.f), m_dYDatum(0.f),
		m_dXOrigin(0.f), m_dYOrigin(0.f), m_FeatureMinMax(0.f, 0.f, 0.f, 0.f), m_pSubStep(nullptr) {};
	CSubStep(UINT32 type, UINT32 n, UINT32 id, double x, double y, double orix, double oriy, RECTD rc, class CStep* p)
		: m_nStepRepeatType(type), m_nNextStepCnt(n), m_nStepID(id), m_dXDatum(x), m_dYDatum(y),
		m_dXOrigin(orix), m_dYOrigin(oriy), m_FeatureMinMax(rc), m_pSubStep(p) {};

	CSubStep& operator=(const CSubStep& m)
	{
		if (this != &m)
		{
			this->m_nStepRepeatType = m.m_nStepRepeatType;
			this->m_nNextStepCnt = m.m_nNextStepCnt;
			this->m_nStepID = m.m_nStepID;
			this->m_dXDatum = m.m_dXDatum;
			this->m_dYDatum = m.m_dYDatum;
			this->m_dXOrigin = m.m_dXOrigin;
			this->m_dYOrigin = m.m_dYOrigin;
			this->m_FeatureMinMax = m.m_FeatureMinMax;
			this->m_pSubStep = m.m_pSubStep;
			this->m_arrStepRepeat = m.m_arrStepRepeat;

			this->ClearSubSteps();
			for (UINT32 i = 0; i < m.m_arrNextStep.GetSize(); i++)			
				m_arrNextStep.Add(m.m_arrNextStep.GetAt(i));			
		}
		return *this;
	};

	~CSubStep() { ClearSubSteps(); };

public:
	void ClearSubSteps();

public:
	UINT32				m_nStepRepeatType;
	UINT32				m_nNextStepCnt;
	UINT32				m_nStepID;				// Step ID
	double				m_dXDatum;
	double				m_dYDatum;
	double				m_dXOrigin;
	double				m_dYOrigin;
	RECTD				m_FeatureMinMax;
	class CStep*		m_pSubStep;

	class CStepRepeat	m_arrStepRepeat;	
	CTypedPtrArray <CObArray, CSubStep*> m_arrNextStep;

protected:
	DECLARE_SERIAL(CSubStep)
};


class CStep : public CObject
{
public:
	CStep() : m_nStepRepeatType(0), m_nStepID(0), m_nSubStepCnt(0), m_FeatureMinMax(0.f, 0.f, 0.f, 0.f),
		m_ActiveRect(0.f, 0.f, 0.f, 0.f), m_strStepName(L"") {};
	CStep(UINT32 type, UINT32 id, UINT32 cnt, RECTD rc, RECTD active, CString name)
		: m_nStepRepeatType(type), m_nStepID(id), m_nSubStepCnt(cnt), m_FeatureMinMax(rc),
		m_ActiveRect(active), m_strStepName(name) {};

	CStep& operator=(const CStep& m)
	{
		if (this != &m)
		{
			this->m_nStepRepeatType = m.m_nStepRepeatType;
			this->m_nStepID = m.m_nStepID;
			this->m_nSubStepCnt = m.m_nSubStepCnt;
			this->m_FeatureMinMax = m.m_FeatureMinMax;
			this->m_ActiveRect = m.m_ActiveRect;
			this->m_strStepName = m.m_strStepName;
			this->m_Stephdr = m.m_Stephdr;
			this->m_Profile = m.m_Profile;

			this->ClearSubSteps();
			for (UINT32 i = 0; i < m.m_arrSubStep.GetSize(); i++)
				m_arrSubStep.Add(m.m_arrSubStep.GetAt(i));

			this->ClearLayers();
			for (UINT32 i = 0; i < m.m_arrLayer.GetSize(); i++)
				m_arrLayer.Add(m.m_arrLayer.GetAt(i));

			this->ClearPackages();
			for (UINT32 i = 0; i < m.m_arrPackage.GetSize(); i++)
				m_arrPackage.Add(m.m_arrPackage.GetAt(i));
		}
		return *this;
	};

	~CStep() { ClearSubSteps(); ClearLayers(); ClearPackages(); };

public:
	void ClearSubSteps();
	void ClearLayers();
	void ClearPackages();
	virtual void Serialize(CArchive& ar);
	UINT32 LoadStep(CString strPathName);
	UINT32 LoadPackage(CString strPathName);

public:
	UINT32			m_nStepRepeatType;
	UINT32			m_nStepID;			// Step ID
	UINT32			m_nSubStepCnt;
	RECTD			m_FeatureMinMax;
	RECTD			m_ActiveRect;
	CString			m_strStepName;		// PCB, STRIP, PANEL

	CStephdr		m_Stephdr;
	CProfile		m_Profile;			// 해당 Step의 외곽 크기를 나타냄.	

	CTypedPtrArray <CObArray, CSubStep*> m_arrSubStep;	//SubStep
	CTypedPtrArray <CObArray, CLayer*> m_arrLayer;	//Layer
	CTypedPtrArray <CObArray, CPackage*> m_arrPackage;	//Package

protected:
	DECLARE_SERIAL(CStep)
};

class CMisc : public CObject
{
public:
	CMisc() : 
		//info
		m_strJobName(L""), m_strVersion(L""),
		//Attrlist
		m_dThickness(0.f), m_strModel(L""), m_strPartNum(L""), m_strLotWeek(L""), m_strLotYear(L""), m_strLotRev(L"")
		{};
	~CMisc() {};

	UINT32 LoadInfo(IN const CString &strPath);
	UINT32 LoadAttrlist(IN const CString &strPath);

	UINT32 SaveInfo(IN const CString &strPath);
	UINT32 SaveAttrlist(IN const CString &strPath);

	CString GetModelName() { return m_strJobName; };
	UINT32 SetModelName(IN const CString &strModelName) {
		m_strJobName = strModelName;
		return RESULT_GOOD;
	};


	//Attrlist
	CString m_strModel;
	CString m_strPartNum;
	CString m_strLotWeek;
	CString m_strLotYear;
	CString m_strLotRev;
	double	m_dThickness;

private:
	//info
	CString m_strJobName;
	CString m_strVersion;	//ODB Version
	
};

class CJobFile : public CObject
{
public:
	CJobFile();
	~CJobFile() { ClearAll(); }

public:
	void ClearAll();
	bool GetCamLoaded() { return m_bLoaded; };
	bool GetMatrixNStepLoaded() { return m_bMatrixNStepLoaded; };

	CString GetModelName() { return m_Misc.GetModelName(); }
	UINT32 SetModelName(IN const CString &strModelName) {
		m_Misc.SetModelName(strModelName);
		return RESULT_GOOD;
	};

	void SetFeatureMinMaxOfStep();
	void SetFeatureMinMaxOfStepNRepeat();

	UINT32 LoadMisc();
	UINT32 LoadMatrix();
	UINT32 LoadMatrix_Sem(SystemSpec_t* pSystemSpec);
	UINT32 LoadFont();
	UINT32 LoadUserSymbols();
	UINT32 LoadStep();
	UINT32 LoadPadUsage();
	UINT32 LoadLayer(UINT32 iStepIndex, CString strStepName, LayerSet_t* pLayerSet);
	void ClearLayers();

	//Save
	UINT32 SaveMisc(IN CString &strPath);
	UINT32 SaveMatrix(bool bProcessMP = false);
	UINT32 SaveFont();
	UINT32 SaveUserSymbols();
	UINT32 SaveStep();
	UINT32 SavePadUsage();

	UINT32 SaveLayer(UINT32 iStepIndex, CString strStepName, LayerSet_t* pLayerSet);



	CStandardFont* GetStandardFont(CString strFontName);

private:
	void ClearSteps();
	void ClearUserSymbols();
	void ClearFonts();
	void ClearPadUsage();	

	//MP, TP Layer가 존재하는지 체크하여 없다면 Layer를 생성
	UINT32 CheckLayer_MpTp();

	

public:
	bool	m_bLoaded;
	bool	m_bMatrixNStepLoaded;
	enum Orient m_eUserDefinedOrient;
	
	
	CString m_strFullPathName;

	CMatrix	m_Matrix;
	std::vector<UINT32>  m_vcLayerNum;
	std::vector<CString> m_vcPreviewLayer;

	CTypedPtrArray <CObArray, CStep*> m_arrStep;
	CTypedPtrArray <CObArray, CStandardFont*> m_arrFont;
	CTypedPtrArray <CObArray, CUserSymbol*> m_arrSymbol; //User-defined Symbol Array
	CTypedPtrArray <CObArray, CAttributeTextString*> m_arrPadUsage;

	CMisc m_Misc;

	CRITICAL_SECTION m_CS;

public:
	static double	InchToMM(double dValue);
	static double	MMToInch(double dValue);
	static double	MilToMM(double dValue);
	static double	MMToMil(double dValue);
	static CPointD  RotatePt(double dx, double dy, double angle, double cenx, double ceny);
	static double	RotatePtX(double dx, double dy, double angle, double cenx, double ceny);
	static double	RotatePtY(double dx, double dy, double angle, double cenx, double ceny);	

protected:
	DECLARE_SERIAL(CJobFile)
};

class CRotMirr : public CObject
{
public:
	CRotMirr() : m_Orient(Orient::NoMir0Deg), m_MirDir(eDirectionType::DirectionTypeNone), m_dX(0), m_dY(0) {};
	CRotMirr(Orient ori, double dX, double dY, eDirectionType mirDir = eDirectionType::DirectionTypeNone)
		: m_Orient(ori), m_MirDir(mirDir), m_dX(dX), m_dY(dY) {};

	CRotMirr& operator=(const CRotMirr& m)
	{
		if (this != &m)
		{
			this->m_Orient = m.m_Orient;
			this->m_MirDir = m.m_MirDir;
			this->m_dX = m.m_dX;
			this->m_dY = m.m_dY;
		}
		return *this;
	};

	enum Orient GetOrient() { return m_Orient; }
	enum eDirectionType GetMirDir() { return m_MirDir; }
	double GetX() {	return m_dX; }
	double GetY() { return m_dY; }

private:
	enum Orient m_Orient;
	enum eDirectionType m_MirDir;
	double m_dX;
	double m_dY;
};

class CRotMirrArr : public CObject
{
private:
	CTypedPtrArray <CObArray, CRotMirr*> m_arrRotMirr;

public:
	CRotMirrArr() {}
	~CRotMirrArr() { ClearAll(); }

	void ClearAll();
	enum Orient GetOrient(UINT32 idx);
	double GetX(UINT32 idx);
	double GetY(UINT32 idx);
	UINT32 GetSize();

	UINT32 RemoveTail();
	enum Orient FinalOrient(enum Orient InOrient = Orient::NoMir0Deg);
	enum eDirectionType FinalDirection();
	UINT32 InputData(double dX, double dY, enum Orient eOrient, enum eDirectionType mirDir = eDirectionType::eHorizontal);

	UINT32 FinalPoint(double *pdFinalX, double *pdFinalY, double dInputX, double dInputY);
	static UINT32 ConvertOrient(enum Orient *pOrient, enum Orient eParent, enum Orient eChild);
	static UINT32 ConvertPoint(double *pdFinalX, double *pdFinalY, double dParentX, double dParentY, double dChildX, double dChildY, enum Orient eParentOrient, enum eDirectionType mirDir = eDirectionType::DirectionTypeNone);
	static UINT32 ReversePoint(double *pdFinalX, double *pdFinalY, double dParentX, double dParentY, enum Orient eParentOrient, enum eDirectionType mirDir = eDirectionType::DirectionTypeNone);


	CRotMirrArr& operator=(const CRotMirrArr& m)
	{
		if (this != &m)
		{
			int nCount = static_cast<int>(m.m_arrRotMirr.GetCount());
			this->ClearAll();
			for (int i = 0; i < nCount; i++)
			{
				CRotMirr* pRotMirr_Ori = m.m_arrRotMirr.GetAt(i);

				CRotMirr* pRotMirr = new CRotMirr(pRotMirr_Ori->GetOrient(), pRotMirr_Ori->GetX(), pRotMirr_Ori->GetY(), pRotMirr_Ori->GetMirDir());

				this->m_arrRotMirr.Add(pRotMirr);
			}
		}
		return *this;
	};

};

