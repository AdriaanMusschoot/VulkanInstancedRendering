#ifndef VK_QUEUEFAMILIES_H
#define VK_QUEUEFAMILIES_H
#include "Engine/Configuration.h"

namespace vkUtil
{

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> GraphicsFamily;
		std::optional<uint32_t> PresentFamily;

		bool AllIndicesSet()
		{
			return GraphicsFamily.has_value() and PresentFamily.has_value();
		}
	};

	QueueFamilyIndices FindQueueFamilies(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, bool isDebugging);

}

#endif