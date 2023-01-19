#include "pch.h"
#include "afxwin.h"
#include "CIniFunction.h"

int CIniFunction::GetINI_Int(CString strApp, CString strKey, CString strDefault, CString strIniFileName)
{
	TCHAR cBuffer[256];
	GetPrivateProfileString(strApp, strKey, strDefault, cBuffer, 256, strIniFileName);
	CString strTemp;
	strTemp.Format(_T("%s"), cBuffer);

	//MODIFY.GoEunKim.2019.10.17.START.Default 값 쓰기
	if (strTemp == "")
	{
		strTemp = strDefault;
	}
	//MODIFY.GoEunKim.2019.10.17.START.Default 값 쓰기

	return _ttoi(strTemp);
}
int CIniFunction::GetINI_Int(CString strApp, CString strKey, int nDefault, CString strIniFileName)
{
	CString strDefault;
	strDefault.Format(_T("%d"), nDefault);
	TCHAR cBuffer[256];
	GetPrivateProfileString(strApp, strKey, strDefault, cBuffer, 256, strIniFileName);
	CString strTemp;
	strTemp.Format(_T("%s"), cBuffer);

	return _ttoi(strTemp);
}


long CIniFunction::GetINI_Long(CString strApp, CString strKey, CString strDefault, CString strIniFileName)
{
	TCHAR cBuffer[256];
	GetPrivateProfileString(strApp, strKey, strDefault, cBuffer, 256, strIniFileName);
	CString strTemp;
	strTemp.Format(_T("%s"), cBuffer);

	//MODIFY.GoEunKim.2019.10.17.START.Default 값 쓰기
	if (strTemp == "")
	{
		strTemp = strDefault;
	}
	//MODIFY.GoEunKim.2019.10.17.START.Default 값 쓰기

	return _ttoi(strTemp);
}
long CIniFunction::GetINI_Long(CString strApp, CString strKey, long nDefault, CString strIniFileName)
{
	CString strDefault;
	strDefault.Format(_T("%ld"), nDefault);

	TCHAR cBuffer[256];
	GetPrivateProfileString(strApp, strKey, strDefault, cBuffer, 256, strIniFileName);
	CString strTemp;
	strTemp.Format(_T("%s"), cBuffer);

	return _ttoi(strTemp);
}
double CIniFunction::GetINI_Double(CString strApp, CString strKey, CString strDefault, CString strIniFileName)
{
	TCHAR cBuffer[256];
	GetPrivateProfileString(strApp, strKey, strDefault, cBuffer, 256, strIniFileName);
	CString strTemp;
	strTemp.Format(_T("%s"), cBuffer);

	//MODIFY.GoEunKim.2019.10.17.START.Default 값 쓰기
	if (strTemp == "")
	{
		strTemp = strDefault;
	}
	//MODIFY.GoEunKim.2019.10.17.START.Default 값 쓰기

	return _ttof(strTemp);
}
double  CIniFunction::GetINI_Double(CString strApp, CString strKey, double dDefault, CString strIniFileName)
{
	CString strDefault;
	strDefault.Format(_T("%f"), dDefault);
	TCHAR cBuffer[256];
	GetPrivateProfileString(strApp, strKey, strDefault, cBuffer, 256, strIniFileName);
	CString strTemp;
	strTemp.Format(_T("%s"), cBuffer);

	return _ttof(strTemp);
}

CString CIniFunction::GetINI_String(CString strApp, CString strKey, CString strDefault, CString strIniFileName)
{
	TCHAR cBuffer[256];
	GetPrivateProfileString(strApp, strKey, strDefault, cBuffer, 256, strIniFileName);
	CString strTemp;
	strTemp.Format(_T("%s"), cBuffer);

	//MODIFY.GoEunKim.2019.10.17.START.Default 값 쓰기
	if (strTemp == "")
	{
		strTemp = strDefault;
	}
	//MODIFY.GoEunKim.2019.10.17.START.Default 값 쓰기

	return strTemp;
}

void CIniFunction::SetINI_Int(CString strApp, CString strKey, int nData, CString strIniFileName)
{
	CString strTemp;
	strTemp.Format(_T("%d"), nData);
	WritePrivateProfileString(strApp, strKey, strTemp, strIniFileName);
}

void CIniFunction::SetINI_Long(CString strApp, CString strKey, long nData, CString strIniFileName)
{
	CString strTemp;
	strTemp.Format(_T("%d"), nData);
	WritePrivateProfileString(strApp, strKey, strTemp, strIniFileName);
}
void  SetINI_Double(CString strApp, CString strKey, double dData, CString strIniFileName)
{
	CString strTemp;
	strTemp.Format(_T("%.5f"), dData);
	WritePrivateProfileString(strApp, strKey, strTemp, strIniFileName);
}

void  CIniFunction::SetINI_Double1(CString strApp, CString strKey, double dData, CString strIniFileName)
{
	CString strTemp;
	strTemp.Format(_T("%.1f"), dData);
	WritePrivateProfileString(strApp, strKey, strTemp, strIniFileName);
}
void  CIniFunction::SetINI_Double2(CString strApp, CString strKey, double dData, CString strIniFileName)
{
	CString strTemp;
	strTemp.Format(_T("%.2f"), dData);
	WritePrivateProfileString(strApp, strKey, strTemp, strIniFileName);
}
void  CIniFunction::SetINI_Double3(CString strApp, CString strKey, double dData, CString strIniFileName)
{
	CString strTemp;
	strTemp.Format(_T("%.3f"), dData);
	WritePrivateProfileString(strApp, strKey, strTemp, strIniFileName);
}

void  CIniFunction::SetINI_Double4(CString strApp, CString strKey, double dData, CString strIniFileName)
{
	CString strTemp;
	strTemp.Format(_T("%.4f"), dData);
	WritePrivateProfileString(strApp, strKey, strTemp, strIniFileName);
}
void  CIniFunction::SetINI_Double5(CString strApp, CString strKey, double dData, CString strIniFileName)
{
	CString strTemp;
	strTemp.Format(_T("%.5f"), dData);
	WritePrivateProfileString(strApp, strKey, strTemp, strIniFileName);
}
void  CIniFunction::SetINI_Double6(CString strApp, CString strKey, double dData, CString strIniFileName)
{
	CString strTemp;
	strTemp.Format(_T("%.6f"), dData);
	WritePrivateProfileString(strApp, strKey, strTemp, strIniFileName);
}
void  CIniFunction::SetINI_Double7(CString strApp, CString strKey, double dData, CString strIniFileName)
{
	CString strTemp;
	strTemp.Format(_T("%.7f"), dData);
	WritePrivateProfileString(strApp, strKey, strTemp, strIniFileName);
}
void  CIniFunction::SetINI_Double8(CString strApp, CString strKey, double dData, CString strIniFileName)
{
	CString strTemp;
	strTemp.Format(_T("%.8f"), dData);
	WritePrivateProfileString(strApp, strKey, strTemp, strIniFileName);
}

void CIniFunction::SetINI_String(CString strApp, CString strKey, CString strData, CString strIniFileName)
{
	WritePrivateProfileString(strApp, strKey, strData, strIniFileName);
}