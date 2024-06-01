#include "Clock.h"

void ave::Clock::Update()
{
	const auto currentTimePoint = std::chrono::high_resolution_clock::now();
	m_DeltaTime = std::chrono::duration<double>(currentTimePoint - m_PreviousTimePoint).count();
	m_PreviousTimePoint = currentTimePoint;
	m_ElapsedTime += m_DeltaTime;
}

double ave::Clock::GetDeltaTime() const
{
	return m_DeltaTime;
}

double ave::Clock::GetElapsedTime() const
{
	return m_ElapsedTime;
}