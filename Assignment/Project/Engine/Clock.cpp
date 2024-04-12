#include "Clock.h"

//double ave::clock::GetDeltaTime()
//{
//	static double deltaTime{};
//	double currentTime{ glfwGetTime() };
//	deltaTime = currentTime - GetLastTime();
//	SetLastTime(currentTime);
//	return deltaTime;
//}
//
//double ave::clock::GetLastTime()
//{
//	static double lastTime{};
//	return lastTime;
//}
//
//void ave::clock::SetLastTime(double currentTime)
//{
//	static double lastTime{};
//	lastTime = currentTime;
//}

void ave::Clock::Update()
{
	const auto currentTimePoint = std::chrono::high_resolution_clock::now();
	m_DeltaTime = std::chrono::duration<double>(currentTimePoint - m_PreviousTimePoint).count();
	m_PreviousTimePoint = currentTimePoint;
}

double ave::Clock::GetDeltaTime() const
{
	return m_DeltaTime;
}
