// dllmain.cpp : DLL 애플리케이션의 진입점을 정의합니다.
#include "pch.h"
#include "iArchive.h"
#include "iArchiveDoc.h"

static class IArchiveDoc	g_ArchiveDoc;

//BOOL APIENTRY DllMain( HMODULE hModule,
//                       DWORD  ul_reason_for_call,
//                       LPVOID lpReserved
//                     )
//{
//    switch (ul_reason_for_call)
//    {
//    case DLL_PROCESS_ATTACH:
//    case DLL_THREAD_ATTACH:
//    case DLL_THREAD_DETACH:
//    case DLL_PROCESS_DETACH:
//        break;
//    }
//    return TRUE;
//}

//1. Create
IARCHIVE_API UINT32	cmnCreate(OUT Version& v)
{
    if (RESULT_GOOD == g_ArchiveDoc.GetVersionInfo(v))
        return g_ArchiveDoc.Create();
    else
        return  RESULT_BAD;
}

//2. Initiate
IARCHIVE_API UINT32 cmnInitiate()
{
    return g_ArchiveDoc.Initiate(); 
}

//3. Destory
IARCHIVE_API UINT32 cmnDestroy()
{
    return g_ArchiveDoc.Destroy();
}

//4. Extract
IARCHIVE_API UINT32 cmnExtract(IN BOOL bAlwaysCreate, IN LPCTSTR fileName)
{
	return g_ArchiveDoc.Extract(bAlwaysCreate, fileName);	
}

//5. Load Misc
IARCHIVE_API UINT32 cmnLoadMisc()
{
	return g_ArchiveDoc.LoadMisc();
}

//6. Load Matrix
IARCHIVE_API UINT32 cmnLoadMatrix()
{
    return g_ArchiveDoc.LoadMatrix();
}

//7. Load Step
IARCHIVE_API UINT32 cmnLoadStep()
{
    return g_ArchiveDoc.LoadStep();
}

//8. Load Font
IARCHIVE_API UINT32 cmnLoadFont()
{
    return g_ArchiveDoc.LoadFont();
}

//9. Load UserSymbol
IARCHIVE_API UINT32 cmnLoadUserSymbols()
{
    return g_ArchiveDoc.LoadUserSymbols();
}

//10. Get Step Count
IARCHIVE_API UINT32 cmnGetStepCount(OUT UINT32& iCount)
{
    return g_ArchiveDoc.GetStepCount(iCount);
}

//11. Get Step Repeat Count
IARCHIVE_API UINT32 cmnGetStepRepeatCount(OUT std::vector<UINT32>& vcCount)
{
    return g_ArchiveDoc.GetStepRepeatCount(vcCount);
}

//12. Get Step Names
IARCHIVE_API UINT32 cmnGetStepInfo(OUT std::vector<stStepInfo*>* pStepInfo)
{
    return g_ArchiveDoc.GetStepInfo(pStepInfo);
}

//13. Get Model Name	
IARCHIVE_API UINT32 cmnGetModelName(OUT CString& strModel)
{
    return g_ArchiveDoc.GetModelName(strModel);
}

//14. Get Layer Count
IARCHIVE_API UINT32 cmnGetLayerCount(OUT UINT32& iCount)
{
    return g_ArchiveDoc.GetLayerCount(iCount);
}

//15. Get Layer Names
IARCHIVE_API UINT32 cmnGetLayerInfo(OUT std::vector<stLayerInfo*>* pLayerInfo)
{
    return g_ArchiveDoc.GetLayerInfo(pLayerInfo);
}

//16. Get Component Count
IARCHIVE_API UINT32 cmnGetComponentCount(OUT std::vector<UINT32>& vcCount)
{
    return g_ArchiveDoc.GetComponentCount(vcCount);
}

//17. Get Component Info
IARCHIVE_API UINT32 cmnGetComponentInfo(OUT std::vector<stComponentInfo*>* pCompInfo)
{
    return g_ArchiveDoc.GetComponentInfo(pCompInfo);
}

//18. Load Layer
IARCHIVE_API UINT32 cmnLoadLayer(IN UserLayerSet_t* pstUserLayerSet, IN BOOL bCheckOnly)
{
	return g_ArchiveDoc.LoadLayer(pstUserLayerSet, bCheckOnly);
}

//19. Get Job File
IARCHIVE_API UINT32 cmnGetJobFile(OUT void** pJobFile)
{
	return g_ArchiveDoc.GetJobFile(pJobFile);
}

//20. Set SystempSpec
IARCHIVE_API UINT32 cmnSetSystemSpec(IN SystemSpec_t* pSystemSpec)
{
	return g_ArchiveDoc.SetSystemSpec(pSystemSpec);
}

//21. Clear Layers
IARCHIVE_API UINT32 cmnClearLayers()
{
	return g_ArchiveDoc.ClearLayers();
}

//22. Clear Layers
IARCHIVE_API UINT32 cmnSaveMisc(IN CString &strPath)
{
	return g_ArchiveDoc.SaveMisc(strPath);
}

IARCHIVE_API UINT32 cmnSaveLayer(IN UserLayerSet_t* pstUserLayerSet, IN CString &strPath)
{
	return g_ArchiveDoc.SaveLayer(pstUserLayerSet, strPath);
}

IARCHIVE_API UINT32 cmnCompress(IN CString &strPath)
{
	return g_ArchiveDoc.Compress(strPath);
}