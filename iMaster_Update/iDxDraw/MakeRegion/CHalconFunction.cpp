//#include "../pch.h"
#include "pch.h"
#include "CHalconFunction.h"
#include "../CDrawFunction.h"

#include "shlobj_core.h"

CHalconFunction::CHalconFunction()
{

}

CHalconFunction::~CHalconFunction()
{

}

UINT32 CHalconFunction::CreateMergeImage(IN const CString &strDst, vector<vector<SPLIT_POS>> &vecvecSplitPos,
	IN const int &nOverlap, IN const int &nTotalWidth, IN const int &nTotalHeight, IN BOOL bSaveRaw)
{
	try
	{
		CFileFind file;

		// Local iconic variables
		vector<HObject> vecho_ObjectsConcat;//Col 영상 모음//RowCount 만큼 존재함
		vector<HObject> vecho_TiledImage_Col;//Col 영상 합진결과//RowCount 만큼 존재함
		HObject  ho_Image;
		HObject  ho_ObjectsConcat;//Total col
		HObject ho_TiledImage;//Out Image;

		// Local control variables
		HTuple  hv_Index, hv_Width, hv_Height, hv_Channels;
		HTuple  hv_OffsetCols, hv_OffsetRows, hv_nRow1s, hv_nCol1s;
		HTuple  hv_nRow2s, hv_nCol2s, hv_nTotalWidth, hv_nTotalHeight;

		HObject  ho_Red, ho_Green, ho_Blue;
		HObject  ho_Hue, ho_Saturation, ho_Intensity;

		int nRowCount = static_cast<int>(vecvecSplitPos.size());
		if (nRowCount <= 0) return RESULT_BAD;
		int nColCount = static_cast<int>(vecvecSplitPos[0].size());

		vecho_ObjectsConcat.resize(nRowCount);
		vecho_TiledImage_Col.resize(nRowCount);

		//Make Column Concat Image
		for (int i = 0; i < nRowCount; i++)
		{
			for (int j = 0; j < nColCount; j++)
			{
				if (!file.FindFile(vecvecSplitPos[i][j].strPathName))
					return RESULT_BAD;

				ReadImage(&ho_Image, HTuple(vecvecSplitPos[i][j].strPathName));

				CountChannels(ho_Image, &hv_Channels);

				if (hv_Channels == 3)
				{
					AccessChannel(ho_Image, &ho_Intensity, 1);
					//Color to Gray
				//	Decompose3(ho_Image, &ho_Red, &ho_Green, &ho_Blue);
				//	TransFromRgb(ho_Red, ho_Green, ho_Blue, &ho_Hue, &ho_Saturation, &ho_Intensity,
				//		"hsv");

					if (j == 0)
						vecho_ObjectsConcat[i] = ho_Intensity;
					else
						ConcatObj(vecho_ObjectsConcat[i], ho_Intensity, &vecho_ObjectsConcat[i]);

					GetImageSize(ho_Intensity, &hv_Width, &hv_Height);
				}
				else if (hv_Channels == 1)
				{
					if (j == 0)					
						vecho_ObjectsConcat[i] = ho_Image;					
					else					
						ConcatObj(vecho_ObjectsConcat[i], ho_Image, &vecho_ObjectsConcat[i]);					

					GetImageSize(ho_Image, &hv_Width, &hv_Height);
				}
				else
					return RESULT_BAD;				
			}			
		}

		//
		hv_OffsetRows.Clear();
		hv_OffsetCols.Clear();
		hv_nRow1s.Clear();
		hv_nCol1s.Clear();
		hv_nRow2s.Clear();
		hv_nCol2s.Clear();
		for (int j = 0; j < nColCount; j++)
		{
			hv_OffsetRows.Append(0);
			hv_OffsetCols.Append(vecvecSplitPos[0][j].ptPos.x);
			hv_nRow1s.Append(0);
			hv_nCol1s.Append(0);
			hv_nRow2s.Append(hv_Height);
			hv_nCol2s.Append(hv_Width);
		}

		//Make Column Tiled Image
		for (int i = 0; i < nRowCount; i++)
		{
			TileImagesOffset(vecho_ObjectsConcat[i], &vecho_TiledImage_Col[i], hv_OffsetRows, hv_OffsetCols,
				hv_nRow1s, hv_nCol1s, hv_nRow2s, hv_nCol2s, HTuple(nTotalWidth), hv_Height);
		}

		//Make Row Concat Image
		for (int i = 0; i < nRowCount; i++)
		{
			if (i == 0)
			{
				ho_ObjectsConcat = vecho_TiledImage_Col[i];
			}
			else
			{
				ConcatObj(ho_ObjectsConcat, vecho_TiledImage_Col[i], &ho_ObjectsConcat);
			}
		}

		//
		hv_OffsetRows.Clear();
		hv_OffsetCols.Clear();
		hv_nRow1s.Clear();
		hv_nCol1s.Clear();
		hv_nRow2s.Clear();
		hv_nCol2s.Clear();
		for (int i = 0; i < nRowCount; i++)
		{
			if (i == nRowCount - 1)
			{
				hv_OffsetRows.Append(0);
			}
			else
			{
				//hv_OffsetRows.Append(hv_Height * (nRowCount - 1 - i) + (i > 0 ? nOverlap * (i - 1) : 0));
				hv_OffsetRows.Append(nTotalHeight - hv_Height * (i + 1) + (i > 0 ? nOverlap * i : 0));
			}
			hv_OffsetCols.Append(0);
			hv_nRow1s.Append(0);
			hv_nCol1s.Append(0);
			hv_nRow2s.Append(hv_Height);
			hv_nCol2s.Append(nTotalWidth);
		}

		TileImagesOffset(ho_ObjectsConcat, &ho_TiledImage, hv_OffsetRows, hv_OffsetCols,
			hv_nRow1s, hv_nCol1s, hv_nRow2s, hv_nCol2s, HTuple(nTotalWidth), HTuple(nTotalHeight));

		fs::path fsPath;
		fsPath = std::string(CT2CA(strDst));
		CString strExt = CA2CT(fsPath.extension().string().c_str());
		CString strFileName = CA2CT(fsPath.filename().string().c_str());
		CString strPathName = CA2CT(fsPath.parent_path().string().c_str());

		CreateDirectory(strPathName, NULL);

		CString strDstPathName;
		strDstPathName.Format(_T("%s\\%s.tif"), strPathName, strFileName.Mid(0, strFileName.GetLength() - strExt.GetLength()));
		
		if ( (static_cast<LONGLONG>(nTotalHeight) * static_cast<LONGLONG>(nTotalWidth)) >= 4000000000*0.8)//4Gb
		{
			WriteImage(ho_TiledImage, "bigtiff", 0, HTuple(strDstPathName));			
		}
		else
		{
			WriteImage(ho_TiledImage, "tiff", 0, HTuple(strDstPathName));			
		}
		
		if (bSaveRaw)
			Save_As_Raw(strDstPathName, strDstPathName);
			
	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Error_MergeImage : %s\n"), strError);

		return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 CHalconFunction::SetSplitSize(IN const int &nCountX, IN const int &nCountY)
{
	ClearSplitImage();

	try
	{
		m_vecSplitImage.resize(nCountY);
		m_vecOrgData.resize(nCountY);
		for (int i = 0; i < nCountY; i++)
		{
			m_vecSplitImage[i].resize(nCountX);
			m_vecOrgData[i].resize(nCountX);
			for (int j = 0; j < nCountX; j++)
			{
				GenEmptyObj(&m_vecSplitImage[i][j]);
			}
		}
	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Error_SetSplitSize : %s\n"), strError);

		return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 CHalconFunction::ClearSplitImage()
{
	int nCountY = static_cast<int>(m_vecSplitImage.size());
	for (int i = 0; i < nCountY; i++)
	{
		int nCountX = static_cast<int>(m_vecSplitImage[i].size());
		for (int j = 0; j < nCountX; j++)
		{
			delete m_vecOrgData[i][j];
		}
		m_vecSplitImage[i].clear();	
		m_vecOrgData[i].clear();

	}
	m_vecSplitImage.clear();
	m_vecOrgData.clear();

	return RESULT_GOOD;
}

UINT32 CHalconFunction::SetSplitImage(IN const int &nIndexX, IN const int &nIndexY, IN const int &nWidth, IN const int &nHeight, IN BYTE *pImage)
{
	try
	{
		HObject ho_Image;
		GenImage1Extern(&ho_Image, "byte", HTuple(nWidth), HTuple(nHeight), Hlong(pImage), 0);

		m_vecSplitImage[nIndexY][nIndexX] = ho_Image;
		m_vecOrgData[nIndexY][nIndexX] = pImage;
	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Error_SetSplitImage : %s\n"), strError);

		return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 CHalconFunction::MakeSwathImage(IN const CString &strDst, vector<vector<SPLIT_POS>> &vecvecSplitPos, IN const int &nOverlap,
	IN const int &nTotalWidth, IN const int &nTotalHeight, IN const BOOL &bSaveRaw)
{
	try
	{
		CFileFind file;

		// Local iconic variables
		vector<HObject> vecho_ObjectsConcat;//Col 영상 모음//RowCount 만큼 존재함
		vector<HObject> vecho_TiledImage_Col;//Col 영상 합진결과//RowCount 만큼 존재함
		HObject  ho_ObjectsConcat;//Total col
		HObject ho_TiledImage;//Out Image;

		// Local control variables
		HTuple  hv_Index, hv_Width, hv_Height, hv_Channels;
		HTuple  hv_OffsetCols, hv_OffsetRows, hv_nRow1s, hv_nCol1s;
		HTuple  hv_nRow2s, hv_nCol2s, hv_nTotalWidth, hv_nTotalHeight;
		
		int nRowCount = static_cast<int>(vecvecSplitPos.size());
		if (nRowCount <= 0) return RESULT_BAD;
		int nColCount = static_cast<int>(vecvecSplitPos[0].size());

		vecho_ObjectsConcat.resize(nRowCount);
		vecho_TiledImage_Col.resize(nRowCount);
		
		//Make Column Concat Image
		for (int i = 0; i < nRowCount; i++)
		{
			GenEmptyObj(&vecho_ObjectsConcat[i]);
			GenEmptyObj(&vecho_TiledImage_Col[i]);
			for (int j = 0; j < nColCount; j++)
			{
				if (j == 0)
					vecho_ObjectsConcat[i] = m_vecSplitImage[i][j];
				else
					ConcatObj(vecho_ObjectsConcat[i], m_vecSplitImage[i][j], &vecho_ObjectsConcat[i]);

				GetImageSize(m_vecSplitImage[i][j], &hv_Width, &hv_Height);
			}
		}

		//
		hv_OffsetRows.Clear();
		hv_OffsetCols.Clear();
		hv_nRow1s.Clear();
		hv_nCol1s.Clear();
		hv_nRow2s.Clear();
		hv_nCol2s.Clear();
		for (int j = 0; j < nColCount; j++)
		{
			hv_OffsetRows.Append(0);
			hv_OffsetCols.Append(vecvecSplitPos[0][j].ptPos.x);
			hv_nRow1s.Append(0);
			hv_nCol1s.Append(0);
			hv_nRow2s.Append(hv_Height);
			hv_nCol2s.Append(hv_Width);
		}

		//Make Column Tiled Image
		for (int i = 0; i < nRowCount; i++)
		{
			TileImagesOffset(vecho_ObjectsConcat[i], &vecho_TiledImage_Col[i], hv_OffsetRows, hv_OffsetCols,
				hv_nRow1s, hv_nCol1s, hv_nRow2s, hv_nCol2s, HTuple(nTotalWidth), hv_Height);
		}

		//Make Row Concat Image
		for (int i = 0; i < nRowCount; i++)
		{
			if (i == 0)
			{
				ho_ObjectsConcat = vecho_TiledImage_Col[i];
			}
			else
			{
				ConcatObj(ho_ObjectsConcat, vecho_TiledImage_Col[i], &ho_ObjectsConcat);
			}
		}

		//
		hv_OffsetRows.Clear();
		hv_OffsetCols.Clear();
		hv_nRow1s.Clear();
		hv_nCol1s.Clear();
		hv_nRow2s.Clear();
		hv_nCol2s.Clear();
		for (int i = 0; i < nRowCount; i++)
		{
			if (i == nRowCount - 1)
			{
				hv_OffsetRows.Append(0);
			}
			else
			{
				//hv_OffsetRows.Append(hv_Height * (nRowCount - 1 - i) + (i > 0 ? nOverlap * (i - 1) : 0));
				hv_OffsetRows.Append(nTotalHeight - hv_Height * (i + 1) + (i > 0 ? nOverlap * i : 0));
			}
			hv_OffsetCols.Append(0);
			hv_nRow1s.Append(0);
			hv_nCol1s.Append(0);
			hv_nRow2s.Append(hv_Height);
			hv_nCol2s.Append(nTotalWidth);
		}

		TileImagesOffset(ho_ObjectsConcat, &ho_TiledImage, hv_OffsetRows, hv_OffsetCols,
			hv_nRow1s, hv_nCol1s, hv_nRow2s, hv_nCol2s, HTuple(nTotalWidth), HTuple(nTotalHeight));

		fs::path fsPath;
		fsPath = std::string(CT2CA(strDst));
		CString strExt = CA2CT(fsPath.extension().string().c_str());
		CString strFileName = CA2CT(fsPath.filename().string().c_str());
		CString strPathName = CA2CT(fsPath.parent_path().string().c_str());

		::SHCreateDirectoryEx(NULL, strPathName, NULL);

		CString strDstPathName;

		// 2023.01.18 KJH ADD
		// 기존 : halcon -> tif 파일 저장 후 raw 파일로 다시 저장 
		// 수정 : hacon -> raw 파일로 저장 
		strDstPathName.Format(_T("%s\\%s.raw"), strPathName, strFileName.Mid(0, strFileName.GetLength() - strExt.GetLength()));
		//strDstPathName.Format(_T("%s\\%s.tif"), strPathName, strFileName.Mid(0, strFileName.GetLength() - strExt.GetLength()));

		//if ((static_cast<LONGLONG>(nTotalHeight) * static_cast<LONGLONG>(nTotalWidth)) >= 4000000000 * 0.8)//4Gb
		//{
		//	WriteImage(ho_TiledImage, "bigtiff", 0, HTuple(strDstPathName));
		//}
		//else
		//{
		//	WriteImage(ho_TiledImage, "tiff", 0, HTuple(strDstPathName));
		//}

		if (bSaveRaw)
		{
			//Save_As_Raw(ho_TiledImage, strDstPathName);
			Save_As_Raw_V2(ho_TiledImage, strDstPathName);
		}
	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Error_MakeSwathImage : %s\n"), strError);

		return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 CHalconFunction::CreateMergeRegion(IN const CString &strDst, vector<vector<SPLIT_POS>> &vecvecSplitPos, IN const int &nOverlap,
	IN const int &nTotalWidth, IN const int &nTotalHeight)
{
	try
	{
		SetSystem("clip_region", "false");
		SetSystem("width", nTotalWidth);
		SetSystem("height", nTotalHeight);

		HObject ho_Region;
		HObject ho_Temp;
		HObject ho_MoveX;
		HObject ho_MoveY;
		HObject ho_Total;
		int nRowCount = static_cast<int>(vecvecSplitPos.size());
		if (nRowCount <= 0) return RESULT_BAD;
		int nColCount = static_cast<int>(vecvecSplitPos[0].size());

		for (int i = 0; i < nRowCount; i++)
		{
			GenEmptyObj(&ho_Region);

			for (int j = 0; j < nColCount; j++)
			{
				ReadObject(&ho_Temp, HTuple(vecvecSplitPos[i][j].strPathName_Region));

				MoveRegion(ho_Temp, &ho_MoveX, 0, vecvecSplitPos[i][j].ptPos.x - 1);

				if (j == 0)
				{
					ho_Region = ho_MoveX;
				}
				else
				{
					ConcatObj(ho_Region, ho_MoveX, &ho_Region);
				}
				
			}
			
			int nOffsetY;
			if (i == nRowCount - 1)
			{
				nOffsetY = 0;
			}
			else
			{
				nOffsetY = nTotalHeight - vecvecSplitPos[i][0].nSplitHeight_Pixel * (i + 1) + (i > 0 ? nOverlap * i : 0);
			}
			MoveRegion(ho_Region, &ho_MoveY, nOffsetY, 0);

			if (i == 0)
			{
				ho_Total = ho_MoveY;
			}
			else
			{
				ConcatObj(ho_Total, ho_MoveY, &ho_Total);
			}
			
		}

		fs::path fsPath;
		fsPath = std::string(CT2CA(strDst));
		CString strExt = CA2CT(fsPath.extension().string().c_str());
		CString strFileName = CA2CT(fsPath.filename().string().c_str());
		CString strPathName = CA2CT(fsPath.parent_path().string().c_str());

		CString strDstPathName;
		strDstPathName.Format(_T("%s\\%s.hobj"), strPathName, strFileName.Mid(0, strFileName.GetLength() - strExt.GetLength()));

		WriteRegion(ho_Total, HTuple(strDstPathName));
		
	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Error_CreateMergeRegion : %s\n"), strError);

		return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 CHalconFunction::CreateConcatRegion(IN vector<CString> &vecSrcRegion, CString  &strDstPathName, IN const BOOL &bSrcDelete)
{
	try
	{
		SetSystem("clip_region", "false");

		HObject ho_Region;
		HObject ho_Concat;
		GenEmptyRegion(&ho_Concat);
		int nCount = static_cast<int>(vecSrcRegion.size());
		for (int i = 0; i < nCount; i++)
		{
			if (PathFileExists(vecSrcRegion[i]))
			{//파일이 존재한다
				ReadRegion(&ho_Region, HTuple(vecSrcRegion[i]));
			}
			else
			{//존재하지 않음
				GenEmptyRegion(&ho_Region);
			}

			if (i == 0)
			{
				ho_Concat = ho_Region;
			}
			else
			{
				ConcatObj(ho_Concat, ho_Region, &ho_Concat);
			}
		}

		WriteRegion(ho_Concat, HTuple(strDstPathName));

		if (bSrcDelete == TRUE)
		{
			for (int i = 0; i < nCount; i++)
			{
				if (PathFileExists(vecSrcRegion[i]))
				{
#ifdef _UNICODE
					DeleteFileW(vecSrcRegion[i]);
#else
					DeleteFileA(vecSrcRegion[i]);
#endif	
				}
			}
		}

	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Error_CreateConcatRegion : %s\n"), strError);

		return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 CHalconFunction::CreateUnitAlignRegion(IN const CString &strLayerName, IN const int &nAlignCount, IN FovInfo_t *pFovInfo, CellInfo_t *pCellInfo, 
	IN const CString &strSrcPathName, IN const CString &strDstPathName, IN const CString &strImageType, IN const BOOL &bIsBlockMode)
{
	if (strLayerName == _T("")) return RESULT_BAD;
	if (nAlignCount == 0) return RESULT_BAD;
	if (pFovInfo == nullptr) return RESULT_BAD;
	if (pCellInfo == nullptr) return RESULT_BAD;

	//로드 해야할 Align 패턴 경로
	vector<CString> vecLoadPathName;
	CString strFullPathName;
	for (int i = 0; i < nAlignCount; i++)
	{
		strFullPathName.Format(_T("%s\\Align_%d.%s"), strSrcPathName, i + 1, strImageType);
		vecLoadPathName.push_back(strFullPathName);
	}

	try
	{
		CFileFind file;
		HObject hoAlignImg;
		vector<HObject> vecHo_AlignRegion;
		vecHo_AlignRegion.resize(nAlignCount);
		//패턴 로드 먼저 한다.
		for (int i = 0; i < nAlignCount; i++)
		{

			if (!file.FindFile(vecLoadPathName[i]))
				return RESULT_BAD;

			ReadImage(&hoAlignImg, HTuple(vecLoadPathName[i]));
			FastThreshold(hoAlignImg, &vecHo_AlignRegion[i], 10, 255, 1);
			//Threshold(hoAlignImg, &vecHo_AlignRegion[i], 30, 255);
		}
		
		HObject ho_AlignTemp;
		HObject ho_Union;//Align Index가 같은것 끼리
		HObject ho_Concat;//Union을 Concat

		int iSwathNum = static_cast<int>(pFovInfo->stBasic.swathNum);
		if (iSwathNum <= 0) return RESULT_BAD;
		int iCellCol = static_cast<int>(pCellInfo->stBasic.cellNum.cx) / iSwathNum;
		int iCellRow = static_cast<int>(pCellInfo->stBasic.cellNum.cy);

		int iSwathNumInBlock = static_cast<int>(pFovInfo->stBasic.swathNumInBlock);		
		int	iCellRowInBlock = static_cast<int>(pCellInfo->stBasic.cell_In_Block.cy);		
		if (iSwathNumInBlock <= 0 || iCellRowInBlock <= 0) return RESULT_BAD;

		int nCellWidth = abs(pFovInfo->vcFovData[0]->rcRectPxl.right - pFovInfo->vcFovData[0]->rcRectPxl.left);
		int nCellHeight = abs(pFovInfo->vcFovData[0]->rcRectPxl.bottom - pFovInfo->vcFovData[0]->rcRectPxl.top);
		SetSystem("width", nCellWidth);
		SetSystem("height", nCellHeight);

		UINT32 iOffsetX = pFovInfo->vcFovData[0]->rcRectPxl.left;
		UINT32 iOffsetY = pFovInfo->vcFovData[0]->rcRectPxl.top;
		CRect rtAlign;

		int iSwathIdx = 0;
		for (auto itSwath : pFovInfo->vcFovData)
		{
			if (bIsBlockMode && iSwathIdx >= iSwathNumInBlock)
				break;

			for (auto itCell : pCellInfo->vcCellData)
			{
				if (itSwath->fovIdx.x != itCell->swathIdx)
					continue;

				if (bIsBlockMode && (itCell->fovIdx.y / iCellRowInBlock))
					break;

				int iCell = itCell->swathIdx * (iCellCol * iCellRow) + (iCellCol * itCell->fovIdx.y) + itCell->fovIdx.x;

				auto itAlign_in_Cell = pCellInfo->vcAlign_in_Cell.find(iCell);
				if (itAlign_in_Cell == pCellInfo->vcAlign_in_Cell.end())
				{
					continue;
				}

				vector<CellAlignInfo_t> vecAlignInfo = itAlign_in_Cell->second;
				int nAlignCount = static_cast<int>(vecAlignInfo.size());

				GenEmptyObj(&ho_Concat);
				for (int nAlign = 0; nAlign < nAlignCount; nAlign++)
				{
					if (nAlign >= static_cast<int>(vecHo_AlignRegion.size()))
					{
						return RESULT_BAD;
					}

					GenEmptyRegion(&ho_Union);
					int nUnionCount = static_cast<int>(vecAlignInfo[nAlign].vcInCell_Pixel.size());
					for (int nUnion = 0; nUnion < nUnionCount; nUnion++)
					{
						rtAlign = vecAlignInfo[nAlign].vcInCell_Pixel[nUnion];
						rtAlign.NormalizeRect();

						MoveRegion(vecHo_AlignRegion[nAlign], &ho_AlignTemp, rtAlign.top, rtAlign.left);
						//같은 인덱끼리 Union한다.

						if (nUnion == 0)
						{
							ho_Union = ho_AlignTemp;
						}
						else
						{
							Union2(ho_Union, ho_AlignTemp, &ho_Union);
						}
					}

					ConcatObj(ho_Concat, ho_Union, &ho_Concat);
				}

				strFullPathName.Format(_T("%s\\S%d_R%d_C%d_Align.hobj"), strDstPathName, itCell->swathIdx + 1, itCell->fovIdx.y + 1, itCell->fovIdx.x + 1);
				WriteRegion(ho_Concat, HTuple(strFullPathName));
			}
			iSwathIdx++;
		}
	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Error_CreateMergeRegion : %s\n"), strError);

		return RESULT_BAD;
	}

//	for (auto it : vecLoadPathName)
//	{
//#ifdef _UNICODE
//		DeleteFileW(it);
//#else
//		DeleteFileA(it);
//#endif	
//	}

	return RESULT_GOOD;
}

UINT32 CHalconFunction::ConvRegion(IN const CString &strSrcPathName, IN const CString &strDstPath)
{
	HObject ho_Image, ho_Intensity;
	HObject ho_Region;
	HTuple hv_Channels;
	HTuple hv_Width, hv_Height;

	try
	{
		ReadImage(&ho_Image, HTuple(strSrcPathName));

		CountChannels(ho_Image, &hv_Channels);

		if (hv_Channels == 3)
		{
			AccessChannel(ho_Image, &ho_Intensity, 1);
		}
		else
		{
			ho_Intensity = ho_Image;
		}

		GetImageSize(ho_Image, &hv_Width, &hv_Height);
		
		FastThreshold(ho_Intensity, &ho_Region, 10, 255, 1);

		CString strDstPathName;

		fs::path fsPath;
		fsPath = std::string(CT2CA(strSrcPathName));
		CString strExt = CA2CT(fsPath.extension().string().c_str());
		CString strPathName = CA2CT(fsPath.parent_path().string().c_str());
		CString strFileName = CA2CT(fsPath.filename().string().c_str());

		fsPath = std::string(CT2CA(strDstPath));
		strPathName = CA2CT(fsPath.parent_path().string().c_str());

		strDstPathName.Format(_T("%s\\%s.hobj"), strPathName, strFileName.Mid(0, strFileName.GetLength() - strExt.GetLength()));

		SetSystem("clip_region", "false");
		SetSystem("width", hv_Width);
		SetSystem("height", hv_Height);

		WriteRegion(ho_Region, HTuple(strDstPathName));

	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Error_ConvRegion : %s\n"), strError);

		return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 CHalconFunction::ConvRegion(IN const CString strFileName, IN const int &nWidth, IN const int &nHeight, IN BYTE *pImage)
{
	try
	{
		HObject ho_Image;
		HObject ho_Region;
		HTuple hv_Channels;
		HTuple hv_Width, hv_Height;
		
		GenImage1Extern(&ho_Image, "byte", HTuple(nWidth), HTuple(nHeight), Hlong(pImage), 0);

		GetImageSize(ho_Image, &hv_Width, &hv_Height);

		FastThreshold(ho_Image, &ho_Region, 10, 255, 1);

		CString strDstPathName;

		fs::path fsPath;
		fsPath = std::string(CT2CA(strFileName));
		CString strExt = CA2CT(fsPath.extension().string().c_str());
		CString strPathName = CA2CT(fsPath.parent_path().string().c_str());
		CString strFileName = CA2CT(fsPath.filename().string().c_str());
		
		strDstPathName.Format(_T("%s\\%s.hobj"), strPathName, strFileName.Mid(0, strFileName.GetLength() - strExt.GetLength()));

		SetSystem("clip_region", "false");
		SetSystem("width", hv_Width);
		SetSystem("height", hv_Height);

		WriteRegion(ho_Region, HTuple(strDstPathName));

	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Error_ConvRegion : %s\n"), strError);

		return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 CHalconFunction::ClearRegion()
{
	GenEmptyObj(&m_hoFeatureConcat);

	return RESULT_GOOD;
}

UINT32 CHalconFunction::AddRegion(vector<RegionData> &vecRegionData, IN const float &fMoveX, IN const float &fMoveY)
{
	HObject ho_Moved_Region;

	int nCount = static_cast<int>(vecRegionData.size());
	for (int i = 0; i < nCount; i++)
	{
		MoveRegion(vecRegionData[i].hObject, &ho_Moved_Region, fMoveY, fMoveX);

		if (vecRegionData[i].bPolarity == true)
		{
			ConcatObj(m_hoFeatureConcat, ho_Moved_Region, &m_hoFeatureConcat);
		}
		else
		{
			Difference(m_hoFeatureConcat, ho_Moved_Region, &m_hoFeatureConcat);
		}
	}

	return RESULT_GOOD;
}

UINT32 CHalconFunction::SaveRegion(IN const CString &strDst)
{
	try
	{
		//Save 시만 clip region을 true로 변경
		//Region 생성시 false, save 시, true
		SetSystem("clip_region", "true");

		HTuple hv_Height;
		GetSystem("height",&hv_Height);

		HObject ho_Mirror;
		MirrorRegion(m_hoFeatureConcat, &ho_Mirror, "row", hv_Height);

		WriteRegion(ho_Mirror, HTuple(strDst));

		SetSystem("clip_region", "false");
	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Error_SaveRegion : %s\n"), strError);

		return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 CHalconFunction::SaveImage(IN const CString strDstFileName, IN const CString strFileExt, 
	IN const int &nWidth, IN const int &nHeight, IN BYTE *pImage)
{
	try
	{
		HObject ho_Image;
		GenImage1Extern(&ho_Image, "byte", HTuple(nWidth), HTuple(nHeight), Hlong(pImage), 0);

		HTuple hv_Ext;
		if (strFileExt == _T("tif"))
			hv_Ext = _T("tiff deflate 1");
		else
			hv_Ext = HTuple(strFileExt);

		WriteImage(ho_Image, hv_Ext, 0, HTuple(strDstFileName));
	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Error_SaveImage : %s\n"), strError);

		return RESULT_BAD;
	}

	return RESULT_GOOD;
}

UINT32 CHalconFunction::Save_As_8bit(IN const CString &strSrcFileName, IN const CString &strDstFileName, IN int iChannel)
{
	CFileFind file;
	if (!file.FindFile(strSrcFileName))
		return RESULT_BAD;

	CString strFormat = strDstFileName.Right(3);
	strFormat.MakeLower();

	if (strFormat.CompareNoCase(_T("tif")) == 0)
		strFormat = _T("tiff");

	try
	{
		HObject  ho_Image, ho_ChImage;
		HTuple   hv_Channels;
		HTuple   hv_TargetCh = (HTuple)iChannel;

		ReadImage(&ho_Image, HTuple(strSrcFileName));

		CountChannels(ho_Image, &hv_Channels);

		if (hv_Channels == 3)
		{
			if (hv_TargetCh > hv_Channels)
				return RESULT_BAD;

			AccessChannel(ho_Image, &ho_ChImage, hv_TargetCh);

			WriteImage(ho_ChImage, HTuple(strFormat), 0, HTuple(strDstFileName));
		}
		else if (hv_Channels == 1)
		{
			WriteImage(ho_Image, HTuple(strFormat), 0, HTuple(strDstFileName));
		}
	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Error_Save_As_8bit : %s\n"), strError);

		return RESULT_BAD;
	}
	return RESULT_GOOD;
}

UINT32 CHalconFunction::Save_As_Raw(IN const CString &strSrcFileName, IN const CString &strDstFileName)
{
	CFileFind file;
	if (!file.FindFile(strSrcFileName))
		return RESULT_BAD;

	try
	{
		HObject ho_Image;

		ReadImage(&ho_Image, HTuple(strSrcFileName));

		return Save_As_Raw(ho_Image, strDstFileName);
	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Save_As_Raw : %s\n"), strError);

		return RESULT_BAD;
	}
	return RESULT_GOOD;
}

UINT32 CHalconFunction::Save_As_Raw(IN const CString strFileName, IN const int &nWidth, IN const int &nHeight, IN BYTE *pImage)
{
	try
	{
		HObject ho_Image;
		HObject ho_Region;
		HTuple hv_Channels;
		HTuple hv_Width, hv_Height;

		GenImage1Extern(&ho_Image, "byte", HTuple(nWidth), HTuple(nHeight), Hlong(pImage), 0);
		
		Save_As_Raw(ho_Image, strFileName);

	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Error_Save_As_Raw : %s\n"), strError);

		return RESULT_BAD;
	}

	return RESULT_GOOD;
}

//temporary Support for AVI
UINT32 CHalconFunction::Save_As_Raw(IN const HObject &hoImage, IN const CString &strDstFileName)
{
	CString strOutFileName = strDstFileName;
	CString strFormat = strOutFileName.Right(3);
	strFormat.MakeLower();

	if (strFormat.CompareNoCase(_T("raw")) != 0)
		strOutFileName.Replace(strFormat, _T("raw"));

	//CFile file;
	FILE *fp;
	int errorno = 0;
	if (errorno != _wfopen_s(&fp, strOutFileName, L"w+b"))
		return RESULT_BAD;

	try
	{
		HTuple	 hv_Pointer, hv_Type, hv_Width, hv_Height;

		GetImagePointer1(hoImage, &hv_Pointer, &hv_Type, &hv_Width, &hv_Height);

		BYTE *pbSrc = (LPBYTE)hv_Pointer.L();
		if (pbSrc == nullptr) return RESULT_BAD;

		size_t width = static_cast<size_t>(hv_Width.L());
		size_t height = static_cast<size_t>(hv_Height.L());
		size_t length = width * height;

	//	long lLength = static_cast<long>(hv_Width.L() * hv_Height.L());
		if (length <= 0) return RESULT_BAD;

		fwrite(pbSrc, sizeof(BYTE), length, fp);
		fclose(fp);

		//검증.....
/*		if (errorno != _wfopen_s(&fp, strOutFileName, L"rb"))
			return RESULT_BAD;
		
		BYTE *pbDst = new BYTE[length];
		if (pbDst == nullptr)
			return RESULT_BAD;

		fseek(fp, 0L, SEEK_END);
		long len = ftell(fp);///sizeof(short) - WIDTH*HEIGHT;
		fseek(fp, len - length * sizeof(BYTE), SEEK_SET);

		//파일 메모리에 적재 및 읽기 에러 체크
		long ReadCount = static_cast<long>(fread(pbDst, sizeof(BYTE), length, fp));
		if (ReadCount < length)
			return RESULT_BAD;

		fclose(fp);

		HObject ho_result;

		GenImage1(&ho_result, "byte", hv_Width, hv_Height, (Hlong)pbDst);

		strOutFileName.Replace(_T("raw"), _T("bmp"));
		WriteImage(ho_result, "bmp", 0, HTuple(strOutFileName));

		if (pbDst) delete[]pbDst;
		pbDst = nullptr;
*/
	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Save_As_Raw : %s\n"), strError);

		return RESULT_BAD;
	}
	return RESULT_GOOD;
}

// 2023.01.18 KJH ADD
// Hobject to Raw 직접 저장 위해 추가
UINT32 CHalconFunction::Save_As_Raw_V2(IN const HObject &hoImage, IN const CString &strDstFileName)
{	

	CString strOutFileName = strDstFileName;
	CString strFormat = strOutFileName.Right(3);
	strFormat.MakeLower();

	if (strFormat.CompareNoCase(_T("raw")) != 0)
		strOutFileName.Replace(strFormat, _T("raw"));

	//CFile file;
	FILE *fp;
	int errorno = 0;
	if (errorno != _wfopen_s(&fp, strOutFileName, L"w+b"))
		return RESULT_BAD;

	try
	{
		HTuple	 hv_Pointer, hv_Type, hv_Width, hv_Height;

		GetImagePointer1(hoImage, &hv_Pointer, &hv_Type, &hv_Width, &hv_Height);

		BYTE *pbSrc = (LPBYTE)hv_Pointer.L();
		if (pbSrc == nullptr) return RESULT_BAD;

		size_t width = static_cast<size_t>(hv_Width.L());
		size_t height = static_cast<size_t>(hv_Height.L());
		size_t length = width * height;

		//	long lLength = static_cast<long>(hv_Width.L() * hv_Height.L());
		if (length <= 0) return RESULT_BAD;

		fwrite(pbSrc, sizeof(BYTE), length, fp);
		fclose(fp);

	}
	catch (HException &error)
	{
		HTuple ErrMsgHTuple;
		ErrMsgHTuple = error.ErrorMessage();

		CString strError = ErrMsgHTuple[0].S();
		TRACE(_T("Save_As_Raw : %s\n"), strError);

		return RESULT_BAD;
	}
	return RESULT_GOOD;
}