#pragma once

#include "../iCommonIF/CommonIF-Struct.h"
#include "../iCommonIF/CommonIF-Define.h"
#include "../iODB/iODB.h"

class CLoadLayerJob
{
private:
	UINT32  iStepIdx = 0;
	CString strStepName = L"";
	LayerSet_t stLayerSet;

public:
	void SetData(UINT32 i, CString str, LayerSet_t* p) { iStepIdx = i; strStepName = str; stLayerSet = *p; }
	LayerSet_t* GetLayerSet() { return &stLayerSet; }
	CString GetStepName() { return strStepName; }
	UINT32 GetStepIndex() { return iStepIdx; }
};

class ILoadLayerOp
{
public:
	virtual bool AddJob(CLoadLayerJob stJob) = 0;
	virtual bool RunJob(UINT32 nWorkIdx) = 0;
	virtual bool RunAllJobs() = 0;
	virtual void WaitForJobFinish() = 0;

	virtual void RemoveAllJobs() = 0;
	virtual bool ResetAllJobEvents() = 0;

	virtual bool StartThreads(UINT32 iStepCount, UINT32 iLayerCount, UINT32 iThreadCount) = 0;
	virtual bool StopAllThreads() = 0;
	
	virtual void SetJobFile(CJobFile* pJobFile) = 0;
	virtual CJobFile* GetJobFile() = 0;

	virtual bool GetThreadStatus() = 0;
	virtual UINT32 GetThreadCount() = 0;

	virtual void SetParentInitThread(UINT32 iThreadCnt) = 0;
	virtual void CloseParentEvents() = 0;
	virtual void CreateParentEvents(UINT32 iLayerCount) = 0;
};

class CLoadLayerBase : public ILoadLayerOp
{
protected:
	UINT32 m_iThreadCount = 0;
	UINT32 m_iLayerCount = 0;

	CJobFile* m_pJobFile = nullptr;
	CRITICAL_SECTION m_stJobCritcalSection;
	CRITICAL_SECTION m_stDefectCritcalSection;

public:
	bool m_bThreadStatus = false;
	HANDLE* m_phThreadEvent = nullptr;
	HANDLE* m_phJobComplete = nullptr;
	HANDLE** m_phParentFinish = nullptr;

public:
	void SetJobFile(CJobFile* pJobFile) { m_pJobFile = pJobFile; }
	CJobFile* GetJobFile() { return m_pJobFile; }
};
