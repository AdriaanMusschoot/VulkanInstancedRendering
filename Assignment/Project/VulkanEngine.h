#ifndef VULKAN_ENGINE_H
#define VULKAN_ENGINE_H
#include "Configuration.h"
#include <GLFW/glfw3.h>
#include "Frame.h"

namespace ave
{

	class VulkanEngine final
	{
	public:
		VulkanEngine();
		~VulkanEngine();
	
		VulkanEngine(const VulkanEngine& other) = delete;
		VulkanEngine(VulkanEngine&& other) = delete;
		VulkanEngine& operator=(const VulkanEngine& other) = delete;
		VulkanEngine& operator=(VulkanEngine&& other) = delete;
	
	private:
		bool m_IsDebugging{ true };
	
		const std::string m_WindowName{ "GP2 Assignment" };
		const uint32_t m_Width{ 690 };
		const uint32_t m_Height{ 480 };
		
		//cannot have unique ptr because incomplete typedefined struct
		GLFWwindow * m_WindowPtr{ nullptr };
	
		vk::Instance m_Instance{ nullptr };
		vk::DebugUtilsMessengerEXT m_DebugMessenger{ nullptr };
		vk::DispatchLoaderDynamic m_DLDInstance;
		vk::SurfaceKHR m_Surface;
	
		vk::PhysicalDevice m_PhysicalDevice{ nullptr };
		vk::Device m_Device{ nullptr };
		vk::Queue m_GraphicsQueue{ nullptr };
		vk::Queue m_PresentQueue{ nullptr };
		vk::SwapchainKHR m_Swapchain{ nullptr };
		std::vector<vkUtil::SwapchainFrame> m_SwapchainFrameVec; 
		vk::Extent2D m_SwapchainExtent;
		vk::Format m_SwapchainFormat;
	
		void CreateGLFWWindow();
		void CreateInstance();
		void CreateDevice();
		void CreatePipeline();
	};

}
#endif