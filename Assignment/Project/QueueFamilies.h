#ifndef VK_QUEUEFAMILIES_H
#define VK_QUEUEFAMILIES_H
#include "Configuration.h"

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

	QueueFamilyIndices FindQueueFamilies(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, bool isDebugging)
	{
		QueueFamilyIndices queueFamilyIndices;

		std::vector queueFamilyVec = physicalDevice.getQueueFamilyProperties();

		if (isDebugging)
		{
			std::cout << "\nNumber of queue families supported: " << queueFamilyVec.size() << "\n";
		}

		int index{};
		for (const auto& queueFamily : queueFamilyVec)
		{
			if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
			{
				queueFamilyIndices.GraphicsFamily = index;

				if (isDebugging)
				{
					std::cout << "Queue family \"" << index << "\" is suitable for graphics\n";
				}
			}

			if (physicalDevice.getSurfaceSupportKHR(index, surface))
			{
				queueFamilyIndices.PresentFamily = index;

				if (isDebugging)
				{
					std::cout << "Queue family \"" << index << "\" is suitable for presenting\n";
				}
			}
			if (queueFamilyIndices.AllIndicesSet())
			{
				break;
			}
			++index;
		}

		return queueFamilyIndices;
	}

}

#endif