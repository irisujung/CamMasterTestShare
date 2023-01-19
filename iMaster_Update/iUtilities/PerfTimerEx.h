#pragma once

#include "afxwin.h"
#include <chrono>

enum class CTimerUnit_en
{
	EN_SECONDS,
	EN_MIL_SEC,
	M_MIC_SEC,
	M_NANO_SEC,
};

class __declspec(dllexport) CPerfTimerEx
{
private:
	std::chrono::time_point<std::chrono::system_clock> m_tmStart;
	std::chrono::time_point<std::chrono::system_clock> m_tmEnd;
	bool m_bStarted = false;
	bool m_bRunComplete = false;
public:
	CPerfTimerEx();
	~CPerfTimerEx();
	void StartTimer();
	double StopTimer(CTimerUnit_en enTimeUnit);
	double GetElapsedTime(CTimerUnit_en enTimeUnit);
};