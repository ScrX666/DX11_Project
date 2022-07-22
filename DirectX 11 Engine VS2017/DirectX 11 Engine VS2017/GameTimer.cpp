#include "GameTimer.h"

#include <windows.h>

GameTimer::GameTimer()
	:
	m_performanceCountPeriod(0.0),
	m_atStart(0),
	m_deltaTime(-1.0f),
	m_accumulationWhenPaused(0),
	m_atStop(0),
	m_last(0),
	m_current(0),
	m_bIsStopped(false)
{
	uint64_t performanceFreq;
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&performanceFreq));
	m_performanceCountPeriod = 1.0 / (double)performanceFreq;

	start = std::chrono::high_resolution_clock::now();
	stop = std::chrono::high_resolution_clock::now();
}

float GameTimer::GetTotalTime() const
{
	if (m_bIsStopped)
	{
		return static_cast<float>((m_atStop - m_atStart - m_accumulationWhenPaused) * m_performanceCountPeriod);
	}

	return  static_cast<float>((m_current - m_atStart - m_accumulationWhenPaused) * m_performanceCountPeriod);
}

float GameTimer::GetDeltaTime() const
{
	return  static_cast<float>(m_deltaTime);
}

bool GameTimer::IsStopped()
{
	return m_bIsStopped;
}

void GameTimer::Reset()
{
	uint64_t now;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&now));
	m_atStart = now;
	m_last = now;
	m_atStop = 0;
	m_bIsStopped = false;
}

void GameTimer::Start()
{
	uint64_t now;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&now));
	if (m_bIsStopped)
	{
		m_accumulationWhenPaused += (now - m_atStop);
		m_atStop = 0;
		m_bIsStopped = false;
	}

	m_last = now;
}

void GameTimer::Stop()
{
	if (!m_bIsStopped) {
		int64_t now;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&now));
		m_atStop = now;
		m_bIsStopped = true;
	}
}

void GameTimer::Tick()
{
	if (m_bIsStopped) 
	{
		m_deltaTime = 0.0;
		return;
	}

	uint64_t now;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&now));
	m_current = now;

	m_deltaTime = (m_current - m_last) * m_performanceCountPeriod;

	m_last = m_current;

	//Force nonnegative,because if the processor goes into a power save mode or we get shuffled to
	//another processor, then mDeltaTime can be negative.
	if (m_deltaTime < 0.0) 
	{
		m_deltaTime = 0.0;
	}
	

}


double GameTimer::GetMilisecondsElapsed()
{
	if (!m_bIsStopped)
	{
		auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - start);
		return elapsed.count();
	}
	else
	{
		auto elapsed = std::chrono::duration<double, std::milli>(stop - start);
		return elapsed.count();
	}
}


