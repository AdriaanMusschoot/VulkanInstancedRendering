#ifndef VK_SWAPCHAIN_H
#define VK_SWAPCHAIN_H
#include "Engine/Configuration.h"
#include "Utils/Logging.h"
#include "Utils/QueueFamilies.h"
#include "Utils/Frame.h"
#include "Image.h"
namespace vkInit
{

	struct SwapchainSupportDetails
	{
		vk::SurfaceCapabilitiesKHR Capabilities;
		std::vector<vk::SurfaceFormatKHR> FormatVec;
		std::vector<vk::PresentModeKHR> PresentModeVec;
	};

	struct SwapchainBundle
	{
		vk::SwapchainKHR Swapchain;
		std::vector<vkUtil::SwapchainFrame> FrameVec;
		vk::Format Format;
		vk::Extent2D Extent;
	};

	SwapchainSupportDetails QuerySwapchainSupport(const vk::PhysicalDevice& physicalDevice, const vk::SurfaceKHR& surface, bool isDebugging)
	{
		SwapchainSupportDetails supportDetails;

		supportDetails.Capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);

		if (isDebugging)
		{
			std::cout << "\nSwapchain supports following surface capabilities:\n";
			std::cout << "\tMinimum image count: " << supportDetails.Capabilities.minImageCount << "\n";
			std::cout << "\tMaximum image count: " << supportDetails.Capabilities.maxImageCount << "\n";

			std::cout << "\tCurrent extend:\n";
			std::cout << "\t\tWidth: " << supportDetails.Capabilities.currentExtent.width << "\n";
			std::cout << "\t\tHeight: " << supportDetails.Capabilities.currentExtent.height << "\n";

			std::cout << "\tMaximum image array layers: " << supportDetails.Capabilities.maxImageArrayLayers << "\n";

			std::cout << "\tCurrent transform(s):\n";
			std::vector stringVec{ LogTransformBits(supportDetails.Capabilities.currentTransform) };
			for (const auto& line : stringVec)
			{
				std::cout << "\t\t\"" << line << "\"\n";
			}

			std::cout << "\tCurrent alpha composite(s):\n";
			stringVec = LogAlphaCompositeBits(supportDetails.Capabilities.supportedCompositeAlpha);
			for (const auto& line : stringVec)
			{
				std::cout << "\t\t\"" << line << "\"\n";
			}

			std::cout << "\tCurrent alpha composite(s):\n";
			stringVec = LogImageUsageBits(supportDetails.Capabilities.supportedUsageFlags);
			for (const auto& line : stringVec)
			{
				std::cout << "\t\t\"" << line << "\"\n";
			}
		}

		supportDetails.FormatVec = physicalDevice.getSurfaceFormatsKHR(surface);

		if (isDebugging)
		{
			std::cout << "\nSupported pixel formats and color spaces:\n";
			for (const auto& supportedFormat : supportDetails.FormatVec)
			{
				std::cout << "\tpixel format: " << vk::to_string(supportedFormat.format) << '\n';
				std::cout << "\tcolor space: " << vk::to_string(supportedFormat.colorSpace) << '\n';
			}
		}

		supportDetails.PresentModeVec = physicalDevice.getSurfacePresentModesKHR(surface);
		if (isDebugging)
		{
			std::cout << "\nSurface present modes:\n";
			for (const auto& presentMode : supportDetails.PresentModeVec)
			{
				std::cout << "\t" << LogPresentMode(presentMode) << "\n";
			}
		}

		return supportDetails;
	}

	vk::SurfaceFormatKHR ChooseSwapchainSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formatVec)
	{
		for (const auto& format : formatVec)
		{
			if (format.format == vk::Format::eB8G8R8A8Unorm and
				format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
			{
				return format;
			}
		}
		return formatVec[0];
	}

	vk::PresentModeKHR ChooseSwapchainPresentMode(const std::vector<vk::PresentModeKHR>& presentModeVec)
	{
		for (const auto& presentMode : presentModeVec)
		{
			if (presentMode == vk::PresentModeKHR::eMailbox)
			{
				return presentMode;
			}
		}

		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D ChooseSwapchainExtent(uint32_t width, uint32_t height, const vk::SurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != UINT32_MAX and
			capabilities.currentExtent.height != UINT32_MAX)
		{
			return capabilities.currentExtent;
		}
		else
		{
			vk::Extent2D extent{ width, height };

			extent.width = std::min
			(
				capabilities.maxImageExtent.width,
				std::max(capabilities.minImageExtent.width, extent.width)
			);

			extent.height = std::min
			(
				capabilities.maxImageExtent.height,
				std::max(capabilities.minImageExtent.height, extent.height)
			);
			return extent;
		}
	}

	SwapchainBundle CreateSwapchain(const vk::PhysicalDevice& physicalDevice, const vk::Device& device, const vk::SurfaceKHR& surface, uint32_t width, uint32_t height, bool isDebugging)
	{
		SwapchainSupportDetails supportDetails{ QuerySwapchainSupport(physicalDevice, surface, isDebugging) };

		vk::SurfaceFormatKHR format{ ChooseSwapchainSurfaceFormat(supportDetails.FormatVec) };

		vk::PresentModeKHR presentMode{ ChooseSwapchainPresentMode(supportDetails.PresentModeVec) };

		vk::Extent2D extent{ ChooseSwapchainExtent(width, height, supportDetails.Capabilities) };

		uint32_t imageCount
		{
			std::min
			(
				supportDetails.Capabilities.minImageCount + 1,
				supportDetails.Capabilities.maxImageCount
			)
		};

		vk::SwapchainCreateInfoKHR swapchainCreateInfo
		{
			vk::SwapchainCreateFlagsKHR{},
			surface,
			imageCount,
			format.format,
			format.colorSpace,
			extent,
			1,
			vk::ImageUsageFlagBits::eColorAttachment
		};

		vkUtil::QueueFamilyIndices familyIndices{ vkUtil::FindQueueFamilies(physicalDevice, surface, isDebugging) };

		std::array<uint32_t, 2> queueFamilyIndices
		{
			familyIndices.GraphicsFamily.value(),
			familyIndices.PresentFamily.value()
		};

		if (familyIndices.GraphicsFamily.value() != familyIndices.PresentFamily.value())
		{
			swapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			swapchainCreateInfo.queueFamilyIndexCount = 2;
			swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();
		}
		else
		{
			swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
		}

		swapchainCreateInfo.preTransform = supportDetails.Capabilities.currentTransform;
		swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		swapchainCreateInfo.presentMode = presentMode;
		swapchainCreateInfo.clipped = VK_TRUE;

		swapchainCreateInfo.oldSwapchain = vk::SwapchainKHR{ nullptr };

		SwapchainBundle bundle{};
		try
		{
			bundle.Swapchain = device.createSwapchainKHR(swapchainCreateInfo);
		}
		catch (const vk::SystemError& systemError)
		{
			if (isDebugging)
			{
				std::cout << systemError.what() << "\n";
			}
			throw std::runtime_error{ "Swapchain creation failed\n" };
		}

		std::vector<vk::Image> tempImageVec = device.getSwapchainImagesKHR(bundle.Swapchain);
		bundle.FrameVec.resize(tempImageVec.size());
		for (int idx{}; idx < tempImageVec.size(); ++idx)
		{
			bundle.FrameVec[idx].Image = tempImageVec[idx];
			bundle.FrameVec[idx].ImageView = vkInit::CreateImageView(device, bundle.FrameVec[idx].Image, format.format, vk::ImageAspectFlagBits::eColor);
		}

		bundle.Format = format.format;
		bundle.Extent = extent;

		return bundle;
	}

}

#endif