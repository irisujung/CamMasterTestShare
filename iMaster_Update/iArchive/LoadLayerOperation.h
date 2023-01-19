#pragma once
#include "LoadLayerInit.h"
#include <list>

class CLoadLayerOperation : public CLoadLayerBase
{
private:
	bool m_bRunAllJob;
	std::list <CLoadLayerJob> m_lstJob;
	CWinThread** m_pJobThreadAry = nullptr;
	std::vector<bool> m_vecFinished;

	void StopJobThreads();
	void StartJobThreads(UINT32 iLayerCount, UINT32 iThreadCount);

public:
	CLoadLayerOperation();
	virtual ~CLoadLayerOperation();

public:
	bool AddJob(CLoadLayerJob stJob) override;
	bool RunJob(UINT32 nWorkIdx) override;
	bool RunAllJobs() override;
	void WaitForJobFinish() override;

	void RemoveAllJobs() override;
	bool ResetAllJobEvents() override;

	bool StartThreads(UINT32 iStepCount, UINT32 iLayerCount, UINT32 iThreadCount) override;
	bool StopAllThreads() override;

	bool GetThreadStatus() override { return m_bThreadStatus; }
	UINT32 GetThreadCount() override { return m_iThreadCount; }

	void CloseParentEvents() override { }
	void CreateParentEvents(UINT32 iLayerCount) override { }
	void SetParentInitThread(UINT32 iThreadCnt) override { m_iThreadCount = iThreadCnt; }

public:
	//static
	static UINT32 LoadCamThread(LPVOID Param);
};

class CInitThreadPara
{
public:
	CInitThreadPara(CLoadLayerOperation* pLoadLayerOperation, UINT32 iThreadID)
		:m_pLoadLayerOperation(pLoadLayerOperation), m_iThreadID(iThreadID)
	{
	}
	UINT32 GetThreadID() { return m_iThreadID; }
	CLoadLayerOperation* GetInitOperation() { return m_pLoadLayerOperation; }

private:
	CLoadLayerOperation* m_pLoadLayerOperation = nullptr;
	UINT32 m_iThreadID = 0;
};