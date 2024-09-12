#ifndef VULKAN_ENGINE_H
#define VULKAN_ENGINE_H
#include "Engine/Configuration.h"
#include <GLFW/glfw3.h>
#include "Utils/Frame.h"
#include "Utils/Camera.h"
#include "Pipeline/Pipeline.h"
#include "Pipeline/RenderPass.h"
#include "Rendering/InstancedMesh.h"
#include "Utils/FileReader.h"
#include "Rendering/InstancedScene.h"

namespace ave
{

	class VulkanEngine final
	{
	public:
		VulkanEngine(const std::string& windowName, int width, int height, GLFWwindow* windowPtr);
		~VulkanEngine();
	
		VulkanEngine(const VulkanEngine& other) = delete;
		VulkanEngine(VulkanEngine&& other) = delete;
		VulkanEngine& operator=(const VulkanEngine& other) = delete;
		VulkanEngine& operator=(VulkanEngine&& other) = delete;
	
		void Render();
	private:
	
		const std::string m_WindowName{ "GP2 Assignment" };
		int m_Width{ 690 };
		int m_Height{ 420 };
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
	
		vk::DescriptorSetLayout m_DescriptorSetLayoutFrame;
		vk::DescriptorPool m_DescriptorPoolFrame;
	
		vk::DescriptorSetLayout m_DescriptorSetLayoutMesh;
		vk::DescriptorPool m_DescriptorPoolMesh;
		uint32_t m_NumberOfTextures{ 10 };

		std::unique_ptr<vkInit::RenderPass> m_RenderPassUPtr;
		std::unique_ptr<vkInit::Pipeline<vkUtil::Vertex3D>> m_Pipeline3DUPtr;

		std::unique_ptr <ave::InstancedScene<vkUtil::Vertex3D>> m_InstancedScene3DUPtr{ nullptr };

		vk::CommandPool m_CommandPool;
		vk::CommandBuffer m_MainCommandBuffer;

		int m_MaxNrFramesInFlight;
		int m_CurrentFrameNr;

		std::unique_ptr<ave::Camera> m_CameraUPtr;

		void CreateInstance();
		void CreateDevice();
		void CreateSwapchain();
		void CreateFrameBuffers();
		void CreateFrameResources();
		void CreateDescriptorSetLayouts();
		void CreatePipelines();
		void SetUpRendering();
		void Create3DScene();

		void PrepareFrame(uint32_t imgIdx);
		void RecordDrawCommands(const vk::CommandBuffer& commandBuffer, uint32_t imageIndex);

		void RecreateSwapchain();
		void DestroySwapchain();

		void PrintKeyBindings();
	};

}
#endif