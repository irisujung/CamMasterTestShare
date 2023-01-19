// CDirect_Base.cpp: 구현 파일
//
#include "pch.h"
#include "CDirect_Base.h"
#include "Def_Viewer.h"

// CDirect_Base


CDirect_Base::CDirect_Base()
{
	
}

CDirect_Base::~CDirect_Base()
{
}

// CDirect_Base 메시지 처리기

UINT32 CDirect_Base::SetSystemSpec(SystemSpec_t* pSystemSpec)
{
	if (pSystemSpec == nullptr) return RESULT_BAD;
	
	m_pSystemSpec = pSystemSpec;

	return RESULT_GOOD;
}

UINT32 CDirect_Base::SetUserSetInfo(UserSetInfo_t *pUserSetInfo, UserLayerSet_t *pUserLayerSetInfo)
{
	if (pUserSetInfo == nullptr) return RESULT_BAD;
	if (pUserLayerSetInfo == nullptr) return RESULT_BAD;

	m_pUserSetInfo = pUserSetInfo;
	m_pUserLayerSetInfo = pUserLayerSetInfo;

	return RESULT_GOOD;
}

UINT32 CDirect_Base::Initialize(IN CJobFile* pCAMData, IN ViewInfo_t* pViewInfo)
{
	if (pViewInfo == nullptr) return RESULT_BAD;

	m_pViewInfo = pViewInfo;

	// D2D1_FACTORY_TYPE_SINGLE_THREADED (0) : 싱글 스레드 기반의 Factory 객체 생성
	// D2D1_FACTORY_TYPE_MULTI_THREADED (1) : 멀티 스레드 기반의 Factory 객체 생성
	if (S_OK == D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &m_pFactory)) {
		//AfxMessageBox(_T("Factory 생성에 성공했습니다!"));
		// 성공했으면 p_factory에 사용 가능한 Factory 객체의 주소가 저장되어 있다.
// 
//  		ID2D1HwndRenderTarget *prender;
//  		m_pFactory->CreateHwndRenderTarget();
// 		prender->
// 
// 		prender->efe
		
		CRect r = m_pViewInfo->rcRect;

		/////////
		

		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		D3D_FEATURE_LEVEL featureLevel;
		if (D3D11CreateDevice(
			nullptr,                    // specify null to use the default adapter
			D3D_DRIVER_TYPE_HARDWARE,//D3D_DRIVER_TYPE_HARDWARE, //D3D_DRIVER_TYPE_REFERENCE,//D3D_DRIVER_TYPE_HARDWARE,
			0,
			creationFlags,              // optionally set debug and Direct2D compatibility flags
			featureLevels,              // list of feature levels this app can support
			ARRAYSIZE(featureLevels),   // number of possible feature levels
			D3D11_SDK_VERSION,
			&m_p3DDevice,                    // returns the Direct3D device created
			&featureLevel,            // returns feature level of device created
			&m_p3DContext                    // returns the device immediate context
		) != S_OK) return RESULT_BAD;

		if (m_p3DDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&m_pDxgiDevice) != S_OK) return RESULT_BAD;	  // Obtain the underlying DXGI device of the Direct3D11 device.
		if (m_pFactory->CreateDevice(m_pDxgiDevice, &m_pD2Device) != S_OK) return RESULT_BAD;

		if (m_pD2Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS, &m_pD2DContext) != S_OK) return RESULT_BAD;

		if (m_pDxgiDevice->GetAdapter(&m_pDxgiAdapter) != S_OK) return RESULT_BAD;	 // Identify the physical adapter (GPU or card) this device is runs on.	
		if (m_pDxgiAdapter->GetParent(IID_PPV_ARGS(&m_pDxgiFactory)) != S_OK) return RESULT_BAD;	 // Get the factory object that created the DXGI device.

		SetViewInfo(pViewInfo);		

	}
	else {
		//MessageBox(m_hWnd, _T("Factory 생성에 실패했습니다!"), _T("D2D1CreateFactory"), MB_ICONERROR);
		AfxMessageBox(_T("Factory 생성에 실패했습니다!"));
	}

	return RESULT_GOOD;
}

UINT32 CDirect_Base::SetViewInfo(IN ViewInfo_t* pViewInfo)
{//창크기가 변경되었을 때 호출
	if (m_pFactory == nullptr) return RESULT_BAD;
	if (m_pD2DContext == nullptr) return RESULT_BAD;
	if (pViewInfo == nullptr) return RESULT_BAD;
	
	m_pViewInfo = pViewInfo;
	m_nMaxLayer = m_pSystemSpec->sysBasic.iMaxLayerNum;

	BOOL bNeedMake = FALSE;
	int nRenderCount = static_cast<int>(m_vecRender.size());
	
	//RenderTarget을 신규로 만들어야 하는지 체크
	if (nRenderCount <= 0 && m_nMaxLayer != 0)
	{//아직 Initial 되지 않았다면, 만들고 시작.
		m_vecRender.resize(m_nMaxLayer, nullptr);
		m_vecRender_Substep.resize(m_nMaxLayer, nullptr);
		
		nRenderCount = m_nMaxLayer;
		bNeedMake = TRUE;
	}
	else if (m_vecRender[0] != nullptr &&
		(m_vecRender[0]->GetSize().width != pViewInfo->ptSizeXY.x ||
			m_vecRender[0]->GetSize().height != pViewInfo->ptSizeXY.y))
	{//View 크기가 달라진 경우 새로 만들자.
		
		for (int i = 0; i < nRenderCount; i++)
		{
			m_vecRender[i]->Release();
			m_vecRender[i] = nullptr;

			m_vecRender_Substep[i]->Release();
			m_vecRender_Substep[i] = nullptr;

		}

		bNeedMake = TRUE;
	}
	else if (m_vecRender[0] == nullptr)
	{
		bNeedMake = TRUE;
	}

	//신규로 만들어야 할 경우 
	if (bNeedMake == TRUE)
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		swapChainDesc.Width = 0;                           // use automatic sizing
		swapChainDesc.Height = 0;
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // this is the most common swapchain format
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;                // don't use multi-sampling
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;                     // use double buffering to enable flip
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // all apps must use this SwapEffect
		swapChainDesc.Flags = 0;

		if (m_pSwapChain1 != nullptr)
		{
			m_pSwapChain1->Release();
		}

		if (m_pDxgiFactory->CreateSwapChainForHwnd(m_p3DDevice, *m_pViewInfo->pHwnd,
			&swapChainDesc, nullptr, nullptr, &m_pSwapChain1) != S_OK) return RESULT_BAD;	 // Get the final swap chain for this window from the DXGI factory.

		D2D1_BITMAP_PROPERTIES1 bitmapProperties;
		bitmapProperties.dpiX = 96;
		bitmapProperties.dpiY = 96;
		bitmapProperties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
		bitmapProperties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
		bitmapProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
		bitmapProperties.colorContext = nullptr;

		if (m_pSwapChain1->GetBuffer(0, IID_PPV_ARGS(&m_pDxgiBackBuffer)) != S_OK) return RESULT_BAD;	 // Direct2D needs the dxgi version of the backbuffer surface pointer.

		if (m_pD2DContext->CreateBitmapFromDxgiSurface(m_pDxgiBackBuffer,
			&bitmapProperties, &m_pD2DBitmap) != S_OK) return RESULT_BAD;	 // Get a D2D surface from the DXGI back buffer to use as the D2D render target.

		m_pD2DContext->SetTarget(m_pD2DBitmap);
		// 		m_pD2DContext->BeginDraw();
		// 		m_pD2DContext->Clear(D2D1::ColorF(D2D1::ColorF::Black));
		// 		m_pD2DContext->EndDraw();

		if (m_pSwapChain1->Present(1, 0) != S_OK) return RESULT_BAD;

		
		m_vecRender.resize(m_nMaxLayer);
		m_vecRender_Substep.resize(m_nMaxLayer);

		ID2D1BitmapRenderTarget* pTempRender = nullptr;
		for (int i = 0; i < nRenderCount; i++)
		{
			if (m_pD2DContext->CreateCompatibleRenderTarget(D2D1::SizeF(static_cast<float>(m_pViewInfo->ptSizeXY.x),
				static_cast<float>(m_pViewInfo->ptSizeXY.y)), &pTempRender) == S_OK)
			{
				m_vecRender[i] = pTempRender;
			}

			if (m_pD2DContext->CreateCompatibleRenderTarget(D2D1::SizeF(static_cast<float>(m_pViewInfo->ptSizeXY.x),
				static_cast<float>(m_pViewInfo->ptSizeXY.y)), &pTempRender) == S_OK)
			{
				m_vecRender_Substep[i] = pTempRender;
			}
		}

		if (m_pCompositeEffect != nullptr)
		{
			m_pCompositeEffect->Release();
		}

		if (m_pD2DContext->CreateEffect(CLSID_D2D1Composite, &m_pCompositeEffect) != S_OK) return RESULT_BAD;
		if (m_pCompositeEffect->SetValue(D2D1_COMPOSITE_PROP_MODE, D2D1_COMPOSITE_MODE_PLUS) != S_OK) return RESULT_BAD;
	}

	return RESULT_GOOD;
}

void CDirect_Base::Render()
{
	if (nullptr == m_pFactory)
		return;	

	//Null Check
	for (int i = 0; i < m_nMaxLayer; i++)
	{
		if (m_vecRender[i] == nullptr) return;
		if (m_vecRender_Substep[i] == nullptr) return;
	}

	for (int i = 0; i < m_nMaxLayer; i++)
	{
		m_vecRender[i]->BeginDraw();
		m_vecRender[i]->Clear(0);

		m_vecRender_Substep[i]->BeginDraw();
		m_vecRender_Substep[i]->Clear(0);
	}

	//
	OnRender();
	
	HRESULT hRes;
	for (int i = 0; i < m_nMaxLayer; i++)
	{
		hRes = m_vecRender[i]->EndDraw();
		if (hRes != S_OK)
		{
			break;
		}

		hRes = m_vecRender_Substep[i]->EndDraw();
		if (hRes != S_OK)
		{
			break;
		}
	}

	//
	DrawOut();

	
}

void CDirect_Base::DrawOut()
{
	m_pD2DContext->BeginDraw();
	m_pD2DContext->SetTransform(D2D1::Matrix3x2F::Identity());

	m_pD2DContext->Clear(D2D1::ColorF(D2D1::ColorF::Black));	

	m_pD2DContext->DrawImage(m_pCompositeEffect);

	m_pD2DContext->EndDraw();

	if (m_pSwapChain1 != NULL)
	{
		SUCCEEDED(m_pSwapChain1->Present(1, 0));
	}
}

UINT32 CDirect_Base::ClearDisplay()
{
	for (int i = 0; i < m_nMaxLayer; i++)
	{
		m_vecRender[i]->BeginDraw();
		m_vecRender[i]->Clear(0);
	}

	for (int i = 0; i < m_nMaxLayer; i++)
	{
		m_vecRender[i]->EndDraw();
	}

	DrawOut();

	return RESULT_GOOD;
}

UINT32 CDirect_Base::Release()
{
	if (m_pFactory != nullptr)
	{
		m_pFactory->Release();
		m_pFactory = nullptr;
	}


	SafeRelease(&m_pCompositeEffect);
	SafeRelease(&m_pDxgiBackBuffer);
	SafeRelease(&m_pDxgiFactory);
	SafeRelease(&m_pDxgiAdapter);
	SafeRelease(&m_pDxgiDevice);
	SafeRelease(&m_pD2DContext);
	SafeRelease(&m_pD2Device);
	SafeRelease(&m_pD2DBitmap);
	SafeRelease(&m_pSwapChain1);
	SafeRelease(&m_p3DContext);
	SafeRelease(&m_p3DDevice);

	return RESULT_GOOD;
}
