#include "pch.h"
#include "CBackBuffer.h"
#include "../CDrawLayer.h"

CBackBuffer::CBackBuffer()
{

}

CBackBuffer::~CBackBuffer()
{
	if (m_pDrawFunction != nullptr)
	{
		delete m_pDrawFunction;
		m_pDrawFunction = nullptr;
	}

	if (m_pRenderTarget != nullptr)
	{
		m_pRenderTarget->Release();
		m_pRenderTarget = nullptr;
	}

	if (m_pImageFactory != nullptr)
	{
		m_pImageFactory->Release();
		m_pImageFactory = nullptr;
	}
	if (m_pBitmap != nullptr)
	{
		m_pBitmap->Release();
		m_pBitmap = nullptr;
	}

	
}

void CBackBuffer::SetLink(IN ID2D1Factory1 *pFactory, IN ID2D1DeviceContext* p2DContext, IN SystemSpec_t *pSystemSpec)
{
	m_pFactory = pFactory;
	m_p2DContext = p2DContext;
	m_pSystemSpec = pSystemSpec;

	if (m_pImageFactory == nullptr)
	{
		CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory),
			reinterpret_cast<void**>(&m_pImageFactory));
	}

	if (m_pDrawFunction == nullptr)
	{
		m_pDrawFunction = new CDrawFunction();
		m_pDrawFunction->_SetLink(m_pFactory, m_p2DContext);
	}
}


void CBackBuffer::SetPictureBox(IN const D2D1_RECT_F &frtPictureBox)
{
	m_frtPictureBox = frtPictureBox;

	float fPictureWidth = static_cast<float>(fabs(m_frtPictureBox.right - m_frtPictureBox.left));
	float fPictureHeight = static_cast<float>(fabs(m_frtPictureBox.bottom - m_frtPictureBox.top));
	
	CameraSpec_t *pSpec = nullptr;
	if (m_pSystemSpec->sysBasic.ScanType == eScanType::eLineScan)
		pSpec = &m_pSystemSpec->sysCamera[true];
	else
		pSpec = &m_pSystemSpec->sysCamera[false];

	double dResolMM = pSpec->dResolution / 1000.f;
	float fTargetRes = static_cast<float>(dResolMM);

	m_sizeTarget.width = fPictureWidth / fTargetRes;
	m_sizeTarget.height = fPictureHeight / fTargetRes;

	//큰사이즈를 기준으로 같도록 만들어 준다.
// 	if (m_sizeTarget.width > m_sizeTarget.height)
// 	{
// 		m_sizeTarget.height = m_sizeTarget.width;
// 	}
// 	else if (m_sizeTarget.width < m_sizeTarget.height)
// 	{
// 		m_sizeTarget.width = m_sizeTarget.height;
// 	}

	//기준 크기보다 크다면, 기준크기로 만든다.
	if (m_sizeTarget.width > enumRenderTargetSize)
	{
		m_sizeTarget.width = enumRenderTargetSize;
	}

	if (m_sizeTarget.height > enumRenderTargetSize)
	{
		m_sizeTarget.height = enumRenderTargetSize;
	}

	m_p2DContext->CreateCompatibleRenderTarget(m_sizeTarget, &m_pRenderTarget);
}

void CBackBuffer::OnRender(IN  CLayer *pLayer, IN const LayerSet_t* pLayerSet, CTypedPtrArray <CObArray, CStandardFont*>* pFontArr,
	CRotMirrArr *pRotMirrArr,IN const UserSetInfo_t *pUserSetInfo)
{
	if (m_pRenderTarget == nullptr) return;

	m_pRenderTarget->BeginDraw();
	m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));//Black

	DrawRender(m_pRenderTarget, pLayer, pLayerSet, pFontArr, pRotMirrArr, TRUE, pUserSetInfo);

	m_pRenderTarget->EndDraw();

	//
	D2D1_RECT_U rt;
	rt = GetBitmapRect();
	int nBitmapWidth = abs(static_cast<long>(rt.right - rt.left));
	int nBitmapHeight = abs(static_cast<long>(rt.bottom - rt.top));
	
	if (m_pBitmap == nullptr ||
		m_pBitmap->GetSize().width != nBitmapWidth ||
		m_pBitmap->GetSize().height != nBitmapHeight)
	{
		if (m_pBitmap != nullptr)
		{
			m_pBitmap->Release();
			m_pBitmap = nullptr;
		}

		D2D1_BITMAP_PROPERTIES properties;
		properties.pixelFormat = m_pRenderTarget->GetPixelFormat();
		m_pRenderTarget->GetDpi(&properties.dpiX, &properties.dpiY);
		//m_vecRenderTarget[nIndex]->CreateBitmap(D2D1::SizeU(nBitmapWidth, nBitmapHeight), properties, &pBitmap);

		m_pRenderTarget->CreateBitmap(D2D1::SizeU(nBitmapWidth, nBitmapHeight), properties, &m_pBitmap);

		//if (m_pRenderTarget == nullptr) return m_pBitmap;

		ID2D1Bitmap* pBitmapSrc = GetOriginBitmap();

		if (pBitmapSrc != nullptr)
		{
			D2D1_POINT_2U ptDst = D2D1::Point2U(0, 0);
			D2D1_RECT_U rt;
			rt = GetBitmapRect();

			HRESULT hRt = m_pBitmap->CopyFromBitmap(&ptDst, pBitmapSrc, &rt);
			pBitmapSrc->Release();
		}
	}
	
}

void CBackBuffer::DrawRender(ID2D1RenderTarget* pRender, IN  CLayer *pLayer, IN const LayerSet_t* pLayerSet,
	CTypedPtrArray <CObArray, CStandardFont*>* pFontArr, CRotMirrArr *pRotMirrArr, IN const BOOL &bNeedUpdate, IN const UserSetInfo_t *pUserSetInfo)
{
	if (pRender == nullptr) return;
	if (pLayer == nullptr) return;
	if (pLayerSet == nullptr) return;

	//Set Scale
	float fInnerSizeX = pRender->GetSize().width;
	float fInnerSizeY = pRender->GetSize().height;

	float fInnerScaleX = fabs(fInnerSizeX / (m_frtPictureBox.right - m_frtPictureBox.left));
	float fInnerScaleY = fabs(fInnerSizeY / (m_frtPictureBox.bottom - m_frtPictureBox.top));
	//float fInnerScale = fInnerScaleX <= fInnerScaleY ? fInnerScaleX : fInnerScaleY;

	D2D1_POINT_2F fptCenter;
	fptCenter.x = (m_frtPictureBox.left + (m_frtPictureBox.right - m_frtPictureBox.left) / 2.f);//rtClient.Width() / 2.f;
	fptCenter.y = (m_frtPictureBox.top + (m_frtPictureBox.bottom - m_frtPictureBox.top) / 2.f);
	//D2D1::Matrix3x2F matTrans = m_matScaleView.TransformPoint(fptCenter);

	D2D1_POINT_2F fptPan;
	fptPan.x = (fInnerSizeX /2.f) - (fptCenter.x ) * fInnerScaleX;
	fptPan.y = ((fInnerSizeY /2.f) - (fptCenter.y ) * fInnerScaleY);

	m_matDrawTransform = D2D1::Matrix3x2F(fInnerScaleX, 0, 0, fInnerScaleY, fptPan.x, fptPan.y);
	pRender->SetTransform(m_matDrawTransform);

	m_pDrawFunction->OnRender_Layer(pRender, pLayer, pLayerSet, pFontArr, pRotMirrArr, bNeedUpdate, pUserSetInfo);
	
}

ID2D1Bitmap* CBackBuffer::GetOriginBitmap()
{
	ID2D1Bitmap* pBitmap = nullptr;
	if (m_pRenderTarget == nullptr) return pBitmap;

	m_pRenderTarget->GetBitmap(&pBitmap);

	return pBitmap;
}

ID2D1Bitmap* CBackBuffer::GetDrawBitmap()
{
	if (m_pBitmap == nullptr) return m_pBitmap;
	

	return m_pBitmap;
}

D2D1_RECT_U CBackBuffer::NormalizeRect(IN const D2D1_RECT_U& rtRect)
{
	D2D1_RECT_U rtNormalize = rtRect;

	if (rtRect.left > rtRect.right)
	{
		rtNormalize.left = rtRect.right;
		rtNormalize.right = rtRect.left;
	}

	if (rtRect.top > rtRect.bottom)
	{
		rtNormalize.top = rtRect.bottom;
		rtNormalize.bottom = rtRect.top;
	}

	return rtNormalize;
}

D2D1_RECT_U  CBackBuffer::GetBitmapRect()
{
	
	D2D1_RECT_U rt = D2D1::RectU(0, 0, 0, 0);

	
	if (m_pRenderTarget == nullptr) return rt;

	D2D1::Matrix3x2F *pDrawTransform = &m_matDrawTransform;
	ID2D1RenderTarget* pRenderTarget = m_pRenderTarget;

	if (pRenderTarget != nullptr)
	{
		rt.left = static_cast<int>(m_sizeTarget.width / 2.f - (fabs(m_frtPictureBox.right - m_frtPictureBox.left) / 2.  * pDrawTransform->m11));
		rt.top = static_cast<int>(m_sizeTarget.height / 2.f - (fabs(m_frtPictureBox.bottom - m_frtPictureBox.top) / 2. * pDrawTransform->m22));
		rt.right = static_cast<int>(rt.left + (fabs(m_frtPictureBox.right - m_frtPictureBox.left) * pDrawTransform->m11));
		rt.bottom = static_cast<int>(rt.top + (fabs(m_frtPictureBox.bottom - m_frtPictureBox.top) * pDrawTransform->m22));
	}

	return rt;
}

void CBackBuffer::SaveImage(IWICBitmap *pBitmap, CString &strfilename)
{
	CString filename_lower = strfilename;
	filename_lower = filename_lower.MakeLower();
	CString ext = filename_lower.Right(4);
	GUID guid = GUID_ContainerFormatPng;
	if (ext == L".png")
		guid = GUID_ContainerFormatPng;
	else if (ext == L".jpg")
		guid = GUID_ContainerFormatJpeg;

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

	if (hr != S_OK) return;

	IWICBitmapEncoder* pEncoder;

	hr = m_pImageFactory->CreateEncoder(guid,
	nullptr, // vendor
	&pEncoder);

	if (hr != S_OK) return;

	hr = pEncoder->Initialize(pFile, WICBitmapEncoderNoCache);
	if (hr != S_OK) return;

	IWICBitmapFrameEncode *pFrame;
	IPropertyBag2 *pProperties;

	hr = pEncoder->CreateNewFrame(&pFrame, &pProperties);
	if (hr != S_OK) return;

	hr = pFrame->Initialize(pProperties);
	if (hr != S_OK) return;

	UINT width, height;
	hr = pBitmap->GetSize(&width, &height);
	if (hr != S_OK) return;
	hr = pFrame->SetSize(width, height);
	if (hr != S_OK) return;

	GUID pixelFormat;
	hr = pBitmap->GetPixelFormat(&pixelFormat);
	if (hr != S_OK) return;

	auto negotiated = pixelFormat;
	hr = pFrame->SetPixelFormat(&negotiated);
	if (hr != S_OK) return;

	hr = pFrame->WriteSource(pBitmap, nullptr);
	if (hr != S_OK) return;

	hr = pFrame->Commit();
	if (hr != S_OK) return;
	hr = pEncoder->Commit();
	if (hr != S_OK) return;

	//
	
	pFrame->Release();
	pProperties->Release();
	pEncoder->Release();
	pFile->Release();
}
