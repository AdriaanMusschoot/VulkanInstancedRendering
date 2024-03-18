#pragma once
#include "vulkanbase/VulkanUtil.h"

struct QueueFamilyIndices;
namespace amu
{

	class CommandPool
	{
	public:
		void CreateCommandPool(const VkDevice& device, const QueueFamilyIndices& queueFamilyIndices);
		VkCommandBuffer  CreateCommandBuffer();
		const VkCommandPool& GetCommandPool();	
		void Destroy();
	private:
		VkCommandPool m_CommandPool{};
		//TODO add device make it so that you dont need the device in all the other functions
		VkDevice m_Device{};
	};

}