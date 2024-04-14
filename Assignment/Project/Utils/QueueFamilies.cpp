#include "QueueFamilies.h"

vkUtil::QueueFamilyIndices vkUtil::FindQueueFamilies(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, bool isDebugging)
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
