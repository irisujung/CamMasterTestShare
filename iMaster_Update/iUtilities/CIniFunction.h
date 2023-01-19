#pragma once

class __declspec(dllexport) CIniFunction
{
public:
	CIniFunction() {};
	~CIniFunction() {};

	static int		GetINI_Int(CString strApp, CString strKey, CString strDefault, CString strIniFileName);
	static int		GetINI_Int(CString strApp, CString strKey, int nDefault, CString strIniFileName);
	static double	GetINI_Double(CString strApp, CString strKey, CString strDefault, CString strIniFileName);
	static double	GetINI_Double(CString strApp, CString strKey, double dDefault, CString strIniFileName);
	static CString	GetINI_String(CString strApp, CString strKey, CString strDefault, CString strIniFileName);
	static long		GetINI_Long(CString strApp, CString strKey, CString strDefault, CString strIniFileName);
	static long		GetINI_Long(CString strApp, CString strKey, long nDefault, CString strIniFileName);


	static void SetINI_Int(CString strApp, CString strKey, int nData, CString strIniFileName);
	static void SetINI_Long(CString strApp, CString strKey, long nData, CString strIniFileName);
	static void SetINI_Double(CString strApp, CString strKey, double dData, CString strIniFileName);
	static void SetINI_Double1(CString strApp, CString strKey, double dData, CString strIniFileName);
	static void SetINI_Double2(CString strApp, CString strKey, double dData, CString strIniFileName);
	static void SetINI_Double3(CString strApp, CString strKey, double dData, CString strIniFileName);
	static void SetINI_Double4(CString strApp, CString strKey, double dData, CString strIniFileName);
	static void SetINI_Double5(CString strApp, CString strKey, double dData, CString strIniFileName);
	static void SetINI_Double6(CString strApp, CString strKey, double dData, CString strIniFileName);
	static void SetINI_Double7(CString strApp, CString strKey, double dData, CString strIniFileName);
	static void SetINI_Double8(CString strApp, CString strKey, double dData, CString strIniFileName);
	static void SetINI_String(CString strApp, CString strKey, CString strData, CString strIniFileName);


};

