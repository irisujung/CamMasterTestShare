#include "pch.h"
#include "CSaveLayer.h"
#include "../CDrawLayer.h"
#include "../CDrawProfile.h"

#include <wrl.h>//Comptr
using namespace Microsoft::WRL;

#include "shlobj_core.h"

CSaveLayer::CSaveLayer()
{
}

CSaveLayer::~CSaveLayer()
{
	_ClearRender();

	if (m_pDrawFunction != nullptr)
	{
		delete m_pDrawFunction;
		m_pDrawFunction = nullptr;
	}

	if (m_pDrawProfile != nullptr)
	{
		delete m_pDrawProfile;
		m_pDrawProfile = nullptr;
	}
}

void CSaveLayer::SetLink(IWICImagingFactory2 *pImageFactory, IN ID2D1Factory1 *pFactory, IN ID2D1DeviceContext* p2DContext)
{
	m_pFactory = pFactory;
	m_pD2DContext = p2DContext;
	m_pImageFactory = pImageFactory;	

	if (m_pDrawFunction == nullptr)
	{
		m_pDrawFunction = new CDrawFunction();
		m_pDrawFunction->_SetLink(m_pFactory, m_pD2DContext);
	}

	if (m_pDrawProfile == nullptr)
	{
		m_pDrawProfile = new CDrawProfile();
		m_pDrawProfile->SetLink(m_pFactory, m_pD2DContext);
	}
}


UINT32 CSaveLayer::SetPictureBox(IN const D2D1_RECT_F &frtPictureBox, double dResolution, BOOL bIsProfile, std::mutex *pMutex)
{
	m_frtPictureBox = frtPictureBox;
	m_dResolution = dResolution;
	m_pMutex = pMutex;

	double dPictureWidth = fabs(m_frtPictureBox.right - m_frtPictureBox.left);
	double dPictureHeight = fabs(m_frtPictureBox.bottom - m_frtPictureBox.top);

	m_sizeTarget.width = static_cast<float>(static_cast<int>(dPictureWidth / dResolution + 0.5));
	m_sizeTarget.height = static_cast<float>(static_cast<int>(dPictureHeight / dResolution + 0.5));
	
// 	
// 	//m_pD2DContext->CreateCompatibleRenderTarget(m_sizeTarget, &m_pBmpRenderTarget);
// 
// 
// 	m_pImageFactory->CreateBitmap(m_sizeTarget.width, m_sizeTarget.height,
// 		GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnLoad, &m_pWicBitmap);

	//m_pFactory->CreateWicBitmapRenderTarget(m_pWicBitmap, D2D1::RenderTargetProperties(), &m_pRenderTarget);

	//m_pRenderTarget->CreateCompatibleRenderTarget()
	
	_ClearRender();

	if (_MakeRender(dResolution, bIsProfile) != RESULT_GOOD)
		return RESULT_BAD;

// 	if (S_OK != m_pD2DContext->CreateCompatibleRenderTarget(m_sizeTarget, &m_pBmpRenderTarget))
// 		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CSaveLayer::_ClearRender()
{
// 	if (m_pBmpRenderTarget != nullptr)
// 	{
// 		m_pBmpRenderTarget->Release();
// 		m_pBmpRenderTarget = nullptr;
// 	}


	m_pDrawFunction->SafeRelease(&m_pWicBitmap);
	m_pDrawFunction->SafeRelease(&m_pImageRender);

	return RESULT_GOOD;
}

UINT32 CSaveLayer::_MakeRender(IN const double &dResolution, IN const BOOL &bIsProfile)
{
	D2D1_POINT_2F				ptSplit_mm = D2D1::Point2F(0, 0);//분햘 저장할 경우의 분할 저장 실체 크기

	m_vecvecSplitPos.clear();

	if (m_sizeTarget.width - MAX_RENDER_WIDTH <= 0.f)
	{
		m_ptSplitCount.x = 1;
		m_sizeSplitImage.width = m_sizeTarget.width;
	}
	else
	{
		m_ptSplitCount.x = static_cast<int>(ceil((m_sizeTarget.width - MAX_RENDER_WIDTH) / (MAX_RENDER_WIDTH- m_nOverlapPixel)) + 1);
		m_sizeSplitImage.width = static_cast<float>(MAX_RENDER_WIDTH);
	}

	int nRenderHeight = MAX_RENDER_PIXEL_COUNT / MAX_RENDER_WIDTH;//Height Pixel Count;
	
	//Profile 일때는 m_ptSplitCount.x, y(1,1)
	if (bIsProfile)
		nRenderHeight = min(MAX_RENDER_WIDTH, max(static_cast<int>(MAX_RENDER_PIXEL_COUNT / m_sizeTarget.width), (int)m_sizeTarget.width));			

	if (m_sizeTarget.height - nRenderHeight <= 0.f)
	{
		m_ptSplitCount.y = 1;
		m_sizeSplitImage.height = m_sizeTarget.height;
	}
	else
	{
		m_ptSplitCount.y = static_cast<int>(ceil((m_sizeTarget.height - nRenderHeight) / ( nRenderHeight - m_nOverlapPixel)) + 1);
		m_sizeSplitImage.height = static_cast<float>(nRenderHeight);
	}

	m_vecvecSplitPos.resize(m_ptSplitCount.y);
	for (int i = 0; i < m_ptSplitCount.y; i++)
	{
		m_vecvecSplitPos[i].resize(m_ptSplitCount.x);
	}

	//////////////////////////////////////////////////////////////////////////
	//PosX
	for (int i = 0; i < m_ptSplitCount.y; i++)
	{
		for (int j = 0; j < m_ptSplitCount.x; j++)
		{
			if (j == m_ptSplitCount.x - 1)
			{
				m_vecvecSplitPos[i][j].ptPos.x = static_cast<int>(m_sizeTarget.width - m_sizeSplitImage.width);
			}
			else
			{
				m_vecvecSplitPos[i][j].ptPos.x = static_cast<int>(j * (m_sizeSplitImage.width - m_nOverlapPixel));
			}
			if (i == m_ptSplitCount.y - 1)
			{
				m_vecvecSplitPos[i][j].ptPos.y = static_cast<int>(m_sizeTarget.height - m_sizeSplitImage.height);
			}
			else
			{
				m_vecvecSplitPos[i][j].ptPos.y = static_cast<int>(i * (m_sizeSplitImage.height - m_nOverlapPixel));
			}

			m_vecvecSplitPos[i][j].nSplitWidth_Pixel = static_cast<int>(m_sizeSplitImage.width + 0.5);
			m_vecvecSplitPos[i][j].nSplitHeight_Pixel = static_cast<int>(m_sizeSplitImage.height + 0.5);
		}
	}
	
	
	//////////////////////////////////////////////////////////////////////////


	ptSplit_mm.x = static_cast<float>(m_sizeSplitImage.width * dResolution);
	ptSplit_mm.y = static_cast<float>(m_sizeSplitImage.height * dResolution);

	//Set Scale
	float fInnerSizeX = m_sizeSplitImage.width;
	float fInnerSizeY = m_sizeSplitImage.height;

	float fInnerScaleX = fabs(m_sizeSplitImage.width / ptSplit_mm.x);
	float fInnerScaleY = fabs(m_sizeSplitImage.height / ptSplit_mm.y);
	if (fInnerScaleX <= fInnerScaleY)
	{
		m_fSplitScale = fInnerScaleX;
	}
	else
	{
		m_fSplitScale = fInnerScaleY;
	}

	_ClearRender();

	//HRESULT hr = m_pD2DContext->CreateCompatibleRenderTarget(m_sizeSplitImage, &m_pBmpRenderTarget);
	HRESULT hr;
	if (m_pWicBitmap == nullptr)
	{
		hr = m_pImageFactory->CreateBitmap(UINT(m_sizeSplitImage.width), UINT(m_sizeSplitImage.height),
			GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnDemand, &m_pWicBitmap);
	}
	if (m_pWicBitmap == nullptr) return RESULT_BAD;

	D2D1_RENDER_TARGET_PROPERTIES properties = D2D1::RenderTargetProperties();
	properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
	properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
	// 
	properties.minLevel = D2D1_FEATURE_LEVEL_DEFAULT;

	if (m_pImageRender == nullptr)
	{
		hr = m_pFactory->CreateWicBitmapRenderTarget(m_pWicBitmap, properties, &m_pImageRender);
	}

	if (m_pImageRender == nullptr)
	{
		return RESULT_BAD;
	}
	

	return RESULT_GOOD;
}

UINT32 CSaveLayer::_BeginDraw(IN const BOOL &bClear)
{
	if (m_pImageRender == nullptr) return RESULT_BAD;

	m_pImageRender->BeginDraw();

	if ( bClear == TRUE )
	{
		m_pImageRender->Clear(D2D1::ColorF(D2D1::ColorF::Black));//Black|
	}

	return RESULT_GOOD;
}

HRESULT CSaveLayer::_EndDraw()
{
	if (m_pImageRender == nullptr) return RESULT_BAD;

	return m_pImageRender->EndDraw();
}

UINT32 CSaveLayer::_SaveImage(IWICBitmapSource *pBitmap, IN const CString &strfilename)
{
	CString filename_lower = strfilename;
	filename_lower = filename_lower.MakeLower();
	CString ext = filename_lower.Right(4);
	GUID guid = GUID_ContainerFormatBmp;
	if (ext == L".png")
		guid = GUID_ContainerFormatPng;
	else if (ext == L".jpg")
		guid = GUID_ContainerFormatJpeg;
	else if (ext == L".tif")
		guid = GUID_ContainerFormatTiff;

	ext = filename_lower.Right(5);
	if (ext == L".jpeg" || ext == L".jpg+")
		guid = GUID_ContainerFormatJpeg;



	IStream* pFile;

	HRESULT hr;
	hr = SHCreateStreamOnFileEx(strfilename,
		STGM_CREATE | STGM_WRITE | STGM_SHARE_EXCLUSIVE,
		FILE_ATTRIBUTE_NORMAL,
		TRUE, // create
		nullptr, // template
		&pFile);

	if (hr != S_OK) return RESULT_BAD;

	IWICBitmapEncoder* pEncoder;

	hr = m_pImageFactory->CreateEncoder(guid,
	nullptr, // vendor
	&pEncoder);

	if (hr != S_OK) return RESULT_BAD;

	hr = pEncoder->Initialize(pFile, WICBitmapEncoderNoCache);
	if (hr != S_OK) return RESULT_BAD;

	IWICBitmapFrameEncode *pFrame;
	IPropertyBag2 *pProperties;
	
	hr = pEncoder->CreateNewFrame(&pFrame, &pProperties);
	if (hr != S_OK) return RESULT_BAD;

	hr = pFrame->Initialize(pProperties);
	if (hr != S_OK) return RESULT_BAD;

	UINT width, height;
	hr = pBitmap->GetSize(&width, &height);
	if (hr != S_OK) return RESULT_BAD;
	hr = pFrame->SetSize(width, height);
	if (hr != S_OK) return RESULT_BAD;

	GUID pixelFormat;
	hr = pBitmap->GetPixelFormat(&pixelFormat);
	if (hr != S_OK) return RESULT_BAD;

	auto negotiated = pixelFormat;
	hr = pFrame->SetPixelFormat(&negotiated);
	if (hr != S_OK) return RESULT_BAD;

	hr = pFrame->WriteSource(pBitmap, nullptr);
	if (hr != S_OK) return RESULT_BAD;

	hr = pFrame->Commit();
	if (hr != S_OK) return RESULT_BAD;
	hr = pEncoder->Commit();
	if (hr != S_OK) return RESULT_BAD;

	//
	
	pFrame->Release();
	pProperties->Release();
	pEncoder->Release();
	pFile->Release();

	return RESULT_GOOD;
}


UINT32 CSaveLayer::_SaveImage(ID2D1Bitmap *pBitmap, IN const CString &strfilename)
{
	HRESULT hr;

	CString filename_lower = strfilename;
	filename_lower = filename_lower.MakeLower();
	CString ext = filename_lower.Right(4);
	GUID guid = GUID_ContainerFormatBmp;
	if (ext == L".png")
		guid = GUID_ContainerFormatPng;
	else if (ext == L".jpg")
		guid = GUID_ContainerFormatJpeg;
	else if (ext == L".tif")
		guid = GUID_ContainerFormatTiff;

	ext = filename_lower.Right(5);
	if (ext == L".jpeg" || ext == L".jpg+")
		guid = GUID_ContainerFormatJpeg;

	// Create and initialize WIC Bitmap Encoder.
	ComPtr<IWICBitmapEncoder> wicBitmapEncoder;

	if (m_pMutex)
	{
		//std::unique_lock<std::mutex> clLocker(*m_pMutex);

		hr = m_pImageFactory->CreateEncoder(
			guid,
			nullptr,    // No preferred codec vendor.
			&wicBitmapEncoder
		);

		//clLocker.unlock();
	}
	else
	{
		hr = m_pImageFactory->CreateEncoder(
			guid,
			nullptr,    // No preferred codec vendor.
			&wicBitmapEncoder
		);
	}

	if (hr != S_OK) return RESULT_BAD;

	ComPtr <IStream> pStream;
	hr = SHCreateStreamOnFileEx(strfilename,
		STGM_CREATE | STGM_WRITE ,//| STGM_SHARE_EXCLUSIVE,
		FILE_ATTRIBUTE_NORMAL,
		TRUE, // create
		nullptr, // template
		&pStream);

	if (hr != S_OK) return RESULT_BAD;

	hr = wicBitmapEncoder->Initialize(
		pStream.Get(),
		WICBitmapEncoderNoCache
	);

	if (hr != S_OK) return RESULT_BAD;

	// Create and initialize WIC Frame Encoder.
	ComPtr<IWICBitmapFrameEncode> wicFrameEncode;

	hr = wicBitmapEncoder->CreateNewFrame(
		&wicFrameEncode,
		nullptr     // No encoder options.
	);

	if (hr != S_OK) return RESULT_BAD;

	hr = wicFrameEncode->Initialize(nullptr);
	if (hr != S_OK) return RESULT_BAD;

	// Retrieve D2D Device.
	ComPtr<ID2D1Device> d2dDevice;	

	// Create IWICImageEncoder.
	ComPtr<IWICImageEncoder> imageEncoder;
	
	if (m_pMutex)
	{
		m_pD2DContext->GetDevice(&d2dDevice);

		//std::unique_lock<std::mutex> clLocker(*m_pMutex);

		hr = m_pImageFactory->CreateImageEncoder(
			d2dDevice.Get(),
			&imageEncoder
		);

		//clLocker.unlock();
	}
	else
	{
		m_pD2DContext->GetDevice(&d2dDevice);

		hr = m_pImageFactory->CreateImageEncoder(
			d2dDevice.Get(),
			&imageEncoder
		);
	}

	if (hr != S_OK) return RESULT_BAD;

	hr = imageEncoder->WriteFrame(
		pBitmap,
		wicFrameEncode.Get(),
		nullptr     // Use default WICImageParameter options.
	);

	if (hr != S_OK) return RESULT_BAD;

	hr = wicFrameEncode->Commit();

	if (hr != S_OK) return RESULT_BAD;

	hr = wicBitmapEncoder->Commit();

	if (hr != S_OK) return RESULT_BAD;

	// Flush all memory buffers to the next-level storage object.

	hr = pStream.Get()->Commit(STGC_DEFAULT);
	
	if (hr != S_OK) return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CSaveLayer::SaveProfile(IN const CString &strfilename, IN CStep *pStep, CRotMirrArr *pRotMirrArr, IN const BOOL &bSubDraw, IN const BOOL &bSubSubDraw, IN BOOL bSubSubSubDraw)
{
	if (pStep == nullptr) return RESULT_BAD;
	if (m_pDrawProfile == nullptr) return RESULT_BAD;

	HRESULT hr;
	UINT32 nRet = RESULT_BAD;

	m_pImageRender->BeginDraw();
	m_pImageRender->Clear();

	_SetTransform(m_pImageRender);

	m_pDrawProfile->OnRender_Save(pStep, pRotMirrArr, bSubDraw, bSubSubDraw, bSubSubSubDraw, m_pImageRender);

	hr = m_pImageRender->EndDraw();

	if (hr == S_OK)
	{
		nRet = _SaveImage(m_pWicBitmap, strfilename);
	}

	return nRet;
}


void	CSaveLayer::_SetTransform(ID2D1RenderTarget* pRender)
{
	D2D1_POINT_2F fptCenter;
	fptCenter.x = (m_frtPictureBox.left + (m_frtPictureBox.right - m_frtPictureBox.left) / 2.f);//rtClient.Width() / 2.f;
	fptCenter.y = (m_frtPictureBox.top + (m_frtPictureBox.bottom - m_frtPictureBox.top) / 2.f);
	//D2D1::Matrix3x2F matTrans = m_matScaleView.TransformPoint(fptCenter);

	D2D1_POINT_2F fptPan = D2D1::Point2F(0.f, 0.f);
	_SetViewScale(m_fSplitScale, fptPan);

	D2D1::Matrix3x2F matViewScale = D2D1::Matrix3x2F(m_matDrawTransform.m11, m_matDrawTransform.m12, m_matDrawTransform.m21, m_matDrawTransform.m22, 0, 0);
	D2D1_POINT_2F fptCenterT = matViewScale.TransformPoint(fptCenter);

	fptPan.x = (-1.0f*fptCenterT.x) + m_sizeSplitImage.width / 2.f;
	fptPan.y = (-1.0f*fptCenterT.y) + m_sizeSplitImage.height / 2.f;

	_SetViewScale(m_fSplitScale, fptPan);

	pRender->SetTransform(m_matDrawTransform);

}


void CSaveLayer::_SetViewScale(IN const float &fScale, IN const D2D_POINT_2F &fptPan)
{
	D2D1::Matrix3x2F matScale = D2D1::Matrix3x2F(fScale, 0, 0, fScale, 0, 0);
	m_matDrawTransform = D2D1::Matrix3x2F(matScale.m11, matScale.m12, matScale.m21*-1.0f, matScale.m22*-1.0f, fptPan.x, fptPan.y);
}
