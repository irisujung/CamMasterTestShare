#include "pch.h"
#include "LoadLayerOperation.h"

CLoadLayerOperation::CLoadLayerOperation()
{
	m_bThreadStatus = false;
	m_iThreadCount = 0;
	m_iLayerCount = 0;
}

CLoadLayerOperation::~CLoadLayerOperation()
{
	RemoveAllJobs();
}

void CLoadLayerOperation::StopJobThreads()
{
	EnterCriticalSection(&m_stJobCritcalSection);
	m_lstJob.clear();
	LeaveCriticalSection(&m_stJobCritcalSection);

	for (UINT32 iLoop = 0; iLoop < m_iThreadCount; iLoop++)
	{
		::SetEvent(m_phThreadEvent[iLoop]);

		::WaitForSingleObject(m_pJobThreadAry[iLoop]->m_hThread, INFINITE);
		delete m_pJobThreadAry[iLoop];

		CloseHandle(m_phJobComplete[iLoop]);
		CloseHandle(m_phThreadEvent[iLoop]);
	}

	delete[]m_phJobComplete;
	m_phJobComplete = nullptr;
	delete[] m_pJobThreadAry;
	m_pJobThreadAry = nullptr;
	delete[] m_phThreadEvent;
	m_phThreadEvent = nullptr;

	m_vecFinished.clear();
}

void CLoadLayerOperation::StartJobThreads(UINT32 iLayerCount, UINT32 iThreadCount)
{
	m_phThreadEvent = new HANDLE[iThreadCount];
	m_phJobComplete = new HANDLE[iThreadCount];
	m_pJobThreadAry = new CWinThread*[iThreadCount];

	for (UINT32 iLoop = 0; iLoop < iThreadCount; iLoop++)
	{
		m_phThreadEvent[iLoop] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		m_phJobComplete[iLoop] = CreateEvent(nullptr, TRUE, FALSE, nullptr);

		auto pPara = new CInitThreadPara(this, iLoop);
		m_pJobThreadAry[iLoop] = AfxBeginThread(LoadCamThread, pPara, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
		m_pJobThreadAry[iLoop]->m_bAutoDelete = FALSE;
		m_pJobThreadAry[iLoop]->ResumeThread();
	}

	m_iThreadCount = iThreadCount;

	for (UINT32 iLoop = 0; iLoop < iLayerCount; iLoop++)
		m_vecFinished.push_back(true);
}

bool CLoadLayerOperation::AddJob(CLoadLayerJob stJob)
{
	if (m_pJobFile == nullptr)
		return false;

	EnterCriticalSection(&m_stJobCritcalSection);
	m_lstJob.push_back(stJob);
	LeaveCriticalSection(&m_stJobCritcalSection);

	return true;
}

bool CLoadLayerOperation::RunJob(UINT32 nWorkIdx)
{
	::SetEvent(m_phThreadEvent[nWorkIdx % m_iThreadCount]);
	return true;
}

bool CLoadLayerOperation::RunAllJobs()
{
	m_bRunAllJob = true;

	for (UINT32 iLoop = 0; iLoop < m_iThreadCount; iLoop++)
	{
		::SetEvent(m_phThreadEvent[iLoop]);
	}
	return true;
}

void CLoadLayerOperation::WaitForJobFinish()
{
	if (m_iThreadCount < 1 || !m_bThreadStatus)
		return;

	::WaitForMultipleObjects(m_iThreadCount, m_phJobComplete, TRUE, INFINITE);

	m_bRunAllJob = false;
}

void CLoadLayerOperation::RemoveAllJobs()
{
	m_lstJob.clear();
	m_bRunAllJob = false;
	m_vecFinished.clear();
}

bool CLoadLayerOperation::ResetAllJobEvents()
{
	m_bRunAllJob = false;

	if (m_iThreadCount < 1)
		return false;

	if (m_phJobComplete == nullptr)
		return false;

	for (UINT32 iLoop = 0; iLoop < m_iThreadCount; iLoop++)
	{
		::ResetEvent(m_phJobComplete[iLoop]);
	}
	return true;
}

bool CLoadLayerOperation::StartThreads(UINT32 iStepCount, UINT32 iLayerCount, UINT32 iThreadCount)
{
	if (m_bThreadStatus)
		return false;

	m_bRunAllJob = false;
	m_bThreadStatus = true;

	m_iLayerCount = iLayerCount * iStepCount;

	InitializeCriticalSection(&m_stJobCritcalSection);
	InitializeCriticalSection(&m_stDefectCritcalSection);

	if (iThreadCount > m_iLayerCount)
		iThreadCount = m_iLayerCount;

	StartJobThreads(m_iLayerCount, iThreadCount);
	return true;
}

bool CLoadLayerOperation::StopAllThreads()
{
	m_bThreadStatus = false;
	m_bRunAllJob = false;

	if (m_iThreadCount < 1)
		return false;

	StopJobThreads();

	m_iThreadCount = 0;
	m_iLayerCount = 0;

	m_pJobFile = nullptr;

	DeleteCriticalSection(&m_stJobCritcalSection);
	DeleteCriticalSection(&m_stDefectCritcalSection);

	return true;
}

UINT32 CLoadLayerOperation::LoadCamThread(LPVOID Param)
{
	auto pPara = reinterpret_cast<CInitThreadPara*>(Param);
	CLoadLayerOperation* pOperation = pPara->GetInitOperation();
	UINT32 iThreadId = pPara->GetThreadID();
	delete pPara;

	POSITION stPos = nullptr;
	bool bRun = false;

	while (pOperation->m_bThreadStatus)
	{
		if (!bRun)
		{
			::WaitForSingleObject(pOperation->m_phThreadEvent[iThreadId], INFINITE);
			bRun = true;
		}
		ResetEvent(pOperation->m_phJobComplete[iThreadId]);

		EnterCriticalSection(&pOperation->m_stJobCritcalSection);
		if (pOperation->m_lstJob.empty())
		{
			if (bRun)
			{
				bRun = false;
				::SetEvent(pOperation->m_phJobComplete[iThreadId]);
				ResetEvent(pOperation->m_phThreadEvent[iThreadId]);
			}
			LeaveCriticalSection(&pOperation->m_stJobCritcalSection);
			continue;
		}
		auto stJob = pOperation->m_lstJob.front();
		LayerSet_t* pData = stJob.GetLayerSet();
		CString strStepName = stJob.GetStepName();
		UINT32 iStepIndex = stJob.GetStepIndex();

		if (!pOperation->m_vecFinished[iThreadId])
		{
			LeaveCriticalSection(&pOperation->m_stJobCritcalSection);
			continue;
		}

		pOperation->m_vecFinished[iThreadId] = false;

		pOperation->m_lstJob.pop_front();
		LeaveCriticalSection(&pOperation->m_stJobCritcalSection);

		auto* pJobFile = (CJobFile*)pOperation->GetJobFile();
		pJobFile->LoadLayer(iStepIndex, strStepName, pData);

		EnterCriticalSection(&pOperation->m_stJobCritcalSection);
		pOperation->m_vecFinished[iThreadId] = true;
		LeaveCriticalSection(&pOperation->m_stJobCritcalSection);
	}
	return true;
}