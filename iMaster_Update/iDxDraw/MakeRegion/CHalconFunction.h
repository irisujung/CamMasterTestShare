#pragma once


#include "Def_MakeRegion.h"

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;


class CHalconFunction
{
public:

	

public:
	CHalconFunction();
	~CHalconFunction();

	//Merge Master Image : SplitImage -> Master Image
	UINT32 CreateMergeImage(IN const CString &strDst, vector<vector<SPLIT_POS>> &vecvecSplitPos, IN const int &nOverlap,
		IN const int &nTotalWidth, IN const int &nTotalHeight, IN BOOL bSaveRaw = FALSE);

	UINT32 CreateMergeRegion(IN const CString &strDst, vector<vector<SPLIT_POS>> &vecvecSplitPos, IN const int &nOverlap,
		IN const int &nTotalWidth, IN const int &nTotalHeight);

	UINT32 CreateConcatRegion(IN vector<CString> &vecSrcRegion, CString  &strDstPathName, IN const BOOL &bSrcDelete = TRUE);

	static UINT32 CreateUnitAlignRegion(IN const CString &strLayerName, IN const int &nAlignCount, IN FovInfo_t *pFovInfo, CellInfo_t *pCellInfo, 
		IN const CString &strSrcPathName, IN const CString &strDstPathName, IN const CString &strImageType, IN const BOOL &bIsBlockMode);


	UINT32 ConvRegion(IN const CString &strSrcPathName, IN const CString &strDstPath);
	UINT32 ConvRegion(IN const CString strFileName, IN const int &nWidth, IN const int &nHeight, IN BYTE *pImage);

	UINT32 ClearRegion();

	
	UINT32 AddRegion(vector<RegionData> &vecRegionData, IN const float &fMoveX = 0.f, IN const float &fMoveY = 0.f);
	UINT32 SaveRegion(IN const CString &strDst);

	//Make Line Region
	//UINT32 MakeRegion(IN vector<MakeFeatureData>& vecPointData, OUT vector<RegionData> &vecRegionData);

	//Pointer를 받아 Save
	UINT32 SaveImage(IN const CString strDstFileName, IN const CString strFileExt, IN const int &nWidth, IN const int &nHeight, IN BYTE *pImage);

	UINT32 Save_As_8bit(IN const CString &strSrcFileName, IN const CString &strDstFileName, IN int iChannel = 1);

	//아직 테스트하지 않음.
	//Swawth image_v2
	UINT32 SetSplitSize(IN const int &nCountX, IN const int &nCountY);
	UINT32 SetSplitImage(IN const int &nIndexX, IN const int &nIndexY, IN const int &nWidth, IN const int &nHeight, IN BYTE *pImage);
	UINT32 MakeSwathImage(IN const CString &strDst, vector<vector<SPLIT_POS>> &vecvecSplitPos, IN const int &nOverlap,
		IN const int &nTotalWidth, IN const int &nTotalHeight, IN const BOOL &bSaveRaw = FALSE);
	UINT32 ClearSplitImage();

	UINT32 Save_As_Raw(IN const CString &strSrcFileName, IN const CString &strDstFileName);
	UINT32 Save_As_Raw(IN const CString strFileName, IN const int &nWidth, IN const int &nHeight, IN BYTE *pImage);

	// 2023.01.18 KJH ADD
	// Hobject to Raw 직접 저장 위해 추가
	UINT32 Save_As_Raw_V2(IN const HObject &hoImage, IN const CString &strDstFileName);

private:
	
	UINT32 Save_As_Raw(IN const HObject &hoImage, IN const CString &strDstFileName);
private :
	vector<vector<HObject>> m_vecSplitImage;
	vector<vector<BYTE*>> m_vecOrgData;
	
	HObject  m_hoFeatureConcat;
};

