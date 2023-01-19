#include "pch.h"
#include "D3DXBase .h"
#include "DDS.h"

CD3DXBase::CD3DXBase()
{

}

CD3DXBase::~CD3DXBase()
{
	CleanUp();
}

void CD3DXBase::SetActiveTextureIndex(int iSurface)
{
	if (iSurface >= m_iTextureCount)	
		m_iActiveTexture = 0;	
	else	
		m_iActiveTexture = iSurface;	
}

bool CD3DXBase::InitializeResources(int iSurfaceCount, CWnd *pWndParent)
{
	m_bInitialized = false;

	if (pWndParent == nullptr) return false;

	// check for Direct3D9Ex support to solve problem with lost device when computer is locked
	const HMODULE c_hDirect3D = ::GetModuleHandle(_T("d3d9.dll"));

	if (!c_hDirect3D)	
		return false;	

	if (iSurfaceCount < 1) 
		return false;

	if (m_pclTexture != NULL)
	{
		MessageBox(NULL, _T("Invalid control state."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		return false;
	}

	using Direct3DCreate9ExFunction = HRESULT(WINAPI *)(UINT, IDirect3D9Ex **);

	const auto c_fpDirect3D9ExCreator =
		reinterpret_cast<Direct3DCreate9ExFunction>(::GetProcAddress(
			c_hDirect3D, "Direct3DCreate9Ex"));

	ATL::CComPtr< IDirect3D9 > pclDirect3D;

	if (c_fpDirect3D9ExCreator)
	{
		IDirect3D9Ex *pclDirect3D9Ex = NULL;
		Direct3DCreate9Ex(D3D_SDK_VERSION, &pclDirect3D9Ex);

		if (pclDirect3D9Ex)		
			pclDirect3D.Attach(pclDirect3D9Ex);		
	}
	else
	{
		pclDirect3D.Attach(Direct3DCreate9(D3D_SDK_VERSION));
	}

	if (!pclDirect3D)	
		return false;	

	D3DCAPS9 stDeviceCaps;
	if (FAILED(pclDirect3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &stDeviceCaps)))	
		return false;	

	m_szMaxSupportedSize.cx = stDeviceCaps.MaxTextureWidth;
	m_szMaxSupportedSize.cy = stDeviceCaps.MaxTextureHeight;

	const DWORD c_dwDeviceBehaviorFlags =
		D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE |
		((D3DSHADER_VERSION_MAJOR(stDeviceCaps.VertexShaderVersion) >= 2) ?
			D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING);

	::ZeroMemory(&m_stPresentParams, sizeof(D3DPRESENT_PARAMETERS));

	m_stPresentParams.Windowed = TRUE;
	m_stPresentParams.hDeviceWindow = pWndParent->GetSafeHwnd();
	m_stPresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_stPresentParams.BackBufferFormat = D3DFMT_A8R8G8B8;
	m_stPresentParams.BackBufferWidth = 1;
	m_stPresentParams.BackBufferHeight = 1;

	if (c_fpDirect3D9ExCreator)
	{
		IDirect3DDevice9Ex *pclDevice9Ex = NULL;

		static_cast<IDirect3D9Ex *>(pclDirect3D.p)->CreateDeviceEx(
			D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, c_dwDeviceBehaviorFlags, &m_stPresentParams,
			NULL, &pclDevice9Ex);

		if (!pclDevice9Ex)		
			return false;		

		m_pDirect3DDevice.Attach(pclDevice9Ex);
	}
	else
	{
		if (FAILED(pclDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL,
			c_dwDeviceBehaviorFlags, &m_stPresentParams, &m_pDirect3DDevice)))		
			return false;		
	}

	if (!ResetSwapChain(pWndParent))	
		return false;	

	if (FAILED(D3DXCreateSprite(m_pDirect3DDevice, &m_pSprite)))	
		return false;	

	m_pclTexture = new LPDIRECT3DTEXTURE9[iSurfaceCount];

	m_pbImageLoaded = new bool[iSurfaceCount];

	m_pstSurfaceSize = new SIZE[iSurfaceCount];
	memset(m_pstSurfaceSize, 0, sizeof(SIZE) * iSurfaceCount);

	m_iTextureCount = iSurfaceCount;

	for (int iLoop = 0; iLoop < m_iTextureCount; iLoop++)
	{
		m_pclTexture[iLoop] = nullptr;
		m_pbImageLoaded[iLoop] = false;
	}

	m_bInitialized = true;

	return true;
}

VOID WINAPI texFill(D3DXVECTOR4* pOut, CONST D3DXVECTOR2* pTexCoord, CONST D3DXVECTOR2* pTexelSize, LPVOID pData)
{
	auto *pTexData = (TexelFill_t*)pData;
	float fCol_R = 0.0, fCol_G = 0.0, fCol_B = 0.0;

	switch (pTexData->enImgType)
	{
		case ImageType_en::EN_8_BPP:
		{
			__int64 iRow = int(pTexCoord->y * pTexData->iHeight);
			__int64 iCol = int(pTexCoord->x * pTexData->iWidth);

			__int64 iPixIndex = iRow * pTexData->iWidth + iCol;

			fCol_R = (float)((BYTE*)pTexData->pData)[iPixIndex] /*/ 255*/;
			fCol_G = fCol_R;
			fCol_B = fCol_R;
		}
		break;

		case ImageType_en::EN_1_BPP:
		{
			__int64 iRow = int(pTexCoord->y * pTexData->iHeight);
			__int64 iCol = int(pTexCoord->x * pTexData->iWidth);

			__int64 iBitIndex = iRow * pTexData->iWidth + iCol;

			__int64 iByteIndex = iBitIndex / 8;
			__int64 iBitInByteIndex = iBitIndex % 8;

			BYTE bTargetByte = ((BYTE*)pTexData->pData)[iByteIndex];
			bTargetByte = bTargetByte >> (7 - iBitInByteIndex);
			bool bBitStatus = (bool)bTargetByte;

			if (bBitStatus)
				fCol_R = 1.0;
			else
				fCol_R = 0.0;

			fCol_G = fCol_R;
			fCol_B = fCol_R;

		}
		break;

		case  ImageType_en::EN_16_BPP:
		{
			//RGB 565

			__int64 iRow = int(pTexCoord->y * pTexData->iHeight);
			__int64 iCol = int(pTexCoord->x * pTexData->iWidth);

			__int64 iTupleIndex = iRow * pTexData->iWidth + iCol;

			__int64 iByteIndex = iTupleIndex * 2;

			WORD wPixelVal = 0;
			wPixelVal = ((BYTE*)pTexData->pData)[iByteIndex];
			wPixelVal = wPixelVal << 8;
			wPixelVal = wPixelVal || ((BYTE*)pTexData->pData)[iByteIndex + 1];

			WORD red_mask = 0xF800;
			WORD green_mask = 0x7E0;
			WORD blue_mask = 0x1F;


			fCol_R = static_cast<float>(((wPixelVal & red_mask) >> 11) / 255.);
			fCol_G = static_cast<float>(((wPixelVal & green_mask) >> 5) / 255.);
			fCol_B = static_cast<float>(((wPixelVal & blue_mask)) / 255.);

		}
		break;

		case  ImageType_en::EN_24_BPP:
		{
			//RGB 565

			__int64 iRow = int(pTexCoord->y * pTexData->iHeight);
			__int64 iCol = int(pTexCoord->x * pTexData->iWidth);

			__int64 iTupleIndex = iRow * pTexData->iWidth + iCol;

			__int64 iByteIndex = iTupleIndex * 3;

			fCol_R = static_cast<float>(((BYTE*)pTexData->pData)[iByteIndex] / 255.);
			fCol_G = static_cast<float>(((BYTE*)pTexData->pData)[iByteIndex + 1] / 255.);
			fCol_B = static_cast<float>(((BYTE*)pTexData->pData)[iByteIndex + 2] / 255.);

		}
		break;
	}

	*pOut = D3DXVECTOR4(fCol_R, fCol_G, fCol_B, 1.f);
}

bool CD3DXBase::GenerateTextureFromRawBuffer(BYTE *pBuff, int iWidth, int iHeight, ImageType_en enImgType, int iSurfaceIndex)
{
	HRESULT hRes;
	CRect clRect;

	m_dZoomLevel = 1.0;
	m_stScaling.x = 0.0;
	m_stScaling.y = 0.0;

	m_stTranslation.x = 0.0;
	m_stTranslation.y = 0.0;

	m_stRotationPoint.x = 0.0;
	m_stRotationPoint.y = 0.0;

	if (iSurfaceIndex >= m_iTextureCount)
		return false;

	m_pbImageLoaded[iSurfaceIndex] = false;	
	m_pstSurfaceSize[iSurfaceIndex].cx = iWidth;
	m_pstSurfaceSize[iSurfaceIndex].cy = iHeight;
	m_enImgFormat = D3DFMT_P8;

	if ((iWidth < 1) || (iHeight < 1))
	{
		MessageBox(NULL, _T("Direct X not initialized."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		return false;
	}

	if (m_pclTexture[iSurfaceIndex] != nullptr)
	{
		m_pclTexture[iSurfaceIndex]->Release();
		m_pclTexture[iSurfaceIndex] = nullptr;
	}

	hRes = D3DXCreateTexture(m_pDirect3DDevice, iWidth, iHeight, 0, 0, D3DFMT_R8G8B8, D3DPOOL_DEFAULT, &(m_pclTexture[iSurfaceIndex]));
	
	if (FAILED(hRes))
	{
		if (hRes == D3DERR_NOTAVAILABLE)
		{
			MessageBox(NULL, _T("Direct X texture not available."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		}
		else if (hRes == D3DERR_OUTOFVIDEOMEMORY)
		{
			MessageBox(NULL, _T("Direct X texture out of video memory."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		}
		else if (hRes == D3DERR_INVALIDCALL)
		{
			MessageBox(NULL, _T("Direct X texture invalid call."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		}
		else if (hRes == D3DXERR_INVALIDDATA)
		{
			MessageBox(NULL, _T("Direct X texture invalid data."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		}
		else if (hRes == E_OUTOFMEMORY)
		{
			MessageBox(NULL, _T("Direct X texture out of memory."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		}
		else
		{
			MessageBox(NULL, _T("Direct X texture not created."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		}
		return false;
	}

	TexelFill_t texFillData;
	texFillData.enImgType = enImgType;
	texFillData.iHeight = iHeight;
	texFillData.iWidth = iWidth;
	texFillData.pData = pBuff;

	hRes = D3DXFillTexture(m_pclTexture[iSurfaceIndex], &texFill, &texFillData);
	if (FAILED(hRes))	
		return false;	

	m_pbImageLoaded[iSurfaceIndex] = true;
	return true;
}

bool CD3DXBase::GenerateTextureFromFile(CString strFPath, int iSurfaceIndex)
{
	HRESULT hRes;

	m_dZoomLevel = 1.0;
	m_stScaling.x = 0;
	m_stScaling.y = 0;

	m_stTranslation.x = 0;
	m_stTranslation.y = 0;

	m_stRotationPoint.x = 0;
	m_stRotationPoint.y = 0;
	
	if (iSurfaceIndex >= m_iTextureCount)
		return false;

	m_pbImageLoaded[iSurfaceIndex] = false;

	if (!m_pDirect3DDevice)
	{
		MessageBox(NULL, _T("Direct X not initialized  [GenerateTextureFromFile - Direct3DDevice is NULL]."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		return false;
	}

	if (m_pclTexture[iSurfaceIndex] != NULL)
	{
		m_pclTexture[iSurfaceIndex]->Release();
	}

	if (!m_pSprite)
	{
		MessageBox(NULL, _T("Direct X not initialized [GenerateTextureFromFile - Sprite is NULL]."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		return false;
	}

	D3DXIMAGE_INFO stInfo;
	D3DXGetImageInfoFromFile(strFPath, &stInfo);

	m_pstSurfaceSize[iSurfaceIndex].cx = stInfo.Width;
	m_pstSurfaceSize[iSurfaceIndex].cy = stInfo.Height;
	m_enImgFormat = stInfo.Format;

	hRes = D3DXCreateTextureFromFileEx(m_pDirect3DDevice, strFPath, D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 1, 0, m_enImgFormat, D3DPOOL_DEFAULT, D3DX_FILTER_LINEAR, D3DX_FILTER_NONE, 0, NULL, NULL, &(m_pclTexture[iSurfaceIndex]));
	if (FAILED(hRes))
	{
		//MessageBox(_T("Direct X failed to load file [GenerateTextureFromFile]."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		return false;
	}

	m_pbImageLoaded[iSurfaceIndex] = true;

	return true;
}

void CD3DXBase::CleanUp() //function to delete/release things to prevent memory leak
{
	if (m_pbImageLoaded)
		delete[]m_pbImageLoaded;

	if (m_pstSurfaceSize)
		delete[]m_pstSurfaceSize;

	if (!m_pclTexture) return;

	for (int iLoop = 0; iLoop < m_iTextureCount; iLoop++)
	{
		if (m_pclTexture[iLoop] != NULL)
		{
			m_pclTexture[iLoop]->Release();
			m_pclTexture[iLoop] = NULL;
		}
	}

	delete[]m_pclTexture;
	m_pclTexture = nullptr;
}

bool CD3DXBase::ResetSwapChain(CWnd *pWnd)
{
	m_pclSwapChain.Release();

	if (!pWnd) return false;

	RECT stDisplayWndRect;
	::GetClientRect(pWnd->GetSafeHwnd(), &stDisplayWndRect);
	if (stDisplayWndRect.left == stDisplayWndRect.right ||
		stDisplayWndRect.top == stDisplayWndRect.bottom)
		return false;

	D3DPRESENT_PARAMETERS stPresentParams = m_stPresentParams;
	stPresentParams.BackBufferWidth = stDisplayWndRect.right;
	stPresentParams.BackBufferHeight = stDisplayWndRect.bottom;

	bool bSwapChainReset = false;

	if (SUCCEEDED(m_pDirect3DDevice->CreateAdditionalSwapChain(&stPresentParams,
		&m_pclSwapChain)))
	{
		ATL::CComPtr< IDirect3DSurface9 > pclBackBuffer;

		if (SUCCEEDED(m_pclSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO,
			&pclBackBuffer)))
		{
			if (SUCCEEDED(m_pDirect3DDevice->SetRenderTarget(0, pclBackBuffer)))
			{
				bSwapChainReset = true;
			}
		}
	}

	return bSwapChainReset;
}


DoubleSize_t CD3DXBase::CalculateGlobalScale()
{
	double dXSize, dYSize;

	if (m_pstSurfaceSize[m_iActiveTexture].cx > m_szMaxSupportedSize.cx)
	{
		dXSize = m_szMaxSupportedSize.cx * m_stScaling.x;
	}
	else
	{
		dXSize = m_pstSurfaceSize[m_iActiveTexture].cx * m_stScaling.x;
	}
	if (m_pstSurfaceSize[m_iActiveTexture].cy > m_szMaxSupportedSize.cy)
	{
		dYSize = m_szMaxSupportedSize.cy * m_stScaling.y;
	}
	else
	{
		dYSize = m_pstSurfaceSize[m_iActiveTexture].cy * m_stScaling.y;
	}

	DoubleSize_t stGlobalScaling;

	stGlobalScaling.dSx = dXSize / m_pstSurfaceSize[m_iActiveTexture].cx;
	stGlobalScaling.dSy = dYSize / m_pstSurfaceSize[m_iActiveTexture].cy;

	return stGlobalScaling;
}

bool CD3DXBase::IsActiveTextureLoaded(int iSurfaceIndex)
{
	if (!m_pbImageLoaded) return false;

	if (iSurfaceIndex >= m_iTextureCount)
		return false;

	return m_pbImageLoaded[iSurfaceIndex];
}

bool CD3DXBase::DisplayImage(int iSurfaceIndex, CWnd *pTargetWnd, DoubleSize_t &stGlobalScale)
{
	HRESULT hRes;
	D3DXMATRIX mat;

	if (pTargetWnd == nullptr) return false;

	ResetSwapChain(pTargetWnd);

	if (iSurfaceIndex >= m_iTextureCount)
		return false;

	if (!m_pbImageLoaded) return false;
	if (!m_pbImageLoaded[iSurfaceIndex]) return false;

	CRect clRect;
	pTargetWnd->GetClientRect(&clRect);
	
	double dImageWidth = m_pstSurfaceSize[iSurfaceIndex].cx;
	double dImageHeight = m_pstSurfaceSize[iSurfaceIndex].cy;
	double dWidth = clRect.Width();
	double dHeight = clRect.Height();

	//auto  dNewHeight = (DOUBLE)(dImageHeight / (DOUBLE)((dImageWidth / dWidth)));
	//if (dNewHeight <= dHeight)
	//	dHeight = dNewHeight;
	//else
	//	dWidth = (DOUBLE)(dImageWidth / ((DOUBLE)(dImageHeight / dHeight)));


	if (dImageWidth > m_szMaxSupportedSize.cx)
		m_stScaling.x = (FLOAT)(dWidth / m_szMaxSupportedSize.cx);
	else
		m_stScaling.x = (FLOAT)(dWidth / dImageWidth);


	if (dImageHeight > m_szMaxSupportedSize.cy)
		m_stScaling.y = (FLOAT)(dHeight / m_szMaxSupportedSize.cy);
	else
		m_stScaling.y = (FLOAT)(dHeight / dImageHeight);
	
	stGlobalScale = CalculateGlobalScale();

	if (!m_pDirect3DDevice)
	{
		MessageBox(NULL, _T("Direct X not initialized [DisplayImage - Full - Direct3DDevice is NULL]."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		return false;
	}

	if (!m_pclSwapChain)
	{
		MessageBox(NULL, _T("Direct X not initialized  [DisplayImage - Full - SwapChain is NULL]."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		return false;
	}

	if (m_pclTexture[iSurfaceIndex] == NULL)
	{
		MessageBox(NULL, _T("Direct X not initialized [DisplayImage - Full - Texture is NULL]."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		return false;
	}

	if (!m_pSprite)
	{
		MessageBox(NULL, _T("Direct X not initialized [DisplayImage - Full - Sprite is NULL]."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		return false;
	}

	hRes = m_pDirect3DDevice->BeginScene();
	if (FAILED(hRes))
	{
		hRes = m_pDirect3DDevice->EndScene();
		if (FAILED(hRes))
		{
			MessageBox(NULL, _T("Direct X failed to begin scene. But end scene also failed. [DisplayImage - Full]."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
			return false;
		}

		hRes = m_pDirect3DDevice->BeginScene();
		if (FAILED(hRes))
		{
			MessageBox(NULL, _T("Direct X failed to begin scene. Retry begin scene also failed. [DisplayImage - Full]."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
			return false;
		}
	}

	hRes = m_pDirect3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	if (FAILED(hRes))
	{
		MessageBox(NULL, _T("Direct X failed to clear scene [DisplayImage - Full]."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		return false;
	}

	hRes = m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
	if (FAILED(hRes))
	{
		hRes = m_pSprite->End();
		if (FAILED(hRes))
		{
			MessageBox(NULL, _T("Direct X failed to begin blend [DisplayImage - Full]. But end sprite also failed."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
			return false;
		}

		hRes = m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
		if (FAILED(hRes))
		{
			MessageBox(NULL, _T("Direct X failed to begin blend [DisplayImage - Full]. Retry begin blend also failed."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
			return false;
		}
	}

	hRes = m_pDirect3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DX_FILTER_LINEAR);/*D3DTEXF_GAUSSIANQUAD);*/
	if (FAILED(hRes))
	{
		MessageBox(NULL, _T("Direct X failed set min filter [DisplayImage - Full]."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		return false;
	}

	hRes = m_pDirect3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DX_FILTER_LINEAR);/*D3DTEXF_GAUSSIANQUAD);*/
	if (FAILED(hRes))
	{
		MessageBox(NULL, _T("Direct X failed to set max filter [DisplayImage - Full]."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		return false;
	}

	hRes = m_pDirect3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	if (FAILED(hRes))
	{
		MessageBox(NULL, _T("Direct X failed to set mip filter [DisplayImage - Full]."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		return false;
	}

	D3DXMatrixIdentity(&mat);

	D3DXMatrixTransformation2D(&mat, NULL, 0.0, &m_stScaling, &m_stRotationPoint, 0, &m_stTranslation);

	hRes = m_pSprite->SetTransform(&mat);
	if (FAILED(hRes))
	{
		MessageBox(NULL, _T("Direct X failed to set transformation [DisplayImage - Full]."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		return false;
	}

	hRes = m_pDirect3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	if (FAILED(hRes))
	{
		MessageBox(NULL, _T("Direct X failed to set texture stage state [DisplayImage - Full]."), APPLICATION_TITLE,
			MB_ICONERROR | MB_OK);
		return false;
	}

	hRes = m_pSprite->Draw(m_pclTexture[iSurfaceIndex], NULL, NULL, NULL, 0xFFFFFFFF);
	if (FAILED(hRes))
	{
		MessageBox(NULL, _T("Direct X failed to draw [DisplayImage - Full]."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		return false;
	}

	hRes = m_pSprite->Flush();
	if (FAILED(hRes))
	{
		MessageBox(NULL, _T("Direct X failed to flush sprite [DisplayImage - Full]."), APPLICATION_TITLE,
			MB_ICONERROR | MB_OK);
		return false;
	}

	hRes = m_pSprite->End();
	if (FAILED(hRes))
	{
		MessageBox(NULL, _T("Direct X failed to finish draw [DisplayImage - Full]."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		return false;
	}

	hRes = m_pDirect3DDevice->EndScene();
	if (FAILED(hRes))
	{
		MessageBox(NULL, _T("Direct X failed to end scene [DisplayImage - Full]."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		return false;
	}

	hRes = m_pclSwapChain->Present(NULL, NULL, pTargetWnd->GetSafeHwnd(), NULL, 0);
	if (FAILED(hRes))
	{
		//DisplayError(hRes);
		MessageBox(NULL, _T("Direct X failed to present scene [DisplayImage - Full]."), APPLICATION_TITLE, MB_ICONERROR | MB_OK);
		return false;
	}

	return true;
}

void CD3DXBase::ClearDisplay()
{
	for (int iLoop = 0; iLoop < m_iTextureCount; iLoop++)
	{
		m_pbImageLoaded[iLoop] = false;
		if (m_pclTexture[iLoop] != NULL)
		{
			m_pclTexture[iLoop]->Release();
			m_pclTexture[iLoop] = NULL;
		}
		m_pstSurfaceSize[iLoop].cx = NULL;
		m_pstSurfaceSize[iLoop].cy = NULL;
	}
}

void CD3DXBase::OnSize(CWnd *pTargetWnd)
{
	if (!m_bInitialized)					return;
	if (pTargetWnd == nullptr)				return;
	if (!ResetSwapChain(pTargetWnd))		return;	

	if (!m_pbImageLoaded)					return;
	if (!m_pbImageLoaded[m_iActiveTexture]) return;

	pTargetWnd->Invalidate();
}