#include "pch.h"
#include "framework.h"
#include "PerfTimerEx.h"

CPerfTimerEx::CPerfTimerEx()
{
}

CPerfTimerEx::~CPerfTimerEx()
{
}

void CPerfTimerEx::StartTimer()
{
	m_bStarted = true;
	m_bRunComplete = false;
	m_tmStart = std::chrono::system_clock::now();
}

double CPerfTimerEx::StopTimer(CTimerUnit_en enTimeUnit)
{
	if (!m_bStarted) return 0.0;

	m_tmEnd = std::chrono::system_clock::now();
	m_bRunComplete = true;
	m_bStarted = false;

	return (GetElapsedTime(enTimeUnit));
}

double CPerfTimerEx::GetElapsedTime(CTimerUnit_en enTimeUnit)
{
	if (!m_bRunComplete) return 0.0;

	switch (enTimeUnit)
	{
	case CTimerUnit_en::EN_MIL_SEC:
		return (double)(std::chrono::duration_cast<std::chrono::milliseconds>(m_tmEnd - m_tmStart).count());
		break;

	case CTimerUnit_en::EN_SECONDS:
		return (double)(std::chrono::duration_cast<std::chrono::seconds>(m_tmEnd - m_tmStart).count());
		break;

	case CTimerUnit_en::M_MIC_SEC:
		return (double)(std::chrono::duration_cast<std::chrono::microseconds>(m_tmEnd - m_tmStart).count());
		break;

	case CTimerUnit_en::M_NANO_SEC:
		return (double)(std::chrono::duration_cast<std::chrono::nanoseconds>(m_tmEnd - m_tmStart).count());
		break;
	}
	return 0.0;
}