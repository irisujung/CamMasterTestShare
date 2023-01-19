#ifndef __IARCHIVE_H__
#define __IARCHIVE_H__

#ifdef IARCHIVE_EXPORTS
#define IARCHIVE_API __declspec(dllexport)
#else
#define IARCHIVE_API __declspec(dllimport)
#endif

#include "../iCommonIF/CommonIF-Struct.h"
#include "../iCommonIF/CommonIF-Error.h"
#include "../iCommonIF/CommonIF-Define.h"
#include "../iCommonIF/CommonIF-Path.h"
#include <vector>

#ifndef IARCHIVE_EXPORTS
namespace IARCHIVE
{
#endif
#ifdef __cplusplus
	extern "C" {
#endif
	// function definitions
	//1. Create
		IARCHIVE_API UINT32	cmnCreate(OUT Version& v);
	//2. initiate
		IARCHIVE_API UINT32 cmnInitiate();	
	//3. Destory
		IARCHIVE_API UINT32 cmnDestroy();
	//4. Extract ODB
		IARCHIVE_API UINT32 cmnExtract(IN BOOL bAlwaysCreate, IN LPCTSTR fileName);
	//5. Load Misc
		IARCHIVE_API UINT32 cmnLoadMisc();
	//6. Load Matrix
		IARCHIVE_API UINT32 cmnLoadMatrix();
	//7. Load Step
		IARCHIVE_API UINT32 cmnLoadStep();
	//8. Load Font
		IARCHIVE_API UINT32 cmnLoadFont();
	//9. Load UserSymbol
		IARCHIVE_API UINT32 cmnLoadUserSymbols();
	//10. Get Step Count
		IARCHIVE_API UINT32 cmnGetStepCount(OUT UINT32& iCount);
	//11. Get Step Repeat Count
		IARCHIVE_API UINT32 cmnGetStepRepeatCount(OUT std::vector<UINT32>& vcCount);
	//12. Get Step Info		
		IARCHIVE_API UINT32 cmnGetStepInfo(OUT std::vector<stStepInfo*>* pStepInfo);
	//13. Get Model Name	
		IARCHIVE_API UINT32 cmnGetModelName(OUT CString& strModel);
	//14. Get Layer Count
		IARCHIVE_API UINT32 cmnGetLayerCount(OUT UINT32& iCount);
	//15. Get Layer Info
		IARCHIVE_API UINT32 cmnGetLayerInfo(OUT std::vector<stLayerInfo*>* pLayerInfo);
	//16. Get Component Count
		IARCHIVE_API UINT32 cmnGetComponentCount(OUT std::vector<UINT32>& vcCount);
	//17. Get Component Info
		IARCHIVE_API UINT32 cmnGetComponentInfo(OUT std::vector<stComponentInfo*>* pCompInfo);
	//18. Load Layer
		IARCHIVE_API UINT32 cmnLoadLayer(IN UserLayerSet_t* pstUserLayerSet, IN BOOL bCheckOnly);
	//19. Get Job File
		IARCHIVE_API UINT32 cmnGetJobFile(OUT void** pJobFile);
	//20. Set SystempSpec
		IARCHIVE_API UINT32 cmnSetSystemSpec(IN SystemSpec_t* pSystemSpec);
	//21. Clear Layers
		IARCHIVE_API UINT32 cmnClearLayers();

	//22. SaveMisc
		IARCHIVE_API UINT32 cmnSaveMisc(IN CString &strPath);
	//23. SaveLayer
		IARCHIVE_API UINT32 cmnSaveLayer(IN UserLayerSet_t* pstUserLayerSet, IN CString &strPath);

		//24. Compress
		IARCHIVE_API UINT32 cmnCompress(IN CString &strPath);

#ifdef __cplusplus
	}
#endif

#ifndef IARCHIVE_EXPORTS
} //end of namespace
#endif
#endif
