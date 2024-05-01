#ifndef VK_DEVICE_H
#define VK_DEVICE_H
#include "Engine/Configuration.h"
#include "Utils/QueueFamilies.h"

namespace vkInit
{

	bool CheckPhysicalDeviceExtensionSupport
	(
		const vk::PhysicalDevice& physicalDevice,
		const std::vector<const char*>& requestedExtensionVec
	)
	{
		std::set<std::string> requiredExtensionSet(requestedExtensionVec.begin(), requestedExtensionVec.end());

		std::cout << "Physical device extension support:\n";

		for (const auto& supportedExtension : physicalDevice.enumerateDeviceExtensionProperties())
		{
			std::cout << "\t\"" << supportedExtension.extensionName << "\"\n";
			
			requiredExtensionSet.erase(supportedExtension.extensionName);
		}

		return requiredExtensionSet.empty();
	}

	bool CheckPhysicalDeviceSuitability(const vk::PhysicalDevice& physicalDevice)
	{
		
		std::cout << "Checking suitability of device\n";
		

		const std::vector<const char*> requestedExtensionVec
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		std::cout << "Requested physical device extensions:\n";
		for (const auto& requestedExtension : requestedExtensionVec)
		{
			std::cout << "\t\"" << requestedExtension << "\"\n";
		}
		

		if (CheckPhysicalDeviceExtensionSupport(physicalDevice, requestedExtensionVec))
		{
			std::cout << "Physical device can support the extensions\n";
			
			return true;
		}
		else
		{
			std::cout << "Physical device can not support the extensions\n";
			
			return false;
		}
	}

	vk::PhysicalDevice ChoosePhysicalDevice(const vk::Instance& instance)
	{
		std::cout << "\nChoosing physical device\n";
		

		std::vector<vk::PhysicalDevice> availableDeviceVec = instance.enumeratePhysicalDevices();

		
		std::cout << availableDeviceVec.size() << " available device(s)\n";
		

		for (const auto& availableDevice : availableDeviceVec)
		{
			
			LogDeviceProperties(availableDevice);
			
			if (CheckPhysicalDeviceSuitability(availableDevice))
			{
				return availableDevice;
			}
		}

		return nullptr;
	}

	vk::Device CreateLogicalDevice(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface)
	{
		vkUtil::QueueFamilyIndices queueFamilyIndices{ vkUtil::FindQueueFamilies(physicalDevice, surface) };

		std::vector<uint32_t> uniqueIndexVec;
		uniqueIndexVec.emplace_back(queueFamilyIndices.GraphicsFamily.value());
		if (queueFamilyIndices.PresentFamily.value() != queueFamilyIndices.GraphicsFamily.value())
		{
			uniqueIndexVec.emplace_back(queueFamilyIndices.PresentFamily.value());
		}
		
		float queuePriority{ 1.0f };

		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfoVec;

		for (const auto& uniqueIndex : uniqueIndexVec)
		{
			queueCreateInfoVec.emplace_back
			(
				vk::DeviceQueueCreateInfo
				{
					vk::DeviceQueueCreateFlags{},
					uniqueIndex,
					1,
					&queuePriority
				}
			);
		}

		std::vector<const char*> deviceExtensionVec
		{ 
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		vk::PhysicalDeviceFeatures physicalDeviceFeatures{};

		std::vector<const char*> enabledLayerVec{};
		
		enabledLayerVec.emplace_back("VK_LAYER_KHRONOS_validation");
		

		vk::DeviceCreateInfo deviceCreateInfo
		{
			vk::DeviceCreateFlags{},
			static_cast<uint32_t>(queueCreateInfoVec.size()),
			queueCreateInfoVec.data(),
			static_cast<uint32_t>(enabledLayerVec.size()),
			enabledLayerVec.data(),
			static_cast<uint32_t>(deviceExtensionVec.size()),
			deviceExtensionVec.data(),
			&physicalDeviceFeatures
		};
		
		try
		{
			vk::Device device = physicalDevice.createDevice(deviceCreateInfo);

			std::cout << "Logical device creation successful\n";

			return device;
		}
		catch(const vk::SystemError& systemError)
		{
			std::cout << systemError.what() << "\n";
			
			return nullptr;
		}
	}

	std::array<vk::Queue, 2> GetQueuesFromGPU(const vk::PhysicalDevice& physicalDevice, const vk::Device& device, const vk::SurfaceKHR& surface)
	{
		vkUtil::QueueFamilyIndices queueFamilyIndices = vkUtil::FindQueueFamilies(physicalDevice, surface);

		return	std::array<vk::Queue, 2>
				{
					device.getQueue(queueFamilyIndices.GraphicsFamily.value(), 0),
					device.getQueue(queueFamilyIndices.PresentFamily.value(), 0)
				};
	}

}
#endif 