#pragma once

#include "afxwin.h"
#include "../iCommonIF/CommonIF-Error.h"
#include "CIniFunction.h"

#include <vector>
using namespace std;

class __declspec(dllexport) CUtils
{
public:
	CUtils() {};
	~CUtils() {};

	static CString GetVersionInfo(const CString strFile);
	static CString GetFileInfo(UINT32 mode, LPCTSTR strFullPath);
	static CString GetDriveLetter();
	static CString GetAppPath();

	static UINT32 RemoveDirectoryFile(LPCTSTR PathDir);
	static UINT32 MoveDirectoryFile(BOOL bSrcDelete, LPCTSTR strFrom, LPCTSTR strTo);

	static UINT32 GetCoreCount();
	static UINT32 CheckCreateFolder(CString strFolderPath);
	static UINT32 CheckFile(CString strFilePath);
	static CHAR* ConvertWideCharToAnsiString(CString strData);

	//
	static CString GetFileName(IN const CString &strPathName);
	static CString ChangeExt(IN const CString &strFileName, IN const CString &strExtName);

	//Start ~ End String 까지의 기존 String을 strNew로 교체한다.
	static void ChangeString(CString &strData, IN const CString &strStart, IN const CString &strEnd, IN const CString &strNew);

	//입력받은 String을 Separation일 기준으로 나누어 준다. 예시) strSeparation = ","
	static void Split(const CString& strString, const CString& strSeparation, vector<CString>& vecWord);
	//CString 을 문자별로 Slice 해서 vecotr에 담는다.
	static void SliceString(const CString& strString, const CString& strSeparation, vector<CString>& vecWord);
	static void StringOutputIndex(CString strLayer, vector<CString> vecWord, INT32& iIndex, BOOL& bCheck);

	static BOOL CopyFolder(const CString& strFrom, const CString& strTo);

	template<typename T>
	static BOOL OutOfRange(const vector<T> &vector, int nIndex)
	{
		return nIndex < 0 || nIndex >= static_cast<int>(vector.size());
	}
	//

	static BOOL IsNumber(IN CString str);
	
};