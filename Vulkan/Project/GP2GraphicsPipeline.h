#pragma once
#include "vulkanbase/VulkanUtil.h"

class GraphicsPipeline
{
public:
	void BeginRenderPass(const VkCommandBuffer& commandBuffer);
	void SetViewPort(const VkCommandBuffer& commandBuffer);
	void SetScissor(const VkCommandBuffer& commandBuffer);
	void EndRenderPass(const VkCommandBuffer& commandBuffer);
private:
	VkPipeline m_GraphicsPipeline;
};