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

	//Start ~ End String ������ ���� String�� strNew�� ��ü�Ѵ�.
	static void ChangeString(CString &strData, IN const CString &strStart, IN const CString &strEnd, IN const CString &strNew);

	//�Է¹��� String�� Separation�� �������� ������ �ش�. ����) strSeparation = ","
	static void Split(const CString& strString, const CString& strSeparation, vector<CString>& vecWord);
	//CString �� ���ں��� Slice �ؼ� vecotr�� ��´�.
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