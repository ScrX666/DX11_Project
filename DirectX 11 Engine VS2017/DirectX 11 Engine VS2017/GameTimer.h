
#ifndef GAMETIMER_H
#define GAMETIMER_H
#include <chrono>
class GameTimer
{
public:
	GameTimer();

	float TotalTime()const;		// ����Ϸʱ��
	float DeltaTime()const;		// ֡���ʱ��

	void Reset();               // ����Ϣѭ��֮ǰ����
	void Start();               // ��ȡ����ͣ��ʱ�����
	void Stop();                // ����ͣ��ʱ�����
	void Tick();                // ��ÿһ֡��ʱ�����
	double GetMilisecondsElapsed();//��ȡ������

private:
	double m_SecondsPerCount;
	double m_DeltaTime;

	__int64 m_BaseTime;
	__int64 m_PausedTime;
	__int64 m_StopTime;
	__int64 m_PrevTime;
	__int64 m_CurrTime;
	bool isrunning = false;
	bool m_Stopped;
#ifdef _WIN32
	std::chrono::time_point<std::chrono::steady_clock> start;
	std::chrono::time_point<std::chrono::steady_clock> stop;
#else
	std::chrono::time_point<std::chrono::system_clock> start;
	std::chrono::time_point<std::chrono::system_clock> stop;
#endif

};

#endif // GAMETIMER_H
