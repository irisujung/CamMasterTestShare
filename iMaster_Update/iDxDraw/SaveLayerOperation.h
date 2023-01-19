#pragma once

#include "SaveLayerInit.h"
#include <list>

class CSaveLayerOperation : public CSaveLayerBase
{
private:
	bool m_bRunAllJob;
	std::list <CSaveLayerJob*> m_lstJob;
	CWinThread** m_pJobThreadAry = nullptr;
	std::vector<bool> m_vecFinished;

	void StopJobThreads();
	void StartJobThreads(UINT32 iLayerCount, UINT32 iThreadCount);

public:
	CSaveLayerOperation();
	virtual ~CSaveLayerOperation();

	bool AddJob(CSaveLayerJob* stJob) override;
	bool RunJob(UINT32 nWorkIdx) override;
	bool RunAllJobs() override;
	void WaitForJobFinish() override;

	void RemoveAllJobs() override;
	bool ResetAllJobEvents() override;

	bool StartThreads(UINT32 iLayerCount, UINT32 iThreadCount) override;
	bool StopAllThreads() override;

	bool GetThreadStatus() override { return m_bThreadStatus; }
	UINT32 GetThreadCount() override { return m_iThreadCount; }

	void CloseParentEvents() override { }
	void CreateParentEvents(UINT32 iLayerCount) override { }
	void SetParentInitThread(UINT32 iThreadCnt) override { m_iThreadCount = iThreadCnt; }
	
public:
	static UINT JobThread(LPVOID Param);

private :
	UINT32 _SaveLayer();
	UINT32 _ConcatRegion();
	UINT32 _GetFilteredFeature();
	UINT32 _GetFilteredFeature_V3(CSaveLayerJob *stJob);
	UINT32 _SaveFilteredFeature();
};



class CSaveLayerThreadPara
{
public:
	CSaveLayerThreadPara(CSaveLayerOperation* pOp, UINT32 iThreadID)
		:m_pSaveLayerOperation(pOp), m_iThreadID(iThreadID)
	{
	}
	UINT32 GetThreadID() { return m_iThreadID; }
	CSaveLayerOperation* GetInitOperation() { return m_pSaveLayerOperation; }

private:
	CSaveLayerOperation* m_pSaveLayerOperation = nullptr;
	UINT32 m_iThreadID = 0;
};
