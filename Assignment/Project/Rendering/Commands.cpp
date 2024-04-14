#include "Commands.h"

vk::CommandPool vkInit::CreateCommandPool(const vk::Device& device, const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, bool isDebugging)
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

void vkInit::CreateFrameCommandBuffers(const CommandBufferInBundle& in, bool isDebugging)
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

vk::CommandBuffer vkInit::CreateMainCommandBuffer(const CommandBufferInBundle& in, bool isDebugging)
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

void vkInit::BeginSingleCommand(const vk::CommandBuffer& commandBuffer)
{
	commandBuffer.reset();

	vk::CommandBufferBeginInfo bufferBeginInfo{};
	bufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	commandBuffer.begin(bufferBeginInfo);
}

void vkInit::EndSingleCommand(const vk::CommandBuffer& commandBuffer, const vk::Queue& queue)
{
	commandBuffer.end();

	vk::SubmitInfo submitInfo;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vk::Result result{ queue.submit(1, &submitInfo, nullptr) };
	if (result != vk::Result::eSuccess)
	{
		std::cout << "Commandbuffer submission failure\n";
	}

	queue.waitIdle();
}
