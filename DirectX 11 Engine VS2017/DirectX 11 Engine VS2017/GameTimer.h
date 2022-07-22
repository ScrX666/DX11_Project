
#ifndef GAMETIMER_H
#define GAMETIMER_H
#include <chrono>
class GameTimer
{
public:
	GameTimer();

	float TotalTime()const;		// 总游戏时间
	float DeltaTime()const;		// 帧间隔时间

	void Reset();               // 在消息循环之前调用
	void Start();               // 在取消暂停的时候调用
	void Stop();                // 在暂停的时候调用
	void Tick();                // 在每一帧的时候调用
	double GetMilisecondsElapsed();//获取毫秒数

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
