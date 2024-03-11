#include "vulkanbase/VulkanBase.h"

void VulkanBase::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
	m_CommandBuffer.BeginCommandBuffer();

	drawFrame(imageIndex, m_CommandBuffer.GetCommandBuffer());

	m_CommandBuffer.EndCommandBuffer();
}

void VulkanBase::drawFrame(uint32_t imageIndex, const VkCommandBuffer& commandBuffer) 
{
	m_RenderPass.BeginRenderPass(commandBuffer, swapChainExtent, swapChainFramebuffers, imageIndex);
	
	m_Pipeline.DrawFrame(commandBuffer, swapChainExtent);

	m_CircleMesh.Draw(commandBuffer);
	m_TriangleMesh.Draw(commandBuffer);

	vkCmdEndRenderPass(commandBuffer);
}

//goes in utils, maybe
QueueFamilyIndices VulkanBase::findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}