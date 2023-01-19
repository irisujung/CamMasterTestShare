// iUtilities.cpp : 정적 라이브러리를 위한 함수를 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "iUtilities.h"

#include "shlobj_core.h"

CString CUtils::GetVersionInfo(const CString strFile)
{
	CString strVersion = L"";
	DWORD  verHandle = NULL;
	DWORD  verSize = GetFileVersionInfoSize(strFile, &verHandle);
	if (verSize != NULL)
		return strVersion;

	UINT   size = 0;
	LPBYTE lpBuffer = NULL;
	LPSTR verData = new char[verSize];

	if (GetFileVersionInfo(strFile, verHandle, verSize, verData))
	{
		if (VerQueryValue(verData, L"\\", (VOID FAR * FAR*) & lpBuffer, &size))
		{
			if (size)
			{
				VS_FIXEDFILEINFO* verInfo = (VS_FIXEDFILEINFO*)lpBuffer;
				if (verInfo->dwSignature == 0xfeef04bd)
				{
					// Doesn't matter if you are on 32 bit or 64 bit,
					// DWORD is always 32 bits, so first two revision numbers
					// come from dwFileVersionMS, last two come from dwFileVersionLS
					strVersion.Format(L"%d.%d.%d.%d",
						(verInfo->dwFileVersionMS >> 16) & 0xffff,
						(verInfo->dwFileVersionMS >> 0) & 0xffff,
						(verInfo->dwFileVersionLS >> 16) & 0xffff,
						(verInfo->dwFileVersionLS >> 0) & 0xffff
					);
				}
			}
		}
	}
	delete[] verData;

	return strVersion;
}

CString CUtils::GetFileInfo(UINT32 mode, LPCTSTR strFullPath)
{
	char szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szName[_MAX_FNAME], szExt[_MAX_EXT];
	_splitpath_s((CStringA)strFullPath, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szName, _MAX_FNAME, szExt, _MAX_EXT);

	switch (mode)
	{
	case 0:
		return szDrive;
	case 1:
		return szDir;
	case 2:
		return szName;
	case 3:
		return szExt;
	default:
		return szName;
	}
}

CString CUtils::GetAppPath()
{
	char cPath[4096];
	CString strPath = L"", strMsg;

	DWORD dwRes = GetModuleFileName(NULL, LPWCH(cPath), 4096);

	if (dwRes == 0)
	{
		dwRes = GetLastError();
		strMsg.Format(L"%d", dwRes);
		strMsg = L"Error :" + strMsg;
		return L"";
	}

	strPath.Format(L"%s", cPath);
	strPath = strPath.Left(strPath.ReverseFind('\\'));

	return strPath;
}

CString CUtils::GetDriveLetter()
{
	CString strAppPath = GetAppPath();	
	return strAppPath.Mid(0, 2); //Gives the Drive letter name
}

UINT32 CUtils::RemoveDirectoryFile(LPCTSTR PathDir)
{
	if (PathDir == NULL) 
		return RESULT_BAD;

	BOOL bRst = RESULT_BAD;
	CString szNextDirPath, szRoot;
	CFileFind filefinder;

	szRoot.Format(L"%s\\*.*", PathDir);
	bRst = filefinder.FindFile(szRoot);
	if (bRst)
	{
		while (bRst)
		{
			bRst = filefinder.FindNextFile();
			if (filefinder.IsDots()) continue;
			if (filefinder.IsDirectory())
			{
				RemoveDirectoryFile(filefinder.GetFilePath());
			}
			else bRst = DeleteFile(filefinder.GetFilePath());
		}
		filefinder.Close();
		return RemoveDirectory(PathDir);
	}
	else 
		return bRst;
}

UINT32 CUtils::MoveDirectoryFile(BOOL bSrcDelete, LPCTSTR strFrom, LPCTSTR strTo)
{
	CString search_path, src_path, dst_path;
	WIN32_FIND_DATA file_data;

	search_path.Format(L"%s*.*", strFrom);
	HANDLE search_handle = FindFirstFile(search_path, &file_data);

	if (INVALID_HANDLE_VALUE != search_handle)
	{
		do {
			if (FILE_ATTRIBUTE_ARCHIVE & file_data.dwFileAttributes)
			{
				src_path.Format(L"%s%s", strFrom, file_data.cFileName);
				dst_path.Format(L"%s%s", strTo, file_data.cFileName);

				MoveFile(src_path, dst_path);
			}
			else if (FILE_ATTRIBUTE_DIRECTORY & file_data.dwFileAttributes)
			{
				if (_tcscmp(L".", file_data.cFileName) && _tcscmp(L"..", file_data.cFileName))
				{
					src_path.Format(L"%s%s\\", strFrom, file_data.cFileName);
					dst_path.Format(L"%s%s\\", strTo, file_data.cFileName);

					CreateDirectory(dst_path, NULL);
					MoveDirectoryFile(TRUE, src_path, dst_path);
				}
			}
		} while (FindNextFile(search_handle, &file_data));

		FindClose(search_handle);
	}

	if (bSrcDelete) 
		RemoveDirectory(strFrom);

	return RESULT_GOOD;
}

UINT32 CUtils::GetCoreCount()
{
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	return static_cast<UINT32>(systemInfo.dwNumberOfProcessors);
}

UINT32 CUtils::CheckCreateFolder(CString strFolderPath)
{ 
	BOOL bRes;
	DWORD dErr = ERROR_SUCCESS;

	INT32 iDepth = 0;
	CString strTemp = L"", strFolder = L"";
	INT32 iPos = strFolderPath.Find('\\', 3);

	while (iPos > 0)
	{
		strTemp = strFolderPath.Mid(0, iPos);
		if (iDepth < 1)
		{
			iDepth++;
			AfxExtractSubString(strFolder, strTemp, 1, '\\');
			if (strFolder.GetLength() > 0)
				CreateDirectory(strTemp, NULL);
		}
		else
		{
			bRes = CreateDirectory(strTemp, NULL);
			if (!bRes)
			{
				dErr = GetLastError();
			}
		}

		iPos = strFolderPath.Find('\\', iPos + 1);
	}

	if (strFolderPath.Compare(strTemp) != 0)
	{
		bRes = CreateDirectory(strFolderPath, NULL);
		if (!bRes)
		{
			dErr = GetLastError();
		}
	}

	if (dErr == ERROR_PATH_NOT_FOUND)
		return RESULT_BAD;

	return RESULT_GOOD;
}

UINT32 CUtils::CheckFile(CString strFilePath)
{
	if (strFilePath.GetLength() < 1) 
		return RESULT_BAD;

	CFile FL;
	CString strMsg = L"Can not read File: ";
	if (!FL.Open(strFilePath, CFile::modeRead))
		return RESULT_BAD;

	FL.Close();
	return RESULT_GOOD;
}

CHAR* CUtils::ConvertWideCharToAnsiString(CString strData)
{
	if (strData.GetLength() < 1)
		return nullptr;

	const size_t newsizew = strData.GetLength() + 1;
	CHAR *nstringw = new char[newsizew];

	size_t convertedCharsw = 0;
	wcstombs_s(&convertedCharsw, nstringw, newsizew, strData.LockBuffer(), _TRUNCATE);

	return nstringw;
}

CString CUtils::GetFileName(IN const CString &strPathName)
{
	CString strFileName = strPathName.Right(strPathName.GetLength() - strPathName.ReverseFind(_T('\\')) - 1);

	return strFileName;
}


CString CUtils::ChangeExt(IN const CString &strFileName, IN const CString &strExtName)
{
	int nIndex = strFileName.ReverseFind(_T('.'));
	CString strName;
	if (nIndex != -1)
	{
		strName = strFileName.Left(nIndex);
	}
	else
	{
		strName = strFileName;
	}

	CString strChangeExt = strName + strExtName;

	return strChangeExt;
}

void CUtils::ChangeString(CString &strData, IN const CString &strStart, IN const CString &strEnd, IN const CString &strNew)
{
	int nStart;
	int nEnd;
	
	nStart = strData.Find(strStart, 0);
	nEnd = strData.Find(strEnd, nStart);
	strData.Delete(nStart, nEnd - nStart);
	strData.Insert(nStart, strNew);
}

void CUtils::Split(const CString& strString, const CString& strSeparation, vector<CString>& vecWord)
{
	vecWord.clear();

	CString strText = strString;

	long nStart = 0, nPos = 0;

	while (TRUE)
	{
		nPos = strText.Find(strSeparation, nStart);

		if (nPos == -1)
		{
			CString strWord = strText.Mid(nStart, strText.GetLength() - nStart);
			if (strWord != _T(""))
			{
				vecWord.push_back(strWord);
			}
			break;
		}

		vecWord.push_back(strText.Mid(nStart, nPos - nStart));

		nStart = nPos + 1;
	}
}

void CUtils::SliceString(const CString& strString, const CString& strSeparation, vector<CString>& vecWord)
{
	vecWord.clear();

	CString strText = strString;
	int iStart = 0, iFirstPos = 0, iEndPos = 0;

	int iLength = strSeparation.GetLength();
	if (iLength == 0)
	{
		iFirstPos = strText.Find(strSeparation, iStart);
		iEndPos = iFirstPos;
	}
	else
	{
		iFirstPos = strText.Find(strSeparation[iStart], iStart);
		iEndPos = strText.Find(strSeparation[iLength-1], iStart);
	}
	
	vecWord.push_back(strText.Mid(iStart, iFirstPos - iStart));
	
	strText.Delete(0, iEndPos + 1);
	
	if (strText != "")
	{		
		vecWord.push_back(strText);
	}

};

void CUtils::StringOutputIndex(CString strLayer, vector<CString> vecWord, INT32& iIndex, BOOL& bCheck)
{
	int nSize = static_cast<int>(vecWord.size());
	//bool bFind = false;
	CString strTemp = L"", strTemp2 = L"", strLayerIdx = L"";	
	int iFind = -1, iFind2 = -1, iLength = -1;

	//for (int i = 0; i < nSize; i++)
	//{
	if (nSize == 1)
	{
		//strTemp = vecWord[i];
		strTemp = vecWord[nSize-1];
		strTemp.MakeUpper();
		iFind = strLayer.Find(strTemp);

		// ORC-L01 과 같은경우 ORC- 까지 지워버리기 때문에 Error 처리
		if (iFind >= 2)
		{
			iFind = -1;
		}

		if (iFind != -1)
		{
			strLayer.Delete(0, iFind + 1);

			if (strLayer.GetLength() > 2)
				bCheck = FALSE;
			else
				bCheck = TRUE;
		}
		if (strTemp.GetLength() == 1 && 
			(iFind == 0 || iFind == -1))
		{
			bCheck = FALSE;
		}
	}
	else if (nSize > 1)
	{
		strTemp = vecWord[nSize-2];
		iFind = strLayer.Find(strTemp);
		
		if (iFind >= 2)
		{
			iFind = -1;
		}

		if (iFind != -1)
		{
			strLayer.Delete(0, iFind + 1);

			strTemp2 = vecWord[nSize-1];
			iFind2 = strLayer.Find(strTemp2);
			if (iFind2 == -1)				
				bCheck = FALSE;				
			else				
				bCheck = TRUE;				

			strLayer.Delete(iFind2, strLayer.GetLength());
				
			if (strLayer.GetLength() > 2)
				bCheck = FALSE;
			else if (strLayer.GetLength() == 2)
				bCheck = TRUE;
		}

		/*if (strTemp.GetLength() == 1 &&
			(iFind == 0 || iFind2 == -1))
		{
			bCheck = FALSE;
		}*/
			
		//}

		//if (bCheck) break;
	}

	if (bCheck)
	{
		iLength = strLayer.GetLength();
		for (int i = 0; i < iLength; i++)
		{
			if (isdigit(strLayer.GetAt(i)))
			{
				strLayerIdx.Insert(strLayerIdx.GetLength(), strLayer.GetAt(i));
			}			
		}	

		if (strLayerIdx == _T(""))
		{
			iIndex = -1;
			bCheck = FALSE;
		}
		else
		{
			iIndex = _ttoi(strLayerIdx);
		}
	}
	else
	{
		iIndex = -1;
	}
}

BOOL CUtils::CopyFolder(const CString& strFrom, const CString& strTo)
{//폴더의 하위파일을 복사하는 함수

	CFileFind finder;
	CString sWildCard = strFrom + "*.*";
	CString strOldModelName;
	CString strNewModelName;
	BOOL bFound;
	BOOL bWorking = finder.FindFile(sWildCard);

	if (bWorking == FALSE)
	{
		return FALSE;
	}

	bFound = bWorking;

	::SHCreateDirectoryEx(NULL, strTo, NULL);
	while (bWorking)
	{

		bWorking = finder.FindNextFile(); // 다음 파일 or 폴더 존재시 TRUE 반환

		if (finder.IsDots()) continue;

		CString sFrom2 = finder.GetFilePath();
		CString sTo2 = strTo + finder.GetFileName();
		//CString sTo2 = strTo + "\\" + finder.GetFileName();

		// 디렉토리이면 재귀 호출
		if (finder.IsDirectory())
		{
			sFrom2 += _T("\\");
			sTo2 += _T("\\");
			CopyFolder(sFrom2, sTo2);
			continue;
		}

#ifdef _UNICODE
		CopyFileW(sFrom2, sTo2, FALSE);
#else
		CopyFileA(sFrom2, sTo2, FALSE);
#endif
	}
	finder.Close();

	return bFound;
}

BOOL CUtils::IsNumber(IN CString str)
{
	int nCount = str.GetLength();

	int nCommaCount = 0;
	int i;
	for (i = 0; i < nCount; i++) {
		TCHAR temp = str.GetAt(i);
		// 음수 처리.
		if (i == 0 && temp == '-')
		{
			continue;
		}

		//소수점 Comma인지 Comm가 한개 인지 체크
		if (temp == '.')
		{
			if (nCommaCount <= 1)
			{
				continue;
			}
			else
			{
				break;
			}
			nCommaCount++;
		}

		// 입력된 키가 0 ~ 9 사이인가를 체크.
		if (temp >= '0' && temp <= '9')
		{
			continue;
		}
		else
		{
			break;
		}

	}
	if (i == nCount)
		return TRUE;

	return FALSE;
}


