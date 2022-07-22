#pragma once

#include <stdint.h>


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
};
