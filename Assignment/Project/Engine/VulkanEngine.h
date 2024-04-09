#ifndef VULKAN_ENGINE_H
#define VULKAN_ENGINE_H
#include "Engine/Configuration.h"
#include <GLFW/glfw3.h>
#include "Utils/Frame.h"
#include "Rendering/Scene.h"

namespace ave
{

	class VulkanEngine final
	{
	public:
		VulkanEngine(const std::string& windowName, int width, int height, GLFWwindow* windowPtr, bool isDebugging);
		~VulkanEngine();
	
		VulkanEngine(const VulkanEngine& other) = delete;
		VulkanEngine(VulkanEngine&& other) = delete;
		VulkanEngine& operator=(const VulkanEngine& other) = delete;
		VulkanEngine& operator=(VulkanEngine&& other) = delete;
	
		void Render(const Scene* scenePtr);
	private:
		bool m_IsDebugging{ true };
	
		const std::string m_WindowName{ "GP2 Assignment" };
		int m_Width{ 690 };
		int m_Height{ 480 };
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
	
		vk::PipelineLayout m_PipelineLayout;
		vk::RenderPass m_RenderPass;
		vk::Pipeline m_Pipeline;

		vk::CommandPool m_CommandPool;
		vk::CommandBuffer m_MainCommandBuffer;

		int m_MaxNrFramesInFlight;
		int m_CurrentFrameNr;

		std::unique_ptr<ave::Mesh> m_MeshUPtr;

		void CreateInstance();
		void CreateDevice();
		void CreateSwapchain();
		void CreateFrameBuffers();
		void CreateSynchronizationObjects();
		void CreatePipeline();
		void SetUpRendering();
		void CreateMeshes();
		void PrepareScene(const vk::CommandBuffer& commandBuffer);

		void RecordDrawCommand(const vk::CommandBuffer& commandBuffer, uint32_t imageIndex, const Scene* scenePtr);

		void RecreateSwapchain();
		void DestroySwapchain();	
	};

}
#endif