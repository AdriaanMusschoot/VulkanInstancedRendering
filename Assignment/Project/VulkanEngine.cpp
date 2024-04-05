#include "VulkanEngine.h"
#include "Instance.h"
#include "Logging.h"
#include "Device.h"

VulkanEngine::VulkanEngine()
{
	if (m_IsDebugging)
	{
		std::cout << "Ladies and gentleman, start your engines\n";
	}

	CreateGLFWWindow();
	CreateInstance();
	CreateDevice();
}

VulkanEngine::~VulkanEngine()
{
	if (m_IsDebugging)
	{
		std::cout << "The engine died out\n";
	}

	m_Device.destroy();

	m_Instance.destroySurfaceKHR(m_Surface);
	if (m_IsDebugging)
	{
		m_Instance.destroyDebugUtilsMessengerEXT(m_DebugMessenger, nullptr, m_DLDInstance);
	}
	m_Instance.destroy();

	glfwTerminate();
}

void VulkanEngine::CreateGLFWWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	if (m_WindowPtr = glfwCreateWindow(m_Width, m_Height, m_WindowName.c_str(), nullptr, nullptr); m_WindowPtr != nullptr)
	{
		if (m_IsDebugging)
		{
			std::cout << "Window creation successful\n";
		}
	}
	else
	{
		if (m_IsDebugging)
		{
			std::cout << "Window creation failure\n";
		}
	}
}

void VulkanEngine::CreateInstance()
{
	m_Instance = vkInit::CreateInstance(m_IsDebugging, m_WindowName);

	m_DLDInstance = vk::DispatchLoaderDynamic{ m_Instance, vkGetInstanceProcAddr };

	if (m_IsDebugging)
	{
		m_DebugMessenger = vkInit::CreateDebugMessenger(m_Instance, m_DLDInstance);
	}

	VkSurfaceKHR oldStyleSurface;
	if (glfwCreateWindowSurface(m_Instance, m_WindowPtr, nullptr, &oldStyleSurface) != VK_SUCCESS)
	{
		if (m_IsDebugging)
		{
			std::cout << "Window surface creation failure\n";
		}
	}
	else if (m_IsDebugging)
	{
		std::cout << "Window surface creation successful\n";
	}
	//copy constructor that takes old surface for the new surface
	m_Surface = oldStyleSurface;
}

void VulkanEngine::CreateDevice()
{
	m_PhysicalDevice = vkInit::ChoosePhysicalDevice(m_Instance, m_IsDebugging);

	m_Device = vkInit::CreateLogicalDevice(m_PhysicalDevice, m_Surface, m_IsDebugging);

	std::array<vk::Queue, 2> queues = vkInit::GetQueuesFromGPU(m_PhysicalDevice, m_Device, m_Surface, m_IsDebugging);
	m_GraphicsQueue = queues[0];
	m_PresentQueue = queues[1];
}