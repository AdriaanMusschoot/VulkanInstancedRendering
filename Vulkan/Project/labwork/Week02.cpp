#include "vulkanbase/VulkanBase.h"

void VulkanBase::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
	m_CommandBuffer.BeginCommandBuffer();

	m_RenderPass.BeginRenderPass(commandBuffer, swapChainExtent, swapChainFramebuffers[imageIndex]);

	m_Pipeline.BindPipeline(commandBuffer, swapChainExtent);

	m_Scene.Render(commandBuffer);

	m_RenderPass.EndRenderPass(commandBuffer);

	m_CommandBuffer.EndCommandBuffer();
}

void VulkanBase::CreateTriangle()
{
	amu::Mesh<Vertex> temp{ physicalDevice, device };
	temp.AddVertex(Vertex{ { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } });
	temp.AddVertex(Vertex{ { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } });
	temp.AddVertex(Vertex{ { 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } });
	temp.AddVertex(Vertex{ { -0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f } });

	temp.AddIndex(uint16_t(0));
	temp.AddIndex(uint16_t(1));
	temp.AddIndex(uint16_t(2));
	temp.AddIndex(uint16_t(2));
	temp.AddIndex(uint16_t(3));
	temp.AddIndex(uint16_t(0));

	temp.InitializeVertexBuffers(graphicsQueue, m_CommandPool);

	m_Scene.AddMesh(std::move(temp));
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