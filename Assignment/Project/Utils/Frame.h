#ifndef VK_FRAME_H
#define VK_FRAME_H
#include "Engine/Configuration.h"
#include "Buffer.h"

namespace vkUtil
{

	struct UBO
	{
		glm::mat4 ModelMatrix;
		glm::mat4 ViewMatrix;
		glm::mat4 ProjectionMatrix;
	};

	struct SwapchainFrame
	{
		vk::Image Image;
		vk::ImageView ImageView;
		vk::Framebuffer Framebuffer;

		vk::CommandBuffer CommandBuffer;

		vk::Semaphore SemaphoreImageAvailable;
		vk::Semaphore SemaphoreRenderingFinished;
		vk::Fence InFlightFence;

		UBO WVPMatrix;
		vkUtil::DataBuffer WVPBuffer;
		void* WVPWriteLocationPtr;

		vk::DescriptorBufferInfo UBODescriptorInfo;
		vk::DescriptorSet UBODescriptorSet;

		void CreateUBOResources(const vk::Device& device, const vk::PhysicalDevice& physicalDevice)
		{
			BufferInBundle input;
			input.Device = device;
			input.PhysicalDevice = physicalDevice;
			input.MemoryPropertyFlags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
			input.Size = sizeof(UBO);
			input.UsageFlags = vk::BufferUsageFlagBits::eUniformBuffer;

			WVPBuffer = vkUtil::CreateBuffer(input);
			WVPWriteLocationPtr = device.mapMemory(WVPBuffer.BufferMemory, 0, sizeof(UBO));

			UBODescriptorInfo.buffer = WVPBuffer.Buffer;
			UBODescriptorInfo.offset = 0;
			UBODescriptorInfo.range = sizeof(UBO);
		}

		void WriteDescriptorSet(const vk::Device& device)
		{
			vk::WriteDescriptorSet writeInfo{};
			writeInfo.dstSet = UBODescriptorSet;
			writeInfo.dstBinding = 0;
			writeInfo.dstArrayElement = 0;
			writeInfo.descriptorCount = 1;
			writeInfo.descriptorType = vk::DescriptorType::eUniformBuffer;
			writeInfo.pBufferInfo = &UBODescriptorInfo;

			device.updateDescriptorSets(writeInfo, nullptr);
		}
	};

}

#endif