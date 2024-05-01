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

	void CreateFrameBuffers(const FrameBufferInBundle& in, std::vector<vkUtil::SwapchainFrame>& frameVec);
}

#endif