#ifndef VK_COMMANDS_H
#define VK_COMMANDS_H
#include "Engine/Configuration.h"
#include "Utils/QueueFamilies.h"
#include "Utils/Frame.h"

namespace vkInit
{
	struct CommandBufferInBundle
	{
		vk::Device Device;
		vk::CommandPool CommandPool;
		std::vector<vkUtil::SwapchainFrame>& FrameVec;
	};

	vk::CommandPool CreateCommandPool(const vk::Device& device, const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface);

	void CreateFrameCommandBuffers(const CommandBufferInBundle& in);

	vk::CommandBuffer CreateMainCommandBuffer(const CommandBufferInBundle& in);

	void BeginSingleCommand(const vk::CommandBuffer& commandBuffer);

	void EndSingleCommand(const vk::CommandBuffer& commandBuffer, const vk::Queue& queue);

}

#endif