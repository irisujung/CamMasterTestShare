#pragma once

#include <Wincodec.h>     // IWICImagingFactory를 사용하기 위해서 추가 
#include <d3d11.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <dxgi1_2.h>
#include <d2d1effects.h>

#include <d2d1_1.h>
#include <D2D1Helper.h>
#include <afxmt.h>

#pragma comment(lib, "D2D1.lib")

#pragma comment(lib, "d2d1.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxguid.lib")

#include "Def_Viewer.h"


#include <vector>
using namespace std;

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#define MAX_RENDER_WIDTH		16000
#define MAX_RENDER_PIXEL_COUNT	204800000//16000 * 16000 * 0.8//안정성을 위하여 80%만 사용


//mm로 변경시.
//#define CONVERT_MAG 700 //Render Target Size = Data Size x CONVERT_MAG

class CDrawFunction;
class CMakeRegionFunction;
class CDrawProfile;

class CSaveLayer
{
protected:
	

public:
	CSaveLayer();
	virtual ~CSaveLayer();

	void SetLink(IWICImagingFactory2 *pImageFactory, IN ID2D1Factory1 *pFactory,IN ID2D1DeviceContext* p2DContext);

	UINT32 SetPictureBox(IN const D2D1_RECT_F &frtPictureBox, double dResolution, BOOL bIsProfile = FALSE, std::mutex *pMutex = nullptr);

	void SetDrawMode(eDrawLevel e) { m_eDrawLevel = e; }

	void SetFovInfo(IN FovInfo_t* pFovInfo) {
		m_pFovInfo = pFovInfo;
	};

	virtual UINT32 SaveImage(IN const CString &strfilename, IN const double &dCamAngle,
		IN CStep *pStep, IN LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
		CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo, IN const BOOL &bSaveRegion = TRUE,
		IN const SaveMode eSaveMode = SaveMode::SaveMode_All, IN bool bExceptDummy = false, IN CString strLineArcWidth = _T(""),
		double dAngle = 0.f, bool bMirror = false) = 0;

	virtual UINT32 _CreateMergeImage(IN const CString &strfilename, IN BOOL bIsSaveRaw = FALSE) = 0;

	//필터링된 Feaure를 넣어주면 출력
	virtual UINT32 SaveImage_FilteredFeature(IN const CString &strFileName,
		vector<CFeature *> vecFeature,
		IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo) = 0;

	UINT32 SaveProfile(IN const CString &strfilename, IN CStep *pStep, CRotMirrArr *pRotMirrArr, IN const BOOL &bSubDraw, IN const BOOL &bSubSubDraw, IN BOOL bSubSubSubDraw = TRUE);
		
public:
	CEvent	m_hFinishEvent;

	
protected:

	UINT32 _BeginDraw(IN const BOOL &bClear = TRUE);
	HRESULT _EndDraw();
	
	void	_SetTransform(ID2D1RenderTarget* pRender);
	void	_SetViewScale(IN const float &fScale, IN const D2D_POINT_2F &fptPan);

	UINT32	_SaveImage(ID2D1Bitmap *pBitmap, IN const CString &strfilename);
	UINT32	_SaveImage(IWICBitmapSource *pBitmap, IN const CString &strfilename);

	UINT32 _ClearRender();
	UINT32 _MakeRender(IN const double &dResolution, IN const BOOL &bIsProfile);

protected:
	bool					m_bClearOption = true;
	//ID2D1BitmapRenderTarget *m_pBmpRenderTarget = nullptr;

	IWICBitmap						*m_pWicBitmap = nullptr;
	ID2D1RenderTarget				*m_pImageRender = nullptr;
	
	//PictureBox
	D2D1_RECT_F				m_frtPictureBox = D2D1::RectF(0.f, 0.f, 0.f, 0.f);

	D2D1::Matrix3x2F		m_matDrawTransform;

	//
	eDrawLevel						m_eDrawLevel = eDrawLevel::eUnitLv;
	
	//
	D2D1_SIZE_F					m_sizeTarget = D2D1::SizeF(0, 0);//Output 영상의 최종 크기
	CPoint						m_ptSplitCount = CPoint(0, 0);//분할 저장 할 때의 분할 갯수
	float						m_fSplitScale = 0.f;
	const int					m_nOverlapPixel = 100;
	double						m_dResolution = 0.0;
	D2D1_SIZE_F					m_sizeSplitImage = D2D1::SizeF(0, 0);//Split Image 크기

	//SplitImage Pos
	vector<vector<SPLIT_POS>>	m_vecvecSplitPos;
	


	//DrawLayer
	CDrawFunction					*m_pDrawFunction = nullptr;
	
	//Draw Profile
	CDrawProfile					*m_pDrawProfile = nullptr;
	
	//외부 Ptr
	//지우지 말것
	ID2D1Factory1					*m_pFactory = nullptr;
	ID2D1DeviceContext				*m_pD2DContext = nullptr;
	IWICImagingFactory2				*m_pImageFactory = nullptr;
	FovInfo_t						*m_pFovInfo = nullptr;
	std::mutex						*m_pMutex = nullptr;
		
};

