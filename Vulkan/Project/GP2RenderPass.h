#pragma once
#include "vulkanbase/VulkanUtil.h"

namespace amu
{

	class RenderPass
	{
	public:
		void CreateRenderPass(VkDevice device, VkFormat swapChainImageFormat);
		void BeginRenderPass(VkCommandBuffer commandBuffer, VkExtent2D swapchainExtent, VkFramebuffer frameBuffer);
		void EndRenderPass(VkCommandBuffer commandBuffer);
		VkRenderPass GetRenderPass() const { return m_VkRenderPass; }

		void Destroy();
	private:
		VkRenderPass m_VkRenderPass;
	
		VkDevice m_VkDevice;
	};

}