#include "VulkanEngine.h"
#include <iostream>
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
			std::cout << "Window creation succesfull\n";
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
}

void VulkanEngine::CreateDevice()
{
	m_PhysicalDevice = vkInit::ChoosePhysicalDevice(m_Instance, m_IsDebugging);
}
