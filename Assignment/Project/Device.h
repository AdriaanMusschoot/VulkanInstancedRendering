#ifndef VK_DEVICE_H
#define VK_DEVICE_H
#include "Configuration.h"
#include <set>

namespace vkInit
{
	void LogDeviceProperties(const vk::PhysicalDevice& physicalDevice)
	{
		vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();

		std::cout << "\nPhysical device name: " << properties.deviceName << "\n";

		std::cout << "Physical device type: ";
		switch (properties.deviceType)
		{
		case vk::PhysicalDeviceType::eCpu:
			std::cout << "cpu";
			break;
		case vk::PhysicalDeviceType::eDiscreteGpu:
			std::cout << "discrete gpu";
			break;
		case vk::PhysicalDeviceType::eIntegratedGpu:
			std::cout << "integrated cpu";
			break;
		case vk::PhysicalDeviceType::eVirtualGpu:
			std::cout << "virtual gpu";
			break;
		case vk::PhysicalDeviceType::eOther:
		default:
			std::cout << "other";
			break;
		}
		std::cout << "\n";
	}

	bool CheckPhysicalDeviceExtensionSupport
	(
		const vk::PhysicalDevice& physicalDevice,
		const std::vector<const char*>& requestedExtensionVec,
		const bool& isDebugging
	)
	{
		std::set<std::string> requiredExtensionSet(requestedExtensionVec.begin(), requestedExtensionVec.end());

		if (isDebugging)
		{
			std::cout << "Physical device extension support:\n";
			for (const auto& supportedExtension : physicalDevice.enumerateDeviceExtensionProperties())
			{
				if (isDebugging)
				{
					std::cout << "\t\"" << supportedExtension.extensionName << "\"\n";
				}
				requiredExtensionSet.erase(supportedExtension.extensionName);
			}
		}
		return requiredExtensionSet.empty();
	}

	bool CheckPhysicalDeviceSuitability(const vk::PhysicalDevice& physicalDevice, bool isDebugging)
	{
		if (isDebugging)
		{
			std::cout << "Checking suitability of device\n";
		}

		const std::vector<const char*> requestedExtensionVec
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		if (isDebugging)
		{
			std::cout << "Requested physical device extensions:\n";
			for (const auto& requestedExtension : requestedExtensionVec)
			{
				std::cout << "\t\"" << requestedExtension << "\"\n";
			}
		}

		if (CheckPhysicalDeviceExtensionSupport(physicalDevice, requestedExtensionVec, isDebugging))
		{
			if (isDebugging)
			{
				std::cout << "Physical device can support the extensions\n";
			}
			return true;
		}
		else
		{
			if (isDebugging)
			{
				std::cout << "Physical device can not support the extensions\n";
			}
			return false;
		}
	}

	vk::PhysicalDevice ChoosePhysicalDevice(const vk::Instance& instance, bool isDebugging)
	{
		if (isDebugging)
		{
			std::cout << "\nChoosing physical device\n";
		}

		std::vector<vk::PhysicalDevice> availableDeviceVec = instance.enumeratePhysicalDevices();

		if (isDebugging)
		{
			std::cout << availableDeviceVec.size() << " available device(s)\n";
		}

		for (const auto& availableDevice : availableDeviceVec)
		{
			if (isDebugging)
			{
				LogDeviceProperties(availableDevice);
			}

			if (CheckPhysicalDeviceSuitability(availableDevice, isDebugging))
			{
				return availableDevice;
			}
		}

		return nullptr;
	}
}
#endif