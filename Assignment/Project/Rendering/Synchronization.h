#ifndef VK_SYNCHRONIZATION_H
#define VK_SYNCHRONIZATION_H
#include "Engine/Configuration.h"

namespace vkInit
{

	vk::Semaphore CreateSemaphore(const vk::Device& device)
	{
		vk::SemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.flags = vk::SemaphoreCreateFlags{};

		try
		{
			return device.createSemaphore(semaphoreCreateInfo);
		}
		catch (const vk::SystemError& systemError)
		{
			std::cout << systemError.what() << "\n";

			return nullptr;
		}
	}

	vk::Fence CreateFence(const vk::Device& device)
	{
		vk::FenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.flags = vk::FenceCreateFlags{} | vk::FenceCreateFlagBits::eSignaled;

		try
		{
			return device.createFence(fenceCreateInfo);
		}
		catch (const vk::SystemError& systemError)
		{
			std::cout << systemError.what() << "\n";

			return nullptr;
		}
	}
}

#endif