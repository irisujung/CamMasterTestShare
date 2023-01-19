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

#pragma comment(lib, "D2D1.lib")

#pragma comment(lib, "d2d1.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxguid.lib")

#include "Def_Viewer.h"


#include <vector>
using namespace std;

class CDrawFunction;

//mm로 변경시.
//#define CONVERT_MAG 700 //Render Target Size = Data Size x CONVERT_MAG

class CBackBuffer
{
	enum
	{
		//enumRenderTargetSize = 1024,
		enumRenderTargetSize = 10240,
		//enumRenderTargetSize = 15240,
		//enumRenderTargetSize = 20480,
		//enumRenderTargetSize = 30720,
	};

public:
	CBackBuffer();
	~CBackBuffer();

	void SetLink(IN ID2D1Factory1 *pFactory,IN ID2D1DeviceContext* p2DContext, IN SystemSpec_t *pSystemSpec);
	void SetPictureBox(IN const D2D1_RECT_F &frtPictureBox);

	void OnRender(IN  CLayer *pLayer, IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
		CRotMirrArr *pRotMirrArr, IN const UserSetInfo_t *pUserSetInfo);

	
	ID2D1Bitmap* GetDrawBitmap();

	D2D1_RECT_U NormalizeRect(IN const D2D1_RECT_U& rtRect);
	
public:


	
private:
	void DrawRender(ID2D1RenderTarget* pRender, IN  CLayer *pLayer, IN const LayerSet_t* pLayerSet,
		CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo);

	ID2D1Bitmap* GetOriginBitmap();
	D2D1_RECT_U  GetBitmapRect();

	void SaveImage(IWICBitmap *pBitmap, CString &strfilename);
private:

	IWICImagingFactory		*m_pImageFactory = nullptr;

	D2D1_SIZE_F m_sizeTarget = D2D1::SizeF(enumRenderTargetSize, enumRenderTargetSize);
	//vector<ID2D1BitmapRenderTarget*> m_vecRenderTarget;
	//vector<ID2D1BitmapRenderTarget*> m_vecRenderTarget;
	//vector<IWICBitmap *>	  m_vecWicBitmap; 

	ID2D1BitmapRenderTarget*	m_pRenderTarget = nullptr;

	ID2D1Bitmap* m_pBitmap = nullptr;
	
	//PictureBox
	D2D1_RECT_F m_frtPictureBox = D2D1::RectF(0.f, 0.f, 0.f, 0.f);

	D2D1::Matrix3x2F m_matDrawTransform;

	//DrawLayer
	CDrawFunction					*m_pDrawFunction = nullptr;

	//Split Rect
	int m_nSplitCountX = 0;
	int m_nSplitCountY = 0;
	vector<D2D1_RECT_F> m_vecSplitRect;
	
	//외부 Ptr
	//지우지 말것
	ID2D1Factory1 *m_pFactory = nullptr;
	ID2D1DeviceContext* m_p2DContext = nullptr;
	SystemSpec_t	*m_pSystemSpec = nullptr;

	
	
};

