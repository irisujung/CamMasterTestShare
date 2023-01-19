// dllmain.cpp : DLL 애플리케이션의 진입점을 정의합니다.
#include "pch.h"
#include "iArchiveIF.h"

IArchiveDLL* g_pArchiveDLL = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

//1. Create
IARCHIVEIF_API UINT32 IArchive_Create(OUT Version& v)
{
    UINT32 rst = RESULT_BAD;

    //dynamic loading
    if (g_pArchiveDLL == NULL)
    {
        g_pArchiveDLL = new IArchiveDLL(DLLNAME_IARCHIVE);
        if (g_pArchiveDLL)        
            rst = g_pArchiveDLL->Create(v);  
    }
    return rst;
}

//2. initiate
IARCHIVEIF_API UINT32 IArchive_Initiate()
{
    if (g_pArchiveDLL)
        return g_pArchiveDLL->Initiate();

    return RESULT_BAD;
}

//3. Destory
IARCHIVEIF_API UINT32 IArchive_Destory()
{
    UINT32 rst = RESULT_BAD;
    if (g_pArchiveDLL)
    {
        rst = g_pArchiveDLL->Destroy();
        delete g_pArchiveDLL;
        g_pArchiveDLL = NULL;
    }

    return rst;
}

//4. Extract ODB
IARCHIVEIF_API UINT32 IArchive_Extract(IN BOOL bAlwaysCreate, IN LPCTSTR fileName)
{
    if (g_pArchiveDLL)
        return g_pArchiveDLL->Extract(bAlwaysCreate, fileName);

    return RESULT_BAD;
}

//5. Load Misc
IARCHIVEIF_API UINT32 IArchive_LoadMisc()
{
	if (g_pArchiveDLL)
		return g_pArchiveDLL->LoadMisc();

	return RESULT_BAD;
}

//6. Load Matrix
IARCHIVEIF_API UINT32 IArchive_LoadMatrix()
{
    if (g_pArchiveDLL)
        return g_pArchiveDLL->LoadMatrix();

    return RESULT_BAD;
}

//7. Load Step
IARCHIVEIF_API UINT32 IArchive_LoadStep()
{
    if (g_pArchiveDLL)
        return g_pArchiveDLL->LoadStep();

    return RESULT_BAD;
}

//8. Load Font
IARCHIVEIF_API UINT32 IArchive_LoadFont()
{
    if (g_pArchiveDLL)
        return g_pArchiveDLL->LoadFont();

    return RESULT_BAD;
}

//9. Load UserSymbols
IARCHIVEIF_API UINT32 IArchive_LoadUserSymbols()
{
    if (g_pArchiveDLL)
        return g_pArchiveDLL->LoadUserSymbols();

    return RESULT_BAD;
}

//10. Get Step Count
IARCHIVEIF_API UINT32 IArchive_GetStepCount(OUT UINT32& iCount)
{
    if (g_pArchiveDLL)
        return g_pArchiveDLL->GetStepCount(iCount);

    return RESULT_BAD;
}

//11. Get Step Repeat Count
IARCHIVEIF_API UINT32 IArchive_GetStepRepeatCount(OUT std::vector<UINT32>& vcCount)
{
    if (g_pArchiveDLL)
        return g_pArchiveDLL->GetStepRepeatCount(vcCount);

    return RESULT_BAD;
}

//12. Get Step Info
IARCHIVEIF_API UINT32 IArchive_GetStepInfo(OUT std::vector<stStepInfo*>* pStepInfo)
{
    if (g_pArchiveDLL)
        return g_pArchiveDLL->GetStepInfo(pStepInfo);

    return RESULT_BAD;
}

//13. Get Model Name
IARCHIVEIF_API UINT32 IArchive_GetModelName(OUT CString& strModel)
{
    if (g_pArchiveDLL)
        return g_pArchiveDLL->GetModelName(strModel);

    return RESULT_BAD;
}

//14. Get Layer Count
IARCHIVEIF_API UINT32 IArchive_GetLayerCount(OUT UINT32& iCount)
{
    if (g_pArchiveDLL)
        return g_pArchiveDLL->GetLayerCount(iCount);

    return RESULT_BAD;
}

//15. Get Layer Info
IARCHIVEIF_API UINT32 IArchive_GetLayerInfo(OUT std::vector<stLayerInfo*>* pLayerInfo)
{
    if (g_pArchiveDLL)
        return g_pArchiveDLL->GetLayerInfo(pLayerInfo);

    return RESULT_BAD;
}

//16. Get Component Count
IARCHIVEIF_API UINT32 IArchive_GetComponentCount(OUT std::vector<UINT32>& vcCount)
{
    if (g_pArchiveDLL)
        return g_pArchiveDLL->GetComponentCount(vcCount);

    return RESULT_BAD;
}

//17. Get Component Info
IARCHIVEIF_API UINT32 IArchive_GetComponentInfo(OUT std::vector<stComponentInfo*>* pCompInfo)
{
    if (g_pArchiveDLL)
        return g_pArchiveDLL->GetComponentInfo(pCompInfo);

    return RESULT_BAD;
}

//18. Load Layer
IARCHIVEIF_API UINT32 IArchive_LoadLayer(IN UserLayerSet_t* pstUserLayerSet, IN BOOL bCheckOnly)
{
	if (g_pArchiveDLL)
		return g_pArchiveDLL->LoadLayer(pstUserLayerSet, bCheckOnly);

	return RESULT_BAD;
}

//19. Get Job File
IARCHIVEIF_API UINT32 IArchive_GetJobFile(OUT void** pJobFile)
{
	if (g_pArchiveDLL)
		return g_pArchiveDLL->GetJobFile(pJobFile);

	return RESULT_BAD;
}

//20. SetSystemSpec
IARCHIVEIF_API UINT IArchive_SetSystemSpec(IN SystemSpec_t* pSystemSpec)
{
	if (g_pArchiveDLL)
		return g_pArchiveDLL->SetSystemSpec(pSystemSpec);

	return RESULT_BAD;
}

//21. Clear Layers
IARCHIVEIF_API UINT32 IArchive_ClearLayers()
{
	if (g_pArchiveDLL)
		return g_pArchiveDLL->ClearLayers();

	return RESULT_BAD;
}

//22. Save Misc
IARCHIVEIF_API UINT32 IArchive_SaveMisc(IN CString &strPath)
{
	if (g_pArchiveDLL)
		return g_pArchiveDLL->SaveMisc(strPath);

	return RESULT_BAD;
}

//13. Save Layer
IARCHIVEIF_API UINT32 IArchive_SaveLayer(IN UserLayerSet_t* pstUserLayerSet, CString &strPath)
{
	if (g_pArchiveDLL)
		return g_pArchiveDLL->SaveLayer(pstUserLayerSet, strPath);

	return RESULT_BAD;
}

//24. Compress
IARCHIVEIF_API UINT32 IArchive_Compress(IN CString &strPath)
{
	if (g_pArchiveDLL)
		return g_pArchiveDLL->Compress(strPath);

	return RESULT_BAD;
}