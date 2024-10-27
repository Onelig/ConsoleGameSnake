#pragma once
#include <chrono>

class Timer
{
private:
	std::chrono::time_point<std::chrono::steady_clock> m_StartTime;

public:
	void Start();
	float GetDuration() const;
};