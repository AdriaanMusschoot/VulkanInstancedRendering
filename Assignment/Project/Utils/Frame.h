#ifndef VK_FRAME_H
#define VK_FRAME_H
#include "Engine/Configuration.h"

namespace vkUtil
{

	struct SwapchainFrame
	{
		vk::Image Image;
		vk::ImageView ImageView;
		vk::Framebuffer Framebuffer;
		vk::CommandBuffer CommandBuffer;
		vk::Semaphore SemaphoreImageAvailable;
		vk::Semaphore SemaphoreRenderingFinished;
		vk::Fence InFlightFence
			;
	};

}

#endif