#include "Image.h"

vk::Image vkInit::CreateImage(const ImageInBundle& in, bool isDebugging)
{
	vk::ImageCreateInfo imgCreateInfo{};
	imgCreateInfo.flags = vk::ImageCreateFlags{};
	imgCreateInfo.imageType = vk::ImageType::e2D;
	imgCreateInfo.extent = vk::Extent3D{ in.Extent, 1 };
	imgCreateInfo.mipLevels = 1;
	imgCreateInfo.arrayLayers = 1;
	imgCreateInfo.format = in.Format;
	imgCreateInfo.tiling = in.Tiling;
	imgCreateInfo.initialLayout = vk::ImageLayout::eUndefined;
	imgCreateInfo.usage = in.UsageFlags;
	imgCreateInfo.sharingMode = vk::SharingMode::eExclusive;
	imgCreateInfo.samples = vk::SampleCountFlagBits::e1;

	try
	{
		return in.Device.createImage(imgCreateInfo);
	}
	catch (const vk::SystemError& systemError)
	{
		if (isDebugging)
		{
			std::cout << systemError.what() << "\n";
		}
		return nullptr;
	}
}

vk::DeviceMemory vkInit::CreateImageMemory(const ImageInBundle& in, const vk::Image& image, bool isDebugging)
{
	vk::MemoryRequirements memoryRequirements{ in.Device.getImageMemoryRequirements(image) };

	vk::MemoryAllocateInfo memAllocateInfo{};
	memAllocateInfo.allocationSize = memoryRequirements.size;
	memAllocateInfo.memoryTypeIndex = vkUtil::FindMemoryTypeIndex(in.PhysicalDevice, memoryRequirements.memoryTypeBits, in.MemoryPropertyFlags);

	try
	{
		vk::DeviceMemory imgMemory{ in.Device.allocateMemory(memAllocateInfo) };
		in.Device.bindImageMemory(image, imgMemory, 0);
		return imgMemory;
	}
	catch (const vk::SystemError& systemError)
	{
		if (isDebugging)
		{
			std::cout << systemError.what() << "\n";
		}
		return nullptr;
	}

}

void vkInit::TransitionImageLayout(const ImageLayoutTransitionInBundle& in)
{
	vkInit::BeginSingleCommand(in.CommandBuffer);

	vk::ImageSubresourceRange imgSubRange{};
	imgSubRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	imgSubRange.baseMipLevel = 0;
	imgSubRange.levelCount = 1;
	imgSubRange.baseArrayLayer = 0;
	imgSubRange.layerCount = 1;

	vk::ImageMemoryBarrier barrier;
	barrier.oldLayout = in.OldLayout;
	barrier.newLayout = in.NewLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = in.Image;
	barrier.subresourceRange = imgSubRange;

	vk::PipelineStageFlags sourceStageFlags;
	vk::PipelineStageFlags destinationStageFlags;
	if (in.OldLayout == vk::ImageLayout::eUndefined)
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eNoneKHR;
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		//flag is before the pipeline starts
		sourceStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStageFlags = vk::PipelineStageFlagBits::eTransfer;
	}
	else
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		//flag is before the pipeline starts
		sourceStageFlags = vk::PipelineStageFlagBits::eTransfer;
		destinationStageFlags = vk::PipelineStageFlagBits::eFragmentShader;
	}

	in.CommandBuffer.pipelineBarrier
	(
		sourceStageFlags,
		destinationStageFlags,
		vk::DependencyFlags{},
		nullptr,
		nullptr,
		barrier
	);

	vkInit::EndSingleCommand(in.CommandBuffer, in.Queue);
}

void vkInit::CopyBufferToImage(const BufferCopyImageInBundle& in)
{
	vkInit::BeginSingleCommand(in.CommandBuffer);

	vk::BufferImageCopy copy;
	copy.bufferOffset = 0;
	copy.bufferRowLength = 0;
	copy.bufferImageHeight = 0;

	vk::ImageSubresourceLayers imgSubLayers;
	imgSubLayers.aspectMask = vk::ImageAspectFlagBits::eColor;
	imgSubLayers.mipLevel = 0;
	imgSubLayers.baseArrayLayer = 0;
	imgSubLayers.layerCount = 1;
	
	copy.imageSubresource = imgSubLayers;

	copy.imageOffset = vk::Offset3D{ 0, 0, 0 };
	copy.imageExtent = vk::Extent3D{ in.Extent, 1 };

	in.CommandBuffer.copyBufferToImage
	(
		in.SourceBuffer,
		in.DestinationImage,
		vk::ImageLayout::eTransferDstOptimal,
		copy
	);

	vkInit::EndSingleCommand(in.CommandBuffer, in.Queue);
}

vk::ImageView vkInit::CreateImageView(const vk::Device& device, const vk::Image& image, const vk::Format& format, const vk::ImageAspectFlags& aspectFlags)
{
	vk::ImageViewCreateInfo imgViewCreateInfo{};
	imgViewCreateInfo.image = image;
	imgViewCreateInfo.format = format;
	imgViewCreateInfo.viewType = vk::ImageViewType::e2D;
	imgViewCreateInfo.components.r = vk::ComponentSwizzle::eIdentity;
	imgViewCreateInfo.components.g = vk::ComponentSwizzle::eIdentity;
	imgViewCreateInfo.components.b = vk::ComponentSwizzle::eIdentity;
	imgViewCreateInfo.components.a = vk::ComponentSwizzle::eIdentity;
	imgViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
	imgViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imgViewCreateInfo.subresourceRange.levelCount = 1;
	imgViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imgViewCreateInfo.subresourceRange.layerCount = 1;

	return device.createImageView(imgViewCreateInfo);
}

vk::Format vkInit::GetSupportedFormat(const vk::PhysicalDevice& physicalDevice, const std::vector<vk::Format>& formatVec, const vk::ImageTiling& tiling, const vk::FormatFeatureFlags& featureFlags)
{
	for (const auto& format : formatVec)
	{
		vk::FormatProperties properties{ physicalDevice.getFormatProperties(format) };

		if (tiling == vk::ImageTiling::eLinear and
			(properties.linearTilingFeatures & featureFlags) == featureFlags)
		{
			return format;
		}
		else if (tiling == vk::ImageTiling::eOptimal and
			(properties.optimalTilingFeatures & featureFlags) == featureFlags)
		{
			return format; 
		}
	}
}