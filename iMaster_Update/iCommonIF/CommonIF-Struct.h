#pragma once

#ifndef __COMMONIF_STRUCT_H__
#define __COMMONIF_STRUCT_H__

#include <atlstr.h> 
#include <vector>
#include <map>
#include "CommonIF-Define.h"
using namespace std;

typedef struct tagVersion {
	UINT32 Major, Minor, Release, Build;

	tagVersion() : Major(0), Minor(0), Release(0), Build(0) {};
	tagVersion(UINT32 ma, UINT32 mi, UINT32 r, UINT32 b) : Major(ma), Minor(mi), Release(r), Build(b) {};

	tagVersion(const tagVersion& m)
	{
		Major = m.Major;
		Minor = m.Minor;
		Release = m.Release;
		Build = m.Build;
	};

	tagVersion& operator=(const tagVersion& m)
	{
		if (this != &m)
		{
			this->Major = m.Major;
			this->Minor = m.Minor;
			this->Release = m.Release;
			this->Build = m.Build;
		}
		return *this;
	};

}Version;

typedef struct tagDoubleRect
{
	double	left;
	double	right;
	double	top;
	double	bottom;

	tagDoubleRect() : left(0.f), right(0.f), top(0.f), bottom(0.f) {};
	tagDoubleRect(double l, double t, double r, double b) : left(l), right(r), top(t), bottom(b) {};
	tagDoubleRect(const tagDoubleRect& drect)
	{
		left = drect.left;
		right = drect.right;
		top = drect.top;
		bottom = drect.bottom;
	};
	tagDoubleRect& operator=(const tagDoubleRect& drect)
	{
		if (this != &drect)
		{
			this->left = drect.left;
			this->right = drect.right;
			this->top = drect.top;
			this->bottom = drect.bottom;
		}
		return *this;
	};
	bool operator!=(const tagDoubleRect& drect)
	{
		if (left == drect.left && right == drect.right &&
			top == drect.top && bottom == drect.bottom)
			return false;
		else
			return true;
	};
	inline BOOL IsPtInRectD(double dX, double dY)
	{
		if (left <= dX && dX < right && top <= dY && dY < bottom)
			return TRUE;
		else
			return FALSE;
	}
	inline BOOL IsPtInRectD_R(double dX, double dY)
	{
		if (left <= dX && dX < right && top >= dY && dY > bottom)
			return TRUE;
		else
			return FALSE;
	}
	inline BOOL IsRectInRectD(const tagDoubleRect& drect)
	{
		if (IsPtInRectD(drect.left, drect.top) && IsPtInRectD(drect.right, drect.bottom) )
			return TRUE;
		else
			return FALSE;
	}
	inline BOOL IsIntersection(const tagDoubleRect& drect)
	{//교집합이 있는지 체크
		if (((drect.right >= left) &&
			(drect.bottom >= top) &&
			(drect.left <= right) &&
			(drect.top <= bottom)))
		{
			return TRUE;
		}
		return FALSE;
	}

	BOOL Intersection(const tagDoubleRect& drect, tagDoubleRect &drtIntersection)
	{

		if (((drect.right >= left) &&
			(drect.bottom >= top) &&
			(drect.left <= right) &&
			(drect.top <= bottom)))
		{
			drtIntersection = *this;

			//top
			if (top > drect.top) drtIntersection.top = top;
			else if (top < drect.top) drtIntersection.top = drect.top;

			//bottom
			if (bottom < drect.bottom) drtIntersection.bottom = bottom;
			else if (bottom > drect.bottom) drtIntersection.bottom = drect.bottom;

			//right
			if (right < drect.right) drtIntersection.right = right;
			else if (right > drect.right) drtIntersection.right = drect.right;

			//left
			if (left > drect.left) drtIntersection.left = left;
			else if (left < drect.left)	drtIntersection.left = drect.left;

			return TRUE;
		}

		return FALSE;
	}


	inline void NormalizeRectD()
	{
		double nTemp;
		if (left > right)
		{
			nTemp = left;
			left = right;
			right = nTemp;
		}
		if (top > bottom)
		{
			nTemp = top;
			top = bottom;
			bottom = nTemp;
		}
	}
	inline BOOL IsRectNull()
	{
		return (left == 0.0 && right == 0.0 && top == 0.0 && bottom == 0.0);
	}
	inline void SetRectEmpty()
	{
		left = right = top = bottom = 0.0;
	}
	static bool comp(const tagDoubleRect *t1, const tagDoubleRect *t2)
	{
		if (fabs(t1->left - t2->left) < EPSILON_DELTA2)
		{
			if (t1->top > t2->top) return true;
			else if (fabs(t1->top - t2->top) < EPSILON_DELTA2) return false;
			else return false;
		}
		else if (t1->left > t2->left)
			return false;

		return true;
	}
	static bool comp_var(const tagDoubleRect& t1, const tagDoubleRect& t2)
	{
		if (fabs(t1.left - t2.left) < EPSILON_DELTA2)
		{
			if (t1.top > t2.top) return true;
			else if (fabs(t1.top - t2.top) < EPSILON_DELTA2) return false;
			else return false;
		}
		else if (t1.left > t2.left)
			return false;

		return true;
	}
	inline double width()
	{
		NormalizeRectD();
		return right - left;
	}
	inline double height()
	{
		NormalizeRectD();
		return bottom - top;
	}
	inline double CenterX()
	{
		NormalizeRectD();
		return (right + left) / 2.;
	}
	inline double CenterY()
	{
		NormalizeRectD();
		return (bottom + top) / 2.;
	}

	inline void OffsetRect(IN const double &dOffsetX, IN const double &dOffsetY)
	{
		left += dOffsetX;
		top += dOffsetY;
		right += dOffsetX;
		bottom += dOffsetY;
	}

	inline void ScaleRect(IN const double &dScaleX, IN const double &dScaleY)
	{
		left *= dScaleX;
		top *= dScaleY;
		right *= dScaleX;
		bottom *= dScaleY;
	}

}RECTD;

typedef struct tagPointDXY2 { double	x, y; }	PointD2, PointDXY;
class CPointD : public PointDXY
{
public:
	CPointD() { x = y = 0; };
	CPointD(double x1, double y1) { x = x1; y = y1; };
	CPointD(double* p) { x = p[0]; y = p[1]; };
	CPointD(PointDXY p) { x = p.x; y = p.y; };
	CPointD(POINT  p) { x = p.x; y = p.y; };
	void operator=(PointDXY p) { x = p.x; y = p.y; };
	void operator=(POINT p) { x = p.x; y = p.y; };
	void operator+=(PointDXY p) { x += p.x; y += p.y; };
	void operator-=(PointDXY p) { x -= p.x; y -= p.y; };
	void operator*=(PointDXY p) { x *= p.x; y *= p.y; };
	void operator/=(PointDXY p) { if (p.x) x /= p.x; if (p.y) y /= p.y; };

	CPointD operator+(double a) { CPointD p1(x + a, y + a); return p1; };
	CPointD operator-(double a) { CPointD p1(x - a, y - a); return p1; };
	CPointD operator*(double a) { CPointD p1(x * a, y * a); return p1; };
	CPointD operator/(double a) { if (a == 0) return (*this); CPointD p1(x / a, y / a); return p1; };
	CPointD operator+(POINT p) { CPointD p1(x + p.x, y + p.y); return p1; };
	CPointD operator+(PointDXY p) { CPointD p1(x + p.x, y + p.y); return p1; };
	CPointD operator-(PointDXY p) { CPointD p1(x - p.x, y - p.y); return p1; };
	CPointD operator*(PointDXY p) { CPointD p1(x * p.x, y * p.y); return p1; };
	CPointD operator/(PointDXY p) { CPointD p1((p.x) ? x / p.x : x, (p.y) ? y / p.y : y); return p1; };
	double	GetNorm() { return sqrt(x * x + y * y); }; //norm, distance
	void	MakeNormVec() { double n = GetNorm(); x /= n; y /= n; };
	CPointD GetNormVec() { double n = GetNorm(); CPointD p1(x / n, y / n); return p1; };
	double	GetDist(CPointD p) { return sqrt((x - p.x) * (x - p.x) + (y - p.y) * (y - p.y)); };
};

struct Log_Msg
{
	CString strLog;
	LogMsgType enMsgType;

	Log_Msg() : strLog(L""), enMsgType(LogMsgType::EN_ERROR) {};
	Log_Msg(CString log, LogMsgType e) : strLog(log), enMsgType(e) {};
};

struct stStepRepeat
{
	bool	bMirror;
	double	dAngle;
	UINT32	iNx;
	UINT32	iNy;
	CString strName;
	eDirectionType mirDir;

	stStepRepeat() : bMirror(false), dAngle(0.f), iNx(0), iNy(0), strName(L""), mirDir(eDirectionType::DirectionTypeNone) {};
	stStepRepeat(const stStepRepeat& m)
	{
		bMirror = m.bMirror;
		dAngle = m.dAngle;
		iNx = m.iNx;
		iNy = m.iNy;
		strName = m.strName;
		mirDir = m.mirDir;
	};
	stStepRepeat& operator=(const stStepRepeat& m)
	{
		if (this != &m)
		{
			this->bMirror = m.bMirror;
			this->dAngle = m.dAngle;
			this->iNx = m.iNx;
			this->iNy = m.iNy;
			this->strName = m.strName;
			this->mirDir = m.mirDir;
		}
		return *this;
	};
};

struct stStepInfo
{
	CString strName;
	std::vector< stStepRepeat*> vcStepRepeat;	

	stStepInfo() : strName(L"") {};
	~stStepInfo() { Clear(); }
	void Clear()
	{
		if (vcStepRepeat.size())
		{			
			for (auto it : vcStepRepeat)
			{
				if (it)
				{
					delete it;
					it = nullptr;
				}
			}
			vcStepRepeat.clear();
		}
	}
	stStepInfo(const stStepInfo& m)
	{
		strName = m.strName;
		Clear();

		for (auto it : m.vcStepRepeat)
		{
			stStepRepeat* pStep = new stStepRepeat();
			*pStep = *it;
			vcStepRepeat.emplace_back(pStep);
		}
	};
	stStepInfo& operator=(const stStepInfo& m)
	{
		if (this != &m)
		{
			this->strName = m.strName;
			Clear();

			for (auto it : m.vcStepRepeat)
			{
				stStepRepeat* pStep = new stStepRepeat();
				*pStep = *it;
				this->vcStepRepeat.emplace_back(pStep);
			}

		}
		return *this;
	};
};

struct stLayerInfo
{
	bool	bIsPositive;
	bool	bIsTop;
	
	CString strName;
	CString strContext;
	CString strType;
	UINT32	iPairIndex;//Pair
	CString strPairLayerName;

	stLayerInfo() : bIsPositive(true), bIsTop(true), strName(L""), strContext(L""), strType(L""), iPairIndex(0), strPairLayerName(L""){};
	stLayerInfo(const stLayerInfo& m)
	{
		bIsPositive = m.bIsPositive;
		strName = m.strName;
		strContext = m.strContext;
		strType = m.strType;
		bIsTop = m.bIsTop;
		iPairIndex = m.iPairIndex;
		strPairLayerName = m.strPairLayerName;
	};
	stLayerInfo& operator=(const stLayerInfo& m)
	{
		if (this != &m)
		{
			this->bIsPositive = m.bIsPositive;
			this->strName = m.strName;
			this->strContext = m.strContext;
			this->strType = m.strType;
			this->bIsTop = m.bIsTop;
			this->iPairIndex = m.iPairIndex;
			this->strPairLayerName = m.strPairLayerName;
		}
		return *this;
	};
};

struct stPackage
{
	UINT32  iPinCnt;
	CString strName;
	CString strType;

	stPackage() : iPinCnt(0), strName(L""), strType(L"") {};
	stPackage(const stPackage& m)
	{
		iPinCnt = m.iPinCnt;
		strName = m.strName;
		strType = m.strType;
	};
	stPackage& operator=(const stPackage& m)
	{
		if (this != &m)
		{
			this->iPinCnt = m.iPinCnt;
			this->strName = m.strName;
			this->strType = m.strType;
		}
		return *this;
	};
};
struct stComponentInfo
{
	std::vector<stPackage*> vcPackages;

	stComponentInfo() {};
	~stComponentInfo() { Clear(); }
	void Clear()
	{
		if (vcPackages.size())
		{
			for (auto it : vcPackages)
			{
				if (it)
				{
					delete it;
					it = nullptr;
				}
			}
			vcPackages.clear();
		}
	}
	stComponentInfo(const stComponentInfo& m)
	{
		Clear();

		for (auto it : m.vcPackages)
		{
			stPackage* pPkg = new stPackage();
			*pPkg = *it;
			vcPackages.emplace_back(pPkg);
		}
	};
	stComponentInfo& operator=(const stComponentInfo& m)
	{
		if (this != &m)
		{
			Clear();

			for (auto it : m.vcPackages)
			{
				stPackage* pPkg = new stPackage();
				*pPkg = *it;
				this->vcPackages.emplace_back(pPkg);
			}

		}
		return *this;
	};
};

struct BasicSpec_t
{
	bool			bShowAllSteps;
	bool			bProfileMode;
	bool			bExceptDummy;
	bool			bKeepLocalCopy;

	bool			bExceptImageData;//false : All 저장, true : Image Data를 제외하고 저장
	bool			bLineArc_SubSave;//false : 기본 저장, true : Line, Arc 선폭별 저장

	UINT32			iMaxLayerNum;
	UINT32			iIPUNum;

	// 언어 추가 22.07.20 DJ.J
	eLanguage		LanguageType;
	eUserSite		UserSite;
	eMachineType    McType;
	ePlatFormType   PlatType;
	eProductType	ProdType;
	eCamType		CamType;
	eMasterLevel	MasterLevel;
	eScanType		ScanType;
	CString			strImageType;

	//레이어 추출시 Viewer 설정
	eDirectionType	emDirectionType;
	double			dAngle;

	//마스터 데이터 생성시 Back 방향 설정
	bool			bBackMirror;
	double			dBackAngle;

	// 마스터 데이터 생성시 Save Image Pixel 기준 설정
	double			dSaveImageWidth;
	double			dSaveImageHeight;

	BasicSpec_t() : bShowAllSteps(false), bProfileMode(false), bExceptDummy(false), bKeepLocalCopy(false), bExceptImageData(false), bLineArc_SubSave(false), iMaxLayerNum(MAX_LAYER_NUM), iIPUNum(3),
		UserSite(eUserSite::UserSiteNone), McType(eMachineType::MachineTypeNone), PlatType(ePlatFormType::PlatFormTypeNone), ProdType(eProductType::ProductTypeNone),LanguageType(eLanguage::LanguageNone),
		CamType(eCamType::CamTypeNone), MasterLevel(eMasterLevel::MasterLevelNone), ScanType(eScanType::ScanTypeNone), strImageType(_T("")),
		emDirectionType(eDirectionType::DirectionTypeNone), dAngle(0.0), bBackMirror(false), dBackAngle(0.0), dSaveImageWidth(0.0), dSaveImageHeight(0.0){};

	BasicSpec_t(const BasicSpec_t& m)
	{
		bShowAllSteps = m.bShowAllSteps;
		bProfileMode = m.bProfileMode;
		bExceptDummy = m.bExceptDummy;
		bKeepLocalCopy = m.bKeepLocalCopy;
		bExceptImageData = m.bExceptImageData;
		bLineArc_SubSave = m.bLineArc_SubSave;
		iMaxLayerNum = m.iMaxLayerNum;
		iIPUNum = m.iIPUNum;
		UserSite = m.UserSite;
		McType = m.McType;
		PlatType = m.PlatType;
		ProdType = m.ProdType;
		CamType = m.CamType;
		MasterLevel = m.MasterLevel;
		ScanType = m.ScanType;
		strImageType = m.strImageType;
		LanguageType = m.LanguageType;

		emDirectionType = m.emDirectionType;
		dAngle = m.dAngle;

		bBackMirror = m.bBackMirror;
		dBackAngle = m.dBackAngle;

		dSaveImageWidth = m.dSaveImageWidth;
		dSaveImageHeight = m.dSaveImageHeight;
		
	};

	BasicSpec_t& operator=(const BasicSpec_t& m)
	{
		if (this != &m)
		{
			this->bShowAllSteps = m.bShowAllSteps;
			this->bProfileMode = m.bProfileMode;
			this->bExceptDummy = m.bExceptDummy;
			this->bKeepLocalCopy = m.bKeepLocalCopy;
			this->bExceptImageData = m.bExceptImageData;
			this->bLineArc_SubSave = m.bLineArc_SubSave;
			this->iMaxLayerNum = m.iMaxLayerNum;
			this->iIPUNum = m.iIPUNum;
			this->UserSite = m.UserSite;
			this->McType = m.McType;
			this->PlatType = m.PlatType;
			this->ProdType = m.ProdType;
			this->CamType = m.CamType;
			this->MasterLevel = m.MasterLevel;
			this->ScanType = m.ScanType;
			this->strImageType = m.strImageType;
			this->LanguageType = m.LanguageType;

			this->emDirectionType = m.emDirectionType;
			this->dAngle = m.dAngle;

			this->bBackMirror = m.bBackMirror;
			this->dBackAngle = m.dBackAngle;

			this->dSaveImageWidth = m.dSaveImageWidth;
			this->dSaveImageHeight = m.dSaveImageHeight;
			
		}
		return *this;
	};
};

struct CameraSpec_t
{
	UINT32	iFrameWidth;
	UINT32  iFrameHeight;
	UINT32  iMinOverlapX;
	UINT32  iMinOverlapY;
	UINT32  iMinMarginX;
	UINT32  iMinMarginY;
	UINT32  iCellColNum;
	UINT32  iCellRowNum;
	UINT32  iCellMinOverlapX;
	UINT32  iCellMinOverlapY;

	double  dDummyMarginX;
	double  dDummyMarginY;
	double  dResolution;

	CameraSpec_t() : iFrameWidth(0), iFrameHeight(0), iMinOverlapX(0), iMinOverlapY(0), iMinMarginX(0),
		iMinMarginY(0), dResolution(0.f), iCellColNum(0), iCellRowNum(0), iCellMinOverlapX(0), iCellMinOverlapY(0),
		dDummyMarginX(0.f), dDummyMarginY(0.f) {};
	CameraSpec_t(const CameraSpec_t& m)
	{
		iFrameWidth = m.iFrameWidth;
		iFrameHeight = m.iFrameHeight;
		iMinOverlapX = m.iMinOverlapX;
		iMinOverlapY = m.iMinOverlapY;
		iMinMarginX = m.iMinMarginX;
		iMinMarginY = m.iMinMarginY;
		iCellColNum = m.iCellColNum;
		iCellRowNum = m.iCellRowNum;
		iCellMinOverlapX = m.iCellMinOverlapX;
		iCellMinOverlapY = m.iCellMinOverlapY;
		dDummyMarginX = m.dDummyMarginX;
		dDummyMarginY = m.dDummyMarginY;
		dResolution = m.dResolution;
	};

	CameraSpec_t& operator=(const CameraSpec_t& m)
	{
		if (this != &m)
		{
			this->iFrameWidth = m.iFrameWidth;
			this->iFrameHeight = m.iFrameHeight;
			this->iMinOverlapX = m.iMinOverlapX;
			this->iMinOverlapY = m.iMinOverlapY;
			this->iMinMarginX = m.iMinMarginX;
			this->iMinMarginY = m.iMinMarginY;
			this->dResolution = m.dResolution;
			this->iCellColNum = m.iCellColNum;
			this->iCellRowNum = m.iCellRowNum;
			this->iCellMinOverlapX = m.iCellMinOverlapX;
			this->iCellMinOverlapY = m.iCellMinOverlapY;
			this->dDummyMarginX = m.dDummyMarginX;
			this->dDummyMarginY = m.dDummyMarginY;
		}
		return *this;
	};
};

struct PathSpec_t
{
	CString strCamPath;
	CString strMasterPath;
	CString strLogPath;

	PathSpec_t() : strCamPath(L""), strMasterPath(L""), strLogPath(L"") {};
	PathSpec_t(const PathSpec_t& m)
	{
		strCamPath = m.strCamPath;
		strMasterPath = m.strMasterPath;
		strLogPath = m.strLogPath;
	};

	PathSpec_t& operator=(const PathSpec_t& m)
	{
		if (this != &m)
		{
			this->strCamPath = m.strCamPath;
			this->strMasterPath = m.strMasterPath;
			this->strLogPath = m.strLogPath;
		}
		return *this;
	};
};

struct FeatureSpec_t
{
	int nShape;
	CString strRotate;
	CString strMirror;
	CString strAttr;
	CString strAttrString;
	CString strFeature;
	double dHeight;
	double dWidth; 
	double dRadius;
	double dRatio;
	double dMoveX;
	double dMoveY;
	double dMoveX_Key;
	double dMoveY_Key;
	BOOL bWheelMode;
	BOOL bDragMode;
	BOOL bKeyMode;
	BOOL bPreview;



	FeatureSpec_t() : strRotate(L""), strMirror(L""), strAttr(L""), strAttrString(L""), strFeature(L""), dHeight(0.f), dWidth(0.f), dRadius(0.f), dMoveX_Key(0.f), dMoveY_Key(0.f),bKeyMode(FALSE),
		dRatio(0.0),bWheelMode(FALSE),nShape(-1) ,dMoveX(0.f), dMoveY(0.f), bDragMode(FALSE), bPreview(FALSE){};

	~FeatureSpec_t() { Clear(); }

	void Clear()

	{
		nShape = -1;
		strRotate = L"";
		strMirror = L"";
		strAttr = L"";
		strAttrString = L"";
		strFeature = L"";
		dHeight = 0.f;
		dWidth = 0.f;
		dRadius = 0.f;
		dRatio = 0;
		bWheelMode = FALSE;
		dMoveX = 0.f;
		dMoveY = 0.f;
		dMoveX_Key = 0.f;
		dMoveY_Key = 0.f;
		bDragMode = FALSE;
		bPreview = FALSE;
		bKeyMode = FALSE;
	}
	

	FeatureSpec_t(const FeatureSpec_t&m)
	{
		nShape = m.nShape;
		strRotate = m.strRotate;
		strMirror = m.strMirror;
		strAttr = m.strAttr;
		strAttrString = m.strAttrString;
		strFeature = m.strFeature;
		dHeight = m.dHeight;
		dWidth = m.dWidth;
		dRadius = m.dRadius;
		dRatio = m.dRatio;
		bWheelMode = m.bWheelMode;
		dMoveX = m.dMoveX;
		dMoveY = m.dMoveY;
		dMoveX_Key = m.dMoveX_Key;
		dMoveY_Key = m.dMoveY_Key;
		bDragMode = m.bDragMode;
		bPreview = m.bPreview;
		bKeyMode = m.bKeyMode;
	}

	FeatureSpec_t&operator=(const FeatureSpec_t&m)
	{
		if (this != &m)
		{
			this->nShape = m.nShape;
			this->strRotate = m.strRotate;
			this->strMirror = m.strMirror;
			this->strAttr = m.strAttr;
			this->strAttrString = m.strAttrString;
			this->strFeature = m.strFeature;
			this->dHeight = m.dHeight;
			this->dWidth = m.dWidth;
			this->dRadius = m.dRadius;
			this->dRatio = m.dRatio;
			this->bWheelMode = m.bWheelMode;
			this->dMoveX = m.dMoveX;
			this->dMoveY = m.dMoveY;
			this->dMoveX_Key = m.dMoveX_Key;
			this->dMoveY_Key = m.dMoveY_Key;
			this->bDragMode = m.bDragMode;
			this->bPreview = m.bPreview;
			this->bKeyMode = m.bKeyMode;
		}

		return *this;
	};

};

struct StepSpec_t
{
	std::vector<CString> vcPanelNames;
	std::vector<CString> vcStripNames;
	std::vector<CString> vcUnitNames;

	StepSpec_t() {};
	~StepSpec_t() { Clear(); }
	void Clear()
	{		
		vcPanelNames.clear();
		vcStripNames.clear();
		vcUnitNames.clear();
	}

	StepSpec_t(const StepSpec_t& m)
	{
		vcPanelNames.clear();
		for (auto it : m.vcPanelNames)
		{
			vcPanelNames.emplace_back(it);
		}
		vcStripNames.clear();
		for (auto it : m.vcStripNames)
		{
			vcStripNames.emplace_back(it);
		}
		vcUnitNames.clear();
		for (auto it : m.vcUnitNames)
		{
			vcUnitNames.emplace_back(it);
		}
	};

	StepSpec_t& operator=(const StepSpec_t& m)
	{
		if (this != &m)
		{
			this->vcPanelNames.clear();
			this->vcStripNames.clear();
			this->vcUnitNames.clear();

			for (auto it : m.vcPanelNames)
			{
				this->vcPanelNames.emplace_back(it);
			}
			for (auto it : m.vcStripNames)
			{
				this->vcStripNames.emplace_back(it);
			}
			for (auto it : m.vcUnitNames)
			{
				this->vcUnitNames.emplace_back(it);
			}
		}
		return *this;
	};
};

struct LayerInfo_t
{
	COLORREF				layerColor;
	CString					strDefine;
	std::vector<CString>	vcFrontLayerNames;
	std::vector<CString>	vcBackLayerNames;

	LayerInfo_t() : layerColor(0), strDefine(L"") {};
	~LayerInfo_t() { Clear(); }
	void Clear() 
	{
		vcFrontLayerNames.clear(); 
		vcBackLayerNames.clear();
	}
	LayerInfo_t(const LayerInfo_t& m)
	{
		layerColor = m.layerColor;
		strDefine = m.strDefine;

		Clear();
		for (auto it : m.vcFrontLayerNames)
			vcFrontLayerNames.emplace_back(it);
		for (auto it : m.vcBackLayerNames)
			vcBackLayerNames.emplace_back(it);
	}
	LayerInfo_t& operator=(const LayerInfo_t& m)
	{
		if (this != &m)
		{
			this->layerColor = m.layerColor;
			this->strDefine = m.strDefine;

			this->Clear();
			for (auto it : m.vcFrontLayerNames)
				this->vcFrontLayerNames.emplace_back(it);
			for (auto it : m.vcBackLayerNames)
				this->vcBackLayerNames.emplace_back(it);
		}
		return *this;
	};
};

struct LayerSpec_t
{
	std::vector<LayerInfo_t*> vcLayer;

	LayerSpec_t() {};
	~LayerSpec_t() { Clear(); }
	void Clear()
	{
		for (auto it : vcLayer)
		{
			it->Clear();
			delete it;
			it = nullptr;
		}
		vcLayer.clear();
	}
	LayerSpec_t(const LayerSpec_t& m)
	{
		Clear();
		for (auto it : m.vcLayer)
		{
			LayerInfo_t* pLayer = new LayerInfo_t;
			*pLayer = *it;
			vcLayer.emplace_back(pLayer);
		}
	}
	LayerSpec_t& operator=(const LayerSpec_t& m)
	{
		if (this != &m)
		{
			this->Clear();
			for (auto it : m.vcLayer)
			{
				LayerInfo_t* pLayer = new LayerInfo_t;
				*pLayer = *it;
				this->vcLayer.emplace_back(pLayer);
			}
		}
		return *this;
	};
};


struct NSIS_CameraSpec_t
{
	UINT32	iFovWidth;
	UINT32  iFovHeight;
	double  dResolution;
	double  dMagnification;
	double  dZoom;

	NSIS_CameraSpec_t() : iFovWidth(0), iFovHeight(0), dResolution(0.f), dMagnification(0.f), dZoom(0.f) {};
	NSIS_CameraSpec_t(const NSIS_CameraSpec_t& m)
	{
		iFovWidth = m.iFovWidth;
		iFovHeight = m.iFovHeight;
		dResolution = m.dResolution;
		dMagnification = m.dMagnification;
		dZoom = m.dZoom;
	};

	NSIS_CameraSpec_t& operator=(const NSIS_CameraSpec_t& m)
	{
		if (this != &m)
		{
			this->iFovWidth = m.iFovWidth;
			this->iFovHeight = m.iFovHeight;
			this->dResolution = m.dResolution;
			this->dMagnification = m.dMagnification;
			this->dZoom = m.dZoom;
		}
		return *this;
	};
};

struct Language_t
{
	std::vector<CString> vecCamMaster;
	std::vector<CString> vecCategory;
	std::vector<CString> vecHome;
	std::vector<CString> vecRefresh;
	std::vector<CString> vecSetting;
	std::vector<CString> vecMode;
	std::vector<CString> vecLineScan;
	std::vector<CString> vecAreaScan;
	std::vector<CString> vecPanel;
	std::vector<CString> vecPreview;
	std::vector<CString> vecManufacture;
	std::vector<CString> vecPreSR;
	std::vector<CString> vecPostSR;
	std::vector<CString> vecSide;
	std::vector<CString> vecFront;
	std::vector<CString> vecBack;
	std::vector<CString> vecProcess;
	std::vector<CString> vecLoadLayer;
	std::vector<CString> vecMakeData;
	std::vector<CString> vecTransferData;
	std::vector<CString> vecMakeAlign;
	std::vector<CString> vecRecipe;
	std::vector<CString> vecRecipeAlign;
	std::vector<CString> vecRecipeAlignAuto;
	std::vector<CString> vecRecipeAlignManual;
	std::vector<CString> vecRecipeNSIS;
	std::vector<CString> vecRecipeSR;
	std::vector<CString> vecRecipeALL;
	std::vector<CString> vecEdit;
	std::vector<CString> vecEditAlign;
	std::vector<CString> vecADDFeature;	
	std::vector<CString> vecShow;
	std::vector<CString> vecShowCAMFOV;
	std::vector<CString> vecShowCAMCELL;
	std::vector<CString> vecShowAll;
	std::vector<CString> vecShowProfile;
	std::vector<CString> vecShowDummy;
	std::vector<CString> vecShowSurface;
	std::vector<CString> vecShowAlign;
	std::vector<CString> vecShowMask;
	std::vector<CString> vecFOV;
	std::vector<CString> vecWorkDirHor;
	std::vector<CString> vecWorkDirHor2;
	std::vector<CString> vecWorkDirVer;
	std::vector<CString> vecView360;
	std::vector<CString> vecView90;
	std::vector<CString> vecView180;
	std::vector<CString> vecView270;
	std::vector<CString> vecViewFOV2;
	std::vector<CString> vecSaveCategory;
	std::vector<CString> vecSave;
	std::vector<CString> vecSaveAs;
	std::vector<CString> vecInformation;
	std::vector<CString> vecRuler;
	std::vector<CString> vecVersion;
	std::vector<CString> vecDocCategory;
	std::vector<CString> vecDocJob;
	std::vector<CString> vecDocCam;
	std::vector<CString> vecDocLayer;
	std::vector<CString> vecDocMeasure;
	std::vector<CString> vecDocMinimap;
	std::vector<CString> vecDocLog;
	
	Language_t() {};
	~Language_t() { Clear(); }
	void Clear()
	{			
		vecCamMaster.clear();
		vecCategory.clear();
		vecHome.clear();
		vecRefresh.clear();
		vecSetting.clear();
		vecMode.clear();
		vecLineScan.clear();
		vecAreaScan.clear();
		vecPanel.clear();
		vecPreview.clear();
		vecManufacture.clear();
		vecPreSR.clear();
		vecPostSR.clear();
		vecSide.clear();
		vecFront.clear();
		vecBack.clear();
		vecProcess.clear();
		vecLoadLayer.clear();
		vecMakeData.clear();
		vecTransferData.clear();
		vecMakeAlign.clear();
		vecRecipe.clear();
		vecRecipeAlign.clear();
		vecRecipeAlignAuto.clear();
		vecRecipeAlignManual.clear();
		vecRecipeNSIS.clear();
		vecRecipeSR.clear();
		vecRecipeALL.clear();
		vecEdit.clear();
		vecEditAlign.clear();
		vecADDFeature.clear();
		vecShow.clear();
		vecShowCAMFOV.clear();
		vecShowCAMCELL.clear();
		vecShowAll.clear();
		vecShowProfile.clear();
		vecShowDummy.clear();
		vecShowSurface.clear();
		vecShowAlign.clear();
		vecShowMask.clear();
		vecFOV.clear();
		vecWorkDirHor.clear();
		vecWorkDirHor2.clear();
		vecWorkDirVer.clear();
		vecView360.clear();
		vecView90.clear();
		vecView180.clear();
		vecView270.clear();
		vecViewFOV2.clear();
		vecSaveCategory.clear();
		vecSave.clear();
		vecSaveAs.clear();
		vecInformation.clear();
		vecRuler.clear();
		vecVersion.clear();
		vecDocCategory.clear();
		vecDocJob.clear();
		vecDocCam.clear();
		vecDocLayer.clear();
		vecDocMeasure.clear();
		vecDocMinimap.clear();
		vecDocLog.clear();
	}

	Language_t(const Language_t& m)
	{	
		vecCamMaster.clear();
		for (auto it : m.vecCamMaster)
		{
			vecCamMaster.emplace_back(it);
		}
		vecCategory.clear();
		for (auto it : m.vecCategory)
		{
			vecCategory.emplace_back(it);
		}
		vecHome.clear();
		for (auto it : m.vecHome)
		{
			vecHome.emplace_back(it);
		}
		vecRefresh.clear();
		for (auto it : m.vecRefresh)
		{
			vecRefresh.emplace_back(it);
		}
		vecSetting.clear();
		for (auto it : m.vecSetting)
		{
			vecSetting.emplace_back(it);
		}
		vecMode.clear();
		for (auto it : m.vecMode)
		{
			vecMode.emplace_back(it);
		}
		vecLineScan.clear();
		for (auto it : m.vecLineScan)
		{
			vecLineScan.emplace_back(it);
		}
		vecAreaScan.clear();
		for (auto it : m.vecAreaScan)
		{
			vecAreaScan.emplace_back(it);
		}
		vecPanel.clear();
		for (auto it : m.vecPanel)
		{
			vecPanel.emplace_back(it);
		}
		vecPreview.clear();
		for (auto it : m.vecPreview)
		{
			vecPreview.emplace_back(it);
		}
		vecManufacture.clear();
		for (auto it : m.vecManufacture)
		{
			vecManufacture.emplace_back(it);
		}
		vecPreSR.clear();
		for (auto it : m.vecPreSR)
		{
			vecPreSR.emplace_back(it);
		}
		vecPostSR.clear();
		for (auto it : m.vecPostSR)
		{
			vecPostSR.emplace_back(it);
		}
		vecSide.clear();
		for (auto it : m.vecSide)
		{
			vecSide.emplace_back(it);
		}
		vecFront.clear();
		for (auto it : m.vecFront)
		{
			vecFront.emplace_back(it);
		}
		vecBack.clear();
		for (auto it : m.vecBack)
		{
			vecBack.emplace_back(it);
		}
		vecProcess.clear();
		for (auto it : m.vecProcess)
		{
			vecProcess.emplace_back(it);
		}
		vecLoadLayer.clear();
		for (auto it : m.vecLoadLayer)
		{
			vecLoadLayer.emplace_back(it);
		}
		vecMakeData.clear();
		for (auto it : m.vecMakeData)
		{
			vecMakeData.emplace_back(it);
		}
		vecTransferData.clear();
		for (auto it : m.vecTransferData)
		{
			vecTransferData.emplace_back(it);
		}
		vecMakeAlign.clear();
		for (auto it : m.vecMakeAlign)
		{
			vecMakeAlign.emplace_back(it);
		}
		vecRecipe.clear();
		for (auto it : m.vecRecipe)
		{
			vecRecipe.emplace_back(it);
		}
		vecRecipeAlign.clear();
		for (auto it : m.vecRecipeAlign)
		{
			vecRecipeAlign.emplace_back(it);
		}
		vecRecipeAlignAuto.clear();
		for (auto it : m.vecRecipeAlignAuto)
		{
			vecRecipeAlignAuto.emplace_back(it);
		}
		vecRecipeAlignManual.clear();
		for (auto it : m.vecRecipeAlignManual)
		{
			vecRecipeAlignManual.emplace_back(it);
		}
		vecRecipeNSIS.clear();
		for (auto it : m.vecRecipeNSIS)
		{
			vecRecipeNSIS.emplace_back(it);
		}
		vecRecipeSR.clear();
		for (auto it : m.vecRecipeSR)
		{
			vecRecipeSR.emplace_back(it);
		}
		vecRecipeALL.clear();
		for (auto it : m.vecRecipeALL)
		{
			vecRecipeALL.emplace_back(it);
		}
		vecEdit.clear();
		for (auto it : m.vecEdit)
		{
			vecEdit.emplace_back(it);
		}
		vecEditAlign.clear();
		for (auto it : m.vecEditAlign)
		{
			vecEditAlign.emplace_back(it);
		}
		vecADDFeature.clear();
		for (auto it : m.vecADDFeature)
		{
			vecADDFeature.emplace_back(it);
		}
		vecShow.clear();
		for (auto it : m.vecShow)
		{
			vecShow.emplace_back(it);
		}
		vecShowCAMFOV.clear();
		for (auto it : m.vecShowCAMFOV)
		{
			vecShowCAMFOV.emplace_back(it);
		}
		vecShowCAMCELL.clear();
		for (auto it : m.vecShowCAMCELL)
		{
			vecShowCAMCELL.emplace_back(it);
		}
		vecShowAll.clear();
		for (auto it : m.vecShowAll)
		{
			vecShowAll.emplace_back(it);
		}
		vecShowProfile.clear();
		for (auto it : m.vecShowProfile)
		{
			vecShowProfile.emplace_back(it);
		}
		vecShowDummy.clear();
		for (auto it : m.vecShowDummy)
		{
			vecShowDummy.emplace_back(it);
		}
		vecShowSurface.clear();
		for (auto it : m.vecShowSurface)
		{
			vecShowSurface.emplace_back(it);
		}
		vecShowAlign.clear();
		for (auto it : m.vecShowAlign)
		{
			vecShowAlign.emplace_back(it);
		}
		vecShowMask.clear();
		for (auto it : m.vecShowMask)
		{
			vecShowMask.emplace_back(it);
		}
		vecFOV.clear();
		for (auto it : m.vecFOV)
		{
			vecFOV.emplace_back(it);
		}
		vecWorkDirHor.clear();
		for (auto it : m.vecWorkDirHor)
		{
			vecWorkDirHor.emplace_back(it);
		}
		vecWorkDirHor2.clear();
		for (auto it : m.vecWorkDirHor2)
		{
			vecWorkDirHor2.emplace_back(it);
		}
		vecWorkDirVer.clear();
		for (auto it : m.vecWorkDirVer)
		{
			vecWorkDirVer.emplace_back(it);
		}
		vecView360.clear();
		for (auto it : m.vecView360)
		{
			vecView360.emplace_back(it);
		}
		vecView90.clear();
		for (auto it : m.vecView90)
		{
			vecView90.emplace_back(it);
		}
		vecView180.clear();
		for (auto it : m.vecView180)
		{
			vecView180.emplace_back(it);
		}
		vecView270.clear();
		for (auto it : m.vecView270)
		{
			vecView270.emplace_back(it);
		}
		vecViewFOV2.clear();
		for (auto it : m.vecViewFOV2)
		{
			vecViewFOV2.emplace_back(it);
		}
		vecSaveCategory.clear();
		for (auto it : m.vecSaveCategory)
		{
			vecSaveCategory.emplace_back(it);
		}
		vecSave.clear();
		for (auto it : m.vecSave)
		{
			vecSave.emplace_back(it);
		}
		vecSaveAs.clear();
		for (auto it : m.vecSaveAs)
		{
			vecSaveAs.emplace_back(it);
		}
		vecInformation.clear();
		for (auto it : m.vecInformation)
		{
			vecInformation.emplace_back(it);
		}
		vecRuler.clear();
		for (auto it : m.vecRuler)
		{
			vecRuler.emplace_back(it);
		}
		vecVersion.clear();
		for (auto it : m.vecVersion)
		{
			vecVersion.emplace_back(it);
		}
		vecDocCategory.clear();
		for (auto it : m.vecDocCategory)
		{
			vecDocCategory.emplace_back(it);
		}
		vecDocJob.clear();
		for (auto it : m.vecDocJob)
		{
			vecDocJob.emplace_back(it);
		}
		vecDocCam.clear();
		for (auto it : m.vecDocCam)
		{
			vecDocCam.emplace_back(it);
		}
		vecDocLayer.clear();
		for (auto it : m.vecDocLayer)
		{
			vecDocLayer.emplace_back(it);
		}
		vecDocMeasure.clear();
		for (auto it : m.vecDocMeasure)
		{
			vecDocMeasure.emplace_back(it);
		}
		vecDocMinimap.clear();
		for (auto it : m.vecDocMinimap)
		{
			vecDocMinimap.emplace_back(it);
		}
		vecDocLog.clear();
		for (auto it : m.vecDocLog)
		{
			vecDocLog.emplace_back(it);
		}
	};

	Language_t& operator=(const Language_t& m)
	{
		if (this != &m)
		{
			this->vecCamMaster.clear();

			for (auto it : m.vecCamMaster)
			{
				this->vecCamMaster.emplace_back(it);
			}

			this->vecCategory.clear();

			for (auto it : m.vecCategory)
			{
				this->vecCategory.emplace_back(it);
			}

			this->vecHome.clear();

			for (auto it : m.vecHome)
			{
				this->vecHome.emplace_back(it);
			}

			this->vecRefresh.clear();

			for (auto it : m.vecRefresh)
			{
				this->vecRefresh.emplace_back(it);
			}

			this->vecSetting.clear();

			for (auto it : m.vecSetting)
			{
				this->vecSetting.emplace_back(it);
			}

			this->vecMode.clear();

			for (auto it : m.vecMode)
			{
				this->vecMode.emplace_back(it);
			}

			this->vecLineScan.clear();

			for (auto it : m.vecLineScan)
			{
				this->vecLineScan.emplace_back(it);
			}

			this->vecAreaScan.clear();

			for (auto it : m.vecAreaScan)
			{
				this->vecAreaScan.emplace_back(it);
			}
			this->vecPanel.clear();

			for (auto it : m.vecPanel)
			{
				this->vecPanel.emplace_back(it);
			}

			this->vecPreview.clear();

			for (auto it : m.vecPreview)
			{
				this->vecPreview.emplace_back(it);
			}

			this->vecManufacture.clear();

			for (auto it : m.vecManufacture)
			{
				this->vecManufacture.emplace_back(it);
			}

			this->vecPreSR.clear();

			for (auto it : m.vecPreSR)
			{
				this->vecPreSR.emplace_back(it);
			}

			this->vecPostSR.clear();

			for (auto it : m.vecPostSR)
			{
				this->vecPostSR.emplace_back(it);
			}

			this->vecSide.clear();

			for (auto it : m.vecSide)
			{
				this->vecSide.emplace_back(it);
			}

			this->vecFront.clear();

			for (auto it : m.vecFront)
			{
				this->vecFront.emplace_back(it);
			}

			this->vecBack.clear();

			for (auto it : m.vecBack)
			{
				this->vecBack.emplace_back(it);
			}

			this->vecProcess.clear();

			for (auto it : m.vecProcess)
			{
				this->vecProcess.emplace_back(it);
			}

			this->vecLoadLayer.clear();

			for (auto it : m.vecLoadLayer)
			{
				this->vecLoadLayer.emplace_back(it);
			}

			this->vecMakeData.clear();

			for (auto it : m.vecMakeData)
			{
				this->vecMakeData.emplace_back(it);
			}

			this->vecTransferData.clear();

			for (auto it : m.vecTransferData)
			{
				this->vecTransferData.emplace_back(it);
			}

			this->vecMakeAlign.clear();

			for (auto it : m.vecMakeAlign)
			{
				this->vecMakeAlign.emplace_back(it);
			}

			this->vecRecipe.clear();

			for (auto it : m.vecRecipe)
			{
				this->vecRecipe.emplace_back(it);
			}

			this->vecRecipeAlign.clear();

			for (auto it : m.vecRecipeAlign)
			{
				this->vecRecipeAlign.emplace_back(it);
			}

			this->vecRecipeAlignAuto.clear();

			for (auto it : m.vecRecipeAlignAuto)
			{
				this->vecRecipeAlignAuto.emplace_back(it);
			}

			this->vecRecipeAlignManual.clear();

			for (auto it : m.vecRecipeAlignManual)
			{
				this->vecRecipeAlignManual.emplace_back(it);
			}

			this->vecRecipeNSIS.clear();

			for (auto it : m.vecRecipeNSIS)
			{
				this->vecRecipeNSIS.emplace_back(it);
			}

			this->vecRecipeSR.clear();

			for (auto it : m.vecRecipeSR)
			{
				this->vecRecipeSR.emplace_back(it);
			}

			this->vecRecipeALL.clear();

			for (auto it : m.vecRecipeALL)
			{
				this->vecRecipeALL.emplace_back(it);
			}

			this->vecEdit.clear();

			for (auto it : m.vecEdit)
			{
				this->vecEdit.emplace_back(it);
			}

			this->vecEditAlign.clear();

			for (auto it : m.vecEditAlign)
			{
				this->vecEditAlign.emplace_back(it);
			}

			this->vecADDFeature.clear();

			for (auto it : m.vecADDFeature)
			{
				this->vecADDFeature.emplace_back(it);
			}

			this->vecShow.clear();

			for (auto it : m.vecShow)
			{
				this->vecShow.emplace_back(it);
			}

			this->vecShowCAMFOV.clear();

			for (auto it : m.vecShowCAMFOV)
			{
				this->vecShowCAMFOV.emplace_back(it);
			}

			this->vecShowCAMCELL.clear();

			for (auto it : m.vecShowCAMCELL)
			{
				this->vecShowCAMCELL.emplace_back(it);
			}

			this->vecShowAll.clear();

			for (auto it : m.vecShowAll)
			{
				this->vecShowAll.emplace_back(it);
			}

			this->vecShowProfile.clear();

			for (auto it : m.vecShowProfile)
			{
				this->vecShowProfile.emplace_back(it);
			}

			this->vecShowDummy.clear();

			for (auto it : m.vecShowDummy)
			{
				this->vecShowDummy.emplace_back(it);
			}

			this->vecShowSurface.clear();

			for (auto it : m.vecShowSurface)
			{
				this->vecShowSurface.emplace_back(it);
			}

			this->vecShowAlign.clear();

			for (auto it : m.vecShowAlign)
			{
				this->vecShowAlign.emplace_back(it);
			}

			this->vecShowMask.clear();

			for (auto it : m.vecShowMask)
			{
				this->vecShowMask.emplace_back(it);
			}

			this->vecFOV.clear();

			for (auto it : m.vecFOV)
			{
				this->vecFOV.emplace_back(it);
			}

			this->vecWorkDirHor.clear();

			for (auto it : m.vecWorkDirHor)
			{
				this->vecWorkDirHor.emplace_back(it);
			}

			this->vecWorkDirHor2.clear();

			for (auto it : m.vecWorkDirHor2)
			{
				this->vecWorkDirHor2.emplace_back(it);
			}

			this->vecWorkDirVer.clear();

			for (auto it : m.vecWorkDirVer)
			{
				this->vecWorkDirVer.emplace_back(it);
			}

			this->vecView360.clear();

			for (auto it : m.vecView360)
			{
				this->vecView360.emplace_back(it);
			}

			this->vecView90.clear();

			for (auto it : m.vecView90)
			{
				this->vecView90.emplace_back(it);
			}

			this->vecView180.clear();

			for (auto it : m.vecView180)
			{
				this->vecView180.emplace_back(it);
			}

			this->vecView270.clear();

			for (auto it : m.vecView270)
			{
				this->vecView270.emplace_back(it);
			}

			this->vecSaveCategory.clear();

			for (auto it : m.vecSaveCategory)
			{
				this->vecSaveCategory.emplace_back(it);
			}

			this->vecSave.clear();

			for (auto it : m.vecSave)
			{
				this->vecSave.emplace_back(it);
			}

			this->vecSaveAs.clear();

			for (auto it : m.vecSaveAs)
			{
				this->vecSaveAs.emplace_back(it);
			}

			this->vecInformation.clear();

			for (auto it : m.vecInformation)
			{
				this->vecInformation.emplace_back(it);
			}

			this->vecRuler.clear();

			for (auto it : m.vecRuler)
			{
				this->vecRuler.emplace_back(it);
			}

			this->vecVersion.clear();

			for (auto it : m.vecVersion)
			{
				this->vecVersion.emplace_back(it);
			}

			this->vecDocCategory.clear();

			for (auto it : m.vecDocCategory)
			{
				this->vecDocCategory.emplace_back(it);
			}

			this->vecDocJob.clear();

			for (auto it : m.vecDocJob)
			{
				this->vecDocJob.emplace_back(it);
			}

			this->vecDocCam.clear();

			for (auto it : m.vecDocCam)
			{
				this->vecDocCam.emplace_back(it);
			}

			this->vecDocLayer.clear();

			for (auto it : m.vecDocLayer)
			{
				this->vecDocLayer.emplace_back(it);
			}

			this->vecDocMeasure.clear();

			for (auto it : m.vecDocMeasure)
			{
				this->vecDocMeasure.emplace_back(it);
			}

			this->vecDocMinimap.clear();

			for (auto it : m.vecDocMinimap)
			{
				this->vecDocMinimap.emplace_back(it);
			}

			this->vecDocLog.clear();

			for (auto it : m.vecDocLog)
			{
				this->vecDocLog.emplace_back(it);
			}			

		}
		return *this;
	};


};


struct NSIS_AutoAlign_t
{	
	DOUBLE  dAutoAlignX;
	DOUBLE  dAutoAlignY;

	NSIS_AutoAlign_t() : dAutoAlignX(0), dAutoAlignY(0) {};
	NSIS_AutoAlign_t(const NSIS_AutoAlign_t& m)
	{
		dAutoAlignX = m.dAutoAlignX;
		dAutoAlignY = m.dAutoAlignY;		
	};

	NSIS_AutoAlign_t& operator=(const NSIS_AutoAlign_t& m)
	{
		if (this != &m)
		{
			this->dAutoAlignX = m.dAutoAlignX;
			this->dAutoAlignY = m.dAutoAlignY;			
		}
		return *this;
	};
};

struct AutoAlign_t
{
	BOOL	bAutoAlign;
	DOUBLE  dRectWidth;
	DOUBLE  dRectHeight;

	std::vector<NSIS_AutoAlign_t*>	vecAutoAlign;
	
	AutoAlign_t() : bAutoAlign(FALSE), dRectWidth(60), dRectHeight(60) {};
	~AutoAlign_t() { Clear(); }

	void Clear()
	{
		ClearAutoAlign();
	}

	void ClearAutoAlign()
	{
		for (auto it : vecAutoAlign)
		{
			delete it;
			it = nullptr;
		}
		vecAutoAlign.clear();
	}

	AutoAlign_t(const AutoAlign_t& m)
	{
		bAutoAlign = m.bAutoAlign;		
		dRectWidth = m.dRectWidth;
		dRectHeight = m.dRectHeight;

		for (auto it : m.vecAutoAlign)
		{
			NSIS_AutoAlign_t* pSpec = new NSIS_AutoAlign_t;
			*pSpec = *it;
			vecAutoAlign.emplace_back(pSpec);
		}
	};

	AutoAlign_t& operator=(const AutoAlign_t& m)
	{
		if (this != &m)
		{
			this->bAutoAlign = m.bAutoAlign;
			this->dRectWidth = m.dRectWidth;
			this->dRectHeight = m.dRectHeight;

			for (auto it : m.vecAutoAlign)
			{
				NSIS_AutoAlign_t* pSpec = new NSIS_AutoAlign_t;
				*pSpec = *it;
				this->vecAutoAlign.emplace_back(pSpec);
			}
			
		}
		return *this;
	};
};

struct NSISSpec_t
{
	INT32						sysCameraOrient_Top;
	INT32						sysCameraOrient_Bot;
	INT32						sysAlignRatio;
	INT32						sysUnitAlignRatio;
	INT32						sysNSISPort;
	INT32						sysCamPort;

	double						sysAlignLens;
	double						sysAlignZoom;
	double						sysUnitAlignLens;
	double						sysUnitAlignZoom;
	double						sysZoomMarginRatio;

	// 2022.10.04 KJH ADD
	// CheckLensZoom_SNU 위해 from SNU Code 추가 

	//YJD START 2021.11.29 - ALIGN EXPAND CAM
	bool						sysUseExCam;
	double						sysAlignExCamLens;
	double						sysAlignExCamZoom;
	//YJD END 2021.11.29 - ALIGN EXPAND CAM

	//YJD START 2022.04.08 - MEASURE TYPE SCALE
	double						sysMeasureDefaultScale;
	double						sysMeasureTypeScale[static_cast<UINT32>(MeasureType::MeasureTypeNone)];
	//YJD END 2022.04.08 - MEASURE TYPE SCALE

	//YJD START 22.06.13 - SAVE MEASURE IMAGE
	bool						sysUseSaveMeasurePoint;
	double						sysSaveMeasureScale;
	//YJD END 22.06.13 - SAVE MEASURE IMAGE

	// 2022.11.02. KJH ADD
	// Unit Layer Image Save Parameter
	bool						sysUseSaveUnitLayer;
		

	CString						sysNSISIP;
	CString						sysCamIP;

	std::vector<NSIS_CameraSpec_t*>	sys2DCamera;
	std::vector<NSIS_CameraSpec_t*>	sys3DCamera;

	NSISSpec_t() : sysCameraOrient_Top(0), sysCameraOrient_Bot(0), sysAlignRatio(0), sysUnitAlignRatio(0),
		sysNSISPort(0), sysCamPort(0), sysAlignLens(0.f), sysAlignZoom(0.f), sysUnitAlignLens(0.f), 
		sysUnitAlignZoom(0.f), sysZoomMarginRatio(0.f), sysNSISIP(L""), sysCamIP(L"") {};
	~NSISSpec_t() { Clear(); }

	void Clear()
	{
		Clear2DCamera();
		Clear3DCamera();
	}

	void Clear2DCamera()
	{
		for (auto it : sys2DCamera)
		{
			delete it;
			it = nullptr;
		}
		sys2DCamera.clear();
	}
	void Clear3DCamera()
	{
		for (auto it : sys3DCamera)
		{
			delete it;
			it = nullptr;
		}
		sys3DCamera.clear();
	}

	NSISSpec_t(const NSISSpec_t& m)
	{
		sysCameraOrient_Top = m.sysCameraOrient_Top;
		sysCameraOrient_Bot = m.sysCameraOrient_Bot;
		sysAlignRatio = m.sysAlignRatio;
		sysUnitAlignRatio = m.sysUnitAlignRatio;
		sysNSISPort = m.sysNSISPort;
		sysCamPort = m.sysCamPort;

		sysAlignLens = m.sysAlignLens;
		sysAlignZoom = m.sysAlignZoom;
		sysUnitAlignLens = m.sysUnitAlignLens;
		sysUnitAlignZoom = m.sysUnitAlignZoom;
		sysZoomMarginRatio = m.sysZoomMarginRatio;

		sysNSISIP = m.sysNSISIP;
		sysCamIP = m.sysCamIP;

		//YJD START 2021.11.29 - ALIGN EXPAND CAM
		sysUseExCam = m.sysUseExCam;
		sysAlignExCamLens = m.sysAlignExCamLens;
		sysAlignExCamZoom = m.sysAlignExCamZoom;
		//YJD END 2021.11.29 - ALIGN EXPAND CAM

		//YJD START 22.06.13 - SAVE MEASURE IMAGE
		sysUseSaveMeasurePoint = m.sysUseSaveMeasurePoint;
		sysSaveMeasureScale = m.sysSaveMeasureScale;
		//YJD END 22.06.13 - SAVE MEASURE IMAGE

		//YJD START 2022.04.08 - MEASURE TYPE SCALE
		for (int i = 0; i < (int)MeasureType::MeasureTypeNone; i++)
		{
			sysMeasureTypeScale[i] = m.sysMeasureTypeScale[i];
		}
		//YJD END 2022.04.08 - MEASURE TYPE SCALE

		// 2022.11.02 KJH ADD
		// Unit Layer Image Save Parameter
		sysUseSaveUnitLayer = m.sysUseSaveUnitLayer;

		for (auto it : m.sys2DCamera)
		{
			NSIS_CameraSpec_t* pSpec = new NSIS_CameraSpec_t;
			*pSpec = *it;
			sys2DCamera.emplace_back(pSpec);
		}

		for (auto it : m.sys3DCamera)
		{
			NSIS_CameraSpec_t* pSpec = new NSIS_CameraSpec_t;
			*pSpec = *it;
			sys3DCamera.emplace_back(pSpec);
		}
	};

	NSISSpec_t& operator=(const NSISSpec_t& m)
	{
		if (this != &m)
		{
			this->sysCameraOrient_Top = m.sysCameraOrient_Top;
			this->sysCameraOrient_Bot = m.sysCameraOrient_Bot;
			this->sysAlignRatio = m.sysAlignRatio;
			this->sysUnitAlignRatio = m.sysUnitAlignRatio;
			this->sysNSISPort = m.sysNSISPort;
			this->sysCamPort = m.sysCamPort;

			this->sysAlignLens = m.sysAlignLens;
			this->sysAlignZoom = m.sysAlignZoom;
			this->sysUnitAlignLens = m.sysUnitAlignLens;
			this->sysUnitAlignZoom = m.sysUnitAlignZoom;
			this->sysZoomMarginRatio = m.sysZoomMarginRatio;

			this->sysNSISIP = m.sysNSISIP;
			this->sysCamIP = m.sysCamIP;

			//YJD START 2021.11.29 - ALIGN EXPAND CAM
			this->sysUseExCam = m.sysUseExCam;
			this->sysAlignExCamLens = m.sysAlignExCamLens;
			this->sysAlignExCamZoom = m.sysAlignExCamZoom;
			//YJD END 2021.11.29 - ALIGN EXPAND CAM

			//YJD START 22.06.13 - SAVE MEASURE IMAGE
			this->sysUseSaveMeasurePoint = m.sysUseSaveMeasurePoint;
			this->sysSaveMeasureScale = m.sysSaveMeasureScale;
			//YJD END 22.06.13 - SAVE MEASURE IMAGE

			//YJD START 2022.04.08 - MEASURE TYPE SCALE
			for (int i = 0; i < (int)MeasureType::MeasureTypeNone; i++)
			{
				this->sysMeasureTypeScale[i] = m.sysMeasureTypeScale[i];
			}
			//YJD END 2022.04.08 - MEASURE TYPE SCALE

			// 2022.11.02 KJH ADD
			// Unit Layer Image Save Parameter
			this->sysUseSaveUnitLayer = m.sysUseSaveUnitLayer;

			for (auto it : m.sys2DCamera)
			{
				NSIS_CameraSpec_t* pSpec = new NSIS_CameraSpec_t;
				*pSpec = *it;
				this->sys2DCamera.emplace_back(pSpec);
			}

			for (auto it : m.sys3DCamera)
			{
				NSIS_CameraSpec_t* pSpec = new NSIS_CameraSpec_t;
				*pSpec = *it;
				this->sys3DCamera.emplace_back(pSpec);
			}
		}
		return *this;
	};
};

struct SystemSpec_t
{
	bool						bLoaded;
	BasicSpec_t					sysBasic;
	CameraSpec_t				sysCamera[2];
	PathSpec_t					sysPath;
	StepSpec_t					sysStep;
	LayerSpec_t					sysLayer;
	NSISSpec_t					sysNSIS;	//NSIS Sys Spec
	Language_t					sysLanguage;
	AutoAlign_t					sysAutoAlign; // NSIS SEM Auto Align
	

	SystemSpec_t() : bLoaded(false) {};
	~SystemSpec_t() { Clear(); }
	void Clear()
	{
		sysNSIS.Clear();
	}
	
	SystemSpec_t(const SystemSpec_t& m)
	{
		Clear();

		bLoaded = m.bLoaded;
		sysBasic = m.sysBasic;
		sysCamera[0] = m.sysCamera[0];
		sysCamera[1] = m.sysCamera[1];
		sysPath = m.sysPath;
		sysStep = m.sysStep;
		sysLayer = m.sysLayer;
		sysNSIS = m.sysNSIS;
		sysLanguage = m.sysLanguage;
		sysAutoAlign = m.sysAutoAlign;
	};

	SystemSpec_t& operator=(const SystemSpec_t& m)
	{
		if (this != &m)
		{
			Clear();

			this->bLoaded = m.bLoaded;
			this->sysBasic = m.sysBasic;
			this->sysCamera[0] = m.sysCamera[0];
			this->sysCamera[1] = m.sysCamera[1];
			this->sysPath = m.sysPath;
			this->sysStep = m.sysStep;
			this->sysLayer = m.sysLayer;
			this->sysNSIS = m.sysNSIS;			
			this->sysLanguage = m.sysLanguage;
			this->sysAutoAlign = m.sysAutoAlign;
		}
		return *this;
	};
};

struct ViewInfo_t
{
	HWND*	pHwnd;
	HDC		hDC;
	RECT	rcRect;
	POINT	ptSizeXY;

	ViewInfo_t() : pHwnd(nullptr){};
	ViewInfo_t(const ViewInfo_t& m)
	{
		pHwnd = m.pHwnd;
		hDC = m.hDC;
		rcRect = m.rcRect;
		ptSizeXY = m.ptSizeXY;
	};

	ViewInfo_t& operator=(const ViewInfo_t& m)
	{
		if (this != &m)
		{
			this->pHwnd = m.pHwnd;
			this->hDC = m.hDC;
			this->rcRect = m.rcRect;
			this->ptSizeXY = m.ptSizeXY;
		}
		return *this;
	};
};

struct LayerSet_t
{
	bool		bCheck;
	bool		bPolarity;
	UINT32	    index;
	COLORREF	color;
	PointDXY	dScaleXY;
	CString		strLayer;
	CString		strDefine;

	LayerSet_t() : bCheck(false), bPolarity(true), index(0), color(RGB(255,255,255)),
		dScaleXY(CPointD(1.0, 1.0)), strLayer(L""), strDefine(L"") {};
	LayerSet_t(const LayerSet_t& m)
	{
		bCheck = m.bCheck;
		bPolarity = m.bPolarity;
		index = m.index;
		color = m.color;
		dScaleXY = m.dScaleXY;
		strLayer = m.strLayer;
		strDefine = m.strDefine;
	};

	LayerSet_t& operator=(const LayerSet_t& m)
	{
		if (this != &m)
		{
			this->bCheck = m.bCheck;
			this->bPolarity = m.bPolarity;
			this->index = m.index;
			this->color = m.color;
			this->dScaleXY = m.dScaleXY;
			this->strLayer = m.strLayer;
			this->strDefine = m.strDefine;
		}
		return *this;
	};
};

struct UserLayerSet_t
{
	bool	bIsPreview;
	UINT32	iPreviewNum;
	UINT32  iSelectedNum;

	INT32   nPanelStepIdx;

	std::vector<INT32>	vcStripStepIdx;
	std::vector<INT32>	vcUnitStepIdx;

	std::vector<LayerSet_t*> vcLayerSet;

	UserLayerSet_t() : bIsPreview(false), iPreviewNum(0), iSelectedNum(0), nPanelStepIdx(-1) {
		vcLayerSet.clear();
	};
	~UserLayerSet_t() {	ClearStep(); ClearLayer(); }
	void ClearStep()
	{
		nPanelStepIdx = -1;
		vcStripStepIdx.clear();
		vcUnitStepIdx.clear();
	}
	void ClearLayer()
	{
//		bIsPreview = false;
		iPreviewNum = 0;
		iSelectedNum = 0;

		for (auto it : vcLayerSet)
		{
			delete it;
			it = nullptr;
		}
		vcLayerSet.clear();
	}
	UserLayerSet_t(const UserLayerSet_t& m)
	{
		ClearStep();
		ClearLayer();

		bIsPreview = m.bIsPreview;
		iPreviewNum = m.iPreviewNum;
		iSelectedNum = m.iSelectedNum;
		nPanelStepIdx = m.nPanelStepIdx;
		
		for (auto it : m.vcStripStepIdx)
			vcStripStepIdx.emplace_back(it);

		for (auto it : m.vcUnitStepIdx)
			vcUnitStepIdx.emplace_back(it);

		for (auto it : m.vcLayerSet)
		{
			LayerSet_t* pLayer = new LayerSet_t;
			*pLayer = *it;
			vcLayerSet.emplace_back(pLayer);
		}
	}
	UserLayerSet_t& operator=(const UserLayerSet_t& m)
	{
		if (this != &m)
		{
			this->ClearStep();
			this->ClearLayer();

			this->bIsPreview = m.bIsPreview;
			this->iPreviewNum = m.iPreviewNum;
			this->iSelectedNum = m.iSelectedNum;
			this->nPanelStepIdx = m.nPanelStepIdx;
			
			for (auto it : m.vcStripStepIdx)
				this->vcStripStepIdx.emplace_back(it);

			for (auto it : m.vcUnitStepIdx)
				this->vcUnitStepIdx.emplace_back(it);

			for (auto it : m.vcLayerSet)
			{
				LayerSet_t* pLayer = new LayerSet_t;
				*pLayer = *it;
				this->vcLayerSet.emplace_back(pLayer);
			}
		}
		return *this;
	};
};

struct ViewScale_t
{
	PointDXY dScaleXY;
	PointDXY dScaleOff;
	PointDXY dImageOff;
	RECTD	 rcProfile;

	ViewScale_t() : dScaleXY(CPointD(1.0, 1.0)), dScaleOff(CPointD(0, 0)), dImageOff(CPointD(0, 0)), rcProfile(0,0,0,0) {};
	ViewScale_t(const ViewScale_t& m)
	{
		dScaleXY = m.dScaleXY;
		dScaleOff = m.dScaleOff;
		dImageOff = m.dImageOff;
		rcProfile = m.rcProfile;
	};

	ViewScale_t& operator=(const ViewScale_t& m)
	{
		if (this != &m)
		{
			this->dScaleXY = m.dScaleXY;
			this->dScaleOff = m.dScaleOff;
			this->dImageOff = m.dImageOff;
			this->rcProfile = m.rcProfile;
		}
		return *this;
	};
};

struct ViewScaleInfo_t
{
	std::vector<ViewScale_t*> vcViewScale;

	ViewScaleInfo_t() {};
	~ViewScaleInfo_t() { Clear(); }
	void Clear()
	{
		for (auto it : vcViewScale)
		{
			delete it;
			it = nullptr;
		}
		vcViewScale.clear();
	}
	ViewScaleInfo_t(const ViewScaleInfo_t& m)
	{
		Clear();
		for (auto it : m.vcViewScale)
		{
			ViewScale_t* pScale = new ViewScale_t;
			*pScale = *it;
			vcViewScale.emplace_back(pScale);
		}
	}
	ViewScaleInfo_t& operator=(const ViewScaleInfo_t& m)
	{
		if (this != &m)
		{
			this->Clear();
			for (auto it : m.vcViewScale)
			{
				ViewScale_t* pScale = new ViewScale_t;
				*pScale = *it;
				this->vcViewScale.emplace_back(pScale);
			}
		}
		return *this;
	};
};

struct UserSetInfo_t
{
	bool			bShowStepRepeat;
	bool			bShowProfile;
	bool			bShowSurface;
	bool			bShowPanelDummy;		//for NSIS
	bool			bShowFOV;
	bool			bShowCell;
	bool			bMirror;
	bool			bSideChanged;
	bool			bShowUnitAlign;
	bool			bShowInspMask;

	enum eFovType		 fovType;
	enum eCoordinateUnit coordUnit;
	enum eProdSide		 prodSide;
	enum eDirectionType	 mirrorDir;
	enum eUserSite		 userSite;
	enum eMachineType	 mcType;
	enum eRecipeMode	 rcpMode;
	enum eProdProcess	 prodProcess;

	INT32			iStepIdx;
	DOUBLE			dAngle;
	RECTD			rcZoomRect;
	RECTD			rcDrawRect;
	CString			strWorkLayer;
	CString			strPairLayer;

	UserSetInfo_t() : bShowStepRepeat(false), bShowProfile(true), bShowSurface(false), bShowPanelDummy(false),
		bShowFOV(false), bShowCell(false), bMirror(false), bSideChanged(false), 
		bShowUnitAlign(false), bShowInspMask(false),
		fovType(eFovType::eVerticalJigjag),
		coordUnit(eCoordinateUnit::eInchToMM), prodSide(eProdSide::eTop), mirrorDir(eDirectionType::DirectionTypeNone), 
		userSite(eUserSite::UserSiteNone), mcType(eMachineType::MachineTypeNone), rcpMode(eRecipeMode::RecipeModeNone),
		prodProcess(eProdProcess::ePreSR),
		iStepIdx(0), dAngle(0),	rcZoomRect(0, 0, 0, 0), rcDrawRect(0, 0, 0, 0),	strWorkLayer(L""), strPairLayer(L""){};

	UserSetInfo_t(const UserSetInfo_t& m)
	{
		bShowStepRepeat = m.bShowStepRepeat;
		bShowProfile = m.bShowProfile;
		bShowSurface = m.bShowSurface;
		bShowPanelDummy = m.bShowPanelDummy;
		bShowFOV = m.bShowFOV;
		bShowCell = m.bShowCell;
		bMirror = m.bMirror;
		bSideChanged = m.bSideChanged;
		bShowUnitAlign = m.bShowUnitAlign;
		bShowInspMask = m.bShowInspMask;
		fovType = m.fovType;
		coordUnit = m.coordUnit;
		prodSide = m.prodSide;
		mirrorDir = m.mirrorDir;
		userSite = m.userSite;
		mcType = m.mcType;
		rcpMode = m.rcpMode;
		prodProcess = m.prodProcess;
		iStepIdx = m.iStepIdx;
		dAngle = m.dAngle;
		rcZoomRect = m.rcZoomRect;
		rcDrawRect = m.rcDrawRect;
		strWorkLayer = m.strWorkLayer;
		strPairLayer = m.strPairLayer;
	};

	UserSetInfo_t& operator=(const UserSetInfo_t& m)
	{
		if (this != &m)
		{
			this->bShowStepRepeat = m.bShowStepRepeat;
			this->bShowProfile = m.bShowProfile;
			this->bShowSurface = m.bShowSurface;
			this->bShowPanelDummy = m.bShowPanelDummy;
			this->bShowFOV = m.bShowFOV;
			this->bShowCell = m.bShowCell;
			this->bMirror = m.bMirror;
			this->bSideChanged = m.bSideChanged;
			this->bShowUnitAlign = m.bShowUnitAlign;
			this->bShowInspMask = m.bShowInspMask;
			this->fovType = m.fovType;
			this->coordUnit = m.coordUnit;
			this->prodSide = m.prodSide;
			this->mirrorDir = m.mirrorDir;
			this->userSite = m.userSite;
			this->mcType = m.mcType;
			this->rcpMode = m.rcpMode;
			this->prodProcess = m.prodProcess;
			this->iStepIdx = m.iStepIdx;
			this->dAngle = m.dAngle;
			this->rcZoomRect = m.rcZoomRect;
			this->rcDrawRect = m.rcDrawRect;
			this->strWorkLayer = m.strWorkLayer;
			this->strPairLayer = m.strPairLayer;
		}
		return *this;
	};
};

struct MeasureSpec_t
{
	double  dRange;

	double	dMin;//OOS : Spec
	double	dTarget;
	double  dMax;

	double	dMin_OOR;
	double	dMax_OOR;

	double	dMin_OOC;
	double	dMax_OOC;

	// 2022.05.08
	// 주석추가 김준호
	// OOC, OOR Min/Max 추가
	//MeasureSpec_t() : dRange(50), dMin(0), dTarget(0), dMax(0) {};
	MeasureSpec_t() : dRange(50), dMin(0), dTarget(0), dMax(0), dMin_OOC(0), dMax_OOC(0), dMin_OOR(0), dMax_OOR(0) {};
	MeasureSpec_t(const MeasureSpec_t& m)
	{
		dRange = m.dRange;
		dMin = m.dMin;
		dTarget = m.dTarget;
		dMax = m.dMax;

		dMin_OOR = m.dMin_OOR;
		dMax_OOR = m.dMax_OOR;

		dMin_OOC = m.dMin_OOC;
		dMax_OOC = m.dMax_OOC;
	}
	MeasureSpec_t& operator=(const MeasureSpec_t& m)
	{
		if (this != &m)
		{
			this->dRange = m.dRange;
			this->dMin = m.dMin;
			this->dTarget = m.dTarget;
			this->dMax = m.dMax;

			this->dMin_OOR = m.dMin_OOR;
			this->dMax_OOR = m.dMax_OOR;

			this->dMin_OOC = m.dMin_OOC;
			this->dMax_OOC = m.dMax_OOC;
		}
		return *this;
	};
};

struct StepRptInfo_t
{
	INT32	QuadStep;
	INT32	StripStep;
	INT32	UnitStep;
	POINT	QuadIdx;
	POINT	StripIdx;
	POINT	UnitIdx;

	StepRptInfo_t() { Init(); }
	void Init()
	{
		QuadStep = StripStep = UnitStep = -1;
		QuadIdx.x = QuadIdx.y = -1;
		StripIdx.x = StripIdx.y = -1;
		UnitIdx.x = UnitIdx.y = -1;
	}
	StepRptInfo_t(const StepRptInfo_t& m)
	{
		QuadStep = m.QuadStep;
		StripStep = m.StripStep;
		UnitStep = m.UnitStep;
		QuadIdx = m.QuadIdx;
		StripIdx = m.StripIdx;
		UnitIdx = m.UnitIdx;
	}
	StepRptInfo_t& operator=(const StepRptInfo_t& m)
	{
		if (this != &m)
		{
			this->QuadStep = m.QuadStep;
			this->StripStep = m.StripStep;
			this->UnitStep = m.UnitStep;
			this->QuadIdx = m.QuadIdx;
			this->StripIdx = m.StripIdx;
			this->UnitIdx = m.UnitIdx;
		}
		return *this;
	};
};

struct MeasureItem_t
{
	BOOL    bIs3D;
	UINT8   iScanMode;			//Mag. & Zoom 조합 (9종 for 3D, 6종 for2D)
	UINT8	iFeatureType;
	UINT8	iMeasureType;
	UINT8	iMeasureSide;
	INT32	iUnitIndex;
	INT32   iStripIndex;
	StepRptInfo_t stStepRpt;
	RECTD	MinMaxRect;
	RECTD	MinMaxRect_In_Panel;
	MeasureSpec_t	stSpec;
	CString	strMeasureType;
	double	dLens;
	double	dZoom;

	//YJD START 2021.11.29 - ALIGN EXPAND CAM
	bool bAlignExCam = false;
	//YJD END 2021.11.29 - ALIGN EXPAND CAM

	void*	pFeature;

	MeasureItem_t() : bIs3D(TRUE), iScanMode(-1), iFeatureType(UCHAR_MAX), iMeasureType(UCHAR_MAX), iMeasureSide(UCHAR_MAX), dLens(50.0), dZoom(1.0),
		iUnitIndex(-1), iStripIndex(-1), MinMaxRect(tagDoubleRect(0, 0, 0, 0)), pFeature(nullptr), strMeasureType(L""), MinMaxRect_In_Panel(tagDoubleRect(0, 0, 0, 0)){
		stStepRpt.Init();
	};

	MeasureItem_t(const MeasureItem_t& m)
	{
		bIs3D = m.bIs3D;
		iScanMode = m.iScanMode;
		iFeatureType = m.iFeatureType;
		iMeasureType = m.iMeasureType;
		iMeasureSide = m.iMeasureSide;
		iUnitIndex = m.iUnitIndex;
		iStripIndex = m.iStripIndex;
		stStepRpt = m.stStepRpt;
		MinMaxRect = m.MinMaxRect;
		MinMaxRect_In_Panel = m.MinMaxRect_In_Panel;
		stSpec = m.stSpec;
		pFeature = m.pFeature;
		dLens = m.dLens;
		dZoom = m.dZoom;
		strMeasureType = m.strMeasureType;

		//YJD START 2021.11.29 - ALIGN EXPAND CAM
		bAlignExCam = m.bAlignExCam;
		//YJD END 2021.11.29 - ALIGN EXPAND CAM
	}
	MeasureItem_t& operator=(const MeasureItem_t& m)
	{
		if (this != &m)
		{
			this->bIs3D = m.bIs3D;
			this->iScanMode = m.iScanMode;
			this->iFeatureType = m.iFeatureType;
			this->iMeasureType = m.iMeasureType;
			this->iMeasureSide = m.iMeasureSide;
			this->iUnitIndex = m.iUnitIndex;
			this->iStripIndex = m.iStripIndex;
			this->stStepRpt = m.stStepRpt;
			this->MinMaxRect = m.MinMaxRect;
			this->MinMaxRect_In_Panel = m.MinMaxRect_In_Panel;
			this->stSpec = m.stSpec;
			this->pFeature = m.pFeature;
			this->dLens = m.dLens;
			this->dZoom = m.dZoom;
			this->strMeasureType = m.strMeasureType;

			//YJD START 2021.11.29 - ALIGN EXPAND CAM
			this->bAlignExCam = m.bAlignExCam;
			//YJD END 2021.11.29 - ALIGN EXPAND CAM
		}
		return *this;
	};
};

struct MeasureUnit_t
{
	std::vector<MeasureItem_t*> vcAlign;	//Unit Align Info
	std::vector<MeasureItem_t*> vcPoints;	//Unit NSIS Measure Points
	std::vector<MeasureItem_t*> vcPoints_SR;//Unit SR Measure Points
	UINT8	iType;
	INT32	iStripIdx;
	INT32	iIndex;
	RECTD	UnitRect;
	RECTD	UnitRect_In_Panel;
	StepRptInfo_t stStepRpt;
	eMinimapMode eMapMode;

	MeasureUnit_t() {
		stStepRpt.Init();  eMapMode = eMinimapMode::MinimapModeNone;
	};
	~MeasureUnit_t() { Clear(); };
	void Clear()
	{
		for (auto it : vcAlign)
		{
			delete it;
			it = nullptr;
		}
		vcAlign.clear();

		for (auto it : vcPoints)
		{
			delete it;
			it = nullptr;
		}
		vcPoints.clear();

		for (auto it : vcPoints_SR)
		{
			delete it;
			it = nullptr;
		}
		vcPoints_SR.clear();
	}
	MeasureUnit_t(const MeasureUnit_t& m)
	{
		Clear();
		
		for (auto it : m.vcAlign)
		{
			MeasureItem_t* pMeasure = new MeasureItem_t;
			*pMeasure = *it;
			vcAlign.emplace_back(pMeasure);
		}

		for (auto it : m.vcPoints)
		{
			MeasureItem_t* pMeasure = new MeasureItem_t;
			*pMeasure = *it;
			vcPoints.emplace_back(pMeasure);
		}

		for (auto it : m.vcPoints_SR)
		{
			MeasureItem_t* pMeasure = new MeasureItem_t;
			*pMeasure = *it;
			vcPoints_SR.emplace_back(pMeasure);
		}

		iType = m.iType;
		iStripIdx = m.iStripIdx;
		iIndex = m.iIndex;
		UnitRect = m.UnitRect;
		UnitRect_In_Panel = m.UnitRect_In_Panel;
		stStepRpt = m.stStepRpt;
		eMapMode = m.eMapMode;
	}
	MeasureUnit_t& operator=(const MeasureUnit_t& m)
	{
		if (this != &m)
		{
			this->Clear();
			
			for (auto it : m.vcAlign)
			{
				MeasureItem_t* pMeasure = new MeasureItem_t;
				*pMeasure = *it;
				this->vcAlign.emplace_back(pMeasure);
			}

			for (auto it : m.vcPoints)
			{
				MeasureItem_t* pMeasure = new MeasureItem_t;
				*pMeasure = *it;
				this->vcPoints.emplace_back(pMeasure);
			}

			for (auto it : m.vcPoints_SR)
			{
				MeasureItem_t* pMeasure = new MeasureItem_t;
				*pMeasure = *it;
				this->vcPoints_SR.emplace_back(pMeasure);
			}

			this->iType = m.iType;
			this->iStripIdx = m.iStripIdx;
			this->iIndex = m.iIndex;
			this->UnitRect = m.UnitRect;
			this->UnitRect_In_Panel = m.UnitRect_In_Panel;
			this->stStepRpt = m.stStepRpt;
			this->eMapMode = m.eMapMode;
		}

		return *this;
	};
};

struct MeasurePanel_t
{
	std::vector<MeasureItem_t*> vcAlign;		//Panel Align Info
	std::vector<MeasureUnit_t*> vcUnits;		//Unit Info (Align + Measure Points)
	std::vector<MeasureUnit_t*> vcStripDummy;	//Strip Dummy Info (Align + Measure Points)
	std::map<INT32, RECTD*> vcUnitRects;		//Unit Rect for Drawing

	MeasurePanel_t() {};
	~MeasurePanel_t() { Clear(); };
	void Clear()
	{
		for (auto it : vcAlign)
		{
			delete it;
			it = nullptr;
		}
		vcAlign.clear();

		for (auto it : vcUnits)
		{
			it->Clear();
			delete it;
			it = nullptr;
		}
		vcUnits.clear();

		for (auto it : vcStripDummy)
		{
			it->Clear();
			delete it;
			it = nullptr;
		}
		vcStripDummy.clear();

		for (auto it : vcUnitRects)
		{
			delete it.second;
			it.second = nullptr;
		}
		vcUnitRects.clear();
	}
	MeasurePanel_t(const MeasurePanel_t& m)
	{
		Clear();

		for (auto it : m.vcAlign)
		{
			MeasureItem_t* pMeasure = new MeasureItem_t;
			*pMeasure = *it;
			vcAlign.emplace_back(pMeasure);
		}

		for (auto it : m.vcUnits)
		{
			MeasureUnit_t* pMeasure = new MeasureUnit_t;
			*pMeasure = *it;
			vcUnits.emplace_back(pMeasure);
		}

		for (auto it : m.vcStripDummy)
		{
			MeasureUnit_t* pMeasure = new MeasureUnit_t;
			*pMeasure = *it;
			vcStripDummy.emplace_back(pMeasure);
		}

		for (auto it : m.vcUnitRects)
		{
			RECTD* pRect = new RECTD;
			*pRect = *it.second;
			vcUnitRects.insert(std::make_pair(it.first, pRect));
		}
	}
	MeasurePanel_t& operator=(const MeasurePanel_t& m)
	{
		if (this != &m)
		{
			this->Clear();

			for (auto it : m.vcAlign)
			{
				MeasureItem_t* pMeasure = new MeasureItem_t;
				*pMeasure = *it;
				this->vcAlign.emplace_back(pMeasure);
			}

			for (auto it : m.vcUnits)
			{
				MeasureUnit_t* pMeasure = new MeasureUnit_t;
				*pMeasure = *it;
				this->vcUnits.emplace_back(pMeasure);
			}

			for (auto it : m.vcStripDummy)
			{
				MeasureUnit_t* pMeasure = new MeasureUnit_t;
				*pMeasure = *it;
				this->vcStripDummy.emplace_back(pMeasure);
			}

			for (auto it : m.vcUnitRects)
			{
				RECTD* pRect = new RECTD;
				*pRect = *it.second;
				this->vcUnitRects.insert(std::make_pair(it.first, pRect));
			}
					
		}
		return *this;
	};
};

struct UnitInfo_t
{
//	bool		bIsCropped;
	BOOL		bInspect;
	UINT32		iType;	
	UINT32		iStripIdx;
	UINT32		iBlockIdx;
	UINT32		iUnitIdx_Ori;
	UINT32		iUnitIdx;
	UINT32		iCellIdx;
	UINT32		iOrient;

	RECTD		rcRect;
	RECTD		rcRect_In_Panel;
	RECTD		rcProfile;
	RECTD		rcProfile_In_Panel;
	RECTD		rcFovRect;

	StepRptInfo_t stStepRpt;

	//hj.kim
	//Align/Mask Info
	std::vector<std::vector<RECTD>>	vcAlign;
	std::vector<std::vector<RECTD>>	vcMask;

	std::vector<std::vector<RECTD>>	vcAlign_In_Panel;
	std::vector<std::vector<RECTD>>	vcMask_In_Panel;

	UnitInfo_t() : bInspect(FALSE), iType(0), iStripIdx(0), iUnitIdx_Ori(0), iUnitIdx(0), iBlockIdx(0),
		rcRect(tagDoubleRect(0,0,0,0)), rcRect_In_Panel(tagDoubleRect(0, 0, 0, 0)),
		rcProfile(tagDoubleRect(0, 0, 0, 0)), rcProfile_In_Panel(tagDoubleRect(0, 0, 0, 0)),
		rcFovRect(tagDoubleRect(0, 0, 0, 0)), iCellIdx(0), iOrient(0) {
		stStepRpt.Init();
	};

	~UnitInfo_t() { Clear(); }

	void Clear()
	{
		vcAlign.clear();
		vcMask.clear();

		vcAlign_In_Panel.clear();
		vcMask_In_Panel.clear();
	}

	UnitInfo_t(const UnitInfo_t& m)
	{
		bInspect = m.bInspect;
		iType = m.iType;
		iStripIdx = m.iStripIdx;
		iBlockIdx = m.iBlockIdx;
		iUnitIdx_Ori = m.iUnitIdx_Ori;
		iUnitIdx = m.iUnitIdx;
		iCellIdx = m.iCellIdx;
		iOrient = m.iOrient;
		stStepRpt = m.stStepRpt;
		rcRect = m.rcRect;
		rcRect_In_Panel = m.rcRect_In_Panel;
		rcProfile = m.rcProfile;
		rcProfile_In_Panel = m.rcProfile_In_Panel;
		rcFovRect = m.rcFovRect;

		vcAlign = m.vcAlign;
		vcMask = m.vcMask;

		vcAlign_In_Panel = m.vcAlign_In_Panel;
		vcMask_In_Panel = m.vcMask_In_Panel;
	}
	UnitInfo_t& operator=(const UnitInfo_t& m)
	{
		if (this != &m)
		{
			this->bInspect = m.bInspect;
			this->iType = m.iType;
			this->iStripIdx = m.iStripIdx;
			this->iBlockIdx = m.iBlockIdx;
			this->iUnitIdx_Ori = m.iUnitIdx_Ori;
			this->iUnitIdx = m.iUnitIdx;
			this->iCellIdx = m.iCellIdx;
			this->iOrient = m.iOrient;
			this->stStepRpt = m.stStepRpt;
			this->rcRect = m.rcRect;
			this->rcRect_In_Panel = m.rcRect_In_Panel;
			this->rcProfile = m.rcProfile;
			this->rcProfile_In_Panel = m.rcProfile_In_Panel;
			this->rcFovRect = m.rcFovRect;

			this->vcAlign = m.vcAlign;
			this->vcMask = m.vcMask;

			this->vcAlign_In_Panel = m.vcAlign_In_Panel;
			this->vcMask_In_Panel = m.vcMask_In_Panel;
		}
		return *this;
	};
	
	static bool compY(const UnitInfo_t *t1, const UnitInfo_t *t2)
	{
		if (fabs(t1->rcProfile.left - t2->rcProfile.left) < EPSILON_DELTA2)
		{
			if (t1->rcProfile.top > t2->rcProfile.top) return true;
			else if (fabs(t1->rcProfile.top - t2->rcProfile.top) < EPSILON_DELTA2) return false;			
			else return false;
		}
		else if (t1->rcProfile.left > t2->rcProfile.left)
			return false;

		return true;
	}

	static bool compX(const UnitInfo_t *t1, const UnitInfo_t *t2)
	{
		if (fabs(t1->rcProfile.left - t2->rcProfile.left) < EPSILON_DELTA2)
		{
			if (t1->rcProfile.top > t2->rcProfile.top) return true;
			else if (fabs(t1->rcProfile.top - t2->rcProfile.top) < EPSILON_DELTA2) return false;
			else return false;
		}
		else if (t1->rcProfile.left < t2->rcProfile.left)
			return false;

		return true;
	}

	static bool comp_reverseY(const UnitInfo_t *t1, const UnitInfo_t *t2)
	{
		if (fabs(t1->rcProfile.left - t2->rcProfile.left) < EPSILON_DELTA2)
		{
			if (t1->rcProfile.top < t2->rcProfile.top) return true;
			else if (fabs(t1->rcProfile.top - t2->rcProfile.top) < EPSILON_DELTA2) return false;
			else return false;
		}
		else if (t1->rcProfile.left > t2->rcProfile.left)
			return false;

		return true;
	}

	static bool comp_reverseX(const UnitInfo_t *t1, const UnitInfo_t *t2)
	{
		if (fabs(t1->rcProfile.left - t2->rcProfile.left) < EPSILON_DELTA2)
		{
			if (t1->rcProfile.top < t2->rcProfile.top) return true;
			else if (fabs(t1->rcProfile.top - t2->rcProfile.top) < EPSILON_DELTA2) return false;
			else return false;
		}
		else if (t1->rcProfile.left < t2->rcProfile.left)
			return false;

		return true;
	}
};

struct BlockInfo_t
{
	UINT32		iBlockIdx;
	UINT32      iRowUnitNumInBlock;
	UINT32		iColUnitNumInBlock;
	RECTD		rcRect;///
	RECTD		rcRect_In_Panel;
	RECTD		rcProfile;

	std::vector<UnitInfo_t*> vcUnitInfo;

	BlockInfo_t() : iBlockIdx(0), iRowUnitNumInBlock(0), iColUnitNumInBlock(0), 
		rcRect(tagDoubleRect(0, 0, 0, 0)), rcRect_In_Panel(tagDoubleRect(0, 0, 0, 0)), rcProfile(tagDoubleRect(0, 0, 0, 0)) {};
	~BlockInfo_t() { Clear(); }
	void Clear()
	{
		iBlockIdx = 0;
		iRowUnitNumInBlock = 0;
		iColUnitNumInBlock = 0;
		rcRect = tagDoubleRect(0, 0, 0, 0);
		rcRect_In_Panel = tagDoubleRect(0, 0, 0, 0);
		rcProfile = tagDoubleRect(0, 0, 0, 0);

		int nInfoSize = static_cast<int>(vcUnitInfo.size());
		for ( int i = 0 ; i < nInfoSize ; i++)
		{
			if (vcUnitInfo[i] != nullptr)
			{
				delete vcUnitInfo[i];
				vcUnitInfo[i] = nullptr;
			}
		}
		vcUnitInfo.clear();
	}
	BlockInfo_t(const BlockInfo_t& m)
	{
		Clear();

		iBlockIdx = m.iBlockIdx;
		iRowUnitNumInBlock = m.iRowUnitNumInBlock;
		iColUnitNumInBlock = m.iColUnitNumInBlock;
		rcRect = m.rcRect;
		rcRect_In_Panel = m.rcRect_In_Panel;
		rcProfile = m.rcProfile;

		for (auto it : m.vcUnitInfo)
		{
			UnitInfo_t* pUnit = new UnitInfo_t;
			*pUnit = *it;
			vcUnitInfo.emplace_back(pUnit);
		}
	}
	BlockInfo_t& operator=(const BlockInfo_t& m)
	{
		if (this != &m)
		{
			this->Clear();

			this->iBlockIdx = m.iBlockIdx;
			this->iRowUnitNumInBlock = m.iRowUnitNumInBlock;
			this->iColUnitNumInBlock = m.iColUnitNumInBlock;
			this->rcRect = m.rcRect;
			this->rcRect_In_Panel = m.rcRect_In_Panel;
			this->rcProfile = m.rcProfile;

			for (auto it : m.vcUnitInfo)
			{
				UnitInfo_t* pUnit = new UnitInfo_t;
				*pUnit = *it;
				this->vcUnitInfo.emplace_back(pUnit);
			}
		}
		return *this;
	};

	static bool comp(const BlockInfo_t *t1, const BlockInfo_t *t2)
	{
		if (fabs(t1->rcProfile.left - t2->rcProfile.left) < EPSILON_DELTA2)
		{
			if (t1->rcProfile.top > t2->rcProfile.top) return true;
			if (fabs(t1->rcProfile.top - t2->rcProfile.top) < EPSILON_DELTA2) return false;			
			else return false;
		}
		else if (t1->rcProfile.left > t2->rcProfile.left)
			return false;

		return true;
	}
	static bool compX(const BlockInfo_t *t1, const BlockInfo_t *t2)
	{
		if (fabs(t1->rcProfile.left - t2->rcProfile.left) < EPSILON_DELTA2)
		{
			if (t1->rcProfile.top > t2->rcProfile.top) return true;
			else if (fabs(t1->rcProfile.top - t2->rcProfile.top) < EPSILON_DELTA2) return false;
			else return false;
		}
		else if (t1->rcProfile.left < t2->rcProfile.left)
			return false;

		return true;
	}
	static bool comp_reverseY(const BlockInfo_t *t1, const BlockInfo_t *t2)
	{
		if (fabs(t1->rcProfile.left - t2->rcProfile.left) < EPSILON_DELTA2)
		{
			if (t1->rcProfile.top < t2->rcProfile.top) return true;
			if (fabs(t1->rcProfile.top - t2->rcProfile.top) < EPSILON_DELTA2) return false;
			else return false;
		}
		else if (t1->rcProfile.left > t2->rcProfile.left)
			return false;

		return true;
	}

	static bool comp_reverseX(const BlockInfo_t *t1, const BlockInfo_t *t2)
	{
		if (fabs(t1->rcProfile.left - t2->rcProfile.left) < EPSILON_DELTA2)
		{
			if (t1->rcProfile.top < t2->rcProfile.top) return true;
			if (fabs(t1->rcProfile.top - t2->rcProfile.top) < EPSILON_DELTA2) return false;
			else return false;
		}
		else if (t1->rcProfile.left < t2->rcProfile.left)
			return false;

		return true;
	}
};

struct StripInfo_t
{
	BOOL		bInspect;
	UINT32		iType;
	UINT32		iStripIdx;
	UINT32      iRowBlockNumInStrip;
	UINT32		iColBlockNumInStrip;
	UINT32		iRowUnitNumInStrip;
	UINT32		iColUnitNumInStrip;
	UINT32		iOrient;
	
	RECTD		rcRect;
	RECTD		rcRect_In_Panel;
	RECTD		rcProfile;
	RECTD		rcRect_wo_Dummy;
	RECTD		rcProfile_wo_Dummy;
	CString     strStepName;

	StepRptInfo_t stStepRpt;

	std::vector<BlockInfo_t*> vcBlockInfo;
	std::vector<UnitInfo_t*> vcUnitInfo;

	//hj.kim
	//Align/Mask Info
	std::vector<std::vector<RECTD>>	vcAlign;
	std::vector<std::vector<RECTD>>	vcMask;

	std::vector<std::vector<RECTD>>	vcAlign_In_Panel;
	std::vector<std::vector<RECTD>>	vcMask_In_Panel;

	StripInfo_t() : bInspect(FALSE), iType(0), iStripIdx(0), iRowBlockNumInStrip(0), iColBlockNumInStrip(0),
		iRowUnitNumInStrip(0), iColUnitNumInStrip(0), rcRect(tagDoubleRect(0, 0, 0, 0)),
		rcRect_In_Panel(tagDoubleRect(0, 0, 0, 0)), rcProfile(tagDoubleRect(0, 0, 0, 0)), rcRect_wo_Dummy(tagDoubleRect(0, 0, 0, 0)),
		rcProfile_wo_Dummy(tagDoubleRect(0, 0, 0, 0)), iOrient(0), strStepName(L"") {
		stStepRpt.Init();
	};
	~StripInfo_t() { Clear(); }
	void Clear()
	{
		bInspect = FALSE;
		iType = 0;
		iStripIdx = 0;
		iRowBlockNumInStrip = 0;
		iColBlockNumInStrip = 0;
		iRowUnitNumInStrip = 0;
		iColUnitNumInStrip = 0;
		stStepRpt.Init();
		rcRect = tagDoubleRect(0, 0, 0, 0);
		rcRect_In_Panel = tagDoubleRect(0, 0, 0, 0);
		rcProfile = tagDoubleRect(0, 0, 0, 0);
		rcRect_wo_Dummy = tagDoubleRect(0, 0, 0, 0);
		rcProfile_wo_Dummy = tagDoubleRect(0, 0, 0, 0);
		strStepName = L"";

		if (vcBlockInfo.size() > 0)
		{
			int nBlockSize = static_cast<int>(vcBlockInfo.size());
			for (int i = 0 ; i < nBlockSize ; i++ )
			{
				if (vcBlockInfo[i] != nullptr)
				{
					delete vcBlockInfo[i];
					vcBlockInfo[i] = nullptr;
				}
			}
			vcBlockInfo.clear();
		}
		else
		{
			int nInfoSize = static_cast<int>(vcUnitInfo.size());
			for (int i = 0; i < nInfoSize; i++)
			{
				if (vcUnitInfo[i] != nullptr)
				{
					delete vcUnitInfo[i];
					vcUnitInfo[i] = nullptr;
				}
			}
			vcUnitInfo.clear();
		}		
		
		vcUnitInfo.clear();

		vcAlign.clear();
		vcMask.clear();

		vcAlign_In_Panel.clear();
		vcMask_In_Panel.clear();
	}
	StripInfo_t(const StripInfo_t& m)
	{
		Clear();

		bInspect = m.bInspect;
		iType = m.iType;
		iStripIdx = m.iStripIdx;
		iRowBlockNumInStrip = m.iRowBlockNumInStrip;
		iColBlockNumInStrip = m.iColBlockNumInStrip;
		iRowUnitNumInStrip = m.iRowUnitNumInStrip;
		iColUnitNumInStrip = m.iColUnitNumInStrip;
		stStepRpt = m.stStepRpt;
		rcRect = m.rcRect;
		rcRect_In_Panel = m.rcRect_In_Panel;
		rcProfile = m.rcProfile;
		rcRect_wo_Dummy = m.rcRect_wo_Dummy;
		rcProfile_wo_Dummy = m.rcProfile_wo_Dummy;
		strStepName = m.strStepName;

		if (m.vcBlockInfo.size() > 0)
		{
			std::vector<UnitInfo_t*> vecUnitInfo_Temp;

			for (auto it : m.vcBlockInfo)
			{
				BlockInfo_t* pBlock = new BlockInfo_t;
				*pBlock = *it;
				vcBlockInfo.emplace_back(pBlock);

				for (auto itNewBlock : pBlock->vcUnitInfo)
				{
					vecUnitInfo_Temp.push_back(itNewBlock);
				}
				
			}

			vcUnitInfo = vecUnitInfo_Temp;
		}
		else
		{
			for (auto it : m.vcUnitInfo)
			{
				UnitInfo_t* pUnit = new UnitInfo_t;
				*pUnit = *it;
				vcUnitInfo.emplace_back(pUnit);
			}
		}
		
		

		vcAlign = m.vcAlign;
		vcMask = m.vcMask;

		vcAlign_In_Panel = m.vcAlign_In_Panel;
		vcMask_In_Panel = m.vcMask_In_Panel;
	}
	StripInfo_t& operator=(const StripInfo_t& m)
	{
		if (this != &m)
		{
			this->Clear();

			this->bInspect = m.bInspect;
			this->iType = m.iType;
			this->iStripIdx = m.iStripIdx;
			this->iRowBlockNumInStrip = m.iRowBlockNumInStrip;
			this->iColBlockNumInStrip = m.iColBlockNumInStrip;
			this->iRowUnitNumInStrip = m.iRowUnitNumInStrip;
			this->iColUnitNumInStrip = m.iColUnitNumInStrip;
			this->stStepRpt = m.stStepRpt;
			this->rcRect = m.rcRect;
			this->rcRect_In_Panel = m.rcRect_In_Panel;
			this->rcProfile = m.rcProfile;
			this->rcRect_wo_Dummy = m.rcRect_wo_Dummy;
			this->rcProfile_wo_Dummy = m.rcProfile_wo_Dummy;
			this->strStepName = m.strStepName;


			if (m.vcBlockInfo.size() > 0)
			{
				std::vector<UnitInfo_t*> vecUnitInfo_Temp;

				for (auto it : m.vcBlockInfo)
				{
					BlockInfo_t* pBlock = new BlockInfo_t;
					*pBlock = *it;
					vcBlockInfo.emplace_back(pBlock);

					for (auto itNewBlock : pBlock->vcUnitInfo)
					{
						vecUnitInfo_Temp.push_back(itNewBlock);
					}

				}

				vcUnitInfo = vecUnitInfo_Temp;
			}
			else
			{
				for (auto it : m.vcUnitInfo)
				{
					UnitInfo_t* pUnit = new UnitInfo_t;
					*pUnit = *it;
					vcUnitInfo.emplace_back(pUnit);
				}
			}

			vcAlign = m.vcAlign;
			vcMask = m.vcMask;

			vcAlign_In_Panel = m.vcAlign_In_Panel;
			vcMask_In_Panel = m.vcMask_In_Panel;
		}
		return *this;
	};

	static bool compY(const StripInfo_t *t1, const StripInfo_t *t2) 
	{
		if (fabs(t1->rcProfile.left - t2->rcProfile.left) < EPSILON_DELTA2)
		{
			if (t1->rcProfile.top > t2->rcProfile.top) return true;
			else if (fabs(t1->rcProfile.top - t2->rcProfile.top) < EPSILON_DELTA2) return false;
			else return false;
		}
		else if (t1->rcProfile.left > t2->rcProfile.left)
			return false;

		return true;
	}

	static bool compX(const StripInfo_t *t1, const StripInfo_t *t2)
	{
		if (fabs(t1->rcProfile.left - t2->rcProfile.left) < EPSILON_DELTA2)
		{
			if (t1->rcProfile.top > t2->rcProfile.top) return true;
			else if (fabs(t1->rcProfile.top - t2->rcProfile.top) < EPSILON_DELTA2) return false;
			else return false;
		}
		else if (t1->rcProfile.left < t2->rcProfile.left)
			return false;

		return true;
	}

	static bool comp_reverseY(const StripInfo_t *t1, const StripInfo_t *t2)
	{
		if (fabs(t1->rcProfile.left - t2->rcProfile.left) < EPSILON_DELTA2)
		{
			if (t1->rcProfile.top < t2->rcProfile.top) return true;
			else if (fabs(t1->rcProfile.top - t2->rcProfile.top) < EPSILON_DELTA2) return false;
			else return false;
		}
		else if (t1->rcProfile.left > t2->rcProfile.left)
			return false;

		return true;
	}

	static bool comp_reverseX(const StripInfo_t *t1, const StripInfo_t *t2)
	{
		if (fabs(t1->rcProfile.left - t2->rcProfile.left) < EPSILON_DELTA2)
		{
			if (t1->rcProfile.top < t2->rcProfile.top) return true;
			else if (fabs(t1->rcProfile.top - t2->rcProfile.top) < EPSILON_DELTA2) return false;
			else return false;
		}
		else if (t1->rcProfile.left < t2->rcProfile.left)
			return false;

		return true;
	}
};

struct TypeInfo_t
{
	UINT32		nNum;
	UINT32		iOrient;
	SIZE		szProfileSize;	//[pxl]
	SIZE		szFeatureSize;	//[pxl]
	PointDXY	dProfileSize;	//[mm]
	PointDXY	dFeatureSize;	//[mm]
	CString		strStepName;

	TypeInfo_t()
	{
		nNum = 0;
		iOrient = 0;
		szProfileSize.cx = szProfileSize.cy = 0;
		szFeatureSize.cx = szFeatureSize.cy = 0;
		dProfileSize = dFeatureSize = CPointD(0, 0);
		strStepName = L"";
	}

	TypeInfo_t(const TypeInfo_t& m)
	{
		nNum = m.nNum;
		iOrient = m.iOrient;
		szProfileSize = m.szProfileSize;
		szFeatureSize = m.szFeatureSize;
		dProfileSize = m.dProfileSize;
		dFeatureSize = m.dFeatureSize;
		strStepName = m.strStepName;
	}
	TypeInfo_t& operator=(const TypeInfo_t& m)
	{
		if (this != &m)
		{
			this->nNum = m.nNum;
			this->iOrient = m.iOrient;
			this->szProfileSize = m.szProfileSize;
			this->szFeatureSize = m.szFeatureSize;
			this->dProfileSize = m.dProfileSize;
			this->dFeatureSize = m.dFeatureSize;
			this->strStepName = m.strStepName;
		}
		return *this;
	};
};

struct PanelInfo_t
{
	bool		bIsStripVertical;	//Strip 세로 배열
	UINT32		iStripTypeNum;
	UINT32		iRowStripNumInPanel;
	UINT32		iColStripNumInPanel;
	UINT32		iRowUnitNumInPanel;
	UINT32		iColUnitNumInPanel;
	INT32		iSelStripRow;
	INT32		iSelStripCol;
	INT32		iSelUnitRow;
	INT32		iSelUnitCol;
	RECTD		rcRect;				//ActiveRect : profile or feature
	RECTD		rcRect_wo_Dummy;
	RECTD		rcProfile_wo_Dummy;
	RECTD		rcRect_In_Panel;

	std::vector<TypeInfo_t*> vcStripType;
	std::vector<TypeInfo_t*> vcUnitType;

	std::vector<StripInfo_t*> vcStripInfo;
	std::vector<RECTD*> vcAlignMarkInfo;

	MeasurePanel_t	stMeasureInfo;				// Features for NSIS

	//hj.kim
	//Align/Mask Info
	std::vector<RECTD>	vcAlign_Draw;
	std::vector<RECTD>	vcMask_Draw;

	std::vector<RECTD>	vcAlign_In_Panel;
	std::vector<RECTD>	vcMask_In_Panel;
	
	PanelInfo_t() : bIsStripVertical(false),iStripTypeNum(0), iRowStripNumInPanel(0), iColStripNumInPanel(0), iRowUnitNumInPanel(0),
		iColUnitNumInPanel(0), iSelStripRow(-1), iSelStripCol(-1), iSelUnitRow(-1), iSelUnitCol(-1), rcRect(tagDoubleRect(0, 0, 0, 0)),
		rcRect_wo_Dummy(tagDoubleRect(0, 0, 0, 0)),	rcProfile_wo_Dummy(tagDoubleRect(0, 0, 0, 0)), rcRect_In_Panel(tagDoubleRect(0, 0, 0, 0)) {};
	~PanelInfo_t() { Clear(); }
	void Clear()
	{
		bIsStripVertical = false;
		iStripTypeNum = 0;
		iRowStripNumInPanel = 0;
		iColStripNumInPanel = 0;
		iRowUnitNumInPanel = 0;
		iColUnitNumInPanel = 0;
		iSelStripRow = -1;
		iSelStripCol = -1;
		iSelUnitRow = -1;
		iSelUnitCol = -1;
		rcRect = tagDoubleRect(0, 0, 0, 0);
		rcRect_wo_Dummy = tagDoubleRect(0, 0, 0, 0);
		rcProfile_wo_Dummy = tagDoubleRect(0, 0, 0, 0);
		rcRect_In_Panel = tagDoubleRect(0, 0, 0, 0);

		for (auto it : vcStripType)
		{
			delete it;
			it = nullptr;
		}
		vcStripType.clear();

		for (auto it : vcUnitType)
		{
			delete it;
			it = nullptr;
		}
		vcUnitType.clear();

		for (auto it : vcStripInfo)
		{
			it->Clear();
			delete it;
			it = nullptr;
		}
		vcStripInfo.clear();

		for (auto it : vcAlignMarkInfo)
		{
			delete it;
			it = nullptr;
		}
		vcAlignMarkInfo.clear();	
		
		stMeasureInfo.Clear();	

		vcAlign_Draw.clear();
		vcMask_Draw.clear();

		vcAlign_In_Panel.clear();
		vcMask_In_Panel.clear();
	}
	PanelInfo_t(const PanelInfo_t& m)
	{
		Clear();

		bIsStripVertical = m.bIsStripVertical;
		iStripTypeNum = m.iStripTypeNum;
		iRowStripNumInPanel = m.iRowStripNumInPanel;
		iColStripNumInPanel = m.iColStripNumInPanel;
		iRowUnitNumInPanel = m.iRowUnitNumInPanel;
		iColUnitNumInPanel = m.iColUnitNumInPanel;
		iSelStripRow = m.iSelStripRow;
		iSelStripCol = m.iSelStripCol;
		iSelUnitRow = m.iSelUnitRow;
		iSelUnitCol = m.iSelUnitCol;
		rcRect = m.rcRect;
		rcRect_wo_Dummy = m.rcRect_wo_Dummy;
		rcProfile_wo_Dummy = m.rcProfile_wo_Dummy;
		rcRect_In_Panel = m.rcRect_In_Panel;

		for (auto it : m.vcStripType)
		{
			TypeInfo_t* pType = new TypeInfo_t;
			*pType = *it;
			vcStripType.emplace_back(pType);
		}

		for (auto it : m.vcUnitType)
		{
			TypeInfo_t* pType = new TypeInfo_t;
			*pType = *it;
			vcUnitType.emplace_back(pType);
		}

		for (auto it : m.vcStripInfo)
		{
			StripInfo_t* pStrip = new StripInfo_t;
			*pStrip = *it;
			vcStripInfo.emplace_back(pStrip);
		}

		for (auto it : m.vcAlignMarkInfo)
		{
			RECTD* pRect = new RECTD;
			*pRect = *it;
			vcAlignMarkInfo.emplace_back(pRect);
		}
		
		stMeasureInfo = m.stMeasureInfo;

		vcAlign_Draw = m.vcAlign_Draw;
		vcMask_Draw = m.vcMask_Draw;

		vcAlign_In_Panel = m.vcAlign_In_Panel;
		vcMask_In_Panel = m.vcMask_In_Panel;
	}
	PanelInfo_t& operator=(const PanelInfo_t& m)
	{
		if (this != &m)
		{
			this->Clear();

			this->bIsStripVertical = m.bIsStripVertical;
			this->iStripTypeNum = m.iStripTypeNum;
			this->iRowStripNumInPanel = m.iRowStripNumInPanel;
			this->iColStripNumInPanel = m.iColStripNumInPanel;
			this->iRowUnitNumInPanel = m.iRowUnitNumInPanel;
			this->iColUnitNumInPanel = m.iColUnitNumInPanel;
			this->iSelStripRow = m.iSelStripRow;
			this->iSelStripCol = m.iSelStripCol;
			this->iSelUnitRow = m.iSelUnitRow;
			this->iSelUnitCol = m.iSelUnitCol;
			this->rcRect = m.rcRect;
			this->rcRect_wo_Dummy = m.rcRect_wo_Dummy;
			this->rcProfile_wo_Dummy = m.rcProfile_wo_Dummy;
			this->rcRect_In_Panel = m.rcRect_In_Panel;

			for (auto it : m.vcStripType)
			{
				TypeInfo_t* pType = new TypeInfo_t;
				*pType = *it;
				this->vcStripType.emplace_back(pType);
			}

			for (auto it : m.vcUnitType)
			{
				TypeInfo_t* pType = new TypeInfo_t;
				*pType = *it;
				this->vcUnitType.emplace_back(pType);
			}

			for (auto it : m.vcStripInfo)
			{
				StripInfo_t* pStrip = new StripInfo_t;
				*pStrip = *it;
				this->vcStripInfo.emplace_back(pStrip);
			}

			for (auto it : m.vcAlignMarkInfo)
			{
				RECTD* pRect = new RECTD;
				*pRect = *it;
				this->vcAlignMarkInfo.emplace_back(pRect);
			}
			
			this->stMeasureInfo = m.stMeasureInfo;
		}

		this->vcAlign_Draw = m.vcAlign_Draw;
		this->vcMask_Draw = m.vcMask_Draw;

		this->vcAlign_In_Panel = m.vcAlign_In_Panel;
		this->vcMask_In_Panel = m.vcMask_In_Panel;

		return *this;
	};
};

struct FovData_t
{
	UINT32  swathIdx;
	POINT	fovIdx;
	RECTD	rcDraw;
	RECTD	rcRectMM;
	RECTD	rcRectMM_In_Panel;
	RECT	rcRectPxl;
	RECT	rcOverlapPxl;	

	FovData_t()
	{
		swathIdx = 0;
		fovIdx.x = fovIdx.y = 0;
		memset(&rcDraw, 0, sizeof(RECTD));
		memset(&rcRectMM, 0, sizeof(RECTD));
		memset(&rcRectMM_In_Panel, 0, sizeof(RECTD));
		memset(&rcRectPxl, 0, sizeof(RECT));
		memset(&rcOverlapPxl, 0, sizeof(RECT));
	}

	FovData_t(const FovData_t& m)
	{
		swathIdx = m.swathIdx;
		fovIdx = m.fovIdx;
		rcDraw = m.rcDraw;
		rcRectMM = m.rcRectMM;
		rcRectMM_In_Panel = m.rcRectMM_In_Panel;
		rcRectPxl = m.rcRectPxl;
		rcOverlapPxl = m.rcOverlapPxl;
	}
	FovData_t& operator=(const FovData_t& m)
	{
		if (this != &m)
		{
			this->swathIdx = m.swathIdx;
			this->fovIdx = m.fovIdx;
			this->rcDraw = m.rcDraw;
			this->rcRectMM = m.rcRectMM;
			this->rcRectMM_In_Panel = m.rcRectMM_In_Panel;
			this->rcRectPxl = m.rcRectPxl;
			this->rcOverlapPxl = m.rcOverlapPxl;
		}
		return *this;
	};
	static bool comp(const FovData_t *t1, const FovData_t *t2)
	{
		if (t1->fovIdx.x == t2->fovIdx.x) { // x 좌표가 같다면
			return t1->fovIdx.y < t2->fovIdx.y; // y 좌표를 오름차순으로
		}

		return t1->fovIdx.x < t2->fovIdx.x; // x 좌표가 같지 않다면 x 좌표를 오름차순으로
	}
};

struct CellBasic_t
{
	SIZE		cellNum;
	SIZE		cellSize;
	SIZE		cellOverlap;
	SIZE		cell_In_Block;

	CellBasic_t() { Clear(); }
	~CellBasic_t() { }
	void Clear()
	{
		cellNum.cx = cellNum.cy = 0;
		cellSize.cx = cellSize.cy = 0;
		cellOverlap.cx = cellOverlap.cy = 0;
		cell_In_Block.cx = cell_In_Block.cy = 0;
	}
	CellBasic_t(const CellBasic_t& m)
	{
		Clear();

		cellNum = m.cellNum;
		cellSize = m.cellSize;
		cellOverlap = m.cellOverlap;
		cell_In_Block = m.cell_In_Block;
	}
	CellBasic_t& operator=(const CellBasic_t& m)
	{
		if (this != &m)
		{
			this->Clear();

			this->cellNum = m.cellNum;
			this->cellSize = m.cellSize;
			this->cellOverlap = m.cellOverlap;
			this->cell_In_Block = m.cell_In_Block;
		}
		return *this;
	};
};

struct CellMaskInfo_t
{
	std::vector<int> vcUnitIndex;//Mask가 포함되어 있는 Unit Index

	std::vector<RECTD> vcInPanel_mm;//판넬 에서의 좌표
	std::vector<RECTD> vcInCell_mm;

	std::vector<RECT> vcInCell_Pixel;

	CellMaskInfo_t() { Clear(); }
	~CellMaskInfo_t() { Clear(); }

	void Clear()
	{
		vcUnitIndex.clear();

		vcInPanel_mm.clear();
		vcInCell_mm.clear();
		vcInCell_Pixel.clear();
	}
};

struct CellAlignInfo_t
{
	std::vector<RECTD> vcInPanel_mm;//판넬 에서의 좌표
	std::vector<RECTD> vcInCell_mm;

	std::vector<RECT> vcInCell_Pixel;

	CellAlignInfo_t() { Clear(); }
	~CellAlignInfo_t() { Clear(); }

	void Clear()
	{
		vcInPanel_mm.clear();
		vcInCell_mm.clear();
		vcInCell_Pixel.clear();
	}
};

struct CellInfo_t
{
	bool bChanged;
	CellBasic_t stBasic;
	std::vector<FovData_t*> vcCellData;
	std::map<int, std::vector<UnitInfo_t*>> vcUnit_in_Cell;

	//
	std::map<int, std::vector<StripInfo_t*>> vcStrip_in_Cell;

	std::map<int, std::vector<CellAlignInfo_t>> vcAlign_in_Cell;//Cell 영역 안에 있는 Align 정보 
	std::map<int, std::vector<CellMaskInfo_t>> vcMask_in_Cell;//Cell 영역 안에 있는 Mask 정보 Vector

	CellInfo_t() { Clear(); }
	~CellInfo_t() { Clear(); }
	void Clear()
	{
		bChanged = false;
		stBasic.Clear();

		for (auto it : vcCellData)
		{
			delete it;
			it = nullptr;
		}

		vcCellData.clear();
		vcUnit_in_Cell.clear();
		vcStrip_in_Cell.clear();
		vcAlign_in_Cell.clear();
		vcMask_in_Cell.clear();
	}
	CellInfo_t(const CellInfo_t& m)
	{
		Clear();

		bChanged = m.bChanged;
		stBasic = m.stBasic;
		for (auto it : m.vcCellData)
		{
			FovData_t* pFov = new FovData_t;
			*pFov = *it;
			vcCellData.emplace_back(pFov);
		}

		for (auto it : m.vcUnit_in_Cell)
		{
			vcUnit_in_Cell.insert(std::make_pair(it.first, it.second));
		}

		for (auto it : m.vcStrip_in_Cell)
		{
			vcStrip_in_Cell.insert(std::make_pair(it.first, it.second));
		}

		for (auto it : m.vcAlign_in_Cell)
		{
			vcAlign_in_Cell.insert(std::make_pair(it.first, it.second));
		}

		for (auto it : m.vcMask_in_Cell)
		{
			vcMask_in_Cell.insert(std::make_pair(it.first, it.second));
		}
	}
	CellInfo_t& operator=(const CellInfo_t& m)
	{
		if (this != &m)
		{
			this->Clear();

			this->bChanged = m.bChanged;
			this->stBasic = m.stBasic;
			for (auto it : m.vcCellData)
			{
				FovData_t* pFov = new FovData_t;
				*pFov = *it;
				this->vcCellData.emplace_back(pFov);
			}

			for (auto it : m.vcUnit_in_Cell)
			{
				this->vcUnit_in_Cell.insert(std::make_pair(it.first, it.second));
			}

			for (auto it : m.vcStrip_in_Cell)
			{
				vcStrip_in_Cell.insert(std::make_pair(it.first, it.second));
			}

			for (auto it : m.vcAlign_in_Cell)
			{
				this->vcAlign_in_Cell.insert(std::make_pair(it.first, it.second));
			}

			for (auto it : m.vcMask_in_Cell)
			{
				this->vcMask_in_Cell.insert(std::make_pair(it.first, it.second));
			}
		}
		return *this;
	};
};

struct FovBasic_t
{
	eScanType	scanType;
	eFovType	fovType;
	SIZE		fovSize;
	SIZE		fovOverlap;
	SIZE		fovMargin;
	SIZE		prodSize;
	PointDXY	dummyMargin;
	UINT32		swathNum;
	UINT32		swathNumInBlock;
	double		resolution;
	
	FovBasic_t() { Clear(); }
	~FovBasic_t() { }
	void Clear()
	{
		scanType = eScanType::eLineScan;
		fovType = eFovType::eVerticalJigjag;
		fovSize.cx = fovSize.cy = 0;
		fovOverlap.cx = fovOverlap.cy = 0;
		fovMargin.cx = fovMargin.cy = 0;
		prodSize.cx = prodSize.cy = 0;
		dummyMargin.x = dummyMargin.y = 0;
		swathNum = swathNumInBlock = 0;
		resolution = 0.f;
	}
	FovBasic_t(const FovBasic_t& m)
	{
		Clear();

		scanType = m.scanType;
		fovType = m.fovType;
		fovSize = m.fovSize;
		fovOverlap = m.fovOverlap;
		fovMargin = m.fovMargin;
		prodSize = m.prodSize;
		dummyMargin = m.dummyMargin;
		swathNum = m.swathNum;
		swathNumInBlock = m.swathNumInBlock;
		resolution = m.resolution;
	}
	FovBasic_t& operator=(const FovBasic_t& m)
	{
		if (this != &m)
		{
			this->Clear();
			this->scanType = m.scanType;
			this->fovType = m.fovType;
			this->fovSize = m.fovSize;
			this->fovOverlap = m.fovOverlap;
			this->fovMargin = m.fovMargin;
			this->prodSize = m.prodSize;
			this->dummyMargin = m.dummyMargin;
			this->swathNum = m.swathNum;
			this->swathNumInBlock = m.swathNumInBlock;
			this->resolution = m.resolution;
		}
		return *this;
	};
};

struct FovInfo_t
{
	bool					bChanged;
	FovBasic_t				stBasic;
	std::vector<FovData_t*> vcFovData;
	std::vector<FovData_t*> vcFovData_Block;

	FovInfo_t() { Clear(); }
	~FovInfo_t() { Clear(); }
	void Clear()
	{
		bChanged = false;
		stBasic.Clear();

		for (auto it : vcFovData)
		{
			delete it;
			it = nullptr;
		}
		vcFovData.clear();

		for (auto it : vcFovData_Block)
		{
			delete it;
			it = nullptr;
		}
		vcFovData_Block.clear();
	}
	FovInfo_t(const FovInfo_t& m)
	{
		Clear();

		stBasic = m.stBasic;
		for (auto it : m.vcFovData)
		{
			FovData_t* pFov = new FovData_t;
			*pFov = *it;
			vcFovData.emplace_back(pFov);
		}

		for (auto it : m.vcFovData_Block)
		{
			FovData_t* pFov = new FovData_t;
			*pFov = *it;
			vcFovData_Block.emplace_back(pFov);
		}
	}
	FovInfo_t& operator=(const FovInfo_t& m)
	{
		if (this != &m)
		{
			this->Clear();
			
			this->stBasic = m.stBasic;
			for (auto it : m.vcFovData)
			{
				FovData_t* pFov = new FovData_t;
				*pFov = *it;
				this->vcFovData.emplace_back(pFov);
			}

			for (auto it : m.vcFovData_Block)
			{
				FovData_t* pFov = new FovData_t;
				*pFov = *it;
				this->vcFovData_Block.emplace_back(pFov);
			}
		}
		return *this;
	};
};

struct stLayerPair
{
	stLayerPair()
	{
		Clear();
	}
	~stLayerPair()
	{
		Clear();
	}
	void Clear()
	{
		strTop.Empty();
		strBot.Empty();
	}

	CString strTop;
	CString strBot;
};

struct stLayerInfoTemp
{
	INT32 iTotNum;
	vector<INT32> vecLayerNum;
	vector<INT32> vecMPLayerNum;
	vector<INT32> vecLayerNo;
	vector<INT32> vecMPLayerNo;

	vector<INT32> vecTPLayerNum;
	vector<INT32> vecTPLayerNo;

	stLayerInfoTemp() {}
	~stLayerInfoTemp() { Clear(); }
	bool Alloc(INT32 iNum)
	{
		Clear();
		if (iNum <= 0) 
			return false;

		iTotNum = iNum;
		vecLayerNum.resize(iTotNum);
		vecMPLayerNum.resize(iTotNum);
		vecLayerNo.resize(iTotNum);
		vecMPLayerNo.resize(iTotNum);

		vecTPLayerNum.resize(iTotNum);
		vecTPLayerNo.resize(iTotNum);

		for (int i = 0; i < iTotNum; i++)
		{
			vecLayerNum[i] = 0xFFFF;
			vecMPLayerNum[i] = 0xFFFF;
			vecLayerNo[i] = 0;
			vecMPLayerNo[i] = 0;

			vecTPLayerNum[i] = 0xFFFF;
			vecTPLayerNo[i] = 0;
		}
		return true;
	}
	void Clear()
	{
		vecLayerNum.clear();
		vecMPLayerNum.clear();
		vecLayerNo.clear();
		vecMPLayerNo.clear();

		vecTPLayerNum.clear();
		vecTPLayerNo.clear();
	}
	stLayerInfoTemp(const stLayerInfoTemp& m)
	{
		Alloc(m.iTotNum);

		vecLayerNum = m.vecLayerNum;
		vecMPLayerNum = m.vecMPLayerNum;
		vecLayerNo = m.vecLayerNo;
		vecMPLayerNo = m.vecMPLayerNo;

		vecTPLayerNum = m.vecTPLayerNum;
		vecTPLayerNo = m.vecTPLayerNo;
	}
	stLayerInfoTemp& operator=(const stLayerInfoTemp& m)
	{
		if (this != &m)
		{
			this->Alloc(m.iTotNum);

			for (int i = 0; i < iTotNum; i++)
			{
				this->vecLayerNum[i]	= m.vecLayerNum[i];
				this->vecMPLayerNum[i]	= m.vecMPLayerNum[i];
				this->vecLayerNo[i]		= m.vecLayerNo[i];
				this->vecMPLayerNo[i]	= m.vecMPLayerNo[i];

				this->vecTPLayerNum[i]	= m.vecTPLayerNum[i];
				this->vecTPLayerNo[i]	= m.vecTPLayerNo[i];
			}
		}
		return *this;
	};
};

typedef struct stRectInfo
{
	RECTD rcRect;
	RECTD rcRect_In_Panel;

	stRectInfo() :rcRect(tagDoubleRect(0, 0, 0, 0)), rcRect_In_Panel(tagDoubleRect(0, 0, 0, 0)) {}
	~stRectInfo() { }
	
	stRectInfo(const stRectInfo& m)
	{
		rcRect = m.rcRect;
		rcRect_In_Panel = m.rcRect_In_Panel;
	}
	stRectInfo& operator=(const stRectInfo& m)
	{
		if (this != &m)
		{
			this->rcRect = m.rcRect;
			this->rcRect_In_Panel = m.rcRect_In_Panel;
		}
		return *this;
	};
	static bool comp(const stRectInfo &t1, const stRectInfo &t2)
	{
		if (fabs(t1.rcRect.left - t2.rcRect.left) < EPSILON_DELTA2)
		{
			if (t1.rcRect.top > t2.rcRect.top) return true;
			else if (fabs(t1.rcRect.top - t2.rcRect.top) < EPSILON_DELTA2) return false;
			else return false;
		}
		else if (t1.rcRect.left > t2.rcRect.left)
			return false;

		return true;
	};

}RectInfo;

#endif