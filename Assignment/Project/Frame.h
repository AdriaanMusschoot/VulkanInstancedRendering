#ifndef VK_FRAME_H
#define VK_FRAME_H
#include "Configuration.h"

namespace vkUtil
{

	struct SwapchainFrame
	{
		vk::Image Image;
		vk::ImageView ImageView;
	};

}

#endif