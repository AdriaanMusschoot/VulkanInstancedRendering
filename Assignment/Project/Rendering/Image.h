#ifndef VK_IMAGE_H
#define VK_IMAGE_H
#include "Engine/Configuration.h"
#include "Utils/Buffer.h"
#include "Rendering/Commands.h"
namespace vkInit
{

	struct ImageInBundle
	{
		vk::Device Device;
		vk::PhysicalDevice PhysicalDevice;
		vk::Extent2D Extent;
		vk::ImageTiling Tiling;
		vk::ImageUsageFlags UsageFlags;
		vk::MemoryPropertyFlags MemoryPropertyFlags;
		vk::Format Format;
	};

	struct ImageLayoutTransitionInBundle
	{
		vk::CommandBuffer CommandBuffer;
		vk::Queue Queue;
		vk::Image Image;
		vk::ImageLayout OldLayout;
		vk::ImageLayout NewLayout;
	};

	struct BufferCopyImageInBundle
	{
		vk::CommandBuffer CommandBuffer;
		vk::Queue Queue;
		vk::Buffer SourceBuffer;
		vk::Image DestinationImage;
		vk::Extent2D Extent;
	};

	vk::Image CreateImage(const ImageInBundle& in, bool isDebugging);

	vk::DeviceMemory CreateImageMemory(const ImageInBundle& in, const vk::Image& image, bool isDebugging);

	void TransitionImageLayout(const ImageLayoutTransitionInBundle& in);

	void CopyBufferToImage(const BufferCopyImageInBundle& in);

	vk::ImageView CreateImageView(const vk::Device& device, const vk::Image& image, const vk::Format& format, const vk::ImageAspectFlags& aspectFlags);

	vk::Format GetSupportedFormat(const vk::PhysicalDevice& physicalDevice, const std::vector<vk::Format>& formatVec, const vk::ImageTiling& tiling, const vk::FormatFeatureFlags& featureFlags);
}

#endif