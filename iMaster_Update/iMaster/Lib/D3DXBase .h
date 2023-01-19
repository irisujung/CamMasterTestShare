#pragma once
#include "afxwin.h"
#include "../../iCommonIF/CommonIF-Define.h"
#include <d3d9.h>
#include "../../_ExLib/DirectX_SDK/Include/d3dx9.h"

#pragma comment (lib, "d3d9.lib")

#if defined(_M_X64) || defined(WIN64)
	#pragma comment (lib, "../../_ExLib/DirectX_SDK/Lib/x64/d3dx9.lib")
#else
	#pragma comment (lib, "../../_ExLib/DirectX_SDK/Lib/x86/d3dx9.lib")
#endif

enum class ImageType_en
{
	EN_ERROR = 0,
	EN_1_BPP = 1,
	EN_8_BPP = 8,
	EN_16_BPP = 16,
	EN_24_BPP = 24,
};

struct DoubleSize_t
{
	double dSx;
	double dSy;
};

struct TexelFill_t
{
	BYTE *pData = nullptr;
	ImageType_en enImgType = ImageType_en::EN_ERROR;
	int iHeight = 0;
	int iWidth = 0;
};

class CD3DXBase 
{
protected:
	int m_iActiveTexture = 0;
	double m_dZoomLevel = 1.0;
	int m_iTextureCount = 0;
	SIZE *m_pstSurfaceSize = nullptr;
	bool m_bInitialized = false;

	bool *m_pbImageLoaded = nullptr;

	D3DPRESENT_PARAMETERS m_stPresentParams;
	ATL::CComPtr< IDirect3DDevice9 > m_pDirect3DDevice;
	ATL::CComPtr< ID3DXSprite > m_pSprite;
	LPDIRECT3DTEXTURE9	*m_pclTexture = nullptr;
	LPDIRECT3DTEXTURE9	m_clMinimapTexture = nullptr;
	ATL::CComPtr< IDirect3DSwapChain9 > m_pclSwapChain;

	D3DXVECTOR2 m_stScaling;
	D3DXVECTOR2 m_stTranslation;
	D3DXVECTOR2 m_stRotationPoint;
	D3DFORMAT m_enImgFormat = D3DFMT_R8G8B8;

	SIZE m_szMaxSupportedSize;
//	D3DXVECTOR2 m_stGlobalScaling;


	void CleanUp();
	bool ResetSwapChain(CWnd *pWnd);
	DoubleSize_t CalculateGlobalScale();

public:
	CD3DXBase();
	~CD3DXBase();

	int GetActiveTexture() { return m_iActiveTexture; }
	bool IsActiveTextureLoaded(int iSurfaceIndex);

	bool DisplayImage(int iSurfaceIndex, CWnd *pTargetWnd, DoubleSize_t &stGlobalScale);
	void SetActiveTextureIndex(int iSurface);
	bool InitializeResources(int iSurfaceCount, CWnd *pWndParent);
	bool GenerateTextureFromFile(CString strFPath, int iSurfaceIndex);
	void ClearDisplay();

	bool GenerateTextureFromRawBuffer(BYTE *pBuffer, int iWidth, int iHeight, ImageType_en enImgType, int iSurfaceIndex);

	void OnSize(CWnd *pTargetWnd);

};