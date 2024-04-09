#ifndef VK_COMMANDS_H
#define VK_COMMANDS_H
#include "Engine/Configuration.h"
#include "Utils/QueueFamilies.h"

namespace vkInit
{
	struct CommandBufferInBundle
	{
		vk::Device Device;
		vk::CommandPool CommandPool;
		std::vector<vkUtil::SwapchainFrame>& FrameVec;
	};

	vk::CommandPool CreateCommandPool(const vk::Device& device, const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, bool isDebugging)
	{
		vkUtil::QueueFamilyIndices queueFamilyIndices{ vkUtil::FindQueueFamilies(physicalDevice, surface, isDebugging) };

		vk::CommandPoolCreateInfo poolCreateInfo{};
		poolCreateInfo.flags = vk::CommandPoolCreateFlags{} | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		poolCreateInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();

		try
		{
			return device.createCommandPool(poolCreateInfo);
		}
		catch (const vk::SystemError& systemError)
		{
			if (isDebugging)
			{
				std::cout << systemError.what() << "\n";
			}
			return nullptr;
		}
	}

	void CreateFrameCommandBuffers(const CommandBufferInBundle& in, bool isDebugging)
	{
		vk::CommandBufferAllocateInfo bufferAllocInfo{};
		bufferAllocInfo.commandPool = in.CommandPool;
		bufferAllocInfo.level = vk::CommandBufferLevel::ePrimary;
		bufferAllocInfo.commandBufferCount = 1;

		for (int idx{}; idx < in.FrameVec.size(); ++idx)
		{
			try
			{
				in.FrameVec[idx].CommandBuffer = in.Device.allocateCommandBuffers(bufferAllocInfo)[0];
				if (isDebugging)
				{
					std::cout << "Command buffer allocated for frame " << idx << "\n";
				}
			}
			catch (const vk::SystemError& systemError)
			{
				if (isDebugging)
				{
					std::cout << "Command buffer allocation for frame " << idx << " failed\n";
					std::cout << systemError.what() << "\n";
				}
			}
		}
	}

	vk::CommandBuffer CreateMainCommandBuffer(const CommandBufferInBundle& in, bool isDebugging)
	{
		vk::CommandBufferAllocateInfo bufferAllocInfo{};
		bufferAllocInfo.commandPool = in.CommandPool;
		bufferAllocInfo.level = vk::CommandBufferLevel::ePrimary;
		bufferAllocInfo.commandBufferCount = 1;

		try
		{
			vk::CommandBuffer commandBuffer{ in.Device.allocateCommandBuffers(bufferAllocInfo)[0] };

			if (isDebugging)
			{
				std::cout << "\nMain command buffer allocation successful\n";
			}

			return commandBuffer;
		}
		catch (const vk::SystemError& systemError)
		{
			if (isDebugging)
			{
				std::cout << systemError.what() << "\n";
			}
			
			return nullptr;
		}
	}
}

#endif