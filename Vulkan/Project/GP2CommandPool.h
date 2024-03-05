#pragma once
#include "vulkanbase/VulkanUtil.h"

struct QueueFamilyIndices;

class CommandPool
{
public:
	void CreateCommandPool(const VkDevice& device, const QueueFamilyIndices& queueFamilyIndices);
	VkCommandBuffer  CreateCommandBuffer(const VkDevice& device);
	void DestroyCommandPool(const VkDevice& device);
private:
	VkCommandPool m_CommandPool{};
};
