#pragma once
#include "vulkanbase/VulkanUtil.h"

struct QueueFamilyIndices;

class CommandPool
{
public:
	//also responsible for creating command buffers (the abstraction I made)
	void createCommandPool(const VkDevice& device, const QueueFamilyIndices& queueFamilyIndices);
	VkCommandBuffer  createCommandBuffer(const VkDevice& device);
private:
	VkCommandPool m_CommandPool;
};
