#include "VulkanEngine.h"
#include "Instance.h"
#include "Logging.h"
#include "Device.h"
#include "Swapchain.h"
#include "Pipeline.h"
ave::VulkanEngine::VulkanEngine()
{
	if (m_IsDebugging)
	{
		std::cout << "Ladies and gentleman, start your engines\n";
	}

	CreateGLFWWindow();
	CreateInstance();
	CreateDevice();
	CreatePipeline();
}

ave::VulkanEngine::~VulkanEngine()
{
	for (auto& frame : m_SwapchainFrameVec)
	{
		m_Device.destroyImageView(frame.ImageView);
	}
	m_Device.destroySwapchainKHR(m_Swapchain);
	m_Device.destroy();

	m_Instance.destroySurfaceKHR(m_Surface);
	if (m_IsDebugging)
	{
		m_Instance.destroyDebugUtilsMessengerEXT(m_DebugMessenger, nullptr, m_DLDInstance);
	}
	m_Instance.destroy();

	glfwTerminate();

	if (m_IsDebugging)
	{
		std::cout << "The engine died out\n";
	}
}

void ave::VulkanEngine::CreateGLFWWindow()
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

void ave::VulkanEngine::CreateInstance()
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

void ave::VulkanEngine::CreateDevice()
{
	m_PhysicalDevice = vkInit::ChoosePhysicalDevice(m_Instance, m_IsDebugging);

	m_Device = vkInit::CreateLogicalDevice(m_PhysicalDevice, m_Surface, m_IsDebugging);

	std::array<vk::Queue, 2> queues = vkInit::GetQueuesFromGPU(m_PhysicalDevice, m_Device, m_Surface, m_IsDebugging);
	m_GraphicsQueue = queues[0];
	m_PresentQueue = queues[1];

	vkInit::SwapchainBundle tempBunlde = vkInit::CreateSwapchain(m_PhysicalDevice, m_Device, m_Surface, m_Width, m_Height, m_IsDebugging);
	m_Swapchain = tempBunlde.Swapchain;
	m_SwapchainFrameVec = tempBunlde.FrameVec;
	m_SwapchainExtent = tempBunlde.Extent;
	m_SwapchainFormat = tempBunlde.Format;
}

void ave::VulkanEngine::CreatePipeline()
{
	vkInit::GraphicsPipelineInBundle specification{};
	specification.Device = m_Device;
	specification.VertexFilePath = "shaders/shader.vert.spv";
	specification.FragmentFilePath = "shaders/shader.frag.spv";
	specification.SwapchainExtent = m_SwapchainExtent;

	vkInit::GraphicsPipelineOutBundle out{ vkInit::CreateGraphicsPipeline(specification, m_IsDebugging) };
}