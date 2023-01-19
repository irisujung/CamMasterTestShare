#pragma once

#include <vector>
using namespace std;

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
// CDirect_Base

#pragma comment(lib, "d2d1.lib")
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxguid.lib")


//
#include "../../iCommonIF/CommonIF-Struct.h"
#include "../../iCommonIF/CommonIF-Error.h"
#include "../../iCommonIF/CommonIF-Define.h"
#include "../../iCommonIF/CommonIF-Path.h"
#include "../../iUtilities/iUtilities.h"
#include "../../iODB/iODB.h"


class __declspec(dllexport) CDirect_Base
{

public:
	CDirect_Base();
	virtual ~CDirect_Base();

	UINT32 SetSystemSpec(SystemSpec_t* pSystemSpec);
	UINT32 SetUserSetInfo(UserSetInfo_t *pUserSetInfo, UserLayerSet_t *pUserLayerSetInfo);
	UINT32 Initialize(IN CJobFile* pCAMData, IN ViewInfo_t* pViewInfo);
	
	UINT32 SetViewInfo(IN ViewInfo_t* pViewInfo);//창크기가 변경되었을 때 호출

	virtual UINT32 ClearDisplay();

	//Destroy
	UINT32 Release();


protected:

	virtual void OnRender() = 0;

	
	
protected:
	ID2D1Factory1 *m_pFactory = nullptr;

	//
	ID3D11Device*					m_p3DDevice;     // the pointer to our Direct3D device interface
	ID3D11DeviceContext*			m_p3DContext;	// the pointer to our Direct3D device context
	IDXGIDevice1*					m_pDxgiDevice;
	ID2D1Device*					m_pD2Device;
	ID2D1DeviceContext*				m_pD2DContext;
	IDXGIAdapter*					m_pDxgiAdapter;
	IDXGIFactory2*					m_pDxgiFactory;
	IDXGISwapChain1*				m_pSwapChain1 = nullptr;
	ID2D1Effect*					m_pCompositeEffect = nullptr;
	IDXGISurface1*					m_pDxgiBackBuffer;
	ID2D1Bitmap1*					m_pD2DBitmap;

	int m_nMaxLayer = 0;
	vector<ID2D1BitmapRenderTarget*> m_vecRender;
	vector<ID2D1BitmapRenderTarget*> m_vecRender_Substep;
	//

protected:
	
	D2D1::Matrix3x2F m_ViewScale;//Scale + Panning 정보

	//외부 포인터
	//지우지 말것
	SystemSpec_t	*m_pSystemSpec = nullptr;
	ViewInfo_t		*m_pViewInfo = nullptr;
	CJobFile		*m_pJobData = nullptr;

	UserSetInfo_t 	*m_pUserSetInfo = nullptr;
	UserLayerSet_t	*m_pUserLayerSetInfo = nullptr;


protected:
	void DrawOut();
		
public:

	void Render();

private:
	
	



};


