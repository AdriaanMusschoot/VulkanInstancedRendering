#ifndef VK_AVE_APP
#define VK_AVE_APP
#include "Configuration.h"
#include <GLFW/glfw3.h>
#include "VulkanEngine.h"
#include "Rendering/Scene.h"

namespace ave
{

	class App final
	{
	public:
		App(const std::string& windowName, uint32_t width, uint32_t height, bool isDebugging);
		~App();

		App(const App& other) = delete;
		App(App&& other) = delete;
		App& operator=(const App& other) = delete;
		App& operator=(App&& other) = delete;

		void Run();
	private:
		std::unique_ptr<VulkanEngine> m_VKEngineUPtr;

		const std::string m_WindowName{ "GP2 Assignment" };
		const uint32_t m_Width{ 690 };
		const uint32_t m_Height{ 480 };
		//cannot have unique ptr because incomplete typedefined struct
		GLFWwindow* m_WindowPtr{ nullptr };	

		double m_TimeElapsed{ 0 };
		int m_NumberOfFrames{ 0 };

		void CreateGLFWWindow(bool isDebugging);

		void CalculateFPS();
	};

}

#endif