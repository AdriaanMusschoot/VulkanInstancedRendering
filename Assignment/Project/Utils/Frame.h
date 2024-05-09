#ifndef VK_FRAME_H
#define VK_FRAME_H
#include "Engine/Configuration.h"
#include "Buffer.h"

namespace vkUtil
{

	struct UBO
	{
		glm::mat4 ViewMatrix;
		glm::mat4 ProjectionMatrix;
	};
	
	struct SwapchainFrame
	{
		vk::Device Device;
		vk::PhysicalDevice PhysicalDevice;
		 
		vk::Image Image;
		vk::ImageView ImageView;
		vk::Framebuffer Framebuffer;

		vk::Image DepthBuffer;
		vk::ImageView DepthBufferView;
		vk::DeviceMemory DepthBufferMemory;
		vk::Format DepthFormat;
		vk::Extent2D DepthExtent;

		vk::CommandBuffer CommandBuffer;

		vk::Semaphore SemaphoreImageAvailable;
		vk::Semaphore SemaphoreRenderingFinished;
		vk::Fence InFlightFence;

		UBO VPMatrix{};
		vkUtil::DataBuffer VPBuffer;
		void* VPWriteLocationPtr{ nullptr };

		vk::DescriptorBufferInfo UBODescriptorInfo;

		std::vector<glm::mat4> WMatrixVec;
		vkUtil::DataBuffer WBuffer;
		void* WBufferWriteLocationPtr{ nullptr };

		vk::DescriptorBufferInfo WDescriptorInfo;

		//shared by WDescriptorInfo and VPDescriptorInfo
		vk::DescriptorSet DescriptorSet;

		void CreateDescriptorResources();

		void WriteDescriptorSet();

		void CreateDepthResources();

		void Destroy();
	};

}

#endif