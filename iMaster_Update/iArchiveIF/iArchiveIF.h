#ifndef __IARCHIVEIF_H__
#define __IARCHIVEIF_H__

#ifdef IARCHIVEIF_EXPORTS
#define IARCHIVEIF_API __declspec(dllexport)
#else
#define IARCHIVEIF_API __declspec(dllimport)
#endif

#include "../iCommonIF/CommonIF-Define.h"
#include "../iCommonIF/CommonIF-Error.h"
#include "../iCommonIF/CommonIF-Path.h"
#include "../iCommonIF/CommonIF-Struct.h"
#include <vector>

#ifndef IARCHIVEIF_EXPORTS
namespace IARCHIVEIF
{
#endif
	//Static DLL Load
#ifdef __cplusplus
	extern "C" {
#endif

	// IArchive
	//1. Create
	IARCHIVEIF_API UINT32 IArchive_Create(OUT Version& v);
	//2. initiate
	IARCHIVEIF_API UINT32 IArchive_Initiate();
	//3. Destory
	IARCHIVEIF_API UINT32 IArchive_Destory();
	//4. Extract 
	IARCHIVEIF_API UINT32 IArchive_Extract(IN BOOL bAlwaysCreate, IN LPCTSTR fileName);
	//5. Load Misc
	IARCHIVEIF_API UINT32 IArchive_LoadMisc();
	//6. Load Matrix
	IARCHIVEIF_API UINT32 IArchive_LoadMatrix();
	//7. Load Step
	IARCHIVEIF_API UINT32 IArchive_LoadStep();
	//8. Load Font
	IARCHIVEIF_API UINT32 IArchive_LoadFont();
	//9. Load UserSymbols
	IARCHIVEIF_API UINT32 IArchive_LoadUserSymbols();
	//10. Get Step Count
	IARCHIVEIF_API UINT32 IArchive_GetStepCount(OUT UINT32& iCount);
	//11. Get Step Repeat Count
	IARCHIVEIF_API UINT32 IArchive_GetStepRepeatCount(OUT std::vector<UINT32>& vcCount);
	//12. Get Step Info
	IARCHIVEIF_API UINT32 IArchive_GetStepInfo(OUT std::vector<stStepInfo*>* pStepInfo);
	//13. Get Model Name
	IARCHIVEIF_API UINT32 IArchive_GetModelName(OUT CString& strModel);
	//14. Get Layer Count
	IARCHIVEIF_API UINT32 IArchive_GetLayerCount(OUT UINT32& iCount);
	//15. Get Layer Info
	IARCHIVEIF_API UINT32 IArchive_GetLayerInfo(OUT std::vector<stLayerInfo*>* pLayerInfo);
	//16. Get Component Count
	IARCHIVEIF_API UINT32 IArchive_GetComponentCount(OUT std::vector<UINT32>& vcCount);
	//17. Get Component Info
	IARCHIVEIF_API UINT32 IArchive_GetComponentInfo(OUT std::vector<stComponentInfo*>* pCompInfo);
	//18. Load Layer
	IARCHIVEIF_API UINT32 IArchive_LoadLayer(IN UserLayerSet_t* pstUserLayerSet, IN BOOL bCheckOnly = FALSE);
	//19. Get Job File
	IARCHIVEIF_API UINT32 IArchive_GetJobFile(OUT void** pJobFile);
	//20. SetSystemSpec
	IARCHIVEIF_API UINT32 IArchive_SetSystemSpec(IN SystemSpec_t* pSystemSpec);
	//21. Clear Layers
	IARCHIVEIF_API UINT32 IArchive_ClearLayers();

	//22. Save Misc
	IARCHIVEIF_API UINT32 IArchive_SaveMisc(IN CString &strPath);

	//23. Save Layer
	IARCHIVEIF_API UINT32 IArchive_SaveLayer(IN UserLayerSet_t* pstUserLayerSet, IN CString &strPath);

	//24. Compress
	IARCHIVEIF_API UINT32 IArchive_Compress(IN CString &strPath);

#ifdef __cplusplus
	}
#endif

	class IArchiveDLL
	{
	private:
#if defined(_M_X64) || defined(WIN64)
#define DLLNAME_IARCHIVE	L"iArchive.dll"
#else
#define DLLNAME_IARCHIVE	_T("iArchive32.dll")
#endif

#define	SA_ProcessNum		24
		typedef UINT32(__cdecl* SA_Create)				(OUT Version& v);
		typedef UINT32(__cdecl* SA_Initiate)			();
		typedef UINT32(__cdecl* SA_Destroy)				();
		typedef UINT32(__cdecl* SA_Extract)				(IN BOOL bAlwaysCreate, IN LPCTSTR fileName);
		typedef UINT32(__cdecl* SA_LoadMisc)			();
		typedef UINT32(__cdecl* SA_LoadMatrix)			();
		typedef UINT32(__cdecl* SA_LoadStep)			();
		typedef UINT32(__cdecl* SA_LoadFont)			();
		typedef UINT32(__cdecl* SA_LoadUserSymbols)		();
		typedef UINT32(__cdecl* SA_GetStepCount)		(OUT UINT32& iCount);
		typedef UINT32(__cdecl* SA_GetStepRepeatCount)	(OUT std::vector<UINT32>& vcCount);
		typedef UINT32(__cdecl* SA_GetStepInfo)			(OUT std::vector<stStepInfo*>* pStepInfo);
		typedef UINT32(__cdecl* SA_GetModelName)		(OUT CString& strModel);
		typedef UINT32(__cdecl* SA_GetLayerCount)		(OUT UINT32& iCount);
		typedef UINT32(__cdecl* SA_GetLayerInfo)		(OUT std::vector<stLayerInfo*>* pLayerInfo);
		typedef UINT32(__cdecl* SA_GetComponentCount)	(OUT std::vector<UINT32>& vcCount);
		typedef UINT32(__cdecl* SA_GetComponentInfo)	(OUT std::vector<stComponentInfo*>* pCompInfo);
		typedef UINT32(__cdecl* SA_LoadLayer)			(IN UserLayerSet_t* pstUserLayerSet, IN BOOL bCheckOnly);
		typedef UINT32(__cdecl* SA_GetJobFile)			(OUT void** pJobFile);
		typedef UINT32(__cdecl* SA_SetSystemSpec)		(IN SystemSpec_t* pSystemSpec);
		typedef UINT32(__cdecl* SA_ClearLayers)			();
		typedef UINT32(__cdecl* SA_SaveMisc)			(IN CString &strPath);
		typedef UINT32(__cdecl* SA_SaveLayer)			(IN UserLayerSet_t* pstUserLayerSet, IN CString &strPath);
		typedef UINT32(__cdecl* SA_Compress)			(IN CString &strPath);

		HMODULE			hModule;
		FARPROC			fpProc[SA_ProcessNum];

	public:
		IArchiveDLL(CString dllName)
		{
			hModule = NULL;
			LoadDLL(dllName);
		};
		~IArchiveDLL(void) { FreeDLL(); };
		void FreeDLL()
		{
			CString str;
			str.Format(_T("\r\nDll Free<0x:%X>"), hModule);
			OutputDebugString(str);

			hModule = NULL;
			memset(fpProc, 0, sizeof(FARPROC) * SA_ProcessNum);
		}

		UINT32 LoadDLL(CString dllName)
		{
			UINT32 rst = RESULT_GOOD, i, count = 0;
			CString str, str2;
			const char* pcstrProcName, strProcNameCMN[SA_ProcessNum][30]
				= { { "cmnCreate" }, { "cmnInitiate" }, { "cmnDestroy" }, { "cmnExtract" }, { "cmnLoadMisc" },{ "cmnLoadMatrix" },
					{ "cmnLoadStep" }, { "cmnLoadFont" }, { "cmnLoadUserSymbols" }, { "cmnGetStepCount" }, { "cmnGetStepRepeatCount" },
					{ "cmnGetStepInfo"}, { "cmnGetModelName"}, { "cmnGetLayerCount" }, { "cmnGetLayerInfo"}, { "cmnGetComponentCount" },
					{ "cmnGetComponentInfo"}, { "cmnLoadLayer"}, { "cmnGetJobFile"}, { "cmnSetSystemSpec" }, { "cmnClearLayers" },
					{ "cmnSaveMisc" }, { "cmnSaveLayer" }, { "cmnCompress" } };
			const char strProcNameDIV[SA_ProcessNum][30]
				= { { "divCreate" }, { "divInitiate" }, { "divDestroy" }, { "divExtract" }, { "cmnLoadMisc" }, { "divLoadMatrix"},
					{ "divLoadStep" }, { "divLoadFont" }, { "divLoadUserSymbols" }, { "divGetStepCount" }, { "divGetStepRepeatCount" },
					{ "divGetStepInfo"}, { "divGetModelName"}, { "divGetLayerCount" }, { "divGetLayerInfo"}, { "divGetComponentCount" },
					{ "divGetComponentInfo"}, { "divLoadLayer"}, { "divGetJobFile"}, { "divSetSystemSpec" }, { "divClearLayers" },
					{ "divSaveMisc" }, { "divSaveLayer" }, { "divCompress" } };
			FreeDLL();

			str.Format(_T("\r\nDll<%s>Load"), dllName);	OutputDebugString(str);
			try {
				hModule = ::LoadLibrary(dllName);
			}//LoadLibraryEx 
			catch (...) { str.Format(_T("\r\nDll<%s>LoadLibrary() : Exception"), dllName);	OutputDebugString(str); }
			if (hModule == NULL)
			{
				str.Format(_T("\r\nDll<%s>Load Error!!!<%s>"), dllName, ::GetLastError());	OutputDebugString(str);
				return RESULT_BAD;
			}
			else
			{
				str.Format(_T("\r\nDll<%s>Load OK!!!<0x:%X>"), dllName, hModule);	OutputDebugString(str);
			}
			//GetProcAddress
			for (i = 0; i < SA_ProcessNum; i++)
			{
				pcstrProcName = (dllName == DLLNAME_IARCHIVE) ? strProcNameCMN[i] : strProcNameDIV[i];
				if (strcmp(pcstrProcName, "") == 0) continue;
				try { fpProc[i] = ::GetProcAddress(hModule, pcstrProcName); }
				catch (...) { str2 = pcstrProcName; str.Format(_T("\r\nDll<%s>GetProcAddress(%s) : Exception"), dllName, str2);	OutputDebugString(str); }
				if (fpProc[i]) continue;
				rst = RESULT_BAD; ++count;
				str2 = pcstrProcName;
				str.Format(_T("\r\nDll<%s>GetProcAddress(%s) : Failed !!!"), dllName, str2);	OutputDebugString(str);
			}
			if (count) str.Format(_T("\r\nDll<%s>GetProcAddress() (Failed Num :%d) !!!"), dllName, count);	OutputDebugString(str);
			return rst;
		};

		UINT32 Create(OUT Version& v)
		{
			if (!fpProc[0]) return RESULT_BAD; return ((SA_Create)fpProc[0])(v);
		};
		UINT32 Initiate()
		{
			if (!fpProc[1]) return RESULT_BAD; return ((SA_Initiate)fpProc[1])();
		};
		UINT32 Destroy()
		{
			if (!fpProc[2]) return RESULT_BAD; return ((SA_Destroy)fpProc[2])();
		};
		UINT32 Extract(IN BOOL bAlwaysCreate, IN LPCTSTR fileName)
		{
			if (!fpProc[3]) return RESULT_BAD; return ((SA_Extract)fpProc[3])(bAlwaysCreate, fileName);
		};
		UINT32 LoadMisc()
		{
			if (!fpProc[4]) return RESULT_BAD; return ((SA_LoadMisc)fpProc[4])();
		};
		UINT32 LoadMatrix()
		{
			if (!fpProc[5]) return RESULT_BAD; return ((SA_LoadMatrix)fpProc[5])();
		};
		UINT32 LoadStep()
		{
			if (!fpProc[6]) return RESULT_BAD; return ((SA_LoadStep)fpProc[6])();
		};
		UINT32 LoadFont()
		{
			if (!fpProc[7]) return RESULT_BAD; return ((SA_LoadFont)fpProc[7])();
		};
		UINT32 LoadUserSymbols()
		{
			if (!fpProc[8]) return RESULT_BAD; return ((SA_LoadUserSymbols)fpProc[8])();
		};
		UINT32 GetStepCount(OUT UINT32& iCount)
		{
			if (!fpProc[9]) return RESULT_BAD; return ((SA_GetStepCount)fpProc[9])(iCount);
		};
		UINT32 GetStepRepeatCount(OUT std::vector<UINT32>& vcCount)
		{
			if (!fpProc[10]) return RESULT_BAD; return ((SA_GetStepRepeatCount)fpProc[10])(vcCount);
		};
		UINT32 GetStepInfo(OUT std::vector<stStepInfo*>* pStepInfo)
		{
			if (!fpProc[11]) return RESULT_BAD; return ((SA_GetStepInfo)fpProc[11])(pStepInfo);
		};
		UINT32 GetModelName(OUT CString& strModel)
		{
			if (!fpProc[12]) return RESULT_BAD; return ((SA_GetModelName)fpProc[12])(strModel);
		};
		UINT32 GetLayerCount(OUT UINT32& iCount)
		{
			if (!fpProc[13]) return RESULT_BAD; return ((SA_GetLayerCount)fpProc[13])(iCount);
		};
		UINT32 GetLayerInfo(OUT std::vector<stLayerInfo*>* pLayerInfo)
		{
			if (!fpProc[14]) return RESULT_BAD; return ((SA_GetLayerInfo)fpProc[14])(pLayerInfo);
		};
		UINT32 GetComponentCount(OUT std::vector<UINT32>& vcCount)
		{
			if (!fpProc[15]) return RESULT_BAD; return ((SA_GetComponentCount)fpProc[15])(vcCount);
		};
		UINT32 GetComponentInfo(OUT std::vector<stComponentInfo*>* pCompInfo)
		{
			if (!fpProc[16]) return RESULT_BAD; return ((SA_GetComponentInfo)fpProc[16])(pCompInfo);
		};
		UINT32 LoadLayer(IN UserLayerSet_t* pstUserLayerSet, IN BOOL bCheckOnly)
		{
			if (!fpProc[17]) return RESULT_BAD; return ((SA_LoadLayer)fpProc[17])(pstUserLayerSet, bCheckOnly);
		};
		UINT32 GetJobFile(OUT void** pJobFile)
		{
			if (!fpProc[18]) return RESULT_BAD; return ((SA_GetJobFile)fpProc[18])(pJobFile);
		};
		UINT32 SetSystemSpec(IN SystemSpec_t* pSystemSpec)
		{
			if (!fpProc[19]) return RESULT_BAD; return ((SA_SetSystemSpec)fpProc[19])(pSystemSpec);
		};
		UINT32 ClearLayers()
		{
			if (!fpProc[20]) return RESULT_BAD; return ((SA_ClearLayers)fpProc[20])();
		};

		//Save Misc
		UINT32 SaveMisc(IN CString &strPath)
		{
			if (!fpProc[21]) return RESULT_BAD; return ((SA_SaveMisc)fpProc[21])(strPath);
		};

		//Save Layer
		UINT32 SaveLayer(IN UserLayerSet_t* pstUserLayerSet, IN CString &strPath)
		{
			if (!fpProc[22]) return RESULT_BAD; return ((SA_SaveLayer)fpProc[22])(pstUserLayerSet, strPath);
		};

		//Compress
		UINT32 Compress(IN CString &strPath)
		{
			if (!fpProc[23]) return RESULT_BAD; return ((SA_Compress)fpProc[23])(strPath);
		};
	};
#ifndef IARCHIVEIF_EXPORTS	
}
#endif
#endif