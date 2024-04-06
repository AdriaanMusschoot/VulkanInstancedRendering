#ifndef VK_LOGGING_H
#define VK_LOGGING_H
#include "Configuration.h"

namespace vkInit
{

	VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallBack
	(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* callBackDataPtr,
		void* userDataPtr
	)
	{
		std::cerr << "Validation layer: " << callBackDataPtr->pMessage << "\n";

		return VK_FALSE;
	}

	vk::DebugUtilsMessengerEXT CreateDebugMessenger(const vk::Instance& instance, const vk::DispatchLoaderDynamic& dLDInstance)
	{
		vk::DebugUtilsMessengerCreateInfoEXT createInfo
		{
			vk::DebugUtilsMessengerCreateFlagsEXT
			,
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
			DebugCallBack,
			nullptr
		};

		return instance.createDebugUtilsMessengerEXT(createInfo, nullptr, dLDInstance);
	}

	std::vector<std::string> LogTransformBits(const vk::SurfaceTransformFlagsKHR& bits)
	{
		std::vector<std::string> result;

		if (bits & vk::SurfaceTransformFlagBitsKHR::eIdentity)
		{
			result.emplace_back("identity");
		}
		if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate90) 
		{
			result.emplace_back("90 degree rotation");
		}
		if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate180) 
		{
			result.emplace_back("180 degree rotation");
		}
		if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate270) 
		{
			result.emplace_back("270 degree rotation");
		}
		if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirror) 
		{
			result.emplace_back("horizontal mirror");
		}
		if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate90) 
		{
			result.emplace_back("horizontal mirror, then 90 degree rotation");
		}
		if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate180) 
		{
			result.push_back("horizontal mirror, then 180 degree rotation");
		}
		if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate270) 
		{
			result.emplace_back("horizontal mirror, then 270 degree rotation");
		}
		if (bits & vk::SurfaceTransformFlagBitsKHR::eInherit) 
		{
			result.emplace_back("inherited");
		}

		return result;
	}

	std::vector<std::string> LogAlphaCompositeBits(const vk::CompositeAlphaFlagsKHR& bits) 
	{
		std::vector<std::string> result;

		if (bits & vk::CompositeAlphaFlagBitsKHR::eOpaque) 
		{
			result.emplace_back("opaque (alpha ignored)");
		}
		if (bits & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied) 
		{
			result.emplace_back("pre multiplied (alpha expected to already be multiplied in image)");
		}
		if (bits & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied) 
		{
			result.emplace_back("post multiplied (alpha will be applied during composition)");
		}
		if (bits & vk::CompositeAlphaFlagBitsKHR::eInherit) 
		{
			result.emplace_back("inherited");
		}

		return result;
	}

	std::vector<std::string> LogImageUsageBits(const vk::ImageUsageFlags& bits) 
	{
		std::vector<std::string> result;

		if (bits & vk::ImageUsageFlagBits::eTransferSrc) 
		{
			result.emplace_back("transfer src: image can be used as the source of a transfer command.");
		}
		if (bits & vk::ImageUsageFlagBits::eTransferDst) 
		{
			result.emplace_back("transfer dst: image can be used as the destination of a transfer command.");
		}
		if (bits & vk::ImageUsageFlagBits::eSampled) 
		{
			result.emplace_back("sampled: image can be used to create a VkImageView suitable for occupying a \
VkDescriptorSet slot either of type VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE or \
VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, and be sampled by a shader.");
		}
		if (bits & vk::ImageUsageFlagBits::eStorage)
		{
			result.emplace_back("storage: image can be used to create a VkImageView suitable for occupying a \
VkDescriptorSet slot of type VK_DESCRIPTOR_TYPE_STORAGE_IMAGE.");
		}
		if (bits & vk::ImageUsageFlagBits::eColorAttachment) 
		{
			result.emplace_back("color attachment: image can be used to create a VkImageView suitable for use as \
a color or resolve attachment in a VkFramebuffer.");
		}
		if (bits & vk::ImageUsageFlagBits::eDepthStencilAttachment) 
		{
			result.emplace_back("depth/stencil attachment: image can be used to create a VkImageView \
suitable for use as a depth/stencil or depth/stencil resolve attachment in a VkFramebuffer.");
		}
		if (bits & vk::ImageUsageFlagBits::eTransientAttachment) 
		{
			result.emplace_back("transient attachment: implementations may support using memory allocations \
with the VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT to back an image with this usage. This \
bit can be set for any image that can be used to create a VkImageView suitable for use as \
a color, resolve, depth/stencil, or input attachment.");
		}
		if (bits & vk::ImageUsageFlagBits::eInputAttachment) 
		{
			result.emplace_back("input attachment: image can be used to create a VkImageView suitable for \
occupying VkDescriptorSet slot of type VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT; be read from \
a shader as an input attachment; and be used as an input attachment in a framebuffer.");
		}
		if (bits & vk::ImageUsageFlagBits::eFragmentDensityMapEXT) 
		{
			result.emplace_back("fragment density map: image can be used to create a VkImageView suitable \
for use as a fragment density map image.");
		}
		if (bits & vk::ImageUsageFlagBits::eFragmentShadingRateAttachmentKHR) 
		{
			result.emplace_back("fragment shading rate attachment: image can be used to create a VkImageView \
suitable for use as a fragment shading rate attachment or shading rate image");
		}
		return result;
	}

	std::string LogPresentMode(const vk::PresentModeKHR& presentMode) 
	{
		switch (presentMode) {
		case vk::PresentModeKHR::eImmediate:
			return "Immediate";
			break;
		case vk::PresentModeKHR::eMailbox:
			return "Mailbox";
			break;
		case vk::PresentModeKHR::eFifo:
			return "Fifo";
			break;
		case vk::PresentModeKHR::eFifoRelaxed:
			return "Relaxed fifo";
			break;
		case vk::PresentModeKHR::eSharedDemandRefresh:
			return "Shared demand refresh";
			break;
		case vk::PresentModeKHR::eSharedContinuousRefresh:
			return "Shared continuous refresh";
			break;
		default:
			return "Somehow you have an undefined present mode\n";
			break;
		}

	}

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

}

#endif