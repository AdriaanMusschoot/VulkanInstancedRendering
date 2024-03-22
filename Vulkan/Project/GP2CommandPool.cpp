#include "GP2CommandPool.h"

#include "vulkanbase/VulkanBase.h"

void amu::CommandPool::CreateCommandPool(const VkDevice& device, const QueueFamilyIndices& queueFamilyIndices)
{
	//move it to the base pass it as parameter
	m_VkDevice = device;

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &m_VkCommandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

VkCommandBuffer amu::CommandPool::CreateCommandBuffer()
{
	VkCommandBuffer commandBuffer;

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_VkCommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(m_VkDevice, &allocInfo, &commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
	return commandBuffer;
}

const VkCommandPool& amu::CommandPool::GetCommandPool()
{
	return m_VkCommandPool;
}

void amu::CommandPool::Destroy()
{
	vkDestroyCommandPool(m_VkDevice, m_VkCommandPool, nullptr);
}
