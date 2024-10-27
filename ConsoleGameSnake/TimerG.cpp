#include "TimerG.h"

void Timer::Start()
{
	m_StartTime = std::chrono::high_resolution_clock::now();
}

float Timer::GetDuration() const
{
	std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - m_StartTime;
	return static_cast<int>(duration.count());
}