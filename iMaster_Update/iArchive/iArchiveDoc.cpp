#include "pch.h"
#include "iArchiveDoc.h"
#include "../iArchiveIF/iArchiveIF.h"
#include "LoadLayerOperation.h"


UINT32 IArchiveDoc::GetVersionInfo(OUT Version& v)
{
	CUtils Utils;
	CString strVersion = Utils.GetVersionInfo(DLLNAME_IARCHIVE);
	
	CString str = L"";
	AfxExtractSubString(str, strVersion, 0, '.');	m_DLLVer.Major = _ttoi(str);
	AfxExtractSubString(str, strVersion, 1, '.');	m_DLLVer.Minor = _ttoi(str);
	AfxExtractSubString(str, strVersion, 2, '.');	m_DLLVer.Release = _ttoi(str);
	AfxExtractSubString(str, strVersion, 3, '.');	m_DLLVer.Build = _ttoi(str);
	
	v = m_DLLVer;

	return RESULT_GOOD;
}

UINT32 IArchiveDoc::Create()
{
	if (m_pArchive)
		m_pArchive->ClearAll();
	else
		m_pArchive = new CJobFile();

	CUtils Utils;
	CString sss = Utils.GetAppPath() + JOB_DATA_PATH;
	CreateDirectory(Utils.GetAppPath() + JOB_DATA_PATH, NULL);

	if (m_pLoadLayerOp == nullptr)
		m_pLoadLayerOp = new CLoadLayerOperation();

	if (m_pArchive)
		return RESULT_GOOD;
	else
		return RESULT_BAD;
}

UINT32 IArchiveDoc::Initiate()
{
	if (m_pLoadLayerOp)
		m_pLoadLayerOp->StopAllThreads();

	if (m_pArchive)
	{
		m_pArchive->ClearAll();
		return RESULT_GOOD;
	}
	else
	return RESULT_BAD;
}

UINT32 IArchiveDoc::Destroy()
{
	if (m_pLoadLayerOp)
	{
		delete (CLoadLayerOperation*)m_pLoadLayerOp;
		m_pLoadLayerOp = nullptr;
	}

	if (m_pArchive)
	{
		m_pArchive->ClearAll();
		delete m_pArchive;
		m_pArchive = nullptr;
	}

	return RESULT_GOOD;
}

UINT32 IArchiveDoc::Extract(IN BOOL bAlwaysCreate, IN LPCTSTR fileName)
{
	if (m_pArchive)
		return ExtractODB(bAlwaysCreate, fileName);

	return RESULT_BAD;
}

// 2022.06.21
// 김준호 주석처리
// ODB 압축 풀때 SaveDate 비교 위해 내용 변경 
//UINT32 IArchiveDoc::ExtractODB(IN BOOL bAlwaysCreate, IN LPCTSTR fileName)
//{
//	if (m_pArchive)
//		m_pArchive->ClearAll();
//
//	CUtils Utils;
//	CString strExt = Utils.GetFileInfo(3, fileName);
//	if (strExt.CompareNoCase(L".tgz"))
//		return ERROR_FILE_FORMAT;
//
//	// Get Current Path
//	CString strAppPath = Utils.GetAppPath();
//	//strAppPath.TrimRight(L"\\");
//	CString strTgzPath = strAppPath + TGZ_TOOL_PATH;
//	CString strJobPath = strAppPath + JOB_DATA_PATH;
//
//	// Set Model Name
//	CFileFind finder;
//	CString strDstPath = L"";
//	CString strFName = Utils.GetFileInfo(2, fileName);
//	int nPos = strFName.ReverseFind(L';');
//	if (nPos != -1) strFName.Delete(nPos, strFName.GetLength() - nPos);
//	nPos = strFName.ReverseFind(L' ');
//	if (nPos != -1) strFName.Delete(0, nPos + 1);
//
//	strFName.Replace(L"-job", L"");
//	strFName.Replace(L"_aoi", L"");
//	strFName.Replace(L"_outsourcing", L"");
//
//
//	//m_pArchive->m_Misc.SetModelName(strFName);
//	//hj.kim
//	//Job Name을 Text로 출력하는 경우가 있어서 대문자로 변경
//	m_pArchive->m_Misc.SetModelName(strFName.MakeUpper());
//
//	m_pArchive->m_Misc.m_strModel = strFName;
//	strDstPath.Format(L"%s%s", strJobPath, m_pArchive->m_Misc.GetModelName());
//
//	m_pArchive->m_strFullPathName = strDstPath;
//	
//	if (finder.FindFile(strDstPath))
//	{
//		if (bAlwaysCreate)
//			Utils.RemoveDirectoryFile(strDstPath);
//		else
//			return RESULT_GOOD;
//	}
//
//	//Copy to Job Folder
//	CString strChangePath = L"";
//	strChangePath.Format(L"%s%s.tgz", strJobPath, m_pArchive->m_Misc.GetModelName());
//	if (!CopyFile(fileName, strChangePath.GetBuffer(0), FALSE))
//		return ERROR_COPY_FILE;
//
//	// 다른 프로그램 실행 하기 위한 변수
//	STARTUPINFO si;
//	memset(&si, 0x00, sizeof(STARTUPINFO));
//	si.cb = sizeof(STARTUPINFO);
//	si.dwFlags = STARTF_USESHOWWINDOW; //2006.04.28
//	si.wShowWindow = SW_HIDE;
//
//	PROCESS_INFORMATION pi;
//	CString strCmdLine = L"";
//
//	CString strTempPath = strJobPath + L"Temp\\";
//	Utils.RemoveDirectoryFile(strTempPath);
//	CreateDirectory(strTempPath, NULL);
//
//	// 1. tgz -> tar 압축풀기
////	strCmdLine.Format(L"%sgunzip.exe -d -S .tgz %s", strTgzPath, strDstPath);
//	strCmdLine.Format(L"%s7z.exe e %s -o%s", strTgzPath, strChangePath, strTempPath);// strJobPath);
//	if (!CreateProcess(NULL, strCmdLine.GetBuffer(0), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
//		return ERROR_CREATE_PROCESS;
//
//	WaitForSingleObject(pi.hProcess, INFINITE);
//	CloseHandle(pi.hThread);
//	DeleteFile(strChangePath);
//
//	// tar파일명 확인 
//	CString strTarFileName = L"";
//	BOOL bExist = finder.FindFile(strTempPath + L"*.*");
//	while (bExist)
//	{
//		bExist = finder.FindNextFile();
//		if (finder.IsDots() || finder.IsDirectory())
//			continue;
//
//		strTarFileName = finder.GetFileName();
//		break;
//	}
//
//	if (strTarFileName.GetLength() > 0)
//	{
//		CString strName = Utils.GetFileInfo(2, strTarFileName);
//		if (strName.CompareNoCase(m_pArchive->m_Misc.m_strModel) != 0)
//		{
//			CString strOld = strTarFileName;
//			strTarFileName.Replace(strName, m_pArchive->m_Misc.m_strModel);
//
//			CFile::Rename(strTempPath + strOld, strTempPath + strTarFileName);			
//		}
//	
//		CString strOut = strJobPath + strTarFileName;
//		if (!CopyFile(strTempPath + strTarFileName, strOut.GetBuffer(0), FALSE))
//			return ERROR_COPY_FILE;
//		DeleteFile(strTempPath + strTarFileName);
//	}	
//
//	// 2. tar 압축 풀기	
//	memset(&si, 0x00, sizeof(STARTUPINFO));
//	si.cb = sizeof(STARTUPINFO);
//	si.dwFlags = STARTF_USESHOWWINDOW; //2006.04.28
//	si.wShowWindow = SW_HIDE;
//
//	
//	strChangePath.Replace(L"tgz", L"tar");
//	
//	CreateDirectory(strDstPath, NULL);
//	strCmdLine.Format(L"%s7z.exe x %s -o%s -aoa", strTgzPath, strChangePath, strDstPath);
//	if (!CreateProcess(NULL, strCmdLine.GetBuffer(0), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
//		return ERROR_CREATE_PROCESS;
//
//	WaitForSingleObject(pi.hProcess, INFINITE);
//	CloseHandle(pi.hThread);
//	DeleteFile(strChangePath);
//
//	strChangePath.Format(L"%s\\*.*", strDstPath);
//
//	CString strChangeFolder = L"";
//	bExist = finder.FindFile(strChangePath);
//	while (bExist)
//	{
//		bExist = finder.FindNextFile();
//		if (finder.IsDots()) continue;
//		else if (finder.IsDirectory())
//		{
//			strChangeFolder = finder.GetFilePath();
//			break;
//		}
//	}
//
//	strChangePath.Replace(L"*.*", L"");
//	if (strChangeFolder.GetLength() > 0)
//	{
//		strChangeFolder.Format(L"%s\\", strChangeFolder);
//		Utils.MoveDirectoryFile(TRUE, strChangeFolder, strChangePath);
//	}
//
//	// 3. \\steps\\...\\layers\\xxx.Z 압축 풀기
//	memset(&si, 0x00, sizeof(STARTUPINFO));
//	si.cb = sizeof(STARTUPINFO);
//	si.dwFlags = STARTF_USESHOWWINDOW; //2006.04.28
//	si.wShowWindow = SW_HIDE;
//
//	strDstPath.Format(L"%s%s\\steps\\*.*", strJobPath, m_pArchive->m_Misc.GetModelName());	
//
//	bExist = finder.FindFile(strDstPath);
//	strDstPath.Replace(L"*.*", L"");
//	if (!bExist)
//	{
//		CString strMsg;
//		strMsg.Format(L"The Selected File is not ODB...");
//		AfxMessageBox(strMsg, MB_OK | MB_ICONEXCLAMATION);
//		return FALSE;
//	}
//
//	CFileFind finderSub, finderSub2;
//	CString strLayerPath = L"", strFeaturePath = L"";
//	while (bExist)
//	{
//		bExist = finder.FindNextFile();
//		if (finder.IsDots()) continue;
//		else if (finder.IsDirectory())
//		{
//			strLayerPath.Format(L"%s%s\\layers\\*.*", strDstPath, finder.GetFileName());
//			BOOL bExistSub = finderSub.FindFile(strLayerPath);
//
//			while (bExistSub)
//			{
//				bExistSub = finderSub.FindNextFile();
//				if (finderSub.IsDots()) continue;
//				else if (finderSub.IsDirectory())
//				{
//					strFeaturePath.Format(L"%s\\features.Z", finderSub.GetFilePath());
//					if (finderSub2.FindFile(strFeaturePath))
//					{
//						//	strCmdLine.Format(L"%sgunzip.exe -d -S .Z %s", strTgzPath, strFeaturePath);
//						strCmdLine.Format(L"%s7z.exe e %s -o%s -r -aoa", strTgzPath, strFeaturePath, finderSub.GetFilePath());
//						if (!CreateProcess(NULL, strCmdLine.GetBuffer(0), NULL, NULL, FALSE, 0, NULL, NULL/*strJobPath*/, &si, &pi))
//							return FALSE;
//						
//						WaitForSingleObject(pi.hProcess, INFINITE);
//						CloseHandle(pi.hThread);
//					}
//				}
//			}
//		}
//	}
//	CloseHandle(pi.hProcess);
//
//
//	return RESULT_GOOD;
//}

UINT32 IArchiveDoc::ExtractODB(IN BOOL bAlwaysCreate, IN LPCTSTR fileName)
{
	if (m_pArchive)
		m_pArchive->ClearAll();

	CUtils Utils;
	CString strExt = Utils.GetFileInfo(3, fileName);
	if (strExt.CompareNoCase(L".tgz"))
		return ERROR_FILE_FORMAT;

	// Get Current Path
	CString strAppPath = Utils.GetAppPath();	
	CString strTgzPath = strAppPath + TGZ_TOOL_PATH;
	CString strJobPath = strAppPath + JOB_DATA_PATH;

	// Set Model Name	
	CString strDstPath = L"";
	CString strFName = Utils.GetFileInfo(2, fileName);
	int nPos = strFName.ReverseFind(L';');
	if (nPos != -1) strFName.Delete(nPos, strFName.GetLength() - nPos);
	nPos = strFName.ReverseFind(L' ');
	if (nPos != -1) strFName.Delete(0, nPos + 1);

	strFName.Replace(L"-job", L"");
	strFName.Replace(L"_aoi", L"");
	strFName.Replace(L"_outsourcing", L"");

	//hj.kim
	//Job Name을 Text로 출력하는 경우가 있어서 대문자로 변경
	m_pArchive->m_Misc.SetModelName(strFName.MakeUpper());

	m_pArchive->m_Misc.m_strModel = strFName;
	strDstPath.Format(L"%s%s", strJobPath, m_pArchive->m_Misc.GetModelName());

	m_pArchive->m_strFullPathName = strDstPath;

	// ================================================================= //
	// 2022.06.22
	// 김준호 주석추가
	// ODB 압축풀어서 비교하기 위한 구문 추가	
	
	CFileFind filefinder;
	CString strTempPath = strJobPath + L"Temp\\";
	if (filefinder.FindFile(strJobPath + L"Temp") == TRUE)
	{
		Utils.RemoveDirectoryFile(strTempPath);
	}

	BOOL bRet = FALSE;
	BOOL bMiscCheck = FALSE;
	BOOL bFirst = FALSE;
	if (filefinder.FindFile(strDstPath) == TRUE)
	{		
		bMiscCheck = TRUE;		
		bRet = ExtractTgz(strTgzPath, strTempPath, fileName, bFirst);		
		if (bRet != RESULT_GOOD)
			return RESULT_BAD;		
	}
	else
	{
		//bRet = ExtractTgz(strTgzPath, strJobPath, fileName, bFirst);
		bFirst = TRUE;
		bRet = ExtractTgz(strTgzPath, strTempPath, fileName, bFirst);
		if (bRet != RESULT_GOOD)
			return RESULT_BAD;
		bFirst = FALSE;
	}

	if (bMiscCheck)
	{
		CString strDstPathInfo = m_pArchive->m_strFullPathName + INFO_PATH;
		CString strTempPathInfo = strDstPathInfo;
		strTempPathInfo.Replace(L"Job", L"Job\\Temp");


		if (!filefinder.FindFile(strDstPathInfo))
			return RESULT_BAD;
		if (!filefinder.FindFile(strTempPathInfo))
			return RESULT_BAD;

		double dDstSaveDate = 0, dTempSaveDate = 0;
		bRet = ReadMiscInfoFile(strDstPathInfo, dDstSaveDate);
		if (bRet != RESULT_GOOD)
			return RESULT_BAD;
		bRet = ReadMiscInfoFile(strTempPathInfo, dTempSaveDate);
		if (bRet != RESULT_GOOD)
			return RESULT_BAD;

		if (dDstSaveDate < dTempSaveDate)
		{
			// Dst Folder Remove after Temp Folder Move
			Utils.RemoveDirectoryFile(strDstPath);		
			Utils.MoveDirectoryFile(TRUE, strTempPath, strJobPath);
		}
		else
		{
			Utils.RemoveDirectoryFile(strTempPath);
		}
	}	

	// ================================================================= //

	return RESULT_GOOD;
}

BOOL IArchiveDoc::ExtractTgz(IN LPCTSTR strTgzPath, IN CString strFolderPath, IN LPCTSTR strFileName, IN BOOL bFirst)
{
	CUtils Utils;
	CFileFind finder;

	//Utils.RemoveDirectoryFile(strFolderPath);
	Utils.CheckCreateFolder(strFolderPath);

	CString strCheckFolder = L"";
	strCheckFolder.Format(L"%s%s", strFolderPath, m_pArchive->GetModelName());

	if (finder.FindFile(strCheckFolder))
	{
		/*if (bAlwaysCreate)
			Utils.RemoveDirectoryFile(strDstPath);
		else
			return RESULT_GOOD;*/

		return RESULT_GOOD;
	}

	//Copy to Temp Folder
	CString strChangePath = L"";
	strChangePath.Format(L"%s%s.tgz", strFolderPath, m_pArchive->m_Misc.GetModelName());
	if (!CopyFile(strFileName, strChangePath.GetBuffer(0), FALSE))
		return ERROR_COPY_FILE;

	// 다른 프로그램 실행 하기 위한 변수
	STARTUPINFO si;
	memset(&si, 0x00, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW; //2006.04.28
	si.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION pi;
	CString strCmdLine = L"";

	/*CString strTempPath = L"";
	strTempPath.Format(L"%s%s", strFolderPath, L"Temp\\");
	Utils.RemoveDirectoryFile(strTempPath);
	CreateDirectory(strTempPath, NULL);*/

	// 1. tgz -> tar 압축풀기
//	strCmdLine.Format(L"%sgunzip.exe -d -S .tgz %s", strTgzPath, strDstPath);
	//strCmdLine.Format(L"%s7z.exe e %s -o%s", strTgzPath, strChangePath, strTempPath);// strJobPath);

	strCmdLine.Format(L"%s7z.exe e %s -o%s", strTgzPath, strChangePath, strFolderPath);
	if (!CreateProcess(NULL, strCmdLine.GetBuffer(0), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		return ERROR_CREATE_PROCESS;

	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hThread);
	DeleteFile(strChangePath);

	// tar파일명 확인 
	CString strTarFileName = L"";
	BOOL bExist = finder.FindFile(strFolderPath + L"*.*");
	while (bExist)
	{
		bExist = finder.FindNextFile();
		if (finder.IsDots() || finder.IsDirectory())
			continue;

		strTarFileName = finder.GetFileName();
		break;
	}

	if (strTarFileName.GetLength() > 0)
	{
		CString strName = Utils.GetFileInfo(2, strTarFileName);
		if (strName.CompareNoCase(m_pArchive->m_Misc.m_strModel) != 0)
		{
			CString strOld = strTarFileName;
			strTarFileName.Replace(strName, m_pArchive->m_Misc.m_strModel);			
		}		
	}

	// 2. tar 압축 풀기	
	memset(&si, 0x00, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW; //2006.04.28
	si.wShowWindow = SW_HIDE;	

	CString strDstPath = strFolderPath;

	strChangePath.Replace(L"tgz", L"tar");

	CreateDirectory(strDstPath, NULL);
	strCmdLine.Format(L"%s7z.exe x %s -o%s -aoa", strTgzPath, strChangePath, strDstPath);
	if (!CreateProcess(NULL, strCmdLine.GetBuffer(0), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		return ERROR_CREATE_PROCESS;

	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hThread);
	DeleteFile(strChangePath);

	strChangePath.Format(L"%s*.*", strDstPath);

	CString strChangeFolder = L"";
	
	bExist = finder.FindFile(strChangePath);
	while (bExist)
	{
		bExist = finder.FindNextFile();
		if (finder.IsDots()) continue;
		else if (finder.IsDirectory())
		{
			strChangeFolder = finder.GetFilePath();
			break;			
			/*if (finder.FindFile(strCheckFolder) == TRUE)
			{
				strChangeFolder = strCheckFolder;
				break;
			}*/
		}
	}
	if (finder.FindFile(strChangeFolder) == FALSE)
		return RESULT_BAD;

	CString strFolderName = Utils.GetFileInfo(2, strChangeFolder);
	CString strNewFolderName = m_pArchive->GetModelName();
	CString strNewPath = strChangeFolder;
	if (m_pArchive->GetModelName().CompareNoCase(strFolderName) != 0)
	{	
		strNewPath.Replace(strFolderName, strNewFolderName);
		CFile::Rename(strChangeFolder, strNewPath);
	}
	else
	{
		// Temp 밑에 압축푼뒤 폴더명 찾아서 압축파일명 폴더로 변경
		CFile::Rename(strChangeFolder, strChangeFolder.MakeUpper());
	}

	

	//strChangePath.Replace(L"*.*", L"");
	//if (strChangeFolder.GetLength() > 0)
	//{
	//	strChangeFolder.Format(L"%s\\", strChangeFolder);
	//	Utils.MoveDirectoryFile(TRUE, strChangeFolder, strCheckFolder);
	//	//Utils.MoveDirectoryFile(TRUE, strChangeFolder, strChangePath);
	//}

	// 3. \\steps\\...\\layers\\xxx.Z 압축 풀기
	memset(&si, 0x00, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW; //2006.04.28
	si.wShowWindow = SW_HIDE;

	strDstPath.Format(L"%s\\steps\\*.*", strCheckFolder);
	

	bExist = finder.FindFile(strDstPath);
	strDstPath.Replace(L"*.*", L"");
	if (!bExist)
	{
		CString strMsg;
		strMsg.Format(L"The Selected File is not ODB...");
		AfxMessageBox(strMsg, MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	CFileFind finderSub, finderSub2;
	CString strLayerPath = L"", strFeaturePath = L"";
	while (bExist)
	{
		bExist = finder.FindNextFile();
		if (finder.IsDots()) continue;
		else if (finder.IsDirectory())
		{
			strLayerPath.Format(L"%s%s\\layers\\*.*", strDstPath, finder.GetFileName());
			BOOL bExistSub = finderSub.FindFile(strLayerPath);

			while (bExistSub)
			{
				bExistSub = finderSub.FindNextFile();
				if (finderSub.IsDots()) continue;
				else if (finderSub.IsDirectory())
				{
					strFeaturePath.Format(L"%s\\features.Z", finderSub.GetFilePath());
					if (finderSub2.FindFile(strFeaturePath))
					{
						//	strCmdLine.Format(L"%sgunzip.exe -d -S .Z %s", strTgzPath, strFeaturePath);
						strCmdLine.Format(L"%s7z.exe e %s -o%s -r -aoa", strTgzPath, strFeaturePath, finderSub.GetFilePath());
						if (!CreateProcess(NULL, strCmdLine.GetBuffer(0), NULL, NULL, FALSE, 0, NULL, NULL/*strJobPath*/, &si, &pi))
							return FALSE;

						WaitForSingleObject(pi.hProcess, INFINITE);
						CloseHandle(pi.hThread);
					}
				}
			}
		}
	}
	CloseHandle(pi.hProcess);	


	if (bFirst == TRUE)
	{
		CString strJobPath = strFolderPath;
		strJobPath.Replace(L"Temp\\", L"");
				
		Utils.MoveDirectoryFile(TRUE, strFolderPath, strJobPath);
		Utils.RemoveDirectoryFile(strFolderPath);
	}


	return RESULT_GOOD;
}

BOOL IArchiveDoc::ReadMiscInfoFile(IN CString fileName, OUT double &dSaveDate)
{
	CStdioFile cStdFile;

	if (!cStdFile.Open(fileName.GetBuffer(0), CFile::modeRead | CFile::typeText))
		return RESULT_BAD;

	CString strTmp = L"", strFindTmp = L"SAVE_DATE", strToken = L"";
	while (cStdFile.ReadString(strTmp))
	{
		strTmp.TrimLeft(_T("\t "));
		strTmp.TrimRight(_T("\t "));
		strTmp.MakeUpper();
		if (strTmp.GetLength() == 0) continue;	// 빈줄 Skip

		if (strTmp.Find(strFindTmp) >= 0)
		{
			AfxExtractSubString(strToken, strTmp, 1, '=');
			//strSaveDate = strToken;						
			dSaveDate = _tstof(strToken);			
			continue;
		}
	}
	return RESULT_GOOD;
}

UINT32 IArchiveDoc::CompressODB(IN LPCTSTR fileName)
{
	CUtils Utils;

	// Get Current Path
	CString strAppPath = Utils.GetAppPath();
	//strAppPath.TrimRight(L"\\");
	CString strTgzPath = strAppPath + TGZ_TOOL_PATH;
	CString strJobPath = strAppPath + JOB_DATA_PATH;

	// 다른 프로그램 실행 하기 위한 변수
	STARTUPINFO si;
	memset(&si, 0x00, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW; //2006.04.28
	si.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION pi;
	CString strCmdLine = L"";

	CString strModelFullPathName = strJobPath + m_pArchive->m_Misc.GetModelName() + _T("\\");
	CString strFullPathName_Tar = strJobPath + _T("Temp\\") + m_pArchive->m_Misc.GetModelName() + _T(".tar");
	CString strFullPathName_Tgz = strJobPath + _T("Temp\\") + m_pArchive->m_Misc.GetModelName() + _T(".tgz");

	strCmdLine.Format(L"%s7z.exe a %s %s", strTgzPath, strFullPathName_Tar, strModelFullPathName);
	if (!CreateProcess(NULL, strCmdLine.GetBuffer(0), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		return ERROR_CREATE_PROCESS;

	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hThread);

	strCmdLine.Format(L"%s7z.exe a %s %s", strTgzPath, strFullPathName_Tgz, strFullPathName_Tar);
	if (!CreateProcess(NULL, strCmdLine.GetBuffer(0), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
		return ERROR_CREATE_PROCESS;

	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hThread);

	CString strOdbPath = m_pSystemSpec->sysPath.strCamPath;
	//끝자리가 "\\" 또는 "/" 아닐경우
	if (strOdbPath.Mid(strOdbPath.GetLength() - 1, 1) != _T("\\") &&
		strOdbPath.Mid(strOdbPath.GetLength() - 1, 1) != _T("/"))
	{
		strOdbPath += _T("\\");
	}

	//Copy
	CString strDstPath = strOdbPath + m_pArchive->m_Misc.GetModelName() + _T(".tgz");
	if (!CopyFile(strFullPathName_Tgz, strDstPath, FALSE))
	{
		return RESULT_BAD;
	}

	// Temp 아래 Tar, Tgz 파일 삭제
	DeleteFile(strFullPathName_Tar);
	DeleteFile(strFullPathName_Tgz);


	return RESULT_GOOD;
}

UINT32 IArchiveDoc::LoadMisc()
{
	if (m_pArchive)
	{
		m_pArchive->m_bMatrixNStepLoaded = false;
		return m_pArchive->LoadMisc();
	}

	return RESULT_BAD;
}

UINT32 IArchiveDoc::LoadMatrix()
{
	if (m_pArchive)
	{
		if (m_pSystemSpec)
		{
			
			if (m_pSystemSpec->sysBasic.McType == eMachineType::eNSIS &&
				m_pSystemSpec->sysBasic.UserSite == eUserSite::eSEM)
				return m_pArchive->LoadMatrix_Sem(m_pSystemSpec);
			else
			{
				return m_pArchive->LoadMatrix();
			}
		}
		else
		{
			return m_pArchive->LoadMatrix();
		}

	}

	return RESULT_BAD;
}

UINT32 IArchiveDoc::LoadStep()
{
	if (m_pArchive)	
	{
		if (RESULT_GOOD == m_pArchive->LoadStep())
		{
			m_pArchive->m_bMatrixNStepLoaded = true;
			return RESULT_GOOD;
		}
	}

	return RESULT_BAD;
}

UINT32 IArchiveDoc::LoadFont()
{
	if (m_pArchive)
		return m_pArchive->LoadFont();

	return RESULT_BAD;
}

UINT32 IArchiveDoc::LoadUserSymbols()
{
	if (m_pArchive)
	{
		m_pArchive->LoadPadUsage();
		return m_pArchive->LoadUserSymbols();
	}

	return RESULT_BAD;
}

UINT32 IArchiveDoc::GetStepCount(OUT UINT32& iCount)
{
	if (m_pArchive)
	{
		iCount = static_cast<UINT32>(m_pArchive->m_Matrix.m_arrMatrixStep.GetSize());
		return RESULT_GOOD;
	}
	else
	{
		iCount = 0;
		return RESULT_BAD;
	}
}

UINT32 IArchiveDoc::GetStepRepeatCount(OUT std::vector<UINT32>& vcCount)
{
	if (m_pArchive)
	{
		UINT32 iStepCnt = static_cast<UINT32>(m_pArchive->m_Matrix.m_arrMatrixStep.GetSize());
		if (vcCount.size() < iStepCnt)
			return RESULT_BAD;

		for (UINT32 iStep = 0; iStep < iStepCnt; iStep++)
		{
			vcCount[iStep] = static_cast<UINT32>(m_pArchive->m_arrStep[iStep]->m_Stephdr.m_arrStepRepeat.GetSize());
		}
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 IArchiveDoc::GetStepInfo(OUT std::vector<stStepInfo*>* pStepInfo)
{
	if (m_pArchive)
	{
		if (pStepInfo == nullptr)
			return RESULT_BAD;

		UINT32 iStepCnt = static_cast<UINT32>(m_pArchive->m_Matrix.m_arrMatrixStep.GetSize());
		if (pStepInfo->size() < iStepCnt || m_pArchive->m_arrStep.GetSize() < iStepCnt)
			return RESULT_BAD;
		
		for (UINT32 iStep = 0; iStep < iStepCnt; iStep++)
		{
			stStepInfo* pInfo = pStepInfo->at(iStep);
			if (pInfo == nullptr)
				return RESULT_BAD;

			CStep* pStep = m_pArchive->m_arrStep[iStep];

			UINT32 iSubStep = static_cast<UINT32>(pStep->m_Stephdr.m_arrStepRepeat.GetSize());
			if (pInfo->vcStepRepeat.size() < iSubStep)
				return RESULT_BAD;

			pInfo->strName = pStep->m_strStepName;

			for (UINT32 iSub = 0; iSub < iSubStep; iSub++)
			{
				pInfo->vcStepRepeat[iSub]->bMirror = pStep->m_Stephdr.m_arrStepRepeat[iSub]->m_bMirror;
				pInfo->vcStepRepeat[iSub]->dAngle = pStep->m_Stephdr.m_arrStepRepeat[iSub]->m_dAngle;
				pInfo->vcStepRepeat[iSub]->iNx = pStep->m_Stephdr.m_arrStepRepeat[iSub]->m_nNX;
				pInfo->vcStepRepeat[iSub]->iNy = pStep->m_Stephdr.m_arrStepRepeat[iSub]->m_nNY;
				pInfo->vcStepRepeat[iSub]->strName = pStep->m_Stephdr.m_arrStepRepeat[iSub]->m_strName;
				pInfo->vcStepRepeat[iSub]->mirDir = pStep->m_Stephdr.m_arrStepRepeat[iSub]->m_mirDir;
			}
		}		
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 IArchiveDoc::GetModelName(OUT CString& strModel)
{
	if (m_pArchive)
	{
		strModel = m_pArchive->GetModelName();
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 IArchiveDoc::GetLayerInfo(OUT std::vector<stLayerInfo*>* pLayerInfo)
{
	if (m_pArchive)
	{
		if (pLayerInfo == nullptr)
			return RESULT_BAD;

		UINT32 iLayerCnt = static_cast<UINT32>(m_pArchive->m_Matrix.m_arrMatrixLayer.GetSize());
		if (pLayerInfo->size() < iLayerCnt)
			return RESULT_BAD;

		for (UINT32 iLayer = 0; iLayer < iLayerCnt; iLayer++)
		{
			stLayerInfo* pInfo = pLayerInfo->at(iLayer);
			if (pInfo == nullptr)
				return RESULT_BAD;

			pInfo->strName = m_pArchive->m_Matrix.m_arrMatrixLayer[iLayer]->m_strName;
			pInfo->strContext = m_pArchive->m_Matrix.m_arrMatrixLayer[iLayer]->m_strContext;
			pInfo->strType = m_pArchive->m_Matrix.m_arrMatrixLayer[iLayer]->m_strType;
			pInfo->bIsPositive = m_pArchive->m_Matrix.m_arrMatrixLayer[iLayer]->m_enPolarity == Polarity::PPositive ? true : false;
		}

		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 IArchiveDoc::GetLayerCount(OUT UINT32& iCount)
{
	if (m_pArchive)
	{
		iCount = static_cast<UINT32>(m_pArchive->m_Matrix.m_arrMatrixLayer.GetSize());
		return RESULT_GOOD;
	}
	else
	{
		iCount = 0;
		return RESULT_BAD;
	}
}

UINT32 IArchiveDoc::GetComponentCount(OUT std::vector<UINT32>& vcCount)
{
	if (m_pArchive)
	{
		UINT32 iStepCnt = static_cast<UINT32>(m_pArchive->m_Matrix.m_arrMatrixStep.GetSize());
		if (vcCount.size() < iStepCnt)
			return RESULT_BAD;

		for (UINT32 iStep = 0; iStep < iStepCnt; iStep++)
		{
			vcCount[iStep] = static_cast<UINT32>(m_pArchive->m_arrStep[iStep]->m_arrPackage.GetSize());
		}
		return RESULT_GOOD;
	}
	else	
		return RESULT_BAD;	
}

UINT32 IArchiveDoc::GetComponentInfo(OUT std::vector<stComponentInfo*>* pCompInfo)
{
	if (m_pArchive)
	{
		if (pCompInfo == nullptr)
			return RESULT_BAD;

		UINT32 iStepCnt = static_cast<UINT32>(m_pArchive->m_Matrix.m_arrMatrixStep.GetSize());
		if (pCompInfo->size() < iStepCnt || m_pArchive->m_arrStep.GetSize() < iStepCnt)
			return RESULT_BAD;

		for (UINT32 iStep = 0; iStep < iStepCnt; iStep++)
		{
			stComponentInfo* pInfo = pCompInfo->at(iStep);
			if (pInfo == nullptr)
				return RESULT_BAD;

			CStep* pStep = m_pArchive->m_arrStep[iStep];

			UINT32 iSubPkg = static_cast<UINT32>(pStep->m_arrPackage.GetSize());
			if (pInfo->vcPackages.size() < iSubPkg)
				return RESULT_BAD;

			for (UINT32 iSub = 0; iSub < iSubPkg; iSub++)
			{
				CPackage* pPkgTmp = pStep->m_arrPackage.GetAt(iSub);
				if (pPkgTmp == nullptr) 
					continue;

				pInfo->vcPackages[iSub]->strName = pPkgTmp->m_strPkgName;

				CRecord* pRecTmp = pPkgTmp->m_pRecord;
				if (pRecTmp)
				{
					switch (pRecTmp->m_eType)
					{
						case RecordType::RC: pInfo->vcPackages[iSub]->strType = _T("RECT"); break;
						case RecordType::CR: pInfo->vcPackages[iSub]->strType = _T("CIRCLE"); break;
						case RecordType::SQ: pInfo->vcPackages[iSub]->strType = _T("SQUARE"); break;
						case RecordType::CT: pInfo->vcPackages[iSub]->strType = _T("CONTOUR"); break;
						default: pInfo->vcPackages[iSub]->strType = _T("UNKNOWN"); break;
					}
				}
				pInfo->vcPackages[iSub]->iPinCnt = static_cast<UINT32>(pPkgTmp->m_arrPin.GetSize());
			}
		}
		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 IArchiveDoc::LoadLayer(IN UserLayerSet_t* pstUserLayerSet, IN BOOL bCheckOnly)
{
	if (pstUserLayerSet == nullptr || m_pLoadLayerOp == nullptr)
		return RESULT_BAD;

	UINT32 iStepCnt = static_cast<UINT32>(m_pArchive->m_Matrix.m_arrMatrixStep.GetSize());
	if (iStepCnt <= 0)
		return RESULT_BAD;

	CUtils Utils;
	UINT32 iCoreNum = max(1, Utils.GetCoreCount() - 2);

	if (m_pArchive)
	{
		m_pArchive->m_bLoaded = false;
		m_pArchive->ClearLayers();

		m_pLoadLayerOp->SetJobFile(m_pArchive);
		
		UINT32 iLayerNum = static_cast<UINT32>(pstUserLayerSet->vcLayerSet.size());
		if (bCheckOnly)
		{
			iLayerNum = 0;
			for (auto it : pstUserLayerSet->vcLayerSet)
			{
				if (!it->bCheck)
					continue;

				iLayerNum++;
			}
		}
		if (pstUserLayerSet->bIsPreview)
			iLayerNum = pstUserLayerSet->iPreviewNum;
		
		m_pLoadLayerOp->StartThreads(iStepCnt, iLayerNum, iCoreNum);

		if (bCheckOnly)
		{
			for (auto it : pstUserLayerSet->vcLayerSet)
			{
				if (!it->bCheck)
					continue;
				
				for (UINT32 iStep = 0; iStep < iStepCnt; iStep++)
				{
					CStep* pStep = m_pArchive->m_arrStep[iStep];

					CLoadLayerJob stJob;
					stJob.SetData(iStep, pStep->m_strStepName, it);
					m_pLoadLayerOp->AddJob(stJob);
				}
			}
		}
		else
		{
			for (auto it : pstUserLayerSet->vcLayerSet)
			{
				for (UINT32 iStep = 0; iStep < iStepCnt; iStep++)
				{
					CStep* pStep = m_pArchive->m_arrStep[iStep];

					CLoadLayerJob stJob;
					stJob.SetData(iStep, pStep->m_strStepName, it);
					m_pLoadLayerOp->AddJob(stJob);
				}
			}
		}

		if (m_pLoadLayerOp->RunAllJobs())
		{
			m_pLoadLayerOp->WaitForJobFinish();

			m_pArchive->SetFeatureMinMaxOfStep();
			m_pArchive->SetFeatureMinMaxOfStepNRepeat();

			m_pLoadLayerOp->ResetAllJobEvents();
		}

		if (!m_pLoadLayerOp->StopAllThreads())
			return RESULT_BAD;

		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 IArchiveDoc::LoadLayer_All()
{
	if (m_pLoadLayerOp == nullptr)
		return RESULT_BAD;

	UINT32 iStepCnt = static_cast<UINT32>(m_pArchive->m_Matrix.m_arrMatrixStep.GetSize());
	if (iStepCnt <= 0)
		return RESULT_BAD;

	CUtils Utils;
	UINT32 iCoreNum = max(1, Utils.GetCoreCount() - 2);

	if (m_pArchive)
	{
		m_pArchive->m_bLoaded = false;
		m_pArchive->ClearLayers();

		m_pLoadLayerOp->SetJobFile(m_pArchive);

		//"Unit"으로 되어 있는 Step에 대한 Layer만 가져오기 위함
		int nUnitIndex = -1;
		int iStepNum = static_cast<int>(m_pArchive->m_arrStep.GetSize());
		for (int i = 0; i < iStepNum; i++)
		{
			if (m_pArchive->m_arrStep[i]->m_strStepName == _T("Unit"))
			{
				nUnitIndex = i;
				break;
			}
		}
		if (nUnitIndex < 0) return RESULT_BAD;
		if (nUnitIndex >= iStepNum) return RESULT_BAD;

		UINT32 iLayerNum = static_cast<UINT32>(m_pArchive->m_arrStep[nUnitIndex]->m_arrLayer.GetSize());
		
		m_pLoadLayerOp->StartThreads(iStepCnt, iLayerNum, iCoreNum);

		/*
		for (auto it : pstUserLayerSet->vcLayerSet)
		{
			for (UINT32 iStep = 0; iStep < iStepCnt; iStep++)
			{
				CStep* pStep = m_pArchive->m_arrStep[iStep];

				CLoadLayerJob stJob;
				stJob.SetData(iStep, pStep->m_strStepName, it);
				m_pLoadLayerOp->AddJob(stJob);
			}
		}
		*/
		

		if (m_pLoadLayerOp->RunAllJobs())
		{
			m_pLoadLayerOp->WaitForJobFinish();

			m_pArchive->SetFeatureMinMaxOfStep();
			m_pArchive->SetFeatureMinMaxOfStepNRepeat();

			m_pLoadLayerOp->ResetAllJobEvents();
		}

		if (!m_pLoadLayerOp->StopAllThreads())
			return RESULT_BAD;

		return RESULT_GOOD;
	}
	else
		return RESULT_BAD;
}

UINT32 IArchiveDoc::GetJobFile(OUT void** pJobFile)
{
	if (m_pArchive == nullptr)
		return RESULT_BAD;

	*pJobFile = m_pArchive;
	return RESULT_GOOD;
}

UINT32 IArchiveDoc::SetSystemSpec(IN SystemSpec_t* pSystemSpec)
{
	m_pSystemSpec = nullptr;
	if (pSystemSpec == nullptr)
		return RESULT_BAD;

	m_pSystemSpec = pSystemSpec;
	return RESULT_GOOD;
}

UINT32 IArchiveDoc::ClearLayers()
{
	if (m_pArchive == nullptr)
		return RESULT_BAD;
	
	m_pArchive->m_bLoaded = false;
	m_pArchive->ClearLayers();
	
	return RESULT_GOOD;
}

UINT32 IArchiveDoc::SaveMisc(IN CString &strPath)
{
	if (m_pArchive)
	{
		return m_pArchive->SaveMisc(strPath);
	}

	return RESULT_BAD;
}

UINT32 IArchiveDoc::SaveLayer(IN UserLayerSet_t* pstUserLayerSet, IN CString &strPath)
{
	if (m_pArchive == nullptr) return RESULT_BAD;

	UINT32 iStepCnt = static_cast<UINT32>(m_pArchive->m_Matrix.m_arrMatrixStep.GetSize());
	if (iStepCnt <= 0)
		return RESULT_BAD;

	//끝에 "\\" 없애기
	int nLength = strPath.GetLength();
	if (strPath.Mid(nLength - 1) == _T("\\"))
	{
		strPath.Delete(nLength - 1);
	}

	m_pArchive->m_strFullPathName = strPath;

	for (auto it : pstUserLayerSet->vcLayerSet)
	{
		for (UINT32 iStep = 0; iStep < iStepCnt; iStep++)
		{
			CStep* pStep = m_pArchive->m_arrStep[iStep];

			m_pArchive->SaveLayer(iStep, pStep->m_strStepName, it);
		}
	}
// 			CLoadLayerJob stJob;
// 			stJob.SetData(iStep, pStep->m_strStepName, it);


	return RESULT_GOOD;
	
}

UINT32 IArchiveDoc::Compress(IN LPCTSTR fileName)
{
	if (m_pArchive)
		return CompressODB(fileName);

	return RESULT_BAD;
}