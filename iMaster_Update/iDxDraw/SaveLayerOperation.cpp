#include "pch.h"
#include "SaveLayerOperation.h"
#include "../iUtilities/PerfTimerEx.h"

#include "DxDraw/CDirectView.h"

CSaveLayerOperation::CSaveLayerOperation()
{
	m_bThreadStatus = false;
	m_iThreadCount = 0;
	m_iLayerCount = 0;
}

CSaveLayerOperation::~CSaveLayerOperation()
{
	RemoveAllJobs();
}

void CSaveLayerOperation::RemoveAllJobs()
{
	m_lstJob.clear();
	m_bRunAllJob = false;
	m_vecFinished.clear();
}

void CSaveLayerOperation::StopJobThreads()
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

void CSaveLayerOperation::StartJobThreads(UINT32 iLayerCount, UINT32 iThreadCount)
{
	m_phThreadEvent = new HANDLE[iThreadCount];
	m_phJobComplete = new HANDLE[iThreadCount];
	m_pJobThreadAry = new CWinThread*[iThreadCount];

	for (UINT32 iLoop = 0; iLoop < iThreadCount; iLoop++)
	{
		m_phThreadEvent[iLoop] = CreateEventW(nullptr, FALSE, FALSE, nullptr);
		m_phJobComplete[iLoop] = CreateEventW(nullptr, TRUE, FALSE, nullptr);

		auto pPara = new CSaveLayerThreadPara(this, iLoop);
		m_pJobThreadAry[iLoop] = AfxBeginThread(JobThread, pPara, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
		m_pJobThreadAry[iLoop]->m_bAutoDelete = FALSE;
		m_pJobThreadAry[iLoop]->ResumeThread();
	}

	m_iThreadCount = iThreadCount;

	for (UINT32 iLoop = 0; iLoop < iLayerCount; iLoop++)
		m_vecFinished.push_back(true);
}

bool CSaveLayerOperation::AddJob(CSaveLayerJob* stJob)
{
	if (m_pJobFile == nullptr)
		return false;

	EnterCriticalSection(&m_stJobCritcalSection);
	m_lstJob.push_back(stJob);
	LeaveCriticalSection(&m_stJobCritcalSection);

	return true;
}

bool CSaveLayerOperation::RunJob(UINT32 nWorkIdx)
{
	::SetEvent(m_phThreadEvent[nWorkIdx % m_iThreadCount]);
	return true;
}

bool CSaveLayerOperation::RunAllJobs()
{
	m_bRunAllJob = true;

	for (UINT32 iLoop = 0; iLoop < m_iThreadCount; iLoop++)
	{
		::SetEvent(m_phThreadEvent[iLoop]);
	}
	return true;
}

void CSaveLayerOperation::WaitForJobFinish()
{
	if (m_iThreadCount < 1 || !m_bThreadStatus)
		return;

	::WaitForMultipleObjects(m_iThreadCount, m_phJobComplete, TRUE, INFINITE);

	m_bRunAllJob = false;
}

bool CSaveLayerOperation::ResetAllJobEvents()
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

bool CSaveLayerOperation::StartThreads(UINT32 iLayerCount, UINT32 iThreadCount)
{
	if (m_bThreadStatus)
		return false;

	m_bRunAllJob = false;
	m_bThreadStatus = true;

	m_iLayerCount = iLayerCount;

	InitializeCriticalSection(&m_stJobCritcalSection);

	if (iThreadCount > m_iLayerCount)
		iThreadCount = m_iLayerCount;

	StartJobThreads(m_iLayerCount, iThreadCount);
	return true;
}

bool CSaveLayerOperation::StopAllThreads()
{
	m_bThreadStatus = false;
	m_bRunAllJob = false;

	if (m_iThreadCount < 1)
		return true;

	StopJobThreads();

	m_iThreadCount = 0;
	m_iLayerCount = 0;

	m_pJobFile = nullptr;

	DeleteCriticalSection(&m_stJobCritcalSection);

	return true;
}

UINT CSaveLayerOperation::JobThread(LPVOID Param)
{
	auto pPara = reinterpret_cast<CSaveLayerThreadPara*>(Param);
	CSaveLayerOperation* pOperation = pPara->GetInitOperation();
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
		auto eJobMode = stJob->GetJobMode();

		switch (eJobMode)
		{
		case JobMode::SaveLayer :
			pOperation->_SaveLayer();
			break;
		case JobMode::ConcatRegion:
			pOperation->_ConcatRegion();
			break;
		case JobMode::GetFilteredFeature:
			pOperation->_GetFilteredFeature();
			break;
		case JobMode::GetFilteredFeature_V3:
		{

			auto stJob = pOperation->m_lstJob.front();
			pOperation->m_lstJob.pop_front();
			LeaveCriticalSection(&pOperation->m_stJobCritcalSection);

			pOperation->_GetFilteredFeature_V3(stJob);
		}
			break;
		case JobMode::SaveFilteredFeature:
			pOperation->_SaveFilteredFeature();
			break;
		default:

			LeaveCriticalSection(&pOperation->m_stJobCritcalSection);
			continue;

			break;
		}
		

//		::WaitForSingleObject(pSaveLayer->m_hFinishEvent, INFINITE);
//		pSaveLayer->m_hFinishEvent.ResetEvent();

		if (stJob) stJob->Clear();
		delete stJob;
		stJob = nullptr;		
	}

	return true;
}

UINT32 CSaveLayerOperation::_SaveLayer()
{
	auto stJob = m_lstJob.front();

	auto strFileName = stJob->GetFileName();
	auto iLayerIndex = stJob->GetLayerIdx();
	auto iThreadIndex = stJob->GetThreadIdx();
	auto* pLayerSet = stJob->GetLayerSet();
	auto frtSwath = stJob->GetSwathRect();
	auto* pSaveLayer = stJob->GetSaveLayer();
	auto dResolution = stJob->GetResolution();
	auto strFilePath = stJob->GetFilePath();
	auto strImageType = stJob->GetImageType();
	auto* pStep = stJob->GetStep();
	auto* pRotMirrArr = stJob->GetRotMirrArr();
	auto* pUserSetInfo = stJob->GetUserSetInfo();
	auto eSaveMode = stJob->GetSaveMode();
	auto eOpMode = stJob->GetOpMode();
	auto eSaveRaw = stJob->IsSaveRaw();
	auto dAngle = stJob->GetAngle();
	auto bMirror = stJob->GetMirror();
	auto strDstPath = stJob->GetDestPath();
	auto bExceptDummy = stJob->IsExceptDummy();
	auto strLineArcWidth = stJob->GetLineArcWidth();
	auto nWidthIndex = stJob->GetWidthIndex();

	if (!m_vecFinished[iThreadIndex])
	{
		LeaveCriticalSection(&m_stJobCritcalSection);
		return RESULT_BAD;
	}

	m_vecFinished[iThreadIndex] = false;

	m_lstJob.pop_front();
	LeaveCriticalSection(&m_stJobCritcalSection);

	auto* pJobFile = (CJobFile*)GetJobFile();

	int nSaveMode = static_cast<int>(eSaveMode);

	//[KJLEE] 12480 x 12800 HDevelop Test
	// 1) TIF : file size 6.10MB, read_image() 2.30sec
	// 2) PNG : file size 1.37MB, read_image() 1.48sec
	// PNG가 TIF 대비 메모리 22.5% Save, 리딩 시간 35.7% 감소

	CString strFullPathName = _T("");
	if (strFileName.GetLength() < 1)
		strFullPathName.Format(_T("%s\\%s%s%s.%s"), strFilePath, pLayerSet->strLayer, strFileName, SaveMode_Name::strName[nSaveMode], strImageType);
	else if (eSaveMode == SaveMode::SaveMode_Line_Pos_Sub || eSaveMode == SaveMode::SaveMode_Line_Neg_Sub ||
		eSaveMode == SaveMode::SaveMode_Arc_Pos_Sub || eSaveMode == SaveMode::SaveMode_Arc_Neg_Sub ||
		eSaveMode == SaveMode::SaveMode_LineArc_Pos_Sub || eSaveMode == SaveMode::SaveMode_LineArc_Neg_Sub)
		strFullPathName.Format(_T("%s\\%s_%s%s_%d.%s"), strFilePath, pLayerSet->strLayer, strFileName, SaveMode_Name::strName[nSaveMode], nWidthIndex, strImageType);
	else
		strFullPathName.Format(_T("%s\\%s_%s%s.%s"), strFilePath, pLayerSet->strLayer, strFileName, SaveMode_Name::strName[nSaveMode], strImageType);


	CPerfTimerEx clTimer;
	CString strLog;

	clTimer.StartTimer();

	if (RESULT_GOOD != pSaveLayer->SetPictureBox(frtSwath, dResolution, FALSE, &m_Mutex))
	{
		EnterCriticalSection(&m_stJobCritcalSection);
		m_vecFinished[iThreadIndex] = true;
		LeaveCriticalSection(&m_stJobCritcalSection);

		if (stJob) stJob->Clear();
		delete stJob;
		stJob = nullptr;

		return RESULT_BAD;
	}

	//		pSaveLayer->m_hFinishEvent.ResetEvent();

	switch (eOpMode)
	{
	case OpMode::MakeLayer:
	{
		BOOL bSaveRegion = TRUE;
		if (pUserSetInfo->mcType == eMachineType::eAOI)
		{
			bSaveRegion = TRUE;
		}
		else
		{
			bSaveRegion = FALSE;
		}

		if (dAngle != UNKNOWN_ANGLE)
			pSaveLayer->SaveImage(strFullPathName, 0., pStep, pLayerSet, &pJobFile->m_arrFont, pRotMirrArr, pUserSetInfo, bSaveRegion, eSaveMode, bExceptDummy, strLineArcWidth, dAngle, bMirror);
		else
			pSaveLayer->SaveImage(strFullPathName, 0., pStep, pLayerSet, &pJobFile->m_arrFont, pRotMirrArr, pUserSetInfo, bSaveRegion, eSaveMode, bExceptDummy, strLineArcWidth);

		double dTime = clTimer.StopTimer(CTimerUnit_en::EN_MIL_SEC);
		strLog.Format(_T("\n[MakeLayer] >>>> %s : %s, Processing Time[%.2f ms]"), pLayerSet->strLayer, strFileName, dTime);
		OutputDebugString(strLog);
	}
	break;
	case OpMode::MergeSwath:
	{
		pSaveLayer->_CreateMergeImage(strFullPathName, eSaveRaw);

		double dTime = clTimer.StopTimer(CTimerUnit_en::EN_MIL_SEC);
		strLog.Format(_T("\n[MergeImage] >>>> %s : %s, Processing Time[%.2f ms]"), pLayerSet->strLayer, strFileName, dTime);
		OutputDebugString(strLog);
	}
	break;
	case OpMode::MakeRegion:
	{
		((CSaveLayer_AOI*)(pSaveLayer))->SaveMasterRegion(strFullPathName, 0, pStep, pLayerSet, &pJobFile->m_arrFont, pRotMirrArr, TRUE, pUserSetInfo, eSaveMode);

		double dTime = clTimer.StopTimer(CTimerUnit_en::EN_MIL_SEC);
		strLog.Format(_T("\n[MakeRegion] >>>> %s : %s, Processing Time[%.2f ms]"), pLayerSet->strLayer, strFileName, dTime);
		OutputDebugString(strLog);
	}
	break;
	case OpMode::ConvRegion:
	{
		((CSaveLayer_AOI*)(pSaveLayer))->ConvRegion(strFullPathName, strDstPath);

		double dTime = clTimer.StopTimer(CTimerUnit_en::EN_MIL_SEC);
		strLog.Format(_T("\n[ConvRegion] >>>> %s : %s, Processing Time[%.2f ms]"), pLayerSet->strLayer, strFileName, dTime);
		OutputDebugString(strLog);
	}
	break;
	case OpMode::MakeSplitLayer_Merge:
	{
		if (dAngle != UNKNOWN_ANGLE)
			pSaveLayer->SaveImage(strFullPathName, 0., pStep, pLayerSet, &pJobFile->m_arrFont, pRotMirrArr, pUserSetInfo, TRUE, eSaveMode, bExceptDummy, _T(""), dAngle, bMirror);
		else
			pSaveLayer->SaveImage(strFullPathName, 0., pStep, pLayerSet, &pJobFile->m_arrFont, pRotMirrArr, pUserSetInfo, TRUE, eSaveMode, bExceptDummy, _T(""));

		double dTime = clTimer.StopTimer(CTimerUnit_en::EN_MIL_SEC);
		strLog.Format(_T("\n[MakeSplitLayer_Merge][MakeSplitLayer] >>>> %s : %s, Processing Time[%.2f ms]"), pLayerSet->strLayer, strFileName, dTime);
		OutputDebugString(strLog);

		clTimer.StartTimer();
		pSaveLayer->_CreateMergeImage(strFullPathName, eSaveRaw);

		dTime = clTimer.StopTimer(CTimerUnit_en::EN_MIL_SEC);
		strLog.Format(_T("\n[MakeSplitLayer_Merge][Merge] >>>> %s : %s, Processing Time[%.2f ms]"), pLayerSet->strLayer, strFileName, dTime);
		OutputDebugString(strLog);
	}
	break;
	}

	EnterCriticalSection(&m_stJobCritcalSection);
	m_vecFinished[iThreadIndex] = true;
	LeaveCriticalSection(&m_stJobCritcalSection);

	return RESULT_GOOD;
}

UINT32 CSaveLayerOperation::_ConcatRegion()
{
	auto stJob = m_lstJob.front();

	auto iThreadIndex = stJob->GetThreadIdx();
	auto* pSaveLayer = stJob->GetSaveLayer();
	
	auto vecSrcPathName = stJob->GetSrcPathName_Vector();
	auto strDstPathName = stJob->GetDestPath();

	if (!m_vecFinished[iThreadIndex])
	{
		LeaveCriticalSection(&m_stJobCritcalSection);
		return RESULT_BAD;
	}

	m_vecFinished[iThreadIndex] = false;

	m_lstJob.pop_front();
	LeaveCriticalSection(&m_stJobCritcalSection);

	auto* pJobFile = (CJobFile*)GetJobFile();

	CPerfTimerEx clTimer;
	CString strLog;

	clTimer.StartTimer();

	((CSaveLayer_AOI*)pSaveLayer)->CreateConcatRegion(vecSrcPathName, strDstPathName);

	double dTime = clTimer.StopTimer(CTimerUnit_en::EN_MIL_SEC);
	strLog.Format(_T("\n[ConcatRegion] >>>> %s Processing Time[%.2f ms]"), strDstPathName, dTime);
	OutputDebugString(strLog);


	EnterCriticalSection(&m_stJobCritcalSection);
	m_vecFinished[iThreadIndex] = true;
	LeaveCriticalSection(&m_stJobCritcalSection);

	return RESULT_GOOD;
}

UINT32 CSaveLayerOperation::_GetFilteredFeature()
{
	auto stJob = m_lstJob.front();

	//
	auto iThreadIndex = stJob->GetThreadIdx();

	auto pDirectView = stJob->GetDirectView();

	auto *vecvecFilterdFeature = stJob->GetFilteredFeature_vecvec();
	auto pStep = stJob->GetStep();
	auto pLayerSet = stJob->GetLayerSet();
	auto pRotMirrArr = stJob->GetRotMirrArr();

	auto eLineOption = stJob->GetLineOption();
	auto *vecLineWidth = stJob->GetLineWidth();
	auto drtView_mm = stJob->GetView_mm();
	//

	if (!m_vecFinished[iThreadIndex])
	{
		LeaveCriticalSection(&m_stJobCritcalSection);
		return RESULT_BAD;
	}

	m_vecFinished[iThreadIndex] = false;

	m_lstJob.pop_front();
	LeaveCriticalSection(&m_stJobCritcalSection);

	pDirectView->GetFilteredFeature(*vecvecFilterdFeature, pStep, pLayerSet, pRotMirrArr, eLineOption, *vecLineWidth, drtView_mm);

	EnterCriticalSection(&m_stJobCritcalSection);
	m_vecFinished[iThreadIndex] = true;
	LeaveCriticalSection(&m_stJobCritcalSection);

	return RESULT_GOOD;
}


UINT32 CSaveLayerOperation::_GetFilteredFeature_V3(CSaveLayerJob *stJob)
{
	
	//
	auto iThreadIndex = stJob->GetThreadIdx();

	auto pDirectView = stJob->GetDirectView();

	auto *vecvecvecFilterdFeature = stJob->GetFilteredFeature_V3();
	auto pStep = stJob->GetStep();
	auto pLayerSet = stJob->GetLayerSet();
	auto pRotMirrArr = stJob->GetRotMirrArr();

	auto *vecvecLineWidth = stJob->GetLineWidth_V3();
	auto drtView_mm = stJob->GetView_mm();
	//

	if (!m_vecFinished[iThreadIndex])
	{
		return RESULT_BAD;
	}

	m_vecFinished[iThreadIndex] = false;

	

	pDirectView->GetFilteredFeature_V3(*vecvecvecFilterdFeature, pStep, pLayerSet, pRotMirrArr, *vecvecLineWidth, drtView_mm);

	//EnterCriticalSection(&m_stJobCritcalSection);
	m_vecFinished[iThreadIndex] = true;
	//LeaveCriticalSection(&m_stJobCritcalSection);

	return RESULT_GOOD;
}


UINT32 CSaveLayerOperation::_SaveFilteredFeature()
{
	auto stJob = m_lstJob.front();

	//
	auto iThreadIndex = stJob->GetThreadIdx();
	auto* pSaveLayer = stJob->GetSaveLayer();
	
	auto eMcType = stJob->GetMcType();
	auto pLayerSet = stJob->GetLayerSet();
	auto pUserSetInfo = stJob->GetUserSetInfo();
	auto strFilePath = stJob->GetFilePath();
	auto strFileName = stJob->GetFileName();
	auto eSaveMode = stJob->GetSaveMode();
	auto strImageType = stJob->GetImageType();
	auto nWidthIndex = stJob->GetWidthIndex();

	auto frtSwath = stJob->GetSwathRect();
	auto dResolution = stJob->GetResolution();

	auto vecFilteredFeature = stJob->GetFilteredFeature();
	auto strDstPathName = stJob->GetDestPath();

	//
	if (!m_vecFinished[iThreadIndex])
	{
		LeaveCriticalSection(&m_stJobCritcalSection);
		return RESULT_BAD;
	}

	m_vecFinished[iThreadIndex] = false;

	m_lstJob.pop_front();
	LeaveCriticalSection(&m_stJobCritcalSection);

	if (vecFilteredFeature.size() > 0)
	{

		int nSaveMode = static_cast<int>(eSaveMode);

		if (RESULT_GOOD != pSaveLayer->SetPictureBox(frtSwath, dResolution, FALSE, &m_Mutex))
		{
			EnterCriticalSection(&m_stJobCritcalSection);
			m_vecFinished[iThreadIndex] = true;
			LeaveCriticalSection(&m_stJobCritcalSection);

			if (stJob) stJob->Clear();
			delete stJob;
			stJob = nullptr;

			return RESULT_BAD;
		}

		CString strFullPathName = _T("");
		if (strFileName.GetLength() < 1)
			strFullPathName.Format(_T("%s\\%s%s%s.%s"), strFilePath, pLayerSet->strLayer, strFileName, SaveMode_Name::strName[nSaveMode], strImageType);
		else if (eSaveMode == SaveMode::SaveMode_Line_Pos_Sub || eSaveMode == SaveMode::SaveMode_Line_Neg_Sub ||
			eSaveMode == SaveMode::SaveMode_Arc_Pos_Sub || eSaveMode == SaveMode::SaveMode_Arc_Neg_Sub ||
			eSaveMode == SaveMode::SaveMode_LineArc_Pos_Sub || eSaveMode == SaveMode::SaveMode_LineArc_Neg_Sub)
			strFullPathName.Format(_T("%s\\%s_%s%s_%d.%s"), strFilePath, pLayerSet->strLayer, strFileName, SaveMode_Name::strName[nSaveMode], nWidthIndex, strImageType);
		else
			strFullPathName.Format(_T("%s\\%s_%s%s.%s"), strFilePath, pLayerSet->strLayer, strFileName, SaveMode_Name::strName[nSaveMode], strImageType);

		CPerfTimerEx clTimer;
		CString strLog;
		clTimer.StartTimer();

		pSaveLayer->SaveImage_FilteredFeature(strFullPathName, vecFilteredFeature, pLayerSet, nullptr, nullptr, pUserSetInfo);

		double dTime = clTimer.StopTimer(CTimerUnit_en::EN_MIL_SEC);
		strLog.Format(_T("\n[SaveFilteredFeature] >>>> %s Processing Time[%.2f ms]"), strDstPathName, dTime);
		OutputDebugString(strLog);
	}


	EnterCriticalSection(&m_stJobCritcalSection);
	m_vecFinished[iThreadIndex] = true;
	LeaveCriticalSection(&m_stJobCritcalSection);

	return RESULT_GOOD;
}