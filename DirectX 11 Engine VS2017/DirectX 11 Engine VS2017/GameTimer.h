#pragma once

#include <stdint.h>
#include <chrono>

class GameTimer
{
public:
	GameTimer();
	~GameTimer() = default;

	void Reset();
	void Start();
	void Stop();
	bool IsStopped();

	float GetDeltaTime() const; //In seconds.
	float GetTotalTime() const;
	double GetMilisecondsElapsed();//get milliseconds

	void Tick();
private:
	double m_performanceCountPeriod;    //In seconds.
	double m_deltaTime;
	uint64_t m_atStart;
	uint64_t m_accumulationWhenPaused;
	uint64_t m_atStop;
	uint64_t m_last;
	uint64_t m_current;
	bool m_bIsStopped;

#ifdef _WIN32
	std::chrono::time_point<std::chrono::steady_clock> start;
	std::chrono::time_point<std::chrono::steady_clock> stop;
#else
	std::chrono::time_point<std::chrono::system_clock> start;
	std::chrono::time_point<std::chrono::system_clock> stop;
#endif
};
