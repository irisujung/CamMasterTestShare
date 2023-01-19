#pragma once

#include "../iCommonIF/CommonIF-Struct.h"
#include "../iCommonIF/CommonIF-Error.h"
#include "../iCommonIF/CommonIF-Define.h"
#include "../iCommonIF/CommonIF-Path.h"
#include "../iODB/iODB.h"
#include "../iUtilities/iUtilities.h"
#include "LoadLayerInit.h"

class IArchiveDoc
{
public:
	IArchiveDoc() : m_DLLVer(0,0,0,0), m_pArchive(nullptr) {};
	~IArchiveDoc() {};

public:
	UINT32 GetVersionInfo(OUT Version& v);
	UINT32 Create();
	UINT32 Initiate();
	UINT32 Destroy();
	
	UINT32 Extract(IN BOOL bAlwaysCreate, IN LPCTSTR fileName);
	UINT32 LoadMisc();
	UINT32 LoadMatrix();
	UINT32 LoadStep();
	UINT32 LoadFont();
	UINT32 LoadUserSymbols();

	UINT32 GetStepCount(OUT UINT32& iCount);
	UINT32 GetStepRepeatCount(OUT std::vector<UINT32>& vcCount);
	UINT32 GetStepInfo(OUT std::vector<stStepInfo*>* pStepInfo);
	UINT32 GetModelName(OUT CString& strModel);

	UINT32 GetLayerCount(OUT UINT32& iCount);
	UINT32 GetLayerInfo(OUT std::vector<stLayerInfo*>* pLayerInfo);

	UINT32 GetComponentCount(OUT std::vector<UINT32>& vcCount);
	UINT32 GetComponentInfo(OUT std::vector<stComponentInfo*>* pCompInfo);

	UINT32 LoadLayer(IN UserLayerSet_t* pstUserLayerSet, IN BOOL bCheckOnly);
	UINT32 LoadLayer_All();
	UINT32 GetJobFile(OUT void** pJobFile);

	UINT32 SetSystemSpec(IN SystemSpec_t* pSystemSpec);
	UINT32 ClearLayers();

	//Save
	UINT32 SaveMisc(IN CString &strPath);
	//Save Layer
	UINT32 SaveLayer(IN UserLayerSet_t* pstUserLayerSet, IN CString &strPath);

	//Compress
	UINT32 Compress(IN LPCTSTR fileName);

private:
	UINT32 ExtractODB(IN BOOL bAlwaysCreate, IN LPCTSTR fileName);
	UINT32 CompressODB(IN LPCTSTR fileName);

	// Tgz File Extract Function
	BOOL ExtractTgz(IN LPCTSTR strTgzPath, IN CString strFolderPath, IN LPCTSTR strFileName, IN BOOL bFirst);
	
	// SaveDate Read Function	
	BOOL ReadMiscInfoFile(IN CString fileName, OUT double &dSaveDate);

private:
	Version			m_DLLVer;
	CJobFile*		m_pArchive = nullptr;
	ILoadLayerOp*	m_pLoadLayerOp = nullptr;
	SystemSpec_t*	m_pSystemSpec = nullptr;
};