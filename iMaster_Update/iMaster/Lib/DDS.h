#ifndef _DDS_H_
#define _DDS_H_


///////////////////////////////////////////////////////////////////////////////////////////////////

/** NOTE:
	The _DDSURFACEDESC2 structure declared in <ddraw.h> must not be used for creating a DDS file
	from memory, because size of this structure changes from 124 bytes in the Win32 platform, to
	136 bytes in the x64 platform, whereas DirectX requires size of this structure to be 124
	bytes irrespective of the platform for creating a DDS texture from data stored in memory
	using the D3DXCreateTextureFromFileInMemoryEx() method.
	
	The below DirectDraw related DDS file structures are described in DirectX Graphics
	Documentation. Other constants required by these structures must be used by including <ddraw.h>.
*/

#define DDSD_CAPS	0x00000001
#define DDSD_HEIGHT	0x00000002
#define DDSD_WIDTH	0x00000004
#define DDSD_PITCH	0x00000008
#define DDSD_PIXELFORMAT	0x00001000
#define DDPF_ALPHAPIXELS	0x1
#define DDPF_ALPHA	0x2 
#define DDPF_RGB	0x40
#define DDSCAPS_TEXTURE	0x1000
#define DDPF_LUMINANCE	0x20000
#define DDPF_YUV 0x200

struct DDS_PIXELFORMAT
{
	UINT dwSize;
	UINT dwFlags;
	UINT dwFourCC;
	UINT dwRGBBitCount;
	UINT dwRBitMask;
	UINT dwGBitMask;
	UINT dwBBitMask;
	UINT dwABitMask;
};

struct DDS_HEADER
{
	DWORD dwSize;
	DWORD dwFlags;
	DWORD dwHeight;
	DWORD dwWidth;
	DWORD dwLinearSize;
	DWORD dwDepth;
	DWORD dwMipMapCount;
	DWORD dwReserved1[ 11 ];
	DDS_PIXELFORMAT ddpf;
	DWORD dwCaps;
	DWORD dwCaps2;
	DWORD dwCaps3;
	DWORD dwCaps4;
	DWORD dwReserved2;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _DDS_H_
