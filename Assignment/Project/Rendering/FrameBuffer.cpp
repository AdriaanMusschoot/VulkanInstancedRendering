#include "FrameBuffer.h"

void vkInit::CreateFrameBuffers(const FrameBufferInBundle& in, std::vector<vkUtil::SwapchainFrame>& frameVec)
{
	for (int idx{}; idx < frameVec.size(); ++idx)
	{
		std::vector<vk::ImageView> attachementVec{ frameVec[idx].ImageView, frameVec[idx].DepthBufferView };

		vk::FramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.flags = vk::FramebufferCreateFlags{};
		framebufferCreateInfo.renderPass = in.RenderPass;
		framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachementVec.size());
		framebufferCreateInfo.pAttachments = attachementVec.data();
		framebufferCreateInfo.width = in.SwapchainExtent.width;
		framebufferCreateInfo.height = in.SwapchainExtent.height;
		framebufferCreateInfo.layers = 1;

		try
		{
			frameVec[idx].Framebuffer = in.Device.createFramebuffer(framebufferCreateInfo);

			std::cout << "Frame buffer creation for " << idx << " successful\n";
		}
		catch (const vk::SystemError& systemError)
		{
			std::cout << systemError.what() << "\n";
		}
	}
}
