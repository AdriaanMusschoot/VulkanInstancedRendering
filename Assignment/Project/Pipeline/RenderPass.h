#ifndef VK_RENDERPASS_H
#define VK_RENDERPASS_H
#include "Engine/Configuration.h"
namespace vkInit
{
	struct RenderPassInBundle
	{
		vk::Device Device;
		vk::Format SwapchainImageFormat;
		vk::Format DepthFormat;
	};

	class RenderPass final
	{
	public:
		RenderPass(const RenderPassInBundle& in, bool isDebugging);
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

		vk::RenderPass CreateRenderPass(const vk::Format& swapchainImgFormat, const vk::Format& depthFormat, bool isDebugging);

	};
}
#endif