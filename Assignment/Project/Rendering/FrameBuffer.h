#ifndef VK_FRAMEBUFFER_H
#define VK_FRAMEBUFFER_H
#include "Engine/Configuration.h"
#include "Utils/Frame.h"	
namespace vkInit
{

	struct FrameBufferInBundle
	{
		vk::Device Device;
		vk::RenderPass RenderPass;
		vk::Extent2D SwapchainExtent;
	};

	void CreateFrameBuffers(const FrameBufferInBundle& in, std::vector<vkUtil::SwapchainFrame>& frameVec, bool isDebugging)
	{
		for (int idx{}; idx < frameVec.size(); ++idx)
		{
			std::vector<vk::ImageView> attachementVec{ frameVec[idx].ImageView};

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

				if (isDebugging)
				{
					std::cout << "Frame buffer creation for " << idx << " successful\n";
				}
			}
			catch (const vk::SystemError& systemError)
			{
				if (isDebugging)
				{
					std::cout << systemError.what() << "\n";
				}
			}
		}
	}
}

#endif