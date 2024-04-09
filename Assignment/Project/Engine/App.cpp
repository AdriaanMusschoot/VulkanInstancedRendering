#include "App.h"

ave::App::App(const std::string& windowName, uint32_t width, uint32_t height, bool isDebugging)
	: m_WindowName{ windowName }
	, m_Width{ width }
	, m_Height{ height }
	, m_TriangleSceneUPtr{ std::make_unique<Scene>() }
{
	CreateGLFWWindw(isDebugging);

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
		m_VKEngineUPtr->Render(m_TriangleSceneUPtr.get());
		CalculateFPS();
	}
}

void ave::App::CreateGLFWWindw(bool isDebugging)
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
}

void ave::App::CalculateFPS()
{
	m_CurrentTime = glfwGetTime();
	double delta{ m_CurrentTime - m_LastTime };

	if (delta >= 1)
	{
		int frameRate{ std::max(1, static_cast<int>(m_NumberOfFrames / delta)) };
		std::stringstream title;
		title << frameRate << " fps";
		glfwSetWindowTitle(m_WindowPtr, title.str().c_str());
		m_LastTime = m_CurrentTime;
		m_NumberOfFrames = -1;
		m_FrameTime = 1000.0f / frameRate;
	}

	++m_NumberOfFrames;
}
