#pragma once

#include "../DxDraw/Def_Viewer.h"

#pragma warning( disable: 4805)//
#pragma comment( lib, "halconcppxl.lib" )
#include "HalconCpp.h"
using namespace HalconCpp;

#pragma comment( lib, "hdevenginecpp.lib" )
#include "HDevEngineCpp.h"
using namespace HDevEngineCpp;

#include <vector>
using namespace std;

typedef struct _RegionData
{
	bool bPolarity;
	HObject hObject;

	_RegionData()
	{
		bPolarity = true;
		GenEmptyObj(&hObject);
	}
	~_RegionData()
	{
		GenEmptyObj(&hObject);
	}
}RegionData;

typedef struct _RegionPointData
{
	bool bPolarity;
	vector<GEODRAW> vecPointData;

}RegionPointData;



class CUtil
{
public:
	CUtil() {};
	~CUtil() {};

	static void DeleteAllFiles(IN const CString &strPath) {
		CFileFind finder;

		BOOL bWorking = finder.FindFile((CString)strPath + _T("*.*"));

		BOOL bDelete = FALSE;
		while (bWorking)
		{
			bWorking = finder.FindNextFile();
			if (finder.IsDots())
			{
				continue;
			}
			else if (finder.IsDirectory())
			{
				CString filePath = finder.GetFilePath() + _T("\\");
				DeleteAllFiles(filePath);

				RemoveDirectory(filePath);
			}
			else
			{
				CString filePath = finder.GetFilePath();
#ifdef _UNICODE
				bDelete = DeleteFileW(filePath);
#else
				bDelete = DeleteFileA(filePath);
#endif
			}
		}

		finder.Close();
	}
};
