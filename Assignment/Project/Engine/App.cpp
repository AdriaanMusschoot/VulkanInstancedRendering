#include "App.h"
#include "Clock.h"

ave::App::App(const std::string& windowName, uint32_t width, uint32_t height, bool isDebugging)
	: m_WindowName{ windowName }
	, m_Width{ width }
	, m_Height{ height }
{
	CreateGLFWWindow(isDebugging);

	m_VKEngineUPtr = std::make_unique<VulkanEngine>(m_WindowName, m_Width, m_Height, m_WindowPtr, isDebugging);
}

ave::App::~App()
{
	glfwTerminate();
}

void ave::App::Run()
{
	while (!glfwWindowShouldClose(m_WindowPtr))
	{
		glfwPollEvents();
		ave::Clock::GetInstance().Update();
		CalculateFPS();
		m_VKEngineUPtr->Render();
	}
}

void ave::App::CreateGLFWWindow(bool isDebugging)
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	if (m_WindowPtr = glfwCreateWindow(m_Width, m_Height, m_WindowName.c_str(), nullptr, nullptr); m_WindowPtr != nullptr)
	{
		if (isDebugging)
		{
			std::cout << "Window creation successful\n";
		}
	}
	else
	{
		if (isDebugging)
		{
			std::cout << "Window creation failure\n";
		}
	}

	glfwSetWindowUserPointer(m_WindowPtr, this);
}

void ave::App::CalculateFPS()
{
	double deltaTime{ ave::Clock::GetInstance().GetDeltaTime() };
	m_TimeElapsed += deltaTime;
	if (m_TimeElapsed >= 1.0)
	{
		int frameRate{ std::max(-1, static_cast<int>(m_NumberOfFrames / m_TimeElapsed)) };
		std::stringstream title;
		title << frameRate << " fps";
		glfwSetWindowTitle(m_WindowPtr, title.str().c_str());
		m_TimeElapsed = 0.0;
		m_NumberOfFrames = -1;
	}

	++m_NumberOfFrames;
}
