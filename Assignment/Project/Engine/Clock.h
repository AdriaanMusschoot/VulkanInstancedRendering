#ifndef VK_CLOCK_H
#define VK_CLOCK_H
#include "Engine/Configuration.h"
#include <GLFW/glfw3.h>

namespace ave
{

	template <typename T>
	class Singleton
	{
	public:
		static T& GetInstance()
		{
			static T instance{};
			return instance;
		}

		virtual ~Singleton() = default;
		Singleton(const Singleton& other) = delete;
		Singleton(Singleton&& other) = delete;
		Singleton& operator=(const Singleton& other) = delete;
		Singleton& operator=(Singleton&& other) = delete;

	protected:
		Singleton() = default;
	};

	class Clock final : public Singleton<Clock>
	{
	public:
		void Update();
	
		double GetDeltaTime() const;
		double GetElapsedTime() const;
	private:
		friend class Singleton<Clock>;
		Clock() = default;

		std::chrono::high_resolution_clock::time_point m_PreviousTimePoint;
	
		double m_DeltaTime = 0;
		double m_ElapsedTime = 0;
	};
}


#endif