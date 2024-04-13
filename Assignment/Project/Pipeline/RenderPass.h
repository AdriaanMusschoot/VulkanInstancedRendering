#ifndef VK_RENDERPASS_H
#define VK_RENDERPASS_H
#include "Engine/Configuration.h"
namespace vkInit
{
	class RenderPass final
	{
	public:
		RenderPass(const vk::Device& device, const vk::Format& swapchainImgFormat, bool isDebugging);
		~RenderPass();

		RenderPass(const RenderPass& other) = delete;
		RenderPass(RenderPass&& other) = delete;
		RenderPass& operator=(const RenderPass& other) = delete;
		RenderPass& operator=(RenderPass&& other) = delete;

		void BeginRenderPass(const vk::CommandBuffer& commandBuffer, const vk::Framebuffer& frameBuffer, const vk::Extent2D& swapchainExtent);
		void EndRenderPass(const vk::CommandBuffer& commandBuffer);

		vk::RenderPass const& GetRenderPass() const;
	private:
		vk::RenderPass m_RenderPass;

		vk::Device m_Device;

		vk::RenderPass CreateRenderPass(const vk::Format& swapchainImgFormat, bool isDebugging);

	};
}
#endif