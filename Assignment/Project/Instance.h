#ifndef VK_INSTANCE_H
#define VK_INSTANCE_H
#include "Configuration.h"

namespace vkInit
{

	bool CheckSupportExtensionsAndLayers(const std::vector<const char*>& requiredLayerVec, const std::vector<const char*>& requiredExtensionVec, bool isDebugging)
	{
		std::vector<vk::LayerProperties> supportedLayerVec{ vk::enumerateInstanceLayerProperties() };

		if (isDebugging)
		{
			std::cout << "\nLayers supported:\n";
			for (const auto& supportedLayer : supportedLayerVec)
			{
				std::cout << "\t\"" << supportedLayer.layerName << "\"\n";
			}
		}

		for (const auto& requiredLayers : requiredLayerVec)
		{
			bool wasFound = std::any_of(supportedLayerVec.begin(), supportedLayerVec.end(),
				[&](const vk::LayerProperties& supportedLayer)
				{
					return strcmp(requiredLayers, supportedLayer.layerName) == 0;
				});
			if (wasFound)
			{
				if (isDebugging)
				{
					std::cout << "Layer: " << requiredLayers << " is supported\n";
				}
			}
			else
			{
				if (isDebugging)
				{
					std::cout << "Layer: " << requiredLayers << "is not supported\n";
				}
				return false;
			}
		}

		std::vector<vk::ExtensionProperties> supportedExtensionVec{ vk::enumerateInstanceExtensionProperties() };

		if (isDebugging)
		{
			std::cout << "\nExtensions supported:\n";
			for (const auto& supportedExtension : supportedExtensionVec)
			{
				std::cout << "\t\"" << supportedExtension.extensionName << "\"\n";
			}
		}

		for (const auto& requiredExtension : requiredExtensionVec)
		{
			bool wasFound = std::any_of(supportedExtensionVec.begin(), supportedExtensionVec.end(),
				[&](const vk::ExtensionProperties& supportedExtension)
				{
					return strcmp(requiredExtension, supportedExtension.extensionName) == 0;
				});
			if (wasFound)
			{
				if (isDebugging)
				{
					std::cout << "Extension: " << requiredExtension << " is supported\n";
				}
			}
			else
			{
				if (isDebugging)
				{
					std::cout << "Extension: " << requiredExtension << " is not supported\n";
				}
				return false;
			}
		}

		return true;
	}

	vk::Instance CreateInstance(bool isDebugging, const std::string& name)
	{
		if (isDebugging)
		{
			std::cout << "Creating instance\n";
		}

		uint32_t versionNumber{ 0 };
		vkEnumerateInstanceVersion(&versionNumber);

		if (isDebugging)
		{
			std::cout << "Supports vulkan variant: " << VK_API_VERSION_VARIANT(versionNumber) << ", version number: "
					  << VK_API_VERSION_MAJOR(versionNumber) << "."
					  << VK_API_VERSION_MINOR(versionNumber) << "."
					  << VK_API_VERSION_PATCH(versionNumber) << "\n";
		}

		versionNumber &= ~(0xFFFU);

		versionNumber = VK_MAKE_API_VERSION(0, 1, 0, 0);

		vk::ApplicationInfo applicationInfo
		{
			name.c_str(),
			versionNumber,
			"VulkanEngine",
			versionNumber,
			versionNumber
		};

		std::vector<const char*> layerVec;

		if (isDebugging)
		{
			layerVec.emplace_back("VK_LAYER_KHRONOS_validation");
		}

		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions{ glfwGetRequiredInstanceExtensions(&glfwExtensionCount) };
		
		std::vector<const char*> requiredExtensionVec(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (isDebugging)
		{
			requiredExtensionVec.emplace_back("VK_EXT_debug_utils");
		}

		if (!CheckSupportExtensionsAndLayers(layerVec, requiredExtensionVec, isDebugging))
		{
			return nullptr;
		}

		vk::InstanceCreateInfo createInfo
		{
			vk::InstanceCreateFlags{},
			&applicationInfo,
			static_cast<uint32_t>(layerVec.size()),				 //layer count
			layerVec.data(),									 //layers
			static_cast<uint32_t>(requiredExtensionVec.size()),	 //extension count
			requiredExtensionVec.data()							 //extensions
		};
		
		try
		{
			return vk::createInstance(createInfo);
		}
		catch (const vk::SystemError & systemError)
		{
			if (isDebugging)
			{
				std::cout << systemError.what() << "\n";
			}
			return nullptr;
		}
	}

}

#endif